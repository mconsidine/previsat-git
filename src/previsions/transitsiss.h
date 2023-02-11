/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    transitsiss.h
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

#ifndef TRANSITSISS_H
#define TRANSITSISS_H

#include "previsionsconst.h"


class Corps;
class Satellite;
struct EphemeridesTransits;

class TransitsIss
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Accesseurs
     */
    static QMap<QString, QList<QList<ResultatPrevisions> > > &resultats();
    static DonneesPrevisions donnees();

    /*
     * Modificateurs
     */
    static void setConditions(const ConditionsPrevisions &conditions);


    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculPrevisions Calcul des transits de l'ISS devant la Lune ou le Soleil
     * @param nombre nombre de satellites
     * @return nombre de satellites
     */
    static int CalculTransits(int &nombre);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */

    /*
     * Methodes privees
     */
    /**
     * @brief CalculAngleMin Calcul de l'angle minimum du panneau
     * @param jjm tableau de dates
     * @param satellite satellite
     * @param observateur observateur
     * @param soleil Soleil
     * @param minmax tableau de la date et de l'angle minimum
     */
    static QPair<double, double> CalculAngleMin(const QList<double> jjm, const CorpsTransit &typeCorps, Satellite &satellite);

    /**
     * @brief CalculDate Calcul de la date ou la distance angulaire est minimale
     * @param jjm tableau de dates
     * @param typeCorps type du corps (SOLEIL ou LUNE)
     * @param itransit cas d'un transit
     * @param satellite satellite
     * @return date ou la distance angulaire est minimale
     */
    static double CalculDate(const QList<double> jjm, const CorpsTransit & typeCorps, const bool itransit, Satellite &satellite);

    /**
     * @brief CalculElements Calcul des dates caracteristiques de la conjonction ou du transit
     * @param jmax date
     * @param typeCorps type du corps (SOLEIL ou LUNE)
     * @param itransit cas d'un transit
     * @param satellite satellite
     * @return ensemble des dates
     */
    static QList<Date> CalculElements(const double jmax, const CorpsTransit &typeCorps, const bool itransit, Satellite &satellite);

    /**
     * @brief CalculEphemSoleilLune Calcul des ephemerides du Soleil et de la Lune
     * @return tableau d'ephemerides
     */
    static QMap<CorpsTransit, QList<EphemeridesTransits> > CalculEphemSoleilLune();


};

#endif // TRANSITSISS_H
