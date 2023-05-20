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
 * >    transits.cpp
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
 * >    20 mai 2023
 *
 */

#include <QElapsedTimer>
#include "configuration/configuration.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/maths/maths.h"
#include "transits.h"


// Pas de calcul ou d'interpolation
static constexpr double PAS0 = DATE::NB_JOUR_PAR_MIN;
static constexpr double PAS1 = 10. * DATE::NB_JOUR_PAR_SEC;
static constexpr double PAS_INT0 = 10. * DATE::NB_JOUR_PAR_SEC;
static constexpr double TEMPS1 = 16. * DATE::NB_JOUR_PAR_MIN;

static ConditionsPrevisions _conditions;
static QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
static DonneesPrevisions _donnees;

struct EphemeridesTransits
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
QMap<QString, QList<QList<ResultatPrevisions> > > &Transits::resultats()
{
    return _resultats;
}

DonneesPrevisions Transits::donnees()
{
    return _donnees;
}


/*
 * Modificateurs
 */
void Transits::setConditions(const ConditionsPrevisions &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des transits devant la Lune ou le Soleil
 */
int Transits::CalculTransits(int &nombre)
{
    /* Declarations des variables locales */
    QElapsedTimer tps;
    Soleil soleil;
    Lune lune;
    QList<Satellite> sats;

    /* Initialisations */
    tps.start();
    _donnees.ageElementsOrbitaux.clear();
    _resultats.clear();
    double elemMin = -DATE::DATE_INFINIE;
    double elemMax = DATE::DATE_INFINIE;
    QMap<QString, ElementsOrbitaux> tabElem = _conditions.tabElem;

    // Creation du tableau de satellites
    QMapIterator it0(tabElem);
    while (it0.hasNext()) {
        it0.next();

        if ((it0.key().toInt() == Configuration::instance()->noradStationSpatiale().toInt()) && !_conditions.listeElemIss.isEmpty()) {

            sats.append(Satellite(_conditions.listeElemIss));

            elemMin = _conditions.listeElemIss.first().epoque.jourJulienUTC();
            elemMax = _conditions.listeElemIss.last().epoque.jourJulienUTC();

        } else {

            const ElementsOrbitaux elem = it0.value();
            sats.append(Satellite(elem));

            const double epok = elem.epoque.jourJulienUTC();
            if (epok > elemMin) {
                elemMin = epok;
            }

            if (epok < elemMax) {
                elemMax = epok;
            }
        }
    }

    if ((tabElem.keys().count() > 1) || !_conditions.listeElemIss.isEmpty()) {

        const double age1 = fabs(_conditions.jj1 - elemMin);
        const double age2 = fabs(_conditions.jj1 - elemMax);
        _donnees.ageElementsOrbitaux.append(qMin(age1, age2));
        _donnees.ageElementsOrbitaux.append(qMax(age1, age2));

    } else {
        _donnees.ageElementsOrbitaux.append(fabs(_conditions.jj1 - elemMin));
    }

    // Generation des ephemerides du Soleil et de la Lune
    const QMap<CorpsTransit, QList<EphemeridesTransits> > tabEphem = CalculEphemSoleilLune();

    /* Corps de la methode */
    bool atrouve;
    int it;
    double pasInt;
    double jj0;
    double jj2;
    double ang;
    double ang0;
    double rayon = 0.;
    Date date;
    Date date2;
    Date date3;
    Corps corps;
    Observateur obsmax;
    ConditionEclipse condEcl;
    Satellite sat;
    std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;
    QList<Date> dates;
    QPair<double, double> minmax;
    ResultatPrevisions res;
    QList<ResultatPrevisions> result;
    QList<QList<ResultatPrevisions> > resultatSat;

    // Boucle sur les satellites
    QListIterator it1(sats);
    while (it1.hasNext()) {

        resultatSat.clear();
        sat = it1.next();

        const double perigee = TERRE::RAYON_TERRESTRE * pow(TERRE::KE * DATE::NB_MIN_PAR_JOUR / (MATHS::DEUX_PI * sat.elementsOrbitaux().no),
                                                            MATHS::DEUX_TIERS) * (1. - sat.elementsOrbitaux().ecco);
        const double periode = DATE::NB_JOUR_PAR_MIN * (floor(TERRE::KE * pow(MATHS::DEUX_PI * perigee, MATHS::DEUX_TIERS)) - 16.);

        // Boucle sur le tableau d'ephemerides
        QMapIterator it2(tabEphem);
        while (it2.hasNext()) {
            it2.next();

            const CorpsTransit typeCorps = it2.key();

            QListIterator it3(it2.value());
            while (it3.hasNext()) {

                const EphemeridesTransits ephem = it3.next();

                // Date
                date = Date(ephem.jourJulienUTC, 0., false);

                // Lieu d'observation
                const Observateur obs(ephem.positionObservateur, Vecteur3D(), ephem.rotHz, _conditions.observateur.aaer(),
                                      _conditions.observateur.aray());

                corps.setPosition((typeCorps == CorpsTransit::CORPS_SOLEIL) ? ephem.positionSoleil : ephem.positionLune);

                // Position du satellite
                sat.CalculPosVit(date);
                sat.CalculCoordHoriz(obs, false);

                if (sat.hauteur() >= _conditions.hauteur) {

                    jj0 = date.jourJulienUTC() - PAS0;
                    jj2 = jj0 + TEMPS1;

                    ang = 0.;
                    ang0 = MATHS::PI;

                    resultatSat.clear();

                    do {
                        const Date date0(jj0, 0., false);

                        _conditions.observateur.CalculPosVit(date0);

                        // Position du satellite
                        sat.CalculPosVit(date0);
                        sat.CalculCoordHoriz(_conditions.observateur, false);

                        // Position du corps (Soleil ou Lune)
                        if (typeCorps == CorpsTransit::CORPS_SOLEIL) {
                            soleil.CalculPosition(date0);
                            corps.setPosition(soleil.position());
                        }

                        if (typeCorps == CorpsTransit::CORPS_LUNE) {
                            lune.CalculPosition(date0);
                            corps.setPosition(lune.position());
                        }

                        corps.CalculCoordHoriz(_conditions.observateur, false);

                        // Calcul de l'angle satellite - observateur - Corps
                        ang = corps.dist().Angle(sat.dist());
                        if (ang < ang0) {
                            ang0 = ang;
                        }

                        jj0 += PAS1;
                    } while ((jj0 <= jj2) && (ang < (ang0 + MATHS::EPSDBL100)));

                    // Il y a une conjonction ou un transit : on determine l'angle de separation minimum
                    if ((jj0 <= jj2 - PAS1) && (ang0 < _conditions.seuilConjonction + MATHS::DEG2RAD) && (sat.hauteur() >= 0.)) {

                        // Recherche de l'instant precis de l'angle minimum par interpolation
                        jj0 -= 2. * PAS1;

                        jjm[0] = jj0 - PAS1;
                        jjm[1] = jj0;
                        jjm[2] = jj0 + PAS1;

                        minmax = CalculAngleMin(jjm, typeCorps, sat);

                        it = 0;
                        pasInt = PAS_INT0;
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

                        // Position du satellite
                        sat.CalculPosVit(date2);
                        sat.CalculCoordHoriz(_conditions.observateur, false);

                        if ((sat.hauteur() >= _conditions.hauteur) && (minmax.second <= _conditions.seuilConjonction)) {

                            dates.clear();
                            for(int i=0; i<5; i++) {
                                dates.append(Date());
                            }

                            // Position du corps (Soleil ou Lune)
                            soleil.CalculPosition(date2);
                            soleil.CalculCoordHoriz(_conditions.observateur, false);

                            if (typeCorps == CorpsTransit::CORPS_SOLEIL) {
                                corps.setPosition(soleil.position());
                                rayon = SOLEIL::RAYON_SOLAIRE;
                            }

                            if (typeCorps == CorpsTransit::CORPS_LUNE) {
                                lune.CalculPosition(date2);
                                corps.setPosition(lune.position());
                                rayon = LUNE::RAYON_LUNAIRE;
                            }

                            corps.CalculCoordHoriz(_conditions.observateur, false);

                            // Angle de separation
                            ang = corps.dist().Angle(sat.dist());

                            // Rayon apparent du corps
                            const double rayonApparent = asin(rayon / corps.distance());

                            const bool itr = (ang < rayonApparent);
                            const bool iconj = (ang <= _conditions.seuilConjonction);
                            const bool ilu = (typeCorps == CorpsTransit::CORPS_LUNE) && (itr || iconj) &&
                                    (_conditions.calcTransitLunaireJour || (soleil.hauteur() < 0.));

                            condEcl.CalculSatelliteEclipse(sat.position(), soleil, &lune, _conditions.refraction);

                            if ((itr && (typeCorps == CorpsTransit::CORPS_SOLEIL)) || ilu) {

                                // Calcul des dates extremes de la conjonction ou du transit
                                dates[2] = date2;
                                dates = CalculElements(minmax.first, typeCorps, itr, sat);

                                // Recalcul de la position pour chacune des dates
                                result.clear();
                                for(int j=0; j<5; j++) {

                                    res.obsmax = Observateur();

                                    _conditions.observateur.CalculPosVit(dates[j]);

                                    // Elements orbitaux
                                    res.elements = sat.elementsOrbitaux();

                                    // Nom du satellite
                                    res.nom = sat.elementsOrbitaux().nom;

                                    // Position du satellite
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
                                    condEcl.CalculSatelliteEclipse(sat.position(), soleil, &lune, _conditions.refraction);

                                    // Date calendaire
                                    res.date = Date(dates[j].jourJulien() + DATE::EPS_DATES, 0.);
                                    if (j == 2) {
                                        date3 = res.date;
                                    }

                                    // Coordonnees topocentriques du satellite
                                    res.azimut = sat.azimut();
                                    res.hauteur = sat.hauteur();

                                    // Coordonnees equatoriales du satellite
                                    res.ascensionDroite = sat.ascensionDroite();
                                    res.declinaison = sat.declinaison();
                                    res.constellation = sat.constellation();

                                    // Distance angulaire
                                    corps.setPosition((typeCorps == CorpsTransit::CORPS_SOLEIL) ? soleil.position() : lune.position());
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
                                    if (j == 2) {
                                        res.duree = fabs(dates[3].jourJulienUTC() - dates[1].jourJulienUTC()) * DATE::NB_SEC_PAR_JOUR;
                                    } else {
                                        res.duree = 0.;
                                    }

                                    // Recherche du maximum
                                    const Vecteur3D direction = corps.dist() - sat.dist();
                                    obsmax = Observateur::CalculIntersectionEllipsoide(dates[j], sat.position(), direction);

                                    if (!obsmax.nomlieu().isEmpty()) {

                                        obsmax.CalculPosVit(dates[j]);
                                        sat.CalculCoordHoriz(obsmax, false);

                                        if (typeCorps == CorpsTransit::CORPS_SOLEIL) {
                                            soleil.CalculPosition(dates[j]);
                                            corps.setPosition(soleil.position());
                                            rayon = SOLEIL::RAYON_SOLAIRE;
                                        }

                                        if (typeCorps == CorpsTransit::CORPS_LUNE) {
                                            lune.CalculPosition(dates[j]);
                                            corps.setPosition(lune.position());
                                            rayon = LUNE::RAYON_LUNAIRE;
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
                    } else if (sat.hauteur() < _conditions.hauteur) {
                        date = Date(date.jourJulienUTC() + periode, 0., false);
                    }

                    date = Date(date.jourJulienUTC() + PAS0, 0., false);

                    // Recherche de la nouvelle date dans le tableau d'ephemerides
                    atrouve = false;
                    while (it3.hasNext() && !atrouve) {
                        const double jj = it3.next().jourJulienUTC;
                        if (jj >= date.jourJulienUTC()) {
                            atrouve = true;
                            it3.previous();
                        }
                    }

                    if (!resultatSat.isEmpty()) {
                        _resultats.insert(date3.ToShortDateAMJ(DateFormat::FORMAT_LONG, DateSysteme::SYSTEME_24H), resultatSat);
                    }
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
QPair<double, double> Transits::CalculAngleMin(const std::array<double, MATHS::DEGRE_INTERPOLATION> jjm, const CorpsTransit &typeCorps,
                                               Satellite &satellite)
{
    /* Declarations des variables locales */
    Corps corps;
    Soleil soleil;
    Lune lune;
    std::array<double, MATHS::DEGRE_INTERPOLATION> ang;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        _conditions.observateur.CalculPosVit(date);

        // Position de l'ISS
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(_conditions.observateur, false);

        // Position de la Lune ou du Soleil
        if (typeCorps == CorpsTransit::CORPS_SOLEIL) {
            soleil.CalculPosition(date);
            corps.setPosition(soleil.position());
        }

        if (typeCorps == CorpsTransit::CORPS_LUNE) {
            lune.CalculPosition(date);
            corps.setPosition(lune.position());
        }

        corps.CalculCoordHoriz(_conditions.observateur, false);

        // Angle de reflexion
        ang[i] = corps.dist().Angle(satellite.dist());
    }

    /* Retour */
    return Maths::CalculExtremumInterpolation3(jjm, ang);
}

/*
 * Calcul de la date ou la distance angulaire est minimale
 */
double Transits::CalculDate(const std::array<double, MATHS::DEGRE_INTERPOLATION> jjm, const CorpsTransit &typeCorps, const bool itransit,
                            Satellite &satellite)
{
    /* Declarations des variables locales */
    double dist;
    Corps corps;
    Soleil soleil;
    Lune lune;
    std::array<double, MATHS::DEGRE_INTERPOLATION> angle;

    /* Initialisations */

    /* Corps de la methode */
    for(int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        _conditions.observateur.CalculPosVit(date);

        // Position de l'ISS
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(_conditions.observateur, false);

        // Position du corps
        if (typeCorps == CorpsTransit::CORPS_SOLEIL) {
            soleil.CalculPosition(date);
            corps.setPosition(soleil.position());
        }

        if (typeCorps == CorpsTransit::CORPS_LUNE) {
            lune.CalculPosition(date);
            corps.setPosition(lune.position());
        }

        corps.CalculCoordHoriz(_conditions.observateur, false);

        angle[i] = corps.dist().Angle(satellite.dist());
    }

    if (itransit) {

        const double rayon = (typeCorps == CorpsTransit::CORPS_SOLEIL) ? SOLEIL::RAYON_SOLAIRE : LUNE::RAYON_LUNAIRE;
        dist = asin(rayon / corps.distance());

    } else {
        dist = _conditions.seuilConjonction;
    }

    /* Retour */
    return Maths::CalculValeurXInterpolation3(jjm, angle, dist, DATE::EPS_DATES);
}

/*
 * Calcul des dates caracteristiques de la conjonction ou du transit
 */
QList<Date> Transits::CalculElements(const double jmax, const CorpsTransit &typeCorps, const bool itransit, Satellite &satellite)
{
    /* Declarations des variables locales */
    std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;

    /* Initialisations */

    /* Corps de la methode */
    // Date de debut
    jjm[0] = jmax - PAS1;
    jjm[1] = 0.5 * (jjm[0] + jmax);
    jjm[2] = jmax;

    double dateInf = CalculDate(jjm, typeCorps, itransit, satellite);

    // Iterations supplementaires pour affiner la date
    int it = 0;
    double tmp = 0.;
    while ((fabs(dateInf - tmp) > DATE::EPS_DATES) && (it < 20)) {

        tmp = dateInf;
        jjm[1] = dateInf;
        jjm[2] = jmax;
        jjm[0] = 2. * jjm[1] - jjm[2];

        dateInf = CalculDate(jjm, typeCorps, itransit, satellite);
        it++;
    }

    // Premiere date pour le trace sur la map
    double date1 = dateInf - 0.8 * DATE::NB_JOUR_PAR_SEC;

    // Date de fin
    jjm[0] = jmax;
    jjm[2] = jmax + PAS1;
    jjm[1] = 0.5 * (jjm[0] + jjm[2]);

    double dateSup = CalculDate(jjm, typeCorps, itransit, satellite);

    // Iterations supplementaires pour affiner la date
    it = 0;
    tmp = 0.;
    while ((fabs(dateSup - tmp) > DATE::EPS_DATES) && (it < 20)) {

        tmp = dateSup;
        jjm[1] = dateSup;
        jjm[0] = jmax ;
        jjm[2] = 2. * jjm[1] - jjm[0];

        dateSup = CalculDate(jjm, typeCorps, itransit, satellite);
        it++;
    }

    // Deuxieme date pour le trace sur la map
    double date2 = dateSup + 0.8 * DATE::NB_JOUR_PAR_SEC;

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
QMap<CorpsTransit, QList<EphemeridesTransits> > Transits::CalculEphemSoleilLune()
{
    /* Declarations des variables locales */
    Date date;
    Soleil soleil;
    Lune lune;
    EphemeridesTransits eph;
    QList<EphemeridesTransits> tabEphem;
    QMap<CorpsTransit, QList<EphemeridesTransits> > res;

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

                eph.jourJulienUTC = date.jourJulienUTC();
                eph.positionObservateur = _conditions.observateur.position();
                eph.rotHz = _conditions.observateur.rotHz();
                eph.positionSoleil = soleil.position();
                eph.corps = CorpsTransit::CORPS_SOLEIL;

                tabEphem.append(eph);
            }

            date = Date(date.jourJulienUTC() + PAS0, 0., false);
        } while (date.jourJulienUTC() <= _conditions.jj2);

        res.insert(CorpsTransit::CORPS_SOLEIL, tabEphem);
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

                eph.jourJulienUTC = date.jourJulienUTC();
                eph.positionObservateur = _conditions.observateur.position();
                eph.rotHz = _conditions.observateur.rotHz();
                eph.positionLune = lune.position();
                eph.corps = CorpsTransit::CORPS_LUNE;

                tabEphem.append(eph);
            }

            date = Date(date.jourJulienUTC() + PAS0, 0., false);
        } while (date.jourJulienUTC() <= _conditions.jj2);

        res.insert(CorpsTransit::CORPS_LUNE, tabEphem);
    }

    /* Retour */
    return res;
}
