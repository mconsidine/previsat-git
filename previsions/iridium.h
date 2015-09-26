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
 * >    iridium.h
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
 * >    17 juillet 2011
 *
 * Date de revision
 * >    26 septembre 2015
 *
 */

#ifndef IRIDIUM_H
#define IRIDIUM_H

#include "conditions.h"
#include "flashs.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"

class Iridium : public Flashs
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
     * @brief CalculFlashsIridium Calcul des flashs Iridium
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param result tableau de resultats
     */
    static void CalculFlashsIridium(const Conditions &conditions, Observateur &observateur, QStringList &result);

    /**
     * @brief CalculMagnitudeIridium Calcul de la magnitude du flash Iridium
     * @param extinction Prise en compte de l'extinction atmospherique
     * @param ope Prise en compte des satellites operationnels uniquement
     * @param tabSts tableau de statut des satellites Iridium
     * @param satellite satellite
     * @param soleil Soleil
     * @param observateur observateur
     * @return magnitude du flash
     */
    static double CalculMagnitudeIridium(const bool extinction, const bool ope, const QStringList &tabSts, const Satellite &satellite,
                                         const Soleil &soleil, const Observateur &observateur);

    /**
     * @brief LectureStatutIridium Lecture du fichier de statut des satellites Iridium
     * @param ope prise en compte des satellites operationnels
     * @param tabStsIri tableau de statut des satellites Iridium
     * @return nombre de satellites Iridium contenus dans le fichier de statut
     */
    static int LectureStatutIridium(const char ope, QStringList &tabStsIri);

    /**
     * @brief MagnitudeFlash Determination de la magnitude du flash
     * @param ext prise en compte de l'extinction atmospherique
     * @param angle angle de reflexion
     * @param observateur observateur
     * @param soleil Soleil
     * @param satellite satellite
     * @return valeur de la magnitude du flash
     */
    static double MagnitudeFlash(const bool ext, const double angle, const Observateur &observateur, const Soleil &soleil,
                                 Satellite &satellite);


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

#endif // IRIDIUM_H
