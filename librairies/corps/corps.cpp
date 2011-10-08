/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <cmath>
#include <fstream>
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
    Vecteur3D vec1 = Vecteur3D(-cos(_azimut) * ch, sin(_azimut) * ch, sin(_hauteur));
    Vecteur3D vec2 = Vecteur3D(observateur.getRotHz().Transposee() * vec1);

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
void Corps::CalculCoordHoriz(const Observateur &observateur, const bool acalc)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _dist = _position - observateur.getPosition();
    _distance = _dist.Norme();
    Vecteur3D vec = observateur.getRotHz() * _dist;

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
        _hauteur = ht;
    }

    if (acalc) {

        // Azimut
        _azimut = atan2(vec.getY(), -vec.getX());
        if (_azimut < 0.)
            _azimut += DEUX_PI;

        Vecteur3D distp = _vitesse - observateur.getVitesse();

        // Taux de variation de la distance a l'observateur
        _rangeRate = _dist * distp /_distance;
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
    _longitude = Maths::modulo(Observateur::CalculTempsSideralGreenwich(date) - atan2(_position.getY(), _position.getX()), DEUX_PI);
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

    /* Initialisations */

    /* Corps de la methode */


    /* Retour */
    return;
}

/*
 * Calcul de l'altitude et de l'altitude
 */
void Corps::CalculLatitudeAltitude()
{
    /* Declarations des variables locales */
    double c, lat, sph;

    /* Initialisations */
    c = 1.;
    _latitude = PI;

    /* Corps de la methode */
    // Latitude
    const double r0 = sqrt(_position.getX() * _position.getX() + _position.getY() * _position.getY());
    _latitude = atan2(_position.getZ(), r0);
    do {
        lat = _latitude;
        sph = sin(lat);
        c = 1. / sqrt(1. - E2 * sph * sph);
        _latitude = atan((_position.getZ() + RAYON_TERRESTRE * c * E2 * sph) / r0);
    } while (fabs(_latitude - lat) > 1.e-7);

    // Altitude
    _altitude = (r0 < 1.e-3) ? fabs(_position.getZ()) - RAYON_TERRESTRE * (1. - APLA) : r0 / cos(_latitude) - RAYON_TERRESTRE * c;

    /* Retour */
    return;
}

/*
 * Lecture du fichier de constellations
 */
void Corps::InitTabConstellations()
{
    /* Declarations des variables locales */
    int i;
    double ad1, ad2, de;
    char ligne[4096];

    /* Initialisations */
    i = 0;

    /* Corps de la methode */
    FILE *fcst = NULL;
    if ((fcst = fopen("data/constellations.cst", "r")) != NULL) {

        while (fgets(ligne, 4096, fcst) != NULL) {

            QString cst(ligne);
            cst = cst.mid(0, 3);
            sscanf(ligne, "%*s%8lf%8lf%9lf", &ad1, &ad2, &de);
            _tabConst[i] = cst;
            _tabCstCoord[i][0] = ad1 * HEUR2RAD;
            _tabCstCoord[i][1] = ad2 * HEUR2RAD;
            _tabCstCoord[i][2] = de * DEG2RAD;
            i++;
        }
    }
    fclose(fcst);

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
