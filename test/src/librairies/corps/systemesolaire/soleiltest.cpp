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
#include "librairies/observateur/observateur.h"
#include "soleiltest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void SoleilTest::testAll()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);
    Date::Initialisation(dirCommonData);

    testCalculPosition();
    testCalculLeverMeridienCoucher();
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

void SoleilTest::testCalculLeverMeridienCoucher()
{
    qInfo(Q_FUNC_INFO);

    Soleil soleil;

    // Toutes les heures sont definies
    Date date(2022, 5, 12, 5, 6, 7., 2. / 24.);
    Observateur obs1("Paris", -2.348640000, +48.853390000, 30);
    soleil.CalculLeverMeridienCoucher(date, obs1);

    QCOMPARE(soleil.dateLever().jourJulienUTC(), 8166.677083333334);
    QCOMPARE(soleil.dateMeridien().jourJulienUTC(), 8166.990972222223);
    QCOMPARE(soleil.dateCoucher().jourJulienUTC(), 8167.305555555556);

    QCOMPARE(soleil.datesCrepuscules().at(0).jourJulienUTC(), 8166.650000000001);
    QCOMPARE(soleil.datesCrepuscules().at(1).jourJulienUTC(), 8166.615972222223);
    QCOMPARE(soleil.datesCrepuscules().at(2).jourJulienUTC(), 8166.573611111112);
    QCOMPARE(soleil.datesCrepuscules().at(3).jourJulienUTC(), 8167.332638888889);
    QCOMPARE(soleil.datesCrepuscules().at(4).jourJulienUTC(), 8167.366666666667);
    QCOMPARE(soleil.datesCrepuscules().at(5).jourJulienUTC(), 8167.409722222223);


    // Cas des nuits blanches
    date = Date(2022, 6, 12, 5, 6, 7., 2. / 24.);
    soleil.CalculLeverMeridienCoucher(date, obs1);

    QCOMPARE(soleil.dateLever().jourJulienUTC(), 8197.659027777778);
    QCOMPARE(soleil.dateMeridien().jourJulienUTC(), 8197.993055555557);
    QCOMPARE(soleil.dateCoucher().jourJulienUTC(), 8198.327777777778);

    QCOMPARE(soleil.datesCrepuscules().at(0).jourJulienUTC(), 8197.628472222223);
    QCOMPARE(soleil.datesCrepuscules().at(1).jourJulienUTC(), 8197.586805555557);
    QCOMPARE(soleil.datesCrepuscules().at(2).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(3).jourJulienUTC(), 8198.359027777778);
    QCOMPARE(soleil.datesCrepuscules().at(4).jourJulienUTC(), 8198.400694444445);
    QCOMPARE(soleil.datesCrepuscules().at(5).jourJulienUTC(), DATE_INFINIE);


    // Cas du pole nord au mois de juin (pas de lever ni de coucher)
    Observateur obs2("Pole nord", 0., +90., 0);
    soleil.CalculLeverMeridienCoucher(date, obs2);

    QCOMPARE(soleil.dateLever().jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.dateMeridien().jourJulienUTC(), 8198.);
    QCOMPARE(soleil.dateCoucher().jourJulienUTC(), DATE_INFINIE);

    QCOMPARE(soleil.datesCrepuscules().at(0).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(1).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(2).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(3).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(4).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(5).jourJulienUTC(), DATE_INFINIE);

    // Cas du pole nord a l'equinoxe
    date = Date(2022, 3, 19, 5, 6, 7., 2. / 24.);
    soleil.CalculLeverMeridienCoucher(date, obs2);

    QCOMPARE(soleil.dateLever().jourJulienUTC(), 8112.699305555556);
    QCOMPARE(soleil.dateMeridien().jourJulienUTC(), 8113.0055555555555);
    QCOMPARE(soleil.dateCoucher().jourJulienUTC(), DATE_INFINIE);

    QCOMPARE(soleil.datesCrepuscules().at(0).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(1).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(2).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(3).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(4).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(5).jourJulienUTC(), DATE_INFINIE);

    // Cas d'un lieu pres du cercle polaire
    date = Date(2022, 1, 15, 5, 6, 7., 2. / 24.);
    Observateur obs3("Tromsö", 18.954656, 69.649979, 0.);
    soleil.CalculLeverMeridienCoucher(date, obs3);

    QCOMPARE(soleil.dateLever().jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.dateMeridien().jourJulienUTC(), 8050.059027777778);
    QCOMPARE(soleil.dateCoucher().jourJulienUTC(), DATE_INFINIE);

    QCOMPARE(soleil.datesCrepuscules().at(0).jourJulienUTC(), 8050.182638888889);
    QCOMPARE(soleil.datesCrepuscules().at(1).jourJulienUTC(), 8050.2444444444445);
    QCOMPARE(soleil.datesCrepuscules().at(2).jourJulienUTC(), 8050.295833333334);
    QCOMPARE(soleil.datesCrepuscules().at(3).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(4).jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(soleil.datesCrepuscules().at(5).jourJulienUTC(), DATE_INFINIE);
}
