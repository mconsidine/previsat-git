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
 * >    soleil.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >
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
 * >
 *
 */

#include "soleil.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/systemesolaire/SoleilConstants.h"

Soleil::Soleil()
{
    _distanceUA = 0.;
}

Soleil::Soleil(const Vecteur3D &position)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _position = position;
    _distanceUA = position.Norme();

    /* Retour */
    return;
}

Soleil::~Soleil()
{
}

void Soleil::CalculPosition(const Date &date)
{
    /* Declarations des variables locales */
    double u, u1;

    /* Initialisations */
    const double tu = date.getJourJulienUTC() * NB_SIECJ_PAR_JOURS;
    const double tu2 = tu * tu;

    /* Corps de la methode */
    // Longitude moyenne
    const double ls = DEG2RAD * Maths::modulo(280.466457 + 36000.7698278 * tu + 0.00030322 * tu2, T360);

    // Longitude du perihelie
    const double lp = DEG2RAD * Maths::modulo(282.937348 + 1.7195366 * tu + 0.00045688 * tu2, T360);

    // Excentricite
    const double e = 0.01670843 - 4.2037e-5 * tu - 1.267e-7 * tu2;

    // Anomalie moyenne
    const double ms = ls -lp;

    // Resolution de l'equation de Kepler : u = ms + e sin(u)
    u = ms;
    do {
        u1 = u;
        u = u1 + (ms + e * sin(u1) - u1) / (1. - e * cos(u1));
    } while (fabs(u - u1) > 1.e-9);

    // Anomalie vraie
    const double v = 2. * atan(sqrt((1. + e) / (1. - e)) * tan(0.5 * u));

    // Longitude vraie
    const double lv = lp + v;

    // Rayon vecteur
    _distanceUA = 1.000001018 * (1. - e * cos(u));
    const double rp = _distanceUA * UA;

    const double obliquite = ARCSEC2RAD * (84381.448 - 46.815 * tu - 0.00059 * tu2 + 0.001813 * tu * tu2);

    const double xx = rp * sin(lv);
    _position = Vecteur3D(rp * cos(lv), xx * cos(obliquite), xx * sin(obliquite));

    /* Retour */
    return;
}

/* Accesseurs */
double Soleil::getDistanceUA() const
{
    return _distanceUA;
}
