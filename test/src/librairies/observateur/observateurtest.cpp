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
 * >    observateurtest.cpp
 *
 * Localisation
 * >    test.librairies.observateur
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

#include <QtTest>
#include "librairies/dates/date.h"
#include "librairies/observateur/observateur.h"
#include "observateurtest.h"
#include "testtools.h"


using namespace TestTools;

Observateur obs("Paris", -2.34864, 48.85339, 30.);


void ObservateurTest::testAll()
{
    testObservateur();
    testCalculPosVit();
    testCalculTempsSideralGreenwich();
    testCalculCap();
    testCalculDistance();
    testCalculIntersectionEllipsoide();
}

void ObservateurTest::testObservateur()
{
    qInfo(Q_FUNC_INFO);

    QCOMPARE(obs.aaer(), 0.11762384079681);
    QCOMPARE(obs.aray(), 0.144556622780765);
    QCOMPARE(obs.nomlieu(), "Paris");
    QCOMPARE(obs.longitude(), -2.34864 * MATHS::DEG2RAD);
    QCOMPARE(obs.latitude(), 48.85339 * MATHS::DEG2RAD);
    QCOMPARE(obs.altitude(), 30. * 1.e-3);

    const Observateur obs2(obs.position(), obs.vitesse(), obs.rotHz(), obs.aaer(), obs.aray());
    QCOMPARE(obs2.aaer(), obs.aaer());
    QCOMPARE(obs2.aray(), obs.aray());
    CompareVecteurs3D(obs2.position(), obs.position());
    CompareVecteurs3D(obs2.vitesse(), obs.vitesse());
    CompareMatrices3D(obs2.rotHz(), obs.rotHz());
}

void ObservateurTest::testCalculPosVit()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2453736.5 - DATE::TJ2000, 0., false);
    obs.CalculPosVit(date);
    QCOMPARE(obs.tempsSideralGreenwich(), 1.7541749818606813);

    const Vecteur3D pos(-935.5234973576179, 4099.350266962309, 4779.867771479536);
    const Vecteur3D vit(-0.29892937078171145, -0.06821945728093355, 0.);
    CompareVecteurs3D(obs.position(), pos);
    CompareVecteurs3D(obs.vitesse(), vit);

    const Vecteur3D vec1(-0.16754305134751404, -0.9749344359089238, -0.14639730743464133);
    const Vecteur3D vec2(0.7341532887298128, -0.2224923497088127, 0.6414952088427875);
    const Vecteur3D vec3(-0.6579880504935971, 0., 0.7530283695901739);
    const Matrice3D mat(vec1, vec2, vec3);
    CompareMatrices3D(obs.rotHz(), mat);
}

void ObservateurTest::testCalculCap()
{
    qInfo(Q_FUNC_INFO);

    const Observateur obs2("Lyon", -4.85, 45.75, 175.);
    QCOMPARE(obs.CalculCap(obs2).first, "SE");
    QCOMPARE(obs.CalculCap(obs2).second, 2.6252070672262);

    const Observateur obs1("Lieu", 0., 90., 0.);
    QCOMPARE(obs1.CalculCap(obs).first, "S");
    QCOMPARE(obs1.CalculCap(obs).second, MATHS::PI);

    QCOMPARE(obs.CalculCap(obs1).first, "N");
    QCOMPARE(obs.CalculCap(obs1).second, 0.);
}

void ObservateurTest::testCalculDistance()
{
    qInfo(Q_FUNC_INFO);

    const Observateur obs2("Lyon", -4.85, 45.75, 175.);
    QCOMPARE(obs.CalculDistance(obs2), 393.42900023395015);
}

void ObservateurTest::testCalculIntersectionEllipsoide()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2019, 6, 22, 20, 41, 16.521, 0.);
    const Vecteur3D pos(-3612.82086791975, -3560.1021783526726, 5101.584296529584);
    const Vecteur3D dir(0.21415669380530017, 0.5068783427737875, -0.8349917700943416);
    const Observateur obsmax = Observateur::CalculIntersectionEllipsoide(date, pos, dir);

    QCOMPARE(obsmax.longitude(), -0.021584028379274756);
    QCOMPARE(obsmax.latitude(), 0.7598665103893715);
}

void ObservateurTest::testCalculTempsSideralGreenwich()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2453736.5 - DATE::TJ2000, 0., false);
    QCOMPARE(Observateur::CalculTempsSideralGreenwich(date), 1.7541749818606813);
}
