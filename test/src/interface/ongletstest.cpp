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
 * >    ongletstest.cpp
 *
 * Localisation
 * >    test.interface
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
#include "configuration/configuration.h"
#include "interface/onglets.h"
#include "ui_onglets.h"
#include "librairies/corps/satellite/satellite.h"
#include "ongletstest.h"
#include "test/src/testtools.h"


using namespace TestTools;

Onglets *onglets;

static QDir dir;

void OngletsTest::testAll()
{
    // Initialisations
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + QDir::separator() + "data";
    Corps::InitTabConstellations(dirCommonData);

    const QString dirLocalData = dir.path() + QDir::separator() + QDir::separator() + "data";
    Configuration::instance()->_dirLocalData = dirLocalData;
    Date::Initialisation(dirLocalData);

    Configuration::instance()->LectureDonneesSatellites();

    const Date date(2020, 8, 15, 10, 0, 0., 2. / 24.);

    Observateur observateur("Paris", -002.34864, 48.85339, 30.);
    observateur.CalculPosVit(date);
    QList<Observateur> obs;
    obs.append(observateur);
    Configuration::instance()->setObservateurs(obs);

    Soleil soleil;
    soleil.CalculPosition(date);
    soleil.CalculCoordHoriz(observateur);
    soleil.CalculCoordTerrestres(observateur);
    soleil.CalculCoordEquat(observateur);
    Configuration::instance()->soleil() = soleil;

    Lune lune;
    lune.CalculPosition(date);
    lune.CalculPhase(soleil);
    lune.CalculCoordHoriz(observateur);
    lune.CalculMagnitude(soleil);
    lune.CalculCoordTerrestres(observateur);
    lune.CalculCoordEquat(observateur);
    Configuration::instance()->lune() = lune;

    const QString nomfic = dir.path() + QDir::separator() + "test" + QDir::separator() + "tle" + QDir::separator() + "visual.txt";

    const QStringList listeTLE(QStringList () << "25544");
    const QMap<QString, TLE> mapTLE = TLE::LectureFichier(nomfic, listeTLE);

    Satellite sat(mapTLE.first());

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(observateur);

    sat._conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, lune, true);
    sat.CalculCoordTerrestres(observateur);
    sat.CalculCoordEquat(observateur);
    sat._magnitude.Calcul(sat.conditionEclipse(), observateur, sat.distance(), sat.hauteur(),
                          sat.tle().donnees().magnitudeStandard(), true, true);

    sat.CalculElementsOsculateurs(date);
    const Date dateInit = Date(date.jourJulienUTC(), 0., false);
    sat.CalculTracesAuSol(dateInit, 1, true, true);
    sat._phasage.Calcul(sat.elements(), sat.tle().no());
    sat.CalculBeta(soleil);
    sat._signal.Calcul(sat.rangeRate(), sat.distance());
    Configuration::instance()->listeSatellites().append(sat);

    onglets = new Onglets();
    onglets->show(date);
    onglets->AffichageLieuObs();

    // Verifications des fichiers
    testOngletGeneral();
    testOngletElementsOsculateurs();
    testOngletInformations();

    if (onglets != nullptr) {
        delete onglets;
        onglets = nullptr;
    }
}

void OngletsTest::testOngletGeneral()
{
    qInfo(Q_FUNC_INFO);

    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "onglet_general.txt";
    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + "onglet_general.txt";

    onglets->SauveOngletGeneral(ficRes);

    CompareFichiers(ficRes, ficRef);
}

void OngletsTest::testOngletElementsOsculateurs()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2020, 8, 15, 10, 0, 0., 2. / 24.);

    for(int i=1; i<=4; i++) {

        if (onglets != nullptr) {
            delete onglets;
            onglets = nullptr;
        }

        onglets = new Onglets();
        onglets->ui()->typeParametres->setCurrentIndex(i - 1);

        if (i == 2) {
            onglets->_uniteVitesse = true;
        }

        if (i == 3) {
            onglets->_uniteVitesse = false;
            onglets->ui()->unitesMi->setChecked(true);
        }

        if (i == 4) {
            onglets->show(date);
            onglets->ui()->typeRepere->setCurrentIndex(1);
        }

        onglets->show(date);
        onglets->AffichageLieuObs();

        const QString nomfic = QString("onglet_elements%1.txt").arg(i);
        const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + nomfic;
        const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + nomfic;

        onglets->SauveOngletElementsOsculateurs(ficRes);

        CompareFichiers(ficRes, ficRef);
    }
}

void OngletsTest::testOngletInformations()
{
    qInfo(Q_FUNC_INFO);

    // TODO
    for(int i=1; i<=2; i++) {

        const QString nomfic = QString("onglet_informations%1.txt").arg(i);
        const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + nomfic;
        const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + nomfic;

        onglets->ui()->informations->setCurrentIndex(i - 1);
        if (i == 2) {
            onglets->on_informations_currentChanged(1);
            onglets->on_noradDonneesSat_valueChanged(25544);
            onglets->on_satellitesTrouves_currentRowChanged(0);
        }
        onglets->SauveOngletInformations(ficRes);

        CompareFichiers(ficRes, ficRef);
    }
}
