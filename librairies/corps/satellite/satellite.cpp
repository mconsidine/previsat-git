/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2016  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    20 aout 2016
 *
 */

#include <QCoreApplication>
#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "satellite.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


bool Satellite::initCalcul = false;
static QString noradStationSpatiale = "25544";

/* Constructeurs */
Satellite::Satellite()
{
    _eclipseTotale = true;
    _eclipsePartielle = false;
    _eclipseAnnulaire = false;
    _luminositeEclipseSoleil = 1.;
    _luminositeEclipseLune = 1.;
    _phiSoleil = 0.;
    _phiSoleilRefr = 0.;
    _phiTerre = 0.;
    _phiLune = 0.;
    _elongationSoleil = 0.;
    _elongationLune = 0.;

    _ieralt = true;
    _methMagnitude = 'v';
    _nbOrbites = 0;
    _ageTLE = 0.;
    _attenuation = 0.;
    _beta = 0.;
    _delai = 0.;
    _doppler = 0.;
    _fractionIlluminee = 0.;
    _magnitude = 99.;
    _magnitudeStandard = 99.;
    _section = 0.;
    _t1 = 0.;
    _t2 = 0.;
    _t3 = 0.;
}

/* Destructeur */
Satellite::~Satellite()
{
}

/* Methodes */
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

Date Satellite::CalculDateAOSSuiv(const Date &dateCalcul, const Observateur &observateur, QString &ctypeAOS, double &azimAOS, bool &aos)
{
    /* Declarations des variables locales */
    Date dateAOS;

    /* Initialisations */
    ctypeAOS = QObject::tr("AOS");
    Satellite sat = *this;
    Observateur obs = observateur;
    aos = sat.hasAOS(obs);

    /* Corps de la methode */
    if (aos) {

        // Calcul du prochain AOS ou LOS
        double jjm[3], ht[3];
        double periode = NB_JOUR_PAR_MIN;
        if (sat._hauteur >= 0.)
            ctypeAOS = QObject::tr("LOS");

        double tAOS = 0.;
        double t_ht = dateCalcul.jourJulienUTC();

        bool afin = false;
        int iter = 0;
        while (!afin) {

            jjm[0] = t_ht;
            jjm[1] = jjm[0] + 0.5 * periode;
            jjm[2] = jjm[0] + periode;

            for(int i=0; i<3; i++) {

                const Date date(jjm[i], 0., false);

                obs.CalculPosVit(date);

                sat.CalculPosVit(date);
                sat.CalculCoordHoriz(obs, false, false);
                ht[i] = sat._hauteur;
            }

            const bool atst1 = ht[0] * ht[1] < 0.;
            const bool atst2 = ht[1] * ht[2] < 0.;
            if (atst1 || atst2) {

                t_ht = (atst1) ? jjm[1] : jjm[2];

                if (ctypeAOS == QObject::tr("AOS")) {
                    jjm[0] = t_ht - periode;
                    jjm[1] = t_ht - 0.5 * periode;
                    jjm[2] = t_ht;
                } else {
                    jjm[0] = t_ht;
                    jjm[1] = t_ht + 0.5 * periode;
                    jjm[2] = t_ht + periode;
                }

                while (fabs(tAOS - t_ht) > EPS_DATES) {

                    tAOS = t_ht;

                    for(int i=0; i<3; i++) {

                        const Date date(jjm[i], 0., false);

                        obs.CalculPosVit(date);

                        // Position du satellite
                        sat.CalculPosVit(date);
                        sat.CalculCoordHoriz(obs, true, false);
                        ht[i] = sat._hauteur;
                    }

                    t_ht = Maths::CalculValeurXInterpolation3(jjm, ht, 0., EPS_DATES);
                    periode *= 0.5;

                    jjm[0] = t_ht - periode;
                    jjm[1] = t_ht;
                    jjm[2] = t_ht + periode;
                }
                dateAOS = Date(tAOS, 0.);
                obs.CalculPosVit(dateAOS);

                // Position du satellite
                sat.CalculPosVit(dateAOS);
                sat.CalculCoordHoriz(obs, true, false);
                azimAOS = sat._azimut;
                afin = true;
            } else {
                t_ht += periode;
                iter++;

                if (iter > 50000) {
                    afin = true;
                    aos = false;
                }
            }
        }
    }

    /* Retour */
    return (dateAOS);
}

