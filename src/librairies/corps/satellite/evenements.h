/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des evenements lies au satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 janvier 2020
 *
 * Date de revision
 * >    17 octobre 2022
 *
 */

#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QObject>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "evenementsconst.h"
#include "librairies/dates/date.h"


class Satellite;
class Observateur;

class Evenements
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculAOS Calcul de l'AOS (ou LOS) suivant ou precedent
     * @param dateInit date initiale
     * @param satellite satellite
     * @param observateur observateur
     * @param sensCalcul sens de calcul (vrai pour l'AOS suivant)
     * @param hauteurMin hauteur minimale du satellite
     * @param refraction prise en compte de la refraction
     * @return elements de l'AOS (ou LOS) suivant ou precedent
     */
    static ElementsAOS CalculAOS(const Date &dateInit, const Satellite &satellite, const Observateur &observateur,
                                 const SensCalcul &sensCalcul = SensCalcul::CHRONOLOGIQUE, const double hauteurMin = 0., const bool refraction = true);

    /**
     * @brief CalculNoeudOrbite Calcul du noeud (ascendant ou descendant) precedent ou suivant
     * @param dateInit date initiale
     * @param satellite satellite
     * @param sensCalcul sens du calcul (vrai pour le noeud suivant)
     * @param typeNoeud noeud ascendant ou noeud descendant
     * @return date du noeud (ascendant ou descendant) precedent ou suivant
     */
    static Date CalculNoeudOrbite(const Date &dateInit, const Satellite &satellite, const SensCalcul &sensCalcul = SensCalcul::CHRONOLOGIQUE,
                                  const TypeNoeudOrbite &typeNoeud = TypeNoeudOrbite::NOEUD_ASCENDANT);

    /**
     * @brief CalculOmbrePenombre Calcul du prochain passage ombre->penombre ou penombre->ombre
     * @param dateInit date initiale
     * @param satellite satellite
     * @param nbTrajectoires nombre de trajectoires
     * @param acalcEclipseLune calcul des eclipses produites par la Lune
     * @param refraction prise en compte de la refraction
     * @return date du prochain passage ombre->penombre ou penombre->ombre
     */
    static Date CalculOmbrePenombre(const Date &dateInit, const Satellite &satellite, const int nbTrajectoires, const bool acalcEclipseLune,
                                    const bool refraction);

    /*
     * Accesseurs
     */


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */

    /*
     * Methodes privees
     */


};

#endif // EVENEMENTS_H
