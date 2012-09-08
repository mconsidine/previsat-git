/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    satellite.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires lies a la position des satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    7 septembre 2012
 *
 */

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QTextStream>
#include "satellite.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/systemesolaire/SoleilConstants.h"
#include "librairies/exceptions/previsatexception.h"

static const double J3SJ2 = J3 / J2;
static const double RPTIM = OMEGA * NB_SEC_PAR_MIN;

bool Satellite::initCalcul = false;

/* Constructeurs */
Satellite::Satellite()
{
    _eclipse = true;
    _ieralt = true;
    _penombre = false;
    _methMagnitude = 'v';
    _nbOrbites = 0;
    _ageTLE = 0.;
    _elongation = 0.;
    _fractionIlluminee = 0.;
    _magnitude = 99.;
    _magnitudeStandard = 99.;
    _rayonApparentSoleil = 0.;
    _rayonApparentTerre = 0.;
    _section = 0.;
    _t1 = 0.;
    _t2 = 0.;
    _t3 = 0.;
    _sat = SatVariables();
}

Satellite::Satellite(const TLE &tle)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _tle = tle;
    SGP4Init();

    /* Retour */
    return;
}

/* Destructeur */
Satellite::~Satellite()
{
}

/* Methodes */
/*
 * Calcul de la position et de la vitesse du satellite
 * Modele SGP4 : d'apres l'article "Revisiting Spacetrack Report #3: Rev 1" de David Vallado (2006)
 */