/*
 * Calcul de la date du noeud ascendant precedent a la date donnee
 */
Date Satellite::CalculDateNoeudAscPrec(const Date &date)
{
    /* Declarations des variables locales */
    Date j0 = date;
    Satellite sat = *this;

    /* Initialisations */
    const double st = 1. / (_tle.no() * T360);

    /* Corps de la methode */
    double lat1 = _latitude;
    int i = -1;
    bool atrouve = false;
    while (!atrouve) {

        j0 = Date(date.jourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(j0);

        // Latitude
        const double lat = sat.CalculLatitude(sat.position());

        if (lat1 > 0. && lat < 0.)
            atrouve = true;
        lat1 = lat;

        i--;
    }

    double jjm[3], lati[3];
    bool afin = false;
    double jj0 = j0.jourJulienUTC();
    double periode = st;
    while (!afin) {

        jjm[0] = jj0 - periode;
        jjm[1] = jj0;
        jjm[2] = jj0 + periode;

        for(int j=0; j<3; j++) {

            j0 = Date(jjm[j], 0., false);

            // Position du satellite
            sat.CalculPosVit(j0);

            // Latitude
            lati[j] = sat.CalculLatitude(sat.position());
        }

        const double t_noeudAsc = Maths::CalculValeurXInterpolation3(jjm, lati, 0., EPS_DATES);
        if (fabs(jj0 - t_noeudAsc) < EPS_DATES)
            afin = true;
        jj0 = t_noeudAsc;
        periode *= 0.5;
    }

    /* Retour */
    return (Date(jj0, 0., false));
}

Date Satellite::CalculDateOmbrePenombreSuiv(const Date &dateCalcul, const int nbTrajectoires, const bool acalcEclipseLune,
                                            const bool refraction)
{
    /* Declarations des variables locales */
    Date dateEcl;

    /* Initialisations */
    // Parcours du tableau "trace au sol"
    int i = 0;
    const int dn = (_eclipseTotale) ? 1 : 0;
    if (_traceAuSol.isEmpty())
        CalculTracesAuSol(dateCalcul, nbTrajectoires, acalcEclipseLune, refraction);

    QListIterator<QVector<double> > it(_traceAuSol);
    while (it.hasNext()) {
        const QVector<double> list = it.next();
        if (list.at(3) >= dateCalcul.jourJulienUTC()) {
            const int dn0 = ((int) list.at(2))%2;
            if (dn != dn0)
                it.toBack();
        }
        i++;
    }

    /* Corps de la methode */
    if (i < _traceAuSol.size()) {
        double ecl[3], jjm[3];
        Satellite satellite = *this;
        double t_ecl = _traceAuSol.at(i-1).at(3);
        double periode = t_ecl - _traceAuSol.at(i-2).at(3);

        bool afin = false;
        double tdn = dateCalcul.jourJulienUTC();
        while (!afin) {

            jjm[0] = t_ecl - periode;
            jjm[1] = t_ecl;
            jjm[2] = t_ecl + periode;

            for(int j=0; j<3; j++) {

                const Date date(jjm[j], 0., false);

                // Position du satellite
                satellite.CalculPosVit(date);

                // Position du Soleil
                Soleil soleil;
                soleil.CalculPosition(date);

                // Position de la Lune
                Lune lune;
                if (acalcEclipseLune)
                    lune.CalculPosition(date);

                // Conditions d'eclipse du satellite
                satellite.CalculSatelliteEclipse(soleil, lune, acalcEclipseLune, refraction);
                ecl[j] = (satellite._luminositeEclipseLune < satellite._luminositeEclipseSoleil) ?
                            satellite._phiLune - satellite._phiSoleil - satellite._elongationLune :
                            satellite._phiTerre - satellite._phiSoleilRefr - satellite._elongationSoleil;
            }

            if ((ecl[0] * ecl[2] < 0.) || (ecl[0] > 0. && ecl[2] > 0.))
                tdn = qRound(NB_SEC_PAR_JOUR * Maths::CalculValeurXInterpolation3(jjm, ecl, 0., EPS_DATES)) * NB_JOUR_PAR_SEC;
            periode *= 0.5;
            if (fabs(tdn - t_ecl) < EPS_DATES)
                afin = true;
            t_ecl = tdn;
        }
        dateEcl = Date(t_ecl, 0.);
    } else {
        dateEcl = Date(dateCalcul.jourJulienUTC() - 10., 0.);
    }

    /* Retour */
    return (dateEcl);
}

/*
 * Calcul des elements osculateurs du satellite et du nombre d'orbites
 */
void Satellite::CalculElementsOsculateurs(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _elements.CalculElementsOsculateurs(_position, _vitesse);

    // Nombre d'orbites
    _ageTLE = date.jourJulienUTC() - _tle.epoque().jourJulienUTC();
    _nbOrbites = _tle.nbOrbites() +
            (int) (floor((_tle.no() + _ageTLE * _tle.bstar()) * _ageTLE + modulo(_tle.omegao() + _tle.mo(), DEUX_PI) / T360 -
                         modulo(_elements.argumentPerigee() + _elements.anomalieVraie(), DEUX_PI) / DEUX_PI + 0.5));

    /* Retour */
    return;
}

/*
 * Calcul de la magnitude visuelle du satellite
 */
void Satellite::CalculMagnitude(const Observateur &observateur, const bool extinction, const bool effetEclipsePartielle) {

    /* Declarations des variables locales */

    /* Initialisations */
    _magnitude = 99.;

    /* Corps de la methode */
    if (!_eclipseTotale) {

        // Fraction illuminee
        _fractionIlluminee = 0.5 * (1. + cos(_elongationSoleil));

        // Magnitude
        if (_magnitudeStandard < 99.) {
            _magnitude = _magnitudeStandard - 15.75 + 2.5 * log10(_distance * _distance / _fractionIlluminee);

            // Prise en compte des eclipses partielles ou annulaires
            if (effetEclipsePartielle) {
                const double luminositeEclipse = qMin(_luminositeEclipseLune, _luminositeEclipseSoleil);
                if (luminositeEclipse > 0. && luminositeEclipse <= 1.)
                    _magnitude += -2.5 * log10(luminositeEclipse);
            }

            // Prise en compte de l'extinction atmospherique
            if (extinction) {
                _magnitude += ExtinctionAtmospherique(observateur);
            }
        }
    }

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
        _sgp4.Calcul(date, _tle);
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
void Satellite::CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil, const Lune &lune,
                                            const int nbTracesAuSol, const bool visibilite, const bool extinction,
                                            const bool traceCiel, const bool mcc, const bool refraction, const bool acalcEclipseLune,
                                            const bool effetEclipsePartielle, QList<Satellite> &satellites)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    for (int isat=0; isat<satellites.size(); isat++) {

        // Position ECI du satellite
        satellites[isat].CalculPosVit(date);

        // Coordonnees horizontales du satellite
        satellites[isat].CalculCoordHoriz(observateur);

        // Calcul des conditions d'eclipse
        satellites[isat].CalculSatelliteEclipse(soleil, lune, acalcEclipseLune, refraction);

        // Calcul des coordonnees terrestres
        satellites[isat].CalculCoordTerrestres(observateur);
        satellites[isat]._ieralt = (satellites[isat].altitude() < 0.);

        // Calcul de la zone de visibilite du satellite
        if (visibilite) {
            const double beta = (mcc && satellites[isat]._tle.nom().toLower().startsWith("tdrs")) ?
                        PI_SUR_DEUX + 8.7 * DEG2RAD :
                        acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellites[isat]._altitude)) - 0.5 * REFRACTION_HZ;
            satellites[isat].CalculZoneVisibilite(beta);
        }

        // Calcul de la trajectoire dans le ciel
        if (traceCiel && satellites.at(isat).isVisible()) {
            Observateur obs(observateur);
            satellites[isat].CalculTraceCiel(date, acalcEclipseLune, refraction, obs);
        }

        if (isat == 0) {

            // Calcul des coordonnees equatoriales
            satellites[isat].CalculCoordEquat(observateur);

            // Calcul de la magnitude
            satellites[isat].CalculMagnitude(observateur, extinction, effetEclipsePartielle);

            // Calcul des elements osculateurs et du nombre d'orbites
            satellites[isat].CalculElementsOsculateurs(date);

            // Calcul de la trajectoire
            if (nbTracesAuSol > 0) {

                const Date dateInit = (mcc && satellites.at(isat).tle().norad() == noradStationSpatiale) ?
                            Date(satellites[isat].CalculDateNoeudAscPrec(date).jourJulienUTC() - EPS_DATES, 0.,
                                 false) : Date(date.jourJulienUTC(), 0., false);

                satellites[isat].CalculTracesAuSol(dateInit, nbTracesAuSol, acalcEclipseLune, refraction);
            }

            // Calcul de l'angle beta
            satellites[isat].CalculBeta(soleil);

            // Calcul des proprietes du signal (Doppler@100MHz, attenuation@100MHz et delai)
            satellites[isat].CalculSignal();
        }
    }

    /* Retour */
    return;
}

