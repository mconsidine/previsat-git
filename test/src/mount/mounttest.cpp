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
 * >    mounttest.cpp
 *
 * Location
 * >    test/mount
 *
 * Description
 * >    Tests of the mount control layer
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <algorithm>
#include <cmath>
#include <QtTest>
#include <QElapsedTimer>
#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTemporaryDir>
#include <QThread>
#include <QTimeZone>
#include "librairies/corps/corps.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/dates/date.h"
#include "mount/lx200driver.h"
#include "mount/mountclock.h"
#include "mount/mountsimulator.h"
#include "mount/passtracker.h"
#include "mount/satellitepointing.h"
#include "mount/synscandriver.h"
#include "mounttest.h"


static constexpr double ARCSEC = MATHS::ARCSEC2RAD;

/*
 * Folder test/data of the source tree, searched upwards from the working and executable folders
 */
static QString FindTestData()
{
    const QStringList starts { QDir::currentPath(), QCoreApplication::applicationDirPath() };

    for (const QString &start : starts) {
        QDir dir(start);
        for (int i = 0; i < 8; i++) {
            if (QFile::exists(dir.filePath("test/data/taiutc.dat"))) {
                return dir.filePath("test/data");
            }
            if (!dir.cdUp()) {
                break;
            }
        }
    }

    return QString();
}

static bool IssPass(PassDefinition &pass);

void MountTest::testAll()
{
    const QString data = FindTestData();
    QVERIFY2(!data.isEmpty(), "test/data/taiutc.dat not found above the working directory");

    Corps::Initialisation(data);
    Date::Initialisation(data);

    testLx200Formatting();
    testLx200Parsing();
    testSynScanEncoding();
    testPointing();
    testSimulator();
    testSimulatedPass();
    testContinuousPass();
    testTargetOffsets();
    testOnStepRateMath();
    testOnStepDriver();
    testPrePassChecks();
}

void MountTest::testPrePassChecks()
{
    qInfo(Q_FUNC_INFO);

    // Greenwich mean sidereal time at J2000.0 (2000-01-01 12:00 UT): 18h 41m 50.548s
    const double gmst2000 = (18. + 41. / 60. + 50.548 / 3600.) * MATHS::HEUR2RAD;
    const double twoSeconds = 2. * 15. * ARCSEC;
    QVERIFY2(fabs(remainder(SatellitePointing::LocalSiderealTime(0., 0.) - gmst2000, MATHS::DEUX_PI)) < twoSeconds,
             qPrintable(QString("GMST %1 h").arg(SatellitePointing::LocalSiderealTime(0., 0.) / MATHS::HEUR2RAD, 0, 'f', 6)));

    // 90 degrees west: 6 hours earlier
    QVERIFY(fabs(remainder(SatellitePointing::LocalSiderealTime(0., MATHS::PI_SUR_DEUX) - (gmst2000 - MATHS::PI_SUR_DEUX),
                           MATHS::DEUX_PI)) < twoSeconds);

    // Clock advice: 1 s of sidereal time = 2 pi / 86164.0905 rad
    const double second = MATHS::DEUX_PI / 86164.0905;
    QVERIFY(PassTracker::ClockAdvice(0.5 * second).contains("OK"));
    const QString late = PassTracker::ClockAdvice(10. * second);
    QVERIFY2(late.startsWith("WARNING") && late.contains("10.0 s") && !late.contains("time zone"), qPrintable(late));
    QVERIFY(PassTracker::ClockAdvice(-3600. * second).contains("time zone"));

    // Meridian advice: a pass starting 20 degrees east of the meridian and ending 30 degrees west
    PassSummary summary;
    summary.valid = true;
    summary.crossesMeridian = true;
    summary.startHourAngle = -20. * MATHS::DEG2RAD;
    summary.minHourAngle = -20. * MATHS::DEG2RAD;
    summary.maxHourAngle = 30. * MATHS::DEG2RAD;
    summary.rateAtMeridian = 0.8 * MATHS::DEG2RAD;

    QString advice = PassTracker::MeridianAdvice(summary, false, 0., 0.);
    QVERIFY2(advice.contains("120 min past the meridian on the side for the eastern sky") && advice.contains("80 min on the side for the western sky"),
             qPrintable(advice));

    advice = PassTracker::MeridianAdvice(summary, true, 15. * MATHS::DEG2RAD, 15. * MATHS::DEG2RAD);
    QVERIFY2(advice.contains("E 60 min, W 60 min") && advice.contains("flip is likely"), qPrintable(advice));

    advice = PassTracker::MeridianAdvice(summary, true, 30. * MATHS::DEG2RAD, 5. * MATHS::DEG2RAD);
    QVERIFY2(advice.contains("may be enough"), qPrintable(advice));

    // A pass that stays west of the meridian
    summary.crossesMeridian = false;
    summary.startHourAngle = 10. * MATHS::DEG2RAD;
    summary.minHourAngle = 10. * MATHS::DEG2RAD;
    advice = PassTracker::MeridianAdvice(summary, false, 0., 0.);
    QVERIFY2(advice.contains("stays west") && advice.contains("no meridian flip"), qPrintable(advice));

    // The real ISS pass of the other tests
    PassDefinition pass;
    QVERIFY(IssPass(pass));
    SatellitePointing pointing(Satellite(pass.elements.first()), pass.observer);
    const PassSummary iss = pointing.Summarize(pass.jdStart, pass.jdEnd, 10. * MATHS::DEG2RAD);
    QVERIFY(iss.valid);
    QVERIFY((iss.minHourAngle <= iss.startHourAngle) && (iss.startHourAngle <= iss.maxHourAngle));
    qInfo().noquote() << PassTracker::MeridianAdvice(iss, false, 0., 0.);
}

