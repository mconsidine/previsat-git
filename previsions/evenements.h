/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2018  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    evenements.h
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    23 juillet 2011
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#include "conditions.h"
#include "librairies/corps/satellite/satellite.h"

class Evenements
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculEvenements Calcul des evenements orbitaux
     * @param conditions conditions d'observation
     */
    static void CalculEvenements(const Conditions &conditions);

    /**
     * @brief FinTraitement Finalisation du traitement (liberation memoire)
     */
    static void FinTraitement();

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    /**
     * @brief CalculEphemerides Calcul des ephemerides du satellite
     * @param conditions conditions d'observation
     */
    static void CalculEphemerides(const Conditions &conditions);

    /**
     * @brief CalculEvt Calcul des caracteristiques de l'evenement
     * @param xtab tableau de dates
     * @param ytab tableau des donnees caracterisant l'evenement
     * @param yval valeur de la donnee pour l'evenement
     * @param typeEvt type d'evenement
     * @param conditions conditions d'observation
     * @param sat satellite
     */
    static void CalculEvt(const double xtab[3], const double ytab[3], const double yval, const QString &typeEvt,
                          const Conditions &conditions, Satellite &sat);

};

#endif // EVENEMENTS_H
