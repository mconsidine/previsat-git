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
 * >    evenementsconst.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >     Donnees relatives aux evenements satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef EVENEMENTSCONST_H
#define EVENEMENTSCONST_H

#include <QString>
#include "librairies/dates/date.h"


/*
 * Enumerations
 */
enum class SensCalcul {
    CHRONOLOGIQUE,
    ANTI_CHRONOLOGIQUE
};

enum class TypeNoeudOrbite {
    NOEUD_ASCENDANT,
    NOEUD_DESCENDANT
};

/*
 * Definitions des constantes
 */

/*
 * Definitions des structures
 */
struct ElementsAOS {
    Date date;
    double azimut;
    QString typeAOS = QObject::tr("AOS", "Acquisition of signal");
    bool aos;
};

#endif // EVENEMENTSCONST_H
