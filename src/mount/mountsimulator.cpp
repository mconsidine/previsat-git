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
 * >    mountsimulator.cpp
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    MountDriver
 *
 * Description
 * >    Simulated equatorial mount: no hardware, for dry runs of a pass and for unit tests
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <algorithm>
#include <cmath>
#include "librairies/dates/dateconst.h"
#include "librairies/maths/mathsconst.h"
#include "mountclock.h"
#include "mountsimulator.h"


// Integration step of the axis motion (s)
static constexpr double MAX_STEP_S = 0.02;


MountSimulator::MountSimulator(const MountClock &clock,
                               const SimulatorSettings &settings) :
    MountDriver(TransportSettings()),
    _clock(clock),
    _settings(settings),
    _connected(false),
    _moving(false),
    _lastUpdateJd(0.),
    _arrivalJd(-1.)
{
    _axis1.wraps = true;
    _equatorial = true;
}

QString MountSimulator::Connect()
{
    _connected = true;
    _lastUpdateJd = _clock.NowJdUtc();
    return Handshake();
}

void MountSimulator::Disconnect()
{
    _connected = false;
}

bool MountSimulator::IsConnected() const
{
    return _connected;
}

QString MountSimulator::Name() const
{
    return "Simulator";
}

QString MountSimulator::Handshake()
{
    return QString("Simulated equatorial mount (%1°/s, %2°/s²)").arg(_settings.maxRate).arg(_settings.acceleration);
}

void MountSimulator::SetSiteAndTime(const double latitude,
                                    const double longitude,
                                    const QDateTime &utc)
{
    Q_UNUSED(latitude)
    Q_UNUSED(longitude)
    Q_UNUSED(utc)
}

void MountSimulator::GotoRaDec(const double rightAscension,
                               const double declination)
{
    if (!_connected) {
        throw MountError("Mount is not connected");
    }

    Update();
    if (_moving) {
        throw MountError("Goto refused: a goto is already in progress");
    }

    _axis1.target = rightAscension;
    _axis2.target = declination;
    _moving = true;
    _arrivalJd = -1.;
}

bool MountSimulator::IsSlewing()
{
    Update();
    return _moving;
}

void MountSimulator::GetRaDec(double &rightAscension,
                              double &declination)
{
    Update();

    rightAscension = fmod(_axis1.position, MATHS::DEUX_PI);
    if (rightAscension < 0.) {
        rightAscension += MATHS::DEUX_PI;
    }
    declination = _axis2.position;
}

void MountSimulator::Stop()
{
    Update();

    _axis1.target = _axis1.position;
    _axis1.velocity = 0.;
    _axis2.target = _axis2.position;
    _axis2.velocity = 0.;
    _moving = false;
}

bool MountSimulator::Park()
{
    Stop();
    return false;
}

void MountSimulator::Update()
{
    const double now = _clock.NowJdUtc();
    double remaining = (now - _lastUpdateJd) * DATE::NB_SEC_PAR_JOUR;
    _lastUpdateJd = now;

    if (!_moving) {
        return;
    }

    while (remaining > 0.) {

        const double dt = std::min(remaining, MAX_STEP_S);
        remaining -= dt;

        const bool done1 = StepAxis(_axis1, dt);
        const bool done2 = StepAxis(_axis2, dt);

        if (done1 && done2 && (_arrivalJd < 0.)) {
            _arrivalJd = now - remaining * DATE::NB_JOUR_PAR_SEC;
        }
    }

    if ((_arrivalJd >= 0.) && ((now - _arrivalJd) * DATE::NB_SEC_PAR_JOUR >= _settings.settleSeconds)) {
        _moving = false;
    }
}

bool MountSimulator::StepAxis(Axis &axis,
                              const double dt) const
{
    const double maxRate = _settings.maxRate * MATHS::DEG2RAD;
    const double acceleration = _settings.acceleration * MATHS::DEG2RAD;

    double distance = axis.target - axis.position;
    if (axis.wraps) {
        distance = remainder(distance, MATHS::DEUX_PI);
    }

    // Close enough and slow enough: snap to the target
    if ((fabs(distance) < 1.e-7) && (fabs(axis.velocity) < acceleration * dt)) {
        axis.position += distance;
        axis.velocity = 0.;
        return true;
    }

    // Trapezoidal profile: fastest speed from which the axis can still stop at the target
    const double stopSpeed = sqrt(2. * acceleration * fabs(distance));
    const double wanted = std::copysign(std::min(maxRate, stopSpeed), distance);
    const double dv = std::clamp(wanted - axis.velocity, -acceleration * dt, acceleration * dt);

    axis.velocity += dv;

    // Do not overshoot during the last step
    double move = axis.velocity * dt;
    if ((fabs(move) > fabs(distance)) && (move * distance > 0.)) {
        move = distance;
        axis.velocity = 0.;
    }
    axis.position += move;

    return false;
}
