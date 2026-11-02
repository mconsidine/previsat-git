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
 * >    lx200driver.cpp
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    MountDriver
 *
 * Description
 * >    Meade LX200 command set: classic LX200, and the OnStep / OnStepX dialect
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <algorithm>
#include <cmath>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QRegularExpression>
#include <QThread>
#include "librairies/maths/mathsconst.h"
#include "lx200driver.h"


// Sidereal rate (deg/s): how fast the right ascension read on an equatorial mount grows when its axes stand still
static constexpr double SIDEREAL_DEG_S = 360.98564736629 / 86400.;


static QString tr(const char *text)
{
    return QCoreApplication::translate("Lx200Driver", text);
}

/*
 * Monotonic time (ms)
 */
static qint64 NowMs()
{
    static QElapsedTimer timer;
    if (!timer.isValid()) {
        timer.start();
    }
    return timer.elapsed();
}


Lx200Driver::Lx200Driver(const TransportSettings &settings,
                         const bool onStep) :
    MountDriver(settings),
    _onStep(onStep),
    _highPrecision(true),
    _extendedPositions(false),
    _rateCalibrated(false),
    _rateChangeDuringMove(false),
    _checkPierSide(false),
    _guiding(false),
    _direction1(0),
    _direction2(0),
    _lastMoveMs { 0, 0 }
{
}

QString Lx200Driver::Name() const
{
    return (_onStep) ? "OnStep / OnStepX (LX200)" : "Meade LX200 classic";
}

QString Lx200Driver::Handshake()
{
    // ACK returns the alignment mode: A = alt-azimuth, P = polar (fork on wedge), G = German equatorial, L = land
    char mode = '?';
    _transport.Send(QByteArray(1, '\x06'));
    try {
        mode = _transport.ReadCount(1, REPLY_TIMEOUT_MS).at(0);
    } catch (const MountError &) {
        // Not every firmware answers ACK: the position query below is the real test
    }

    QByteArray ra = Query(":GR#");

    // Classic LX200 starts in low precision (HH:MM.T): toggle to high precision (HH:MM:SS)
    if (ra.count(':') < 2) {
        _transport.Send(":U#");
        ra = Query(":GR#");
    }

    _highPrecision = (ra.count(':') >= 2);
    ParseRightAscension(ra);

    _equatorial = ((mode == 'P') || (mode == 'G'));

    // OnStepX extended precision positions (sub-arcsecond): better feedback for continuous tracking
    _extendedPositions = false;
    if (_onStep) {
        try {
            ParseRightAscension(Query(":GRH#", 500));
            ParseDeclination(Query(":GDH#", 500));
            _extendedPositions = true;
        } catch (const MountError &) {
        }
    }

    _rateCalibrated = false;
    _guiding = false;
    _direction1 = 0;
    _direction2 = 0;

    QString identity = Name();
    if (_onStep) {
        try {
            identity = QString("%1 %2").arg(QString::fromLatin1(Query(":GVP#")), QString::fromLatin1(Query(":GVN#")));
        } catch (const MountError &) {
        }
    }

    QString geometry;
    switch (mode) {
    case 'A':
        geometry = tr("alt-azimuth");
        break;
    case 'P':
        geometry = tr("polar");
        break;
    case 'G':
        geometry = tr("German equatorial");
        break;
    case 'L':
        geometry = tr("land");
        break;
    default:
        geometry = tr("unknown geometry");
        break;
    }

    return QString("%1 (%2, %3)").arg(identity, geometry, (_highPrecision) ? tr("high precision") : tr("low precision"));
}

