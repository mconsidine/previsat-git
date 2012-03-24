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
 * >    transitiss.h
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
 * >    24 juillet 2011
 *
 * Date de revision
 * >
 *
 */

#ifndef TRANSITISS_H
#define TRANSITISS_H

#include "conditions.h"
#include "librairies/corps/satellite/satellite.h"

class TransitISS
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    static void CalculTransitsISS(const Conditions &conditions, Observateur &observateur);

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    static void CalculEphemSoleilLune(const Conditions &conditions, Observateur &observateur,
                                      QVector<QList<QVector<double > > > &tabEphem);
    static void CalculAngleMin(Satellite &satellite, Observateur &observateur, const double jjm[], const int typeCorps,
                               double minmax[]);
    static void CalculElements(Satellite &satellite, Observateur &observateur, const double jmax, const int typeCorps,
                               const bool itransit, const double seuilConjonction, Date dates[]);
    static void CalculDate(Satellite &satellite, Observateur &observateur, const double jjm[], const int typeCorps,
                           const bool itransit, const double seuilConjonction, double &dateInter);


};

#endif // TRANSITISS_H
