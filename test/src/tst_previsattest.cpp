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
 * >    1er octobre 2023
 *
 */

#include <iostream>
#include <QCoreApplication>
#include <QtTest>
#include "interface/generaltest.h"
#include "interface/informationstest.h"
#include "interface/osculateurstest.h"
#include "librairies/corps/satellite/conditioneclipsetest.h"
#include "librairies/corps/satellite/donneestest.h"
#include "librairies/corps/satellite/elementsosculateurstest.h"
#include "librairies/corps/satellite/evenementstest.h"
#include "librairies/corps/satellite/gpformattest.h"
#include "librairies/corps/satellite/magnitudetest.h"
#include "librairies/corps/satellite/phasagetest.h"
#include "librairies/corps/satellite/satellitetest.h"
#include "librairies/corps/satellite/sgp4test.h"
#include "librairies/corps/satellite/signaltest.h"
#include "librairies/corps/satellite/tletest.h"
#include "librairies/corps/systemesolaire/lunetest.h"
#include "librairies/corps/systemesolaire/planetetest.h"
#include "librairies/corps/systemesolaire/soleiltest.h"
#include "librairies/dates/datetest.h"
#include "librairies/maths/mathstest.h"
#include "librairies/maths/matrice3dtest.h"
#include "librairies/maths/vecteur3dtest.h"
#include "librairies/observateur/observateurtest.h"
#include "librairies/systeme/fichierxmltest.h"
#include "librairies/systeme/logmessagetest.h"
#include "librairies/systeme/telechargementtest.h"
#include "previsions/evenementsorbitauxtest.h"
#include "previsions/flashstest.h"
#include "previsions/previsiontest.h"
#include "previsions/starlinktest.h"
#include "previsions/telescopetest.h"
#include "previsions/transitstest.h"


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
    Vecteur3DTest::testAll();
    Matrice3DTest::testAll();
    DateTest::testAll();

    SoleilTest::testAll();
    PlaneteTest::testAll();
    LuneTest::testAll();
    ObservateurTest::testAll();

    TelechargementTest::testAll();
    FichierXmlTest::testAll();

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
    StarlinkTest::testAll();
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
#if (CLEANUP_TEST)
    qInfo("Nettoyage des fichiers produits");
    QDir di(QDir::current().path() + QDir::separator() + "test");
    di.removeRecursively();
#endif
}

QTEST_MAIN(PreviSatTest)

#include "tst_previsattest.moc"
