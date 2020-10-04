/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    transitsiss.cpp
 *
 * Localisation
 * >    previsions
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

#include <QElapsedTimer>
#include <QMap>
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/maths/maths.h"
#include "transitsiss.h"


// Pas de calcul ou d'interpolation
static const double PAS0 = NB_JOUR_PAR_MIN;
static const double PAS1 = 10. * NB_JOUR_PAR_SEC;
static const double PAS_INT0 = 10. * NB_JOUR_PAR_SEC;
static const double TEMPS1 = 16. * NB_JOUR_PAR_MIN;

static ConditionsPrevisions _conditions;
static QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
static DonneesPrevisions _donnees;

struct Ephemerides
{
    double jourJulienUTC;
    Vecteur3D positionObservateur;
    Matrice3D rotHz;
    Vecteur3D positionSoleil;
    Vecteur3D positionLune;
    CorpsTransit corps;
};


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Accesseurs
 */
QMap<QString, QList<QList<ResultatPrevisions> > > &TransitsIss::resultats()
{
    return _resultats;
}

DonneesPrevisions TransitsIss::donnees()
{
    return _donnees;
}


/*
 * Modificateurs
 */
void TransitsIss::setConditions(const ConditionsPrevisions &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des transits de l'ISS devant la Lune ou le Soleil
 */
int TransitsIss::CalculTransits(int &nombre)
{
    /* Declarations des variables locales */
    QElapsedTimer tps;
    Soleil soleil;
    Lune lune;

    /* Initialisations */
    tps.start();
    _donnees.ageTle.clear();
    _resultats.clear();

    // Lecture du TLE de l'ISS
    const QList<TLE> tabtle = TLE::LectureFichier3le(_conditions.fichier);
    const double periode = 1. / tabtle.at(0).no() - TEMPS1;
    Satellite sat(tabtle);

    const double age1 = fabs(_conditions.jj1 - tabtle.first().epoque().jourJulienUTC());
    const double age2 = fabs(_conditions.jj1 - tabtle.last().epoque().jourJulienUTC());
    _donnees.ageTle.append(qMin(age1, age2));
    _donnees.ageTle.append(qMax(age1, age2));

    // Generation des ephemerides du Soleil et de la Lune
    const QMap<CorpsTransit, QList<Ephemerides> > tabEphem = CalculEphemSoleilLune();

    /* Corps de la methode */
    // Boucle sur le tableau d'ephemerides
    QMapIterator<CorpsTransit, QList<Ephemerides> > it1(tabEphem);
    while (it1.hasNext()) {
        it1.next();

        const CorpsTransit typeCorps = it1.key();

        QListIterator<Ephemerides> it2(it1.value());
        while (it2.hasNext()) {

            const Ephemerides ephem = it2.next();

            // Date
            Date date(ephem.jourJulienUTC, 0., false);

            // Lieu d'observation
            const Observateur obs(ephem.positionObservateur, Vecteur3D(), ephem.rotHz, _conditions.observateur.aaer(), _conditions.observateur.aray());

            Corps corps;
            corps.setPosition((typeCorps == CORPS_SOLEIL) ? ephem.positionSoleil : ephem.positionLune);

            // Position de l'ISS
            sat.CalculPosVit(date);
            sat.CalculCoordHoriz(obs, false);

            if (sat.hauteur() >= _conditions.hauteur) {

                double jj0 = date.jourJulienUTC() - PAS0;
                double jj2 = jj0 + TEMPS1;

                double ang;
                double ang0 = PI;

                Date date2;
                QList<QList<ResultatPrevisions> > resultatSat;

                do {
                    const Date date0(jj0, 0., false);

                    _conditions.observateur.CalculPosVit(date0);

                    // Position de l'ISS
                    sat.CalculPosVit(date0);
                    sat.CalculCoordHoriz(_conditions.observateur, false);

                    // Position du corps (Soleil ou Lune)
                    if (typeCorps == CORPS_SOLEIL) {
                        soleil.CalculPosition(date0);
                        corps.setPosition(soleil.position());
                    }

                    if (typeCorps == CORPS_LUNE) {
                        lune.CalculPosition(date0);
                        corps.setPosition(lune.position());
                    }

                    corps.CalculCoordHoriz(_conditions.observateur, false);

                    // Calcul de l'angle ISS - observateur -  Corps
                    ang = corps.dist().Angle(sat.dist());
                    if (ang < ang0) {
                        ang0 = ang;
                    }

                    jj0 += PAS1;
                } while ((jj0 <= jj2) && (ang < ang0 + EPSDBL100));

                // Il y a une conjonction ou un transit : on determine l'angle de separation minimum
                if ((jj0 <= jj2 - PAS1) && (ang0 < _conditions.seuilConjonction + DEG2RAD) && (sat.hauteur() >= 0.)) {

                    QList<double> jjm;
                    QPair<double, double> minmax;

                    // Recherche de l'instant precis de l'angle minimum par interpolation
                    jj0 -= 2. * PAS1;
                    jjm.append(jj0 - PAS1);
                    jjm.append(jj0);
                    jjm.append(jj0 + PAS1);

                    minmax = CalculAngleMin(jjm, typeCorps, sat);

                    int it = 0;
                    double pasInt = PAS_INT0;
                    while ((fabs(ang - minmax.second) > 1.e-5) && (it < 10)) {

                        ang = minmax.second;
                        jjm[0] = minmax.first - pasInt;
                        jjm[1] = minmax.first;
                        jjm[2] = minmax.first + pasInt;

                        minmax = CalculAngleMin(jjm, typeCorps, sat);
                        pasInt *= 0.5;
                        it++;
                    }

                    date2 = Date(minmax.first, 0., false);

                    _conditions.observateur.CalculPosVit(date2);

                    // Position de l'ISS
                    sat.CalculPosVit(date2);
                    sat.CalculCoordHoriz(_conditions.observateur, false);

                    if ((sat.hauteur() >= _conditions.hauteur) && (minmax.second <= _conditions.seuilConjonction)) {

                        QList<Date> dates;
                        for(int i=0; i<5; i++) {
                            dates.append(Date());
                        }

                        // Position du corps (Soleil ou Lune)
                        soleil.CalculPosition(date2);
                        soleil.CalculCoordHoriz(_conditions.observateur, false);

                        double rayon;
                        if (typeCorps == CORPS_SOLEIL) {
                            corps.setPosition(soleil.position());
                            rayon = RAYON_SOLAIRE;
                        }

                        if (typeCorps == CORPS_LUNE) {
                            lune.CalculPosition(date2);
                            corps.setPosition(lune.position());
                            rayon = RAYON_LUNAIRE;
                        }

                        corps.CalculCoordHoriz(_conditions.observateur, false);

                        // Angle de separation
                        ang = corps.dist().Angle(sat.dist());

                        // Rayon apparent du corps
                        const double rayonApparent = asin(rayon / corps.distance());

                        const bool itr = (ang < rayonApparent);
                        const bool iconj = (ang <= _conditions.seuilConjonction);
                        const bool ilu = (typeCorps == CORPS_LUNE) && (itr || iconj) &&
                                (_conditions.calcTransitLunaireJour || (soleil.hauteur() < 0.));

                        ConditionEclipse condEcl;
                        condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

                        if ((itr && (typeCorps == CORPS_SOLEIL)) || ilu) {

                            // Calcul des dates extremes de la conjonction ou du transit
                            dates[2] = date2;
                            dates = CalculElements(minmax.first, typeCorps, itr, sat);

                            // Recalcul de la position pour chacune des dates
                            QList<ResultatPrevisions> result;
                            for(int j=0; j<5; j++) {

                                ResultatPrevisions res;

                                _conditions.observateur.CalculPosVit(dates[j]);

                                // Position de l'ISS
                                sat.CalculPosVit(dates[j]);
                                sat.CalculCoordHoriz(_conditions.observateur);
                                sat.CalculCoordEquat(_conditions.observateur);

                                // Altitude et distance du satellite
                                sat.CalculLatitude(sat.position());
                                res.altitude = sat.CalculAltitude(sat.position());
                                res.distance = sat.distance();

                                // Posiition du Soleil
                                soleil.CalculPosition(dates[j]);
                                soleil.CalculCoordHoriz(_conditions.observateur);

                                // Position de la Lune
                                lune.CalculPosition(dates[j]);
                                condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

                                // Date calendaire (UTC)
                                res.date = Date(dates[j].jourJulienUTC(), 0.);

                                // Coordonnees topocentriques du satellite
                                res.azimut = sat.azimut();
                                res.hauteur = sat.hauteur();

                                // Coordonnees equatoriales du satellite
                                res.ascensionDroite = sat.ascensionDroite();
                                res.declinaison = sat.declinaison();
                                res.constellation = sat.constellation();

                                // Distance angulaire
                                corps.setPosition((typeCorps == CORPS_SOLEIL) ? soleil.position() : lune.position());
                                corps.CalculCoordHoriz(_conditions.observateur);
                                res.angle = corps.dist().Angle(sat.dist());

                                // Coordonnees topocentriques du Soleil
                                res.azimutSoleil = soleil.azimut();
                                res.hauteurSoleil = soleil.hauteur();

                                // Informations sur le transit ou la conjonction
                                res.transit = itr;
                                res.typeCorps = typeCorps;
                                res.eclipse = condEcl.eclipseTotale();
                                res.penombre = (condEcl.eclipseAnnulaire() || condEcl.eclipsePartielle());

                                // Recherche du maximum
                                const Vecteur3D direction = corps.dist() - sat.dist();
                                Observateur obsmax = Observateur::CalculIntersectionEllipsoide(dates[j], sat.position(), direction);

                                if (!obsmax.nomlieu().isEmpty()) {

                                    obsmax.CalculPosVit(dates[j]);
                                    sat.CalculCoordHoriz(obsmax, false);

                                    if (typeCorps == CORPS_SOLEIL) {
                                        soleil.CalculPosition(dates[j]);
                                        corps.setPosition(soleil.position());
                                        rayon = RAYON_SOLAIRE;
                                    }

                                    if (typeCorps == CORPS_LUNE) {
                                        lune.CalculPosition(dates[j]);
                                        corps.setPosition(lune.position());
                                        rayon = RAYON_LUNAIRE;
                                    }

                                    corps.CalculCoordHoriz(obsmax, false);

                                    res.obsmax = obsmax;
                                    res.distanceObs = _conditions.observateur.CalculDistance(obsmax);
                                    res.cap = _conditions.observateur.CalculCap(obsmax).first;
                                }
                                result.append(res);
                            }

                            if (!result.isEmpty()) {
                                resultatSat.append(result);
                            }
                        }
                    }

                    date = Date(jj2, 0., false);
                } else {
                    if (sat.hauteur() < _conditions.hauteur) {
                        date = Date(date.jourJulienUTC() + periode, 0., false);
                    }
                }
                date = Date(date.jourJulienUTC() + PAS0, 0., false);

                // Recherche de la nouvelle date dans le tableau d'ephemerides
                bool atrouve = false;
                while (it2.hasNext() && !atrouve) {
                    const double jj = it2.next().jourJulienUTC;
                    if (jj >= date.jourJulienUTC()) {
                        atrouve = true;
                        it2.previous();
                    }
                }

                if (!resultatSat.isEmpty()) {
                    _resultats.insert(date2.ToShortDateAMJ(FORMAT_LONG, SYSTEME_24H), resultatSat);
                }
            }
        }
    }

    _donnees.tempsEcoule = tps.elapsed();

    /* Retour */
    return nombre;
}


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
 * Calcul de l'angle minimum du panneau
 */
QPair<double, double> TransitsIss::CalculAngleMin(const QList<double> jjm, const CorpsTransit &typeCorps, Satellite &satellite)
{
    /* Declarations des variables locales */
    Corps corps;
    Soleil soleil;
    Lune lune;
    QList<double> ang;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        _conditions.observateur.CalculPosVit(date);

        // Position de l'ISS
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(_conditions.observateur, false);

        // Position de la Lune ou du Soleil
        if (typeCorps == CORPS_SOLEIL) {
            soleil.CalculPosition(date);
            corps.setPosition(soleil.position());
        }

        if (typeCorps == CORPS_LUNE) {
            lune.CalculPosition(date);
            corps.setPosition(lune.position());
        }

        corps.CalculCoordHoriz(_conditions.observateur, false);

        // Angle de reflexion
        ang.append(corps.dist().Angle(satellite.dist()));
    }

    /* Retour */
    return Maths::CalculExtremumInterpolation3(jjm, ang);
}