void Satellite::CalculPosVit(const Date &date)
{
    /* Declarations des variables locales */
    int ktr;
    double coseo1, cosip, eo1, sineo1, sinip, tem5, temp, tempa, tempe, templ, xlm;

    /* Initialisations */
    coseo1 = 0.;
    sineo1 = 0.;

    // Calcul du temps ecoule depuis l'epoque (en minutes)
    const double tsince = NB_MIN_PAR_JOUR * (date.getJourJulienUTC() - _tle.getEpoque().getJourJulienUTC());

    /* Corps de la methode */
    try {
        _sat.t = tsince;

        // Prise en compte des termes seculaires de la gravite et du freinage atmospherique
        const double xmdf = _sat.mo + _sat.mdot * _sat.t;
        const double argpdf = _sat.argpo + _sat.argpdot * _sat.t;
        const double nodedf = _sat.omegao + _sat.nodedot * _sat.t;
        _sat.argpm = argpdf;
        _sat.mm = xmdf;
        const double t2 = _sat.t * _sat.t;
        _sat.nodem = nodedf + _sat.nodecf * t2;
        tempa = 1. - _sat.cc1 * _sat.t;
        tempe = _sat.bstar * _sat.cc4 * _sat.t;
        templ = _sat.t2cof * t2;

        if (!_sat.isimp) {

            const double delomg = _sat.omgcof * _sat.t;
            const double delm = _sat.xmcof * (pow((1. + _sat.eta * cos(xmdf)), 3.) - _sat.delmo);
            temp = delomg + delm;
            _sat.mm = xmdf + temp;
            _sat.argpm = argpdf - temp;
            const double t3 = t2 * _sat.t;
            const double t4 = t3 * _sat.t;
            tempa += -_sat.d2 * t2 - _sat.d3 * t3 - _sat.d4 * t4;
            tempe += _sat.bstar * _sat.cc5 * (sin(_sat.mm) - _sat.sinmao);
            templ += _sat.t3cof * t3 + t4 * (_sat.t4cof + _sat.t * _sat.t5cof);
        }

        _sat.nm = _sat.no;
        _sat.em = _sat.ecco;
        _sat.inclm = _sat.inclo;

        if (_sat.method == 'd') {
            const double tc = _sat.t;
            Dspace(tc);
        }

        const double am = pow((KE / _sat.nm), DEUX_TIERS) * tempa * tempa;
        _sat.nm = KE * pow(am, -1.5);
        _sat.em -= tempe;

        if (_sat.em < 1.e-6)
            _sat.em = 1.e-6;

        _sat.mm += _sat.no * templ;
        xlm = _sat.mm + _sat.argpm + _sat.nodem;
        _sat.emsq = _sat.em * _sat.em;
        temp = 1. - _sat.emsq;

        _sat.nodem = fmod(_sat.nodem, DEUX_PI);
        _sat.argpm = fmod(_sat.argpm, DEUX_PI);
        xlm = fmod(xlm, DEUX_PI);
        _sat.mm = fmod(xlm - _sat.argpm - _sat.nodem, DEUX_PI);

        _sat.sinim = sin(_sat.inclm);
        _sat.cosim = cos(_sat.inclm);

        // Prise en compte des termes periodiques luni-solaires
        _sat.ep = _sat.em;
        _sat.xincp = _sat.inclm;
        _sat.argpp = _sat.argpm;
        _sat.nodep = _sat.nodem;
        _sat.mp = _sat.mm;
        sinip = _sat.sinim;
        cosip = _sat.cosim;
        if (_sat.method == 'd') {
            Dpper();
            if (_sat.xincp < 0.) {
                _sat.xincp = -_sat.xincp;
                _sat.nodep += PI;
                _sat.argpp -= PI;
            }
        }

        // Termes longue periode
        if (_sat.method == 'd') {
            sinip = sin(_sat.xincp);
            cosip = cos(_sat.xincp);
            _sat.aycof = -0.5 * J3SJ2 * sinip;

            if (fabs(cosip + 1.) > 1.5e-12) {
                _sat.xlcof = -0.25 * J3SJ2 * sinip * (3. + 5. * cosip) / (1. + cosip);
            } else {
                _sat.xlcof = -0.25 * J3SJ2 * sinip * (3. + 5. * cosip) / 1.5e-12;
            }
        }

        const double axnl = _sat.ep * cos(_sat.argpp);
        temp = 1. / (am * (1. - _sat.ep * _sat.ep));
        const double aynl = _sat.ep * sin(_sat.argpp) + temp * _sat.aycof;
        const double xl = _sat.mp + _sat.argpp + _sat.nodep + temp * _sat.xlcof * axnl;

        // Resolution de l'equation de Kepler
        const double u = fmod(xl - _sat.nodep, DEUX_PI);
        eo1 = u;
        tem5 = 9999.9;
        ktr = 1;

        while (fabs(tem5) >= EPSDBL && ktr < 10) {
            sineo1 = sin(eo1);
            coseo1 = cos(eo1);
            tem5 = 1. - coseo1 * axnl - sineo1 * aynl;
            tem5 = (u - aynl * coseo1 + axnl * sineo1 - eo1) / tem5;
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
            const double sinu = am / rl * (sineo1 - aynl - axnl * temp);
            const double cosu = am / rl * (coseo1 - axnl + aynl * temp);
            su = atan2(sinu, cosu);
            const double sin2u = (cosu + cosu) * sinu;
            const double cos2u = 1. - 2. * sinu * sinu;
            temp = 1. / pl;
            const double temp1 = 0.5 * J2 * temp;
            const double temp2 = temp1 * temp;

            // Prise en compte des termes courte periode
            if (_sat.method == 'd') {

                const double cosisq = cosip * cosip;
                _sat.con41 = 3. * cosisq - 1.;
                _sat.x1mth2 = 1. - cosisq;
                _sat.x7thm1 = 7. * cosisq - 1.;
            }

            const double mrt = RAYON_TERRESTRE *
                    (rl * (1. - 1.5 * temp2 * betal * _sat.con41) + 0.5 * temp1 * _sat.x1mth2 * cos2u);
            su -= 0.25 * temp2 * _sat.x7thm1 * sin2u;
            const double xnode = _sat.nodep + 1.5 * temp2 * cosip * sin2u;
            const double xinc = _sat.xincp + 1.5 * temp2 * cosip * sinip * cos2u;
            const double mvt = RAYON_TERRESTRE * NB_MIN_PAR_SEC *
                    (KE * rdotl - _sat.nm * temp1 * _sat.x1mth2 * sin2u);
            const double rvdot = RAYON_TERRESTRE * NB_MIN_PAR_SEC *
                    (KE * rvdotl + _sat.nm * temp1 * (_sat.x1mth2 * cos2u + 1.5 * _sat.con41));

            // Vecteurs directeurs
            const double sinsu = sin(su);
            const double cossu = cos(su);
            const double snod = sin(xnode);
            const double cnod = cos(xnode);
            const double cosi = cos(xinc);

            Vecteur3D mm = Vecteur3D(-snod * cosi, cnod * cosi, sin(xinc));
            Vecteur3D mm1 = mm * sinsu;
            Vecteur3D mm2 = mm * cossu;

            Vecteur3D nn = Vecteur3D(cnod, snod, 0.);
            Vecteur3D nn1 = nn * (-cossu);
            Vecteur3D nn2 = nn * sinsu;

            Vecteur3D uu = mm1 - nn1;
            Vecteur3D uu1 = uu * mvt;
            Vecteur3D vv = mm2 - nn2;
            Vecteur3D vv1 = vv * (-rvdot);

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
 * Determination de la condition d'eclipse du satellite
 */
void Satellite::CalculSatelliteEclipse(const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _rayonApparentTerre = asin(RAYON_TERRESTRE / _position.Norme());
    if (std::isnan(_rayonApparentTerre))
        _rayonApparentTerre = PI_SUR_DEUX;

    const Vecteur3D rho = soleil.getPosition() - _position;
    _rayonApparentSoleil = asin(RAYON_SOLAIRE / rho.Norme());

    _elongation = soleil.getPosition().Angle((-_position));

    // Test si le satellite est en phase d'eclipse
    _eclipse = (_rayonApparentTerre > _rayonApparentSoleil
                && _elongation < _rayonApparentTerre - _rayonApparentSoleil) ? true : false;

    // Test si le satellite est dans la penombre
    _penombre = (_eclipse || (fabs(_rayonApparentTerre - _rayonApparentSoleil) < _elongation
                              && _elongation < _rayonApparentTerre + _rayonApparentSoleil)) ? true : false;

    /* Retour */
    return;
}

/*
 * Calcul de la magnitude visuelle du satellite
 */
void Satellite::CalculMagnitude(const Observateur &observateur, const bool extinction) {

    /* Declarations des variables locales */

    /* Initialisations */
    _magnitude = 99.;

    /* Corps de la methode */
    if (!_eclipse) {

        // Fraction illuminee
        _fractionIlluminee = 0.5 * (1. + cos(_elongation));

        // Magnitude
        if (_magnitudeStandard < 99.) {
            _magnitude = _magnitudeStandard - 15.75 + 2.5 * log10(_distance * _distance / _fractionIlluminee);
            if (extinction) {
                _magnitude += ExtinctionAtmospherique(observateur);
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Determination de l'extinction atmospherique, issu de l'article
 * "Magnitude corrections for atmospheric extinction" de Daniel Green, 1992
 */
double Satellite::ExtinctionAtmospherique(const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double corr = 0.;

    /* Corps de la methode */
    if (_hauteur >= 0.) {

        const double cosz = cos(PI_SUR_DEUX - _hauteur);
        const double x = 1. / (cosz + 0.025 * exp(-11. * cosz));
        corr = x * (0.016 + observateur.getAray() + observateur.getAaer());
    }

    /* Retour */
    return (corr);
}

/*
 * Calcul des elements osculateurs du satellite et du nombre d'orbites
 */
void Satellite::CalculElementsOsculateurs(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _elements.CalculElementsOsculateurs(_position, _vitesse);

    // Nombre d'orbites
    _ageTLE = date.getJourJulienUTC() - _tle.getEpoque().getJourJulienUTC();
    _nbOrbites = _tle.getNbOrbites() +
            (int) (floor((_tle.getNo() + _ageTLE * _tle.getBstar()) * _ageTLE +
                         Maths::modulo(_tle.getOmegao() + _tle.getMo(), DEUX_PI) / T360 -
                         Maths::modulo(_elements.getArgumentPerigee() + _elements.getAnomalieVraie(),
                                       DEUX_PI) / DEUX_PI + 0.5));

    /* Retour */
    return;
}

/*
 * Calcul de la position d'une liste de satellites
 */
void Satellite::CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil,
                                            const int nbTracesAuSol, const bool visibilite, const bool extinction,
                                            const bool traceCiel, QList<Satellite> &satellites)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    for (int isat=0; isat<satellites.size(); isat++) {

        // Position ECI du satellite
        satellites[isat].CalculPosVit(date);

        // Coordonnees horizontales du satellite
        satellites[isat].CalculCoordHoriz(observateur);

        // Calcul des conditions d'eclipse
        satellites[isat].CalculSatelliteEclipse(soleil);

        // Calcul des coordonnees terrestres
        satellites[isat].CalculCoordTerrestres(observateur);
        satellites[isat]._ieralt = (satellites[isat].getAltitude() < 0.) ? true : false;

        // Calcul de la zone de visibilite du satellite
        if (visibilite)
            satellites[isat].CalculZoneVisibilite();

        // Calcul de la trajectoire dans le ciel
        if (traceCiel)
            satellites[isat].CalculTraceCiel(date, observateur);

        if (isat == 0) {

            // Calcul des coordonnees equatoriales
            satellites[isat].CalculCoordEquat(observateur);

            // Calcul de la magnitude
            satellites[isat].CalculMagnitude(observateur, extinction);

            // Calcul des elements osculateurs et du nombre d'orbites
            satellites[isat].CalculElementsOsculateurs(date);

            // Calcul de la trajectoire
            satellites[isat].CalculTracesAuSol(date, nbTracesAuSol);
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture des donnees relatives aux satellites
 */
void Satellite::LectureDonnees(const QStringList &listeSatellites, const QVector<TLE> &tabtle, QList<Satellite> &satellites)
{
    /* Declarations des variables locales */
    QString dirDat;

    /* Initialisations */
#if defined (Q_OS_WIN)
    dirDat = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif
    const int nb = listeSatellites.size();

    if (!Satellite::initCalcul) {
        satellites.clear();
        QVectorIterator<TLE> it(tabtle);
        while (it.hasNext()) {
            satellites.append(Satellite(it.next()));
        }
        Satellite::initCalcul = true;
    }

    for (int isat=0; isat<nb; isat++)
        satellites[isat]._magnitudeStandard = 99.;

    /* Corps de la methode */
    const QString fic = dirDat + QDir::separator() + "donnees.sat";
    QFile fi(fic);
    if (fi.exists()) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        int j = 0;
        while (!flux.atEnd() && j < nb) {

            const QString ligne = flux.readLine();
            const QString norad = ligne.mid(0, 5);
            for(int isat=0; isat<nb; isat++) {
                if (listeSatellites.at(isat) == norad) {
                    satellites[isat]._t1 = ligne.mid(6, 5).toDouble();
                    satellites[isat]._t2 = ligne.mid(12, 4).toDouble();
                    satellites[isat]._t3 = ligne.mid(17, 4).toDouble();
                    satellites[isat]._magnitudeStandard = ligne.mid(22, 4).toDouble();
                    satellites[isat]._methMagnitude = ligne.at(27).toAscii();
                    satellites[isat]._section = ligne.mid(29, 6).toDouble();
                    j++;
                    break;
                }
            }
        }
    }
    fi.close();

    /* Retour */
    return;
}

/*
 * Initialisation du modele SGP4
 */
void Satellite::SGP4Init()
{
    /* Declarations des variables locales */
    double del;

    /* Initialisations */

    /* Corps de la methode */
    // Recuperation des elements du TLE et formattage
    _sat.argpo = DEG2RAD * _tle.getArgpo();
    _sat.bstar = _tle.getBstar();
    _sat.ecco = _tle.getEcco();
    _sat.epoque = _tle.getEpoque();
    _sat.inclo = DEG2RAD * _tle.getInclo();
    _sat.mo = DEG2RAD * _tle.getMo();
    _sat.no = _tle.getNo() * DEUX_PI * NB_JOUR_PAR_MIN;
    if (_sat.no < EPSDBL100)
        throw PreviSatException();

    _sat.omegao = DEG2RAD * _tle.getOmegao();

    const double ss = 78. / RAYON_TERRESTRE + 1.;
    const double qzms2t = pow((120. - 78.) / RAYON_TERRESTRE, 4.);
    _sat.t = 0.;

    _sat.eccsq = _sat.ecco * _sat.ecco;
    _sat.omeosq = 1. - _sat.eccsq;
    _sat.rteosq = sqrt(_sat.omeosq);
    _sat.cosio = cos(_sat.inclo);
    _sat.cosio2 = _sat.cosio * _sat.cosio;

    const double ak = pow(KE / _sat.no, DEUX_TIERS);
    const double d1 = 0.75 * J2 * (3. * _sat.cosio2 - 1.) / (_sat.rteosq * _sat.omeosq);
    del = d1 / (ak * ak);
    const double adel = ak * (1. - del * del - del * (0.5 * DEUX_TIERS + 134. * del * del / 81.));
    del = d1 / (adel * adel);
    _sat.no /= (1. + del);

    _sat.ao = pow(KE / _sat.no, DEUX_TIERS);
    _sat.sinio = sin(_sat.inclo);
    const double po = _sat.ao * _sat.omeosq;
    _sat.con42 = 1. - 5. * _sat.cosio2;
    _sat.con41 = -_sat.con42 - _sat.cosio2 - _sat.cosio2;
    _sat.posq = po * po;
    _sat.rp = _sat.ao * (1. - _sat.ecco);
    _sat.method = 'n';

    _sat.gsto = Observateur::CalculTempsSideralGreenwich(_sat.epoque);

    if (_sat.omeosq >= 0. || _sat.no > 0.) {
        double cc3, qzms24, sfour;

        _sat.isimp = false;
        if (_sat.rp < 220. / RAYON_TERRESTRE + 1.)
            _sat.isimp = true;

        sfour = ss;
        qzms24 = qzms2t;

        if (_sat.rp < 1.)
            throw new PreviSatException();

        const double perige = (_sat.rp - 1.) * RAYON_TERRESTRE;
        if (perige < 156.) {
            sfour = perige - 78.;
            if (perige < 98.)
                sfour = 20.;

            qzms24 = pow((120. - sfour) / RAYON_TERRESTRE, 4.);
            sfour /= RAYON_TERRESTRE + 1.;
        }

        const double pinvsq = 1. / _sat.posq;
        const double tsi = 1. / (_sat.ao - sfour);
        _sat.eta = _sat.ao * _sat.ecco * tsi;

        const double etasq = _sat.eta * _sat.eta;
        const double eeta = _sat.ecco * _sat.eta;
        const double psisq = fabs(1. - etasq);
        const double coef = qzms24 * pow(tsi, 4.);
        const double coef1 = coef * pow(psisq, -3.5);
        const double cc2 = coef1 * _sat.no * (_sat.ao * (1. + 1.5 * etasq + eeta * (4. + etasq)) + 0.375 * J2 * tsi / psisq *
                                              _sat.con41 * (8. + 3. * etasq * (8. + etasq)));
        _sat.cc1 = _sat.bstar * cc2;
        cc3 = 0.;
        if (_sat.ecco > 1.e-4)
            cc3 = -2. * coef * tsi * J3SJ2 * _sat.no * _sat.sinio / _sat.ecco;

        _sat.x1mth2 = 1. - _sat.cosio2;
        _sat.cc4 = 2. * _sat.no * coef1 * _sat.ao * _sat.omeosq *
                (_sat.eta * (2. + 0.5 * etasq) + _sat.ecco * (0.5 + 2. * etasq) - J2 * tsi /
                 (_sat.ao * psisq) * (-3. * _sat.con41 * (1. - 2. * eeta + etasq * (1.5 - 0.5 * eeta)) +
                                      0.75 * _sat.x1mth2 * (2. * etasq - eeta * (1. + etasq)) * cos(2. * _sat.argpo)));
        _sat.cc5 = 2. * coef1 * _sat.ao * _sat.omeosq * (1. + 2.75 * (etasq + eeta) + eeta * etasq);

        const double cosio4 = _sat.cosio2 * _sat.cosio2;
        const double temp1 = 1.5 * J2 * pinvsq * _sat.no;
        const double temp2 = 0.5 * temp1 * J2 * pinvsq;
        const double temp3 = -0.46875 * J4 * pinvsq * pinvsq * _sat.no;
        _sat.mdot = _sat.no + 0.5 * temp1 * _sat.rteosq * _sat.con41 + 0.0625 * temp2 * _sat.rteosq *
                (13. - 78. * _sat.cosio2 + 137. * cosio4);
        _sat.argpdot = -0.5 * temp1 * _sat.con42 + 0.0625 * temp2 * (7. - 114. * _sat.cosio2 + 395. * cosio4) +
                temp3 * (3. - 36. * _sat.cosio2 + 49. * cosio4);
        const double xhdot1 = -temp1 * _sat.cosio;
        _sat.nodedot = xhdot1 + (0.5 * temp2 * (4. - 19. * _sat.cosio2) + 2. * temp3 * (3. - 7. * _sat.cosio2)) * _sat.cosio;
        _sat.xpidot = _sat.argpdot + _sat.nodedot;
        _sat.omgcof = _sat.bstar * cc3 * cos(_sat.argpo);
        _sat.xmcof = 0.;
        if (_sat.ecco > 1.e-4)
            _sat.xmcof = -DEUX_TIERS * coef * _sat.bstar / eeta;

        _sat.nodecf = 3.5 * _sat.omeosq * xhdot1 * _sat.cc1;
        _sat.t2cof = 1.5 * _sat.cc1;

        if (fabs(_sat.cosio + 1.) > 1.5e-12) {
            _sat.xlcof = -0.25 * J3SJ2 * _sat.sinio * (3. + 5. * _sat.cosio) / (1. + _sat.cosio);
        } else {
            _sat.xlcof = -0.25 * J3SJ2 * _sat.sinio * (3. + 5. * _sat.cosio) / 1.5e-12;
        }

        _sat.aycof = -0.5 * J3SJ2 * _sat.sinio;
        _sat.delmo = pow((1. + _sat.eta * cos(_sat.mo)), 3.);
        _sat.sinmao = sin(_sat.mo);
        _sat.x7thm1 = 7. * _sat.cosio2 - 1.;

        // Initialisation du modele haute orbite
        if (DEUX_PI >= 225. * _sat.no) {

            _sat.method = 'd';
            _sat.isimp = true;
            _sat.inclm = _sat.inclo;

            const double tc = 0.;
            Dscom(tc);

            _sat.mp = _sat.mo;
            _sat.argpp = _sat.argpo;
            _sat.ep = _sat.ecco;
            _sat.nodep = _sat.omegao;
            _sat.xincp = _sat.inclo;

            Dpper();

            _sat.argpm = 0.;
            _sat.nodem = 0.;
            _sat.mm = 0.;

            Dsinit(tc);
        }

        if (!_sat.isimp) {

            const double cc1sq = _sat.cc1 * _sat.cc1;
            _sat.d2 = 4. * _sat.ao * tsi * cc1sq;
            const double temp = _sat.d2 * tsi * _sat.cc1 / 3.;
            _sat.d3 = (17. * _sat.ao + sfour) * temp;
            _sat.d4 = 0.5 * temp * _sat.ao * tsi * (221. * _sat.ao + 31. * sfour) * _sat.cc1;
            _sat.t3cof = _sat.d2 + 2. * cc1sq;
            _sat.t4cof = 0.25 * (3. * _sat.d3 + _sat.cc1 * (12. * _sat.d2 + 10. * cc1sq));
            _sat.t5cof = 0.2 * (3. * _sat.d4 + 12. * _sat.cc1 * _sat.d3 + 6. * _sat.d2 * _sat.d2 + 15. *
                                cc1sq * (2. * _sat.d2 + cc1sq));
        }
        _sat.init = true;
    } else {
        throw new PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void Satellite::Dscom(const double tc) {

    /* Declarations des variables locales */
    double zx;

    /* Initialisations */

    /* Corps de la methode */
    _sat.nm = _sat.no;
    _sat.em = _sat.ecco;
    _sat.snodm = sin(_sat.omegao);
    _sat.cnodm = cos(_sat.omegao);
    _sat.sinomm = sin(_sat.argpo);
    _sat.cosomm = cos(_sat.argpo);
    _sat.sinim = sin(_sat.inclo);
    _sat.cosim = cos(_sat.inclo);
    _sat.emsq = _sat.em * _sat.em;
    const double betasq = 1. - _sat.emsq;
    _sat.rtemsq = sqrt(betasq);

    // Initialisation des termes luni-solaires
    _sat.day = _sat.epoque.getJourJulienUTC() + NB_JOURS_PAR_SIECJ + tc * NB_JOUR_PAR_MIN;
    const double xnodce = fmod(4.523602 - 0.00092422029 * _sat.day, DEUX_PI);
    const double stem = sin(xnodce);
    const double ctem = cos(xnodce);
    const double zcosil = 0.91375164 - 0.03568096 * ctem;
    const double zsinil = sqrt(1. - zcosil * zcosil);
    const double zsinhl = 0.089683511 * stem / zsinil;
    const double zcoshl = sqrt(1. - zsinhl * zsinhl);
    _sat.gam = 5.8351514 + 0.001944368 * _sat.day;
    zx = 0.39785416 * stem / zsinil;
    const double zy = zcoshl * ctem + 0.91744867 * zsinhl * stem;
    zx = atan2(zx, zy);
    zx = _sat.gam + zx - xnodce;
    const double zcosgl = cos(zx);
    const double zsingl = sin(zx);

    // Termes solaires
    double zcosg = ZCOSGS;
    double zsing = ZSINGS;
    double zcosi = ZCOSIS;
    double zsini = ZSINIS;
    double zcosh = _sat.cnodm;
    double zsinh = _sat.snodm;
    double cc = C1SS;
    double xnoi = 1. / _sat.nm;

    for (int lsflg = 1; lsflg <= 2; lsflg++) {

        const double a1 = zcosg * zcosh + zsing * zcosi * zsinh;
        const double a3 = -zsing * zcosh + zcosg * zcosi * zsinh;
        const double a7 = -zcosg * zsinh + zsing * zcosi * zcosh;
        const double a8 = zsing * zsini;
        const double a9 = zsing * zsinh + zcosg * zcosi * zcosh;
        const double a10 = zcosg * zsini;
        const double a2 = _sat.cosim * a7 + _sat.sinim * a8;
        const double a4 = _sat.cosim * a9 + _sat.sinim * a10;
        const double a5 = -_sat.sinim * a7 + _sat.cosim * a8;
        const double a6 = -_sat.sinim * a9 + _sat.cosim * a10;

        const double x1 = a1 * _sat.cosomm + a2 * _sat.sinomm;
        const double x2 = a3 * _sat.cosomm + a4 * _sat.sinomm;
        const double x3 = -a1 * _sat.sinomm + a2 * _sat.cosomm;
        const double x4 = -a3 * _sat.sinomm + a4 * _sat.cosomm;
        const double x5 = a5 * _sat.sinomm;
        const double x6 = a6 * _sat.sinomm;
        const double x7 = a5 * _sat.cosomm;
        const double x8 = a6 * _sat.cosomm;

        _sat.z31 = 12. * x1 * x1 - 3. * x3 * x3;
        _sat.z32 = 24. * x1 * x2 - 6. * x3 * x4;
        _sat.z33 = 12. * x2 * x2 - 3. * x4 * x4;
        _sat.z1 = 3. * (a1 * a1 + a2 * a2) + _sat.z31 * _sat.emsq;
        _sat.z2 = 6. * (a1 * a3 + a2 * a4) + _sat.z32 * _sat.emsq;
        _sat.z3 = 3. * (a3 * a3 + a4 * a4) + _sat.z33 * _sat.emsq;
        _sat.z11 = -6. * a1 * a5 + _sat.emsq * (-24. * x1 * x7 - 6. * x3 * x5);
        _sat.z12 = -6. * (a1 * a6 + a3 * a5) + _sat.emsq * (-24. * (x2 * x7 + x1 * x8) - 6. * (x3 * x6 + x4 * x5));
        _sat.z13 = -6. * a3 * a6 + _sat.emsq * (-24. * x2 * x8 - 6. * x4 * x6);
        _sat.z21 = 6. * a2 * a5 + _sat.emsq * (24. * x1 * x5 - 6. * x3 * x7);
        _sat.z22 = 6. * (a4 * a5 + a2 * a6) + _sat.emsq * (24. * (x2 * x5 + x1 * x6) - 6. * (x4 * x7 + x3 * x8));
        _sat.z23 = 6. * a4 * a6 + _sat.emsq * (24. * x2 * x6 - 6. * x4 * x8);
        _sat.z1 = _sat.z1 + _sat.z1 + betasq * _sat.z31;
        _sat.z2 = _sat.z2 + _sat.z2 + betasq * _sat.z32;
        _sat.z3 = _sat.z3 + _sat.z3 + betasq * _sat.z33;
        _sat.s3 = cc * xnoi;
        _sat.s2 = -0.5 * _sat.s3 / _sat.rtemsq;
        _sat.s4 = _sat.s3 * _sat.rtemsq;
        _sat.s1 = -15. * _sat.em * _sat.s4;
        _sat.s5 = x1 * x3 + x2 * x4;
        _sat.s6 = x2 * x3 + x1 * x4;
        _sat.s7 = x2 * x4 - x1 * x3;

        // Termes lunaires
        if (lsflg == 1) {
            _sat.ss1 = _sat.s1;
            _sat.ss2 = _sat.s2;
            _sat.ss3 = _sat.s3;
            _sat.ss4 = _sat.s4;
            _sat.ss5 = _sat.s5;
            _sat.ss6 = _sat.s6;
            _sat.ss7 = _sat.s7;
            _sat.sz1 = _sat.z1;
            _sat.sz2 = _sat.z2;
            _sat.sz3 = _sat.z3;
            _sat.sz11 = _sat.z11;
            _sat.sz12 = _sat.z12;
            _sat.sz13 = _sat.z13;
            _sat.sz21 = _sat.z21;
            _sat.sz22 = _sat.z22;
            _sat.sz23 = _sat.z23;
            _sat.sz31 = _sat.z31;
            _sat.sz32 = _sat.z32;
            _sat.sz33 = _sat.z33;
            zcosg = zcosgl;
            zsing = zsingl;
            zcosi = zcosil;
            zsini = zsinil;
            zcosh = zcoshl * _sat.cnodm + zsinhl * _sat.snodm;
            zsinh = _sat.snodm * zcoshl - _sat.cnodm * zsinhl;
            cc = C1L;
        }
    }

    _sat.zmol = fmod(4.7199672 + 0.2299715 * _sat.day - _sat.gam, DEUX_PI);
    _sat.zmos = fmod(6.2565837 + 0.017201977 * _sat.day, DEUX_PI);

    // Termes solaires
    _sat.se2 = 2. * _sat.ss1 * _sat.ss6;
    _sat.se3 = 2. * _sat.ss1 * _sat.ss7;
    _sat.si2 = 2. * _sat.ss2 * _sat.sz12;
    _sat.si3 = 2. * _sat.ss2 * (_sat.sz13 - _sat.sz11);
    _sat.sl2 = -2. * _sat.ss3 * _sat.sz2;
    _sat.sl3 = -2. * _sat.ss3 * (_sat.sz3 - _sat.sz1);
    _sat.sl4 = -2. * _sat.ss3 * (-21. - 9. * _sat.emsq) * ZES;
    _sat.sgh2 = 2. * _sat.ss4 * _sat.sz32;
    _sat.sgh3 = 2. * _sat.ss4 * (_sat.sz33 - _sat.sz31);
    _sat.sgh4 = -18. * _sat.ss4 * ZES;
    _sat.sh2 = -2. * _sat.ss2 * _sat.sz22;
    _sat.sh3 = -2. * _sat.ss2 * (_sat.sz23 - _sat.sz21);

    // Termes lunaires
    _sat.ee2 = 2. * _sat.s1 * _sat.s6;
    _sat.e3 = 2. * _sat.s1 * _sat.s7;
    _sat.xi2 = 2. * _sat.s2 * _sat.z12;
    _sat.xi3 = 2. * _sat.s2 * (_sat.z13 - _sat.z11);
    _sat.xl2 = -2. * _sat.s3 * _sat.z2;
    _sat.xl3 = -2. * _sat.s3 * (_sat.z3 - _sat.z1);
    _sat.xl4 = -2. * _sat.s3 * (-21. - 9. * _sat.emsq) * ZEL;
    _sat.xgh2 = 2. * _sat.s4 * _sat.z32;
    _sat.xgh3 = 2. * _sat.s4 * (_sat.z33 - _sat.z31);
    _sat.xgh4 = -18. * _sat.s4 * ZEL;
    _sat.xh2 = -2. * _sat.s2 * _sat.z22;
    _sat.xh3 = -2. * _sat.s2 * (_sat.z23 - _sat.z21);

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void Satellite::Dpper() {

    /* Declarations des variables locales */
    double f2, f3, pgh, ph, sinzf, zf, zm;

    /* Initialisations */

    /* Corps de la methode */
    zm = _sat.zmos + ZNS * _sat.t;
    if (!_sat.init)
        zm = _sat.zmos;

    zf = zm + 2. * ZES * sin(zm);
    sinzf = sin(zf);
    f2 = 0.5 * sinzf * sinzf - 0.25;
    f3 = -0.5 * sinzf * cos(zf);
    const double ses = _sat.se2 * f2 + _sat.se3 * f3;
    const double sis = _sat.si2 * f2 + _sat.si3 * f3;
    const double sls = _sat.sl2 * f2 + _sat.sl3 * f3 + _sat.sl4 * sinzf;
    const double sghs = _sat.sgh2 * f2 + _sat.sgh3 * f3 + _sat.sgh4 * sinzf;
    const double shs = _sat.sh2 * f2 + _sat.sh3 * f3;
    zm = _sat.zmol + ZNL * _sat.t;
    if (!_sat.init)
        zm = _sat.zmol;

    zf = zm + 2. * ZEL * sin(zm);
    sinzf = sin(zf);
    f2 = 0.5 * sinzf * sinzf - 0.25;
    f3 = -0.5 * sinzf * cos(zf);

    const double sel = _sat.ee2 * f2 + _sat.e3 * f3;
    const double sil = _sat.xi2 * f2 + _sat.xi3 * f3;
    const double sll = _sat.xl2 * f2 + _sat.xl3 * f3 + _sat.xl4 * sinzf;
    const double sghl = _sat.xgh2 * f2 + _sat.xgh3 * f3 + _sat.xgh4 * sinzf;
    const double shll = _sat.xh2 * f2 + _sat.xh3 * f3;
    const double pe = ses + sel;
    const double pinc = sis + sil;
    const double pl = sls + sll;
    pgh = sghs + sghl;
    ph = shs + shll;

    if (_sat.init) {

        _sat.xincp += pinc;
        _sat.ep += pe;
        const double sinip = sin(_sat.xincp);
        const double cosip = cos(_sat.xincp);

        // Application directe des termes periodiques
        if (_sat.xincp >= 0.2) {
            ph /= sinip;
            pgh -= cosip * ph;
            _sat.argpp += pgh;
            _sat.nodep += ph;
            _sat.mp += pl;
        } else {

            // Application des termes periodiques avec la modification de Lyddane
            double alfdp, betdp, xls;

            const double sinop = sin(_sat.nodep);
            const double cosop = cos(_sat.nodep);
            alfdp = sinip * sinop;
            betdp = sinip * cosop;
            const double dalf = ph * cosop + pinc * cosip * sinop;
            const double dbet = -ph * sinop + pinc * cosip * cosop;
            alfdp += dalf;
            betdp += dbet;
            _sat.nodep = fmod(_sat.nodep, DEUX_PI);
            if (_sat.nodep < 0.)
                _sat.nodep += DEUX_PI;

            xls = _sat.mp + _sat.argpp + cosip * _sat.nodep;
            const double dls = pl + pgh - pinc * _sat.nodep * sinip;
            xls += dls;
            const double xnoh = _sat.nodep;
            _sat.nodep = atan2(alfdp, betdp);
            if (_sat.nodep < 0.)
                _sat.nodep += DEUX_PI;

            if (fabs(xnoh - _sat.nodep) > PI)
                _sat.nodep = (_sat.nodep < xnoh) ? _sat.nodep + DEUX_PI : _sat.nodep - DEUX_PI;

            _sat.mp += pl;
            _sat.argpp = xls - _sat.mp - cosip * _sat.nodep;
        }
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void Satellite::Dsinit(const double tc) {

    /* Declarations des variables locales */
    double shll, shs;

    /* Initialisations */

    /* Corps de la methode */
    _sat.irez = 0;
    if (_sat.nm < 0.0052359877 && _sat.nm > 0.0034906585)
        _sat.irez = 1;

    if (_sat.nm >= 0.00826 && _sat.nm <= 0.00924 && _sat.em >= 0.5)
        _sat.irez = 2;

    // Termes solaires
    const double ses = _sat.ss1 * ZNS * _sat.ss5;
    const double sis = _sat.ss2 * ZNS * (_sat.sz11 + _sat.sz13);
    const double sls = -(ZNS * _sat.ss3 * (_sat.sz1 + _sat.sz3 - 14. - 6. * _sat.emsq));
    const double sghs = _sat.ss4 * ZNS * (_sat.sz31 + _sat.sz33 - 6.);
    shs = -(ZNS * _sat.ss2 * (_sat.sz21 + _sat.sz23));

    if (_sat.inclm < 0.052359877 || _sat.inclm > PI - 0.052359877)
        shs = 0.;

    if (_sat.sinim != 0.)
        shs /= _sat.sinim;

    const double sgs = sghs - _sat.cosim * shs;

    // Termes lunaires
    _sat.dedt = ses + _sat.s1 * ZNL * _sat.s5;
    _sat.didt = sis + _sat.s2 * ZNL * (_sat.z11 + _sat.z13);
    _sat.dmdt = sls - ZNL * _sat.s3 * (_sat.z1 + _sat.z3 - 14. - 6. * _sat.emsq);
    const double sghl = _sat.s4 * ZNL * (_sat.z31 + _sat.z33 - 6.);
    shll = -(ZNL * _sat.s2 * (_sat.z21 + _sat.z23));

    if (_sat.inclm < 0.052359877 || _sat.inclm > PI - 0.052359877)
        shll = 0.;

    _sat.domdt = sgs + sghl;
    _sat.dnodt = shs;
    if (_sat.sinim != 0.) {
        _sat.domdt -= (_sat.cosim / _sat.sinim * shll);
        _sat.dnodt += (shll / _sat.sinim);
    }

    // Calcul des effets de resonance haute orbite
    _sat.dndt = 0.;
    const double theta = fmod(_sat.gsto + tc * RPTIM, DEUX_PI);
    _sat.em += _sat.dedt * _sat.t;
    _sat.inclm += _sat.didt * _sat.t;
    _sat.argpm += _sat.domdt * _sat.t;
    _sat.nodem += _sat.dnodt * _sat.t;
    _sat.mm += _sat.dmdt * _sat.t;
    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (inclm < 0.)
    // {
    //     inclm = -inclm
    //     argpm = argpm - PI
    //     nodem = nodem + PI
    // }

    // Initialisation des termes de resonance
    if (_sat.irez != 0) {

        const double aonv = pow(_sat.nm / KE, DEUX_TIERS);

        // Resonance geopotentielle pour les orbites de 12h
        if (_sat.irez == 2) {
            double temp, temp1;

            const double cosisq = _sat.cosim * _sat.cosim;
            const double emo = _sat.em;
            _sat.em = _sat.ecco;
            const double emsqo = _sat.emsq;
            _sat.emsq = _sat.eccsq;
            const double eoc = _sat.em * _sat.emsq;
            _sat.g201 = -0.306 - (_sat.em - 0.64) * 0.44;

            if (_sat.em <= 0.65) {
                _sat.g211 = 3.616 - 13.247 * _sat.em + 16.29 * _sat.emsq;
                _sat.g310 = -19.302 + 117.39 * _sat.em - 228.419 * _sat.emsq + 156.591 * eoc;
                _sat.g322 = -18.9068 + 109.7927 * _sat.em - 214.6334 * _sat.emsq + 146.5816 * eoc;
                _sat.g410 = -41.122 + 242.694 * _sat.em - 471.094 * _sat.emsq + 313.953 * eoc;
                _sat.g422 = -146.407 + 841.88 * _sat.em - 1629.014 * _sat.emsq + 1083.435 * eoc;
                _sat.g520 = -532.114 + 3017.977 * _sat.em - 5740.032 * _sat.emsq + 3708.276 * eoc;
            } else {
                _sat.g211 = -72.099 + 331.819 * _sat.em - 508.738 * _sat.emsq + 266.724 * eoc;
                _sat.g310 = -346.844 + 1582.851 * _sat.em - 2415.925 * _sat.emsq + 1246.113 * eoc;
                _sat.g322 = -342.585 + 1554.908 * _sat.em - 2366.899 * _sat.emsq + 1215.972 * eoc;
                _sat.g410 = -1052.797 + 4758.686 * _sat.em - 7193.992 * _sat.emsq + 3651.957 * eoc;
                _sat.g422 = -3581.69 + 16178.11 * _sat.em - 24462.77 * _sat.emsq + 12422.52 * eoc;
                if (_sat.em > 0.715) {
                    _sat.g520 = -5149.66 + 29936.92 * _sat.em - 54087.36 * _sat.emsq + 31324.56 * eoc;
                } else {
                    _sat.g520 = 1464.74 - 4664.75 * _sat.em + 3763.64 * _sat.emsq;
                }
            }
            if (_sat.em < 0.7) {
                _sat.g533 = -919.2277 + 4988.61 * _sat.em - 9064.77 * _sat.emsq + 5542.21 * eoc;
                _sat.g521 = -822.71072 + 4568.6173 * _sat.em - 8491.4146 * _sat.emsq + 5337.524 * eoc;
                _sat.g532 = -853.666 + 4690.25 * _sat.em - 8624.77 * _sat.emsq + 5341.4 * eoc;
            } else {
                _sat.g533 = -37995.78 + 161616.52 * _sat.em - 229838.2 * _sat.emsq + 109377.94 * eoc;
                _sat.g521 = -51752.104 + 218913.95 * _sat.em - 309468.16 * _sat.emsq + 146349.42 * eoc;
                _sat.g532 = -40023.88 + 170470.89 * _sat.em - 242699.48 * _sat.emsq + 115605.82 * eoc;
            }

            const double sini2 = _sat.sinim * _sat.sinim;
            _sat.f220 = 0.75 * (1. + 2. * _sat.cosim + cosisq);
            _sat.f221 = 1.5 * sini2;
            _sat.f321 = 1.875 * _sat.sinim * (1. - 2. * _sat.cosim - 3. * cosisq);
            _sat.f322 = -1.875 * _sat.sinim * (1. + 2. * _sat.cosim - 3. * cosisq);
            _sat.f441 = 35. * sini2 * _sat.f220;
            _sat.f442 = 39.375 * sini2 * sini2;
            _sat.f522 = 9.84375 * _sat.sinim * (sini2 * (1. - 2. * _sat.cosim - 5. * cosisq) + 0.33333333 *
                                                (-2. + 4. * _sat.cosim + 6. * cosisq));
            _sat.f523 = _sat.sinim * (4.92187512 * sini2 * (-2. - 4. * _sat.cosim + 10. * cosisq) +
                                      6.56250012 * (1. + 2. * _sat.cosim - 3. * cosisq));
            _sat.f542 = 29.53125 * _sat.sinim * (2. - 8. * _sat.cosim + cosisq * (-12. + 8. * _sat.cosim + 10. * cosisq));
            _sat.f543 = 29.53125 * _sat.sinim * (-2. - 8. * _sat.cosim + cosisq * (12. + 8. * _sat.cosim - 10. * cosisq));

            const double xno2 = _sat.nm * _sat.nm;
            const double ainv2 = aonv * aonv;
            temp1 = 3. * xno2 * ainv2;
            temp = temp1 * ROOT22;
            _sat.d2201 = temp * _sat.f220 * _sat.g201;
            _sat.d2211 = temp * _sat.f221 * _sat.g211;
            temp1 *= aonv;
            temp = temp1 * ROOT32;
            _sat.d3210 = temp * _sat.f321 * _sat.g310;
            _sat.d3222 = temp * _sat.f322 * _sat.g322;
            temp1 *= aonv;
            temp = 2. * temp1 * ROOT44;
            _sat.d4410 = temp * _sat.f441 * _sat.g410;
            _sat.d4422 = temp * _sat.f442 * _sat.g422;
            temp1 *= aonv;
            temp = temp1 * ROOT52;
            _sat.d5220 = temp * _sat.f522 * _sat.g520;
            _sat.d5232 = temp * _sat.f523 * _sat.g532;
            temp = 2. * temp1 * ROOT54;
            _sat.d5421 = temp * _sat.f542 * _sat.g521;
            _sat.d5433 = temp * _sat.f543 * _sat.g533;
            _sat.xlamo = fmod(_sat.mo + _sat.omegao + _sat.omegao - theta - theta, DEUX_PI);
            _sat.xfact = _sat.mdot + _sat.dmdt + 2. * (_sat.nodedot + _sat.dnodt - RPTIM) - _sat.no;
            _sat.em = emo;
            _sat.emsq = emsqo;
        }

        // Termes de resonance synchrones
        if (_sat.irez == 1) {
            _sat.g200 = 1. + _sat.emsq * (-2.5 + 0.8125 * _sat.emsq);
            _sat.g310 = 1. + 2. * _sat.emsq;
            _sat.g300 = 1. + _sat.emsq * (-6. + 6.60937 * _sat.emsq);
            _sat.f220 = 0.75 * (1. + _sat.cosim) * (1. + _sat.cosim);
            _sat.f311 = 0.9375 * _sat.sinim * _sat.sinim * (1. + 3. * _sat.cosim) - 0.75 * (1. + _sat.cosim);
            _sat.f330 = 1. + _sat.cosim;
            _sat.f330 = 1.875 * _sat.f330 * _sat.f330 * _sat.f330;
            _sat.del1 = 3. * _sat.nm * _sat.nm * aonv * aonv;
            _sat.del2 = 2. * _sat.del1 * _sat.f220 * _sat.g200 * Q22;
            _sat.del3 = 3. * _sat.del1 * _sat.f330 * _sat.g300 * Q33 * aonv;
            _sat.del1 *= _sat.f311 * _sat.g310 * Q31 * aonv;
            _sat.xlamo = fmod(_sat.mo + _sat.omegao + _sat.argpo - theta, DEUX_PI);
            _sat.xfact = _sat.mdot + _sat.xpidot - RPTIM + _sat.dmdt + _sat.domdt + _sat.dnodt - _sat.no;
        }

        // Initialisation de l'integrateur
        _sat.xli = _sat.xlamo;
        _sat.xni = _sat.no;
        _sat.atime = 0.;
        _sat.nm = _sat.no + _sat.dndt;
    }

    /* Retour */
    return;
}

/*
 * Modele haute orbite
 */
void Satellite::Dspace(const double tc) {

    /* Declarations des variables locales */
    double xldot, xnddt, xndt;

    /* Initialisations */
    xldot = 0.;
    xnddt = 0.;
    xndt = 0.;

    /* Corps de la methode */
    // Calcul des effets de resonance haute orbite
    _sat.dndt = 0.;
    const double theta = fmod(_sat.gsto + tc * RPTIM, DEUX_PI);
    _sat.em += _sat.dedt * _sat.t;
    _sat.inclm += _sat.didt * _sat.t;
    _sat.argpm += _sat.domdt * _sat.t;
    _sat.nodem += _sat.dnodt * _sat.t;
    _sat.mm += _sat.dmdt * _sat.t;

    // sgp4fix for negative inclinations
    // the following if statement should be commented out
    // if (inclm < 0.)
    // {
    //     inclm = -inclm
    //     argpm = argpm - PI
    //     nodem = nodem + PI
    // }

    // Integration numerique (Euler-MacLaurin)
    if (_sat.irez != 0) {
        double ft = 0.;

        if (_sat.atime == 0. || _sat.t * _sat.atime <= 0. || fabs(_sat.t) < fabs(_sat.atime)) {
            _sat.atime = 0.;
            _sat.xni = _sat.no;
            _sat.xli = _sat.xlamo;
        }
        if (_sat.t > 0.) {
            _sat.delt = STEPP;
        } else {
            _sat.delt = STEPN;
        }

        int iretn = 381;
        while (iretn == 381) {
            // Calculs des termes derives

            // Termes de resonance quasi-synchrones
            if (_sat.irez != 2) {
                xndt = _sat.del1 * sin(_sat.xli - FASX2) + _sat.del2 * sin(2. * (_sat.xli - FASX4)) +
                        _sat.del3 * sin(3. * (_sat.xli - FASX6));
                xldot = _sat.xni + _sat.xfact;
                xnddt = _sat.del1 * cos(_sat.xli - FASX2) + 2. * _sat.del2 * cos(2. * (_sat.xli - FASX4)) +
                        3. * _sat.del3 * cos(3. * (_sat.xli - FASX6));
                xnddt *= xldot;
            } else {

                // Termes de resonance d'environ 12h
                const double xomi = _sat.argpo + _sat.argpdot * _sat.atime;
                const double x2omi = xomi + xomi;
                const double x2li = _sat.xli + _sat.xli;
                xndt = _sat.d2201 * sin(x2omi + _sat.xli - G22) + _sat.d2211 * sin(_sat.xli - G22) +
                        _sat.d3210 * sin(xomi + _sat.xli - G32) + _sat.d3222 * sin(-xomi + _sat.xli - G32) +
                        _sat.d4410 * sin(x2omi + x2li - G44) + _sat.d4422 * sin(x2li - G44) + _sat.d5220 *
                        sin(xomi + _sat.xli - G52) + _sat.d5232 * sin(-xomi + _sat.xli - G52) + _sat.d5421 *
                        sin(xomi + x2li - G54) + _sat.d5433 * sin(-xomi + x2li - G54);
                xldot = _sat.xni + _sat.xfact;
                xnddt = _sat.d2201 * cos(x2omi + _sat.xli - G22) + _sat.d2211 * cos(_sat.xli - G22) +
                        _sat.d3210 * cos(xomi + _sat.xli - G32) + _sat.d3222 * cos(-xomi + _sat.xli - G32) +
                        _sat.d5220 * cos(xomi + _sat.xli - G52) + _sat.d5232 * cos(-xomi + _sat.xli - G52) +
                        2.0 * (_sat.d4410 * cos(x2omi + x2li - G44) + _sat.d4422 * cos(x2li - G44) +
                               _sat.d5421 * cos(xomi + x2li - G54) + _sat.d5433 * cos(-xomi + x2li - G54));
                xnddt *= xldot;
            }

            // Integrateur
            if (fabs(_sat.t - _sat.atime) >= STEPP) {
                iretn = 381;
            } else {
                ft = _sat.t - _sat.atime;
                iretn = 0;
            }

            if (iretn == 381) {
                _sat.xli += xldot * _sat.delt + xndt * STEP2;
                _sat.xni += xndt * _sat.delt + xnddt * STEP2;
                _sat.atime += _sat.delt;
            }
        }

        _sat.nm = _sat.xni + xndt * ft + xnddt * ft * ft * 0.5;
        const double xl = _sat.xli + xldot * ft + xndt * ft * ft * 0.5;
        if (_sat.irez != 1) {
            _sat.mm = xl - 2. * _sat.nodem + 2. * theta;
            _sat.dndt = _sat.nm - _sat.no;
        } else {
            _sat.mm = xl - _sat.nodem - _sat.argpm + theta;
            _sat.dndt = _sat.nm - _sat.no;
        }
        _sat.nm = _sat.no + _sat.dndt;
    }

    /* Retour */
    return;
}

/*
 * Calcul de la trajectoire du satellite
 */
void Satellite::CalculTracesAuSol(const Date &date, const int nbOrbites)
{
    /* Declarations des variables locales */
    double lat, lon, phi;
    Satellite sat = Satellite(_tle);
    Soleil soleil;

    /* Initialisations */
    const double st = 1. / (_tle.getNo() * T360);

    /* Corps de la methode */
    _traceAuSol.clear();
    for (int i=0; i<360 * nbOrbites; i++) {

        const Date j0 = Date(date.getJourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(j0);

        // Longitude
        const Vecteur3D position = sat._position;
        lon = RAD2DEG * Maths::modulo(PI + atan2(position.getY(), position.getX()) -
                                      Observateur::CalculTempsSideralGreenwich(j0), DEUX_PI);
        if (lon < 0.)
            lon += T360;

        // Latitude
        const double r = sqrt(position.getX() * position.getX() + position.getY() * position.getY());
        lat = atan(position.getZ() / r);
        phi = DEUX_PI;
        while (fabs(lat - phi) > 1.e-7) {
            phi = lat;
            const double sph = sin(phi);
            const double ct = 1. / sqrt(1. - E2 * sph * sph);
            lat = atan((position.getZ() + RAYON_TERRESTRE * ct * E2 * sph) / r);
        }
        lat = RAD2DEG * (PI_SUR_DEUX - lat);

        // Position du Soleil
        soleil.CalculPosition(date);

        // Conditions d'eclipse
        sat.CalculSatelliteEclipse(soleil);

        QVector<double> list;
        list << lon << lat << ((sat._eclipse) ? 1. : 0.);
        _traceAuSol.append(list);
    }

    /* Retour */
    return;
}

/*
 * Calcul de la trace dans le ciel
 */
void Satellite::CalculTraceCiel(const Date &date, Observateur observateur)
{
    /* Declarations des variables locales */
    Satellite sat = Satellite(_tle);
    Soleil soleil;

    /* Initialisations */
    bool afin = false;
    int i = 0;
    const double st = 1. / (_tle.getNo() * T360);

    /* Corps de la methode */
    _traceCiel.clear();
    while (!afin) {

        const Date j0 = Date(date.getJourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(j0);

        // Position de l'observateur
        observateur.CalculPosVit(j0);

        // Coordonnees horizontales
        sat.CalculCoordHoriz(observateur);

        if (sat._hauteur >= 0. && i < 360) {

            // Position du Soleil
            soleil.CalculPosition(j0);

            // Conditions d'eclipse
            sat.CalculSatelliteEclipse(soleil);

            QVector<double> list;
            list << sat._hauteur << sat._azimut << ((sat._eclipse) ? 1. : 0.);
            _traceCiel.append(list);

        } else {
            afin = true;
        }
        i++;
    }

    /* Retour */
    return;
}

/* Accesseurs */
bool Satellite::isEclipse() const
{
    return _eclipse;
}

bool Satellite::isIeralt() const
{
    return _ieralt;
}

double Satellite::getAgeTLE() const
{
    return _ageTLE;
}

double Satellite::getElongation() const
{
    return _elongation;
}

double Satellite::getFractionIlluminee() const
{
    return _fractionIlluminee;
}

double Satellite::getMagnitude() const
{
    return _magnitude;
}

double Satellite::getMagnitudeStandard() const
{
    return _magnitudeStandard;
}

char Satellite::getMethMagnitude() const
{
    return _methMagnitude;
}

char Satellite::getMethod() const
{
    return _sat.method;
}

int Satellite::getNbOrbites() const
{
    return _nbOrbites;
}

bool Satellite::isPenombre() const
{
    return _penombre;
}

double Satellite::getRayonApparentSoleil() const
{
    return _rayonApparentSoleil;
}

double Satellite::getRayonApparentTerre() const
{
    return _rayonApparentTerre;
}

double Satellite::getSection() const
{
    return _section;
}

double Satellite::getT1() const
{
    return _t1;
}

double Satellite::getT2() const
{
    return _t2;
}

double Satellite::getT3() const
{
    return _t3;
}

TLE Satellite::getTle() const
{
    return _tle;
}

ElementsOsculateurs Satellite::getElements() const
{
    return _elements;
}

QList<QVector<double> > Satellite::getTraceAuSol() const
{
    return _traceAuSol;
}

QList<QVector<double> > Satellite::getTraceCiel() const
{
    return _traceCiel;
}
