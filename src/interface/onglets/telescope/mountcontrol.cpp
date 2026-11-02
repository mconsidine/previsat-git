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
 * >    mountcontrol.cpp
 *
 * Location
 * >    interface/onglets/telescope
 *
 * Heritage
 * >    QDialog
 *
 * Description
 * >    Window to connect a mount (LX200, OnStepX, SynScan) and drive it along the selected pass
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <algorithm>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "mountcontrol.h"


// Registry
#if (PORTABLE_BUILD)
static QSettings settings(QString("%1.ini").arg(APP_NAME), QSettings::IniFormat);
#else
static QSettings settings(ORG_NAME, APP_NAME);
#endif


MountControl::MountControl(QWidget *parent) :
    QDialog(parent),
    _tracker(new PassTracker),
    _hasPass(false),
    _connected(false),
    _state(static_cast<int>(PassTracker::State::DISCONNECTED))
{
    setWindowTitle(tr("Live mount control"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    BuildInterface();
    LoadSettings();

    // All mount I/O is blocking: the tracker lives in its own thread
    _tracker->moveToThread(&_thread);
    connect(&_thread, &QThread::finished, _tracker, &QObject::deleteLater);

    connect(_tracker, &PassTracker::connectionChanged, this, &MountControl::OnConnectionChanged);
    connect(_tracker, &PassTracker::stateChanged, this, &MountControl::OnStateChanged);
    connect(_tracker, &PassTracker::logMessage, this, &MountControl::OnLogMessage);
    connect(_tracker, &PassTracker::positionUpdated, this, &MountControl::OnPositionUpdated);

    _thread.start();
    UpdateButtons();
}

MountControl::~MountControl()
{
    SaveSettings();

    PassTracker *tracker = _tracker;
    QMetaObject::invokeMethod(tracker, [tracker]() {
        tracker->Abort();
        tracker->DisconnectMount();
    }, Qt::BlockingQueuedConnection);

    _thread.quit();
    _thread.wait();
}

void MountControl::SetPass(const PassDefinition &pass)
{
    _pass = pass;
    _hasPass = !pass.elements.isEmpty();

    const QString fmt = "yyyy-MM-dd HH:mm:ss";
    _passDescription->setText(tr("%1 from %2\nStart: %3   End: %4 (local time)")
                              .arg(pass.satelliteName, pass.observer.nomlieu())
                              .arg(SatellitePointing::ToDateTimeUtc(pass.jdStart).toLocalTime().toString(fmt))
                              .arg(SatellitePointing::ToDateTimeUtc(pass.jdEnd).toLocalTime().toString(fmt)));
    UpdateButtons();
}

void MountControl::ToggleConnection()
{
    PassTracker *tracker = _tracker;

    if (_connected) {
        QMetaObject::invokeMethod(tracker, [tracker]() {
            tracker->Abort();
            tracker->DisconnectMount();
        }, Qt::QueuedConnection);
        return;
    }

    const MountProtocol protocol = static_cast<MountProtocol>(_protocol->currentData().toInt());

    TransportSettings transport;
    transport.type = static_cast<TransportType>(_link->currentData().toInt());
    transport.serialPort = _serialPort->currentText().section(' ', 0, 0);
    transport.baudRate = _baudRate->currentText().toInt();
    transport.host = _host->text().trimmed();
    transport.tcpPort = static_cast<quint16>(_tcpPort->value());

    const bool sendSiteAndTime = _sendSiteAndTime->isChecked() && _hasPass;
    const double latitude = _pass.observer.latitude();
    const double longitude = _pass.observer.longitude();

    if (_sendSiteAndTime->isChecked() && !_hasPass) {
        OnLogMessage(tr("No observing site selected yet: site and time not sent"));
    }

    _connectButton->setEnabled(false);
    SaveSettings();

    QMetaObject::invokeMethod(tracker, [=]() {
        tracker->ConnectMount(protocol, transport, sendSiteAndTime, latitude, longitude);
    }, Qt::QueuedConnection);
}

void MountControl::ArmPass()
{
    if (!_hasPass) {
        return;
    }

    TrackerSettings trackerSettings;
    trackerSettings.minAltitude = _minAltitude->value() * MATHS::DEG2RAD;
    trackerSettings.maxAltitude = _maxAltitude->value() * MATHS::DEG2RAD;
    trackerSettings.prePositionSeconds = _prePosition->value();
    trackerSettings.dwellSeconds = _dwell->value();
    trackerSettings.parkAtEnd = _parkAtEnd->isChecked();

    if (trackerSettings.maxAltitude <= trackerSettings.minAltitude) {
        QMessageBox::warning(this, windowTitle(), tr("The maximum altitude must be greater than the minimum altitude"));
        return;
    }

    SaveSettings();

    PassTracker *tracker = _tracker;
    const PassDefinition pass = _pass;
    QMetaObject::invokeMethod(tracker, [=]() {
        tracker->Arm(pass, trackerSettings);
    }, Qt::QueuedConnection);
}

void MountControl::AbortPass()
{
    PassTracker *tracker = _tracker;
    QMetaObject::invokeMethod(tracker, [tracker]() {
        tracker->Abort();
    }, Qt::QueuedConnection);
}

void MountControl::RefreshSerialPorts()
{
    const QString current = _serialPort->currentText().section(' ', 0, 0);
    _serialPort->clear();

    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        const QString description = info.description().isEmpty() ? QString() : QString(" (%1)").arg(info.description());
        _serialPort->addItem(info.portName() + description);
    }

    if (!current.isEmpty()) {
        const int idx = _serialPort->findText(current, Qt::MatchStartsWith);
        if (idx >= 0) {
            _serialPort->setCurrentIndex(idx);
        } else {
            _serialPort->setEditText(current);
        }
    }
}

void MountControl::OnConnectionChanged(const bool connected,
                                       const QString &identity)
{
    _connected = connected;
    _identity->setText(connected ? identity : tr("Not connected"));
    UpdateButtons();
}

void MountControl::OnStateChanged(const int state,
                                  const QString &description)
{
    _state = state;
    _status->setText(description);
    UpdateButtons();
}

void MountControl::OnLogMessage(const QString &message)
{
    _log->appendPlainText(QString("%1  %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss"), message));
}

