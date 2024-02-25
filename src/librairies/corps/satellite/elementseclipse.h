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
 * >    elementseclipse.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >     Elements d'une eclipse
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

#ifndef ELEMENTSECLIPSE_H
#define ELEMENTSECLIPSE_H


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

/*
 * Definitions des structures
 */
struct ElementsEclipse {
    double luminosite = 1.;
    double elongation;
    double phiSoleil;
    double phi;
    TypeEclipse type = TypeEclipse::NON_ECLIPSE;
};

#endif // ELEMENTSECLIPSE_H
