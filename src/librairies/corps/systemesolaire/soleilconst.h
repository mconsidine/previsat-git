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
 * >    soleilconst.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes liees au Soleil
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

#ifndef SOLEILCONST_H
#define SOLEILCONST_H

#include <QVector>


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */
namespace SOLEIL {

// Demi-grand axe de l'orbite de la Terre
static constexpr double DEMI_GRAND_AXE_TERRE = 1.0000010178;

// Magnitude du centre du disque solaire
static constexpr double MAGNITUDE_SOLEIL = -26.98;

// Rayon equatorial solaire (km)
static constexpr double RAYON_SOLAIRE = 696342.;


// Termes periodiques
static const QVector<double> kp = {   16002, 21863, 32004, 10931, 14529, 16368,  15318, 32794 };
static const QVector<double> ca = {      64,  -152,    62,    -8,    32,   -41,     19,   -11 };
static const QVector<double> sa = {    -150,   -46,    68,    54,    14,    24,    -28,    22 };
static const QVector<double> kq = {      10, 16002, 21863, 10931,  1473, 32004,   4387,    73 };
static const QVector<double> cl = {    -325,  -322,   -79,   232,   -52,    97,     55,   -41 };
static const QVector<double> sl = {    -105,  -137,   258,    35,  -116,   -88,   -112,   -80 };

}

#endif // SOLEILCONST_H
