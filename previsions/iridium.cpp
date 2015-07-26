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
 * >    26 juillet 2015
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <math.h>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QTime>
#pragma GCC diagnostic warning "-Wconversion"
#include "iridium.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/maths/maths.h"

// Pas de calcul ou d'interpolation
static const double PAS0 = NB_JOUR_PAR_MIN;
static const double PAS1 = 10. * NB_JOUR_PAR_SEC;
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
static const QByteArray LISTE_MIR = QObject::tr("ADG").toLatin1();
static const int LISTE_PAN[] = {0, 1, 2};

// Pour le calcul de l'assombrissement sur le bord pour la magnitude du Soleil
static const double TAB_INT[] = {0.3, 0.93, -0.23};

static char _mir;
static int _pan;
static Vecteur3D _solsat;
static Matrice3D _PR;

static QStringList res;
static QList<Satellite> sats;
static QVector<TLE> tabtle;
static QList<QVector<double > > tabEphem;

/*
 * Calcul des flashs Iridium
 */
void Iridium::CalculFlashsIridium(const Conditions &conditions, Observateur &observateur, QStringList &result)
{
    /* Declarations des variables locales */
    QString ligne;
    QTime tps;

    /* Initialisations */
    result.clear();
    tabtle = conditions.tabtle();
    QVectorIterator<TLE> it1(tabtle);
    while (it1.hasNext()) {
        const TLE tle = it1.next();
        sats.append(Satellite(tle));
    }

    // Ecriture de l'entete du fichier de previsions
    Conditions::EcrireEntete(observateur, conditions, tabtle, false);

    /* Corps de la methode */
    tps.start();

    // Calcul des ephemerides du Soleil et du lieu d'observation
    CalculEphemSoleilObservateur(conditions, observateur);

    QListIterator<QVector<double> > it2(tabEphem);

    // Boucle sur les satellites
    QStringListIterator it3 = QStringListIterator(conditions.tabStsIri());
    QListIterator<Satellite> it4(sats);
    while (it4.hasNext()) {

        Satellite sat = it4.next();
        const QString sts = it3.next();

        // Boucle sur le tableau d'ephemerides du Soleil
        it2.toFront();
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

            // Position ECI du Soleil
            const Vecteur3D solPos(list.at(13), list.at(14), list.at(15));
            Soleil soleil(solPos);

            // Position du satellite
            sat.CalculPosVit(date);
            sat.CalculCoordHoriz(obs, false);

            // Le satellite a une hauteur superieure a celle specifiee par l'utilisateur
            if (sat.hauteur() >= conditions.haut()) {

                double temp = -DATE_INFINIE;

                // Determination de la condition d'eclipse du satellite
                sat.CalculSatelliteEclipse(soleil, conditions.refr());

                // Le satellite n'est pas eclipse
                if (!sat.isEclipse()) {

                    double jj0 = date.jourJulienUTC();
                    const double jj2 = jj0 + TEMPS1;
                    do {

                        double minmax[2];

                        // Calcul de l'angle de reflexion
                        _pan = -1;
                        const double angref = AngleReflexion(sat, soleil);
                        const double pas = (angref < 0.5) ? PAS1 : PAS0;

                        if (angref <= 0.3) {

                            double jjm[3];
                            jjm[0] = jj0 - NB_JOUR_PAR_MIN;
                            jjm[1] = jj0;
                            jjm[2] = jj0 + NB_JOUR_PAR_MIN;

                            // Calcul par interpolation de l'instant correspondant
                            // a l'angle de reflexion minimum
                            CalculAngleMin(jjm, sat, observateur, soleil, minmax);

                            // Iterations supplementaires pour affiner la date du maximum
                            double pasInt = PAS_INT0;
                            for (int it=0; it<4; it++) {

                                jjm[0] = minmax[0] - pasInt;
                                jjm[1] = minmax[0];
                                jjm[2] = minmax[0] + pasInt;

                                CalculAngleMin(jjm, sat, observateur, soleil, minmax);
                                pasInt *= 0.5;
                            }

                            if (minmax[0] - temp > PAS1)
                                DeterminationFlash(minmax, sts, conditions, temp, observateur, sat, soleil);

                        } // fin if (angref <= 0.2)

                        jj0 += pas;
                        const Date date0(jj0, 0., false);

                        observateur.CalculPosVit(date0);

                        // Position du satellite
                        sat.CalculPosVit(date0);
                        sat.CalculCoordHoriz(observateur, false);

                        // Position du Soleil
                        soleil.CalculPosition(date0);

                        // Condition d'eclipse du satellite
                        sat.CalculSatelliteEclipse(soleil, conditions.refr());

                        if (sat.isEclipse() || sat.hauteur() < conditions.haut())
                            jj0 = jj2 + PAS0;
                    } while (jj0 <= jj2);
                    date = Date(jj0 + TEMPS2, 0., false);
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

    // Tri chronologique
    if (conditions.chr())
        res.sort();

    // Ecriture des resultats dans le fichier de previsions
    QFile fichier(conditions.out());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    if (res.count() > 0) {

        ligne = QObject::tr("Ir     Date       Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Cst Ang  Mir Magn   Alt   Dist" \
                            "  Az Soleil  Haut Soleil   Long Max    Lat Max     Distance  Magn Max");
        result.append(ligne.mid(4));
        flux << ligne << endl;

        int i = 0;
        while (i < res.count()) {

            ligne = res.at(i);

            const QString flashMax = ligne.mid(336, 4) + ligne.mid(170, 120) + ligne.mid(291, 44).remove(QRegExp("\\s+$"));
            const QString flash = (conditions.nbl() == 1) ? flashMax : ligne.mid(166, 4) + ligne.mid(0, 120) + "\n" +
                                                               flashMax + "\n" + ligne.mid(506, 4) + ligne.mid(340, 120);

            result.append((ligne.mid(0, 170) + ligne.right(5)).trimmed());
            result.append((ligne.mid(170, 170) + ligne.right(5)).trimmed());
            result.append(ligne.mid(340).trimmed());
            result.append("");

            flux << flash.trimmed() << endl << endl;
            i++;
        }
    }

    ligne = QObject::tr("Temps écoulé : %1s");
    ligne = ligne.arg(1.e-3 * fin, 0, 'f', 2);
    flux << ligne << endl;
    fichier.close();
    FinTraitement();

    /* Retour */
    return;
}

double Iridium::CalculMagnitudeIridium(const bool extinction, const Satellite &satellite, const Soleil &soleil,
                                       const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _pan = -1;
    Satellite sat = satellite;

    /* Corps de la methode */
    const double angRef = AngleReflexion(satellite, soleil);

    /* Retour */
    return (MagnitudeFlash(extinction, angRef, observateur, soleil, sat));
}

void Iridium::FinTraitement()
{
    res.clear();
    tabtle.clear();
    sats.clear();
    tabEphem.clear();
}

/*
 * Lecture du fichier de statut des satellites Iridium
 */
int Iridium::LectureStatutIridium(const char ope, QStringList &tabStsIri)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    const QString dirLocalData = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(),
                                                        QStandardPaths::LocateDirectory) + "data";
#endif

    int i = 0;

    /* Corps de la methode */
    QFile fichier(dirLocalData + QDir::separator() + "iridium.sts");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    while (!flux.atEnd()) {
        const QString ligne = flux.readLine();
        tabStsIri.append(ligne);
        if (ligne.size() == 9) {
            i++;
        } else {
            if (ligne.at(10) == '?' && ope == 'n')
                i++;
        }
    }
    fichier.close();

    /* Retour */
    return (i);
}

/*
 * Calcul de l'angle de reflexion du panneau
 */
double Iridium::AngleReflexion(const Satellite &satellite, const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double ang = PI;

    /* Corps de la methode */
    const Vecteur3D xx = satellite.vitesse().Normalise();
    const Vecteur3D yy = satellite.position().Normalise() ^ xx;
    const Vecteur3D zz = xx ^ yy;

    // Matrice de passage ECI geocentrique -> ECI satellite
    const Matrice3D P(xx, yy, zz);

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
        const Matrice3D R(v1, v2, v3);

        // Matrice produit P x R
        Matrice3D tmp = P * R;
        const Matrice3D PR = tmp.Transposee();

        // Position observateur dans le repere panneau
        const Vecteur3D obsat = PR * (-satellite.dist());

        // Position Soleil dans le repere panneau
        Vecteur3D solsat = PR * (soleil.position() - satellite.position());

        // Position du reflet du Soleil
        solsat = Vecteur3D(solsat.x(), -solsat.y(), -solsat.z());

        // Angle de reflexion
        const double temp = obsat.Angle(solsat);
        if (_pan == -1) {
            if (temp < ang) {
                ang = temp;
                j = i;
                _mir = LISTE_MIR[i];
                _PR = PR;
                _solsat = solsat;
            }
        } else {
            ang = temp;
            _PR = PR;
            _solsat = solsat;
        }
    }

    if (_pan == -1)
        _pan = LISTE_PAN[j];

    /* Retour */
    return (ang);
}