/*
 * Determination de la condition d'eclipse du satellite
 */
void Satellite::CalculSatelliteEclipse(const Soleil &soleil, const Lune &lune, const bool acalcEclipseLune, const bool refraction)
{
    /* Declarations des variables locales */
    const Lune empty;

    /* Initialisations */
    _luminositeEclipseSoleil = 1.;
    _luminositeEclipseLune = 1.;
    _typeEclipseLune = NON_ECLIPSE;

    /* Corps de la methode */
    if (acalcEclipseLune)
        CalculSatelliteEclipseCorps(soleil, LUNE, lune);
    CalculSatelliteEclipseCorps(soleil, TERRE, empty, refraction);

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

void Satellite::CalculSatelliteEclipseCorps(const Soleil &soleil, const CorpsOccultant &corpsOccultant, const Lune &lune,
                                            const bool refraction)
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

    const Vecteur3D rhoSatSol = soleil.position() - _position;
    const double distSatSol = rhoSatSol.Norme();

    switch (corpsOccultant) {
    default:
    case TERRE:
    {
        rhoSatCorps = _position;
        const double tanlat = _position.z() / sqrt(_position.x() * _position.x() + _position.y() * _position.y());
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
        rhoSatCorps = _position - lune.position();
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
    }

    /* Retour */
    return;
}

/*
 * Calcul de la trace dans le ciel
 */
void Satellite::CalculTraceCiel(const Date &date, const bool acalcEclipseLune, const bool refraction, const Observateur &observateur,
                                const int sec)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;

    /* Initialisations */
    _traceCiel.clear();
    if (_elements.demiGrandAxe() < EPSDBL100)
        CalculElementsOsculateurs(date);

    /* Corps de la methode */
    if (!isGeo()) {

        bool afin = false;
        int i = 0;
        const double step = 1. / (_tle.no() * T360);
        const double st = (sec == 0) ? step : sec * NB_JOUR_PAR_SEC;
        Satellite sat = *this;
        Observateur obs = observateur;

        while (!afin) {

            const Date j0 = Date(date.jourJulienUTC() + i * st, 0., false);

            // Position du satellite
            sat.CalculPosVit(j0);

            // Position de l'observateur
            obs.CalculPosVit(j0);

            // Coordonnees horizontales
            sat.CalculCoordHoriz(obs);

            if (sat._hauteur >= 0. && i < 86400) {

                // Position du Soleil
                soleil.CalculPosition(j0);

                // Position de la Lune
                if (acalcEclipseLune)
                    lune.CalculPosition(j0);

                // Conditions d'eclipse
                sat.CalculSatelliteEclipse(soleil, lune, acalcEclipseLune, refraction);

                const QVector<double> list(QVector<double> () << sat._hauteur << sat._azimut <<
                                           ((sat._eclipseTotale) ? 1. : (sat._eclipsePartielle || sat._eclipseAnnulaire) ? 2. : 0.) <<
                                           j0.jourJulienUTC());
                _traceCiel.append(list);

            } else {
                if (i > 0)
                    afin = true;
            }
            i++;
        }
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
    if (incl >= PI_SUR_DEUX)
        incl = PI - incl;

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

/*
 * Lecture des donnees relatives aux satellites
 */
void Satellite::LectureDonnees(const QStringList &listeSatellites, const QVector<TLE> &tabtle, QList<Satellite> &satellites)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else

#if QT_VERSION >= 0x050000
    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    const QString dirLocalData =
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory).at(0) +
            dirAstr + QDir::separator() + "data";
#else
    const QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

#endif

    const int nb = listeSatellites.size();

    if (!Satellite::initCalcul) {
        satellites.clear();
        QVectorIterator<TLE> it(tabtle);
        while (it.hasNext()) {
            satellites.append(Satellite(it.next()));
        }
        Satellite::initCalcul = true;
    }

    for (int isat=0; isat<nb; isat++) {

        satellites[isat]._cospar = "";
        satellites[isat]._t1 = 0.;
        satellites[isat]._t2 = 0.;
        satellites[isat]._t3 = 0.;
        satellites[isat]._magnitudeStandard = 99.;
        satellites[isat]._methMagnitude = ' ';
        satellites[isat]._section = 0.;
        satellites[isat]._dateLancement = "";
        satellites[isat]._periode = "";
        satellites[isat]._perigee = "";
        satellites[isat]._apogee = "";
        satellites[isat]._inclinaison = "";
        satellites[isat]._categorieOrbite = "";
        satellites[isat]._pays = "";
        satellites[isat]._siteLancement = "";
    }

    /* Corps de la methode */
    const QString fic = dirLocalData + QDir::separator() + "donnees.sat";
    QFile fi(fic);
    if (fi.exists()) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        int j = 0;
        while (!flux.atEnd() && j < nb) {

            const QString ligne = flux.readLine();
            const QString norad = ligne.mid(0, 5);
            for(int isat=0; isat<nb; isat++) {

                if (listeSatellites.at(isat) == norad) {

                    satellites[isat]._cospar = ligne.mid(6, 11);
                    satellites[isat]._t1 = ligne.mid(18, 5).toDouble();
                    satellites[isat]._t2 = ligne.mid(24, 4).toDouble();
                    satellites[isat]._t3 = ligne.mid(29, 4).toDouble();
                    satellites[isat]._magnitudeStandard = ligne.mid(34, 4).toDouble();
                    satellites[isat]._methMagnitude = ligne.at(39).toLatin1();
                    satellites[isat]._section = ligne.mid(41, 6).toDouble();
                    satellites[isat]._dateLancement = ligne.mid(48, 10);
                    satellites[isat]._periode = ligne.mid(70, 10);
                    satellites[isat]._perigee = ligne.mid(81, 7);
                    satellites[isat]._apogee = ligne.mid(89, 7);
                    satellites[isat]._inclinaison = ligne.mid(97, 6);
                    satellites[isat]._categorieOrbite = ligne.mid(104, 6).trimmed();
                    satellites[isat]._pays = ligne.mid(111, 5).trimmed();
                    satellites[isat]._siteLancement = ligne.mid(117, 5).trimmed();
                    j++;
                    break;
                }
            }
        }
    }
    fi.close();

    /* Retour */
    return;
}

