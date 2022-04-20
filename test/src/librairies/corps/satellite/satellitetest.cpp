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
 * >    satellitetest.cpp
 *
 * Localisation
 * >    test.librairies.corps.satellite
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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "satellitetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

const Donnees donnees("025544 1998-067A    30.0 20.0  0.0 -0.5 v 399.00 1998/11/20                 92.90     411     421  51.64 LEO/I  ISS   TTMTR ISS (ZARYA)");


void SatelliteTest::testAll()
{
    testCalculPosVit();
    testCalculElementsOsculateurs();
    testCalculSatelliteEclipse();
    testCalculMagnitude();
    testCalculAOS();
    testCalculNoeudOrbite();
    testCalculOmbrePenombre();
}

void SatelliteTest::testCalculPosVit()
{
    qInfo(Q_FUNC_INFO);

    const QString fmt = "%1 %2 %3 %4 %5 %6 %7";

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "tle" + QDir::separator() + "sgp4.txt";
    QMap<QString, TLE> mapTLE = TLE::LectureFichier(fic);

    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "sgp4_res.txt";
    QFileInfo ff(ficRes);

    QDir di(ff.path());
    if (!di.exists()) {
        di.mkpath(ff.path());
    }

    QFile res(ficRes);
    res.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&res);

    QMapIterator<QString, TLE> it(mapTLE);
    while (it.hasNext()) {
        it.next();

        const TLE tle = it.value();
        Satellite sat(tle);

        const QString li0 = tle.ligne0();
        const double deb = li0.mid(18, 10).toDouble();
        const double fin = li0.mid(30, 10).toDouble();
        const double pas = li0.mid(45, 6).toDouble();
        double t = deb;

        flux << it.key() << endl;

        double temp = (tle.epoque().jourJulienUTC() * NB_MIN_PAR_JOUR + deb) * NB_JOUR_PAR_MIN;
        Date date(temp, 0.);
        do {

            sat.CalculPosVit(date);

            flux << fmt.arg(t, 17, 'f', 8).arg(sat.position().x(), 16, 'f', 8).arg(sat.position().y(), 16, 'f', 8).arg(sat.position().z(), 16, 'f', 8)
                    .arg(sat.vitesse().x(), 12, 'f', 9).arg(sat.vitesse().y(), 12, 'f', 9).arg(sat.vitesse().z(), 12, 'f', 9) << endl;

            t += pas;
            temp = (date.jourJulienUTC() * NB_MIN_PAR_JOUR + pas) * NB_JOUR_PAR_MIN;
            date = Date(temp, 0.);

        } while (date.jourJulienUTC() <= tle.epoque().jourJulienUTC() + fin * NB_JOUR_PAR_MIN + NB_JOUR_PAR_MIN);
    }
    res.close();

    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "sgp4_ref.txt";
    CompareFichiers(ficRes, ficRef);
}

void SatelliteTest::testCalculElementsOsculateurs()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const Date date(2019, 12, 31, 16, 0, 0., 0.);
    TLE tle(ligne0, ligne1, ligne2);
    tle._donnees = donnees;
    Satellite sat(tle);
    sat.CalculPosVit(date);
    sat.CalculElementsOsculateurs(date);

    QCOMPARE(sat.elements().demiGrandAxe(), 6798.85189976);
    QCOMPARE(sat.elements().excentricite(), 0.0018128223606058153);
    QCOMPARE(sat.elements().inclinaison() * RAD2DEG, 51.6517225856);
    QCOMPARE(sat.elements().ascensionDroiteNoeudAscendant() * RAD2DEG, 102.366453861676);
    QCOMPARE(sat.elements().argumentPerigee() * RAD2DEG, 85.2679709845);
    QCOMPARE(sat.elements().anomalieMoyenne() * RAD2DEG, 309.721525136);
    QCOMPARE(sat.elements().anomalieVraie() * RAD2DEG, 309.561512947);
    QCOMPARE(sat.elements().anomalieExcentrique() * RAD2DEG, 309.641542207352);
    QCOMPARE(sat.elements().apogee(), 6811.17701051);
    QCOMPARE(sat.elements().perigee(), 6786.52678901);
    QCOMPARE(sat.elements().periode(), 1.5465104151524678);
    QCOMPARE(sat.elements().exCirc(), 0.0001495499187902825);
    QCOMPARE(sat.elements().eyCirc(), 0.001806643222360675);
    QCOMPARE(sat.elements().pso() * RAD2DEG, 34.98950591815299);
    QCOMPARE(sat.elements().ix(), -0.10364994279345696);
    QCOMPARE(sat.elements().iy(), 0.47274656851982749);
    QCOMPARE(sat.elements().exCEq(), -0.0017967533185599463);
    QCOMPARE(sat.elements().eyCEq(), -0.0002408369227418477);
    QCOMPARE(sat.elements().argumentLongitudeVraie() * RAD2DEG, 137.195937793353972);
    QCOMPARE(sat.ageTLE(), 0.761133626667);
    QCOMPARE(sat.nbOrbites(), static_cast<unsigned int> (120589));
}

