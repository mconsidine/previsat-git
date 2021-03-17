/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    observateur.cpp
 *
 * Localisation
 * >    librairies.observateur
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    30 juillet 2011
 *
 * Date de revision
 * >    6 juillet 2019
 *
 */

#include <cmath>
#include <QStringList>
#include "librairies/corps/systemesolaire/terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "observateur.h"


/**********
 * PUBLIC *
 **********/

static QStringList listeCap(QStringList() << "N" << "NNE" << "NE" << "ENE" << "E" << "ESE" << "SE" << "SSE" << "S" << "SSW" << "SW" << "WSW" << "W"
                            << "WNW" << "NW" << "NNW");


/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Observateur::Observateur()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nomlieu = "";
    _longitude = 0.;
    _latitude = 0.;
    _altitude = 0.;
    _coslat = 1.;
    _sinlat = 0.;
    _rayon = 0.;
    _posZ = 0.;
    _aaer = 0.;
    _aray = 0.;
    _tempsSideralGreenwich = 0.;

    /* Retour */
    return;
}

/*
 * Constructeur a partir des coordonnees geographiques d'un lieu d'observation
 */
Observateur::Observateur(const QString &nom, const double lon, const double lat, const double alt) :
    _nomlieu(nom)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _tempsSideralGreenwich = 0.;

    /* Corps du constructeur */
    _longitude = DEG2RAD * lon;
    _latitude = DEG2RAD * lat;
    _altitude = alt * 1.e-3;

    _coslat = cos(_latitude);
    _sinlat = sin(_latitude);

    const double coster = 1. / sqrt(1. - E2 * _sinlat * _sinlat);
    const double sinter = G2 * coster;

    _rayon = (RAYON_TERRESTRE * coster + _altitude) * _coslat;
    _posZ = (RAYON_TERRESTRE * sinter + _altitude) * _sinlat;

    // Pour l'extinction atmospherique
    _aray = 0.1451 * exp(-_altitude * (1. / 7.996));
    _aaer = 0.120 * exp(-_altitude * (1. / 1.5));

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un lieu d'observation
 */
Observateur::Observateur(const Observateur &observateur) :
    _nomlieu(observateur.nomlieu()), _position(observateur.position()), _vitesse(observateur.vitesse()), _rotHz(observateur.rotHz())
{
    /* Declarations des variables locales */

    /* Initialisations */
    _tempsSideralGreenwich = 0.;

    /* Corps du constructeur */
    _longitude = observateur._longitude;
    _latitude = observateur._latitude;
    _altitude = observateur._altitude;

    _coslat = observateur._coslat;
    _sinlat = observateur._sinlat;

    _rayon = observateur._rayon;
    _posZ = observateur._posZ;

    // Pour l'extinction atmospherique
    _aray = observateur._aray;
    _aaer = observateur._aaer;

    /* Retour */
    return;
}

/*
 * Constructeur a partir des donnees relatives au lieu d'observation a une date donnee
 * (pour le calcul des previsions)
 */
Observateur::Observateur(const Vecteur3D &pos, const Vecteur3D &vit, const Matrice3D &matRotHz, const double aaerVal, const double arayVal) :
    _position(pos), _vitesse(vit), _rotHz(matRotHz)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _longitude = 0.;
    _latitude = 0.;
    _altitude = 0.;
    _coslat = 1.;
    _sinlat = 0.;
    _rayon = 0.;
    _posZ = 0.;
    _tempsSideralGreenwich = 0.;
    _aaer = aaerVal;
    _aray = arayVal;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
double Observateur::aaer() const
{
    return _aaer;
}

double Observateur::altitude() const
{
    return _altitude;
}

double Observateur::aray() const
{
    return _aray;
}

double Observateur::latitude() const
{
    return _latitude;
}

double Observateur::longitude() const
{
    return _longitude;
}

QString Observateur::nomlieu() const
{
    return _nomlieu;
}

Vecteur3D Observateur::position() const
{
    return _position;
}

Matrice3D Observateur::rotHz() const
{
    return _rotHz;
}

double Observateur::tempsSideralGreenwich() const
{
    return _tempsSideralGreenwich;
}

