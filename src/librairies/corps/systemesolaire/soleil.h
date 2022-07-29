/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    14 juin 2022
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
     * @param pos vecteur position du Soleil
     */
    explicit Soleil(const Vecteur3D &pos);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculLeverMeridienCoucher Calcul des heures de lever/coucher/passage au meridien/crepuscules pour une date donnee
     * @param date date
     * @param observateur observateur
     */
    void CalculLeverMeridienCoucher(const Date &date, const Observateur &observateur);

    /**
     * @brief CalculPosition Calcul de la position du Soleil a partir du modele simplifie
     * de l'Astronomical Algorithms 2nd edition de Jean Meeus, p163-164
     * @param date date
     */
    void CalculPosition(const Date &date);


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


};


#endif // SOLEIL_H
