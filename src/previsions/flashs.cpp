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
 * >    flashs.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des flashs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    12 septembre 2015
 *
 * Date de revision
 * >    14 octobre 2022
 *
 */

#include <QElapsedTimer>
#include "configuration/configuration.h"
#include "flashs.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/maths/maths.h"

#define NB_PAN (3)


struct EphemeridesFlashs
{
    double jourJulienUTC;
    Vecteur3D positionObservateur;
    Matrice3D rotHz;
    Vecteur3D positionSoleil;
};

// Pas de calcul ou d'interpolation
static const double PAS0 = NB_JOUR_PAR_MIN;
static const double PAS1 = 10. * NB_JOUR_PAR_SEC;
static const double PAS_INT0 = 30. * NB_JOUR_PAR_SEC;
static const double PAS_INT1 = 2. * NB_JOUR_PAR_SEC;
static const double TEMPS1 = 16. * NB_JOUR_PAR_MIN;
static const double TEMPS2 = 76. * NB_JOUR_PAR_MIN;

// Nom et numeros des panneaux
static const QByteArray LISTE_MIR = QObject::tr("FCB", "Front, Central, Backward panels of MetOp satellite").toLatin1();
static const int LISTE_PAN[NB_PAN] = { 0, 1, 2 };

static int _pan;
static char _mir;
static Vecteur3D _direction;
static ConditionsFlashs _conditions;
static QList<QList<ResultatPrevisions> > _resultatSat;
static QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
static DonneesPrevisions _donnees;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Accesseurs
 */
QMap<QString, QList<QList<ResultatPrevisions> > > &Flashs::resultats()
{
    return _resultats;
}

DonneesPrevisions Flashs::donnees()
{
    return _donnees;
}


/*
 * Modificateurs
 */