Vecteur3D Observateur::vitesse() const
{
    return _vitesse;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la position et de la vitesse du lieu d'observation
 */
void Observateur::CalculPosVit(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _tempsSideralGreenwich = CalculTempsSideralGreenwich(date);

    // Temps sideral local
    const double tsl = _tempsSideralGreenwich - _longitude;
    const double costsl = cos(tsl);
    const double sintsl = sin(tsl);

    /* Corps de la methode */
    // Position de l'observateur
    _position = Vecteur3D(_rayon * costsl, _rayon * sintsl, _posZ);

    // Vitesse de l'observateur
    _vitesse = Vecteur3D(-OMEGA * _position.y(), OMEGA * _position.x(), 0.);

    // Matrice utile pour le calcul des coordonnees horizontales
    const Vecteur3D v1(_sinlat * costsl, -sintsl, _coslat * costsl);
    const Vecteur3D v2(_sinlat * sintsl, costsl, _coslat * sintsl);
    const Vecteur3D v3(-_coslat, 0., _sinlat);
    _rotHz = Matrice3D(v1, v2, v3);

    /* Retour */
    return;
}

/*
 * Calcul du temps sideral de Greenwich
 * D'apres la formule donnee dans l'Astronomical Algorithms 2nd edition de Jean Meeus, p88
 */
double Observateur::CalculTempsSideralGreenwich(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double tu = date.jourJulienUTC() * NB_SIECJ_PAR_JOURS;
    const double tu2 = tu * tu;

    /* Corps de la methode */

    /* Retour */
    return (DEG2RAD * modulo(280.46061837 + 360.98564736629 * date.jourJulienUTC() + tu2 * (0.000387933 - tu * (1. / 38710000.)), T360));
}

/*
 * Calcul du cap d'un lieu d'observation par rapport a un autre
 */
QPair<QString, double> Observateur::CalculCap(const Observateur &lieuDistant)
{
    /* Declarations des variables locales */
    QPair<QString, double> res;

    /* Initialisations */
    double cap = 0.;

    /* Corps de la methode */
    if (_coslat < EPSDBL100) {
        cap = (_latitude > 0.) ? PI : 0.;
    } else {

        if (lieuDistant._coslat < EPSDBL100) {
            cap = (lieuDistant._latitude > 0.) ? 0. : PI;
        } else {

            const double num = sin(_longitude - lieuDistant._longitude) * lieuDistant._coslat;
            const double den = _coslat * lieuDistant._sinlat - _sinlat * lieuDistant._coslat * cos(_longitude - lieuDistant._longitude);
            cap = modulo(atan2(num, den), DEUX_PI);
        }
    }
    const int idx = static_cast<int> (cap * RAD2DEG / 22.5);
    res.first = listeCap.at(idx);
    res.second = cap;

    /* Retour */
    return (res);
}

/*
 * Calcul de la distance entre 2 lieux d'observation mesuree le long de la surface terrestre en tenant compte de l'applatissement
 * du globe terrestre, mais sans prise en compte de l'altitude
 * Astronomical Algorithms 2nd edition de Jean Meeus, p85
 */
double Observateur::CalculDistance(const Observateur &observateur) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double f = 0.5 * (_latitude + observateur._latitude);
    const double g = 0.5 * (_latitude - observateur._latitude);
    const double l = 0.5 * (_longitude - observateur._longitude);

    const double sf = sin(f);
    const double cf = cos(f);
    const double sf2 = sf * sf;
    const double cf2 = cf * cf;

    const double sg = sin(g);
    const double cg = cos(g);
    const double sg2 = sg * sg;
    const double cg2 = cg * cg;

    const double sl = sin(l);
    const double cl = cos(l);
    const double sl2 = sl * sl;
    const double cl2 = cl * cl;

    /* Corps de la methode */
    const double s = sg2 * cl2 + cf2 * sl2;
    const double c = cg2 * cl2 + sf2 * sl2;

    const double om = atan(sqrt(s / c));
    const double r3 = 3. * sqrt(s * c) / om;
    const double d = 2. * om * RAYON_TERRESTRE;
    const double h1 = 0.5 * (r3 - 1.) / c;
    const double h2 = 0.5 * (r3 + 1.) / s;

    /* Retour */
    return (d * (1. + APLA * (h1 * sf2 * cg2 - h2 * cf2 * sg2)));
}

/*
 * Calcul des coordonnees geographiques du lieu a l'intersection d'un vecteur pointant vers la Terre et de l'ellipsoide terrestre
 */
Observateur Observateur::CalculIntersectionEllipsoide(const Date &date, const Vecteur3D origine, const Vecteur3D direction)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString nom = "";
    double lon = 0.;
    double lat = 0.;

    const double x = origine.x();
    const double y = origine.y();
    const double z = origine.z();
    const double z2 = z * z;
    const double r2 = x * x + y * y;

    const Vecteur3D dir = direction.Normalise();
    const double dx = dir.x();
    const double dy = dir.y();
    const double dz = dir.z();
    const double cz2 = dx * dx + dy * dy;

    /* Corps de la methode */
    const double a = 1. - E2 * cz2;
    const double b = -(G2 * (x * dx + y * dy) + z * dz);
    const double c = G2 * (r2 - RAYON_TERRESTRE * RAYON_TERRESTRE) + z2;
    const double b2 = b * b;
    const double ac = a * c;

    if (b2 > ac) {

        const double s = sqrt(b2 - ac);
        const double k1 = (b < 0.) ? (b - s) / a : c / (b + s);
        const double k2 = c / (a * k1);
        const double k = (fabs(k1) < fabs(k2)) ? k1 : k2;

        const Vecteur3D intersection = origine + (dir * k);
        const double tsg = CalculTempsSideralGreenwich(date);

        // Longitude
        lon = modulo(tsg - atan2(intersection.y(), intersection.x()), DEUX_PI);
        if (fabs(lon) > PI) {
            lon -= sgn(lon) * DEUX_PI;
        }
        lon *= RAD2DEG;

        // Latitude
        lat = RAD2DEG * atan2(intersection.z(), G2 * sqrt(intersection.x() * intersection.x() + intersection.y() * intersection.y()));

        nom = "INTERSECT";
    }

    /* Retour */
    return (Observateur(nom, lon, lat, 0.));
}

/*
 * Affectation d'un observateur
 */
Observateur &Observateur::operator = (const Observateur &observateur)
{
    _nomlieu = observateur._nomlieu;
    _longitude = observateur._longitude;
    _latitude = observateur._latitude;
    _altitude = observateur._altitude;
    _coslat = observateur._coslat;
    _sinlat = observateur._sinlat;
    _rayon = observateur._rayon;
    _posZ = observateur._posZ;
    _position = observateur._position;
    _vitesse = observateur._vitesse;
    _rotHz = observateur._rotHz;
    _aaer = observateur._aaer;
    _aray = observateur._aray;
    _tempsSideralGreenwich = observateur._tempsSideralGreenwich;

    return (*this);
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

