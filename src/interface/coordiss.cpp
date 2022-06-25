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
 * >    coordiss.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 mars 2022
 *
 * Date de revision
 * >
 *
 */

#include "configuration/configuration.h"
#include "librairies/corps/satellite/evenements.h"
#include "coordiss.h"
#pragma GCC diagnostic ignored "-Wconversion"
#include "ui_coordiss.h"
#pragma GCC diagnostic warning "-Wconversion"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
CoordISS::CoordISS(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CoordISS)
{
    _ui->setupUi(this);

    setPolice();
}


/*
 * Destructeur
 */
CoordISS::~CoordISS()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::CoordISS *CoordISS::ui()
{
    return _ui;
}


/*
 * Methodes publiques
 */
/*
 * Calcul du numero d'orbite de l'ISS
 */
int CoordISS::CalculNumeroOrbiteISS(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int numOrbite = 0;

    /* Corps de la methode */
    if (!Configuration::instance()->listeSatellites().isEmpty()) {

        Satellite sat = Configuration::instance()->listeSatellites().at(0);
        sat.CalculPosVit(date);
        sat.CalculElementsOsculateurs(date);
        Date dateCalcul(date.jourJulienUTC() + sat.elements().periode() * NB_JOUR_PAR_HEUR, 0., false);

        sat.CalculPosVit(dateCalcul);
        sat.CalculCoordTerrestres(dateCalcul);

        Date dateNA = Evenements::CalculNoeudOrbite(dateCalcul, sat, false);
        sat.CalculPosVit(dateNA);
        sat.CalculCoordTerrestres(dateNA);
        double lon1 = sat.longitude();

        bool atrouveOrb = false;
        double lon2;
        while (!atrouveOrb) {

            dateCalcul = Date(dateNA.jourJulienUTC() - NB_JOUR_PAR_MIN, 0., false);
            sat.CalculPosVit(dateCalcul);
            sat.CalculCoordTerrestres(dateCalcul);

            dateNA = Evenements::CalculNoeudOrbite(dateCalcul, sat, false);
            sat.CalculPosVit(dateNA);
            sat.CalculCoordTerrestres(dateNA);
            lon2 = sat.longitude();

            atrouveOrb = ((lon2 < 0.) && (lon1 > 0.));
            numOrbite++;
            lon1 = lon2;
        }
    }

    /* Retour */
    return (numOrbite);
}

/*
 * Affichage des courbes sur la carte du monde
 */
void CoordISS::show(const Date &dateCourante, const Date &dateEcl)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Prochaine transition J/N
    QString chaine = "D/N : %1";
    const double delai = dateEcl.jourJulienUTC() - dateCourante.jourJulienUTC();
    const Date delaiEcl(delai - 0.5, 0.);
    const QString cDelai = (delai >= 0.) ? delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(12, 7) : "0:00:00";
    _ui->nextTransitionISS->setText(chaine.arg(cDelai));

    const int numOrb = CalculNumeroOrbiteISS(dateCourante);

    // Affichage des donnees du blackboard
    chaine = "LAT = %1";
    _ui->latitudeISS->setText(chaine.arg(Configuration::instance()->listeSatellites().at(0).latitude() * RAD2DEG, 0, 'f', 1));
    chaine = "ALT = %1";
    _ui->altitudeISS->setText(chaine.arg(Configuration::instance()->listeSatellites().at(0).altitude() * MILE_PAR_KM, 0, 'f', 1));
    chaine = "LON = %1";
    _ui->longitudeISS->setText(chaine.arg(-Configuration::instance()->listeSatellites().at(0).longitude() * RAD2DEG, 0, 'f', 1));
    chaine = "INC = %1";
    _ui->inclinaisonISS->setText(chaine.arg(Configuration::instance()->listeSatellites().at(0).elements().inclinaison() * RAD2DEG, 0, 'f', 1));
    chaine = "ORB = %1";
    _ui->orbiteISS->setText(chaine.arg(numOrb));
    chaine = "BETA = %1";
    _ui->betaISS->setText(chaine.arg(Configuration::instance()->listeSatellites().at(0).beta() * RAD2DEG, 0, 'f', 1));

    /* Retour */
    return;
}

void CoordISS::setPolice()
{
    _ui->altitudeISS->setFont(Configuration::instance()->policeWcc());
    _ui->betaISS->setFont(Configuration::instance()->policeWcc());
    _ui->inclinaisonISS->setFont(Configuration::instance()->policeWcc());
    _ui->latitudeISS->setFont(Configuration::instance()->policeWcc());
    _ui->longitudeISS->setFont(Configuration::instance()->policeWcc());
    _ui->nextTransitionISS->setFont(Configuration::instance()->policeWcc());
    _ui->orbiteISS->setFont(Configuration::instance()->policeWcc());
}


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */
