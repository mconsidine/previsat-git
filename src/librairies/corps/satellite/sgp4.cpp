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
 * >    sgp4.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Modele orbital SGP4
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 octobre 2015
 *
 * Date de revision
 * >    3 decembre 2019
 *
 */

#include "librairies/corps/systemesolaire/terreconst.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "sgp4.h"
#include "tle.h"


/**********
 * PUBLIC *
 **********/

class SGP4Private
{
public:
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

};

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
SGP4::SGP4()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    // Elements orbitaux moyens
    _argpo = 0.;
    _bstar = 0.;
    _ecco = 0.;
    _inclo = 0.;
    _mo = 0.;
    _no = 0.;
    _omegao = 0.;

    d_data = new SGP4Private();
    _init = false;

    /* Retour */
    return;
}

/*
 * Accesseurs
 */
char SGP4::method() const
{
    return d_data->_method;
}

Vecteur3D SGP4::position() const
{
    return _position;
}

Vecteur3D SGP4::vitesse() const
{
    return _vitesse;
}

void SGP4::setInit(const bool init)
{
    _init = init;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la position et de la vitesse
 */
void SGP4::Calcul(const Date &date, const TLE &tle)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_init) {
        SGP4Init(tle);
    }

    /* Corps de la methode */
    try {

        double coseo1 = 0.;
        double sineo1 = 0.;

        // Calcul du temps ecoule depuis l'epoque (en minutes)
        const double tsince = NB_MIN_PAR_JOUR * (date.jourJulienUTC() - tle.epoque().jourJulienUTC());

        d_data->_t = tsince;

        // Prise en compte des termes seculaires de la gravite et du freinage atmospherique
        const double xmdf = _mo + d_data->_mdot * d_data->_t;
        const double argpdf = _argpo + d_data->_argpdot * d_data->_t;
        const double nodedf = _omegao + d_data->_nodedot * d_data->_t;
        d_data->_argpm = argpdf;
        d_data->_mm = xmdf;
        const double tt2 = d_data->_t * d_data->_t;
        d_data->_nodem = nodedf + d_data->_nodecf * tt2;
        double tempa = 1. - d_data->_cc1 * d_data->_t;
        double tempe = _bstar * d_data->_cc4 * d_data->_t;
        double templ = d_data->_t2cof * tt2;

        if (!d_data->_isimp) {

            const double delomg = d_data->_omgcof * d_data->_t;
            const double delmtmp = 1. + d_data->_eta * cos(xmdf);
            const double delm = d_data->_xmcof * (delmtmp * delmtmp * delmtmp - d_data->_delmo);
            double temp = delomg + delm;
            d_data->_mm = xmdf + temp;
            d_data->_argpm = argpdf - temp;
            const double tt3 = tt2 * d_data->_t;
            const double t4 = tt3 * d_data->_t;
            tempa = tempa - d_data->_d2 * tt2 - d_data->_d3 * tt3 - d_data->_d4 * t4;
            tempe = tempe + _bstar * d_data->_cc5 * (sin(d_data->_mm) - d_data->_sinmao);
            templ = templ + d_data->_t3cof * tt3 + t4 * (d_data->_t4cof + d_data->_t * d_data->_t5cof);
        }

        d_data->_nm = _no;
        d_data->_em = _ecco;
        d_data->_inclm = _inclo;

        if (d_data->_method == 'd') {
            const double tc = d_data->_t;
            Dspace(tc);
        }

        const double am = pow((KE / d_data->_nm), DEUX_TIERS) * tempa * tempa;
        d_data->_nm = KE * pow(am, -1.5);
        d_data->_em -= tempe;

        if (d_data->_em < 1.e-6) {
            d_data->_em = 1.e-6;
        }

        d_data->_mm = d_data->_mm + _no * templ;
        double xlm = d_data->_mm + d_data->_argpm + d_data->_nodem;
        d_data->_emsq = d_data->_em * d_data->_em;
        double temp = 1. - d_data->_emsq;

        d_data->_nodem = fmod(d_data->_nodem, DEUX_PI);
        d_data->_argpm = fmod(d_data->_argpm, DEUX_PI);
        xlm = fmod(xlm, DEUX_PI);
        d_data->_mm = fmod(xlm - d_data->_argpm - d_data->_nodem, DEUX_PI);

        d_data->_sinim = sin(d_data->_inclm);
        d_data->_cosim = cos(d_data->_inclm);

        // Prise en compte des termes periodiques luni-solaires
        d_data->_ep = d_data->_em;
        d_data->_xincp = d_data->_inclm;
        d_data->_argpp = d_data->_argpm;
        d_data->_nodep = d_data->_nodem;
        d_data->_mp = d_data->_mm;
        double sinip = d_data->_sinim;
        double cosip = d_data->_cosim;

        // Termes longue periode
        if (d_data->_method == 'd') {
            Dpper();
            if (d_data->_xincp < 0.) {
                d_data->_xincp = -d_data->_xincp;
                d_data->_nodep += PI;
                d_data->_argpp -= PI;
            }

            sinip = sin(d_data->_xincp);
            cosip = cos(d_data->_xincp);
            d_data->_aycof = -0.5 * J3SJ2 * sinip;

            if (fabs(cosip + 1.) > 1.5e-12) {
                d_data->_xlcof = -0.25 * J3SJ2 * sinip * (3. + 5. * cosip) / (1. + cosip);
            } else {
                d_data->_xlcof = -0.25 * J3SJ2 * sinip * (3. + 5. * cosip) * (1. / 1.5e-12);
            }
        }

        const double axnl = d_data->_ep * cos(d_data->_argpp);
        temp = 1. / (am * (1. - d_data->_ep * d_data->_ep));
        const double aynl = d_data->_ep * sin(d_data->_argpp) + temp * d_data->_aycof;
        const double xl = d_data->_mp + d_data->_argpp + d_data->_nodep + temp * d_data->_xlcof * axnl;

        // Resolution de l'equation de Kepler
        const double u = fmod(xl - d_data->_nodep, DEUX_PI);
        double eo1 = u;
        double tem5 = 9999.9;
        int ktr = 1;

        while ((fabs(tem5) >= EPSDBL) && (ktr <= 10)) {
            sineo1 = sin(eo1);
            coseo1 = cos(eo1);
            tem5 = (u - aynl * coseo1 + axnl * sineo1 - eo1) / (1. - coseo1 * axnl - sineo1 * aynl);
            if (fabs(tem5) >= 0.95) {
                tem5 = (tem5 > 0.) ? 0.95 : -0.95;
            }
            eo1 += tem5;
            ktr++;
        }

        // Termes courte periode
        const double ecose = axnl * coseo1 + aynl * sineo1;
        const double esine = axnl * sineo1 - aynl * coseo1;
        const double el2 = axnl * axnl + aynl * aynl;
        const double pl = am * (1. - el2);

        if (pl >= 0.) {

            const double rl = am * (1. - ecose);
            const double rdotl = sqrt(am) * esine / rl;
            const double rvdotl = sqrt(pl) / rl;
            const double betal = sqrt(1. - el2);
            temp = esine / (1. + betal);
            const double tmp1 = am / rl;
            const double sinu = tmp1 * (sineo1 - aynl - axnl * temp);
            const double cosu = tmp1 * (coseo1 - axnl + aynl * temp);
            double su = atan2(sinu, cosu);
            const double sin2u = (cosu + cosu) * sinu;
            const double cos2u = 1. - 2. * sinu * sinu;
            temp = 1. / pl;
            const double temp1 = 0.5 * J2 * temp;
            const double temp2 = temp1 * temp;

            // Prise en compte des termes courte periode
            if (d_data->_method == 'd') {

                const double cosisq = cosip * cosip;
                d_data->_con41 = 3. * cosisq - 1.;
                d_data->_x1mth2 = 1. - cosisq;
                d_data->_x7thm1 = 7. * cosisq - 1.;
            }

            const double mrt = RAYON_TERRESTRE * (rl * (1. - 1.5 * temp2 * betal * d_data->_con41) + 0.5 * temp1 * d_data->_x1mth2 * cos2u);
            su = su - 0.25 * temp2 * d_data->_x7thm1 * sin2u;
            const double temp3 = 1.5 * temp2 * cosip;
            const double xnode = d_data->_nodep + temp3 * sin2u;
            const double xinc = d_data->_xincp + temp3 * sinip * cos2u;
            const double mvt = rdotl - d_data->_nm * temp1 * d_data->_x1mth2 * sin2u / KE;
            const double rvdot = rvdotl + d_data->_nm * temp1 * (d_data->_x1mth2 * cos2u + 1.5 * d_data->_con41) / KE;

            // Vecteurs directeurs
            const double sinsu = sin(su);
            const double cossu = cos(su);
            const double snod = sin(xnode);
            const double cnod = cos(xnode);
            const double cosi = cos(xinc);
            const double sini = sin(xinc);

            const double xmx = -snod * cosi;
            const double xmy = cnod * cosi;
            const Vecteur3D uu(xmx * sinsu + cnod * cossu, xmy * sinsu + snod * cossu, sini * sinsu);
            const Vecteur3D vv(xmx * cossu - cnod * sinsu, xmy * cossu - snod * sinsu, sini * cossu);

            // Position et vitesse
            _position = uu * mrt;
            _vitesse = (uu * mvt + vv * rvdot) * RTMS;
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}



/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */
/*
 * Modele haute orbite
 */
void SGP4::Dpper() {

    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    double zm = (_init) ? d_data->_zmos + ZNS * d_data->_t : d_data->_zmos;

    double zf = zm + 2. * ZES * sin(zm);
    double sinzf = sin(zf);
    double f2 = 0.5 * sinzf * sinzf - 0.25;
    double f3 = -0.5 * sinzf * cos(zf);
    const double ses = d_data->_se2 * f2 + d_data->_se3 * f3;
    const double sis = d_data->_si2 * f2 + d_data->_si3 * f3;
    const double sls = d_data->_sl2 * f2 + d_data->_sl3 * f3 + d_data->_sl4 * sinzf;
    const double sghs = d_data->_sgh2 * f2 + d_data->_sgh3 * f3 + d_data->_sgh4 * sinzf;
    const double shs = d_data->_sh2 * f2 + d_data->_sh3 * f3;

    zm = (_init) ? d_data->_zmol + ZNL * d_data->_t : d_data->_zmol;

    zf = zm + 2. * ZEL * sin(zm);
    sinzf = sin(zf);
    f2 = 0.5 * sinzf * sinzf - 0.25;
    f3 = -0.5 * sinzf * cos(zf);

    const double sel = d_data->_ee2 * f2 + d_data->_e3 * f3;
    const double sil = d_data->_xi2 * f2 + d_data->_xi3 * f3;
    const double sll = d_data->_xl2 * f2 + d_data->_xl3 * f3 + d_data->_xl4 * sinzf;
    const double sghl = d_data->_xgh2 * f2 + d_data->_xgh3 * f3 + d_data->_xgh4 * sinzf;
    const double shll = d_data->_xh2 * f2 + d_data->_xh3 * f3;
    const double pe = ses + sel;
    const double pinc = sis + sil;
    const double pl = sls + sll;
    double pgh = sghs + sghl;
    double ph = shs + shll;

    if (_init) {

        d_data->_xincp += pinc;
        d_data->_ep += pe;
        const double sinip = sin(d_data->_xincp);
        const double cosip = cos(d_data->_xincp);

        // Application directe des termes periodiques
        if (d_data->_xincp >= 0.2) {
            ph /= sinip;
            pgh = pgh - cosip * ph;
            d_data->_argpp += pgh;
            d_data->_nodep += ph;
            d_data->_mp += pl;
        } else {

            // Application des termes periodiques avec la modification de Lyddane
            const double sinop = sin(d_data->_nodep);
            const double cosop = cos(d_data->_nodep);
            double alfdp = sinip * sinop;
            double betdp = sinip * cosop;
            const double tmp = pinc * cosip;
            const double dalf = ph * cosop + tmp * sinop;
            const double dbet = -ph * sinop + tmp * cosop;
            alfdp += dalf;
            betdp += dbet;
            d_data->_nodep = fmod(d_data->_nodep, DEUX_PI);
            if (d_data->_nodep < 0.) {
                d_data->_nodep += DEUX_PI;
            }

            double xls = d_data->_mp + d_data->_argpp + cosip * d_data->_nodep;
            const double dls = pl + pgh - pinc * d_data->_nodep * sinip;
            xls += dls;
            const double xnoh = d_data->_nodep;
            d_data->_nodep = atan2(alfdp, betdp);
            if (d_data->_nodep < 0.) {
                d_data->_nodep += DEUX_PI;
            }

            if (fabs(xnoh - d_data->_nodep) > PI) {
                d_data->_nodep = (d_data->_nodep < xnoh) ? d_data->_nodep + DEUX_PI : d_data->_nodep - DEUX_PI;
            }

            d_data->_mp += pl;
            d_data->_argpp = xls - d_data->_mp - cosip * d_data->_nodep;
        }
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void SGP4::Dscom(const double tc) {

    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    d_data->_nm = _no;
    d_data->_em = _ecco;
    d_data->_snodm = sin(_omegao);
    d_data->_cnodm = cos(_omegao);
    d_data->_sinomm = sin(_argpo);
    d_data->_cosomm = cos(_argpo);
    d_data->_sinim = sin(_inclo);
    d_data->_cosim = cos(_inclo);
    d_data->_emsq = d_data->_em * d_data->_em;
    const double betasq = 1. - d_data->_emsq;
    d_data->_rtemsq = sqrt(betasq);

    // Initialisation des termes luni-solaires
    d_data->_day = _epoque.jourJulienUTC() + NB_JOURS_PAR_SIECJ + tc * NB_JOUR_PAR_MIN;
    const double xnodce = fmod(4.5236020 - 0.00092422029 * d_data->_day, DEUX_PI);
    const double stem = sin(xnodce);
    const double ctem = cos(xnodce);
    const double zcosil = 0.91375164 - 0.03568096 * ctem;
    const double zsinil = sqrt(1. - zcosil * zcosil);
    const double zsinhl = 0.089683511 * stem / zsinil;
    const double zcoshl = sqrt(1. - zsinhl * zsinhl);
    d_data->_gam = 5.8351514 + 0.0019443680 * d_data->_day;
    double zx = 0.39785416 * stem / zsinil;
    const double zy = zcoshl * ctem + 0.91744867 * zsinhl * stem;
    zx = atan2(zx, zy);
    zx = d_data->_gam + zx - xnodce;
    const double zcosgl = cos(zx);
    const double zsingl = sin(zx);

    // Termes solaires
    double zcosg = ZCOSGS;
    double zsing = ZSINGS;
    double zcosi = ZCOSIS;
    double zsini = ZSINIS;
    double zcosh = d_data->_cnodm;
    double zsinh = d_data->_snodm;
    double cc = C1SS;
    double xnoi = 1. / d_data->_nm;

    for (int lsflg = 1; lsflg <= 2; lsflg++) {

        const double a0 = zcosi * zsinh;
        const double a1 = zcosg * zcosh + zsing * a0;
        const double a3 = -zsing * zcosh + zcosg * a0;
        const double a7 = -zcosg * zsinh + zsing * zcosi * zcosh;
        const double a8 = zsing * zsini;
        const double a9 = zsing * zsinh + zcosg * zcosi * zcosh;
        const double a10 = zcosg * zsini;
        const double a2 = d_data->_cosim * a7 + d_data->_sinim * a8;
        const double a4 = d_data->_cosim * a9 + d_data->_sinim * a10;
        const double a5 = -d_data->_sinim * a7 + d_data->_cosim * a8;
        const double a6 = -d_data->_sinim * a9 + d_data->_cosim * a10;

        const double x1 = a1 * d_data->_cosomm + a2 * d_data->_sinomm;
        const double x2 = a3 * d_data->_cosomm + a4 * d_data->_sinomm;
        const double x3 = -a1 * d_data->_sinomm + a2 * d_data->_cosomm;
        const double x4 = -a3 * d_data->_sinomm + a4 * d_data->_cosomm;
        const double x5 = a5 * d_data->_sinomm;
        const double x6 = a6 * d_data->_sinomm;
        const double x7 = a5 * d_data->_cosomm;
        const double x8 = a6 * d_data->_cosomm;

        d_data->_z31 = 12. * x1 * x1 - 3. * x3 * x3;
        d_data->_z32 = 24. * x1 * x2 - 6. * x3 * x4;
        d_data->_z33 = 12. * x2 * x2 - 3. * x4 * x4;
        d_data->_z1 = 3. * (a1 * a1 + a2 * a2) + d_data->_z31 * d_data->_emsq;
        d_data->_z2 = 6. * (a1 * a3 + a2 * a4) + d_data->_z32 * d_data->_emsq;
        d_data->_z3 = 3. * (a3 * a3 + a4 * a4) + d_data->_z33 * d_data->_emsq;
        d_data->_z11 = -6. * a1 * a5 + d_data->_emsq * (-24. * x1 * x7 - 6. * x3 * x5);
        d_data->_z12 = -6. * (a1 * a6 + a3 * a5) + d_data->_emsq * (-24. * (x2 * x7 + x1 * x8) - 6. * (x3 * x6 + x4 * x5));
        d_data->_z13 = -6. * a3 * a6 + d_data->_emsq * (-24. * x2 * x8 - 6. * x4 * x6);
        d_data->_z21 = 6. * a2 * a5 + d_data->_emsq * (24. * x1 * x5 - 6. * x3 * x7);
        d_data->_z22 = 6. * (a4 * a5 + a2 * a6) + d_data->_emsq * (24. * (x2 * x5 + x1 * x6) - 6. * (x4 * x7 + x3 * x8));
        d_data->_z23 = 6. * a4 * a6 + d_data->_emsq * (24. * x2 * x6 - 6. * x4 * x8);
        d_data->_z1 = d_data->_z1 + d_data->_z1 + betasq * d_data->_z31;
        d_data->_z2 = d_data->_z2 + d_data->_z2 + betasq * d_data->_z32;
        d_data->_z3 = d_data->_z3 + d_data->_z3 + betasq * d_data->_z33;
        d_data->_s3 = cc * xnoi;
        d_data->_s2 = -0.5 * d_data->_s3 / d_data->_rtemsq;
        d_data->_s4 = d_data->_s3 * d_data->_rtemsq;
        d_data->_s1 = -15. * d_data->_em * d_data->_s4;
        d_data->_s5 = x1 * x3 + x2 * x4;
        d_data->_s6 = x2 * x3 + x1 * x4;
        d_data->_s7 = x2 * x4 - x1 * x3;

        // Termes lunaires
        if (lsflg == 1) {
            d_data->_ss1 = d_data->_s1;
            d_data->_ss2 = d_data->_s2;
            d_data->_ss3 = d_data->_s3;
            d_data->_ss4 = d_data->_s4;
            d_data->_ss5 = d_data->_s5;
            d_data->_ss6 = d_data->_s6;
            d_data->_ss7 = d_data->_s7;
            d_data->_sz1 = d_data->_z1;
            d_data->_sz2 = d_data->_z2;
            d_data->_sz3 = d_data->_z3;
            d_data->_sz11 = d_data->_z11;
            d_data->_sz12 = d_data->_z12;
            d_data->_sz13 = d_data->_z13;
            d_data->_sz21 = d_data->_z21;
            d_data->_sz22 = d_data->_z22;
            d_data->_sz23 = d_data->_z23;
            d_data->_sz31 = d_data->_z31;
            d_data->_sz32 = d_data->_z32;
            d_data->_sz33 = d_data->_z33;
            zcosg = zcosgl;
            zsing = zsingl;
            zcosi = zcosil;
            zsini = zsinil;
            zcosh = zcoshl * d_data->_cnodm + zsinhl * d_data->_snodm;
            zsinh = d_data->_snodm * zcoshl - d_data->_cnodm * zsinhl;
            cc = C1L;
        }
    }

    d_data->_zmol = fmod(4.7199672 + 0.22997150 * d_data->_day - d_data->_gam, DEUX_PI);
    d_data->_zmos = fmod(6.2565837 + 0.017201977 * d_data->_day, DEUX_PI);

    // Termes solaires
    const double tmp1 = 2. * d_data->_ss1;
    const double tmp2 = 2. * d_data->_ss2;
    const double tmp3 = -2. * d_data->_ss3;
    const double tmp4 = 2. * d_data->_ss4;
    d_data->_se2 = tmp1 * d_data->_ss6;
    d_data->_se3 = tmp1 * d_data->_ss7;
    d_data->_si2 = tmp2 * d_data->_sz12;
    d_data->_si3 = tmp2 * (d_data->_sz13 - d_data->_sz11);
    d_data->_sl2 = tmp3 * d_data->_sz2;
    d_data->_sl3 = tmp3 * (d_data->_sz3 - d_data->_sz1);
    d_data->_sl4 = tmp3 * (-21. - 9. * d_data->_emsq) * ZES;
    d_data->_sgh2 = tmp4 * d_data->_sz32;
    d_data->_sgh3 = tmp4 * (d_data->_sz33 - d_data->_sz31);
    d_data->_sgh4 = -18. * d_data->_ss4 * ZES;
    d_data->_sh2 = -tmp2 * d_data->_sz22;
    d_data->_sh3 = -tmp2 * (d_data->_sz23 - d_data->_sz21);

    // Termes lunaires
    const double tmpl1 = 2. * d_data->_s1;
    const double tmpl2 = 2. * d_data->_s2;
    const double tmpl3 = -2. * d_data->_s3;
    const double tmpl4 = 2. * d_data->_s4;
    d_data->_ee2 = tmpl1 * d_data->_s6;
    d_data->_e3 = tmpl1 * d_data->_s7;
    d_data->_xi2 = tmpl2 * d_data->_z12;
    d_data->_xi3 = tmpl2 * (d_data->_z13 - d_data->_z11);
    d_data->_xl2 = tmpl3 * d_data->_z2;
    d_data->_xl3 = tmpl3 * (d_data->_z3 - d_data->_z1);
    d_data->_xl4 = tmpl3 * (-21. - 9. * d_data->_emsq) * ZEL;
    d_data->_xgh2 = tmpl4 * d_data->_z32;
    d_data->_xgh3 = tmpl4 * (d_data->_z33 - d_data->_z31);
    d_data->_xgh4 = -18. * d_data->_s4 * ZEL;
    d_data->_xh2 = -tmpl2 * d_data->_z22;
    d_data->_xh3 = -tmpl2 * (d_data->_z23 - d_data->_z21);

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void SGP4::Dsinit(const double tc) {

    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    d_data->_irez = 0;
    if ((d_data->_nm < 0.0052359877) && (d_data->_nm > 0.0034906585)) {
        d_data->_irez = 1;
    }

    if ((d_data->_nm >= 0.00826) && (d_data->_nm <= 0.00924) && (d_data->_em >= 0.5)) {
        d_data->_irez = 2;
    }

    // Termes solaires
    const double ses = d_data->_ss1 * ZNS * d_data->_ss5;
    const double sis = d_data->_ss2 * ZNS * (d_data->_sz11 + d_data->_sz13);
    const double sls = -(ZNS * d_data->_ss3 * (d_data->_sz1 + d_data->_sz3 - 14. - 6. * d_data->_emsq));
    const double sghs = d_data->_ss4 * ZNS * (d_data->_sz31 + d_data->_sz33 - 6.);
    double shs = -(ZNS * d_data->_ss2 * (d_data->_sz21 + d_data->_sz23));

    if ((d_data->_inclm < 0.052359877) || (d_data->_inclm > PI - 0.052359877)) {
        shs = 0.;
    }
    if (fabs(d_data->_sinim) > EPSDBL100) {
        shs /= d_data->_sinim;
    }

    const double sgs = sghs - d_data->_cosim * shs;

    // Termes lunaires
    d_data->_dedt = ses + d_data->_s1 * ZNL * d_data->_s5;
    d_data->_didt = sis + d_data->_s2 * ZNL * (d_data->_z11 + d_data->_z13);
    d_data->_dmdt = sls - ZNL * d_data->_s3 * (d_data->_z1 + d_data->_z3 - 14. - 6. * d_data->_emsq);
    const double sghl = d_data->_s4 * ZNL * (d_data->_z31 + d_data->_z33 - 6.);
    double shll = -(ZNL * d_data->_s2 * (d_data->_z21 + d_data->_z23));

    if ((d_data->_inclm < 0.052359877) || (d_data->_inclm > PI - 0.052359877)) {
        shll = 0.;
    }

    d_data->_domdt = sgs + sghl;
    d_data->_dnodt = shs;
    if (fabs(d_data->_sinim) > EPSDBL100) {
        d_data->_domdt = d_data->_domdt - (d_data->_cosim / d_data->_sinim * shll);
        d_data->_dnodt = d_data->_dnodt + (shll / d_data->_sinim);
    }

    // Calcul des effets de resonance haute orbite
    d_data->_dndt = 0.;
    const double theta = fmod(d_data->_gsto + tc * RPTIM, DEUX_PI);
    d_data->_em = d_data->_em + d_data->_dedt * d_data->_t;
    d_data->_inclm = d_data->_inclm + d_data->_didt * d_data->_t;
    d_data->_argpm = d_data->_argpm + d_data->_domdt * d_data->_t;
    d_data->_nodem = d_data->_nodem + d_data->_dnodt * d_data->_t;
    d_data->_mm = d_data->_mm + d_data->_dmdt * d_data->_t;
    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (d_data->_inclm < 0.) {
    //     d_data->_inclm = -d_data->_inclm;
    //     d_data->_argpm = d_data->_argpm - PI;
    //     d_data->_nodem = d_data->_nodem + PI;
    // }

    // Initialisation des termes de resonance
    if (d_data->_irez != 0) {

        const double aonv = pow(d_data->_nm / KE, DEUX_TIERS);

        // Resonance geopotentielle pour les orbites de 12h
        if (d_data->_irez == 2) {

            const double cosisq = d_data->_cosim * d_data->_cosim;
            const double emo = d_data->_em;
            d_data->_em = _ecco;
            const double emsqo = d_data->_emsq;
            d_data->_emsq = d_data->_eccsq;
            const double eoc = d_data->_em * d_data->_emsq;
            d_data->_g201 = -0.306 - (d_data->_em - 0.64) * 0.440;

            if (d_data->_em <= 0.65) {
                d_data->_g211 = 3.616 - 13.2470 * d_data->_em + 16.2900 * d_data->_emsq;
                d_data->_g310 = -19.302 + 117.3900 * d_data->_em - 228.4190 * d_data->_emsq + 156.5910 * eoc;
                d_data->_g322 = -18.9068 + 109.7927 * d_data->_em - 214.6334 * d_data->_emsq + 146.5816 * eoc;
                d_data->_g410 = -41.122 + 242.6940 * d_data->_em - 471.0940 * d_data->_emsq + 313.9530 * eoc;
                d_data->_g422 = -146.407 + 841.880 * d_data->_em - 1629.014 * d_data->_emsq + 1083.4350 * eoc;
                d_data->_g520 = -532.114 + 3017.977 * d_data->_em - 5740.032 * d_data->_emsq + 3708.2760 * eoc;
            } else {
                d_data->_g211 = -72.099 + 331.819 * d_data->_em - 508.738 * d_data->_emsq + 266.724 * eoc;
                d_data->_g310 = -346.844 + 1582.851 * d_data->_em - 2415.925 * d_data->_emsq + 1246.113 * eoc;
                d_data->_g322 = -342.585 + 1554.908 * d_data->_em - 2366.899 * d_data->_emsq + 1215.972 * eoc;
                d_data->_g410 = -1052.797 + 4758.686 * d_data->_em - 7193.992 * d_data->_emsq + 3651.957 * eoc;
                d_data->_g422 = -3581.690 + 16178.110 * d_data->_em - 24462.77 * d_data->_emsq + 12422.520 * eoc;
                if (d_data->_em > 0.715) {
                    d_data->_g520 = -5149.66 + 29936.92 * d_data->_em - 54087.36 * d_data->_emsq + 31324.56 * eoc;
                } else {
                    d_data->_g520 = 1464.74 - 4664.75 * d_data->_em + 3763.64 * d_data->_emsq;
                }
            }
            if (d_data->_em < 0.7) {
                d_data->_g533 = -919.22770 + 4988.6100 * d_data->_em - 9064.7700 * d_data->_emsq + 5542.21 * eoc;
                d_data->_g521 = -822.71072 + 4568.6173 * d_data->_em - 8491.4146 * d_data->_emsq + 5337.524 * eoc;
                d_data->_g532 = -853.66600 + 4690.2500 * d_data->_em - 8624.7700 * d_data->_emsq + 5341.4 * eoc;
            } else {
                d_data->_g533 = -37995.780 + 161616.52 * d_data->_em - 229838.20 * d_data->_emsq + 109377.94 * eoc;
                d_data->_g521 = -51752.104 + 218913.95 * d_data->_em - 309468.16 * d_data->_emsq + 146349.42 * eoc;
                d_data->_g532 = -40023.880 + 170470.89 * d_data->_em - 242699.48 * d_data->_emsq + 115605.82 * eoc;
            }

            const double sini2 = d_data->_sinim * d_data->_sinim;
            d_data->_f220 = 0.75 * (1. + 2. * d_data->_cosim + cosisq);
            d_data->_f221 = 1.5 * sini2;
            const double tmp1 = 1.875 * d_data->_sinim;
            const double tmp2 = 2. * d_data->_cosim;
            const double tmp3 = 3. * cosisq;
            d_data->_f321 = tmp1 * (1. - tmp2 - tmp3);
            d_data->_f322 = -tmp1 * (1. + tmp2 - tmp3);
            d_data->_f441 = 35. * sini2 * d_data->_f220;
            d_data->_f442 = 39.3750 * sini2 * sini2;
            const double tmp4 = 10. * cosisq;
            d_data->_f522 = 9.84375 * d_data->_sinim * (sini2 * (1. - tmp2 - 5. * cosisq) + 0.33333333 *
                                        (-2. + 4. * d_data->_cosim + 6. * cosisq));
            d_data->_f523 = d_data->_sinim * (4.92187512 * sini2 * (-2. - 4. * d_data->_cosim + tmp4) +
                              6.56250012 * (1. + tmp2 - tmp3));
            const double tmp5 = 8. * d_data->_cosim;
            const double tmp6 = 29.53125 * d_data->_sinim;
            d_data->_f542 = tmp6 * (2. - tmp5 + cosisq * (-12. + tmp5 + tmp4));
            d_data->_f543 = tmp6 * (-2. - tmp5 + cosisq * (12. + tmp5 - tmp4));

            const double xno2 = d_data->_nm * d_data->_nm;
            const double ainv2 = aonv * aonv;
            double temp1 = 3. * xno2 * ainv2;
            double temp = temp1 * ROOT22;
            d_data->_d2201 = temp * d_data->_f220 * d_data->_g201;
            d_data->_d2211 = temp * d_data->_f221 * d_data->_g211;
            temp1 *= aonv;
            temp = temp1 * ROOT32;
            d_data->_d3210 = temp * d_data->_f321 * d_data->_g310;
            d_data->_d3222 = temp * d_data->_f322 * d_data->_g322;
            temp1 *= aonv;
            temp = 2. * temp1 * ROOT44;
            d_data->_d4410 = temp * d_data->_f441 * d_data->_g410;
            d_data->_d4422 = temp * d_data->_f442 * d_data->_g422;
            temp1 *= aonv;
            temp = temp1 * ROOT52;
            d_data->_d5220 = temp * d_data->_f522 * d_data->_g520;
            d_data->_d5232 = temp * d_data->_f523 * d_data->_g532;
            temp = 2. * temp1 * ROOT54;
            d_data->_d5421 = temp * d_data->_f542 * d_data->_g521;
            d_data->_d5433 = temp * d_data->_f543 * d_data->_g533;
            d_data->_xlamo = fmod(_mo + _omegao + _omegao - theta - theta, DEUX_PI);
            d_data->_xfact = d_data->_mdot + d_data->_dmdt + 2. * (d_data->_nodedot + d_data->_dnodt - RPTIM) - _no;
            d_data->_em = emo;
            d_data->_emsq = emsqo;
        }

        // Termes de resonance synchrones
        if (d_data->_irez == 1) {
            d_data->_g200 = 1. + d_data->_emsq * (-2.5 + 0.8125 * d_data->_emsq);
            d_data->_g310 = 1. + 2. * d_data->_emsq;
            d_data->_g300 = 1. + d_data->_emsq * (-6. + 6.60937 * d_data->_emsq);
            d_data->_f220 = 0.75 * (1. + d_data->_cosim) * (1. + d_data->_cosim);
            d_data->_f311 = 0.9375 * d_data->_sinim * d_data->_sinim * (1. + 3. * d_data->_cosim) - 0.75 * (1. + d_data->_cosim);
            d_data->_f330 = 1. + d_data->_cosim;
            d_data->_f330 = 1.875 * d_data->_f330 * d_data->_f330 * d_data->_f330;
            d_data->_del1 = 3. * d_data->_nm * d_data->_nm * aonv * aonv;
            d_data->_del2 = 2. * d_data->_del1 * d_data->_f220 * d_data->_g200 * Q22;
            d_data->_del3 = 3. * d_data->_del1 * d_data->_f330 * d_data->_g300 * Q33 * aonv;
            d_data->_del1 = d_data->_del1 * d_data->_f311 * d_data->_g310 * Q31 * aonv;
            d_data->_xlamo = fmod(_mo + _omegao + _argpo - theta, DEUX_PI);
            d_data->_xfact = d_data->_mdot + d_data->_xpidot - RPTIM + d_data->_dmdt + d_data->_domdt + d_data->_dnodt - _no;
        }

        // Initialisation de l'integrateur
        d_data->_xli = d_data->_xlamo;
        d_data->_xni = _no;
        d_data->_atime = 0.;
        d_data->_nm = _no + d_data->_dndt;
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void SGP4::Dspace(const double tc) {

    /* Declarations des variables locales */

    /* Initialisations */
    double xldot = 0.;
    double xnddt = 0.;
    double xndt = 0.;

    /* Corps de la methode */
    // Calcul des effets de resonance haute orbite
    d_data->_dndt = 0.;
    const double theta = fmod(d_data->_gsto + tc * RPTIM, DEUX_PI);
    d_data->_em = d_data->_em + d_data->_dedt * d_data->_t;
    d_data->_inclm = d_data->_inclm + d_data->_didt * d_data->_t;
    d_data->_argpm = d_data->_argpm + d_data->_domdt * d_data->_t;
    d_data->_nodem = d_data->_nodem + d_data->_dnodt * d_data->_t;
    d_data->_mm = d_data->_mm + d_data->_dmdt * d_data->_t;

    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (d_data->_inclm < 0.) {
    //     d_data->_inclm = -d_data->_inclm;
    //     d_data->_argpm = d_data->_argpm - PI;
    //     d_data->_nodem = d_data->_nodem + PI;
    // }

    // Integration numerique (Euler-MacLaurin)
    if (d_data->_irez != 0) {

        double ft = 0.;
        if (fabs(d_data->_atime) <= EPSDBL100 || d_data->_t * d_data->_atime <= 0. || fabs(d_data->_t) < fabs(d_data->_atime)) {
            d_data->_atime = 0.;
            d_data->_xni = _no;
            d_data->_xli = d_data->_xlamo;
        }

        d_data->_delt = (d_data->_t > 0.) ? STEPP : STEPN;

        int iretn = 381;
        while (iretn == 381) {
            // Calculs des termes derives

            // Termes de resonance quasi-synchrones
            if (d_data->_irez != 2) {
                xndt = d_data->_del1 * sin(d_data->_xli - FASX2) + d_data->_del2 * sin(2. * (d_data->_xli - FASX4)) +
                        d_data->_del3 * sin(3. * (d_data->_xli - FASX6));
                xldot = d_data->_xni + d_data->_xfact;
                xnddt = d_data->_del1 * cos(d_data->_xli - FASX2) + 2. * d_data->_del2 * cos(2. * (d_data->_xli - FASX4)) + 3. * d_data->_del3 *
                        cos(3. * (d_data->_xli - FASX6));
                xnddt *= xldot;
            } else {

                // Termes de resonance d'environ 12h
                const double xomi = _argpo + d_data->_argpdot * d_data->_atime;
                const double x2omi = xomi + xomi;
                const double x2li = d_data->_xli + d_data->_xli;
                xndt = d_data->_d2201 * sin(x2omi + d_data->_xli - G22) + d_data->_d2211 * sin(d_data->_xli - G22) +
                        d_data->_d3210 * sin(xomi + d_data->_xli - G32) + d_data->_d3222 * sin(-xomi + d_data->_xli - G32) +
                        d_data->_d4410 * sin(x2omi + x2li - G44) + d_data->_d4422 * sin(x2li - G44) + d_data->_d5220 *
                        sin(xomi + d_data->_xli - G52) + d_data->_d5232 * sin(-xomi + d_data->_xli - G52) + d_data->_d5421 *
                        sin(xomi + x2li - G54) + d_data->_d5433 * sin(-xomi + x2li - G54);
                xldot = d_data->_xni + d_data->_xfact;
                xnddt = d_data->_d2201 * cos(x2omi + d_data->_xli - G22) + d_data->_d2211 * cos(d_data->_xli - G22) +
                        d_data->_d3210 * cos(xomi + d_data->_xli - G32) + d_data->_d3222 * cos(-xomi + d_data->_xli - G32) +
                        d_data->_d5220 * cos(xomi + d_data->_xli - G52) + d_data->_d5232 * cos(-xomi + d_data->_xli - G52) +
                        2.0 * (d_data->_d4410 * cos(x2omi + x2li - G44) + d_data->_d4422 * cos(x2li - G44) +
                               d_data->_d5421 * cos(xomi + x2li - G54) + d_data->_d5433 * cos(-xomi + x2li - G54));
                xnddt *= xldot;
            }

            // Integrateur
            if (fabs(d_data->_t - d_data->_atime) >= STEPP) {
                iretn = 381;
            } else {
                ft = d_data->_t - d_data->_atime;
                iretn = 0;
            }

            if (iretn == 381) {
                d_data->_xli = d_data->_xli + xldot * d_data->_delt + xndt * STEP2;
                d_data->_xni = d_data->_xni + xndt * d_data->_delt + xnddt * STEP2;
                d_data->_atime += d_data->_delt;
            }
        }

        const double ft2 = ft * ft * 0.5;
        d_data->_nm = d_data->_xni + xndt * ft + xnddt * ft2;
        const double xl = d_data->_xli + xldot * ft + xndt * ft2;
        if (d_data->_irez == 1) {
            d_data->_mm = xl - d_data->_nodem - d_data->_argpm + theta;
            d_data->_dndt = d_data->_nm - _no;
        } else {
            d_data->_mm = xl - 2. * d_data->_nodem + 2. * theta;
            d_data->_dndt = d_data->_nm - _no;
        }
        d_data->_nm = _no + d_data->_dndt;
    }

    /* Retour */
    return;
}

/*
 * Initialisation du modele SGP4
 */
void SGP4::SGP4Init(const TLE &tle)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Recuperation des elements du TLE et formattage
    _argpo = DEG2RAD * tle.argpo();
    _bstar = tle.bstar();
    _ecco = tle.ecco();
    _epoque = tle.epoque();
    _inclo = DEG2RAD * tle.inclo();
    _mo = DEG2RAD * tle.mo();
    _no = tle.no() * DEUX_PI * NB_JOUR_PAR_MIN;
    _omegao = DEG2RAD * tle.omegao();

    const double ss = 78. * X1SRT + 1.;
    const double qzms2t = pow((120. - 78.) * X1SRT, 4.);
    d_data->_t = 0.;

    d_data->_eccsq = _ecco * _ecco;
    d_data->_omeosq = 1. - d_data->_eccsq;
    d_data->_rteosq = sqrt(d_data->_omeosq);
    d_data->_cosio = cos(_inclo);
    d_data->_cosio2 = d_data->_cosio * d_data->_cosio;

    const double ak = pow(KE / _no, DEUX_TIERS);
    const double d1 = 0.75 * J2 * (3. * d_data->_cosio2 - 1.) / (d_data->_rteosq * d_data->_omeosq);
    double del = d1 / (ak * ak);
    const double del2 = del * del;
    const double adel = ak * (1. - del2 - del * (0.5 * DEUX_TIERS + del2 * (134. / 81.)));
    del = d1 / (adel * adel);
    _no = _no / (1. + del);

    d_data->_ao = pow(KE / _no, DEUX_TIERS);
    d_data->_sinio = sin(_inclo);
    const double po = d_data->_ao * d_data->_omeosq;
    d_data->_con42 = 1. - 5. * d_data->_cosio2;
    d_data->_con41 = -d_data->_con42 - d_data->_cosio2 - d_data->_cosio2;
    d_data->_posq = po * po;
    d_data->_rp = d_data->_ao * (1. - _ecco);
    d_data->_method = 'n';

    d_data->_gsto = Observateur::CalculTempsSideralGreenwich(_epoque);

    if ((d_data->_omeosq >= 0.) || (_no > 0.)) {

        d_data->_isimp = (d_data->_rp < 220. * X1SRT + 1.);

        double sfour = ss;
        double qzms24 = qzms2t;

        const double perige = (d_data->_rp - 1.) * RAYON_TERRESTRE;
        if (perige < 156.) {
            sfour = perige - 78.;
            if (perige < 98.) {
                sfour = 20.;
            }

            qzms24 = pow((120. - sfour) * X1SRT, 4.);
            sfour = sfour / RAYON_TERRESTRE + 1.;
        }

        const double pinvsq = 1. / d_data->_posq;
        const double tsi = 1. / (d_data->_ao - sfour);
        d_data->_eta = d_data->_ao * _ecco * tsi;

        const double etasq = d_data->_eta * d_data->_eta;
        const double eeta = _ecco * d_data->_eta;
        const double psisq = fabs(1. - etasq);
        const double coef = qzms24 * pow(tsi, 4.);
        const double coef1 = coef * pow(psisq, -3.5);
        const double cc2 = coef1 * _no * (d_data->_ao * (1. + 1.5 * etasq + eeta * (4. + etasq)) + 0.375 * J2 * tsi / psisq *
                                          d_data->_con41 * (8. + 3. * etasq * (8. + etasq)));
        d_data->_cc1 = _bstar * cc2;
        double cc3 = 0.;
        if (_ecco > 1.e-4) {
            cc3 = -2. * coef * tsi * J3SJ2 * _no * d_data->_sinio / _ecco;
        }

        d_data->_x1mth2 = 1. - d_data->_cosio2;
        d_data->_cc4 = 2. * _no * coef1 * d_data->_ao * d_data->_omeosq *
                (d_data->_eta * (2. + 0.5 * etasq) + _ecco * (0.5 + 2. * etasq) - J2 * tsi /
                 (d_data->_ao * psisq) * (-3. * d_data->_con41 * (1. - 2. * eeta + etasq * (1.5 - 0.5 * eeta)) +
                                  0.75 * d_data->_x1mth2 * (2. * etasq - eeta * (1. + etasq)) * cos(2. * _argpo)));
        d_data->_cc5 = 2. * coef1 * d_data->_ao * d_data->_omeosq * (1. + 2.75 * (etasq + eeta) + eeta * etasq);

        const double cosio4 = d_data->_cosio2 * d_data->_cosio2;
        const double temp1 = 1.5 * J2 * pinvsq * _no;
        const double temp2 = 0.5 * temp1 * J2 * pinvsq;
        const double temp3 = -0.46875 * J4 * pinvsq * pinvsq * _no;
        d_data->_mdot = _no + 0.5 * temp1 * d_data->_rteosq * d_data->_con41 + 0.0625 * temp2 * d_data->_rteosq *
                (13. - 78. * d_data->_cosio2 + 137. * cosio4);
        d_data->_argpdot = -0.5 * temp1 * d_data->_con42 + 0.0625 * temp2 * (7. - 114. * d_data->_cosio2 + 395. * cosio4) +
                temp3 * (3. - 36. * d_data->_cosio2 + 49. * cosio4);
        const double xhdot1 = -temp1 * d_data->_cosio;
        d_data->_nodedot = xhdot1 + (0.5 * temp2 * (4. - 19. * d_data->_cosio2) + 2. * temp3 * (3. - 7. * d_data->_cosio2)) * d_data->_cosio;
        d_data->_xpidot = d_data->_argpdot + d_data->_nodedot;
        d_data->_omgcof = _bstar * cc3 * cos(_argpo);
        d_data->_xmcof = 0.;
        if (_ecco > 1.e-4) {
            d_data->_xmcof = -DEUX_TIERS * coef * _bstar / eeta;
        }

        d_data->_nodecf = 3.5 * d_data->_omeosq * xhdot1 * d_data->_cc1;
        d_data->_t2cof = 1.5 * d_data->_cc1;

        if (fabs(d_data->_cosio + 1.) > 1.5e-12) {
            d_data->_xlcof = -0.25 * J3SJ2 * d_data->_sinio * (3. + 5. * d_data->_cosio) / (1. + d_data->_cosio);
        } else {
            d_data->_xlcof = -0.25 * J3SJ2 * d_data->_sinio * (3. + 5. * d_data->_cosio) * (1. / 1.5e-12);
        }

        d_data->_aycof = -0.5 * J3SJ2 * d_data->_sinio;
        d_data->_delmo = pow((1. + d_data->_eta * cos(_mo)), 3.);
        d_data->_sinmao = sin(_mo);
        d_data->_x7thm1 = 7. * d_data->_cosio2 - 1.;

        // Initialisation du modele haute orbite
        if (DEUX_PI >= 225. * _no) {

            d_data->_method = 'd';
            d_data->_isimp = true;
            d_data->_inclm = _inclo;

            const double tc = 0.;
            Dscom(tc);

            d_data->_mp = _mo;
            d_data->_argpp = _argpo;
            d_data->_ep = _ecco;
            d_data->_nodep = _omegao;
            d_data->_xincp = _inclo;

            Dpper();

            d_data->_argpm = 0.;
            d_data->_nodem = 0.;
            d_data->_mm = 0.;

            Dsinit(tc);
        }

        if (!d_data->_isimp) {

            const double cc1sq = d_data->_cc1 * d_data->_cc1;
            d_data->_d2 = 4. * d_data->_ao * tsi * cc1sq;
            const double temp = d_data->_d2 * tsi * d_data->_cc1 * (1. / 3.);
            d_data->_d3 = (17. * d_data->_ao + sfour) * temp;
            d_data->_d4 = 0.5 * temp * d_data->_ao * tsi * (221. * d_data->_ao + 31. * sfour) * d_data->_cc1;
            d_data->_t3cof = d_data->_d2 + 2. * cc1sq;
            d_data->_t4cof = 0.25 * (3. * d_data->_d3 + d_data->_cc1 * (12. * d_data->_d2 + 10. * cc1sq));
            d_data->_t5cof = 0.2 * (3. * d_data->_d4 + 12. * d_data->_cc1 * d_data->_d3 + 6. * d_data->_d2 * d_data->_d2 + 15. * cc1sq * (2. * d_data->_d2 + cc1sq));
        }

        _init = true;
    }

    /* Retour */
    return;
}
