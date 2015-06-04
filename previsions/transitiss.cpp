/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    transitiss.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des transits de l'ISS devant le Soleil et la Lune
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 juillet 2011
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QTime>
#pragma GCC diagnostic warning "-Wconversion"
#include "transitiss.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"

// Pas de calcul ou d'interpolation
static const double PAS0 = NB_JOUR_PAR_MIN;
static const double PAS1 = 10. * NB_JOUR_PAR_SEC;
static const double PAS_INT0 = 10. * NB_JOUR_PAR_SEC;

static QStringList res;
static QVector<TLE> tabtle;
static QVector<QList<QVector<double> > > tabEphem;

/*
 * Calcul des transits ISS devant la Lune et/ou le Soleil
 */
void TransitISS::CalculTransitsISS(const Conditions &conditions, Observateur &observateur, QStringList &result)
{
    /* Declarations des variables locales */
    double rayon = 0.;
    Soleil soleil;
    Lune lune;
    QTime tps;

    /* Initialisations */
    const double temps1 = 16. * NB_JOUR_PAR_MIN;
    const QString fmt = "%1%2 %3 %4 %5 %6 %7   %8    %9   %10 %11 %12%13  %14%15";
    const QStringList listeTLE("25544");

    // Lecture du TLE
    TLE::LectureFichier(conditions.fic(), listeTLE, tabtle);

    const double periode = 1. / tabtle.at(0).no() - temps1;

    // Ecriture de l'entete
    Conditions::EcrireEntete(observateur, conditions, tabtle, true);

    /* Corps de la methode */
    result.clear();
    tps.start();
    Satellite sat = Satellite(tabtle.at(0));

    // Generation des ephemerides du Soleil et de la Lune
    CalculEphemSoleilLune(conditions, observateur);

    // Boucle sur le tableau d'ephemerides
    QVectorIterator<QList<QVector<double> > > it1(tabEphem);
    while (it1.hasNext()) {

        QListIterator<QVector<double> > it2(it1.next());

        while (it2.hasNext()) {

            const QVector<double> list = it2.next();
            Date date(list.at(0), 0., false);

            // Donnees liees au lieu d'observation
            const Vecteur3D obsPos(list.at(1), list.at(2), list.at(3));
            const Vecteur3D v1(list.at(4), list.at(5), list.at(6));
            const Vecteur3D v2(list.at(7), list.at(8), list.at(9));
            const Vecteur3D v3(list.at(10), list.at(11), list.at(12));
            const Matrice3D mat(v1, v2, v3);
            const Observateur obs(obsPos, Vecteur3D(), mat, observateur.aaer(), observateur.aray());

            Corps corps;
            corps.setPosition(Vecteur3D(list.at(13), list.at(14), list.at(15)));
            int typeCorps = (int) (list.at(16) + EPSDBL100);

            // Position de l'ISS
            sat.CalculPosVit(date);
            sat.CalculCoordHoriz(obs, false);

            if (sat.hauteur() >= conditions.haut()) {

                double ang, ang0, jj0, jj2;

                jj0 = date.jourJulienUTC() - PAS0;
                jj2 = jj0 + temps1;
                ang0 = PI;

                do {
                    const Date date0(jj0, 0., false);

                    observateur.CalculPosVit(date0);

                    // Position de l'ISS
                    sat.CalculPosVit(date0);
                    sat.CalculCoordHoriz(observateur, false);

                    // Position du corps (Soleil ou Lune)
                    if (typeCorps == 1) {
                        soleil.CalculPosition(date0);
                        corps.setPosition(soleil.position());
                    }
                    if (typeCorps == 2) {
                        lune.CalculPosition(date0);
                        corps.setPosition(lune.position());
                    }
                    corps.CalculCoordHoriz(observateur, false);

                    // Calcul de l'angle ISS - observateur - corps
                    ang = corps.dist().Angle(sat.dist());
                    if (ang < ang0)
                        ang0 = ang;

                    jj0 += PAS1;
                } while (jj0 <= jj2 && ang < ang0 + EPSDBL100 && sat.hauteur() >= 0.);

                // Il y a une conjonction ou un transit : on determine l'angle de separation minimum
                if (jj0 <= jj2 - PAS1 && ang0 < conditions.seuilConjonction() + DEG2RAD) {

                    int it;
                    double jjm[3], minmax[2];

                    // Recherche de l'instant precis de l'angle minimum par interpolation
                    jj0 -= 2. * PAS1;
                    jjm[0] = jj0 - PAS1;
                    jjm[1] = jj0;
                    jjm[2] = jj0 + PAS1;

                    CalculAngleMin(sat, observateur, jjm, typeCorps, minmax);

                    // Iterations supplementaires pour affiner la date du minimum
                    it = 0;
                    double pasInt = PAS_INT0;
                    while (fabs(ang - minmax[1]) > 1.e-5 && it < 10) {

                        ang = minmax[1];
                        jjm[0] = minmax[0] - pasInt;
                        jjm[1] = minmax[0];
                        jjm[2] = minmax[0] + pasInt;

                        CalculAngleMin(sat, observateur, jjm, typeCorps, minmax);
                        pasInt *= 0.5;
                        it++;
                    }

                    Date date2(minmax[0], 0., false);

                    observateur.CalculPosVit(date2);

                    // Position de l'ISS
                    sat.CalculPosVit(date2);
                    sat.CalculCoordHoriz(observateur, false);

                    if (sat.hauteur() >= conditions.haut() && minmax[1] <= conditions.seuilConjonction()) {

                        Date dates[3];

                        // Position du corps (Soleil ou Lune)
                        soleil.CalculPosition(date2);
                        if (typeCorps == 1) {
                            corps.setPosition(soleil.position());
                            rayon = RAYON_SOLAIRE;
                        }
                        if (typeCorps == 2) {
                            lune.CalculPosition(date2);
                            corps.setPosition(lune.position());
                            rayon = RAYON_LUNAIRE;
                        }
                        corps.CalculCoordHoriz(observateur, false);

                        // Angle de separation
                        ang = corps.dist().Angle(sat.dist());

                        // Rayon apparent du corps
                        const double rayonApparent = asin(rayon / corps.distance());

                        const bool itr = (ang <= rayonApparent);
                        sat.CalculSatelliteEclipse(soleil, conditions.refr());

                        if (itr || (!itr && !sat.isEclipse())) {

                            // Calcul des dates extremes de la conjonction ou du transit
                            dates[1] = date2;
                            CalculElements(sat, observateur, minmax[0], typeCorps, itr,
                                           conditions.seuilConjonction(), dates);

                            // Recalcul de la position pour chacune des dates en vue de l'ecriture des resultats
                            QString transit = "";
                            for (int j=0; j<3; j++) {

                                observateur.CalculPosVit(dates[j]);

                                // Position de l'ISS
                                sat.CalculPosVit(dates[j]);
                                sat.CalculCoordHoriz(observateur);
                                sat.CalculCoordEquat(observateur);

                                // Altitude du satellite
                                const Vecteur3D position = sat.position();
                                sat.CalculLatitude(position);
                                double altitude = sat.CalculAltitude(sat.position());

                                // Position du Soleil
                                soleil.CalculPosition(dates[j]);
                                soleil.CalculCoordHoriz(observateur);
                                sat.CalculSatelliteEclipse(soleil, conditions.refr());

                                // Ecriture du resultat

                                // Date calendaire
                                const double offset = (conditions.ecart()) ?
                                            conditions.offset() :
                                            Date::CalculOffsetUTC(Date(dates[j].jourJulienUTC(), 0.).ToQDateTime(1));
                                const Date date3(dates[j].jourJulien() + offset + EPS_DATES, 0.);

                                // Coordonnees topocentriques du satellite
                                const QString az = Maths::ToSexagesimal(sat.azimut(), DEGRE, 3, 0, false, false);
                                const QString ht = Maths::ToSexagesimal(sat.hauteur(), DEGRE, 2, 0,  false, false);

                                // Coordonnees equatoriales du satellite
                                const QString ad = Maths::ToSexagesimal(sat.ascensionDroite(), HEURE1, 2, 0, false, false);
                                const QString de = Maths::ToSexagesimal(sat.declinaison(), DEGRE, 2, 0, true, false);

                                // Distance angulaire
                                ang = corps.dist().Angle(sat.dist()) * RAD2DEG;

                                // Altitude du satellite et distance a l'observateur
                                double distance = sat.distance();
                                if (conditions.unite() == QObject::tr("nmi")) {
                                    altitude *= MILE_PAR_KM;
                                    distance *= MILE_PAR_KM;
                                }

                                // Coordonnees topocentriques du Soleil
                                const QString azs = Maths::ToSexagesimal(soleil.azimut(), DEGRE, 3, 0, false, false);
                                const QString hts = Maths::ToSexagesimal(soleil.hauteur(), DEGRE, 2, 0, true, false);

                                QString resultat = fmt.
                                        arg(date3.ToShortDateAMJ(FORMAT_LONG, (conditions.syst()) ? SYSTEME_24H : SYSTEME_12H)).
                                        arg(az).arg(ht).arg(ad).arg(de).arg(sat.constellation()).arg(ang, 5, 'f', 2).
                                        arg((itr) ? QObject::tr("T") : QObject::tr("C")).
                                        arg((typeCorps == 1) ? QObject::tr("S") : QObject::tr("L")).
                                        arg((sat.isEclipse()) ? QObject::tr("Omb") : (sat.isPenombre()) ?
                                                                    QObject::tr("Pen") : QObject::tr("Lum")).
                                        arg(altitude, 6, 'f', 1).arg(distance, 6, 'f', 1).arg(azs).arg(hts);

                                // Recherche du maximum (transit ou conjonction)
                                QString max(35, ' ');
                                const Vecteur3D direction = corps.dist() - sat.dist();
                                Observateur obsmin =
                                        Observateur::CalculIntersectionEllipsoide(dates[j], position, direction);

                                if (!obsmin.nomlieu().isEmpty()) {
                                    obsmin.CalculPosVit(dates[j]);
                                    sat.CalculCoordHoriz(obsmin, false);

                                    if (typeCorps == 1) {
                                        soleil.CalculPosition(dates[j]);
                                        corps.setPosition(soleil.position());
                                        rayon = RAYON_SOLAIRE;
                                    }
                                    if (typeCorps == 2) {
                                        lune.CalculPosition(dates[j]);
                                        corps.setPosition(lune.position());
                                        rayon = RAYON_LUNAIRE;
                                    }
                                    corps.CalculCoordHoriz(obsmin, false);

                                    const double distanceObs = observateur.CalculDistance(obsmin);
                                    const double cap = observateur.CalculCap(obsmin) * RAD2DEG;

                                    QString dir;
                                    if (cap >= 315. || cap < 45.)
                                        dir = QObject::tr("(N)");
                                    if (cap >= 45. && cap < 135.)
                                        dir = QObject::tr("(E)");
                                    if (cap >= 135. && cap < 225.)
                                        dir = QObject::tr("(S)");
                                    if (cap >= 225. && cap < 315.)
                                        dir = QObject::tr("(W)");

                                    const QString ew = (obsmin.longitude() >= 0.) ? QObject::tr("W") : QObject::tr("E");
                                    const QString ns = (obsmin.latitude() >= 0.) ? QObject::tr("N") : QObject::tr("S");

                                    // Ecriture de la chaine de caracteres
                                    const QString fmt2 = "   %1 %2  %3 %4  %5 %6";
                                    max = fmt2.arg(fabs(obsmin.longitude() * RAD2DEG), 8, 'f', 4,QChar('0')).
                                            arg(ew).arg(fabs(obsmin.latitude() * RAD2DEG), 7, 'f', 4, QChar('0')).
                                            arg(ns).arg(distanceObs, 5, 'f', 1).arg(dir);
                                }
                                transit.append(resultat.arg(max));
                            }
                            res.append(transit);
                        }
                    }
                    date = Date(jj2, 0., false);
                } else {
                    if (sat.hauteur() < conditions.haut())
                        date = Date(date.jourJulienUTC() + periode, 0., false);
                }
                date = Date(date.jourJulienUTC() + PAS0, 0., false);

                // Recherche de la nouvelle date dans le tableau d'ephemerides
                bool atrouve = false;
                while (it2.hasNext() && !atrouve) {
                    const double jj = it2.next().at(0);
                    if (jj >= date.jourJulienUTC()) {
                        atrouve = true;
                        it2.previous();
                    }
                }
            }
        }
    }
    int fin = tps.elapsed();

    // Ecriture des resultats dans le fichier de previsions
    res.sort();
    QFile fichier(conditions.out());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    QString ligne;
    if (res.size() > 0) {

        ligne = QObject::tr("   Date       Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Cst  Ang  Type Corps Ill   Alt   Dist" \
                            "  Az Soleil  Haut Soleil    Long Max    Lat Max   Distance");
        result.append(ligne);
        flux << ligne << endl;

        int i = 0;
        while (i < res.count()) {

            const QString ligne0 = res.at(i);
            const QString ligne1 = ligne0.mid(0, 163).trimmed();
            const QString ligne2 = ligne0.mid(163, 163).trimmed();
            const QString ligne3 = ligne0.mid(326, 163).trimmed();
            const QString transit = ligne1 + "\n" + ligne2 + "\n" + ligne3;
            flux << transit << endl;
            flux << endl;

            result.append(ligne1);
            result.append(ligne2);
            result.append(ligne3);
            result.append("");
            i++;
        }
        if (res.count() > 0)
            if (!res.at(res.count() - 1).isEmpty())
                flux << endl;
    }

    ligne = QObject::tr("Temps écoulé : %1s");
    ligne = ligne.arg(1.e-3 * fin, 0, 'f', 2);
    flux << ligne << endl;
    fichier.close();
    FinTraitement();

    /* Retour */
    return;
}

