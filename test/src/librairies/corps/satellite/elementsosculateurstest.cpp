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
 * >    elementsosculateurstest.cpp
 *
 * Localisation
 * >    test.librairies.corps.satellite
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

#include <QtTest>
#include "elementsosculateurstest.h"
#include "librairies/corps/satellite/elementsosculateurs.h"
#include "librairies/corps/satellite/sgp4.h"
#include "librairies/corps/satellite/tle.h"
#include "testtools.h"


using namespace TestTools;

void ElementsOsculateursTest::testAll()
{
    testCalcul();
}

void ElementsOsculateursTest::testCalcul()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const Date date(2019, 12, 31, 16, 0, 0., 0.);
    const TLE tle(ligne0, ligne1, ligne2);

    SGP4 sgp4;
    sgp4.Calcul(date, tle.elements());

    ElementsOsculateurs elem;
    elem.Calcul(sgp4.position(), sgp4.vitesse());

    QCOMPARE(elem.demiGrandAxe(), 6798.85189976);
    QCOMPARE(elem.excentricite(), 0.0018128223606058153);
    QCOMPARE(elem.inclinaison() * MATHS::RAD2DEG, 51.6517225856);
    QCOMPARE(elem.ascensionDroiteNoeudAscendant() * MATHS::RAD2DEG, 102.366453861676);
    QCOMPARE(elem.argumentPerigee() * MATHS::RAD2DEG, 85.2679709845);
    QCOMPARE(elem.anomalieMoyenne() * MATHS::RAD2DEG, 309.721525136);
    QCOMPARE(elem.anomalieVraie() * MATHS::RAD2DEG, 309.561512947);
    QCOMPARE(elem.anomalieExcentrique() * MATHS::RAD2DEG, 309.641542207352);
    QCOMPARE(elem.apogee(), 6811.17701051);
    QCOMPARE(elem.perigee(), 6786.52678901);
    QCOMPARE(elem.periode(), 1.5465104151524678);
    QCOMPARE(elem.exCirc(), 0.0001495499187907403);
    QCOMPARE(elem.eyCirc(), 0.001806643222360675);
    QCOMPARE(elem.pso() * MATHS::RAD2DEG, 34.98950591815299);
    QCOMPARE(elem.ix(), -0.10364994279345696);
    QCOMPARE(elem.iy(), 0.47274656851982749);
    QCOMPARE(elem.exCEq(), -0.0017967533185599463);
    QCOMPARE(elem.eyCEq(), -0.00024083692274148706);
    QCOMPARE(elem.argumentLongitudeVraie() * MATHS::RAD2DEG, 137.195937793353972);
}
