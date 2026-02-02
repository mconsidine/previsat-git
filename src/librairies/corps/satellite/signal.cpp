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
 * >    25 fevrier 2023
 *
 */

#include <cmath>
#include "librairies/corps/corpsconst.h"
#include "librairies/maths/mathsconst.h"
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
    _attenuation = 0.;
    _delai = 0.;
    _doppler = 0.;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des elements du signal
 */
void Signal::Calcul(const double rangeRate,
                    const double distance,
                    const double frequence)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if ((distance > MATHS::EPSDBL100) && (fabs(rangeRate) > MATHS::EPSDBL100) && (frequence > MATHS::EPSDBL100)) {

        // Delai en secondes
        const double delaiSec = distance / CORPS::VITESSE_LUMIERE;

        /* Corps de la methode */
        // Decalage Doppler (Hz)
        _doppler = -frequence * rangeRate / CORPS::VITESSE_LUMIERE;

        // Attenuation (free-space path loss) avec la formule exacte, en dB
        _attenuation = 20. * log10(4. * MATHS::PI * delaiSec * frequence);

        // Delai du signal en millisecondes (dans le vide)
        _delai = 1000. * delaiSec;
    }

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

