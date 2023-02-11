/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    evenements.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 mars 2020
 *
 * Date de revision
 * >    19 juin 2022
 *
 */

#include "evenements.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "satellite.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Accesseurs
 */

/*
 * Methodes publiques
 */
/*
 * Calcul de l'AOS (ou LOS) suivant ou precedent
 */
ElementsAOS Evenements::CalculAOS(const Date &dateInit, const Satellite &satellite, const Observateur &observateur, const bool sensCalcul,
                                  const double hauteurMin, const bool refraction)
{
    /* Declarations des variables locales */
    ElementsAOS elements;

    /* Initialisations */
    Satellite sat = satellite;
    Observateur obs = observateur;
    elements.aos = sat.hasAOS(obs);
    elements.azimut = 0.;
    const double st = (sensCalcul) ? 1. : -1.;

    /* Corps de la methode */
    if (elements.aos) {

        double periode = st * NB_JOUR_PAR_MIN;
        if (((sat.hauteur() - hauteurMin) * st) > EPSDBL100) {
            elements.typeAOS = QObject::tr("LOS", "Loss of signal");
        }

        double tAOS = 0.;
        double t_ht = dateInit.jourJulienUTC();

        QList<double> jjm;
        QList<double> ht;

        bool afin = false;
        int iter = 0;
        while (!afin) {

            jjm.clear();
            ht.clear();

            jjm.append(t_ht);
            jjm.append(jjm.at(0) + 0.5 * periode);
            jjm.append(jjm.at(0) + periode);

            for(int i=0; i<3; i++) {

                const Date date(jjm.at(i), 0., false);
                obs.CalculPosVit(date);
                sat.CalculPosVit(date);
                sat.CalculCoordHoriz(obs, false, refraction, true);
                ht.append(sat.hauteur() - hauteurMin);
            }

            const bool atst1 = (ht.at(0) * ht.at(1) < 0.);
            const bool atst2 = (ht.at(1) * ht.at(2) < 0.);
            if (atst1 || atst2) {

                t_ht = (atst1) ? jjm.at(1) : jjm.at(2);

                if (elements.typeAOS == QObject::tr("AOS", "Acquisition of signal")) {
                    jjm[0] = t_ht - periode;
                    jjm[1] = t_ht - 0.5 * periode;
                    jjm[2] = t_ht;

                } else {
                    jjm[0] = t_ht;
                    jjm[1] = t_ht + 0.5 * periode;
                    jjm[2] = t_ht + periode;
                }

                while (fabs(tAOS - t_ht) > EPS_DATES) {

                    tAOS = t_ht;

                    for(int i=0; i<3; i++) {

                        const Date date(jjm.at(i), 0., false);
                        obs.CalculPosVit(date);
                        sat.CalculPosVit(date);
                        sat.CalculCoordHoriz(obs, true, refraction, true);
                        ht[i] = sat.hauteur() - hauteurMin;
                    }

                    t_ht = Maths::CalculValeurXInterpolation3(jjm, ht, 0., EPS_DATES);
                    periode *= 0.5;

                    jjm[0] = t_ht - periode;
                    jjm[1] = t_ht;
                    jjm[2] = t_ht + periode;
                }

                elements.date = Date(tAOS, 0.);

                obs.CalculPosVit(elements.date);
                sat.CalculPosVit(elements.date);
                sat.CalculCoordHoriz(obs, true, refraction, true);
                elements.azimut = sat.azimut();
                afin = true;

            } else {

                t_ht += periode;
                iter++;

                if (iter > 50000) {
                    afin = true;
                    elements.aos = false;
                }
            }
        }
    }

    /* Retour */
    return elements;
}

/*
 * Calcul du noeud (ascendant ou descendant) precedent ou suivant
 */
