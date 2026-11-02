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
#include <QRegularExpression>
#include "librairies/maths/mathsconst.h"
#include "lx200driver.h"


static QString tr(const char *text)
{
    return QCoreApplication::translate("Lx200Driver", text);
}


Lx200Driver::Lx200Driver(const TransportSettings &settings,
                         const bool onStep) :
    MountDriver(settings),
    _onStep(onStep),
    _highPrecision(true)
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
    rightAscension = ParseRightAscension(Query(":GR#"));
    declination = ParseDeclination(Query(":GD#"));
}

void Lx200Driver::Stop()
{
    _transport.Send(":Q#");
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
