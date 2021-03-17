/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    evenementsorbitaux.h
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

#ifndef EVENEMENTSORBITAUX_H
#define EVENEMENTSORBITAUX_H

#include "previsionsconst.h"


class Satellite;
struct EphemeridesEvenements;

class EvenementsOrbitaux
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
     * @brief CalculEvenements Calcul des evenements orbitaux
     * @param nombre nombre de satellites
     * @return nombre de satellites
     */
    static int CalculEvenements(int &nombre);


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
     * @brief CalculEphemerides Calcul des ephemerides du satellite, du Soleil et de la Lune
     * @param satellites satellites
     * @return ephemerides
     */
    static QMap<QString, QList<EphemeridesEvenements> > CalculEphemerides(const QList<Satellite> &satellites);

    /**
     * @brief CalculEvt Calcul des elements de l'evenement orbital
     * @param jjm tableau de dates
     * @param evt grandeurs caracteristiques de l'evenement
     * @param yval valeur que doit prendre la grandeur caracteristique de l'evenement
     * @param sat satellite
     * @return elements de l'evenement
     */
    static ResultatPrevisions CalculEvt(const QList<double> &jjm, const QList<double> &evt, const double yval, Satellite &sat);


};

#endif // EVENEMENTSORBITAUX_H
