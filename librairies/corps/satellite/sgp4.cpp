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
 * >     20 octobre 2019
 *
 */

#include <cmath>
#include "librairies/corps/systemesolaire/TerreConstants.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/observateur/observateur.h"
#include "sgp4.h"

static const double J3SJ2 = J3 / J2;
static const double RPTIM = OMEGA * NB_SEC_PAR_MIN;
static const double X1SRT = 1. / RAYON_TERRESTRE;
static const double RTMS = RAYON_TERRESTRE * NB_MIN_PAR_SEC;

/* Constructeurs */
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

    // Variables du modele SGP4
    _isimp = false;
    _method = 'n';
    _irez = 0;
    _ao = 0.;
    _argpdot = 0.;
    _argpm = 0.;
    _argpp = 0.;
    _atime = 0.;
    _aycof = 0.;
    _cc1 = 0.;
    _cc4 = 0.;
    _cc5 = 0.;
    _cnodm = 0.;
    _con41 = 0.;
    _con42 = 0.;
    _cosim = 0.;
    _cosio = 0.;
    _cosio2 = 0.;
    _cosomm = 0.;
    _d2 = 0.;
    _d2201 = 0.;
    _d2211 = 0.;
    _d3 = 0.;
    _d3210 = 0.;
    _d3222 = 0.;
    _d4 = 0.;
    _d4410 = 0.;
    _d4422 = 0.;
    _d5220 = 0.;
    _d5232 = 0.;
    _d5421 = 0.;
    _d5433 = 0.;
    _day = 0.;
    _dedt = 0.;
    _del1 = 0.;
    _del2 = 0.;
    _del3 = 0.;
    _delmo = 0.;
    _delt = 0.;
    _didt = 0.;
    _dmdt = 0.;
    _domdt = 0.;
    _dndt = 0.;
    _dnodt = 0.;
    _eccsq = 0.;
    _ee2 = 0.;
    _e3 = 0.;
    _em = 0.;
    _emsq = 0.;
    _ep = 0.;
    _eta = 0.;
    _f220 = 0.;
    _f221 = 0.;
    _f311 = 0.;
    _f321 = 0.;
    _f322 = 0.;
    _f330 = 0.;
    _f441 = 0.;
    _f442 = 0.;
    _f522 = 0.;
    _f523 = 0.;
    _f542 = 0.;
    _f543 = 0.;
    _g200 = 0.;
    _g201 = 0.;
    _g211 = 0.;
    _g300 = 0.;
    _g310 = 0.;
    _g322 = 0.;
    _g410 = 0.;
    _g422 = 0.;
    _g520 = 0.;
    _g521 = 0.;
    _g532 = 0.;
    _g533 = 0.;
    _gam = 0.;
    _gsto = 0.;
    _inclm = 0.;
    _mdot = 0.;
    _mm = 0.;
    _mp = 0.;
    _nm = 0.;
    _nodecf = 0.;
    _nodedot = 0.;
    _nodem = 0.;
    _nodep = 0.;
    _omeosq = 0.;
    _omgcof = 0.;
    _posq = 0.;
    _rp = 0.;
    _rtemsq = 0.;
    _rteosq = 0.;
    _s1 = 0.;
    _s2 = 0.;
    _s3 = 0.;
    _s4 = 0.;
    _s5 = 0.;
    _s6 = 0.;
    _s7 = 0.;
    _se2 = 0.;
    _se3 = 0.;
    _sgh2 = 0.;
    _sgh3 = 0.;
    _sgh4 = 0.;
    _sh2 = 0.;
    _sh3 = 0.;
    _si2 = 0.;
    _si3 = 0.;
    _sinim = 0.;
    _sinio = 0.;
    _sinmao = 0.;
    _sinomm = 0.;
    _sl2 = 0.;
    _sl3 = 0.;
    _sl4 = 0.;
    _snodm = 0.;
    _ss1 = 0.;
    _ss2 = 0.;
    _ss3 = 0.;
    _ss4 = 0.;
    _ss5 = 0.;
    _ss6 = 0.;
    _ss7 = 0.;
    _sz1 = 0.;
    _sz2 = 0.;
    _sz3 = 0.;
    _sz11 = 0.;
    _sz12 = 0.;
    _sz13 = 0.;
    _sz21 = 0.;
    _sz22 = 0.;
    _sz23 = 0.;
    _sz31 = 0.;
    _sz32 = 0.;
    _sz33 = 0.;
    _t = 0.;
    _t2cof = 0.;
    _t3cof = 0.;
    _t4cof = 0.;
    _t5cof = 0.;
    _x1mth2 = 0.;
    _x7thm1 = 0.;
    _xfact = 0.;
    _xi2 = 0.;
    _xi3 = 0.;
    _xincp = 0.;
    _xgh2 = 0.;
    _xgh3 = 0.;
    _xgh4 = 0.;
    _xh2 = 0.;
    _xh3 = 0.;
    _xl2 = 0.;
    _xl3 = 0.;
    _xl4 = 0.;
    _xlamo = 0.;
    _xlcof = 0.;
    _xli = 0.;
    _xmcof = 0.;
    _xni = 0.;
    _xpidot = 0.;
    _z1 = 0.;
    _z2 = 0.;
    _z3 = 0.;
    _z11 = 0.;
    _z12 = 0.;
    _z13 = 0.;
    _z21 = 0.;
    _z22 = 0.;
    _z23 = 0.;
    _z31 = 0.;
    _z32 = 0.;
    _z33 = 0.;
    _zmol = 0.;
    _zmos = 0.;
    _init = false;

    /* Retour */
    return;
}

