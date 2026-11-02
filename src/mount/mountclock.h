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
 * >    mountclock.h
 *
 * Location
 * >    mount
 *
 * Description
 * >    Time source of the tracker and of the mount simulator: the system clock, or a simulated clock
 *      that tests advance faster than real time
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef MOUNTCLOCK_H
#define MOUNTCLOCK_H


class MountClock
{
public:

    virtual ~MountClock() = default;

    /**
     * @brief NowJdUtc Current time, as a Julian day 2000 (UTC)
     */
    virtual double NowJdUtc() const;

    /**
     * @brief System Clock of the computer
     */
    static const MountClock &System();

};


class SimulatedClock : public MountClock
{
public:

    explicit SimulatedClock(const double jdUtc = 0.);

    double NowJdUtc() const override;

    void Set(const double jdUtc);

    void Advance(const double seconds);


private:

    double _jdUtc;

};

#endif // MOUNTCLOCK_H
