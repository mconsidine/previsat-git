/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    previsatexception.cpp
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
#include "previsatexception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut (propagation de l'exception)
 */
PreviSatException::PreviSatException() throw()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _typeMessage = MessageType::INFO;

    /* Retour */
    return;
}

/*
 * Propagation de l'exception avec affichage d'un message
 */
PreviSatException::PreviSatException(const QString &message, const MessageType &typeMessage) throw()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _typeMessage = typeMessage;
    if (!message.trimmed().isEmpty()) {
#if (BUILD_TEST == false)
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

