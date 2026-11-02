/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * _______________________________________________________________________________________________________
 *
 * File name
 * >    passtracker.cpp
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    QObject
 *
 * Description
 * >    Drives a mount along a satellite pass. Lives in its own thread (all mount I/O is blocking).
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <algorithm>
#include <cmath>
#include <QDir>
#include <QFile>
#include <QMetaEnum>
#include <QTimer>
#include "librairies/dates/date.h"
#include "passtracker.h"


static constexpr double RAD2ARCMIN = MATHS::RAD2DEG * MATHS::ARCMIN_PAR_DEG;


PassTracker::PassTracker(QObject *parent) :
    QObject(parent),
    _clock(&MountClock::System()),
    _timer(new QTimer(this)),
    _state(State::DISCONNECTED),
    _aimJd(0.),
    _gotoIssuedJd(0.),
    _slewing(false),
    _leadSeconds(INITIAL_LEAD_S),
    _lastErrorArcmin(-1.),
    _separationArcmin(-1.),
    _mountRa(0.),
    _mountDec(0.),
    _mountPositionKnown(false),
    _gotoCount(0),
    _lateArrivals(0),
    _lastStatusSecond(-1)
{
    _timer->setInterval(TICK_MS);
    _timer->setTimerType(Qt::PreciseTimer);
    connect(_timer, &QTimer::timeout, this, &PassTracker::Tick);
}

PassTracker::~PassTracker()
{
    CloseLog();
    if (_driver) {
        _driver->Disconnect();
    }
}

void PassTracker::SetClock(const MountClock &clock)
{
    _clock = &clock;
}

void PassTracker::ConnectMount(const MountProtocol protocol,
                               const TransportSettings &transport,
                               const bool sendSiteAndTime,
                               const double latitude,
                               const double longitude)
{
    _timer->stop();
    _driver.reset();

    try {

        std::unique_ptr<MountDriver> driver = MountDriver::Create(protocol, transport);
        const QString link = (protocol == MountProtocol::SIMULATOR) ? tr("no hardware") : MountTransport(transport).Description();
        emit logMessage(tr("Connecting to %1 (%2)...").arg(driver->Name(), link));

        const QString identity = driver->Connect();
        emit logMessage(tr("Connected: %1").arg(identity));

        if (sendSiteAndTime) {
            driver->SetSiteAndTime(latitude, longitude, QDateTime::currentDateTimeUtc());
            emit logMessage(tr("Site and time sent to the mount"));
        }

        _driver = std::move(driver);
        emit connectionChanged(true, identity);
        SetState(State::IDLE, tr("Connected"));

    } catch (const MountError &e) {
        emit logMessage(tr("ERROR: %1").arg(e.message()));
        emit connectionChanged(false, QString());
        SetState(State::DISCONNECTED, tr("Not connected"));
    }
}

void PassTracker::DisconnectMount()
{
    _timer->stop();

    if (_driver) {
        _driver->Disconnect();
        _driver.reset();
        emit logMessage(tr("Disconnected"));
    }

    _pointing.reset();
    emit connectionChanged(false, QString());
    SetState(State::DISCONNECTED, tr("Not connected"));
}

void PassTracker::Arm(const PassDefinition &pass,
                      const TrackerSettings &settings)
{
    if (!_driver) {
        emit logMessage(tr("Connect the mount first"));
        return;
    }

    if ((_state == State::WAITING) || (_state == State::PREPOSITIONING) || (_state == State::TRACKING)) {
        emit logMessage(tr("A pass is already armed: abort it first"));
        return;
    }

    if (pass.elements.isEmpty()) {
        emit logMessage(tr("No orbital elements for the satellite"));
        return;
    }

    _pass = pass;
    _settings = settings;

    try {

        const Satellite satellite = (pass.elements.size() > 1) ? Satellite(pass.elements) : Satellite(pass.elements.first());
        _pointing = std::make_unique<SatellitePointing>(satellite, pass.observer);

        const double now = _clock->NowJdUtc();
        if (pass.jdEnd <= now) {
            emit logMessage(tr("This pass is already over"));
            return;
        }

        const PassSummary summary = _pointing->Summarize(std::max(pass.jdStart, now), pass.jdEnd, settings.minAltitude);

        if (summary.maxAltitude < settings.minAltitude) {
            emit logMessage(tr("The satellite never rises above the minimum altitude during this pass"));
            return;
        }

        emit logMessage(tr("Pass of %1: maximum altitude %2° at %3 UTC, maximum angular rate %4°/s")
                        .arg(pass.satelliteName)
                        .arg(summary.maxAltitude * MATHS::RAD2DEG, 0, 'f', 1)
                        .arg(SatellitePointing::ToDateTimeUtc(summary.jdMaxAltitude).toString("HH:mm:ss"))
                        .arg(summary.maxRate * MATHS::RAD2DEG, 0, 'f', 2));

        if (summary.maxAltitude > settings.maxAltitude) {
            emit logMessage(tr("WARNING: the satellite goes above the maximum altitude (%1°): that part of the pass will be skipped")
                            .arg(settings.maxAltitude * MATHS::RAD2DEG, 0, 'f', 0));
        }

        if (_driver->IsEquatorial() && summary.crossesMeridian) {
            emit logMessage(tr("WARNING: the pass crosses the meridian. An equatorial mount may perform a meridian flip "
                               "during the pass and lose the satellite: widen the meridian limits of the mount if possible."));
        }

    } catch (const std::exception &) {
        emit logMessage(tr("ERROR: cannot compute the pass"));
        return;
    }

    _aimJd = 0.;
    _slewing = false;
    _leadSeconds = INITIAL_LEAD_S;
    _lastErrorArcmin = -1.;
    _separationArcmin = -1.;
    _mountPositionKnown = false;
    _trackingSeparations.clear();
    _gotoCount = 0;
    _lateArrivals = 0;
    _lastStatusSecond = -1;

    OpenLog();
    SetState(State::WAITING, tr("Armed"));
    _timer->start();
    Tick();
}

