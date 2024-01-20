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
 * >    configurationconst.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >     Donnees generales pour la configuration
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

#ifndef CONFIGURATIONCONST_H
#define CONFIGURATIONCONST_H


/*
 * Macros
 */
#define EFFACE_OBJET(objet)   \
    if ((objet) != nullptr) { \
        delete (objet);       \
        (objet) = nullptr;    \
    }


/*
 * Enumerations
 */
enum class TypeTelechargement {
    COORDONNEES = 0,
    CARTES,
    NOTIFICATIONS
};

enum class NotificationSonore {
    NOTIFICATION_AOS,
    ATTENTE_AOS,
    NOTIFICATION_LOS,
    ATTENTE_LOS
};


/*
 * Definitions des constantes
 */


#endif // CONFIGURATIONCONST_H
