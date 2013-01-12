/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <QObject>
#include "lune.h"
#include "librairies/maths/maths.h"

static const double _tabLon[60] = {6288774., 1274027., 658314., 213618., -185116., -114332., 58793., 57066., 53322., 45758.,
                                   -40923., -34720., -30383., 15327., -12528., 10980., 10675., 10034., 8548., -7888., -6766.,
                                   -5163., 4987., 4036., 3994., 3861., 3665., -2689., -2602., 2390., -2348., 2236., -2120.,
                                   -2069., 2048., -1773., -1595., 1215., -1110., -892., -810., 759., -713., -700., 691.,
                                   596., 549., 537., 520., -487., -399., -381., 351., -340., 330., 327., -323., 299., 294., 0.};

static const double _tabDist[60] = {-20905355., -3699111., -2955968., -569925., 48888., -3149., 246158., -152138., -170733.,
                                    -204586., -129620., 108743., 104755., 10321., 0., 79661., -34782., -23210., -21636.,
                                    24208., 30824., -8379., -16675., -12831., -10445., -11650., 14403., -7003., 0., 10056.,
                                    6322., -9884., 5751., 0., -4950., 4130., 0., -3958., 0., 3258., 2616., -1897., -2117.,
                                    2354., 0., 0., -1423., -1117., -1571., -1739., 0., -4421., 0., 0., 0., 0., 1165., 0., 0.,
                                    8752.};

static const double _tabLat[60] = {5128122., 280602., 277693., 173237., 55413., 46271., 32573., 17198., 9266., 8822., 8216.,
                                   4324., 4200., -3359., 2463., 2211., 2065., -1870., 1828., -1794., -1749., -1565., -1491.,
                                   -1475., -1410., -1344., -1335., 1107., 1021., 833., 777., 671., 607., 596., 491., -451.,
                                   439., 422., 421., -366., -351., 331., 315., 302., -283., -229., 223., 223., -220., -220.,
                                   -185., 181., -177., 176., 166., -164., 132., -119., 115., 107.};

static const int _tabCoef1[60][4] = {{0, 0, 1, 0}, {2, 0, -1, 0}, {2, 0, 0, 0}, {0, 0, 2, 0}, {0, 1, 0, 0}, {0, 0, 0, 2},
                                     {2, 0, -2, 0}, {2, -1, -1, 0}, {2, 0, 1, 0}, {2, -1, 0, 0}, {0, 1, -1, 0}, {1, 0, 0, 0},
                                     {0, 1, 1, 0}, {2, 0, 0, -2}, {0, 0, 1, 2}, {0, 0, 1, -2}, {4, 0, -1, 0}, {0, 0, 3, 0},
                                     {4, 0, -2, 0}, {2, 1, -1, 0}, {2, 1, 0, 0}, {1, 0, -1, 0}, {1, 1, 0, 0}, {2, -1, 1, 0},
                                     {2, 0, 2, 0}, {4, 0, 0, 0}, {2, 0, -3, 0}, {0, 1, -2, 0}, {2, 0, -1, 2}, {2, -1, -2, 0},
                                     {1, 0, 1, 0}, {2, -2, 0, 0}, {0, 1, 2, 0}, {0, 2, 0, 0}, {2, -2, -1, 0}, {2, 0, 1, -2},
                                     {2, 0, 0, 2}, {4, -1, -1, 0}, {0, 0, 2, 2}, {3, 0, -1, 0}, {2, 1, 1, 0}, {4, -1, -2, 0},
                                     {0, 2, -1, 0}, {2, 2, -1, 0}, {2, 1, -2, 0}, {2, -1, 0, -2}, {4, 0, 1, 0}, {0, 0, 4, 0},
                                     {4, -1, 0, 0}, {1, 0, -2, 0}, {2, 1, 0, -2}, {0, 0, 2, -2}, {1, 1, 1, 0}, {3, 0, -2, 0},
                                     {4, 0, -3, 0}, {2, -1, 2, 0}, {0, 2, 1, 0}, {1, 1, -1, 0}, {2, 0, 3, 0}, {2, 0, -1, -2}};

static const int _tabCoef2[60][4] = {{0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, -1}, {2, 0, 0, -1}, {2, 0, -1, 1}, {2, 0, -1, -1},
                                     {2, 0, 0, 1}, {0, 0, 2, 1}, {2, 0, 1, -1}, {0, 0, 2, -1}, {2, -1, 0, -1}, {2, 0, -2, -1},
                                     {2, 0, 1, 1}, {2, 1, 0, -1}, {2, -1, -1, 1}, {2, -1, 0, 1}, {2, -1, -1, -1},
                                     {0, 1, -1, -1}, {4, 0, -1, -1}, {0, 1, 0, 1}, {0, 0, 0, 3}, {0, 1, -1, 1}, {1, 0, 0, 1},
                                     {0, 1, 1, 1}, {0, 1, 1, -1}, {0, 1, 0, -1}, {1, 0, 0, -1}, {0, 0, 3, 1}, {4, 0, 0, -1},
                                     {4, 0, -1, 1}, {0, 0, 1, -3}, {4, 0, -2, 1}, {2, 0, 0, -3}, {2, 0, 2, -1},
                                     {2, -1, 1, -1}, {2, 0, -2, 1}, {0, 0, 3, -1}, {2, 0, 2, 1}, {2, 0, -3, -1},
                                     {2, 1, -1, 1}, {2, 1, 0, 1}, {4, 0, 0, 1}, {2, -1, 1, 1}, {2, -2, 0, -1}, {0, 0, 1, 3},
                                     {2, 1, 1, -1}, {1, 1, 0, -1}, {1, 1, 0, 1}, {0, 1, -2, -1}, {2, 1, -1, -1}, {1, 0, 1, 1},
                                     {2, -1, -2, -1}, {0, 1, 2, 1}, {4, 0, -2, -1}, {4, -1, -1, -1}, {1, 0, 1, -1},
                                     {4, 0, 1, -1}, {1, 0, -1, -1}, {4, -1, 0, -1}, {2, -2, 0, 1}};

