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
 * >    30 decembre 2018
 *
 */

#include <cmath>
#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "corps.h"
#include "librairies/corps/systemesolaire/terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"


/**********
 * PUBLIC *
 **********/

struct ConstElem {
    QString nom;
    double ad1;
    double ad2;
    double dec;
};

static QList<ConstElem> _tabConst;
static QList<double> _caz;
static QList<double> _saz;


/*
 * Constructeurs
 */
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

    _lonEcl = 0.;
    _latEcl = 0.;
    _ct = 0.;
    _r0 = 0.;

    if (_caz.isEmpty() || _saz.isEmpty()) {

        _caz.clear();
        _saz.clear();
        for(int i=0; i<360; i++) {
            const double az = i * DEG2RAD;
            _caz.append(cos(az));
            _saz.append(sin(az));
        }
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
double Corps::altitude() const
{
    return _altitude;
}

double Corps::ascensionDroite() const
{
    return _ascensionDroite;
}

double Corps::azimut() const
{
    return _azimut;
}

QString Corps::constellation() const
{
    return _constellation;
}

double Corps::declinaison() const
{
    return _declinaison;
}

Vecteur3D Corps::dist() const
{
    return _dist;
}

double Corps::distance() const
{
    return _distance;
}

double Corps::hauteur() const
{
    return _hauteur;
}

double Corps::latitude() const
{
    return _latitude;
}

double Corps::longitude() const
{
    return _longitude;
}

double Corps::lonEcl() const
{
    return _lonEcl;
}

Vecteur3D Corps::position() const
{
    return _position;
}

void Corps::setPosition(const Vecteur3D &pos)
{
    _position = pos;
}

double Corps::rangeRate() const
{
    return _rangeRate;
}

bool Corps::isVisible() const
{
    return _visible;
}

Vecteur3D Corps::vitesse() const
{
    return _vitesse;
}

QVector<QPointF> Corps::zone() const
{
    return _zone;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de l'altitude du corps
 */
double Corps::CalculAltitude(const Vecteur3D &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return ((_r0 < 1.e-3) ? fabs(pos.z()) - RAYON_TERRESTRE * (1. - APLA) : _r0 / cos(_latitude) - RAYON_TERRESTRE * _ct);
}

/*
 * Calcul des coordonnees equatoriales
 */
void Corps::CalculCoordEquat(const Observateur &observateur, const bool determinationConstellation)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _constellation = "";

    const double ch = cos(_hauteur);
    const Vecteur3D vec1(-cos(_azimut) * ch, sin(_azimut) * ch, sin(_hauteur));
    const Vecteur3D vec2(observateur.rotHz().Transposee() * vec1);

    /* Corps de la methode */
    // Declinaison
    _declinaison = asin(vec2.z());

    // Ascension droite
    _ascensionDroite = atan2(vec2.y(), vec2.x());
    if (_ascensionDroite < 0.) {
        _ascensionDroite += DEUX_PI;
    }

    // Determination de la constellation
    if (determinationConstellation) {

        try {

            if (_tabConst.isEmpty()) {
                throw PreviSatException(QObject::tr("Tableau de constellations vide"), WARNING);
            }

            bool atrouve = false;
            int i = 0;
            while (!atrouve && (i < _tabConst.size())) {
                if (_declinaison >= _tabConst.at(i).dec) {
                    if (_ascensionDroite < _tabConst.at(i).ad2) {
                        if (_ascensionDroite >= _tabConst.at(i).ad1) {
                            atrouve = true;
                            _constellation = _tabConst.at(i).nom;
                        }
                    }
                }
                i++;
            }
        } catch (PreviSatException &e) {
            throw PreviSatException();
        }
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
    _dist = _position - observateur.position();
    _distance = _dist.Norme();
    const Vecteur3D vec = observateur.rotHz() * _dist;

    /* Corps de la methode */
    // Hauteur
    const double ht = asin(vec.z() / _distance);

    // Prise en compte de la refraction atmospherique
    const double htd = RAD2DEG * ht;
    const double refraction = (htd >= -1.) ? DEG2RAD * 1.02 / (ARCMIN_PAR_DEG * tan(DEG2RAD * (htd + 10.3 / (htd + 5.11)))) : 0.;

    _hauteur = ht + refraction;

    // Visibilite du corps
    if (_hauteur >= 0.) {
        _visible = true;
    } else {
        _visible = false;
        if (arefr) {
            _hauteur = ht;
        }
    }

    if (acalc) {

        // Azimut
        _azimut = atan2(vec.y(), -vec.x());
        if (_azimut < 0.) {
            _azimut += DEUX_PI;
        }

        const Vecteur3D distp = _vitesse - observateur.vitesse();

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
    if (_vec1.Nul()) {
        const double cd = cos(_declinaison);
        _vec1 = Vecteur3D(cos(_ascensionDroite) * cd, sin(_ascensionDroite) * cd, sin(_declinaison));
    }
    const Vecteur3D vec2 = observateur.rotHz() * _vec1;

    /* Corps de la methode */
    // Hauteur
    const double ht = asin(vec2.z());

    if (ht > -DEG2RAD) {

        // Prise en compte de la refraction atmospherique
        const double htd = ht * RAD2DEG;
        const double refraction = (htd >= -1.) ? DEG2RAD * 1.02 / (ARCMIN_PAR_DEG * tan(DEG2RAD * (htd + 10.3 / (htd + 5.11)))) : 0.;

        _hauteur = ht + refraction;
        if (_hauteur >= 0.) {
            // Azimut
            _azimut = atan2(vec2.y(), -vec2.x());
            if (_azimut < 0.) {
                _azimut += DEUX_PI;
            }
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
    _longitude = modulo(observateur.tempsSideralGreenwich() - atan2(_position.y(), _position.x()), DEUX_PI);
    if (fabs(_longitude) > PI) {
        _longitude -= sgn(_longitude) * DEUX_PI;
    }

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
    _longitude = modulo(Observateur::CalculTempsSideralGreenwich(date) - atan2(_position.y(), _position.x()), DEUX_PI);
    if (fabs(_longitude) > PI) {
        _longitude -= sgn(_longitude) * DEUX_PI;
    }

    CalculLatitudeAltitude();

    /* Retour */
    return;
}

/*
 * Calcul de la latitude geodesique
 */
double Corps::CalculLatitude(const Vecteur3D &pos)
{
    /* Declarations des variables locales */
    double lat;

    /* Initialisations */
    _ct = 1.;
    _latitude = PI;
    const double re2 = RAYON_TERRESTRE * E2;

    /* Corps de la methode */
    _r0 = sqrt(pos.x() * pos.x() + pos.y() * pos.y());
    _latitude = atan2(pos.z(), _r0);
    do {
        lat = _latitude;
        const double sph = sin(lat);
        _ct = 1. / sqrt(1. - E2 * sph * sph);
        _latitude = atan((pos.z() + re2 * _ct * sph) / _r0);
    } while (fabs(_latitude - lat) > 1.e-7);

    /* Retour */
    return (_latitude);
}

void Corps::CalculPosVitECEF(const Date &date, Vecteur3D &positionECEF, Vecteur3D &vitesseECEF) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double gmst = Observateur::CalculTempsSideralGreenwich(date);
    const Vecteur3D omegaTerre(0., 0., OMEGA);

    /* Corps de la methode */
    positionECEF = _position.Rotation(AXE_Z, gmst);
    vitesseECEF = _vitesse.Rotation(AXE_Z, gmst) - (omegaTerre ^ positionECEF);

    /* Retour */
    return;
}

/*
 * Calcul de la zone de visibilite
 */
void Corps::CalculZoneVisibilite(const double beta)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double srad = beta + REFRACTION_HZ;
    const double cla = cos(_latitude);
    const double sla = sin(_latitude);
    const double clo = cos(_longitude);
    const double slo = sin(_longitude);
    const double cra = cos(srad);
    const double sra = sin(srad);

    /* Corps de la methode */
    _zone.clear();
    for(int i=0; i<360; i++) {

        const double z1 = sra * _caz[i];

        const double x2 = cra * cla - z1 * sla;
        const double y2 = sra * _saz[i];
        const double z2 = cra * sla + z1 * cla;

        const double x3 = x2 * clo - y2 * slo;
        const double y3 = x2 * slo + y2 * clo;

        const double lo1 = atan2(y3, x3);
        const double la1 = asin(z2);

        const QPointF pt(fmod(PI - lo1, DEUX_PI) * RAD2DEG, (PI_SUR_DEUX - la1) * RAD2DEG);
        _zone.append(pt);
    }
    _zone.append(_zone.at(0));

    /* Retour */
    return;
}

/*
 * Lecture du fichier de constellations
 */
void Corps::InitTabConstellations(const QString &dirCommonData)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString fic = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "constellations.dat";
        QFile fi(fic);
        if (fi.exists() && fi.size() != 0) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);

            _tabConst.clear();
            while (!flux.atEnd()) {

                const QString ligne = flux.readLine();
                if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#')) {
                    const QStringList list = ligne.split(" ", QString::SkipEmptyParts);
                    ConstElem cst;
                    cst.nom = list.at(0);
                    cst.ad1 = list.at(1).toDouble() * HEUR2RAD;
                    cst.ad2 = list.at(2).toDouble() * HEUR2RAD;
                    cst.dec = list.at(3).toDouble() * DEG2RAD;
                    _tabConst.append(cst);
                }
            }
        } else {
            throw PreviSatException(QObject::tr("Fichier %1 absent").arg(QDir::toNativeSeparators(fic)), WARNING);
        }
        fi.close();
    } catch (PreviSatException &e) {
    }

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
    const double t = date.jourJulienTT() * NB_SIECJ_PAR_JOURS;

    /* Corps de la methode */
    const double om = DEG2RAD * modulo(125.04 - 1934.136 * t, T360);
    const double x = vecteur.x() - DEG2RAD * 0.00478 * sin(om);
    const double obliquite = ARCSEC2RAD * (84381.448 + t * (-46.815 + t * (-0.00059 + 0.001813 * t)));
    const double cb = cos(vecteur.y());
    const double sb = sin(vecteur.y());
    const double ce = cos(obliquite);
    const double se = sin(obliquite);
    const double xx = vecteur.z() * cb * sin(x);

    /* Retour */
    return Vecteur3D(vecteur.z() * cb * cos(x), xx * ce - vecteur.z() * se * sb, xx * se + vecteur.z() * ce * sb);
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
/*
 * Calcul de la latitude terrestre et de l'altitude (avec prise en compte de l'aplatissement du globe terrestre)
 */
void Corps::CalculLatitudeAltitude()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Latitude
    CalculLatitude(_position);

    // Altitude
    _altitude = CalculAltitude(_position);

    /* Retour */
    return;
}