void Satellite::CalculSignal()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Decalage Doppler a 100 MHz
    _doppler = -100.e6 * _rangeRate / VITESSE_LUMIERE;

    // Attenuation (free-space path loss) a 100 MHz
    _attenuation = 72.45 + 20. * log10(_distance);

    // Delai du signal en millisecondes (dans le vide)
    _delai = 1000. * _distance / VITESSE_LUMIERE;

    /* Retour */
    return;
}

/*
 * Calcul de la trace au sol du satellite
 */
void Satellite::CalculTracesAuSol(const Date &date, const int nbOrb, const bool acalcEclipseLune, const bool refraction)
{
    /* Declarations des variables locales */
    Satellite sat = *this;
    Soleil soleil;
    Lune lune;

    /* Initialisations */
    const double st = 1. / (_tle.no() * T360);

    /* Corps de la methode */
    _traceAuSol.clear();
    for (int i=0; i<360 * nbOrb; i++) {

        const Date j0(date.jourJulienUTC() + i * st, 0., false);

        // Position du satellite
        sat.CalculPosVit(j0);

        // Longitude
        const Vecteur3D pos = sat._position;
        double lon = RAD2DEG * modulo(PI + atan2(pos.y(), pos.x()) - Observateur::CalculTempsSideralGreenwich(j0), DEUX_PI);
        if (lon < 0.)
            lon += T360;

        // Latitude
        double lat = sat.CalculLatitude(pos);
        lat = RAD2DEG * (PI_SUR_DEUX - lat);

        // Position du Soleil
        soleil.CalculPosition(j0);

        // Position de la Lune
        if (acalcEclipseLune)
            lune.CalculPosition(j0);

        // Conditions d'eclipse
        sat.CalculSatelliteEclipse(soleil, lune, acalcEclipseLune, refraction);

        const QVector<double> list(QVector<double> () << lon << lat <<
                                   ((sat._eclipseTotale) ? 1. : (sat._eclipsePartielle || sat._eclipseAnnulaire) ? 2. : 0) <<
                                   j0.jourJulienUTC());
        _traceAuSol.append(list);
    }

    /* Retour */
    return;
}


