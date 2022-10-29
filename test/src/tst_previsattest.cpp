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
 * >    tst_previsattest.cpp
 *
 * Localisation
 * >    test
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

#include <iostream>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QCoreApplication>
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "test/src/interface/generaltest.h"
#include "test/src/interface/informationstest.h"
#include "test/src/interface/osculateurstest.h"
#include "test/src/librairies/corps/satellite/conditioneclipsetest.h"
#include "test/src/librairies/corps/satellite/donneestest.h"
#include "test/src/librairies/corps/satellite/elementsosculateurstest.h"
#include "test/src/librairies/corps/satellite/evenementstest.h"
#include "test/src/librairies/corps/satellite/gpformattest.h"
#include "test/src/librairies/corps/satellite/magnitudetest.h"
#include "test/src/librairies/corps/satellite/phasagetest.h"
#include "test/src/librairies/corps/satellite/satellitetest.h"
#include "test/src/librairies/corps/satellite/sgp4test.h"
#include "test/src/librairies/corps/satellite/signaltest.h"
#include "test/src/librairies/corps/satellite/tletest.h"
#include "test/src/librairies/corps/systemesolaire/lunetest.h"
#include "test/src/librairies/corps/systemesolaire/planetetest.h"
#include "test/src/librairies/corps/systemesolaire/soleiltest.h"
#include "test/src/librairies/dates/datetest.h"
#include "test/src/librairies/maths/mathstest.h"
#include "test/src/librairies/observateur/observateurtest.h"
#include "test/src/librairies/systeme/logmessagetest.h"
#include "test/src/librairies/systeme/telechargementtest.h"
#include "test/src/previsions/evenementsorbitauxtest.h"
#include "test/src/previsions/flashstest.h"
#include "test/src/previsions/previsiontest.h"
#include "test/src/previsions/telescopetest.h"
#include "test/src/previsions/transitstest.h"


class PreviSatTest : public QObject
{
    Q_OBJECT

public:
    PreviSatTest();
    ~PreviSatTest();

private slots:
    void testAll();
    void cleanupTestCase();

};

PreviSatTest::PreviSatTest()
{
    qApp->setApplicationName("PreviSat");
}

PreviSatTest::~PreviSatTest()
{

}

void PreviSatTest::testAll()
{
    // Librairies
    MathsTest::testAll();
    DateTest::testAll();
    TelechargementTest::testAll();
    SoleilTest::testAll();
    PlaneteTest::testAll();
    LuneTest::testAll();
    ObservateurTest::testAll();

    DonneesTest::testAll();
    TLETest::testAll();
    GPFormatTest::testAll();
    SignalTest::testAll();
    PhasageTest::testAll();
    SGP4Test::testAll();
    ElementsOsculateursTest::testAll();
    ConditionEclipseTest::testAll();
    MagnitudeTest::testAll();
    SatelliteTest::testAll();
    EvenementsTest::testAll();

    // Previsions
    PrevisionTest::testAll();
    FlashsTest::testAll();
    TransitsTest::testAll();
    EvenementsOrbitauxTest::testAll();
    TelescopeTest::testAll();

    // Informations sur le satellite
    GeneralTest::testAll();
    OsculateursTest::testAll();
    InformationsTest::testAll();

    // Tester en dernier
    LogMessageTest::testAll();
}

void PreviSatTest::cleanupTestCase()
{
#if (QT_VERSION >= 0x050000)
#if (CLEANUP_TEST == true)
    qInfo("Nettoyage des fichiers produits");
    QDir di(QDir::current().path() + QDir::separator() + "test");
    di.removeRecursively();
#endif
#endif
}

QTEST_MAIN(PreviSatTest)

#include "tst_previsattest.moc"