/* Methodes */
void SGP4::Calcul(const Date &date, const TLE &tle)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_init) {
        SGP4Init(tle);
    }

    /* Corps de la methode */
    try {

        int ktr;
        double coseo1, cosip, eo1, sineo1, sinip, tem5, temp, tempa, tempe, templ, xlm;

        coseo1 = 0.;
        sineo1 = 0.;

        // Calcul du temps ecoule depuis l'epoque (en minutes)
        const double tsince = NB_MIN_PAR_JOUR * (date.jourJulienUTC() - tle.epoque().jourJulienUTC());

        _t = tsince;

        // Prise en compte des termes seculaires de la gravite et du freinage atmospherique
        const double xmdf = _mo + _mdot * _t;
        const double argpdf = _argpo + _argpdot * _t;
        const double nodedf = _omegao + _nodedot * _t;
        _argpm = argpdf;
        _mm = xmdf;
        const double tt2 = _t * _t;
        _nodem = nodedf + _nodecf * tt2;
        tempa = 1. - _cc1 * _t;
        tempe = _bstar * _cc4 * _t;
        templ = _t2cof * tt2;

        if (!_isimp) {

            const double delomg = _omgcof * _t;
            const double delmtmp = 1. + _eta * cos(xmdf);
            const double delm = _xmcof * (delmtmp * delmtmp * delmtmp - _delmo);
            temp = delomg + delm;
            _mm = xmdf + temp;
            _argpm = argpdf - temp;
            const double tt3 = tt2 * _t;
            const double t4 = tt3 * _t;
            tempa += -_d2 * tt2 - _d3 * tt3 - _d4 * t4;
            tempe += _bstar * _cc5 * (sin(_mm) - _sinmao);
            templ += _t3cof * tt3 + t4 * (_t4cof + _t * _t5cof);
        }

        _nm = _no;
        _em = _ecco;
        _inclm = _inclo;

        if (_method == 'd') {
            const double tc = _t;
            Dspace(tc);
        }

        const double am = pow((KE / _nm), DEUX_TIERS) * tempa * tempa;
        _nm = KE * pow(am, -1.5);
        _em -= tempe;

        if (_em < 1.e-6) {
            _em = 1.e-6;
        }

        _mm += _no * templ;
        xlm = _mm + _argpm + _nodem;
        _emsq = _em * _em;
        temp = 1. - _emsq;

        _nodem = fmod(_nodem, DEUX_PI);
        _argpm = fmod(_argpm, DEUX_PI);
        xlm = fmod(xlm, DEUX_PI);
        _mm = fmod(xlm - _argpm - _nodem, DEUX_PI);

        _sinim = sin(_inclm);
        _cosim = cos(_inclm);

        // Prise en compte des termes periodiques luni-solaires
        _ep = _em;
        _xincp = _inclm;
        _argpp = _argpm;
        _nodep = _nodem;
        _mp = _mm;
        sinip = _sinim;
        cosip = _cosim;

        // Termes longue periode
        if (_method == 'd') {
            Dpper();
            if (_xincp < 0.) {
                _xincp = -_xincp;
                _nodep += PI;
                _argpp -= PI;
            }

            sinip = sin(_xincp);
            cosip = cos(_xincp);
            _aycof = -0.5 * J3SJ2 * sinip;

            if (fabs(cosip + 1.) > 1.5e-12) {
                _xlcof = -0.25 * J3SJ2 * sinip * (3. + 5. * cosip) / (1. + cosip);
            } else {
                _xlcof = -0.25 * J3SJ2 * sinip * (3. + 5. * cosip) * (1. / 1.5e-12);
            }
        }

        const double axnl = _ep * cos(_argpp);
        temp = 1. / (am * (1. - _ep * _ep));
        const double aynl = _ep * sin(_argpp) + temp * _aycof;
        const double xl = _mp + _argpp + _nodep + temp * _xlcof * axnl;

        // Resolution de l'equation de Kepler
        const double u = fmod(xl - _nodep, DEUX_PI);
        eo1 = u;
        tem5 = 9999.9;
        ktr = 1;

        while (fabs(tem5) >= EPSDBL && ktr <= 10) {
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
            double su;

            const double rl = am * (1. - ecose);
            const double rdotl = sqrt(am) * esine / rl;
            const double rvdotl = sqrt(pl) / rl;
            const double betal = sqrt(1. - el2);
            temp = esine / (1. + betal);
            const double tmp1 = am / rl;
            const double sinu = tmp1 * (sineo1 - aynl - axnl * temp);
            const double cosu = tmp1 * (coseo1 - axnl + aynl * temp);
            su = atan2(sinu, cosu);
            const double sin2u = (cosu + cosu) * sinu;
            const double cos2u = 1. - 2. * sinu * sinu;
            temp = 1. / pl;
            const double temp1 = 0.5 * J2 * temp;
            const double temp2 = temp1 * temp;

            // Prise en compte des termes courte periode
            if (_method == 'd') {

                const double cosisq = cosip * cosip;
                _con41 = 3. * cosisq - 1.;
                _x1mth2 = 1. - cosisq;
                _x7thm1 = 7. * cosisq - 1.;
            }

            const double mrt = RAYON_TERRESTRE * (rl * (1. - 1.5 * temp2 * betal * _con41) + 0.5 * temp1 * _x1mth2 * cos2u);
            su -= 0.25 * temp2 * _x7thm1 * sin2u;
            const double temp3 = 1.5 * temp2 * cosip;
            const double xnode = _nodep + temp3 * sin2u;
            const double xinc = _xincp + temp3 * sinip * cos2u;
            const double mvt = RTMS * (KE * rdotl - _nm * temp1 * _x1mth2 * sin2u);
            const double rvdot = RTMS * (KE * rvdotl + _nm * temp1 * (_x1mth2 * cos2u + 1.5 * _con41));

            // Vecteurs directeurs
            const double sinsu = sin(su);
            const double cossu = cos(su);
            const double snod = sin(xnode);
            const double cnod = cos(xnode);
            const double cosi = cos(xinc);

            const Vecteur3D mm(-snod * cosi, cnod * cosi, sin(xinc));
            const Vecteur3D mm1 = mm * sinsu;
            const Vecteur3D mm2 = mm * cossu;

            const Vecteur3D nn(cnod, snod, 0.);
            const Vecteur3D nn1 = nn * (-cossu);
            const Vecteur3D nn2 = nn * sinsu;

            const Vecteur3D uu = mm1 - nn1;
            const Vecteur3D uu1 = uu * mvt;
            const Vecteur3D vv = mm2 - nn2;
            const Vecteur3D vv1 = vv * (-rvdot);

            // Position et vitesse
            _position = uu * mrt;
            _vitesse = uu1 - vv1;
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void SGP4::Dpper() {

    /* Declarations des variables locales */
    double f2, f3, pgh, ph, sinzf, zf, zm;

    /* Initialisations */

    /* Corps de la methode */
    zm = _zmos + ZNS * _t;
    if (!_init) {
        zm = _zmos;
    }

    zf = zm + 2. * ZES * sin(zm);
    sinzf = sin(zf);
    f2 = 0.5 * sinzf * sinzf - 0.25;
    f3 = -0.5 * sinzf * cos(zf);
    const double ses = _se2 * f2 + _se3 * f3;
    const double sis = _si2 * f2 + _si3 * f3;
    const double sls = _sl2 * f2 + _sl3 * f3 + _sl4 * sinzf;
    const double sghs = _sgh2 * f2 + _sgh3 * f3 + _sgh4 * sinzf;
    const double shs = _sh2 * f2 + _sh3 * f3;
    zm = _zmol + ZNL * _t;
    if (!_init) {
        zm = _zmol;
    }

    zf = zm + 2. * ZEL * sin(zm);
    sinzf = sin(zf);
    f2 = 0.5 * sinzf * sinzf - 0.25;
    f3 = -0.5 * sinzf * cos(zf);

    const double sel = _ee2 * f2 + _e3 * f3;
    const double sil = _xi2 * f2 + _xi3 * f3;
    const double sll = _xl2 * f2 + _xl3 * f3 + _xl4 * sinzf;
    const double sghl = _xgh2 * f2 + _xgh3 * f3 + _xgh4 * sinzf;
    const double shll = _xh2 * f2 + _xh3 * f3;
    const double pe = ses + sel;
    const double pinc = sis + sil;
    const double pl = sls + sll;
    pgh = sghs + sghl;
    ph = shs + shll;

    if (_init) {

        _xincp += pinc;
        _ep += pe;
        const double sinip = sin(_xincp);
        const double cosip = cos(_xincp);

        // Application directe des termes periodiques
        if (_xincp >= 0.2) {
            ph /= sinip;
            pgh -= cosip * ph;
            _argpp += pgh;
            _nodep += ph;
            _mp += pl;
        } else {

            // Application des termes periodiques avec la modification de Lyddane
            double alfdp, betdp, xls;

            const double sinop = sin(_nodep);
            const double cosop = cos(_nodep);
            alfdp = sinip * sinop;
            betdp = sinip * cosop;
            const double tmp = pinc * cosip;
            const double dalf = ph * cosop + tmp * sinop;
            const double dbet = -ph * sinop + tmp * cosop;
            alfdp += dalf;
            betdp += dbet;
            _nodep = fmod(_nodep, DEUX_PI);
            if (_nodep < 0.) {
                _nodep += DEUX_PI;
            }

            xls = _mp + _argpp + cosip * _nodep;
            const double dls = pl + pgh - pinc * _nodep * sinip;
            xls += dls;
            const double xnoh = _nodep;
            _nodep = atan2(alfdp, betdp);
            if (_nodep < 0.) {
                _nodep += DEUX_PI;
            }

            if (fabs(xnoh - _nodep) > PI) {
                _nodep = (_nodep < xnoh) ? _nodep + DEUX_PI : _nodep - DEUX_PI;
            }

            _mp += pl;
            _argpp = xls - _mp - cosip * _nodep;
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
    double zx;

    /* Initialisations */

    /* Corps de la methode */
    _nm = _no;
    _em = _ecco;
    _snodm = sin(_omegao);
    _cnodm = cos(_omegao);
    _sinomm = sin(_argpo);
    _cosomm = cos(_argpo);
    _sinim = sin(_inclo);
    _cosim = cos(_inclo);
    _emsq = _em * _em;
    const double betasq = 1. - _emsq;
    _rtemsq = sqrt(betasq);

    // Initialisation des termes luni-solaires
    _day = _epoque.jourJulienUTC() + NB_JOURS_PAR_SIECJ + tc * NB_JOUR_PAR_MIN;
    const double xnodce = fmod(4.523602 - 0.00092422029 * _day, DEUX_PI);
    const double stem = sin(xnodce);
    const double ctem = cos(xnodce);
    const double zcosil = 0.91375164 - 0.03568096 * ctem;
    const double zsinil = sqrt(1. - zcosil * zcosil);
    const double zsinhl = 0.089683511 * stem / zsinil;
    const double zcoshl = sqrt(1. - zsinhl * zsinhl);
    _gam = 5.8351514 + 0.001944368 * _day;
    zx = 0.39785416 * stem / zsinil;
    const double zy = zcoshl * ctem + 0.91744867 * zsinhl * stem;
    zx = atan2(zx, zy);
    zx = _gam + zx - xnodce;
    const double zcosgl = cos(zx);
    const double zsingl = sin(zx);

    // Termes solaires
    double zcosg = ZCOSGS;
    double zsing = ZSINGS;
    double zcosi = ZCOSIS;
    double zsini = ZSINIS;
    double zcosh = _cnodm;
    double zsinh = _snodm;
    double cc = C1SS;
    double xnoi = 1. / _nm;

    for (int lsflg = 1; lsflg <= 2; lsflg++) {

        const double a0 = zcosi * zsinh;
        const double a1 = zcosg * zcosh + zsing * a0;
        const double a3 = -zsing * zcosh + zcosg * a0;
        const double a7 = -zcosg * zsinh + zsing * zcosi * zcosh;
        const double a8 = zsing * zsini;
        const double a9 = zsing * zsinh + zcosg * zcosi * zcosh;
        const double a10 = zcosg * zsini;
        const double a2 = _cosim * a7 + _sinim * a8;
        const double a4 = _cosim * a9 + _sinim * a10;
        const double a5 = -_sinim * a7 + _cosim * a8;
        const double a6 = -_sinim * a9 + _cosim * a10;

        const double x1 = a1 * _cosomm + a2 * _sinomm;
        const double x2 = a3 * _cosomm + a4 * _sinomm;
        const double x3 = -a1 * _sinomm + a2 * _cosomm;
        const double x4 = -a3 * _sinomm + a4 * _cosomm;
        const double x5 = a5 * _sinomm;
        const double x6 = a6 * _sinomm;
        const double x7 = a5 * _cosomm;
        const double x8 = a6 * _cosomm;

        _z31 = 12. * x1 * x1 - 3. * x3 * x3;
        _z32 = 24. * x1 * x2 - 6. * x3 * x4;
        _z33 = 12. * x2 * x2 - 3. * x4 * x4;
        _z1 = 3. * (a1 * a1 + a2 * a2) + _z31 * _emsq;
        _z2 = 6. * (a1 * a3 + a2 * a4) + _z32 * _emsq;
        _z3 = 3. * (a3 * a3 + a4 * a4) + _z33 * _emsq;
        _z11 = -6. * a1 * a5 + _emsq * (-24. * x1 * x7 - 6. * x3 * x5);
        _z12 = -6. * (a1 * a6 + a3 * a5) + _emsq * (-24. * (x2 * x7 + x1 * x8) - 6. * (x3 * x6 + x4 * x5));
        _z13 = -6. * a3 * a6 + _emsq * (-24. * x2 * x8 - 6. * x4 * x6);
        _z21 = 6. * a2 * a5 + _emsq * (24. * x1 * x5 - 6. * x3 * x7);
        _z22 = 6. * (a4 * a5 + a2 * a6) + _emsq * (24. * (x2 * x5 + x1 * x6) - 6. * (x4 * x7 + x3 * x8));
        _z23 = 6. * a4 * a6 + _emsq * (24. * x2 * x6 - 6. * x4 * x8);
        _z1 = _z1 + _z1 + betasq * _z31;
        _z2 = _z2 + _z2 + betasq * _z32;
        _z3 = _z3 + _z3 + betasq * _z33;
        _s3 = cc * xnoi;
        _s2 = -0.5 * _s3 / _rtemsq;
        _s4 = _s3 * _rtemsq;
        _s1 = -15. * _em * _s4;
        _s5 = x1 * x3 + x2 * x4;
        _s6 = x2 * x3 + x1 * x4;
        _s7 = x2 * x4 - x1 * x3;

        // Termes lunaires
        if (lsflg == 1) {
            _ss1 = _s1;
            _ss2 = _s2;
            _ss3 = _s3;
            _ss4 = _s4;
            _ss5 = _s5;
            _ss6 = _s6;
            _ss7 = _s7;
            _sz1 = _z1;
            _sz2 = _z2;
            _sz3 = _z3;
            _sz11 = _z11;
            _sz12 = _z12;
            _sz13 = _z13;
            _sz21 = _z21;
            _sz22 = _z22;
            _sz23 = _z23;
            _sz31 = _z31;
            _sz32 = _z32;
            _sz33 = _z33;
            zcosg = zcosgl;
            zsing = zsingl;
            zcosi = zcosil;
            zsini = zsinil;
            zcosh = zcoshl * _cnodm + zsinhl * _snodm;
            zsinh = _snodm * zcoshl - _cnodm * zsinhl;
            cc = C1L;
        }
    }

    _zmol = fmod(4.7199672 + 0.2299715 * _day - _gam, DEUX_PI);
    _zmos = fmod(6.2565837 + 0.017201977 * _day, DEUX_PI);

    // Termes solaires
    const double tmp1 = 2. * _ss1;
    const double tmp2 = 2. * _ss2;
    const double tmp3 = -2. * _ss3;
    const double tmp4 = 2. * _ss4;
    _se2 = tmp1 * _ss6;
    _se3 = tmp1 * _ss7;
    _si2 = tmp2 * _sz12;
    _si3 = tmp2 * (_sz13 - _sz11);
    _sl2 = tmp3 * _sz2;
    _sl3 = tmp3 * (_sz3 - _sz1);
    _sl4 = tmp3 * (-21. - 9. * _emsq) * ZES;
    _sgh2 = tmp4 * _sz32;
    _sgh3 = tmp4 * (_sz33 - _sz31);
    _sgh4 = -18. * _ss4 * ZES;
    _sh2 = -tmp2 * _sz22;
    _sh3 = -tmp2 * (_sz23 - _sz21);

    // Termes lunaires
    const double tmpl1 = 2. * _s1;
    const double tmpl2 = 2. * _s2;
    const double tmpl3 = -2. * _s3;
    const double tmpl4 = 2. * _s4;
    _ee2 = tmpl1 * _s6;
    _e3 = tmpl1 * _s7;
    _xi2 = tmpl2 * _z12;
    _xi3 = tmpl2 * (_z13 - _z11);
    _xl2 = tmpl3 * _z2;
    _xl3 = tmpl3 * (_z3 - _z1);
    _xl4 = tmpl3 * (-21. - 9. * _emsq) * ZEL;
    _xgh2 = tmpl4 * _z32;
    _xgh3 = tmpl4 * (_z33 - _z31);
    _xgh4 = -18. * _s4 * ZEL;
    _xh2 = -tmpl2 * _z22;
    _xh3 = -tmpl2 * (_z23 - _z21);

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void SGP4::Dsinit(const double tc) {

    /* Declarations des variables locales */
    double shll, shs;

    /* Initialisations */

    /* Corps de la methode */
    _irez = 0;
    if (_nm < 0.0052359877 && _nm > 0.0034906585) {
        _irez = 1;
    }

    if (_nm >= 0.00826 && _nm <= 0.00924 && _em >= 0.5) {
        _irez = 2;
    }

    // Termes solaires
    const double ses = _ss1 * ZNS * _ss5;
    const double sis = _ss2 * ZNS * (_sz11 + _sz13);
    const double sls = -(ZNS * _ss3 * (_sz1 + _sz3 - 14. - 6. * _emsq));
    const double sghs = _ss4 * ZNS * (_sz31 + _sz33 - 6.);
    shs = -(ZNS * _ss2 * (_sz21 + _sz23));

    if (_inclm < 0.052359877 || _inclm > PI - 0.052359877) {
        shs = 0.;
    }
    if (fabs(_sinim) > EPSDBL100) {
        shs /= _sinim;
    }

    const double sgs = sghs - _cosim * shs;

    // Termes lunaires
    _dedt = ses + _s1 * ZNL * _s5;
    _didt = sis + _s2 * ZNL * (_z11 + _z13);
    _dmdt = sls - ZNL * _s3 * (_z1 + _z3 - 14. - 6. * _emsq);
    const double sghl = _s4 * ZNL * (_z31 + _z33 - 6.);
    shll = -(ZNL * _s2 * (_z21 + _z23));

    if (_inclm < 0.052359877 || _inclm > PI - 0.052359877) {
        shll = 0.;
    }

    _domdt = sgs + sghl;
    _dnodt = shs;
    if (fabs(_sinim) > EPSDBL100) {
        _domdt -= (_cosim / _sinim * shll);
        _dnodt += (shll / _sinim);
    }

    // Calcul des effets de resonance haute orbite
    _dndt = 0.;
    const double theta = fmod(_gsto + tc * RPTIM, DEUX_PI);
    _em += _dedt * _t;
    _inclm += _didt * _t;
    _argpm += _domdt * _t;
    _nodem += _dnodt * _t;
    _mm += _dmdt * _t;
    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (_inclm < 0.) {
    //     _inclm = -_inclm;
    //     _argpm = _argpm - PI;
    //     _nodem = _nodem + PI;
    // }

    // Initialisation des termes de resonance
    if (_irez != 0) {

        const double aonv = pow(_nm / KE, DEUX_TIERS);

        // Resonance geopotentielle pour les orbites de 12h
        if (_irez == 2) {
            double temp, temp1;

            const double cosisq = _cosim * _cosim;
            const double emo = _em;
            _em = _ecco;
            const double emsqo = _emsq;
            _emsq = _eccsq;
            const double eoc = _em * _emsq;
            _g201 = -0.306 - (_em - 0.64) * 0.44;

            if (_em <= 0.65) {
                _g211 = 3.616 - 13.247 * _em + 16.29 * _emsq;
                _g310 = -19.302 + 117.39 * _em - 228.419 * _emsq + 156.591 * eoc;
                _g322 = -18.9068 + 109.7927 * _em - 214.6334 * _emsq + 146.5816 * eoc;
                _g410 = -41.122 + 242.694 * _em - 471.094 * _emsq + 313.953 * eoc;
                _g422 = -146.407 + 841.88 * _em - 1629.014 * _emsq + 1083.435 * eoc;
                _g520 = -532.114 + 3017.977 * _em - 5740.032 * _emsq + 3708.276 * eoc;
            } else {
                _g211 = -72.099 + 331.819 * _em - 508.738 * _emsq + 266.724 * eoc;
                _g310 = -346.844 + 1582.851 * _em - 2415.925 * _emsq + 1246.113 * eoc;
                _g322 = -342.585 + 1554.908 * _em - 2366.899 * _emsq + 1215.972 * eoc;
                _g410 = -1052.797 + 4758.686 * _em - 7193.992 * _emsq + 3651.957 * eoc;
                _g422 = -3581.69 + 16178.11 * _em - 24462.77 * _emsq + 12422.52 * eoc;
                if (_em > 0.715) {
                    _g520 = -5149.66 + 29936.92 * _em - 54087.36 * _emsq + 31324.56 * eoc;
                } else {
                    _g520 = 1464.74 - 4664.75 * _em + 3763.64 * _emsq;
                }
            }
            if (_em < 0.7) {
                _g533 = -919.2277 + 4988.61 * _em - 9064.77 * _emsq + 5542.21 * eoc;
                _g521 = -822.71072 + 4568.6173 * _em - 8491.4146 * _emsq + 5337.524 * eoc;
                _g532 = -853.666 + 4690.25 * _em - 8624.77 * _emsq + 5341.4 * eoc;
            } else {
                _g533 = -37995.78 + 161616.52 * _em - 229838.2 * _emsq + 109377.94 * eoc;
                _g521 = -51752.104 + 218913.95 * _em - 309468.16 * _emsq + 146349.42 * eoc;
                _g532 = -40023.88 + 170470.89 * _em - 242699.48 * _emsq + 115605.82 * eoc;
            }

            const double sini2 = _sinim * _sinim;
            _f220 = 0.75 * (1. + 2. * _cosim + cosisq);
            _f221 = 1.5 * sini2;
            const double tmp1 = 1.875 * _sinim;
            const double tmp2 = 2. * _cosim;
            const double tmp3 = 3. * cosisq;
            _f321 = tmp1 * (1. - tmp2 - tmp3);
            _f322 = -tmp1 * (1. + tmp2 - tmp3);
            _f441 = 35. * sini2 * _f220;
            _f442 = 39.375 * sini2 * sini2;
            const double tmp4 = 10. * cosisq;
            _f522 = 9.84375 * _sinim * (sini2 * (1. - tmp2 - 5. * cosisq) + 0.33333333 *
                                        (-2. + 4. * _cosim + 6. * cosisq));
            _f523 = _sinim * (4.92187512 * sini2 * (-2. - 4. * _cosim + tmp4) +
                              6.56250012 * (1. + tmp2 - tmp3));
            const double tmp5 = 8. * _cosim;
            const double tmp6 = 29.53125 * _sinim;
            _f542 = tmp6 * (2. - tmp5 + cosisq * (-12. + tmp5 + tmp4));
            _f543 = tmp6 * (-2. - tmp5 + cosisq * (12. + tmp5 - tmp4));

            const double xno2 = _nm * _nm;
            const double ainv2 = aonv * aonv;
            temp1 = 3. * xno2 * ainv2;
            temp = temp1 * ROOT22;
            _d2201 = temp * _f220 * _g201;
            _d2211 = temp * _f221 * _g211;
            temp1 *= aonv;
            temp = temp1 * ROOT32;
            _d3210 = temp * _f321 * _g310;
            _d3222 = temp * _f322 * _g322;
            temp1 *= aonv;
            temp = 2. * temp1 * ROOT44;
            _d4410 = temp * _f441 * _g410;
            _d4422 = temp * _f442 * _g422;
            temp1 *= aonv;
            temp = temp1 * ROOT52;
            _d5220 = temp * _f522 * _g520;
            _d5232 = temp * _f523 * _g532;
            temp = 2. * temp1 * ROOT54;
            _d5421 = temp * _f542 * _g521;
            _d5433 = temp * _f543 * _g533;
            _xlamo = fmod(_mo + _omegao + _omegao - theta - theta, DEUX_PI);
            _xfact = _mdot + _dmdt + 2. * (_nodedot + _dnodt - RPTIM) - _no;
            _em = emo;
            _emsq = emsqo;
        }

        // Termes de resonance synchrones
        if (_irez == 1) {
            _g200 = 1. + _emsq * (-2.5 + 0.8125 * _emsq);
            _g310 = 1. + 2. * _emsq;
            _g300 = 1. + _emsq * (-6. + 6.60937 * _emsq);
            _f220 = 0.75 * (1. + _cosim) * (1. + _cosim);
            _f311 = 0.9375 * _sinim * _sinim * (1. + 3. * _cosim) - 0.75 * (1. + _cosim);
            _f330 = 1. + _cosim;
            _f330 = 1.875 * _f330 * _f330 * _f330;
            _del1 = 3. * _nm * _nm * aonv * aonv;
            _del2 = 2. * _del1 * _f220 * _g200 * Q22;
            _del3 = 3. * _del1 * _f330 * _g300 * Q33 * aonv;
            _del1 *= _f311 * _g310 * Q31 * aonv;
            _xlamo = fmod(_mo + _omegao + _argpo - theta, DEUX_PI);
            _xfact = _mdot + _xpidot - RPTIM + _dmdt + _domdt + _dnodt - _no;
        }

        // Initialisation de l'integrateur
        _xli = _xlamo;
        _xni = _no;
        _atime = 0.;
        _nm = _no + _dndt;
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void SGP4::Dspace(const double tc) {

    /* Declarations des variables locales */
    double xldot, xnddt, xndt;

    /* Initialisations */
    xldot = 0.;
    xnddt = 0.;
    xndt = 0.;

    /* Corps de la methode */
    // Calcul des effets de resonance haute orbite
    _dndt = 0.;
    const double theta = fmod(_gsto + tc * RPTIM, DEUX_PI);
    _em += _dedt * _t;
    _inclm += _didt * _t;
    _argpm += _domdt * _t;
    _nodem += _dnodt * _t;
    _mm += _dmdt * _t;

    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (_inclm < 0.) {
    //     _inclm = -_inclm;
    //     _argpm = _argpm - PI;
    //     _nodem = _nodem + PI;
    // }

    // Integration numerique (Euler-MacLaurin)
    if (_irez != 0) {

        double ft = 0.;
        if (fabs(_atime) <= EPSDBL100 || _t * _atime <= 0. || fabs(_t) < fabs(_atime)) {
            _atime = 0.;
            _xni = _no;
            _xli = _xlamo;
        }

        _delt = (_t > 0) ? STEPP : STEPN;

        int iretn = 381;
        while (iretn == 381) {
            // Calculs des termes derives

            // Termes de resonance quasi-synchrones
            if (_irez != 2) {
                xndt = _del1 * sin(_xli - FASX2) + _del2 * sin(2. * (_xli - FASX4)) +
                        _del3 * sin(3. * (_xli - FASX6));
                xldot = _xni + _xfact;
                xnddt = _del1 * cos(_xli - FASX2) + 2. * _del2 * cos(2. * (_xli - FASX4)) + 3. * _del3 * cos(3. * (_xli - FASX6));
                xnddt *= xldot;
            } else {

                // Termes de resonance d'environ 12h
                const double xomi = _argpo + _argpdot * _atime;
                const double x2omi = xomi + xomi;
                const double x2li = _xli + _xli;
                xndt = _d2201 * sin(x2omi + _xli - G22) + _d2211 * sin(_xli - G22) +
                        _d3210 * sin(xomi + _xli - G32) + _d3222 * sin(-xomi + _xli - G32) +
                        _d4410 * sin(x2omi + x2li - G44) + _d4422 * sin(x2li - G44) + _d5220 *
                        sin(xomi + _xli - G52) + _d5232 * sin(-xomi + _xli - G52) + _d5421 *
                        sin(xomi + x2li - G54) + _d5433 * sin(-xomi + x2li - G54);
                xldot = _xni + _xfact;
                xnddt = _d2201 * cos(x2omi + _xli - G22) + _d2211 * cos(_xli - G22) +
                        _d3210 * cos(xomi + _xli - G32) + _d3222 * cos(-xomi + _xli - G32) +
                        _d5220 * cos(xomi + _xli - G52) + _d5232 * cos(-xomi + _xli - G52) +
                        2.0 * (_d4410 * cos(x2omi + x2li - G44) + _d4422 * cos(x2li - G44) +
                               _d5421 * cos(xomi + x2li - G54) + _d5433 * cos(-xomi + x2li - G54));
                xnddt *= xldot;
            }

            // Integrateur
            if (fabs(_t - _atime) >= STEPP) {
                iretn = 381;
            } else {
                ft = _t - _atime;
                iretn = 0;
            }

            if (iretn == 381) {
                _xli += xldot * _delt + xndt * STEP2;
                _xni += xndt * _delt + xnddt * STEP2;
                _atime += _delt;
            }
        }

        const double ft2 = ft * ft * 0.5;
        _nm = _xni + xndt * ft + xnddt * ft2;
        const double xl = _xli + xldot * ft + xndt * ft2;
        if (_irez == 1) {
            _mm = xl - _nodem - _argpm + theta;
            _dndt = _nm - _no;
        } else {
            _mm = xl - 2. * _nodem + 2. * theta;
            _dndt = _nm - _no;
        }
        _nm = _no + _dndt;
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
    _t = 0.;

    _eccsq = _ecco * _ecco;
    _omeosq = 1. - _eccsq;
    _rteosq = sqrt(_omeosq);
    _cosio = cos(_inclo);
    _cosio2 = _cosio * _cosio;

    const double ak = pow(KE / _no, DEUX_TIERS);
    const double d1 = 0.75 * J2 * (3. * _cosio2 - 1.) / (_rteosq * _omeosq);
    double del = d1 / (ak * ak);
    const double del2 = del * del;
    const double adel = ak * (1. - del2 - del * (0.5 * DEUX_TIERS + del2 * (134. / 81.)));
    del = d1 / (adel * adel);
    _no /= (1. + del);

    _ao = pow(KE / _no, DEUX_TIERS);
    _sinio = sin(_inclo);
    const double po = _ao * _omeosq;
    _con42 = 1. - 5. * _cosio2;
    _con41 = -_con42 - _cosio2 - _cosio2;
    _posq = po * po;
    _rp = _ao * (1. - _ecco);
    _method = 'n';

    _gsto = Observateur::CalculTempsSideralGreenwich(_epoque);

    if (_omeosq >= 0. || _no > 0.) {
        double cc3, qzms24, sfour;

        _isimp = (_rp < 220. * X1SRT + 1.);

        sfour = ss;
        qzms24 = qzms2t;

        const double perige = (_rp - 1.) * RAYON_TERRESTRE;
        if (perige < 156.) {
            sfour = perige - 78.;
            if (perige < 98.) {
                sfour = 20.;
            }

            qzms24 = pow((120. - sfour) * X1SRT, 4.);
            sfour /= RAYON_TERRESTRE + 1.;
        }

        const double pinvsq = 1. / _posq;
        const double tsi = 1. / (_ao - sfour);
        _eta = _ao * _ecco * tsi;

        const double etasq = _eta * _eta;
        const double eeta = _ecco * _eta;
        const double psisq = fabs(1. - etasq);
        const double coef = qzms24 * pow(tsi, 4.);
        const double coef1 = coef * pow(psisq, -3.5);
        const double cc2 = coef1 * _no * (_ao * (1. + 1.5 * etasq + eeta * (4. + etasq)) + 0.375 * J2 * tsi / psisq *
                                          _con41 * (8. + 3. * etasq * (8. + etasq)));
        _cc1 = _bstar * cc2;
        cc3 = 0.;
        if (_ecco > 1.e-4) {
            cc3 = -2. * coef * tsi * J3SJ2 * _no * _sinio / _ecco;
        }

        _x1mth2 = 1. - _cosio2;
        _cc4 = 2. * _no * coef1 * _ao * _omeosq *
                (_eta * (2. + 0.5 * etasq) + _ecco * (0.5 + 2. * etasq) - J2 * tsi /
                 (_ao * psisq) * (-3. * _con41 * (1. - 2. * eeta + etasq * (1.5 - 0.5 * eeta)) +
                                  0.75 * _x1mth2 * (2. * etasq - eeta * (1. + etasq)) * cos(2. * _argpo)));
        _cc5 = 2. * coef1 * _ao * _omeosq * (1. + 2.75 * (etasq + eeta) + eeta * etasq);

        const double cosio4 = _cosio2 * _cosio2;
        const double temp1 = 1.5 * J2 * pinvsq * _no;
        const double temp2 = 0.5 * temp1 * J2 * pinvsq;
        const double temp3 = -0.46875 * J4 * pinvsq * pinvsq * _no;
        _mdot = _no + 0.5 * temp1 * _rteosq * _con41 + 0.0625 * temp2 * _rteosq *
                (13. - 78. * _cosio2 + 137. * cosio4);
        _argpdot = -0.5 * temp1 * _con42 + 0.0625 * temp2 * (7. - 114. * _cosio2 + 395. * cosio4) +
                temp3 * (3. - 36. * _cosio2 + 49. * cosio4);
        const double xhdot1 = -temp1 * _cosio;
        _nodedot = xhdot1 + (0.5 * temp2 * (4. - 19. * _cosio2) + 2. * temp3 * (3. - 7. * _cosio2)) * _cosio;
        _xpidot = _argpdot + _nodedot;
        _omgcof = _bstar * cc3 * cos(_argpo);
        _xmcof = 0.;
        if (_ecco > 1.e-4) {
            _xmcof = -DEUX_TIERS * coef * _bstar / eeta;
        }

        _nodecf = 3.5 * _omeosq * xhdot1 * _cc1;
        _t2cof = 1.5 * _cc1;

        if (fabs(_cosio + 1.) > 1.5e-12) {
            _xlcof = -0.25 * J3SJ2 * _sinio * (3. + 5. * _cosio) / (1. + _cosio);
        } else {
            _xlcof = -0.25 * J3SJ2 * _sinio * (3. + 5. * _cosio) * (1. / 1.5e-12);
        }

        _aycof = -0.5 * J3SJ2 * _sinio;
        _delmo = pow((1. + _eta * cos(_mo)), 3.);
        _sinmao = sin(_mo);
        _x7thm1 = 7. * _cosio2 - 1.;

        // Initialisation du modele haute orbite
        if (DEUX_PI >= 225. * _no) {

            _method = 'd';
            _isimp = true;
            _inclm = _inclo;

            const double tc = 0.;
            Dscom(tc);

            _mp = _mo;
            _argpp = _argpo;
            _ep = _ecco;
            _nodep = _omegao;
            _xincp = _inclo;

            Dpper();

            _argpm = 0.;
            _nodem = 0.;
            _mm = 0.;

            Dsinit(tc);
        }

        if (!_isimp) {

            const double cc1sq = _cc1 * _cc1;
            _d2 = 4. * _ao * tsi * cc1sq;
            const double temp = _d2 * tsi * _cc1 * (1. / 3.);
            _d3 = (17. * _ao + sfour) * temp;
            _d4 = 0.5 * temp * _ao * tsi * (221. * _ao + 31. * sfour) * _cc1;
            _t3cof = _d2 + 2. * cc1sq;
            _t4cof = 0.25 * (3. * _d3 + _cc1 * (12. * _d2 + 10. * cc1sq));
            _t5cof = 0.2 * (3. * _d4 + 12. * _cc1 * _d3 + 6. * _d2 * _d2 + 15. * cc1sq * (2. * _d2 + cc1sq));
        }

        _init = true;
    }

    /* Retour */
    return;
}


/* Accesseurs */
char SGP4::method() const
{
    return (_method);
}

Vecteur3D SGP4::position() const
{
    return (_position);
}

Vecteur3D SGP4::vitesse() const
{
    return (_vitesse);
}

void SGP4::setInit(const bool init)
{
    _init = init;
}
