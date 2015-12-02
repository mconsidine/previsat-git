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
 * >    12 septembre 2015
 *
 * Date de revision
 * >
 *
 */

#ifndef FLASHS_H
#define FLASHS_H

#include "conditions.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"

// Pas de calcul ou d'interpolation
static const double PAS0 = NB_JOUR_PAR_MIN;
static const double PAS1 = 10. * NB_JOUR_PAR_SEC;
static const double PAS_INT0 = 30. * NB_JOUR_PAR_SEC;
static const double PAS_INT1 = 2. * NB_JOUR_PAR_SEC;
static const double TEMPS1 = 16. * NB_JOUR_PAR_MIN;
static const double TEMPS2 = 76. * NB_JOUR_PAR_MIN;

// Pour le calcul de l'assombrissement sur le bord pour la magnitude du Soleil
static const double TAB_INT[] = { 0.3, 0.93, -0.23 };


class Flashs
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculFlashs Calcul des flashs
     * @param idsat identifiant du satellite
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param result tableau de resultats
     */
    static void CalculFlashs(const QString idsat, const Conditions &conditions, Observateur &observateur, QStringList &result);

    /**
     * @brief FinTraitement Finalisation du traitement (liberation memoire)
     */
    static void FinTraitement();

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */
    static char _mir;
    static int _pan;
    static bool _psol;
    static double _surf;
    static Vecteur3D _direction;

    static QString _sts;
    static QStringList _res;
    static QList<Satellite> _sats;
    static QVector<TLE> _tabtle;
    static QList<QVector<double > > _tabEphem;


    /* Methodes protegees */
    /**
     * @brief CalculAngleMin Calcul de l'angle minimum du panneau
     * @param jjm tableau de dates
     * @param typeCalc type de calcul (IRIDIUM ou METOP)
     * @param satellite satellite
     * @param observateur observateur
     * @param soleil Soleil
     * @param minmax tableau de la date et de l'angle minimum
     */
    static void CalculAngleMin(const double jjm[], const TypeCalcul typeCalc, Satellite &satellite, Observateur &observateur,
                               Soleil &soleil, double minmax[]);

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
     * @param conditions conditions d'observation
     * @param temp date temporaire
     * @param observateur observateur
     * @param sat satellite
     * @param soleil Soleil
     */
    static void DeterminationFlash(const double minmax[2], const Conditions &conditions, double &temp,
                                   Observateur &observateur, Satellite &sat, Soleil &soleil);

    /**
     * @brief EcrireFlash Ecriture d'une ligne de flash
     * @param date date
     * @param i numero de ligne du flash
     * @param altitude altitude
     * @param angref angle de reflexion
     * @param mag magnitude
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param soleil Soleil
     * @param sat satellite
     * @return ligne du flash
     */
    static QString EcrireFlash(const Date &date, const int i, const double altitude, const double angref, const double mag,
                               const Conditions &conditions, const Observateur &observateur, const Soleil &soleil, Satellite &sat);

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
     * @brief RotationRV Calcul de la matrice de rotation du repere equatorial au repere orbital local
     * @param position position du satellite (repere equatorial)
     * @param vitesse vitesse du satellite
     * @param lacet angle de lacet (en radians)
     * @param tangage angle de tangage (en radians)
     * @param inpl rotation plan LVLH (inpl = 0 : pas de rotation, x pointe selon le vecteur vitesse;
     *                                 inpl = 1 : rotation de 90° autour de l'axe X;
     *                                 inpl = 2 : rotation LVLH)
     * @return matrice de rotation
     */
    static Matrice3D RotationRV(const Vecteur3D &position, const Vecteur3D &vitesse, const double lacet, const double tangage,
                                const int inpl);

    /**
     * @brief RotationYawSteering Calcul de la matrice de rotation du repere equatorial au repere defini par la loi locale de
     *  yaw steering
     * @param satellite satellite
     * @param lacet angle de lacet (en radians)
     * @param tangage angle de tangage (en radians)
     * @return matrice de rotation
     */
    static Matrice3D RotationYawSteering(const Satellite &satellite, const double lacet, const double tangage);

private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    /**
     * @brief AngleReflexion Calcul de l'angle de reflexion du panneau
     * @param typeCalc type de calcul (IRIDIUM ou METOP)
     * @param satellite satellite
     * @param soleil Soleil
     * @return angle de reflexion du panneau
     */
    static double AngleReflexion(const TypeCalcul typeCalc, const Satellite &satellite, const Soleil &soleil);

    /**
     * @brief MagnitudeFlash Determination de la magnitude du flash
     * @param typeCalc type de calcul (IRIDIUM ou METOP)
     * @param ext prise en compte de l'extinction atmospherique
     * @param angle angle de reflexion
     * @param observateur observateur
     * @param soleil Soleil
     * @param satellite satellite
     * @return valeur de la magnitude du flash
     */
    static double MagnitudeFlash(const TypeCalcul typeCalc, const bool ext, const double angle, const Observateur &observateur,
                                 const Soleil &soleil, Satellite &satellite);


};

#endif // FLASHS_H