/*
 * Minimal OnStep on a local TCP port, for driver tests: German equatorial mount, gotos are instantaneous,
 * custom-rate moves suspend sidereal tracking (as OnStepX documents) and :Mw increases the hour angle.
 */
class FakeOnStep : public QObject
{
public:

    quint16 Start()
    {
        _server = new QTcpServer(this);
        _server->listen(QHostAddress::LocalHost, 0);
        connect(_server, &QTcpServer::newConnection, this, [this]() {
            _socket = _server->nextPendingConnection();
            connect(_socket, &QTcpSocket::readyRead, this, [this]() { OnData(); });
        });
        _clock.start();
        _lastMs = 0;
        return _server->serverPort();
    }

    void State(int &direction1, double &rate1, int &direction2, double &rate2)
    {
        const QMutexLocker locker(&_mutex);
        direction1 = _direction1;
        rate1 = _rate1;
        direction2 = _direction2;
        rate2 = _rate2;
    }

private:

    static constexpr double SIDEREAL = 360.98564736629 / 86400.;

    QTcpServer *_server = nullptr;
    QTcpSocket *_socket = nullptr;
    QByteArray _buffer;
    QElapsedTimer _clock;
    qint64 _lastMs = 0;
    QMutex _mutex;

    double _ra = 90.;       // deg
    double _dec = 20.;
    double _targetRa = 0.;
    double _targetDec = 0.;
    double _rate1 = 0.;     // deg/s
    double _rate2 = 0.;
    int _direction1 = 0;
    int _direction2 = 0;

    void Advance()
    {
        const qint64 now = _clock.elapsed();
        const double dt = (now - _lastMs) / 1000.;
        _lastMs = now;

        // Axis 1 at rest: tracking keeps the RA. Moving: tracking suspended, RA = LST - HA
        if (_direction1 != 0) {
            _ra += (SIDEREAL - _direction1 * _rate1) * dt;
        }
        _dec += _direction2 * _rate2 * dt;
    }

    void OnData()
    {
        _buffer += _socket->readAll();

        while (!_buffer.isEmpty()) {

            if (_buffer.at(0) == '\x06') {
                _buffer.remove(0, 1);
                _socket->write("G");
                continue;
            }

            const qsizetype end = _buffer.indexOf('#');
            if (end < 0) {
                break;
            }

            const QByteArray command = _buffer.left(end + 1);
            _buffer.remove(0, end + 1);

            const QMutexLocker locker(&_mutex);
            Advance();
            _socket->write(Reply(command));
        }
        _socket->flush();
    }

