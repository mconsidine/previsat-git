/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    iridium.h
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
 * >    17 juillet 2011
 *
 * Date de revision
 * >
 *
 */

#ifndef IRIDIUM_H
#define IRIDIUM_H

#include "conditions.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"

class Iridium
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    static void CalculFlashsIridium(const Conditions &conditions, Observateur &observateur);

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    static void CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur,
                                             QList<QVector<double> > &tabEphem);
    static double AngleReflexion(const Satellite &satellite, const Soleil &soleil);
    static void CalculAngleMin(Satellite &satellite, Observateur &observateur, Soleil &soleil,
                               const double jjm[], double minmax[]);
    static void CalculLimitesFlash(Satellite &satellite, Observateur &observateur, Soleil &soleil,
                                   const Conditions &conditions, const double dateMaxFlash, const double mgn0,
                                   Date lim[]);
    static void LimiteFlash(Satellite satellite, Observateur observateur, Soleil soleil,
                            const Conditions conditions, const double jjm[], const double mgn0,
                            double limite[]);
    static double MagnitudeFlash(Satellite satellite, const Observateur observateur, const Soleil soleil,
                                 const double angle, const bool ext);
    static int LectureStatutIridium(const char ope);

};

#endif // IRIDIUM_H
