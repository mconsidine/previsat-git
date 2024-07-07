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
 * >    donneestest.cpp
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >    7 juillet 2024
 *
 */

#include <QtTest>
#include "librairies/corps/satellite/donnees.h"
#include "donneestest.h"
#include "testtools.h"


using namespace TestTools;


QSqlDatabase dbSatellites;


void DonneesTest::testAll()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    static_cast<void> (dir.cd(APP_NAME));

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";

    dbSatellites = QSqlDatabase::addDatabase("QSQLITE");
    dbSatellites.setDatabaseName(dirLocalData + QDir::separator() + "satellites.db");
    dbSatellites.open();

    testRequeteCospar();
    testRequeteNom();
    testRequeteNorad();

    const QString nomDb = dbSatellites.connectionName();
    dbSatellites = QSqlDatabase();
    dbSatellites.removeDatabase(nomDb);
}

void DonneesTest::testRequeteCospar()
{
    qInfo(Q_FUNC_INFO);

    const Donnees donneeIss = Donnees::RequeteCospar(dbSatellites, "1998-067A").first();
    CompareDonneesSatellite(donneeIss);
}

void DonneesTest::testRequeteNom()
{
    qInfo(Q_FUNC_INFO);

    const QList<Donnees> listeDonnees = Donnees::RequeteNom(dbSatellites, "ISS");

    Donnees donneeIss;
    QListIterator it(listeDonnees);
    while (it.hasNext()) {

        const Donnees donnee = it.next();
        if (donnee.nom() == "ISS (ZARYA)") {
            donneeIss = donnee;
            it.toBack();
        }
    }

    CompareDonneesSatellite(donneeIss);
}

void DonneesTest::testRequeteNorad()
{
    qInfo(Q_FUNC_INFO);

    const Donnees donneeIss = Donnees::RequeteNorad(dbSatellites, "25544").first();
    CompareDonneesSatellite(donneeIss);
}

void DonneesTest::CompareDonneesSatellite(const Donnees &donneeIss)
{
    QCOMPARE(donneeIss.norad(), "25544");
    QCOMPARE(donneeIss.cospar(), "1998-067A");
    QCOMPARE(donneeIss.dateLancement(), "1998-11-20T06:40:00");
    QCOMPARE(donneeIss.dateRentree(), "");
    QCOMPARE(donneeIss.stsDateRentree(), "");
    QCOMPARE(donneeIss.stsHeureRentree(), "");
    QCOMPARE(donneeIss.masseSec(), "19000");
    QCOMPARE(donneeIss.masseTot(), "20351");
    QCOMPARE(donneeIss.longueur(), 12.6);
    QCOMPARE(donneeIss.diametre(), 4.2);
    QCOMPARE(donneeIss.envergure(), 23.9);
    QCOMPARE(donneeIss.forme(), "Cyl + 2 Pan");
    QCOMPARE(donneeIss.classe(), 'C');
    QCOMPARE(donneeIss.categorie(), "SS");
    QCOMPARE(donneeIss.discipline(), "-");
    QCOMPARE(donneeIss.magnitudeStandard(), -0.43);
    QCOMPARE(donneeIss.methMagnitude(), 'v');
    QCOMPARE(donneeIss.periode(), "92.93");
    QCOMPARE(donneeIss.perigee(), 412);
    QCOMPARE(donneeIss.apogee(), 425);
    QCOMPARE(donneeIss.inclinaison(), "51.64");
    QCOMPARE(donneeIss.orbite(), "LEO/I");
    QCOMPARE(donneeIss.pays(), "ISS");
    QCOMPARE(donneeIss.siteLancement(), "TTMTR");
    QCOMPARE(donneeIss.nom(), "ISS (ZARYA)");
}
