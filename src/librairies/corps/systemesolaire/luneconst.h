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
 * >    luneconst.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes liees a la Lune
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    25 fevrier 2023
 *
 */

#ifndef LUNECONST_H
#define LUNECONST_H

#include <QPointF>
#include <QVector>
#include <cmath>
#include "librairies/maths/mathsconst.h"


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
namespace LUNE {

static constexpr unsigned int NB_PHASES = 4;
static constexpr unsigned int NB_TERMES_PERIODIQUES = 60;

// Rayon equatorial lunaire (km)
static constexpr double RAYON_LUNAIRE = 1737.4;

// Pour le calcul de la magnitude
static constexpr double B0 = 2.01;
static constexpr double H = 0.07;
static constexpr double P0 = 1.68;
static constexpr double W0 = 0.21;

static constexpr double GA = sqrt(1. - W0);
static constexpr double R0 = (1. - GA) / (1. + GA);
static constexpr double R0S2 = 0.5 * R0;
static constexpr double W0S8 = 0.125 * W0;
static constexpr double R2S6 = R0 * R0 / 6.;
static constexpr double P = W0S8 * ((1. + B0) * P0 - 1.) + R0S2 + R2S6;
static constexpr double B = 0.29;
static constexpr double C = 0.39;

}

static const QVector<QPointF> _tableKappa1 = {
    { 0., 1. },
    { 2., 0.991 },
    { 5., 0.981 },
    { 10., 0.970 },
    { 20., 0.957 },
    { 30., 0.947 },
    { 40., 0.938 },
    { 50., 0.926 },
    { 60., 0.911 },
    { 70., 0.891 },
    { 80., 0.864 }
};

static const QVector<QPointF> _tableKappa2 = {
    { 80., 0.864 },
    { 90., 0.827 },
    { 100., 0.777 },
    { 110., 0.708 },
    { 120., 0.617 },
    { 130., 0.503 },
    { 140., 0.374 }
};

static const QVector<QPointF> _tableKappa3 = {
    { 140., 0.374 },
    { 150., 0.244 },
    { 160., 0.127 }
};

static const QVector<QPointF> _tableKappa4 = {
    { 160., 0.127 },
    { 170., 0.037 },
    { 180., 0. }
};


static constexpr std::array<double, LUNE::NB_TERMES_PERIODIQUES> _tabLon = {
    6288774., 1274027., 658314., 213618., -185116., -114332., 58793., 57066., 53322., 45758., -40923., -34720., -30383., 15327., -12528., 10980., 10675.,
    10034., 8548., -7888., -6766., -5163., 4987., 4036., 3994., 3861., 3665., -2689., -2602., 2390., -2348., 2236., -2120., -2069., 2048., -1773., -1595.,
    1215., -1110., -892., -810., 759., -713., -700., 691., 596., 549., 537., 520., -487., -399., -381., 351., -340., 330., 327., -323., 299., 294., 0.
};

static constexpr std::array<double, LUNE::NB_TERMES_PERIODIQUES> _tabDist = {
    -20905355., -3699111., -2955968., -569925., 48888., -3149., 246158., -152138., -170733., -204586., -129620., 108743., 104755., 10321., 0., 79661.,
    -34782., -23210., -21636., 24208., 30824., -8379., -16675., -12831., -10445., -11650., 14403., -7003., 0., 10056., 6322., -9884., 5751., 0., -4950.,
    4130., 0., -3958., 0., 3258., 2616., -1897., -2117., 2354., 0., 0., -1423., -1117., -1571., -1739., 0., -4421., 0., 0., 0., 0., 1165., 0., 0., 8752.
};

static constexpr std::array<double, LUNE::NB_TERMES_PERIODIQUES> _tabLat = {
    5128122., 280602., 277693., 173237., 55413., 46271., 32573., 17198., 9266., 8822., 8216., 4324., 4200., -3359., 2463., 2211., 2065., -1870., 1828.,
    -1794., -1749., -1565., -1491., -1475., -1410., -1344., -1335., 1107., 1021., 833., 777., 671., 607., 596., 491., -451., 439., 422., 421., -366.,
    -351., 331., 315., 302., -283., -229., 223., 223., -220., -220., -185., 181., -177., 176., 166., -164., 132., -119., 115., 107. };

