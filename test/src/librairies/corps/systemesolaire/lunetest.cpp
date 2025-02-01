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
 * >    lunetest.cpp
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
 * >    1er fevrier 2025
 *
 */

#include <QtTest>
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/dates/date.h"
#include "librairies/observateur/observateur.h"
#include "lunetest.h"
#include "testtools.h"


using namespace TestTools;

void LuneTest::testAll()
{
    testCalculPositionSimp();
    testCalculMagnitudePhase();
    testCalculLeverMeridienCoucher();
    testCalculDatesPhases();
}

void LuneTest::testCalculPositionSimp()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Lune lune;
    lune.CalculPositionSimp(date);
    const Vecteur3D pos(-251640.85931597583, 254391.46471626704, 87678.7116029949);
    CompareVecteurs3D(lune.position(), pos);
}

void LuneTest::testCalculMagnitudePhase()
{
    qInfo(Q_FUNC_INFO);

    const Date date1(1992, 4, 11, 23, 59, 1.816, 0.);
    Soleil soleil;
    soleil.CalculPositionSimp(date1);
    Lune lune;
    lune.CalculPositionSimp(date1);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.6785704098427325);
    QCOMPARE(lune.luneCroissante(), true);
    QCOMPARE(lune.anglePhase(), 1.2055912868186884);
    QCOMPARE(lune.phase(), QObject::tr("Premier quartier"));
    QCOMPARE(lune.magnitude(), -10.653131296343474);

    const Date date2(2023, 5, 3, 0, 0, 0., 0.);
    soleil.CalculPositionSimp(date2);
    lune.CalculPositionSimp(date2);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.9194146268228294);
    QCOMPARE(lune.luneCroissante(), true);
    QCOMPARE(lune.anglePhase(), 0.5756672283689478);
    QCOMPARE(lune.phase(), QObject::tr("Gibbeuse croissante"));
    QCOMPARE(lune.magnitude(), -11.570426293357274);

    const Date date3(2023, 5, 16, 12, 0, 0., 0.);
    soleil.CalculPositionSimp(date3);
    lune.CalculPositionSimp(date3);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.11338723721879734);
    QCOMPARE(lune.luneCroissante(), false);
    QCOMPARE(lune.anglePhase(), 2.454708350973763);
    QCOMPARE(lune.phase(), QObject::tr("Dernier croissant"));
    QCOMPARE(lune.magnitude(), -6.837637841800411);

    const Date date4(2023, 5, 19, 17, 50, 0., 0.);
    soleil.CalculPositionSimp(date4);
    lune.CalculPositionSimp(date4);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.0004625325275197101);
    QCOMPARE(lune.luneCroissante(), true);
    QCOMPARE(lune.anglePhase(), 3.098576198262582);
    QCOMPARE(lune.phase(), QObject::tr("Nouvelle Lune"));
    QCOMPARE(lune.magnitude(), -2.497383209945962);

    const Date date5(2023, 5, 5, 0, 0, 0., 0.);
    soleil.CalculPositionSimp(date5);
    lune.CalculPositionSimp(date5);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.9937846767778056);
    QCOMPARE(lune.luneCroissante(), true);
    QCOMPARE(lune.anglePhase(), 0.1578384341715419);
    QCOMPARE(lune.phase(), QObject::tr("Pleine Lune"));
    QCOMPARE(lune.magnitude(), -12.221111842562916);
}

void LuneTest::testCalculLeverMeridienCoucher()
{
    qInfo(Q_FUNC_INFO);

    Lune lune;

    // Toutes les heures sont definies
    Date date(2022, 5, 1, 5, 6, 7., 2. / 24.);
    Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    lune.CalculLeverMeridienCoucher(date, DateSysteme::SYSTEME_24H, obs, false);

    QCOMPARE(lune.dateLever(), "06h52");
    QCOMPARE(lune.dateMeridien(), "14h18");
    QCOMPARE(lune.dateCoucher(), "21h59");

    // L'heure de coucher n'existe pas
    date = Date(2022, 5, 3, 5, 6, 7., 2. / 24.);
    lune.CalculLeverMeridienCoucher(date, DateSysteme::SYSTEME_24H, obs, false);

    QCOMPARE(lune.dateLever(), "07h40");
    QCOMPARE(lune.dateMeridien(), "15h52");
    QCOMPARE(lune.dateCoucher(), "-");

    // L'heure de passage au meridien n'existe pas
    date = Date(2022, 5, 14, 5, 6, 7., 2. / 24.);
    lune.CalculLeverMeridienCoucher(date, DateSysteme::SYSTEME_24H, obs, false);

    QCOMPARE(lune.dateLever(), "19h26");
    QCOMPARE(lune.dateMeridien(), "-");
    QCOMPARE(lune.dateCoucher(), "05h24");

    // L'heure de lever n'existe pas
    date = Date(2022, 5, 18, 5, 6, 7., 2. / 24.);
    lune.CalculLeverMeridienCoucher(date, DateSysteme::SYSTEME_24H, obs, false);

    QCOMPARE(lune.dateLever(), "-");
    QCOMPARE(lune.dateMeridien(), "03h39");
    QCOMPARE(lune.dateCoucher(), "07h28");
}

void LuneTest::testCalculDatesPhases()
{
    qInfo(Q_FUNC_INFO);

    Date date(2022, 4, 1, 0, 0, 0., 2. / 24.);
    Lune lune;
    lune.CalculDatesPhases(date);

    QCOMPARE(lune.datesPhases()[0], "2022-04-01 08h24");
    QCOMPARE(lune.datesPhases()[1], "2022-04-09 08h47");
    QCOMPARE(lune.datesPhases()[2], "2022-04-16 20h55");
    QCOMPARE(lune.datesPhases()[3], "2022-04-23 13h56");

    // Lunaison suivante
    date = Date(2022, 4, 24, 0, 0, 0., 2. / 24.);
    lune.CalculDatesPhases(date);

    QCOMPARE(lune.datesPhases()[0], "2022-04-30 22h27");
    QCOMPARE(lune.datesPhases()[1], "2022-05-09 02h20");
    QCOMPARE(lune.datesPhases()[2], "2022-05-16 06h13");
    QCOMPARE(lune.datesPhases()[3], "2022-05-22 20h42");
}
