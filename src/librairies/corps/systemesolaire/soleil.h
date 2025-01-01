/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    soleil.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position du Soleil
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    20 juillet 2024
 *
 */

#ifndef SOLEIL_H
#define SOLEIL_H

#include "librairies/corps/corps.h"
#include "soleilconst.h"


class Soleil : public Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Soleil Constructeur par defaut
     */
    Soleil();

    /**
     * @brief Soleil Constructeur a partir du vecteur position
     * @param[in] pos vecteur position du Soleil
     */
    explicit Soleil(const Vecteur3D &pos);

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculPositionSimp Calcul simplifie de la position du Soleil
     * @cite Astronomical Algorithms 2nd edition de Jean Meeus, p163-164
     * @param[in] date date
     */
    void CalculPositionSimp(const Date &date);

    /**
     * @brief CalculPosVit Calcul de la position-vitesse du corps
     * @param[in] date date
     */
    void CalculPosVit(const Date &date) override {
        CalculPositionSimp(date);
    }


    /*
     * Accesseurs
     */
    double distanceUA() const;


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
    double _distanceUA;


    /*
     * Methodes privees
     */

    /**
     * @brief CalculEphemLeverMeridienCoucher Calcul des ephemerides du Soleil pour determiner les heures de lever/meriden/coucher
     * @param[in] date date
     * @param[in] observateur observateur
     */
    void CalculEphemLeverMeridienCoucher(const Date &date,
                                         const Observateur &observateur) override;


};


#endif // SOLEIL_H
