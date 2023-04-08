/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    22 mai 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "satellitetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void SatelliteTest::testAll()
{
    testCalculBeta();
    testCalculCercleAcquisition();
    testCalculCoordHoriz2();
    testCalculPosVit1();
    testCalculPosVit2();
    testCalculPosVitECEF();
    testCalculElementsOsculateurs();
    testCalculTraceCiel();
    testCalculPosVitListeSatellites();
    testHasAos1();
    testHasAos2();
    testIsGeo1();
    testIsGeo2();
}

void SatelliteTest::testCalculBeta()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const Date date(2019, 12, 31, 16, 0, 0., 0.);
    TLE tle(ligne0, ligne1, ligne2);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPosition(date);

    sat.CalculBeta(soleil);

    QCOMPARE(arrondi(sat.beta(), 12), -0.269748743468);
}

void SatelliteTest::testCalculCercleAcquisition()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const Date date(2019, 12, 31, 16, 0, 0., 0.);
    TLE tle(ligne0, ligne1, ligne2);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);

    const Observateur station("Paris", -2.348640000, +48.853390000, 30);
    sat.CalculCercleAcquisition(station);

    QCOMPARE(sat.zone().size(), 360u);
}

void SatelliteTest::testCalculCoordHoriz2()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle.elements());

    Observateur obs("Paris", -2.348640000, +48.853390000, 30);

    Date date(2020, 1, 11, 21, 20, 0., 0.);
    obs.CalculPosVit(date);

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true, false);
    sat.CalculCoordEquat(obs, false);
    sat.CalculCoordHoriz2(obs);

    QCOMPARE(sat.hauteur(), 0.010451579950689613);
    QCOMPARE(sat.azimut(), 3.1795612051615314);

    date = Date(2020, 1, 11, 21, 28, 30., 0.);
    obs.CalculPosVit(date);

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true, false);
    sat.CalculCoordEquat(obs, false);
    sat.CalculCoordHoriz2(obs);

    QCOMPARE(sat.hauteur(), -0.013773458990370531);
}

void SatelliteTest::testCalculPosVit1()
{
    qInfo(Q_FUNC_INFO);

    const QString fmt = "%1 %2 %3 %4 %5 %6 %7";

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "sgp4.txt";
    const QMap<QString, ElementsOrbitaux> mapElem = TLE::LectureFichier(fic, QString(), 0);

    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "sgp4_res.txt";
    QFileInfo ff(ficRes);

    QDir di(ff.path());
    if (!di.exists()) {
        di.mkpath(ff.path());
    }

    QFile res(ficRes);
    res.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&res);

    Satellite sat;
    double t;
    double temp;
    Date date;

    QMapIterator it(mapElem);
    while (it.hasNext()) {
        it.next();

        const ElementsOrbitaux elem = it.value();
        sat = Satellite(elem);

        const QString li0 = TLE::_mapTLE[it.key()]._ligne0;
        const double deb = li0.mid(18, 10).toDouble();
        const double fin = li0.mid(30, 10).toDouble();
        const double pas = li0.mid(45, 6).toDouble();
        t = deb;

        flux << it.key() << Qt::endl;

        temp = (elem.epoque.jourJulienUTC() * DATE::NB_MIN_PAR_JOUR + deb) * DATE::NB_JOUR_PAR_MIN;
        date = Date(temp, 0.);
        do {

            sat.CalculPosVit(date);

            flux << fmt.arg(t, 17, 'f', 8).arg(sat.position().x(), 16, 'f', 8).arg(sat.position().y(), 16, 'f', 8).arg(sat.position().z(), 16, 'f', 8)
                    .arg(sat.vitesse().x(), 12, 'f', 9).arg(sat.vitesse().y(), 12, 'f', 9).arg(sat.vitesse().z(), 12, 'f', 9) << Qt::endl;

            t += pas;
            temp = (date.jourJulienUTC() * DATE::NB_MIN_PAR_JOUR + pas) * DATE::NB_JOUR_PAR_MIN;
            date = Date(temp, 0.);

        } while (date.jourJulienUTC() <= elem.epoque.jourJulienUTC() + fin * DATE::NB_JOUR_PAR_MIN + DATE::NB_JOUR_PAR_MIN);
    }
    res.close();

    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "sgp4_ref.txt";
    CompareFichiers(ficRes, ficRef);
}

void SatelliteTest::testCalculPosVit2()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->LectureDonneesSatellites();

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "iss.gp";
    const QList<ElementsOrbitaux> listeElem = GPFormat::LectureFichierListeGP(fic, Configuration::instance()->donneesSatellites(),
                                                                              Configuration::instance()->lgRec());

    Satellite sat(listeElem);
    QCOMPARE(sat._listElements.size(), listeElem.size());

    const Date date(2023, 2, 2, 0, 0, 0., 0.);
    sat.CalculPosVit(date);
    const Vecteur3D pos(-73.24427091902145, 6704.632372584346, 1106.632182287279);
    const Vecteur3D vit(-4.827118447150993, 0.9222014946152153, -5.8768841331293284);
    CompareVecteurs3D(sat.position(), pos);
    CompareVecteurs3D(sat.vitesse(), vit);
}

