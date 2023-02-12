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
 * >    17 octobre 2022
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
 * Methodes publiques
 */
/*
 * Calcul de l'AOS (ou LOS) suivant ou precedent
 */
ElementsAOS Evenements::CalculAOS(const Date &dateInit, const Satellite &satellite, const Observateur &observateur,
                                  const SensCalcul &sensCalcul, const double hauteurMin, const bool refraction)
{
    /* Declarations des variables locales */
    ElementsAOS elements;

    /* Initialisations */
    Satellite sat = satellite;
    Observateur obs = observateur;
    elements.aos = sat.hasAOS(obs);
    elements.azimut = 0.;
    const double st = (sensCalcul == SensCalcul::CHRONOLOGIQUE) ? 1. : -1.;

    /* Corps de la methode */
    if (elements.aos) {

        double periode = st * NB_JOUR_PAR_MIN;
        if (((sat.hauteur() - hauteurMin) * st) > EPSDBL100) {
            elements.typeAOS = QObject::tr("LOS", "Loss of signal");
        }

        double tAOS = 0.;
        double t_ht = dateInit.jourJulienUTC();

        std::array<double, DEGRE_INTERPOLATION> jjm;
        std::array<double, DEGRE_INTERPOLATION> ht;

        bool afin = false;
        unsigned int iter = 0;
        while (!afin) {

            jjm[0] = t_ht;
            jjm[1] = jjm[0] + 0.5 * periode;
            jjm[2] = jjm[0] + periode;

            for(unsigned int i=0; i<DEGRE_INTERPOLATION; i++) {

                const Date date(jjm[i], 0., false);
                obs.CalculPosVit(date);
                sat.CalculPosVit(date);
                sat.CalculCoordHoriz(obs, false, refraction, true);
                ht[i] = sat.hauteur() - hauteurMin;
            }

            const bool atst1 = (ht[0] * ht[1] < 0.);
            const bool atst2 = (ht[1] * ht[2] < 0.);
            if (atst1 || atst2) {

                t_ht = (atst1) ? jjm[1] : jjm[2];

                if (elements.typeAOS == QObject::tr("AOS", "Acquisition of signal")) {
                    jjm[0] = t_ht - periode;
                    jjm[1] = t_ht - 0.5 * periode;
                    jjm[2] = t_ht;

                } else {
                    jjm[0] = t_ht;
                    jjm[1] = t_ht + 0.5 * periode;
                    jjm[2] = t_ht + periode;
                }

                while (fabs(tAOS - t_ht) > EPSDBL100) {

                    tAOS = t_ht;

                    for(unsigned int i=0; i<DEGRE_INTERPOLATION; i++) {

                        const Date date(jjm[i], 0., false);
                        obs.CalculPosVit(date);
                        sat.CalculPosVit(date);
                        sat.CalculCoordHoriz(obs, true, refraction, true);
                        ht[i] = sat.hauteur() - hauteurMin;
                    }

                    t_ht = Maths::CalculValeurXInterpolation3(jjm, ht, 0., EPSDBL100);
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

                if (iter > ITERATIONS_MAX) {
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
Date Evenements::CalculNoeudOrbite(const Date &dateInit, const Satellite &satellite, const SensCalcul &sensCalcul, const TypeNoeudOrbite &typeNoeud)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Date date = dateInit;
    Satellite sat = satellite;
    const double st = ((sensCalcul == SensCalcul::CHRONOLOGIQUE) ? 1. : -1.) / (sat.elementsOrbitaux().no * T360);
    double lat1 = sat.latitude();

    /* Corps de la methode */
    bool atrouve = false;
    while (!atrouve) {

        date = Date(date.jourJulienUTC() + st, 0., false);

        sat.CalculPosVit(date);
        const double lat = sat.CalculLatitude(sat.position());

        if ((lat1 * lat) < 0.) {
            if (sensCalcul == SensCalcul::CHRONOLOGIQUE) {
                atrouve = (typeNoeud == TypeNoeudOrbite::NOEUD_ASCENDANT) ? lat1 < 0. : lat < 0.;
            } else {
                atrouve = (typeNoeud == TypeNoeudOrbite::NOEUD_ASCENDANT) ? lat1 > 0. : lat > 0.;
            }
        }
        lat1 = lat;
    }


    std::array<double, DEGRE_INTERPOLATION> jjm;
    std::array<double, DEGRE_INTERPOLATION> lati;

    double t_n = date.jourJulienUTC();
    double periode = st;
    bool afin = false;
    while (!afin) {

        jjm[0] = t_n - periode;
        jjm[1] = t_n;
        jjm[2] = t_n + periode;

        for(unsigned int i=0; i<DEGRE_INTERPOLATION; i++) {

            date = Date(jjm[i], 0., false);
            sat.CalculPosVit(date);
            lati[i] = sat.CalculLatitude(sat.position());
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

    QListIterator it(sat.traceAuSol());
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

        std::array<double, DEGRE_INTERPOLATION> jjm;
        std::array<double, DEGRE_INTERPOLATION> ecl;
        Soleil soleil;
        Lune lune;
        ConditionEclipse conditionEclipse;

        bool afin = false;
        while (!afin) {

            jjm[0] = t_ecl - periode;
            jjm[1] = t_ecl;
            jjm[2] = t_ecl + periode;

            for(unsigned int j=0; j<DEGRE_INTERPOLATION; j++) {

                const Date date(jjm[j], 0., false);

                // Position du satellite
                sat.CalculPosVit(date);

                // Position du Soleil
                soleil.CalculPosition(date);

                // Position de la Lune
                if (acalcEclipseLune) {
                    lune.CalculPosition(date);
                }

                // Conditions d'eclipse du satellite
                conditionEclipse.CalculSatelliteEclipse(sat.position(), soleil, &lune, refraction);
                const ElementsEclipse elements = (conditionEclipse.eclipseLune().luminosite < conditionEclipse.eclipseSoleil().luminosite) ?
                            conditionEclipse.eclipseLune() : conditionEclipse.eclipseSoleil();
                ecl[j] = elements.phi - elements.phiSoleil - elements.elongation;
            }

            if (((ecl[0] * ecl[2]) < 0.) || ((ecl[0] > 0.) && (ecl[2] > 0.))) {
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


/*
 * Accesseurs
 */


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

