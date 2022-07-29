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
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/observateur/observateur.h"
#include "satellitetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void SatelliteTest::testAll()
{
    testCalculPosVit();
    testCalculElementsOsculateurs();
    testHasAos1();
    testHasAos2();
    testIsGeo1();
    testIsGeo2();
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

        temp = (elem.epoque.jourJulienUTC() * NB_MIN_PAR_JOUR + deb) * NB_JOUR_PAR_MIN;
        date = Date(temp, 0.);
        do {

            sat.CalculPosVit(date);

            flux << fmt.arg(t, 17, 'f', 8).arg(sat.position().x(), 16, 'f', 8).arg(sat.position().y(), 16, 'f', 8).arg(sat.position().z(), 16, 'f', 8)
                    .arg(sat.vitesse().x(), 12, 'f', 9).arg(sat.vitesse().y(), 12, 'f', 9).arg(sat.vitesse().z(), 12, 'f', 9) << Qt::endl;

            t += pas;
            temp = (date.jourJulienUTC() * NB_MIN_PAR_JOUR + pas) * NB_JOUR_PAR_MIN;
            date = Date(temp, 0.);

        } while (date.jourJulienUTC() <= elem.epoque.jourJulienUTC() + fin * NB_JOUR_PAR_MIN + NB_JOUR_PAR_MIN);
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

    const QString donnees = "025544 1998-067A    30.0 20.0  0.0 -0.5 v 399.00 1998-11-20                 92.90     412     421  51.64 LEO/I  ISS   " \
            "TTMTR ISS (ZARYA)";

    tle._elements.donnees = Donnees(donnees);

    Satellite sat(tle.elements());
    sat.CalculPosVit(date);
    sat.CalculElementsOsculateurs(date);

    QCOMPARE(sat.ageElementsOrbitaux(), 0.761133626667);
    QCOMPARE(sat.nbOrbites(), static_cast<unsigned int> (120589));
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
