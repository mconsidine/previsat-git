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
 * >    sgp4test.cpp
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    8 juin 2022
 *
 * Date de revision
 * >
 *
 */

#include <QtTest>
#include "librairies/corps/satellite/sgp4.h"
#include "sgp4test.h"
#include "testtools.h"


using namespace TestTools;

void SGP4Test::testAll()
{
    testCalcul();
}

void SGP4Test::testCalcul()
{
    qInfo(Q_FUNC_INFO);

    ElementsOrbitaux elem;
    elem.argpo = 86.4732;
    elem.bstar = -0.00000323550000;
    elem.ecco = 0.0005093;
    elem.epoque = Date::ConversionDateIso("2019-12-30T21:43:58.054656");
    elem.inclo = 51.6448;
    elem.mo = 19.8785;
    elem.no = 15.49521149;
    elem.omegao = 106.1119;

    const Date date(2019, 12, 31, 16, 0, 0., 0.);

    SGP4 sgp4;
    sgp4.Calcul(date, elem);

    CompareVecteurs3D(sgp4.position(), Vecteur3D(-3544.428216345714, 4929.712850570476, 3041.79014495648));
    CompareVecteurs3D(sgp4.vitesse(), Vecteur3D(-2.8702757143487916, -5.120469245895756, 4.930085584199513));
}
