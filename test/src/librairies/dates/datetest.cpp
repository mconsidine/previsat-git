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
 * >    datestest.cpp
 *
 * Localisation
 * >    test.librairies.dates
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
#include "librairies/dates/date.h"
#include "datetest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void DateTest::testAll()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString locale = QLocale::system().name().section('_', 0, 0);
    const QString dirLocalData = dir.path() + QDir::separator() + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    testDates();
    if (locale == "fr") {
        testToLongDate();
    } else {
        qInfo("Fonction ToLongDate non testee");
    }
    testToQDateTime();
    testToShortDate();
    testToShortDateAMJ();
    testToShortDateAMJmillisec();
    testCalculOffsetUTC();
    testConversionDateNasa();
}

void DateTest::testToLongDate()
{
    qInfo(Q_FUNC_INFO);

    const Date date1(2006, 1, 15, 21, 24, 37.5, 1. / 24.);
    QCOMPARE(date1.ToLongDate(SYSTEME_24H), "Dimanche 15 janvier 2006 22:24:38");
}

void DateTest::testToQDateTime()
{
    qInfo(Q_FUNC_INFO);

    const Date date2(2006, 1, 15, 21, 24, 37.5, 0.);
    const QDateTime date3(QDate(2006, 1, 15), QTime(21, 24, 38));
    QCOMPARE(date2.ToQDateTime(1), date3);
}

void DateTest::testToShortDate()
{
    qInfo(Q_FUNC_INFO);

    const Date date1(2006, 1, 15, 21, 24, 37.5345, 1. / 24.);
    QCOMPARE(date1.ToShortDate(FORMAT_COURT, SYSTEME_24H), "15/01/2006 22:24:38 ");
    QCOMPARE(date1.ToShortDate(FORMAT_COURT, SYSTEME_12H), "15/01/2006 10:24:38p");
    QCOMPARE(date1.ToShortDate(FORMAT_LONG, SYSTEME_24H), "15/01/2006 22:24:37.5 ");
    QCOMPARE(date1.ToShortDate(FORMAT_MILLISEC, SYSTEME_24H), "15/01/2006 22:24:37.535 ");
    QCOMPARE(date1.ToShortDate(FORMAT_MILLISEC, SYSTEME_12H), "15/01/2006 10:24:37.535p");
}

void DateTest::testToShortDateAMJ()
{
    qInfo(Q_FUNC_INFO);

    const Date date1(2006, 1, 15, 21, 24, 37.5345, 1. / 24.);
    QCOMPARE(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H), "2006/01/15 22:24:38 ");
    QCOMPARE(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_12H), "2006/01/15 10:24:38p");
    QCOMPARE(date1.ToShortDateAMJ(FORMAT_LONG, SYSTEME_24H), "2006/01/15 22:24:37.5 ");
    QCOMPARE(date1.ToShortDateAMJ(FORMAT_LONG, SYSTEME_12H), "2006/01/15 10:24:37.5p");
    QCOMPARE(date1.ToShortDateAMJ(FORMAT_MILLISEC, SYSTEME_24H), "2006/01/15 22:24:37.535 ");
    QCOMPARE(date1.ToShortDateAMJ(FORMAT_MILLISEC, SYSTEME_12H), "2006/01/15 10:24:37.535p");
}

void DateTest::testToShortDateAMJmillisec()
{
    qInfo(Q_FUNC_INFO);

    const Date date1(2006, 1, 15, 21, 24, 37.5345, 1. / 24.);
    QCOMPARE(date1.ToShortDateAMJmillisec(), "2006/01/15 21:24:37.535");
    const Date date2(2006, 1, 15, 21, 24, 37.5344, 1. / 24.);
    QCOMPARE(date2.ToShortDateAMJmillisec(), "2006/01/15 21:24:37.534");
}

void DateTest::testCalculOffsetUTC()
{
    qInfo(Q_FUNC_INFO);

    const QDateTime date4(QDate(2006, 1, 15), QTime(20, 24, 37));
    QCOMPARE(Date::CalculOffsetUTC(date4), 1. / 24.);
}

void DateTest::testConversionDateNasa()
{
    qInfo(Q_FUNC_INFO);

    const Date date5(2006, 1, 15, 21, 24, 37.5, 0.);
    CompareDates(Date::ConversionDateNasa("2006-015T21:24:37.5Z"), date5);
}

void DateTest::testDates()
{
    qInfo(Q_FUNC_INFO);

    const Date date1(2006, 1, 15, 21, 24, 37.5, 1. / 24.);
    QCOMPARE(date1.annee(), 2006);
    QCOMPARE(date1.mois(), 1);
    QCOMPARE(date1.jour(), 15);
    QCOMPARE(date1.heure(), 21);
    QCOMPARE(date1.minutes(), 24);
    QCOMPARE(date1.secondes(), 37.5);
    QCOMPARE(date1.offsetUTC(), 1. / 24.);
    QCOMPARE(date1.jourJulienUTC(), 2206.3921006944444);
    QCOMPARE(date1.jourJulienTT(), 2206.3928551388888);
    QCOMPARE(date1.jourJulien(), 2206.4337673611111);

    const Date date2(2206.3921006944444, 1. / 24.);
    CompareDates(date2, date1);

    const Date date3(2006, 1, 15.8921006944444, 1. / 24.);
    CompareDates(date2, date1);
}
