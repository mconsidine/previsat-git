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
 * >    magnitude.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul de la magnitude du satellite artificiel
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

#include "magnitude.h"


/* Constructeurs */
Magnitude::Magnitude()
{
    _fractionIlluminee = 0.;
    _magnitude = 99.;
}

/* Methodes */
/*
 * Calcul de la magnitude visuelle du satellite
 */
void Magnitude::Calcul(const Observateur &observateur, const ConditionEclipse &conditionEclipse, const double distance, const double hauteur,
                       const double magnitudeStandard, const bool extinction, const bool effetEclipsePartielle) {

    /* Declarations des variables locales */

    /* Initialisations */
    _magnitude = 99.;

    /* Corps de la methode */
    if (!conditionEclipse.isEclipseTotale()) {

        // Fraction illuminee
        _fractionIlluminee = 0.5 * (1. + cos(conditionEclipse.elongationSoleil()));

        // Magnitude
        if (magnitudeStandard < 99.) {
            _magnitude = magnitudeStandard - 15.75 + 2.5 * log10(distance * distance / _fractionIlluminee);

            // Prise en compte des eclipses partielles ou annulaires
            if (effetEclipsePartielle) {
                const double luminositeEclipse = qMin(conditionEclipse.luminositeEclipseLune(), conditionEclipse.luminositeEclipseSoleil());
                if (luminositeEclipse > 0. && luminositeEclipse <= 1.)
                    _magnitude += -2.5 * log10(luminositeEclipse);
            }

            // Prise en compte de l'extinction atmospherique
            if (extinction) {
                _magnitude += ExtinctionAtmospherique(hauteur, observateur);
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Determination de l'extinction atmospherique, issu de l'article
 * "Magnitude corrections for atmospheric extinction" de Daniel Green, 1992
 */
double Magnitude::ExtinctionAtmospherique(const double hauteur, const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double corr = 0.;

    /* Corps de la methode */
    if (hauteur >= 0.) {

        const double cosz = cos(PI_SUR_DEUX - hauteur);
        const double x = 1. / (cosz + 0.025 * exp(-11. * cosz));
        corr = x * (0.016 + observateur.aray() + observateur.aaer());
    }

    /* Retour */
    return (corr);
}


/* Accesseurs */
double Magnitude::fractionIlluminee() const
{
    return _fractionIlluminee;
}

double Magnitude::magnitude() const
{
    return _magnitude;
}
