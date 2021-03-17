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
 * >    luneconst.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes liees a la Lune
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

#ifndef LUNECONST_H
#define LUNECONST_H

#include <cmath>
#include "librairies/maths/mathsconst.h"


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
// Rayon equatorial lunaire (km)
static const double RAYON_LUNAIRE = 1737.4;

// Pour le calcul de la magnitude
static const double THETA = 20. * DEG2RAD;
static const double B0 = 2.01;
static const double H = 0.07;
static const double P0 = 1.68;
static const double W0 = 0.21;

static const double GA = sqrt(1. - W0);
static const double R0 = (1. - GA) / (1. + GA);
static const double R2 = R0 * R0;
static const double R0S2 = 0.5 * R0;
static const double W0S8 = 0.125 * W0;


#endif // LUNECONST_H
