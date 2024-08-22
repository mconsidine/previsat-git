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
 * >    generaltest.cpp
 *
 * Localisation
 * >    test.interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    29 octobre 2019
 *
 * Date de revision
 * >    22 aout 2024
 *
 */

#include <QtTest>

#include "generaltest.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "interface/onglets/general/general.h"
#include "interface/onglets/osculateurs/osculateurs.h"
#include "librairies/corps/satellite/tle.h"
#include "testtools.h"


using namespace TestTools;

void GeneralTest::testAll()
{
    testSauveOngletGeneral();
}

void GeneralTest::testSauveOngletGeneral()
{
    qInfo(Q_FUNC_INFO);

    General *general = nullptr;
    Osculateurs *osculateurs = nullptr;

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    Configuration::instance()->_locale = "fr";

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Configuration::instance()->_dirLocalData = dirLocalData;
    Date::Initialisation(dirLocalData);

    Configuration::instance()->OuvertureBaseDonneesSatellites();
    Configuration::instance()->_mapNomsConstellations = GestionnaireXml::LectureNomsConstellations();

    const Date date(2020, 8, 15, 10, 0, 0., 2. / 24.);

    Observateur observateur("Paris", -2.34864, 48.85339, 30.);
    observateur.CalculPosVit(date);
    QList<Observateur> obs;
    obs.append(observateur);
    Configuration::instance()->_observateurs = obs;

    Soleil soleil;
    soleil.CalculPosVit(date);
    soleil.CalculCoordHoriz(observateur, true);
    soleil.CalculCoordTerrestres(observateur);
    soleil.CalculCoordEquat(observateur);
    soleil.CalculLeverMeridienCoucher(date, DateSysteme::SYSTEME_24H, observateur);
    Configuration::instance()->soleil() = soleil;

    Lune lune;
    lune.CalculPosVit(date);
    lune.CalculPhase(soleil);
    lune.CalculCoordHoriz(observateur, true);
    lune.CalculMagnitude(soleil);
    lune.CalculCoordTerrestres(observateur);
    lune.CalculCoordEquat(observateur);
    lune.CalculLeverMeridienCoucher(date, DateSysteme::SYSTEME_24H, observateur);
    lune.CalculDatesPhases(date);
    Configuration::instance()->lune() = lune;

    const QString nomfic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";

    const QStringList listeElem(QStringList () << "25544");
    QMap<QString, ElementsOrbitaux> mapElem = TLE::Lecture(nomfic, Configuration::instance()->dbSatellites(), listeElem);

    Satellite sat(mapElem.first());

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(observateur, true);

    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, &lune, true);
    sat.CalculCoordTerrestres(observateur);
    sat.CalculCoordEquat(observateur);
    sat._magnitude.Calcul(sat.conditionEclipse(), observateur, sat.distance(), sat.hauteur(),
                          sat.elementsOrbitaux().donnees.magnitudeStandard(), true, true);

    sat.CalculElementsOsculateurs(date);
    const Date dateInit = Date(date.jourJulienUTC(), 0., false);
    sat.CalculTracesAuSol(dateInit, 1, true, true);
    sat._phasage.Calcul(sat.elementsOsculateurs(), sat.elementsOrbitaux().no);
    sat.CalculBeta(soleil);
    sat._signal.Calcul(sat.rangeRate(), sat.distance());
    Configuration::instance()->listeSatellites().append(sat);

    osculateurs = new Osculateurs();
    general = new General(nullptr, osculateurs);
    general->show(date);
    general->AffichageLieuObs();

    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "onglet_general.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "onglet_general.txt";

    general->SauveOngletGeneral(ficRes);

    CompareFichiers(ficRes, ficRef);

    Configuration::instance()->FermetureBaseDonneesSatellites();
}
