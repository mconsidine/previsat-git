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
 * >    mounttransport.h
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

#ifndef MOUNTTRANSPORT_H
#define MOUNTTRANSPORT_H

#include <exception>
#include <memory>
#include <QByteArray>
#include <QString>


class QIODevice;

/**
 * @brief Error raised by the mount control layer
 * (the project's Exception class shows a message box, which must not happen from the tracking thread)
 */
class MountError : public std::exception
{
public:
    explicit MountError(const QString &message) :
        _message(message),
        _utf8(message.toUtf8())
    {
    }

    const QString &message() const
    {
        return _message;
    }

    const char *what() const noexcept override
    {
        return _utf8.constData();
    }

private:
    QString _message;
    QByteArray _utf8;
};


enum class TransportType {
    SERIAL = 0,
    TCP
};

struct TransportSettings {
    TransportType type = TransportType::SERIAL;
    QString serialPort;
    int baudRate = 9600;
    QString host = "192.168.0.1";
    quint16 tcpPort = 9999;
};


class MountTransport
{
public:

    explicit MountTransport(const TransportSettings &settings);
    ~MountTransport();

    MountTransport(const MountTransport &) = delete;
    MountTransport &operator = (const MountTransport &) = delete;

    /**
     * @brief Open Open the link
     * @throw MountError
     */
    void Open();

    void Close();

    bool IsOpen() const;

    /**
     * @brief Send Discard any pending input, then write a command
     * @param[in] command bytes to send
     * @throw MountError
     */
    void Send(const QByteArray &command);

    /**
     * @brief ReadUntil Read up to (and excluding) a terminator character
     * @param[in] terminator terminator
     * @param[in] timeoutMs timeout (ms)
     * @return reply without terminator
     * @throw MountError on timeout
     */
    QByteArray ReadUntil(const char terminator,
                         const int timeoutMs);

    /**
     * @brief ReadCount Read a fixed number of bytes
     * @param[in] count number of bytes
     * @param[in] timeoutMs timeout (ms)
     * @return reply
     * @throw MountError on timeout
     */
    QByteArray ReadCount(const int count,
                         const int timeoutMs);

    void DiscardInput();

    QString Description() const;

    const TransportSettings &settings() const;


private:

    static constexpr int CONNECT_TIMEOUT_MS = 5000;
    static constexpr int WRITE_TIMEOUT_MS = 1000;

    TransportSettings _settings;
    std::unique_ptr<QIODevice> _device;
    QByteArray _buffer;

    /**
     * @brief WaitForData Wait for incoming bytes and append them to the buffer
     * @param[in] timeoutMs timeout (ms)
     * @return true if bytes were received
     */
    bool WaitForData(const int timeoutMs);

    void CheckOpen() const;

};

#endif // MOUNTTRANSPORT_H
