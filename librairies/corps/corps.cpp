/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    corps.cpp
 *
 * Localisation
 * >    librairies.corps
 *
 * Heritage
 * >
 *
 * Description
 * >    Donnees relatives a un corps (satellite, Soleil, ...)
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    16 juin 2013
 *
 */

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QTextStream>
#include <cmath>
#include "corps.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

static bool _initCst = false;
static QString _tabConst[358];
static double _tabCstCoord[358][3];

/*
 * Constructeur par defaut
 */
Corps::Corps()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _azimut = 0.;
    _hauteur = 0.;
    _distance = 0.;

    _ascensionDroite = 0.;
    _declinaison = 0.;

    _longitude = 0.;
    _latitude = 0.;
    _altitude = 0.;

    _visible = false;
    _rangeRate = 0.;

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Corps::~Corps()
{
}


/* Methodes */
/*
 * Calcul des coordonnees equatoriales
 */
void Corps::CalculCoordEquat(const Observateur &observateur)
{
    /* Declarations des variables locales */
    bool atrouve;
    int i;

    /* Initialisations */
    atrouve = false;
    i = 0;
    const double ch = cos(_hauteur);
    const Vecteur3D vec1 = Vecteur3D(-cos(_azimut) * ch, sin(_azimut) * ch, sin(_hauteur));
    const Vecteur3D vec2 = Vecteur3D(observateur.getRotHz().Transposee() * vec1);

    /* Corps de la methode */
    // Declinaison
    _declinaison = asin(vec2.getZ());

    // Ascension droite
    _ascensionDroite = atan2(vec2.getY(), vec2.getX());
    if (_ascensionDroite < 0.)
        _ascensionDroite += DEUX_PI;

    // Determination de la constellation
    if (!_initCst) {
        InitTabConstellations();
        _initCst = true;
    }

    while (!atrouve) {
        if (_declinaison >= _tabCstCoord[i][2]) {
            if (_ascensionDroite < _tabCstCoord[i][1]) {
                if (_ascensionDroite >= _tabCstCoord[i][0]) {
                    atrouve = true;
                    _constellation = _tabConst[i];
                }
            }
        }
        i++;
    }

    /* Retour */
    return;
}

/*
 * Calcul des coordonnees horizontales
 */
void Corps::CalculCoordHoriz(const Observateur &observateur, const bool acalc, const bool arefr)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _dist = _position - observateur.getPosition();
    _distance = _dist.Norme();
    const Vecteur3D vec = observateur.getRotHz() * _dist;

    /* Corps de la methode */
    // Hauteur
    const double ht = asin(vec.getZ() / _distance);

    // Prise en compte de la refraction atmospherique
    const double htd = RAD2DEG * ht;
    const double refraction = DEG2RAD * 1.02 / (ARCMIN_PAR_DEG * tan(DEG2RAD * (htd + 10.3 / (htd + 5.11))));

    _hauteur = ht + refraction;

    // Visibilite du corps
    if (_hauteur >= 0.) {
        _visible = true;
    } else {
        _visible = false;
        if (arefr)
            _hauteur = ht;
    }

    if (acalc) {

        // Azimut
        _azimut = atan2(vec.getY(), -vec.getX());
        if (_azimut < 0.)
            _azimut += DEUX_PI;

        const Vecteur3D distp = _vitesse - observateur.getVitesse();

        // Taux de variation de la distance a l'observateur
        _rangeRate = _dist * distp /_distance;
    }

    /* Retour */
    return;
}

/*
 * Calcul des coordonnees horizontales (avec condition de visibilite)
 */
