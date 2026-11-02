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
 * >    mountclock.cpp
 *
 * Location
 * >    mount
 *
 * Description
 * >    Time source of the tracker and of the mount simulator
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include "librairies/dates/dateconst.h"
#include "mountclock.h"
#include "satellitepointing.h"


double MountClock::NowJdUtc() const
{
    return SatellitePointing::NowJdUtc();
}

const MountClock &MountClock::System()
{
    static const MountClock clock {};
    return clock;
}

SimulatedClock::SimulatedClock(const double jdUtc) :
    _jdUtc(jdUtc)
{
}

double SimulatedClock::NowJdUtc() const
{
    return _jdUtc;
}

void SimulatedClock::Set(const double jdUtc)
{
    _jdUtc = jdUtc;
}

void SimulatedClock::Advance(const double seconds)
{
    _jdUtc += seconds * DATE::NB_JOUR_PAR_SEC;
}
