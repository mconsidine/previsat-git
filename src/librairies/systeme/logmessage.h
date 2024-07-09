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
 * >    9 juillet 2024
 *
 */

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H


class QString;

class LogMessage
{
#if (BUILD_TEST)
    friend class LogMessageTest;
#endif
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief LogMessage Constructeur par defaut
     * @param[in] baseNomFichier base du nom du fichier de log
     * @param[in] nbMaxFic nombre maximum de fichiers
     */
    LogMessage(const QString &baseNomFichier,
               const unsigned int nbMaxFic);

    /*
     * Destructeur
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Fermeture Fermeture du fichier de log
     */
    static void Fermeture();

    /*
     * Accesseurs
     */
    static const QString &nomFicLog();


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
    static QString _nomFicLog;


    /*
     * Methodes privees
     */


};

#endif // LOGMESSAGE_H
