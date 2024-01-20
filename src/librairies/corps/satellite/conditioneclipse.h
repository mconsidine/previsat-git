/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    Determination des conditions d'eclipse
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 septembre 2016
 *
 * Date de revision
 * >    21 septembre 2022
 *
 */

#ifndef CONDITIONECLIPSE_H
#define CONDITIONECLIPSE_H

#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"


class Vecteur3D;

struct ElementsEclipse {
    double luminosite = 1.;
    double elongation;
    double phiSoleil;
    double phi;
    TypeEclipse type = TypeEclipse::NON_ECLIPSE;
};

class ConditionEclipse
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief ConditionEclipse Constructeur par defaut
     */
    ConditionEclipse() {
        _eclipseTotale = false;
        _eclipsePartielle = false;
        _eclipseAnnulaire = false;
        _eclipseLune.type = TypeEclipse::NON_ECLIPSE;
        _eclipseSoleil.type = TypeEclipse::NON_ECLIPSE;
    }


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculSatelliteEclipse Calcul de la condition d'eclipse du satellite
     * @param[in] position position du satellite
     * @param[in] soleil Soleil
     * @param[in] lune Lune
     * @param[in] refraction prise en compte de la refraction (cas de la Terre)
     */
    void CalculSatelliteEclipse(const Vecteur3D &position, const Soleil &soleil, const Lune *lune = nullptr, const bool refraction = true);


    /*
     * Accesseurs
     */
    bool eclipseTotale() const;
    bool eclipsePartielle() const;
    bool eclipseAnnulaire() const;
    const ElementsEclipse &eclipseLune() const;
    const ElementsEclipse &eclipseSoleil() const;


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
     * @param[in] position position du satellite
     * @param[in] positionCorpsOccultant position du corps occultant
     * @param[in] corpsOccultant corps occultant (TERRE ou LUNE)
     * @param[in] refraction prise en compte de la refraction (cas de la Terre)
     * @return elements de l'eclipse
     */
    ElementsEclipse CalculEclipse(const Vecteur3D &position, const Vecteur3D &positionCorpsOccultant, const CorpsOccultant &corpsOccultant,
                                  const bool refraction) const;

};

#endif // CONDITIONECLIPSE_H
