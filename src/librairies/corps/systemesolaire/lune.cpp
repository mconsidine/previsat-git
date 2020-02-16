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
 * >    lune.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position de la Lune
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    30 decembre 2018
 *
 */

#include <QObject>
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "lune.h"
#include "soleil.h"


static const double _tabLon[60] = { 6288774., 1274027., 658314., 213618., -185116., -114332., 58793., 57066., 53322., 45758., -40923.,
                                    -34720., -30383., 15327., -12528., 10980., 10675., 10034., 8548., -7888., -6766., -5163., 4987., 4036.,
                                    3994., 3861., 3665., -2689., -2602., 2390., -2348., 2236., -2120., -2069., 2048., -1773., -1595., 1215.,
                                    -1110., -892., -810., 759., -713., -700., 691., 596., 549., 537., 520., -487., -399., -381., 351., -340.,
                                    330., 327., -323., 299., 294., 0. };

static const double _tabDist[60] = { -20905355., -3699111., -2955968., -569925., 48888., -3149., 246158., -152138., -170733., -204586.,
                                     -129620., 108743., 104755., 10321., 0., 79661., -34782., -23210., -21636., 24208., 30824., -8379.,
                                     -16675., -12831., -10445., -11650., 14403., -7003., 0., 10056., 6322., -9884., 5751., 0., -4950.,
                                     4130., 0., -3958., 0., 3258., 2616., -1897., -2117., 2354., 0., 0., -1423., -1117., -1571., -1739.,
                                     0., -4421., 0., 0., 0., 0., 1165., 0., 0., 8752. };

static const double _tabLat[60] = { 5128122., 280602., 277693., 173237., 55413., 46271., 32573., 17198., 9266., 8822., 8216., 4324., 4200., -3359.,
                                    2463., 2211., 2065., -1870., 1828., -1794., -1749., -1565., -1491., -1475., -1410., -1344., -1335., 1107., 1021.,
                                    833., 777., 671., 607., 596., 491., -451., 439., 422., 421., -366., -351., 331., 315., 302., -283., -229., 223.,
                                    223., -220., -220., -185., 181., -177., 176., 166., -164., 132., -119., 115., 107. };

static const int _tabCoef1[60][4] = { {0, 0, 1, 0}, {2, 0, -1, 0}, {2, 0, 0, 0}, {0, 0, 2, 0}, {0, 1, 0, 0}, {0, 0, 0, 2}, {2, 0, -2, 0},
                                      {2, -1, -1, 0}, {2, 0, 1, 0}, {2, -1, 0, 0}, {0, 1, -1, 0}, {1, 0, 0, 0}, {0, 1, 1, 0}, {2, 0, 0, -2},
                                      {0, 0, 1, 2}, {0, 0, 1, -2}, {4, 0, -1, 0}, {0, 0, 3, 0}, {4, 0, -2, 0}, {2, 1, -1, 0}, {2, 1, 0, 0},
                                      {1, 0, -1, 0}, {1, 1, 0, 0}, {2, -1, 1, 0}, {2, 0, 2, 0}, {4, 0, 0, 0}, {2, 0, -3, 0}, {0, 1, -2, 0},
                                      {2, 0, -1, 2}, {2, -1, -2, 0}, {1, 0, 1, 0}, {2, -2, 0, 0}, {0, 1, 2, 0}, {0, 2, 0, 0}, {2, -2, -1, 0},
                                      {2, 0, 1, -2}, {2, 0, 0, 2}, {4, -1, -1, 0}, {0, 0, 2, 2}, {3, 0, -1, 0}, {2, 1, 1, 0}, {4, -1, -2, 0},
                                      {0, 2, -1, 0}, {2, 2, -1, 0}, {2, 1, -2, 0}, {2, -1, 0, -2}, {4, 0, 1, 0}, {0, 0, 4, 0}, {4, -1, 0, 0},
                                      {1, 0, -2, 0}, {2, 1, 0, -2}, {0, 0, 2, -2}, {1, 1, 1, 0}, {3, 0, -2, 0}, {4, 0, -3, 0}, {2, -1, 2, 0},
                                      {0, 2, 1, 0}, {1, 1, -1, 0}, {2, 0, 3, 0}, {2, 0, -1, -2} };