void Corps::CalculCoordHoriz2(const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _visible = false;
    _hauteur = -PI;
    if (_vec1.isNul()) {
        const double cd = cos(_declinaison);
        _vec1 = Vecteur3D(cos(_ascensionDroite) * cd, sin(_ascensionDroite) * cd, sin(_declinaison));
    }
    Vecteur3D vec2 = observateur.getRotHz() * _vec1;

    /* Corps de la methode */
    // Hauteur
    const double ht = asin(vec2.getZ());

    if (ht > -DEG2RAD) {

        // Prise en compte de la refraction atmospherique
        const double htd = ht * RAD2DEG;
        const double refraction = DEG2RAD * 1.02 / (ARCMIN_PAR_DEG * tan(DEG2RAD * (htd + 10.3 / (htd + 5.11))));

        _hauteur = ht + refraction;
        if (_hauteur >= 0.) {
            // Azimut
            _azimut = atan2(vec2.getY(), -vec2.getX());
            if (_azimut < 0.)
                _azimut += DEUX_PI;
            _visible = true;
        } else {
            _visible = false;
            _hauteur = ht;
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul des coordonnees terrestres du corps a la date courante
 */
void Corps::CalculCoordTerrestres(const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Longitude
    _longitude = Maths::modulo(observateur.getTempsSideralGreenwich() - atan2(_position.getY(), _position.getX()), DEUX_PI);
    if (fabs(_longitude) > PI)
        _longitude -= Maths::sgn(_longitude) * DEUX_PI;

    CalculLatitudeAltitude();

    /* Retour */
    return;
}

/*
 * Calcul des coordonnees terrestres du corps a une date donnee
 */
void Corps::CalculCoordTerrestres(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Longitude
    _longitude = Maths::modulo(Observateur::CalculTempsSideralGreenwich(date) - atan2(_position.getY(), _position.getX()),
                               DEUX_PI);
    if (fabs(_longitude) > PI)
        _longitude -= Maths::sgn(_longitude) * DEUX_PI;

    CalculLatitudeAltitude();

    /* Retour */
    return;
}

/*
 * Calcul de la zone de visibilite
 */
void Corps::CalculZoneVisibilite()
{
    /* Declarations des variables locales */
    double lo1;

    /* Initialisations */
    double lo0 = _longitude;
    if (lo0 > 0.)
        lo0 -= DEUX_PI;
    const double cl0 = cos(_latitude);
    const double sl0 = sin(_latitude);
    double beta = acos((RAYON_TERRESTRE - 15.) / _position.Norme());
    if (std::isnan(beta))
        beta = 0.;
    const double cb = cos(beta);
    const double sb = sin(beta);

    /* Corps de la methode */
    _zone.clear();
    for(int i=0; i<360; i++) {

        const double az = DEG2RAD * i;
        const double la1 = asin(sl0 * cb + cos(az) * sb * cl0);

        if (i == 0 && beta > PI_SUR_DEUX - _latitude) {
            lo1 = lo0 + PI;
        } else {
            if (i == 180 && beta > PI_SUR_DEUX + _latitude) {
                lo1 = lo0 + PI;
            } else {
                const double numden = (cb - sl0 * sin(la1)) / (cl0 * cos(la1));
                if (fabs(numden > 1.)) {
                    lo1 = lo0;
                } else {
                    lo1 = (i <= 180) ? lo0 + acos(numden) : lo0 - acos(numden);
                }
            }
        }
        const QPointF pt(fmod(PI - lo1, DEUX_PI) * RAD2DEG, (PI_SUR_DEUX - la1) * RAD2DEG);
        _zone.append(pt);
    }
    _zone.append(_zone.at(0));

    /* Retour */
    return;
}

/*
 * Conversion d'un vecteur en coordonnees ecliptiques spheriques en coordonnees cartesiennes equatoriales
 */
Vecteur3D Corps::Sph2Cart(const Vecteur3D &vecteur, const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double t = date.getJourJulienUTC() * NB_SIECJ_PAR_JOURS;
    const double t2 = t * t;

    /* Corps de la methode */
    const double obliquite = ARCSEC2RAD * (84381.448 - 46.815 * t - 0.00059 * t2 + 0.001813 * t * t2);
    const double cb = cos(vecteur.getY());
    const double sb = sin(vecteur.getY());
    const double ce = cos(obliquite);
    const double se = sin(obliquite);
    const double xx = vecteur.getZ() * cb * sin(vecteur.getX());

    /* Retour */
    return Vecteur3D(vecteur.getZ() * cb * cos(vecteur.getX()), xx * ce - vecteur.getZ() * se * sb,
                     xx * se + vecteur.getZ() * ce * sb);
}

/*
 * Calcul de la latitude terrestre et de l'altitude (avec prise en compte de l'aplatissement du globe terrestre)
 */
void Corps::CalculLatitudeAltitude()
{
    /* Declarations des variables locales */
    double c, lat;

    /* Initialisations */
    c = 1.;
    _latitude = PI;

    /* Corps de la methode */
    // Latitude
    const double r0 = sqrt(_position.getX() * _position.getX() + _position.getY() * _position.getY());
    _latitude = atan2(_position.getZ(), r0);
    do {
        lat = _latitude;
        const double sph = sin(lat);
        c = 1. / sqrt(1. - E2 * sph * sph);
        _latitude = atan((_position.getZ() + RAYON_TERRESTRE * c * E2 * sph) / r0);
    } while (fabs(_latitude - lat) > 1.e-7);

    // Altitude
    _altitude = (r0 < 1.e-3) ? fabs(_position.getZ()) - RAYON_TERRESTRE * (1. - APLA) :
                               r0 / cos(_latitude) - RAYON_TERRESTRE * c;

    /* Retour */
    return;
}

/*
 * Lecture du fichier de constellations
 */
void Corps::InitTabConstellations()
{
    /* Declarations des variables locales */
    QString dirDat;

    /* Initialisations */
#if defined (Q_OS_WIN)
    dirDat = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

    /* Corps de la methode */
    const QString fic = dirDat + QDir::separator() + "constellations.cst";
    QFile fi(fic);
    if (fi.exists()) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        int i = 0;
        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();
            _tabConst[i] = ligne.mid(0, 3);
            _tabCstCoord[i][0] = ligne.mid(4, 7).toDouble() * HEUR2RAD;
            _tabCstCoord[i][1] = ligne.mid(12, 7).toDouble() * HEUR2RAD;
            _tabCstCoord[i][2] = ligne.mid(20, 8).toDouble() * DEG2RAD;
            i++;
        }
    }
    fi.close();

    /* Retour */
    return;
}

/* Accesseurs */
double Corps::getAltitude() const
{
    return _altitude;
}

double Corps::getAscensionDroite() const
{
    return _ascensionDroite;
}

double Corps::getAzimut() const
{
    return _azimut;
}

QString Corps::getConstellation() const
{
    return _constellation;
}

double Corps::getDeclinaison() const
{
    return _declinaison;
}

Vecteur3D Corps::getDist() const
{
    return _dist;
}

double Corps::getDistance() const
{
    return _distance;
}

double Corps::getHauteur() const
{
    return _hauteur;
}

double Corps::getLatitude() const
{
    return _latitude;
}

double Corps::getLongitude() const
{
    return _longitude;
}

Vecteur3D Corps::getPosition() const
{
    return _position;
}

void Corps::setPosition(Vecteur3D position)
{
    _position = position;
}

double Corps::getRangeRate() const
{
    return _rangeRate;
}

bool Corps::isVisible() const
{
    return _visible;
}

Vecteur3D Corps::getVitesse() const
{
    return _vitesse;
}

QVector<QPointF> Corps::getZone() const
{
    return _zone;
}
