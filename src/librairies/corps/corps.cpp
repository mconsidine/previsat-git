/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    12 mars 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "corps.h"
#include "terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"


static constexpr double RE2 = TERRE::RAYON_TERRESTRE * TERRE::E2;

struct ConstElem {
    double ad1;
    double ad2;
    double dec;
    QString nom;
};

static QList<ConstElem> _tabConst;
static std::array<double, 360> _caz;
static std::array<double, 360> _saz;


/**********
 * PUBLIC *
 **********/

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

    /* Retour */
    return;
}


/*
 * Modificateurs
 */
void Corps::setPosition(const Vecteur3D &pos)
{
    _position = pos;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de l'altitude du corps
 */
double Corps::CalculAltitude(const Vecteur3D &pos) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return ((_r0 < 1.e-3) ? fabs(pos.z()) - TERRE::RAYON_TERRESTRE * (1. - TERRE::APLA) : _r0 / cos(_latitude) - TERRE::RAYON_TERRESTRE * _ct);
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
        _ascensionDroite += MATHS::DEUX_PI;
    }

    // Determination de la constellation
    if (determinationConstellation) {

        try {

            if (_tabConst.isEmpty()) {
                throw PreviSatException(QObject::tr("Tableau de constellations vide"), MessageType::WARNING);
            }

            QListIterator it(_tabConst);
            while (it.hasNext()) {

                const ConstElem elem = it.next();

                if (_declinaison >= elem.dec) {

                    if (_ascensionDroite < elem.ad2) {
                        if (_ascensionDroite >= elem.ad1) {
                            it.toBack();
                            _constellation = elem.nom;
                        }
                    }
                }
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
void Corps::CalculCoordHoriz(const Observateur &observateur, const bool acalc, const bool arefr, const bool aos)
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
    const double refraction = (arefr) ? CalculRefractionAtmospherique(ht) : 0.;

    _hauteur = ht + refraction;

    // Visibilite du corps
    if (_hauteur >= 0.) {
        _visible = true;
    } else {
        _visible = false;
        if (!aos) {
            _hauteur = ht;
        }
    }

    if (acalc) {

        // Azimut
        _azimut = atan2(vec.y(), -vec.x());
        if (_azimut < 0.) {
            _azimut += MATHS::DEUX_PI;
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
    _hauteur = -MATHS::PI;
    const double cd = cos(_declinaison);
    const Vecteur3D _vec1 = Vecteur3D(cos(_ascensionDroite) * cd, sin(_ascensionDroite) * cd, sin(_declinaison));
    const Vecteur3D vec2 = observateur.rotHz() * _vec1;

    /* Corps de la methode */
    // Hauteur
    const double ht = asin(vec2.z());

    if (ht > -MATHS::DEG2RAD) {

        // Prise en compte de la refraction atmospherique
        const double refraction = CalculRefractionAtmospherique(ht);

        _hauteur = ht + refraction;
        if (_hauteur >= 0.) {

            // Azimut
            _azimut = atan2(vec2.y(), -vec2.x());
            if (_azimut < 0.) {
                _azimut += MATHS::DEUX_PI;
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
    _longitude = modulo(observateur.tempsSideralGreenwich() - atan2(_position.y(), _position.x()), MATHS::DEUX_PI);
    if (fabs(_longitude) > MATHS::PI) {
        _longitude -= sgn(_longitude) * MATHS::DEUX_PI;
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
    _longitude = modulo(Observateur::CalculTempsSideralGreenwich(date) - atan2(_position.y(), _position.x()), MATHS::DEUX_PI);
    if (fabs(_longitude) > MATHS::PI) {
        _longitude -= sgn(_longitude) * MATHS::DEUX_PI;
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
    _latitude = MATHS::PI;

    /* Corps de la methode */
    _r0 = sqrt(pos.x() * pos.x() + pos.y() * pos.y());
    _latitude = atan2(pos.z(), _r0);

    do {
        lat = _latitude;
        const double sph = sin(lat);
        _ct = 1. / sqrt(1. - TERRE::E2 * sph * sph);
        _latitude = atan((pos.z() + RE2 * _ct * sph) / _r0);

    } while (fabs(_latitude - lat) > 1.e-7);

    /* Retour */
    return (_latitude);
}

/*
 * Calcul des lever/passage au meridien/coucher
 */
void Corps::CalculLeverMeridienCoucher(const Date &date, const DateSysteme &syst, const bool calculCrepuscules)
{
    /* Declarations des variables locales */
    unsigned int fin;

    /* Initialisations */
    const unsigned int taille = (calculCrepuscules) ? 9 : 3;
    std::array<Date, 9> datesEvt;
    std::array<int, 9> listIdx;

    datesEvt.fill(Date(DATE::DATE_INFINIE, 0.));
    listIdx.fill(-1);

    int idx = 1;

    /* Corps de la methode */
    QListIterator it(_ephem);
    Ephemerides eph1 = it.next();

    while (it.hasNext()) {

        const Ephemerides eph2 = it.next();

        const double ht1 = eph1.hauteur;
        const double ht2 = eph2.hauteur;
        const double az1 = eph1.azimut;
        const double az2 = eph2.azimut;

        // Detection d'un passage à l'horizon
        if ((ht1 * ht2) < 0.) {

            if (ht1 < ht2) {

                // Lever
                listIdx[0] = idx;

            } else {

                // Coucher
                listIdx[2] = idx;
            }
        }

        if ((az1 < MATHS::PI) && (az2 > MATHS::PI)) {

            // Passage au meridien
            listIdx[1] = idx;
        }

        // Crepuscules
        if (calculCrepuscules) {
            for(int i=1; i<=3; i++) {

                const double ht1a = ht1 + 6. * i * MATHS::DEG2RAD;
                const double ht2a = ht2 + 6. * i * MATHS::DEG2RAD;

                if ((ht1a * ht2a) < 0.) {

                    if (listIdx.at(0) == -1) {
                        listIdx[2+i] = idx;
                    } else {
                        listIdx[5+i] = idx;
                    }

                }
            }
        }

        fin = 0;
        for(unsigned int i=0; i<taille; i++) {
            if (listIdx[i] != -1) {
                fin++;
            }
        }

        if (fin == taille) {
            it.toBack();
        }

        eph1 = eph2;

        idx++;
    }

    unsigned int iter;
    int j;
    double dateEvt;
    double t_val;
    double yval;
    std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;
    std::array<double, MATHS::DEGRE_INTERPOLATION> val;

    for(unsigned int i=0; i<taille; i++) {

        j = listIdx[i];
        if (j == 0) {
            j = 1;
        }

        if (j >= (_ephem.size() - 1)) {
            j = static_cast<int> (_ephem.size() - 2);
        }

        if (j != -1) {

            const Ephemerides eph0 = _ephem.at(j - 1);
            eph1 = _ephem.at(j);
            const Ephemerides eph2 = _ephem.at(j + 1);

            dateEvt = eph1.jourJulienUTC;
            t_val = 0.;

            // Calcul par interpolation des differentes dates
            jjm[0] = eph0.jourJulienUTC;
            jjm[1] = eph1.jourJulienUTC;
            jjm[2] = eph2.jourJulienUTC;

            if (i == 1) {

                // Passage au meridien
                val[0] = eph0.azimut;
                val[1] = eph1.azimut;
                val[2] = eph2.azimut;
                yval = MATHS::PI;

            } else {

                // Lever ou coucher
                val[0] = eph0.hauteur;
                val[1] = eph1.hauteur;
                val[2] = eph2.hauteur;
                yval = 0.;

                // Crepuscules
                if (calculCrepuscules) {
                    if ((i == 3) || (i == 6)) {
                        yval = -6. * MATHS::DEG2RAD;
                    } else if ((i == 4) || (i == 7)) {
                        yval = -12. * MATHS::DEG2RAD;
                    } else if ((i == 5) || (i == 8)) {
                        yval = -18. * MATHS::DEG2RAD;
                    }
                }
            }

            iter = 0;
            while ((fabs(t_val - dateEvt) > DATE::EPS_DATES) && (iter < MATHS::ITERATIONS_MAX)) {

                t_val = dateEvt;
                dateEvt = Maths::CalculValeurXInterpolation3(jjm, val, yval, 1.e-8);

                iter++;
            }

            if (iter < MATHS::ITERATIONS_MAX) {

                // Date arrondie a la minute
                const Date dateCalc = Date(floor(dateEvt * DATE::NB_MIN_PAR_JOUR + 0.5) * DATE::NB_JOUR_PAR_MIN, date.offsetUTC());
                if (date.jour() == dateCalc.jour()) {
                    datesEvt[i] = dateCalc;
                }
            }
        }
    }

    _dateLever = (fabs(datesEvt[0].jourJulienUTC() - DATE::DATE_INFINIE) < DATE::EPS_DATES) ?
                "-" : datesEvt[0].ToShortDate(DateFormat::FORMAT_COURT, syst).section(" ", 1).remove(5, 3).replace(":", "h").trimmed();
    _dateMeridien = (fabs(datesEvt[1].jourJulienUTC() - DATE::DATE_INFINIE) < DATE::EPS_DATES) ?
                "-" : datesEvt[1].ToShortDate(DateFormat::FORMAT_COURT, syst).section(" ", 1).remove(5, 3).replace(":", "h").trimmed();
    _dateCoucher = (fabs(datesEvt[2].jourJulienUTC() - DATE::DATE_INFINIE) < DATE::EPS_DATES) ?
                "-" : datesEvt[2].ToShortDate(DateFormat::FORMAT_COURT, syst).section(" ", 1).remove(5, 3).replace(":", "h").trimmed();

    if (calculCrepuscules) {
        for(int i=3; i<9; i++) {
            _datesCrepuscules[i-3] =
                    (fabs(datesEvt[i].jourJulienUTC() - DATE::DATE_INFINIE) < DATE::EPS_DATES) ?
                        "-" : datesEvt[i].ToShortDate(DateFormat::FORMAT_COURT, syst).section(" ", 1).remove(5, 3).replace(":", "h").trimmed();
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul de la position et de la vitesse dans le repere ECEF
 */
void Corps::CalculPosVitECEF(const Date &date, Vecteur3D &positionECEF, Vecteur3D &vitesseECEF) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double gmst = Observateur::CalculTempsSideralGreenwich(date);
    const Vecteur3D omegaTerre(0., 0., TERRE::OMEGA);

    /* Corps de la methode */
    positionECEF = _position.Rotation(AxeType::AXE_Z, gmst);
    vitesseECEF = _vitesse.Rotation(AxeType::AXE_Z, gmst) - (omegaTerre ^ positionECEF);

    /* Retour */
    return;
}

/*
 * Calcul de la refraction atmospherique
 */
double Corps::CalculRefractionAtmospherique(const double ht) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double htd = ht * MATHS::RAD2DEG;

    /* Corps de la methode */

    /* Retour */
    return (htd >= -3.) ? MATHS::DEG2RAD * 1.02 / (MATHS::ARCMIN_PAR_DEG * tan(MATHS::DEG2RAD * (htd + 10.3 / (htd + 5.11)))) : 0.;
}

/*
 * Calcul de la zone de visibilite
 */
void Corps::CalculZoneVisibilite(const double beta)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double srad = beta + TERRE::REFRACTION_HZ;
    const double cla = cos(_latitude);
    const double sla = sin(_latitude);
    const double clo = cos(_longitude);
    const double slo = sin(_longitude);
    const double cra = cos(srad);
    const double sra = sin(srad);

    /* Corps de la methode */
    for(unsigned int i=0; i<_caz.size(); i++) {

        const double z1 = sra * _caz[i];

        const double x2 = cra * cla - z1 * sla;
        const double y2 = sra * _saz[i];
        const double z2 = cra * sla + z1 * cla;

        const double x3 = x2 * clo - y2 * slo;
        const double y3 = x2 * slo + y2 * clo;

        const double lo1 = atan2(y3, x3);
        const double la1 = asin(z2);

        const QPointF pt(fmod(MATHS::PI - lo1, MATHS::DEUX_PI) * MATHS::RAD2DEG, (MATHS::PI_SUR_DEUX - la1) * MATHS::RAD2DEG);
        _zone[i] = pt;
    }

    /* Retour */
    return;
}

/*
 * Initialisation des elements relatifs aux corps
 */
void Corps::Initialisation(const QString &dirCommonData)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _tabConst.clear();

    /* Corps de la methode */
    // Tableaux des cosinus et des sinus de l'azimut
    for(unsigned int i=0; i<_caz.size(); i++) {
        const double az = i * MATHS::DEG2RAD;
        _caz[i] = cos(az);
        _saz[i] = sin(az);
    }

    try {

        // Lecture du fichier de constellations
        const QString fichierConstellations = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "constellations.dat";

        QFile fi(fichierConstellations);
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

                const QString contenuFichier = fi.readAll();
                const QStringList listeConstellations = contenuFichier.split("\n", Qt::SkipEmptyParts);

                ConstElem cst;
                QStringListIterator it (listeConstellations);
                while (it.hasNext()) {

                    const QString ligne = it.next();

                    if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#')) {

                        const QStringList list = ligne.split(" ", Qt::SkipEmptyParts);

                        cst.nom = list.first();
                        cst.ad1 = list.at(1).toDouble() * MATHS::HEUR2RAD;
                        cst.ad2 = list.at(2).toDouble() * MATHS::HEUR2RAD;
                        cst.dec = list.at(3).toDouble() * MATHS::DEG2RAD;
                        _tabConst.append(cst);
                    }
                }
            }
            fi.close();

            if (_tabConst.isEmpty()) {
                const QFileInfo ff(fi.fileName());
                throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                        .arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
            }

#if (BUILD_TEST == false)
            qInfo() << "Lecture fichier constellations.dat OK";
#endif

        } else {
            const QFileInfo ff(fi.fileName());
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2")
                                    .arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

/*
 * Conversion d'un vecteur en coordonnees ecliptiques spheriques en coordonnees cartesiennes equatoriales
 */
Vecteur3D Corps::Sph2Cart(const Vecteur3D &vecteur, const Date &date) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double t = date.jourJulienTT() * DATE::NB_SIECJ_PAR_JOURS;

    /* Corps de la methode */
    const double om = MATHS::DEG2RAD * modulo(125.04 - 1934.136 * t, MATHS::T360);
    const double x = vecteur.x() - MATHS::DEG2RAD * 0.00478 * sin(om);
    const double obliquite = MATHS::ARCSEC2RAD * (84381.448 + t * (-46.815 + t * (-0.00059 + 0.001813 * t)));
    const double cb = cos(vecteur.y());
    const double sb = sin(vecteur.y());
    const double ce = cos(obliquite);
    const double se = sin(obliquite);
    const double xx = vecteur.z() * cb * sin(x);

    /* Retour */
    return Vecteur3D(vecteur.z() * cb * cos(x), xx * ce - vecteur.z() * se * sb, xx * se + vecteur.z() * ce * sb);
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

const QString &Corps::constellation() const
{
    return _constellation;
}

double Corps::declinaison() const
{
    return _declinaison;
}

const Vecteur3D &Corps::dist() const
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

const Vecteur3D &Corps::position() const
{
    return _position;
}

double Corps::rangeRate() const
{
    return _rangeRate;
}

bool Corps::isVisible() const
{
    return _visible;
}

const Vecteur3D &Corps::vitesse() const
{
    return _vitesse;
}

const std::array<QPointF, 360> &Corps::zone() const
{
    return _zone;
}

const QString &Corps::dateLever() const
{
    return _dateLever;
}

const QString &Corps::dateMeridien() const
{
    return _dateMeridien;
}

const QString &Corps::dateCoucher() const
{
    return _dateCoucher;
}

const std::array<QString, 6> &Corps::datesCrepuscules() const
{
    return _datesCrepuscules;
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