static const int _tabCoef2[60][4] = { {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, -1}, {2, 0, 0, -1}, {2, 0, -1, 1}, {2, 0, -1, -1}, {2, 0, 0, 1},
                                      {0, 0, 2, 1}, {2, 0, 1, -1}, {0, 0, 2, -1}, {2, -1, 0, -1}, {2, 0, -2, -1}, {2, 0, 1, 1}, {2, 1, 0, -1},
                                      {2, -1, -1, 1}, {2, -1, 0, 1}, {2, -1, -1, -1}, {0, 1, -1, -1}, {4, 0, -1, -1}, {0, 1, 0, 1}, {0, 0, 0, 3},
                                      {0, 1, -1, 1}, {1, 0, 0, 1}, {0, 1, 1, 1}, {0, 1, 1, -1}, {0, 1, 0, -1}, {1, 0, 0, -1}, {0, 0, 3, 1},
                                      {4, 0, 0, -1}, {4, 0, -1, 1}, {0, 0, 1, -3}, {4, 0, -2, 1}, {2, 0, 0, -3}, {2, 0, 2, -1}, {2, -1, 1, -1},
                                      {2, 0, -2, 1}, {0, 0, 3, -1}, {2, 0, 2, 1}, {2, 0, -3, -1}, {2, 1, -1, 1}, {2, 1, 0, 1}, {4, 0, 0, 1},
                                      {2, -1, 1, 1}, {2, -2, 0, -1}, {0, 0, 1, 3}, {2, 1, 1, -1}, {1, 1, 0, -1}, {1, 1, 0, 1}, {0, 1, -2, -1},
                                      {2, 1, -1, -1}, {1, 0, 1, 1}, {2, -1, -2, -1}, {0, 1, 2, 1}, {4, 0, -2, -1}, {4, -1, -1, -1}, {1, 0, 1, -1},
                                      {4, 0, 1, -1}, {1, 0, -1, -1}, {4, -1, 0, -1}, {2, -2, 0, 1} };

static const Vecteur3D w(0., 0., 1.);


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
    _anglePhase = 0.;
    _fractionIlluminee = 0.;
    _magnitude = 99.;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
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