void MountControl::OnPositionUpdated(const double azimuth,
                                     const double altitude,
                                     const double errorArcmin)
{
    QString text = tr("Satellite: azimuth %1°, altitude %2°").arg(azimuth, 0, 'f', 2).arg(altitude, 0, 'f', 2);
    if (errorArcmin >= 0.) {
        text += "   " + tr("last pointing error: %1'").arg(errorArcmin, 0, 'f', 1);
    }
    _position->setText(text);
}

void MountControl::closeEvent(QCloseEvent *evt)
{
    const auto state = static_cast<PassTracker::State>(_state);
    const bool active = (state == PassTracker::State::WAITING) || (state == PassTracker::State::PREPOSITIONING)
                        || (state == PassTracker::State::TRACKING);

    if (active) {
        const QMessageBox::StandardButton answer =
            QMessageBox::question(this, windowTitle(), tr("A pass is armed or being tracked. Abort it and stop the mount?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (answer != QMessageBox::Yes) {
            evt->ignore();
            return;
        }
        AbortPass();
    }

    SaveSettings();
    evt->accept();
}

void MountControl::BuildInterface()
{
    auto *mainLayout = new QVBoxLayout(this);

    /*
     * Connection
     */
    auto *connectionGroup = new QGroupBox(tr("Mount connection"), this);
    auto *connectionLayout = new QFormLayout(connectionGroup);

    _protocol = new QComboBox(connectionGroup);
    _protocol->addItem(tr("OnStep / OnStepX (LX200 extended)"), static_cast<int>(MountProtocol::ONSTEPX));
    _protocol->addItem(tr("Meade LX200 classic"), static_cast<int>(MountProtocol::LX200_CLASSIC));
    _protocol->addItem(tr("Sky-Watcher SynScan hand controller"), static_cast<int>(MountProtocol::SYNSCAN));
    connectionLayout->addRow(tr("Protocol:"), _protocol);

    _link = new QComboBox(connectionGroup);
    _link->addItem(tr("Wi-Fi / network (TCP)"), static_cast<int>(TransportType::TCP));
    _link->addItem(tr("Serial / USB"), static_cast<int>(TransportType::SERIAL));
    connectionLayout->addRow(tr("Link:"), _link);

    _linkSettings = new QStackedWidget(connectionGroup);

    auto *tcpPage = new QWidget(_linkSettings);
    auto *tcpLayout = new QHBoxLayout(tcpPage);
    tcpLayout->setContentsMargins(0, 0, 0, 0);
    _host = new QLineEdit(tcpPage);
    _host->setPlaceholderText("192.168.0.1");
    _tcpPort = new QSpinBox(tcpPage);
    _tcpPort->setRange(1, 65535);
    tcpLayout->addWidget(new QLabel(tr("Address:"), tcpPage));
    tcpLayout->addWidget(_host, 1);
    tcpLayout->addWidget(new QLabel(tr("Port:"), tcpPage));
    tcpLayout->addWidget(_tcpPort);

    auto *serialPage = new QWidget(_linkSettings);
    auto *serialLayout = new QHBoxLayout(serialPage);
    serialLayout->setContentsMargins(0, 0, 0, 0);
    _serialPort = new QComboBox(serialPage);
    _serialPort->setEditable(true);
    auto *refresh = new QPushButton(tr("Refresh"), serialPage);
    _baudRate = new QComboBox(serialPage);
    _baudRate->addItems({ "4800", "9600", "19200", "38400", "57600", "115200" });
    serialLayout->addWidget(new QLabel(tr("Port:"), serialPage));
    serialLayout->addWidget(_serialPort, 1);
    serialLayout->addWidget(refresh);
    serialLayout->addWidget(new QLabel(tr("Baud:"), serialPage));
    serialLayout->addWidget(_baudRate);

    _linkSettings->addWidget(tcpPage);
    _linkSettings->addWidget(serialPage);
    connectionLayout->addRow(QString(), _linkSettings);

    _sendSiteAndTime = new QCheckBox(tr("Send observing site and computer time to the mount on connection"), connectionGroup);
    connectionLayout->addRow(QString(), _sendSiteAndTime);

    auto *connectRow = new QHBoxLayout;
    _connectButton = new QPushButton(tr("Connect"), connectionGroup);
    _identity = new QLabel(tr("Not connected"), connectionGroup);
    _identity->setWordWrap(true);
    connectRow->addWidget(_connectButton);
    connectRow->addWidget(_identity, 1);
    connectionLayout->addRow(connectRow);

    mainLayout->addWidget(connectionGroup);

    /*
     * Pass
     */
    auto *passGroup = new QGroupBox(tr("Pass"), this);
    auto *passLayout = new QFormLayout(passGroup);

    _passDescription = new QLabel(tr("No pass selected: choose a satellite in the telescope tab"), passGroup);
    _passDescription->setWordWrap(true);
    passLayout->addRow(_passDescription);

    _minAltitude = new QSpinBox(passGroup);
    _minAltitude->setRange(0, 80);
    _minAltitude->setSuffix("°");
    passLayout->addRow(tr("Minimum altitude:"), _minAltitude);

    _maxAltitude = new QSpinBox(passGroup);
    _maxAltitude->setRange(30, 90);
    _maxAltitude->setSuffix("°");
    _maxAltitude->setToolTip(tr("Alt-azimuth mounts cannot follow a satellite close to the zenith"));
    passLayout->addRow(tr("Maximum altitude:"), _maxAltitude);

    _prePosition = new QSpinBox(passGroup);
    _prePosition->setRange(10, 1800);
    _prePosition->setSuffix(" s");
    passLayout->addRow(tr("Slew to start point ahead of pass:"), _prePosition);

    _dwell = new QDoubleSpinBox(passGroup);
    _dwell->setRange(0., 10.);
    _dwell->setDecimals(1);
    _dwell->setSingleStep(0.5);
    _dwell->setSuffix(" s");
    _dwell->setToolTip(tr("Time to stay at each aim point after the satellite reaches it, before jumping ahead"));
    passLayout->addRow(tr("Dwell at each aim point:"), _dwell);

    _parkAtEnd = new QCheckBox(tr("Park the mount at the end of the pass (OnStep)"), passGroup);
    passLayout->addRow(QString(), _parkAtEnd);

    auto *passButtons = new QHBoxLayout;
    _armButton = new QPushButton(tr("Arm pass"), passGroup);
    _abortButton = new QPushButton(tr("STOP / Abort"), passGroup);
    _abortButton->setStyleSheet("QPushButton:enabled { color: white; background-color: #b00020; font-weight: bold; }");
    passButtons->addWidget(_armButton);
    passButtons->addWidget(_abortButton);
    passLayout->addRow(passButtons);

    mainLayout->addWidget(passGroup);

    /*
     * Status
     */
    _status = new QLabel(tr("Not connected"), this);
    QFont bold = _status->font();
    bold.setBold(true);
    _status->setFont(bold);
    _position = new QLabel(this);

    auto *clockNote = new QLabel(tr("Pointing depends on the computer clock: synchronise it (NTP) before the pass."), this);
    clockNote->setWordWrap(true);

    _log = new QPlainTextEdit(this);
    _log->setReadOnly(true);
    _log->setMaximumBlockCount(2000);
    _log->setMinimumHeight(140);

    mainLayout->addWidget(_status);
    mainLayout->addWidget(_position);
    mainLayout->addWidget(clockNote);
    mainLayout->addWidget(_log, 1);

    resize(560, 640);

    connect(_link, &QComboBox::currentIndexChanged, _linkSettings, &QStackedWidget::setCurrentIndex);
    connect(refresh, &QPushButton::clicked, this, &MountControl::RefreshSerialPorts);
    connect(_connectButton, &QPushButton::clicked, this, &MountControl::ToggleConnection);
    connect(_armButton, &QPushButton::clicked, this, &MountControl::ArmPass);
    connect(_abortButton, &QPushButton::clicked, this, &MountControl::AbortPass);
}

void MountControl::LoadSettings()
{
    RefreshSerialPorts();

    _protocol->setCurrentIndex(std::max(0, _protocol->findData(settings.value("mount/protocol", 0).toInt())));
    _link->setCurrentIndex(std::max(0, _link->findData(settings.value("mount/link", static_cast<int>(TransportType::TCP)).toInt())));
    _linkSettings->setCurrentIndex(_link->currentIndex());
    _host->setText(settings.value("mount/host", "192.168.0.1").toString());
    _tcpPort->setValue(settings.value("mount/tcpPort", 9999).toInt());
    _baudRate->setCurrentText(settings.value("mount/baudRate", "9600").toString());
    _sendSiteAndTime->setChecked(settings.value("mount/sendSiteAndTime", true).toBool());

    const QString port = settings.value("mount/serialPort", "").toString();
    if (!port.isEmpty()) {
        const int idx = _serialPort->findText(port, Qt::MatchStartsWith);
        if (idx >= 0) {
            _serialPort->setCurrentIndex(idx);
        } else {
            _serialPort->setEditText(port);
        }
    }

    _minAltitude->setValue(settings.value("mount/minAltitude", 10).toInt());
    _maxAltitude->setValue(settings.value("mount/maxAltitude", 88).toInt());
    _prePosition->setValue(settings.value("mount/prePosition", 120).toInt());
    _dwell->setValue(settings.value("mount/dwell", 1.).toDouble());
    _parkAtEnd->setChecked(settings.value("mount/parkAtEnd", false).toBool());

    // Usual link of each protocol, applied when the user changes the protocol
    connect(_protocol, &QComboBox::currentIndexChanged, this, [this]() {
        const auto protocol = static_cast<MountProtocol>(_protocol->currentData().toInt());
        const TransportType link = (protocol == MountProtocol::ONSTEPX) ? TransportType::TCP : TransportType::SERIAL;
        _link->setCurrentIndex(_link->findData(static_cast<int>(link)));
        _baudRate->setCurrentText("9600");
        if (protocol == MountProtocol::ONSTEPX) {
            _host->setText("192.168.0.1");
            _tcpPort->setValue(9999);
        }
    });
}

void MountControl::SaveSettings() const
{
    settings.setValue("mount/protocol", _protocol->currentData().toInt());
    settings.setValue("mount/link", _link->currentData().toInt());
    settings.setValue("mount/host", _host->text().trimmed());
    settings.setValue("mount/tcpPort", _tcpPort->value());
    settings.setValue("mount/serialPort", _serialPort->currentText().section(' ', 0, 0));
    settings.setValue("mount/baudRate", _baudRate->currentText());
    settings.setValue("mount/sendSiteAndTime", _sendSiteAndTime->isChecked());
    settings.setValue("mount/minAltitude", _minAltitude->value());
    settings.setValue("mount/maxAltitude", _maxAltitude->value());
    settings.setValue("mount/prePosition", _prePosition->value());
    settings.setValue("mount/dwell", _dwell->value());
    settings.setValue("mount/parkAtEnd", _parkAtEnd->isChecked());
}

void MountControl::UpdateButtons()
{
    const auto state = static_cast<PassTracker::State>(_state);
    const bool active = (state == PassTracker::State::WAITING) || (state == PassTracker::State::PREPOSITIONING)
                        || (state == PassTracker::State::TRACKING);

    _connectButton->setEnabled(true);
    _connectButton->setText(_connected ? tr("Disconnect") : tr("Connect"));

    for (QWidget *w : std::initializer_list<QWidget *> { _protocol, _link, _linkSettings, _sendSiteAndTime }) {
        w->setEnabled(!_connected);
    }

    _armButton->setEnabled(_connected && _hasPass && !active);
    _abortButton->setEnabled(_connected);

    for (QWidget *w : std::initializer_list<QWidget *> { _minAltitude, _maxAltitude, _prePosition, _dwell, _parkAtEnd }) {
        w->setEnabled(!active);
    }
}
