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
 * >    telescopetest.cpp
 *
 * Localisation
 * >    test.previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    18 juin 2019
 *
 * Date de revision
 * >    19 juin 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "configuration/configuration.h"
#include "telescopetest.h"
#include "librairies/corps/corps.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/tle.h"
#include "previsions/telescope.h"
#include "test/src/testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void TelescopeTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->LectureDonneesSatellites();

    conditions.jj1 = 7531.416666666667;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 20.; // ms
    conditions.observateur = Observateur("Paris", -2.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.listeSatellites.append("25544");

    testCalculSuiviTelescope1();
}

void TelescopeTest::testCalculSuiviTelescope1()
{
    qInfo(Q_FUNC_INFO);

    // Test des satellites potentiellement visibles
    int n = 0;
    conditions.fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "tle" + QDir::separator() + "visual.txt";

    const int lgrec = Configuration::instance()->lgRec();
    const QStringList listeTLE(QStringList () << "25544");
    QMap<QString, ElementsOrbitaux> mapTLE = TLE::LectureFichier(conditions.fichier, Configuration::instance()->donneesSatellites(), lgrec, listeTLE);

    Satellite sat(mapTLE.first());

    // Determination du prochain passage
    Date date(conditions.jj1, 0., false);
    conditions.observateur.CalculPosVit(date);

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(conditions.observateur);

    Soleil soleil;
    soleil.CalculPosition(date);

    Lune lune;
    lune.CalculPosition(date);

    ConditionEclipse condEcl;
    condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, true);

    // Date de lever
    const ElementsAOS elemAOS = Evenements::CalculAOS(date, sat, conditions.observateur);
    const Date dateAosSuivi(elemAOS.date.jourJulienUTC(), conditions.offset);

    // Date de coucher
    date = Date(dateAosSuivi.jourJulienUTC() + 10. * NB_JOUR_PAR_SEC, 0., false);
    conditions.observateur.CalculPosVit(date);

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(conditions.observateur);

    soleil.CalculPosition(date);
    lune.CalculPosition(date);
    condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, true);

    const ElementsAOS elemLOS = Evenements::CalculAOS(date, sat, conditions.observateur);
    const Date dateLosSuivi(elemLOS.date.jourJulienUTC(), conditions.offset);

    conditions.jj1 = dateAosSuivi.jourJulienUTC();
    conditions.nbIter = qRound(((dateLosSuivi.jourJulienUTC() - dateAosSuivi.jourJulienUTC()) * NB_MILLISEC_PAR_JOUR + 1000.) / conditions.pas);

    // Nom du fichier genere
    const QString fmtFicOut = "%1%2%3T%4%5_%6%7%8T%9%10_%11.csv";
    const QString nomfic = fmtFicOut.arg(dateAosSuivi.annee()).arg(dateAosSuivi.mois(), 2, 10, QChar('0')).arg(dateAosSuivi.jour(), 2, 10, QChar('0'))
            .arg(dateAosSuivi.heure(), 2, 10, QChar('0')).arg(dateAosSuivi.minutes(), 2, 10, QChar('0'))
            .arg(dateLosSuivi.annee()).arg(dateLosSuivi.mois(), 2, 10, QChar('0')).arg(dateLosSuivi.jour(), 2, 10, QChar('0'))
            .arg(dateLosSuivi.heure(), 2, 10, QChar('0')).arg(dateLosSuivi.minutes() + 1, 2, 10, QChar('0')).arg(sat.elementsOrbitaux().norad);
    conditions.ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + nomfic;

    // Generation du fichier de suivi
    Telescope::setConditions(conditions);
    Telescope::CalculSuiviTelescope(n);

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + nomfic;
    CompareFichiers(conditions.ficRes, ficRef);
}
