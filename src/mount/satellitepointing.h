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
 * >    satellitepointing.h
 *
 * Location
 * >    mount
 *
 * Description
 * >    Where to point the telescope to see a satellite at a given time
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef SATELLITEPOINTING_H
#define SATELLITEPOINTING_H

#include <QDateTime>
#include "librairies/corps/satellite/satellite.h"
#include "librairies/maths/mathsconst.h"
#include "librairies/observateur/observateur.h"


struct PointingSample {
    double jdUtc = 0.;              // Julian day 2000, UTC
    double azimuth = 0.;            // rad, from north through east
    double altitude = 0.;           // rad, apparent (with refraction)
    double rightAscension = 0.;     // rad, topocentric, of date
    double declination = 0.;        // rad, topocentric, of date
    double hourAngle = 0.;          // rad, in ]-pi, pi]
    double range = 0.;              // km
};

struct PassSummary {
    double jdMaxAltitude = 0.;
    double maxAltitude = -MATHS::PI_SUR_DEUX;
    double maxRate = 0.;            // rad/s, apparent angular rate on the sky
    bool crossesMeridian = false;   // hour angle changes sign while the satellite is above the minimum altitude
    double jdMeridian = 0.;         // first meridian crossing (Julian day 2000, UTC)
    double rateAtMeridian = 0.;     // rad/s, angular rate at that crossing
    double startHourAngle = 0.;     // rad, hour angles over the part of the pass above the minimum altitude
    double minHourAngle = 0.;
    double maxHourAngle = 0.;
    bool valid = false;             // at least one sample above the minimum altitude
};


class SatellitePointing
{
public:

    SatellitePointing(const Satellite &satellite,
                      const Observateur &observer);

    /**
     * @brief Compute Pointing towards the satellite
     * @param[in] jdUtc Julian day 2000, UTC
     * @return pointing
     */
    PointingSample Compute(const double jdUtc);

    /**
     * @brief Summarize Pass characteristics over a time window
     * @param[in] jdStart start (Julian day 2000, UTC)
     * @param[in] jdEnd end (Julian day 2000, UTC)
     * @param[in] minAltitude only samples above this apparent altitude are considered (rad)
     * @param[in] stepSeconds sampling step (s)
     * @return summary
     */
    PassSummary Summarize(const double jdStart,
                          const double jdEnd,
                          const double minAltitude,
                          const double stepSeconds = 1.);

    /**
     * @brief NowJdUtc Current system time, as a Julian day 2000 (UTC), with millisecond resolution
     */
    static double NowJdUtc();

    /**
     * @brief ToDateTimeUtc Julian day 2000 (UTC) to QDateTime
     */
    static QDateTime ToDateTimeUtc(const double jdUtc);

    /**
     * @brief LocalSiderealTime Local sidereal time computed by PreviSat (rad, in [0, 2 pi[)
     * @param[in] jdUtc Julian day 2000, UTC
     * @param[in] longitude longitude (rad, positive towards the west)
     */
    static double LocalSiderealTime(const double jdUtc,
                                    const double longitude);

    static double AngularSeparation(const double ra1,
                                    const double dec1,
                                    const double ra2,
                                    const double dec2);


private:

    Satellite _satellite;
    Observateur _observer;

};

#endif // SATELLITEPOINTING_H
