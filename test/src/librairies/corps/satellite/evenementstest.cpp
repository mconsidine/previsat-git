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
 * >    8 juin 2022
 *
 * Date de revision
 * >
 *
 */

#include <QtTest>
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "evenementstest.h"
#include "testtools.h"


using namespace TestTools;

void EvenementsTest::testAll()
{
    testCalculAOS();
    testCalculNoeudOrbite();
    testCalculOmbrePenombre();
}

void EvenementsTest::testCalculAOS()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle.elements());

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    sat.CalculPosVit(date);

    Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    obs.CalculPosVit(date);

    sat.CalculCoordHoriz(obs, true);

    // AOS suivant
    const ElementsAOS elements1 = Evenements::CalculAOS(date, sat, obs, SensCalcul::CHRONOLOGIQUE);
    QCOMPARE(arrondi(elements1.date.jourJulienUTC(), 8), 7315.38871499);
    QCOMPARE(elements1.typeAOS, QObject::tr("AOS", "Acquisition of signal"));
    QCOMPARE(elements1.aos, true);
    QCOMPARE(arrondi(elements1.azimut, 8), 3.20959864);

    // AOS precedent
    date = Date(2020, 1, 11, 21, 20, 0., 0.);
    sat.CalculPosVit(date);
    obs.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);

    const ElementsAOS elements2 = Evenements::CalculAOS(date, sat, obs, SensCalcul::ANTI_CHRONOLOGIQUE);
    QCOMPARE(arrondi(elements2.date.jourJulienUTC(), 8), 7315.38871499);
    QCOMPARE(elements2.typeAOS, QObject::tr("AOS", "Acquisition of signal"));
    QCOMPARE(elements2.aos, true);
    QCOMPARE(arrondi(elements2.azimut, 8), 3.20959864);

    // LOS suivant
    const ElementsAOS elements3 = Evenements::CalculAOS(date, sat, obs, SensCalcul::CHRONOLOGIQUE);
    QCOMPARE(arrondi(elements3.date.jourJulienUTC(), 8), 7315.39473843);
    QCOMPARE(elements3.typeAOS, QObject::tr("LOS", "Loss of signal"));
    QCOMPARE(elements3.aos, true);
    QCOMPARE(arrondi(elements3.azimut, 8), 1.41392820);

    // LOS precedent
    date = Date(2020, 1, 11, 21, 40, 0., 0.);
    sat.CalculPosVit(date);
    obs.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);

    const ElementsAOS elements4 = Evenements::CalculAOS(date, sat, obs, SensCalcul::ANTI_CHRONOLOGIQUE);
    QCOMPARE(arrondi(elements4.date.jourJulienUTC(), 8), 7315.39473843);
    QCOMPARE(elements4.typeAOS, QObject::tr("LOS", "Loss of signal"));
    QCOMPARE(elements4.aos, true);
    QCOMPARE(arrondi(elements4.azimut, 8), 1.41392820);
}

void EvenementsTest::testCalculNoeudOrbite()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle.elements());

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    sat.CalculPosVit(date);
    sat.CalculCoordTerrestres(date);

    // Noeud ascendant suivant
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat).jourJulienUTC(), 7314.866214644489);

    // Noeud descendant suivant
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat, SensCalcul::CHRONOLOGIQUE, TypeNoeudOrbite::NOEUD_DESCENDANT).jourJulienUTC(), 7314.898407218954);

    // Noeud ascendant precedent
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat, SensCalcul::ANTI_CHRONOLOGIQUE).jourJulienUTC(), 7314.801722417209);

    // Noeud descendant precedent
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat, SensCalcul::ANTI_CHRONOLOGIQUE, TypeNoeudOrbite::NOEUD_DESCENDANT).jourJulienUTC(),
             7314.833914946533);
}

void EvenementsTest::testCalculOmbrePenombre()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle.elements());

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    sat.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPositionSimp(date);

    Lune lune;
    lune.CalculPositionSimp(date);
    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, &lune, true);

    // Prochain passage penombre->ombre
    QCOMPARE(Evenements::CalculOmbrePenombre(date, sat, 2, true, true).jourJulienUTC(), 7314.86554398148);
}
