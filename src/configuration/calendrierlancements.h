/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    calendrierlancements.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Informations sur les lancements
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#ifndef CALENDRIERLANCEMENTS_H
#define CALENDRIERLANCEMENTS_H

#include <QString>


/*
 * Definitions des structures
 */
struct CalendrierLancements {

    QString date;
    QString heure;
    QString lancement;
    QString site;
    QString commentaire;
    QString maj;
};

#endif // CALENDRIERLANCEMENTS_H
