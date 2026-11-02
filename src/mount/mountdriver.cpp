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
 * >    mountdriver.cpp
 *
 * Location
 * >    mount
 *
 * Description
 * >    Common interface of the telescope mount command protocols
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include "lx200driver.h"
#include "mountclock.h"
#include "mountdriver.h"
#include "mountsimulator.h"
#include "synscandriver.h"


std::unique_ptr<MountDriver> MountDriver::Create(const MountProtocol protocol,
                                                 const TransportSettings &settings)
{
    switch (protocol) {
    case MountProtocol::ONSTEPX:
        return std::make_unique<Lx200Driver>(settings, true);

    case MountProtocol::LX200_CLASSIC:
        return std::make_unique<Lx200Driver>(settings, false);

    case MountProtocol::SYNSCAN:
        return std::make_unique<SynScanDriver>(settings);

    case MountProtocol::SIMULATOR:
        return std::make_unique<MountSimulator>(MountClock::System());
    }

    return nullptr;
}

MountDriver::MountDriver(const TransportSettings &settings) :
    _transport(settings),
    _equatorial(false)
{
}

QString MountDriver::Connect()
{
    _transport.Open();

    try {
        return Handshake();
    } catch (const MountError &) {
        _transport.Close();
        throw;
    }
}

void MountDriver::Disconnect()
{
    _transport.Close();
}

bool MountDriver::IsConnected() const
{
    return _transport.IsOpen();
}

bool MountDriver::IsEquatorial() const
{
    return _equatorial;
}
