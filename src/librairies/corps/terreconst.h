/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
namespace TERRE {

// Rayon equatorial terrestre (km)
static constexpr double RAYON_TERRESTRE = 6378.135;

// Constante geocentrique de la gravitation (km^3 s^-2)
static constexpr double GE = 398600.8;
static constexpr double KE = DATE::NB_SEC_PAR_MIN * sqrt(GE / (RAYON_TERRESTRE * RAYON_TERRESTRE * RAYON_TERRESTRE));
static constexpr double X1GE = 1. / GE;

// Premieres harmoniques zonales
static constexpr double J2 = 0.001082616;
static constexpr double J3 = -0.00000253881;
static constexpr double J4 = -0.00000165597;

// Rapport du jour solaire moyen sur le jour sideral
static constexpr double OMEGA0 = 1.0027379093507951;

// Vitesse de rotation de la Terre (rad s^-1)
static constexpr double OMEGA = MATHS::DEUX_PI * TERRE::OMEGA0 * DATE::NB_JOUR_PAR_SEC;

// Aplatissement de la Terre
static constexpr double APLA = 1. / 298.26;
static constexpr double E2 = APLA * (2. - APLA);
static constexpr double G2 = (1. - APLA) * (1. - APLA);

// Refraction totale a l'horizon (rad)
static constexpr double REFRACTION_HZ = 1.14925 * MATHS::DEG2RAD;

// Nombre de miles nautiques par kilometre
static constexpr double MILE_PAR_KM = 1. / 1.852;

// Nombre de pieds par metre
static constexpr double PIED_PAR_METRE = 1. / 0.3048;

// Nombre de kilogrammes par livre
static constexpr double KG_PAR_LIVRE = 0.45359237;

}

#endif // TERRECONST_H
