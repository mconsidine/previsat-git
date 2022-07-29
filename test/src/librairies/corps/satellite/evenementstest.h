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
 * >    evenementstest.h
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe Evenements
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef EVENEMENTSTEST_H
#define EVENEMENTSTEST_H


class EvenementsTest
{
public:

    static void testAll();

private:

    static void testCalculAOS();
    static void testCalculNoeudOrbite();
    static void testCalculOmbrePenombre();

};

#endif // EVENEMENTSTEST_H
