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
 * >    prevision.h
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
 * >    3 juin 2015
 *
 */

#ifndef FLASHS_H
#define FLASHS_H

#include "previsionsconst.h"


class ConditionEclipse;
class Satellite;
class Soleil;
struct Ephemerides;

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
    static DonneesPrevisions donnees();

    /*
     * Modificateurs
     */
    static void setConditions(const ConditionsPrevisions &conditions);

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    static int CalculFlashs(int &nombre);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */

    /*
     * Methodes privees
     */
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
    static QPair<double, double> CalculAngleMin(const QList<double> jjm, Satellite &satellite, Soleil &soleil);

    /**
     * @brief CalculEphemSoleilObservateur Calcul des ephemerides du Soleil et de l'observateur
     * @return tableau d'ephemerides
     */
    static QList<Ephemerides> CalculEphemSoleilObservateur();

    /**
     * @brief CalculLimitesFlash Calcul des bornes inferieures et superieures du flash
     * @param mgn0 magnitude maximale
     * @param dateMaxFlash date du maximum du flash
     * @param satellite satellite
     * @param soleil Soleil
     * @param lim dates limites du flash
     */
    static void CalculLimitesFlash(const double mgn0, const double dateMaxFlash, Satellite &satellite, Soleil &soleil, Date lim[]);

    /**
     * @brief DeterminationFlash Determination du flash
     * @param minmax date et angle de reflexion
     * @param temp date temporaire
     * @param sat satellite
     * @param soleil Soleil
     */
    static void DeterminationFlash(const QPair<double, double> minmax, double &temp, Satellite &sat, Soleil &soleil);

    /**
     * @brief LimiteFlash Calcul d'une limite du flash
     * @param mgn0 magnitude maximale
     * @param jjm tableau de dates
     * @param satellite satellite
     * @param soleil Soleil
     * @param limite date et valeur limite du flash
     */
    static void LimiteFlash(const double mgn0, const QList<double> jjm, Satellite &satellite, Soleil &soleil, double limite[]);

    /**
     * @brief MagnitudeFlash Determination de la magnitude du flash
     * @param angle angle de reflexion
     * @param satellite satellite
     * @return valeur de la magnitude du flash
     */
    static double MagnitudeFlash(const double angle, const ConditionEclipse &condEcl, Satellite &satellite);

    /**
     * @brief RotationRV Calcul de la matrice de rotation du repere equatorial au repere orbital local
     * D'apres l'algorithme developpe dans SatCal de Gerhard Holtkamp
     * http://www.aaw-darmstadt.de/websystem/_main.php?page=download.php
     * @param position position du satellite (repere equatorial)
     * @param vitesse vitesse du satellite
     * @param lacet angle de lacet (en radians)
     * @param tangage angle de tangage (en radians)
     * @param inpl rotation plan LVLH (inpl = 0 : pas de rotation, x pointe selon le vecteur vitesse;
     *                                 inpl = 1 : rotation de 90° autour de l'axe X;
     *                                 inpl = 2 : rotation LVLH)
     * @return matrice de rotation
     */
    static Matrice3D RotationRV(const Vecteur3D &position, const Vecteur3D &vitesse, const double lacet, const double tangage, const int inpl);

    /**
     * @brief RotationYawSteering Calcul de la matrice de rotation du repere equatorial au repere defini par la loi locale de yaw steering
     * D'apres l'algorithme developpe dans SatCal de Gerhard Holtkamp
     * http://www.aaw-darmstadt.de/websystem/_main.php?page=download.php
     * @param satellite satellite
     * @param lacet angle de lacet (en radians)
     * @param tangage angle de tangage (en radians)
     * @return matrice de rotation
     */
    static Matrice3D RotationYawSteering(const Satellite &satellite, const double lacet, const double tangage);


};

#endif // FLASHS_H
