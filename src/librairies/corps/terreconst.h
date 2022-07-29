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
 * >    terreconst.h
 *
 * Localisation
 * >    librairies.corps
 *
 * Heritage
 * >
 *
 * Description
 * >     Donnees relatives a la Terre (WGS 72)
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    16 novembre 2018
 *
 */

#ifndef TERRECONST_H
#define TERRECONST_H

#include <cmath>
#include "librairies/dates/dateconst.h"
#include "librairies/maths/mathsconst.h"


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
// Rayon equatorial terrestre (km)
static const double RAYON_TERRESTRE = 6378.135;

// Constante geocentrique de la gravitation (km^3 s^-2)
static const double GE = 398600.8;
static const double KE = NB_SEC_PAR_MIN * sqrt(GE / (RAYON_TERRESTRE * RAYON_TERRESTRE * RAYON_TERRESTRE));
static const double X1GE = 1. / GE;

// Premieres harmoniques zonales
static const double J2 = 0.001082616;
static const double J3 = -0.00000253881;
static const double J4 = -0.00000165597;

// Rapport du jour solaire moyen sur le jour sideral
static const double OMEGA0 = 1.0027379093507951;

// Vitesse de rotation de la Terre (rad s^-1)
static const double OMEGA = DEUX_PI * OMEGA0 * NB_JOUR_PAR_SEC;

// Aplatissement de la Terre
static const double APLA = 1. / 298.26;
static const double E2 = APLA * (2. - APLA);
static const double G2 = (1. - APLA) * (1. - APLA);

// Refraction totale a l'horizon (rad)
static const double REFRACTION_HZ = 1.14925 * DEG2RAD;

// Nombre de miles nautiques par kilometre
static const double MILE_PAR_KM = 1. / 1.852;

// Nombre de pieds par metre
static const double PIED_PAR_METRE = 1. / 0.3048;

// Nombre de kilogrammes par livre
static const double KG_PAR_LIVRE = 0.45359237;


#endif // TERRECONST_H