void Flashs::setConditions(const ConditionsFlashs &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Determination des flashs
 */
int Flashs::CalculFlashs(int &nombre)
{
    /* Declarations des variables locales */
    QElapsedTimer tps;
    QList<Satellite> sats;

    /* Initialisations */
    double tlemin = -DATE_INFINIE;
    double tlemax = DATE_INFINIE;
    const double angrefMax = 1.;
    const double pasmax = 3. * PAS1;

    tps.start();
    _donnees.ageElementsOrbitaux.clear();
    _resultats.clear();

    // Creation de la liste d'elements orbitaux
    const QMap<QString, ElementsOrbitaux> tabElem = GPFormat::LectureFichier(_conditions.fichier, Configuration::instance()->donneesSatellites(),
                                                          Configuration::instance()->lgRec(), _conditions.listeSatellites);
    // TODO si TLE

    // Creation du tableau de satellites
    QMapIterator<QString, ElementsOrbitaux> it1(tabElem);
    while (it1.hasNext()) {
        it1.next();

        const ElementsOrbitaux elem = it1.value();
        sats.append(Satellite(elem));

        const double epok = elem.epoque.jourJulienUTC();
        if (epok > tlemin) {
            tlemin = epok;
        }

        if (epok < tlemax) {
            tlemax = epok;
        }
    }

    if (tabElem.keys().count() == 1) {
        _donnees.ageElementsOrbitaux.append(fabs(_conditions.jj1 - tlemin));
    } else {

        const double age1 = fabs(_conditions.jj1 - tlemin);
        const double age2 = fabs(_conditions.jj1 - tlemax);
        _donnees.ageElementsOrbitaux.append(qMin(age1, age2));
        _donnees.ageElementsOrbitaux.append(qMax(age1, age2));
    }

    // Calcul des ephemerides du Soleil et du lieu d'observation
    const QList<EphemeridesFlashs> tabEphem = CalculEphemSoleilObservateur();


    /* Corps de la methode */
    QListIterator<EphemeridesFlashs> it2(tabEphem);

    bool atrouve;
    double jj0;
    double pasInt;
    double temp;
    Date date;
    Soleil soleil;
    Lune lune;
    Satellite sat;
    ConditionEclipse condEcl;
    std::array<double, DEGRE_INTERPOLATION> jjm;
    QPair<double, double> minmax;

    // Boucle sur les satellites
    QListIterator<Satellite> it4(sats);
    while (it4.hasNext()) {

        sat = it4.next();

        temp = -DATE_INFINIE;
        _resultatSat.clear();

        // Bouble sur le tableau d'ephemerides
        it2.toFront();
        while (it2.hasNext()) {

            const EphemeridesFlashs ephem = it2.next();

            // Date
            date = Date(ephem.jourJulienUTC, 0., false);

            // Lieu d'observation
            const Observateur obs(ephem.positionObservateur, Vecteur3D(), ephem.rotHz, _conditions.observateur.aaer(), _conditions.observateur.aray());

            // Position ECI du Soleil
            soleil = Soleil(ephem.positionSoleil);

            // Position du satellite
            sat.CalculPosVit(date);
            sat.CalculCoordHoriz(obs, false);

            // Le satellite a une hauteur superieure a celle specifiee par l'utilisateur
            if (sat.hauteur() >= _conditions.hauteur) {

                if (_conditions.calcEclipseLune) {
                    lune.CalculPosition(date);
                }

                // Determination de la condition d'eclipse du satellite
                condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

                // Le satellite n'est pas eclipse
                if (!condEcl.eclipseTotale()) {

                    jj0 = date.jourJulienUTC();
                    const double jj2 = jj0 + TEMPS1;

                    do {

                        // Calcul de l'angle de reflexion
                        _pan = 0;
                        const double angref = AngleReflexion(sat, soleil);
                        const double pas = (angref < 0.5) ? PAS1 : PAS0;

                        if (angref <= angrefMax) {

                            jjm[0] = jj0 - NB_JOUR_PAR_MIN;
                            jjm[1] = jj0;
                            jjm[2] = jj0 + NB_JOUR_PAR_MIN;

                            // Calcul par interpolation de l'instant correspondant
                            // a l'angle de reflexion minimum
                            minmax = CalculAngleMin(jjm, sat, soleil);

                            // Iterations supplementaires pour affiner la date du maximum
                            pasInt = PAS_INT0;
                            for (int it=0; it<4; it++) {

                                jjm[0] = minmax.first - pasInt;
                                jjm[1] = minmax.first;
                                jjm[2] = minmax.first + pasInt;

                                minmax = CalculAngleMin(jjm, sat, soleil);
                                pasInt *= 0.5;
                            }

                            if (fabs(minmax.first - temp) > pasmax) {
                                DeterminationFlash(minmax, temp, sat, soleil);
                            }

                        } // fin if (angref <= angrefMax)

                        jj0 += pas;
                        const Date date0(jj0, 0., false);

                        _conditions.observateur.CalculPosVit(date0);

                        // Position du satellite
                        sat.CalculPosVit(date0);
                        sat.CalculCoordHoriz(_conditions.observateur, false);

                        // Position du Soleil
                        soleil.CalculPosition(date0);

                        if (_conditions.calcEclipseLune) {
                            lune.CalculPosition(date0);
                        }

                        // Condition d'eclipse du satellite
                        condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

                        if (condEcl.eclipseTotale() || (sat.hauteur() < _conditions.hauteur)) {
                            jj0 = jj2 + PAS0;
                        }
                    } while (jj0 <= jj2);

                    date = Date(jj0 + TEMPS2, 0., false);
                }
                date = Date(date.jourJulienUTC() + PAS0, 0., false);

                // Recherche de la nouvelle date dans le tableau d'ephemerides
                atrouve = false;
                while (it2.hasNext() && !atrouve) {
                    const double jj = it2.next().jourJulienUTC;
                    if (jj >= date.jourJulienUTC()) {
                        atrouve = true;
                        it2.previous();
                    }
                }
            }
        }

        if (!_resultatSat.isEmpty()) {
            _resultats.insert(sat.elementsOrbitaux().norad, _resultatSat);
        }
    }

    _donnees.tempsEcoule = tps.elapsed();

    /* Retour */
    return nombre;
}

/*
 * Calcul de la magnitude du flash
 */
double Flashs::CalculMagnitudeFlash(const Date &date, const Satellite &satellite, const Soleil &soleil, const bool calcEclipseLune,
                                    const bool refraction)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Satellite sat = satellite;
    Lune lune;
    if (calcEclipseLune) {
        lune.CalculPosition(date);
    }

    ConditionEclipse condEcl;
    condEcl.CalculSatelliteEclipse(satellite.position(), soleil, lune, refraction);

    /* Corps de la methode */
    const double ang = AngleReflexion(satellite, soleil);
    const double magnitude = MagnitudeFlash(ang, condEcl, sat);

    /* Retour */
    return magnitude;
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
 * Calcul de l'angle de reflexion du panneau
 */
double Flashs::AngleReflexion(const Satellite &satellite, const Soleil &soleil)
{
    /* Declarations des variables locales */
    int imin;
    int imax;
    Matrice3D matrice1;
    Matrice3D matrice3;

    /* Initialisations */
    const SatellitesFlashs sts = Configuration::instance()->mapFlashs()[satellite.elementsOrbitaux().norad];
    double ang = PI;
    int j = 0;
    imin = _pan;
    imax = static_cast<int> (sts.angles.count());

    /* Corps de la methode */
    for(int i=imin; i<imax; i++) {

        matrice1 = RotationYawSteering(satellite, sts.angles.at(i).first, sts.angles.at(i).second);
        const Matrice3D matrice2 = matrice1.Transposee();
        const Vecteur3D vecteur1 = matrice2.vecteur1();

        const Vecteur3D solsat = soleil.position() - satellite.position();
        const double surf = solsat.Angle(vecteur1);

        matrice3 = RotationRV(solsat, vecteur1, 0., 0., 0);
        const Matrice3D matrice4(AxeType::AXE_Z, -surf);
        const Vecteur3D vecteur2 = matrice4.vecteur1();
        const Matrice3D matrice5 = matrice3.Transposee();
        const Vecteur3D vecteur3 = matrice5 * vecteur2;

        const Vecteur3D obsat = -satellite.dist();
        const double tmp = vecteur3.Angle(obsat);

        if (tmp < ang) {
            ang = tmp;
            j = i;
            _mir = (sts.nomsat.contains("metop", Qt::CaseInsensitive)) ? LISTE_MIR[j] : 'S';
            _direction = vecteur3;
        }
    }

    _pan = (sts.nomsat.contains("metop", Qt::CaseInsensitive)) ? LISTE_PAN[j] : 0;

    /* Retour */
    return (ang);
}

/*
 * Calcul de l'angle minimum du panneau
 */
QPair<double, double> Flashs::CalculAngleMin(const std::array<double, DEGRE_INTERPOLATION> jjm, Satellite &satellite, Soleil &soleil)
{
    /* Declarations des variables locales */
    std::array<double, DEGRE_INTERPOLATION> ang;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        _conditions.observateur.CalculPosVit(date);

        // Position du satellite
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(_conditions.observateur, false);

        // Position du Soleil
        soleil.CalculPosition(date);

        // Angle de reflexion
        ang[i] = AngleReflexion(satellite, soleil);
    }

    /* Retour */
    return Maths::CalculExtremumInterpolation3(jjm, ang);
}

