/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    prevision.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des previsions de passage des satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    19 janvier 2013
 *
 */

#include <QFile>
#include <QTime>
#include <QTextStream>
#include "prevision.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

static QVector<TLE> tabtle;
static QList<Satellite> sats;
static QList<QVector<double > > tabEphem;

/*
 * Calcul des previsions de passage
 */
void Prevision::CalculPassages(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    bool pass;

    QString ligne;
    QTime tps;

    /* Initialisations */
    pass = false;
    const QString fmt = "%1 %2 %3 %4 %5  %6 %7%8 %9 %10  %11";

    // Creation de la liste de TLE
    TLE::LectureFichier(conditions.getFic(), conditions.getListeSatellites(), tabtle);

    // Creation du tableau de satellites
    QVectorIterator<TLE> it1(tabtle);
    while (it1.hasNext()) {
        sats.append(Satellite(it1.next()));
    }

    // Lecture des donnees satellite (magnitude standard)
    Satellite::LectureDonnees(conditions.getListeSatellites(), tabtle, sats);

    // Ecriture de l'entete du fichier de previsions
    Conditions::EcrireEntete(observateur, conditions, tabtle, false);

    /* Corps de la methode */
    tps.start();

    // Calcul des ephemerides du Soleil et du lieu d'observation
    CalculEphemSoleilObservateur(conditions, observateur);

    QListIterator<QVector<double> > it3(tabEphem);

    // Ouverture du fichier de resultat
    QFile fichier(conditions.getOut());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    // Boucle sur les satellites
    QListIterator<Satellite> it2(sats);
    while (it2.hasNext()) {

        Satellite sat = it2.next();
        int ent = 0;

        const double perigee = RAYON_TERRESTRE * pow(KE * NB_MIN_PAR_JOUR / (DEUX_PI * sat.getTle().getNo()),
                                                     DEUX_TIERS) * (1. - sat.getTle().getEcco());
        const double periode = NB_JOUR_PAR_MIN * (floor(KE * pow(DEUX_PI * perigee, DEUX_TIERS)) - 16.);

        // Boucle sur le tableau d'ephemerides du Soleil
        it3.toFront();
        while (it3.hasNext()) {

            const QVector<double> list = it3.next();
            Date date = Date(list.at(0), 0., false);

            // Donnees liees au lieu d'observation
            const Vecteur3D obsPos = Vecteur3D(list.at(1), list.at(2), list.at(3));
            const Vecteur3D v1 = Vecteur3D(list.at(4), list.at(5), list.at(6));
            const Vecteur3D v2 = Vecteur3D(list.at(7), list.at(8), list.at(9));
            const Vecteur3D v3 = Vecteur3D(list.at(10), list.at(11), list.at(12));
            const Matrice mat = Matrice(v1, v2, v3);
            const Observateur obs = Observateur(obsPos, Vecteur3D(), mat, observateur.getAaer(), observateur.getAray());

            // Position ECI du Soleil
            const Vecteur3D solPos = Vecteur3D(list.at(13), list.at(14), list.at(15));
            Soleil soleil = Soleil(solPos);

            // Position ECI du satellite
            sat.CalculPosVit(date);

            // Position topocentrique du satellite
            sat.CalculCoordHoriz(obs, false);

            // Le satellite a une hauteur superieure a celle specifiee
            if (sat.getHauteur() >= conditions.getHaut()) {

                // Conditions d'eclipse du satellite
                sat.CalculSatelliteEclipse(soleil);

                // Le satellite n'est pas eclipse
                if (!sat.isEclipse() || !conditions.getEcl()) {

                    // Magnitude du satellite
                    sat.CalculMagnitude(obs, conditions.getExt());

                    // Toutes les conditions sont remplies
                    if (sat.getMagnitude() < conditions.getMgn1() ||
                            (sat.getMagnitudeStandard() > 98. && conditions.getMgn1() > 98.) || !conditions.getEcl()) {

                        if (ent == 2)
                            ent = 1;

                        sat.CalculCoordHoriz(obs);
                        soleil.CalculCoordHoriz(obs);

                        // Ascension droite, declinaison, constellation
                        sat.CalculCoordEquat(obs);

                        bool afin = false;
                        while (!afin) {

                            if ((!sat.isEclipse() || !conditions.getEcl()) &&
                                    (sat.getMagnitude() < conditions.getMgn1() ||
                                     sat.getMagnitudeStandard() > 98. || !conditions.getEcl())) {

                                // Altitude du satellite
                                double altitude, ct, lat, phi;
                                const Vecteur3D position = sat.getPosition();
                                const double r = sqrt(position.getX() * position.getX() + position.getY() * position.getY());
                                lat = atan((position.getZ() / r));
                                do {
                                    phi = lat;
                                    const double sph = sin(phi);
                                    ct = 1. / (sqrt(1. - E2 * sph * sph));
                                    lat = atan((position.getZ() + RAYON_TERRESTRE * ct * E2 * sph) / r);
                                } while (fabs(lat - phi) > 1.e-7);
                                altitude = r / cos(lat) - RAYON_TERRESTRE * ct;

                                // Ecriture du resultat
                                if (ent == 0) {

                                    const QString nomsat = sat.getTle().getNom();

                                    if (nomsat.toLower() == "iss") {
                                        flux << "ISS" << endl;
                                    } else {
                                        ligne = nomsat;
                                        if (nomsat.contains("R/B") || nomsat.contains(" DEB"))
                                            ligne = ligne.append(QObject::tr("  (numéro NORAD : %1)")).
                                                    arg(sat.getTle().getNorad());
                                        flux << ligne << endl;
                                    }

                                    flux << QObject::tr("   Date     Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const Magn  Altitude  Distance  Az Soleil   Haut Soleil") << endl;
                                    ent = 1;
                                }

                                // Calcul de la date calendaire
                                const Date date2 = Date(date.getJourJulien() + conditions.getDtu() + EPS_DATES, 0.);

                                // Coordonnees topocentriques du satellite
                                const QString az = Maths::ToSexagesimal(sat.getAzimut(), DEGRE, 3, 0, false, false);
                                const QString ht = Maths::ToSexagesimal(sat.getHauteur(), DEGRE, 2, 0, false, false);;

                                // Coordonnees equatoriales du satellite
                                const QString ad = Maths::ToSexagesimal(sat.getAscensionDroite(), HEURE1, 2, 0, false, false);
                                const QString de = Maths::ToSexagesimal(sat.getDeclinaison(), DEGRE, 2, 0, true, false);

                                // Magnitude
                                const double mag = sat.getMagnitude();
                                QString magn;
                                if (mag > 98.) {
                                    magn = (conditions.getEcl() || sat.getMagnitudeStandard() > 98.) ?
                                                " ????  " : " ----  ";
                                } else {
                                    const QString fmagn = "%1%2%3%4";
                                    const QString esp = (mag < 9.95) ? " " : "";
                                    const QString signe = (mag >= 0.) ? "+" : "-";
                                    const QString pen = (sat.isPenombre()) ? "* " : "  ";
                                    magn = fmagn.arg(esp).arg(signe).arg(fabs(mag), 0, 'f', 1, QChar('0')).arg(pen);
                                }

                                // Altitude du satellite et distance a l'observateur
                                double distance = sat.getDistance();
                                if (conditions.getUnite() == QObject::tr("mi")) {
                                    altitude *= MILE_PAR_KM;
                                    distance *= MILE_PAR_KM;
                                }

                                // Coordonnees topocentriques du Soleil
                                const QString azs = Maths::ToSexagesimal(soleil.getAzimut(), DEGRE, 3, 0, false, false);
                                const QString hts = Maths::ToSexagesimal(soleil.getHauteur(), DEGRE, 2, 0, true, false);

                                const QString result(fmt.arg(date2.ToShortDateChrono(COURT)).arg(az).arg(ht).arg(ad).arg(de).
                                        arg(sat.getConstellation()).arg(magn).arg(altitude, 8, 'f', 1).
                                        arg(distance, 9, 'f', 1).arg(azs).arg(hts));

                                flux << result << endl;
                                pass = true;
                            } else {
                                if (pass) {
                                    pass = false;
                                    flux << endl;
                                }
                            }

                            // Calcul pour le pas suivant
                            date = Date(date.getJourJulienUTC() + conditions.getPas0(), 0., false);
                            observateur.CalculPosVit(date);
                            soleil.CalculPosition(date);
                            soleil.CalculCoordHoriz(observateur);

                            if (soleil.getHauteur() > conditions.getCrep() ||
                                    date.getJourJulienUTC() > conditions.getJj2() + conditions.getPas0()) {
                                afin = true;
                            } else {

                                sat.CalculPosVit(date);
                                sat.CalculCoordHoriz(observateur);
                                if (sat.getHauteur() < conditions.getHaut()) {
                                    afin = true;
                                } else {
                                    sat.CalculSatelliteEclipse(soleil);
                                    sat.CalculMagnitude(observateur, conditions.getExt());
                                    sat.CalculCoordEquat(observateur);
                                }
                            }
                        } // fin while (!afin)

                        ent = 2;
                        if (pass) {
                            pass = false;
                            flux << endl;
                        }
                        date = Date(date.getJourJulienUTC() + periode, 0., false);

                        // Recherche de la nouvelle date dans le tableau d'ephemerides
                        bool atrouve = false;
                        while (it3.hasNext() && !atrouve) {
                            const double jj = it3.next().at(0);
                            if (jj >= date.getJourJulienUTC()) {
                                atrouve = true;
                                it3.previous();
                            }
                        }
                    }
                }
            }
        }
    }
    int fin = tps.elapsed();

    flux << endl;
    ligne = QObject::tr("Temps écoulé : %1s");
    ligne = ligne.arg(1.e-3 * fin, 0, 'f', 2);
    flux << ligne << endl;
    fichier.close();
    FinTraitement();

    /* Retour */
    return;
}