/* Accesseurs */
bool Satellite::isEclipseTotale() const
{
    return _eclipseTotale;
}

bool Satellite::isEclipsePartielle() const
{
    return _eclipsePartielle;
}

bool Satellite::isEclipseAnnulaire() const
{
    return _eclipseAnnulaire;
}

TypeEclipse Satellite::typeEclipseSoleil() const
{
    return _typeEclipseSoleil;
}

TypeEclipse Satellite::typeEclipseLune() const
{
    return _typeEclipseLune;
}

double Satellite::luminositeEclipseSoleil() const
{
    return _luminositeEclipseSoleil;
}

double Satellite::luminositeEclipseLune() const
{
    return _luminositeEclipseLune;
}

double Satellite::elongationSoleil() const
{
    return _elongationSoleil;
}

double Satellite::elongationLune() const
{
    return _elongationLune;
}

double Satellite::phiSoleil() const
{
    return _phiSoleil;
}

double Satellite::phiSoleilRefr() const
{
    return _phiSoleilRefr;
}

double Satellite::phiTerre() const
{
    return _phiTerre;
}

double Satellite::phiLune() const
{
    return _phiLune;
}

bool Satellite::isIeralt() const
{
    return _ieralt;
}

double Satellite::ageTLE() const
{
    return _ageTLE;
}

