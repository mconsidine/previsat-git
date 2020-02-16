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
 * >    satellite.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires lies a la position des satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    30 decembre 2018
 *
 */

#include <cmath>
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "satellite.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Satellite::Satellite()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ieralt = true;
    _nbOrbites = 0;
    _ageTLE = 0.;
    _beta = 0.;
    _deltaNbOrb = -1;

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un tableau de TLE
 */
Satellite::Satellite(const QVector<TLE> &tabtle) :
    _tabtle(tabtle)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ieralt = true;
    _nbOrbites = 0;
    _ageTLE = 0.;
    _beta = 0.;
    _deltaNbOrb = -1;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
bool Satellite::isIeralt() const
{
    return _ieralt;
}

double Satellite::ageTLE() const
{
    return _ageTLE;
}

double Satellite::beta() const
{
    return _beta;
}

char Satellite::method() const
{
    return _sgp4.method();
}

int Satellite::deltaNbOrb() const
{
    return _deltaNbOrb;
}

unsigned int Satellite::nbOrbites() const
{
    return _nbOrbites;
}

TLE Satellite::tle() const
{
    return _tle;
}

ConditionEclipse Satellite::conditionEclipse() const
{
    return _conditionEclipse;
}

ElementsOsculateurs Satellite::elements() const
{
    return _elements;
}

Magnitude Satellite::magnitude() const
{
    return _magnitude;
}

Phasage Satellite::phasage() const
{
    return _phasage;
}

Signal Satellite::signal() const
{
    return _signal;
}

QList<ElementsTraceSol> Satellite::traceAuSol() const
{
    return _traceAuSol;
}

//QList<QVector<double> > Satellite::traceCiel() const
//{
//    return _traceCiel;
//}


/*
 * Methodes publiques
 */
/*
 * Calcul de l'angle beta (angle entre le plan de l'orbite et la direction du Soleil)
 */
void Satellite::CalculBeta(const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Vecteur3D w = _position ^ _vitesse;

    /* Corps de la methode */
    _beta = PI_SUR_DEUX - soleil.position().Angle(w);

    /* Retour */
    return;
}

/*
 * Calcul de la position et de la vitesse du satellite
 * Modele SGP4 : d'apres l'article "Revisiting Spacetrack Report #3: Rev 1" de David Vallado (2006)
 */
void Satellite::CalculPosVit(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (!_tabtle.isEmpty()) {

            const double jjsav = _tle.dateDebutValidite().jourJulienUTC();
            _tle = _tabtle.at(0);
            QVectorIterator<TLE> it(_tabtle);
            while (it.hasNext()) {

                const TLE xtle = it.next();
                if (date.jourJulienUTC() >= xtle.dateDebutValidite().jourJulienUTC()) {
                    _tle = xtle;
                } else {
                    it.toBack();
                }
            }
            if (fabs(_tle.dateDebutValidite().jourJulienUTC() - jjsav) > EPS_DATES) {
                _sgp4.setInit(false);
            }
        }

        _sgp4.Calcul(date, _tle);
        _position = _sgp4.position();
        _vitesse = _sgp4.vitesse();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Satellite::CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil, const Lune &lune, QList<Satellite> &satellites,
                                            const int nbTracesAuSol, const bool acalcEclipseLune, const bool effetEclipsePartielle, const bool extinction,
                                            const bool refraction)

{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    for(int i=0; i<satellites.size(); i++) {

        // Position ECI du satellite
        satellites[i].CalculPosVit(date);

        // Coordonnees horizontales du satellite
        satellites[i].CalculCoordHoriz(observateur);

        // Calcul des conditions d'eclipse
        satellites[i]._conditionEclipse.CalculSatelliteEclipse(satellites[i]._position, soleil, lune, refraction);

        // Coordonnees terrestres du satellite
        satellites[i].CalculCoordTerrestres(observateur);

        if (i == 0) {

            // Coordonnees equatoriales
            satellites[i].CalculCoordEquat(observateur);

            // Magnitude et fraction illuminee
            satellites[i]._magnitude.Calcul(satellites[i]._conditionEclipse, observateur, satellites[i]._distance, satellites[i]._hauteur,
                                            satellites[i]._tle.donnees().magnitudeStandard(), extinction, effetEclipsePartielle);

            // Elements osculateurs
            satellites[i].CalculElementsOsculateurs(date);

            // Calcul des traces au sol
            if (nbTracesAuSol > 0) {
                // TODO
                const Date dateInit = date;
                satellites[i].CalculTracesAuSol(dateInit, nbTracesAuSol, acalcEclipseLune, refraction);
            }

            // Calcul du phasage
            satellites[i]._phasage.Calcul(satellites[i]._elements, satellites[i].tle().no());

            // Calcul de l'angle beta
            satellites[i].CalculBeta(soleil);

            // Calcul des proprietes du signal (Doppler@100MHz, attenuation@100MHz et delai)
            satellites[i]._signal.Calcul(satellites[i].rangeRate(), satellites[i].distance());
        }
    }

    /* Retour */
    return;
}

