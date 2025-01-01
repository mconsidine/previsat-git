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
 * >    starlinktest.cpp
 *
 * Localisation
 * >    test.previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    1er octobre 223
 *
 * Date de revision
 * >    7 juillet 2024
 *
 */

#include <QtTest>
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "librairies/corps/corps.h"
#include "librairies/dates/date.h"
#include "librairies/corps/satellite/gpformat.h"
#include "previsions/prevision.h"
#include "starlinktest.h"
#include "testtools.h"


using namespace TestTools;

void StarlinkTest::testAll()
{
    testStarlink1();
}

void StarlinkTest::testStarlink1()
{
    qInfo(Q_FUNC_INFO);

    int n = 0;
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    Configuration::instance()->_dirLocalData = dirLocalData;
    Configuration::instance()->OuvertureBaseDonneesSatellites();
    Configuration::instance()->AjoutDonneesSatellitesStarlink("Starlink G6-19 Pre-Launch", "starlink-g6-19", "2023-09-30 02:00:00", "2023-09-30 03:05:19");

    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "starlink-g6-19.xml";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "starlink_20230930_20231007.txt";

    const QMap<QString, ElementsOrbitaux> tabElem = GPFormat::Lecture(fichier);

    ConditionsPrevisions conditions;
    conditions.jj1 = 8672.586805555557;
    conditions.jj2 = 8679.586805555557;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.observateur = Observateur("Paris", -2.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.eclipse = true;
    conditions.magnitudeLimite = 10.;
    conditions.hauteur = 0.;
    conditions.crepuscule = -6. * MATHS::DEG2RAD;
    conditions.extinction = true;
    conditions.refraction = true;
    conditions.effetEclipsePartielle = true;
    conditions.calcEclipseLune = true;
    conditions.tabElem[tabElem.firstKey()] = tabElem[tabElem.firstKey()];
    conditions.tabElem.first().donnees.setMagnitudeStandard(STARLINK::MAGNITUDE_STANDARD);
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    Prevision::setConditions(conditions);
    Prevision::CalculPrevisions(n);

    AfficherResultats *afficher = new AfficherResultats(TypeCalcul::STARLINK, conditions, Prevision::donnees(), Prevision::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
                           + "starlink_20230930_20231007.txt";
    CompareFichiers(ficRes, ficRef);

    Configuration::instance()->FermetureBaseDonneesSatellites();
}
