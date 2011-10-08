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
 * >    iridium.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des flashs Iridium
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    17 juillet 2011
 *
 * Date de revision
 * >
 *
 */

#include <math.h>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include "iridium.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/messages.h"
#include "librairies/exceptions/previsatexception.h"

// Pas de calcul ou d'interpolation
static const double PAS0 = 2. * NB_JOUR_PAR_MIN;
static const double PAS1 = 30. * NB_JOUR_PAR_SEC;
static const double PAS_INT0 = 10. * NB_JOUR_PAR_SEC;
static const double PAS_INT1 = 2. * NB_JOUR_PAR_SEC;
static const double TEMPS1 = 16. * NB_JOUR_PAR_MIN;
static const double TEMPS2 = 76. * NB_JOUR_PAR_MIN;

// Donnees sur la geometrie des panneaux
static const double AIRE_MMA = 1.88 * 0.86 * 1.e-6;
static const double PHI = -40. * DEG2RAD;
static const double COSPHI = cos(PHI);
static const double SINPHI = sin(PHI);

// Nom et numeros des panneaux
static const QByteArray LISTE_MIR = QObject::tr("ADG").toAscii();
static const int LISTE_PAN[] = {0, 1, 2};

// Pour le calcul de l'assombrissement sur le bord pour la magnitude du Soleil
static const double TAB_INT[] = {0.3, 0.93, -0.23};

static char _mir;
static int _pan;
static Vecteur3D _solsat;
static Matrice _PR;
static QStringList _tabStsIri;

/*
 * Calcul des flashs Iridium
 */