/*
 * Calcul des ephemerides du Soleil et de l'observateur
 */
QList<EphemeridesFlashs> Flashs::CalculEphemSoleilObservateur()
{
    /* Declarations des variables locales */
    Soleil soleil;
    EphemeridesFlashs eph;
    QList<EphemeridesFlashs> tabEphem;

    /* Initialisations */
    const double pas = NB_JOUR_PAR_MIN;

    /* Corps de la methode */
    Date date(_conditions.jj1, 0., false);
    do {

        // Position ECI de l'observateur
        _conditions.observateur.CalculPosVit(date);

        // Position du Soleil
        soleil.CalculPosition(date);
        soleil.CalculCoordHoriz(_conditions.observateur, false);

        if (soleil.hauteur() <= _conditions.crepuscule) {

            eph.jourJulienUTC = date.jourJulienUTC();
            eph.positionObservateur = _conditions.observateur.position();
            eph.rotHz = _conditions.observateur.rotHz();
            eph.positionSoleil = soleil.position();

            tabEphem.append(eph);
        }

        date = Date(date.jourJulienUTC() + pas, 0., false);
    } while (date.jourJulienUTC() <= _conditions.jj2);

    /* Retour */
    return tabEphem;
}

/*
 * Calcul des bornes inferieures et superieures du flash
 */
