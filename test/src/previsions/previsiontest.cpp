/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    previsiontest.cpp
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
 * >    4 fevrier 2023
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
#include "interface/afficherresultats.h"
#include "librairies/corps/corps.h"
#include "librairies/corps/satellite/tle.h"
#include "previsions/prevision.h"
#include "previsiontest.h"
#include "test/src/testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void PrevisionTest::testAll()
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
    conditions.jj2 = 7538.416666666667;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.observateur = Observateur("Paris", -2.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.eclipse = true;
    conditions.magnitudeLimite = 99.;
    conditions.hauteur = 0.;
    conditions.crepuscule = PI_SUR_DEUX;
    conditions.extinction = true;
    conditions.refraction = true;
    conditions.effetEclipsePartielle = true;
    conditions.calcEclipseLune = true;

    testCalculPrevisions1();
    testCalculPrevisions2();
    testCalculPrevisions3();
}

void PrevisionTest::testCalculPrevisions1()
{
    qInfo(Q_FUNC_INFO);

    // Test des satellites potentiellement visibles
    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "previsions1_20200815_20200822.txt";

    conditions.tabElem = TLE::LectureFichier(fichier, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec());
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    Prevision::setConditions(conditions);
    Prevision::CalculPrevisions(n);

    AfficherResultats *afficher = new AfficherResultats(TypeCalcul::PREVISIONS, conditions, Prevision::donnees(), Prevision::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
            + "previsions1_20200815_20200822.txt";
    CompareFichiers(ficRes, ficRef);
}

void PrevisionTest::testCalculPrevisions2()
{
    qInfo(Q_FUNC_INFO);

    // Test des satellites geostationnaires
    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "geo.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "previsions2_20200815_20200822.txt";

    conditions.pas = 0.0034722222222222222;
    conditions.hauteur = 10. * DEG2RAD;
    conditions.crepuscule = -6. * DEG2RAD;
    conditions.tabElem = TLE::LectureFichier(fichier, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec());
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    Prevision::setConditions(conditions);
    Prevision::CalculPrevisions(n);

    AfficherResultats *afficher = new AfficherResultats(TypeCalcul::PREVISIONS, conditions, Prevision::donnees(), Prevision::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
            + "previsions2_20200815_20200822.txt";
    CompareFichiers(ficRes, ficRef);
}

void PrevisionTest::testCalculPrevisions3()
{
    qInfo(Q_FUNC_INFO);

    // Test des satellites HEO
    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "heo.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "previsions3_20200815_20200822.txt";

    conditions.pas = 0.0034722222222222222;
    conditions.hauteur = 5. * DEG2RAD;
    conditions.crepuscule = -6. * DEG2RAD;
    conditions.tabElem = TLE::LectureFichier(fichier, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec());
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    Prevision::setConditions(conditions);
    Prevision::CalculPrevisions(n);

    AfficherResultats *afficher = new AfficherResultats(TypeCalcul::PREVISIONS, conditions, Prevision::donnees(), Prevision::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
            + "previsions3_20200815_20200822.txt";
    CompareFichiers(ficRes, ficRef);
}
