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
 * >    conditioneclipse.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul de la condition d'eclipse du satellite artificiel
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 septembre 2016
 *
 * Date de revision
 * >    16 decembre 2018
 *
 */

#include "conditioneclipse.h"


/* Constructeurs */
ConditionEclipse::ConditionEclipse() :
    _typeEclipseSoleil(NON_ECLIPSE), _typeEclipseLune(NON_ECLIPSE)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _eclipseTotale = false;
    _eclipsePartielle = false;
    _eclipseAnnulaire = false;
    _luminositeEclipseSoleil = 0.;
    _luminositeEclipseLune = 0.;
    _phiSoleil = 0.;
    _phiSoleilRefr = 0.;
    _phiTerre = 0.;
    _phiLune = 0.;
    _elongationSoleil = 0.;
    _elongationLune = 0.;

    /* Retour */
    return;
}

/* Methodes */
/*
 * Determination de la condition d'eclipse du satellite
 */
void ConditionEclipse::CalculSatelliteEclipse(const Soleil &soleil, const Lune &lune, const Vecteur3D &position, const bool acalcEclipseLune,
                                              const bool refraction)
{
    /* Declarations des variables locales */
    const Lune empty;

    /* Initialisations */
    _luminositeEclipseSoleil = 1.;
    _luminositeEclipseLune = 1.;
    _typeEclipseLune = NON_ECLIPSE;

    /* Corps de la methode */
    if (acalcEclipseLune) {
        CalculSatelliteEclipseCorps(soleil, position, LUNE, lune);
    }
    CalculSatelliteEclipseCorps(soleil, position, TERRE, empty, refraction);

    if (_typeEclipseSoleil != NON_ECLIPSE && _typeEclipseLune != NON_ECLIPSE) {

        if (_luminositeEclipseSoleil < _luminositeEclipseLune) {
            _typeEclipseLune = NON_ECLIPSE;
            _luminositeEclipseLune = 1.;
        } else {
            _typeEclipseSoleil = NON_ECLIPSE;
            _luminositeEclipseSoleil = 1.;
        }
    }

    _eclipseTotale = (_typeEclipseSoleil == ECLIPSE_TOTALE || _typeEclipseLune == ECLIPSE_TOTALE);
    _eclipsePartielle = (_typeEclipseSoleil == ECLIPSE_PARTIELLE || _typeEclipseLune == ECLIPSE_PARTIELLE);
    _eclipseAnnulaire = (_typeEclipseSoleil == ECLIPSE_ANNULAIRE || _typeEclipseLune == ECLIPSE_ANNULAIRE);

    /* Retour */
    return;
}

void ConditionEclipse::CalculSatelliteEclipseCorps(const Soleil &soleil, const Vecteur3D &position, const CorpsOccultant &corpsOccultant,
                                                   const Lune &lune, const bool refraction)
{
    /* Declarations des variables locales */
    double rayonCorps;
    Vecteur3D rhoSatCorps;

    /* Initialisations */
    TypeEclipse *typeEclipse = NULL;
    double *elongation = NULL;
    double *luminosite = NULL;
    double *phiCorps = NULL;
    double *phiSol = NULL;

    const Vecteur3D rhoSatSol = soleil.position() - position;
    const double distSatSol = rhoSatSol.Norme();

    switch (corpsOccultant) {
    default:
    case TERRE:
    {
        rhoSatCorps = position;
        const double tanlat = position.z() / sqrt(position.x() * position.x() + position.y() * position.y());
        const double u = atan(tanlat / (1. - APLA));
        const double cu = cos(u);
        const double su = sin(u);
        rayonCorps = RAYON_TERRESTRE * sqrt(cu * cu + G2 * su * su);

        typeEclipse = &_typeEclipseSoleil;
        elongation = &_elongationSoleil;
        luminosite = &_luminositeEclipseSoleil;
        phiCorps = &_phiTerre;
        phiSol = &_phiSoleilRefr;
        break;
    }

    case LUNE:
        rhoSatCorps = position - lune.position();
        rayonCorps = RAYON_LUNAIRE;
        typeEclipse = &_typeEclipseLune;
        elongation = &_elongationLune;
        luminosite = &_luminositeEclipseLune;
        phiCorps = &_phiLune;
        phiSol = &_phiSoleil;
        break;
    }
    const double distSatCorps = rhoSatCorps.Norme();

    /* Corps de la methode */
    *phiSol = asin(RAYON_SOLAIRE / distSatSol);
    if (std::isnan(*phiSol)) {
        *phiSol = PI_SUR_DEUX;
    } else {
        if (corpsOccultant == TERRE && refraction)
            *phiSol += REFRACTION_HZ;
    }

    *phiCorps = asin(rayonCorps / distSatCorps);
    if (std::isnan(*phiCorps))
        *phiCorps = PI_SUR_DEUX;

    *elongation = (-rhoSatCorps).Angle(rhoSatSol);

    if (((*phiSol + *phiCorps) <= *elongation) || distSatSol <= distSatCorps) {
        *typeEclipse = NON_ECLIPSE;
        *luminosite = 1.;

    } else if ((*phiCorps - *phiSol) >= *elongation) {
        *typeEclipse = ECLIPSE_TOTALE;
        *luminosite = 0.;

    } else if ((*phiSol - *phiCorps) >= *elongation) {
        *typeEclipse = ECLIPSE_ANNULAIRE;
        const double cps = cos(*phiSol);
        const double cpc = cos(*phiCorps);
        *luminosite = 1. - (1. - cpc) / (1. - cps);

    } else {
        *typeEclipse = ECLIPSE_PARTIELLE;
        const double cps = cos(*phiSol);
        const double cpc = cos(*phiCorps);
        const double cth = cos(*elongation);
        const double sps = sin(*phiSol);
        const double spc = sin(*phiCorps);
        const double sth = sin(*elongation);

        const double tmp1 = cps * acos((cpc - cps * cth) / (sps * sth));
        const double tmp2 = cpc * acos((cps - cpc * cth) / (spc * sth));
        const double tmp3 = acos((cth - cps * cpc) / (sps * spc));
        *luminosite = 1. - (PI - tmp1 - tmp2 - tmp3) / (PI * (1. - cps));
        if (*luminosite > 1.)
            *luminosite = 1.;
    }

    /* Retour */
    return;
}


/* Accesseurs */
bool ConditionEclipse::isEclipseTotale() const
{
    return _eclipseTotale;
}

bool ConditionEclipse::isEclipsePartielle() const
{
    return _eclipsePartielle;
}

bool ConditionEclipse::isEclipseAnnulaire() const
{
    return _eclipseAnnulaire;
}

TypeEclipse ConditionEclipse::typeEclipseSoleil() const
{
    return _typeEclipseSoleil;
}

TypeEclipse ConditionEclipse::typeEclipseLune() const
{
    return _typeEclipseLune;
}

double ConditionEclipse::luminositeEclipseSoleil() const
{
    return _luminositeEclipseSoleil;
}

double ConditionEclipse::luminositeEclipseLune() const
{
    return _luminositeEclipseLune;
}

double ConditionEclipse::elongationSoleil() const
{
    return _elongationSoleil;
}

double ConditionEclipse::elongationLune() const
{
    return _elongationLune;
}

double ConditionEclipse::phiSoleil() const
{
    return _phiSoleil;
}

double ConditionEclipse::phiSoleilRefr() const
{
    return _phiSoleilRefr;
}

double ConditionEclipse::phiTerre() const
{
    return _phiTerre;
}

double ConditionEclipse::phiLune() const
{
    return _phiLune;
}