void Flashs::CalculLimitesFlash(const double mgn0, const double dateMaxFlash, Satellite &satellite, Soleil &soleil, std::array<Date, 3> lim)
{
    /* Declarations des variables locales */
    double tmp;
    std::array<double, DEGRE_INTERPOLATION> jjm;
    std::array<double, 4> limite;
    std::array<double, 4> lim0;

    /* Initialisations */
    double dateInf;
    double dateSup = DATE_INFINIE;
    double jj0 = dateMaxFlash - PAS_INT0;
    double jj2 = dateMaxFlash + PAS_INT0;

    /* Corps de la methode */
    // Determination de la date inferieure du flash
    jjm[0] = jj0 - PAS1;
    jjm[1] = 0.5 * (dateMaxFlash + jj0 - PAS1);
    jjm[2] = dateMaxFlash;

    LimiteFlash(mgn0, jjm, satellite, soleil, limite);

    limite[1] = DATE_INFINIE;

    int it;
    double pasInt;

    for (int i=0; i<4; i++) {
        lim0[i] = limite[i];
        if (lim0[i] < DATE_INFINIE) {

            it = 0;
            pasInt = PAS_INT1;
            if (fabs(mgn0 - _conditions.magnitudeLimite) <= EPSDBL100) {
                pasInt *= 0.5;
            }
            do {
                it++;
                tmp = lim0[i];
                jjm[0] = lim0[i] - pasInt;
                jjm[1] = lim0[i];
                jjm[2] = lim0[i] + pasInt;

                LimiteFlash(mgn0, jjm, satellite, soleil, lim0);
                pasInt *= 0.5;
            } while ((fabs(lim0[i] - tmp) > EPS_DATES) && (lim0[i] < DATE_INFINIE) && (it < 10));

            limite[i] = ((lim0[i] < DATE_INFINIE) && (it < 10)) ? lim0[i] : -DATE_INFINIE;
        } else {
            limite[i] = -DATE_INFINIE;
        }
    }

    dateInf = qMax(limite[0], limite[1]);
    dateInf = qMax(dateInf, limite[2]);
    dateInf = qMax(dateInf, limite[3]);


    // Determination de la date superieure du flash
    jjm[0] = dateMaxFlash;
    jjm[1] = 0.5 * (dateMaxFlash + jj2 + PAS1);
    jjm[2] = jj2 + PAS1;

    LimiteFlash(mgn0, jjm, satellite, soleil, limite);

    limite[1] = DATE_INFINIE;

    for (int i=0; i<4; i++) {
        lim0[i] = limite[i];
        if (lim0[i] < DATE_INFINIE) {

            it = 0;
            pasInt = PAS_INT1;
            if (fabs(mgn0 - _conditions.magnitudeLimite) <= EPSDBL100) {
                pasInt *= 0.5;
            }
            do {
                it++;
                tmp = lim0[i];
                jjm[0] = lim0[i] - pasInt;
                jjm[1] = lim0[i];
                jjm[2] = lim0[i] + pasInt;

                LimiteFlash(mgn0, jjm, satellite, soleil, lim0);
                pasInt *= 0.5;
            } while ((fabs(lim0[i] - tmp) > EPS_DATES) && (lim0[i] < DATE_INFINIE) && (it < 10));

            limite[i] = ((lim0[i] < DATE_INFINIE) && (it < 10)) ? lim0[i] : DATE_INFINIE;
        } else {
            limite[i] = DATE_INFINIE;
        }
    }

    dateSup = qMin(limite[0], limite[1]);
    dateSup = qMin(dateSup, limite[2]);
    dateSup = qMin(dateSup, limite[3]);

    double dateMax = dateMaxFlash;

    jjm[0] = dateInf;
    jjm[1] = 0.5 * (dateInf + dateSup);
    jjm[2] = dateSup;

    QPair<double, double> minmax;
    minmax = CalculAngleMin(jjm, satellite, soleil);

    // Iterations supplementaires pour affiner la date du maximum
    pasInt = PAS_INT0;
    for (int i=0; i<4; i++) {

        jjm[0] = minmax.first - pasInt;
        jjm[1] = minmax.first;
        jjm[2] = minmax.first + pasInt;

        minmax = CalculAngleMin(jjm, satellite, soleil);
        pasInt *= 0.5;
    }

    dateMax = minmax.first;

    if ((dateInf < (dateSup - EPS_DATES)) && (fabs(dateInf) < DATE_INFINIE) && (fabs(dateSup) < DATE_INFINIE)) {

        if (dateMax < dateInf) {
            dateMax = dateInf;
        }

        if (dateMax > dateSup) {
            dateMax = dateSup;
        }

        lim[0] = Date(dateInf, 0., false);
        lim[1] = Date(dateMax, 0., false);
        lim[2] = Date(dateSup, 0., false);

    } else {
        lim[1] = Date(DATE_INFINIE, 0., false);
    }

    /* Retour */
    return;
}

