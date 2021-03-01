/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    phasage.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    6 octobre 2016
 *
 * Date de revision
 * >
 *
 */

#ifndef PHASAGE_H
#define PHASAGE_H

#include "elementsosculateurs.h"


class Phasage
{
public:

    /* Constructeurs */
    Phasage();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void Calcul(const ElementsOsculateurs &elements, const double &n0);

    /* Accesseurs */
    int nu0() const;
    int dt0() const;
    int ct0() const;
    int nbOrb() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    int _nu0;
    int _dt0;
    int _ct0;
    int _nbOrb;


    /* Methodes privees */

};

#endif // PHASAGE_H
