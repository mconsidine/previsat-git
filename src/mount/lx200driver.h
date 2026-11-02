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

    /**
     * @brief GetSiderealTime :GS# (:GSH# on OnStepX)
     */
    bool GetSiderealTime(double &localSiderealTime) override;

    /**
     * @brief GetMeridianLimits OnStep :GXE9# (East) and :GXEA# (West), minutes past the meridian
     */
    bool GetMeridianLimits(double &limitEast,
                           double &limitWest) override;

    /**
     * @brief PierSide OnStep :Gm#
     */
    QString PierSide() override;


    /*
     * Rate control (OnStep / OnStepX equatorial mounts, after CalibrateRates)
     *
     * Each axis is driven with OnStep's custom guide rate (:RA / :RE, degrees per second) and the move commands
     * (:Mw :Me / :Mn :Ms). Directions, scale and whether sidereal tracking continues during a move are not
     * assumed: the calibration measures them. Moves are re-sent at least every MOVE_KEEPALIVE_MS so that
     * OnStep's own guide time limit stops the mount if PreviSat stops updating it.
     */
    bool SupportsRaDecRates() const override;

    void SetRaDecRates(const double raRate,
                       const double decRate) override;

    QString RateControlStatus() const override;

    bool CanCalibrateRates() const override;

    QString CalibrateRates() override;

    /**
     * @brief Measured response of one axis: coordinate rate = background + scale x move speed
     * (deg/s; positive speed = :Mw for axis 1, :Mn for axis 2)
     */
    struct AxisCalibration {
        double scale = 0.;
        double background = 0.;
    };

    /**
     * @brief SolveAxisCalibration Axis response from the coordinate rates measured with the forward and reverse moves
     * @param[in] rateForward coordinate rate during the forward move (deg/s)
     * @param[in] rateReverse coordinate rate during the reverse move (deg/s)
     * @param[in] speed commanded speed (deg/s)
     * @param[out] axis response
     * @param[out] error explanation if the response is implausible
     * @return true if the response is plausible (scale within 20 % of +/- 1)
     */
    static bool SolveAxisCalibration(const double rateForward,
                                     const double rateReverse,
                                     const double speed,
                                     AxisCalibration &axis,
                                     QString &error);

    /**
     * @brief MoveSpeed Signed move speed (deg/s) giving the wanted coordinate rate (deg/s)
     */
    static double MoveSpeed(const double coordinateRate,
                            const AxisCalibration &axis);

    /**
     * @brief FormatGuideRate Argument of :RA / :RE (degrees per second)
     */
    static QByteArray FormatGuideRate(const double degreesPerSecond);


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
    static constexpr int MOVE_KEEPALIVE_MS = 1000;
    static constexpr double MIN_MOVE_SPEED = 1.e-6;         // deg/s, below this the axis is stopped
    static constexpr double CALIBRATION_SPEED = 0.5;        // deg/s

    bool _onStep;
    bool _highPrecision;
    bool _extendedPositions;    // OnStepX :GRH# / :GDH#

    // Rate control state
    bool _rateCalibrated;
    AxisCalibration _axis1;
    AxisCalibration _axis2;
    bool _rateChangeDuringMove; // a new :RA / :RE value applies to a move in progress
    QByteArray _pierSide;       // pier side at calibration (E, W or N)
    bool _checkPierSide;        // a goto happened since the last rate command: the pier side may have changed
    bool _guiding;
    int _direction1;            // current move direction of each axis (-1, 0, 1)
    int _direction2;
    qint64 _lastMoveMs[2];

    /**
     * @brief SendAxisMove Send the speed and, when needed, the move command of one axis
     */
    void SendAxisMove(const int axis,
                      const double speed);

    /**
     * @brief StopGuiding Stop the moves; sidereal tracking resumes
     */
    void StopGuiding();

    QByteArray QueryPierSide();

    /**
     * @brief ReadPositionDeg Mount position in degrees, and the time of the measurement (ms, middle of the request)
     */
    void ReadPositionDeg(double &ra,
                         double &dec,
                         qint64 &timeMs);

    /**
     * @brief MeasureMove Coordinate rate (deg/s) of one axis during a move
     */
    double MeasureMove(const int axis,
                       const bool forward,
                       const double speed);

    void WaitForStop(const int timeoutMs);

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
