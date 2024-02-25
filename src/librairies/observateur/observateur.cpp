/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    25 fevrier 2023
 *
 */

#include <cmath>
#include <QCoreApplication>
#include "librairies/corps/terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "observateur.h"


static const char* listeCap[] = {
    QT_TRANSLATE_NOOP("cardinal point", "N"),
    QT_TRANSLATE_NOOP("cardinal point", "NNE"),
    QT_TRANSLATE_NOOP("cardinal point", "NE"),
    QT_TRANSLATE_NOOP("cardinal point", "ENE"),
    QT_TRANSLATE_NOOP("cardinal point", "E"),
    QT_TRANSLATE_NOOP("cardinal point", "ESE"),
    QT_TRANSLATE_NOOP("cardinal point", "SE"),
    QT_TRANSLATE_NOOP("cardinal point", "SSE"),
    QT_TRANSLATE_NOOP("cardinal point", "S"),
    QT_TRANSLATE_NOOP("cardinal point", "SSW"),
    QT_TRANSLATE_NOOP("cardinal point", "SW"),
    QT_TRANSLATE_NOOP("cardinal point", "WSW"),
    QT_TRANSLATE_NOOP("cardinal point", "W"),
    QT_TRANSLATE_NOOP("cardinal point", "WNW"),
    QT_TRANSLATE_NOOP("cardinal point", "NW"),
    QT_TRANSLATE_NOOP("cardinal point", "NNW")
};


/**********
 * PUBLIC *
 **********/


/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Observateur::Observateur() :
    _nomlieu("")
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
    _aaer = 0.;
    _aray = 0.;
    _tempsSideralGreenwich = 0.;

    /* Retour */
    return;
}

/*
 * Constructeur a partir des coordonnees geographiques d'un lieu d'observation
 */
Observateur::Observateur(const QString &nom,
                         const double lon,
                         const double lat,
                         const double alt) :
    _nomlieu(nom)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _tempsSideralGreenwich = 0.;

    /* Corps du constructeur */
    _longitude = MATHS::DEG2RAD * lon;
    _latitude = MATHS::DEG2RAD * lat;
    _altitude = alt * 1.e-3;

    _coslat = cos(_latitude);
    _sinlat = sin(_latitude);

    const double coster = 1. / sqrt(1. - TERRE::E2 * _sinlat * _sinlat);
    const double sinter = TERRE::G2 * coster;

    _rayon = (TERRE::RAYON_TERRESTRE * coster + _altitude) * _coslat;
    _posZ = (TERRE::RAYON_TERRESTRE * sinter + _altitude) * _sinlat;

    // Pour l'extinction atmospherique
    _aray = 0.1451 * exp(-_altitude * (1. / 7.996));
    _aaer = 0.120 * exp(-_altitude * (1. / 1.5));

    /* Retour */
    return;
}

/*
 * Constructeur a partir des donnees relatives au lieu d'observation a une date donnee
 * (pour le calcul des previsions)
 */
Observateur::Observateur(const Vecteur3D &pos,
                         const Vecteur3D &vit,
                         const Matrice3D &matRotHz,
                         const double aaerVal,
                         const double arayVal) :
    _position(pos),
    _vitesse(vit),
    _rotHz(matRotHz)
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
    _vitesse = Vecteur3D(-TERRE::OMEGA * _position.y(), TERRE::OMEGA * _position.x(), 0.);

    // Matrice utile pour le calcul des coordonnees horizontales
    const Vecteur3D v1(_sinlat * costsl, -sintsl, _coslat * costsl);
    const Vecteur3D v2(_sinlat * sintsl, costsl, _coslat * sintsl);
    const Vecteur3D v3(-_coslat, 0., _sinlat);
    _rotHz = Matrice3D(v1, v2, v3);

    /* Retour */
    return;
}

/*
 * Calcul du temps sideral moyen de Greenwich
 */