    QByteArray Reply(const QByteArray &command)
    {
        if (command == ":GR#") {
            return Lx200Driver::FormatRightAscension(_ra * MATHS::DEG2RAD, true) + "#";
        }
        if (command == ":GD#") {
            return Lx200Driver::FormatDeclination(_dec * MATHS::DEG2RAD, true) + "#";
        }
        if ((command == ":GS#") || (command == ":GSH#")) {
            // Fixed sidereal time 06:00:00 (the test compares it with what the driver reads)
            return (command == ":GS#") ? "06:00:00#" : "06:00:00.00#";
        }
        if ((command == ":GXE9#") || (command == ":GXEA#")) {
            // Meridian limits: 60 and 30 minutes past the meridian
            return (command == ":GXE9#") ? "60#" : "30#";
        }
        if (command == ":GRH#") {
            // HH:MM:SS.SSSS
            double hours = fmod(_ra / 15., 24.);
            if (hours < 0.) {
                hours += 24.;
            }
            const int h = static_cast<int>(hours);
            const int m = static_cast<int>((hours - h) * 60.);
            const double s = ((hours - h) * 60. - m) * 60.;
            return QString("%1:%2:%3#").arg(h, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(s, 7, 'f', 4, QChar('0')).toLatin1();
        }
        if (command == ":GDH#") {
            // sDD*MM:SS.SSS
            const double deg = fabs(_dec);
            const int d = static_cast<int>(deg);
            const int m = static_cast<int>((deg - d) * 60.);
            const double s = ((deg - d) * 60. - m) * 60.;
            return QString("%1%2*%3:%4#").arg(QChar((_dec < 0.) ? '-' : '+')).arg(d, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0'))
                .arg(s, 6, 'f', 3, QChar('0')).toLatin1();
        }
        if (command == ":GVP#") {
            return "On-Step#";
        }
        if (command == ":GVN#") {
            return "10.25#";
        }
        if (command == ":Gm#") {
            return "E#";
        }
        if (command == ":D#") {
            return "#";
        }
        if (command == ":Te#") {
            return "1";
        }
        if (command.startsWith(":Sr")) {
            _targetRa = Lx200Driver::ParseRightAscension(command.mid(3, command.size() - 4)) * MATHS::RAD2DEG;
            return "1";
        }
        if (command.startsWith(":Sd")) {
            _targetDec = Lx200Driver::ParseDeclination(command.mid(3, command.size() - 4)) * MATHS::RAD2DEG;
            return "1";
        }
        if (command == ":MS#") {
            _ra = _targetRa;
            _dec = _targetDec;
            _direction1 = 0;
            _direction2 = 0;
            return "0";
        }
        if (command.startsWith(":RA")) {
            _rate1 = command.mid(3, command.size() - 4).toDouble();
            return QByteArray();
        }
        if (command.startsWith(":RE")) {
            _rate2 = command.mid(3, command.size() - 4).toDouble();
            return QByteArray();
        }
        if ((command == ":Mw#") || (command == ":Me#")) {
            _direction1 = (command == ":Mw#") ? 1 : -1;
            return QByteArray();
        }
        if ((command == ":Mn#") || (command == ":Ms#")) {
            _direction2 = (command == ":Mn#") ? 1 : -1;
            return QByteArray();
        }
        if ((command == ":Qw#") || (command == ":Qe#")) {
            _direction1 = 0;
            return QByteArray();
        }
        if ((command == ":Qn#") || (command == ":Qs#")) {
            _direction2 = 0;
            return QByteArray();
        }
        if (command == ":Q#") {
            _direction1 = 0;
            _direction2 = 0;
            return QByteArray();
        }
        if (command == ":U#") {
            return QByteArray();
        }
        return "0";
    }
};

void MountTest::testOnStepDriver()
{
    qInfo(Q_FUNC_INFO);

    QThread serverThread;
    auto *fake = new FakeOnStep;
    fake->moveToThread(&serverThread);
    QObject::connect(&serverThread, &QThread::finished, fake, &QObject::deleteLater);
    serverThread.start();

    quint16 port = 0;
    QMetaObject::invokeMethod(fake, [&]() { port = fake->Start(); }, Qt::BlockingQueuedConnection);
    QVERIFY(port != 0);

    TransportSettings link;
    link.type = TransportType::TCP;
    link.host = "127.0.0.1";
    link.tcpPort = port;

    Lx200Driver driver(link, true);
    const QString identity = driver.Connect();
    qInfo().noquote() << identity;
    QVERIFY(driver.IsEquatorial());
    QVERIFY(driver.CanCalibrateRates());
    QVERIFY(!driver.SupportsRaDecRates());

    // Calibration against the fake mount (real time: about 15 s)
    QString report;
    try {
        report = driver.CalibrateRates();
    } catch (const MountError &e) {
        QFAIL(qPrintable(e.message()));
    }
    qInfo().noquote() << report;
    QVERIFY(driver.SupportsRaDecRates());
    QVERIFY2(report.contains("suspended"), qPrintable(report));
    QVERIFY2(report.contains("applies to"), qPrintable(report));

    int direction1, direction2;
    double rate1, rate2;
    double ra, dec;

    // RA rate +0.3 deg/s, Dec rate -0.1 deg/s: with :Mw increasing the hour angle and tracking suspended,
    // the RA axis must move east at 0.3 - sidereal, the Dec axis south at 0.1
    driver.SetRaDecRates(0.3 * MATHS::DEG2RAD, -0.1 * MATHS::DEG2RAD);
    driver.GetRaDec(ra, dec);    // replies come after the commands: the fake has processed them
    fake->State(direction1, rate1, direction2, rate2);
    QCOMPARE(direction1, -1);
    QVERIFY2(fabs(rate1 - (0.3 - 360.98564736629 / 86400.)) < 2.e-3, qPrintable(QString("axis 1 at %1 deg/s").arg(rate1)));
    QCOMPARE(direction2, -1);
    QVERIFY2(fabs(rate2 - 0.1) < 2.e-3, qPrintable(QString("axis 2 at %1 deg/s").arg(rate2)));

    // (0, 0): moves stopped, back to sidereal tracking
    driver.SetRaDecRates(0., 0.);
    driver.GetRaDec(ra, dec);
    fake->State(direction1, rate1, direction2, rate2);
    QCOMPARE(direction1, 0);
    QCOMPARE(direction2, 0);

    // Clock and meridian queries
    double lst = 0.;
    QVERIFY(driver.GetSiderealTime(lst));
    QVERIFY(fabs(lst - 6. * MATHS::HEUR2RAD) < ARCSEC);

    double limitEast = 0.;
    double limitWest = 0.;
    QVERIFY(driver.GetMeridianLimits(limitEast, limitWest));
    QVERIFY(fabs(limitEast - 15. * MATHS::DEG2RAD) < 1.e-9);
    QVERIFY(fabs(limitWest - 7.5 * MATHS::DEG2RAD) < 1.e-9);
    QCOMPARE(driver.PierSide(), QString("E"));

    driver.Disconnect();
    serverThread.quit();
    serverThread.wait();
}

void MountTest::testOnStepRateMath()
{
    qInfo(Q_FUNC_INFO);

    const double sidereal = 360.98564736629 / 86400.;
    Lx200Driver::AxisCalibration axis;
    QString error;

    // Tracking suspended during moves, :Mw increases the right ascension
    QVERIFY(Lx200Driver::SolveAxisCalibration(0.5 + sidereal, -0.5 + sidereal, 0.5, axis, error));
    QVERIFY(fabs(axis.scale - 1.) < 1.e-12);
    QVERIFY(fabs(axis.background - sidereal) < 1.e-12);

    // A wanted RA rate of 0.3 deg/s: move west at 0.3 - sidereal; RA rate 0 (follow the stars): sidereal speed east
    QVERIFY(fabs(Lx200Driver::MoveSpeed(0.3, axis) - (0.3 - sidereal)) < 1.e-12);
    QVERIFY(fabs(Lx200Driver::MoveSpeed(0., axis) + sidereal) < 1.e-12);

    // Tracking continues, :Mw decreases the right ascension
    QVERIFY(Lx200Driver::SolveAxisCalibration(-0.49, 0.51, 0.5, axis, error));
    QVERIFY(fabs(axis.scale + 1.) < 1.e-12);
    QVERIFY(fabs(axis.background - 0.01) < 1.e-12);
    QVERIFY(fabs(Lx200Driver::MoveSpeed(0.21, axis) + 0.2) < 1.e-12);

    // Moves ten times too slow (unit problem): refused
    QVERIFY(!Lx200Driver::SolveAxisCalibration(0.05, -0.05, 0.5, axis, error));
    QVERIFY(!error.isEmpty());

    // No response at all: refused, and no speed computed from it
    QVERIFY(!Lx200Driver::SolveAxisCalibration(sidereal, sidereal, 0.5, axis, error));
    QCOMPARE(Lx200Driver::MoveSpeed(1., Lx200Driver::AxisCalibration()), 0.);

    QCOMPARE(Lx200Driver::FormatGuideRate(0.25), QByteArray("0.250000"));
    QCOMPARE(Lx200Driver::FormatGuideRate(0.0041781), QByteArray("0.004178"));
}

void MountTest::testLx200Formatting()
{
    qInfo(Q_FUNC_INFO);

    // Right ascension
    QCOMPARE(Lx200Driver::FormatRightAscension(0., true), QByteArray("00:00:00"));
    QCOMPARE(Lx200Driver::FormatRightAscension(12.5 * MATHS::HEUR2RAD, true), QByteArray("12:30:00"));
    QCOMPARE(Lx200Driver::FormatRightAscension(-1. * MATHS::HEUR2RAD, true), QByteArray("23:00:00"));
    QCOMPARE(Lx200Driver::FormatRightAscension((23. + 59. / 60. + 59.6 / 3600.) * MATHS::HEUR2RAD, true), QByteArray("00:00:00"));
    QCOMPARE(Lx200Driver::FormatRightAscension((12. + 34.5 / 60.) * MATHS::HEUR2RAD, false), QByteArray("12:34.5"));

    // Declination
    QCOMPARE(Lx200Driver::FormatDeclination(-0.5 * MATHS::DEG2RAD, true), QByteArray("-00*30:00"));
    QCOMPARE(Lx200Driver::FormatDeclination((45. + 30. / 60. + 15. / 3600.) * MATHS::DEG2RAD, true), QByteArray("+45*30:15"));
    QCOMPARE(Lx200Driver::FormatDeclination((45. + 30. / 60. + 15. / 3600.) * MATHS::DEG2RAD, false), QByteArray("+45*30"));
    QCOMPARE(Lx200Driver::FormatDeclination(91. * MATHS::DEG2RAD, true), QByteArray("+90*00:00"));

    // Site (longitudes positive towards the west)
    QCOMPARE(Lx200Driver::FormatLatitude(48.85339 * MATHS::DEG2RAD), QByteArray("+48*51"));
    QCOMPARE(Lx200Driver::FormatLatitude(-33.5 * MATHS::DEG2RAD), QByteArray("-33*30"));
    QCOMPARE(Lx200Driver::FormatLongitude(71.5 * MATHS::DEG2RAD), QByteArray("071*30"));
    QCOMPARE(Lx200Driver::FormatLongitude(-2.34864 * MATHS::DEG2RAD), QByteArray("357*39"));

    // Hours to add to local time to obtain UTC
    QCOMPARE(Lx200Driver::FormatUtcOffset(-5 * 3600, false), QByteArray("+05"));
    QCOMPARE(Lx200Driver::FormatUtcOffset(3600, false), QByteArray("-01"));
    QCOMPARE(Lx200Driver::FormatUtcOffset(0, true), QByteArray("+00"));
    QCOMPARE(Lx200Driver::FormatUtcOffset(19800, false), QByteArray("-05.5"));
    QCOMPARE(Lx200Driver::FormatUtcOffset(19800, true), QByteArray("-05:30"));
    QCOMPARE(Lx200Driver::FormatUtcOffset(-12600, false), QByteArray("+03.5"));
}

void MountTest::testLx200Parsing()
{
    qInfo(Q_FUNC_INFO);

    QVERIFY(fabs(Lx200Driver::ParseRightAscension("12:30:00") - 12.5 * MATHS::HEUR2RAD) < ARCSEC);
    QVERIFY(fabs(Lx200Driver::ParseRightAscension("12:34.5") - (12. + 34.5 / 60.) * MATHS::HEUR2RAD) < ARCSEC);
    QVERIFY(fabs(Lx200Driver::ParseRightAscension("01:02:03.5000") - (1. + 2. / 60. + 3.5 / 3600.) * MATHS::HEUR2RAD) < ARCSEC);

    const double dec = (45. + 30. / 60. + 15. / 3600.) * MATHS::DEG2RAD;
    QVERIFY(fabs(Lx200Driver::ParseDeclination("+45*30:15") - dec) < ARCSEC);
    QVERIFY(fabs(Lx200Driver::ParseDeclination("+45*30'15") - dec) < ARCSEC);
    QVERIFY(fabs(Lx200Driver::ParseDeclination("+45*30") - (45.5 * MATHS::DEG2RAD)) < ARCSEC);

    // Meade hand controllers use the degree sign of their own character set (0xDF)
    const QByteArray meade = QByteArray("-05") + static_cast<char>(0xDF) + QByteArray("30:00");
    QVERIFY(fabs(Lx200Driver::ParseDeclination(meade) + 5.5 * MATHS::DEG2RAD) < ARCSEC);

    bool thrown = false;
    try {
        Lx200Driver::ParseRightAscension("garbage");
    } catch (const MountError &) {
        thrown = true;
    }
    QVERIFY(thrown);

    QVERIFY(Lx200Driver::GotoErrorText('1', true).contains("horizon"));
    QVERIFY(Lx200Driver::GotoErrorText('4', true).contains("parked"));
}

void MountTest::testSynScanEncoding()
{
    qInfo(Q_FUNC_INFO);

    QCOMPARE(SynScanDriver::EncodeAngle(0.), QByteArray("00000000"));
    QCOMPARE(SynScanDriver::EncodeAngle(MATHS::PI_SUR_DEUX), QByteArray("40000000"));
    QCOMPARE(SynScanDriver::EncodeAngle(MATHS::PI), QByteArray("80000000"));
    QCOMPARE(SynScanDriver::EncodeAngle(-MATHS::PI_SUR_DEUX), QByteArray("C0000000"));

    QVERIFY(fabs(SynScanDriver::DecodeAngle("80000000") - MATHS::PI) < 1.e-9);
    QVERIFY(fabs(SynScanDriver::DecodeAngle("4000") - MATHS::PI_SUR_DEUX) < 1.e-9);

    const double angle = 1.234567;
    QVERIFY(fabs(SynScanDriver::DecodeAngle(SynScanDriver::EncodeAngle(angle)) - angle) < MATHS::DEUX_PI / 16777216.);

    bool thrown = false;
    try {
        SynScanDriver::DecodeAngle("12345");
    } catch (const MountError &) {
        thrown = true;
    }
    QVERIFY(thrown);
}

void MountTest::testPointing()
{
    qInfo(Q_FUNC_INFO);

    QVERIFY(fabs(SatellitePointing::AngularSeparation(0., 0., 0., MATHS::DEG2RAD) - MATHS::DEG2RAD) < 1.e-12);
    QVERIFY(fabs(SatellitePointing::AngularSeparation(0., 89.9 * MATHS::DEG2RAD, MATHS::PI, 89.9 * MATHS::DEG2RAD)
                 - 0.2 * MATHS::DEG2RAD) < 1.e-9);

    QCOMPARE(SatellitePointing::ToDateTimeUtc(0.), QDateTime(QDate(2000, 1, 1), QTime(12, 0), QTimeZone::UTC));

    const Date now;
    QVERIFY(fabs(SatellitePointing::NowJdUtc() - now.jourJulienUTC()) * DATE::NB_SEC_PAR_JOUR < 2.);
}

void MountTest::testSimulator()
{
    qInfo(Q_FUNC_INFO);

    SimulatedClock clock(0.);
    SimulatorSettings settings;
    settings.maxRate = 4.;
    settings.acceleration = 3.;
    settings.settleSeconds = 0.5;

    MountSimulator mount(clock, settings);
    mount.Connect();

    // 20 degree move on the declination axis: 4/3 s to accelerate and to decelerate, about 6.3 s in all
    mount.GotoRaDec(0., 20. * MATHS::DEG2RAD);
    QVERIFY(mount.IsSlewing());

    bool thrown = false;
    try {
        mount.GotoRaDec(0., 0.);
    } catch (const MountError &) {
        thrown = true;
    }
    QVERIFY2(thrown, "a goto while slewing must be refused");

    double elapsed = 0.;
    while (mount.IsSlewing() && (elapsed < 30.)) {
        clock.Advance(0.05);
        elapsed += 0.05;
    }
    QVERIFY2((elapsed > 6.5) && (elapsed < 7.5), qPrintable(QString("slew + settle took %1 s").arg(elapsed)));

    double ra, dec;
    mount.GetRaDec(ra, dec);
    QVERIFY(fabs(ra) < ARCSEC);
    QVERIFY(fabs(dec - 20. * MATHS::DEG2RAD) < ARCSEC);

    // Right ascension takes the shortest way round: 350 -> 10 degrees is a 20 degree move
    mount.GotoRaDec(350. * MATHS::DEG2RAD, 20. * MATHS::DEG2RAD);
    while (mount.IsSlewing()) {
        clock.Advance(0.05);
    }
    mount.GotoRaDec(10. * MATHS::DEG2RAD, 20. * MATHS::DEG2RAD);
    elapsed = 0.;
    while (mount.IsSlewing() && (elapsed < 30.)) {
        clock.Advance(0.05);
        elapsed += 0.05;
    }
    QVERIFY2(elapsed < 7.5, qPrintable(QString("wrap-around slew took %1 s").arg(elapsed)));

    // Stop halts the motion where it is
    mount.GotoRaDec(100. * MATHS::DEG2RAD, 20. * MATHS::DEG2RAD);
    clock.Advance(2.);
    mount.Stop();
    QVERIFY(!mount.IsSlewing());
    mount.GetRaDec(ra, dec);
    clock.Advance(5.);
    double ra2, dec2;
    mount.GetRaDec(ra2, dec2);
    QVERIFY(fabs(ra2 - ra) < ARCSEC);
}

void MountTest::testSimulatedPass()
{
    qInfo(Q_FUNC_INFO);

    // ISS pass over Paris, elements of 11 January 2020 (as in EvenementsTest)
    const TLE tle("ISS (ZARYA)",
                  "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993",
                  "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533");
    Satellite sat(tle.elements());
    Observateur obs("Paris", -2.348640000, +48.853390000, 30);

    const double minAltitude = 10. * MATHS::DEG2RAD;

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    obs.CalculPosVit(date);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);
    const ElementsAOS aos = Evenements::CalculAOS(date, sat, obs, SensCalcul::CHRONOLOGIQUE, minAltitude);
    QVERIFY(aos.aos);

    date = Date(aos.date.jourJulienUTC() + 10. * DATE::NB_JOUR_PAR_SEC, 0.);
    obs.CalculPosVit(date);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);
    const ElementsAOS los = Evenements::CalculAOS(date, sat, obs, SensCalcul::CHRONOLOGIQUE, minAltitude);
    QVERIFY(los.aos);