void SatelliteTest::testCalculSatelliteEclipse()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19365.62109818 -.00000196  00000-0  46169-5 0  9997";
    const QString ligne2 = "2 25544  51.6443 102.5677 0005157  88.3081  52.3104 15.49523379205885";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle);

    // Satellite non eclipse
    Date date(2019, 12, 31, 17, 10, 19., 0.);
    sat.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPosition(date);

    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().elongation, 1.2372960132682804);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().luminosite, 1.);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().phi, 1.2119664974413888);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().phiSoleil, 0.02479180294354775);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().type, NON_ECLIPSE);
    QCOMPARE(sat.conditionEclipse().eclipseTotale(), false);
    QCOMPARE(sat.conditionEclipse().eclipseAnnulaire(), false);
    QCOMPARE(sat.conditionEclipse().eclipsePartielle(), false);

    // Satellite eclipse partielle
    date = Date(2019, 12, 31, 17, 10, 20., 0.);
    sat.CalculPosVit(date);

    soleil.CalculPosition(date);

    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().elongation, 1.2362158066090845);
    QCOMPARE(arrondi(sat.conditionEclipse().eclipseSoleil().luminosite, 9), 0.998031093);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().phi, 1.2119741808986393);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().phiSoleil, 0.02479180272118075);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().type, ECLIPSE_PARTIELLE);
    QCOMPARE(sat.conditionEclipse().eclipseTotale(), false);
    QCOMPARE(sat.conditionEclipse().eclipseAnnulaire(), false);
    QCOMPARE(sat.conditionEclipse().eclipsePartielle(), true);

    // Satellite eclipse totale
    date = Date(2019, 12, 31, 17, 11, 06., 0.);
    sat.CalculPosVit(date);

    soleil.CalculPosition(date);

    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().elongation, 1.1865621461299487);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().luminosite, 0.);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().phi, 1.2123346939787312);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().phiSoleil, 0.024791792594873272);
    QCOMPARE(sat.conditionEclipse().eclipseSoleil().type, ECLIPSE_TOTALE);
    QCOMPARE(sat.conditionEclipse().eclipseTotale(), true);
    QCOMPARE(sat.conditionEclipse().eclipseAnnulaire(), false);
    QCOMPARE(sat.conditionEclipse().eclipsePartielle(), false);

    // Eclipse partielle par la Lune
    const QString lig0 = "DELTA 2 R/B";
    const QString lig1 = "1 25876U 99041E   19364.85374955 -.00000173  00000-0  10259-4 0  9993";
    const QString lig2 = "2 25876  51.7660 259.8304 0530830 209.4912 147.5079 14.04380440 44797";
    const TLE tle2(lig0, lig1, lig2);
    Satellite sat2(tle2);
    date = Date(2019, 12, 26, 5, 19, 15., 0.);
    sat2.CalculPosVit(date);

    Lune lune;
    lune.CalculPosition(date);
    soleil.CalculPosition(date);

    sat2._conditionEclipse.CalculSatelliteEclipse(sat2.position(), soleil, lune);
    QCOMPARE(arrondi(sat2.conditionEclipse().eclipseLune().elongation, 9), 0.009338529);
    QCOMPARE(arrondi(sat2.conditionEclipse().eclipseLune().luminosite, 8), 0.99999258);
    QCOMPARE(sat2.conditionEclipse().eclipseLune().phi, 0.004608512352456751);
    QCOMPARE(sat2.conditionEclipse().eclipseLune().phiSoleil, 0.004733071678722244);
    QCOMPARE(sat2.conditionEclipse().eclipseLune().type, ECLIPSE_PARTIELLE);
    QCOMPARE(sat2.conditionEclipse().eclipseTotale(), false);
    QCOMPARE(sat2.conditionEclipse().eclipseAnnulaire(), false);
    QCOMPARE(sat2.conditionEclipse().eclipsePartielle(), true);

    // Eclipse annulaire par la Lune
    const QString li0 = "DELTA 2 R/B(1)";
    const QString li1 = "1 20362U 89097B   19364.87075154  .00082239  42899-5  25040-3 0  9991";
    const QString li2 = "2 20362  35.6000 176.6941 0017172  52.6853  58.3225 15.90793923645671";
    const TLE tle3(li0, li1, li2);
    Satellite sat3(tle3);
    date = Date(2019, 12, 26, 7, 4, 10., 0.);
    sat3.CalculPosVit(date);

    lune.CalculPosition(date);
    soleil.CalculPosition(date);

    sat3._conditionEclipse.CalculSatelliteEclipse(sat3.position(), soleil, lune);
    QCOMPARE(sat3.conditionEclipse().eclipseLune().elongation, 0.00021250440118772637);
    QCOMPARE(sat3.conditionEclipse().eclipseLune().luminosite, 0.08983496455163091);
    QCOMPARE(sat3.conditionEclipse().eclipseLune().phi, 0.004515254625907768);
    QCOMPARE(sat3.conditionEclipse().eclipseLune().phiSoleil, 0.00473284420586589);
    QCOMPARE(sat3.conditionEclipse().eclipseLune().type, ECLIPSE_ANNULAIRE);
    QCOMPARE(sat3.conditionEclipse().eclipseTotale(), false);
    QCOMPARE(sat3.conditionEclipse().eclipseAnnulaire(), true);
    QCOMPARE(sat3.conditionEclipse().eclipsePartielle(), false);
}