void TransitISS::FinTraitement()
{
    res.clear();
    tabtle.clear();
    tabEphem.clear();
}

/*
 * Calcul de l'angle minimum
 */
void TransitISS::CalculAngleMin(Satellite &satellite, Observateur &observateur, const double jjm[], const int typeCorps,
                                double minmax[])
{
    /* Declarations des variables locales */
    double angle[3];
    Corps corps;
    Soleil soleil;
    Lune lune;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        observateur.CalculPosVit(date);

        // Position de l'ISS
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(observateur, false);

        // Position du corps
        if (typeCorps == 1) {
            soleil.CalculPosition(date);
            corps.setPosition(soleil.position());
        }
        if (typeCorps == 2) {
            lune.CalculPosition(date);
            corps.setPosition(lune.position());
        }
        corps.CalculCoordHoriz(observateur, false);

        angle[i] = corps.dist().Angle(satellite.dist());
    }

    Maths::CalculExtremumInterpolation3(jjm, angle, minmax);


    /* Retour */
    return;
}

/*
 * Calcul de la date d'une borne du transit ou conjonction
 */
void TransitISS::CalculDate(Satellite &satellite, Observateur &observateur, const double jjm[], const int typeCorps,
                            const bool itransit, const double seuilConjonction, double &dateInter)
{
    /* Declarations des variables locales */
    double dist;
    double angle[3];
    Corps corps;
    Soleil soleil;
    Lune lune;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        observateur.CalculPosVit(date);

        // Position de l'ISS
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(observateur, false);

        // Position du corps
        if (typeCorps == 1) {
            soleil.CalculPosition(date);
            corps.setPosition(soleil.position());
        }
        if (typeCorps == 2) {
            lune.CalculPosition(date);
            corps.setPosition(lune.position());
        }
        corps.CalculCoordHoriz(observateur, false);

        angle[i] = corps.dist().Angle(satellite.dist());
    }

    if (itransit) {

        double rayon;
        if (typeCorps == 1)
            rayon = RAYON_SOLAIRE;
        if (typeCorps == 2)
            rayon = RAYON_LUNAIRE;

        dist = asin(rayon / corps.distance());
    } else {
        dist = seuilConjonction;
    }
    dateInter = Maths::CalculValeurXInterpolation3(jjm, angle, dist, EPS_DATES);

    /* Retour */
    return;
}


