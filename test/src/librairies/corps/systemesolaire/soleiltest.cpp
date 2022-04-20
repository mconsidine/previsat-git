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
 * >    soleiltest.cpp
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
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/systemesolaire/soleil.h"
#include "soleiltest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void SoleilTest::testAll()
{
    testCalculPosition();
}

void SoleilTest::testCalculPosition()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 10, 12, 23, 59, 0.816, 0.);
    Soleil soleil;
    soleil.CalculPosition(date);
    const Vecteur3D pos(-140328287.9615894, -46628772.91250957, -20216954.727520175);
    CompareVecteurs3D(soleil.position(), pos);
    QCOMPARE(soleil.distanceUA(), pos.Norme() * KM2UA);
}
