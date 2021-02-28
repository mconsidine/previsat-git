/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    telescope.cpp
 *
 * Localisation
 * >    previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 octobre 2020
 *
 * Date de revision
 * >
 *
 */

#include <cmath>
#include "configuration/configuration.h"
#include "telescope.h"


static ConditionsPrevisions _conditions;
static QStringList _resultats;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Accesseurs
 */
QStringList &Telescope::resultats()
{
    return _resultats;
}

/*
 * Modificateurs
 */
void Telescope::setConditions(const ConditionsPrevisions &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des coordonnees du satellite pour le suivi avec un telescope
 */
int Telescope::CalculSuiviTelescope(int &nombre)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt = "%1,%2,%3,%4";
    _resultats.clear();

    // Creation de la liste de TLE
    const QMap<QString, TLE> tabTle = TLE::LectureFichier(Configuration::instance()->dirLocalData(), _conditions.fichier, _conditions.listeSatellites);

    // Satellite
    Satellite sat(tabTle.first());

    /* Corps de la methode */
    _resultats.append("\"Time (UTCG)\",\"Range (km)\",\"Right Ascen (deg)\",\"Declination (deg)\"");

    Date date = Date(floor(_conditions.jj1 * NB_MILLISEC_PAR_JOUR + 1000. * _conditions.pas+1.) * NB_JOUR_PAR_MILLISEC, 0.);

    do {

        // Position de l'observateur
        _conditions.observateur.CalculPosVit(date);

        // Position du satellite
        sat.CalculPosVit(date);

        // Position topocentrique du satellite
        sat.CalculCoordHoriz(_conditions.observateur);

        // Ascension droite, declinaison
        sat.CalculCoordEquat(_conditions.observateur, false);

        _resultats.append(fmt.arg(date.ToShortDateAMJmillisec()).arg(sat.distance(), 16, 'f', 6).
                          arg(sat.ascensionDroite() * RAD2DEG, 16, 'f', 6).arg(sat.declinaison() * RAD2DEG, 16, 'f', 6));

        date = Date(floor(date.jourJulienUTC() * NB_MILLISEC_PAR_JOUR + 1000. * _conditions.pas) * NB_JOUR_PAR_MILLISEC, 0.);

    } while (date.jourJulienUTC() <= _conditions.jj2);

    /* Retour */
    return nombre;
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

