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
 * >    synscandriver.h
 *
 * Location
 * >    mount
 *
 * Heritage
 * >    MountDriver
 *
 * Description
 * >    Sky-Watcher SynScan hand controller serial protocol (9600 bd, 8N1)
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef SYNSCANDRIVER_H
#define SYNSCANDRIVER_H

#include "mountdriver.h"


class SynScanDriver : public MountDriver
{
public:

    explicit SynScanDriver(const TransportSettings &settings);

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


    /*
     * Coordinate encoding (public for unit tests)
     */
    /**
     * @brief EncodeAngle Angle as an 8 hex digit fraction of a revolution (precise commands)
     */
    static QByteArray EncodeAngle(const double angle);

    /**
     * @brief DecodeAngle 8 (or 4) hex digit fraction of a revolution, in [0, 2 pi[
     * @throw MountError
     */
    static double DecodeAngle(const QByteArray &hex);


protected:

    QString Handshake() override;


private:

    static constexpr int REPLY_TIMEOUT_MS = 2000;

    QByteArray Query(const QByteArray &command);

};

#endif // SYNSCANDRIVER_H
