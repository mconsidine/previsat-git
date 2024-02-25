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
 * >    osculateurstest.cpp
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
#include "osculateurstest.h"
#include "../../../src/interface/onglets/osculateurs/ui_osculateurs.h"
#include "configuration/configuration.h"
#include "interface/onglets/general/general.h"
#include "interface/onglets/osculateurs/osculateurs.h"
#include "librairies/corps/satellite/tle.h"
#include "testtools.h"


using namespace TestTools;

void OsculateursTest::testAll()
{
    testSauveOngletOsculateurs();
}

void OsculateursTest::testSauveOngletOsculateurs()
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

    Configuration::instance()->LectureDonneesSatellites();

    const Date date(2020, 8, 15, 10, 0, 0., 2. / 24.);

    Observateur observateur("Paris", -2.34864, 48.85339, 30.);
    observateur.CalculPosVit(date);
    QList<Observateur> obs;
    obs.append(observateur);
    Configuration::instance()->_observateurs = obs;

    const QString nomfic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.txt";

    const int lgrec = Configuration::instance()->lgRec();
    const QStringList listeElem(QStringList () << "25544");
    QMap<QString, ElementsOrbitaux> mapElem = TLE::Lecture(nomfic, Configuration::instance()->donneesSatellites(), lgrec, listeElem);

    Satellite sat(mapElem.first());

    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(observateur, true);

    sat.CalculElementsOsculateurs(date);
    sat.CalculLatitude();
    sat.CalculAltitude();
    const Date dateInit = Date(date.jourJulienUTC(), 0., false);
    sat.CalculTracesAuSol(dateInit, 1, true, true);
    sat._phasage.Calcul(sat.elementsOsculateurs(), sat.elementsOrbitaux().no);
    sat._signal.Calcul(sat.rangeRate(), sat.distance());
    Configuration::instance()->listeSatellites().append(sat);


    for(int i=1; i<=4; i++) {

        EFFACE_OBJET(osculateurs);
        EFFACE_OBJET(general);

        osculateurs = new Osculateurs();
        general = new General(nullptr, osculateurs);

        osculateurs->ui()->typeParametres->setCurrentIndex(i - 1);

        if (i == 2) {
            general->_uniteVitesse = true;
        }

        if (i == 3) {
            general->_uniteVitesse = false;
        }

        osculateurs->show(date);
        general->AffichageDate(date);
        general->AffichageVitesses(date);

        const QString fic = QString("onglet_elements%1.txt").arg(i);
        const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + fic;
        const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + fic;

        osculateurs->SauveOngletElementsOsculateurs(ficRes);

        CompareFichiers(ficRes, ficRef);
    }
}
