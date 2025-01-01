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
 * >
 *
 */

#include <QtTest>
#include "librairies/corps/satellite/signal.h"
#include "signaltest.h"
#include "testtools.h"


using namespace TestTools;

void SignalTest::testAll()
{
    testCalcul();
}

void SignalTest::testCalcul()
{
    qInfo(Q_FUNC_INFO);

    Signal signal;
    signal.Calcul(2.857, 12053.);

    QCOMPARE(signal.attenuation(), 154.0696863531102);
    QCOMPARE(signal.delai(), 40.20448039423327);
    QCOMPARE(signal.doppler(), -952.9926199811205);
}
