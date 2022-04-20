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
 * >    mathstest.cpp
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

#include <QPair>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/maths/maths.h"
#include "librairies/maths/matrice3d.h"
#include "librairies/maths/vecteur3d.h"
#include "mathstest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void MathsTest::testAll()
{
    testMaths();
    testVecteur3D();
    testMatrice3D();
}

void MathsTest::testMaths()
{
    qInfo(Q_FUNC_INFO);

    // Signe
    QCOMPARE(sgn(5.), 1);
    QCOMPARE(sgn(-5.), -1);

    // Arrondi
    QCOMPARE(arrondi(37.49, 0), 37.);
    QCOMPARE(arrondi(37.5, 0), 38.);
    QCOMPARE(arrondi(37.99, 0), 38.);
    QCOMPARE(arrondi(-37.49, 0), -37.);
    QCOMPARE(arrondi(-37.5, 0), -38.);
    QCOMPARE(arrondi(-37.99, 0), -38.);
    QCOMPARE(arrondi(37.123456789, 5), 37.12346);
    QCOMPARE(arrondi(-37.123456789, 5), -37.12346);

    // Modulo
    QCOMPARE(modulo(450., 360.), 90.);
    QCOMPARE(modulo(-450., 360.), 270.);

    // Extremum par interpolation
    const QList<double> xtab1(QList<double> () << 12. << 16. << 20.);
    const QList<double> ytab1(QList<double> () << 1.3814294 << 1.3812213 << 1.3812453);
    const QPair<double, double> extremum(17.5863851788, 1.38120304665537);
    QPair<double, double> val;

    val = Maths::CalculExtremumInterpolation3(xtab1, ytab1);
    QCOMPARE(val.first, extremum.first);
    QCOMPARE(val.second, extremum.second);

    // Determination abscisse par interpolation
    const QList<double> xtab2(QList<double> () << 26. << 27. << 28.);
    const QList<double> ytab2(QList<double> () << -1693.4 << 406.3 << 2303.2);
    QCOMPARE(Maths::CalculValeurXInterpolation3(xtab2, ytab2, 0., EPSDBL), 26.798732704968522);

    // Affichage en sexagesimal
    QCOMPARE(Maths::ToSexagesimal(12.3456789, NO_TYPE, 2, 4, true, true), "+12° 20' 44.4440\"");
    QCOMPARE(Maths::ToSexagesimal(-0.123456789, DEGRE, 2, 4, false, false), "-07°04'24.7907\"");
    QCOMPARE(Maths::ToSexagesimal(1.23456789, HEURE1, 2, 0, false, false), " 04h42m57s");
}

void MathsTest::testMatrice3D()
{
    qInfo(Q_FUNC_INFO);

    const Matrice3D mat0;
    const Vecteur3D vec0;
    const Matrice3D mat1(vec0, vec0, vec0);
    CompareMatrices3D(mat0, mat1);

    const Vecteur3D vec1(1., 2., 3.);
    const Vecteur3D vec2(4., 5., 6.);
    const Vecteur3D vec3(7., 8., 9.);
    const Matrice3D mat2(vec1, vec2, vec3);
    const Matrice3D mat3 = mat2;
    CompareMatrices3D(mat2, mat3);

    const Matrice3D mat4(AXE_X, PI / 6.);
    const Vecteur3D vec4(1., 0., 0.);
    const Vecteur3D vec5(0., sqrt(3.) / 2., -0.5);
    const Vecteur3D vec6(0., 0.5, sqrt(3.) / 2.);
    const Matrice3D mat5(vec4, vec5, vec6);
    CompareMatrices3D(mat4, mat5);

    const Vecteur3D vec7(1., 4., 7.);
    const Vecteur3D vec8(2., 5., 8.);
    const Vecteur3D vec9(3., 6., 9.);
    const Matrice3D mat6(vec7, vec8, vec9);
    CompareMatrices3D(mat2.Transposee(), mat6);

    const Vecteur3D vec10(14., 32., 50.);
    CompareVecteurs3D(mat6 * vec1, vec10);

    const Vecteur3D vec11(14., 32, 50.);
    const Vecteur3D vec12(32., 77., 122.);
    const Vecteur3D vec13(50., 122., 194.);
    const Matrice3D mat7(vec11, vec12, vec13);
    CompareMatrices3D(mat6 * mat2, mat7);
}

void MathsTest::testVecteur3D()
{
    qInfo(Q_FUNC_INFO);

    // Constructeurs
    const Vecteur3D vec0;
    QCOMPARE(vec0.x(), 0.);
    QCOMPARE(vec0.y(), 0.);
    QCOMPARE(vec0.z(), 0.);

    Vecteur3D vec1(1., 2., 3.);
    QCOMPARE(vec1.x(), 1.);
    QCOMPARE(vec1.y(), 2.);
    QCOMPARE(vec1.z(), 3.);

    Vecteur3D vec2 = vec1;
    CompareVecteurs3D(vec2, vec1);

    // Operations
    // Addition de 2 vecteurs
    vec2 = Vecteur3D(4., 5., 6.);
    const Vecteur3D vec3(5., 7., 9.);
    CompareVecteurs3D(vec1 + vec2, vec3);

    // Vecteur oppose
    const Vecteur3D vec4(-1., -2., -3.);
    CompareVecteurs3D(-vec1, vec4);

    // Soustraction de 2 vecteurs
    CompareVecteurs3D(vec3 - vec2, vec1);

    // Produit avec un scalaire
    Vecteur3D vec5(-2., -4., -6.);
    CompareVecteurs3D(vec1 * (-2.), vec5);

    // Produit scalaire
    Vecteur3D vec6(7., -3., 2.);
    QCOMPARE(vec1 * vec6, 7.);

    // Produit vectoriel
    const Vecteur3D vec7(-6., 4., -5.);
    const Vecteur3D vec8(-22., -13., 16.);
    CompareVecteurs3D(vec1 ^ vec7, vec8);

    // Angle
    QCOMPARE(vec1.Angle(vec7), 1.9778921693662815374593799604662);

    // Norme
    QCOMPARE(vec1.Norme(), sqrt(14.));

    // Normalise
    const Vecteur3D vec9 = vec1 * (1. / sqrt(14.));
    CompareVecteurs3D(vec1.Normalise(), vec9);

    // Nul
    QCOMPARE(vec0.Nul(), true);
    QCOMPARE(vec1.Nul(), false);

    // Rotation
    double ang = 30. * DEG2RAD;
    vec2 = Vecteur3D(1., 3.2320508075688772935274463415059, 1.5980762113533159402911695122588);
    CompareVecteurs3D(vec1.Rotation(AXE_X, ang), vec2);

    vec2 = Vecteur3D(-0.63397459621556135323627682924706, 2., 3.0980762113533159402911695122588);
    CompareVecteurs3D(vec1.Rotation(AXE_Y, ang), vec2);

    vec2 = Vecteur3D(1.8660254037844386467637231707529, 1.2320508075688772935274463415059, 3.);
    CompareVecteurs3D(vec1.Rotation(AXE_Z, ang), vec2);
}
