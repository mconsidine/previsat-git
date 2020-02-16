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
 * >    message.h
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
 * >    1er janvier 2019
 *
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include "messageConstants.h"


class QMainWindow;

class Message
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief Afficher Affichage d'un message dans une boite de message
     * @param message Message a afficher
     * @param typeMessage Type de message (ERREUR, INFO, WARNING)
     */
    static void Afficher(const QString &message, const MessageType &typeMessage);

    /**
     * @brief setFenetreParent Definition de la fenetre parent (pour centrer l'affichage)
     * @param fenetre fenetre
     */
    static void setFenetreParent(QMainWindow *fenetre);

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */


};

#endif // MESSAGE_H
