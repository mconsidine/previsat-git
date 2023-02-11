/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Nom du fichier
 * >    satellitetest.h
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe Satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    18 juin 2019
 *
 * Date de revision
 * >
 *
 */

#ifndef SATELLITETEST_H
#define SATELLITETEST_H


class SatelliteTest
{
public:

    static void testAll();

private:

    static void testCalculPosVit();
    static void testCalculElementsOsculateurs();
    static void testCalculSatelliteEclipse();
    static void testCalculMagnitude();
    static void testCalculAOS();
    static void testCalculCercleAcquisition();
    static void testCalculNoeudOrbite();
    static void testCalculOmbrePenombre();
    static void testCalculPosVitListeSatellites();
    static void testCalculTraceCiel();

};

#endif // SATELLITETEST_H
