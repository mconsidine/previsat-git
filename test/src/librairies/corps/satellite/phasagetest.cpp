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
 * >    phasagetest.cpp
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
#include "librairies/corps/corpsconst.h"
#include "librairies/corps/satellite/elementsosculateurs.h"
#include "librairies/corps/satellite/phasage.h"
#include "phasagetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void PhasageTest::testAll()
{
    testCalcul1();
    testCalcul2();
}

void PhasageTest::testCalcul1()
{
    qInfo(Q_FUNC_INFO);

    const double n0 = 15.49876730;
    const Vecteur3D position(-1325.792, -5033.380, -4370.977);
    const Vecteur3D vitesse(6.734057, 1.205983, -3.437162);

    ElementsOsculateurs elem;
    elem.Calcul(position, vitesse);

    Phasage phasage;
    phasage.Calcul(elem, n0);

    QCOMPARE(phasage.ct0(), 2);
    QCOMPARE(phasage.dt0(), 1);
    QCOMPARE(phasage.nbOrb(), 31);
    QCOMPARE(phasage.nu0(), 15);
}

void PhasageTest::testCalcul2()
{
    qInfo(Q_FUNC_INFO);

    const double n0 = 1.;
    const Vecteur3D position;
    const Vecteur3D vitesse;

    ElementsOsculateurs elem;
    elem.Calcul(position, vitesse);

    Phasage phasage;
    phasage.Calcul(elem, n0);

    QCOMPARE(phasage.ct0(), CORPS::ELEMENT_PHASAGE_INDEFINI);
    QCOMPARE(phasage.dt0(), CORPS::ELEMENT_PHASAGE_INDEFINI);
    QCOMPARE(phasage.nbOrb(), CORPS::ELEMENT_PHASAGE_INDEFINI);
    QCOMPARE(phasage.nu0(), CORPS::ELEMENT_PHASAGE_INDEFINI);
}
