/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    14 juin 2015
 *
 */

#ifndef MATHSCONST_H
#define MATHSCONST_H

/*
 * Enumerations
 */
enum AngleFormatType {
    DEGRE,
    HEURE1,
    HEURE2,
    RADIAN,
    ARCSEC,
    NO_TYPE
};

enum AxeType {
    AXE_X,
    AXE_Y,
    AXE_Z
};


/*
 * Definitions des constantes
 */
// Seuils numeriques
static const double EPSDBL = 1.e-12;
static const double EPSDBL100 = 1.e-10;

// Constantes mathematiques
static const double PI = 3.1415926535897932384626433832795;
static const double DEUX_PI = 2. * PI;
static const double PI_SUR_DEUX = 0.5 * PI;
static const double DEUX_TIERS = 2. / 3.;

static const double T360 = 360.;
static const double ARCMIN_PAR_DEG = 60.;
static const double ARCSEC_PAR_MIN = 60.;
static const double ARCSEC_PAR_DEG = 3600.;

static const double DEG2RAD = PI / 180.;
static const double ARCSEC2RAD = PI / 648000.;
static const double HEUR2RAD = PI / 12.;

static const double DEUX_SUR_PI = 1. / PI_SUR_DEUX;
static const double DEG_PAR_ARCMIN = 1. / ARCMIN_PAR_DEG;
static const double DEG_PAR_ARCSEC = 1. / ARCSEC_PAR_DEG;
static const double RAD2DEG = 1. / DEG2RAD;
static const double RAD2HEUR = 1. / HEUR2RAD;


#endif // MATHSCONST_H