/*
 * Calcul des elements du transit ou de la conjonction
 */
void TransitISS::CalculElements(Satellite &satellite, Observateur &observateur, const double jmax, const int typeCorps,
                                const bool itransit, const double seuilConjonction, Date dates[])
{
    /* Declarations des variables locales */
    int it;
    double dateInf, dateSup, tmp;
    double jjm[3];

    /* Initialisations */
    tmp = 0.;

    /* Corps de la methode */
    // Date de debut
    jjm[0] = jmax - PAS1;
    jjm[2] = jmax;
    jjm[1] = 0.5 * (jjm[0] + jjm[2]);

    CalculDate(satellite, observateur, jjm, typeCorps, itransit, seuilConjonction, dateInf);

    // Iterations supplementaires pour affiner la date
    it = 0;
    while (fabs(dateInf - tmp) > EPS_DATES || it < 6) {

        tmp = dateInf;
        jjm[1] = dateInf;
        jjm[2] = jmax;
        jjm[0] = 2. * jjm[1] - jjm[2];

        CalculDate(satellite, observateur, jjm, typeCorps, itransit, seuilConjonction, dateInf);
        it++;
    }


    // Date de fin
    jjm[0] = jmax;
    jjm[2] = jmax + PAS1;
    jjm[1] = 0.5 * (jjm[0] + jjm[2]);

    CalculDate(satellite, observateur, jjm, typeCorps, itransit, seuilConjonction, dateSup);

    // Iterations supplementaires pour affiner la date
    it = 0;
    while (fabs(dateSup - tmp) > EPS_DATES || it < 6) {

        tmp = dateSup;
        jjm[1] = dateSup;
        jjm[0] = jmax;
        jjm[2] = 2. * jjm[1] - jjm[0];

        CalculDate(satellite, observateur, jjm, typeCorps, itransit, seuilConjonction, dateSup);
        it++;
    }

    dates[0] = Date(dateInf, 0., false);
    dates[2] = Date(dateSup, 0., false);

    /* Retour */
    return;
}