double Observateur::CalculTempsSideralGreenwich(const Date &date)
{
    /* Declarations des variables locales */
    double jj0;

    /* Initialisations */
    const double jj = date.jourJulienUTC();
    const double f = modf(jj + 0.5, &jj0) * DATE::NB_SEC_PAR_JOUR * TERRE::OMEGA0;

    double jj0h = static_cast<long> (jj) + 0.5;
    if (jj0h > jj) {
        jj0h -= 1.;
    }

    const double t = jj0h * DATE::NB_SIECJ_PAR_JOURS;

    /* Corps de la methode */

    /* Retour */
    return modulo(f + 24110.54841 + t * (8640184.812866 + t * (0.093104 - 6.2e-6 * t)), DATE::NB_SEC_PAR_JOUR) * 15. * MATHS::ARCSEC2RAD;
}

/*
 * Calcul du cap d'un lieu d'observation par rapport a un autre
 */
QPair<QString, double> Observateur::CalculCap(const Observateur &lieuDistant) const
{
    /* Declarations des variables locales */
    QPair<QString, double> res;

    /* Initialisations */
    double cap = 0.;

    /* Corps de la methode */
    if (_coslat < MATHS::EPSDBL100) {
        cap = (_latitude > 0.) ? MATHS::PI : 0.;
    } else {

        if (lieuDistant._coslat < MATHS::EPSDBL100) {
            cap = (lieuDistant._latitude > 0.) ? 0. : MATHS::PI;
        } else {

            const double delta = _longitude - lieuDistant._longitude;
            const double num = sin(delta) * lieuDistant._coslat;
            const double den = _coslat * lieuDistant._sinlat - _sinlat * lieuDistant._coslat * cos(delta);
            cap = modulo(atan2(num, den), MATHS::DEUX_PI);
        }
    }
    const int idx = static_cast<int> (cap * MATHS::RAD2DEG / 22.5);
    res.first = QCoreApplication::translate("cardinal point", listeCap[idx]);
    res.second = cap;

    /* Retour */
    return (res);
}

/*
 * Calcul de la distance entre 2 lieux d'observation
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
    const double d = 2. * om * TERRE::RAYON_TERRESTRE;
    const double h1 = 0.5 * (r3 - 1.) / c;
    const double h2 = 0.5 * (r3 + 1.) / s;

    /* Retour */
    return (d * (1. + TERRE::APLA * (h1 * sf2 * cg2 - h2 * cf2 * sg2)));
}

/*
 * Calcul des coordonnees geographiques du lieu a l'intersection d'un vecteur pointant vers la Terre et de l'ellipsoide terrestre
 */
Observateur Observateur::CalculIntersectionEllipsoide(const Date &date,
                                                      const Vecteur3D &origine,
                                                      const Vecteur3D &direction)
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
    const double a = 1. - TERRE::E2 * cz2;
    const double b = -(TERRE::G2 * (x * dx + y * dy) + z * dz);
    const double c = TERRE::G2 * (r2 - TERRE::RAYON_TERRESTRE * TERRE::RAYON_TERRESTRE) + z2;
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
        lon = modulo(tsg - atan2(intersection.y(), intersection.x()), MATHS::DEUX_PI);
        if (fabs(lon) > MATHS::PI) {
            lon -= sgn(lon) * MATHS::DEUX_PI;
        }

        lon *= MATHS::RAD2DEG;

        // Latitude
        lat = MATHS::RAD2DEG * atan2(intersection.z(), TERRE::G2 * sqrt(intersection.x() * intersection.x() + intersection.y() * intersection.y()));

        nom = "INTERSECT";
    }

    /* Retour */
    return (Observateur(nom, lon, lat));
}


/*
 * Accesseurs
 */
double Observateur::longitude() const
{
    return _longitude;
}

double Observateur::latitude() const
{
    return _latitude;
}

double Observateur::altitude() const
{
    return _altitude;
}

const QString &Observateur::nomlieu() const
{
    return _nomlieu;
}

double Observateur::aaer() const
{
    return _aaer;
}

double Observateur::aray() const
{
    return _aray;
}

double Observateur::tempsSideralGreenwich() const
{
    return _tempsSideralGreenwich;
}

Vecteur3D Observateur::position() const
{
    return _position;
}

Vecteur3D Observateur::vitesse() const
{
    return _vitesse;
}

Matrice3D Observateur::rotHz() const
{
    return _rotHz;
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

