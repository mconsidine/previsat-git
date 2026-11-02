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
 * >    synscandriver.cpp
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    MountDriver
 *
 * Description
 * >    Sky-Watcher SynScan hand controller serial protocol (9600 bd, 8N1)
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <cmath>
#include <QCoreApplication>
#include "librairies/maths/mathsconst.h"
#include "synscandriver.h"


static QString tr(const char *text)
{
    return QCoreApplication::translate("SynScanDriver", text);
}

static QByteArray Bytes(std::initializer_list<int> values)
{
    QByteArray bytes;
    for (const int v : values) {
        bytes.append(static_cast<char>(v & 0xFF));
    }
    return bytes;
}


SynScanDriver::SynScanDriver(const TransportSettings &settings) :
    MountDriver(settings)
{
}

QString SynScanDriver::Name() const
{
    return "SynScan";
}

QString SynScanDriver::Handshake()
{
    // Echo test
    if (Query("Kx") != "x") {
        throw MountError(tr("The hand controller did not answer the echo test: is it a SynScan controller?"));
    }

    QString version;
    try {
        const QByteArray v = Query("V");
        if (v.size() == 6) {
            // Recent firmware: 3 bytes written as hex, e.g. 040213 = 4.02.19
            version = QString("%1.%2.%3").arg(v.mid(0, 2).toInt(nullptr, 16)).arg(v.mid(2, 2).toInt(nullptr, 16), 2, 10, QChar('0'))
                          .arg(v.mid(4, 2).toInt(nullptr, 16), 2, 10, QChar('0'));
        } else if (v.size() == 2) {
            version = QString("%1.%2").arg(static_cast<uchar>(v.at(0))).arg(static_cast<uchar>(v.at(1)), 2, 10, QChar('0'));
        }
    } catch (const MountError &) {
    }

    // Tracking mode: 0 off, 1 alt-azimuth, 2 EQ north, 3 EQ south
    try {
        const QByteArray mode = Query("t");
        if (!mode.isEmpty()) {
            _equatorial = ((mode.at(0) == 2) || (mode.at(0) == 3));
        }
    } catch (const MountError &) {
    }

    bool aligned = true;
    try {
        const QByteArray reply = Query("J");
        aligned = (!reply.isEmpty() && (reply.at(0) != 0));
    } catch (const MountError &) {
    }

    QString identity = QString("SynScan %1 (%2)").arg(version, (_equatorial) ? tr("equatorial") : tr("alt-azimuth"));
    if (!aligned) {
        identity += " - " + tr("WARNING: alignment not complete, gotos will be refused or inaccurate");
    }

    return identity;
}

void SynScanDriver::SetSiteAndTime(const double latitude,
                                   const double longitude,
                                   const QDateTime &utc)
{
    // Location: W + lat deg, min, sec, N=0/S=1, lon deg, min, sec, E=0/W=1
    const auto dms = [](const double angle, int &d, int &m, int &s) {
        const int total = static_cast<int>(std::lround(fabs(angle * MATHS::RAD2DEG) * 3600.));
        d = total / 3600;
        m = (total / 60) % 60;
        s = total % 60;
    };

    double lon = fmod(longitude, MATHS::DEUX_PI);
    if (lon > MATHS::PI) {
        lon -= MATHS::DEUX_PI;
    } else if (lon < -MATHS::PI) {
        lon += MATHS::DEUX_PI;
    }

    int latD, latM, latS, lonD, lonM, lonS;
    dms(latitude, latD, latM, latS);
    dms(lon, lonD, lonM, lonS);

    Query("W" + Bytes({ latD, latM, latS, (latitude < 0.) ? 1 : 0, lonD, lonM, lonS, (lon > 0.) ? 1 : 0 }));

    // Time: H + local hour, min, sec, month, day, year - 2000, offset from UTC (signed, whole hours), DST flag.
    // The controller only knows whole-hour offsets: send the "local" time matching the rounded offset
    // so that the UTC it derives is exact.
    const int offsetHours = static_cast<int>(std::lround(utc.toLocalTime().offsetFromUtc() / 3600.));
    const QDateTime local = utc.toOffsetFromUtc(offsetHours * 3600);

    Query("H" + Bytes({ local.time().hour(), local.time().minute(), local.time().second(), local.date().month(),
                        local.date().day(), local.date().year() - 2000, (offsetHours < 0) ? 256 + offsetHours : offsetHours, 0 }));
}

void SynScanDriver::GotoRaDec(const double rightAscension,
                              const double declination)
{
    Query("r" + EncodeAngle(rightAscension) + "," + EncodeAngle(declination));
}

bool SynScanDriver::IsSlewing()
{
    const QByteArray reply = Query("L");
    return (!reply.isEmpty() && (reply.at(0) == '1'));
}

void SynScanDriver::GetRaDec(double &rightAscension,
                             double &declination)
{
    const QByteArray reply = Query("e");
    const QList<QByteArray> fields = reply.split(',');

    if (fields.size() != 2) {
        throw MountError(tr("Unexpected position from hand controller: %1").arg(QString::fromLatin1(reply)));
    }

    rightAscension = DecodeAngle(fields.at(0));
    declination = DecodeAngle(fields.at(1));
    if (declination > MATHS::PI) {
        declination -= MATHS::DEUX_PI;
    }
}

void SynScanDriver::Stop()
{
    // Cancel goto, then zero the slew rate of both axes (P, 2, axis 16/17, direction 36, rate 0)
    Query("M");
    Query(Bytes({ 'P', 2, 16, 36, 0, 0, 0, 0 }));
    Query(Bytes({ 'P', 2, 17, 36, 0, 0, 0, 0 }));
}

bool SynScanDriver::Park()
{
    Stop();
    return false;
}

QByteArray SynScanDriver::EncodeAngle(const double angle)
{
    double fraction = fmod(angle / MATHS::DEUX_PI, 1.);
    if (fraction < 0.) {
        fraction += 1.;
    }

    // 24 significant bits, low byte left at zero as the hand controller does
    const quint32 value = (static_cast<quint32>(std::llround(fraction * 16777216.)) & 0xFFFFFFu) << 8;
    return QString("%1").arg(value, 8, 16, QChar('0')).toUpper().toLatin1();
}

double SynScanDriver::DecodeAngle(const QByteArray &hex)
{
    bool ok = false;
    const quint32 value = hex.trimmed().toUInt(&ok, 16);
    const int digits = static_cast<int>(hex.trimmed().size());

    if (!ok || ((digits != 8) && (digits != 4))) {
        throw MountError(tr("Unexpected angle from hand controller: %1").arg(QString::fromLatin1(hex)));
    }

    const double scale = (digits == 8) ? 4294967296. : 65536.;
    return value / scale * MATHS::DEUX_PI;
}

QByteArray SynScanDriver::Query(const QByteArray &command)
{
    _transport.Send(command);
    return _transport.ReadUntil('#', REPLY_TIMEOUT_MS);
}
