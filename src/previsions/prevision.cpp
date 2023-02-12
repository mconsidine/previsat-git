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
 * >    previsions.cpp
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
 * >    4 fevrier 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QElapsedTimer>
#include <QFileInfo>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "prevision.h"


static ConditionsPrevisions _conditions;
static QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
static DonneesPrevisions _donnees;

struct EphemeridesPrevisions
{
    double jourJulienUTC;
    Vecteur3D positionObservateur;
    Matrice3D rotHz;
    Vecteur3D positionSoleil;
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
QMap<QString, QList<QList<ResultatPrevisions> > > &Prevision::resultats()
{
    return _resultats;
}

DonneesPrevisions Prevision::donnees()
{
    return _donnees;
}


/*
 * Modificateurs
 */
void Prevision::setConditions(const ConditionsPrevisions &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des previsions de passage
 */
int Prevision::CalculPrevisions(int &nombre)
{
    /* Declarations des variables locales */
    QElapsedTimer tps;
    QList<Satellite> sats;

    /* Initialisations */
    double elemMin = -DATE_INFINIE;
    double elemMax = DATE_INFINIE;
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
        if (epok > elemMin) {
            elemMin = epok;
        }

        if (epok < elemMax) {
            elemMax = epok;
        }
    }

    if (tabElem.keys().count() == 1) {
        _donnees.ageElementsOrbitaux.append(fabs(_conditions.jj1 - elemMin));
    } else {

        const double age1 = fabs(_conditions.jj1 - elemMin);
        const double age2 = fabs(_conditions.jj1 - elemMax);
        _donnees.ageElementsOrbitaux.append(qMin(age1, age2));
        _donnees.ageElementsOrbitaux.append(qMax(age1, age2));
    }

    // Calcul des ephemerides du Soleil et du lieu d'observation
    const QList<EphemeridesPrevisions> tabEphem = CalculEphemSoleilObservateur();

    /* Corps de la methode */
    QListIterator it3(tabEphem);

    bool afin;
    bool atrouve;
    Date date;
    Soleil soleil;
    Lune lune;
    Satellite sat;
    ConditionEclipse condEcl;
    Magnitude magnitude;
    ResultatPrevisions res;
    QList<ResultatPrevisions> result;
    QList<QList<ResultatPrevisions> > resultatSat;

    // Boucle sur les satellites
    QListIterator it2(sats);
    while (it2.hasNext()) {

        resultatSat.clear();
        sat = it2.next();

        const double perigee = RAYON_TERRESTRE * pow(KE * NB_MIN_PAR_JOUR / (DEUX_PI * sat.elementsOrbitaux().no), DEUX_TIERS) *
                (1. - sat.elementsOrbitaux().ecco);
        const double periode = NB_JOUR_PAR_MIN * (floor(KE * pow(DEUX_PI * perigee, DEUX_TIERS)) - 16.);

        // Boucle sur le tableau d'ephemerides
        it3.toFront();
        while (it3.hasNext()) {

            const EphemeridesPrevisions ephem = it3.next();

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

                // Conditions d'eclipse du satellite
                condEcl.CalculSatelliteEclipse(sat.position(), soleil, &lune, _conditions.refraction);

                // Le satellite n'est pas eclipse
                if (!condEcl.eclipseTotale() || !_conditions.eclipse) {

                    // Magnitude du satellite
                    magnitude.Calcul(condEcl, obs, sat.distance(), sat.hauteur(), sat.elementsOrbitaux().donnees.magnitudeStandard(),
                                     _conditions.extinction,
                                     _conditions.effetEclipsePartielle);

                    // Toutes les conditions sont remplies
                    if (magnitude.magnitude() < _conditions.magnitudeLimite ||
                            ((sat.elementsOrbitaux().donnees.magnitudeStandard() > 98.) && (_conditions.magnitudeLimite > 98.))
                            || !_conditions.eclipse) {

                        sat.CalculCoordHoriz(obs);
                        soleil.CalculCoordHoriz(obs);

                        // Ascension droite, declinaison, constellation
                        sat.CalculCoordEquat(obs);

                        result.clear();

                        afin = false;
                        while (!afin) {

                            if ((!condEcl.eclipseTotale() ||
                                 !_conditions.eclipse) && ((magnitude.magnitude() < _conditions.magnitudeLimite) ||
                                                           ((sat.elementsOrbitaux().donnees.magnitudeStandard() > 98.) || !_conditions.eclipse))) {

                                // Nom du satellite
                                res.nom = sat.elementsOrbitaux().nom;

                                // Elements orbitaux
                                res.elements = sat.elementsOrbitaux();

                                // Altitude du satellite
                                sat.CalculLatitude(sat.position());
                                res.altitude = sat.CalculAltitude(sat.position());

                                // Date calendaire (UTC)
                                res.date = Date(date.jourJulienUTC(), 0.);

                                // Coordonnees topocentriques du satellite
                                res.azimut = sat.azimut();
                                res.hauteur = sat.hauteur();

                                // Coordonnees equatoriales du satellite
                                res.ascensionDroite = sat.ascensionDroite();
                                res.declinaison = sat.declinaison();
                                res.constellation = sat.constellation();

                                // Magnitude
                                res.magnitude = magnitude.magnitude();
                                res.magnitudeStd = sat.elementsOrbitaux().donnees.magnitudeStandard();
                                res.penombre = (condEcl.eclipsePartielle() || condEcl.eclipseAnnulaire());

                                // Distance a l'observateur
                                res.distance = sat.distance();

                                // Coordonnees topocentriques du Soleil
                                res.azimutSoleil = soleil.azimut();
                                res.hauteurSoleil = soleil.hauteur();

                                result.append(res);
                            }

                            // Calcul pour le pas suivant
                            date = Date(date.jourJulienUTC() + _conditions.pas, 0., false);
                            _conditions.observateur.CalculPosVit(date);
                            soleil.CalculPosition(date);
                            soleil.CalculCoordHoriz(_conditions.observateur);

                            if ((soleil.hauteur() > _conditions.crepuscule) || (date.jourJulienUTC() > (_conditions.jj2 + _conditions.pas))) {
                                afin = true;
                            } else {
                                sat.CalculPosVit(date);
                                sat.CalculCoordHoriz(_conditions.observateur);

                                if (sat.hauteur() < _conditions.hauteur) {
                                    afin = true;
                                } else {
                                    if (_conditions.calcEclipseLune) {
                                        lune.CalculPosition(date);
                                    }
                                    condEcl.CalculSatelliteEclipse(sat.position(), soleil, &lune, _conditions.refraction);
                                    magnitude.Calcul(condEcl, _conditions.observateur, sat.distance(), sat.hauteur(),
                                                     sat.elementsOrbitaux().donnees.magnitudeStandard(), _conditions.extinction,
                                                     _conditions.effetEclipsePartielle);

                                    sat.CalculCoordEquat(_conditions.observateur);
                                }
                            }
                        } // fin while afin

                        if (!result.isEmpty()) {
                            resultatSat.append(result);
                        }
                        date = Date(date.jourJulienUTC() + periode, 0., false);

                        // Recherche de la nouvelle date dans le tableau d'ephemerides
                        atrouve = false;
                        while (it3.hasNext() && !atrouve) {
                            const double jj = it3.next().jourJulienUTC;
                            if (jj >= date.jourJulienUTC()) {
                                atrouve = true;
                                it3.previous();
                            }
                        }
                    }
                }
            }
        }

        if (!resultatSat.isEmpty()) {
            _resultats.insert(sat.elementsOrbitaux().nom + " " + sat.elementsOrbitaux().norad, resultatSat);
        }
    }

    _donnees.tempsEcoule = tps.elapsed();

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
 * Calcul des ephemerides du Soleil et de l'observateur
 */
QList<EphemeridesPrevisions> Prevision::CalculEphemSoleilObservateur()
{
    /* Declarations des variables locales */
    Soleil soleil;
    EphemeridesPrevisions eph;
    QList<EphemeridesPrevisions> tabEphem;

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
