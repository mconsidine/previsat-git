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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"
#include "lunetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void LuneTest::testAll()
{
    testCalculPosition();
    testCalculPhase();
    testCalculMagnitude();
    testCalculLeverMeridienCoucher();
    testCalculDatesPhases();
}

void LuneTest::testCalculPosition()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Lune lune;
    lune.CalculPosition(date);
    const Vecteur3D pos(-251640.85931597583, 254391.46471626704, 87678.7116029949);
    CompareVecteurs3D(lune.position(), pos);
}

void LuneTest::testCalculPhase()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Soleil soleil;
    soleil.CalculPosition(date);
    Lune lune;
    lune.CalculPosition(date);
    lune.CalculPhase(soleil);
    QCOMPARE(lune.fractionIlluminee(), 0.6785704098427325);
}

void LuneTest::testCalculMagnitude()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 4, 11, 23, 59, 1.816, 0.);
    Soleil soleil;
    soleil.CalculPosition(date);
    Lune lune;
    lune.CalculPosition(date);
    lune.CalculPhase(soleil);
    lune.CalculMagnitude(soleil);
    QCOMPARE(lune.magnitude(), -10.543566245403385);
}

void LuneTest::testCalculLeverMeridienCoucher()
{
    qInfo(Q_FUNC_INFO);

    Lune lune;

    // Toutes les heures sont definies
    Date date(2022, 5, 1, 5, 6, 7., 2. / 24.);
    Observateur obs("Paris", -2.348640000, +48.853390000, 30);
    lune.CalculLeverMeridienCoucher(date, obs);

    QCOMPARE(lune.dateLever().jourJulienUTC(), 8155.7027777777785);
    QCOMPARE(lune.dateMeridien().jourJulienUTC(), 8156.012500000001);
    QCOMPARE(lune.dateCoucher().jourJulienUTC(), 8156.332638888889);

    // L'heure de coucher n'existe pas
    date = Date(2022, 5, 5, 5, 6, 7., 2. / 24.);
    lune.CalculLeverMeridienCoucher(date, obs);

    QCOMPARE(lune.dateLever().jourJulienUTC(), 8159.788888888889);
    QCOMPARE(lune.dateMeridien().jourJulienUTC(), 8160.147916666667);
    QCOMPARE(lune.dateCoucher().jourJulienUTC(), DATE_INFINIE);

    // L'heure de passage au meridien n'existe pas
    date = Date(2022, 5, 16, 5, 6, 7., 2. / 24.);
    lune.CalculLeverMeridienCoucher(date, obs);

    QCOMPARE(lune.dateLever().jourJulienUTC(), 8171.347222222223);
    QCOMPARE(lune.dateMeridien().jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(lune.dateCoucher().jourJulienUTC(), 8170.672916666667);

    // L'heure de lever n'existe pas
    date = Date(2022, 5, 20, 5, 6, 7., 2. / 24.);
    lune.CalculLeverMeridienCoucher(date, obs);

    QCOMPARE(lune.dateLever().jourJulienUTC(), DATE_INFINIE);
    QCOMPARE(lune.dateMeridien().jourJulienUTC(), 8174.659027777778);
    QCOMPARE(lune.dateCoucher().jourJulienUTC(), 8174.821527777778);
}

void LuneTest::testCalculDatesPhases()
{
    qInfo(Q_FUNC_INFO);

    Date date(2022, 4, 2, 0, 0, 0., 2. / 24.);
    Lune lune;
    lune.CalculDatesPhases(date);

    QCOMPARE(lune.datesPhases()[0].jourJulienUTC(), 8125.766956375188);
    QCOMPARE(lune.datesPhases()[1].jourJulienUTC(), 8133.782897121634);
    QCOMPARE(lune.datesPhases()[2].jourJulienUTC(), 8141.288298666821);
    QCOMPARE(lune.datesPhases()[3].jourJulienUTC(), 8147.99753680239);

    // Lunaison suivante
    date = Date(2022, 4, 24, 0, 0, 0., 2. / 24.);
    lune.CalculDatesPhases(date);

    QCOMPARE(lune.datesPhases()[0].jourJulienUTC(), 8155.35252820452);
    QCOMPARE(lune.datesPhases()[1].jourJulienUTC(), 8163.514488563886);
    QCOMPARE(lune.datesPhases()[2].jourJulienUTC(), 8170.676375584048);
    QCOMPARE(lune.datesPhases()[3].jourJulienUTC(), 8177.279776173665);
}
