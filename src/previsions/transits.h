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
 * >    transits.h
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
 * >    25 fevrier 2023
 *
 */

#ifndef TRANSITS_H
#define TRANSITS_H

#include "previsionsconst.h"


class Corps;
class Satellite;
struct EphemeridesTransits;

class Transits
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
     * @param[in/out] nombre nombre de satellites
     * @return nombre de satellites
     */
    static int CalculTransits(const int &nombre);


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
     * @param[in] jjm tableau de dates
     * @param[in] typeCorps type du corps (SOLEIL ou LUNE)
     * @param[in/out] satellite satellite
     * @return tableau de la date et de l'angle minimum
     */
    static QPointF CalculAngleMin(const std::array<double, MATHS::DEGRE_INTERPOLATION> jjm,
                                  const CorpsTransit &typeCorps,
                                  Satellite &satellite);

    /**
     * @brief CalculDate Calcul de la date ou la distance angulaire est minimale
     * @param[in] jjm tableau de dates
     * @param[in] typeCorps type du corps (SOLEIL ou LUNE)
     * @param[in] itransit cas d'un transit
     * @param[in/out] satellite satellite
     * @return date ou la distance angulaire est minimale
     */
    static double CalculDate(const std::array<double, MATHS::DEGRE_INTERPOLATION> jjm,
                             const CorpsTransit &typeCorps,
                             const bool itransit,
                             Satellite &satellite);

    /**
     * @brief CalculElements Calcul des dates caracteristiques de la conjonction ou du transit
     * @param[in] jmax date
     * @param[in] typeCorps type du corps (SOLEIL ou LUNE)
     * @param[in] itransit cas d'un transit
     * @param[in/out] satellite satellite
     * @return ensemble des dates
     */
    static QList<Date> CalculElements(const double jmax,
                                      const CorpsTransit &typeCorps,
                                      const bool itransit,
                                      Satellite &satellite);

    /**
     * @brief CalculEphemSoleilLune Calcul des ephemerides du Soleil et de la Lune
     * @return tableau d'ephemerides
     */
    static QMap<CorpsTransit, QList<EphemeridesTransits> > CalculEphemSoleilLune();


};

#endif // TRANSITSISS_H