void Prevision::FinTraitement()
{
    tabtle.clear();
    sats.clear();
    tabEphem.clear();
}

/*
 * Calcul des ephemerides du Soleil et de la position de l'observateur
 */
void Prevision::CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    bool svis;
    QVector<double> tab;
    Soleil soleil;

    /* Initialisations */
    svis = true;
    const double pas = NB_JOUR_PAR_MIN;

    /* Corps de la methode */
    Date date = Date(conditions.getJj1(), 0., false);
    do {

        // Position ECI de l'observateur
        observateur.CalculPosVit(date);

        // Position ECI du Soleil
        soleil.CalculPosition(date);

        // Position topocentrique du Soleil
        soleil.CalculCoordHoriz(observateur, false);

        if (soleil.getHauteur() <= conditions.getCrep()) {

            svis = false;
            tab.clear();

            // Remplissage du tableau d'ephemerides
            tab.push_back(date.getJourJulienUTC());

            tab.push_back(observateur.getPosition().getX());
            tab.push_back(observateur.getPosition().getY());
            tab.push_back(observateur.getPosition().getZ());
            tab.push_back(observateur.getRotHz().getVecteur1().getX());
            tab.push_back(observateur.getRotHz().getVecteur1().getY());
            tab.push_back(observateur.getRotHz().getVecteur1().getZ());
            tab.push_back(observateur.getRotHz().getVecteur2().getX());
            tab.push_back(observateur.getRotHz().getVecteur2().getY());
            tab.push_back(observateur.getRotHz().getVecteur2().getZ());
            tab.push_back(observateur.getRotHz().getVecteur3().getX());
            tab.push_back(observateur.getRotHz().getVecteur3().getY());
            tab.push_back(observateur.getRotHz().getVecteur3().getZ());

            tab.push_back(soleil.getPosition().getX());
            tab.push_back(soleil.getPosition().getY());
            tab.push_back(soleil.getPosition().getZ());

            tabEphem.append(tab);

        } else {
            if (!svis) {
                svis = true;
                if (conditions.getCrep() <= EPSDBL100)
                    date = Date(date.getJourJulienUTC() + 0.375, 0., false);
            }
        }

        date = Date(date.getJourJulienUTC() + pas, 0., false);
    } while (date.getJourJulienUTC() <= conditions.getJj2());

    tab.clear();

    /* Retour */
    return;
}