double Satellite::attenuation() const
{
    return _attenuation;
}

double Satellite::beta() const
{
    return _beta;
}

double Satellite::delai() const
{
    return _delai;
}

double Satellite::doppler() const
{
    return _doppler;
}

double Satellite::fractionIlluminee() const
{
    return _fractionIlluminee;
}

double Satellite::magnitude() const
{
    return _magnitude;
}

double Satellite::magnitudeStandard() const
{
    return _magnitudeStandard;
}

char Satellite::methMagnitude() const
{
    return _methMagnitude;
}

char Satellite::method() const
{
    return _sgp4.method();
}

int Satellite::nbOrbites() const
{
    return _nbOrbites;
}

double Satellite::section() const
{
    return _section;
}

double Satellite::t1() const
{
    return _t1;
}

double Satellite::t2() const
{
    return _t2;
}

double Satellite::t3() const
{
    return _t3;
}

TLE Satellite::tle() const
{
    return _tle;
}

QString Satellite::dateLancement() const
{
    return _dateLancement;
}

QString Satellite::categorieOrbite() const
{
    return _categorieOrbite;
}

QString Satellite::pays() const
{
    return _pays;
}

QString Satellite::siteLancement() const
{
    return _siteLancement;
}

ElementsOsculateurs Satellite::elements() const
{
    return _elements;
}

QList<QVector<double> > Satellite::traceAuSol() const
{
    return _traceAuSol;
}

QList<QVector<double> > Satellite::traceCiel() const
{
    return _traceCiel;
}
