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
#include "librairies/observateur/observateur.h"
#include "soleiltest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void SoleilTest::testAll()
{
    try {
        Corps::Initialisation("empty");
    } catch (std::exception &e) {
    }

    try {
        Soleil soleil;
        soleil.CalculCoordEquat(Observateur());
    } catch (std::exception &e) {
    }

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);
    Date::Initialisation(dirCommonData);

    testSoleil();
    testCalculPosition();
    testCalculLeverMeridienCoucher();
}

void SoleilTest::testSoleil()
{
    qInfo(Q_FUNC_INFO);

    const Soleil soleil(Vecteur3D(-140328287.9615894, -46628772.91250957, -20216954.727520175));
    const Vecteur3D pos(-140328287.9615894, -46628772.91250957, -20216954.727520175);
    CompareVecteurs3D(soleil.position(), pos);
}

void SoleilTest::testCalculPosition()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 10, 12, 23, 59, 0.816, 0.);

    Soleil soleil;
    soleil.CalculPosition(date);

    const Vecteur3D pos(-140328287.9615894, -46628772.91250957, -20216954.727520175);
    CompareVecteurs3D(soleil.position(), pos);
    QCOMPARE(soleil.distanceUA(), pos.Norme() * SOLEIL::KM2UA);
}

void SoleilTest::testCalculLeverMeridienCoucher()
{
    qInfo(Q_FUNC_INFO);

    Soleil soleil;

    // Toutes les heures sont definies
    Date date(2022, 5, 12, 5, 6, 7., 2. / 24.);
    Observateur obs1("Paris", -2.348640000, +48.853390000, 30);
    soleil.CalculLeverMeridienCoucher(date, obs1, DateSysteme::SYSTEME_24H);

    QCOMPARE(soleil.dateLever(), "06h15");
    QCOMPARE(soleil.dateMeridien(), "13h47");
    QCOMPARE(soleil.dateCoucher(), "21h20");

    QCOMPARE(soleil.datesCrepuscules().at(0), "05h36");
    QCOMPARE(soleil.datesCrepuscules().at(1), "04h47");
    QCOMPARE(soleil.datesCrepuscules().at(2), "03h46");
    QCOMPARE(soleil.datesCrepuscules().at(3), "21h59");
    QCOMPARE(soleil.datesCrepuscules().at(4), "22h48");
    QCOMPARE(soleil.datesCrepuscules().at(5), "23h50");


    // Cas des nuits blanches
    date = Date(2022, 6, 12, 5, 6, 7., 2. / 24.);
    soleil.CalculLeverMeridienCoucher(date, obs1, DateSysteme::SYSTEME_24H);

    QCOMPARE(soleil.dateLever(), "05h49");
    QCOMPARE(soleil.dateMeridien(), "13h50");
    QCOMPARE(soleil.dateCoucher(), "21h52");

    QCOMPARE(soleil.datesCrepuscules().at(0), "05h05");
    QCOMPARE(soleil.datesCrepuscules().at(1), "04h05");
    QCOMPARE(soleil.datesCrepuscules().at(2), "-");
    QCOMPARE(soleil.datesCrepuscules().at(3), "22h37");
    QCOMPARE(soleil.datesCrepuscules().at(4), "23h37");
    QCOMPARE(soleil.datesCrepuscules().at(5), "-");


    // Cas du pole nord au mois de juin (pas de lever ni de coucher)
    Observateur obs2("Pole nord", 0., +90., 0);
    soleil.CalculLeverMeridienCoucher(date, obs2, DateSysteme::SYSTEME_24H);

    QCOMPARE(soleil.dateLever(), "-");
    QCOMPARE(soleil.dateMeridien(), "14h00");
    QCOMPARE(soleil.dateCoucher(), "-");

    QCOMPARE(soleil.datesCrepuscules().at(0), "-");
    QCOMPARE(soleil.datesCrepuscules().at(1), "-");
    QCOMPARE(soleil.datesCrepuscules().at(2), "-");
    QCOMPARE(soleil.datesCrepuscules().at(3), "-");
    QCOMPARE(soleil.datesCrepuscules().at(4), "-");
    QCOMPARE(soleil.datesCrepuscules().at(5), "-");

    // Cas du pole nord a l'equinoxe
    date = Date(2022, 3, 19, 5, 6, 7., 2. / 24.);
    soleil.CalculLeverMeridienCoucher(date, obs2, DateSysteme::SYSTEME_24H);

    QCOMPARE(soleil.dateLever(), "06h47");
    QCOMPARE(soleil.dateMeridien(), "14h08");
    QCOMPARE(soleil.dateCoucher(), "-");

    QCOMPARE(soleil.datesCrepuscules().at(0), "-");
    QCOMPARE(soleil.datesCrepuscules().at(1), "-");
    QCOMPARE(soleil.datesCrepuscules().at(2), "-");
    QCOMPARE(soleil.datesCrepuscules().at(3), "-");
    QCOMPARE(soleil.datesCrepuscules().at(4), "-");
    QCOMPARE(soleil.datesCrepuscules().at(5), "-");

    // Cas d'un lieu pres du cercle polaire
    date = Date(2022, 1, 15, 5, 6, 7., 2. / 24.);
    Observateur obs3("Tromsö", 18.954656, 69.649979, 0.);
    soleil.CalculLeverMeridienCoucher(date, obs3, DateSysteme::SYSTEME_24H);

    QCOMPARE(soleil.dateLever(), "-");
    QCOMPARE(soleil.dateMeridien(), "15h25");
    QCOMPARE(soleil.dateCoucher(), "-");

    QCOMPARE(soleil.datesCrepuscules().at(0), "18h23");
    QCOMPARE(soleil.datesCrepuscules().at(1), "19h52");
    QCOMPARE(soleil.datesCrepuscules().at(2), "21h06");
    QCOMPARE(soleil.datesCrepuscules().at(3), "-");
    QCOMPARE(soleil.datesCrepuscules().at(4), "-");
    QCOMPARE(soleil.datesCrepuscules().at(5), "-");
}
