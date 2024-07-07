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
 * >    7 juillet 2024
 *
 */

#include <QtTest>
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "librairies/corps/corps.h"
#include "librairies/corps/satellite/gpformat.h"
#include "previsions/transits.h"
#include "transitstest.h"
#include "testtools.h"


using namespace TestTools;

static ConditionsPrevisions conditions;
static QDir dir;


void TransitsTest::testAll()
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
    Configuration::instance()->_noradStationSpatiale = "025544";
    Configuration::instance()->OuvertureBaseDonneesSatellites();

    conditions.jj1 = 8435.416666666667;
    conditions.jj2 = 8465.416666666667;
    conditions.offset = 0.04166666666666667;
    conditions.systeme = true;
    conditions.pas = 0.0006944444444444445;
    conditions.observateur = Observateur("Paris", -2.34864, 48.85339, 30.);
    conditions.unite = "km";
    conditions.eclipse = false;
    conditions.seuilConjonction = 20. * MATHS::DEG2RAD;
    conditions.hauteur = 5. * MATHS::DEG2RAD;
    conditions.crepuscule = MATHS::PI_SUR_DEUX;
    conditions.extinction = false;
    conditions.refraction = true;
    conditions.effetEclipsePartielle = true;
    conditions.calcEclipseLune = false;
    conditions.calcTransitLunaireJour = true;
    conditions.calcEphemSoleil = true;
    conditions.calcEphemLune = true;
    conditions.listeSatellites << "025544" << "027386" << "027424" << "048274";

    testCalculTransits();

    Configuration::instance()->FermetureBaseDonneesSatellites();
}

void TransitsTest::testCalculTransits()
{
    qInfo(Q_FUNC_INFO);

    int n = 0;
    const QString fichier = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual_transits.xml";
    const QString fichierIss = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "iss.gp";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "transits_20230205_20230307.txt";

    conditions.tabElem = GPFormat::Lecture(fichier, Configuration::instance()->dbSatellites(), conditions.listeSatellites);
    conditions.listeElemIss = GPFormat::LectureListeGP(fichierIss, Configuration::instance()->dbSatellites());
    conditions.ficRes = ficRes;

    // Lancement du calcul de previsions
    Transits::setConditions(conditions);
    Transits::CalculTransits(n);

    AfficherResultats *afficher = new AfficherResultats(TypeCalcul::TRANSITS, conditions, Transits::donnees(), Transits::resultats());
    afficher->on_actionEnregistrerTxt_triggered();

    // Comparaison avec les resultats de reference
    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator()
            + "transits_20230205_20230307.txt";
    CompareFichiers(ficRes, ficRef);
}