    PassDefinition pass;
    pass.satelliteName = "ISS";
    pass.elements.append(tle.elements());
    pass.observer = Observateur("Paris", -2.348640000, +48.853390000, 30);
    pass.jdStart = aos.date.jourJulienUTC();
    pass.jdEnd = los.date.jourJulienUTC();

    const double duration = (pass.jdEnd - pass.jdStart) * DATE::NB_SEC_PAR_JOUR;
    QVERIFY2(duration > 60., qPrintable(QString("pass of %1 s").arg(duration)));

    QTemporaryDir logDir;
    QVERIFY(logDir.isValid());

    TrackerSettings settings;
    settings.minAltitude = minAltitude;
    settings.maxAltitude = 88. * MATHS::DEG2RAD;
    settings.prePositionSeconds = 120;
    settings.dwellSeconds = 1.;
    settings.logDirectory = logDir.path();

    SimulatedClock clock(pass.jdStart - 200. * DATE::NB_JOUR_PAR_SEC);

    PassTracker tracker;
    tracker.SetClock(clock);
    auto mount = std::make_unique<MountSimulator>(clock);
    mount->Connect();
    tracker._driver = std::move(mount);
    tracker._state = PassTracker::State::IDLE;

    tracker.Arm(pass, settings);
    QCOMPARE(tracker._state, PassTracker::State::WAITING);

