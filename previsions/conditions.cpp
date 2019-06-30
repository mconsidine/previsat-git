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
 * >    conditions.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Parametres pour le calcul des previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    30 juin 2019
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "conditions.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

/*
 * Constructeurs
 */

/*
 * Destructeur
 */
Conditions::~Conditions()
{
}

/*
 * Ecriture de l'entete du fichier de resultats
 */
void Conditions::EcrireEntete(const Observateur &observateur, const Conditions &conditions, QVector<TLE> &tabtle,
                              const bool itransit)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString ligne1 = "";
    const QString lon = Maths::ToSexagesimal(fabs(observateur.longitude()), DEGRE, 3, 0, false, false);
    const QString ew = (observateur.longitude() >= 0.) ? QObject::tr("Ouest") : QObject::tr("Est");

    const QString lat = Maths::ToSexagesimal(fabs(observateur.latitude()), DEGRE, 2, 0, false, false);
    const QString ns = (observateur.latitude() >= 0.) ? QObject::tr("Nord") : QObject::tr("Sud");

    const double alt = (conditions._unite == QObject::tr("km")) ? observateur.altitude() :
                                                                  observateur.altitude() * PIED_PAR_METRE;

    const QString unit = (conditions._unite == QObject::tr("km")) ? QObject::tr("m") : QObject::tr("ft");
    const QString cond1 = QObject::tr("Conditions d'observations :") + " ";
    const QString cond2 = QObject::tr("Hauteur minimale du satellite = %1°");

    /* Corps de la methode */
    const double offset = (conditions._ecart) ? conditions._offset :
                                                Date::CalculOffsetUTC(Date(conditions._jj1, 0.).ToQDateTime(1).toLocalTime());
    const Date date(conditions._jj1 + offset + EPS_DATES, 0.);

    // Calcul de l'age des TLE
    if (tabtle.size() == 1) {
        ligne1 = QObject::tr("Age du TLE                : %1 jours (au %2)");
        ligne1 = ligne1.arg(fabs(conditions._jj1 - tabtle.at(0).epoque().jourJulienUTC()), 4, 'f', 2).
                arg(date.ToShortDate(FORMAT_COURT, (conditions._syst) ? SYSTEME_24H : SYSTEME_12H).trimmed());

    } else {

        double age1 = 0.;
        double age2 = 0.;

        if (itransit) {

            const double date1 = tabtle.first().epoque().jourJulienUTC();
            const double date2 = tabtle.last().epoque().jourJulienUTC();

            age1 = qMin(conditions._jj1 - date1, date2 - conditions._jj1);
            age2 = qMin(date1 - conditions._jj2, conditions._jj2 - date2);

            if ((age1 < 0.) && (age2 < 0.)) {
                ligne1 = QObject::tr("Age du TLE le plus ancien : %1 jours");
                ligne1 = ligne1.arg(fabs(qMax(age1, age2)), 4, 'f', 2);
            }
        } else {

            double tlemin = -DATE_INFINIE;
            double tlemax = DATE_INFINIE;

            QVectorIterator<TLE> it(tabtle);
            while (it.hasNext()) {
                const TLE tle = it.next();
                const double epok = tle.epoque().jourJulienUTC();
                if (epok > tlemin) tlemin = epok;
                if (epok < tlemax) tlemax = epok;
            }

            if (tlemax > conditions._jj1 || tlemin > conditions._jj1) {
                if (tlemin > tlemax) {
                    const double tmp = tlemin;
                    tlemin = tlemax;
                    tlemax = tmp;
                }
            }

            age1 = fabs(conditions._jj1 - tlemin);
            age2 = fabs(conditions._jj1 - tlemax);

            ligne1 = QObject::tr("Age du TLE le plus récent : %1 jours (au %2)\nAge du TLE le plus ancien : %3 jours");
            ligne1 = ligne1.arg(age1, 4, 'f', 2).
                    arg(date.ToShortDate(FORMAT_COURT, (conditions._syst) ? SYSTEME_24H : SYSTEME_12H).trimmed()).arg(age2, 4, 'f', 2);
        }
    }

    QFile fichier(conditions._out);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    const QString titre = "%1 %2 / %3 (c) %4";
    flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
            arg(QString(APP_ANNEES_DEV)) << endl << endl;

    QString ligne2 = QObject::tr("Lieu d'observation        : %1     %2 %3   %4 %5   %6 %7");
    ligne2 = ligne2.arg(observateur.nomlieu()).arg(lon).arg(ew).arg(lat).arg(ns).arg(1000. * alt, 0, 'f', 0).arg(unit);
    flux << ligne2 << endl;

    ligne2 = QObject::tr("Fuseau horaire            : %1");
    QString chaine = QObject::tr("UTC");
    if (fabs(conditions._offset) > EPSDBL100) {
        QTime heur;
        heur = heur.addSecs((int) (conditions._offset * NB_SEC_PAR_JOUR + EPS_DATES));
        chaine = chaine.append((conditions._offset > 0.) ? " + " : " - ").append(heur.toString("hh:mm"));
    }
    flux << ligne2.arg(chaine) << endl;

    if (itransit) {
        flux << (cond1 + cond2).arg(conditions._haut * RAD2DEG) << endl;

    } else {
        flux << cond1 + QObject::tr("Hauteur maximale du Soleil = %1°").arg(conditions._crep * RAD2DEG) << endl;
        flux << QString(cond1.size(), ' ') << cond2.arg(conditions._haut * RAD2DEG) << endl;
    }

    flux << QObject::tr("Unité de distance         : %1").arg(conditions._unite) << endl << endl;
    flux << ligne1 << endl << endl << endl;
    fichier.close();

    /* Retour */
    return;
}