void Iridium::CalculFlashsIridium(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    bool vis;
    int i, nb;
    QString ligne;
    QStringList listeSatellites, res;
    QTime tps;
    QList<Satellite> sats;
    QVector<TLE> tabtle;
    QList<QVector<double > > tabEphem;

    /* Initialisations */
    vis = false;
    i = 0;
    nb = 0;

    // Lecture du fichier de statut des satellites Iridium
    nb = LectureStatutIridium(conditions.getOpe());
    if (nb == 0)
        throw PreviSatException(QObject::tr("IRIDIUM : Erreur rencontrée lors de l'exécution\nAucun satellite Iridium susceptible de produire des flashs dans le fichier de statut"), Messages::WARNING);

    // Creation de la liste de satellites
    QStringListIterator it1(_tabStsIri);
    while (it1.hasNext()) {
        listeSatellites.append(it1.next().mid(4, 5));
    }

    // Verification du fichier TLE
    if (TLE::VerifieFichier(conditions.getFic(), false) == 0) {
        QString msg = QObject::tr("IRIDIUM : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE");
        msg = msg.arg(conditions.getFic());
        throw PreviSatException(msg, Messages::WARNING);
    }

    // Lecture du fichier TLE
    TLE::LectureFichier(conditions.getFic(), listeSatellites, tabtle);

    // Mise a jour de la liste de satellites et creation du tableau de satellites
    listeSatellites.clear();
    QVectorIterator<TLE> it2(tabtle);
    QVector<TLE> tabtle2;
    while (it2.hasNext()) {
        TLE tle = it2.next();
        if (tle.getNorad() == "") {
            _tabStsIri.removeAt(i);
        } else {
            sats.append(Satellite(tle));
            listeSatellites.append(tle.getNorad());
            tabtle2.append(tle);
            i++;
        }
    }

    // Il n'y a aucun satellite Iridium dans le fichier TLE
    if (listeSatellites.size() == 0)
        throw PreviSatException(QObject::tr("IRIDIUM : Erreur rencontrée lors de l'exécution\nAucun satellite Iridium n'a été trouvé dans le fichier TLE"), Messages::WARNING);

    // Ecriture de l'entete du fichier de previsions
    Conditions::EcrireEntete(observateur, conditions, tabtle2, false);

    /* Corps de la methode */
    tps.start();

    // Calcul des ephemerides du Soleil et du lieu d'observation
    CalculEphemSoleilObservateur(conditions, observateur, tabEphem);

    QListIterator<QVector<double> > it3(tabEphem);

    // Boucle sur les satellites
    it1 = QStringListIterator(_tabStsIri);
    QListIterator<Satellite> it4(sats);
    while (it4.hasNext()) {

        Satellite sat = it4.next();
        QString sts = it1.next();

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
            Observateur obs = Observateur(obsPos, Vecteur3D(), mat, observateur.getAaer(), observateur.getAray());

            // Position ECI du Soleil
            const Vecteur3D solPos = Vecteur3D(list.at(13), list.at(14), list.at(15));
            Soleil soleil = Soleil(solPos);

            // Position du satellite
            sat.CalculPosVit(date);
            sat.CalculCoordHoriz(obs, false);

            // Le satellite a une hauteur superieure a celle specifiee par l'utilisateur
            if (sat.getHauteur() >= conditions.getHaut()) {

                vis = true;
                double temp = -DATE_INFINIE;

                // Determination de la condition d'eclipse du satellite
                sat.CalculSatelliteEclipse(soleil);

                // Le satellite n'est pas eclipse
                if (!sat.isEclipse()) {

                    double angref = PI;
                    double pas = PAS0;
                    double minmax[2], jjm[3];

                    double jj0 = date.getJourJulienUTC();
                    double jj2 = jj0 + TEMPS1;
                    do {

                        // Calcul de l'angle de reflexion
                        _pan = -1;
                        angref = AngleReflexion(sat, soleil);

                        pas = (angref < 0.5) ? PAS1 : PAS0;

                        if (angref <= 0.2) {

                            jjm[0] = jj0 - NB_JOUR_PAR_MIN;
                            jjm[1] = jj0;
                            jjm[2] = jj0 + NB_JOUR_PAR_MIN;

                            // Calcul par interpolation de l'instant correspondant a l'angle de reflexion minimum
                            CalculAngleMin(sat, observateur, soleil, jjm, minmax);


                            // Iterations supplementaires pour affiner la date du maximum
                            double pasInt = PAS_INT0;
                            for (int it=0; it<4; it++) {

                                jjm[0] = minmax[0] - pasInt;
                                jjm[1] = minmax[0];
                                jjm[2] = minmax[0] + pasInt;

                                CalculAngleMin(sat, observateur, soleil, jjm, minmax);
                                pasInt *= 0.5;
                            }

                            if (minmax[0] - temp > PAS_INT1) {

                                Date date2 = Date(minmax[0], 0., false);

                                observateur.CalculPosVit(date2);

                                // Position du satellite
                                sat.CalculPosVit(date2);
                                sat.CalculCoordHoriz(observateur, false);

                                if (sat.getHauteur() >= 0.) {

                                    // Position du Soleil
                                    soleil.CalculPosition(date2);
                                    soleil.CalculCoordHoriz(observateur, false);

                                    double mgn0 = (soleil.getHauteur() < conditions.getCrep()) ? conditions.getMgn1() : conditions.getMgn2();

                                    // Magnitude du flash
                                    double mag = MagnitudeFlash(sat, observateur, soleil, minmax[1], conditions.getExt());

                                    if (mag <= mgn0) {

                                        Date dates[conditions.getNbl()];

                                        // Calcul des limites du flash
                                        CalculLimitesFlash(sat, observateur, soleil, conditions, minmax[0], mgn0, dates);

                                        if (dates[conditions.getNbl() / 2].getJourJulienUTC() < DATE_INFINIE) {

                                            temp = minmax[0];
                                            Observateur obsmax;

                                            // Calcul des valeurs exactes pour les differentes dates
                                            _pan = -1;
                                            for (i=0; i<conditions.getNbl(); i++) {

                                                observateur.CalculPosVit(dates[i]);

                                                // Position du satellite
                                                sat.CalculPosVit(dates[i]);
                                                sat.CalculCoordHoriz(observateur);

                                                // Position du Soleil
                                                soleil.CalculPosition(dates[i]);
                                                soleil.CalculCoordHoriz(observateur);

                                                // Condition d'eclipse du satellite
                                                sat.CalculSatelliteEclipse(soleil);

                                                // Angle de reflexion
                                                angref = AngleReflexion(sat, soleil);

                                                // Magnitude du flash
                                                mag = MagnitudeFlash(sat, observateur, soleil, angref, conditions.getExt());

                                                if (angref > conditions.getAng0() + 1.e-3 || mag > mgn0 + 0.01) {
                                                    if (res.count() % conditions.getNbl() != 0)
                                                        res.removeLast();
                                                    if (res.count() % conditions.getNbl() != 0)
                                                        res.removeLast();
                                                }

                                                // Ascension droite/declinaison/constellation
                                                sat.CalculCoordEquat(observateur);

                                                // Altitude du satellite
                                                double altitude, ct, latitude, phi;
                                                Vecteur3D position = sat.getPosition();
                                                const double r = sqrt(position.getX() * position.getX() + position.getY() * position.getY());
                                                latitude = atan(position.getZ() / r);
                                                do {
                                                    phi = latitude;
                                                    double sph = sin(phi);
                                                    ct = 1. / sqrt(1. - E2 * sph * sph);
                                                    latitude = atan((position.getZ() + RAYON_TERRESTRE * ct * E2 * sph) / r);
                                                } while (fabs(latitude - phi) > 1.e-7);
                                                altitude = r / cos(latitude) - RAYON_TERRESTRE * ct;

                                                // Ecriture du flash

                                                // Date calendaire
                                                Date date3 = Date(dates[i].getJourJulienUTC() + conditions.getDtu() + EPS_DATES, 0., true);
                                                ligne = date3.ToShortDate(Date::LONG);

                                                // Coordonnees topocentriques
                                                ligne = ligne.append(Maths::ToSexagesimal(sat.getAzimut(), Maths::DEGRE, 3, 0, false ,false)).append(" ");
                                                ligne = ligne.append(Maths::ToSexagesimal(sat.getHauteur(), Maths::DEGRE, 2, 0, false ,false)).append(" ");

                                                // Coordonnees equatoriales
                                                ligne = ligne.append(Maths::ToSexagesimal(sat.getAscensionDroite(), Maths::HEURE1, 2, 0, false ,false)).append(" ");
                                                ligne = ligne.append(Maths::ToSexagesimal(sat.getDeclinaison(), Maths::DEGRE, 2, 0, true ,false)).append(" ");

                                                ligne = ligne.append(sat.getConstellation()).append(" ");

                                                // Angle de reflexion, miroir
                                                ligne = ligne.append("%1  %2  ");
                                                ligne = ligne.arg(angref * RAD2DEG, 4, 'f', 2).arg(_mir);

                                                // Magnitude
                                                ligne = ligne.append((mag > 0.) ? "+%1" : "-%1");
                                                ligne = ligne.arg(fabs(mag), 2, 'f', 1);
                                                ligne = ligne.append((sat.isPenombre()) ? "*" : " ");

                                                // Altitude du satellite et distance a l'observateur
                                                ligne = ligne.append("%1 %2");
                                                double distance = sat.getDistance();
                                                if (conditions.getUnite() == QObject::tr("mi")) {
                                                    altitude *= MILE_PAR_KM;
                                                    distance *= MILE_PAR_KM;
                                                }
                                                ligne = ligne.arg(altitude, 6, 'f', 1).arg(distance, 6, 'f', 1);

                                                // Coordonnees topocentriques du Soleil
                                                ligne = ligne.append(Maths::ToSexagesimal(soleil.getAzimut(), Maths::DEGRE, 3, 0, false ,false)).append(" ");
                                                ligne = ligne.append(Maths::ToSexagesimal(soleil.getHauteur(), Maths::DEGRE, 2, 0, true ,false));
                                                ligne = ligne.append(QString::number(i));

                                                // Recherche des coordonnees geographiques ou se produit le maximum du flash
                                                if (i == conditions.getNbl() / 2) {

                                                    Vecteur3D direction = _PR.Transposee() * _solsat;
                                                    obsmax = Observateur::CalculIntersectionEllipsoide(dates[i], position, direction);
                                                    if (obsmax.getNomlieu() != "") {

                                                        obsmax.CalculPosVit(dates[i]);
                                                        sat.CalculCoordHoriz(obsmax, false);

                                                        // Distance entre les 2 lieux d'observation
                                                        const double distanceObs = observateur.CalculDistance(obsmax);
                                                        double diff = obsmax.getLongitude() - observateur.getLongitude();
                                                        if (fabs(diff) > PI)
                                                            diff -= Maths::sgn(diff) * PI;
                                                        const QString dir = (diff > 0) ? QObject::tr("(W)") : QObject::tr("(E)");

                                                        // Angle de reflexion pour le lieu du maximum
                                                        const double angRefMax = AngleReflexion(sat, soleil);

                                                        // Magnitude du flash
                                                        const double magFlashMax = MagnitudeFlash(sat, obsmax, soleil, angRefMax, conditions.getExt());

                                                        const QString ew = (obsmax.getLongitude() >= 0.) ? QObject::tr("W") : QObject::tr("E");
                                                        const QString ns = (obsmax.getLatitude() >= 0.) ? QObject::tr("N") : QObject::tr("S");

                                                        // Ecriture de la chaine de caracteres
                                                        ligne = ligne.append("   %1 %2  %3 %4  %5 %6    ").append((magFlashMax > 0.) ? "+%7" : "-%7");
                                                        ligne = ligne.arg(fabs(obsmax.getLongitude() * RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew).arg(fabs(obsmax.getLatitude() * RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns).arg(distanceObs, 5, 'f', 1).arg(dir).arg(fabs(magFlashMax), 2, 'f', 1).append((sat.isPenombre()) ? "*" : "");
                                                    }
                                                }

                                                // Numero Iridium
                                                if (sts.length() == 9) {
                                                    ligne = ligne.append(sts.mid(0, 3)).append(" ");
                                                } else {
                                                    ligne = ligne.append(sts.mid(0, 3)).trimmed().append("? ");
                                                }

                                                res.append(ligne);
                                            } // fin for
                                        }
                                    }
                                }
                            }
                        } // fin if (angref <= 0.2)

                        jj0 += pas;
                        Date date0 = Date(jj0, 0., false);

                        observateur.CalculPosVit(date0);

                        // Position du satellite
                        sat.CalculPosVit(date0);
                        sat.CalculCoordHoriz(observateur, false);

                        // Position du Soleil
                        soleil.CalculPosition(date0);

                        // Condition d'eclipse du satellite
                        sat.CalculSatelliteEclipse(soleil);

                        if (sat.isEclipse() || sat.getHauteur() < conditions.getHaut())
                            jj0 = jj2 + PAS0;
                    } while (jj0 <= jj2);
                    date = Date(jj0 + TEMPS2, 0., false);
                }
                date = Date(date.getJourJulienUTC() + PAS0, 0., false);

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
    int fin = tps.elapsed();

    // Tri chronologique
    if (conditions.getChr())
        res.sort();

    // Ecriture des resultats dans le fichier de previsions
    QFile fichier(conditions.getOut());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    if (res.count() > 0) {
        flux << QObject::tr("Ir     Date      Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Cst Ang  Mir Magn   Alt   Dist  Az Soleil  Haut Soleil   Long Max    Lat Max    Distance  Magn Max") << endl;

        i = 0;
        while (i < res.count()) {
            for (int j=0; j<conditions.getNbl(); j++) {
                ligne = res.at(i);
                flux << ligne.mid(ligne.length() - 4) << ligne.mid(0, ligne.length() - 4).remove(119, 1) << endl;
                i++;
            }
            flux << endl;
        }
    }

    ligne = QObject::tr("Temps écoulé : %1s");
    ligne = ligne.arg(1.e-3 * fin, 0, 'f', 2);
    flux << ligne << endl;
    fichier.close();

    /* Retour */
    return;
}

void Iridium::CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur, QList<QVector<double> > &tabEphem)
{
    /* Declarations des variables locales */
    int i;
    QVector<double> tab;
    Soleil soleil;

    /* Initialisations */
    i = 0;

    /* Corps de la methode */
    Date date = Date(conditions.getJj1(), 0., false);
    do {

        // Position ECI de l'observateur
        observateur.CalculPosVit(date);

        // Position ECI du Soleil
        soleil.CalculPosition(date);

        // Position topocentrique du Soleil
        soleil.CalculCoordHoriz(observateur, false);

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

        date = Date(date.getJourJulienUTC() + PAS0, 0., false);
    } while (date.getJourJulienUTC() <= conditions.getJj2());

    /* Retour */
    return;
}

double Iridium::AngleReflexion(const Satellite &satellite, const Soleil &soleil)
{
    /* Declarations des variables locales */
    double ang;

    /* Initialisations */
    ang = PI;

    /* Corps de la methode */
    Vecteur3D xx = satellite.getVitesse().Normalise();
    Vecteur3D yy = satellite.getPosition().Normalise() ^ xx;
    Vecteur3D zz = xx ^ yy;

    // Matrice de passage ECI geocentrique -> ECI satellite
    Matrice P = Matrice(xx, yy, zz);

    int imin, imax;
    if (_pan == -1) {
        imin = 0;
        imax = 3;
    } else {
        imin = _pan;
        imax = _pan + 1;
    }

    int j = 0;
    for (int i=imin; i<imax; i++) {

        const double psi = i * DEUX_TIERS * PI;
        const double cospsi = cos(psi);
        const double sinpsi = sin(psi);

        const Vecteur3D v1(COSPHI * cospsi, -COSPHI * sinpsi, SINPHI);
        const Vecteur3D v2(sinpsi, cospsi, 0.);
        const Vecteur3D v3(-SINPHI * cospsi, SINPHI * sinpsi, COSPHI);

        // Matrice de rotation repere satellite -> repere panneau
        Matrice R(v1, v2, v3);

        // Matrice produit P x R
        _PR = (P * R).Transposee();

        // Position observateur dans le repere panneau
        Vecteur3D tmp = -satellite.getDist();
        Vecteur3D obsat = _PR * tmp;

        // Position Soleil dans le repere panneau
        tmp = soleil.getPosition() - satellite.getPosition();
        _solsat = _PR * tmp;

        // Position du reflet du Soleil
        _solsat = Vecteur3D(_solsat.getX(), -_solsat.getY(), -_solsat.getZ());

        // Angle de reflexion
        const double temp = obsat.Angle(_solsat);
        if (_pan == -1) {
            if (temp < ang) {
                ang = temp;
                j = i;
                _mir = LISTE_MIR[i];
            }
        } else {
            ang = temp;
        }
    }

    if (_pan == -1)
        _pan = LISTE_PAN[j];

    /* Retour */
    return (ang);
}

void Iridium::CalculAngleMin(Satellite &satellite, Observateur &observateur, Soleil &soleil, const double jjm[], double minmax[])
{
    /* Declarations des variables locales */
    double ang[3];
    Date date;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        date = Date(jjm[i], 0., false);

        observateur.CalculPosVit(date);

        // Position du satellite
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(observateur, false);

        // Position du Soleil
        soleil.CalculPosition(date);

        // Angle de reflexion
        ang[i] = AngleReflexion(satellite, soleil);
    }

    // Determination du minimum par interpolation
    Maths::CalculExtremumInterpolation3(jjm, ang, minmax);

    /* Retour */
    return;
}

void Iridium::CalculLimitesFlash(Satellite &satellite, Observateur &observateur, Soleil &soleil, const Conditions &conditions, const double dateMaxFlash, const double mgn0, Date lim[])
{
    /* Declarations des variables locales */
    double tmp;
    double jjm[3], limite[4], lim0[4];

    /* Initialisations */
    double dateInf = -DATE_INFINIE;
    double dateSup = DATE_INFINIE;
    double jj0 = dateMaxFlash - PAS_INT0;
    double jj2 = dateMaxFlash + PAS_INT0;

    /* Corps de la methode */
    // Determination de la date inferieure du flash
    jjm[0] = jj0 - PAS1;
    jjm[1] = 0.5 * (dateMaxFlash + jj0 - PAS1);
    jjm[2] = dateMaxFlash;

    LimiteFlash(satellite, observateur, soleil, conditions, jjm, mgn0, limite);

    for (int i=0; i<4; i++) {
        lim0[i] = limite[i];
        if (lim0[i] < DATE_INFINIE) {

            int it = 0;
            double pasInt = PAS_INT1;
            do {
                it++;
                tmp = lim0[i];
                jjm[0] = lim0[i] - pasInt;
                jjm[1] = lim0[i];
                jjm[2] = lim0[i] + pasInt;

                LimiteFlash(satellite, observateur, soleil, conditions, jjm, mgn0, lim0);
                pasInt *= 0.5;
            } while (fabs(lim0[i] - tmp) > EPS_DATES && lim0[i] < DATE_INFINIE && it < 10);

            if (lim0[i] < DATE_INFINIE && it < 10)
                limite[i] = lim0[i];
        } else {
            limite[i] = -DATE_INFINIE;
        }
    }

    dateInf = Maths::max(limite[0], limite[1]);
    dateInf = Maths::max(dateInf, limite[2]);
    dateInf = Maths::max(dateInf, limite[3]);


    // Determination de la date superieure du flash
    jjm[0] = dateMaxFlash;
    jjm[1] = 0.5 * (dateMaxFlash + jj2);
    jjm[2] = jj2;

    LimiteFlash(satellite, observateur, soleil, conditions, jjm, mgn0, limite);

    for (int i=0; i<4; i++) {
        lim0[i] = limite[i];
        if (lim0[i] < DATE_INFINIE) {

            int it = 0;
            double pasInt = PAS_INT1;
            do {
                it++;
                tmp = lim0[i];
                jjm[0] = lim0[i] - pasInt;
                jjm[1] = lim0[i];
                jjm[2] = lim0[i] + pasInt;

                LimiteFlash(satellite, observateur, soleil, conditions, jjm, mgn0, lim0);
                pasInt *= 0.5;
            } while (fabs(lim0[i] - tmp) > EPS_DATES && lim0[i] < DATE_INFINIE && it < 10);

            if (lim0[i] < DATE_INFINIE && it < 10)
                limite[i] = lim0[i];
        } else {
            limite[i] = DATE_INFINIE;
        }
    }

    dateSup = Maths::min(limite[0], limite[1]);
    dateSup = Maths::min(dateSup, limite[2]);
    dateSup = Maths::min(dateSup, limite[3]);

    jjm[0] = dateInf;
    jjm[1] = 0.5 * (dateInf + dateSup);
    jjm[2] = dateSup;

    double minmax[2];
    CalculAngleMin(satellite, observateur, soleil, jjm, minmax);
    double dateMax = minmax[0];

    if (dateInf < dateSup - EPS_DATES) {
        if (dateMax < dateInf)
            dateMax = dateInf;
        if (dateMax > dateSup)
            dateMax = dateSup;

        lim[conditions.getNbl() / 2] = Date(dateMax, 0., false);
        if (conditions.getNbl() == 3) {
            lim[0] = Date(dateInf, 0., false);
            lim[2] = Date(dateSup, 0., false);
        }
    } else {
        lim[conditions.getNbl() / 2] = Date(DATE_INFINIE, 0., false);
    }

    /* Retour */
    return;
}

void Iridium::LimiteFlash(Satellite satellite, Observateur observateur, Soleil soleil, const Conditions conditions, const double jjm[], const double mgn0, double limite[])
{
    /* Declarations des variables locales */
    double ang[3], ecl[3], ht[3], mag[3];
    Date date;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        date = Date(jjm[i], 0., false);

        observateur.CalculPosVit(date);

        // Position du satellite
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(observateur, false);;
        ht[i] = satellite.getHauteur();

        // Position du Soleil
        soleil.CalculPosition(date);

        // Conditions d'eclipse du satellite
        satellite.CalculSatelliteEclipse(soleil);
        ecl[i] = satellite.getRayonApparentTerre() - satellite.getRayonApparentSoleil() - satellite.getElongation();

        // Angle de reflexion
        ang[i] = AngleReflexion(satellite, soleil);

        // Magnitude du satellite
        mag[i] = MagnitudeFlash(satellite, observateur, soleil, ang[i], conditions.getExt());
    }

    double t_ang, t_ecl, t_ht, t_mag;
    // Calcul par interpolation de la date pour laquelle la magnitude est egale a la magnitude specifiee par l'utilisateur
    t_mag = Maths::CalculValeurXInterpolation3(jjm, mag, mgn0, EPS_DATES);

    // Calcul par interpolation de la date pour laquelle l'angle de reflexion est egal a l'angle de reflexion specifie par l'utilisateur
    t_ang = Maths::CalculValeurXInterpolation3(jjm, ang, conditions.getAng0(), EPS_DATES);

    // Calcul par interpolation de la date pour laquelle la hauteur est egale a la hauteur specifie par l'utilisateur
    if ((ht[0] - conditions.getHaut()) * (ht[0] - conditions.getHaut()) < 0. || (ht[0] < conditions.getHaut() && ht[2] < conditions.getHaut())) {
        t_ht = Maths::CalculValeurXInterpolation3(jjm, ht, conditions.getHaut(), EPS_DATES);
    } else {
        t_ht = DATE_INFINIE;
    }

    if ((ecl[0] * ecl[2] < 0.) || (ecl[0] > 0. && ecl[2] > 0.)) {
        t_ecl = Maths::CalculValeurXInterpolation3(jjm, ecl, 0., EPS_DATES);
    } else {
        t_ecl = DATE_INFINIE;
    }

    limite[0] = t_mag;
    limite[1] = t_ang;
    limite[2] = t_ht;
    limite[3] = t_ecl;

    /* Retour */
    return;
}

double Iridium::MagnitudeFlash(Satellite satellite, const Observateur observateur, const Soleil soleil, const double angle, const bool ext)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;
    const double omega = RAYON_SOLAIRE / (soleil.getDistanceUA() * UA);
    const double invDist3 = 1. / (satellite.getDistance() * satellite.getDistance() * satellite.getDistance());
    const double aireProjetee = fabs(satellite.getDist().getX()) * invDist3 * AIRE_MMA;

    /* Corps de la methode */
    if (angle < omega) {
        // Reflexion speculaire

        // Calcul de la magnitude du point du Soleil
        const double cosAngle = cos(angle);
        const double cosOmega = cos(omega);
        const double cosPsi = sqrt((cosAngle * cosAngle - cosOmega * cosOmega)) / sin(omega);
        double psiterm = 1.;
        double intens = 0.;
        for (int i=0; i<3; i++) {
            intens += TAB_INT[i] * psiterm;
            psiterm *= cosPsi;
        }

        const double magSol = MAGNITUDE_SOLEIL - 2.5 * log10(intens);

        // Correction due a la surface eclairee
        const double surface = PI * omega * omega;
        const double magCorr = -2.5 * log10(aireProjetee / surface);

        // Magnitude du flash
        magnitude = magSol + magCorr;

    } else {
        // Reflexion non speculaire

        // Magnitude standard (approche empirique)
        const double magnitudeStandard = 3.2 * log(angle * RAD2DEG) - 2.450012;
        magnitude = magnitudeStandard - 2.5 * log10(aireProjetee / 1.e-12);
    }

    // Prise en compte de l'extinction atmospherique
    if (ext)
        magnitude += satellite.ExtinctionAtmospherique(observateur);

    /* Retour */
    return (magnitude);
}

int Iridium::LectureStatutIridium(const char ope)
{
    /* Declarations des variables locales */
    int i;
    QString ligne;

    /* Initialisations */
    i = 0;

    /* Corps de la methode */
    QFile fichier("data/iridium.sts");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    while (!flux.atEnd()) {

        ligne = flux.readLine();
        if (ligne.size() == 9) {
            _tabStsIri.append(ligne);
            i++;
        } else {
            if (ligne.at(10) == '?' && ope == 'n') {
                _tabStsIri.append(ligne);
                i++;
            }
        }
    }
    fichier.close();

    /* Retour */
    return (i);
}
