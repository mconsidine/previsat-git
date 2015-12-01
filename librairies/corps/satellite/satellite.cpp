/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    30 novembre 2015
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
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "satellite.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


bool Satellite::initCalcul = false;

/* Constructeurs */
Satellite::Satellite()
{
    _eclipse = true;
    _ieralt = true;
    _penombre = false;
    _methMagnitude = 'v';
    _nbOrbites = 0;
    _ageTLE = 0.;
    _attenuation = 0.;
    _beta = 0.;
    _delai = 0.;
    _doppler = 0.;
    _elongation = 0.;
    _fractionIlluminee = 0.;
    _magnitude = 99.;
    _magnitudeStandard = 99.;
    _rayonOmbre = 0.;
    _rayonPenombre = 0.;
    _section = 0.;
    _t1 = 0.;
    _t2 = 0.;
    _t3 = 0.;
}

Satellite::Satellite(const TLE &xtle)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _eclipse = true;
    _ieralt = true;
    _penombre = false;
    _methMagnitude = 'v';
    _nbOrbites = 0;
    _ageTLE = 0.;
    _attenuation = 0.;
    _beta = 0.;
    _delai = 0.;
    _doppler = 0.;
    _elongation = 0.;
    _fractionIlluminee = 0.;
    _magnitude = 99.;
    _magnitudeStandard = 99.;
    _rayonOmbre = 0.;
    _rayonPenombre = 0.;
    _section = 0.;
    _t1 = 0.;
    _t2 = 0.;
    _t3 = 0.;
    _tle = xtle;

    /* Retour */
    return;
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
void Satellite::CalculMagnitude(const Observateur &observateur, const bool extinction) {

    /* Declarations des variables locales */

    /* Initialisations */
    _magnitude = 99.;

    /* Corps de la methode */
    if (!_eclipse) {

        // Fraction illuminee
        _fractionIlluminee = 0.5 * (1. + cos(_elongation));

        // Magnitude
        if (_magnitudeStandard < 99.) {
            _magnitude = _magnitudeStandard - 15.75 + 2.5 * log10(_distance * _distance / _fractionIlluminee);

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
void Satellite::CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil,
                                            const int nbTracesAuSol, const bool visibilite, const bool extinction,
                                            const bool traceCiel, const bool mcc, const bool refraction, QList<Satellite> &satellites)
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
        satellites[isat].CalculSatelliteEclipse(soleil, refraction);

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
            satellites[isat].CalculTraceCiel(date, refraction, obs);
        }

        if (isat == 0) {

            // Calcul des coordonnees equatoriales
            satellites[isat].CalculCoordEquat(observateur);

            // Calcul de la magnitude
            satellites[isat].CalculMagnitude(observateur, extinction);

            // Calcul des elements osculateurs et du nombre d'orbites
            satellites[isat].CalculElementsOsculateurs(date);

            // Calcul de la trajectoire
            if (nbTracesAuSol > 0) {

                const Date dateInit = (mcc && satellites.at(isat).tle().norad() == "25544") ?
                        Date(satellites[isat].CalculDateNoeudAscPrec(date).jourJulienUTC() - EPS_DATES, 0.,
                             false) : Date(date.jourJulienUTC(), 0., false);

                satellites[isat].CalculTracesAuSol(dateInit, nbTracesAuSol, refraction);
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
void Satellite::CalculSatelliteEclipse(const Soleil &soleil, const bool refraction)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double rs = 1. / soleil.position().Norme();
    const double ro = 1. / _position.Norme();
    const double tanlat = _position.z() / sqrt(_position.x() * _position.x() + _position.y() * _position.y());
    const double u = atan(tanlat / (1. - APLA));
    const double cu = cos(u);
    const double su = sin(u);
    const double r = RAYON_TERRESTRE * sqrt(cu * cu + G2 * su * su);

    /* Corps de la methode */
    double psat = asin(r * ro);
    if (std::isnan(psat))
        psat = PI_SUR_DEUX;
    const double rsol = asin(RAYON_SOLAIRE * rs);
    const double psol = asin(RAYON_TERRESTRE * rs);

    // Rayon de l'ombre de la Terre
    _rayonOmbre = psol + psat - rsol;

    // Prise en compte de la refraction atmospherique
    if (refraction)
        _rayonOmbre -= REFRACTION_HZ;

    // Rayon de la penombre de la Terre
    _rayonPenombre = psol + 1.01 * psat + rsol;

    _elongation = _position.Angle(-soleil.position());

    // Test si le satellite est en phase d'eclipse
    _eclipse = (_elongation < _rayonOmbre);

    // Test si le satellite est dans la penombre
    _penombre = (_eclipse || _elongation < _rayonPenombre);

    /* Retour */
    return;
}

/*
 * Calcul de la trace dans le ciel
 */
void Satellite::CalculTraceCiel(const Date &date, const bool refraction, const Observateur &observateur, const int sec)
{
    /* Declarations des variables locales */
    Soleil soleil;

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

                // Conditions d'eclipse
                sat.CalculSatelliteEclipse(soleil, refraction);

                const QVector<double> list(QVector<double> () << sat._hauteur << sat._azimut <<
                                           ((sat._eclipse) ? 1. : (sat._penombre) ? 2. : 0.) << j0.jourJulienUTC());
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
        satellites[isat]._dateReentree = "";
        satellites[isat]._periode = "";
        satellites[isat]._perigee = "";
        satellites[isat]._apogee = "";
        satellites[isat]._inclinaison = "";
        satellites[isat]._categorieOrbite = "";
        satellites[isat]._pays = "";
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
                    satellites[isat]._dateReentree = ligne.mid(59, 10);
                    satellites[isat]._periode = ligne.mid(70, 10);
                    satellites[isat]._perigee = ligne.mid(81, 7);
                    satellites[isat]._apogee = ligne.mid(89, 7);
                    satellites[isat]._inclinaison = ligne.mid(97, 6);
                    satellites[isat]._categorieOrbite = ligne.mid(104, 6).trimmed();
                    satellites[isat]._pays = ligne.mid(111, 5).trimmed();
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
void Satellite::CalculTracesAuSol(const Date &date, const int nbOrb, const bool refraction)
{
    /* Declarations des variables locales */
    Satellite sat = *this;
    Soleil soleil;

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
        soleil.CalculPosition(date);

        // Conditions d'eclipse
        sat.CalculSatelliteEclipse(soleil, refraction);

        const QVector<double> list(QVector<double> () << lon << lat << ((sat._eclipse) ? 1. : (sat._penombre) ? 2. : 0)
                                   << j0.jourJulienUTC());
        _traceAuSol.append(list);
    }

    /* Retour */
    return;
}


/* Accesseurs */
bool Satellite::isEclipse() const
{
    return _eclipse;
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

double Satellite::elongation() const
{
    return _elongation;
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

bool Satellite::isPenombre() const
{
    return _penombre;
}

double Satellite::rayonOmbre() const
{
    return _rayonOmbre;
}

double Satellite::rayonPenombre() const
{
    return _rayonPenombre;
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
