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
 * >    satellitepointing.cpp
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

#include <cmath>
#include <QTimeZone>
#include "librairies/dates/date.h"
#include "satellitepointing.h"


// Unix epoch (1970-01-01 00:00 UTC) as a Julian day 2000
static constexpr double JD2000_UNIX_EPOCH = 2440587.5 - 2451545.;


SatellitePointing::SatellitePointing(const Satellite &satellite,
                                     const Observateur &observer) :
    _satellite(satellite),
    _observer(observer)
{
}

PointingSample SatellitePointing::Compute(const double jdUtc)
{
    const Date date(jdUtc, 0.);

    _observer.CalculPosVit(date);
    _satellite.CalculPosVit(date);

    // Geometric horizontal coordinates, from which the equatorial coordinates are derived
    // (as for the SkyWatcher position file: the mount applies its own refraction model, if any)
    _satellite.CalculCoordHoriz(_observer, true, false);
    _satellite.CalculCoordEquat(_observer, false);

    PointingSample sample;
    sample.jdUtc = jdUtc;
    sample.azimuth = _satellite.azimut();
    sample.altitude = _satellite.hauteur() + Corps::CalculRefractionAtmospherique(_satellite.hauteur());
    sample.rightAscension = _satellite.ascensionDroite();
    sample.declination = _satellite.declinaison();
    sample.range = _satellite.distance();

    double hourAngle = fmod(_observer.tempsSideralGreenwich() - _observer.longitude() - sample.rightAscension, MATHS::DEUX_PI);
    if (hourAngle > MATHS::PI) {
        hourAngle -= MATHS::DEUX_PI;
    } else if (hourAngle <= -MATHS::PI) {
        hourAngle += MATHS::DEUX_PI;
    }
    sample.hourAngle = hourAngle;

    return sample;
}

PassSummary SatellitePointing::Summarize(const double jdStart,
                                         const double jdEnd,
                                         const double minAltitude,
                                         const double stepSeconds)
{
    PassSummary summary;
    const double step = stepSeconds * DATE::NB_JOUR_PAR_SEC;

    bool hasPrevious = false;
    PointingSample previous;

    for (double jd = jdStart; jd <= jdEnd; jd += step) {

        const PointingSample sample = Compute(jd);

        if (sample.altitude < minAltitude) {
            hasPrevious = false;
            continue;
        }

        if (sample.altitude > summary.maxAltitude) {
            summary.maxAltitude = sample.altitude;
            summary.jdMaxAltitude = jd;
        }

        if (hasPrevious) {
            const double rate = AngularSeparation(previous.rightAscension, previous.declination,
                                                  sample.rightAscension, sample.declination) / stepSeconds;
            summary.maxRate = std::max(summary.maxRate, rate);

            // Sign change of the hour angle near 0 (not across +/- pi, which is the lower meridian)
            if ((previous.hourAngle * sample.hourAngle < 0.) && (fabs(previous.hourAngle - sample.hourAngle) < MATHS::PI)) {
                summary.crossesMeridian = true;
            }
        }

        previous = sample;
        hasPrevious = true;
    }

    return summary;
}

double SatellitePointing::NowJdUtc()
{
    return QDateTime::currentMSecsSinceEpoch() * DATE::NB_JOUR_PAR_MILLISEC + JD2000_UNIX_EPOCH;
}

QDateTime SatellitePointing::ToDateTimeUtc(const double jdUtc)
{
    return QDateTime::fromMSecsSinceEpoch(std::llround((jdUtc - JD2000_UNIX_EPOCH) * DATE::NB_MILLISEC_PAR_JOUR), QTimeZone::UTC);
}

double SatellitePointing::AngularSeparation(const double ra1,
                                            const double dec1,
                                            const double ra2,
                                            const double dec2)
{
    // Haversine formula, well conditioned for small separations
    const double sdd = sin(0.5 * (dec2 - dec1));
    const double sda = sin(0.5 * (ra2 - ra1));
    const double h = sdd * sdd + cos(dec1) * cos(dec2) * sda * sda;
    return 2. * asin(std::min(1., sqrt(h)));
}
