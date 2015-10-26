/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 octobre 2015
 *
 * Date de revision
 * >
 *
 */

#ifndef SGP4_H
#define SGP4_H

#include "librairies/dates/date.h"
#include "librairies/maths/vecteur3d.h"
#include "tle.h"


class SGP4
{
public:
    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void Calcul(const Date &date, const TLE &tle);

    /* Accesseurs */
    char method() const;
    Vecteur3D position() const;
    Vecteur3D vitesse() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */
    // Pour le modele haute orbite
    static const double ZEL = 0.0549;
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

    /* Variables privees */
    struct SatVariables
    {
        // Elements orbitaux moyens
        double argpo;
        double bstar;
        double ecco;
        double inclo;
        double mo;
        double no;
        double omegao;
        Date epoque;

        // Variables du modele SGP4
        bool init;
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

    SatVariables _sat;
    Vecteur3D _position;
    Vecteur3D _vitesse;


    /* Methodes privees */
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
    void SGP4Init(const TLE &tle);

};

#endif // SGP4_H
