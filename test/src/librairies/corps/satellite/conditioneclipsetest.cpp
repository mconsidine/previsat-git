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
 * >    elementsosculateurstest.cpp
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
#include "librairies/corps/satellite/conditioneclipse.h"
#include "librairies/corps/satellite/sgp4.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/maths/maths.h"
#include "conditioneclipsetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void ConditionEclipseTest::testAll()
{
    testCalcul();
}

void ConditionEclipseTest::testCalcul()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19365.62109818 -.00000196  00000-0  46169-5 0  9997";
    const QString ligne2 = "2 25544  51.6443 102.5677 0005157  88.3081  52.3104 15.49523379205885";
    const TLE tle(ligne0, ligne1, ligne2);

    // Satellite non eclipse
    Date date(2019, 12, 31, 17, 10, 19., 0.);

    SGP4 sgp4;
    sgp4.Calcul(date, tle.elements());

    Soleil soleil;
    soleil.CalculPosition(date);

    ConditionEclipse cond;
    cond.CalculSatelliteEclipse(sgp4.position(), soleil);

    QCOMPARE(cond.eclipseSoleil().elongation, 1.2372960132682804);
    QCOMPARE(cond.eclipseSoleil().luminosite, 1.);
    QCOMPARE(cond.eclipseSoleil().phi, 1.2119664974413888);
    QCOMPARE(cond.eclipseSoleil().phiSoleil, 0.02479180294354775);
    QCOMPARE(cond.eclipseSoleil().type, TypeEclipse::NON_ECLIPSE);
    QCOMPARE(cond.eclipseTotale(), false);
    QCOMPARE(cond.eclipseAnnulaire(), false);
    QCOMPARE(cond.eclipsePartielle(), false);

    // Satellite eclipse partielle
    date = Date(2019, 12, 31, 17, 10, 20., 0.);
    sgp4.Calcul(date, tle.elements());
    soleil.CalculPosition(date);

    cond.CalculSatelliteEclipse(sgp4.position(), soleil);
    QCOMPARE(cond.eclipseSoleil().elongation, 1.2362158066090845);
    QCOMPARE(arrondi(cond.eclipseSoleil().luminosite, 9), 0.998031093);
    QCOMPARE(cond.eclipseSoleil().phi, 1.2119741808986393);
    QCOMPARE(cond.eclipseSoleil().phiSoleil, 0.02479180272118075);
    QCOMPARE(cond.eclipseSoleil().type, TypeEclipse::ECLIPSE_PARTIELLE);
    QCOMPARE(cond.eclipseTotale(), false);
    QCOMPARE(cond.eclipseAnnulaire(), false);
    QCOMPARE(cond.eclipsePartielle(), true);

    // Satellite eclipse totale
    date = Date(2019, 12, 31, 17, 11, 06., 0.);
    sgp4.Calcul(date, tle.elements());
    soleil.CalculPosition(date);

    cond.CalculSatelliteEclipse(sgp4.position(), soleil);
    QCOMPARE(cond.eclipseSoleil().elongation, 1.1865621461299487);
    QCOMPARE(cond.eclipseSoleil().luminosite, 0.);
    QCOMPARE(cond.eclipseSoleil().phi, 1.2123346939787312);
    QCOMPARE(cond.eclipseSoleil().phiSoleil, 0.024791792594873272);
    QCOMPARE(cond.eclipseSoleil().type, TypeEclipse::ECLIPSE_TOTALE);
    QCOMPARE(cond.eclipseTotale(), true);
    QCOMPARE(cond.eclipseAnnulaire(), false);
    QCOMPARE(cond.eclipsePartielle(), false);

    // Eclipse partielle par la Lune
    const QString lig0 = "DELTA 2 R/B";
    const QString lig1 = "1 25876U 99041E   19364.85374955 -.00000173  00000-0  10259-4 0  9993";
    const QString lig2 = "2 25876  51.7660 259.8304 0530830 209.4912 147.5079 14.04380440 44797";
    const TLE tle2(lig0, lig1, lig2);

    date = Date(2019, 12, 26, 5, 19, 15., 0.);
    sgp4.setInit(false);
    sgp4.Calcul(date, tle2.elements());

    Lune lune;
    lune.CalculPosition(date);
    soleil.CalculPosition(date);

    cond.CalculSatelliteEclipse(sgp4.position(), soleil, lune);
    QCOMPARE(arrondi(cond.eclipseLune().elongation, 9), 0.009338529);
    QCOMPARE(arrondi(cond.eclipseLune().luminosite, 8), 0.99999127);
    QCOMPARE(cond.eclipseLune().phi, 0.004608512352456751);
    QCOMPARE(cond.eclipseLune().phiSoleil, 0.004733071678722244);
    QCOMPARE(cond.eclipseLune().type, TypeEclipse::ECLIPSE_PARTIELLE);
    QCOMPARE(cond.eclipseTotale(), false);
    QCOMPARE(cond.eclipseAnnulaire(), false);
    QCOMPARE(cond.eclipsePartielle(), true);

    // Eclipse annulaire par la Lune
    const QString li0 = "DELTA 2 R/B(1)";
    const QString li1 = "1 20362U 89097B   19364.87075154  .00082239  42899-5  25040-3 0  9991";
    const QString li2 = "2 20362  35.6000 176.6941 0017172  52.6853  58.3225 15.90793923645671";
    const TLE tle3(li0, li1, li2);

    date = Date(2019, 12, 26, 7, 4, 10., 0.);
    sgp4.setInit(false);
    sgp4.Calcul(date, tle3.elements());

    lune.CalculPosition(date);
    soleil.CalculPosition(date);

    cond.CalculSatelliteEclipse(sgp4.position(), soleil, lune);
    QCOMPARE(arrondi(cond.eclipseLune().elongation, 10), 0.0002125044);
    QCOMPARE(cond.eclipseLune().luminosite, 0.08983496455163091);
    QCOMPARE(cond.eclipseLune().phi, 0.004515254625907768);
    QCOMPARE(cond.eclipseLune().phiSoleil, 0.00473284420586589);
    QCOMPARE(cond.eclipseLune().type, TypeEclipse::ECLIPSE_ANNULAIRE);
    QCOMPARE(cond.eclipseTotale(), false);
    QCOMPARE(cond.eclipseAnnulaire(), true);
    QCOMPARE(cond.eclipsePartielle(), false);
}
