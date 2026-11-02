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

struct TrackerSettings {
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

    void IssueGoto(const double jdAim);

    void OnArrival(const double now);

    void Finish();

    static QString FormatSeconds(const double seconds);

};

#endif // PASSTRACKER_H
