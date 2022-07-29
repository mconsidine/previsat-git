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
 * >    signal.cpp
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
 * >
 *
 */

#include <cmath>
#include "librairies/corps/systemesolaire/soleilconst.h"
#include "signal.h"



/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Signal::Signal()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _attenuation = -999999.;
    _delai = -999999.;
    _doppler = -999999.;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des elements du signal
 */
void Signal::Calcul(const double rangeRate, const double distance)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Decalage Doppler a 100 MHz
    _doppler = -100.e6 * rangeRate / VITESSE_LUMIERE;

    // Attenuation (free-space path loss) a 100 MHz
    _attenuation = 72.45 + 20. * log10(distance);

    // Delai du signal en millisecondes (dans le vide)
    _delai = 1000. * distance / VITESSE_LUMIERE;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
double Signal::attenuation() const
{
    return _attenuation;
}

double Signal::delai() const
{
    return _delai;
}

double Signal::doppler() const
{
    return _doppler;
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

