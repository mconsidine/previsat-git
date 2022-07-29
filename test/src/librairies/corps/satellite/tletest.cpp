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
 * >    tletest.cpp
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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/previsatexception.h"
#include "tletest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void TLETest::testAll()
{
    testTLE();
    testLectureFichier();
    testLectureFichier3le();
    testVerifieFichier();
    testRecupereNomsat();
}

void TLETest::testTLE()
{
    qInfo(Q_FUNC_INFO);

    const QString ligne0 = "ISS (ZARYA)";
    const QString ligne1 = "1 25544U 98067A   19364.90553304 -.00000636  00000-0 -32355-5 0  9992";
    const QString ligne2 = "2 25544  51.6448 106.1119 0005093  86.4732  19.8785 15.49521149205776";

    const TLE tle(ligne0, ligne1, ligne2);

    // Premiere ligne
    QCOMPARE(tle.elements().norad, "25544");
    QCOMPARE(tle.elements().cospar, "1998-067A");
    CompareDates(tle.elements().epoque, Date(2019, 12, 30, 21, 43, 58.054655972, 0.));
    QCOMPARE(tle.elements().ndt20, -0.00000636);
    QCOMPARE(tle.elements().ndd60, 0.);
    QCOMPARE(tle.elements().bstar, -0.32355e-5);

    // Deuxieme ligne
    QCOMPARE(tle.elements().inclo, 51.6448);
    QCOMPARE(tle.elements().omegao, 106.1119);
    QCOMPARE(tle.elements().ecco, 0.0005093);
    QCOMPARE(tle.elements().argpo, 86.4732);
    QCOMPARE(tle.elements().mo, 19.8785);
    QCOMPARE(tle.elements().no, 15.49521149);
    QCOMPARE(tle.elements().nbOrbitesEpoque, static_cast<unsigned int> (20577));
}

void TLETest::testLectureFichier()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";
    const QMap<QString, ElementsOrbitaux> mapTLE = TLE::LectureFichier(fic, QString(), 0);

    QCOMPARE(mapTLE.keys().size(), 163);
}

void TLETest::testLectureFichier3le()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "iss.3le";
    const QList<ElementsOrbitaux> listTLE = TLE::LectureFichier3le(fic);

    QCOMPARE(listTLE.size(), 60);
}

void TLETest::testVerifieFichier()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    int nb;
    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual-nok%1.txt";

    for(int i=0; i<=13; i++) {

        nb = -1;
        const QString nomfic = fic.arg(i);
        try {
            nb = TLE::VerifieFichier(nomfic, false);
        } catch (PreviSatException &e) {
        }

        QCOMPARE(nb, 0);
    }
}

void TLETest::testRecupereNomsat()
{
    qInfo(Q_FUNC_INFO);

    QCOMPARE(TLE::RecupereNomsat("1234567890123456789012345.6"), "123456789012345");
    QCOMPARE(TLE::RecupereNomsat("0 ISS"), "ISS");
    QCOMPARE(TLE::RecupereNomsat("1 25544"), "25544");
    QCOMPARE(TLE::RecupereNomsat("ISS (ZARYA)"), "ISS");
    QCOMPARE(TLE::RecupereNomsat("IRIDIUM 64 [+]"), "IRIDIUM 64");
}
