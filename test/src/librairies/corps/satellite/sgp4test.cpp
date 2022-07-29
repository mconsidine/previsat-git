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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/satellite/sgp4.h"
#include "librairies/corps/satellite/tle.h"
#include "sgp4test.h"
#include "test/src/testtools.h"


using namespace TestTools;

void SGP4Test::testAll()
{
    testCalcul();
}

void SGP4Test::testCalcul()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";
    const TLE tle(ligne0, ligne1, ligne2);

    const Date date(2019, 12, 31, 16, 0, 0., 0.);

    SGP4 sgp4;
    sgp4.Calcul(date, tle.elements());

    CompareVecteurs3D(sgp4.position(), Vecteur3D(-3544.428216345714, 4929.712850570476, 3041.79014495648));
    CompareVecteurs3D(sgp4.vitesse(), Vecteur3D(-2.8702757143487916, -5.120469245895756, 4.930085584199513));
}
