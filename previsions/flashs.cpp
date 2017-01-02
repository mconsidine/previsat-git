/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2017  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    flashs.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des flashs Iridium ou MetOp
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    12 septembre 2015
 *
 * Date de revision
 * >    5 septembre 2016
 *
 */

#include <QFile>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "flashs.h"
#include "iridium.h"
#include "metop.h"
#include "librairies/maths/maths.h"

char Flashs::_mir;
int Flashs::_pan;
bool Flashs::_psol;
double Flashs::_surf;
Vecteur3D Flashs::_direction;

QString Flashs::_sts;
QStringList Flashs::_res;
QList<Satellite> Flashs::_sats;
QVector<TLE> Flashs::_tabtle;
QList<QVector<double > > Flashs::_tabEphem;


/*
 * Calcul des flashs
 */
void Flashs::CalculFlashs(const QString idsat, const Conditions &conditions, Observateur &observateur, QStringList &result)
{
    /* Declarations des variables locales */
    QString ligne;
    QTime tps;

    /* Initialisations */
    _psol = conditions.psol();
    _res.clear();
    result.clear();
    const double angrefMax = (conditions.typeCalcul() == IRIDIUM) ? 0.3 : 1.;
    const double pasmax = (conditions.typeCalcul() == IRIDIUM) ? PAS1 : 3. * PAS1;

    _tabtle = conditions.tabtle();
    QVectorIterator<TLE> it1(_tabtle);
    while (it1.hasNext()) {
        const TLE tle = it1.next();
        _sats.append(Satellite(tle));
    }

    // Ecriture de l'entete du fichier de previsions
    Conditions::EcrireEntete(observateur, conditions, _tabtle, false);

    /* Corps de la methode */
    tps.start();

    // Calcul des ephemerides du Soleil et du lieu d'observation
    CalculEphemSoleilObservateur(conditions, observateur);

    QListIterator<QVector<double> > it2(_tabEphem);

    // Boucle sur les satellites
    QStringListIterator it3 = QStringListIterator(conditions.tabSts());
    QListIterator<Satellite> it4(_sats);
    while (it4.hasNext()) {

        Satellite sat = it4.next();
        _sts = it3.next();
        double temp = -DATE_INFINIE;

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

                Lune lune;
                if (conditions.acalcEclipseLune())
                    lune.CalculPosition(date);

                // Determination de la condition d'eclipse du satellite
                ConditionEclipse condEcl;
                condEcl.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(), conditions.refr());

                // Le satellite n'est pas eclipse
                if (!condEcl.isEclipseTotale()) {

                    double jj0 = date.jourJulienUTC();
                    const double jj2 = jj0 + TEMPS1;
                    do {

                        double minmax[2];

                        // Calcul de l'angle de reflexion
                        _pan = -1;
                        const double angref = AngleReflexion(conditions.typeCalcul(), sat, soleil);
                        const double pas = (angref < 0.5) ? PAS1 : PAS0;

                        if (angref <= angrefMax) {

                            double jjm[3];
                            jjm[0] = jj0 - NB_JOUR_PAR_MIN;
                            jjm[1] = jj0;
                            jjm[2] = jj0 + NB_JOUR_PAR_MIN;

                            // Calcul par interpolation de l'instant correspondant
                            // a l'angle de reflexion minimum
                            CalculAngleMin(jjm, conditions.typeCalcul(), sat, observateur, soleil, minmax);

                            // Iterations supplementaires pour affiner la date du maximum
                            double pasInt = PAS_INT0;
                            for (int it=0; it<4; it++) {

                                jjm[0] = minmax[0] - pasInt;
                                jjm[1] = minmax[0];
                                jjm[2] = minmax[0] + pasInt;

                                CalculAngleMin(jjm, conditions.typeCalcul(), sat, observateur, soleil, minmax);
                                pasInt *= 0.5;
                            }

                            if (fabs(minmax[0] - temp) > pasmax)
                                DeterminationFlash(minmax, conditions, temp, observateur, sat, soleil);

                        } // fin if (angref <= 0.2)

                        jj0 += pas;
                        const Date date0(jj0, 0., false);

                        observateur.CalculPosVit(date0);

                        // Position du satellite
                        sat.CalculPosVit(date0);
                        sat.CalculCoordHoriz(observateur, false);

                        // Position du Soleil
                        soleil.CalculPosition(date0);

                        if (conditions.acalcEclipseLune())
                            lune.CalculPosition(date0);

                        // Condition d'eclipse du satellite
                        condEcl.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(),
                                                       conditions.refr());

                        if (condEcl.isEclipseTotale() || sat.hauteur() < conditions.haut())
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
        _res.sort();

    // Ecriture des resultats dans le fichier de previsions
    QFile fichier(conditions.out());
    fichier.open(QIODevice::Append | QIODevice::Text);
    QTextStream flux(&fichier);

    if (_res.count() > 0) {

        const QString enteteLigne =
                QObject::tr("%1   Date       Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Cst  Ang  Mir Magn" \
                            "   Alt   Dist  Az Soleil  Haut Soleil   Long Max    Lat Max     Distance  Magn Max");

        ligne = enteteLigne.arg(idsat);
        result.append(ligne.mid(idsat.length()));
        flux << ligne << endl;

        int i = 0;
        while (i < _res.count()) {

            ligne = _res.at(i);

            QString flash;
            switch (conditions.typeCalcul()) {

            case IRIDIUM:
            {
                const QString flashMax1 = ligne.mid(338, idsat.length()) + ligne.mid(171, 121) +
                        ligne.mid(293, 44).remove(QRegExp("\\s+$"));
                flash = (conditions.nbl() == 1) ? flashMax1 : ligne.mid(167, idsat.length()) + ligne.mid(0, 121) + "\n" +
                                                  flashMax1 + "\n" + ligne.mid(509, idsat.length()) + ligne.mid(342, 121);

                result.append((ligne.mid(0, 171) + ligne.right(5)).trimmed());
                result.append((ligne.mid(171, 171) + ligne.right(5)).trimmed());
                result.append(ligne.mid(341).trimmed());
                result.append("");
                break;
            }

            case METOP:
            {
                const QString flashMax2 = ligne.mid(344, idsat.length()) + ligne.mid(177, 121) +
                        ligne.mid(299, 44).remove(QRegExp("\\s+$"));
                flash = (conditions.nbl() == 1) ? flashMax2 : ligne.mid(167, idsat.length()) + ligne.mid(0, 121) + "\n" +
                                                  flashMax2 + "\n" + ligne.mid(521, idsat.length()) + ligne.mid(354, 121);

                result.append((ligne.mid(0, 181) + ligne.right(5)).trimmed());
                result.append((ligne.mid(177, 181) + ligne.right(5)).trimmed());
                result.append(ligne.mid(354).trimmed());
                result.append("");
                break;
            }

            case PREVISION:
            case EVENEMENTS:
            case TRANSITS:
            default:
                break;
            }

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

void Flashs::FinTraitement()
{
    _res.clear();
    _tabtle.clear();
    _sats.clear();
    _tabEphem.clear();
}

/*
 * Calcul de l'angle minimum du panneau
 */
void Flashs::CalculAngleMin(const double jjm[], const TypeCalcul typeCalc, Satellite &satellite, Observateur &observateur,
                            Soleil &soleil, double minmax[])
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
        ang[i] = AngleReflexion(typeCalc, satellite, soleil);
    }

    // Determination du minimum par interpolation
    Maths::CalculExtremumInterpolation3(jjm, ang, minmax);

    /* Retour */
    return;
}

/*
 * Calcul des ephemerides du Soleil et de la position de l'observateur
 */
void Flashs::CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur)
{
    /* Declarations des variables locales */
    bool svis;
    QVector<double> tab;
    Soleil soleil;

    /* Initialisations */
    svis = true;

    /* Corps de la methode */
    Date date(conditions.jj1(), 0., false);
    do {

        // Position ECI de l'observateur
        observateur.CalculPosVit(date);

        // Position ECI du Soleil
        soleil.CalculPosition(date);

        // Position topocentrique du Soleil
        soleil.CalculCoordHoriz(observateur, false);

        if ((conditions.typeCalcul() == METOP && soleil.hauteur() <= conditions.crep()) || conditions.typeCalcul() == IRIDIUM) {

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

            _tabEphem.append(tab);

        } else {
            if (!svis) {
                svis = true;
                if (conditions.crep() <= EPSDBL100)
                    date = Date(date.jourJulienUTC() + 0.375, 0., false);
            }
        }

        date = Date(date.jourJulienUTC() + PAS0, 0., false);
    } while (date.jourJulienUTC() <= conditions.jj2());

    tab.clear();

    /* Retour */
    return;
}

/*
 * Calcul des bornes inferieures et superieures du flash
 */
void Flashs::CalculLimitesFlash(const double mgn0, const double dateMaxFlash, const Conditions &conditions,
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

    if (conditions.typeCalcul() == METOP)
        limite[1] = DATE_INFINIE;

    for (int i=0; i<4; i++) {
        lim0[i] = limite[i];
        if (lim0[i] < DATE_INFINIE) {

            int it = 0;
            double pasInt = PAS_INT1;
            if (fabs(mgn0 - conditions.mgn2()) <= EPSDBL100)
                pasInt *= 0.5;
            do {
                it++;
                tmp = lim0[i];
                jjm[0] = lim0[i] - pasInt;
                jjm[1] = lim0[i];
                jjm[2] = lim0[i] + pasInt;

                LimiteFlash(mgn0, jjm, conditions, satellite, observateur, soleil, lim0);
                pasInt *= 0.5;
            } while (fabs(lim0[i] - tmp) > EPS_DATES && lim0[i] < DATE_INFINIE && it < 10);

            limite[i] = (lim0[i] < DATE_INFINIE && it < 10) ? lim0[i] : -DATE_INFINIE;
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

    if (conditions.typeCalcul() == METOP)
        limite[1] = DATE_INFINIE;

    for (int i=0; i<4; i++) {
        lim0[i] = limite[i];
        if (lim0[i] < DATE_INFINIE) {

            int it = 0;
            double pasInt = PAS_INT1;
            if (fabs(mgn0 - conditions.mgn2()) <= EPSDBL100)
                pasInt *= 0.5;
            do {
                it++;
                tmp = lim0[i];
                jjm[0] = lim0[i] - pasInt;
                jjm[1] = lim0[i];
                jjm[2] = lim0[i] + pasInt;

                LimiteFlash(mgn0, jjm, conditions, satellite, observateur, soleil, lim0);
                pasInt *= 0.5;
            } while (fabs(lim0[i] - tmp) > EPS_DATES && lim0[i] < DATE_INFINIE && it < 10);

            limite[i] = (lim0[i] < DATE_INFINIE && it < 10) ? lim0[i] : DATE_INFINIE;
        } else {
            limite[i] = DATE_INFINIE;
        }
    }

    dateSup = qMin(limite[0], limite[1]);
    dateSup = qMin(dateSup, limite[2]);
    dateSup = qMin(dateSup, limite[3]);

    double dateMax = dateMaxFlash;

    jjm[0] = dateInf;
    jjm[1] = 0.5 * (dateInf + dateSup);
    jjm[2] = dateSup;

    double minmax[2];
    CalculAngleMin(jjm, conditions.typeCalcul(), satellite, observateur, soleil, minmax);

    // Iterations supplementaires pour affiner la date du maximum
    double pasInt = PAS_INT0;
    for (int it=0; it<4; it++) {

        jjm[0] = minmax[0] - pasInt;
        jjm[1] = minmax[0];
        jjm[2] = minmax[0] + pasInt;

        CalculAngleMin(jjm, conditions.typeCalcul(), satellite, observateur, soleil, minmax);
        pasInt *= 0.5;
    }

    dateMax = minmax[0];

    if (dateInf < dateSup - EPS_DATES && fabs(dateInf) < DATE_INFINIE && fabs(dateSup) < DATE_INFINIE) {
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
void Flashs::DeterminationFlash(const double minmax[], const Conditions &conditions, double &temp,
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

        // Angle de reflexion
        const double ang = AngleReflexion(conditions.typeCalcul(), sat, soleil);

        if (ang <= conditions.ang0()) {

            const double mgn0 = (soleil.hauteur() < conditions.crep()) ? conditions.mgn1() : conditions.mgn2();

            Lune lune;
            if (conditions.acalcEclipseLune())
                lune.CalculPosition(date);

            ConditionEclipse condEcl;
            condEcl.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(), conditions.refr());

            // Magnitude du flash
            double mag = MagnitudeFlash(conditions.typeCalcul(), conditions.ext(), conditions.effetEclipsePartielle(), minmax[1], observateur,
                    soleil, condEcl, sat);

            if (mag <= mgn0) {

                Date dates[3];

                // Calcul des limites du flash
                CalculLimitesFlash(mgn0, minmax[0], conditions, sat, observateur, soleil, dates);
                const double pasmax = (conditions.typeCalcul() == IRIDIUM) ? PAS1 : 3. * PAS1;

                if (dates[1].jourJulienUTC() < DATE_INFINIE && fabs(dates[1].jourJulienUTC() - temp) > pasmax) {

                    temp = minmax[0];

                    // Calcul des valeurs exactes pour les differentes dates
                    _pan = -1;
                    QString flash = "", ligne = "";
                    for(int i=0; i<3; i++) {

                        observateur.CalculPosVit(dates[i]);

                        // Position du satellite
                        sat.CalculPosVit(dates[i]);
                        sat.CalculCoordHoriz(observateur);

                        // Position du Soleil
                        soleil.CalculPosition(dates[i]);
                        soleil.CalculCoordHoriz(observateur);

                        if (conditions.acalcEclipseLune())
                            lune.CalculPosition(dates[i]);

                        // Condition d'eclipse du satellite
                        ConditionEclipse condEcl2;
                        condEcl2.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(), conditions.refr());

                        // Angle de reflexion
                        const double angref = AngleReflexion(conditions.typeCalcul(), sat, soleil);

                        if (angref <= conditions.ang0() + 0.0001) {

                            // Magnitude du flash
                            mag = MagnitudeFlash(conditions.typeCalcul(), conditions.ext(), conditions.effetEclipsePartielle(), angref,
                                                 observateur, soleil, condEcl2, sat);

                            if (mag <= mgn0 + 0.05) {

                                // Ascension droite/declinaison/constellation
                                sat.CalculCoordEquat(observateur);

                                // Altitude du satellite
                                sat.CalculLatitude(sat.position());
                                const double altitude = sat.CalculAltitude(sat.position());

                                // Ecriture du flash
                                ligne = EcrireFlash(dates[i], i, altitude, angref, mag, conditions, observateur, soleil, condEcl2, sat);

                                flash.append(ligne);
                            }
                        }
                    }
                    if (flash.length() == 3 * ligne.length() && flash.length() > 0)
                        _res.append(flash + sat.tle().norad());
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Ecriture d'une ligne de flash
 */
QString Flashs::EcrireFlash(const Date &date, const int i, const double alt, const double angref, const double mag,
                            const Conditions &conditions, const Observateur &observateur,
                            const Soleil &soleil, const ConditionEclipse &condEcl, Satellite &sat)
{
    /* Declarations des variables locales */
    Observateur obsmax;

    /* Initialisations */
    double altitude = alt;
    const QString fmt = "%1%2 %3 %4 %5 %6 %7  %8  %9%10 %11%12 %13%14%15";

    /* Corps de la methode */

    // Date calendaire
    const double offset = (conditions.ecart()) ? conditions.offset() : Date::CalculOffsetUTC(Date(date.jourJulienUTC(), 0.).ToQDateTime(1));
    const Date date3(date.jourJulienUTC() + EPS_DATES, offset, true);

    // Coordonnees topocentriques
    const QString az = Maths::ToSexagesimal(sat.azimut(), DEGRE, 3, 0, false, false);
    const QString ht = Maths::ToSexagesimal(sat.hauteur(), DEGRE, 2, 0, false, false);

    // Coordonnees equatoriales
    const QString ad = Maths::ToSexagesimal(sat.ascensionDroite(), HEURE1, 2, 0, false, false);
    const QString de = Maths::ToSexagesimal(sat.declinaison(), DEGRE, 2, 0, true, false);

    // Magnitude
    const QString fmgn = "%1%2%3";
    const QString magn = fmgn.arg((mag > 0.) ? "+" : "-").arg(fabs(mag), 2, 'f', 1)
            .arg((condEcl.isEclipsePartielle() || condEcl.isEclipseAnnulaire()) ? "*" : " ");

    // Altitude du satellite et distance a l'observateur
    double distance = sat.distance();
    if (conditions.unite() == QObject::tr("nmi")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }

    // Coordonnees topocentriques du Soleil
    const QString azs = Maths::ToSexagesimal(soleil.azimut(), DEGRE, 3, 0, false, false);
    const QString hts = Maths::ToSexagesimal(soleil.hauteur(), DEGRE, 2, 0, true, false);

    QString result = fmt.arg(date3.ToShortDateAMJ(FORMAT_LONG, (conditions.syst()) ? SYSTEME_24H : SYSTEME_12H)).arg(az).arg(ht).
            arg(ad).arg(de).arg(sat.constellation()).arg(angref * RAD2DEG, 5, 'f', 2).arg(_mir).arg(magn).arg(altitude, 6, 'f', 1).
            arg(distance, 6, 'f', 1).arg(azs).arg(hts).arg(i);

    // Recherche des coordonnees geographiques ou se produit le maximum du flash
    QString max(45, ' ');
    const Vecteur3D direction = _direction;
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
        const double angRefMax = AngleReflexion(conditions.typeCalcul(), sat, soleil);

        // Magnitude du flash
        const double magFlashMax = MagnitudeFlash(conditions.typeCalcul(), conditions.ext(), conditions.effetEclipsePartielle(), angRefMax,
                                                  obsmax, soleil, condEcl, sat);

        QString mags = QString((magFlashMax >= 0.) ? "+" : "-") + QString::number(fabs(magFlashMax), 'f', 1).trimmed() +
                QString((condEcl.isEclipsePartielle() || condEcl.isEclipseAnnulaire()) ? "*" : " ");
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

    QString num;
    switch (conditions.typeCalcul()) {

    case IRIDIUM:

        // Numero Iridium
        num = _sts.mid(0, 4).trimmed() + ((_sts.length() == 9) ? " " : "?");
        while (num.length() < 4)
            num += " ";
        break;

    case METOP:

        // Nom du satellite MetOp ou SkyMed
        num = _sts.split(" ", QString::SkipEmptyParts).at(0);
        while (num.length() < 10)
            num += "   ";
        break;

    case PREVISION:
    case EVENEMENTS:
    case TRANSITS:
    default:
        break;
    }

    result = result.append(num);

    /* Retour */
    return (result);
}

/*
 * Calcul d'une limite du flash
 */
void Flashs::LimiteFlash(const double mgn0, const double jjm[], const Conditions &conditions, Satellite &satellite,
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
        satellite.CalculCoordHoriz(observateur, false);
        ht[i] = satellite.hauteur();

        // Position du Soleil
        soleil.CalculPosition(date);

        Lune lune;
        if (conditions.acalcEclipseLune())
            lune.CalculPosition(date);

        // Conditions d'eclipse du satellite
        ConditionEclipse condEcl;
        condEcl.CalculSatelliteEclipse(soleil, lune, satellite.position(), conditions.acalcEclipseLune(), conditions.refr());
        ecl[i] = (condEcl.luminositeEclipseLune() < condEcl.luminositeEclipseSoleil()) ?
                    condEcl.phiLune() - condEcl.phiSoleil() - condEcl.elongationLune() :
                    condEcl.phiTerre() - condEcl.phiSoleilRefr() - condEcl.elongationSoleil();

        // Angle de reflexion
        ang[i] = AngleReflexion(conditions.typeCalcul(), satellite, soleil);

        // Magnitude du satellite
        mag[i] = MagnitudeFlash(conditions.typeCalcul(), conditions.ext(), conditions.effetEclipsePartielle(), ang[i], observateur, soleil, condEcl,
                                satellite);
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

Matrice3D Flashs::RotationRV(const Vecteur3D &position, const Vecteur3D &vitesse, const double lacet, const double tangage,
                             const int inpl)
{
    /* Declarations des variables locales */
    Matrice3D matrice;

    /* Initialisations */
    const Vecteur3D w((position ^ vitesse).Normalise());

    /* Corps de la methode */
    const double alpha = atan2(w.y(), w.x()) + PI;
    const Matrice3D matrice1(AXE_Z, alpha);

    const double beta = -acos(w.z());
    const Matrice3D matrice2(AXE_Y, beta);

    // Conversion dans le plan de l'orbite
    const Matrice3D matrice3 = matrice2 * matrice1;
    const Vecteur3D vecteur1 = matrice3 * vitesse;

    const double gamma = atan2(vecteur1.y(), vecteur1.x());
    const Matrice3D matrice4(AXE_Z, gamma);
    matrice = matrice4 * matrice3;

    if (inpl != 0) {
        const Matrice3D matrice5(AXE_X, PI_SUR_DEUX);
        const Matrice3D matrice6 = matrice5 * matrice;
        matrice = matrice6;

        if (inpl == 2) {
            const double delta = position.Angle(vitesse) - PI_SUR_DEUX;
            const Matrice3D matrice7(AXE_Y, delta);
            const Matrice3D matrice8 = matrice7 * matrice;
            matrice = matrice8;
        }
    }

    // Rotation en lacet
    if (fabs(lacet) > EPSDBL100) {
        const Matrice3D matrice9(AXE_Z, -lacet);
        const Matrice3D matrice10 = matrice9 * matrice;
        matrice = matrice10;
    }

    // Rotation en tangage
    if (fabs(tangage) > EPSDBL100) {
        const Matrice3D matrice11(AXE_Y, -tangage);
        const Matrice3D matrice12 = matrice11 * matrice;
        matrice = matrice12;
    }

    /* Retour */
    return (matrice);
}

Matrice3D Flashs::RotationYawSteering(const Satellite &satellite, const double lacet, const double tangage)
{
    /* Declarations des variables locales */
    Matrice3D matrice;

    /* Initialisations */
    double yaw = lacet;

    /* Corps de la methode */
    matrice = RotationRV(satellite.position(), satellite.vitesse(), 0., 0., 2);
    const Vecteur3D vecteur1 = satellite.position().Normalise();

    const double cosphisq = 1. - vecteur1.z() * vecteur1.z();
    const double cosphi = (cosphisq < 0.) ? 0. : sqrt(cosphisq);
    double cosphi1 = cosphi;
    const double cosinc = cos(satellite.tle().inclo() * DEG2RAD);

    if (cosphi > 0. && cosphi > fabs(cosinc)) {

        const double sinpsi = cosinc / cosphi;
        const double psi = asin(sinpsi);
        const double cospsi = cos(psi);

        if (cospsi > 0.) {

            double psi1 = psi;
            if (satellite.vitesse().z() < 0.)
                psi1 = PI - psi1;
            psi1 = modulo(psi1, DEUX_PI);

            double phi1 = acos(cosphi1);
            double corrgeo = fabs(APLA * sin(2. * phi1));
            if (satellite.position().z() < 0.)
                corrgeo = -corrgeo;

            const Matrice3D matrice1(AXE_Z, psi1);
            const Matrice3D matrice2 = matrice1 * matrice;
            const Matrice3D matrice3(AXE_Y, corrgeo);
            const Matrice3D matrice4 = matrice3 * matrice2;
            const Matrice3D matrice5(AXE_Z, -psi1);
            matrice = matrice5 * matrice4;

            const double tanalpha = (sinpsi - cosphi / satellite.tle().no()) / cospsi;
            if (tanalpha < PI_SUR_DEUX) {

                double alpha = atan(tanalpha);
                alpha -= psi;
                if (satellite.vitesse().z() < 0.)
                    alpha = -alpha;
                yaw += alpha;
            }
        }
    }

    // Rotation en lacet
    if (fabs(yaw) > EPSDBL100) {
        const Matrice3D matrice6(AXE_Z, -yaw);
        const Matrice3D matrice7 = matrice6 * matrice;
        matrice = matrice7;
    }

    // Rotation en tangage
    if (fabs(tangage) > EPSDBL100) {
        const Matrice3D matrice8(AXE_Y, -tangage);
        const Matrice3D matrice9 = matrice8 * matrice;
        matrice = matrice9;
    }

    /* Retour */
    return (matrice);
}

/*
 * Calcul de l'angle de reflexion du panneau
 */
double Flashs::AngleReflexion(const TypeCalcul typeCalc, const Satellite &satellite, const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double ang = PI;

    /* Corps de la methode */
    switch (typeCalc) {
    case IRIDIUM:

        ang = Iridium::AngleReflexion(satellite, soleil);
        break;

    case METOP:

        ang = MetOp::AngleReflexion(satellite, soleil);
        break;

    case PREVISION:
    case EVENEMENTS:
    case TRANSITS:
    default:
        break;
    }

    /* Retour */
    return (ang);
}

/*
 * Determination de la magnitude du flash
 */
double Flashs::MagnitudeFlash(const TypeCalcul typeCalc, const bool ext, const bool eclPartielle, const double angle,
                              const Observateur &observateur, const Soleil &soleil, const ConditionEclipse &condEcl, Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;

    /* Corps de la methode */
    switch (typeCalc) {
    case IRIDIUM:

        magnitude = Iridium::MagnitudeFlash(ext, eclPartielle, angle, observateur, soleil, condEcl, satellite);
        break;

    case METOP:

        magnitude = MetOp::MagnitudeFlash(ext, eclPartielle, angle, observateur, condEcl, satellite);
        break;

    case PREVISION:
    case EVENEMENTS:
    case TRANSITS:
    default:
        break;
    }

    /* Retour */
    return (magnitude);
}