void Lx200Driver::SetSiteAndTime(const double latitude,
                                 const double longitude,
                                 const QDateTime &utc)
{
    if (!QueryBool(":St" + FormatLatitude(latitude) + "#")) {
        throw MountError(tr("Mount rejected the latitude"));
    }

    if (!QueryBool(":Sg" + FormatLongitude(longitude) + "#")) {
        throw MountError(tr("Mount rejected the longitude"));
    }

    const int offset = utc.toLocalTime().offsetFromUtc();
    const QDateTime local = utc.toOffsetFromUtc(offset);

    if (!QueryBool(":SG" + FormatUtcOffset(offset, _onStep) + "#")) {
        throw MountError(tr("Mount rejected the UTC offset"));
    }

    if (!QueryBool(":SL" + local.time().toString("HH:mm:ss").toLatin1() + "#")) {
        throw MountError(tr("Mount rejected the local time"));
    }

    _transport.Send(":SC" + local.date().toString("MM/dd/yy").toLatin1() + "#");
    if (_transport.ReadCount(1, REPLY_TIMEOUT_MS).at(0) != '1') {
        throw MountError(tr("Mount rejected the date"));
    }

    // The classic LX200 then sends "Updating planetary data#" and a line of blanks, and can take several seconds
    const int drainTimeout = (_onStep) ? 500 : 10000;
    try {
        _transport.ReadUntil('#', drainTimeout);
        _transport.ReadUntil('#', drainTimeout);
    } catch (const MountError &) {
    }
}

void Lx200Driver::GotoRaDec(const double rightAscension,
                            const double declination)
{
    // OnStep refuses a goto while the axes are guiding
    if (_guiding) {
        StopGuiding();
        WaitForStop(3000);
    }

    if (!QueryBool(":Sr" + FormatRightAscension(rightAscension, _highPrecision) + "#")) {
        throw MountError(tr("Mount rejected the target right ascension"));
    }

    if (!QueryBool(":Sd" + FormatDeclination(declination, _highPrecision) + "#")) {
        throw MountError(tr("Mount rejected the target declination"));
    }

    _transport.Send(":MS#");
    const char code = _transport.ReadCount(1, REPLY_TIMEOUT_MS).at(0);

    if (code != '0') {
        if (!_onStep) {
            // Classic LX200 follows the error code by a message terminated by #
            try {
                _transport.ReadUntil('#', 500);
            } catch (const MountError &) {
            }
        }
        throw MountError(GotoErrorText(code, _onStep));
    }

    // A German mount may change side of the pier during the goto
    _checkPierSide = true;
}

bool Lx200Driver::IsSlewing()
{
    // Distance bars: one or more characters while slewing, empty otherwise
    const QByteArray bars = Query(":D#");
    return !bars.trimmed().isEmpty();
}

void Lx200Driver::GetRaDec(double &rightAscension,
                           double &declination)
{
    if (_extendedPositions) {
        rightAscension = ParseRightAscension(Query(":GRH#"));
        declination = ParseDeclination(Query(":GDH#"));
    } else {
        rightAscension = ParseRightAscension(Query(":GR#"));
        declination = ParseDeclination(Query(":GD#"));
    }
}

void Lx200Driver::Stop()
{
    _transport.Send(":Q#");
    _guiding = false;
    _direction1 = 0;
    _direction2 = 0;
}

bool Lx200Driver::GetSiderealTime(double &localSiderealTime)
{
    // Same format as a right ascension (HH:MM:SS, or HH:MM:SS.ss on OnStepX)
    localSiderealTime = ParseRightAscension(Query((_extendedPositions) ? ":GSH#" : ":GS#"));
    return true;
}

bool Lx200Driver::GetMeridianLimits(double &limitEast,
                                    double &limitWest)
{
    if (!_onStep || !_equatorial) {
        return false;
    }

    try {
        bool okEast = false;
        bool okWest = false;
        const double minutesEast = Query(":GXE9#", 800).trimmed().toDouble(&okEast);
        const double minutesWest = Query(":GXEA#", 800).trimmed().toDouble(&okWest);
        if (!okEast || !okWest) {
            return false;
        }

        // 4 minutes of time per degree of hour angle
        limitEast = minutesEast * 0.25 * MATHS::DEG2RAD;
        limitWest = minutesWest * 0.25 * MATHS::DEG2RAD;
        return true;

    } catch (const MountError &) {
        return false;
    }
}

QString Lx200Driver::PierSide()
{
    if (!_onStep || !_equatorial) {
        return QString();
    }

    const QByteArray side = QueryPierSide();
    return ((side == "E") || (side == "W")) ? QString::fromLatin1(side) : QString();
}

bool Lx200Driver::SupportsRaDecRates() const
{
    return _onStep && _equatorial && _rateCalibrated;
}

