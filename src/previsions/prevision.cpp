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
 * >    31 octobre 2021
 *
 */

#include <QElapsedTimer>
#include "configuration/configuration.h"
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
    double tlemin = -DATE_INFINIE;
    double tlemax = DATE_INFINIE;

    tps.start();
    _donnees.ageTle.clear();
    _resultats.clear();

    // Creation de la liste de TLE
    const QMap<QString, TLE> tabTle = TLE::LectureFichier(_conditions.fichier, _conditions.listeSatellites);

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

    // Calcul des ephemerides du Soleil et du lieu d'observation
    const QList<EphemeridesPrevisions> tabEphem = CalculEphemSoleilObservateur();

    /* Corps de la methode */
    QListIterator<EphemeridesPrevisions> it3(tabEphem);

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
    QListIterator<Satellite> it2(sats);
    while (it2.hasNext()) {

        resultatSat.clear();
        sat = it2.next();

        const double perigee = RAYON_TERRESTRE * pow(KE * NB_MIN_PAR_JOUR / (DEUX_PI * sat.tle().no()), DEUX_TIERS) * (1. - sat.tle().ecco());
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
                condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);

                // Le satellite n'est pas eclipse
                if (!condEcl.eclipseTotale() || !_conditions.eclipse) {

                    // Magnitude du satellite
                    magnitude.Calcul(condEcl, obs, sat.distance(), sat.hauteur(), sat.tle().donnees().magnitudeStandard(), _conditions.extinction,
                                     _conditions.effetEclipsePartielle);

                    // Toutes les conditions sont remplies
                    if (magnitude.magnitude() < _conditions.magnitudeLimite ||
                            ((sat.tle().donnees().magnitudeStandard() > 98.) && (_conditions.magnitudeLimite > 98.)) || !_conditions.eclipse) {

                        sat.CalculCoordHoriz(obs);
                        soleil.CalculCoordHoriz(obs);

                        // Ascension droite, declinaison, constellation
                        sat.CalculCoordEquat(obs);

                        result.clear();

                        afin = false;
                        while (!afin) {

                            if ((!condEcl.eclipseTotale() ||
                                 !_conditions.eclipse) && ((magnitude.magnitude() < _conditions.magnitudeLimite) ||
                                                           ((sat.tle().donnees().magnitudeStandard() > 98.) || !_conditions.eclipse))) {

                                // Nom du satellite
                                res.nom = sat.tle().nom();

                                // Elements orbitaux
                                res.tle = sat.tle();

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
                                res.magnitudeStd = sat.tle().donnees().magnitudeStandard();
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
                                    condEcl.CalculSatelliteEclipse(sat.position(), soleil, lune, _conditions.refraction);
                                    magnitude.Calcul(condEcl, _conditions.observateur, sat.distance(), sat.hauteur(),
                                                     sat.tle().donnees().magnitudeStandard(), _conditions.extinction, _conditions.effetEclipsePartielle);

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
            _resultats.insert(sat.tle().nom() + " " + sat.tle().norad(), resultatSat);
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
