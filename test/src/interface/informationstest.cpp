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
 * >    informationstest.cpp
 *
 * Localisation
 * >    test.interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    29 octobre 2022
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
#include "informationstest.h"
#include "configuration/configuration.h"
#include "interface/onglets/donnees/informationssatellite.h"
#include "interface/onglets/donnees/recherchesatellite.h"
#include "librairies/corps/satellite/tle.h"
#include "testtools.h"


using namespace TestTools;


void InformationsTest::testAll()
{
    testSauveOngletInformations();
}

void InformationsTest::testSauveOngletInformations()
{
    qInfo(Q_FUNC_INFO);

    InformationsSatellite *informations = new InformationsSatellite();
    RechercheSatellite *recherche = new RechercheSatellite();

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

    Configuration::instance()->LectureDonneesSatellites();

    const QString nomfic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";

    const int lgrec = Configuration::instance()->lgRec();
    const QStringList listeElem(QStringList () << "25544");
    QMap<QString, ElementsOrbitaux> mapElem = TLE::Lecture(nomfic, Configuration::instance()->donneesSatellites(), lgrec, listeElem);

    Satellite sat(mapElem.first());
    Configuration::instance()->listeSatellites().append(sat);

    for(int i=1; i<=2; i++) {

        const QString fic = QString("onglet_informations%1.txt").arg(i);
        const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + fic;
        const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + fic;

        if (i == 1) {
            informations->show(sat);
            informations->SauveOngletInformations(ficRes);
        } else {
            recherche->on_noradDonneesSat_valueChanged(25544);
            recherche->SauveOngletRecherche(ficRes);
        }

        CompareFichiers(ficRes, ficRef);
    }
}
