/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    tletest.h
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe TLE
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef TLETEST_H
#define TLETEST_H


class TLETest
{
public:

    static void testAll();

private:

    static void testTLE();
    static void testLectureFichier();
    static void testLectureFichier3le();
    static void testVerifieFichier();
    static void testRecupereNomsat();

};

#endif // TLETEST_H
