/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    soleil.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position du Soleil
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    14 juin 2015
 *
 */

#include "soleil.h"
#include "librairies/maths/maths.h"

static const double ax = 1.000001018;

Soleil::Soleil()
{
    _distanceUA = 0.;
}

Soleil::Soleil(const Vecteur3D &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _position = pos;
    _distanceUA = pos.Norme();

    /* Retour */
    return;
}

Soleil::~Soleil()
{
}

/* Calcul de la position du Soleil a partir du modele simplifie
 * de l'Astronomical Algorithms 2nd edition de Jean Meeus, p163-164
 */
void Soleil::CalculPosition(const Date &date)
{
    /* Declarations des variables locales */
    double u1;

    /* Initialisations */
    const double t = date.jourJulienTT() * NB_SIECJ_PAR_JOURS;

    /* Corps de la methode */
    // Longitude moyenne
    const double ls = DEG2RAD * modulo(280.466457 + t * (36000.7698278 + 0.00030322 * t), T360);

    // Longitude du perihelie
    const double lp = DEG2RAD * modulo(282.937348 + t * (1.7195366 + 0.00045688 * t), T360);

    // Excentricite
    const double e = 0.016708634 - t * (4.2037e-5 + 1.267e-7 * t);

    // Anomalie moyenne
    const double ms = ls - lp;

    // Resolution de l'equation de Kepler : u = ms + e sin(u)
    double u = ms;
    do {
        u1 = u;
        u = u1 + (ms + e * sin(u1) - u1) / (1. - e * cos(u1));
    } while (fabs(u - u1) > 1.e-9);

    // Anomalie vraie
    const double v = 2. * atan(sqrt((1. + e) / (1. - e)) * tan(0.5 * u));

    // Rayon vecteur
    _distanceUA = ax * (1. - e * cos(u));

    // Longitude apparente
    _lonEcl = lp + v - DEG2RAD * 0.00569;
    const Vecteur3D pos(_lonEcl, 0., _distanceUA);

    // Position cartesienne equatoriale
    _position = Sph2Cart(pos, date) * UA2KM;

    /* Retour */
    return;
}

/* Accesseurs */
double Soleil::distanceUA() const
{
    return _distanceUA;
}
