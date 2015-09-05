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
 * >    5 septembre 2015
 *
 */

#ifndef IRIDIUM_H
#define IRIDIUM_H

#include "conditions.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"

class Iridium
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
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
     * @param satellite satellite
     * @param soleil Soleil
     * @param observateur observateur
     * @return magnitude du flash
     */
    static double CalculMagnitudeIridium(const bool extinction, const Satellite &satellite, const Soleil &soleil,
                                         const Observateur &observateur);

    /**
     * @brief FinTraitement Finalisation du traitement (liberation memoire)
     */
    static void FinTraitement();

    /**
     * @brief LectureStatutIridium Lecture du fichier de statut des satellites Iridium
     * @param ope
     * @param tabStsIri
     * @return
     */
    static int LectureStatutIridium(const char ope, QStringList &tabStsIri);

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
     * @brief AngleReflexion Calcul de l'angle de reflexion du panneau
     * @param satellite satellite
     * @param soleil Soleil
     * @return angle de reflexion du panneau
     */
    static double AngleReflexion(const Satellite &satellite, const Soleil &soleil);

    /**
     * @brief CalculAngleMin Calcul de l'angle minimum du panneau
     * @param jjm tableau de dates
     * @param satellite satellite
     * @param observateur observateur
     * @param soleil Soleil
     * @param minmax tableau de la date et de l'angle minimum
     */
    static void CalculAngleMin(const double jjm[], Satellite &satellite, Observateur &observateur, Soleil &soleil, double minmax[]);

    /**
     * @brief CalculEphemSoleilObservateur Calcul des ephemerides du Soleil et de la position de l'observateur
     * @param conditions conditions d'observation
     * @param observateur observateur
     */
    static void CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur);

    /**
     * @brief CalculLimitesFlash Calcul des bornes inferieures et superieures du flash
     * @param mgn0 magnitude maximale
     * @param dateMaxFlash date du maximum du flash
     * @param conditions conditions d'observation
     * @param satellite satellite
     * @param observateur observateur
     * @param soleil Soleil
     * @param lim dates limites du flash
     */
    static void CalculLimitesFlash(const double mgn0, const double dateMaxFlash, const Conditions &conditions,
                                   Satellite &satellite, Observateur &observateur, Soleil &soleil, Date lim[]);

    /**
     * @brief DeterminationFlash Determination du flash
     * @param minmax date et angle de reflexion
     * @param sts statut du satellite Iridium
     * @param conditions conditions d'observation
     * @param temp date temporaire
     * @param observateur observateur
     * @param sat satellite
     * @param soleil Soleil
     */
    static void DeterminationFlash(const double minmax[2], const QString &sts, const Conditions &conditions, double &temp,
                                   Observateur &observateur, Satellite &sat, Soleil &soleil);

    /**
     * @brief EcrireFlash Ecriture d'une ligne de flash
     * @param date date
     * @param i numero de ligne du flash
     * @param altitude altitude
     * @param angref angle de reflexion
     * @param mag magnitude
     * @param sts statut du satellite Iridium
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param soleil Soleil
     * @param sat satellite
     * @return ligne du flash
     */
    static QString EcrireFlash(const Date &date, const int i, const double altitude, const double angref, const double mag,
                               const QString &sts, const Conditions &conditions, const Observateur &observateur,
                               const Soleil &soleil, Satellite &sat);

    /**
     * @brief LimiteFlash Calcul d'une limite du flash
     * @param mgn0 magnitude maximale
     * @param jjm tableau de dates
     * @param conditions conditions d'observation
     * @param satellite satellite
     * @param observateur observateur
     * @param soleil Soleil
     * @param limite date et valeur limite du flash
     */
    static void LimiteFlash(const double mgn0, const double jjm[], const Conditions &conditions, Satellite &satellite,
                            Observateur &observateur, Soleil &soleil, double limite[]);

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

    static Matrice3D RotationRV(const Vecteur3D &position, const Vecteur3D &vitesse, const double lacet,
                                const double tangage, const int inpl);

};

#endif // IRIDIUM_H
