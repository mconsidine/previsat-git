/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    15 octobre 2022
 *
 */

#include "librairies/exceptions/previsatexception.h"
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
    _deltaNbOrb = -1;
    _sgp4.setInit(false);

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un tableau de TLE
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
    _deltaNbOrb = NB_ORB_INDEFINI;
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
    _beta = PI_SUR_DEUX - soleil.position().Angle(w);

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
    const double angleBeta = acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + _altitude)) - 0.5 * REFRACTION_HZ;
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
    if (_deltaNbOrb == NB_ORB_INDEFINI) {

        const QString dateLancement = _elementsOrbitaux.donnees.dateLancement();
        if (dateLancement.isEmpty()) {
            _deltaNbOrb = 0;
        } else {

            const int annee = dateLancement.mid(0, 4).toInt();
            const int mois = dateLancement.mid(5, 2).toInt();
            const double jour = dateLancement.mid(8, 2).toDouble();
            const Date dateLct(annee, mois, jour, 0.);

            // Nombre theorique d'orbites a l'epoque
            const int nbOrbTheo = static_cast<int> (_elementsOrbitaux.no * (_elementsOrbitaux.epoque.jourJulienUTC() - dateLct.jourJulienUTC()));
            int resteOrb = nbOrbTheo%100000;
            resteOrb += (((_elementsOrbitaux.nbOrbitesEpoque > 50000) && (resteOrb < 50000)) ? 100000 : 0);
            resteOrb -= (((_elementsOrbitaux.nbOrbitesEpoque < 50000) && (resteOrb > 50000)) ? 100000 : 0);
            _deltaNbOrb = nbOrbTheo - resteOrb;
        }
    }

    _nbOrbites = _elementsOrbitaux.nbOrbitesEpoque +
            static_cast<unsigned int> (_deltaNbOrb +
                                       floor((_elementsOrbitaux.no + _ageElementsOrbitaux * _elementsOrbitaux.bstar) * _ageElementsOrbitaux +
                                             modulo(_elementsOrbitaux.omegao + _elementsOrbitaux.mo, DEUX_PI) / T360 -
                                             modulo(_elementsOsculateurs.argumentPerigee() + _elementsOsculateurs.anomalieVraie(), DEUX_PI) /
                                             DEUX_PI + 0.5));

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

        if (!_listElements.isEmpty()) {

            const double jjsav = _elementsOrbitaux.epoque.jourJulienUTC();
            _elementsOrbitaux = _listElements.first();

            // Recherche du TLE le plus recent
            QListIterator it(_listElements);
            while (it.hasNext()) {

                const ElementsOrbitaux elem = it.next();

                if (date.jourJulienUTC() >= elem.epoque.jourJulienUTC()) {
                    _elementsOrbitaux = elem;
                } else {
                    it.toBack();
                }
            }

            // Reinitialisation des valeurs du modele SGP4 en cas de changement de TLE
            if (fabs(_elementsOrbitaux.epoque.jourJulienUTC() - jjsav) > EPS_DATES) {
                _sgp4.setInit(false);
            }
        }

        // Calcul de la position et de la vitesse
        _sgp4.Calcul(date, _elementsOrbitaux);
        _position = _sgp4.position();
        _vitesse = _sgp4.vitesse();

    } catch (PreviSatException &e) {
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
        satellites[i]._conditionEclipse.CalculSatelliteEclipse(satellites[i]._position, soleil, lune, refractionAtmospherique);

        // Coordonnees terrestres du satellite
        satellites[i].CalculCoordTerrestres(observateur);

        // Calcul de la zone de visibilite du satellite
        if (visibilite) {
            const double bt = (mcc && satellites[i]._elementsOrbitaux.nom.toLower().startsWith("tdrs")) ?
                        PI_SUR_DEUX + 8.7 * DEG2RAD :
                        acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellites[i]._altitude)) - 0.5 * REFRACTION_HZ;
            satellites[i].CalculZoneVisibilite(bt);
        }

        // Calcul de la trajectoire dans le ciel
        if (traceCiel && satellites.at(i).isVisible()) {
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
                        Date(Evenements::CalculNoeudOrbite(date, satellites[i], SensCalcul::ANTI_CHRONOLOGIQUE).jourJulienUTC() - EPS_DATES, 0., false);

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
void Satellite::CalculTraceCiel(const Date &date, const bool acalcEclipseLune, const bool refraction, const Observateur &observateur, const int sec)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;

    /* Initialisations */
    _traceCiel.clear();
    if (_elementsOsculateurs.demiGrandAxe() < EPSDBL100) {
        CalculElementsOsculateurs(date);
    }

    /* Corps de la methode */
    if (!isGeo()) {

        bool afin = false;
        int i = 0;
        const double step = 1. / (_elementsOrbitaux.no * T360);
        const double st = (sec == 0) ? step : sec * NB_JOUR_PAR_SEC;
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
            sat.CalculCoordHoriz(obs);

            if ((sat._hauteur >= 0.) && (i < 86400)) {

                // Position du Soleil
                soleil.CalculPosition(j0);

                // Position de la Lune
                if (acalcEclipseLune) {
                    lune.CalculPosition(j0);
                }

                // Conditions d'eclipse
                sat._conditionEclipse.CalculSatelliteEclipse(sat._position, soleil, lune, refraction);

                elem.azimut = sat.azimut();
                elem.hauteur = sat.hauteur();
                elem.jourJulienUTC = j0.jourJulienUTC();
                elem.eclipseTotale = sat._conditionEclipse.eclipseTotale();
                elem.eclipsePartielle = (sat._conditionEclipse.eclipseAnnulaire() || sat._conditionEclipse.eclipsePartielle());
                _traceCiel.append(elem);

            } else {
                if (i > 0) {
                    afin = true;
                }
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
void Satellite::CalculTracesAuSol(const Date &dateInit, const int nbOrb, const bool acalcEclipseLune, const bool refraction)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;
    ElementsTraceSol elem;

    /* Initialisations */
    Satellite sat = *this;
    const double st = 1. / (_elementsOrbitaux.no * T360);
    _traceAuSol.clear();

    /* Corps de la methode */
    for(int i=0; i<360 * nbOrb; i++) {

        const Date date(dateInit.jourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(date);

        // Longitude
        const Vecteur3D pos = sat._position;
        elem.longitude = RAD2DEG * modulo(PI + atan2(pos.y(), pos.x()) - Observateur::CalculTempsSideralGreenwich(date), DEUX_PI);
        if (elem.longitude < 0.) {
            elem.longitude += T360;
        }

        // Latitude
        elem.latitude = sat.CalculLatitude(pos);
        elem.latitude = RAD2DEG * (PI_SUR_DEUX - elem.latitude);

        // Position du Soleil
        soleil.CalculPosition(date);

        // Position de la Lune
        if (acalcEclipseLune) {
            lune.CalculPosition(date);
        }

        // Conditions d'eclipse
        sat._conditionEclipse.CalculSatelliteEclipse(pos, soleil, lune, refraction);

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
    double incl = _elementsOrbitaux.inclo * DEG2RAD;
    if (incl >= PI_SUR_DEUX) {
        incl = PI - incl;
    }

    /* Corps de la methode */

    /* Retour */
    return (incl + acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + _elementsOsculateurs.apogee())) > fabs(observateur.latitude()) && !isGeo());
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

int Satellite::deltaNbOrb() const
{
    return _deltaNbOrb;
}

unsigned int Satellite::nbOrbites() const
{
    return _nbOrbites;
}

const ElementsOrbitaux &Satellite::elementsOrbitaux() const
{
    return _elementsOrbitaux;
}

const ConditionEclipse &Satellite::conditionEclipse() const
{
    return _conditionEclipse;
}

const ElementsOsculateurs &Satellite::elementsOsculateurs() const
{
    return _elementsOsculateurs;
}

const Magnitude &Satellite::magnitude() const
{
    return _magnitude;
}

const Phasage &Satellite::phasage() const
{
    return _phasage;
}

const Signal &Satellite::signal() const
{
    return _signal;
}

const QList<ElementsTraceSol> &Satellite::traceAuSol() const
{
    return _traceAuSol;
}

const QList<ElementsTraceCiel> &Satellite::traceCiel() const
{
    return _traceCiel;
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

