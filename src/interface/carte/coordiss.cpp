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
 * >    coordiss.cpp
 *
 * Localisation
 * >    interface.carte
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 mars 2022
 *
 * Date de revision
 * >    29 janvier 2025
 *
 */

#include "ui_coordiss.h"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/exceptions/exception.h"
#include "coordiss.h"


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

    try {

        qInfo() << "Début Initialisation" << metaObject()->className();

        setPolice();

        qInfo() << "Fin   Initialisation" << metaObject()->className();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
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

        Satellite sat = Configuration::instance()->listeSatellites().first();
        sat.CalculPosVit(date);
        sat.CalculElementsOsculateurs(date);
        Date dateCalcul(date.jourJulienUTC() + sat.elementsOsculateurs().periode() * DATE::NB_JOUR_PAR_HEUR, 0., false);

        sat.CalculPosVit(dateCalcul);
        sat.CalculCoordTerrestres(dateCalcul);

        Date dateNA = Evenements::CalculNoeudOrbite(dateCalcul, sat, SensCalcul::ANTI_CHRONOLOGIQUE);
        sat.CalculPosVit(dateNA);
        sat.CalculCoordTerrestres(dateNA);
        double lon1 = sat.longitude();

        bool atrouveOrb = false;
        while (!atrouveOrb) {

            dateCalcul = Date(dateNA.jourJulienUTC() - DATE::NB_JOUR_PAR_MIN, 0., false);
            sat.CalculPosVit(dateCalcul);
            sat.CalculCoordTerrestres(dateCalcul);

            dateNA = Evenements::CalculNoeudOrbite(dateCalcul, sat, SensCalcul::ANTI_CHRONOLOGIQUE);
            sat.CalculPosVit(dateNA);
            sat.CalculCoordTerrestres(dateNA);
            const double lon2 = sat.longitude();

            atrouveOrb = ((lon2 < 0.) && (lon1 > 0.));
            numOrbite++;
            lon1 = lon2;
        }
    }

    /* Retour */
    return (numOrbite);
}

/*
 * Affichage des coordonnees ISS
 */
void CoordISS::show(const Date &dateCourante, const Date &dateEcl)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Satellite &satellite = Configuration::instance()->listeSatellites().first();

    /* Corps de la methode */
    // Prochaine transition J/N
    QString chaine = "D/N : %1";
    const double delai = dateEcl.jourJulienUTC() - dateCourante.jourJulienUTC();
    const Date delaiEcl(delai - 0.5, 0.);
    const QString cDelai = (delai >= 0.) ? delaiEcl.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(12, 7) : "0:00:00";
    _ui->nextTransitionISS->setText(chaine.arg(cDelai));

    const int numOrb = CalculNumeroOrbiteISS(dateCourante);

    // Affichage des donnees du blackboard
    chaine = "LAT = %1";
    _ui->latitudeISS->setText(chaine.arg(satellite.latitude() * MATHS::RAD2DEG, 0, 'f', 1));
    chaine = "ALT = %1";
    _ui->altitudeISS->setText(chaine.arg(satellite.altitude() * TERRE::MILE_PAR_KM, 0, 'f', 1));
    chaine = "LON = %1";
    _ui->longitudeISS->setText(chaine.arg(-satellite.longitude() * MATHS::RAD2DEG, 0, 'f', 1));
    chaine = "INC = %1";
    _ui->inclinaisonISS->setText(chaine.arg(satellite.elementsOsculateurs().inclinaison() * MATHS::RAD2DEG, 0, 'f', 1));
    chaine = "ORB = %1";
    _ui->orbiteISS->setText(chaine.arg(numOrb));
    chaine = "BETA = %1";
    _ui->betaISS->setText(chaine.arg(satellite.beta() * MATHS::RAD2DEG, 0, 'f', 1));

    /* Retour */
    return;
}

void CoordISS::setPolice()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFont &police = Configuration::instance()->policeWcc();

    /* Corps de la methode */
    _ui->altitudeISS->setFont(police);
    _ui->betaISS->setFont(police);
    _ui->inclinaisonISS->setFont(police);
    _ui->latitudeISS->setFont(police);
    _ui->longitudeISS->setFont(police);
    _ui->nextTransitionISS->setFont(police);
    _ui->orbiteISS->setFont(police);

    /* Retour */
    return;
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

