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
 * >    sgp4.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 octobre 2015
 *
 * Date de revision
 * >    25 fevrier 2023
 *
 */

#include "librairies/exceptions/previsatexception.h"
#include "librairies/observateur/observateur.h"
#include "sgp4.h"


/**********
 * PUBLIC *
 **********/



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
    _init = false;
    _elements.epoque = Date();

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la position et de la vitesse
 */
void SGP4::Calcul(const Date &date, const ElementsOrbitaux &elements)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_init) {
        SGP4Init(elements);
    }

    /* Corps de la methode */
    try {

        double coseo1 = 0.;
        double sineo1 = 0.;

        // Calcul du temps ecoule depuis l'epoque (en minutes)
        const double tsince = DATE::NB_MIN_PAR_JOUR * (date.jourJulienUTC() - elements.epoque.jourJulienUTC());

        _data.t = tsince;

        // Prise en compte des termes seculaires de la gravite et du freinage atmospherique
        const double xmdf = _elements.mo + _data.mdot * _data.t;
        const double argpdf = _elements.argpo + _data.argpdot * _data.t;
        const double nodedf = _elements.omegao + _data.nodedot * _data.t;
        _data.argpm = argpdf;
        _data.mm = xmdf;
        const double tt2 = _data.t * _data.t;
        _data.nodem = nodedf + _data.nodecf * tt2;
        double tempa = 1. - _data.cc1 * _data.t;
        double tempe = _elements.bstar * _data.cc4 * _data.t;
        double templ = _data.t2cof * tt2;

        if (!_data.isimp) {

            const double delomg = _data.omgcof * _data.t;
            const double delmtmp = 1. + _data.eta * cos(xmdf);
            const double delm = _data.xmcof * (delmtmp * delmtmp * delmtmp - _data.delmo);
            double temp = delomg + delm;
            _data.mm = xmdf + temp;
            _data.argpm = argpdf - temp;
            const double tt3 = tt2 * _data.t;
            const double t4 = tt3 * _data.t;
            tempa = tempa - _data.d2 * tt2 - _data.d3 * tt3 - _data.d4 * t4;
            tempe = tempe + _elements.bstar * _data.cc5 * (sin(_data.mm) - _data.sinmao);
            templ = templ + _data.t3cof * tt3 + t4 * (_data.t4cof + _data.t * _data.t5cof);
        }

        _data.nm = _elements.no;
        _data.em = _elements.ecco;
        _data.inclm = _elements.inclo;

        if (_data.method == 'd') {
            const double tc = _data.t;
            Dspace(tc);
        }

        const double am = pow((TERRE::KE / _data.nm), MATHS::DEUX_TIERS) * tempa * tempa;
        _data.nm = TERRE::KE * pow(am, -1.5);
        _data.em -= tempe;

        if (_data.em < 1.e-6) {
            _data.em = 1.e-6;
        }

        _data.mm = _data.mm + _elements.no * templ;
        double xlm = _data.mm + _data.argpm + _data.nodem;
        _data.emsq = _data.em * _data.em;
        double temp = 1. - _data.emsq;

        _data.nodem = fmod(_data.nodem, MATHS::DEUX_PI);
        _data.argpm = fmod(_data.argpm, MATHS::DEUX_PI);
        xlm = fmod(xlm, MATHS::DEUX_PI);
        _data.mm = fmod(xlm - _data.argpm - _data.nodem, MATHS::DEUX_PI);

        _data.sinim = sin(_data.inclm);
        _data.cosim = cos(_data.inclm);

        // Prise en compte des termes periodiques luni-solaires
        _data.ep = _data.em;
        _data.xincp = _data.inclm;
        _data.argpp = _data.argpm;
        _data.nodep = _data.nodem;
        _data.mp = _data.mm;
        double sinip = _data.sinim;
        double cosip = _data.cosim;

        // Termes longue periode
        if (_data.method == 'd') {
            Dpper();
            if (_data.xincp < 0.) {
                _data.xincp = -_data.xincp;
                _data.nodep += MATHS::PI;
                _data.argpp -= MATHS::PI;
            }

            sinip = sin(_data.xincp);
            cosip = cos(_data.xincp);
            _data.aycof = -0.5 * SGP::J3SJ2 * sinip;

            if (fabs(cosip + 1.) > 1.5e-12) {
                _data.xlcof = -0.25 * SGP::J3SJ2 * sinip * (3. + 5. * cosip) / (1. + cosip);
            } else {
                _data.xlcof = -0.25 * SGP::J3SJ2 * sinip * (3. + 5. * cosip) * (1. / 1.5e-12);
            }
        }

        const double axnl = _data.ep * cos(_data.argpp);
        temp = 1. / (am * (1. - _data.ep * _data.ep));
        const double aynl = _data.ep * sin(_data.argpp) + temp * _data.aycof;
        const double xl = _data.mp + _data.argpp + _data.nodep + temp * _data.xlcof * axnl;

        // Resolution de l'equation de Kepler
        const double u = fmod(xl - _data.nodep, MATHS::DEUX_PI);
        double eo1 = u;
        double tem5 = 9999.9;
        int ktr = 1;

        while ((fabs(tem5) >= MATHS::EPSDBL) && (ktr <= 10)) {
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
            const double temp1 = 0.5 * TERRE::J2 * temp;
            const double temp2 = temp1 * temp;

            // Prise en compte des termes courte periode
            if (_data.method == 'd') {

                const double cosisq = cosip * cosip;
                _data.con41 = 3. * cosisq - 1.;
                _data.x1mth2 = 1. - cosisq;
                _data.x7thm1 = 7. * cosisq - 1.;
            }

            const double mrt = TERRE::RAYON_TERRESTRE * (rl * (1. - 1.5 * temp2 * betal * _data.con41) + 0.5 * temp1 * _data.x1mth2 * cos2u);
            su = su - 0.25 * temp2 * _data.x7thm1 * sin2u;
            const double temp3 = 1.5 * temp2 * cosip;
            const double xnode = _data.nodep + temp3 * sin2u;
            const double xinc = _data.xincp + temp3 * sinip * cos2u;
            const double mvt = rdotl - _data.nm * temp1 * _data.x1mth2 * sin2u / TERRE::KE;
            const double rvdot = rvdotl + _data.nm * temp1 * (_data.x1mth2 * cos2u + 1.5 * _data.con41) / TERRE::KE;

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
            _vitesse = (uu * mvt + vv * rvdot) * SGP::RTMS;
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
char SGP4::method() const
{
    return _data.method;
}

const Vecteur3D &SGP4::position() const
{
    return _position;
}

const Vecteur3D &SGP4::vitesse() const
{
    return _vitesse;
}


/*
 * Modificateurs
 */
void SGP4::setInit(const bool init)
{
    _init = init;
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
    double zm = (_init) ? _data.zmos + SGP::ZNS * _data.t : _data.zmos;

    double zf = zm + 2. * SGP::ZES * sin(zm);
    double sinzf = sin(zf);
    double f2 = 0.5 * sinzf * sinzf - 0.25;
    double f3 = -0.5 * sinzf * cos(zf);
    const double ses = _data.se2 * f2 + _data.se3 * f3;
    const double sis = _data.si2 * f2 + _data.si3 * f3;
    const double sls = _data.sl2 * f2 + _data.sl3 * f3 + _data.sl4 * sinzf;
    const double sghs = _data.sgh2 * f2 + _data.sgh3 * f3 + _data.sgh4 * sinzf;
    const double shs = _data.sh2 * f2 + _data.sh3 * f3;

    zm = (_init) ? _data.zmol + SGP::ZNL * _data.t : _data.zmol;

    zf = zm + 2. * SGP::ZEL * sin(zm);
    sinzf = sin(zf);
    f2 = 0.5 * sinzf * sinzf - 0.25;
    f3 = -0.5 * sinzf * cos(zf);

    const double sel = _data.ee2 * f2 + _data.e3 * f3;
    const double sil = _data.xi2 * f2 + _data.xi3 * f3;
    const double sll = _data.xl2 * f2 + _data.xl3 * f3 + _data.xl4 * sinzf;
    const double sghl = _data.xgh2 * f2 + _data.xgh3 * f3 + _data.xgh4 * sinzf;
    const double shll = _data.xh2 * f2 + _data.xh3 * f3;
    const double pe = ses + sel;
    const double pinc = sis + sil;
    const double pl = sls + sll;
    double pgh = sghs + sghl;
    double ph = shs + shll;

    if (_init) {

        _data.xincp += pinc;
        _data.ep += pe;
        const double sinip = sin(_data.xincp);
        const double cosip = cos(_data.xincp);

        // Application directe des termes periodiques
        if (_data.xincp >= 0.2) {
            ph /= sinip;
            pgh = pgh - cosip * ph;
            _data.argpp += pgh;
            _data.nodep += ph;
            _data.mp += pl;
        } else {

            // Application des termes periodiques avec la modification de Lyddane
            const double sinop = sin(_data.nodep);
            const double cosop = cos(_data.nodep);
            double alfdp = sinip * sinop;
            double betdp = sinip * cosop;
            const double tmp = pinc * cosip;
            const double dalf = ph * cosop + tmp * sinop;
            const double dbet = -ph * sinop + tmp * cosop;
            alfdp += dalf;
            betdp += dbet;
            _data.nodep = fmod(_data.nodep, MATHS::DEUX_PI);
            if (_data.nodep < 0.) {
                _data.nodep += MATHS::DEUX_PI;
            }

            double xls = _data.mp + _data.argpp + cosip * _data.nodep;
            const double dls = pl + pgh - pinc * _data.nodep * sinip;
            xls += dls;
            const double xnoh = _data.nodep;
            _data.nodep = atan2(alfdp, betdp);
            if (_data.nodep < 0.) {
                _data.nodep += MATHS::DEUX_PI;
            }

            if (fabs(xnoh - _data.nodep) > MATHS::PI) {
                _data.nodep = (_data.nodep < xnoh) ? _data.nodep + MATHS::DEUX_PI : _data.nodep - MATHS::DEUX_PI;
            }

            _data.mp += pl;
            _data.argpp = xls - _data.mp - cosip * _data.nodep;
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
    _data.nm = _elements.no;
    _data.em = _elements.ecco;
    _data.snodm = sin(_elements.omegao);
    _data.cnodm = cos(_elements.omegao);
    _data.sinomm = sin(_elements.argpo);
    _data.cosomm = cos(_elements.argpo);
    _data.sinim = sin(_elements.inclo);
    _data.cosim = cos(_elements.inclo);
    _data.emsq = _data.em * _data.em;
    const double betasq = 1. - _data.emsq;
    _data.rtemsq = sqrt(betasq);

    // Initialisation des termes luni-solaires
    _data.day = _elements.epoque.jourJulienUTC() + DATE::NB_JOURS_PAR_SIECJ + tc * DATE::NB_JOUR_PAR_MIN;
    const double xnodce = fmod(4.5236020 - 0.00092422029 * _data.day, MATHS::DEUX_PI);
    const double stem = sin(xnodce);
    const double ctem = cos(xnodce);
    const double zcosil = 0.91375164 - 0.03568096 * ctem;
    const double zsinil = sqrt(1. - zcosil * zcosil);
    const double zsinhl = 0.089683511 * stem / zsinil;
    const double zcoshl = sqrt(1. - zsinhl * zsinhl);
    _data.gam = 5.8351514 + 0.0019443680 * _data.day;
    double zx = 0.39785416 * stem / zsinil;
    const double zy = zcoshl * ctem + 0.91744867 * zsinhl * stem;
    zx = atan2(zx, zy);
    zx = _data.gam + zx - xnodce;
    const double zcosgl = cos(zx);
    const double zsingl = sin(zx);

    // Termes solaires
    double zcosg = SGP::ZCOSGS;
    double zsing = SGP::ZSINGS;
    double zcosi = SGP::ZCOSIS;
    double zsini = SGP::ZSINIS;
    double zcosh = _data.cnodm;
    double zsinh = _data.snodm;
    double cc = SGP::C1SS;
    double xnoi = 1. / _data.nm;

    for (int lsflg = 1; lsflg <= 2; lsflg++) {

        const double a0 = zcosi * zsinh;
        const double a1 = zcosg * zcosh + zsing * a0;
        const double a3 = -zsing * zcosh + zcosg * a0;
        const double a7 = -zcosg * zsinh + zsing * zcosi * zcosh;
        const double a8 = zsing * zsini;
        const double a9 = zsing * zsinh + zcosg * zcosi * zcosh;
        const double a10 = zcosg * zsini;
        const double a2 = _data.cosim * a7 + _data.sinim * a8;
        const double a4 = _data.cosim * a9 + _data.sinim * a10;
        const double a5 = -_data.sinim * a7 + _data.cosim * a8;
        const double a6 = -_data.sinim * a9 + _data.cosim * a10;

        const double x1 = a1 * _data.cosomm + a2 * _data.sinomm;
        const double x2 = a3 * _data.cosomm + a4 * _data.sinomm;
        const double x3 = -a1 * _data.sinomm + a2 * _data.cosomm;
        const double x4 = -a3 * _data.sinomm + a4 * _data.cosomm;
        const double x5 = a5 * _data.sinomm;
        const double x6 = a6 * _data.sinomm;
        const double x7 = a5 * _data.cosomm;
        const double x8 = a6 * _data.cosomm;

        _data.z31 = 12. * x1 * x1 - 3. * x3 * x3;
        _data.z32 = 24. * x1 * x2 - 6. * x3 * x4;
        _data.z33 = 12. * x2 * x2 - 3. * x4 * x4;
        _data.z1 = 3. * (a1 * a1 + a2 * a2) + _data.z31 * _data.emsq;
        _data.z2 = 6. * (a1 * a3 + a2 * a4) + _data.z32 * _data.emsq;
        _data.z3 = 3. * (a3 * a3 + a4 * a4) + _data.z33 * _data.emsq;
        _data.z11 = -6. * a1 * a5 + _data.emsq * (-24. * x1 * x7 - 6. * x3 * x5);
        _data.z12 = -6. * (a1 * a6 + a3 * a5) + _data.emsq * (-24. * (x2 * x7 + x1 * x8) - 6. * (x3 * x6 + x4 * x5));
        _data.z13 = -6. * a3 * a6 + _data.emsq * (-24. * x2 * x8 - 6. * x4 * x6);
        _data.z21 = 6. * a2 * a5 + _data.emsq * (24. * x1 * x5 - 6. * x3 * x7);
        _data.z22 = 6. * (a4 * a5 + a2 * a6) + _data.emsq * (24. * (x2 * x5 + x1 * x6) - 6. * (x4 * x7 + x3 * x8));
        _data.z23 = 6. * a4 * a6 + _data.emsq * (24. * x2 * x6 - 6. * x4 * x8);
        _data.z1 = _data.z1 + _data.z1 + betasq * _data.z31;
        _data.z2 = _data.z2 + _data.z2 + betasq * _data.z32;
        _data.z3 = _data.z3 + _data.z3 + betasq * _data.z33;
        _data.s3 = cc * xnoi;
        _data.s2 = -0.5 * _data.s3 / _data.rtemsq;
        _data.s4 = _data.s3 * _data.rtemsq;
        _data.s1 = -15. * _data.em * _data.s4;
        _data.s5 = x1 * x3 + x2 * x4;
        _data.s6 = x2 * x3 + x1 * x4;
        _data.s7 = x2 * x4 - x1 * x3;

        // Termes lunaires
        if (lsflg == 1) {
            _data.ss1 = _data.s1;
            _data.ss2 = _data.s2;
            _data.ss3 = _data.s3;
            _data.ss4 = _data.s4;
            _data.ss5 = _data.s5;
            _data.ss6 = _data.s6;
            _data.ss7 = _data.s7;
            _data.sz1 = _data.z1;
            _data.sz2 = _data.z2;
            _data.sz3 = _data.z3;
            _data.sz11 = _data.z11;
            _data.sz12 = _data.z12;
            _data.sz13 = _data.z13;
            _data.sz21 = _data.z21;
            _data.sz22 = _data.z22;
            _data.sz23 = _data.z23;
            _data.sz31 = _data.z31;
            _data.sz32 = _data.z32;
            _data.sz33 = _data.z33;
            zcosg = zcosgl;
            zsing = zsingl;
            zcosi = zcosil;
            zsini = zsinil;
            zcosh = zcoshl * _data.cnodm + zsinhl * _data.snodm;
            zsinh = _data.snodm * zcoshl - _data.cnodm * zsinhl;
            cc = SGP::C1L;
        }
    }

    _data.zmol = fmod(4.7199672 + 0.22997150 * _data.day - _data.gam, MATHS::DEUX_PI);
    _data.zmos = fmod(6.2565837 + 0.017201977 * _data.day, MATHS::DEUX_PI);

    // Termes solaires
    const double tmp1 = 2. * _data.ss1;
    const double tmp2 = 2. * _data.ss2;
    const double tmp3 = -2. * _data.ss3;
    const double tmp4 = 2. * _data.ss4;
    _data.se2 = tmp1 * _data.ss6;
    _data.se3 = tmp1 * _data.ss7;
    _data.si2 = tmp2 * _data.sz12;
    _data.si3 = tmp2 * (_data.sz13 - _data.sz11);
    _data.sl2 = tmp3 * _data.sz2;
    _data.sl3 = tmp3 * (_data.sz3 - _data.sz1);
    _data.sl4 = tmp3 * (-21. - 9. * _data.emsq) * SGP::ZES;
    _data.sgh2 = tmp4 * _data.sz32;
    _data.sgh3 = tmp4 * (_data.sz33 - _data.sz31);
    _data.sgh4 = -18. * _data.ss4 * SGP::ZES;
    _data.sh2 = -tmp2 * _data.sz22;
    _data.sh3 = -tmp2 * (_data.sz23 - _data.sz21);

    // Termes lunaires
    const double tmpl1 = 2. * _data.s1;
    const double tmpl2 = 2. * _data.s2;
    const double tmpl3 = -2. * _data.s3;
    const double tmpl4 = 2. * _data.s4;
    _data.ee2 = tmpl1 * _data.s6;
    _data.e3 = tmpl1 * _data.s7;
    _data.xi2 = tmpl2 * _data.z12;
    _data.xi3 = tmpl2 * (_data.z13 - _data.z11);
    _data.xl2 = tmpl3 * _data.z2;
    _data.xl3 = tmpl3 * (_data.z3 - _data.z1);
    _data.xl4 = tmpl3 * (-21. - 9. * _data.emsq) * SGP::ZEL;
    _data.xgh2 = tmpl4 * _data.z32;
    _data.xgh3 = tmpl4 * (_data.z33 - _data.z31);
    _data.xgh4 = -18. * _data.s4 * SGP::ZEL;
    _data.xh2 = -tmpl2 * _data.z22;
    _data.xh3 = -tmpl2 * (_data.z23 - _data.z21);

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
    _data.irez = 0;
    if ((_data.nm < 0.0052359877) && (_data.nm > 0.0034906585)) {
        _data.irez = 1;
    }

    if ((_data.nm >= 0.00826) && (_data.nm <= 0.00924) && (_data.em >= 0.5)) {
        _data.irez = 2;
    }

    // Termes solaires
    const double ses = _data.ss1 * SGP::ZNS * _data.ss5;
    const double sis = _data.ss2 * SGP::ZNS * (_data.sz11 + _data.sz13);
    const double sls = -(SGP::ZNS * _data.ss3 * (_data.sz1 + _data.sz3 - 14. - 6. * _data.emsq));
    const double sghs = _data.ss4 * SGP::ZNS * (_data.sz31 + _data.sz33 - 6.);
    double shs = -(SGP::ZNS * _data.ss2 * (_data.sz21 + _data.sz23));

    if ((_data.inclm < 0.052359877) || (_data.inclm > MATHS::PI - 0.052359877)) {
        shs = 0.;
    }
    if (fabs(_data.sinim) > MATHS::EPSDBL100) {
        shs /= _data.sinim;
    }

    const double sgs = sghs - _data.cosim * shs;

    // Termes lunaires
    _data.dedt = ses + _data.s1 * SGP::ZNL * _data.s5;
    _data.didt = sis + _data.s2 * SGP::ZNL * (_data.z11 + _data.z13);
    _data.dmdt = sls - SGP::ZNL * _data.s3 * (_data.z1 + _data.z3 - 14. - 6. * _data.emsq);
    const double sghl = _data.s4 * SGP::ZNL * (_data.z31 + _data.z33 - 6.);
    double shll = -(SGP::ZNL * _data.s2 * (_data.z21 + _data.z23));

    if ((_data.inclm < 0.052359877) || (_data.inclm > MATHS::PI - 0.052359877)) {
        shll = 0.;
    }

    _data.domdt = sgs + sghl;
    _data.dnodt = shs;
    if (fabs(_data.sinim) > MATHS::EPSDBL100) {
        _data.domdt = _data.domdt - (_data.cosim / _data.sinim * shll);
        _data.dnodt = _data.dnodt + (shll / _data.sinim);
    }

    // Calcul des effets de resonance haute orbite
    _data.dndt = 0.;
    const double theta = fmod(_data.gsto + tc * SGP::RPTIM, MATHS::DEUX_PI);
    _data.em = _data.em + _data.dedt * _data.t;
    _data.inclm = _data.inclm + _data.didt * _data.t;
    _data.argpm = _data.argpm + _data.domdt * _data.t;
    _data.nodem = _data.nodem + _data.dnodt * _data.t;
    _data.mm = _data.mm + _data.dmdt * _data.t;
    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (_data.inclm < 0.) {
    //     _data.inclm = -_data.inclm;
    //     _data.argpm = _data.argpm - MATHS::PI;
    //     _data.nodem = _data.nodem + MATHS::PI;
    // }

    // Initialisation des termes de resonance
    if (_data.irez != 0) {

        const double aonv = pow(_data.nm / TERRE::KE, MATHS::DEUX_TIERS);

        // Resonance geopotentielle pour les orbites de 12h
        if (_data.irez == 2) {

            const double cosisq = _data.cosim * _data.cosim;
            const double emo = _data.em;
            _data.em = _elements.ecco;
            const double emsqo = _data.emsq;
            _data.emsq = _data.eccsq;
            const double eoc = _data.em * _data.emsq;
            _data.g201 = -0.306 - (_data.em - 0.64) * 0.440;

            if (_data.em <= 0.65) {
                _data.g211 = 3.616 - 13.2470 * _data.em + 16.2900 * _data.emsq;
                _data.g310 = -19.302 + 117.3900 * _data.em - 228.4190 * _data.emsq + 156.5910 * eoc;
                _data.g322 = -18.9068 + 109.7927 * _data.em - 214.6334 * _data.emsq + 146.5816 * eoc;
                _data.g410 = -41.122 + 242.6940 * _data.em - 471.0940 * _data.emsq + 313.9530 * eoc;
                _data.g422 = -146.407 + 841.880 * _data.em - 1629.014 * _data.emsq + 1083.4350 * eoc;
                _data.g520 = -532.114 + 3017.977 * _data.em - 5740.032 * _data.emsq + 3708.2760 * eoc;
            } else {
                _data.g211 = -72.099 + 331.819 * _data.em - 508.738 * _data.emsq + 266.724 * eoc;
                _data.g310 = -346.844 + 1582.851 * _data.em - 2415.925 * _data.emsq + 1246.113 * eoc;
                _data.g322 = -342.585 + 1554.908 * _data.em - 2366.899 * _data.emsq + 1215.972 * eoc;
                _data.g410 = -1052.797 + 4758.686 * _data.em - 7193.992 * _data.emsq + 3651.957 * eoc;
                _data.g422 = -3581.690 + 16178.110 * _data.em - 24462.77 * _data.emsq + 12422.520 * eoc;
                if (_data.em > 0.715) {
                    _data.g520 = -5149.66 + 29936.92 * _data.em - 54087.36 * _data.emsq + 31324.56 * eoc;
                } else {
                    _data.g520 = 1464.74 - 4664.75 * _data.em + 3763.64 * _data.emsq;
                }
            }
            if (_data.em < 0.7) {
                _data.g533 = -919.22770 + 4988.6100 * _data.em - 9064.7700 * _data.emsq + 5542.21 * eoc;
                _data.g521 = -822.71072 + 4568.6173 * _data.em - 8491.4146 * _data.emsq + 5337.524 * eoc;
                _data.g532 = -853.66600 + 4690.2500 * _data.em - 8624.7700 * _data.emsq + 5341.4 * eoc;
            } else {
                _data.g533 = -37995.780 + 161616.52 * _data.em - 229838.20 * _data.emsq + 109377.94 * eoc;
                _data.g521 = -51752.104 + 218913.95 * _data.em - 309468.16 * _data.emsq + 146349.42 * eoc;
                _data.g532 = -40023.880 + 170470.89 * _data.em - 242699.48 * _data.emsq + 115605.82 * eoc;
            }

            const double sini2 = _data.sinim * _data.sinim;
            _data.f220 = 0.75 * (1. + 2. * _data.cosim + cosisq);
            _data.f221 = 1.5 * sini2;
            const double tmp1 = 1.875 * _data.sinim;
            const double tmp2 = 2. * _data.cosim;
            const double tmp3 = 3. * cosisq;
            _data.f321 = tmp1 * (1. - tmp2 - tmp3);
            _data.f322 = -tmp1 * (1. + tmp2 - tmp3);
            _data.f441 = 35. * sini2 * _data.f220;
            _data.f442 = 39.3750 * sini2 * sini2;
            const double tmp4 = 10. * cosisq;
            _data.f522 = 9.84375 * _data.sinim * (sini2 * (1. - tmp2 - 5. * cosisq) + 0.33333333 *
                                        (-2. + 4. * _data.cosim + 6. * cosisq));
            _data.f523 = _data.sinim * (4.92187512 * sini2 * (-2. - 4. * _data.cosim + tmp4) +
                              6.56250012 * (1. + tmp2 - tmp3));
            const double tmp5 = 8. * _data.cosim;
            const double tmp6 = 29.53125 * _data.sinim;
            _data.f542 = tmp6 * (2. - tmp5 + cosisq * (-12. + tmp5 + tmp4));
            _data.f543 = tmp6 * (-2. - tmp5 + cosisq * (12. + tmp5 - tmp4));

            const double xno2 = _data.nm * _data.nm;
            const double ainv2 = aonv * aonv;
            double temp1 = 3. * xno2 * ainv2;
            double temp = temp1 * SGP::ROOT22;
            _data.d2201 = temp * _data.f220 * _data.g201;
            _data.d2211 = temp * _data.f221 * _data.g211;
            temp1 *= aonv;
            temp = temp1 * SGP::ROOT32;
            _data.d3210 = temp * _data.f321 * _data.g310;
            _data.d3222 = temp * _data.f322 * _data.g322;
            temp1 *= aonv;
            temp = 2. * temp1 * SGP::ROOT44;
            _data.d4410 = temp * _data.f441 * _data.g410;
            _data.d4422 = temp * _data.f442 * _data.g422;
            temp1 *= aonv;
            temp = temp1 * SGP::ROOT52;
            _data.d5220 = temp * _data.f522 * _data.g520;
            _data.d5232 = temp * _data.f523 * _data.g532;
            temp = 2. * temp1 * SGP::ROOT54;
            _data.d5421 = temp * _data.f542 * _data.g521;
            _data.d5433 = temp * _data.f543 * _data.g533;
            _data.xlamo = fmod(_elements.mo + _elements.omegao + _elements.omegao - theta - theta, MATHS::DEUX_PI);
            _data.xfact = _data.mdot + _data.dmdt + 2. * (_data.nodedot + _data.dnodt - SGP::RPTIM) - _elements.no;
            _data.em = emo;
            _data.emsq = emsqo;
        }

        // Termes de resonance synchrones
        if (_data.irez == 1) {
            _data.g200 = 1. + _data.emsq * (-2.5 + 0.8125 * _data.emsq);
            _data.g310 = 1. + 2. * _data.emsq;
            _data.g300 = 1. + _data.emsq * (-6. + 6.60937 * _data.emsq);
            _data.f220 = 0.75 * (1. + _data.cosim) * (1. + _data.cosim);
            _data.f311 = 0.9375 * _data.sinim * _data.sinim * (1. + 3. * _data.cosim) - 0.75 * (1. + _data.cosim);
            _data.f330 = 1. + _data.cosim;
            _data.f330 = 1.875 * _data.f330 * _data.f330 * _data.f330;
            _data.del1 = 3. * _data.nm * _data.nm * aonv * aonv;
            _data.del2 = 2. * _data.del1 * _data.f220 * _data.g200 * SGP::Q22;
            _data.del3 = 3. * _data.del1 * _data.f330 * _data.g300 * SGP::Q33 * aonv;
            _data.del1 = _data.del1 * _data.f311 * _data.g310 * SGP::Q31 * aonv;
            _data.xlamo = fmod(_elements.mo + _elements.omegao + _elements.argpo - theta, MATHS::DEUX_PI);
            _data.xfact = _data.mdot + _data.xpidot - SGP::RPTIM + _data.dmdt + _data.domdt + _data.dnodt - _elements.no;
        }

        // Initialisation de l'integrateur
        _data.xli = _data.xlamo;
        _data.xni = _elements.no;
        _data.atime = 0.;
        _data.nm = _elements.no + _data.dndt;
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

    /* Corps de la methode */
    // Calcul des effets de resonance haute orbite
    _data.dndt = 0.;
    const double theta = fmod(_data.gsto + tc * SGP::RPTIM, MATHS::DEUX_PI);
    _data.em = _data.em + _data.dedt * _data.t;
    _data.inclm = _data.inclm + _data.didt * _data.t;
    _data.argpm = _data.argpm + _data.domdt * _data.t;
    _data.nodem = _data.nodem + _data.dnodt * _data.t;
    _data.mm = _data.mm + _data.dmdt * _data.t;

    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (_data.inclm < 0.) {
    //     _data.inclm = -_data.inclm;
    //     _data.argpm = _data.argpm - MATHS::PI;
    //     _data.nodem = _data.nodem + MATHS::PI;
    // }

    // Integration numerique (Euler-MacLaurin)
    if (_data.irez != 0) {

        double xldot = 0.;
        double xnddt = 0.;
        double xndt = 0.;
        double ft = 0.;

        if (fabs(_data.atime) <= MATHS::EPSDBL100 || _data.t * _data.atime <= 0. || fabs(_data.t) < fabs(_data.atime)) {
            _data.atime = 0.;
            _data.xni = _elements.no;
            _data.xli = _data.xlamo;
        }

        _data.delt = (_data.t > 0.) ? SGP::STEPP : SGP::STEPN;

        int iretn = 381;
        while (iretn == 381) {
            // Calculs des termes derives

            // Termes de resonance quasi-synchrones
            if (_data.irez != 2) {
                xndt = _data.del1 * sin(_data.xli - SGP::FASX2) + _data.del2 * sin(2. * (_data.xli - SGP::FASX4)) +
                        _data.del3 * sin(3. * (_data.xli - SGP::FASX6));
                xldot = _data.xni + _data.xfact;
                xnddt = _data.del1 * cos(_data.xli - SGP::FASX2) + 2. * _data.del2 * cos(2. * (_data.xli - SGP::FASX4)) + 3. * _data.del3 *
                        cos(3. * (_data.xli - SGP::FASX6));
                xnddt *= xldot;
            } else {

                // Termes de resonance d'environ 12h
                const double xomi = _elements.argpo + _data.argpdot * _data.atime;
                const double x2omi = xomi + xomi;
                const double x2li = _data.xli + _data.xli;
                xndt = _data.d2201 * sin(x2omi + _data.xli - SGP::G22) + _data.d2211 * sin(_data.xli - SGP::G22) +
                        _data.d3210 * sin(xomi + _data.xli - SGP::G32) + _data.d3222 * sin(-xomi + _data.xli - SGP::G32) +
                        _data.d4410 * sin(x2omi + x2li - SGP::G44) + _data.d4422 * sin(x2li - SGP::G44) + _data.d5220 *
                        sin(xomi + _data.xli - SGP::G52) + _data.d5232 * sin(-xomi + _data.xli - SGP::G52) + _data.d5421 *
                        sin(xomi + x2li - SGP::G54) + _data.d5433 * sin(-xomi + x2li - SGP::G54);
                xldot = _data.xni + _data.xfact;
                xnddt = _data.d2201 * cos(x2omi + _data.xli - SGP::G22) + _data.d2211 * cos(_data.xli - SGP::G22) +
                        _data.d3210 * cos(xomi + _data.xli - SGP::G32) + _data.d3222 * cos(-xomi + _data.xli - SGP::G32) +
                        _data.d5220 * cos(xomi + _data.xli - SGP::G52) + _data.d5232 * cos(-xomi + _data.xli - SGP::G52) +
                        2.0 * (_data.d4410 * cos(x2omi + x2li - SGP::G44) + _data.d4422 * cos(x2li - SGP::G44) +
                               _data.d5421 * cos(xomi + x2li - SGP::G54) + _data.d5433 * cos(-xomi + x2li - SGP::G54));
                xnddt *= xldot;
            }

            // Integrateur
            if (fabs(_data.t - _data.atime) >= SGP::STEPP) {
                iretn = 381;
            } else {
                ft = _data.t - _data.atime;
                iretn = 0;
            }

            if (iretn == 381) {
                _data.xli = _data.xli + xldot * _data.delt + xndt * SGP::STEP2;
                _data.xni = _data.xni + xndt * _data.delt + xnddt * SGP::STEP2;
                _data.atime += _data.delt;
            }
        }

        const double ft2 = ft * ft * 0.5;
        _data.nm = _data.xni + xndt * ft + xnddt * ft2;
        const double xl = _data.xli + xldot * ft + xndt * ft2;
        if (_data.irez == 1) {
            _data.mm = xl - _data.nodem - _data.argpm + theta;
            _data.dndt = _data.nm - _elements.no;
        } else {
            _data.mm = xl - 2. * _data.nodem + 2. * theta;
            _data.dndt = _data.nm - _elements.no;
        }
        _data.nm = _elements.no + _data.dndt;
    }

    /* Retour */
    return;
}

/*
 * Initialisation du modele SGP4
 */
void SGP4::SGP4Init(const ElementsOrbitaux &elements)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Recuperation des elements du TLE et formattage
    _elements = elements;
    _elements.argpo *= MATHS::DEG2RAD;
    _elements.inclo *= MATHS::DEG2RAD;
    _elements.mo *= MATHS::DEG2RAD;
    _elements.no *= MATHS::DEUX_PI * DATE::NB_JOUR_PAR_MIN;
    _elements.omegao *= MATHS::DEG2RAD;

    const double ss = 78. * SGP::X1SRT + 1.;
    const double qzms2t = pow((120. - 78.) * SGP::X1SRT, 4.);
    _data.t = 0.;

    _data.eccsq = _elements.ecco * _elements.ecco;
    _data.omeosq = 1. - _data.eccsq;
    _data.rteosq = sqrt(_data.omeosq);
    _data.cosio = cos(_elements.inclo);
    _data.cosio2 = _data.cosio * _data.cosio;

    const double ak = pow(TERRE::KE / _elements.no, MATHS::DEUX_TIERS);
    const double d1 = 0.75 * TERRE::J2 * (3. * _data.cosio2 - 1.) / (_data.rteosq * _data.omeosq);
    double del = d1 / (ak * ak);
    const double del2 = del * del;
    const double adel = ak * (1. - del2 - del * (0.5 * MATHS::DEUX_TIERS + del2 * (134. / 81.)));
    del = d1 / (adel * adel);
    _elements.no = _elements.no / (1. + del);

    _data.ao = pow(TERRE::KE / _elements.no, MATHS::DEUX_TIERS);
    _data.sinio = sin(_elements.inclo);
    const double po = _data.ao * _data.omeosq;
    _data.con42 = 1. - 5. * _data.cosio2;
    _data.con41 = -_data.con42 - _data.cosio2 - _data.cosio2;
    _data.posq = po * po;
    _data.rp = _data.ao * (1. - _elements.ecco);
    _data.method = 'n';

    _data.gsto = Observateur::CalculTempsSideralGreenwich(_elements.epoque);

    if ((_data.omeosq >= 0.) || (_elements.no > 0.)) {

        _data.isimp = (_data.rp < 220. * SGP::X1SRT + 1.);

        double sfour = ss;
        double qzms24 = qzms2t;

        const double perige = (_data.rp - 1.) * TERRE::RAYON_TERRESTRE;
        if (perige < 156.) {
            sfour = perige - 78.;
            if (perige < 98.) {
                sfour = 20.;
            }

            qzms24 = pow((120. - sfour) * SGP::X1SRT, 4.);
            sfour = sfour / TERRE::RAYON_TERRESTRE + 1.;
        }

        const double pinvsq = 1. / _data.posq;
        const double tsi = 1. / (_data.ao - sfour);
        _data.eta = _data.ao * _elements.ecco * tsi;

        const double etasq = _data.eta * _data.eta;
        const double eeta = _elements.ecco * _data.eta;
        const double psisq = fabs(1. - etasq);
        const double coef = qzms24 * pow(tsi, 4.);
        const double coef1 = coef * pow(psisq, -3.5);
        const double cc2 = coef1 * _elements.no * (_data.ao * (1. + 1.5 * etasq + eeta * (4. + etasq)) + 0.375 * TERRE::J2 * tsi / psisq *
                                                   _data.con41 * (8. + 3. * etasq * (8. + etasq)));
        _data.cc1 = _elements.bstar * cc2;
        double cc3 = 0.;
        if (_elements.ecco > 1.e-4) {
            cc3 = -2. * coef * tsi * SGP::J3SJ2 * _elements.no * _data.sinio / _elements.ecco;
        }

        _data.x1mth2 = 1. - _data.cosio2;
        _data.cc4 = 2. * _elements.no * coef1 * _data.ao * _data.omeosq *
                (_data.eta * (2. + 0.5 * etasq) + _elements.ecco * (0.5 + 2. * etasq) - TERRE::J2 * tsi /
                 (_data.ao * psisq) * (-3. * _data.con41 * (1. - 2. * eeta + etasq * (1.5 - 0.5 * eeta)) +
                                  0.75 * _data.x1mth2 * (2. * etasq - eeta * (1. + etasq)) * cos(2. * _elements.argpo)));
        _data.cc5 = 2. * coef1 * _data.ao * _data.omeosq * (1. + 2.75 * (etasq + eeta) + eeta * etasq);

        const double cosio4 = _data.cosio2 * _data.cosio2;
        const double temp1 = 1.5 * TERRE::J2 * pinvsq * _elements.no;
        const double temp2 = 0.5 * temp1 * TERRE::J2 * pinvsq;
        const double temp3 = -0.46875 * TERRE::J4 * pinvsq * pinvsq * _elements.no;
        _data.mdot = _elements.no + 0.5 * temp1 * _data.rteosq * _data.con41 + 0.0625 * temp2 * _data.rteosq *
                (13. - 78. * _data.cosio2 + 137. * cosio4);
        _data.argpdot = -0.5 * temp1 * _data.con42 + 0.0625 * temp2 * (7. - 114. * _data.cosio2 + 395. * cosio4) +
                temp3 * (3. - 36. * _data.cosio2 + 49. * cosio4);
        const double xhdot1 = -temp1 * _data.cosio;
        _data.nodedot = xhdot1 + (0.5 * temp2 * (4. - 19. * _data.cosio2) + 2. * temp3 * (3. - 7. * _data.cosio2)) * _data.cosio;
        _data.xpidot = _data.argpdot + _data.nodedot;
        _data.omgcof = _elements.bstar * cc3 * cos(_elements.argpo);
        _data.xmcof = 0.;
        if (_elements.ecco > 1.e-4) {
            _data.xmcof = -MATHS::DEUX_TIERS * coef * _elements.bstar / eeta;
        }

        _data.nodecf = 3.5 * _data.omeosq * xhdot1 * _data.cc1;
        _data.t2cof = 1.5 * _data.cc1;

        if (fabs(_data.cosio + 1.) > 1.5e-12) {
            _data.xlcof = -0.25 * SGP::J3SJ2 * _data.sinio * (3. + 5. * _data.cosio) / (1. + _data.cosio);
        } else {
            _data.xlcof = -0.25 * SGP::J3SJ2 * _data.sinio * (3. + 5. * _data.cosio) * (1. / 1.5e-12);
        }

        _data.aycof = -0.5 * SGP::J3SJ2 * _data.sinio;
        _data.delmo = pow((1. + _data.eta * cos(_elements.mo)), 3.);
        _data.sinmao = sin(_elements.mo);
        _data.x7thm1 = 7. * _data.cosio2 - 1.;

        // Initialisation du modele haute orbite
        if (MATHS::DEUX_PI >= 225. * _elements.no) {

            _data.method = 'd';
            _data.isimp = true;
            _data.inclm = _elements.inclo;

            const double tc = 0.;
            Dscom(tc);

            _data.mp = _elements.mo;
            _data.argpp = _elements.argpo;
            _data.ep = _elements.ecco;
            _data.nodep = _elements.omegao;
            _data.xincp = _elements.inclo;

            Dpper();

            _data.argpm = 0.;
            _data.nodem = 0.;
            _data.mm = 0.;

            Dsinit(tc);
        }

        if (!_data.isimp) {

            const double cc1sq = _data.cc1 * _data.cc1;
            _data.d2 = 4. * _data.ao * tsi * cc1sq;
            const double temp = _data.d2 * tsi * _data.cc1 * (1. / 3.);
            _data.d3 = (17. * _data.ao + sfour) * temp;
            _data.d4 = 0.5 * temp * _data.ao * tsi * (221. * _data.ao + 31. * sfour) * _data.cc1;
            _data.t3cof = _data.d2 + 2. * cc1sq;
            _data.t4cof = 0.25 * (3. * _data.d3 + _data.cc1 * (12. * _data.d2 + 10. * cc1sq));
            _data.t5cof = 0.2 * (3. * _data.d4 + 12. * _data.cc1 * _data.d3 + 6. * _data.d2 * _data.d2 + 15. * cc1sq *
                                    (2. * _data.d2 + cc1sq));
        }

        _init = true;
    }

    /* Retour */
    return;
}
