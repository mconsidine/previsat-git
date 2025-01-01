/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    7 juillet 2024
 *
 */

#include <QtTest>
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/exception.h"
#include "tletest.h"
#include "testtools.h"


using namespace TestTools;

static QDir dir;

void TLETest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    static_cast<void> (dir.cd(APP_NAME));

    testTLE();
    testLectureFichier1();
    testLectureFichier2();
    testMiseAJourFichier();
    testVerifieFichier();
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
    QCOMPARE(tle.elements().nbOrbitesEpoque, 20577u);
}

void TLETest::testLectureFichier1()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";
    const QMap<QString, ElementsOrbitaux> mapTLE = TLE::Lecture(fic);

    QCOMPARE(mapTLE.keys().size(), 163);

    // TLE a 2 lignes
    const QString fic2 = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual2.txt";
    const QMap<QString, ElementsOrbitaux> mapTLE2 = TLE::Lecture(fic2);

    QCOMPARE(mapTLE2.keys().size(), 163);
}

void TLETest::testLectureFichier2()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual3.txt";
    QVERIFY_THROWS_EXCEPTION(Exception, TLE::Lecture(fic));
}

void TLETest::testMiseAJourFichier()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";
    const QString ficnew = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "visual.txt";

    QFile fi(fic);
    const QString ficold = QDir::currentPath() + QDir::separator() + "test" + QDir::separator() + QFileInfo(fic).fileName();
    fi.copy(ficold);
    const QStringList compteRendu = TLE::MiseAJourFichier(ficold, ficnew, QSqlDatabase(), 1);

    QCOMPARE(compteRendu.first(), "visual.txt");
    QCOMPARE(compteRendu.at(1), "157");
    QCOMPARE(compteRendu.at(2), "165");
    QCOMPARE(compteRendu.at(3), "8");
    QCOMPARE(compteRendu.at(4), "6");
}

void TLETest::testVerifieFichier()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual-nok%1.txt";

    for(int i=1; i<=13; i++) {

        const QString nomfic = fic.arg(i);
        const int nb = TLE::VerifieFichier(nomfic, false);
        QCOMPARE(nb, 0);
    }

    // TLE a 2 lignes
    const QString fic2 = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual2.txt";
    QCOMPARE(TLE::VerifieFichier(fic2), 163);
}
