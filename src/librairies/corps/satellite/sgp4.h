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
 * >    sgp4.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Modele orbital SGP4
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 octobre 2015
 *
 * Date de revision
 * >    30 avril 2022
 *
 */

#ifndef SGP4_H
#define SGP4_H

#include "librairies/dates/date.h"
#include "librairies/maths/vecteur3d.h"
#include "elementsorbitaux.h"
#include "sgp4const.h"


struct SGP4Private;

class SGP4
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief SGP4 Constructeur par defaut
     */
    SGP4();


    /*
     * Methodes publiques
     */
    /**
     * @brief Calcul Calcul de la position et de la vitesse
     * @param date date
     * @param elements elements orbitaux du satellite
     */
    void Calcul(const Date &date, const ElementsOrbitaux &elements);


    /*
     * Accesseurs
     */
    char method() const;
    const Vecteur3D &position() const;
    const Vecteur3D &vitesse() const;


    /*
     * Modificateurs
     */
    void setInit(const bool init);


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
    // Elements orbitaux moyens
    ElementsOrbitaux _elements;

    // Variables du modele SGP4
    SGP4Private *d_data;

    bool _init;
    Vecteur3D _position;
    Vecteur3D _vitesse;


    /*
     * Methodes privees
     */
    /**
     * @brief Dpper Modele haute orbite
     */
    void Dpper();

    /**
     * @brief Dscom Modele haute orbite
     * @param tc date
     */
    void Dscom(const double tc);

    /**
     * @brief Dsinit Modele haute orbite
     * @param tc date
     */
    void Dsinit(const double tc);

    /**
     * @brief Dspace Modele haute orbite
     * @param tc date
     */
    void Dspace(const double tc);

    /**
     * @brief SGP4Init Initialisation du modele SGP4
     * @param tle tle
     */
    void SGP4Init(const ElementsOrbitaux &elements);


};

#endif // SGP4_H