void PassTracker::Abort()
{
    _timer->stop();

    if (_driver) {
        try {
            _driver->Stop();
            Report(tr("Aborted: mount stopped"));
        } catch (const MountError &e) {
            Report(tr("ERROR while stopping the mount: %1").arg(e.message()));
        }
        CloseLog();
        SetState(State::IDLE, tr("Aborted"));
    } else {
        SetState(State::DISCONNECTED, tr("Not connected"));
    }
}

void PassTracker::Tick()
{
    if (!_driver || !_pointing) {
        _timer->stop();
        return;
    }

    const double now = _clock->NowJdUtc();
    const qint64 second = static_cast<qint64>(floor(now * DATE::NB_SEC_PAR_JOUR));
    const bool newSecond = (second != _lastStatusSecond);
    _lastStatusSecond = second;

    try {

        switch (_state) {

        case State::WAITING:
        {
            const double toStart = (_pass.jdStart - now) * DATE::NB_SEC_PAR_JOUR;

            if (toStart > _settings.prePositionSeconds) {
                if (newSecond) {
                    SetState(_state, tr("Pass starts in %1, pre-positioning in %2")
                             .arg(FormatSeconds(toStart), FormatSeconds(toStart - _settings.prePositionSeconds)));
                }
                break;
            }

            const double aim = NextAimTime(std::max(_pass.jdStart, now + _leadSeconds * DATE::NB_JOUR_PAR_SEC));
            if (aim < 0.) {
                Report(tr("No part of the pass is within the altitude limits"));
                Finish();
                break;
            }

            Report(tr("Slewing to the start point of the pass"));
            IssueGoto(aim);
            SetState(State::PREPOSITIONING, tr("Slewing to the start point"));
            break;
        }

        case State::PREPOSITIONING:
        case State::TRACKING:
        {
            if (now >= _pass.jdEnd) {
                Finish();
                break;
            }

            if (_slewing) {
                // Leave the mount a moment to report that it started moving
                if (((now - _gotoIssuedJd) * DATE::NB_SEC_PAR_JOUR < 0.3) || _driver->IsSlewing()) {
                    break;
                }
                _slewing = false;
                OnArrival(now);
            }

            if (now >= _aimJd + _settings.dwellSeconds * DATE::NB_JOUR_PAR_SEC) {

                const double aim = NextAimTime(now + _leadSeconds * DATE::NB_JOUR_PAR_SEC);
                if (aim < 0.) {
                    Finish();
                    break;
                }

                if (_state == State::PREPOSITIONING) {
                    Report(tr("Satellite reached the start point: tracking"));
                    _state = State::TRACKING;
                }

                IssueGoto(aim);

            } else if (newSecond) {

                const double toAim = (_aimJd - now) * DATE::NB_SEC_PAR_JOUR;
                const QString text = (_state == State::PREPOSITIONING) ?
                                         tr("At the start point, satellite arrives in %1").arg(FormatSeconds(std::max(0., toAim))) :
                                         tr("Tracking: goto #%1, lead %2 s, pass ends in %3")
                                             .arg(_gotoCount).arg(_leadSeconds, 0, 'f', 1)
                                             .arg(FormatSeconds((_pass.jdEnd - now) * DATE::NB_SEC_PAR_JOUR));
                SetState(_state, text);
            }
            break;
        }

        default:
            break;
        }

        if (_timer->isActive()) {

            const PointingSample sample = _pointing->Compute(now);

            // Once a second: mount position, separation from the satellite, and a line of the pass log
            if (newSecond && (_state != State::WAITING)) {
                ReadMountPosition(sample);
                if ((_state == State::TRACKING) && (_separationArcmin >= 0.)) {
                    _trackingSeparations.append(_separationArcmin);
                }
                WriteLogRow(now, &sample, QString());
            }

            emit positionUpdated(sample.azimuth * MATHS::RAD2DEG, sample.altitude * MATHS::RAD2DEG, _separationArcmin);
        }

    } catch (const MountError &e) {
        Fail(e.message());
    } catch (const std::exception &) {
        Fail(tr("Computation error"));
    }
}