Date Evenements::CalculNoeudOrbite(const Date &dateInit, const Satellite &satellite, const bool sensCalcul, const bool noeudAscendant)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Date date = dateInit;
    Satellite sat = satellite;
    const double st = ((sensCalcul) ? 1. : -1.) / (sat.tle().no() * T360);
    double lat1 = sat.latitude();

    /* Corps de la methode */
    bool atrouve = false;
    while (!atrouve) {

        date = Date(date.jourJulienUTC() + st, 0., false);

        sat.CalculPosVit(date);
        const double lat = sat.CalculLatitude(sat.position());

        if ((lat1 * lat) < 0.) {
            if (sensCalcul) {
                atrouve = (noeudAscendant) ? lat1 < 0. : lat < 0.;
            } else {
                atrouve = (noeudAscendant) ? lat1 > 0. : lat > 0.;
            }
        }
        lat1 = lat;
    }


    QList<double> jjm;
    QList<double> lati;

    double t_n = date.jourJulienUTC();
    double periode = st;
    bool afin = false;
    while (!afin) {

        jjm.clear();
        lati.clear();

        jjm.append(t_n - periode);
        jjm.append(t_n);
        jjm.append(t_n + periode);

        for(int i=0; i<3; i++) {

            date = Date(jjm.at(i), 0., false);
            sat.CalculPosVit(date);
            lati.append(sat.CalculLatitude(sat.position()));
        }

        const double tNoeud = Maths::CalculValeurXInterpolation3(jjm, lati, 0., EPS_DATES);
        if (fabs(t_n - tNoeud) < EPS_DATES) {
            afin = true;
        }
        t_n = tNoeud;
        periode *= 0.5;
    }

    /* Retour */
    return Date(t_n, 0., false);
}

/*
 * Calcul du prochain passage ombre->penombre ou penombre->ombre
 */
Date Evenements::CalculOmbrePenombre(const Date &dateInit, const Satellite &satellite, const int nbTrajectoires, const bool acalcEclipseLune,
                                     const bool refraction)
{
    /* Declarations des variables locales */
    Date dateEcl;

    /* Initialisations */
    int i = 0;
    Satellite sat = satellite;
    if (sat.traceAuSol().isEmpty()) {
        sat.CalculTracesAuSol(dateInit, nbTrajectoires, acalcEclipseLune, refraction);
    }

    QListIterator<ElementsTraceSol> it(sat.traceAuSol());
    while (it.hasNext()) {
        const ElementsTraceSol elements = it.next();
        if (elements.jourJulienUTC >= dateInit.jourJulienUTC()) {
            if (satellite.conditionEclipse().eclipseTotale() != elements.eclipseTotale) {
                it.toBack();
            }
        }
        i++;
    }

    /* Corps de la methode */
    if (i < sat.traceAuSol().size()) {

        double tdn = dateInit.jourJulienUTC();
        double t_ecl = sat.traceAuSol().at(i-1).jourJulienUTC;
        double periode = sat.traceAuSol().at(i).jourJulienUTC - t_ecl;

        QList<double> jjm;
        QList<double> ecl;
        Soleil soleil;
        Lune lune;
        ConditionEclipse conditionEclipse;

        bool afin = false;
        while (!afin) {

            jjm.clear();
            ecl.clear();

            jjm.append(t_ecl - periode);
            jjm.append(t_ecl);
            jjm.append(t_ecl + periode);

            for(int j=0; j<3; j++) {

                const Date date(jjm.at(j), 0., false);

                // Position du satellite
                sat.CalculPosVit(date);

                // Position du Soleil
                soleil.CalculPosition(date);

                // Position de la Lune
                if (acalcEclipseLune) {
                    lune.CalculPosition(date);
                }

                // Conditions d'eclipse du satellite
                conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, lune, refraction);
                const ElementsEclipse elements = (conditionEclipse.eclipseLune().luminosite < conditionEclipse.eclipseSoleil().luminosite) ?
                            conditionEclipse.eclipseLune() : conditionEclipse.eclipseSoleil();
                ecl.append(elements.phi - elements.phiSoleil - elements.elongation);
            }

            if (((ecl.at(0) * ecl.at(2)) < 0.) || ((ecl.at(0) > 0.) && (ecl.at(2) > 0.))) {
                tdn = qRound(NB_SEC_PAR_JOUR * Maths::CalculValeurXInterpolation3(jjm, ecl, 0., EPS_DATES)) * NB_JOUR_PAR_SEC;
            }

            periode *= 0.5;
            if (fabs(tdn - t_ecl) < EPS_DATES) {
                afin = true;
            }
            t_ecl = tdn;
        }
        dateEcl = Date(t_ecl, 0.);
    } else {
        dateEcl = Date(dateInit.jourJulienUTC() - 10., 0.);
    }

    /* Retour */
    return dateEcl;
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


