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
 * >    evenementsstation.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Informations sur les evenements concernant la station spatiale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    29 octobre 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef EVENEMENTSSTATION_H
#define EVENEMENTSSTATION_H

#include <QString>


struct EvenementsStation {
    QString dateDebutEvenementsStationSpatiale;
    QString dateFinEvenementsStationSpatiale;
    double masseStationSpatiale;
    double surfaceTraineeAtmospherique;
    double coefficientTraineeAtmospherique;
    QStringList evenementsStationSpatiale;
};

#endif // EVENEMENTSSTATION_H