/*
 * Calcul de l'angle minimum du panneau
 */
void Iridium::CalculAngleMin(const double jjm[], Satellite &satellite, Observateur &observateur, Soleil &soleil,
                             double minmax[])
{
    /* Declarations des variables locales */
    double ang[3];

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

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

/*
 * Calcul des ephemerides du Soleil et de la position de l'observateur
 */
void Iridium::CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    QVector<double> tab;
    Soleil soleil;

    /* Initialisations */

    /* Corps de la methode */
    Date date(conditions.jj1(), 0., false);
    do {

        // Position ECI de l'observateur
        observateur.CalculPosVit(date);

        // Position ECI du Soleil
        soleil.CalculPosition(date);

        // Position topocentrique du Soleil
        soleil.CalculCoordHoriz(observateur, false);

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

        date = Date(date.jourJulienUTC() + PAS0, 0., false);
    } while (date.jourJulienUTC() <= conditions.jj2());

    tab.clear();

    /* Retour */
    return;
}

/*
 * Calcul des bornes inferieures et superieures du flash
 */
void Iridium::CalculLimitesFlash(const double mgn0, const double dateMaxFlash, const Conditions &conditions,
                                 Satellite &satellite, Observateur &observateur, Soleil &soleil, Date lim[])
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

    LimiteFlash(mgn0, jjm, conditions, satellite, observateur, soleil, limite);

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

                LimiteFlash(mgn0, jjm, conditions, satellite, observateur, soleil, lim0);
                pasInt *= 0.5;
            } while (fabs(lim0[i] - tmp) > EPS_DATES && lim0[i] < DATE_INFINIE && it < 10);

            if (lim0[i] < DATE_INFINIE && it < 10)
                limite[i] = lim0[i];
        } else {
            limite[i] = -DATE_INFINIE;
        }
    }

    dateInf = qMax(limite[0], limite[1]);
    dateInf = qMax(dateInf, limite[2]);
    dateInf = qMax(dateInf, limite[3]);


    // Determination de la date superieure du flash
    jjm[0] = dateMaxFlash;
    jjm[1] = 0.5 * (dateMaxFlash + jj2 + PAS1);
    jjm[2] = jj2 + PAS1;

    LimiteFlash(mgn0, jjm, conditions, satellite, observateur, soleil, limite);

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

                LimiteFlash(mgn0, jjm, conditions, satellite, observateur, soleil, lim0);
                pasInt *= 0.5;
            } while (fabs(lim0[i] - tmp) > EPS_DATES && lim0[i] < DATE_INFINIE && it < 10);

            if (lim0[i] < DATE_INFINIE && it < 10)
                limite[i] = lim0[i];
        } else {
            limite[i] = DATE_INFINIE;
        }
    }

    dateSup = qMin(limite[0], limite[1]);
    dateSup = qMin(dateSup, limite[2]);
    dateSup = qMin(dateSup, limite[3]);

    jjm[0] = dateInf;
    jjm[1] = 0.5 * (dateInf + dateSup);
    jjm[2] = dateSup;

    double minmax[2];
    CalculAngleMin(jjm, satellite, observateur, soleil, minmax);

    // Iterations supplementaires pour affiner la date du maximum
    double pasInt = PAS_INT0;
    for (int it=0; it<4; it++) {

        jjm[0] = minmax[0] - pasInt;
        jjm[1] = minmax[0];
        jjm[2] = minmax[0] + pasInt;

        CalculAngleMin(jjm, satellite, observateur, soleil, minmax);
        pasInt *= 0.5;
    }

    double dateMax = minmax[0];

    if (dateInf < dateSup - EPS_DATES) {
        if (dateMax < dateInf)
            dateMax = dateInf;
        if (dateMax > dateSup)
            dateMax = dateSup;

        lim[0] = Date(dateInf, 0., false);
        lim[1] = Date(dateMax, 0., false);
        lim[2] = Date(dateSup, 0., false);

    } else {
        lim[1] = Date(DATE_INFINIE, 0., false);
    }

    /* Retour */
    return;
}

