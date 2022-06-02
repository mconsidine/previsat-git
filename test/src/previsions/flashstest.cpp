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
 * >    22 mai 2022
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
#include "interface/afficher.h"
#include "librairies/corps/corps.h"
#include "previsions/flashs.h"
#include "flashstest.h"
#include "test/src/testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void FlashsTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    qApp->setOrganizationName("Astropedia");
    Configuration::instance()->Initialisation();

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::InitTabConstellations(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    conditions.jj1 = 7531.416666666667;
    conditions.jj2 = 7562.416666666667;
    conditions.ecart = true;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.observateur = Observateur("Paris", -2.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.eclipse = true;
    conditions.magnitudeLimite = 2.;
    conditions.hauteur = 10. * DEG2RAD;
    conditions.crepuscule = -6. * DEG2RAD;
    conditions.angleLimite = PI;
    conditions.extinction = true;
    conditions.refraction = true;
    conditions.effetEclipsePartielle = true;
    conditions.calcEclipseLune = true;

    testCalculFlashs();
}

void FlashsTest::testCalculFlashs()
{
    qInfo(Q_FUNC_INFO);

    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "tle" + QDir::separator() + "flares-spctrk.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "flashs1_20200815_20200915.txt";

    conditions.fichier = fichier;
    conditions.ficRes = ficRes;

    // Lancement du calcul des flashs
    Flashs::setConditions(conditions);
    Flashs::CalculFlashs(n);

    Afficher *afficher = new Afficher(FLASHS, conditions, Flashs::donnees(), Flashs::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "flashs1_20200815_20200915.txt";
    CompareFichiers(ficRes, ficRef);
}
