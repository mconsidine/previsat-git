/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2018  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des evenements orbitaux
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    23 juillet 2011
 *
 * Date de revision
 * >    5 septembre 2016
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QCoreApplication>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QTime>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "evenements.h"
#include "librairies/dates/date.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/satellite/tle.h"

static const double PAS = NB_JOUR_PAR_MIN;

static QStringList res;
static QVector<TLE> tabtle;
static QList<Satellite> sats;
static QList<QList<QVector<double > > > tabEphem;

/*
 * Calcul des evenements orbitaux
 */
void Evenements::CalculEvenements(const Conditions &conditions)
{
    /* Declarations des variables locales */
    QTime tps;

    /* Initialisations */

    // Creation de la liste de TLE
    TLE::LectureFichier(conditions.fic(), conditions.listeSatellites(), tabtle);

    // Creation du tableau de satellites
    QVectorIterator<TLE> it1(tabtle);
    while (it1.hasNext()) {
        sats.append(Satellite(it1.next()));
    }

    // Ecriture de l'entete du fichier resultat
    QFile fichier(conditions.out());
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    const QString titre = "%1 %2 / %3 (c) %4";
    flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
            arg(QString(APP_ANNEES_DEV)) << endl << endl;

    const QString fuseau = QObject::tr("Fuseau horaire : %1");
    QString chaine = QObject::tr("UTC");
    if (conditions.ecart()) {
        if (fabs(conditions.offset()) > EPSDBL100) {
            QTime heur;
            heur = heur.addSecs((int) (conditions.offset() * NB_SEC_PAR_JOUR + EPS_DATES));
            chaine = chaine.append((conditions.offset() > 0.) ? " + " : " - ").append(heur.toString("hh:mm"));
        }
    }
    flux << QString((conditions.ecart()) ? fuseau.arg(chaine) : fuseau.arg(QObject::tr("Heure légale"))) << endl << endl;


    /* Corps de la methode */
    tps.start();

    // Calcul des ephemerides du satellite
    CalculEphemerides(conditions);

    // Boucle sur le tableau d'ephemerides
    QListIterator<QList<QVector<double > > > it2(tabEphem);
    it1.toFront();
    while (it2.hasNext()) {

        bool apassNoeuds = false;
        bool apassOmbre = false;
        bool apassPso = false;
        bool atransJn = false;
        res.clear();
        int i = 0, j = 0, k = 0, l = 0, m = 0;
        Satellite sat(it1.next());

        const QList<QVector<double> > list = it2.next();
        const int dim = list.size();
        QListIterator<QVector<double > > it3(list);
        do {

            const QVector<double> list1 = it3.next();
            const QVector<double> list2 = it3.next();
            const QVector<double> list3 = it3.next();

            const double xtab[] = { list1.at(0), list2.at(0), list3.at(0) };

            // Recherche des passages aux noeuds
            if (conditions.apassNoeuds()) {

                j = i;
                if (list1.at(1) * list3.at(1) < 0. && !apassNoeuds) {

                    // Il y a passage a un noeud : calcul par interpolation de la date
                    apassNoeuds = true;
                    const double ytab[] = { list1.at(1), list2.at(1), list3.at(1) };
                    const QString typeNoeud = (ytab[2] >= 0.) ? QObject::tr("Noeud Ascendant - PSO = 0°") :
                                                                QObject::tr("Noeud Descendant - PSO = 180°");
                    CalculEvt(xtab, ytab, 0., typeNoeud, conditions, sat);
                    j++;
                }
            }

            // Recherche des passages ombre/penombre/lumiere
            if (conditions.apassOmbre()) {

                k = i;
                bool tmpOmbre = false;
                if ((list1.at(7) - list1.at(6) - list1.at(5)) * (list3.at(7) - list3.at(6) - list3.at(5)) < 0. && !apassOmbre) {

                    // Il y a passage ombre->lumiere ou lumiere->ombre : calcul par interpolation de la date
                    k = i;
                    tmpOmbre = true;

                    const double ytab1[] = { list1.at(7) - list1.at(6) - list1.at(5),
                                             list2.at(7) - list2.at(6) - list2.at(5),
                                             list3.at(7) - list3.at(6) - list3.at(5) };

                    const QString typeOmbre = (ytab1[2] >= 0.) ? QObject::tr("Pénombre -> Ombre") : QObject::tr("Ombre -> Pénombre");
                    CalculEvt(xtab, ytab1, 0., typeOmbre, conditions, sat);
                }

                if ((list1.at(7) + list1.at(6) - list1.at(5)) * (list3.at(7) + list3.at(6) - list3.at(5)) < 0. && !apassOmbre) {

                    tmpOmbre = true;

                    // Calcul du passage lumiere/penombre
                    const double ytab2[] = { list1.at(7) + list1.at(6) - list1.at(5),
                                             list2.at(7) + list2.at(6) - list2.at(5),
                                             list3.at(7) + list3.at(6) - list3.at(5) };

                    const QString typePenombre = (ytab2[2] >= 0.) ? QObject::tr("Lumière -> Pénombre") : QObject::tr("Pénombre -> Lumière");
                    CalculEvt(xtab, ytab2, 0., typePenombre, conditions, sat);
                }

                if (conditions.acalcEclipseLune()) {
                    if ((list1.at(10) - list1.at(9) - list1.at(8)) * (list3.at(10) - list3.at(9) - list3.at(8)) < 0. && !apassOmbre &&
                            list2.at(11) < list2.at(12)) {

                        // Il y a passage ombre->lumiere ou lumiere->ombre : calcul par interpolation de la date
                        k = i;
                        tmpOmbre = true;

                        const double ytab1[] = { list1.at(10) - list1.at(9) - list1.at(8),
                                                 list2.at(10) - list2.at(9) - list2.at(8),
                                                 list3.at(10) - list3.at(9) - list3.at(8) };

                        const QString typeOmbre = (ytab1[2] >= 0.) ? QObject::tr("Pénombre -> Ombre (Lune)") :
                                                                     QObject::tr("Ombre -> Pénombre (Lune)");
                        CalculEvt(xtab, ytab1, 0., typeOmbre, conditions, sat);
                    }

                    if ((list1.at(10) + list1.at(9) - list1.at(8)) * (list3.at(10) + list3.at(9) - list3.at(8)) < 0. && !apassOmbre &&
                            list2.at(11) < list2.at(12)) {

                        tmpOmbre = true;

                        // Calcul du passage lumiere/penombre
                        const double ytab2[] = { list1.at(10) + list1.at(9) - list1.at(8),
                                                 list2.at(10) + list2.at(9) - list2.at(8),
                                                 list3.at(10) + list3.at(9) - list3.at(8) };

                        const QString typePenombre = (ytab2[2] >= 0.) ? QObject::tr("Lumière -> Pénombre (Lune)") :
                                                                        QObject::tr("Pénombre -> Lumière (Lune)");
                        CalculEvt(xtab, ytab2, 0., typePenombre, conditions, sat);
                    }
                }

                if (tmpOmbre) {
                    apassOmbre = true;
                    k++;
                }
            }

            // Recherche des passages apogee/perigee
            if (conditions.apassApogee()) {

                if ((list2.at(2) > list1.at(2) && list2.at(2) > list3.at(2)) ||
                        (list2.at(2) < list1.at(2) && list2.at(2) < list3.at(2))) {

                    // Il y a passage au perigee ou a l'apogee : calcul par interpolation de la date
                    double minmax[2];
                    const double ytab[] = { list1.at(2), list2.at(2), list3.at(2) };

                    Maths::CalculExtremumInterpolation3(xtab, ytab, minmax);
                    const double offset = (conditions.ecart()) ?
                                conditions.offset() : Date::CalculOffsetUTC(Date(minmax[0], 0.).ToQDateTime(1));
                    const Date date(minmax[0] + EPS_DATES, offset);

                    // Calcul de la position du satellite pour la date calculee
                    sat.CalculPosVit(date);
                    sat.CalculCoordTerrestres(date);

                    const QString typeDist = (ytab[2] >= minmax[1]) ? QObject::tr("Périgée :") : QObject::tr("Apogée :");

                    // Calcul de la PSO
                    sat.CalculElementsOsculateurs(date);
                    const double pso = RAD2DEG * modulo(sat.elements().anomalieVraie() + sat.elements().argumentPerigee(), DEUX_PI);

                    // Ecriture de la ligne de resultat
                    const QString fmt = "%1  %2°  %3° %4  %5° %6  %7 %8 %9 (%10 %9)";
                    double rayonVecteur = minmax[1];
                    double altitude = minmax[1] - RAYON_TERRESTRE;
                    if (conditions.unite() == QObject::tr("nmi")) {
                        rayonVecteur *= MILE_PAR_KM;
                        altitude *= MILE_PAR_KM;
                    }
                    const QString ligne = fmt.arg(date.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H)).
                            arg(pso, 6, 'f', 2, QChar('0')).arg(fabs(sat.longitude() * RAD2DEG), 6, 'f', 2, QChar('0')).
                            arg((sat.longitude() >= 0.) ? QObject::tr("W") : QObject::tr("E")).
                            arg(fabs(sat.latitude()) * RAD2DEG, 5, 'f', 2, QChar('0')).
                            arg((sat.latitude() >= 0.) ? QObject::tr("N") : QObject::tr("S")).
                            arg(typeDist).arg(rayonVecteur, 0, 'f', 1).arg(conditions.unite()).
                            arg(altitude, 0, 'f', 1);

                    res.append(ligne);
                }
            }

            // Recherche des passages au terminateur
            if (conditions.atransJn()) {
                l = i;
                if (list1.at(3) * list3.at(3) < 0. && !atransJn) {

                    // Il y a un passage au terminateur : calcul par interpolation de la date
                    atransJn = true;
                    const double ytab[] = { list1.at(3), list2.at(3), list3.at(3) };
                    const QString typeTrans = (ytab[2] < 0.) ? QObject::tr("Passage terminateur Jour -> Nuit") :
                                                               QObject::tr("Passage terminateur Nuit -> Jour");
                    CalculEvt(xtab, ytab, 0., typeTrans, conditions, sat);
                    l++;
                }
            }

            // Recherche des passages a PSO=90° et PSO=270°
            if (conditions.apassPso()) {

                m = i;
                for (int n=1; n<4; n+=2) {

                    const double noeud = 90. * n * DEG2RAD;
                    if ((list1.at(4) - noeud) * (list3.at(4) - noeud) < 0. && list1.at(4) < noeud && !apassPso) {

                        // Il y a un passage a PSO=90° ou PSO=270° : calcul par interpolation de la date
                        apassPso = true;
                        const double ytab[] = { list1.at(4), list2.at(4), list3.at(4) };
                        const QString typePso = QObject::tr("Passage à PSO =") + " " + QString::number(noeud * RAD2DEG) + "°";
                        CalculEvt(xtab, ytab, noeud, typePso, conditions, sat);
                        m++;
                    }
                }
            }

            if (j == i)
                apassNoeuds = false;
            if (k == i)
                apassOmbre = false;
            if (l == i)
                atransJn = false;
            if (m == i)
                apassPso = false;
            it3.previous();
            it3.previous();
            i++;
        } while (i < dim - 2);

        // Ecriture du fichier
        if (res.count() > 0) {

            // Tri des resultats
            res.sort();

            // Conversion des dates au format AM/PM
            if (!conditions.syst()) {

                QStringList res2;
                res2.clear();

                i = 0;
                while (i < res.count()) {

                    const QStringList listDate = res.at(i).mid(0, 19).split(QRegExp("[/: ]"), QString::SkipEmptyParts);
                    const QString evt = res.at(i).mid(20);

                    const int annee = listDate.at(0).toInt();
                    const int mois = listDate.at(1).toInt();
                    const int jour = listDate.at(2).toInt();
                    const int heure = listDate.at(3).toInt();
                    const int minutes = listDate.at(4).toInt();
                    const double secondes = listDate.at(5).toDouble();

                    const Date date(annee, mois, jour, heure, minutes, secondes, 0.);
                    const QString date12h = date.ToShortDateAMJ(FORMAT_COURT, SYSTEME_12H);
                    res2.append(date12h + evt);
                    i++;
                }
                res = res2;
            }

            QString nom = sat.tle().nom();
            if (nom.contains("R/B") || nom.contains(" DEB"))
                nom = nom.append(QObject::tr("  (numéro NORAD : %1)")).arg(sat.tle().norad());
            flux << nom << endl;
            flux << QObject::tr("   Date      Heure      PSO    Longitude  Latitude  Évènements") << endl;

            i = 0;
            while (i < res.count()) {
                if (i > 0 && res.at(i).mid(0, 10) != res.at(i-1).mid(0, 10))
                    flux << endl;
                flux << res.at(i) << endl;
                i++;
            }
            flux << endl;
            res.clear();
        }
    }
    int fin = tps.elapsed();

    const QString temps = QObject::tr("Temps écoulé : %1s");
    flux << temps.arg(1.e-3 * fin, 0, 'f', 2) << endl;
    fichier.close();
    FinTraitement();

    /* Retour */
    return;
}

