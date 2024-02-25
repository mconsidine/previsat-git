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
 * >    mathsconst.h
 *
 * Localisation
 * >    librairies.maths
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes mathematiques
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    25 fevrier 2023
 *
 */

#ifndef MATHSCONST_H
#define MATHSCONST_H

/*
 * Enumerations
 */
enum class AngleFormatType {
    DEGRE,
    HEURE1,
    HEURE2,
    RADIAN,
    ARCSEC,
    NO_TYPE
};

enum class AxeType {
    AXE_X,
    AXE_Y,
    AXE_Z
};


/*
 * Definitions des constantes
 */
namespace MATHS {

// Seuils numeriques
static constexpr double EPSDBL = 1.e-12;
static constexpr double EPSDBL100 = 1.e-10;

// Constantes mathematiques
static constexpr unsigned int DEGRE_INTERPOLATION = 3;
static constexpr unsigned int ITERATIONS_MAX = 50000;
static constexpr double PI = 3.1415926535897932384626433832795;
static constexpr double DEUX_PI = 2. * PI;
static constexpr double PI_SUR_DEUX = 0.5 * PI;
static constexpr double DEUX_TIERS = 2. / 3.;

static constexpr double T360 = 360.;
static constexpr double ARCMIN_PAR_DEG = 60.;
static constexpr double ARCSEC_PAR_MIN = 60.;
static constexpr double ARCSEC_PAR_DEG = 3600.;
static constexpr double HSEC2RAD = DEUX_TIERS * DEUX_PI;

static constexpr double DEG2RAD = PI / 180.;
static constexpr double ARCMIN2RAD = PI / 10800.;
static constexpr double ARCSEC2RAD = PI / 648000.;
static constexpr double HEUR2RAD = PI / 12.;

static constexpr double DEUX_SUR_PI = 1. / PI_SUR_DEUX;
static constexpr double DEG_PAR_ARCMIN = 1. / ARCMIN_PAR_DEG;
static constexpr double DEG_PAR_ARCSEC = 1. / ARCSEC_PAR_DEG;
static constexpr double RAD2DEG = 1. / DEG2RAD;
static constexpr double RAD2HEUR = 1. / HEUR2RAD;

}

#endif // MATHSCONST_H