/*
 * Methodes publiques
 */
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
    const double t = date.jourJulienTT() * NB_SIECJ_PAR_JOURS;

    // Longitude moyenne de la Lune
    const double ll = DEG2RAD * modulo(218.3164477 + t * (481267.88123421 - t * (0.0015786 + t * ((1. / 538841.) - t * (1. / 65194000.)))), T360);

    // Elongation moyenne de la Lune
    coef[0] = DEG2RAD * modulo(297.8501921 + t * (445267.1114034 - t * (0.0018819 + t * ((1. / 545868.) - t * (1. / 113065000.)))), T360);

    // Anomalie moyenne du Soleil
    coef[1] = DEG2RAD * modulo(357.5291092 + t * (35999.0502909 - t * (0.0001536 + t * (1. / 24490000.))), T360);

    // Anomalie moyenne de la Lune
    coef[2] = DEG2RAD * modulo(134.9633964 + t * (477198.8675055 + t * (0.0087414 + t * ((1. / 69699.) - t * (1. / 14712000.)))), T360);

    // Argument de latitude de la Lune
    coef[3] = DEG2RAD * modulo(93.272095 + t * (483202.0175233 - t * (0.0036539 - t * ((1. / 3526000.) + t * (1. / 863310000.)))), T360);

    coef[4] = 1. - t * (0.002516 + 0.0000074 * t);

    /* Corps de la methode */
    for (int i=0; i<60; i++) {

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
    const double a1 = DEG2RAD * modulo(119.75 + 131.849 * t, T360);
    const double a2 = DEG2RAD * modulo(53.09 + 479264.29 * t, T360);
    const double a3 = DEG2RAD * modulo(313.45 + 481266.484 * t, T360);
    l0 += 3958. * sin(a1) + 1962. * sin(ll - coef[3]) + 318. * sin(a2);
    b0 += -2235. * sin(ll) + 382. * sin(a3) + 175. * (sin(a1 - coef[3]) + sin(a1 + coef[3])) + 127. * sin(ll - coef[2]) - 115. * sin(ll + coef[2]);

    // Coordonnees ecliptiques en repere spherique
    const double temp = DEG2RAD * 1.e-6;
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
 * Calcul de la phase de la Lune
 */
void Lune::CalculPhase(const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double distlune = _position.Norme() * KM2UA;

    /* Corps de la methode */
    const bool sgns = ((soleil.position() ^ _position) * w > 0.);

    // Angle de phase
    const double cospsi = cos(_latEcl) * cos(_lonEcl - soleil.lonEcl());
    _anglePhase = fmod(atan(soleil.distanceUA() * sqrt(1. - cospsi * cospsi) / (distlune - soleil.distanceUA() * cospsi)), PI);
    if (_anglePhase < 0.) {
        _anglePhase += PI;
    }

    // Fraction illuminee
    _fractionIlluminee = 0.5 * (1. + cos(_anglePhase));

    // Phase
    if ((_fractionIlluminee >= 0.) && (_fractionIlluminee < 0.03)) {
        _phase = QObject::tr("Nouvelle Lune");
    }

    if ((_fractionIlluminee >= 0.03) && (_fractionIlluminee < 0.31)) {
        _phase = (sgns) ? QObject::tr("Premier croissant") : QObject::tr("Dernier croissant");
    }

    if ((_fractionIlluminee >= 0.31) && (_fractionIlluminee < 0.69)) {
        _phase = (sgns) ? QObject::tr("Premier quartier") : QObject::tr("Dernier quartier");
    }

    if ((_fractionIlluminee >= 0.69) && (_fractionIlluminee < 0.97)) {
        _phase = (sgns) ? QObject::tr("Gibbeuse croissante") : QObject::tr("Gibbeuse décroissante");
    }

    if (_fractionIlluminee >= 0.97) {
        _phase = QObject::tr("Pleine Lune");
    }

    /* Retour */
    return;
}

void Lune::CalculMagnitude(const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double cosang = cos(_anglePhase);
    const double angs2 = 0.5 * _anglePhase;
    const double tanAngs2 = tan(angs2);
    const double b = B0 / (1. + tanAngs2 / H);

    const double z = tan(THETA) * tanAngs2;
    double fCorr = 1.;
    if (_anglePhase < 1.45) {
        fCorr = (((((0.4619942495 * _anglePhase - 1.9799023103) * _anglePhase + 3.2706222793) * _anglePhase - 2.3757732575) * _anglePhase +
                  0.7066275224) * _anglePhase - 0.2148362906) * _anglePhase + 1.0037993467;

    } else if ((_anglePhase >= 1.45) && (_anglePhase < 2.4)) {
        fCorr = (((((62.0366249494 * _anglePhase - 685.3561374672) * _anglePhase + 3125.5094770692) * _anglePhase - 7538.4095328747) * _anglePhase +
                  10151.6795059045) * _anglePhase - 7242.8081201613) * _anglePhase + 2140.9898221829;

    } else if ((_anglePhase >= 2.4) && (_anglePhase < 2.88)) {
        fCorr = (((((-1.2780230784 * _anglePhase + 20.9856806508) * _anglePhase - 139.5520707759) * _anglePhase + 474.9063054183) * _anglePhase -
                  851.1923961614) * _anglePhase + 720.4059560648) * _anglePhase - 187.0773024225;

    } else {
        fCorr = (((((194.303476382 * _anglePhase - 3472.9895343356) * _anglePhase + 25865.444540608) * _anglePhase - 102742.044141574) * _anglePhase +
                  229573.277184166) * _anglePhase - 273606.243897778) * _anglePhase + 135882.646837775;
    }

    const double kappa = exp(-THETA * (0.32 * sqrt(z) + 0.52 * z)) * fCorr;

    const double p = W0S8 * ((1. + B0) * P0 - 1.) + R0S2 + R2 * (1. / 6.);
    const double ppi = 1. + 0.29 * cosang + 0.39 * (1.5 * cosang * cosang - 0.5);

    const double phi = (W0S8 * ((1. + b) * ppi - 1.) + R0S2 * (1. - R0) * (1. + sin(angs2) * tanAngs2 * log(tan(0.5 * angs2))) +
                        DEUX_TIERS * R2 / PI * (sin(_anglePhase) + (PI - _anglePhase) * cosang)) / p;

    const double dm = kappa * phi;
    const double distlune = _position.Norme() * KM2UA;

    _magnitude = 0.21 + 5. * log10(distlune * soleil.distanceUA()) - 2.5 * log10(dm);

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

