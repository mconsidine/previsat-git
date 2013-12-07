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
 * >    1er decembre 2013
 *
 */

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTime>
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
    TLE::LectureFichier(conditions.getFic(), conditions.getListeSatellites(), tabtle);

    // Creation du tableau de satellites
    QVectorIterator<TLE> it1(tabtle);
    while (it1.hasNext()) {
        sats.append(Satellite(it1.next()));
    }

    // Ecriture de l'entete du fichier resultat
    QFile fichier(conditions.getOut());
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    const QString titre = "%1 %2 / %3 (c) %4";
    flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
            arg(QString(APP_ANNEES_DEV)) << endl << endl;

    QString ligne = QObject::tr("Fuseau horaire            : Heure l�gale");
    flux << ligne << endl;
    flux << QObject::tr("Unit� de distance         : %1").arg(conditions.getUnite()) << endl << endl;

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
        Satellite sat = it1.next();

        const QList<QVector<double> > list = it2.next();
        const int dim = list.size();
        QListIterator<QVector<double > > it3(list);
        do {

            const QVector<double> list1 = it3.next();
            const QVector<double> list2 = it3.next();
            const QVector<double> list3 = it3.next();

            const double xtab[] = { list1.at(0), list2.at(0), list3.at(0) };

            // Recherche des passages aux noeuds
            if (conditions.getApassNoeuds()) {

                j = i;
                if (list1.at(1) * list3.at(1) < 0. && !apassNoeuds) {

                    // Il y a passage a un noeud : calcul par interpolation de la date
                    apassNoeuds = true;
                    const double ytab[] = { list1.at(1), list2.at(1), list3.at(1) };
                    const QString typeNoeud = (ytab[2] >= 0.) ? QObject::tr("Noeud Ascendant - PSO = 0�") :
                                                                QObject::tr("Noeud Descendant - PSO = 180�");
                    CalculEvt(xtab, ytab, 0., typeNoeud, conditions, sat);
                    j++;
                }
            }

            // Recherche des passages ombre/penombre/lumiere
            if (conditions.getApassOmbre()) {

                k = i;
                if ((list1.at(4) - list1.at(5) - list1.at(3)) * (list3.at(4) - list3.at(5) - list3.at(3)) < 0. && !apassOmbre) {

                    // Il y a passage ombre->lumiere ou lumiere->ombre : calcul par interpolation de la date
                    apassOmbre = true;
                    k = i;

                    const double ytab1[] = { list1.at(4) - list1.at(5) - list1.at(3),
                                            list2.at(4) - list2.at(5) - list2.at(3),
                                            list3.at(4) - list3.at(5) - list3.at(3) };

                    const QString typeOmbre = (ytab1[2] >= 0.) ? QObject::tr("P�nombre -> Ombre") : QObject::tr("Ombre -> P�nombre");
                    CalculEvt(xtab, ytab1, 0., typeOmbre, conditions, sat);

                    // Calcul du passage lumiere/penombre
                    const double ytab2[] = { list1.at(4) + list1.at(5) - list1.at(3),
                                            list2.at(4) + list2.at(5) - list2.at(3),
                                            list3.at(4) + list3.at(5) - list3.at(3) };

                    const QString typePenombre = (typeOmbre == QObject::tr("Ombre -> P�nombre")) ?
                                QObject::tr("P�nombre -> Lumi�re") : QObject::tr("Lumi�re -> P�nombre");
                    CalculEvt(xtab, ytab2, 0., typePenombre, conditions, sat);
                    k++;
                }
            }

            // Recherche des passages apogee/perigee
            if (conditions.getApassApogee()) {

                if ((list2.at(2) > list1.at(2) && list2.at(2) > list3.at(2)) ||
                        (list2.at(2) < list1.at(2) && list2.at(2) < list3.at(2))) {

                    // Il y a passage au perigee ou a l'apogee : calcul par interpolation de la date
                    double minmax[2];
                    const double ytab[] = { list1.at(2), list2.at(2), list3.at(2) };

                    Maths::CalculExtremumInterpolation3(xtab, ytab, minmax);
                    const double offset = Date::CalculOffsetUTC(Date(minmax[0], 0.).ToQDateTime(1));
                    const Date date = Date(minmax[0] + offset + EPS_DATES, offset);

                    // Calcul de la position du satellite pour la date calculee
                    sat.CalculPosVit(date);
                    sat.CalculCoordTerrestres(date);

                    const QString typeDist = (ytab[2] >= minmax[1]) ? QObject::tr("P�rig�e :") : QObject::tr("Apog�e :");

                    // Calcul de la PSO
                    sat.CalculElementsOsculateurs(date);
                    const double pso = RAD2DEG *
                            Maths::modulo(sat.getElements().getAnomalieVraie() + sat.getElements().getArgumentPerigee(), DEUX_PI);

                    // Ecriture de la ligne de resultat
                    const QString fmt = "%1  %2�  %3� %4  %5� %6  %7 %8 %9 (%10 %9)";
                    double rayonVecteur = minmax[1];
                    double altitude = minmax[1] - RAYON_TERRESTRE;
                    if (conditions.getUnite() == QObject::tr("nmi")) {
                        rayonVecteur *= MILE_PAR_KM;
                        altitude *= MILE_PAR_KM;
                    }
                    ligne = fmt.arg(date.ToShortDateAMJ(COURT)).arg(pso, 6, 'f', 2, QChar('0')).
                            arg(fabs(sat.getLongitude() * RAD2DEG), 6, 'f', 2, QChar('0')).
                            arg((sat.getLongitude() >= 0.) ? QObject::tr("W") : QObject::tr("E")).
                            arg(fabs(sat.getLatitude()) * RAD2DEG, 5, 'f', 2, QChar('0')).
                            arg((sat.getLatitude() >= 0.) ? QObject::tr("N") : QObject::tr("S")).
                            arg(typeDist).arg(rayonVecteur, 0, 'f', 1).arg(conditions.getUnite()).
                            arg(altitude, 0, 'f', 1);

                    res.append(ligne);
                }
            }

            // Recherche des transitions jour/nuit
            if (conditions.getAtransJn()) {
                l = i;
                if (list1.at(6) * list3.at(6) < 0. && !atransJn) {

                    // Il y a une transition jour/nuit : calcul par interpolation de la date
                    atransJn = true;
                    const double ytab[] = { list1.at(6), list2.at(6), list3.at(6) };
                    const QString typeTrans = (ytab[2] < 0.) ? QObject::tr("Transition jour -> nuit") :
                                                               QObject::tr("Transition nuit -> jour");
                    CalculEvt(xtab, ytab, 0., typeTrans, conditions, sat);
                    l++;
                }
            }

            // Recherche des passages a PSO=90� et PSO=270�
            if (conditions.getApassPso()) {

                m = i;
                for (int n=1; n<4; n+=2) {

                    const double noeud = 90. * n * DEG2RAD;
                    if ((list1.at(7) - noeud) * (list3.at(7) - noeud) < 0. && list1.at(7) < noeud && !apassPso) {

                        // Il y a une transition jour/nuit : calcul par interpolation de la date
                        apassPso = true;
                        const double ytab[] = { list1.at(7), list2.at(7), list3.at(7) };
                        const QString typePso = QObject::tr("Passage � PSO =") + " " + QString::number(noeud * RAD2DEG) + "�";
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

            i = 0;

            // Tri des resultats
            res.sort();

            QString nom = sat.getTle().getNom();
            if (nom.contains("R/B") || nom.contains(" DEB"))
                nom = nom.append(QObject::tr("  (num�ro NORAD : %1)")).arg(sat.getTle().getNorad());
            flux << nom << endl;
            flux << QObject::tr("   Date     Heure      PSO    Longitude  Latitude  �v�nements") << endl;

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

    ligne = QObject::tr("Temps �coul� : %1s");
    ligne = ligne.arg(1.e-3 * fin, 0, 'f', 2);
    flux << ligne << endl;
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
    QVector<double> listVal;
    QList<QVector<double> > tab;

    /* Initialisations */

    /* Corps de la methode */
    QListIterator<Satellite> it1(sats);
    while (it1.hasNext()) {

        tab.clear();
        Satellite sat = it1.next();
        Date date = Date(conditions.getJj1(), 0., false);

        do {

            // Position du Soleil
            soleil.CalculPosition(date);

            // Position du satellite
            sat.CalculPosVit(date);
            sat.CalculSatelliteEclipse(soleil);
            sat.CalculElementsOsculateurs(date);

            // Sauvegarde des donnees
            listVal.clear();
            listVal.append(date.getJourJulienUTC());
            const Vecteur3D position = sat.getPosition();
            listVal.append(position.getZ());
            listVal.append(position.Norme());

            listVal.append(sat.getElongation());
            listVal.append(sat.getRayonApparentTerre());
            listVal.append(sat.getRayonApparentSoleil());
            listVal.append(position * soleil.getPosition());
            listVal.append(Maths::modulo(sat.getElements().getAnomalieVraie() + sat.getElements().getArgumentPerigee(), DEUX_PI));

            tab.append(listVal);

            date = Date(date.getJourJulienUTC() + PAS, 0., false);
        } while (date.getJourJulienUTC() <= conditions.getJj2());

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
    const QString fmt = "%1  %2�  %3� %4  %5� %6  %7";

    /* Corps de la methode */
    const double datp = Maths::CalculValeurXInterpolation3(xtab, ytab, yval, EPS_DATES);
    const double offset = (conditions.getEcart()) ? conditions.getOffset() : Date::CalculOffsetUTC(Date(datp, 0.).ToQDateTime(1));
    const Date date = Date(datp + offset + EPS_DATES, offset);

    // Calcul de la position du satellite pour la date calculee
    sat.CalculPosVit(date);
    sat.CalculCoordTerrestres(date);

    // Calcul de la PSO
    sat.CalculElementsOsculateurs(date);
    const double pso = RAD2DEG *
            Maths::modulo(sat.getElements().getAnomalieVraie() + sat.getElements().getArgumentPerigee(), DEUX_PI);

    // Ecriture de la ligne de resultat
    const QString ligne = fmt.arg(date.ToShortDateAMJ(COURT)).arg(pso, 6, 'f', 2, QChar('0')).
            arg(fabs(sat.getLongitude() * RAD2DEG), 6, 'f', 2, QChar('0')).
            arg((sat.getLongitude() >= 0.) ? QObject::tr("W") : QObject::tr("E")).
            arg(fabs(sat.getLatitude()) * RAD2DEG, 5, 'f', 2, QChar('0')).
            arg((sat.getLatitude() >= 0.) ? QObject::tr("N") : QObject::tr("S")).arg(typeEvt);

    res.append(ligne);

    /* Retour */
    return;
}