/*
 * Calcul de la date ou la distance angulaire est minimale
 */
double TransitsIss::CalculDate(const QList<double> jjm, const CorpsTransit &typeCorps, const bool itransit, Satellite &satellite)
{
    /* Declarations des variables locales */
    double dist;
    Corps corps;
    Soleil soleil;
    Lune lune;
    QList<double> angle;

    /* Initialisations */

    /* Corps de la methode */
    for(int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        _conditions.observateur.CalculPosVit(date);

        // Position de l'ISS
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(_conditions.observateur, false);

        // Position du corps
        if (typeCorps == CORPS_SOLEIL) {
            soleil.CalculPosition(date);
            corps.setPosition(soleil.position());
        }

        if (typeCorps == CORPS_LUNE) {
            lune.CalculPosition(date);
            corps.setPosition(lune.position());
        }

        corps.CalculCoordHoriz(_conditions.observateur, false);

        angle.append(corps.dist().Angle(satellite.dist()));
    }

    if (itransit) {

        const double rayon = (typeCorps == CORPS_SOLEIL) ? RAYON_SOLAIRE : RAYON_LUNAIRE;
        dist = asin(rayon / corps.distance());

    } else {
        dist = _conditions.seuilConjonction;
    }

    /* Retour */
    return Maths::CalculValeurXInterpolation3(jjm, angle, dist, EPS_DATES);
}

