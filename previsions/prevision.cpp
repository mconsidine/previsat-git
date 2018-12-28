/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    11 mars 2018
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QFile>
#include <QSettings>
#include <QTime>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "prevision.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"
#include "librairies/maths/maths.h"

static QVector<TLE> tabtle;
static QList<Satellite> sats;
static QList<QVector<double > > tabEphem;

/*
 * Calcul des previsions de passage
 */
void Prevision::CalculPassages(const Conditions &conditions, Observateur &observateur, QStringList &res)
{
    /* Declarations des variables locales */
    bool pass;

    QString ligne;
    QTime tps;

    /* Initialisations */
    pass = false;
    const QString fmt = "%1%2 %3 %4 %5  %6 %7%8 %9 %10  %11%12";

    // Creation de la liste de TLE
    TLE::LectureFichier(conditions.fic(), conditions.listeSatellites(), tabtle);

    // Creation du tableau de satellites
    QVectorIterator<TLE> it1(tabtle);
    while (it1.hasNext()) {
        sats.append(Satellite(it1.next()));
    }

    // Lecture des donnees satellite (magnitude standard)
    Satellite::LectureDonnees(conditions.listeSatellites(), tabtle, sats);

    // Ecriture de l'entete du fichier de previsions
    Conditions::EcrireEntete(observateur, conditions, tabtle, false);

    /* Corps de la methode */
    res.clear();
    tps.start();

    // Calcul des ephemerides du Soleil et du lieu d'observation
    CalculEphemSoleilObservateur(conditions, observateur);

    QListIterator<QVector<double> > it3(tabEphem);

    // Boucle sur les satellites
    QListIterator<Satellite> it2(sats);
    while (it2.hasNext()) {

        Satellite sat = it2.next();
        int ent = 0;

        const double perigee = RAYON_TERRESTRE * pow(KE * NB_MIN_PAR_JOUR / (DEUX_PI * sat.tle().no()),
                                                     DEUX_TIERS) * (1. - sat.tle().ecco());
        const double periode = NB_JOUR_PAR_MIN * (floor(KE * pow(DEUX_PI * perigee, DEUX_TIERS)) - 16.);

        // Boucle sur le tableau d'ephemerides du Soleil
        it3.toFront();
        while (it3.hasNext()) {

            const QVector<double> list = it3.next();
            Date date(list.at(0), 0., false);

            // Donnees liees au lieu d'observation
            const Vecteur3D obsPos(list.at(1), list.at(2), list.at(3));
            const Vecteur3D v1(list.at(4), list.at(5), list.at(6));
            const Vecteur3D v2(list.at(7), list.at(8), list.at(9));
            const Vecteur3D v3(list.at(10), list.at(11), list.at(12));
            const Matrice3D mat(v1, v2, v3);
            const Observateur obs(obsPos, Vecteur3D(), mat, observateur.aaer(), observateur.aray());

            // Position ECI du Soleil
            const Vecteur3D solPos(list.at(13), list.at(14), list.at(15));
            Soleil soleil(solPos);

            // Position ECI du satellite
            sat.CalculPosVit(date);

            // Position topocentrique du satellite
            sat.CalculCoordHoriz(obs, false);

            // Le satellite a une hauteur superieure a celle specifiee
            if (sat.hauteur() >= conditions.haut()) {

                Lune lune;
                if (conditions.acalcEclipseLune())
                    lune.CalculPosition(date);

                // Conditions d'eclipse du satellite
                ConditionEclipse condEcl;
                condEcl.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(), conditions.refr());

                // Le satellite n'est pas eclipse
                if (!condEcl.isEclipseTotale() || !conditions.ecl()) {

                    // Magnitude du satellite
                    Magnitude magn;
                    magn.Calcul(obs, condEcl, sat.distance(), sat.hauteur(), sat.magnitudeStandard(), conditions.ext(),
                                conditions.effetEclipsePartielle());

                    // Toutes les conditions sont remplies
                    if (magn.magnitude() < conditions.mgn1() ||
                            (sat.magnitudeStandard() > 98. && conditions.mgn1() > 98.) || !conditions.ecl()) {

                        if (ent == 2)
                            ent = 1;

                        sat.CalculCoordHoriz(obs);
                        soleil.CalculCoordHoriz(obs);

                        // Ascension droite, declinaison, constellation
                        sat.CalculCoordEquat(obs);

                        bool afin = false;
                        while (!afin) {

                            if ((!condEcl.isEclipseTotale() || !conditions.ecl()) &&
                                    (magn.magnitude() < conditions.mgn1() || sat.magnitudeStandard() > 98. || !conditions.ecl())) {

                                // Altitude du satellite
                                sat.CalculLatitude(sat.position());
                                double altitude = sat.CalculAltitude(sat.position());

                                // Ecriture du resultat
                                if (ent == 0) {

                                    const QString nomsat = sat.tle().nom();

                                    if (nomsat.toLower() == "iss") {
                                        res.append("ISS");
                                    } else {
                                        ligne = nomsat;
                                        if (nomsat.contains("R/B") || nomsat.contains(" DEB"))
                                            ligne = ligne.append(QObject::tr("  (numéro NORAD : %1)")).
                                                    arg(sat.tle().norad());
                                        res.append(ligne);
                                    }

                                    res.append(QObject::tr("   Date      Heure   Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const" \
                                                           " Magn  Altitude  Distance  Az Soleil   Haut Soleil"));
                                    ent = 1;
                                }

                                // Calcul de la date calendaire
                                const double offset = (conditions.ecart()) ?
                                            conditions.offset() :
                                            Date::CalculOffsetUTC(Date(date.jourJulien(), conditions.offset()).ToQDateTime(1));

                                const Date date2(date.jourJulien() + EPS_DATES, offset);

                                // Coordonnees topocentriques du satellite
                                const QString az = Maths::ToSexagesimal(sat.azimut(), DEGRE, 3, 0, false, false);
                                const QString ht = Maths::ToSexagesimal(sat.hauteur(), DEGRE, 2, 0, false, false);;

                                // Coordonnees equatoriales du satellite
                                const QString ad = Maths::ToSexagesimal(sat.ascensionDroite(), HEURE1, 2, 0, false, false);
                                const QString de = Maths::ToSexagesimal(sat.declinaison(), DEGRE, 2, 0, true, false);

                                // Magnitude
                                const double mag = magn.magnitude();
                                QString magnSat;
                                if (mag > 98.) {
                                    magnSat = (conditions.ecl() || sat.magnitudeStandard() > 98.) ?
                                                " ????  " : " ----  ";
                                } else {
                                    const QString fmagn = "%1%2%3%4";
                                    const QString esp = (mag < 9.95) ? " " : "";
                                    const QString signe = (mag >= 0.) ? "+" : "-";
                                    const QString pen = (condEcl.isEclipsePartielle() || condEcl.isEclipseAnnulaire()) ? "* " : "  ";
                                    magnSat = fmagn.arg(esp).arg(signe).arg(fabs(mag), 0, 'f', 1, QChar('0')).arg(pen);
                                }

                                // Altitude du satellite et distance a l'observateur
                                double distance = sat.distance();
                                if (conditions.unite() == QObject::tr("nmi")) {
                                    altitude *= MILE_PAR_KM;
                                    distance *= MILE_PAR_KM;
                                }

                                // Coordonnees topocentriques du Soleil
                                const QString azs = Maths::ToSexagesimal(soleil.azimut(), DEGRE, 3, 0, false, false);
                                const QString hts = Maths::ToSexagesimal(soleil.hauteur(), DEGRE, 2, 0, true, false);

                                const QString result
                                        (fmt.arg(date2.ToShortDateAMJ(FORMAT_COURT, (conditions.syst()) ? SYSTEME_24H : SYSTEME_12H)).
                                         arg(az).arg(ht).arg(ad).arg(de).arg(sat.constellation()).arg(magnSat).
                                         arg(altitude, 8, 'f', 1).arg(distance, 9, 'f', 1).arg(azs).arg(hts).
                                         arg(sat.tle().norad()));

                                res.append(result);
                                pass = true;
                            } else {
                                if (pass) {
                                    pass = false;
                                    res.append("");
                                }
                            }

                            // Calcul pour le pas suivant
                            date = Date(date.jourJulienUTC() + conditions.pas0(), 0., false);
                            observateur.CalculPosVit(date);
                            soleil.CalculPosition(date);
                            soleil.CalculCoordHoriz(observateur);

                            if (soleil.hauteur() > conditions.crep() ||
                                    date.jourJulienUTC() > conditions.jj2() + conditions.pas0()) {
                                afin = true;
                            } else {

                                sat.CalculPosVit(date);
                                sat.CalculCoordHoriz(observateur);
                                if (sat.hauteur() < conditions.haut()) {
                                    afin = true;
                                } else {
                                    if (conditions.acalcEclipseLune())
                                        lune.CalculPosition(date);
                                    condEcl.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(),
                                                                                  conditions.refr());
                                    magn.Calcul(observateur, condEcl, sat.distance(), sat.hauteur(), sat.magnitudeStandard(),
                                                conditions.ext(), conditions.effetEclipsePartielle());
                                    sat.CalculCoordEquat(observateur);
                                }
                            }
                        } // fin while (!afin)

                        ent = 2;
                        if (pass) {
                            pass = false;
                            res.append("");
                        }
                        date = Date(date.jourJulienUTC() + periode, 0., false);

                        // Recherche de la nouvelle date dans le tableau d'ephemerides
                        bool atrouve = false;
                        while (it3.hasNext() && !atrouve) {
                            const double jj = it3.next().at(0);
                            if (jj >= date.jourJulienUTC()) {
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

    // Ouverture du fichier de resultat
    QFile fichier(conditions.out());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    if (res.count() > 0) {
        int i = 0;
        while (i < res.count()) {
            const QString lig = res.at(i);
            flux << ((lig.startsWith(' ')) ? lig : lig.mid(0, 119)) << endl;
            i++;
        }
    }

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
    Date date(conditions.jj1(), 0., false);
    do {

        // Position ECI de l'observateur
        observateur.CalculPosVit(date);

        // Position ECI du Soleil
        soleil.CalculPosition(date);

        // Position topocentrique du Soleil
        soleil.CalculCoordHoriz(observateur, false);

        if (soleil.hauteur() <= conditions.crep()) {

            svis = false;
            tab.clear();

            // Remplissage du tableau d'ephemerides
            tab.push_back(date.jourJulienUTC());

            tab.push_back(observateur.position().x());
            tab.push_back(observateur.position().y());
            tab.push_back(observateur.position().z());
            tab.push_back(observateur.rotHz().vecteur1().x());
            tab.push_back(observateur.rotHz().vecteur1().y());
            tab.push_back(observateur.rotHz().vecteur1().z());
            tab.push_back(observateur.rotHz().vecteur2().x());
            tab.push_back(observateur.rotHz().vecteur2().y());
            tab.push_back(observateur.rotHz().vecteur2().z());
            tab.push_back(observateur.rotHz().vecteur3().x());
            tab.push_back(observateur.rotHz().vecteur3().y());
            tab.push_back(observateur.rotHz().vecteur3().z());

            tab.push_back(soleil.position().x());
            tab.push_back(soleil.position().y());
            tab.push_back(soleil.position().z());

            tabEphem.append(tab);

        } else {
            if (!svis) {
                svis = true;
                if (conditions.crep() <= EPSDBL100)
                    date = Date(date.jourJulienUTC() + 0.375, 0., false);
            }
        }

        date = Date(date.jourJulienUTC() + pas, 0., false);
    } while (date.jourJulienUTC() <= conditions.jj2());

    tab.clear();

    /* Retour */
    return;
}
