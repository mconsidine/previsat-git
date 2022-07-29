/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    14 aout 2016
 *
 */

#ifndef SOLEILCONST_H
#define SOLEILCONST_H

/*
 * Enumerations
 */
enum class CorpsOccultant {
    TERRE,
    LUNE
};

enum class TypeEclipse {
    NON_ECLIPSE,
    ECLIPSE_PARTIELLE,
    ECLIPSE_ANNULAIRE,
    ECLIPSE_TOTALE
};


/*
 * Definitions des constantes
 */
// Demi-grand axe de l'orbite de la Terre
static const double DEMI_GRAND_AXE_TERRE = 1.000001018;

// Magnitude du centre du disque solaire
static const double MAGNITUDE_SOLEIL = -26.98;

// Rayon equatorial solaire (km)
static const double RAYON_SOLAIRE = 696342.;

// Unite astronomique (km)
static const double UA2KM = 149597870.;

// Vitesse de la lumiere (km/s)
static const double VITESSE_LUMIERE = 299792.458;

static const double KM2UA = 1. / UA2KM;


#endif // SOLEILCONST_H
