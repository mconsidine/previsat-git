/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2018  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    phasage.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des indices du phasage du satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    6 octobre 2016
 *
 * Date de revision
 * >
 *
 */

#include <cmath>
#include "librairies/corps/systemesolaire/TerreConstants.h"
#include "librairies/maths/maths.h"
#include "phasage.h"


/* Constructeurs */
Phasage::Phasage()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nbOrb = -99;
    _ct0 = -99;
    _dt0 = -99;
    _nu0 = -99;

    /* Retour */
    return;
}

/* Methodes */
void Phasage::Calcul(const ElementsOsculateurs &elements, const double &n0)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double cosincl = cos(elements.inclinaison());
    const double cosincl2 = cosincl * cosincl;
    const double rsa = RAYON_TERRESTRE / elements.demiGrandAxe();
    const double rsa2 = rsa * rsa;
    const double moyenMouvement = sqrt(GE / (elements.demiGrandAxe() * elements.demiGrandAxe() * elements.demiGrandAxe()));
    const double aper = NB_MIN_PAR_JOUR / n0;
    const double gamma = 1. - elements.excentricite() * elements.excentricite();
    const double gamma2 = gamma * gamma;

    /* Corps de la methode */
    const double precessionNoeud = RAD2DEG * NB_SEC_PAR_JOUR * (-1.5 * J2 * rsa2 * cosincl * moyenMouvement / gamma2);
    const double precessionPerigee = RAD2DEG * NB_SEC_PAR_JOUR * 0.75 * J2 * rsa2 * (5. * cosincl2 - 1.) * moyenMouvement / gamma2;
    const double dper = aper / (1. + precessionPerigee * DEG2RAD * NB_JOUR_PAR_SEC / moyenMouvement);
    const double nd = n0 * T360 + precessionPerigee;
    const double k = nd / (OMEGA0 * T360 - precessionNoeud);
    _nu0 = (int) qRound(floor(10. * k) * 0.1);
    const double v = n0 * aper / dper;

    bool atrouve = false;
    const double pas = 1.e-6;
    double spec = 0.01;
    double nt0 = 0.;

    while (!atrouve && spec <= 0.05) {
        int iter = 1;
        while (!atrouve && iter <= 38) {
            nt0 = iter * v;
            if (fabs(nt0 - qRound(nt0)) < spec) {
                _ct0 = iter;
                atrouve = true;
            }
            iter++;
        }
        spec += pas;
    }

    if (atrouve) {
        if (precessionNoeud < 0.975 || precessionNoeud > 0.995) {
            _dt0 = (int) qRound(nt0 - _nu0 * _ct0);
        } else {
            _dt0 = (int) qRound((v - _nu0) * _ct0);
        }
        _nbOrb = (int) qRound(nt0);
    } else {
        _nbOrb = -99;
        _ct0 = -99;
        _dt0 = -99;
        _nu0 = -99;
    }

    /* Retour */
    return;
}

/* Accesseurs */
int Phasage::ct0() const
{
    return _ct0;
}

int Phasage::dt0() const
{
    return _dt0;
}

int Phasage::nu0() const
{
    return _nu0;
}

int Phasage::nbOrb() const
{
    return _nbOrb;
}
