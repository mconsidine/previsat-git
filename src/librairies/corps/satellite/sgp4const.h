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
 * >    sgp4const.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes du modele SGP4
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    16 juin 2019
 *
 * Date de revision
 * >    3 decembre 2019
 *
 */

#ifndef SATELLITECONST_H
#define SATELLITECONST_H

#include "librairies/corps/systemesolaire/terreconst.h"


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
static const double J3SJ2 = J3 / J2;
static const double RPTIM = OMEGA * NB_SEC_PAR_MIN;
static const double X1SRT = 1. / RAYON_TERRESTRE;
static const double RTMS = RAYON_TERRESTRE * NB_MIN_PAR_SEC * KE;


// Pour le modele haute orbite
static const double ZEL = 0.05490;
static const double ZES = 0.01675;
static const double ZNL = 1.5835218e-4;
static const double ZNS = 1.19459e-5;
static const double C1SS = 2.9864797e-6;
static const double C1L = 4.7968065e-7;
static const double ZSINIS = 0.39785416;
static const double ZCOSIS = 0.91744867;
static const double ZCOSGS = 0.1945905;
static const double ZSINGS = -0.98088458;
static const double Q22 = 1.7891679e-6;
static const double Q31 = 2.1460748e-6;
static const double Q33 = 2.2123015e-7;
static const double ROOT22 = 1.7891679e-6;
static const double ROOT44 = 7.3636953e-9;
static const double ROOT54 = 2.1765803e-9;
static const double ROOT32 = 3.7393792e-7;
static const double ROOT52 = 1.1428639e-7;
static const double FASX2 = 0.13130908;
static const double FASX4 = 2.8843198;
static const double FASX6 = 0.37448087;
static const double G22 = 5.7686396;
static const double G32 = 0.95240898;
static const double G44 = 1.8014998;
static const double G52 = 1.050833;
static const double G54 = 4.4108898;
static const double STEPP = 720.;
static const double STEPN = -720.;
static const double STEP2 = 259200.;


#endif // SATELLITECONST_H
