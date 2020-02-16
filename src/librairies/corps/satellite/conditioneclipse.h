/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    conditioneclipse.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 septembre 2016
 *
 * Date de revision
 * >
 *
 */

#ifndef CONDITIONECLIPSE_H
#define CONDITIONECLIPSE_H

#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"


class Vecteur3D;

struct ElementsEclipse {
    TypeEclipse type = NON_ECLIPSE;
    double luminosite = 1.;
    double elongation;
    double phiSoleil;
    double phi;
};

class ConditionEclipse
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Accesseurs
     */
    bool eclipseTotale() const;
    bool eclipsePartielle() const;
    bool eclipseAnnulaire() const;
    ElementsEclipse eclipseLune() const;
    ElementsEclipse eclipseSoleil() const;

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
     * @brief CalculSatelliteEclipse Calcul de la condition d'eclipse du satellite
     * @param position position du satellite
     * @param soleil Soleil
     * @param lune Lune
     * @param refraction prise en compte de la refraction (cas de la Terre)
     */
    void CalculSatelliteEclipse(const Vecteur3D &position, const Soleil &soleil, const Lune &lune = Lune(), const bool refraction = true);


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
    bool _eclipseTotale;
    bool _eclipsePartielle;
    bool _eclipseAnnulaire;
    ElementsEclipse _eclipseLune;
    ElementsEclipse _eclipseSoleil;
    Soleil _soleil;

    /*
     * Methodes privees
     */
    /**
     * @brief CalculEclipse Calcul des elements d'une eclipse
     * @param position position du satellite
     * @param positionCorpsOccultant position du corps occultant
     * @param corpsOccultant corps occultant (TERRE ou LUNE)
     * @param refraction prise en compte de la refraction (cas de la Terre)
     * @return elements de l'eclipse
     */
    ElementsEclipse CalculEclipse(const Vecteur3D &position, const Vecteur3D &positionCorpsOccultant, const CorpsOccultant &corpsOccultant, const bool refraction);


};

#endif // CONDITIONECLIPSE_H
