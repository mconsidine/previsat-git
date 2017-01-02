/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2017  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    2 aout 2016
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
    /**
     * @brief SGP4 Constructeur par defaut
     */
    SGP4();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief Calcul Calcul de la position et de la vitesse
     * @param date date
     * @param tle TLE du satellite
     */
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
#if __GNUC__ > 4
#define CONST constexpr
#else
#define CONST const
#endif

    // Pour le modele haute orbite
    static CONST double ZEL = 0.0549;
    static CONST double ZES = 0.01675;
    static CONST double ZNL = 1.5835218e-4;
    static CONST double ZNS = 1.19459e-5;
    static CONST double C1SS = 2.9864797e-6;
    static CONST double C1L = 4.7968065e-7;
    static CONST double ZSINIS = 0.39785416;
    static CONST double ZCOSIS = 0.91744867;
    static CONST double ZCOSGS = 0.1945905;
    static CONST double ZSINGS = -0.98088458;
    static CONST double Q22 = 1.7891679e-6;
    static CONST double Q31 = 2.1460748e-6;
    static CONST double Q33 = 2.2123015e-7;
    static CONST double ROOT22 = 1.7891679e-6;
    static CONST double ROOT44 = 7.3636953e-9;
    static CONST double ROOT54 = 2.1765803e-9;
    static CONST double ROOT32 = 3.7393792e-7;
    static CONST double ROOT52 = 1.1428639e-7;
    static CONST double FASX2 = 0.13130908;
    static CONST double FASX4 = 2.8843198;
    static CONST double FASX6 = 0.37448087;
    static CONST double G22 = 5.7686396;
    static CONST double G32 = 0.95240898;
    static CONST double G44 = 1.8014998;
    static CONST double G52 = 1.050833;
    static CONST double G54 = 4.4108898;
    static CONST double STEPP = 720.;
    static CONST double STEPN = -720.;
    static CONST double STEP2 = 259200.;

    /* Variables privees */
    // Elements orbitaux moyens
    double _argpo;
    double _bstar;
    double _ecco;
    double _inclo;
    double _mo;
    double _no;
    double _omegao;
    Date _epoque;

    // Variables du modele SGP4
    bool _isimp;
    char _method;
    int _irez;
    double _ao;
    double _argpdot;
    double _argpm;
    double _argpp;
    double _atime;
    double _aycof;
    double _cc1;
    double _cc4;
    double _cc5;
    double _cnodm;
    double _con41;
    double _con42;
    double _cosim;
    double _cosio;
    double _cosio2;
    double _cosomm;
    double _d2;
    double _d2201;
    double _d2211;
    double _d3;
    double _d3210;
    double _d3222;
    double _d4;
    double _d4410;
    double _d4422;
    double _d5220;
    double _d5232;
    double _d5421;
    double _d5433;
    double _day;
    double _dedt;
    double _del1;
    double _del2;
    double _del3;
    double _delmo;
    double _delt;
    double _didt;
    double _dmdt;
    double _domdt;
    double _dndt;
    double _dnodt;
    double _eccsq;
    double _ee2;
    double _e3;
    double _em;
    double _emsq;
    double _ep;
    double _eta;
    double _f220;
    double _f221;
    double _f311;
    double _f321;
    double _f322;
    double _f330;
    double _f441;
    double _f442;
    double _f522;
    double _f523;
    double _f542;
    double _f543;
    double _g200;
    double _g201;
    double _g211;
    double _g300;
    double _g310;
    double _g322;
    double _g410;
    double _g422;
    double _g520;
    double _g521;
    double _g532;
    double _g533;
    double _gam;
    double _gsto;
    double _inclm;
    double _mdot;
    double _mm;
    double _mp;
    double _nm;
    double _nodecf;
    double _nodedot;
    double _nodem;
    double _nodep;
    double _omeosq;
    double _omgcof;
    double _posq;
    double _rp;
    double _rtemsq;
    double _rteosq;
    double _s1;
    double _s2;
    double _s3;
    double _s4;
    double _s5;
    double _s6;
    double _s7;
    double _se2;
    double _se3;
    double _sgh2;
    double _sgh3;
    double _sgh4;
    double _sh2;
    double _sh3;
    double _si2;
    double _si3;
    double _sinim;
    double _sinio;
    double _sinmao;
    double _sinomm;
    double _sl2;
    double _sl3;
    double _sl4;
    double _snodm;
    double _ss1;
    double _ss2;
    double _ss3;
    double _ss4;
    double _ss5;
    double _ss6;
    double _ss7;
    double _sz1;
    double _sz2;
    double _sz3;
    double _sz11;
    double _sz12;
    double _sz13;
    double _sz21;
    double _sz22;
    double _sz23;
    double _sz31;
    double _sz32;
    double _sz33;
    double _t;
    double _t2cof;
    double _t3cof;
    double _t4cof;
    double _t5cof;
    double _x1mth2;
    double _x7thm1;
    double _xfact;
    double _xi2;
    double _xi3;
    double _xincp;
    double _xgh2;
    double _xgh3;
    double _xgh4;
    double _xh2;
    double _xh3;
    double _xl2;
    double _xl3;
    double _xl4;
    double _xlamo;
    double _xlcof;
    double _xli;
    double _xmcof;
    double _xni;
    double _xpidot;
    double _z1;
    double _z2;
    double _z3;
    double _z11;
    double _z12;
    double _z13;
    double _z21;
    double _z22;
    double _z23;
    double _z31;
    double _z32;
    double _z33;
    double _zmol;
    double _zmos;

    bool _init;
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
