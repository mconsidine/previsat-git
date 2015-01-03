/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    22 mars 2014
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
    static void CalculFlashsIridium(const Conditions &conditions, Observateur &observateur, QStringList &result);
    static int LectureStatutIridium(const char ope, QStringList &tabStsIri);
    static void FinTraitement();
    static double CalculMagnitudeIridium(const bool extinction, const Satellite &satellite, const Soleil &soleil,
                                         const Observateur &observateur);

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    static void DeterminationFlash(const double minmax[2], const QString &sts, const Conditions &conditions, double &temp,
                                   Observateur &observateur, Satellite &sat, Soleil &soleil);
    static void CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur);
    static double AngleReflexion(const Satellite &satellite, const Soleil &soleil);
    static void CalculAngleMin(const double jjm[], Satellite &satellite, Observateur &observateur, Soleil &soleil,
                               double minmax[]);
    static void CalculLimitesFlash(const double mgn0, const double dateMaxFlash, const Conditions &conditions,
                                   Satellite &satellite, Observateur &observateur, Soleil &soleil, Date lim[]);
    static void LimiteFlash(const double mgn0, const double jjm[], const Conditions &conditions, Satellite &satellite,
                            Observateur &observateur, Soleil &soleil, double limite[]);
    static double MagnitudeFlash(const bool ext, const double angle, const Observateur &observateur, const Soleil &soleil,
                                 Satellite &satellite);
    static QString EcrireFlash(const Date &date, const int i, const double altitude, const double angref, const double mag,
                               const QString &sts, const Conditions &conditions, const Observateur &observateur,
                               const Soleil &soleil, Satellite &sat);


};

#endif // IRIDIUM_H
