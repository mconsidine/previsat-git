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
 * >    previsatexception.h
 *
 * Localisation
 * >    librairies.exceptions
 *
 * Heritage
 * >
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

#ifndef PREVISATEXCEPTION_H
#define PREVISATEXCEPTION_H

#include <exception>
#include "messageconst.h"


class QString;

class PreviSatException : public std::exception
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief PreviSatException Constructeur par defaut (propagation de l'exception)
     */
    PreviSatException() throw();

    /**
     * @brief PreviSatException Propagation de l'exception avec affichage d'un message
     * @param message message
     * @param typeMessage type du message (INFO, WARNING ou ERREUR)
     */
    PreviSatException(const QString &message, const MessageType &typeMessage) throw();


    /*
     * Accesseurs
     */

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    MessageType _typeMessage;

    /*
     * Methodes privees
     */


};


#endif // PREVISATEXCEPTION_H