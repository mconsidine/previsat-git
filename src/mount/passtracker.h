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
 * >    passtracker.h
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    QObject
 *
 * Description
 * >    Drives a mount along a satellite pass. Lives in its own thread (all mount I/O is blocking).
 *
 *      Method ("leapfrog"): goto commands only, which every protocol supports.
 *      The mount is sent to where the satellite will be a few seconds ahead ("lead"), waits there
 *      tracking sidereally while the satellite crosses the field, then jumps ahead again.
 *      The lead adapts to the measured slew duration.
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef PASSTRACKER_H
#define PASSTRACKER_H

#include <memory>
#include <QObject>
#include "librairies/corps/satellite/elementsorbitaux.h"
#include "librairies/observateur/observateur.h"
#include "mountclock.h"
#include "mountdriver.h"
#include "satellitepointing.h"


class QFile;
class QTimer;

struct PassDefinition {
    QString satelliteName;
    QList<ElementsOrbitaux> elements;   // one set of elements, or the ISS ephemeris list
    Observateur observer;
    double jdStart = 0.;                // Julian day 2000, UTC
    double jdEnd = 0.;
};

enum class TrackingMode {
    LEAPFROG = 0,       // goto ahead of the satellite, wait for it, repeat (every mount)
    CONTINUOUS          // follow the satellite with rate commands and position feedback (mounts with rate control)
};

/**
 * @brief Parameters of the continuous tracking controller.
 * Each update, commanded rate = predicted rate of the target + gain x pointing error, where the gain
 * grows from minGain (small errors) to maxGain (errors of gainErrorScale or more).
 */
struct ContinuousSettings {
    double minGain = 0.3;               // 1/s, gain for small errors ("minimum aggression")
    double maxGain = 1.5;               // 1/s, gain for large errors ("maximum aggression")
    double gainErrorScale = 10.;        // arcmin, error at which the gain reaches maxGain
    double deadband = 0.05;             // arcmin, no correction below this error
    double maxCorrectionRate = 1.;      // deg/s on the sky, limit of the correction term
    double maxRate = 5.;                // deg/s per coordinate, limit of the commanded rate
    double maxAcceleration = 5.;        // deg/s^2 per coordinate, limit of the rate change
    double catchUpThreshold = 3.;       // deg, beyond this error the mount catches up with a goto
    double positionDelay = 0.;          // s, age of the position the mount reports (measured by calibration)
};

struct TrackerSettings {
    TrackingMode mode = TrackingMode::LEAPFROG;
    ContinuousSettings continuous;
    double minAltitude = 10. * MATHS::DEG2RAD;      // rad, apparent
    double maxAltitude = 88. * MATHS::DEG2RAD;      // rad, apparent (zenith keyhole of alt-azimuth mounts)
    int prePositionSeconds = 120;                   // slew to the start point this long before the pass
    double dwellSeconds = 1.;                       // stay this long after the satellite reaches the aim point
    bool parkAtEnd = false;
    QString logDirectory;                           // folder of the CSV log of each pass (empty: no log)
};


class PassTracker : public QObject
{
    Q_OBJECT

public:

    enum class State {
        DISCONNECTED = 0,
        IDLE,
        WAITING,            // waiting for the pre-positioning time
        PREPOSITIONING,     // at (or slewing to) the start point, waiting for the satellite
        TRACKING,
        FINISHED
    };
    Q_ENUM(State)

    explicit PassTracker(QObject *parent = nullptr);
    ~PassTracker() override;

    /**
     * @brief SetClock Time source (system clock by default; a simulated clock for tests)
     */
    void SetClock(const MountClock &clock);

    /*
     * The following methods must be called in the tracker thread (QMetaObject::invokeMethod)
     */
    void ConnectMount(const MountProtocol protocol,
                      const TransportSettings &transport,
                      const bool sendSiteAndTime,
                      const double latitude,
                      const double longitude);

    void DisconnectMount();

    /**
     * @brief Arm Check the pass, then wait for it and track it
     */
    void Arm(const PassDefinition &pass,
             const TrackerSettings &settings);

    /**
     * @brief Abort Stop tracking and stop the mount
     */
    void Abort();

    /**
     * @brief CalibrateMount Run the rate calibration of the mount (needed for continuous tracking on OnStep)
     */
    void CalibrateMount();

    /**
     * @brief ClockAdvice Message about the difference between the mount's and the computer's sidereal times (rad)
     */
    static QString ClockAdvice(const double lstDifference);

