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
 * >    prevision.h
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
 * >    11 juillet 2011
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#ifndef PREVISION_H
#define PREVISION_H

#include "previsionsconst.h"


struct EphemeridesPrevisions;

class Prevision
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Accesseurs
     */
    static QMap<QString, QList<QList<ResultatPrevisions> > > &resultats();
    static DonneesPrevisions &donnees();


    /*
     * Modificateurs
     */
    static void setConditions(const ConditionsPrevisions &conditions);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculPrevisions Calcul des previsions de passage
     * @param[in/out] nombre nombre de satellites
     * @return nombre de satellites
     */
    static int CalculPrevisions(const int &nombre);


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
    /**
     * @brief CalculEphemSoleilObservateur Calcul des ephemerides du Soleil et de l'observateur
     * @return tableau d'ephemerides
     */
    static QList<EphemeridesPrevisions> CalculEphemSoleilObservateur();


};

#endif // PREVISION_H
