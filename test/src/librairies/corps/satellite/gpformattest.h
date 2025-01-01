/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    gpformattest.h
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe GPFormat
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

#ifndef GPFORMATTEST_H
#define GPFORMATTEST_H


class GPFormatTest
{
public:

    static void testAll();

private:

    static void testGPFormat();
    static void testLectureFichier1();
    static void testLectureFichier2();
    static void testLectureFichier3();
    static void testLectureFichier4();
    static void testLectureFichierListeGP1();
    static void testLectureFichierListeGP2();
    static void testLectureFichierListeGP3();
    static void testRecupereNomsat();

};

#endif // GPFORMATTEST_H
