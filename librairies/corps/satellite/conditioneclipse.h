/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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


class ConditionEclipse
{
public:

    /* Constructeurs */
    ConditionEclipse();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculSatelliteEclipse Determination de la condition d'eclipse du satellite
     * @param soleil Soleil
     * @param lune Lune
     * @param position position du satellite
     * @param acalcEclipseLune prise en compte des eclipses de Lune
     * @param refraction Prise en compte de la refraction atmospherique
     */
    void CalculSatelliteEclipse(const Soleil &soleil, const Lune &lune, const Vecteur3D &position, const bool acalcEclipseLune,
                                const bool refraction = true);

    /**
     * @brief CalculSatelliteEclipseCorps Calcul de la condition d'eclipse du satellite par un corps occultant
     * @param soleil Soleil
     * @param position position du satellite
     * @param corpsOccultant corps occultant
     * @param lune Lune
     * @param refraction Prise en compte de la refraction atmospherique
     */
    void CalculSatelliteEclipseCorps(const Soleil &soleil, const Vecteur3D &position, const CorpsOccultant &corpsOccultant, const Lune &lune = Lune(),
                                     const bool refraction = true);


    /* Accesseurs */
    bool isEclipseTotale() const;
    bool isEclipsePartielle() const;
    bool isEclipseAnnulaire() const;
    TypeEclipse typeEclipseSoleil() const;
    TypeEclipse typeEclipseLune() const;
    double luminositeEclipseSoleil() const;
    double luminositeEclipseLune() const;
    double elongationSoleil() const;
    double elongationLune() const;
    double phiSoleil() const;
    double phiSoleilRefr() const;
    double phiTerre() const;
    double phiLune() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    bool _eclipseTotale;
    bool _eclipsePartielle;
    bool _eclipseAnnulaire;
    TypeEclipse _typeEclipseSoleil;
    TypeEclipse _typeEclipseLune;
    double _luminositeEclipseSoleil;
    double _luminositeEclipseLune;
    double _phiSoleil;
    double _phiSoleilRefr;
    double _phiTerre;
    double _phiLune;
    double _elongationSoleil;
    double _elongationLune;


    /* Methodes privees */


};

#endif // CONDITIONECLIPSE_H