void Evenements::FinTraitement()
{
    res.clear();
    tabtle.clear();
    sats.clear();
    tabEphem.clear();
}

/*
 * Calcul des ephemerides du satellite
 */
void Evenements::CalculEphemerides(const Conditions &conditions)
{
    /* Declarations des variables locales */
    Soleil soleil;
    Lune lune;
    QVector<double> listVal;
    QList<QVector<double> > tab;

    /* Initialisations */

    /* Corps de la methode */
    QListIterator<Satellite> it1(sats);
    while (it1.hasNext()) {

        tab.clear();
        Satellite sat = it1.next();
        Date date(conditions.jj1(), 0., false);

        do {

            // Position du Soleil
            soleil.CalculPosition(date);

            // Position de la Lune
            if (conditions.acalcEclipseLune())
                lune.CalculPosition(date);

            // Position du satellite
            sat.CalculPosVit(date);

            ConditionEclipse condEcl;
            condEcl.CalculSatelliteEclipse(soleil, lune, sat.position(), conditions.acalcEclipseLune(), conditions.refr());
            sat.CalculElementsOsculateurs(date);

            // Sauvegarde des donnees
            listVal.clear();
            listVal.append(date.jourJulienUTC());
            const Vecteur3D position = sat.position();
            listVal.append(position.z());
            listVal.append(position.Norme());
            listVal.append(position * soleil.position());
            listVal.append(modulo(sat.elements().anomalieVraie() + sat.elements().argumentPerigee(), DEUX_PI));

            listVal.append(condEcl.elongationSoleil());
            listVal.append(condEcl.phiSoleilRefr());
            listVal.append(condEcl.phiTerre());

            if (conditions.acalcEclipseLune()) {
                listVal.append(condEcl.elongationLune());
                listVal.append(condEcl.phiSoleil());
                listVal.append(condEcl.phiLune());
                listVal.append(condEcl.luminositeEclipseLune());
                listVal.append(condEcl.luminositeEclipseSoleil());
            }

            tab.append(listVal);

            date = Date(date.jourJulienUTC() + PAS, 0., false);
        } while (date.jourJulienUTC() <= conditions.jj2());

        tabEphem.append(tab);
    }

    listVal.clear();
    tab.clear();

    /* Retour */
    return;
}

