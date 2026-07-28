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
 * >    28 juillet 2026
 *
 */

#ifndef CONDITIONECLIPSE_H
#define CONDITIONECLIPSE_H

#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "elementseclipse.h"


class Vecteur3D;

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
     */
    void CalculSatelliteEclipse(const Vecteur3D &position,
                                const Soleil &soleil,
                                const Lune *lune = nullptr);


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
     * @brief CalculEclipseLune Calcul des elements d'une eclipse par la Lune
     * @param[in] positionSatellite position du satellite
     * @param[in] positionLune position de la Lune
     * @return elements de l'eclipse
     */
    ElementsEclipse CalculEclipseLune(const Vecteur3D &positionSatellite,
                                     const Vecteur3D &positionLune) const;

    /**
     * @brief CalculEclipseTerre Calcul des elements d'une eclipse par la Terre
     * @param[in] positionSatellite position du satellite
     * @param[in] positionLune position du Soleil
     * @return elements de l'eclipse
     */
    ElementsEclipse CalculEclipseTerre(const Vecteur3D &positionSatellite,
                                       const Vecteur3D &positionSoleil) const;
};

#endif // CONDITIONECLIPSE_H
