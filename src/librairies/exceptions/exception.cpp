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
 * >    Exception.cpp
 *
 * Localisation
 * >    librairies.exceptions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    17 octobre 2022
 *
 */

#include <QString>
#include "message.h"
#include "exception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Propagation de l'exception avec affichage d'un message
 */
Exception::Exception(const QString &message,
                     const MessageType &typeMessage)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    if (!message.trimmed().isEmpty()) {
#if (BUILD_TEST)
        Q_UNUSED(typeMessage)
#else
        Message::Afficher(message, typeMessage);
#endif
    }

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */

/*
 * Accesseurs
 */


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */

