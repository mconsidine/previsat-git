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
 * >    satellitesflashs.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Caracteristiques des angles d'attitude pour les satellites produisant des flashs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    19 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef SATELLITESFLASHS_H
#define SATELLITESFLASHS_H

#include <QList>


/*
 * Definitions des structures
 */
struct SatellitesFlashs
{
    QString nomsat;
    QString norad;
    QList<QPair<double, double> > angles;
};

#endif // SATELLITESFLASHS_H