/*
 * Calcul des ephemerides du Soleil et de la Lune
 */
void TransitISS::CalculEphemSoleilLune(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    bool lvis, svis;
    Soleil soleil;
    Lune lune;
    QVector<double> tab;
    QList<QVector<double> > tabEphemLune, tabEphemSoleil;

    /* Initialisations */
    lvis = false;
    svis = false;

    /* Corps de la methode */
    // Ephemerides du Soleil
    Date date(conditions.jj1(), 0., false);
    if (conditions.acalcSol()) {
        do {

            // Position ECI de l'observateur
            observateur.CalculPosVit(date);

            // Position du Soleil
            soleil.CalculPosition(date);
            soleil.CalculCoordHoriz(observateur, false);

            if (soleil.hauteur() >= conditions.haut()) {

                svis = true;
                tab.clear();

                // Remplissage du tableau d'ephemerides
                tab.append(date.jourJulienUTC());

                tab.append(observateur.position().x());
                tab.append(observateur.position().y());
                tab.append(observateur.position().z());
                tab.push_back(observateur.rotHz().vecteur1().x());
                tab.push_back(observateur.rotHz().vecteur1().y());
                tab.push_back(observateur.rotHz().vecteur1().z());
                tab.push_back(observateur.rotHz().vecteur2().x());
                tab.push_back(observateur.rotHz().vecteur2().y());
                tab.push_back(observateur.rotHz().vecteur2().z());
                tab.push_back(observateur.rotHz().vecteur3().x());
                tab.push_back(observateur.rotHz().vecteur3().y());
                tab.push_back(observateur.rotHz().vecteur3().z());

                tab.append(soleil.position().x());
                tab.append(soleil.position().y());
                tab.append(soleil.position().z());
                tab.append(1);

                tabEphemSoleil.append(tab);
            } else {
                if (svis) {
                    svis = false;
                    date = Date(date.jourJulienUTC() + 0.375, 0., false);
                }
            }

            date = Date(date.jourJulienUTC() + PAS0, 0., false);
        } while (date.jourJulienUTC() <= conditions.jj2());
    }

    // Ephemerides de la Lune
    date = Date(conditions.jj1(), 0., false);
    if (conditions.acalcLune()) {
        do {

            // Position ECI de l'observateur
            observateur.CalculPosVit(date);

            // Position du Soleil
            lune.CalculPosition(date);
            lune.CalculCoordHoriz(observateur, false);

            if (lune.hauteur() >= conditions.haut()) {

                lvis = true;
                tab.clear();

                // Remplissage du tableau d'ephemerides
                tab.append(date.jourJulienUTC());

                tab.append(observateur.position().x());
                tab.append(observateur.position().y());
                tab.append(observateur.position().z());
                tab.push_back(observateur.rotHz().vecteur1().x());
                tab.push_back(observateur.rotHz().vecteur1().y());
                tab.push_back(observateur.rotHz().vecteur1().z());
                tab.push_back(observateur.rotHz().vecteur2().x());
                tab.push_back(observateur.rotHz().vecteur2().y());
                tab.push_back(observateur.rotHz().vecteur2().z());
                tab.push_back(observateur.rotHz().vecteur3().x());
                tab.push_back(observateur.rotHz().vecteur3().y());
                tab.push_back(observateur.rotHz().vecteur3().z());

                tab.append(lune.position().x());
                tab.append(lune.position().y());
                tab.append(lune.position().z());
                tab.append(2);

                tabEphemLune.append(tab);
            } else {
                if (lvis) {
                    lvis = false;
                    date = Date(date.jourJulienUTC() + 0.375, 0., false);
                }
            }

            date = Date(date.jourJulienUTC() + PAS0, 0., false);
        } while (date.jourJulienUTC() <= conditions.jj2());
    }
    tabEphem.append(tabEphemSoleil);
    tabEphem.append(tabEphemLune);

    tab.clear();
    tabEphemLune.clear();
    tabEphemSoleil.clear();

    /* Retour */
    return;
}
