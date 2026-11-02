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
 * >    mounttest.h
 *
 * Location
 * >    test/mount
 *
 * Description
 * >    Tests of the mount control layer: protocol formatting, simulator, and a simulated pass
 *      run faster than real time. Self-contained: run alone with "PreviSatTest testAllMount".
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef MOUNTTEST_H
#define MOUNTTEST_H


class MountTest
{
public:

    static void testAll();

private:

    static void testLx200Formatting();
    static void testLx200Parsing();
    static void testSynScanEncoding();
    static void testPointing();
    static void testSimulator();
    static void testSimulatedPass();

};

#endif // MOUNTTEST_H
