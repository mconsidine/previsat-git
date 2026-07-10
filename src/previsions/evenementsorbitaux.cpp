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
 * >    25 fevrier 2023
 *
 */

#include <QElapsedTimer>
#include <QFileInfo>
#include <QObject>
#include "configuration/configuration.h"
#include "evenementsorbitaux.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
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
    double tlemin = -DATE::DATE_INFINIE;
    double tlemax = DATE::DATE_INFINIE;
    QMap<QString, ElementsOrbitaux> tabElem = _conditions.tabElem;

    tps.start();
    _donnees.ageElementsOrbitaux.clear();
    _resultats.clear();

    // Creation du tableau de satellites
    QMapIterator it1(tabElem);
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

    // Calcul des ephemerides
    const QMap<QString, QList<EphemeridesEvenements> > tabEphem = CalculEphemerides(sats);

    /* Corps de la methode */
    bool passNoeuds;
    bool passOmbre;
    bool passPso;
    bool transitionsJourNuit;
    bool tmpOmbre;
    int k;
    int l;
    int m;
    int n;
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
        passPso = false;
        transitionsJourNuit = false;

        k = 0;
        l = 0;
        m = 0;
        n = 0;

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
            if (_conditions.passageNoeuds) {

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
            if (_conditions.passageOmbre) {

                l = i;
                tmpOmbre = false;
                const double eclipse1 = eph1.phiTerre - eph1.phiSoleil - eph1.elongationSoleil;
                const double eclipse3 = eph3.phiTerre - eph3.phiSoleil - eph3.elongationSoleil;

                if (((eclipse1 * eclipse3) < 0.) && !passOmbre) {

                    // Il y a un passage ombre->lumiere ou lumiere->ombre : calcul par interpolation de la date
                    l = i;
                    tmpOmbre = true;
                    const double eclipse2 = eph2.phiTerre - eph2.phiSoleil - eph2.elongationSoleil;

                    evt[0] = eclipse1;
                    evt[1] = eclipse2;
                    evt[2] = eclipse3;

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (evt[2] >= 0.) ? QObject::tr("Pénombre -> Ombre") : QObject::tr("Ombre -> Pénombre");

                    result.append(res);
                }

                const double penombre1 = eph1.phiTerre + eph1.phiSoleil - eph1.elongationSoleil;
                const double penombre3 = eph3.phiTerre + eph3.phiSoleil - eph3.elongationSoleil;

                if (((penombre1 * penombre3) < 0.) && !passOmbre) {

                    tmpOmbre = true;
                    const double penombre2 = eph2.phiTerre + eph2.phiSoleil - eph2.elongationSoleil;

                    // Il y a un passage lumiere->penombre ou penombre->lumiere : calcul par interpolation de la date
                    evt[0] = penombre1;
                    evt[1] = penombre2;
                    evt[2] = penombre3;

                    res = CalculEvt(jjm, evt, 0., sat);
                    res.typeEvenement = (evt[2] >= 0.) ? QObject::tr("Lumière -> Pénombre") : QObject::tr("Pénombre -> Lumière");

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

                        evt[0] = eclipseLune1;
                        evt[1] = eclipseLune2;
                        evt[2] = eclipseLune3;

                        res = CalculEvt(jjm, evt, 0., sat);
                        res.typeEvenement = (evt[2] >= 0.) ? QObject::tr("Pénombre -> Ombre (Lune)") : QObject::tr("Ombre -> Pénombre (Lune)");

                        result.append(res);
                    }

                    const double penombreLune1 = eph1.phiLune + eph1.phiSoleil - eph1.elongationLune;
                    const double penombreLune3 = eph3.phiLune + eph3.phiSoleil - eph3.elongationLune;

                    if (((penombreLune1 * penombreLune3) < 0.) && !passOmbre && (eph2.luminositeEclipseLune < eph2.luminositeEclipseSoleil)) {

                        // Il y a un passage penombre->lumiere ou lumiere->penombre : calcul par interpolation de la date
                        tmpOmbre = true;
                        const double penombreLune2 = eph2.phiLune + eph2.phiSoleil - eph2.elongationLune;

                        evt[0] = penombreLune1;
                        evt[1] = penombreLune2;
                        evt[2] = penombreLune3;

                        res = CalculEvt(jjm, evt, 0., sat);
                        res.typeEvenement = (evt[2] >= 0.) ? QObject::tr("Lumière -> Pénombre (Lune)") : QObject::tr("Pénombre -> Lumière (Lune)");

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
            if (_conditions.transitionsJourNuit) {

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

            // Calcul des passages au quadrangles
            if (_conditions.passagePso) {

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
            passPso = !(n == i);
            transitionsJourNuit = !(m == i);

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
