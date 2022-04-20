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
 * >    transitsisstest.cpp
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
#include "interface/afficher.h"
#include "librairies/corps/corps.h"
#include "previsions/transitsiss.h"
#include "transitsisstest.h"
#include "test/src/testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void TransitsIssTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + QDir::separator() + "data";
    Corps::InitTabConstellations(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    conditions.jj1 = 7790.416666666667;
    conditions.jj2 = 7821.416666666667;
    conditions.ecart = false;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.observateur = Observateur("Paris", -002.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.eclipse = false;
    conditions.seuilConjonction = 20. * DEG2RAD;
    conditions.hauteur = 5. * DEG2RAD;
    conditions.crepuscule = PI_SUR_DEUX;
    conditions.extinction = false;
    conditions.refraction = true;
    conditions.effetEclipsePartielle = true;
    conditions.calcEclipseLune = false;
    conditions.calcTransitLunaireJour = true;
    conditions.calcEphemSoleil = true;
    conditions.calcEphemLune = true;
    conditions.listeSatellites << "25544";

    testCalculTransitsISS();
}

void TransitsIssTest::testCalculTransitsISS()
{
    qInfo(Q_FUNC_INFO);

    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "tle" + QDir::separator() + "iss.3le";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "transits_20210501_20210601";

    conditions.tabtle = TLE::LectureFichier3le(fichier);
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    TransitsIss::setConditions(conditions);
    TransitsIss::CalculTransits(n);

    Afficher *afficher = new Afficher(TRANSITS, conditions, TransitsIss::donnees(), TransitsIss::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
            + "transits_20210501_20210601.txt";
    CompareFichiers(ficRes, ficRef);
}
