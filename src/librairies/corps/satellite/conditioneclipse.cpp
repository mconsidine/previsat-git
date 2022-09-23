/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    conditioneclipse.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
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

#include "conditioneclipse.h"
#include "librairies/corps/terreconst.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Methodes publiques
 */
/*
 * Calcul de la condition d'eclipse du satellite
 */
void ConditionEclipse::CalculSatelliteEclipse(const Vecteur3D &position, const Soleil &soleil, const Lune &lune, const bool refraction)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _soleil = soleil;

    /* Corps de la methode */
    // Calcul de l'eclipse par la Terre
    _eclipseSoleil = CalculEclipse(position, soleil.position(), CorpsOccultant::TERRE, refraction);

    // Calcul de l'eclipse par la Lune
    if (lune.position().Norme() > EPSDBL100) {
        _eclipseLune = CalculEclipse(position, lune.position(), CorpsOccultant::LUNE, false);
    }

    if ((_eclipseSoleil.type != TypeEclipse::NON_ECLIPSE) && (_eclipseLune.type != TypeEclipse::NON_ECLIPSE)) {

        if (_eclipseSoleil.luminosite < _eclipseLune.luminosite) {
            _eclipseLune.type = TypeEclipse::NON_ECLIPSE;
            _eclipseLune.luminosite = 1.;
        } else {
            _eclipseSoleil.type = TypeEclipse::NON_ECLIPSE;
            _eclipseSoleil.luminosite = 1.;
        }
    }

    _eclipseTotale = ((_eclipseSoleil.type == TypeEclipse::ECLIPSE_TOTALE) || (_eclipseLune.type == TypeEclipse::ECLIPSE_TOTALE));
    _eclipsePartielle = ((_eclipseSoleil.type == TypeEclipse::ECLIPSE_PARTIELLE) || (_eclipseLune.type == TypeEclipse::ECLIPSE_PARTIELLE));
    _eclipseAnnulaire = ((_eclipseSoleil.type == TypeEclipse::ECLIPSE_ANNULAIRE) || (_eclipseLune.type == TypeEclipse::ECLIPSE_ANNULAIRE));

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
bool ConditionEclipse::eclipseTotale() const
{
    return _eclipseTotale;
}

bool ConditionEclipse::eclipsePartielle() const
{
    return _eclipsePartielle;
}

bool ConditionEclipse::eclipseAnnulaire() const
{
    return _eclipseAnnulaire;
}

const ElementsEclipse &ConditionEclipse::eclipseLune() const
{
    return _eclipseLune;
}

const ElementsEclipse &ConditionEclipse::eclipseSoleil() const
{
    return _eclipseSoleil;
}


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */
/*
 * Calcul des elements d'une eclipse
 */
ElementsEclipse ConditionEclipse::CalculEclipse(const Vecteur3D &position, const Vecteur3D &positionCorpsOccultant, const CorpsOccultant &corpsOccultant,
                                                const bool refraction) const
{
    /* Declarations des variables locales */
    double rayon;
    ElementsEclipse elements;
    Vecteur3D rho;

    /* Initialisations */
    const Vecteur3D rhoSatSol = _soleil.position() - position;
    const double distSatSol = rhoSatSol.Norme();

    /* Corps de la methode */
    switch (corpsOccultant) {
    default:
    case CorpsOccultant::TERRE:
    {
        rho = position;
        const double tanlat = position.z() / sqrt(position.x() * position.x() + position.y() * position.y());
        const double u = atan(tanlat / (1. - APLA));
        const double cu = cos(u);
        const double su = sin(u);
        rayon = RAYON_TERRESTRE * sqrt(cu * cu + G2 * su * su);
        break;
    }
    case CorpsOccultant::LUNE:
        rho = position - positionCorpsOccultant;
        rayon = RAYON_LUNAIRE;
        break;
    }

    elements.phiSoleil = asin(RAYON_SOLAIRE / distSatSol);
    if (std::isnan(elements.phiSoleil)) {
        elements.phiSoleil = PI_SUR_DEUX;
    } else {
        if ((corpsOccultant == CorpsOccultant::TERRE) && refraction) {
            elements.phiSoleil += REFRACTION_HZ;
        }
    }

    const double distance = rho.Norme();
    elements.phi = asin(rayon / distance);
    if (std::isnan(elements.phi)) {
        elements.phi = PI_SUR_DEUX;
    }

    elements.elongation = (-rho).Angle(rhoSatSol);

    if (((elements.phiSoleil + elements.phi) <= elements.elongation) || (distSatSol <= distance)) {

        elements.type = TypeEclipse::NON_ECLIPSE;
        elements.luminosite = 1.;

    } else if ((elements.phi - elements.phiSoleil) >= elements.elongation) {

        elements.type = TypeEclipse::ECLIPSE_TOTALE;
        elements.luminosite = 0.;

    } else if ((elements.phiSoleil - elements.phi) >= elements.elongation) {

        elements.type = TypeEclipse::ECLIPSE_ANNULAIRE;
        const double cps = cos(elements.phiSoleil);
        const double cpc = cos(elements.phi);
        elements.luminosite = 1. - (1. - cpc) / (1. - cps);

    } else {

        elements.type = TypeEclipse::ECLIPSE_PARTIELLE;
        const double cps = cos(elements.phiSoleil);
        const double cpc = cos(elements.phi);
        const double cth = cos(elements.elongation);
        const double sps = sin(elements.phiSoleil);
        const double spc = sin(elements.phi);
        const double sth = sin(elements.elongation);

        const double tmp1 = cps * acos((cpc - cps * cth) / (sps * sth));
        const double tmp2 = cpc * acos((cps - cpc * cth) / (spc * sth));
        const double tmp3 = acos((cth - cps * cpc) / (sps * spc));

        elements.luminosite = 1. - (PI - tmp1 - tmp2 - tmp3) / (PI * (1. - cps));

        if (elements.luminosite > 1.) {
            elements.luminosite = 1.;
        }
    }

    /* Retour */
    return elements;
}
