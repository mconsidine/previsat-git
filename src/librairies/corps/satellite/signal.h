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
 * >    27 septembre 2022
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
     * Methodes publiques
     */
    /**
     * @brief Calcul Calcul des elements du signal
     * @param[in] rangeRate taux de variation de la distance au satellite (km/s)
     * @param[in] distance distance au satellite (km)
     * @param[in] frequence frequence (Hz). Par defaut 100 MHz
     */
    void Calcul(const double rangeRate,
                const double distance,
                const double frequence = 100.e6);


    /*
     * Accesseurs
     */
    double attenuation() const;
    double delai() const;
    double doppler() const;


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
    double _attenuation;
    double _delai;
    double _doppler;

    /*
     * Methodes privees
     */


};

#endif // SIGNAL_H