void Evenements::CalculEvt(const double xtab[3], const double ytab[3], const double yval, const QString &typeEvt, const Conditions &conditions,
Satellite &sat)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt = "%1  %2°  %3° %4  %5° %6  %7";

    /* Corps de la methode */
    const double datp = Maths::CalculValeurXInterpolation3(xtab, ytab, yval, EPS_DATES);
    const double offset = (conditions.ecart()) ? conditions.offset() : Date::CalculOffsetUTC(Date(datp, 0.).ToQDateTime(1));
    const Date date(datp + EPS_DATES, offset);

    // Calcul de la position du satellite pour la date calculee
    sat.CalculPosVit(date);
    sat.CalculCoordTerrestres(date);

    // Calcul de la PSO
    sat.CalculElementsOsculateurs(date);
    const double pso = RAD2DEG * modulo(sat.elements().anomalieVraie() + sat.elements().argumentPerigee(), DEUX_PI);

    // Ecriture de la ligne de resultat
    const QString ligne = fmt.arg(date.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H)).
            arg(pso, 6, 'f', 2, QChar('0')).arg(fabs(sat.longitude() * RAD2DEG), 6, 'f', 2, QChar('0')).
            arg((sat.longitude() >= 0.) ? QObject::tr("W") : QObject::tr("E")).
            arg(fabs(sat.latitude()) * RAD2DEG, 5, 'f', 2, QChar('0')).
            arg((sat.latitude() >= 0.) ? QObject::tr("N") : QObject::tr("S")).arg(typeEvt);

    res.append(ligne);

    /* Retour */
    return;
}
