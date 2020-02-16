/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    signal.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des elements du signal
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 septembre 2016
 *
 * Date de revision
 * >
 *
 */

#ifndef SIGNAL_H
#define SIGNAL_H


class Signal
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Signal Constructeur par defaut
     */
    Signal();

    /*
     * Accesseurs
     */
    double attenuation() const;
    double delai() const;
    double doppler() const;

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Calcul Calcul des elements du signal
     * @param rangeRate taux de variation de la distance au satellite
     * @param distance distance au satellite
     */
    void Calcul(const double rangeRate, const double distance);


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
    double _attenuation;
    double _delai;
    double _doppler;

    /*
     * Methodes privees
     */


};

#endif // SIGNAL_H
