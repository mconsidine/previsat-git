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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/satellite/donnees.h"
#include "donneestest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void DonneesTest::testAll()
{
    testDonnees();
}

void DonneesTest::testDonnees()
{
    qInfo(Q_FUNC_INFO);

    const QString donnees = "025544 1998-067A    30.0 20.0  0.0 -0.5 v 399.00 1998-11-20                 92.90     412     421  51.64 LEO/I  ISS   " \
            "TTMTR ISS (ZARYA)";

    const Donnees donneeIss(donnees);

    QCOMPARE(donneeIss.cospar(), "1998-067A");
    QCOMPARE(donneeIss.t1(), 30.);
    QCOMPARE(donneeIss.t2(), 20.);
    QCOMPARE(donneeIss.t3(), 0.);
    QCOMPARE(donneeIss.magnitudeStandard(), -0.5);
    QCOMPARE(donneeIss.methMagnitude(), 'v');
    QCOMPARE(donneeIss.section(), 399.);
    QCOMPARE(donneeIss.dateLancement(), "1998-11-20");
    QCOMPARE(donneeIss.periode(), "92.90");
    QCOMPARE(donneeIss.perigee(), "412");
    QCOMPARE(donneeIss.apogee(), "421");
    QCOMPARE(donneeIss.inclinaison(), "51.64");
    QCOMPARE(donneeIss.categorieOrbite(), "LEO/I");
    QCOMPARE(donneeIss.pays(), "ISS");
    QCOMPARE(donneeIss.siteLancement(), "TTMTR");
}
