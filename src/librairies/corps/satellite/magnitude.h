/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    magnitude.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul de la magnitude visuelle d'un satellite artificiel
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 septembre 2016
 *
 * Date de revision
 * >    21 septembre 2022
 *
 */

#ifndef MAGNITUDE_H
#define MAGNITUDE_H


class ConditionEclipse;
class Observateur;
class Satellite;

class Magnitude
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Magnitude Constructeur par defaut
     */
    Magnitude();


    /*
     * Methodes publiques
     */
    /**
     * @brief Calcul Calcul de la magnitude visuelle du satellite
     * @param[in] conditionEclipse condition d'eclipse du satellite
     * @param[in] observateur observateur
     * @param[in] distance distance au satellite
     * @param[in] hauteur hauteur du satellite
     * @param[in] magnitudeStandard magnitude standard du satellite
     * @param[in] extinction prise en compte de l'extinction atmospherique
     * @param[in] effetEclipsePartielle prise en compte de l'effet des eclipses partielles
     */
    void Calcul(const ConditionEclipse &conditionEclipse, const Observateur &observateur, const double distance, const double hauteur,
                const double magnitudeStandard, const bool extinction = true, const bool effetEclipsePartielle = true);

    /**
     * @brief ExtinctionAtmospherique Determination de l'extinction atmospherique
     * issu de l'article "Magnitude corrections for atmospheric extinction" de Daniel Green, 1992
     * @param[in] observateur observateur
     * @param[in] hauteur hauteur du satellite
     * @return correction de magnitude correspondant a l'extinction atmospherique
     */
    double ExtinctionAtmospherique(const Observateur &observateur, const double hauteur) const;


    /*
     * Accesseurs
     */
    double fractionIlluminee() const;
    double magnitude() const;


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
    double _fractionIlluminee;
    double _magnitude;

    /*
     * Methodes privees
     */


};

#endif // MAGNITUDE_H
