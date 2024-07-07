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
 * >    gpformattest.cpp
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
#include "librairies/exceptions/exception.h"
#include "librairies/corps/satellite/gpformat.h"
#include "gpformattest.h"
#include "testtools.h"


using namespace TestTools;

static QDir dir;

void GPFormatTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    static_cast<void> (dir.cd(APP_NAME));

    testGPFormat();
    testLectureFichier1();
    testLectureFichier2();
    testLectureFichier3();
    testLectureFichier4();
    testLectureFichierListeGP1();
    testLectureFichierListeGP2();
    testLectureFichierListeGP3();
    testRecupereNomsat();
}

void GPFormatTest::testGPFormat()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.xml";
    const QMap<QString, ElementsOrbitaux> mapElem = GPFormat::Lecture(fic, QSqlDatabase(),  QStringList(), true, false);

    QCOMPARE(mapElem["025544"].cospar, "1998-067A");
}

void GPFormatTest::testLectureFichier1()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.xml";
    const QMap<QString, ElementsOrbitaux> mapElem = GPFormat::Lecture(fic, QSqlDatabase(),  QStringList(), true, false);

    QCOMPARE(mapElem.keys().size(), 160);
}

void GPFormatTest::testLectureFichier2()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual2.xml";
    QMap<QString, ElementsOrbitaux> mapElem;

    QVERIFY_THROWS_EXCEPTION(Exception, mapElem = GPFormat::Lecture(fic));
    QCOMPARE(mapElem.size(), 0);
}

void GPFormatTest::testLectureFichier3()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual-nok.xml";
    QMap<QString, ElementsOrbitaux> mapElem;

    QVERIFY_THROWS_EXCEPTION(Exception, mapElem = GPFormat::Lecture(fic));
    QCOMPARE(mapElem.size(), 0);
}

void GPFormatTest::testLectureFichier4()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "data" + QDir::separator() + "taiutc.dat";
    QMap<QString, ElementsOrbitaux> mapElem;

    QVERIFY_THROWS_EXCEPTION(Exception, mapElem = GPFormat::Lecture(fic));
    QCOMPARE(mapElem.size(), 0);
}

void GPFormatTest::testLectureFichierListeGP1()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "iss.gp";
    const QList<ElementsOrbitaux> listeElem = GPFormat::LectureListeGP(fic);

    QCOMPARE(listeElem.size(), 60);
}

void GPFormatTest::testLectureFichierListeGP2()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "iss2.gp";
    QList<ElementsOrbitaux> listeElem;

    QVERIFY_THROWS_EXCEPTION(Exception, listeElem = GPFormat::LectureListeGP(fic, QSqlDatabase(), true));
    QCOMPARE(listeElem.size(), 0);
}

void GPFormatTest::testLectureFichierListeGP3()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "data" + QDir::separator() + "taiutc.dat";
    QList<ElementsOrbitaux> listeElem;

    QVERIFY_THROWS_EXCEPTION(Exception, listeElem = GPFormat::LectureListeGP(fic, QSqlDatabase(), true));
    QCOMPARE(listeElem.size(), 0);
}

void GPFormatTest::testRecupereNomsat()
{
    qInfo(Q_FUNC_INFO);

    QCOMPARE(GPFormat::RecupereNomsat("1234567890123456789012345.6"), "123456789012345");
    QCOMPARE(GPFormat::RecupereNomsat("0 ISS"), "ISS");
    QCOMPARE(GPFormat::RecupereNomsat("1 25544"), "25544");
    QCOMPARE(GPFormat::RecupereNomsat("ISS (ZARYA)"), "ISS");
    QCOMPARE(GPFormat::RecupereNomsat("IRIDIUM 64 [+]"), "IRIDIUM 64");
}
