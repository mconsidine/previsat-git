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
 * >    28 juillet 2026
 *
 */

#include "librairies/corps/terreconst.h"
#include "conditioneclipse.h"
#include "PPM_atm/ppm_atm.h"


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
void ConditionEclipse::CalculSatelliteEclipse(const Vecteur3D &position,
                                              const Soleil &soleil,
                                              const Lune *lune)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _soleil = soleil;
    _eclipsePartielle = false;
    _eclipseAnnulaire = false;

    /* Corps de la methode */
    // Calcul de l'eclipse par la Terre
    _eclipseSoleil = CalculEclipseTerre(position, soleil.position());

    // Calcul de l'eclipse par la Lune
    if (lune != nullptr) {
        _eclipseLune = CalculEclipseLune(position, lune->position());
    }

    // Cas d'eclipse totale
    _eclipseTotale = ((_eclipseSoleil.type == TypeEclipse::ECLIPSE_TOTALE) || (_eclipseLune.type == TypeEclipse::ECLIPSE_TOTALE));

    // Cas d'eclipse partielle
    if (!_eclipseTotale) {
        _eclipsePartielle = ((_eclipseSoleil.type == TypeEclipse::ECLIPSE_PARTIELLE) || (_eclipseLune.type == TypeEclipse::ECLIPSE_PARTIELLE));
        _eclipseAnnulaire = ((_eclipseSoleil.type == TypeEclipse::ECLIPSE_ANNULAIRE) || (_eclipseLune.type == TypeEclipse::ECLIPSE_ANNULAIRE));
    }

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
 * Calcul des elements d'une eclipse par la Lune
 */
ElementsEclipse ConditionEclipse::CalculEclipseLune(const Vecteur3D &positionSatellite, const Vecteur3D &positionLune) const
{
    /* Declarations des variables locales */
    ElementsEclipse elements;

    /* Initialisations */
    const Vecteur3D rho = positionSatellite - positionLune;
    const Vecteur3D rhoSatSol = _soleil.position() - positionSatellite;
    const double distance = rho.Norme();
    const double distSatSol = rhoSatSol.Norme();

    /* Corps de la methode */
    // Calcul des elements de l'eclipse
    elements.phiSoleil = asin(SOLEIL::RAYON_SOLAIRE / distSatSol);
    if (std::isnan(elements.phiSoleil)) {
        elements.phiSoleil = MATHS::PI_SUR_DEUX;
    }

    elements.phi = asin(LUNE::RAYON_LUNAIRE / distance);

    if (std::isnan(elements.phi)) {
        elements.phi = MATHS::PI_SUR_DEUX;
    }

    elements.elongation = (-rho).Angle(rhoSatSol);

    // Discrimination des cas d'eclipse
    if (((elements.phiSoleil + elements.phi) <= elements.elongation) || (distSatSol <= distance)) {

        // Non eclipse
        elements.type = TypeEclipse::NON_ECLIPSE;
        elements.luminosite = 1.;

    } else if ((elements.phi - elements.phiSoleil) >= elements.elongation) {

        // Eclipse totale
        elements.type = TypeEclipse::ECLIPSE_TOTALE;
        elements.luminosite = 0.;

    } else if ((elements.phiSoleil - elements.phi) >= elements.elongation) {

        // Eclipse annulaire
        elements.type = TypeEclipse::ECLIPSE_ANNULAIRE;
        const double cps = cos(elements.phiSoleil);
        const double cpc = cos(elements.phi);
        elements.luminosite = 1. - (1. - cpc) / (1. - cps);

    } else {

        // Eclipse partielle
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

        elements.luminosite = 1. - (MATHS::PI - tmp1 - tmp2 - tmp3) / (MATHS::PI * (1. - cps));

        if (elements.luminosite > 1.) {
            elements.luminosite = 1.;
        }
    }

    /* Retour */
    return elements;
}

/*
 * Calcul des elements d'une eclipse par la Terre
 */
ElementsEclipse ConditionEclipse::CalculEclipseTerre(const Vecteur3D &positionSatellite, const Vecteur3D &positionSoleil) const
{
    /* Declarations des variables locales */
    ElementsEclipse elements;

    /* Initialisations */
    const Vecteur3D rho = positionSatellite;
    const Vecteur3D rhoSatSol = _soleil.position() - positionSatellite;
    const double distance = rho.Norme();
    const double distSatSol = rhoSatSol.Norme();

    elements.elongation = (-rho).Angle(rhoSatSol);

    elements.phiSoleil = asin(SOLEIL::RAYON_SOLAIRE / distSatSol);
    if (std::isnan(elements.phiSoleil)) {
        elements.phiSoleil = MATHS::PI_SUR_DEUX;
    }

    elements.phi = asin(TERRE::RAYON_TERRESTRE / distance);

    if (std::isnan(elements.phi)) {
        elements.phi = MATHS::PI_SUR_DEUX;
    }

    /* Corps de la methode */
    // Modele PPM_atm (2018) :
    // A shadow function model based on perspective projection and atmospheric effect for satellites in eclipse
    // By Zhen Lia, Marek Ziebarta, Santosh Bhattaraia, David Harrisona
    // Code source : https://github.com/whulizhen/PPM_atm_shadow_function
    // Le code source d'origine a ete nettoye et ameliore, puis a ete compile en librairie partagee
    elements.luminosite = PPM_atm::shadow_factor(positionSoleil.tab(), positionSatellite.tab());

    // Discrimination des cas d'eclipse
    if (fabs(elements.luminosite) < MATHS::EPSDBL) {

        // Eclipse totale
        elements.type = TypeEclipse::ECLIPSE_TOTALE;

    } else if (fabs(elements.luminosite - 1.) < MATHS::EPSDBL) {

        // Non eclipse
        elements.type = TypeEclipse::NON_ECLIPSE;

    } else {

        if ((elements.phiSoleil - elements.phi) >= elements.elongation) {

            // Eclipse annulaire
            elements.type = TypeEclipse::ECLIPSE_ANNULAIRE;

        } else {

            // Eclipse partielle
            elements.type = TypeEclipse::ECLIPSE_PARTIELLE;
        }
    }

    /* Retour */
    return elements;
}
