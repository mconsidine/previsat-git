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
#include <array>
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
    _lastStatusSecond(-1),
    _continuous(false),
    _rateActive(false),
    _rateRa(0.),
    _rateDec(0.),
    _lastRateJd(0.),
    _readFailures(0),
    _catchUps(0),
    _timingOffset(0.),
    _crossOffset(0.),
    _abortRequested(false)
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

        driver->SetAbortFlag(&_abortRequested);
        const QString identity = driver->Connect();
        emit logMessage(tr("Connected: %1").arg(identity));

        if (sendSiteAndTime) {
            driver->SetSiteAndTime(latitude, longitude, QDateTime::currentDateTimeUtc());
            emit logMessage(tr("Site and time sent to the mount"));
        }

        _driver = std::move(driver);

        if (sendSiteAndTime) {
            CheckMountClock(longitude);
        }
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

        if (_driver->IsEquatorial()) {
            double limitEast = 0.;
            double limitWest = 0.;
            const bool hasLimits = _driver->GetMeridianLimits(limitEast, limitWest);
            emit logMessage(MeridianAdvice(summary, hasLimits, limitEast, limitWest));
        }

        // Age of the orbital elements (SkyTrack advises less than about a day for close work)
        const double age = now - pass.elements.last().epoque.jourJulienUTC();
        if (age > 3.) {
            emit logMessage(tr("WARNING: the orbital elements are %1 days old: update them for an accurate track").arg(age, 0, 'f', 1));
        } else {
            emit logMessage(tr("Orbital elements: %1 days old").arg(age, 0, 'f', 1));
        }

        CheckMountClock(pass.observer.longitude());

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
    _rateActive = false;
    _rateRa = 0.;
    _rateDec = 0.;
    _readFailures = 0;
    _catchUps = 0;
    _lastPierSide.clear();

    // As SkyTrack: the timing correction (clock, element age) carries over to the next pass, the cross-track one does not
    _crossOffset = 0.;

    _continuous = (settings.mode == TrackingMode::CONTINUOUS);
    if (_continuous && !_driver->SupportsRaDecRates()) {
        _continuous = false;
        emit logMessage(tr("Continuous tracking unavailable (%1): using leapfrog tracking").arg(_driver->RateControlStatus()));
    }

    OpenLog();
    Report(tr("Tracking mode: %1").arg((_continuous) ? tr("continuous") : tr("leapfrog")));
    SetState(State::WAITING, tr("Armed"));
    _timer->start();
    Tick();
}

void PassTracker::RequestAbort()
{
    _abortRequested.store(true);
}