/*
 * Calcul des dates caracteristiques de la conjonction ou du transit
 */
QList<Date> TransitsIss::CalculElements(const double jmax, const CorpsTransit &typeCorps, const bool itransit, Satellite &satellite)
{
    /* Declarations des variables locales */
    QList<double> jjm;

    /* Initialisations */

    /* Corps de la methode */
    // Date de debut
    jjm.append(jmax - PAS1);
    jjm.append(0.5 * (jjm[0] + jmax));
    jjm.append(jmax);

    double dateInf = CalculDate(jjm, typeCorps, itransit, satellite);

    // Iterations supplementaires pour affiner la date
    int it = 0;
    double tmp = 0.;
    while ((fabs(dateInf - tmp) > EPS_DATES) && (it < 6)) {

        tmp = dateInf;
        jjm[1] = dateInf;
        jjm[2] = jmax;
        jjm[0] = 2. * jjm[1] - jjm[2];

        dateInf = CalculDate(jjm, typeCorps, itransit, satellite);
        it++;
    }

    // Premiere date pour le trace sur la map
    double date1 = 0.2 * (4. * jmax + dateInf);

    // Date de fin
    jjm[0] = jmax;
    jjm[2] = jmax + PAS1;
    jjm[1] = 0.5 * (jjm[0] + jjm[2]);

    double dateSup = CalculDate(jjm, typeCorps, itransit, satellite);

    // Iterations supplementaires pour affiner la date
    it = 0;
    tmp = 0.;
    while ((fabs(dateSup - tmp) > EPS_DATES) && (it < 6)) {

        tmp = dateSup;
        jjm[1] = dateSup;
        jjm[0] = jmax;
        jjm[2] = 2. * jjm[1] - jjm[0];

        dateSup = CalculDate(jjm, typeCorps, itransit, satellite);
        it++;
    }

    // Deuxieme date pour le trace sur la map
    double date2 = 0.2 * (4. * jmax + dateSup);

    if ((fabs(date2 - date1) * NB_SEC_PAR_JOUR) < 2.) {
        date1 = dateInf;
        date2 = dateSup;
    }

    QList<Date> dates;
    dates.append(Date(date1, 0., false));
    dates.append(Date(dateInf, 0., false));
    dates.append(Date(jmax, 0., false));
    dates.append(Date(dateSup, 0., false));
    dates.append(Date(date2, 0., false));

    /* Retour */
    return dates;
}

