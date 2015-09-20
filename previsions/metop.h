/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    metop.h
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
 * >    12 septembre 2015
 *
 * Date de revision
 * >
 *
 */

#ifndef METOP_H
#define METOP_H

#include "conditions.h"
#include "flashs.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"


class MetOp : public Flashs
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief AngleReflexion Calcul de l'angle de reflexion du panneau
     * @param satellite satellite
     * @param soleil Soleil
     * @return angle de reflexion du panneau
     */
    static double AngleReflexion(const Satellite &satellite, const Soleil &soleil);

    /**
     * @brief CalculFlashsMetOp Calcul des flashs MetOp
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param result tableau de resultats
     */
    static void CalculFlashsMetOp(const Conditions &conditions, Observateur &observateur, QStringList &result);

    /**
     * @brief CalculMagnitudeMetOp Calcul de la magnitude du flash MetOp
     * @param extinction Prise en compte de l'extinction atmospherique
     * @param satellite satellite
     * @param soleil Soleil
     * @param observateur observateur
     * @return magnitude du flash
     */
    static double CalculMagnitudeMetOp(const bool extinction, const Satellite &satellite, const Soleil &soleil,
                                         const Observateur &observateur);

    /**
     * @brief LectureStatutMetOp Lecture du fichier de statut des satellites MetOp
     * @param tabStsMetOp tableau de statut des satellites MetOp
     * @return nombre de satellites MetOp
     */
    static int LectureStatutMetOp(QStringList &tabStsMetOp);

    /**
     * @brief MagnitudeFlash Determination de la magnitude du flash
     * @param ext prise en compte de l'extinction atmospherique
     * @param angle angle de reflexion
     * @param observateur observateur
     * @param satellite satellite
     * @return valeur de la magnitude du flash
     */
    static double MagnitudeFlash(const bool ext, const double angle, const Observateur &observateur, Satellite &satellite);


    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */


};

#endif // METOP_H