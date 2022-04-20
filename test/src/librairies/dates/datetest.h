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
 * >    datetest.h
 *
 * Localisation
 * >    test.librairies.dates
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe Date
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

#ifndef DATETEST_H
#define DATETEST_H


class DateTest
{
public:

    static void testAll();

private:

    static void testDates();
    static void testCalculOffsetUTC();
    static void testConversionDateNasa();
    static void testToQDateTime();
    static void testToShortDate();
    static void testToShortDateAMJ();
    static void testToShortDateAMJmillisec();
    static void testToLongDate();

};

#endif // DATETEST_H
