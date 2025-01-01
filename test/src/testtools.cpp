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
 * >    testtools.cpp
 *
 * Localisation
 * >    test
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
#include "librairies/maths/matrice3d.h"
#include "librairies/maths/vecteur3d.h"
#include "testtools.h"


namespace TestTools {

void CompareDoublesSeuil(const double actual, const double expected, const double threshold)
{
    QCOMPARE_LE(actual - expected, threshold);
}

void CompareDates(const Date &date1, const Date &date2)
{
    QCOMPARE(date1.annee(), date2.annee());
    QCOMPARE(date1.mois(), date2.mois());
    QCOMPARE(date1.jour(), date2.jour());
    QCOMPARE(date1.heure(), date2.heure());
    QCOMPARE(date1.minutes(), date2.minutes());
    QCOMPARE(fabs(date1.secondes() - date2.secondes()) < DATE::EPS_DATES, true);
    QCOMPARE(date1.offsetUTC(), date2.offsetUTC());
    QCOMPARE(date1.jourJulienUTC(), date2.jourJulienUTC());
    QCOMPARE(date1.jourJulienTT(), date2.jourJulienTT());
    QCOMPARE(date1.jourJulien(), date2.jourJulien());
}

void CompareMatrices3D(const Matrice3D &mat1, const Matrice3D &mat2)
{
    CompareVecteurs3D(mat1.vecteur1(), mat2.vecteur1());
    CompareVecteurs3D(mat1.vecteur2(), mat2.vecteur2());
    CompareVecteurs3D(mat1.vecteur3(), mat2.vecteur3());
}


void CompareVecteurs3D(const Vecteur3D &vec1, const Vecteur3D &vec2)
{
    QCOMPARE(vec1.x(), vec2.x());
    QCOMPARE(vec1.y(), vec2.y());
    QCOMPARE(vec1.z(), vec2.z());
}

void CompareFichiers(const QString &ficRes, const QString &ficRef)
{
    QFile ref(ficRef);
    ref.open(QIODevice::ReadOnly | QIODevice::Text);
    const QString ficref = ref.readAll();
    ref.close();

    QFile res(ficRes);
    res.open(QIODevice::ReadOnly | QIODevice::Text);
    const QString ficres = res.readAll();
    res.close();

    QCOMPARE(ficres, ficref);
}

}
