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
 * >    flashstest.cpp
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
 * >    7 juillet 2024
 *
 */

#include <QtTest>
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "interface/afficherresultats.h"
#include "librairies/corps/corps.h"
#include "librairies/corps/satellite/tle.h"
#include "previsions/flashs.h"
#include "flashstest.h"
#include "testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void FlashsTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->_dirCfg = dirLocalData + QDir::separator() + "config";
    Configuration::instance()->OuvertureBaseDonneesSatellites();
    Configuration::instance()->_mapFlashs = GestionnaireXml::LectureSatellitesFlashs();

    conditions.jj1 = 7531.416666666667;
    conditions.jj2 = 7562.416666666667;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.observateur = Observateur("Paris", -2.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.eclipse = true;
    conditions.magnitudeLimite = 2.;
    conditions.hauteur = 10. * MATHS::DEG2RAD;
    conditions.crepuscule = -6. * MATHS::DEG2RAD;
    conditions.angleLimite = MATHS::PI;
    conditions.extinction = true;
    conditions.refraction = true;
    conditions.effetEclipsePartielle = true;
    conditions.calcEclipseLune = true;

    testCalculFlashs();
    testCalculMagnitudeFlash();

    Configuration::instance()->FermetureBaseDonneesSatellites();
}

void FlashsTest::testCalculFlashs()
{
    qInfo(Q_FUNC_INFO);

    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "flares-spctrk.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "flashs1_20200815_20200915.txt";

    conditions.tabElem = TLE::Lecture(fichier, Configuration::instance()->dbSatellites());
    conditions.ficRes = ficRes;

    // Lancement du calcul des flashs
    Flashs::setConditions(conditions);
    Flashs::CalculFlashs(n);

    AfficherResultats *afficher = new AfficherResultats(TypeCalcul::FLASHS, conditions, Flashs::donnees(), Flashs::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "flashs1_20200815_20200915.txt";
    CompareFichiers(ficRes, ficRef);

    Configuration::instance()->FermetureBaseDonneesSatellites();
}

void FlashsTest::testCalculMagnitudeFlash()
{
    qInfo(Q_FUNC_INFO);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->OuvertureBaseDonneesSatellites();

    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "flares-spctrk.txt";
    const QMap<QString, ElementsOrbitaux> mapTle = TLE::Lecture(fichier, Configuration::instance()->dbSatellites());

    Date date(2020, 9, 13, 4, 48, 3.5, 0.);

    Observateur obs("Paris", -2.34864, 48.85339, 30.);
    obs.CalculPosVit(date);

    Satellite sat(mapTle["33412"]);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(obs, true);

    Soleil soleil;
    soleil.CalculPosVit(date);
    soleil.CalculCoordHoriz(obs, true);

    QCOMPARE(Flashs::CalculMagnitudeFlash(date, sat, soleil, true, true), -1.8325270487881304);

    Configuration::instance()->FermetureBaseDonneesSatellites();
}
