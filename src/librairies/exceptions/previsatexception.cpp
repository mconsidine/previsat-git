/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    31 decembre 2018
 *
 */

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
    _typeMessage = INFO;

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
        Message::Afficher(message, typeMessage);
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
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

