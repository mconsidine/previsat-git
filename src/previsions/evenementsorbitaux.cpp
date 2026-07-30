/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    evenementsorbitaux.cpp
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
 * >    30 juillet 2026
 *
 */

#include <QElapsedTimer>
#include <QFileInfo>
#include <QObject>
#include "configuration/configuration.h"
#include "evenementsorbitaux.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/maths/maths.h"


static ConditionsPrevisions _conditions;
static QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
static DonneesPrevisions _donnees;

struct EphemeridesEvenements
{
    double jourJulienUTC;

    // Pour les passages aux noeuds
    double posZ;

    // Pour les passages ombre/penombre/lumiere
    bool eclipseSoleil;
    bool penombreSoleil;
    bool eclipseLune;
    bool penombreLune;

    // Pour les passages apogee/perigee
    double rayon;

    // Pour les transitions jour/nuit
    double transition;

    // Pour les passages aux quadrangles
    double pso;
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
QMap<QString, QList<QList<ResultatPrevisions> > > &EvenementsOrbitaux::resultats()
{
    return _resultats;
}

DonneesPrevisions &EvenementsOrbitaux::donnees()
{
    return _donnees;
}


/*
 * Modificateurs
 */
void EvenementsOrbitaux::setConditions(const ConditionsPrevisions &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des evenements orbitaux
 */
int EvenementsOrbitaux::CalculEvenements(const int &nombre)
{
    /* Declarations des variables locales */
    QElapsedTimer tps;
    QList<Satellite> sats;

    /* Initialisations */
    double elemMin = -DATE::DATE_INFINIE;
    double elemMax = DATE::DATE_INFINIE;
    QMap<QString, ElementsOrbitaux> tabElem = _conditions.tabElem;

    tps.start();
    _donnees.ageElementsOrbitaux.clear();
    _resultats.clear();

    // Creation du tableau de satellites
    QMapIterator it1(tabElem);
    while (it1.hasNext()) {
        it1.next();

        if ((it1.key().toInt() == Configuration::instance()->noradStationSpatiale().toInt()) && !_conditions.listeElemIss.isEmpty()) {

            sats.append(Satellite(_conditions.listeElemIss));

            elemMin = _conditions.listeElemIss.first().epoque.jourJulienUTC();
            elemMax = _conditions.listeElemIss.last().epoque.jourJulienUTC();

        } else {

            const ElementsOrbitaux elem = it1.value();
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

    // Calcul des ephemerides
    const QMap<QString, QList<EphemeridesEvenements> > tabEphem = CalculEphemerides(sats);

    /* Corps de la methode */
    bool passNoeuds;
    bool passOmbre;
    bool passPenombre;
    bool passPso;
    bool transitionsJourNuit;
    int k;
    int l;
    int m;
    int n;
    int q;
    int i;
    int dim;
    double rayonVecteur;
    double altitude;
    Satellite sat;
    std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;
    std::array<QPointF, MATHS::DEGRE_INTERPOLATION> eve;
    std::array<double, MATHS::DEGRE_INTERPOLATION> evt;
    ResultatPrevisions res;
    QList<ResultatPrevisions> result;
    QList<QList<ResultatPrevisions> > resultatSat;

    // Boucle sur les satellites
    int j = 0;
    QMapIterator it2(tabEphem);
    while (it2.hasNext()) {
        it2.next();

        passNoeuds = false;
        passOmbre = false;
        passPenombre = false;
        passPso = false;
        transitionsJourNuit = false;

        k = 0;
        l = 0;
        m = 0;
        n = 0;
        q = 0;

        sat = sats.at(j);
        resultatSat.clear();
        result.clear();

        // Boucle sur les ephemerides
        i = 0;
        dim = static_cast<int> (it2.value().size() - 2);
        QListIterator it3(it2.value());
        do {

            const EphemeridesEvenements eph1 = it3.next();
            const EphemeridesEvenements eph2 = it3.next();
            const EphemeridesEvenements eph3 = it3.next();

            jjm[0] = eph1.jourJulienUTC;
            jjm[1] = eph2.jourJulienUTC;
            jjm[2] = eph3.jourJulienUTC;

            // Recherche des passages aux noeuds
            if (_conditions.passageNoeuds && it3.hasNext()) {

                k = i;
                if (((eph1.posZ * eph3.posZ) < 0.) && !passNoeuds) {

                    // Il y a un passage a un noeud : calcul de la date par interpolation
                    passNoeuds = true;

                    evt[0] = eph1.posZ;
                    evt[1] = eph2.posZ;
                    evt[2] = eph3.posZ;

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (eph3.posZ >= 0.) ?
                                            QObject::tr("Noeud Ascendant - PSO = 0°", "In orbit position") :
                                            QObject::tr("Noeud Descendant - PSO = 180°", "In orbit position");

                    result.append(res);
                    k++;
                }
            }

            // Recherche des passages ombre/penombre/lumiere
            if (_conditions.passageOmbre && it3.hasNext()) {

                std::array<bool, 2> ecl;
                l = i;
                q = i;

                if ((eph1.eclipseSoleil != eph2.eclipseSoleil) && !passOmbre) {

                    // Il y a un passage ombre->penombre ou penombre->ombre
                    passOmbre = true;

                    bool eclipse;
                    double tdn = eph2.jourJulienUTC;
                    double t_ecl = eph1.jourJulienUTC;

                    Soleil soleil;
                    ConditionEclipse conditionEclipse;

                    bool atrouve = false;
                    while (!atrouve) {

                        jjm[0] = t_ecl;
                        jjm[1] = t_ecl + DATE::NB_JOUR_PAR_SEC;

                        for(unsigned int p=0; p<2; p++) {

                            const Date date(jjm[p], 0., false);

                            // Position du satellite
                            sat.CalculPosVit(date);

                            // Position du Soleil
                            soleil.CalculPositionSimp(date);

                            // Conditions d'eclipse du satellite
                            conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil);

                            ecl[p] = (conditionEclipse.eclipseSoleil().type == TypeEclipse::ECLIPSE_TOTALE);
                        }

                        if (ecl[0] == ecl[1]) {
                            tdn = jjm[1];
                        }

                        if ((fabs(tdn - t_ecl) < DATE::EPS_DATES) || (ecl[0] != ecl[1])) {
                            atrouve = true;
                            tdn = t_ecl;
                            eclipse = ecl[1];
                        }

                        t_ecl = tdn;
                    }

                    if (atrouve) {

                        // Nom du satellite
                        res.nom = sat.elementsOrbitaux().nom;

                        // Date de l'evenement
                        res.date = Date(t_ecl, 0.);

                        // Calcul de la position du satellite
                        sat.CalculPosVit(res.date);
                        sat.CalculCoordTerrestres(res.date);

                        // Calcul de la PSO
                        sat.CalculElementsOsculateurs(res.date);
                        res.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

                        // Longitude, latitude
                        res.longitude = sat.longitude();
                        res.latitude = sat.latitude();

                        res.typeEvenement = (eclipse) ? QObject::tr("Pénombre -> Ombre") : QObject::tr("Ombre -> Pénombre");

                        result.append(res);
                        l++;
                    }
                }

                if ((eph1.penombreSoleil != eph2.penombreSoleil) && !passPenombre) {

                    // Il y a un passage lumiere->penombre ou penombre->lumiere
                    passPenombre = true;

                    bool penombre;
                    double tdn = eph2.jourJulienUTC;
                    double t_ecl = eph1.jourJulienUTC;

                    Soleil soleil;
                    ConditionEclipse conditionEclipse;

                    bool atrouve = false;
                    while (!atrouve) {

                        jjm[0] = t_ecl;
                        jjm[1] = t_ecl + DATE::NB_JOUR_PAR_SEC;

                        for(unsigned int p=0; p<2; p++) {

                            const Date date(jjm[p], 0., false);

                            // Position du satellite
                            sat.CalculPosVit(date);

                            // Position du Soleil
                            soleil.CalculPositionSimp(date);

                            // Conditions d'eclipse du satellite
                            conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil);

                            ecl[p] = (conditionEclipse.eclipseSoleil().type == TypeEclipse::NON_ECLIPSE);
                        }

                        if (ecl[0] == ecl[1]) {
                            tdn = jjm[1];
                        }

                        if ((fabs(tdn - t_ecl) < DATE::EPS_DATES) || (ecl[0] != ecl[1])) {
                            atrouve = true;
                            penombre = ecl[1];
                        }

                        t_ecl = tdn;
                    }

                    if (atrouve) {

                        // Nom du satellite
                        res.nom = sat.elementsOrbitaux().nom;

                        // Date de l'evenement
                        res.date = Date(t_ecl, 0.);

                        // Calcul de la position du satellite
                        sat.CalculPosVit(res.date);
                        sat.CalculCoordTerrestres(res.date);

                        // Calcul de la PSO
                        sat.CalculElementsOsculateurs(res.date);
                        res.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

                        // Longitude, latitude
                        res.longitude = sat.longitude();
                        res.latitude = sat.latitude();

                        res.typeEvenement = (penombre) ? QObject::tr("Pénombre -> Lumière") : QObject::tr("Lumière -> Pénombre");

                        result.append(res);
                        q++;
                    }
                }

                if (_conditions.calcEclipseLune) {

                    if ((eph1.eclipseLune != eph2.eclipseLune) && !passOmbre) {
#if (!COVERAGE_TEST)
                        // Il y a un passage ombre->penombre ou penombre->ombre par la Lune
                        passOmbre = true;

                        bool eclipse;
                        double tdn = eph2.jourJulienUTC;
                        double t_ecl = eph1.jourJulienUTC;

                        Soleil soleil;
                        Lune lune;
                        ConditionEclipse conditionEclipse;

                        bool atrouve = false;
                        while (!atrouve) {

                            jjm[0] = t_ecl;
                            jjm[1] = t_ecl + DATE::NB_JOUR_PAR_SEC;

                            for(unsigned int p=0; p<2; p++) {

                                const Date date(jjm[p], 0., false);

                                // Position du satellite
                                sat.CalculPosVit(date);

                                // Position du Soleil
                                soleil.CalculPositionSimp(date);

                                // Position de la Lune
                                lune.CalculPositionSimp(date);

                                // Conditions d'eclipse du satellite
                                conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, &lune);

                                ecl[p] = (conditionEclipse.eclipseLune().type == TypeEclipse::ECLIPSE_TOTALE);
                            }

                            if (ecl[0] == ecl[1]) {
                                tdn = jjm[1];
                            }

                            if ((fabs(tdn - t_ecl) < DATE::EPS_DATES) || (ecl[0] != ecl[1])) {
                                atrouve = true;
                                eclipse = ecl[1];
                            }

                            t_ecl = tdn;
                        }

                        if (atrouve) {

                            // Nom du satellite
                            res.nom = sat.elementsOrbitaux().nom;

                            // Date de l'evenement
                            res.date = Date(t_ecl, 0.);

                            // Calcul de la position du satellite
                            sat.CalculPosVit(res.date);
                            sat.CalculCoordTerrestres(res.date);

                            // Calcul de la PSO
                            sat.CalculElementsOsculateurs(res.date);
                            res.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

                            // Longitude, latitude
                            res.longitude = sat.longitude();
                            res.latitude = sat.latitude();

                            res.typeEvenement = (eclipse) ? QObject::tr("Ombre -> Pénombre (Lune)") : QObject::tr("Pénombre -> Ombre (Lune)");

                            result.append(res);
                            l++;
                        }
#endif
                    }

                    if ((eph1.penombreLune != eph2.penombreLune) && !passPenombre) {

                        // Il y a un passage lumiere->penombre ou penombre->lumiere par la Lune
                        passPenombre = true;

                        bool penombre;
                        double tdn = eph2.jourJulienUTC;
                        double t_ecl = eph1.jourJulienUTC;

                        Soleil soleil;
                        Lune lune;
                        ConditionEclipse conditionEclipse;

                        bool atrouve = false;
                        while (!atrouve) {

                            jjm[0] = t_ecl;
                            jjm[1] = t_ecl + DATE::NB_JOUR_PAR_SEC;

                            for(unsigned int p=0; p<2; p++) {

                                const Date date(jjm[p], 0., false);

                                // Position du satellite
                                sat.CalculPosVit(date);

                                // Position du Soleil
                                soleil.CalculPositionSimp(date);

                                // Position de la Lune
                                lune.CalculPositionSimp(date);

                                // Conditions d'eclipse du satellite
                                conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, &lune);

                                ecl[p] = (conditionEclipse.eclipseLune().type == TypeEclipse::NON_ECLIPSE);
                            }

                            if (ecl[0] == ecl[1]) {
                                tdn = jjm[1];
                            }

                            if ((fabs(tdn - t_ecl) < DATE::EPS_DATES) || (ecl[0] != ecl[1])) {
                                atrouve = true;
                                penombre = ecl[1];
                            }

                            t_ecl = tdn;
                        }

                        if (atrouve) {

                            // Nom du satellite
                            res.nom = sat.elementsOrbitaux().nom;

                            // Date de l'evenement
                            res.date = Date(t_ecl, 0.);

                            // Calcul de la position du satellite
                            sat.CalculPosVit(res.date);
                            sat.CalculCoordTerrestres(res.date);

                            // Calcul de la PSO
                            sat.CalculElementsOsculateurs(res.date);
                            res.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

                            // Longitude, latitude
                            res.longitude = sat.longitude();
                            res.latitude = sat.latitude();

                            res.typeEvenement = (penombre) ? QObject::tr("Pénombre -> Lumière (Lune)") : QObject::tr("Lumière -> Pénombre (Lune)");

                            result.append(res);
                            q++;
                        }
                    }
                }
            }

            // Calcul des passages apogee/perigee
            if (_conditions.passageApogee && it3.hasNext()) {

                if (((eph2.rayon > eph1.rayon) && (eph2.rayon > eph3.rayon)) || ((eph2.rayon < eph1.rayon) && (eph2.rayon < eph3.rayon))) {

                    // Il y a un passage au perigee ou a l'apogee : calcul par interpolation de la date
                    eve[0] = QPointF(jjm[0], eph1.rayon);
                    eve[1] = QPointF(jjm[1], eph2.rayon);
                    eve[2] = QPointF(jjm[2], eph3.rayon);

                    const QPointF minmax = Maths::CalculExtremumInterpolation3(eve);

                    res.nom = sat.elementsOrbitaux().nom;
                    res.date = Date(minmax.x(), 0.);

                    // Calcul de la position du satellite
                    sat.CalculPosVit(res.date);
                    sat.CalculCoordTerrestres(res.date);

                    // Calcul de la PSO
                    sat.CalculElementsOsculateurs(res.date);
                    res.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

                    // Longitude, latitude
                    res.longitude = sat.longitude();
                    res.latitude = sat.latitude();

                    rayonVecteur = minmax.y();
                    altitude = minmax.y() - TERRE::RAYON_TERRESTRE;
                    if (_conditions.unite == QObject::tr("nmi", "nautical mile")) {
                        rayonVecteur *= TERRE::MILE_PAR_KM;
                        altitude *= TERRE::MILE_PAR_KM;
                    }

                    const QString typeEvt = (eve.at(2).y() >= minmax.y()) ? QObject::tr("Périgée : %1%2 (%3%2)") : QObject::tr("Apogée : %1%2 (%3%2)");
                    res.typeEvenement = typeEvt.arg(rayonVecteur, 0, 'f', 1).arg(_conditions.unite).arg(altitude, 0, 'f', 1);

                    result.append(res);
                }
            }

            // Calcul des transitions jour/nuit
            if (_conditions.transitionsJourNuit && it3.hasNext()) {

                m = i;
                if (((eph1.transition * eph3.transition) < 0.) && !transitionsJourNuit) {

                    // Il y a une transition jour/nuit : calcul par interpolation de la date
                    transitionsJourNuit = true;

                    evt[0] = eph1.transition;
                    evt[1] = eph2.transition;
                    evt[2] = eph3.transition;

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (evt[2] <= 0.) ? QObject::tr("Passage terminateur Jour -> Nuit") :
                                            QObject::tr("Passage terminateur Nuit -> Jour");

                    result.append(res);
                    m++;
                }
            }

            // Calcul des passages aux quadrangles
            if (_conditions.passagePso && it3.hasNext()) {

                n = i;
                for(int p=1; p<4; p+=2) {

                    const double noeud = 90. * p * MATHS::DEG2RAD;
                    if (((eph1.pso - noeud) * (eph3.pso - noeud)) < 0. && (eph1.pso < noeud) && !passPso) {

                        // Il y a un passage aux quadrangles : calcul par interpolation de la date
                        passPso = true;

                        evt[0] = eph1.pso;
                        evt[1] = eph2.pso;
                        evt[2] = eph3.pso;

                        res = CalculEvt(jjm, evt, noeud, sat);
                        res.typeEvenement = QObject::tr("Passage à PSO = %1°", "In orbit position").arg(noeud * MATHS::RAD2DEG);

                        result.append(res);
                        n++;
                    }
                }
            }

            passNoeuds = !(k == i);
            passOmbre = !(l == i);
            passPenombre = !(q == i);
            passPso = !(n == i);
            transitionsJourNuit = !(m == i);

            if (!_conditions.passageOmbre) {
                it3.previous();
            }

            it3.previous();
            it3.previous();

            i++;
        } while (i < dim);

        if (!result.isEmpty()) {
            resultatSat.append(result);
            _resultats.insert(sat.elementsOrbitaux().nom + " " + sat.elementsOrbitaux().norad, resultatSat);
        }

        j++;
    }

