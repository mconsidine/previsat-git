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
 * >    rentrees.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Informations sur les rentrees atmospheriques
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#ifndef RENTREESATMOSPHERIQUES_H
#define RENTREESATMOSPHERIQUES_H

#include <QDateTime>
#include <QString>


/*
 * Definitions des structures
 */
struct RentreesAtmospheriques {

    QDateTime dateRentree;
    QString norad;
    QString cospar;
    QString nom;
    QString taille;
    QString pays;
    QString typeMessage;
};

#endif // RENTREESATMOSPHERIQUES_H
