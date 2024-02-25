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
#include "matrice3dtest.h"
#include "librairies/maths/matrice3d.h"
#include "testtools.h"


using namespace TestTools;


void Matrice3DTest::testAll()
{
    testMatrice3D();
    testTransposee();
    testOperations();
}

void Matrice3DTest::testMatrice3D()
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

    const Matrice3D mat4(AxeType::AXE_X, MATHS::PI / 6.);
    const Vecteur3D vec4(1., 0., 0.);
    const Vecteur3D vec5(0., sqrt(3.) / 2., -0.5);
    const Vecteur3D vec6(0., 0.5, sqrt(3.) / 2.);
    const Matrice3D mat5(vec4, vec5, vec6);
    CompareMatrices3D(mat4, mat5);

    const Matrice3D mat6(AxeType::AXE_Y, MATHS::PI / 6.);
    const Vecteur3D vec7(sqrt(3.) / 2., 0., 0.5);
    const Vecteur3D vec8(0., 1., 0.);
    const Vecteur3D vec9(-0.5, 0., sqrt(3.) / 2.);
    const Matrice3D mat7(vec7, vec8, vec9);
    CompareMatrices3D(mat6, mat7);

    const Matrice3D mat8(AxeType::AXE_Z, MATHS::PI / 6.);
    const Vecteur3D vec10(sqrt(3.) / 2., -0.5, 0.);
    const Vecteur3D vec11(0.5, sqrt(3.) / 2., 0.);
    const Vecteur3D vec12(0., 0., 1.);
    const Matrice3D mat9(vec10, vec11, vec12);
    CompareMatrices3D(mat8, mat9);

    const Matrice3D mat8b(static_cast<AxeType> (4), MATHS::PI / 6.);
    const Vecteur3D vec12b(0., 0., 0.);
    const Matrice3D mat9b(vec12b, vec12b, vec12b);
    CompareMatrices3D(mat8b, mat9b);
}

void Matrice3DTest::testTransposee()
{
    qInfo(Q_FUNC_INFO);

    const Vecteur3D vec1(1., 4., 7.);
    const Vecteur3D vec2(2., 5., 8.);
    const Vecteur3D vec3(3., 6., 9.);
    const Matrice3D mat1(vec1, vec2, vec3);

    const Vecteur3D vec4(1., 2., 3.);
    const Vecteur3D vec5(4., 5., 6.);
    const Vecteur3D vec6(7., 8., 9.);
    const Matrice3D mat2(vec4, vec5, vec6);

    CompareMatrices3D(mat2.Transposee(), mat1);
}

void Matrice3DTest::testOperations()
{
    qInfo(Q_FUNC_INFO);

    const Vecteur3D vec1(1., 2., 3.);
    const Vecteur3D vec2(1., 4., 7.);
    const Vecteur3D vec3(2., 5., 8.);
    const Vecteur3D vec4(3., 6., 9.);
    const Matrice3D mat1(vec2, vec3, vec4);
    const Vecteur3D vec5(14., 32., 50.);
    CompareVecteurs3D(mat1 * vec1, vec5);

    const Vecteur3D vec6(1., 2., 3.);
    const Vecteur3D vec7(4., 5., 6.);
    const Vecteur3D vec8(7., 8., 9.);
    const Matrice3D mat2(vec6, vec7, vec8);

    const Vecteur3D vec10(14., 32, 50.);
    const Vecteur3D vec11(32., 77., 122.);
    const Vecteur3D vec12(50., 122., 194.);
    const Matrice3D mat3(vec10, vec11, vec12);
    CompareMatrices3D(mat1 * mat2, mat3);
}
