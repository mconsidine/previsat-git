/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    magnitude.cpp
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

#include <cmath>
#include "librairies/maths/mathsconst.h"
#include "librairies/observateur/observateur.h"
#include "conditioneclipse.h"
#include "magnitude.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Magnitude::Magnitude()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _fractionIlluminee = 0.;
    _magnitude = MAGNITUDE_INDEFINIE;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la magnitude visuelle du satellite
 */
void Magnitude::Calcul(const ConditionEclipse &conditionEclipse, const Observateur &observateur, const double distance, const double hauteur,
                       const double magnitudeStandard, const bool extinction, const bool effetEclipsePartielle)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _magnitude = MAGNITUDE_INDEFINIE;

    /* Corps de la methode */
    if (!conditionEclipse.eclipseTotale()) {

        // Fraction illuminee
        _fractionIlluminee = 0.5 * (1. + cos(conditionEclipse.eclipseSoleil().elongation));

        // Magnitude visuelle
        if (magnitudeStandard < MAGNITUDE_INDEFINIE) {

            _magnitude = magnitudeStandard - 15.75 + 2.5 * log10(distance * distance / _fractionIlluminee);

            // Prise en compte des eclipses partielles ou annulaires
            if (effetEclipsePartielle) {

                const double luminosite = qMin(conditionEclipse.eclipseSoleil().luminosite, conditionEclipse.eclipseLune().luminosite);
                if ((luminosite > 0.) && (luminosite <= 1.)) {
                    _magnitude += -2.5 * log10(luminosite);
                }
            }

            // Prise en compte de l'extinction atmospherique
            if (extinction) {
                _magnitude += ExtinctionAtmospherique(observateur, hauteur);
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Determination de l'extinction atmospherique
 */
double Magnitude::ExtinctionAtmospherique(const Observateur &observateur, const double hauteur) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    double corr = 0.;

    /* Corps de la methode */
    if (hauteur >= 0.) {
        const double cosz = cos(PI_SUR_DEUX - hauteur);
        corr = (0.016 + observateur.aray() + observateur.aaer()) / (cosz + 0.025 * exp(-11. * cosz));
    }

    /* Retour */
    return corr;
}


/*
 * Accesseurs
 */
double Magnitude::fractionIlluminee() const
{
    return _fractionIlluminee;
}

double Magnitude::magnitude() const
{
    return _magnitude;
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

