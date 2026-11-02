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
 * >    mounttransport.cpp
 *
 * Location
 * >    mount
 *
 * Description
 * >    Byte link to a telescope mount (serial port or TCP socket, e.g. an OnStep Wi-Fi access point)
 *
 * Creation date
 * >    2 November 2026
 *
 */

#include <QCoreApplication>
#include <QDeadlineTimer>
#include <QSerialPort>
#include <QTcpSocket>
#include "mounttransport.h"


MountTransport::MountTransport(const TransportSettings &settings) :
    _settings(settings)
{
}

MountTransport::~MountTransport()
{
    Close();
}

void MountTransport::Open()
{
    Close();

    if (_settings.type == TransportType::SERIAL) {

        auto port = std::make_unique<QSerialPort>();
        port->setPortName(_settings.serialPort);
        port->setBaudRate(_settings.baudRate);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(QSerialPort::NoFlowControl);

        if (!port->open(QIODevice::ReadWrite)) {
            throw MountError(QCoreApplication::translate("MountTransport", "Cannot open serial port %1: %2")
                             .arg(_settings.serialPort, port->errorString()));
        }

        _device = std::move(port);

    } else {

        auto socket = std::make_unique<QTcpSocket>();
        socket->connectToHost(_settings.host, _settings.tcpPort);

        if (!socket->waitForConnected(CONNECT_TIMEOUT_MS)) {
            throw MountError(QCoreApplication::translate("MountTransport", "Cannot connect to %1:%2: %3")
                             .arg(_settings.host).arg(_settings.tcpPort).arg(socket->errorString()));
        }

        // Commands are tiny and latency-sensitive: disable Nagle's algorithm
        socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
        _device = std::move(socket);
    }

    _buffer.clear();
}

void MountTransport::Close()
{
    if (_device) {
        _device->close();
        _device.reset();
    }
    _buffer.clear();
}

bool MountTransport::IsOpen() const
{
    if (!_device || !_device->isOpen()) {
        return false;
    }

    if (_settings.type == TransportType::TCP) {
        const auto *socket = static_cast<const QTcpSocket *>(_device.get());
        return (socket->state() == QAbstractSocket::ConnectedState);
    }

    return true;
}

void MountTransport::Send(const QByteArray &command)
{
    CheckOpen();
    DiscardInput();

    if (_device->write(command) != command.size()) {
        throw MountError(QCoreApplication::translate("MountTransport", "Write to mount failed: %1").arg(_device->errorString()));
    }

    if (!_device->waitForBytesWritten(WRITE_TIMEOUT_MS) && (_device->bytesToWrite() > 0)) {
        throw MountError(QCoreApplication::translate("MountTransport", "Write to mount timed out"));
    }
}

QByteArray MountTransport::ReadUntil(const char terminator,
                                     const int timeoutMs)
{
    CheckOpen();
    const QDeadlineTimer deadline(timeoutMs);

    while (true) {

        _buffer += _device->readAll();

        const qsizetype idx = _buffer.indexOf(terminator);
        if (idx >= 0) {
            const QByteArray reply = _buffer.left(idx);
            _buffer.remove(0, idx + 1);
            return reply;
        }

        if (deadline.hasExpired()) {
            throw MountError(QCoreApplication::translate("MountTransport", "No reply from mount (timeout)"));
        }

        WaitForData(static_cast<int>(deadline.remainingTime()));
    }
}

QByteArray MountTransport::ReadCount(const int count,
                                     const int timeoutMs)
{
    CheckOpen();
    const QDeadlineTimer deadline(timeoutMs);

    while (true) {

        _buffer += _device->readAll();

        if (_buffer.size() >= count) {
            const QByteArray reply = _buffer.left(count);
            _buffer.remove(0, count);
            return reply;
        }

        if (deadline.hasExpired()) {
            throw MountError(QCoreApplication::translate("MountTransport", "No reply from mount (timeout)"));
        }

        WaitForData(static_cast<int>(deadline.remainingTime()));
    }
}

void MountTransport::DiscardInput()
{
    if (_device) {
        _device->readAll();
    }
    _buffer.clear();
}

QString MountTransport::Description() const
{
    if (_settings.type == TransportType::SERIAL) {
        return QString("%1 @ %2 bd").arg(_settings.serialPort).arg(_settings.baudRate);
    }
    return QString("%1:%2").arg(_settings.host).arg(_settings.tcpPort);
}

const TransportSettings &MountTransport::settings() const
{
    return _settings;
}

bool MountTransport::WaitForData(const int timeoutMs)
{
    if (timeoutMs <= 0) {
        return false;
    }

    if (_device->waitForReadyRead(timeoutMs)) {
        _buffer += _device->readAll();
        return true;
    }

    if (!IsOpen()) {
        throw MountError(QCoreApplication::translate("MountTransport", "Connection to mount lost"));
    }

    return false;
}

void MountTransport::CheckOpen() const
{
    if (!IsOpen()) {
        throw MountError(QCoreApplication::translate("MountTransport", "Mount is not connected"));
    }
}
