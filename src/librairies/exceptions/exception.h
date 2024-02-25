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
 * >    Exception.h
 *
 * Localisation
 * >    librairies.exceptions
 *
 * Heritage
 * >    std::exception
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
 * >    31 decembre 2018
 *
 */

#ifndef Exception_H
#define Exception_H

#include <QException>
#include "messageconst.h"


class QString;

class Exception : public QException
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Exception Constructeur par defaut (propagation de l'exception)
     */
    Exception() {}

    /**
     * @brief Exception Propagation de l'exception avec affichage d'un message
     * @param[in] message message
     * @param[in] typeMessage type du message (INFO, WARNING ou ERREUR)
     */
    Exception(const QString &message,
              const MessageType &typeMessage);


    /*
     * Destructeur
     */
    virtual ~Exception() noexcept {
    }


    /*
     * Methodes publiques
     */
    void raise() const override
    {
        throw *this;
    }

    Exception * clone() const override
    {
        return new Exception(*this);
    }


    /*
     * Accesseurs
     */


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */

    /*
     * Methodes privees
     */


};

#endif // Exception_H
