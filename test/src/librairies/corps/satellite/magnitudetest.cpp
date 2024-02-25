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
#include "librairies/corps/satellite/conditioneclipse.h"
#include "librairies/corps/satellite/magnitude.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"
#include "magnitudetest.h"
#include "testtools.h"


using namespace TestTools;

void MagnitudeTest::testAll()
{
    testCalcul();
}

void MagnitudeTest::testCalcul()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19365.62109818 -.00000196  00000-0  46169-5 0  9997";
    const QString ligne2 = "2 25544  51.6443 102.5677 0005157  88.3081  52.3104 15.49523379205885";
    const TLE tle(ligne0, ligne1, ligne2);

    // Satellite non eclipse
    Date date(2020, 1, 1, 6, 11, 0., 0.);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);

    SGP4 sgp4;
    sgp4.Calcul(date, tle.elements());

    Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    obs.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPositionSimp(date);

    ConditionEclipse cond;
    cond.CalculSatelliteEclipse(sgp4.position(), soleil);

    sat.CalculCoordHoriz(obs, true);

    const double magnitudeStandard = -0.5;
    Magnitude magn;
    magn.Calcul(cond, obs, sat.distance(), sat.hauteur(), magnitudeStandard);

    QCOMPARE(magn.magnitude(), -1.778414273382328);
    QCOMPARE(magn.fractionIlluminee(), 0.6315377954832837);
}
