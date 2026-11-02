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
 * >    lx200driver.h
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

#ifndef LX200DRIVER_H
#define LX200DRIVER_H

#include "mountdriver.h"


class Lx200Driver : public MountDriver
{
public:

    /**
     * @brief Lx200Driver
     * @param[in] settings link settings
     * @param[in] onStep true for OnStep / OnStepX, false for a classic Meade LX200
     */
    Lx200Driver(const TransportSettings &settings,
                const bool onStep);

    QString Name() const override;

    void SetSiteAndTime(const double latitude,
                        const double longitude,
                        const QDateTime &utc) override;

    void GotoRaDec(const double rightAscension,
                   const double declination) override;

    bool IsSlewing() override;

    void GetRaDec(double &rightAscension,
                  double &declination) override;

    void Stop() override;

    bool Park() override;


    /*
     * Coordinate formatting and parsing (public for unit tests)
     */
    /**
     * @brief FormatRightAscension HH:MM:SS (high precision) or HH:MM.T (low precision)
     */
    static QByteArray FormatRightAscension(const double rightAscension,
                                           const bool highPrecision);

    /**
     * @brief FormatDeclination sDD*MM:SS (high precision) or sDD*MM (low precision)
     */
    static QByteArray FormatDeclination(const double declination,
                                        const bool highPrecision);

    /**
     * @brief FormatLatitude sDD*MM
     */
    static QByteArray FormatLatitude(const double latitude);

    /**
     * @brief FormatLongitude DDD*MM, 0 to 360 degrees, positive towards the west
     */
    static QByteArray FormatLongitude(const double longitude);

    /**
     * @brief FormatUtcOffset Hours to add to local time to obtain UTC (sHH, sHH.H or sHH:MM)
     * @param[in] offsetFromUtcSeconds local time - UTC (s)
     * @param[in] onStep OnStep dialect (sHH:MM instead of sHH.H)
     */
    static QByteArray FormatUtcOffset(const int offsetFromUtcSeconds,
                                      const bool onStep);

    /**
     * @brief ParseRightAscension Parse HH:MM:SS[.SSSS] or HH:MM.T
     * @throw MountError
     */
    static double ParseRightAscension(const QByteArray &text);

    /**
     * @brief ParseDeclination Parse sDD*MM[:SS], with *, ', :, or the degree sign as separators
     * @throw MountError
     */
    static double ParseDeclination(const QByteArray &text);

    static QString GotoErrorText(const char code,
                                 const bool onStep);


protected:

    QString Handshake() override;


private:

    static constexpr int REPLY_TIMEOUT_MS = 1500;

    bool _onStep;
    bool _highPrecision;

    /**
     * @brief Query Send a command and read a reply terminated by #
     */
    QByteArray Query(const QByteArray &command,
                     const int timeoutMs = REPLY_TIMEOUT_MS);

    /**
     * @brief QueryBool Send a command answered by a single 0 / 1 character
     */
    bool QueryBool(const QByteArray &command);

};

#endif // LX200DRIVER_H