    // Run the pass at 5 updates per (simulated) second, as the real timer does
    const int maxSteps = static_cast<int>((duration + 400.) / 0.2);
    for (int i = 0; (i < maxSteps) && (tracker._state != PassTracker::State::FINISHED)
                    && (tracker._state != PassTracker::State::IDLE); i++) {
        clock.Advance(0.2);
        tracker.Tick();
    }

    qInfo().noquote() << tracker.TrackingSummary();

    QCOMPARE(tracker._state, PassTracker::State::FINISHED);
    QVERIFY2(tracker._gotoCount >= 10, qPrintable(QString("%1 gotos").arg(tracker._gotoCount)));
    QVERIFY2(tracker._lateArrivals <= 2, qPrintable(QString("%1 late arrivals").arg(tracker._lateArrivals)));
    QVERIFY(!tracker._trackingSeparations.isEmpty());

    // The simulator lands exactly where asked
    QVERIFY2((tracker._lastErrorArcmin >= 0.) && (tracker._lastErrorArcmin < 0.1),
             qPrintable(QString("arrival error %1'").arg(tracker._lastErrorArcmin)));

    // Leapfrog: the satellite comes back to the mount at every hop
    QList<double> sorted = tracker._trackingSeparations;
    std::sort(sorted.begin(), sorted.end());
    QVERIFY2(sorted.first() < 10., qPrintable(QString("closest approach %1'").arg(sorted.first())));
    QVERIFY2(sorted.at(sorted.size() / 2) < 180., qPrintable(QString("median separation %1'").arg(sorted.at(sorted.size() / 2))));

