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
 * >    planete.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 janvier 2012
 *
 * Date de revision
 * >    15 juin 2023
 *
 */

#include <QCoreApplication>
#include <cmath>
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "planete.h"
#include "soleil.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Planete::Planete()
{
    _indice = IndicePlanete::MERCURE;
    _elem.fill(0.);
}

/*
 * Constructeur avec l'identifiant de la planete
 */
Planete::Planete(const IndicePlanete &iplanete)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _indice = iplanete;
    _elem.fill(0.);

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la position d'une planete
 */
void Planete::CalculPositionSimp(const Date &date,
                                 const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Vecteur3D solpos = soleil.position() * CORPS::KM2UA;

    /* Corps de la methode */
    // Calcul des elements orbitaux
    CalculElements(date);

    // Calcul des coordonnees spheriques heliocentriques
    CalculCoordonneesSpheriques();

    // Coordonnees cartesiennes equatoriales
    _position = Sph2Cart(_positionSph, date);

    // Position geocentrique
    _dist = _position + solpos;

    // Distance geocentrique (UA)
    _distance = _dist.Norme();

    // Prise en compte de l'aberration
    const double jj2 = date.jourJulienTT() - 0.0057755183 * _distance;
    const Date date2(jj2, 0., false);

    CalculElements(date2);
    CalculCoordonneesSpheriques();
    _position = (Sph2Cart(_positionSph, date2) + solpos) * CORPS::UA2KM;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
QString Planete::nom() const
{
    return QCoreApplication::translate("planet", PLANETE::nomPlanetes[_indice]);
}

IndicePlanete Planete::indice() const
{
    return _indice;
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
 * Calcul des elements orbitaux moyens d'une planete
 */
void Planete::CalculElements(const Date &date)
{
    /* Declarations des variables locales */
    double tt[4];

    /* Initialisations */
    tt[0] = 1.;
    tt[1] = date.jourJulienTT() * DATE::NB_SIECJ_PAR_JOURS;
    tt[2] = tt[1] * tt[1];
    tt[3] = tt[2] * tt[1];

    /* Corps de la methode */
    // Calcul des elements orbitaux
    for(unsigned int i=0; i<PLANETE::NB_ELEMENTS; i++) {
        _elem[i] = 0.;
        for(unsigned int j=0; j<PLANETE::NB_DEGRES_SIMP; j++) {
            _elem[i] += PLANETE::tabPlanetes[static_cast<int> (_indice)][i][j] * tt[j];
        }
    }

    // Reduction des elements
    _elem[0] = fmod(_elem[0], MATHS::T360) * MATHS::DEG2RAD;
    for(unsigned int i=3; i<PLANETE::NB_ELEMENTS; i++) {
        _elem[i] *= MATHS::DEG2RAD;
    }

    /* Retour */
    return;
}

/*
 * Calcul des coordonnees spheriques ecliptiques d'une planete
 */
void Planete::CalculCoordonneesSpheriques()
{
    /* Declarations des variables locales */
    double u1;

    /* Initialisations */
    const double longMoy = _elem[0];
    const double demiGrandAxe = _elem[1];
    const double excentricite = _elem[2];
    const double incl = _elem[3];
    const double cosincl = cos(incl);
    const double longNoeudAsc = _elem[4];
    const double longPerihelie = _elem[5];

    /* Corps de la methode */
    // Anomalie moyenne
    double na = atan(tan(longPerihelie - longNoeudAsc) / cosincl);
    if (cos(longPerihelie - longNoeudAsc) < 0.) {
        na += MATHS::PI;
    }

    double nm = atan(tan(longMoy - longNoeudAsc) / cosincl);
    if (cos(longMoy - longNoeudAsc - nm) < 0.) {
        nm += MATHS::PI;
    }
    const double m = nm - na;

    // Anomalie excentrique (equation de Kepler)
    double u = m;
    do {
        u1 = u;
        u = u1 + (m + excentricite * sin(u1) - u1) / (1. - excentricite * cos(u1));
    } while (fabs(u - u1) > 1.e-9);

    // Anomalie vraie
    const double v = 2. * atan(sqrt((1. + excentricite) / (1. - excentricite)) * tan(0.5 * u));

    // Longitude ecliptique vraie
    double nl = atan(tan(na + v) * cosincl);
    if (cos(na + v - nl) < 0.) {
        nl += MATHS::PI;
    }
    const double l = longNoeudAsc + nl;

    // Latitude ecliptique
    const double b = atan(sin(nl) * tan(incl));

    // Rayon vecteur
    const double r = demiGrandAxe * (1. - excentricite * cos(u));

    // Position
    _positionSph = Vecteur3D(l, b, r);

    /* Retour */
    return;
}
