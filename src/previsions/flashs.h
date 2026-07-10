/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    flashs.h
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
 * >    11 juillet 2011
 *
 * Date de revision
 * >    25 fevrier 2023
 *
 */

#ifndef FLASHS_H
#define FLASHS_H

#include "previsionsconst.h"


class ConditionEclipse;
class Satellite;
class Soleil;
struct EphemeridesFlashs;

class Flashs
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Accesseurs
     */
    static QMap<QString, QList<QList<ResultatPrevisions> > > &resultats();
    static DonneesPrevisions &donnees();


    /*
     * Modificateurs
     */
    static void setConditions(const ConditionsPrevisions &conditions);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculFlashs Determination des flashs
     * @param[in/out] nombre nombre de satellites
     * @return nombre de satellites
     */
    static int CalculFlashs(const int &nombre);

    /**
     * @brief CalculMagnitudeFlash Calcul de la magnitude du flash (pour l'affichage dans l'onglet General)
     * @param[in] date date
     * @param[in] satellite satellite
     * @param[in] soleil Soleil
     * @param[in] calcEclipseLune prise en compte des eclipses par la Lune
     * @param[in] refraction prise en compte de la refraction
     * @return magnitude du flash
     */
    static double CalculMagnitudeFlash(const Date &date,
                                       const Satellite &satellite,
                                       const Soleil &soleil,
                                       const bool calcEclipseLune,
                                       const bool refraction);

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
    /**
     * @brief AngleReflexion Calcul de l'angle de reflexion du panneau
     * @param[in] satellite satellite
     * @param[in] soleil Soleil
     * @return angle de reflexion du panneau
     */
    static double AngleReflexion(const Satellite &satellite,
                                 const Soleil &soleil);

    /**
     * @brief CalculAngleMin Calcul de l'angle minimum du panneau
     * @param[in] jjm tableau de dates
     * @param[in/out] satellite satellite
     * @param[in/out] soleil Soleil
     * @return tableau de la date et de l'angle minimum
     */
    static QPointF CalculAngleMin(const std::array<double, MATHS::DEGRE_INTERPOLATION> jjm,
                                  Satellite &satellite,
                                  Soleil &soleil);

    /**
     * @brief CalculEphemSoleilObservateur Calcul des ephemerides du Soleil et de l'observateur
     * @return tableau d'ephemerides
     */
    static QList<EphemeridesFlashs> CalculEphemSoleilObservateur();

    /**
     * @brief CalculLimitesFlash Calcul des bornes inferieures et superieures du flash
     * @param[in] mgn0 magnitude maximale
     * @param[in] dateMaxFlash date du maximum du flash
     * @param[in/out] satellite satellite
     * @param[in/out] soleil Soleil
     * @param[out] lim dates limites du flash
     */
    static void CalculLimitesFlash(const double mgn0,
                                   const double dateMaxFlash,
                                   Satellite &satellite,
                                   Soleil &soleil,
                                   std::array<Date, 3> &lim);

    /**
     * @brief DeterminationFlash Determination du flash
     * @param[in] minmax date et angle de reflexion
     * @param[out] temp date temporaire
     * @param[in/out] sat satellite
     * @param[in/out] soleil Soleil
     */
    static void DeterminationFlash(const QPointF minmax,
                                   double &temp,
                                   Satellite &sat,
                                   Soleil &soleil);

    /**
     * @brief LimiteFlash Calcul d'une limite du flash
     * @param[in] mgn0 magnitude maximale
     * @param[in] jjm tableau de dates
     * @param[in/out] satellite satellite
     * @param[in/out] soleil Soleil
     * @param[out] limite date et valeur limite du flash
     */
    static void LimiteFlash(const double mgn0,
                            const std::array<double, MATHS::DEGRE_INTERPOLATION> jjm,
                            Satellite &satellite,
                            Soleil &soleil,
                            std::array<double, 4> &limite);

    /**
     * @brief MagnitudeFlash Determination de la magnitude du flash
     * @param[in] angle angle de reflexion
     * @param[in] condEcl conditions d'eclipse
     * @param[in/out] satellite satellite
     * @return valeur de la magnitude du flash
     */
    static double MagnitudeFlash(const double angle,
                                 const ConditionEclipse &condEcl,
                                 const Satellite &satellite);

    /**
     * @brief RotationRV Calcul de la matrice de rotation du repere equatorial au repere orbital local
     * D'apres l'algorithme developpe dans SatCal de Gerhard Holtkamp
     * http://www.aaw-darmstadt.de/websystem/_main.php?page=download.php
     * @param[in] position position du satellite (repere equatorial)
     * @param[in] vitesse vitesse du satellite
     * @param[in] lacet angle de lacet (en radians)
     * @param[in] tangage angle de tangage (en radians)
     * @param[in] inpl rotation plan LVLH (inpl = 0 : pas de rotation, x pointe selon le vecteur vitesse;
     *                                     inpl = 1 : rotation de 90° autour de l'axe X;
     *                                     inpl = 2 : rotation LVLH)
     * @return matrice de rotation
     */
    static Matrice3D RotationRV(const Vecteur3D &position,
                                const Vecteur3D &vitesse,
                                const double lacet,
                                const double tangage,
                                const int inpl);

    /**
     * @brief RotationYawSteering Calcul de la matrice de rotation du repere equatorial au repere defini par la loi locale de yaw steering
     * D'apres l'algorithme developpe dans SatCal de Gerhard Holtkamp
     * http://www.aaw-darmstadt.de/websystem/_main.php?page=download.php
     * @param[in] satellite satellite
     * @param[in] lacet angle de lacet (en radians)
     * @param[in] tangage angle de tangage (en radians)
     * @return matrice de rotation
     */
    static Matrice3D RotationYawSteering(const Satellite &satellite,
                                         const double lacet,
                                         const double tangage);

};

#endif // FLASHS_H
