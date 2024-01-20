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
 * >    satellitesstarlink.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Informations sur les satellites Starlink
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    1er octobre 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef SATELLITESSTARLINK_H
#define SATELLITESSTARLINK_H

#include <QString>


/*
 * Definitions des constantes
 */
namespace STARLINK {

static constexpr double AGE_MAXIMAL_ELEM = 30.;
static constexpr double MAGNITUDE_LIMITE = 10.;
static constexpr double MAGNITUDE_STANDARD = 5.;

}


/*
 * Definitions des structures
 */
struct SatellitesStarlink
{
    QString fichier;
    QString lancement;
    QString deploiement;
};

#endif // SATELLITESSTARLINK_H