QString Lx200Driver::RateControlStatus() const
{
    if (!_onStep) {
        return tr("the classic LX200 has no variable rate: leapfrog tracking only");
    }
    if (!_equatorial) {
        return tr("OnStep rate control is implemented for equatorial mounts only");
    }
    if (!_rateCalibrated) {
        return tr("run the rate calibration first (Calibrate rate control)");
    }
    return tr("rate control calibrated (pier side %1)").arg(QString::fromLatin1(_pierSide));
}

bool Lx200Driver::CanCalibrateRates() const
{
    return _onStep && _equatorial;
}

void Lx200Driver::SetRaDecRates(const double raRate,
                                const double decRate)
{
    if (!SupportsRaDecRates()) {
        throw MountError(RateControlStatus());
    }

    // After a goto, a German mount on the other side of the pier runs its declination axis the other way
    if (_checkPierSide) {
        _checkPierSide = false;
        const QByteArray side = QueryPierSide();
        if (((side == "E") || (side == "W")) && ((_pierSide == "E") || (_pierSide == "W")) && (side != _pierSide)) {
            _axis2.scale = -_axis2.scale;
            _pierSide = side;
        }
    }

    // (0, 0): back to plain sidereal tracking
    if ((raRate == 0.) && (decRate == 0.)) {
        StopGuiding();
        return;
    }

    SendAxisMove(1, MoveSpeed(raRate * MATHS::RAD2DEG, _axis1));
    SendAxisMove(2, MoveSpeed(decRate * MATHS::RAD2DEG, _axis2));
    _guiding = true;
}

QString Lx200Driver::CalibrateRates()
{
    if (!CanCalibrateRates()) {
        throw MountError(RateControlStatus());
    }

    _rateCalibrated = false;
    Stop();
    WaitForStop(5000);

    // Sidereal tracking on (the calibration measures what the moves do on top of it)
    try {
        QueryBool(":Te#");
    } catch (const MountError &) {
    }

    double ra0, dec0;
    qint64 t;
    ReadPositionDeg(ra0, dec0, t);
    if (fabs(dec0) > 70.) {
        throw MountError(tr("Point the mount between declinations -70° and +70° before calibrating (now %1°)")
                         .arg(dec0, 0, 'f', 1));
    }

    _pierSide = QueryPierSide();

    // Position query round trip
    qint64 roundTrip = 0;
    for (int i = 0; i < 3; i++) {
        const qint64 start = NowMs();
        double ra, dec;
        GetRaDec(ra, dec);
        roundTrip += NowMs() - start;
    }
    roundTrip /= 3;

    const double speed = CALIBRATION_SPEED;
    AxisCalibration axis1, axis2;
    bool rateChangeDuringMove = false;

    try {

        const double west = MeasureMove(1, true, speed);
        const double east = MeasureMove(1, false, speed);
        const double north = MeasureMove(2, true, speed);
        const double south = MeasureMove(2, false, speed);

        QString error;
        if (!SolveAxisCalibration(west, east, speed, axis1, error)) {
            throw MountError(tr("Axis 1 (right ascension): %1").arg(error));
        }
        if (!SolveAxisCalibration(north, south, speed, axis2, error)) {
            throw MountError(tr("Axis 2 (declination): %1").arg(error));
        }

        // Does a new speed apply to a move in progress? Axis 1 at half speed, then full speed without re-sending :Mw#
        _transport.Send(":RA" + FormatGuideRate(0.5 * speed) + "#");
        _transport.Send(":Mw#");
        SleepMs(800);
        double raA, raB, raC, raD, dec;
        qint64 tA, tB, tC, tD;
        ReadPositionDeg(raA, dec, tA);
        SleepMs(800);
        ReadPositionDeg(raB, dec, tB);
        _transport.Send(":RA" + FormatGuideRate(speed) + "#");
        SleepMs(700);
        ReadPositionDeg(raC, dec, tC);
        SleepMs(800);
        ReadPositionDeg(raD, dec, tD);
        _transport.Send(":Qw#");
        WaitForStop(5000);

        const double slow = remainder(raB - raA, 360.) / std::max(0.1, (tB - tA) / 1000.) - axis1.background;
        const double fast = remainder(raD - raC, 360.) / std::max(0.1, (tD - tC) / 1000.) - axis1.background;
        rateChangeDuringMove = (fabs(fast) > 1.5 * fabs(slow));

    } catch (const MountError &) {
        try {
            Stop();
        } catch (const MountError &) {
        }
        throw;
    }

    // Back to the starting point
    QString back = tr("returned to the starting point");
    try {
        GotoRaDec(ra0 * MATHS::DEG2RAD, dec0 * MATHS::DEG2RAD);
        SleepMs(500);
        WaitForStop(60000);
    } catch (const MountError &e) {
        back = tr("could not return to the starting point: %1").arg(e.message());
    }

    _axis1 = axis1;
    _axis2 = axis2;
    _rateChangeDuringMove = rateChangeDuringMove;
    _rateCalibrated = true;
    _checkPierSide = false;
    _pierSide = QueryPierSide();

    QString tracking;
    if (fabs(axis1.background) < 0.3 * SIDEREAL_DEG_S) {
        tracking = tr("continues");
    } else if (fabs(axis1.background - SIDEREAL_DEG_S) < 0.3 * SIDEREAL_DEG_S) {
        tracking = tr("is suspended (compensated)");
    } else {
        tracking = tr("behaves unexpectedly (RA drifts %1\"/s with the axis at rest)").arg(axis1.background * 3600., 0, 'f', 1);
    }

    return tr("Rate calibration done, pier side %1, %2.\n"
              "  Axis 1: RA rate = %3 + %4 x speed; axis 2: Dec rate = %5 + %6 x speed (deg/s).\n"
              "  Sidereal tracking %7 during moves; a speed change %8 a move in progress.\n"
              "  Position query round trip %9 ms.")
        .arg(_pierSide.isEmpty() ? tr("unknown") : QString::fromLatin1(_pierSide), back)
        .arg(axis1.background, 0, 'f', 5).arg(axis1.scale, 0, 'f', 3)
        .arg(axis2.background, 0, 'f', 5).arg(axis2.scale, 0, 'f', 3)
        .arg(tracking, (rateChangeDuringMove) ? tr("applies to") : tr("needs the move to be re-sent for"))
        .arg(roundTrip);
}