/*
 * Determination du flash
 */
void Flashs::DeterminationFlash(const QPair<double, double> minmax, double &temp, Satellite &sat, Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Date date = Date(minmax.first, 0., false);

    /* Corps de la methode */
    // Position de l'observateur
    _conditions.observateur.CalculPosVit(date);

    // Position du satellite
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(_conditions.observateur, false);

    if (sat.hauteur() >= 0.) {

        // Position du Soleil
        soleil.CalculPosition(date);
        soleil.CalculCoordHoriz(_conditions.observateur, false);

        // Angle de reflexion
        const double ang = AngleReflexion(sat, soleil);

        if (ang <= _conditions.angleLimite) {

            const double mgn0 = _conditions.magnitudeLimite;

            Lune lune;
            if (_conditions.calcEclipseLune) {
                lune.CalculPosition(date);
            }

            ConditionEclipse condEcl;
            condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

            // Magnitude du flash
            double mag = MagnitudeFlash(minmax.second, condEcl, sat);

            if (mag <= mgn0) {

                std::array<Date, 3> dates;

                // Calcul des limites du flash
                CalculLimitesFlash(mgn0, minmax.first, sat, soleil, dates);
                const double pasmax = 3. * PAS1;

                if ((dates[1].jourJulienUTC() < DATE_INFINIE) && (fabs(dates[1].jourJulienUTC() - temp) > pasmax)) {

                    temp = minmax.first;

                    // Calcul des valeurs exactes pour les differentes dates
                    Observateur obsmax;
                    ConditionEclipse condEcl2;
                    ResultatPrevisions res;
                    QList<ResultatPrevisions> result;
                    for(int i=0; i<3; i++) {

                        res.obsmax = Observateur();

                        _conditions.observateur.CalculPosVit(dates[i]);

                        // Position du satellite
                        sat.CalculPosVit(dates[i]);
                        sat.CalculCoordHoriz(_conditions.observateur);

                        // Position du Soleil
                        soleil.CalculPosition(dates[i]);
                        soleil.CalculCoordHoriz(_conditions.observateur);

                        if (_conditions.calcEclipseLune) {
                            lune.CalculPosition(dates[i]);
                        }

                        // Condition d'eclipse du satellite
                        condEcl2.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

                        // Angle de reflexion
                        const double angref = AngleReflexion(sat, soleil);

                        if (angref <= (_conditions.angleLimite + 0.0001)) {

                            // Magnitude du flash
                            mag = MagnitudeFlash(angref, condEcl2, sat);

                            if (mag <= (mgn0 + 0.05)) {

                                // Ascension droite/declinaison/constellation
                                sat.CalculCoordEquat(_conditions.observateur);

                                // Ecriture du flash

                                // Nom du satellite
                                res.nom = Configuration::instance()->mapFlashs()[sat.elementsOrbitaux().norad].nomsat;

                                // Elements orbitaux
                                res.elements = sat.elementsOrbitaux();

                                // Date calendaire (UTC)
                                res.date = Date(dates[i].jourJulienUTC(), 0.);

                                // Coordonnees topocentriques du satellite
                                res.azimut = sat.azimut();
                                res.hauteur = sat.hauteur();

                                // Coordonnees equatoriales du satellite
                                res.ascensionDroite = sat.ascensionDroite();
                                res.declinaison = sat.declinaison();
                                res.constellation = sat.constellation();

                                // Magnitude
                                res.magnitude = mag;
                                res.magnitudeStd = sat.elementsOrbitaux().donnees.magnitudeStandard();
                                res.penombre = (condEcl.eclipsePartielle() || condEcl.eclipseAnnulaire());

                                // Angle de reflexion
                                res.angleReflexion = angref;

                                // Miroir
                                res.miroir = _mir;

                                // Altitude et distance du satellite
                                sat.CalculLatitude(sat.position());
                                res.altitude = sat.CalculAltitude(sat.position());
                                res.distance = sat.distance();

                                // Coordonnees topocentriques du Soleil
                                res.azimutSoleil = soleil.azimut();
                                res.hauteurSoleil = soleil.hauteur();

                                // Recherche des coordonnees geographiques ou se produit le maximum du flash
                                obsmax = Observateur::CalculIntersectionEllipsoide(dates[1], sat.position(), _direction);

                                if (!obsmax.nomlieu().isEmpty()) {

                                    res.obsmax = obsmax;
                                    obsmax.CalculPosVit(dates[1]);
                                    sat.CalculCoordHoriz(obsmax, false);

                                    // Distance entre les 2 lieux d'observation
                                    res.distanceObs = _conditions.observateur.CalculDistance(obsmax);

                                    // Cap en direction du maximum
                                    res.cap = _conditions.observateur.CalculCap(obsmax).first;

                                    // Angle de reflexion pour le lieu du maximum
                                    const double angRefMax = AngleReflexion(sat, soleil);

                                    // Magnitude du flash et penombre
                                    res.magnitudeMax = MagnitudeFlash(angRefMax, condEcl, sat);
                                    res.penombreMax = (condEcl.eclipsePartielle() || condEcl.eclipseAnnulaire());
                                }

                                result.append(res);
                            }
                        }
                    }

                    if (result.count() == 3) {
                        _resultatSat.append(result);
                    }
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Calcul d'une limite du flash
 */
void Flashs::LimiteFlash(const double mgn0, const std::array<double, DEGRE_INTERPOLATION> jjm, Satellite &satellite, Soleil &soleil,
                         std::array<double, 4> limite)
{
    /* Declarations des variables locales */
    Lune lune;
    ConditionEclipse condEcl;
    std::array<double, DEGRE_INTERPOLATION> ang;
    std::array<double, DEGRE_INTERPOLATION> ecl;
    std::array<double, DEGRE_INTERPOLATION> ht;
    std::array<double, DEGRE_INTERPOLATION> mag;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        _conditions.observateur.CalculPosVit(date);

        // Position du satellite
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(_conditions.observateur, false);
        ht[i] = satellite.hauteur();

        // Position du Soleil
        soleil.CalculPosition(date);

        if (_conditions.calcEclipseLune) {
            lune.CalculPosition(date);
        }

        // Conditions d'eclipse du satellite
        condEcl.CalculSatelliteEclipse(satellite.position(), soleil, lune, _conditions.refraction);
        ecl[i] = (condEcl.eclipseLune().luminosite < condEcl.eclipseSoleil().luminosite) ?
                       condEcl.eclipseLune().phi - condEcl.eclipseLune().phiSoleil - condEcl.eclipseLune().elongation :
                       condEcl.eclipseSoleil().phi - condEcl.eclipseSoleil().phiSoleil - condEcl.eclipseSoleil().elongation;

        // Angle de reflexion
        ang[i] = AngleReflexion(satellite, soleil);

        // Magnitude du satellite
        mag[i] = MagnitudeFlash(ang[i], condEcl, satellite);
    }

    double t_ecl, t_ht;
    // Calcul par interpolation de la date pour laquelle la magnitude est egale a la magnitude specifiee par l'utilisateur
    const double t_mag = Maths::CalculValeurXInterpolation3(jjm, mag, mgn0, EPS_DATES);

    // Calcul par interpolation de la date pour laquelle l'angle de reflexion est egal a l'angle de reflexion specifie par l'utilisateur
    const double t_ang = Maths::CalculValeurXInterpolation3(jjm, ang, _conditions.angleLimite, EPS_DATES);

    // Calcul par interpolation de la date pour laquelle la hauteur est egale a la hauteur specifie par l'utilisateur
    if (((ht[0] - _conditions.hauteur) * (ht[2] - _conditions.hauteur) < 0.) ||
            ((ht[0] < _conditions.hauteur) && (ht[2] < _conditions.hauteur))) {
        t_ht = Maths::CalculValeurXInterpolation3(jjm, ht, _conditions.hauteur, EPS_DATES);
    } else {
        t_ht = DATE_INFINIE;
    }

    if ((ecl[0] * ecl[2] < 0.) || (ecl[0] > 0. && ecl[2] > 0.)) {
        t_ecl = Maths::CalculValeurXInterpolation3(jjm, ecl, 0., EPS_DATES);
    } else {
        t_ecl = DATE_INFINIE;
    }

    limite[0] = t_mag;
    limite[1] = t_ang;
    limite[2] = t_ht;
    limite[3] = t_ecl;

    /* Retour */
    return;
}

/*
 * Determination de la magnitude du flash
 */
double Flashs::MagnitudeFlash(const double angle, const ConditionEclipse &condEcl, Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;
    const double angDeg = angle * RAD2DEG;

    /* Corps de la methode */
    if (satellite.elementsOrbitaux().nom.contains("metop", Qt::CaseInsensitive)) {

        // Magnitude pour l'instrument ASCAT (satellites MetOp)
        if (_pan == 1) {
            magnitude = (angDeg < 0.5) ? 0.5333 * angDeg - 3.6 : 1.9419 * log(angDeg) - 2.769;
        } else {
            magnitude = angDeg - 5.;
        }
    } else {

        // Magnitude pour l'antenne SAR
        magnitude = 0.3061 * angDeg - 2.6051;
    }

    // Prise en compte des eclipses partielles ou annulaires
    if (_conditions.effetEclipsePartielle) {
        const double luminositeEclipse = qMin(condEcl.eclipseLune().luminosite, condEcl.eclipseSoleil().luminosite);
        if (luminositeEclipse > 0. && luminositeEclipse <= 1.) {
            magnitude += -2.5 * log10(luminositeEclipse);
        }
    }

    // Prise en compte de l'extinction atmospherique
    if (_conditions.extinction) {
        magnitude += satellite.magnitude().ExtinctionAtmospherique(_conditions.observateur, satellite.hauteur());
    }

    /* Retour */
    return (magnitude);
}

/*
 * Calcul de la matrice de rotation du repere equatorial au repere orbital local
 */
Matrice3D Flashs::RotationRV(const Vecteur3D &position, const Vecteur3D &vitesse, const double lacet, const double tangage, const int inpl)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Vecteur3D w((position ^ vitesse).Normalise());

    /* Corps de la methode */
    const double alpha = atan2(w.y(), w.x()) + PI;
    const Matrice3D matrice1(AxeType::AXE_Z, alpha);

    const double beta = -acos(w.z());
    const Matrice3D matrice2(AxeType::AXE_Y, beta);

    // Conversion dans le plan de l'orbite
    const Matrice3D matrice3 = matrice2 * matrice1;
    const Vecteur3D vecteur1 = matrice3 * vitesse;

    const double gamma = atan2(vecteur1.y(), vecteur1.x());
    const Matrice3D matrice4(AxeType::AXE_Z, gamma);
    Matrice3D matrice = matrice4 * matrice3;

    if (inpl != 0) {
        const Matrice3D matrice5(AxeType::AXE_X, PI_SUR_DEUX);
        const Matrice3D matrice6 = matrice5 * matrice;
        matrice = matrice6;

        if (inpl == 2) {
            const double delta = position.Angle(vitesse) - PI_SUR_DEUX;
            const Matrice3D matrice7(AxeType::AXE_Y, delta);
            const Matrice3D matrice8 = matrice7 * matrice;
            matrice = matrice8;
        }
    }

    // Rotation en lacet
    if (fabs(lacet) > EPSDBL100) {
        const Matrice3D matrice9(AxeType::AXE_Z, -lacet);
        const Matrice3D matrice10 = matrice9 * matrice;
        matrice = matrice10;
    }

    // Rotation en tangage
    if (fabs(tangage) > EPSDBL100) {
        const Matrice3D matrice11(AxeType::AXE_Y, -tangage);
        const Matrice3D matrice12 = matrice11 * matrice;
        matrice = matrice12;
    }

    /* Retour */
    return (matrice);
}

/*
 * Calcul de la matrice de rotation du repere equatorial au repere defini par la loi locale de yaw steering
 */
Matrice3D Flashs::RotationYawSteering(const Satellite &satellite, const double lacet, const double tangage)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double yaw = lacet;

    /* Corps de la methode */
    Matrice3D matrice = RotationRV(satellite.position(), satellite.vitesse(), 0., 0., 2);
    const Vecteur3D vecteur1 = satellite.position().Normalise();

    const double cosphisq = 1. - vecteur1.z() * vecteur1.z();
    const double cosphi = (cosphisq < 0.) ? 0. : sqrt(cosphisq);
    double cosphi1 = cosphi;
    const double cosinc = cos(satellite.elementsOrbitaux().inclo * DEG2RAD);

    if ((cosphi > 0.) && (cosphi > fabs(cosinc))) {

        const double sinpsi = cosinc / cosphi;
        const double psi = asin(sinpsi);
        const double cospsi = cos(psi);

        if (cospsi > 0.) {

            double psi1 = psi;
            if (satellite.vitesse().z() < 0.) {
                psi1 = PI - psi1;
            }
            psi1 = modulo(psi1, DEUX_PI);

            double phi1 = acos(cosphi1);
            double corrgeo = fabs(APLA * sin(2. * phi1));
            if (satellite.position().z() < 0.) {
                corrgeo = -corrgeo;
            }

            const Matrice3D matrice1(AxeType::AXE_Z, psi1);
            const Matrice3D matrice2 = matrice1 * matrice;
            const Matrice3D matrice3(AxeType::AXE_Y, corrgeo);
            const Matrice3D matrice4 = matrice3 * matrice2;
            const Matrice3D matrice5(AxeType::AXE_Z, -psi1);
            matrice = matrice5 * matrice4;

            const double tanalpha = (sinpsi - cosphi / satellite.elementsOrbitaux().no) / cospsi;
            if (tanalpha < PI_SUR_DEUX) {

                double alpha = atan(tanalpha);
                alpha -= psi;
                if (satellite.vitesse().z() < 0.) {
                    alpha = -alpha;
                }
                yaw += alpha;
            }
        }
    }

    // Rotation en lacet
    if (fabs(yaw) > EPSDBL100) {
        const Matrice3D matrice6(AxeType::AXE_Z, -yaw);
        const Matrice3D matrice7 = matrice6 * matrice;
        matrice = matrice7;
    }

    // Rotation en tangage
    if (fabs(tangage) > EPSDBL100) {
        const Matrice3D matrice8(AxeType::AXE_Y, -tangage);
        const Matrice3D matrice9 = matrice8 * matrice;
        matrice = matrice9;
    }

    /* Retour */
    return (matrice);
}
