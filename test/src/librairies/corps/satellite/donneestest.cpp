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
 * >
 *
 */

#include <QtTest>
#include "librairies/corps/satellite/donnees.h"
#include "donneestest.h"
#include "testtools.h"


using namespace TestTools;

void DonneesTest::testAll()
{
    testDonnees();
}

void DonneesTest::testDonnees()
{
    qInfo(Q_FUNC_INFO);

    const QString donnees = "025544 1998-067A   1998-11-20T06:40:00                           19000     20351    12.6    4.2     23.9 " \
        "Cyl + 2 Pan                      C SS   -                -0.43 v      92.94     416     421  51.64 LEO/I  ISS   TTMTR ISS (ZARYA)";

    const Donnees donneeIss(donnees);

    QCOMPARE(donneeIss.norad(), "025544");
    QCOMPARE(donneeIss.cospar(), "1998-067A");
    QCOMPARE(donneeIss.dateLancement(), "1998-11-20T06:40:00");
    QCOMPARE(donneeIss.dateRentree(), "");
    QCOMPARE(donneeIss.stsDateRentree(), ' ');
    QCOMPARE(donneeIss.stsHeureRentree(), ' ');
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
    QCOMPARE(donneeIss.periode(), "92.94");
    QCOMPARE(donneeIss.perigee(), 416);
    QCOMPARE(donneeIss.apogee(), 421);
    QCOMPARE(donneeIss.inclinaison(), "51.64");
    QCOMPARE(donneeIss.orbite(), "LEO/I");
    QCOMPARE(donneeIss.pays(), "ISS");
    QCOMPARE(donneeIss.siteLancement(), "TTMTR");
    QCOMPARE(donneeIss.nom(), "ISS (ZARYA)");
}
