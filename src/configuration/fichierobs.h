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
 * >    fichierobs.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Lecture/ecriture des fichiers de lieux d'observation
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    21 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef FICHIEROBS_H
#define FICHIEROBS_H

#include "librairies/observateur/observateur.h"


class FichierObs
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Ecriture Ecriture du fichier de lieu d'observation
     * @param[in] ficObsXml nom du fichier
     */
    static void Ecriture(const QString &ficObsXml);

    /**
     * @brief Lecture Lecture du fichier de lieu d'observation
     * @param[in] ficObsXml nom du fichier
     * @param[in] alarme affichage d'un message si le fichier ne contient pas de lieux d'observations
     * @return map contenant les lieux d'observation
     */
    static QMap<QString, Observateur> Lecture(const QString &ficObsXml, const bool alarme);


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
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

#endif // FICHIEROBS_H
