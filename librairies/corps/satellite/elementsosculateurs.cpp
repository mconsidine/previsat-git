/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2014  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    3 novembre 2013
 *
 */

#include "elementsosculateurs.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

static const double temp1 = 1. / GE;

ElementsOsculateurs::ElementsOsculateurs()
{    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _demiGrandAxe = 0.;
    _excentricite = 0.;
    _inclinaison = 0.;
    _ascDroiteNA = 0.;
    _argumentPerigee = 0.;
    _anomalieVraie = 0.;
    _anomalieMoyenne = 0.;
    _anomalieExcentrique = 0.;
    _apogee = 0.;
    _perigee = 0.;
    _periode = 0.;

    _exCEq = 0.;
    _exCirc = 0.;
    _eyCEq = 0.;
    _eyCirc = 0.;
    _pso = 0.;
    _ix = 0.;
    _iy = 0.;
    _argumentLongitudeVraie = 0.;

    /* Retour */
    return;
}

/*
 * Destructeur
 */
ElementsOsculateurs::~ElementsOsculateurs()
{
}

/*
 * Calcul des elements osculateurs pour une orbite elliptique
 * D'apres les formules de Fundamental Astrodynamics and applications, 2nd edition, D. Vallado
 * Trajectoires spatiales, O. Zarrouati
 */
void ElementsOsculateurs::CalculElementsOsculateurs(Vecteur3D &position, Vecteur3D &vitesse)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double p = position.Norme();

    /* Corps de la methode */
    if (p > EPSDBL100) {

        const double v = vitesse.Norme();
        const double temp2 = 1. / p;
        const double v2 = v * v;
        const double rv = position * vitesse;

        // Demi-grand axe
        _demiGrandAxe = 1. / (2. * temp2 - v2 * temp1);

        // Excentricite
        const Vecteur3D exc = (position * (v2 * temp1 - temp2)) - (vitesse * (rv * temp1));
        _excentricite = exc.Norme();

        // Inclinaison
        Vecteur3D h = position ^ vitesse;
        const double ci = h.getZ() / h.Norme();
        _inclinaison = acos(ci);

        // Ascension droite du noeud ascendant
        const Vecteur3D n = Vecteur3D(-h.getY(), h.getX(), 0.);
        const double ca = n.getX() / n.Norme();
        _ascDroiteNA = acos(ca);
        if (n.getY() < 0.)
            _ascDroiteNA = DEUX_PI - _ascDroiteNA;

        // Argument du perigee
        const double ne = n * exc;
        _argumentPerigee = acos(ne / (n.Norme() * _excentricite));
        if (exc.getZ() < 0.)
            _argumentPerigee = DEUX_PI - _argumentPerigee;

        // Anomalie vraie
        const double er = exc * position;
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
        const double nn0 = 1. - 1.5 * J2 * ((2. - 2.5 * temp3 * temp3) / (alpha2 * sqrt(beta) * gamma2) + gamma2 * gamma /
                                            (alpha2 * beta * beta * beta));

        // Apogee, perigee, periode orbitale
        _apogee = _demiGrandAxe * (1. + _excentricite);
        _perigee = _demiGrandAxe * (1. - _excentricite);
        _periode = nn0 * DEUX_PI * sqrt(_demiGrandAxe * _demiGrandAxe * _demiGrandAxe * temp1) * NB_HEUR_PAR_SEC;

        // Calcul des parametres adaptes
        const Vecteur3D hn = h.Normalise();
        const double d = 1. / (1. + hn.getZ());
        _ix = -d * hn.getY();
        _iy = d * hn.getX();

        const double cl = (position.getX() - d * position.getZ() * hn.getX()) / p;
        const double sl = (position.getY() - d * position.getZ() * hn.getY()) / p;
        _argumentLongitudeVraie = atan2(sl, cl);
        if (_argumentLongitudeVraie < 0.)
            _argumentLongitudeVraie += DEUX_PI;

        const double se = rv / sqrt(GE * _demiGrandAxe);
        const double ce = p * v2 / GE - 1.;
        const double e2 = _excentricite * _excentricite;
        const double tmp0 = sqrt(1. - e2);
        const double f = ce - e2;
        const double g = tmp0 * se;
        const double tmp1 = _demiGrandAxe / p;
        _exCEq = tmp1 * (f * cl + g * sl);
        _eyCEq = tmp1 * (f * sl - g * cl);

        const double sa = sin(_ascDroiteNA);

        _exCirc = _exCEq * ca + _eyCEq * sa;
        _eyCirc = _eyCEq * ca - _exCEq * sa;

        const Vecteur3D r = Vecteur3D(ca, sa, 0.);
        const Vecteur3D s = Vecteur3D(-ci * sa, ci * ca, sin(_inclinaison));

        const double xr = position * r;
        const double xs = position * s;

        const double bt = 1. / (1. + tmp0);
        const double ae = atan2(xs / _demiGrandAxe + _eyCirc - se * bt * _exCirc, xr / _demiGrandAxe + _exCirc - se * bt * _eyCirc);
        _pso = ae - _exCirc * sin(ae) + _eyCirc * cos(ae);
        if (_pso < 0.)
            _pso += DEUX_PI;
    }

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

double ElementsOsculateurs::getExCEq() const
{
    return _exCEq;
}

double ElementsOsculateurs::getExCirc() const
{
    return _exCirc;
}

double ElementsOsculateurs::getEyCEq() const
{
    return _eyCEq;
}

double ElementsOsculateurs::getEyCirc() const
{
    return _eyCirc;
}

double ElementsOsculateurs::getInclinaison() const
{
    return _inclinaison;
}

double ElementsOsculateurs::getIx() const
{
    return _ix;
}

double ElementsOsculateurs::getIy() const
{
    return _iy;
}

double ElementsOsculateurs::getArgumentLongitudeVraie() const
{
    return _argumentLongitudeVraie;
}

double ElementsOsculateurs::getPerigee() const
{
    return _perigee;
}

double ElementsOsculateurs::getPeriode() const
{
    return _periode;
}

double ElementsOsculateurs::getPso() const
{
    return _pso;
}
