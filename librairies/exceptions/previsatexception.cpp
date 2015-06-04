/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >    exception
 *
 * Description
 * >    Gestion des exceptions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#include <stdio.h>
#include "message.h"
#include "previsatexception.h"

/*
 * Constructeurs
 */
PreviSatException::PreviSatException() throw()
{
    _ierr = 0;
}

PreviSatException::PreviSatException(const int ierr) throw()
{
    _ierr = ierr;
}

PreviSatException::PreviSatException(const QString &message, const MessageType ierr) throw()
{
    _ierr = ierr;
    Message::Afficher(message, ierr);
}

/*
 * Code d'erreur
 */
const char* PreviSatException::what() const throw()
{
    return (QString::number(_ierr).toStdString().c_str());
}
