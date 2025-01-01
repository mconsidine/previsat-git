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
 * >    vecteur3dtest.cpp
 *
 * Localisation
 * >    test.librairies.maths
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
#include "vecteur3dtest.h"
#include "librairies/maths/vecteur3d.h"
#include "testtools.h"


using namespace TestTools;


void Vecteur3DTest::testAll()
{
    testVecteur3D();
    testOperations();
    testAngle();
    testNormalise();
    testNorme();
    testRotation();
}

void Vecteur3DTest::testVecteur3D()
{
    qInfo(Q_FUNC_INFO);

    // Constructeurs
    const Vecteur3D vec0;
    QCOMPARE(vec0.x(), 0.);
    QCOMPARE(vec0.y(), 0.);
    QCOMPARE(vec0.z(), 0.);

    const Vecteur3D vec1(1., 2., 3.);
    QCOMPARE(vec1.x(), 1.);
    QCOMPARE(vec1.y(), 2.);
    QCOMPARE(vec1.z(), 3.);

    const Vecteur3D vec2 = vec1;
    CompareVecteurs3D(vec2, vec1);
}

void Vecteur3DTest::testOperations()
{
    qInfo(Q_FUNC_INFO);

    // Operations
    // Addition de 2 vecteurs
    const Vecteur3D vec1(1., 2., 3.);
    const Vecteur3D vec2(4., 5., 6.);
    const Vecteur3D vec3(5., 7., 9.);
    CompareVecteurs3D(vec1 + vec2, vec3);

    // Vecteur oppose
    const Vecteur3D vec4(-1., -2., -3.);
    CompareVecteurs3D(-vec1, vec4);

    // Soustraction de 2 vecteurs
    CompareVecteurs3D(vec3 - vec2, vec1);

    // Produit avec un scalaire
    const Vecteur3D vec5(-2., -4., -6.);
    CompareVecteurs3D(vec1 * (-2.), vec5);

    // Produit scalaire
    const Vecteur3D vec6(7., -3., 2.);
    QCOMPARE(vec1 * vec6, 7.);

    // Produit vectoriel
    const Vecteur3D vec7(-6., 4., -5.);
    const Vecteur3D vec8(-22., -13., 16.);
    CompareVecteurs3D(vec1 ^ vec7, vec8);
}

void Vecteur3DTest::testAngle()
{
    qInfo(Q_FUNC_INFO);

    // Angle
    const Vecteur3D vec1(1., 2., 3.);
    const Vecteur3D vec2(-6., 4., -5.);

    QCOMPARE(vec1.Angle(vec2), 1.977892169366);
}

void Vecteur3DTest::testNorme()
{
    qInfo(Q_FUNC_INFO);

    // Norme
    const Vecteur3D vec(1., 2., 3.);
    QCOMPARE(vec.Norme(), sqrt(14.));
}

void Vecteur3DTest::testNormalise()
{
    qInfo(Q_FUNC_INFO);

    // Normalise
    const Vecteur3D vec1(1., 2., 3.);
    const Vecteur3D vec2 = vec1 * (1. / sqrt(14.));
    CompareVecteurs3D(vec1.Normalise(), vec2);
}

void Vecteur3DTest::testRotation()
{
    qInfo(Q_FUNC_INFO);

    // Rotation
    const double ang = 30. * MATHS::DEG2RAD;
    const Vecteur3D vec1(1., 2., 3.);

    Vecteur3D vec2 = Vecteur3D(1., 3.232050807568, 1.598076211353);
    CompareVecteurs3D(vec1.Rotation(AxeType::AXE_X, ang), vec2);

    vec2 = Vecteur3D(-0.633974596215, 2., 3.098076211353);
    CompareVecteurs3D(vec1.Rotation(AxeType::AXE_Y, ang), vec2);

    vec2 = Vecteur3D(1.8660254037844, 1.232050807568, 3.);
    CompareVecteurs3D(vec1.Rotation(AxeType::AXE_Z, ang), vec2);

    vec2 = Vecteur3D();
    CompareVecteurs3D(vec1.Rotation(static_cast<AxeType> (4), ang), vec2);
}