void SatelliteTest::testCalculMagnitude()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19365.62109818 -.00000196  00000-0  46169-5 0  9997";
    const QString ligne2 = "2 25544  51.6443 102.5677 0005157  88.3081  52.3104 15.49523379205885";
    TLE tle(ligne0, ligne1, ligne2);
    tle._donnees = donnees;
    Satellite sat(tle);

    // Satellite non eclipse
    Date date(2020, 1, 1, 6, 11, 0., 0.);
    sat.CalculPosVit(date);

    Observateur obs("Paris", -002.348640000, +48.853390000, 30);
    obs.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPosition(date);

    sat.CalculCoordHoriz(obs);
    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil);
    sat._magnitude.Calcul(sat.conditionEclipse(), obs, sat.distance(), sat.hauteur(), sat.tle().donnees().magnitudeStandard());
    QCOMPARE(sat.magnitude().magnitude(), -1.778414273396659);
    QCOMPARE(sat.magnitude().fractionIlluminee(), 0.6315377954832837);
}

void SatelliteTest::testCalculAOS()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle);

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    sat.CalculPosVit(date);

    Observateur obs("Paris", -002.348640000, +48.853390000, 30);
    obs.CalculPosVit(date);

    sat.CalculCoordHoriz(obs);

    // AOS suivant
    const ElementsAOS elements1 = Evenements::CalculAOS(date, sat, obs, true);
    QCOMPARE(elements1.date.jourJulienUTC(), 7315.3887150086275);
    QCOMPARE(elements1.typeAOS, QObject::tr("AOS", "Acquisition of signal"));
    QCOMPARE(elements1.aos, true);
    QCOMPARE(elements1.azimut, 3.2095957919723137);

    // AOS precedent
    date = Date(2020, 1, 11, 21, 20, 0., 0.);
    sat.CalculPosVit(date);
    obs.CalculPosVit(date);
    sat.CalculCoordHoriz(obs);

    const ElementsAOS elements2 = Evenements::CalculAOS(date, sat, obs, false);
    QCOMPARE(elements2.date.jourJulienUTC(), 7315.3887150086275);
    QCOMPARE(elements2.typeAOS, QObject::tr("AOS", "Acquisition of signal"));
    QCOMPARE(elements2.aos, true);
    QCOMPARE(elements2.azimut, 3.209596657064488);

    // LOS suivant
    const ElementsAOS elements3 = Evenements::CalculAOS(date, sat, obs, true);
    QCOMPARE(elements3.date.jourJulienUTC(), 7315.39473843222);
    QCOMPARE(elements3.typeAOS, QObject::tr("LOS", "Loss of signal"));
    QCOMPARE(elements3.aos, true);
    QCOMPARE(elements3.azimut, 1.4139271914129354);

    // LOS precedent
    date = Date(2020, 1, 11, 21, 40, 0., 0.);
    sat.CalculPosVit(date);
    obs.CalculPosVit(date);
    sat.CalculCoordHoriz(obs);

    const ElementsAOS elements4 = Evenements::CalculAOS(date, sat, obs, false);
    QCOMPARE(elements3.date.jourJulienUTC(), 7315.39473843222);
    QCOMPARE(elements3.typeAOS, QObject::tr("LOS", "Loss of signal"));
    QCOMPARE(elements3.aos, true);
    QCOMPARE(elements3.azimut, 1.4139271914129354);
}

void SatelliteTest::testCalculNoeudOrbite()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle);

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    sat.CalculPosVit(date);
    sat.CalculCoordTerrestres(date);

    // Noeud ascendant suivant
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat).jourJulienUTC(), 7314.866214644489);

    // Noeud descendant suivant
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat, true, false).jourJulienUTC(), 7314.898407218954);

    // Noeud ascendant precedent
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat, false).jourJulienUTC(), 7314.801722417209);

    // Noeud descendant precedent
    QCOMPARE(Evenements::CalculNoeudOrbite(date, sat, false, false).jourJulienUTC(), 7314.833914946533);
}

void SatelliteTest::testCalculOmbrePenombre()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle);

    Date date(2020, 1, 11, 8, 27, 30., 0.);
    sat.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPosition(date);

    Lune lune;
    lune.CalculPosition(date);
    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, lune, true);

    // Prochain passage penombre->ombre
    QCOMPARE(Evenements::CalculOmbrePenombre(date, sat, 2, true, true).jourJulienUTC(), 7314.86554398148);
}
