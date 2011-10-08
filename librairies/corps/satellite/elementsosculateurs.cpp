/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    elementsosculateurs.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Definition des elements osculateurs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >
 *
 */

#include "elementsosculateurs.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

ElementsOsculateurs::ElementsOsculateurs()
{
}

ElementsOsculateurs::~ElementsOsculateurs()
{
}

/*
 * Calcul des elements osculateurs
 * D'apres les formules de Fundamental Astrodynamics and applications, 2nd edition, D. Vallado
 */
void ElementsOsculateurs::CalculElementsOsculateurs(Vecteur3D &position, Vecteur3D &vitesse)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double temp1 = 1. / GE;
    const double p = position.Norme();
    const double v = vitesse.Norme();
    const double temp2 = 1. / p;
    const double v2 = v * v;
    const double rv = position * vitesse;

    /* Corps de la methode */
    // Demi-grand axe
    _demiGrandAxe = 1. / (2. * temp2 - v2 * temp1);

    // Excentricite
    Vecteur3D exc = (position * (v2 * temp1 - temp2)) - (vitesse * (rv * temp1));
    _excentricite = exc.Norme();

    // Inclinaison
    Vecteur3D h = position ^ vitesse;
    _inclinaison = acos(h.getZ() / h.Norme());

    // Ascension droite du noeud ascendant
    Vecteur3D n = Vecteur3D(-h.getY(), h.getX(), 0.);
    _ascDroiteNA = acos(n.getX() / n.Norme());
    if (n.getY() < 0.)
        _ascDroiteNA = DEUX_PI - _ascDroiteNA;

    // Argument du perigee
    const double ne = n * exc;
    _argumentPerigee = acos(ne / (n.Norme() * _excentricite));
    if (exc.getZ() < 0.)
        _argumentPerigee = DEUX_PI - _argumentPerigee;

    // Anomalie vraie
    double er = exc * position;
    _anomalieVraie = acos(er * temp2 / _excentricite);
    if (rv < 0.)
        _anomalieVraie = DEUX_PI - _anomalieVraie;

    // Anomalie excentrique
    _anomalieExcentrique = 2. * atan(sqrt((1. - _excentricite) / (1. + _excentricite)) * tan(0.5 * _anomalieVraie));
    if (_anomalieExcentrique < 0.)
        _anomalieExcentrique += DEUX_PI;

    // Anomalie moyenne
    _anomalieMoyenne = _anomalieExcentrique - _excentricite * sin(_anomalieExcentrique);
    if (_anomalieMoyenne < 0.)
        _anomalieMoyenne += DEUX_PI;

    const double alpha = _demiGrandAxe / RAYON_TERRESTRE;
    const double alpha2 = alpha * alpha;
    const double beta = 1. - _excentricite * _excentricite;
    const double gamma = 1. + _excentricite * cos(_argumentPerigee);
    const double gamma2 = gamma * gamma;
    const double temp3 = sin(_inclinaison);
    const double nn0 = 1. - 1.5 * J2 * ((2. - 2.5 * temp3 * temp3) / (alpha2 * sqrt(beta) * gamma2) + gamma2 * gamma / (alpha2 * beta * beta * beta));

    // Apogee, perigee, periode orbitale
    _apogee = _demiGrandAxe * (1. + _excentricite);
    _perigee = _demiGrandAxe * (1. - _excentricite);
    _periode = nn0 * DEUX_PI * sqrt(_demiGrandAxe * _demiGrandAxe * _demiGrandAxe / GE) * NB_HEUR_PAR_SEC;

    /* Retour */
    return;
}

/* Accesseurs */
double ElementsOsculateurs::getAnomalieExcentrique() const
{
    return _anomalieExcentrique;
}

double ElementsOsculateurs::getAnomalieMoyenne() const
{
    return _anomalieMoyenne;
}

double ElementsOsculateurs::getAnomalieVraie() const
{
    return _anomalieVraie;
}

double ElementsOsculateurs::getApogee() const
{
    return _apogee;
}

double ElementsOsculateurs::getArgumentPerigee() const
{
    return _argumentPerigee;
}

double ElementsOsculateurs::getAscensionDroiteNA() const
{
    return _ascDroiteNA;
}

double ElementsOsculateurs::getDemiGrandAxe() const
{
    return _demiGrandAxe;
}

double ElementsOsculateurs::getExcentricite() const
{
    return _excentricite;
}

double ElementsOsculateurs::getInclinaison() const
{
    return _inclinaison;
}

double ElementsOsculateurs::getPerigee() const
{
    return _perigee;
}

double ElementsOsculateurs::getPeriode() const
{
    return _periode;
}
