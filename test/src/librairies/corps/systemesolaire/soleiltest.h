/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    soleiltest.h
 *
 * Localisation
 * >    test.librairies.corps.systemesolaire
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe Soleil
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

#ifndef SOLEILTEST_H
#define SOLEILTEST_H


class SoleilTest
{
public:

    static void testAll();

private:

    static void testSoleil();
    static void testCalculPosition();
    static void testCalculLeverMeridienCoucher();

};

#endif // SOLEILTEST_H