/*
 * Determination du flash
 */
void Iridium::DeterminationFlash(const double minmax[], const QString &sts, const Conditions &conditions, double &temp,
                                 Observateur &observateur, Satellite &sat, Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Date date = Date(minmax[0], 0., false);

    /* Corps de la methode */
    // Position de l'observateur
    observateur.CalculPosVit(date);

    // Position du satellite
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(observateur, false);

    if (sat.hauteur() >= 0.) {

        // Position du Soleil
        soleil.CalculPosition(date);
        soleil.CalculCoordHoriz(observateur, false);

        const double mgn0 = (soleil.hauteur() < conditions.crep()) ? conditions.mgn1() : conditions.mgn2();

        // Magnitude du flash
        double mag = MagnitudeFlash(conditions.ext(), minmax[1], observateur, soleil, sat);

        if (mag <= mgn0) {

            Date dates[3];

            // Calcul des limites du flash
            CalculLimitesFlash(mgn0, minmax[0], conditions, sat, observateur, soleil, dates);

            if (dates[1].jourJulienUTC() < DATE_INFINIE) {

                temp = minmax[0];

                // Calcul des valeurs exactes pour les differentes dates
                _pan = -1;
                QString flash = "";
                for(int i=0; i<3; i++) {

                    observateur.CalculPosVit(dates[i]);

                    // Position du satellite
                    sat.CalculPosVit(dates[i]);
                    sat.CalculCoordHoriz(observateur);

                    // Position du Soleil
                    soleil.CalculPosition(dates[i]);
                    soleil.CalculCoordHoriz(observateur);

                    // Condition d'eclipse du satellite
                    sat.CalculSatelliteEclipse(soleil, conditions.refr());

                    // Angle de reflexion
                    const double angref = AngleReflexion(sat, soleil);

                    // Magnitude du flash
                    mag = MagnitudeFlash(conditions.ext(), angref, observateur, soleil, sat);

                    // Ascension droite/declinaison/constellation
                    sat.CalculCoordEquat(observateur);

                    // Altitude du satellite
                    sat.CalculLatitude(sat.position());
                    const double altitude = sat.CalculAltitude(sat.position());

                    // Ecriture du flash
                    const QString ligne = EcrireFlash(dates[i], i, altitude, angref, mag, sts, conditions, observateur,
                                                      soleil, sat);

                    flash.append(ligne);
                }
                res.append(flash + sat.tle().norad());
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Ecriture d'une ligne de flash
 */
QString Iridium::EcrireFlash(const Date &date, const int i, const double alt, const double angref, const double mag,
                             const QString &sts, const Conditions &conditions, const Observateur &observateur,
                             const Soleil &soleil, Satellite &sat)
{
    /* Declarations des variables locales */
    Observateur obsmax;

    /* Initialisations */
    double altitude = alt;
    const QString fmt = "%1%2 %3 %4 %5 %6 %7  %8  %9%10 %11%12 %13%14%15";

    /* Corps de la methode */

    // Date calendaire
    const double offset = (conditions.ecart()) ? conditions.offset() :
                                                    Date::CalculOffsetUTC(Date(date.jourJulienUTC(), 0.).ToQDateTime(1));
    const Date date3(date.jourJulienUTC() + offset + EPS_DATES, 0., true);

    // Coordonnees topocentriques
    const QString az = Maths::ToSexagesimal(sat.azimut(), DEGRE, 3, 0, false, false);
    const QString ht = Maths::ToSexagesimal(sat.hauteur(), DEGRE, 2, 0, false, false);

    // Coordonnees equatoriales
    const QString ad = Maths::ToSexagesimal(sat.ascensionDroite(), HEURE1, 2, 0, false, false);
    const QString de = Maths::ToSexagesimal(sat.declinaison(), DEGRE, 2, 0, true, false);

    // Magnitude
    const QString fmgn = "%1%2%3";
    const QString magn = fmgn.arg((mag > 0.) ? "+" : "-").arg(fabs(mag), 2, 'f', 1).arg((sat.isPenombre()) ? "*" : " ");

    // Altitude du satellite et distance a l'observateur
    double distance = sat.distance();
    if (conditions.unite() == QObject::tr("nmi")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }

    // Coordonnees topocentriques du Soleil
    const QString azs = Maths::ToSexagesimal(soleil.azimut(), DEGRE, 3, 0, false, false);
    const QString hts = Maths::ToSexagesimal(soleil.hauteur(), DEGRE, 2, 0, true, false);

    QString result = fmt.arg(date3.ToShortDateAMJ(FORMAT_LONG, (conditions.syst()) ? SYSTEME_24H : SYSTEME_12H)).arg(az).arg(ht).arg(ad).
            arg(de).arg(sat.constellation()).arg(angref * RAD2DEG, 4, 'f', 2).arg(_mir).arg(magn).arg(altitude, 6, 'f', 1).
            arg(distance, 6, 'f', 1).arg(azs).arg(hts).arg(i);

    // Recherche des coordonnees geographiques ou se produit le maximum du flash
    QString max(45, ' ');
    const Vecteur3D direction = _PR.Transposee() * _solsat;
    obsmax = Observateur::CalculIntersectionEllipsoide(date, sat.position(), direction);
    if (!obsmax.nomlieu().isEmpty()) {

        obsmax.CalculPosVit(date);
        sat.CalculCoordHoriz(obsmax, false);

        // Distance entre les 2 lieux d'observation
        const double distanceObs = observateur.CalculDistance(obsmax);
        double diff = obsmax.longitude() - observateur.longitude();
        if (fabs(diff) > PI)
            diff -= sgn(diff) * PI;
        const QString dir = (diff > 0) ? QObject::tr("(W)") : QObject::tr("(E)");

        // Angle de reflexion pour le lieu du maximum
        const double angRefMax = AngleReflexion(sat, soleil);

        // Magnitude du flash
        const double magFlashMax = MagnitudeFlash(conditions.ext(), angRefMax, obsmax, soleil, sat);
        QString mags = QString((magFlashMax >= 0.) ? "+" : "-") + QString::number(fabs(magFlashMax), 'f', 1).trimmed() +
                QString((sat.isPenombre()) ? "*" : " ");
        while (mags.length() < 6)
            mags += " ";

        const QString ew = (obsmax.longitude() >= 0.) ? QObject::tr("W") : QObject::tr("E");
        const QString ns = (obsmax.latitude() >= 0.) ? QObject::tr("N") : QObject::tr("S");

        // Ecriture de la chaine de caracteres
        const QString fmt2 = "   %1 %2  %3 %4  %5 %6   %7";
        max = fmt2.arg(fabs(obsmax.longitude() * RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew).
                arg(fabs(obsmax.latitude() * RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns).arg(distanceObs, 6, 'f', 1).arg(dir).arg(mags);
    }

    result = result.arg(max);

    // Numero Iridium
    QString num = sts.mid(0, 4).trimmed() + ((sts.length() == 9) ? " " : "?");
    while (num.length() < 4)
        num += " ";

    result = result.append(num);

    /* Retour */
    return (result);
}

/*
 * Calcul d'une limite du flash
 */
void Iridium::LimiteFlash(const double mgn0, const double jjm[], const Conditions &conditions, Satellite &satellite,
                          Observateur &observateur, Soleil &soleil, double limite[])
{
    /* Declarations des variables locales */
    double ang[3], ecl[3], ht[3], mag[3];

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        observateur.CalculPosVit(date);

        // Position du satellite
        satellite.CalculPosVit(date);
        satellite.CalculCoordHoriz(observateur, false);;
        ht[i] = satellite.hauteur();

        // Position du Soleil
        soleil.CalculPosition(date);

        // Conditions d'eclipse du satellite
        satellite.CalculSatelliteEclipse(soleil, conditions.refr());
        ecl[i] = satellite.rayonOmbre() - satellite.elongation();

        // Angle de reflexion
        ang[i] = AngleReflexion(satellite, soleil);

        // Magnitude du satellite
        mag[i] = MagnitudeFlash(conditions.ext(), ang[i], observateur, soleil, satellite);
    }

    double t_ecl, t_ht;
    // Calcul par interpolation de la date pour laquelle la magnitude est egale a la magnitude specifiee
    // par l'utilisateur
    const double t_mag = Maths::CalculValeurXInterpolation3(jjm, mag, mgn0, EPS_DATES);

    // Calcul par interpolation de la date pour laquelle l'angle de reflexion est egal a l'angle
    // de reflexion specifie par l'utilisateur
    const double t_ang = Maths::CalculValeurXInterpolation3(jjm, ang, conditions.ang0(), EPS_DATES);

    // Calcul par interpolation de la date pour laquelle la hauteur est egale a la hauteur specifie par l'utilisateur
    if ((ht[0] - conditions.haut()) * (ht[2] - conditions.haut()) < 0. ||
            (ht[0] < conditions.haut() && ht[2] < conditions.haut())) {
        t_ht = Maths::CalculValeurXInterpolation3(jjm, ht, conditions.haut(), EPS_DATES);
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

/*
 * Determination de la magnitude du flash
 */
double Iridium::MagnitudeFlash(const bool ext, const double angle, const Observateur &observateur, const Soleil &soleil,
                               Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;
    const double omega = RAYON_SOLAIRE / (soleil.distanceUA() * UA2KM);
    const double invDist3 = 1. / (satellite.distance() * satellite.distance() * satellite.distance());
    const double aireProjetee = fabs(satellite.dist().x()) * invDist3 * AIRE_MMA;

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
