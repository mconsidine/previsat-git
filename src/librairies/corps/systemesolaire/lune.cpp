/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    29 mai 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QObject>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
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
    std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;
    std::array<double, LUNE::NB_PHASES> jjPhases;
    std::array<double, MATHS::DEGRE_INTERPOLATION> ecartAngle;

    /* Initialisations */
    const double annee = date.annee() + (date.mois() - 1) / 12. + date.jour() / 365.;

    /* Corps de la methode */
    for(unsigned int i=0; i<LUNE::NB_PHASES; i++) {

        const double k = arrondi((annee - DATE::AN2000) * 12.3685, 0) + 0.25 * i;

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

                lune.CalculPosition(dateCalcul);
                soleil.CalculPosition(dateCalcul);

                ecartAngle[j] = modulo(lune._lonEcl - soleil.lonEcl() - angle, MATHS::DEUX_PI);
                if (ecartAngle[j] > MATHS::PI) {
                    ecartAngle[j] -= MATHS::DEUX_PI;
                }
            }

            t_evt = Maths::CalculValeurXInterpolation3(jjm, ecartAngle, 0., 1.e-8);
            pas *= 0.5;

            jjm[0] = t_evt - pas;
            jjm[1] = t_evt;
            jjm[2] = t_evt + pas;

            iter++;
        }

        if (iter < MATHS::ITERATIONS_MAX) {
            _datesPhases[i] = Date(dateEvt, date.offsetUTC()).ToQDateTime(1).toString(Qt::ISODate).remove(16, 3).replace(":", "h").replace("T", " ")
                                  .trimmed();
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul des heures de lever, passage au meridien et coucher
 */
void Lune::CalculLeverMeridienCoucher(const Date &date, const Observateur &observateur, const DateSysteme &syst)
{
    /* Declarations des variables locales */
    Lune lune;
    Ephemerides eph;

    /* Initialisations */
    Observateur obs = observateur;
    Date dateCalcul(date.annee(), date.mois(), date.jour() - date.offsetUTC(), date.offsetUTC());
    const Date dateFin(dateCalcul.jourJulienUTC() + 1., date.offsetUTC(), false);
    _ephem.clear();

    /* Corps de la methode */
    do {

        obs.CalculPosVit(dateCalcul);

        lune.CalculPosition(dateCalcul);
        lune.CalculCoordHoriz(obs, true, true, true);

        eph.jourJulienUTC = dateCalcul.jourJulienUTC();
        eph.hauteur = lune.hauteur();
        eph.azimut = lune.azimut();

        _ephem.append(eph);

        dateCalcul = Date(dateCalcul.jourJulienUTC() + DATE::NB_JOUR_PAR_MIN, 0., false);

    } while (dateCalcul.jourJulienUTC() <= dateFin.jourJulienUTC());

    Corps::CalculLeverMeridienCoucher(date, syst, false);

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
    const double distlune = _position.Norme() * SOLEIL::KM2UA;

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
    const double distlune = _position.Norme() * SOLEIL::KM2UA;

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
 * Calcul de la position de la Lune avec le modele simplifie issu de
 * l'Astronomical Algorithms 2nd edition de Jean Meeus, pp337-342
 */
void Lune::CalculPosition(const Date &date)
{
    /* Declarations des variables locales */
    double coef[5];

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

const QString &Lune::phase() const
{
    return _phase;
}

const std::array<QString, LUNE::NB_PHASES> &Lune::datesPhases() const
{
    return _datesPhases;
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
 * Calcul du jour julien approximatif d'une phase lunaire
 */
double Lune::CalculJourJulienPhase(const double k)
{
    const double t = k / 1236.85;
    const double t2 = t * t;
    const double t3 = t2 * t;
    const double t4 = t3 * t;

    return (5.09766 + 29.530588861 * k + 0.00015437 * t2 - 0.000000150 * t3 + 0.00000000073 * t4);
}
