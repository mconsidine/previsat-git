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
 * >    30 decembre 2018
 *
 */

#include <QElapsedTimer>
#include "configuration/configuration.h"
#include "evenementsorbitaux.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/maths/maths.h"


static ConditionsPrevisions _conditions;
static QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
static DonneesPrevisions _donnees;

struct Ephemerides
{
    double jourJulienUTC;

    // Pour les passages aux noeuds
    double posZ;

    // Pour les passages ombre/penombre/lumiere
    double elongationSoleil;
    double phiSoleil;
    double phiTerre;

    double elongationLune;
    double phiLune;

    double luminositeEclipseSoleil;
    double luminositeEclipseLune;

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

DonneesPrevisions EvenementsOrbitaux::donnees()
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
int EvenementsOrbitaux::CalculEvenements(int &nombre)
{
    /* Declarations des variables locales */
    QElapsedTimer tps;
    QList<Satellite> sats;

    /* Initialisations */
    double tlemin = -DATE_INFINIE;
    double tlemax = DATE_INFINIE;

    tps.start();
    _donnees.ageTle.clear();
    _resultats.clear();

    // Creation de la liste de TLE
    const QMap<QString, TLE> tabTle = TLE::LectureFichier(Configuration::instance()->dirLocalData(), _conditions.fichier, _conditions.listeSatellites);

    // Creation du tableau de satellites
    QMapIterator<QString, TLE> it1(tabTle);
    while (it1.hasNext()) {
        it1.next();

        const TLE tle = it1.value();
        sats.append(Satellite(tle));

        const double epok = tle.epoque().jourJulienUTC();
        if (epok > tlemin) {
            tlemin = epok;
        }

        if (epok < tlemax) {
            tlemax = epok;
        }
    }

    if (tabTle.keys().count() == 1) {
        _donnees.ageTle.append(fabs(_conditions.jj1 - tlemin));
    } else {

        const double age1 = fabs(_conditions.jj1 - tlemin);
        const double age2 = fabs(_conditions.jj1 - tlemax);
        _donnees.ageTle.append(qMin(age1, age2));
        _donnees.ageTle.append(qMax(age1, age2));
    }

    // Calcul des ephemerides
    const QMap<QString, QList<Ephemerides> > tabEphem = CalculEphemerides(sats);

    /* Corps de la methode */
    // Boucle sur les satellites
    int j = 0;
    QMapIterator<QString, QList<Ephemerides> > it2(tabEphem);
    while (it2.hasNext()) {
        it2.next();

        bool passNoeuds = false;
        bool passOmbre = false;
        bool passPso = false;
        bool transitionsJourNuit = false;

        int k = 0;
        int l = 0;
        int m = 0;
        int n = 0;

        Satellite sat = sats.at(j);
        QList<QList<ResultatPrevisions> > resultatSat;
        QList<ResultatPrevisions> result;

        // Boucle sur les ephemerides
        int i = 0;
        int dim = it2.value().size() - 2;
        QListIterator<Ephemerides> it3(it2.value());
        do {

            ResultatPrevisions res;

            const Ephemerides eph1 = it3.next();
            const Ephemerides eph2 = it3.next();
            const Ephemerides eph3 = it3.next();

            QList<double> jjm;
            jjm.append(eph1.jourJulienUTC);
            jjm.append(eph2.jourJulienUTC);
            jjm.append(eph3.jourJulienUTC);

            // Recherche des passages aux noeuds
            if (_conditions.passageNoeuds) {

                k = i;
                if (((eph1.posZ * eph3.posZ) < 0.) && !passNoeuds) {

                    // Il y a un passage a un noeud : calcul de la date par interpolation
                    passNoeuds = true;
                    QList<double> evt;

                    evt.append(eph1.posZ);
                    evt.append(eph2.posZ);
                    evt.append(eph3.posZ);

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (eph3.posZ >= 0.) ? QObject::tr("Noeud Ascendant - PSO = 0°") : QObject::tr("Noeud Descendant - PSO = 180°");

                    result.append(res);
                    k++;
                }
            }

            // Recherche des passages ombre/penombre/lumiere
            if (_conditions.passageOmbre) {

                l = i;
                bool tmpOmbre = false;
                const double eclipse1 = eph1.phiTerre - eph1.phiSoleil - eph1.elongationSoleil;
                const double eclipse3 = eph3.phiTerre - eph3.phiSoleil - eph3.elongationSoleil;

                if (((eclipse1 * eclipse3) < 0.) && !passOmbre) {

                    // Il y a un passage ombre->lumiere ou lumiere->ombre : calcul par interpolation de la date
                    l = i;
                    tmpOmbre = true;
                    const double eclipse2 = eph2.phiTerre - eph2.phiSoleil - eph2.elongationSoleil;

                    QList<double> evt;
                    evt.append(eclipse1);
                    evt.append(eclipse2);
                    evt.append(eclipse3);

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (evt.at(2) >= 0.) ? QObject::tr("Pénombre -> Ombre") : QObject::tr("Ombre -> Pénombre");

                    result.append(res);
                }

                const double penombre1 = eph1.phiTerre + eph1.phiSoleil - eph1.elongationSoleil;
                const double penombre3 = eph3.phiTerre + eph3.phiSoleil - eph3.elongationSoleil;

                if (((penombre1 * penombre3) < 0.) && !passOmbre) {

                    tmpOmbre = true;
                    const double penombre2 = eph2.phiTerre + eph2.phiSoleil - eph2.elongationSoleil;

                    // Il y a un passage lumiere->penombre ou penombre->lumiere : calcul par interpolation de la date
                    QList<double> evt;
                    evt.append(penombre1);
                    evt.append(penombre2);
                    evt.append(penombre3);

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (evt.at(2) >= 0.) ? QObject::tr("Lumière -> Pénombre") : QObject::tr("Pénombre -> Lumière");

                    result.append(res);
                }

                if (_conditions.calcEclipseLune) {

                    const double eclipseLune1 = eph1.phiLune - eph1.phiSoleil - eph1.elongationLune;
                    const double eclipseLune3 = eph3.phiLune - eph3.phiSoleil - eph3.elongationLune;

                    if (((eclipseLune1 * eclipseLune3) < 0.) && !passOmbre && (eph2.luminositeEclipseLune < eph2.luminositeEclipseSoleil)) {

                        // Il y a un passage ombre->lumiere ou lumiere->ombre : calcul par interpolation de la date
                        l = i;
                        tmpOmbre = true;
                        const double eclipseLune2 = eph2.phiLune - eph2.phiSoleil - eph2.elongationLune;

                        QList<double> evt;
                        evt.append(eclipseLune1);
                        evt.append(eclipseLune2);
                        evt.append(eclipseLune3);

                        res = CalculEvt(jjm, evt, 0., sat);
                        res.typeEvenement = (evt.at(2) >= 0.) ? QObject::tr("Pénombre -> Ombre (Lune)") : QObject::tr("Ombre -> Pénombre (Lune)");

                        result.append(res);
                    }

                    const double penombreLune1 = eph1.phiLune + eph1.phiSoleil - eph1.elongationLune;
                    const double penombreLune3 = eph3.phiLune + eph3.phiSoleil - eph3.elongationLune;

                    if (((penombreLune1 * penombreLune3) < 0.) && !passOmbre && (eph2.luminositeEclipseLune < eph2.luminositeEclipseSoleil)) {

                        // Il y a un passage penombre->lumiere ou lumiere->penombre : calcul par interpolation de la date
                        tmpOmbre = true;
                        const double penombreLune2 = eph2.phiLune + eph2.phiSoleil - eph2.elongationLune;

                        QList<double> evt;
                        evt.append(penombreLune1);
                        evt.append(penombreLune2);
                        evt.append(penombreLune3);

                        res = CalculEvt(jjm, evt, 0., sat);
                        res.typeEvenement = (evt.at(2) >= 0.) ? QObject::tr("Lumière -> Pénombre (Lune)") : QObject::tr("Pénombre -> Lumière (Lune)");

                        result.append(res);
                    }
                }

                if (tmpOmbre) {
                    passOmbre = true;
                    l++;
                }
            }

            // Calcul des passages apogee/perigee
            if (_conditions.passageApogee) {

                if (((eph2.rayon > eph1.rayon) && (eph2.rayon > eph3.rayon)) || ((eph2.rayon < eph1.rayon) && (eph2.rayon < eph3.rayon))) {

                    // Il y a un passage au perigee ou a l'apogee : calcul par interpolation de la date
                    QList<double> evt;
                    evt.append(eph1.rayon);
                    evt.append(eph2.rayon);
                    evt.append(eph3.rayon);

                    QPair<double, double> minmax = Maths::CalculExtremumInterpolation3(jjm, evt);

                    res.nom = sat.tle().nom();
                    res.date = Date(minmax.first, 0.);

                    // Calcul de la position du satellite
                    sat.CalculPosVit(res.date);
                    sat.CalculCoordTerrestres(res.date);

                    // Calcul de la PSO
                    sat.CalculElementsOsculateurs(res.date);
                    res.pso = modulo(sat.elements().anomalieVraie() + sat.elements().argumentPerigee(), DEUX_PI);

                    // Longitude, latitude
                    res.longitude = sat.longitude();
                    res.latitude = sat.latitude();

                    double rayonVecteur = minmax.second;
                    double altitude = minmax.second - RAYON_TERRESTRE;
                    if (_conditions.unite == QObject::tr("nmi")) {
                        rayonVecteur *= MILE_PAR_KM;
                        altitude *= MILE_PAR_KM;
                    }

                    const QString typeEvt = (evt.at(2) >= minmax.second) ? QObject::tr("Périgée : %1%2 (%3%2)") : QObject::tr("Apogée : %1%2 (%3%2)");
                    res.typeEvenement = typeEvt.arg(rayonVecteur, 0, 'f', 1).arg(_conditions.unite).arg(altitude, 0, 'f', 1);

                    result.append(res);
                }
            }

            // Calcul des transitions jour/nuit
            if (_conditions.transitionsJourNuit) {

                m = i;
                if (((eph1.transition * eph3.transition) < 0.) && !transitionsJourNuit) {

                    // Il y a une transition jour/nuit : calcul par interpolation de la date
                    transitionsJourNuit = true;

                    QList<double> evt;
                    evt.append(eph1.transition);
                    evt.append(eph2.transition);
                    evt.append(eph3.transition);

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (evt.at(2) <= 0.) ? QObject::tr("Passage terminateur Jour -> Nuit") :
                                                            QObject::tr("Passage terminateur Nuit -> Jour");

                    result.append(res);
                    m++;
                }
            }

            // Calcul des passages au quadrangles
            if (_conditions.passagePso) {

                n = i;
                for(int p=1; p<4; p+=2) {

                    const double noeud = 90. * p * DEG2RAD;
                    if (((eph1.pso - noeud) * (eph3.pso - noeud)) < 0. && (eph1.pso < noeud) && !passPso) {

                        // Il y a un passage aux quadrangles : calcul par interpolation de la date
                        passPso = true;

                        QList<double> evt;
                        evt.append(eph1.pso);
                        evt.append(eph2.pso);
                        evt.append(eph3.pso);

                        res = CalculEvt(jjm, evt, noeud, sat);
                        res.typeEvenement = QObject::tr("Passage à PSO = %1°").arg(noeud * RAD2DEG);

                        result.append(res);
                        n++;
                    }
                }
            }

            passNoeuds = !(k == i);
            passOmbre = !(l == i);
            passPso = !(n == i);
            transitionsJourNuit = !(m == i);

            it3.previous();
            it3.previous();
            i++;
        } while (i < dim);

        if (!result.isEmpty()) {
            resultatSat.append(result);
            _resultats.insert(sat.tle().nom() + " " + sat.tle().norad(), resultatSat);
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
QMap<QString, QList<Ephemerides> > EvenementsOrbitaux::CalculEphemerides(const QList<Satellite> &satellites)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;
    QMap<QString, QList<Ephemerides> > tabEphem;

    /* Initialisations */
    const double pas = NB_JOUR_PAR_MIN;

    /* Corps de la methode */
    QListIterator<Satellite> it(satellites);
    while (it.hasNext()) {

        Satellite sat = it.next();
        QList<Ephemerides> listeEphem;

        Date date(_conditions.jj1, 0., false);
        do {

            Ephemerides eph;

            // Position du Soleil
            soleil.CalculPosition(date);

            // Position de la Lune
            if (_conditions.calcEclipseLune) {
                lune.CalculPosition(date);
            }

            // Position du satellite
            sat.CalculPosVit(date);

            ConditionEclipse condEcl;
            condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune);
            sat.CalculElementsOsculateurs(date);

            // Sauvegarde des donnees
            eph.jourJulienUTC = date.jourJulienUTC();

            // Pour les passages aux noeuds
            eph.posZ = sat.position().z();

            // Pour les passages ombre/penombre/lumiere
            eph.elongationSoleil = condEcl.eclipseSoleil().elongation;
            eph.phiSoleil = condEcl.eclipseSoleil().phiSoleil;
            eph.phiTerre = condEcl.eclipseSoleil().phi;

            if (_conditions.calcEclipseLune) {
                eph.elongationLune = condEcl.eclipseLune().elongation;
                eph.phiLune = condEcl.eclipseLune().phi;
                eph.luminositeEclipseLune = condEcl.eclipseLune().luminosite;
                eph.luminositeEclipseSoleil = condEcl.eclipseSoleil().luminosite;
            }

            // Pour les passages apogee/perigee
            eph.rayon = sat.position().Norme();

            // Pour les transitions jour/nuit
            eph.transition = sat.position() * soleil.position();

            // Pour les passages aux quadrangles
            eph.pso = modulo(sat.elements().anomalieVraie() + sat.elements().argumentPerigee(), DEUX_PI);

            listeEphem.append(eph);

            date = Date(date.jourJulienUTC() + pas, 0., false);
        } while (date.jourJulienUTC() <= _conditions.jj2);

        tabEphem.insert(sat.tle().norad(), listeEphem);
    }

    /* Retour */
    return tabEphem;
}

/*
 * Calcul des elements de l'evenement orbital
 */
ResultatPrevisions EvenementsOrbitaux::CalculEvt(const QList<double> &jjm, const QList<double> &evt, const double yval, Satellite &sat)
{
    /* Declarations des variables locales */
    ResultatPrevisions res;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    res.nom = sat.tle().nom();

    // Calcul de la date par interpolation
    res.date = Date(Maths::CalculValeurXInterpolation3(jjm, evt, yval, EPS_DATES), 0.);

    // Calcul de la position du satellite
    sat.CalculPosVit(res.date);
    sat.CalculCoordTerrestres(res.date);

    // Calcul de la PSO
    sat.CalculElementsOsculateurs(res.date);
    res.pso = modulo(sat.elements().anomalieVraie() + sat.elements().argumentPerigee(), DEUX_PI);

    // Longitude, latitude
    res.longitude = sat.longitude();
    res.latitude = sat.latitude();

    /* Retour */
    return res;
}