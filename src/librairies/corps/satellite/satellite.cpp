/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    1er janvier 2025
 *
 */

#include "librairies/exceptions/exception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "evenements.h"
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
    _nbOrbites = 0;
    _ageElementsOrbitaux = 0.;
    _beta = 0.;
    _sgp4.setInit(false);

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un tableau d'elements orbitaux
 */
Satellite::Satellite(const QList<ElementsOrbitaux> &elem) :
    _listElements(elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nbOrbites = 0;
    _ageElementsOrbitaux = 0.;
    _beta = 0.;
    _elementsOrbitaux = _listElements.first();
    _sgp4.setInit(false);

    /* Retour */
    return;
}


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
    _beta = MATHS::PI_SUR_DEUX - soleil.position().Angle(w);

    /* Retour */
    return;
}

/*
 * Calcul du cercle d'acquisition d'une station
 */
void Satellite::CalculCercleAcquisition(const Observateur &station)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _longitude = station.longitude();
    _latitude = station.latitude();

    /* Corps de la methode */
    const double angleBeta = acos(TERRE::RAYON_TERRESTRE / (TERRE::RAYON_TERRESTRE + _altitude)) - 0.5 * TERRE::REFRACTION_HZ;
    CalculZoneVisibilite(angleBeta);

    /* Retour */
    return;
}

/*
 * Calcul des elements osculateurs
 */
void Satellite::CalculElementsOsculateurs(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Elements osculateurs
    _elementsOsculateurs.Calcul(_position, _vitesse);

    // Age du TLE
    _ageElementsOrbitaux = date.jourJulienUTC() - _elementsOrbitaux.epoque.jourJulienUTC();

    // Nombre d'orbites a la date courante
    const double nb0 = (_elementsOrbitaux.no + _ageElementsOrbitaux * _elementsOrbitaux.bstar) * _ageElementsOrbitaux;
    const double nb1 = modulo(_elementsOrbitaux.omegao + _elementsOrbitaux.mo, MATHS::DEUX_PI) / MATHS::T360;
    const double nb2 = modulo(_elementsOsculateurs.argumentPerigee() + _elementsOsculateurs.anomalieVraie(), MATHS::DEUX_PI) / MATHS::DEUX_PI;
    _nbOrbites = _elementsOrbitaux.nbOrbitesEpoque + static_cast<unsigned int> (floor(nb0 + nb1 - nb2 + 0.5));

    /* Retour */
    return;
}

/*
 * Calcul de la position et de la vitesse du satellite
 * Modele SGP4 : d'apres l'article "Revisiting Spacetrack Report #3: Rev 3" de David Vallado (2006)
 */