void PassTracker::SetState(const State state,
                           const QString &description)
{
    _state = state;
    emit stateChanged(static_cast<int>(state), description);
}

void PassTracker::Fail(const QString &message)
{
    _timer->stop();
    _slewing = false;
    Report(tr("ERROR: %1").arg(message));

    if (_driver) {
        try {
            _driver->Stop();
            Report(tr("Mount stopped"));
        } catch (const MountError &) {
        }
    }

    CloseLog();

    if (_driver && !_driver->IsConnected()) {
        DisconnectMount();
        return;
    }

    SetState(State::IDLE, tr("Stopped after an error"));
}

double PassTracker::NextAimTime(const double jd)
{
    const double step = DATE::NB_JOUR_PAR_SEC;

    for (double t = jd; t <= _pass.jdEnd; t += step) {
        const PointingSample sample = _pointing->Compute(t);
        if ((sample.altitude >= _settings.minAltitude) && (sample.altitude <= _settings.maxAltitude)) {
            return t;
        }
    }

    return -1.;
}

void PassTracker::IssueGoto(const double jdAim)
{
    const PointingSample target = _pointing->Compute(jdAim);

    _driver->GotoRaDec(target.rightAscension, target.declination);

    _aimJd = jdAim;
    _aimTarget = target;
    _gotoIssuedJd = _clock->NowJdUtc();
    _slewing = true;
    _gotoCount++;
}

void PassTracker::OnArrival(const double now)
{
    const double slewSeconds = (now - _gotoIssuedJd) * DATE::NB_SEC_PAR_JOUR;

    // The next slew covers a similar distance: aim far enough ahead to arrive before the satellite
    // (the long slew to the start point says nothing about the short slews of the tracking phase)
    if (_state == State::TRACKING) {
        _leadSeconds = std::clamp(1.3 * slewSeconds + 0.5, MIN_LEAD_S, MAX_LEAD_S);
    }

    try {
        double ra, dec;
        _driver->GetRaDec(ra, dec);
        _lastErrorArcmin = SatellitePointing::AngularSeparation(ra, dec, _aimTarget.rightAscension, _aimTarget.declination)
                           * RAD2ARCMIN;
    } catch (const MountError &) {
        _lastErrorArcmin = -1.;
    }

    const double late = (now - _aimJd) * DATE::NB_SEC_PAR_JOUR;
    if (late > 0.) {
        if (_state == State::TRACKING) {
            _lateArrivals++;
        }
        Report(tr("Goto #%1 arrived %2 s after the satellite (slew took %3 s)")
               .arg(_gotoCount).arg(late, 0, 'f', 1).arg(slewSeconds, 0, 'f', 1));
    }

    WriteLogRow(now, nullptr, tr("Goto #%1 arrived: slew %2 s, pointing error %3'")
                .arg(_gotoCount).arg(slewSeconds, 0, 'f', 1).arg(_lastErrorArcmin, 0, 'f', 2));
}

void PassTracker::Finish()
{
    _timer->stop();
    _slewing = false;

    if (_settings.parkAtEnd && _driver) {
        try {
            if (_driver->Park()) {
                Report(tr("Parking the mount"));
            } else {
                Report(tr("This mount has no park command: mount stopped"));
            }
        } catch (const MountError &e) {
            Report(tr("ERROR while parking: %1").arg(e.message()));
        }
    }

    Report(TrackingSummary());
    CloseLog();
    SetState(State::FINISHED, tr("Pass finished (%1 gotos)").arg(_gotoCount));
}

void PassTracker::Report(const QString &message)
{
    emit logMessage(message);
    WriteLogRow(_clock->NowJdUtc(), nullptr, message);
}

