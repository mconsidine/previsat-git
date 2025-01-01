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
 * >    corpsconst.h
 *
 * Localisation
 * >    librairies.corps
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes liees aux corps
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    23 juillet 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef CORPSCONST_H
#define CORPSCONST_H

#include "librairies/dates/dateconst.h"
#include "librairies/maths/mathsconst.h"


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
namespace CORPS {

static constexpr double MAGNITUDE_INDEFINIE = 99.;

static constexpr double CONSTANTE_GAUSS = 0.017202098950;
static constexpr double OBLIQUITE = 23.43929111111111 * MATHS::DEG2RAD;

// Unite astronomique (km)
static constexpr double UA2KM = 149597870.;

// Vitesse de la lumiere (km/s)
static constexpr double VITESSE_LUMIERE = 299792.458;

static constexpr double KM2UA = 1. / UA2KM;

static constexpr double TAU = UA2KM / (VITESSE_LUMIERE * DATE::NB_SEC_PAR_JOUR);

}

#endif // CORPSCONST_H