void SatelliteTest::testCalculPosVitECEF()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   20011.25516102  .00000195  00000-0  11565-4 0  9993";
    const QString ligne2 = "2 25544  51.6455  49.9243 0005192 121.3003 338.9406 15.49550468207533";
    const TLE tle(ligne0, ligne1, ligne2);
    Satellite sat(tle.elements());

    Date date(2020, 1, 11, 21, 20, 0., 0.);
    sat.CalculPosVit(date);

    Vecteur3D posEcef;
    Vecteur3D vitEcef;
    sat.CalculPosVitECEF(date, posEcef, vitEcef);

    const Vecteur3D posRef(5962.088811390397, 155.9202497577353, 3250.407052726898);
    const Vecteur3D vitRef(-2.7343740085535795, 4.9106707363469955, 4.7562253836562896);
    CompareVecteurs3D(posEcef, posRef);
    CompareVecteurs3D(vitEcef, vitRef);
}

void SatelliteTest::testCalculElementsOsculateurs()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->LectureDonneesSatellites();

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";
    const QMap<QString, ElementsOrbitaux> mapTLE = TLE::LectureFichier(fic, Configuration::instance()->donneesSatellites(),
                                                                       Configuration::instance()->lgRec());

    Date date(2020, 8, 16, 0, 0, 0., 0.);

    Satellite sat(mapTLE["25544"]);
    sat.CalculPosVit(date);
    sat.CalculElementsOsculateurs(date);

    QCOMPARE(sat.ageElementsOrbitaux(), 0.6277587499998845);
    QCOMPARE(sat.nbOrbites(), 124128u);
    QCOMPARE(sat.method(), 'n');
    QCOMPARE(sat.elementsOsculateurs().demiGrandAxe(), 6800.37693729);
}

void SatelliteTest::testCalculTraceCiel()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19365.62109818 -.00000196  00000-0  46169-5 0  9997";
    const QString ligne2 = "2 25544  51.6443 102.5677 0005157  88.3081  52.3104 15.49523379205885";
    const TLE tle(ligne0, ligne1, ligne2);

    Date date(2020, 1, 1, 6, 11, 0., 0.);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);

    const Observateur obs("Paris", -2.348640000, +48.853390000, 30);

    sat.CalculTraceCiel(date, true, true, obs);

    QCOMPARE_GT(sat.traceCiel().size(), 0);
}

void SatelliteTest::testCalculPosVitListeSatellites()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->LectureDonneesSatellites();

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";
    const QMap<QString, ElementsOrbitaux> mapTLE = TLE::LectureFichier(fic, Configuration::instance()->donneesSatellites(),
                                                                       Configuration::instance()->lgRec());

    Date date(2020, 8, 15, 13, 42, 0., 0.);
    Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    obs.CalculPosVit(date);

    Soleil soleil;
    soleil.CalculPosition(date);

    Lune lune;
    lune.CalculPosition(date);

    QList<Satellite> satellites;
    satellites.append(Satellite(mapTLE["25544"]));

    Satellite::CalculPosVitListeSatellites(date, obs, soleil, lune, 3, true, true, true, true, true, true, true, true, satellites);

    QCOMPARE(satellites.first().phasage().nu0(), 15);
    QCOMPARE(satellites.first().signal().attenuation(), 137.4342178796233);
    QCOMPARE(satellites.first().altitude(), 424.52109699485754);
    QCOMPARE(satellites.first().rangeRate(), -6.853740793330628);
    QCOMPARE(satellites.first().magnitude().magnitude(), 4.801106983557947);
    QCOMPARE(satellites.first().ascensionDroite(), 1.2459691983080656);
    QCOMPARE(satellites.first().declinaison(), 0.308777949256643);
    QCOMPARE(satellites.first().constellation(), "Tau");
    QCOMPARE(satellites.first().longitude(), 0.36338638004248525);
    CompareVecteurs3D(satellites.first().dist(), Vecteur3D(542.7538686361972, 1603.1191934262467, 536.5097144664396));
}

void SatelliteTest::testHasAos1()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const Date date(2019, 12, 31, 16, 0, 0., 0.);
    TLE tle(ligne0, ligne1, ligne2);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);
    sat.CalculElementsOsculateurs(date);

    const Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    QCOMPARE(sat.hasAOS(obs), true);
}

void SatelliteTest::testHasAos2()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544   1.6448 106.1119 0005093  86.4732  19.8785 15.49521149205771";

    const Date date(2019, 12, 31, 16, 0, 0., 0.);
    TLE tle(ligne0, ligne1, ligne2);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);
    sat.CalculElementsOsculateurs(date);

    const Observateur obs("Blah", -2.348640000, +88.853390000, 30);
    QCOMPARE(sat.hasAOS(obs), false);
}

void SatelliteTest::testIsGeo1()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const TLE tle(ligne0, ligne1, ligne2);

    Satellite sat(tle.elements());
    QCOMPARE(sat.isGeo(), false);
}

void SatelliteTest::testIsGeo2()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "TDRS 3";
    const QString ligne1 = "1 19548U 88091B   22159.38296255 -.00000293  00000+0  00000+0 0  9995";
    const QString ligne2 = "2 19548  13.7222 351.3902 0034946 334.0729  20.1739  1.00279041110641";

    const TLE tle(ligne0, ligne1, ligne2);

    Satellite sat(tle.elements());
    QCOMPARE(sat.isGeo(), true);
}