static constexpr std::array<std::array<int, 4>, LUNE::NB_TERMES_PERIODIQUES> _tabCoef1 = {
    { {0, 0, 1, 0}, {2, 0, -1, 0}, {2, 0, 0, 0}, {0, 0, 2, 0}, {0, 1, 0, 0}, {0, 0, 0, 2}, {2, 0, -2, 0}, {2, -1, -1, 0}, {2, 0, 1, 0}, {2, -1, 0, 0},
      {0, 1, -1, 0}, {1, 0, 0, 0}, {0, 1, 1, 0}, {2, 0, 0, -2}, {0, 0, 1, 2}, {0, 0, 1, -2}, {4, 0, -1, 0}, {0, 0, 3, 0}, {4, 0, -2, 0}, {2, 1, -1, 0},
      {2, 1, 0, 0}, {1, 0, -1, 0}, {1, 1, 0, 0}, {2, -1, 1, 0}, {2, 0, 2, 0}, {4, 0, 0, 0}, {2, 0, -3, 0}, {0, 1, -2, 0}, {2, 0, -1, 2}, {2, -1, -2, 0},
      {1, 0, 1, 0}, {2, -2, 0, 0}, {0, 1, 2, 0}, {0, 2, 0, 0}, {2, -2, -1, 0}, {2, 0, 1, -2}, {2, 0, 0, 2}, {4, -1, -1, 0}, {0, 0, 2, 2}, {3, 0, -1, 0},
      {2, 1, 1, 0}, {4, -1, -2, 0}, {0, 2, -1, 0}, {2, 2, -1, 0}, {2, 1, -2, 0}, {2, -1, 0, -2}, {4, 0, 1, 0}, {0, 0, 4, 0}, {4, -1, 0, 0}, {1, 0, -2, 0},
      {2, 1, 0, -2}, {0, 0, 2, -2}, {1, 1, 1, 0}, {3, 0, -2, 0}, {4, 0, -3, 0}, {2, -1, 2, 0}, {0, 2, 1, 0}, {1, 1, -1, 0}, {2, 0, 3, 0}, {2, 0, -1, -2} }
};

static constexpr std::array<std::array<int, 4>, LUNE::NB_TERMES_PERIODIQUES> _tabCoef2 = {
    { {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, -1}, {2, 0, 0, -1}, {2, 0, -1, 1}, {2, 0, -1, -1}, {2, 0, 0, 1}, {0, 0, 2, 1}, {2, 0, 1, -1}, {0, 0, 2, -1},
      {2, -1, 0, -1}, {2, 0, -2, -1}, {2, 0, 1, 1}, {2, 1, 0, -1}, {2, -1, -1, 1}, {2, -1, 0, 1}, {2, -1, -1, -1}, {0, 1, -1, -1}, {4, 0, -1, -1},
      {0, 1, 0, 1}, {0, 0, 0, 3}, {0, 1, -1, 1}, {1, 0, 0, 1}, {0, 1, 1, 1}, {0, 1, 1, -1}, {0, 1, 0, -1}, {1, 0, 0, -1}, {0, 0, 3, 1}, {4, 0, 0, -1},
      {4, 0, -1, 1}, {0, 0, 1, -3}, {4, 0, -2, 1}, {2, 0, 0, -3}, {2, 0, 2, -1}, {2, -1, 1, -1}, {2, 0, -2, 1}, {0, 0, 3, -1}, {2, 0, 2, 1},
      {2, 0, -3, -1}, {2, 1, -1, 1}, {2, 1, 0, 1}, {4, 0, 0, 1}, {2, -1, 1, 1}, {2, -2, 0, -1}, {0, 0, 1, 3}, {2, 1, 1, -1}, {1, 1, 0, -1}, {1, 1, 0, 1},
      {0, 1, -2, -1}, {2, 1, -1, -1}, {1, 0, 1, 1}, {2, -1, -2, -1}, {0, 1, 2, 1}, {4, 0, -2, -1}, {4, -1, -1, -1}, {1, 0, 1, -1}, {4, 0, 1, -1},
      {1, 0, -1, -1}, {4, -1, 0, -1}, {2, -2, 0, 1} }
};

static const Vecteur3D _w(0., 0., 1.);


#endif // LUNECONST_H