    // Pass log: header, one line per second of the pass and events
    const QStringList logs = QDir(logDir.path()).entryList(QStringList() << "mount_*.csv", QDir::Files);
    QCOMPARE(logs.size(), 1);

    QFile log(logDir.filePath(logs.first()));
    QVERIFY(log.open(QIODevice::ReadOnly | QIODevice::Text));
    const QStringList lines = QString::fromUtf8(log.readAll()).split('\n', Qt::SkipEmptyParts);
    QVERIFY(lines.first().startsWith("utc,state,sat_az_deg"));
    QVERIFY2(lines.size() > static_cast<int>(duration), qPrintable(QString("%1 log lines").arg(lines.size())));

    // A pass that is over cannot be armed again
    tracker.Arm(pass, settings);
    QCOMPARE(tracker._state, PassTracker::State::FINISHED);
}

/*
 * Next ISS pass above 10 degrees over Paris after 11 January 2020 08:27:30 UTC
 */
static bool IssPass(PassDefinition &pass)
{
    const TLE tle("ISS (ZARYA)",
                  "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993",
                  "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533");
    Satellite sat(tle.elements());
    Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    const double minAltitude = 10. * MATHS::DEG2RAD;

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    obs.CalculPosVit(date);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);
    const ElementsAOS aos = Evenements::CalculAOS(date, sat, obs, SensCalcul::CHRONOLOGIQUE, minAltitude);

    date = Date(aos.date.jourJulienUTC() + 10. * DATE::NB_JOUR_PAR_SEC, 0.);
    obs.CalculPosVit(date);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);
    const ElementsAOS los = Evenements::CalculAOS(date, sat, obs, SensCalcul::CHRONOLOGIQUE, minAltitude);

    pass.satelliteName = "ISS";
    pass.elements = { tle.elements() };
    pass.observer = Observateur("Paris", -2.348640000, +48.853390000, 30);
    pass.jdStart = aos.date.jourJulienUTC();
    pass.jdEnd = los.date.jourJulienUTC();

    return aos.aos && los.aos && (pass.jdEnd > pass.jdStart);
}