bool Lx200Driver::SolveAxisCalibration(const double rateForward,
                                       const double rateReverse,
                                       const double speed,
                                       AxisCalibration &axis,
                                       QString &error)
{
    if (speed <= 0.) {
        error = tr("invalid calibration speed");
        return false;
    }

    axis.scale = (rateForward - rateReverse) / (2. * speed);
    axis.background = 0.5 * (rateForward + rateReverse);

    const double magnitude = fabs(axis.scale);
    if ((magnitude < 0.8) || (magnitude > 1.25)) {
        error = tr("moved at %1°/s for a commanded %2°/s: the rate commands do not behave as expected")
                    .arg(magnitude * speed, 0, 'f', 3).arg(speed, 0, 'f', 3);
        return false;
    }

    return true;
}

double Lx200Driver::MoveSpeed(const double coordinateRate,
                              const AxisCalibration &axis)
{
    if (fabs(axis.scale) < 1.e-6) {
        return 0.;
    }
    return (coordinateRate - axis.background) / axis.scale;
}

QByteArray Lx200Driver::FormatGuideRate(const double degreesPerSecond)
{
    return QByteArray::number(degreesPerSecond, 'f', 6);
}

void Lx200Driver::SendAxisMove(const int axis,
                               const double speed)
{
    const int direction = (fabs(speed) < MIN_MOVE_SPEED) ? 0 : ((speed > 0.) ? 1 : -1);
    int &current = (axis == 1) ? _direction1 : _direction2;
    qint64 &lastMove = _lastMoveMs[axis - 1];

    if (direction == 0) {
        if (current != 0) {
            _transport.Send((axis == 1) ? ":Qw#" : ":Qn#");
            current = 0;
        }
        return;
    }

    _transport.Send(QByteArray((axis == 1) ? ":RA" : ":RE") + FormatGuideRate(fabs(speed)) + "#");

    // Re-send the move on a change of direction, when a new speed needs it, and regularly to keep it alive
    const qint64 now = NowMs();
    if ((direction != current) || !_rateChangeDuringMove || (now - lastMove >= MOVE_KEEPALIVE_MS)) {
        if (axis == 1) {
            _transport.Send((direction > 0) ? ":Mw#" : ":Me#");
        } else {
            _transport.Send((direction > 0) ? ":Mn#" : ":Ms#");
        }
        current = direction;
        lastMove = now;
    }
}

