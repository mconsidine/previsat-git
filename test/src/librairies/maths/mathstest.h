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
 * >    mathstest.h
 *
 * Localisation
 * >    test.librairies.maths
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe Maths
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

#ifndef MATHSTEST_H
#define MATHSTEST_H


class MathsTest
{
public:

    static void testAll();

private:

    static void testFonctionsInline();
    static void testCalculExtremumInterpolation3();
    static void testCalculValeurXInterpolation3();
    static void testInterpolationLagrange();
    static void testToSexagesimal();

};

#endif // MATHSTEST_H