/*
 * Calcul des ephemerides du Soleil et de la Lune
 */
QMap<CorpsTransit, QList<Ephemerides> > TransitsIss::CalculEphemSoleilLune()
{
    /* Declarations des variables locales */
    Date date;
    Soleil soleil;
    Lune lune;
    QList<Ephemerides> tabEphem;
    QMap<CorpsTransit, QList<Ephemerides> > res;

    /* Initialisations */

    /* Corps de la methode */
    if (_conditions.calcEphemSoleil) {

        date = Date(_conditions.jj1, 0., false);
        do {

            // Position ECI de l'observateur
            _conditions.observateur.CalculPosVit(date);

            // Position du Soleil
            soleil.CalculPosition(date);
            soleil.CalculCoordHoriz(_conditions.observateur, false);

            if (soleil.hauteur() >= _conditions.hauteur) {

                Ephemerides eph;
                eph.jourJulienUTC = date.jourJulienUTC();

                eph.positionObservateur = _conditions.observateur.position();
                eph.rotHz = _conditions.observateur.rotHz();
                eph.positionSoleil = soleil.position();
                eph.corps = CORPS_SOLEIL;

                tabEphem.append(eph);
            }

            date = Date(date.jourJulienUTC() + PAS0, 0., false);
        } while (date.jourJulienUTC() <= _conditions.jj2);

        res.insert(CORPS_SOLEIL, tabEphem);
        tabEphem.clear();
    }

    if (_conditions.calcEphemLune) {

        date = Date(_conditions.jj1, 0., false);
        do {

            // Position ECI de l'observateur
            _conditions.observateur.CalculPosVit(date);

            // Position de la Lune
            lune.CalculPosition(date);
            lune.CalculCoordHoriz(_conditions.observateur, false);

            if (lune.hauteur() >= _conditions.hauteur) {

                Ephemerides eph;
                eph.jourJulienUTC = date.jourJulienUTC();

                eph.positionObservateur = _conditions.observateur.position();
                eph.rotHz = _conditions.observateur.rotHz();
                eph.positionLune = lune.position();
                eph.corps = CORPS_LUNE;

                tabEphem.append(eph);
            }

            date = Date(date.jourJulienUTC() + PAS0, 0., false);
        } while (date.jourJulienUTC() <= _conditions.jj2);

        res.insert(CORPS_LUNE, tabEphem);
    }

    /* Retour */
    return res;
}