void PassTracker::Abort()
{
    _timer->stop();
    _rateActive = false;
    _abortRequested.store(false);

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

            if (_rateActive) {
                ContinuousStep(now);
                if (_rateActive && newSecond) {
                    SetState(_state, tr("Continuous tracking: separation %1', rate %2°/s, pass ends in %3")
                             .arg(_separationArcmin, 0, 'f', 1)
                             .arg(hypot(_rateRa * cos(_mountDec), _rateDec) * MATHS::RAD2DEG, 0, 'f', 2)
                             .arg(FormatSeconds((_pass.jdEnd - now) * DATE::NB_SEC_PAR_JOUR)));
                }
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

            if (_continuous) {

                // Waiting at the aim point (start of the pass, or after a catch-up): follow the satellite once it is there
                if (now >= _aimJd) {
                    if (_state == State::PREPOSITIONING) {
                        Report(tr("Satellite reached the start point: continuous tracking"));
                        _state = State::TRACKING;
                    } else {
                        Report(tr("Continuous tracking resumed"));
                    }
                    StartRates(now);
                } else if (newSecond) {
                    SetState(_state, tr("Waiting for the satellite: arrives in %1")
                             .arg(FormatSeconds((_aimJd - now) * DATE::NB_SEC_PAR_JOUR)));
                }
                break;
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
                if (!_rateActive) {
                    // (in continuous tracking the controller reads the position at every update)
                    ReadMountPosition(sample);
                }
                if ((_state == State::TRACKING) && (_separationArcmin >= 0.)) {
                    _trackingSeparations.append(_separationArcmin);
                }
                WriteLogRow(now, &sample, QString());

                // Every 5 s on equatorial mounts: report a change of pier side (meridian flip)
                if (_driver->IsEquatorial() && ((second % 5) == 0)) {
                    const QString side = _driver->PierSide();
                    if (!side.isEmpty()) {
                        if (!_lastPierSide.isEmpty() && (side != _lastPierSide)) {
                            Report(tr("Pier side changed from %1 to %2 (meridian flip)").arg(_lastPierSide, side));
                        }
                        _lastPierSide = side;
                    }
                }
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
    _rateActive = false;
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

PointingSample PassTracker::Target(const double jd)
{
    PointingSample target = _pointing->Compute(jd + _timingOffset * DATE::NB_JOUR_PAR_SEC);

    if (fabs(_crossOffset) > 0.) {

        // Rotate the target by the offset angle, perpendicular to its direction of motion
        const PointingSample next = _pointing->Compute(jd + (_timingOffset + 1.) * DATE::NB_JOUR_PAR_SEC);

        const auto unit = [](const double ra, const double dec) {
            return std::array<double, 3> { cos(dec) * cos(ra), cos(dec) * sin(ra), sin(dec) };
        };

        const std::array<double, 3> u = unit(target.rightAscension, target.declination);
        const std::array<double, 3> v = unit(next.rightAscension, next.declination);
        const std::array<double, 3> m { v[0] - u[0], v[1] - u[1], v[2] - u[2] };

        // u x m points to the right of the motion for an observer at the centre of the sphere
        std::array<double, 3> n { u[1] * m[2] - u[2] * m[1], u[2] * m[0] - u[0] * m[2], u[0] * m[1] - u[1] * m[0] };
        const double norm = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);

        if (norm > 1.e-15) {
            const double c = cos(_crossOffset);
            const double s = sin(_crossOffset) / norm;
            const std::array<double, 3> p { c * u[0] + s * n[0], c * u[1] + s * n[1], c * u[2] + s * n[2] };

            target.rightAscension = atan2(p[1], p[0]);
            if (target.rightAscension < 0.) {
                target.rightAscension += MATHS::DEUX_PI;
            }
            target.declination = asin(std::clamp(p[2], -1., 1.));
        }
    }

    return target;
}

void PassTracker::IssueGoto(const double jdAim)
{
    const PointingSample target = Target(jdAim);

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

void PassTracker::ContinuousStep(const double now)
{
    const ContinuousSettings &cs = _settings.continuous;

    const PointingSample satellite = _pointing->Compute(now);
    if ((satellite.altitude < _settings.minAltitude) || (satellite.altitude > _settings.maxAltitude)) {
        CatchUp(now, tr("satellite outside the altitude limits"));
        return;
    }

    // Measure the mount position; the measurement time is the middle of the request
    bool measured = true;
    double ra = 0.;
    double dec = 0.;
    const double t0 = _clock->NowJdUtc();
    try {
        _driver->GetRaDec(ra, dec);
        _readFailures = 0;
    } catch (const MountError &) {
        measured = false;
        if (++_readFailures > 3) {
            throw;
        }
    }
    const double t1 = _clock->NowJdUtc();

    // Time the reported position refers to: middle of the request, minus the age of the controller's position
    const double tMeasure = 0.5 * (t0 + t1) - cs.positionDelay * DATE::NB_JOUR_PAR_SEC;

    // A rate command takes effect about half a round trip after it is sent
    const double tCommand = t1 + 0.5 * (t1 - t0);

    // Feed-forward: rate of the target at the command time (centred difference over 1 s)
    const double half = 0.5 * DATE::NB_JOUR_PAR_SEC;
    const PointingSample before = Target(tCommand - half);
    const PointingSample after = Target(tCommand + half);
    const double feedRa = remainder(after.rightAscension - before.rightAscension, MATHS::DEUX_PI);
    const double feedDec = after.declination - before.declination;

    double correctionRa = 0.;
    double correctionDec = 0.;

    if (measured) {

        _mountRa = ra;
        _mountDec = dec;
        _mountPositionKnown = true;

        // Separation from the satellite where it was when the reported position was valid
        const PointingSample measuredSatellite = _pointing->Compute(tMeasure);
        _separationArcmin = SatellitePointing::AngularSeparation(ra, dec, measuredSatellite.rightAscension,
                                                                 measuredSatellite.declination) * RAD2ARCMIN;

        // Error at the measurement time, carried to the command time with the rate difference
        const PointingSample target = Target(tMeasure);
        const double lag = (tCommand - tMeasure) * DATE::NB_SEC_PAR_JOUR;
        const double errorRa = remainder(target.rightAscension - ra, MATHS::DEUX_PI) + (feedRa - _rateRa) * lag;
        const double errorDec = (target.declination - dec) + (feedDec - _rateDec) * lag;
        const double cosDec = std::max(cos(target.declination), 0.05);
        const double errorSky = hypot(errorRa * cosDec, errorDec);

        if (errorSky > cs.catchUpThreshold * MATHS::DEG2RAD) {
            _catchUps++;
            CatchUp(now, tr("mount %1° from the target").arg(errorSky * MATHS::RAD2DEG, 0, 'f', 1));
            return;
        }

        if (errorSky > cs.deadband * MATHS::ARCMIN2RAD) {

            const double scale = std::min(1., errorSky / (cs.gainErrorScale * MATHS::ARCMIN2RAD));
            const double gain = cs.minGain + (cs.maxGain - cs.minGain) * scale;

            // Correction on the sky, limited in magnitude, then back to coordinate rates
            double skyRa = gain * errorRa * cosDec;
            double skyDec = gain * errorDec;
            const double magnitude = hypot(skyRa, skyDec);
            const double maxCorrection = cs.maxCorrectionRate * MATHS::DEG2RAD;
            if (magnitude > maxCorrection) {
                skyRa *= maxCorrection / magnitude;
                skyDec *= maxCorrection / magnitude;
            }

            correctionRa = skyRa / cosDec;
            correctionDec = skyDec;
        }

    } else {
        _mountPositionKnown = false;
        _separationArcmin = -1.;
    }

    // Rate and acceleration limits
    const double dt = std::clamp((now - _lastRateJd) * DATE::NB_SEC_PAR_JOUR, 0.05, 1.);
    const double maxRate = cs.maxRate * MATHS::DEG2RAD;
    const double maxChange = cs.maxAcceleration * MATHS::DEG2RAD * dt;

    double rateRa = std::clamp(feedRa + correctionRa, -maxRate, maxRate);
    double rateDec = std::clamp(feedDec + correctionDec, -maxRate, maxRate);
    rateRa = std::clamp(rateRa, _rateRa - maxChange, _rateRa + maxChange);
    rateDec = std::clamp(rateDec, _rateDec - maxChange, _rateDec + maxChange);

    _driver->SetRaDecRates(rateRa, rateDec);
    _rateRa = rateRa;
    _rateDec = rateDec;
    _lastRateJd = now;
}

void PassTracker::StartRates(const double now)
{
    _rateActive = true;
    _rateRa = 0.;
    _rateDec = 0.;
    _readFailures = 0;

    // As if the previous update were one tick ago, so that the first rate change is not over-limited
    _lastRateJd = now - TICK_MS * DATE::NB_JOUR_PAR_MILLISEC;

    ContinuousStep(now);
}

void PassTracker::StopRates()
{
    if (_rateActive) {
        _rateActive = false;
        _rateRa = 0.;
        _rateDec = 0.;
        _driver->SetRaDecRates(0., 0.);
    }
}

void PassTracker::CatchUp(const double now,
                          const QString &reason)
{
    StopRates();

    const double aim = NextAimTime(now + _leadSeconds * DATE::NB_JOUR_PAR_SEC);
    if (aim < 0.) {
        Finish();
        return;
    }

    Report(tr("Catching up with a goto (%1)").arg(reason));
    IssueGoto(aim);
}

void PassTracker::CheckMountClock(const double longitude)
{
    double mountLst = 0.;
    try {
        if (!_driver || !_driver->GetSiderealTime(mountLst)) {
            return;
        }
    } catch (const MountError &e) {
        emit logMessage(tr("Cannot read the sidereal time of the mount: %1").arg(e.message()));
        return;
    }

    const double pcLst = SatellitePointing::LocalSiderealTime(_clock->NowJdUtc(), longitude);
    emit logMessage(ClockAdvice(remainder(mountLst - pcLst, MATHS::DEUX_PI)));
}

QString PassTracker::ClockAdvice(const double lstDifference)
{
    // 1 radian of sidereal time = 86164.1 / (2 pi) seconds of time
    const double seconds = lstDifference * 86164.0905 / MATHS::DEUX_PI;
    const double arcmin = fabs(lstDifference) * RAD2ARCMIN;

    if (fabs(seconds) < 2.) {
        return tr("Mount clock OK: its sidereal time is within %1 s of the computer's").arg(fabs(seconds), 0, 'f', 1);
    }

    QString text = tr("WARNING: the mount's sidereal time differs from the computer's by %1 s, i.e. pointing errors "
                      "up to %2' in right ascension.").arg(seconds, 0, 'f', 1).arg(arcmin, 0, 'f', 1);

    if (fabs(seconds) > 1200.) {
        text += " " + tr("A difference this large usually comes from a wrong time zone / daylight saving setting or a "
                          "longitude sign error in the mount.");
    }

    return text + " " + tr("Check the mount's time, UTC offset and longitude, or connect with \"Send observing site and computer time\".");
}

QString PassTracker::MeridianAdvice(const PassSummary &summary,
                                    const bool hasLimits,
                                    const double limitEast,
                                    const double limitWest)
{
    const auto degrees = [](const double angle) {
        return QString::number(angle * MATHS::RAD2DEG, 'f', 1);
    };

    // 4 minutes of time per degree of hour angle
    const auto minutes = [](const double angle) {
        return static_cast<int>(std::ceil(fabs(angle) * MATHS::RAD2DEG * 4.));
    };

    if (!summary.valid) {
        return QString();
    }

    if (!summary.crossesMeridian) {
        return tr("Meridian: the pass stays %1 of the meridian (hour angles %2° to %3°): no meridian flip expected")
            .arg((summary.maxHourAngle < 0.) ? tr("east") : tr("west"), degrees(summary.minHourAngle), degrees(summary.maxHourAngle));
    }

    // Starting east of the meridian (hour angle < 0), the mount keeps that side if it may track past the meridian
    // up to the largest hour angle; otherwise it must start on the other side and reach back to the smallest one
    const bool startsEast = (summary.startHourAngle < 0.);
    const int sameSide = (startsEast) ? minutes(summary.maxHourAngle) : minutes(summary.minHourAngle);
    const int otherSide = (startsEast) ? minutes(summary.minHourAngle) : minutes(summary.maxHourAngle);

    QString text = tr("WARNING: the pass crosses the meridian at %1 UTC, moving %2°/s there (hour angles %3° to %4°). "
                      "To follow it on one side of the pier without a flip, the mount must be allowed %5 min past the "
                      "meridian on the side for the %6 sky, or %7 min on the side for the %8 sky.")
                       .arg(SatellitePointing::ToDateTimeUtc(summary.jdMeridian).toString("HH:mm:ss"))
                       .arg(summary.rateAtMeridian * MATHS::RAD2DEG, 0, 'f', 2)
                       .arg(degrees(summary.minHourAngle), degrees(summary.maxHourAngle))
                       .arg(sameSide)
                       .arg((startsEast) ? tr("eastern") : tr("western"))
                       .arg(otherSide)
                       .arg((startsEast) ? tr("western") : tr("eastern"));

    if (hasLimits) {
        const int east = minutes(limitEast);
        const int west = minutes(limitWest);
        text += " " + tr("The mount's meridian limits are E %1 min, W %2 min:").arg(east).arg(west) + " ";
        text += (std::max(east, west) >= std::min(sameSide, otherSide)) ?
                    tr("one of them may be enough, depending on the side the mount uses.") :
                    tr("a flip is likely, and the satellite will be lost while it happens. Widen a limit if the mount and "
                       "the optical train have the clearance.");
    }

    return text;
}

void PassTracker::CalibrateMount()
{
    if (!_driver) {
        emit logMessage(tr("Connect the mount first"));
        return;
    }

    if ((_state == State::WAITING) || (_state == State::PREPOSITIONING) || (_state == State::TRACKING)) {
        emit logMessage(tr("A pass is armed: abort it before calibrating"));
        return;
    }

    if (!_driver->CanCalibrateRates()) {
        emit logMessage(tr("No rate calibration: %1").arg(_driver->RateControlStatus()));
        return;
    }

    _abortRequested.store(false);
    SetState(_state, tr("Calibrating rate control (the mount moves on each axis)..."));
    emit logMessage(tr("Rate calibration started: the mount moves about 1° on each axis and comes back (about 25 s)"));

    try {
        emit logMessage(_driver->CalibrateRates());
        SetState(_state, tr("Rate control calibrated"));
    } catch (const MountError &e) {
        try {
            _driver->Stop();
        } catch (const MountError &) {
        }
        emit logMessage(tr("Rate calibration FAILED: %1").arg(e.message()));
        SetState(_state, tr("Rate calibration failed"));
    }
}

void PassTracker::SetOffsets(const double timingSeconds,
                             const double crossTrackArcmin)
{
    _timingOffset = timingSeconds;
    _crossOffset = crossTrackArcmin * MATHS::ARCMIN2RAD;
    Report(tr("Offsets: timing %1 s, cross-track %2'").arg(timingSeconds, 0, 'f', 2).arg(crossTrackArcmin, 0, 'f', 1));
}

void PassTracker::Finish()
{
    _timer->stop();
    _slewing = false;

    if (_driver) {
        try {
            StopRates();
        } catch (const MountError &e) {
            Report(tr("ERROR while returning to sidereal tracking: %1").arg(e.message()));
        }
    }
    _rateActive = false;

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
                "last_arrival_error_arcmin,lead_s,gotos,mode,rate_ra_arcsec_s,rate_dec_arcsec_s,timing_offset_s,"
                "cross_offset_arcmin,event\n");
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
               << QString::number(_gotoCount)
               << ((_rateActive) ? "rate" : "goto")
               << number(_rateRa * MATHS::RAD2DEG * MATHS::ARCSEC_PAR_DEG, 1)
               << number(_rateDec * MATHS::RAD2DEG * MATHS::ARCSEC_PAR_DEG, 1)
               << number(_timingOffset, 2) << number(_crossOffset * RAD2ARCMIN, 2);
    } else {
        for (int i = 0; i < 15; i++) {
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

    return tr("Tracking summary (%1): %2 gotos, %3 catch-ups, %4 late arrivals; mount - satellite separation over %5 s: "
              "median %6', 90% below %7', maximum %8'")
        .arg((_continuous) ? tr("continuous") : tr("leapfrog"))
        .arg(_gotoCount).arg(_catchUps).arg(_lateArrivals).arg(sorted.size())
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