void PassTracker::ReadMountPosition(const PointingSample &satellite)
{
    // A failed readback is not fatal (some mounts do not answer while slewing): the next command will tell
    try {
        _driver->GetRaDec(_mountRa, _mountDec);
        _mountPositionKnown = true;
        _separationArcmin = SatellitePointing::AngularSeparation(_mountRa, _mountDec, satellite.rightAscension,
                                                                 satellite.declination) * RAD2ARCMIN;
    } catch (const MountError &) {
        _mountPositionKnown = false;
        _separationArcmin = -1.;
    }
}

void PassTracker::OpenLog()
{
    CloseLog();

    if (_settings.logDirectory.isEmpty() || _pass.elements.isEmpty()) {
        return;
    }

    QDir().mkpath(_settings.logDirectory);
    const QString name = QString("mount_%1_%2.csv").arg(_pass.elements.first().norad.trimmed(),
                                                         SatellitePointing::ToDateTimeUtc(_pass.jdStart).toString("yyyyMMdd_HHmmss"));

    auto file = std::make_unique<QFile>(_settings.logDirectory + QDir::separator() + name);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit logMessage(tr("Cannot write the pass log %1").arg(file->fileName()));
        return;
    }

    file->write("utc,state,sat_az_deg,sat_alt_deg,sat_ra_h,sat_dec_deg,mount_ra_h,mount_dec_deg,separation_arcmin,"
                "last_arrival_error_arcmin,lead_s,gotos,event\n");
    _logFile = std::move(file);
    emit logMessage(tr("Pass log: %1").arg(QDir::toNativeSeparators(_logFile->fileName())));
}

void PassTracker::WriteLogRow(const double jd,
                              const PointingSample *satellite,
                              const QString &event)
{
    if (!_logFile) {
        return;
    }

    const auto number = [](const double value, const int decimals) {
        return QString::number(value, 'f', decimals);
    };

    QStringList fields;
    fields << SatellitePointing::ToDateTimeUtc(jd).toString("yyyy-MM-ddTHH:mm:ss.zzzZ")
           << QString(QMetaEnum::fromType<State>().valueToKey(static_cast<int>(_state)));

    if (satellite != nullptr) {
        fields << number(satellite->azimuth * MATHS::RAD2DEG, 4) << number(satellite->altitude * MATHS::RAD2DEG, 4)
               << number(satellite->rightAscension * MATHS::RAD2HEUR, 6) << number(satellite->declination * MATHS::RAD2DEG, 5);
        if (_mountPositionKnown) {
            fields << number(_mountRa * MATHS::RAD2HEUR, 6) << number(_mountDec * MATHS::RAD2DEG, 5) << number(_separationArcmin, 2);
        } else {
            fields << QString() << QString() << QString();
        }
        fields << ((_lastErrorArcmin >= 0.) ? number(_lastErrorArcmin, 2) : QString()) << number(_leadSeconds, 2)
               << QString::number(_gotoCount);
    } else {
        for (int i = 0; i < 10; i++) {
            fields << QString();
        }
    }

    QString text = event;
    fields << ((text.isEmpty()) ? QString() : "\"" + text.replace('"', "\"\"") + "\"");

    _logFile->write((fields.join(',') + '\n').toUtf8());
    _logFile->flush();
}

void PassTracker::CloseLog()
{
    if (_logFile) {
        _logFile->close();
        _logFile.reset();
    }
}

QString PassTracker::TrackingSummary() const
{
    if (_trackingSeparations.isEmpty()) {
        return tr("Tracking summary: no mount position measured during tracking");
    }

    QList<double> sorted = _trackingSeparations;
    std::sort(sorted.begin(), sorted.end());

    const auto percentile = [&sorted](const double p) {
        const qsizetype last = sorted.size() - 1;
        const qsizetype idx = std::min(last, static_cast<qsizetype>(std::lround(p * static_cast<double>(last))));
        return sorted.at(idx);
    };

    return tr("Tracking summary: %1 gotos, %2 late arrivals; mount - satellite separation over %3 s: "
              "median %4', 90% below %5', maximum %6'")
        .arg(_gotoCount).arg(_lateArrivals).arg(sorted.size())
        .arg(percentile(0.5), 0, 'f', 1).arg(percentile(0.9), 0, 'f', 1).arg(sorted.last(), 0, 'f', 1);
}

QString PassTracker::FormatSeconds(const double seconds)
{
    const qint64 total = static_cast<qint64>(std::lround(seconds));
    if (total >= 3600) {
        return QString("%1:%2:%3").arg(total / 3600).arg((total / 60) % 60, 2, 10, QChar('0')).arg(total % 60, 2, 10, QChar('0'));
    }
    return QString("%1:%2").arg(total / 60).arg(total % 60, 2, 10, QChar('0'));
}
