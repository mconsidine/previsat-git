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
 * >    logmessage.h
 *
 * Localisation
 * >    librairies.systeme
 *
 * Heritage
 * >
 *
 * Description
 * >    Message de log
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    21 mai 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H


class QString;

class LogMessage
{
#if (BUILD_TEST == true)
    friend class LogMessageTest;
#endif
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief LogMessage Constructeur par defaut
     * @param fichier nom du fichier de log
     */
    explicit LogMessage(const QString &fichier);

    /*
     * Destructeur
     */
    ~LogMessage();


    /*
     * Methodes publiques
     */

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

#endif // LOGMESSAGE_H