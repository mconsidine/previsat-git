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
 * >    mountdriver.h
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

#ifndef MOUNTDRIVER_H
#define MOUNTDRIVER_H

#include <atomic>
#include <memory>
#include <QDateTime>
#include "mounttransport.h"


enum class MountProtocol {
    ONSTEPX = 0,
    LX200_CLASSIC,
    SYNSCAN,
    SIMULATOR
};


/**
 * @brief Base class of the mount protocols
 * All angles are in radians. Equatorial coordinates are topocentric, of date (JNow).
 * Longitudes are positive towards the west (PreviSat and Meade convention).
 * Methods perform blocking I/O and must be called from the tracking thread.
 */
class MountDriver
{
public:

    /**
     * @brief Create Build the driver of a protocol
     * @param[in] protocol mount protocol
     * @param[in] settings link settings
     * @return driver
     */
    static std::unique_ptr<MountDriver> Create(const MountProtocol protocol,
                                               const TransportSettings &settings);

    virtual ~MountDriver() = default;

    /**
     * @brief Connect Open the link and check that the mount answers
     * @return identification of the mount
     * @throw MountError
     */
    virtual QString Connect();

    virtual void Disconnect();

    virtual bool IsConnected() const;

    /**
     * @brief IsEquatorial Mount geometry, as reported by the mount during the handshake
     */
    bool IsEquatorial() const;

    virtual QString Name() const = 0;

    /**
     * @brief SetSiteAndTime Send the observing site and the current time to the mount
     * @param[in] latitude latitude (north positive)
     * @param[in] longitude longitude (west positive)
     * @param[in] utc current UTC date and time
     * @throw MountError
     */
    virtual void SetSiteAndTime(const double latitude,
                                const double longitude,
                                const QDateTime &utc) = 0;

    /**
     * @brief GotoRaDec Slew to equatorial coordinates; the mount tracks sidereally once there
     * @throw MountError if the mount rejects the target
     */
    virtual void GotoRaDec(const double rightAscension,
                           const double declination) = 0;

    virtual bool IsSlewing() = 0;

    virtual void GetRaDec(double &rightAscension,
                          double &declination) = 0;

    /**
     * @brief Stop Stop all motion immediately
     */
    virtual void Stop() = 0;

    /**
     * @brief Park Send the mount to its park position
     * @return false if the protocol has no park command
     */
    virtual bool Park() = 0;

    /**
     * @brief SupportsRaDecRates The mount can move at commanded right ascension / declination rates
     * (needed for continuous tracking)
     */
    virtual bool SupportsRaDecRates() const;

    /**
     * @brief SetRaDecRates Move at the given rates of change of the pointing coordinates
     * (rad/s; (0, 0) means following the stars, i.e. sidereal tracking). A goto or Stop() cancels the rates.
     * @throw MountError
     */
    virtual void SetRaDecRates(const double raRate,
                               const double decRate);

    /**
     * @brief RateControlStatus Why rate control is or is not available, for the user
     */
    virtual QString RateControlStatus() const;

    /**
     * @brief CanCalibrateRates The protocol has a rate calibration (see CalibrateRates)
     */
    virtual bool CanCalibrateRates() const;

    /**
     * @brief CalibrateRates Move the mount a little on each axis to measure how its rate commands behave.
     * Needed before SupportsRaDecRates() can be true on mounts whose behaviour is not known in advance.
     * @return report for the user
     * @throw MountError
     */
    virtual QString CalibrateRates();

    /**
     * @brief SetAbortFlag Flag set from another thread (STOP button) to interrupt long operations such as the calibration
     */
    void SetAbortFlag(const std::atomic<bool> *flag);


protected:

    explicit MountDriver(const TransportSettings &settings);

    /**
     * @brief Handshake Protocol-specific identification, called once the link is open
     * @return identification of the mount
     * @throw MountError
     */
    virtual QString Handshake() = 0;

    /**
     * @brief SleepMs Wait, unless the user asks to stop
     * @throw MountError if the abort flag is set
     */
    void SleepMs(const int milliseconds) const;

    MountTransport _transport;
    bool _equatorial;
    const std::atomic<bool> *_abortFlag;

};

#endif // MOUNTDRIVER_H
