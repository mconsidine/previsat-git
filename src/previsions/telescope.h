/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    telescope.h
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 octobre 2020
 *
 * Date de revision
 * >    14 octobre 2022
 *
 */

#ifndef TELESCOPE_H
#define TELESCOPE_H

#include "previsionsconst.h"


class Telescope
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */
    static void setConditions(const ConditionsPrevisions &conditions);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculSuiviTelescope Calcul des coordonnees du satellite pour le suivi avec un telescope
     * @param nombre nombre de satellites
     * @return nombre de satellites
     */
    static int CalculSuiviTelescope(int &nombre);


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

#endif // TELESCOPE_H
