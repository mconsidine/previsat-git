/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    lune.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    22 mars 2025
 *
 */

#include <QObject>
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "lune.h"
#include "soleil.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Lune::Lune()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _luneCroissante = false;
    _anglePhase = 0.;
    _fractionIlluminee = 0.;
    _magnitude = CORPS::MAGNITUDE_INDEFINIE;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des dates des prochaines eclipses de Lune et de Soleil
 */
void Lune::CalculDatesEclipses(const Date &date)
{
    /* Declarations des variables locales */
    bool atrouve;
    double jj;
    double k;

    /* Initialisations */
    const double annee = date.annee() + (date.mois() - 1) / 12. + date.jour() / 365.;

    /* Corps de la methode */
    for(unsigned int phase=0; phase<=2; phase+=2) {

        k = CalculIndicePhase(annee, phase) - 1;

        atrouve = false;
        while (!atrouve) {

            jj = CalculJourJulienPhase(k);
            const double t = k / 1236.85;
            const double t2 = t * t;

            // Argument de latitude de la Lune
            const double f = MATHS::DEG2RAD * modulo(160.7108 + 390.67050284 * k + t2 * (-0.0016118 + t * (-2.27e-6 + 1.1e-8 * t)), MATHS::T360);

            if (fabs(sin(f)) < 0.36) {

                // Calcul des elements communs aux 2 types d'eclipse (Soleil et Lune)
                CalculElementsEclipses(k, f, phase);
                jj += _dj;

                if (jj >= date.jourJulienTT()) {

                    const QString dateEclipse = Date(jj, date.offsetUTC()).ToQDateTime(DateFormatSec::FORMAT_SEC).toString(Qt::ISODate).remove(16, 3)
                                                    .replace(":", "h").replace("T", " ").trimmed();

                    // Calcul des caracteristiques de l'eclipse
                    atrouve = CalculCaracteristiquesEclipses(phase, dateEclipse, k);

                } else {
                    k += 1.;
                }
            } else {
                k += 1.;
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul des dates des phases lunaires
 */
void Lune::CalculDatesPhases(const Date &date)
{
    /* Declarations des variables locales */
    unsigned int iter;
    double dateEvt;
    double pas;
    double t_evt;
    Lune lune;
    Soleil soleil;
    std::array<double, LUNE::NB_PHASES> jjPhases;
    std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;
    std::array<double, MATHS::DEGRE_INTERPOLATION> ecartAngle;
    std::array<QPointF, MATHS::DEGRE_INTERPOLATION> tab;

    /* Initialisations */
    const double annee = date.annee() + (date.mois() - 1) / 12. + date.jour() / 365.;

    /* Corps de la methode */
    for(unsigned int i=0; i<LUNE::NB_PHASES; i++) {

        const double k = CalculIndicePhase(annee, i);

        // Dates approximatives des phases lunaires
        jjPhases[i] = CalculJourJulienPhase(k);
        const double jj1 = CalculJourJulienPhase(k-1);
        const double jj2 = CalculJourJulienPhase(k+1);

        if (jj1 > date.jourJulienUTC()) {
            jjPhases[i] = jj1;
        } else if (jjPhases[i] < date.jourJulienUTC()) {
            jjPhases[i] = jj2;
        }

        // Obtention des dates precises par interpolation
        pas = 1.;
        jjm[0] = jjPhases[i] - pas;
        jjm[1] = jjPhases[i];
        jjm[2] = jjPhases[i] + pas;

        dateEvt = 0.;
        t_evt = jjPhases[i];
        const double angle = MATHS::PI_SUR_DEUX * i;

        iter = 0;
        while ((fabs(dateEvt - t_evt) > DATE::EPS_DATES) && (iter < MATHS::ITERATIONS_MAX)) {

            dateEvt = t_evt;

            for(unsigned int j=0; j<MATHS::DEGRE_INTERPOLATION; j++) {

                const Date dateCalcul(jjm.at(j), 0., false);

                lune.CalculPositionSimp(dateCalcul);
                soleil.CalculPositionSimp(dateCalcul);

                ecartAngle[j] = modulo(lune._lonEcl - soleil.lonEcl() - angle, MATHS::DEUX_PI);
                if (ecartAngle[j] > MATHS::PI) {
                    ecartAngle[j] -= MATHS::DEUX_PI;
                }

                tab[j] = QPointF(jjm[j], ecartAngle[j]);
            }

            t_evt = Maths::CalculValeurXInterpolation3(tab, 0., 1.e-8);
            pas *= 0.5;

            jjm[0] = t_evt - pas;
            jjm[1] = t_evt;
            jjm[2] = t_evt + pas;

            iter++;
        }

        if (iter < MATHS::ITERATIONS_MAX) {
            _datesPhases[i] = Date(dateEvt, date.offsetUTC()).ToQDateTime(DateFormatSec::FORMAT_SEC).toString(Qt::ISODate).remove(16, 3)
            .replace(":", "h").replace("T", " ").trimmed();
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul de la magnitude visuelle de la Lune
 */
void Lune::CalculMagnitude(const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double kappa = 1.;
    const double ang = _anglePhase * MATHS::RAD2DEG;
    const double cosi = cos(_anglePhase);
    const double cos2i = cosi * cosi;
    const double sini2 = sin(0.5 * _anglePhase);
    const double tani2 = tan(0.5 * _anglePhase);

    /* Corps de la methode */
    if (ang < 80.) {
        kappa = Maths::InterpolationLagrange(_tableKappa1, ang);
    } else if (ang < 140.) {
        kappa = Maths::InterpolationLagrange(_tableKappa2, ang);
    } else if (ang < 160.) {
        kappa = Maths::InterpolationLagrange(_tableKappa3, ang);
    } else {
        kappa = Maths::InterpolationLagrange(_tableKappa4, ang);
    }

    const double b = LUNE::B0 / (1. + tani2 / LUNE::H);
    const double p = 1. + LUNE::B * cosi + LUNE::C * (1.5 * cos2i - 0.5);

    const double tmp1 = LUNE::W0S8 * ((1. + b) * p - 1.);
    const double tmp2 = LUNE::R0S2 * (1. - LUNE::R0) * (1. + sini2 * tani2 * log(tan(0.25 * _anglePhase)));
    const double tmp3 = 4. * LUNE::R2S6 * (sin(_anglePhase) + (MATHS::PI - _anglePhase) * cosi) / MATHS::PI;
    const double phi = (tmp1 + tmp2 + tmp3) / LUNE::P;

    const double dm = kappa * phi;
    const double distlune = _position.Norme() * CORPS::KM2UA;

    _magnitude = LUNE::W0 + 5. * log10(distlune * soleil.distanceUA()) - 2.5 * log10(dm);

    /* Retour */
    return;
}

/*
 * Calcul de la phase actuelle de la Lune
 */
void Lune::CalculPhase(const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double distlune = _position.Norme() * CORPS::KM2UA;

    /* Corps de la methode */
    // Determination si la lune est croissante
    _luneCroissante = ((soleil.position() ^ _position) * _w > 0.);

    // Angle de phase
    const double cospsi = cos(_latEcl) * cos(_lonEcl - soleil.lonEcl());
    _anglePhase = fmod(atan(soleil.distanceUA() * sqrt(1. - cospsi * cospsi) / (distlune - soleil.distanceUA() * cospsi)), MATHS::PI);
    if (_anglePhase < 0.) {
        _anglePhase += MATHS::PI;
    }

    // Fraction illuminee
    _fractionIlluminee = 0.5 * (1. + cos(_anglePhase));

    // Phase
    if ((_fractionIlluminee >= 0.) && (_fractionIlluminee < 0.03)) {
        _phase = QObject::tr("Nouvelle Lune");
    }

    if ((_fractionIlluminee >= 0.03) && (_fractionIlluminee < 0.31)) {
        _phase = (_luneCroissante) ? QObject::tr("Premier croissant") : QObject::tr("Dernier croissant");
    }

    if ((_fractionIlluminee >= 0.31) && (_fractionIlluminee < 0.69)) {
        _phase = (_luneCroissante) ? QObject::tr("Premier quartier") : QObject::tr("Dernier quartier");
    }

    if ((_fractionIlluminee >= 0.69) && (_fractionIlluminee < 0.97)) {
        _phase = (_luneCroissante) ? QObject::tr("Gibbeuse croissante") : QObject::tr("Gibbeuse décroissante");
    }

    if (_fractionIlluminee >= 0.97) {
        _phase = QObject::tr("Pleine Lune");
    }

    /* Retour */
    return;
}

/*
 * Calcul simplifie de la position de la Lune
 */
void Lune::CalculPositionSimp(const Date &date)
{
    /* Declarations des variables locales */
    std::array<double, 5> coef;

    /* Initialisations */
    double b0 = 0.;
    double l0 = 0.;
    double r0 = 0.;
    const double t = date.jourJulienTT() * DATE::NB_SIECJ_PAR_JOURS;

    // Longitude moyenne de la Lune
    const double ll = MATHS::DEG2RAD * modulo(218.3164477 + t * (481267.88123421 - t * (0.0015786 + t * ((1. / 538841.) - t * (1. / 65194000.)))),
                                              MATHS::T360);

    // Elongation moyenne de la Lune
    coef[0] = MATHS::DEG2RAD * modulo(297.8501921 + t * (445267.1114034 - t * (0.0018819 + t * ((1. / 545868.) - t * (1. / 113065000.)))), MATHS::T360);

    // Anomalie moyenne du Soleil
    coef[1] = MATHS::DEG2RAD * modulo(357.5291092 + t * (35999.0502909 - t * (0.0001536 + t * (1. / 24490000.))), MATHS::T360);

    // Anomalie moyenne de la Lune
    coef[2] = MATHS::DEG2RAD * modulo(134.9633964 + t * (477198.8675055 + t * (0.0087414 + t * ((1. / 69699.) - t * (1. / 14712000.)))), MATHS::T360);

    // Argument de latitude de la Lune
    coef[3] = MATHS::DEG2RAD * modulo(93.272095 + t * (483202.0175233 - t * (0.0036539 - t * ((1. / 3526000.) + t * (1. / 863310000.)))), MATHS::T360);

    coef[4] = 1. - t * (0.002516 + 0.0000074 * t);

    /* Corps de la methode */
    for (unsigned int i=0; i<LUNE::NB_TERMES_PERIODIQUES; i++) {

        double ang1 = 0.;
        double ang2 = 0.;

        for (int j=0; j<4; j++) {
            ang1 += coef[j] * _tabCoef1[i][j];
            ang2 += coef[j] * _tabCoef2[i][j];
        }

        const double fact1 = (_tabCoef1[i][1] == 0) ? 1. : pow(coef[4], abs(_tabCoef1[i][1]));
        const double fact2 = (_tabCoef2[i][1] == 0) ? 1. : pow(coef[4], abs(_tabCoef2[i][1]));

        // Termes en longitude
        l0 += _tabLon[i] * fact1 * sin(ang1);

        // Termes en distance
        r0 += _tabDist[i] * fact1 * cos(ang1);

        // Termes en latitude
        b0 += _tabLat[i] * fact2 * sin(ang2);
    }

    // Principaux termes planetaires
    const double a1 = MATHS::DEG2RAD * modulo(119.75 + 131.849 * t, MATHS::T360);
    const double a2 = MATHS::DEG2RAD * modulo(53.09 + 479264.29 * t, MATHS::T360);
    const double a3 = MATHS::DEG2RAD * modulo(313.45 + 481266.484 * t, MATHS::T360);
    l0 += 3958. * sin(a1) + 1962. * sin(ll - coef[3]) + 318. * sin(a2);
    b0 += -2235. * sin(ll) + 382. * sin(a3) + 175. * (sin(a1 - coef[3]) + sin(a1 + coef[3])) + 127. * sin(ll - coef[2]) - 115. * sin(ll + coef[2]);

    // Coordonnees ecliptiques en repere spherique
    const double temp = MATHS::DEG2RAD * 1.e-6;
    _lonEcl = ll + temp * l0;
    _latEcl = temp * b0;
    const double rp = 385000.56 + r0 * 1.e-3;
    const Vecteur3D pos(_lonEcl, _latEcl, rp);

    // Position cartesienne equatoriale
    _position = Sph2Cart(pos, date);

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
bool Lune::luneCroissante() const
{
    return _luneCroissante;
}

double Lune::anglePhase() const
{
    return _anglePhase;
}

double Lune::fractionIlluminee() const
{
    return _fractionIlluminee;
}

double Lune::magnitude() const
{
    return _magnitude;
}

QString Lune::phase() const
{
    return _phase;
}

std::array<QString, LUNE::NB_PHASES> Lune::datesPhases() const
{
    return _datesPhases;
}

QString Lune::dateEclipseSoleil() const
{
    return _dateEclipseSoleil;
}

QString Lune::typeEclipseSoleil() const
{
    return _typeEclipseSoleil;
}

QString Lune::tooltipEclipseSoleil() const
{
    return _tooltipEclipseSoleil;
}

QString Lune::dateEclipseLune() const
{
    return _dateEclipseLune;
}

QString Lune::typeEclipseLune() const
{
    return _typeEclipseLune;
}

QString Lune::tooltipEclipseLune() const
{
    return _tooltipEclipseLune;
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
 * Calcul des caracteristiques des eclipses de Soleil et de Lune
 */
bool Lune::CalculCaracteristiquesEclipses(const unsigned int phase,
                                          const QString &dateEclipse,
                                          double &k)
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool atrouve = false;

    /* Corps de la methode */
    const double g = fabs(_gamma);

    if (phase == 0) {

        // Eclipse de Soleil

        // Condition d'eclipse certaine
        if (g <= 1.5433 + _u) {

            atrouve = true;
            _dateEclipseSoleil = dateEclipse;

            if (g > 0.9972) {

                // Eclipse non centrale
                _typeEclipseSoleil = QObject::tr("P", "Partial Solar eclipse");
                _tooltipEclipseSoleil = QObject::tr("Partielle", "Partial Solar eclipse");

                if (g <= 0.9972 + fabs(_u)) {
                    _typeEclipseSoleil = QObject::tr("A/T", "Annular or Total Solar eclipse");
                    _tooltipEclipseSoleil = QObject::tr("Annulaire ou totale", "Annular or Total Solar eclipse");
                }

            } else {

                // Eclipse centrale
                if (_u < 0) {
                    _typeEclipseSoleil = QObject::tr("T", "Total Solar eclipse");
                    _tooltipEclipseSoleil = QObject::tr("Total", "Total Solar eclipse");

                } else if (_u > 0.0047) {
                    _typeEclipseSoleil = QObject::tr("A", "Annular Solar eclipse");
                    _tooltipEclipseSoleil = QObject::tr("Annulaire", "Annular Solar eclipse");

                } else {

                    const double ww = 0.00464 * sqrt(1. - _gamma * _gamma);
                    if (_u < ww) {
                        _typeEclipseSoleil = QObject::tr("A-T", "Annular-total Solar eclipse");
                        _tooltipEclipseSoleil = QObject::tr("Annulaire-totale", "Annular-total Solar eclipse");
                    } else {
                        _typeEclipseSoleil = QObject::tr("A", "Annular Solar eclipse");
                        _tooltipEclipseSoleil = QObject::tr("Annulaire", "Annular Solar eclipse");
                    }
                }
            }
        } else {
            k += 1.;
        }
    } else {

        // Eclipse de Lune

        // Magnitude de la penombre
        const double magPen = (1.5573 + _u - g) / 0.5450;

        if (magPen > 0.) {

            atrouve = true;
            _dateEclipseLune = dateEclipse;

            _typeEclipseLune = QObject::tr("Pen", "Lunar eclipse by the penumbra");
            _tooltipEclipseLune = QObject::tr("Pénombre", "Lunar eclipse by the penumbra");

            // Magnitude de l'ombre
            const double magOmb = (1.0128 - _u - g) / 0.5450;
            if (magOmb > 0.) {

                _typeEclipseLune = QObject::tr("P", "Lunar eclipse by the shadow (partial)");
                _tooltipEclipseLune = QObject::tr("Partielle", "Lunar eclipse by the shadow (partial)");

                if (magOmb > 1.) {
                    _typeEclipseLune = QObject::tr("T", "Lunar eclipse by the shadow (total)");
                    _tooltipEclipseLune = QObject::tr("Totale", "Lunar eclipse by the shadow (total)");
                }
            }
        } else {
            k += 1.;
        }
    }

    /* Retour */
    return atrouve;
}

/*
 * Calcul des elements communs aux 2 types d'eclipse (Soleil et Lune)
 */
void Lune::CalculElementsEclipses(const double k,
                                  const double f,
                                  unsigned int phase)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double t = k / 1236.85;
    const double t2 = t * t;

    /* Corps de la methode */
    const double omega = MATHS::DEG2RAD * modulo(124.7746 - 1.56375588 * k + t2 * (0.0020672 + 2.15e-6 * t), MATHS::T360);
    const double f1 = f - 0.02665 * MATHS::DEG2RAD * sin(omega);
    const double a1 = MATHS::DEG2RAD * modulo(299.77 + 0.107408 * k - 0.009173 * t2, MATHS::T360);

    const double m = MATHS::DEG2RAD * modulo(2.554 + 29.10535670 * k - t2 * (1.4e-6 + 1.1e-7 * t), MATHS::T360);
    const double mm = MATHS::DEG2RAD * modulo(201.5643 + 385.81693528 * k + t2 * (0.0107582 + t * (1.238e-5 - 5.8e-8 * t)), MATHS::T360);
    const double e = 1. - t * (0.002516 + 0.0000074 * t);

    const double tmp1 = 2. * f1;
    const double tmp2 = 2. * mm;

    _dj = 0.0161 * sin(tmp2)
          - 0.0097 * sin(tmp1)
          + 0.0073 * e * sin(mm - m)
          - 0.0050 * e * sin(mm + m)
          - 0.0023 * sin(mm - tmp1)
          + 0.0021 * e * sin(2. * m)
          + 0.0012 * sin(mm + tmp1)
          + 0.0006 * e * sin(tmp2 + m)
          - 0.0004 * sin(3. * mm)
          - 0.0003 * e * sin(m + tmp1)
          + 0.0003 * sin(a1)
          - 0.0002 * e * sin(m - tmp1)
          - 0.0002 * e * sin(tmp2 - m)
          - 0.0002 * sin(omega);

    if (phase == 0) {
        _dj += -0.4075 * sin(mm) + 0.1721 * e * sin(m);
    } else {
        _dj += -0.4065 * sin(mm) + 0.1727 * e * sin(m);
    }

    _p = 0.2070 * e * sin(m)
         + 0.0024 * e * sin(2. * m)
         - 0.0392 * sin(mm)
         + 0.0116 * sin(tmp2)
         - 0.0073 * e * sin(mm + m)
         + 0.0067 * e * sin(mm - m)
         + 0.0118 * sin(tmp1);

    _q = 5.2207
         - 0.0048 * e * cos(m)
         + 0.0020 * e * cos(2. * m)
         - 0.3299 * cos(mm)
         - 0.0060 * e * cos(mm + m)
         + 0.0041 * e * cos(mm - m);

    const double cf1 = cos(f1);
    _gamma = (_p * cf1 + _q * sin(f1)) * (1. - 0.0048 * fabs(cf1));

    _u = 0.0059
         + 0.0056 * e * cos(m)
         - 0.0182 * cos(mm)
         + 0.0004 * cos(tmp2)
         - 0.0005 * cos(m + mm);

    /* Retour */
    return;
}

/*
 * Calcul des ephemerides de la Lune pour determiner les heures de lever/meriden/coucher
 */
void Lune::CalculEphemLeverMeridienCoucher(const Date &date,
                                           const Observateur &observateur)
{
    /* Declarations des variables locales */
    Lune lune;
    Ephemerides eph;

    /* Initialisations */
    Observateur obs = observateur;
    Date dateCalcul(floor(date.jourJulien() + 0.5) - 0.5 - date.offsetUTC(), date.offsetUTC());
    const Date dateFin(dateCalcul.jourJulienUTC() + 1., date.offsetUTC(), false);
    _ephem.clear();

    /* Corps de la methode */
    do {

        obs.CalculPosVit(dateCalcul);

        lune.CalculPositionSimp(dateCalcul);
        lune.CalculCoordHoriz(obs, true, true, true);

        eph.jourJulienUTC = dateCalcul.jourJulienUTC();
        eph.hauteur = lune._hauteur;
        eph.azimut = lune._azimut;

        _ephem.append(eph);

        dateCalcul = Date(dateCalcul.jourJulienUTC() + DATE::NB_JOUR_PAR_MIN, 0., false);

    } while (dateCalcul.jourJulienUTC() <= dateFin.jourJulienUTC());

    /* Retour */
    return;
}

/*
 * Calcul de l'indice de la Nouvelle Lune (0 pour la premiere Nouvelle Lune de 2000)
 */
double Lune::CalculIndicePhase(const double annee,
                               const unsigned int phase)
{
    return arrondi((annee - DATE::AN2000) * 12.3685, 0) + 0.25 * phase;
}

/*
 * Calcul du jour julien approximatif d'une phase lunaire
 */
double Lune::CalculJourJulienPhase(const double k)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double t = k / 1236.85;
    const double t2 = t * t;

    /* Corps de la methode */

    /* Retour */
    return (5.09766 + 29.530588861 * k + t2 * (0.00015437 + t * (-0.000000150 + 0.00000000073 * t)));
}
