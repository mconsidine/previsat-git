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
 * >    elementsosculateurs.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
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

#include "elementsosculateurs.h"
#include "librairies/corps/terreconst.h"
#include "librairies/maths/mathsconst.h"
#include "librairies/maths/vecteur3d.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
ElementsOsculateurs::ElementsOsculateurs()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _demiGrandAxe = 0.;
    _excentricite = 0.;
    _inclinaison = 0.;
    _ascensionDroiteNoeudAscendant = 0.;
    _argumentPerigee = 0.;
    _anomalieVraie = 0.;
    _anomalieExcentrique = 0.;
    _anomalieMoyenne = 0.;
    _apogee = 0.;
    _perigee = 0.;
    _periode = 0.;
    _exCirc = 0.;
    _eyCirc = 0.;
    _exCEq = 0.;
    _eyCEq = 0.;
    _pso = 0.;
    _ix = 0.;
    _iy = 0.;
    _argumentLongitudeVraie = 0.;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des elements osculateurs pour une orbite elliptique
 */
void ElementsOsculateurs::Calcul(const Vecteur3D &position, const Vecteur3D &vitesse)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double p = position.Norme();

    /* Corps de la methode */
    if (p > MATHS::EPSDBL100) {

        const double v = vitesse.Norme();
        const double x1sp = 1. / p;
        const double v2 = v * v;
        const double v2t = v2 * TERRE::X1GE;
        const double rv = position * vitesse;

        // Demi-grand axe
        _demiGrandAxe = 1. / (2. * x1sp - v2t);

        // Excentricite
        const Vecteur3D exc = (position * (v2t - x1sp)) - (vitesse * (rv * TERRE::X1GE));
        _excentricite = exc.Norme();

        // Inclinaison orbitale
        const Vecteur3D h = position ^ vitesse;
        const double cosincl = h.z() / h.Norme();
        _inclinaison = acos(cosincl);

        // Ascension droite du noeud ascendant
        const Vecteur3D n(-h.y(), h.x(), 0.);
        const double nn = n.Norme();
        const double cosasc = n.x() / nn;
        _ascensionDroiteNoeudAscendant = acos(cosasc);
        if (n.y() < 0.) {
            _ascensionDroiteNoeudAscendant = MATHS::DEUX_PI - _ascensionDroiteNoeudAscendant;
        }

        // Argument du perigee
        const double ne = n * exc;
        _argumentPerigee = acos(ne / (nn * _excentricite));
        if (exc.z() < 0.) {
            _argumentPerigee = MATHS::DEUX_PI - _argumentPerigee;
        }

        // Anomalie vraie
        const double er = exc * position;
        _anomalieVraie = acos(er * x1sp / _excentricite);
        if (rv < 0.) {
            _anomalieVraie = MATHS::DEUX_PI - _anomalieVraie;
        }

        // Anomalie excentrique
        const double x1mexc = 1. - _excentricite;
        const double x1pexc = 1. + _excentricite;
        _anomalieExcentrique = 2. * atan(sqrt(x1mexc / x1pexc) * tan(0.5 * _anomalieVraie));
        if (_anomalieExcentrique < 0.) {
            _anomalieExcentrique += MATHS::DEUX_PI;
        }

        // Anomalie moyenne
        _anomalieMoyenne = _anomalieExcentrique - _excentricite * sin(_anomalieExcentrique);
        if (_anomalieMoyenne < 0.) {
            _anomalieMoyenne += MATHS::DEUX_PI;
        }

        const double exc2 = _excentricite * _excentricite;
        const double alpha = _demiGrandAxe / TERRE::RAYON_TERRESTRE;
        const double alpha2 = alpha * alpha;
        const double beta = 1. - exc2;
        const double sqbeta = sqrt(beta);
        const double gamma = 1. + _excentricite * cos(_argumentPerigee);
        const double gamma2 = gamma * gamma;
        const double sinincl = sin(_inclinaison);
        const double nn0 = 1. - 1.5 * TERRE::J2 * ((2. - 2.5 * sinincl * sinincl) / (alpha2 * sqbeta * gamma2) + gamma2 * gamma /
                                                   (alpha2 * beta * beta * beta));

        // Apogee/perigee/periode
        _apogee = _demiGrandAxe * x1pexc;
        _perigee = _demiGrandAxe * x1mexc;
        _periode = nn0 * MATHS::DEUX_PI * sqrt(_demiGrandAxe * _demiGrandAxe * _demiGrandAxe * TERRE::X1GE) * DATE::NB_HEUR_PAR_SEC;

        // Calcul des parametres adaptes
        const Vecteur3D hn = h.Normalise();
        const double d = 1. / (1. + hn.z());
        _ix = -d * hn.y();
        _iy = d * hn.x();

        const double temp4 = d * position.z();
        const double cl = (position.x() - temp4 * hn.x()) * x1sp;
        const double sl = (position.y() - temp4 * hn.y()) * x1sp;
        _argumentLongitudeVraie = atan2(sl, cl);
        if (_argumentLongitudeVraie < 0.) {
            _argumentLongitudeVraie += MATHS::DEUX_PI;
        }

        const double se = rv / sqrt(TERRE::GE * _demiGrandAxe);
        const double ce = p * v2 / TERRE::GE - 1.;
        const double f = ce - exc2;
        const double g = sqbeta * se;
        const double tmp1 = _demiGrandAxe * x1sp;
        _exCEq = tmp1 * (f * cl + g * sl);
        _eyCEq = tmp1 * (f * sl - g * cl);

        const double sinasc = sin(_ascensionDroiteNoeudAscendant);

        _exCirc = _exCEq * cosasc + _eyCEq * sinasc;
        _eyCirc = _eyCEq * cosasc - _exCEq * sinasc;

        const Vecteur3D r(cosasc, sinasc, 0.);
        const Vecteur3D s(-cosincl * sinasc, cosincl * cosasc, sinincl);

        const double xr = position * r;
        const double xs = position * s;

        const double bt = 1. / (1. + sqbeta);
        const double temp5 = se * bt;
        const double ae = atan2(xs / _demiGrandAxe + _eyCirc - temp5 * _exCirc, xr / _demiGrandAxe + _exCirc - temp5 * _eyCirc);
        _pso = ae - _exCirc * sin(ae) + _eyCirc * cos(ae);
        if (_pso < 0.) {
            _pso += MATHS::DEUX_PI;
        }
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
double ElementsOsculateurs::demiGrandAxe() const
{
    return _demiGrandAxe;
}

double ElementsOsculateurs::excentricite() const
{
    return _excentricite;
}

double ElementsOsculateurs::inclinaison() const
{
    return _inclinaison;
}

double ElementsOsculateurs::ascensionDroiteNoeudAscendant() const
{
    return _ascensionDroiteNoeudAscendant;
}

double ElementsOsculateurs::argumentPerigee() const
{
    return _argumentPerigee;
}

double ElementsOsculateurs::anomalieVraie() const
{
    return _anomalieVraie;
}

double ElementsOsculateurs::anomalieExcentrique() const
{
    return _anomalieExcentrique;
}

double ElementsOsculateurs::anomalieMoyenne() const
{
    return _anomalieMoyenne;
}

double ElementsOsculateurs::apogee() const
{
    return _apogee;
}

double ElementsOsculateurs::perigee() const
{
    return _perigee;
}

double ElementsOsculateurs::periode() const
{
    return _periode;
}

double ElementsOsculateurs::exCirc() const
{
    return _exCirc;
}

double ElementsOsculateurs::eyCirc() const
{
    return _eyCirc;
}

double ElementsOsculateurs::exCEq() const
{
    return _exCEq;
}

double ElementsOsculateurs::eyCEq() const
{
    return _eyCEq;
}

double ElementsOsculateurs::pso() const
{
    return _pso;
}

double ElementsOsculateurs::ix() const
{
    return _ix;
}

double ElementsOsculateurs::iy() const
{
    return _iy;
}

double ElementsOsculateurs::argumentLongitudeVraie() const
{
    return _argumentLongitudeVraie;
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