void Satellite::CalculPosVit(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!_listElements.isEmpty()) {

        const double jjsav = _elementsOrbitaux.epoque.jourJulienUTC();
        _elementsOrbitaux = _listElements.first();

        // Recherche des elements orbitaux les plus recents
        ElementsOrbitaux elem;
        QListIterator it(_listElements);
        while (it.hasNext()) {

            elem = it.next();

            if (date.jourJulienUTC() >= elem.epoque.jourJulienUTC()) {
                _elementsOrbitaux = elem;
            } else {
                it.toBack();
            }
        }

        // Reinitialisation des valeurs du modele SGP4 en cas de changement d'elements orbitaux
        if (fabs(_elementsOrbitaux.epoque.jourJulienUTC() - jjsav) > DATE::EPS_DATES) {
            _sgp4.setInit(false);
        }
    }

    // Calcul de la position et de la vitesse
    try {
        _sgp4.Calcul(date, _elementsOrbitaux);
        _position = _sgp4.position();
        _vitesse = _sgp4.vitesse();

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

/*
 * Calcul de la position d'une liste de satellites
 */
void Satellite::CalculPosVitListeSatellites(const Date &date,
                                            const Observateur &observateur,
                                            const Soleil &soleil,
                                            const Lune &lune,
                                            const int nbTracesAuSol,
                                            const bool acalcEclipseLune,
                                            const bool effetEclipsePartielle,
                                            const bool extinction,
                                            const bool refractionAtmospherique,
                                            const bool traceCiel,
                                            const bool visibilite,
                                            const bool isISS,
                                            const bool mcc,
                                            QList<Satellite> &satellites)

{
    /* Declarations des variables locales */

    /* Initialisations */
    Observateur obs(observateur);

    /* Corps de la methode */
    for(int i=0; i<satellites.size(); i++) {

        // Position ECI du satellite
        satellites[i].CalculPosVit(date);

        // Coordonnees horizontales du satellite
        satellites[i].CalculCoordHoriz(observateur, true, refractionAtmospherique);

        // Calcul des conditions d'eclipse
        satellites[i]._conditionEclipse.CalculSatelliteEclipse(satellites[i]._position, soleil, &lune, refractionAtmospherique);

        // Coordonnees terrestres du satellite
        satellites[i].CalculCoordTerrestres(observateur);

        // Calcul de la zone de visibilite du satellite
        if (visibilite) {
            const double bt = (mcc && satellites[i]._elementsOrbitaux.nom.toLower().startsWith("tdrs")) ?
                                  MATHS::PI_SUR_DEUX + 8.7 * MATHS::DEG2RAD :
                                  acos(TERRE::RAYON_TERRESTRE / (TERRE::RAYON_TERRESTRE + satellites[i]._altitude)) - 0.5 * TERRE::REFRACTION_HZ;
            satellites[i].CalculZoneVisibilite(bt);
        }

        // Calcul de la trajectoire dans le ciel
        if (traceCiel && satellites.at(i)._visible) {
            satellites[i].CalculTraceCiel(date, acalcEclipseLune, refractionAtmospherique, obs);
        }

        if (i == 0) {

            // Coordonnees equatoriales
            satellites[i].CalculCoordEquat(observateur);

            // Magnitude et fraction illuminee
            satellites[i]._magnitude.Calcul(satellites[i]._conditionEclipse, observateur, satellites[i]._distance, satellites[i]._hauteur,
                                            satellites[i]._elementsOrbitaux.donnees.magnitudeStandard(), extinction, effetEclipsePartielle);

            // Elements osculateurs
            satellites[i].CalculElementsOsculateurs(date);

            // Calcul des traces au sol
            if (nbTracesAuSol > 0) {

                const Date dateISS =
                    Date(Evenements::CalculNoeudOrbite(date, satellites[i], SensCalcul::ANTI_CHRONOLOGIQUE).jourJulienUTC() - DATE::EPS_DATES, 0., false);

                const Date dateInit = (mcc && isISS) ? dateISS : Date(date.jourJulienUTC(), 0., false);

                satellites[i].CalculTracesAuSol(dateInit, nbTracesAuSol, acalcEclipseLune, refractionAtmospherique);
            }

            // Calcul du phasage
            satellites[i]._phasage.Calcul(satellites[i]._elementsOsculateurs, satellites[i]._elementsOrbitaux.no);

            // Calcul de l'angle beta
            satellites[i].CalculBeta(soleil);

            // Calcul des proprietes du signal (Doppler@100MHz, attenuation@100MHz et delai)
            satellites[i]._signal.Calcul(satellites[i]._rangeRate, satellites[i]._distance);
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul de la trace dans le ciel
 */
void Satellite::CalculTraceCiel(const Date &date,
                                const bool acalcEclipseLune,
                                const bool refraction,
                                const Observateur &observateur,
                                const int sec,
                                const double jj1)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;

    /* Initialisations */
    _traceCiel.clear();
    if (_elementsOsculateurs.demiGrandAxe() < MATHS::EPSDBL100) {
        CalculElementsOsculateurs(date);
    }

    /* Corps de la methode */
    if (!isGeo()) {

        bool afin = false;
        int i = 0;
        const double step = 1. / (_elementsOrbitaux.no * MATHS::T360);
        const double st = (sec == 0) ? step : sec * DATE::NB_JOUR_PAR_SEC;
        Satellite sat = *this;
        Observateur obs = observateur;
        ElementsTraceCiel elem;

        while (!afin) {

            const Date j0 = Date(date.jourJulienUTC() + i * st, 0., false);

            // Position du satellite
            sat.CalculPosVit(j0);

            // Position de l'observateur
            obs.CalculPosVit(j0);

            // Coordonnees horizontales
            sat.CalculCoordHoriz(obs, true);

            if ((sat._hauteur >= 0.) && (i < static_cast<int> (DATE::NB_SEC_PAR_JOUR))) {

                // Position du Soleil
                soleil.CalculPositionSimp(j0);

                // Position de la Lune
                if (acalcEclipseLune) {
                    lune.CalculPositionSimp(j0);
                }

                // Conditions d'eclipse
                sat._conditionEclipse.CalculSatelliteEclipse(sat._position, soleil, &lune, refraction);

                elem.azimut = sat._azimut;
                elem.hauteur = sat._hauteur;
                elem.jourJulienUTC = j0.jourJulienUTC();
                elem.eclipseTotale = sat._conditionEclipse.eclipseTotale();
                elem.eclipsePartielle = (sat._conditionEclipse.eclipseAnnulaire() || sat._conditionEclipse.eclipsePartielle());
                _traceCiel.append(elem);

            } else if (i > 0) {
                afin = true;
            }

            if (j0.jourJulienUTC() > jj1) {
                afin = true;
            }

            i++;
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul des traces au sol
 */
void Satellite::CalculTracesAuSol(const Date &dateInit,
                                  const int nbOrb,
                                  const bool acalcEclipseLune,
                                  const bool refraction)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;
    ElementsTraceSol elem;

    /* Initialisations */
    Satellite sat = *this;
    const double st = 1. / (_elementsOrbitaux.no * MATHS::T360);
    _traceAuSol.clear();

    /* Corps de la methode */
    for(int i=0; i<360 * nbOrb; i++) {

        const Date date(dateInit.jourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(date);

        // Longitude
        const Vecteur3D pos = sat._position;
        elem.longitude = MATHS::RAD2DEG * modulo(MATHS::PI + atan2(pos.y(), pos.x()) - Observateur::CalculTempsSideralGreenwich(date), MATHS::DEUX_PI);
        if (elem.longitude < 0.) {
            elem.longitude += MATHS::T360;
        }

        // Latitude
        sat.CalculLatitude();
        elem.latitude = sat._latitude;
        elem.latitude = MATHS::RAD2DEG * (MATHS::PI_SUR_DEUX - elem.latitude);

        // Position du Soleil
        soleil.CalculPositionSimp(date);

        // Position de la Lune
        if (acalcEclipseLune) {
            lune.CalculPositionSimp(date);
        }

        // Conditions d'eclipse
        sat._conditionEclipse.CalculSatelliteEclipse(pos, soleil, &lune, refraction);

        elem.jourJulienUTC = date.jourJulienUTC();
        elem.eclipseTotale = sat._conditionEclipse.eclipseTotale();
        elem.eclipsePartielle = (sat._conditionEclipse.eclipseAnnulaire() || sat._conditionEclipse.eclipsePartielle());
        _traceAuSol.append(elem);
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
    double incl = _elementsOrbitaux.inclo * MATHS::DEG2RAD;
    if (incl >= MATHS::PI_SUR_DEUX) {
        incl = MATHS::PI - incl;
    }

    /* Corps de la methode */

    /* Retour */
    return (incl + acos(TERRE::RAYON_TERRESTRE / (TERRE::RAYON_TERRESTRE + _elementsOsculateurs.apogee())) > fabs(observateur.latitude()) && !isGeo());
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
    return (fabs(_elementsOrbitaux.no - 1.0027) < 2.e-4);
}


/*
 * Accesseurs
 */
double Satellite::ageElementsOrbitaux() const
{
    return _ageElementsOrbitaux;
}

double Satellite::beta() const
{
    return _beta;
}

char Satellite::method() const
{
    return _sgp4.method();
}

unsigned int Satellite::nbOrbites() const
{
    return _nbOrbites;
}

ElementsOrbitaux Satellite::elementsOrbitaux() const
{
    return _elementsOrbitaux;
}

ConditionEclipse Satellite::conditionEclipse() const
{
    return _conditionEclipse;
}

ElementsOsculateurs Satellite::elementsOsculateurs() const
{
    return _elementsOsculateurs;
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

QList<ElementsTraceCiel> Satellite::traceCiel() const
{
    return _traceCiel;
}


/*
 * Modificateurs
 */
void Satellite::setConditionEclipse(const ConditionEclipse &condEcl)
{
    _conditionEclipse = condEcl;
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