/*
 * Run a whole pass against the simulator, 5 updates per simulated second
 */
void MountTest::RunPass(PassTracker &tracker,
                        SimulatedClock &clock,
                        const PassDefinition &pass)
{
    const double duration = (pass.jdEnd - pass.jdStart) * DATE::NB_SEC_PAR_JOUR;
    const int maxSteps = static_cast<int>((duration + 400.) / 0.2);

    for (int i = 0; (i < maxSteps) && (tracker._state != PassTracker::State::FINISHED)
                    && (tracker._state != PassTracker::State::IDLE); i++) {
        clock.Advance(0.2);
        tracker.Tick();
    }
}

void MountTest::testContinuousPass()
{
    qInfo(Q_FUNC_INFO);

    PassDefinition pass;
    QVERIFY(IssPass(pass));

    // Ideal feedback, then a position readback 0.3 s late (slow controller)
    for (const double readbackDelay : { 0., 0.3 }) {

        TrackerSettings settings;
        settings.mode = TrackingMode::CONTINUOUS;
        settings.minAltitude = 10. * MATHS::DEG2RAD;
        settings.maxAltitude = 88. * MATHS::DEG2RAD;

        // The calibration will measure this delay on a real mount
        settings.continuous.positionDelay = readbackDelay;

        SimulatorSettings mountSettings;
        mountSettings.maxRate = 4.;
        mountSettings.acceleration = 3.;
        mountSettings.readbackDelaySeconds = readbackDelay;

        SimulatedClock clock(pass.jdStart - 200. * DATE::NB_JOUR_PAR_SEC);

        PassTracker tracker;
        tracker.SetClock(clock);
        auto mount = std::make_unique<MountSimulator>(clock, mountSettings);
        mount->Connect();
        tracker._driver = std::move(mount);
        tracker._state = PassTracker::State::IDLE;

        tracker.Arm(pass, settings);
        QVERIFY(tracker._continuous);

        RunPass(tracker, clock, pass);

        const QString summary = tracker.TrackingSummary();
        qInfo().noquote() << QString("readback delay %1 s:").arg(readbackDelay) << summary;

        QCOMPARE(tracker._state, PassTracker::State::FINISHED);
        QVERIFY2(tracker._trackingSeparations.size() > 60, qPrintable(summary));

        QList<double> sorted = tracker._trackingSeparations;
        std::sort(sorted.begin(), sorted.end());
        const double median = sorted.at(sorted.size() / 2);
        const double p90 = sorted.at((sorted.size() * 9) / 10);

        // Continuous tracking keeps the satellite close to the centre all along the pass
        QVERIFY2(median < 2., qPrintable(summary));
        QVERIFY2(p90 < 6., qPrintable(summary));
        QVERIFY2(tracker._catchUps <= 1, qPrintable(summary));
    }

    // A mount without rate control falls back to leapfrog
    PassTracker tracker;
    SimulatedClock clock(pass.jdStart - 200. * DATE::NB_JOUR_PAR_SEC);
    tracker.SetClock(clock);

    class GotoOnlySimulator : public MountSimulator
    {
    public:
        using MountSimulator::MountSimulator;
        bool SupportsRaDecRates() const override { return false; }
    };

    auto mount = std::make_unique<GotoOnlySimulator>(clock);
    mount->Connect();
    tracker._driver = std::move(mount);
    tracker._state = PassTracker::State::IDLE;

    TrackerSettings settings;
    settings.mode = TrackingMode::CONTINUOUS;
    tracker.Arm(pass, settings);
    QVERIFY(!tracker._continuous);
}

