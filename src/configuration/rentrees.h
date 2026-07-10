/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    rentrees.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Lecture du fichier contenant les rentrees atmospheriques
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#ifndef RENTREES_H
#define RENTREES_H

#include <QList>


class QDomNode;
struct RentreesAtmospheriques;

class Rentrees
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief LectureFichierRentrees Lecture du fichier contenant les rentrees atmospheriques
     * @return rentrees atmospheriques
     * @throw Exception
     */
    static QList<RentreesAtmospheriques> LectureFichierRentrees();


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
    static RentreesAtmospheriques LectureRentree(const QDomNode &sat);


};

#endif // RENTREES_H
