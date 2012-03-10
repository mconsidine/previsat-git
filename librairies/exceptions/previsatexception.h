/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#ifndef PREVISATEXCEPTION_H
#define PREVISATEXCEPTION_H

#include <QString>
#include <exception>
#include "messages.h"

class PreviSatException : public std::exception
{
public:

    /* Constructeurs */
    PreviSatException() throw();
    PreviSatException(const int ierr) throw();
    PreviSatException(const QString message) throw();
    PreviSatException(const QString message, const Messages::MessageType ierr) throw();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    virtual const char* what() const throw();

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    int _ierr;

    /* Methodes privees */

};

#endif // PREVISATEXCEPTION_H