void Lx200Driver::StopGuiding()
{
    _transport.Send(":Qw#");
    _transport.Send(":Qn#");
    _direction1 = 0;
    _direction2 = 0;
    _guiding = false;
}

QByteArray Lx200Driver::QueryPierSide()
{
    try {
        return Query(":Gm#", 800).trimmed();
    } catch (const MountError &) {
        return QByteArray();
    }
}

void Lx200Driver::ReadPositionDeg(double &ra,
                                  double &dec,
                                  qint64 &timeMs)
{
    const qint64 start = NowMs();
    GetRaDec(ra, dec);
    timeMs = (start + NowMs()) / 2;
    ra *= MATHS::RAD2DEG;
    dec *= MATHS::RAD2DEG;
}

double Lx200Driver::MeasureMove(const int axis,
                                const bool forward,
                                const double speed)
{
    _transport.Send(QByteArray((axis == 1) ? ":RA" : ":RE") + FormatGuideRate(speed) + "#");
    if (axis == 1) {
        _transport.Send((forward) ? ":Mw#" : ":Me#");
    } else {
        _transport.Send((forward) ? ":Mn#" : ":Ms#");
    }

    // Leave time to accelerate, then measure over a constant-speed stretch
    SleepMs(800);
    double ra0, dec0, ra1, dec1;
    qint64 t0, t1;
    ReadPositionDeg(ra0, dec0, t0);
    SleepMs(1200);
    ReadPositionDeg(ra1, dec1, t1);

    _transport.Send((axis == 1) ? ":Qw#" : ":Qn#");
    WaitForStop(5000);

    const double seconds = (t1 - t0) / 1000.;
    if (seconds < 0.2) {
        throw MountError(tr("calibration timing error"));
    }

    return (axis == 1) ? remainder(ra1 - ra0, 360.) / seconds : (dec1 - dec0) / seconds;
}

void Lx200Driver::WaitForStop(const int timeoutMs)
{
    const qint64 end = NowMs() + timeoutMs;
    while (NowMs() < end) {
        if (!IsSlewing()) {
            break;
        }
        SleepMs(100);
    }

    // Let the axes settle
    SleepMs(300);
}

bool Lx200Driver::Park()
{
    if (!_onStep) {
        Stop();
        return false;
    }

    return QueryBool(":hP#");
}

QByteArray Lx200Driver::FormatRightAscension(const double rightAscension,
                                             const bool highPrecision)
{
    double hours = fmod(rightAscension / MATHS::HEUR2RAD, 24.);
    if (hours < 0.) {
        hours += 24.;
    }

    if (highPrecision) {
        const int total = static_cast<int>(std::lround(hours * 3600.)) % 86400;
        return QString("%1:%2:%3").arg(total / 3600, 2, 10, QChar('0')).arg((total / 60) % 60, 2, 10, QChar('0'))
            .arg(total % 60, 2, 10, QChar('0')).toLatin1();
    }

    // Tenths of a minute
    const int total = static_cast<int>(std::lround(hours * 600.)) % 14400;
    return QString("%1:%2.%3").arg(total / 600, 2, 10, QChar('0')).arg((total / 10) % 60, 2, 10, QChar('0'))
        .arg(total % 10).toLatin1();
}

QByteArray Lx200Driver::FormatDeclination(const double declination,
                                          const bool highPrecision)
{
    const double deg = std::min(fabs(declination * MATHS::RAD2DEG), 90.);
    const QChar sign = (declination < 0.) ? '-' : '+';

    if (highPrecision) {
        const int total = static_cast<int>(std::lround(deg * 3600.));
        return QString("%1%2*%3:%4").arg(sign).arg(total / 3600, 2, 10, QChar('0')).arg((total / 60) % 60, 2, 10, QChar('0'))
            .arg(total % 60, 2, 10, QChar('0')).toLatin1();
    }

    const int total = static_cast<int>(std::lround(deg * 60.));
    return QString("%1%2*%3").arg(sign).arg(total / 60, 2, 10, QChar('0')).arg(total % 60, 2, 10, QChar('0')).toLatin1();
}

QByteArray Lx200Driver::FormatLatitude(const double latitude)
{
    return FormatDeclination(latitude, false);
}