    /* Retour */
    return nombre;
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
 * Calcul des ephemerides du satellite, du Soleil et de la Lune
 */
QMap<QString, QList<EphemeridesEvenements> > EvenementsOrbitaux::CalculEphemerides(const QList<Satellite> &satellites)
{
    /* Declarations des variables locales */
    Date date;
    Soleil soleil;
    Lune lune;
    Satellite sat;
    ConditionEclipse condEcl;
    EphemeridesEvenements eph;
    QList<EphemeridesEvenements> listeEphem;
    QMap<QString, QList<EphemeridesEvenements> > tabEphem;

    /* Initialisations */
    const double pas = DATE::NB_JOUR_PAR_MIN;

    /* Corps de la methode */
    QListIterator it(satellites);
    while (it.hasNext()) {

        sat = it.next();
        listeEphem.clear();

        date = Date(_conditions.jj1, 0., false);
        do {

            // Position du Soleil
            soleil.CalculPosVit(date);

            // Position de la Lune
            if (_conditions.calcEclipseLune) {
                lune.CalculPosVit(date);
            }

            // Position du satellite
            sat.CalculPosVit(date);

            condEcl.CalculSatelliteEclipse(sat.position(), soleil, &lune);
            sat.CalculElementsOsculateurs(date);

            // Sauvegarde des donnees
            eph.jourJulienUTC = date.jourJulienUTC();

            // Pour les passages aux noeuds
            eph.posZ = sat.position().z();

            // Pour les passages ombre/penombre/lumiere
            eph.eclipseSoleil = (condEcl.eclipseSoleil().type == TypeEclipse::ECLIPSE_TOTALE);
            eph.penombreSoleil = (condEcl.eclipseSoleil().type == TypeEclipse::NON_ECLIPSE);

            if (_conditions.calcEclipseLune) {
                eph.eclipseLune = (condEcl.eclipseLune().type == TypeEclipse::ECLIPSE_TOTALE);
                eph.penombreLune = (condEcl.eclipseLune().type == TypeEclipse::NON_ECLIPSE);
            }

            // Pour les passages apogee/perigee
            eph.rayon = sat.position().Norme();

            // Pour les transitions jour/nuit
            eph.transition = sat.position() * soleil.position();

            // Pour les passages aux quadrangles
            eph.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

            listeEphem.append(eph);

            date = Date(date.jourJulienUTC() + pas, 0., false);
        } while (date.jourJulienUTC() <= _conditions.jj2);

        tabEphem.insert(sat.elementsOrbitaux().norad, listeEphem);
    }

    /* Retour */
    return tabEphem;
}

/*
 * Calcul des elements de l'evenement orbital
 */
ResultatPrevisions EvenementsOrbitaux::CalculEvt(const std::array<double, MATHS::DEGRE_INTERPOLATION> &jjm,
                                                 const std::array<double, MATHS::DEGRE_INTERPOLATION> &evt,
                                                 const double yval, Satellite &sat)
{
    /* Declarations des variables locales */
    ResultatPrevisions res;
    std::array<QPointF, MATHS::DEGRE_INTERPOLATION> eve;

    /* Initialisations */
    for(unsigned int i=0; i<MATHS::DEGRE_INTERPOLATION; i++) {
        eve[i] = QPointF(jjm[i], evt[i]);
    }

    /* Corps de la methode */
    // Nom du satellite
    res.nom = sat.elementsOrbitaux().nom;

    // Calcul de la date par interpolation
    res.date = Date(Maths::CalculValeurXInterpolation3(eve, yval, DATE::EPS_DATES), 0.);

    // Calcul de la position du satellite
    sat.CalculPosVit(res.date);
    sat.CalculCoordTerrestres(res.date);

    // Calcul de la PSO
    sat.CalculElementsOsculateurs(res.date);
    res.pso = modulo(sat.elementsOsculateurs().anomalieVraie() + sat.elementsOsculateurs().argumentPerigee(), MATHS::DEUX_PI);

    // Longitude, latitude
    res.longitude = sat.longitude();
    res.latitude = sat.latitude();

    /* Retour */
    return res;
}