static const Vecteur3D w(0., 0., 1.);

/* Constructeurs */
Lune::Lune()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _fractionIlluminee = 0.;

    /* Retour */
    return;
}

/* Destructeur */
Lune::~Lune()
{
}

/*
 * Calcul de la position de la Lune avec le modele simplifie issu de
 * l'Astronomical Algorithms 2nd edition de Jean Meeus, pp337-342
 */
void Lune::CalculPosition(const Date &date)
{
    /* Declarations des variables locales */
    double ang1, ang2, b0, fact1, fact2, l0, r0;
    double coef[5];

    /* Initialisations */
    b0 = 0.;
    l0 = 0.;
    r0 = 0.;
    const double t = date.getJourJulienUTC() * NB_SIECJ_PAR_JOURS;
    const double t2 = t * t;
    const double t3 = t2 * t;
    const double t4 = t3 * t;

    // Longitude moyenne de la Lune
    const double ll = DEG2RAD * Maths::modulo(218.3164477 + 481267.88123421 * t - 0.0015786 * t2 + t3 / 538841. -
                                              t4 / 65194000., T360);

    // Elongation moyenne de la Lune
    coef[0] = DEG2RAD * Maths::modulo(297.8501921 + 445267.1114034 * t - 0.0018819 * t2 + t3 / 545868. -
                                      t4 / 113065000., T360);

    // Anomalie moyenne du Soleil
    coef[1] = DEG2RAD * Maths::modulo(357.5291092 + 35999.0502909 * t - 0.0001536 * t2 + t3 / 24490000., T360);

    // Anomalie moyenne de la Lune
    coef[2] = DEG2RAD * Maths::modulo(134.9633964 + 477198.8675055 * t + 0.0087414 * t2 + t3 / 69699. -
                                      t4 / 14712000., T360);

    // Argument de latitude de la Lune
    coef[3] = DEG2RAD * Maths::modulo(93.272095 + 483202.0175233 * t - 0.0036539 * t2 - t3 / 3526000. +
                                      t4 / 863310000., T360);

    coef[4] = 1. - 0.002516 * t - 0.0000074 * t2;

    /* Corps de la methode */
    for (int i=0; i<60; i++) {

        ang1 = 0.;
        ang2 = 0.;
        fact1 = 1.;
        fact2 = 1.;

        for (int j=0; j<4; j++) {
            ang1 += coef[j] * _tabCoef1[i][j];
            ang2 += coef[j] * _tabCoef2[i][j];
        }
        if (_tabCoef1[i][1] != 0)
            fact1 = pow(coef[4], fabs(_tabCoef1[i][1]));
        if (_tabCoef2[i][1] != 0)
            fact2 = pow(coef[4], fabs(_tabCoef2[i][1]));

        // Termes en longitude
        l0 += _tabLon[i] * fact1 * sin(ang1);

        // Termes en distance
        r0 += _tabDist[i] * fact1 * cos(ang1);

        // Termes en latitude
        b0 += _tabLat[i] * fact2 * sin(ang2);
    }

    // Principaux termes planetaires
    const double a1 = DEG2RAD * Maths::modulo(119.75 + 131.849 * t, T360);
    const double a2 = DEG2RAD * Maths::modulo(53.09 + 479264.29 * t, T360);
    const double a3 = DEG2RAD * Maths::modulo(313.45 + 481266.484 * t, T360);
    l0 += 3958. * sin(a1) + 1962. * sin(ll - coef[3]) + 318. * sin(a2);
    b0 += -2235. * sin(ll) + 382. * sin(a3) + 175. * (sin(a1 - coef[3]) + sin(a1 + coef[3])) + 127. * sin(ll - coef[2]) -
            115. * sin(ll + coef[2]);

    // Coordonnees ecliptiques en repere spherique
    const double lv = ll + DEG2RAD * l0 * 1.e-6;
    const double bt = DEG2RAD * b0 * 1.e-6;
    const double rp = 385000.56 + r0 * 1.e-3;
    Vecteur3D pos(lv, bt, rp);

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

    /* Corps de la methode */
    // Elongation (ou angle de phase)
    const double elongation = soleil.getPosition().Angle(-_position);

    const bool sgn = ((soleil.getPosition() ^ _position) * w > 0.) ? true : false;

    // Fraction illuminee
    _fractionIlluminee = 0.5 * (1. + cos(elongation));

    // Phase
    if (_fractionIlluminee >= 0. && _fractionIlluminee < 0.03)
        _phase = QObject::tr("Nouvelle Lune");

    if (_fractionIlluminee >= 0.03 && _fractionIlluminee < 0.31)
        _phase = (sgn) ? QObject::tr("Premier croissant") : QObject::tr("Dernier croissant");

    if (_fractionIlluminee >= 0.31 && _fractionIlluminee < 0.69)
        _phase = (sgn) ? QObject::tr("Premier quartier") : QObject::tr("Dernier quartier");

    if (_fractionIlluminee >= 0.69 && _fractionIlluminee < 0.97)
        _phase = (sgn) ? QObject::tr("Gibbeuse croissante") : QObject::tr("Gibbeuse décroissante");

    if (_fractionIlluminee >= 0.97)
        _phase = QObject::tr("Pleine Lune");

    /* Retour */
    return;
}

/* Accesseurs */
double Lune::getFractionIlluminee() const
{
    return _fractionIlluminee;
}

QString Lune::getPhase() const
{
    return _phase;
}