void Satellite::CalculTracesAuSol(const Date &dateInit, const int nbOrb, const bool acalcEclipseLune, const bool refraction)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;

    /* Initialisations */
    Satellite sat = *this;
    const double st = 1. / (_tle.no() * T360);
    _traceAuSol.clear();

    /* Corps de la methode */
    for(int i=0; i<360 * nbOrb; i++) {

        ElementsTraceSol elements;
        const Date date(dateInit.jourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(date);

        // Longitude
        const Vecteur3D pos = sat._position;
        elements.longitude = RAD2DEG * modulo(PI + atan2(pos.y(), pos.x()) - Observateur::CalculTempsSideralGreenwich(date), DEUX_PI);
        if (elements.longitude < 0.) {
            elements.longitude += T360;
        }

        // Latitude
        elements.latitude = sat.CalculLatitude(pos);
        elements.latitude = RAD2DEG * (PI_SUR_DEUX - elements.latitude);

        // Position du Soleil
        soleil.CalculPosition(date);

        // Position de la Lune
        if (acalcEclipseLune) {
            lune.CalculPosition(date);
        }

        // Conditions d'eclipse
        sat._conditionEclipse.CalculSatelliteEclipse(pos, soleil, lune, refraction);

        elements.jourJulienUTC = date.jourJulienUTC();
        elements.eclipseTotale = sat._conditionEclipse.eclipseTotale();
        elements.eclipsePartielle = (sat._conditionEclipse.eclipseAnnulaire() || sat._conditionEclipse.eclipsePartielle());
        _traceAuSol.append(elements);
    }

    /* Retour */
    return;
}

/*
 * Determination si le satellite peut se lever (ou se coucher) dans le ciel de l'observateur
 */
bool Satellite::hasAOS(const Observateur &observateur) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    double incl = _tle.inclo() * DEG2RAD;
    if (incl >= PI_SUR_DEUX) {
        incl = PI - incl;
    }

    /* Corps de la methode */

    /* Retour */
    return (incl + acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + _elements.apogee())) > fabs(observateur.latitude()) && !isGeo());
}

/*
 * Determination si le satellite est geosynchrone
 */
bool Satellite::isGeo() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return (fabs(_tle.no() - 1.0027) < 2.e-4);
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
void Satellite::CalculElementsOsculateurs(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Elements osculateurs
    _elements.Calcul(_position, _vitesse);

    // Age du TLE
    _ageTLE = date.jourJulienUTC() - _tle.epoque().jourJulienUTC();

    // Nombre d'orbites a la date courante
    if (_deltaNbOrb == -1) {

        const QString dateLancement = _tle.donnees().dateLancement();
        if (dateLancement.isEmpty()) {
            _deltaNbOrb = 0;
        } else {
            const int annee = dateLancement.mid(0, 4).toInt();
            const int mois = dateLancement.mid(5, 2).toInt();
            const double jour = dateLancement.mid(8, 2).toDouble();
            const Date dateLct(annee, mois, jour, 0.);

            // Nombre theorique d'orbites a l'epoque
            const int nbOrbTheo = static_cast<int> (_tle.no() * (_tle.epoque().jourJulienUTC() - dateLct.jourJulienUTC()));
            int resteOrb = nbOrbTheo%100000;
            resteOrb += (((_tle.nbOrbitesEpoque() > 50000) && (resteOrb < 50000)) ? 100000 : 0);
            resteOrb -= (((_tle.nbOrbitesEpoque() < 50000) && (resteOrb > 50000)) ? 100000 : 0);
            _deltaNbOrb = nbOrbTheo - resteOrb;
        }
    }

    _nbOrbites = _tle.nbOrbitesEpoque() +
            static_cast<unsigned int> (_deltaNbOrb + floor((_tle.no() + _ageTLE * _tle.bstar()) * _ageTLE + modulo(_tle.omegao() + _tle.mo(), DEUX_PI) / T360 -
                         modulo(_elements.argumentPerigee() + _elements.anomalieVraie(), DEUX_PI) / DEUX_PI + 0.5));

    /* Retour */
    return;
}
