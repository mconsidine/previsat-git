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

#include <QtTest>
#include "librairies/exceptions/exception.h"
#include "librairies/maths/maths.h"
#include "mathstest.h"
#include "testtools.h"

using namespace TestTools;


void MathsTest::testAll()
{
    testFonctionsInline();
    testCalculExtremumInterpolation3();
    testCalculValeurXInterpolation3();
    testInterpolationLagrange();
    testToSexagesimal();
}

void MathsTest::testFonctionsInline()
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
}

void MathsTest::testCalculExtremumInterpolation3()
{
    qInfo(Q_FUNC_INFO);

    // Extremum par interpolation
    const std::array<QPointF, MATHS::DEGRE_INTERPOLATION> tab = {
        QPointF { 12., 1.3814294 },
        QPointF { 16., 1.3812213 },
        QPointF { 20., 1.3812453 }
    };

    const QPointF extremum(17.5863851788, 1.38120304665537);
    QCOMPARE(Maths::CalculExtremumInterpolation3(tab), extremum);
}

void MathsTest::testCalculValeurXInterpolation3()
{
    qInfo(Q_FUNC_INFO);

    // Determination abscisse par interpolation
    const std::array<QPointF, MATHS::DEGRE_INTERPOLATION> tab = {
        QPointF { 26., -1693.4 },
        QPointF { 27., 406.3 },
        QPointF { 28., 2303.2 }
    };

    QCOMPARE(Maths::CalculValeurXInterpolation3(tab, 0., MATHS::EPSDBL), 26.798732704968522);
}

void MathsTest::testInterpolationLagrange()
{
    qInfo(Q_FUNC_INFO);

    // Calcul d'interpolation par polynome de Lagrange
    const QVector<QPointF> table = {
        { 29.43, 0.4913598528 },
        { 30.97, 0.5145891926 },
        { 27.69, 0.4646875083 },
        { 28.11, 0.4711658342 },
        { 31.58, 0.5236885653 },
        { 33.05, 0.5453707057 }
    };

    CompareDoublesSeuil(Maths::InterpolationLagrange(table, 30.), 0.5, MATHS::EPSDBL100);
}

void MathsTest::testToSexagesimal()
{
    qInfo(Q_FUNC_INFO);

    // Affichage en sexagesimal
    QCOMPARE(Maths::ToSexagesimal(12.3456789, AngleFormatType::NO_TYPE, 2, 4, true, true), R"(+12° 20' 44.4440")");
    QCOMPARE(Maths::ToSexagesimal(-0.123456789, AngleFormatType::DEGRE, 2, 4, false, false), R"(-07°04'24.7907")");
    QCOMPARE(Maths::ToSexagesimal(1.23456789, AngleFormatType::HEURE1, 2, 0, false, false), " 04h42m57s");
    QCOMPARE(Maths::ToSexagesimal(0.26179211559393, AngleFormatType::HEURE1, 2, 0, false, false), " 01h00m00s");
    QCOMPARE(Maths::ToSexagesimal(12.3456789, AngleFormatType::NO_TYPE, -2, -1, true, true), R"(+12° 20' 44")");
    QCOMPARE(Maths::ToSexagesimal(1.23456789, AngleFormatType::HEURE1, 3, 0, false, false), " 04h42m57s");
}