void MountTest::testTargetOffsets()
{
    qInfo(Q_FUNC_INFO);

    PassDefinition pass;
    QVERIFY(IssPass(pass));

    PassTracker tracker;
    const Satellite satellite(pass.elements.first());
    tracker._pointing = std::make_unique<SatellitePointing>(satellite, pass.observer);

    const double jd = 0.5 * (pass.jdStart + pass.jdEnd);
    const PointingSample now = tracker._pointing->Compute(jd);
    const PointingSample later = tracker._pointing->Compute(jd + 2. * DATE::NB_JOUR_PAR_SEC);

    // No offset: the satellite itself
    PointingSample target = tracker.Target(jd);
    QVERIFY(SatellitePointing::AngularSeparation(target.rightAscension, target.declination,
                                                 now.rightAscension, now.declination) < ARCSEC);

    // Timing: where the satellite will be 2 s later
    tracker.SetOffsets(2., 0.);
    target = tracker.Target(jd);
    QVERIFY(SatellitePointing::AngularSeparation(target.rightAscension, target.declination,
                                                 later.rightAscension, later.declination) < ARCSEC);

    // Cross-track: 10' away from the satellite, perpendicular to its motion
    tracker.SetOffsets(0., 10.);
    target = tracker.Target(jd);
    const double offset = SatellitePointing::AngularSeparation(target.rightAscension, target.declination,
                                                               now.rightAscension, now.declination);
    QVERIFY2(fabs(offset - 10. * MATHS::ARCMIN2RAD) < 0.01 * MATHS::ARCMIN2RAD,
             qPrintable(QString("cross-track offset %1'").arg(offset / MATHS::ARCMIN2RAD)));

    // Perpendicular: the offset point is as far from the positions 1 s before and 1 s after
    const PointingSample before = tracker._pointing->Compute(jd - DATE::NB_JOUR_PAR_SEC);
    const PointingSample after = tracker._pointing->Compute(jd + DATE::NB_JOUR_PAR_SEC);
    const double toBefore = SatellitePointing::AngularSeparation(target.rightAscension, target.declination,
                                                                 before.rightAscension, before.declination);
    const double toAfter = SatellitePointing::AngularSeparation(target.rightAscension, target.declination,
                                                                after.rightAscension, after.declination);
    QVERIFY2(fabs(toBefore - toAfter) < 0.02 * toAfter,
             qPrintable(QString("%1' before, %2' after").arg(toBefore / MATHS::ARCMIN2RAD).arg(toAfter / MATHS::ARCMIN2RAD)));

    // Opposite sign: the other side
    tracker.SetOffsets(0., -10.);
    const PointingSample opposite = tracker.Target(jd);
    QVERIFY(SatellitePointing::AngularSeparation(target.rightAscension, target.declination,
                                                 opposite.rightAscension, opposite.declination) > 19.9 * MATHS::ARCMIN2RAD);
}
