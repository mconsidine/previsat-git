/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <QFile>
#include <QTime>
#include <QTextStream>
#include <QStringList>
#include "prevision.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

/*
 * Calcul des previsions de passage
 */
void Prevision::CalculPassages(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    bool pass, svis, vis;

    QString ligne;
    QStringList res;
    QTime tps;
    QVector<TLE> tabtle;
    QList<Satellite> sats;
    QList<QVector<double > > tabEphem;

    /* Initialisations */
    pass = false;
    svis = true;
    vis = false;

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
    CalculEphemSoleilObservateur(conditions, observateur, tabEphem);

    QListIterator<QVector<double> > it3(tabEphem);

    // Boucle sur les satellites
    QListIterator<Satellite> it2(sats);
    while (it2.hasNext()) {

        Satellite sat = it2.next();
        int ent = 0;
        vis = false;

        const double perigee = RAYON_TERRESTRE * pow(KE * NB_MIN_PAR_JOUR / (DEUX_PI * sat.getTle().getNo()), DEUX_TIERS) * (1. - sat.getTle().getEcco());
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

                vis = true;
                // Conditions d'eclipse du satellite
                sat.CalculSatelliteEclipse(soleil);

                // Le satellite n'est pas eclipse
                if (!sat.isEclipse() || !conditions.getEcl()) {

                    // Magnitude du satellite
                    sat.CalculMagnitude(obs, conditions.getExt());

                    // Toutes les conditions sont remplies
                    if (sat.getMagnitude() < conditions.getMgn1() || sat.getMagnitudeStandard() > 98. || !conditions.getEcl()) {

                        if (ent == 2)
                            ent = 1;

                        sat.CalculCoordHoriz(obs);
                        soleil.CalculCoordHoriz(obs);

                        // Ascension droite, declinaison, constellation
                        sat.CalculCoordEquat(obs);

                        bool afin = false;
                        while (!afin) {

                            if ((!sat.isEclipse() || !conditions.getEcl()) && (sat.getMagnitude() < conditions.getMgn1() || sat.getMagnitudeStandard() > 98. || !conditions.getEcl())) {

                                // Altitude du satellite
                                double altitude, ct, lat, phi;
                                Vecteur3D position = sat.getPosition();
                                const double r = sqrt(position.getX() * position.getX() + position.getY() * position.getY());
                                lat = atan((position.getZ() / r));
                                do {
                                    phi = lat;
                                    double sph = sin(phi);
                                    ct = 1. / (sqrt(1. - E2 * sph * sph));
                                    lat = atan((position.getZ() + RAYON_TERRESTRE * ct * E2 * sph) / r);
                                } while (fabs(lat - phi) > 1.e-7);
                                altitude = r / cos(lat) - RAYON_TERRESTRE * ct;

                                // Ecriture du resultat
                                if (ent == 0) {
                                    if (res.count() > 0)
                                        if (res.at(res.count() - 1) != "")
                                            res.append("");

                                    QString nom = sat.getTle().getNom();
                                    if (nom.toLower() == "iss") {
                                        res.append("ISS");
                                    } else {
                                        ligne = nom;
                                        if (nom.contains("R/B") || nom.contains(" DEB"))
                                            ligne = ligne.append(QObject::tr("  (numéro NORAD : %1)")).arg(sat.getTle().getNorad());
                                        res.append(ligne);
                                    }
                                    res.append(QObject::tr("   Date     Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const Magn  Altitude  Distance  Az Soleil   Haut Soleil"));
                                    ent = 1;
                                }

                                // Calcul de la date calendaire
                                Date date2 = Date(date.getJourJulien() + conditions.getDtu() + EPS_DATES, 0.);
                                ligne = date2.ToShortDate(Date::COURT).append(" ");

                                // Coordonnees topocentriques du satellite
                                ligne = ligne.append(Maths::ToSexagesimal(sat.getAzimut(), Maths::DEGRE, 3, 0, false, false)).append(" ");
                                ligne = ligne.append(Maths::ToSexagesimal(sat.getHauteur(), Maths::DEGRE, 2, 0, false, false)).append(" ");

                                // Coordonnees equatoriales du satellite
                                ligne = ligne.append(Maths::ToSexagesimal(sat.getAscensionDroite(), Maths::HEURE1, 2, 0, false, false)).append(" ");
                                ligne = ligne.append(Maths::ToSexagesimal(sat.getDeclinaison(), Maths::DEGRE, 2, 0, true, false)).append("  ");

                                // Constellation
                                ligne = ligne.append(sat.getConstellation()).append(" ");

                                // Magnitude
                                const double mag = sat.getMagnitude();
                                if (mag > 98.) {
                                    ligne = (conditions.getEcl() || sat.getMagnitudeStandard() > 98.) ? ligne.append(" ????  ") : ligne.append(" ----  ");
                                } else {
                                    if (mag < 9.95)
                                        ligne = ligne.append(" ");
                                    ligne = (mag >= 0.) ? ligne.append("+%1") : ligne.append("-%1");
                                    ligne = ligne.arg(fabs(mag), 0, 'f', 1, QChar('0'));
                                    ligne = (sat.isPenombre()) ? ligne.append("* ") : ligne.append("  ");
                                }

                                // Altitude du satellite et distance a l'observateur
                                ligne = ligne.append("%1 %2 ");
                                double distance = sat.getDistance();
                                if (conditions.getUnite() == QObject::tr("mi")) {
                                    altitude *= MILE_PAR_KM;
                                    distance *= MILE_PAR_KM;
                                }
                                ligne = ligne.arg(altitude, 8, 'f', 1).arg(distance, 9, 'f', 1);

                                // Coordonnees topocentriques du Soleil
                                ligne = ligne.append(Maths::ToSexagesimal(soleil.getAzimut(), Maths::DEGRE, 3, 0, false, false)).append("  ");
                                ligne = ligne.append(Maths::ToSexagesimal(soleil.getHauteur(), Maths::DEGRE, 2, 0, true, false));

                                res.append(ligne);
                                pass = true;
                            } else {
                                if (pass) {
                                    pass = false;
                                    res.append("");
                                }
                            }

                            // Calcul pour le pas suivant
                            date = Date(date.getJourJulienUTC() + conditions.getPas0(), 0., false);
                            observateur.CalculPosVit(date);
                            soleil.CalculPosition(date);
                            soleil.CalculCoordHoriz(observateur);

                            if (soleil.getHauteur() > conditions.getCrep()) {
                                afin = true;
                            } else {

                                sat.CalculPosVit(date);
                                sat.CalculCoordHoriz(observateur);
                                if (sat.getHauteur() < 0.) {
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
                            res.append("");
                        }
                        date = Date(date.getJourJulienUTC() + periode, 0., false);

                        // Recherche de la nouvelle date dans le tableau d'ephemerides
                        bool atrouve = false;
                        while (it3.hasNext() && !atrouve) {
                            double jj = it3.next().at(0);
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

    // Ecriture des resultats dans le fichier de previsions
    QFile fichier(conditions.getOut());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    int i = 0;
    while (i < res.count()) {
        flux << res.at(i) << endl;
        i++;
    }
    if (res.count() > 0)
        if (res.at(res.count() - 1) != "")
            flux << endl;

    ligne = QObject::tr("Temps écoulé : %1s");
    ligne = ligne.arg(1.e-3 * fin, 0, 'f', 2);
    flux << ligne << endl;
    fichier.close();

    /* Retour */
    return;
}

/*
 * Calcul des ephemerides du Soleil et de la position de l'observateur
 */
void Prevision::CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur, QList<QVector<double> > &tabEphem)
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

    /* Retour */
    return;
}
