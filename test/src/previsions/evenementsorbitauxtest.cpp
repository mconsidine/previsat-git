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
 * >    evenementsorbitauxtest.cpp
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
#include "interface/afficher.h"
#include "librairies/corps/corps.h"
#include "previsions/evenementsorbitaux.h"
#include "evenementsorbitauxtest.h"
#include "test/src/testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void EvenementsOrbitauxTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::InitTabConstellations(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Date::Initialisation(dirLocalData);

    conditions.jj1 = 7531.416666666667;
    conditions.jj2 = 7538.416666666667;
    conditions.ecart = true;
    conditions.offset = 0.08333333333333333;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.unite = "km";
    conditions.eclipse = true;
    conditions.calcEclipseLune = true;
    conditions.listeSatellites << "20580" << "25544";
    conditions.passageNoeuds = true;
    conditions.passageOmbre = true;
    conditions.passageApogee = true;
    conditions.passagePso = true;
    conditions.transitionsJourNuit = true;

    testCalculEvenements1();
}

void EvenementsOrbitauxTest::testCalculEvenements1()
{
    qInfo(Q_FUNC_INFO);

    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "tle" + QDir::separator() + "visual.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "evenements1_20200815_20200822.txt";

    conditions.fichier = fichier;
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    EvenementsOrbitaux::setConditions(conditions);
    EvenementsOrbitaux::CalculEvenements(n);

    Afficher *afficher = new Afficher(EVENEMENTS, conditions, EvenementsOrbitaux::donnees(), EvenementsOrbitaux::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
            + "evenements1_20200815_20200822.txt";
    CompareFichiers(ficRes, ficRef);
}
