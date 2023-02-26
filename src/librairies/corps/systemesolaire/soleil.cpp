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
 * >    soleil.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    25 fevrier 2023
 *
 */

#include <cmath>
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "librairies/maths/matrice3d.h"
#include "librairies/observateur/observateur.h"
#include "soleil.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Soleil::Soleil()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _distanceUA = 0.;

    /* Retour */
    return;
}

Soleil::Soleil(const Vecteur3D &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _position = pos;
    _distanceUA = pos.Norme();

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des heures de lever/coucher/passage au meridien/crepuscules pour une date donnee
 */
void Soleil::CalculLeverMeridienCoucher(const Date &date, const Observateur &observateur, const DateSysteme &syst)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Ephemerides eph;

    /* Initialisations */
    Observateur obs = observateur;
    Date dateCalcul(date.annee(), date.mois(), date.jour() - date.offsetUTC(), date.offsetUTC());
    const Date dateFin(dateCalcul.jourJulienUTC() + 1., date.offsetUTC(), false);
    _ephem.clear();

    /* Corps de la methode */
    do {

        obs.CalculPosVit(dateCalcul);

        soleil.CalculPosition(dateCalcul);
        soleil.CalculCoordHoriz(obs, true, true, true);

        eph.jourJulienUTC = dateCalcul.jourJulienUTC();
        eph.hauteur = soleil.hauteur();
        eph.azimut = soleil.azimut();

        _ephem.append(eph);

        dateCalcul = Date(dateCalcul.jourJulienUTC() + DATE::NB_JOUR_PAR_MIN, 0., false);

    } while (dateCalcul.jourJulienUTC() <= dateFin.jourJulienUTC());

    Corps::CalculLeverMeridienCoucher(date, syst);

    /* Retour */
    return;
}

/*
 * Calcul de la position du Soleil a partir du modele simplifie
 * de l'Astronomical Algorithms 2nd edition de Jean Meeus, p163-164
 */
void Soleil::CalculPosition(const Date &date)
{
    /* Declarations des variables locales */
    double u1;

    /* Initialisations */
    const double t = date.jourJulienTT() * DATE::NB_SIECJ_PAR_JOURS;

    /* Corps de la methode */
    // Longitude moyenne
    const double ls = MATHS::DEG2RAD * modulo(280.466457 + t * (36000.7698278 + 0.00030322 * t), MATHS::T360);

    // Longitude du perihelie
    const double lp = MATHS::DEG2RAD * modulo(282.937348 + t * (1.7195366 + 0.00045688 * t), MATHS::T360);

    // Excentricite
    const double e = 0.016708634 - t * (4.2037e-5 + 1.267e-7 * t);

    // Anomalie moyenne
    const double ms = ls - lp;

    // Resolution de l'equation de Kepler : u = ms + e sin(u)
    double u = ms;
    do {
        u1 = u;
        u = u1 + (ms + e * sin(u1) - u1) / (1. - e * cos(u1));
    } while (fabs(u - u1) > 1.e-9);

    // Anomalie vraie
    const double v = 2. * atan(sqrt((1. + e) / (1. - e)) * tan(0.5 * u));

    // Rayon vecteur
    _distanceUA = SOLEIL::DEMI_GRAND_AXE_TERRE * (1. - e * cos(u));

    // Longitude apparente
    _lonEcl = lp + v - MATHS::DEG2RAD * 0.00569;
    const Vecteur3D pos(_lonEcl, 0., _distanceUA);

    // Position cartesienne equatoriale
    _position = Sph2Cart(pos, date) * SOLEIL::UA2KM;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
double Soleil::distanceUA() const
{
    return _distanceUA;
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