/* Accesseurs */
bool Conditions::ecart() const
{
    return _ecart;
}

bool Conditions::ecl() const
{
    return _ecl;
}

bool Conditions::ext() const
{
    return _ext;
}

bool Conditions::refr() const
{
    return _refr;
}

bool Conditions::acalcEclipseLune() const
{
    return _acalcEclipseLune;
}

bool Conditions::effetEclipsePartielle() const
{
    return _effetEclipsePartielle;
}

bool Conditions::syst() const
{
    return _syst;
}

double Conditions::crep() const
{
    return _crep;
}

double Conditions::haut() const
{
    return _haut;
}

double Conditions::pas0() const
{
    return _pas0;
}

double Conditions::jj1() const
{
    return _jj1;
}

double Conditions::jj2() const
{
    return _jj2;
}

double Conditions::offset() const
{
    return _offset;
}

double Conditions::mgn1() const
{
    return _mgn1;
}

QString Conditions::fic() const
{
    return _fic;
}

QString Conditions::out() const
{
    return _out;
}

QString Conditions::unite() const
{
    return _unite;
}

QStringList Conditions::listeSatellites() const
{
    return _listeSatellites;
}

TypeCalcul Conditions::typeCalcul() const
{
    return _typeCalcul;
}


int Conditions::nbl() const
{
    return _nbl;
}

bool Conditions::chr() const
{
    return _chr;
}

double Conditions::ang0() const
{
    return _ang0;
}

QStringList Conditions::tabSts() const
{
    return _tabSts;
}

QVector<TLE> Conditions::tabtle() const
{
    return _tabtle;
}

bool Conditions::apassApogee() const
{
    return _apassApogee;
}

bool Conditions::apassNoeuds() const
{
    return _apassNoeuds;
}

bool Conditions::apassOmbre() const
{
    return _apassOmbre;
}

bool Conditions::apassPso() const
{
    return _apassPso;
}

bool Conditions::atransJn() const
{
    return _atransJn;
}


bool Conditions::acalcLune() const
{
    return _acalcLune;
}

bool Conditions::acalcSol() const
{
    return _acalcSol;
}

double Conditions::ageTLE() const
{
    return _ageTLE;
}

double Conditions::seuilConjonction() const
{
    return _seuilConjonction;
}
