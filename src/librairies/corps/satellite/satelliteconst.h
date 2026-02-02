/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    satelliteconst.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >     Donnees relatives aux satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 decembre 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef SATELLITECONST_H
#define SATELLITECONST_H


/*
 * Enumerations
 */

/*
 * Definitions des constantes
 */

namespace SATELLITE {

static constexpr int ELEMENT_PHASAGE_INDEFINI = -999;
static constexpr int NB_ORB_INDEFINI = -1;

}


/*
 * Definitions des structures
 */
struct ElementsTraceCiel {
    double hauteur;
    double azimut;
    double jourJulienUTC;
    bool eclipseTotale;
    bool eclipsePartielle;
};

struct ElementsTraceSol {
    double longitude;
    double latitude;
    double jourJulienUTC;
    bool eclipseTotale;
    bool eclipsePartielle;
};

#endif // SATELLITECONST_H
