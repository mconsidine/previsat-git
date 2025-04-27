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
 * >    27 avril 2025
 *
 */

#include <QtTest>
#include "librairies/dates/date.h"
#include "librairies/exceptions/exception.h"
#include "datetest.h"
#include "testtools.h"


using namespace TestTools;

void DateTest::testAll()
{
    testDates1();

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    static_cast<void> (dir.cd(APP_NAME));

    const QLocale locale(QLocale("fr_FR"));
    QLocale::setDefault(locale);
    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";

    Date::Initialisation(dirLocalData);

    testDates2();
    testToLongDate();
    testToQDateTime();
    testToShortDate();
    testToShortDateAMJ();
    testToShortDateAMJmillisec();
    testCalculOffsetUTC();
    testConversionDateIso();
    testConversionDateNasa();
}

void DateTest::testDates1()
{
    qInfo(Q_FUNC_INFO);

    QVERIFY_THROWS_EXCEPTION(Exception, Date());
    QVERIFY_THROWS_EXCEPTION(Exception, Date(2006, 1, 15, 21, 24, 37.5, 1. / 24.));
    QVERIFY_THROWS_EXCEPTION(Exception, Date(2206.3921006944444, 1. / 24.));
    QVERIFY_THROWS_EXCEPTION(Exception, Date(-1506000.3921006944444, 1. / 24.));
    QVERIFY_THROWS_EXCEPTION(Exception, Date(2006, 1, 15.8921006944444, 1. / 24.));
    QVERIFY_THROWS_EXCEPTION(Exception, Date::Initialisation("empty"));
}

void DateTest::testDates2()
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
    CompareDates(date3, date1);

    const Date date4(1966, 1, 15, 21, 24, 37.5, 1. / 24.);
    QCOMPARE(date4.jourJulienTT(), -12403.607476437845);

    const QDateTime dt(QDate(2006, 1, 15), QTime(21, 24, 37));
    const Date date5(dt, 0.);
    const Date date6(2006, 1, 15, 21, 24, 37., 0.);
    CompareDates(date5, date6);
}

void DateTest::testToLongDate()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2006, 1, 15, 21, 24, 37.5, 1. / 24.);
    QCOMPARE(date.ToLongDate(QLocale::system().name().section('_', 0, 0), DateSysteme::SYSTEME_24H), "Dimanche 15 janvier 2006 22:24:38");
}

void DateTest::testToQDateTime()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2006, 1, 15, 21, 24, 37.5, 0.);
    const QDateTime date2(QDate(2006, 1, 15), QTime(21, 24, 38));
    QCOMPARE(date.ToQDateTime(DateFormatSec::FORMAT_SEC), date2);
}

void DateTest::testToShortDate()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2006, 1, 15, 23, 24, 37.5345, 1. / 24.);
    QCOMPARE(date.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H), "16/01/2006 00:24:38 ");
    QCOMPARE(date.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_12H), "16/01/2006 12:24:38a");
    QCOMPARE(date.ToShortDate(DateFormat::FORMAT_LONG, DateSysteme::SYSTEME_24H), "16/01/2006 00:24:37.5 ");
    QCOMPARE(date.ToShortDate(DateFormat::FORMAT_MILLISEC, DateSysteme::SYSTEME_24H), "16/01/2006 00:24:37.535 ");
    QCOMPARE(date.ToShortDate(DateFormat::FORMAT_MILLISEC, DateSysteme::SYSTEME_12H), "16/01/2006 12:24:37.535a");
}

void DateTest::testToShortDateAMJ()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2006, 1, 15, 21, 24, 37.5345, 1. / 24.);
    QCOMPARE(date.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H), "2006/01/15 22:24:38 ");
    QCOMPARE(date.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_12H), "2006/01/15 10:24:38p");
    QCOMPARE(date.ToShortDateAMJ(DateFormat::FORMAT_LONG, DateSysteme::SYSTEME_24H), "2006/01/15 22:24:37.5 ");
    QCOMPARE(date.ToShortDateAMJ(DateFormat::FORMAT_LONG, DateSysteme::SYSTEME_12H), "2006/01/15 10:24:37.5p");
    QCOMPARE(date.ToShortDateAMJ(DateFormat::FORMAT_MILLISEC, DateSysteme::SYSTEME_24H), "2006/01/15 22:24:37.535 ");
    QCOMPARE(date.ToShortDateAMJ(DateFormat::FORMAT_MILLISEC, DateSysteme::SYSTEME_12H), "2006/01/15 10:24:37.535p");
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

    const QDateTime date(QDate(2006, 1, 15), QTime(20, 24, 37));
    QCOMPARE(Date::CalculOffsetUTC(date), 1. / 24.);
}

void DateTest::testConversionDateIso()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2022, 06, 04, 13, 15, 30.829824, 0.);
    CompareDates(Date::ConversionDateIso("2022-06-04T13:15:30.829824"), date);

    // Cas degrades
    QVERIFY_THROWS_EXCEPTION(Exception, Date::ConversionDateIso(""));
    QVERIFY_THROWS_EXCEPTION(Exception, Date::ConversionDateIso("aaa"));
}

void DateTest::testConversionDateNasa()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2006, 1, 15, 21, 24, 37.5, 0.);
    CompareDates(Date::ConversionDateNasa("2006-015T21:24:37.5Z"), date);

    // Cas degrades
    QVERIFY_THROWS_EXCEPTION(Exception, Date::ConversionDateNasa(""));
    QVERIFY_THROWS_EXCEPTION(Exception, Date::ConversionDateNasa("aaa"));
    QVERIFY_THROWS_EXCEPTION(Exception, Date::ConversionDateNasa("aaaTbbb"));
    QVERIFY_THROWS_EXCEPTION(Exception, Date::ConversionDateNasa("2006-015T21"));
}