QByteArray Lx200Driver::FormatLongitude(const double longitude)
{
    double deg = fmod(longitude * MATHS::RAD2DEG, 360.);
    if (deg < 0.) {
        deg += 360.;
    }

    const int total = static_cast<int>(std::lround(deg * 60.)) % 21600;
    return QString("%1*%2").arg(total / 60, 3, 10, QChar('0')).arg(total % 60, 2, 10, QChar('0')).toLatin1();
}

QByteArray Lx200Driver::FormatUtcOffset(const int offsetFromUtcSeconds,
                                        const bool onStep)
{
    // Meade convention: number of hours to ADD to local time to obtain UTC
    const int minutes = -offsetFromUtcSeconds / 60;
    const QChar sign = (minutes < 0) ? '-' : '+';
    const int absMinutes = std::abs(minutes);

    if ((absMinutes % 60) == 0) {
        return QString("%1%2").arg(sign).arg(absMinutes / 60, 2, 10, QChar('0')).toLatin1();
    }

    if (onStep) {
        return QString("%1%2:%3").arg(sign).arg(absMinutes / 60, 2, 10, QChar('0')).arg(absMinutes % 60, 2, 10, QChar('0')).toLatin1();
    }

    return QString("%1%2").arg(sign).arg(absMinutes / 60., 4, 'f', 1, QChar('0')).toLatin1();
}

double Lx200Driver::ParseRightAscension(const QByteArray &text)
{
    static const QRegularExpression re(R"(^\s*(\d{1,2}):(\d{1,2})([:.])(\d+(?:\.\d+)?)\s*$)");
    const QRegularExpressionMatch m = re.match(QString::fromLatin1(text));

    if (!m.hasMatch()) {
        throw MountError(tr("Unexpected right ascension from mount: %1").arg(QString::fromLatin1(text)));
    }

    const double hours = m.captured(1).toDouble();
    double minutes = m.captured(2).toDouble();
    double seconds = 0.;

    if (m.captured(3) == ":") {
        seconds = m.captured(4).toDouble();
    } else {
        minutes += 0.1 * m.captured(4).left(1).toDouble();
    }

    return (hours + minutes / 60. + seconds / 3600.) * MATHS::HEUR2RAD;
}

double Lx200Driver::ParseDeclination(const QByteArray &text)
{
    // Separators: '*', ':', '\'', or the degree sign (0xDF on Meade hand controllers)
    static const QRegularExpression re(R"(^\s*([+-]?)(\d{1,3})[^0-9](\d{1,2})(?:[^0-9](\d+(?:\.\d+)?))?\s*$)");
    const QRegularExpressionMatch m = re.match(QString::fromLatin1(text));

    if (!m.hasMatch()) {
        throw MountError(tr("Unexpected declination from mount: %1").arg(QString::fromLatin1(text)));
    }

    const double deg = m.captured(2).toDouble() + m.captured(3).toDouble() / 60. + m.captured(4).toDouble() / 3600.;
    return ((m.captured(1) == "-") ? -deg : deg) * MATHS::DEG2RAD;
}

QString Lx200Driver::GotoErrorText(const char code,
                                   const bool onStep)
{
    switch (code) {
    case '1':
        return tr("Goto refused: target below the horizon limit");
    case '2':
        return (onStep) ? tr("Goto refused: target above the overhead limit") : tr("Goto refused: target below the higher limit");
    default:
        break;
    }

    if (onStep) {
        switch (code) {
        case '3':
            return tr("Goto refused: mount in standby");
        case '4':
            return tr("Goto refused: mount is parked (unpark it first)");
        case '5':
            return tr("Goto refused: a goto is already in progress");
        case '6':
            return tr("Goto refused: target outside the axis limits");
        case '7':
            return tr("Goto refused: hardware fault");
        case '8':
            return tr("Goto refused: mount already in motion");
        default:
            break;
        }
    }

    return tr("Goto refused by the mount (code %1)").arg(QChar(code));
}

QByteArray Lx200Driver::Query(const QByteArray &command,
                              const int timeoutMs)
{
    _transport.Send(command);
    return _transport.ReadUntil('#', timeoutMs);
}

bool Lx200Driver::QueryBool(const QByteArray &command)
{
    _transport.Send(command);
    return (_transport.ReadCount(1, REPLY_TIMEOUT_MS).at(0) == '1');
}
