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
 * >    transitiss.h
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
 * >    24 juillet 2011
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#ifndef TRANSITISS_H
#define TRANSITISS_H

#include "conditions.h"
#include "librairies/corps/satellite/satellite.h"

class TransitISS
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculTransitsISS Calcul des transits ISS devant la Lune et/ou le Soleil
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param result tableau de resultats
     */
    static void CalculTransitsISS(const Conditions &conditions, Observateur &observateur, QStringList &result);

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
     * @brief CalculAngleMin Calcul de l'angle minimum
     * @param satellite satellite
     * @param observateur observateur
     * @param jjm tableau de dates
     * @param typeCorps type de corps
     * @param minmax date et angle minimum
     */
    static void CalculAngleMin(Satellite &satellite, Observateur &observateur, const double jjm[], const int typeCorps,
                               double minmax[]);

    /**
     * @brief CalculDate Calcul de la date d'une borne du transit ou conjonction
     * @param satellite satellite
     * @param observateur observateur
     * @param jjm tableau de dates
     * @param typeCorps type de corps
     * @param itransit cas d'un transit
     * @param seuilConjonction seuil de conjonction
     * @param dateInter date interpolee
     */
    static void CalculDate(Satellite &satellite, Observateur &observateur, const double jjm[], const int typeCorps,
                           const bool itransit, const double seuilConjonction, double &dateInter);

    /**
     * @brief CalculElements Calcul des elements du transit ou de la conjonction
     * @param satellite satellite
     * @param observateur observateur
     * @param jmax date du maximum
     * @param typeCorps type de corps
     * @param itransit cas d'un transit
     * @param seuilConjonction seuil de conjonction
     * @param dates tableau de dates
     */
    static void CalculElements(Satellite &satellite, Observateur &observateur, const double jmax, const int typeCorps,
                               const bool itransit, const double seuilConjonction, Date dates[]);

    /**
     * @brief CalculEphemSoleilLune Calcul des ephemerides du Soleil et de la Lune
     * @param conditions conditions d'observation
     * @param observateur observateur
     */
    static void CalculEphemSoleilLune(const Conditions &conditions, Observateur &observateur);

};

#endif // TRANSITISS_H
