/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    lunetest.cpp
 *
 * Localisation
 * >    test.librairies.corps.systemesolaire
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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "lunetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void LuneTest::testAll()
{
    testCalculPosition();
    testCalculPhase();
    testCalculMagnitude();
}

void LuneTest::testCalculPosition()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Lune lune;
    lune.CalculPosition(date);
    const Vecteur3D pos(-251640.85931597583, 254391.46471626704, 87678.7116029949);
    CompareVecteurs3D(lune.position(), pos);
}

void LuneTest::testCalculPhase()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Soleil soleil;
    soleil.CalculPosition(date);
    Lune lune;
    lune.CalculPosition(date);
    lune.CalculPhase(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.6785704098427325);
}

void LuneTest::testCalculMagnitude()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Soleil soleil;
    soleil.CalculPosition(date);
    Lune lune;
    lune.CalculPosition(date);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.magnitude(), -10.543566245403385);
}
