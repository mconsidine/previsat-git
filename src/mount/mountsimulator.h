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
 * >    mountsimulator.h
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    MountDriver
 *
 * Description
 * >    Simulated equatorial mount: no hardware, for dry runs of a pass and for unit tests.
 *      Each axis (right ascension, declination) accelerates and decelerates with a limited rate;
 *      once a goto is finished the mount tracks the target sidereally (its RA/Dec no longer change).
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef MOUNTSIMULATOR_H
#define MOUNTSIMULATOR_H

#include "mountdriver.h"


class MountClock;

struct SimulatorSettings {
    double maxRate = 4.;            // deg/s, per axis
    double acceleration = 3.;       // deg/s^2, per axis
    double settleSeconds = 0.5;     // the mount still reports "slewing" this long after reaching the target
    double readbackDelaySeconds = 0.;   // the reported position is this old (slow controller feedback)
};


class MountSimulator : public MountDriver
{
public:

    explicit MountSimulator(const MountClock &clock,
                            const SimulatorSettings &settings = SimulatorSettings());

    QString Connect() override;
    void Disconnect() override;
    bool IsConnected() const override;

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

    bool SupportsRaDecRates() const override;

    void SetRaDecRates(const double raRate,
                       const double decRate) override;


protected:

    QString Handshake() override;


private:

    struct Axis {
        double position = 0.;   // rad
        double target = 0.;     // rad
        double velocity = 0.;   // rad/s
        double commandedRate = 0.;  // rad/s, in rate mode
        bool wraps = false;     // right ascension: shortest way round
    };

    const MountClock &_clock;
    SimulatorSettings _settings;
    bool _connected;
    bool _moving;
    bool _rateMode;
    double _lastUpdateJd;
    double _arrivalJd;
    Axis _axis1;
    Axis _axis2;

    /**
     * @brief Update Move the axes up to the current time
     */
    void Update();

    /**
     * @brief StepAxis Move an axis during dt seconds
     * @return true if the axis is at its target and at rest
     */
    bool StepAxis(Axis &axis,
                  const double dt) const;

    /**
     * @brief StepRate Move an axis during dt seconds in rate mode (acceleration limited)
     */
    void StepRate(Axis &axis,
                  const double dt) const;

    void CheckConnected() const;

};

#endif // MOUNTSIMULATOR_H
