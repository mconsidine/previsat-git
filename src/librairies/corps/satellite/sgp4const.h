/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    26 fevrier 2023
 *
 */

#ifndef SGP4CONST_H
#define SGP4CONST_H

#include "librairies/corps/terreconst.h"


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
namespace SGP {

static constexpr double J3SJ2 = TERRE::J3 / TERRE::J2;
static constexpr double RPTIM = TERRE::OMEGA * DATE::NB_SEC_PAR_MIN;
static constexpr double X1SRT = 1. / TERRE::RAYON_TERRESTRE;
static constexpr double RTMS = TERRE::RAYON_TERRESTRE * DATE::NB_MIN_PAR_SEC * TERRE::KE;


// Pour le modele haute orbite
static constexpr double ZEL = 0.05490;
static constexpr double ZES = 0.01675;
static constexpr double ZNL = 1.5835218e-4;
static constexpr double ZNS = 1.19459e-5;
static constexpr double C1SS = 2.9864797e-6;
static constexpr double C1L = 4.7968065e-7;
static constexpr double ZSINIS = 0.39785416;
static constexpr double ZCOSIS = 0.91744867;
static constexpr double ZCOSGS = 0.1945905;
static constexpr double ZSINGS = -0.98088458;
static constexpr double Q22 = 1.7891679e-6;
static constexpr double Q31 = 2.1460748e-6;
static constexpr double Q33 = 2.2123015e-7;
static constexpr double ROOT22 = 1.7891679e-6;
static constexpr double ROOT44 = 7.3636953e-9;
static constexpr double ROOT54 = 2.1765803e-9;
static constexpr double ROOT32 = 3.7393792e-7;
static constexpr double ROOT52 = 1.1428639e-7;
static constexpr double FASX2 = 0.13130908;
static constexpr double FASX4 = 2.8843198;
static constexpr double FASX6 = 0.37448087;
static constexpr double G22 = 5.7686396;
static constexpr double G32 = 0.95240898;
static constexpr double G44 = 1.8014998;
static constexpr double G52 = 1.050833;
static constexpr double G54 = 4.4108898;
static constexpr double STEPP = 720.;
static constexpr double STEPN = -720.;
static constexpr double STEP2 = 259200.;

}


/*
 * Definition des structures
 */
struct SGP4Data
{
    bool isimp;
    char method;
    int irez;
    double ao;
    double argpdot;
    double argpm;
    double argpp;
    double atime;
    double aycof;
    double cc1;
    double cc4;
    double cc5;
    double cnodm;
    double con41;
    double con42;
    double cosim;
    double cosio;
    double cosio2;
    double cosomm;
    double d2;
    double d2201;
    double d2211;
    double d3;
    double d3210;
    double d3222;
    double d4;
    double d4410;
    double d4422;
    double d5220;
    double d5232;
    double d5421;
    double d5433;
    double day;
    double dedt;
    double del1;
    double del2;
    double del3;
    double delmo;
    double delt;
    double didt;
    double dmdt;
    double domdt;
    double dndt;
    double dnodt;
    double eccsq;
    double ee2;
    double e3;
    double em;
    double emsq;
    double ep;
    double eta;
    double f220;
    double f221;
    double f311;
    double f321;
    double f322;
    double f330;
    double f441;
    double f442;
    double f522;
    double f523;
    double f542;
    double f543;
    double g200;
    double g201;
    double g211;
    double g300;
    double g310;
    double g322;
    double g410;
    double g422;
    double g520;
    double g521;
    double g532;
    double g533;
    double gam;
    double gsto;
    double inclm;
    double mdot;
    double mm;
    double mp;
    double nm;
    double nodecf;
    double nodedot;
    double nodem;
    double nodep;
    double omeosq;
    double omgcof;
    double posq;
    double rp;
    double rtemsq;
    double rteosq;
    double s1;
    double s2;
    double s3;
    double s4;
    double s5;
    double s6;
    double s7;
    double se2;
    double se3;
    double sgh2;
    double sgh3;
    double sgh4;
    double sh2;
    double sh3;
    double si2;
    double si3;
    double sinim;
    double sinio;
    double sinmao;
    double sinomm;
    double sl2;
    double sl3;
    double sl4;
    double snodm;
    double ss1;
    double ss2;
    double ss3;
    double ss4;
    double ss5;
    double ss6;
    double ss7;
    double sz1;
    double sz2;
    double sz3;
    double sz11;
    double sz12;
    double sz13;
    double sz21;
    double sz22;
    double sz23;
    double sz31;
    double sz32;
    double sz33;
    double t;
    double t2cof;
    double t3cof;
    double t4cof;
    double t5cof;
    double x1mth2;
    double x7thm1;
    double xfact;
    double xi2;
    double xi3;
    double xincp;
    double xgh2;
    double xgh3;
    double xgh4;
    double xh2;
    double xh3;
    double xl2;
    double xl3;
    double xl4;
    double xlamo;
    double xlcof;
    double xli;
    double xmcof;
    double xni;
    double xpidot;
    double z1;
    double z2;
    double z3;
    double z11;
    double z12;
    double z13;
    double z21;
    double z22;
    double z23;
    double z31;
    double z32;
    double z33;
    double zmol;
    double zmos;

};

#endif // SGP4CONST_H