    /**
     * @brief MeridianAdvice Message about meridian flips during the pass, for equatorial mounts
     * @param[in] summary pass summary
     * @param[in] hasLimits the mount reported its meridian limits
     * @param[in] limitEast meridian limit East of the mount (rad of hour angle past the meridian)
     * @param[in] limitWest meridian limit West of the mount
     */
    static QString MeridianAdvice(const PassSummary &summary,
                                  const bool hasLimits,
                                  const double limitEast,
                                  const double limitWest);

    /**
     * @brief RequestAbort Thread-safe: interrupt a long blocking operation (calibration) at once.
     * Call it from the interface thread before queuing Abort().
     */
    void RequestAbort();

    /**
     * @brief SetOffsets Corrections applied to the predicted position (absolute values, not increments)
     * @param[in] timingSeconds along the track: the mount points where the satellite will be this many seconds later
     * @param[in] crossTrackArcmin across the track: positive to the right of the direction of motion, as seen by the observer
     */
    void SetOffsets(const double timingSeconds,
                    const double crossTrackArcmin);


signals:

    void connectionChanged(const bool connected,
                           const QString &identity);

    void stateChanged(const int state,
                      const QString &description);

    void logMessage(const QString &message);

    /**
     * @brief positionUpdated Current satellite position (degrees), and angular separation between the mount
     * and the satellite (arcmin, < 0 if unknown)
     */
    void positionUpdated(const double azimuth,
                         const double altitude,
                         const double separationArcmin);


private slots:

    void Tick();


private:

    friend class MountTest;

    static constexpr int TICK_MS = 200;
    static constexpr double INITIAL_LEAD_S = 4.;
    static constexpr double MIN_LEAD_S = 1.5;
    static constexpr double MAX_LEAD_S = 30.;

    const MountClock *_clock;
    std::unique_ptr<MountDriver> _driver;
    std::unique_ptr<SatellitePointing> _pointing;
    std::unique_ptr<QFile> _logFile;
    QTimer *_timer;

    State _state;
    PassDefinition _pass;
    TrackerSettings _settings;

    double _aimJd;              // time at which the satellite reaches the current aim point
    PointingSample _aimTarget;
    double _gotoIssuedJd;
    bool _slewing;
    double _leadSeconds;
    double _lastErrorArcmin;    // pointing error at the last arrival (arcmin)
    double _separationArcmin;   // mount - satellite separation at the last readback (arcmin)
    double _mountRa;
    double _mountDec;
    bool _mountPositionKnown;
    QList<double> _trackingSeparations;
    int _gotoCount;
    int _lateArrivals;
    qint64 _lastStatusSecond;

    // Continuous tracking
    bool _continuous;           // continuous mode selected and supported by the mount
    bool _rateActive;           // rates are being commanded (false while catching up with a goto)
    double _rateRa;             // last commanded rates (rad/s)
    double _rateDec;
    double _lastRateJd;
    int _readFailures;
    int _catchUps;

    // User corrections
    double _timingOffset;       // s
    double _crossOffset;        // rad

    std::atomic<bool> _abortRequested;

    QString _lastPierSide;      // to log meridian flips during the pass

    /**
     * @brief CheckMountClock Compare the mount's sidereal time with the computer's, and log the result
     * @param[in] longitude observer longitude (rad, positive towards the west)
     */
    void CheckMountClock(const double longitude);

    void SetState(const State state,
                  const QString &description);

    /**
     * @brief Report Message to the user, also written in the pass log
     */
    void Report(const QString &message);

    void ReadMountPosition(const PointingSample &satellite);

    void OpenLog();
    void WriteLogRow(const double jd,
                     const PointingSample *satellite,
                     const QString &event);
    void CloseLog();

    /**
     * @brief TrackingSummary Statistics of the mount - satellite separation during the tracking phase
     */
    QString TrackingSummary() const;

    /**
     * @brief Fail Stop the mount after an error, and return to the idle state
     */
    void Fail(const QString &message);

    /**
     * @brief NextAimTime First time >= jd at which the satellite is within the altitude limits
     * @return time (Julian day 2000), or a negative value if there is none before the end of the pass
     */
    double NextAimTime(const double jd);

    /**
     * @brief Target Where to point at a given time: the predicted satellite position with the user offsets
     */
    PointingSample Target(const double jd);

    void IssueGoto(const double jdAim);

    /**
     * @brief ContinuousStep One update of the continuous tracking controller
     * @throw MountError
     */
    void ContinuousStep(const double now);

    void StartRates(const double now);

    /**
     * @brief StopRates Back to sidereal tracking
     * @throw MountError
     */
    void StopRates();

    /**
     * @brief CatchUp Leave rate control and goto ahead of the satellite; rate control resumes when it gets there
     */
    void CatchUp(const double now,
                 const QString &reason);

    void OnArrival(const double now);

    void Finish();

    static QString FormatSeconds(const double seconds);

};

#endif // PASSTRACKER_H
