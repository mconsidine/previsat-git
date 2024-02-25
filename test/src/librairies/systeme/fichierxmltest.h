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
 * >    fichierxmltest.h
 *
 * Localisation
 * >    test.librairies.systeme
 *
 * Heritage
 * >
 *
 * Description
 * >    Tests de la classe FichierXml
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 fevrier 2024
 *
 * Date de revision
 * >
 *
 */

#ifndef FICHIERXMLTEST_H
#define FICHIERXMLTEST_H

class FichierXmlTest
{
public:

    static void testAll();

private:

    static void testFichierXml();
    static void testOuverture1();
    static void testOuverture2();

};

#endif // FICHIERXMLTEST_H
