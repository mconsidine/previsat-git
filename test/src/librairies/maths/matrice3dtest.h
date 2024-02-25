/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    vecteur3dtest.h
 *
 * Localisation
 * >    test.librairies.maths
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe Vecteur3D
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

#ifndef MATRICE3DTEST_H
#define MATRICE3DTEST_H


class Matrice3DTest
{
public:

    static void testAll();

private:

    static void testMatrice3D();
    static void testTransposee();
    static void testOperations();

};

#endif // MATRICE3DTEST_H
