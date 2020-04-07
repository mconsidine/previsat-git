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
 * >    planete.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position des planetes
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 janvier 2012
 *
 * Date de revision
 * >    4 decembre 2015
 *
 */

#ifndef PLANETE_H
#define PLANETE_H

#include "librairies/corps/corps.h"
#include "planeteconst.h"


class Soleil;

class Planete : public Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Planete Constructeur avec l'identifiant de la planete
     * @param iplanete indice de la planete
     */
    explicit Planete(const IndicePlanete &iplanete);


    /*
     * Accesseurs
     */
    QString nom() const;


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
     * @brief CalculPosition Calcul de la position d'une planete
     * @param date date
     * @param soleil soleil
     */
    void CalculPosition(const Date &date, const Soleil &soleil);


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
    IndicePlanete _iplanete;
    double _elem[6];
    Vecteur3D _positionSph;
    QString _nom;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculElements Calcul des elements orbitaux moyens d'une planete
     * @param date date
     */
    void CalculElements(const Date &date);

    /**
     * @brief CalculCoordonneesSpheriques Calcul des coordonnees spheriques ecliptiques d'une planete
     */
    void CalculCoordonneesSpheriques();



};


#endif // PLANETE_H
