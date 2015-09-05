/*
 *     PreviSat, Satellite tracking software
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
 * >    5 septembre 2015
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "conditions.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

/*
 * Constructeurs
 */
Conditions::Conditions()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _acalcLune = false;
    _acalcSol = false;
    _apassApogee = false;
    _apassNoeuds = false;
    _apassOmbre = false;
    _apassPso = false;
    _atransJn = false;
    _ecart = false;
    _ecl = false;
    _ext = false;
    _refr = false;
    _syst = false;
    _chr = true;
    _psol = true;
    _nbl = 0;

    _ageTLE = 0.;
    _ang0 = 0.;
    _crep = 0.;
    _jj1 = 0.;
    _jj2 = 0.;
    _haut = 0.;
    _mgn1 = 99.;
    _mgn2 = 99.;
    _offset = 0.;
    _pas0 = 0.;
    _seuilConjonction = 0.;

    _fic = "";
    _out = "";
    _unite = "";
    _listeSatellites = QStringList("");

    /* Retour */
    return;
}

Conditions::Conditions(const bool pecEcart, const bool eclipse, const bool extinction, const bool refraction, const bool systeme,
                       const int crepuscule, const int hauteur, const int pas, const double jourJulien1, const double jourJulien2,
                       const double offsetUTC, const double magn1, const QString &ficEnt, const QString &ficOut, const QString &unit,
                       const QStringList &listeSat)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ecart = pecEcart;
    _ecl = eclipse;
    _ext = extinction;
    _refr = refraction;
    _syst = systeme;
    _crep = crepuscule * DEG2RAD;
    _haut = hauteur * DEG2RAD;
    _pas0 = pas * NB_JOUR_PAR_SEC;
    _jj1 = jourJulien1;
    _jj2 = jourJulien2;
    _offset = offsetUTC;
    _mgn1 = magn1;
    _fic = ficEnt;
    _out = ficOut;
    _unite = unit;
    _listeSatellites = listeSat;

    _acalcLune = false;
    _acalcSol = false;
    _apassApogee = false;
    _apassNoeuds = false;
    _apassOmbre = false;
    _apassPso = false;
    _atransJn = false;
    _chr = ' ';
    _nbl = 0;
    _ageTLE = 0.;
    _ang0 = 0.;
    _mgn2 = 99.;
    _seuilConjonction = 0.;

    /* Retour */
    return;
}

Conditions::Conditions(const bool pecEcart, const bool extinction, const bool refraction, const bool systeme, const bool chrono,
                       const bool panSol, const int crepuscule, const int hauteur, const int nbLig, const double angle0,
                       const double jourJulien1, const double jourJulien2, const double offsetUTC, const double magn1,
                       const double magn2, const QString &ficEnt, const QString &ficOut, const QString &unit,
                       const QStringList &tabStsIridium, const QVector<TLE> &tabTLEIri)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ecart = pecEcart;
    _ext = extinction;
    _refr = refraction;
    _syst = systeme;
    _crep = crepuscule * DEG2RAD;
    _haut = hauteur * DEG2RAD;
    _nbl = nbLig;
    _chr = chrono;
    _psol = panSol;
    _ang0 = angle0 * DEG2RAD;
    _jj1 = jourJulien1;
    _jj2 = jourJulien2;
    _offset = offsetUTC;
    _mgn1 = magn1;
    _mgn2 = magn2;
    _fic = ficEnt;
    _out = ficOut;
    _unite = unit;
    _tabStsIri = tabStsIridium;
    _tabtle = tabTLEIri;

    _acalcLune = false;
    _acalcSol = false;
    _apassApogee = false;
    _apassNoeuds = false;
    _apassOmbre = false;
    _apassPso = false;
    _atransJn = false;
    _ecl = false;
    _ageTLE = 0.;
    _pas0 = 0.;
    _seuilConjonction = 0.;

    /* Retour */
    return;
}

Conditions::Conditions(const bool apassageApogee, const bool apassageNoeuds, const bool apassageOmbre, const bool apassagePso,
                       const bool atransitionJn, const bool pecEcart, const bool refraction, const bool systeme,
                       const double jourJulien1, const double jourJulien2, const double offsetUTC, const QString &ficEnt,
                       const QString &ficOut, const QString &unit, const QStringList &listeSat)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _apassApogee = apassageApogee;
    _apassNoeuds = apassageNoeuds;
    _apassOmbre = apassageOmbre;
    _apassPso = apassagePso;
    _atransJn = atransitionJn;
    _ecart = pecEcart;
    _refr = refraction;
    _syst = systeme;
    _jj1 = jourJulien1;
    _jj2 = jourJulien2;
    _offset = offsetUTC;
    _fic = ficEnt;
    _out = ficOut;
    _unite = unit;
    _listeSatellites = listeSat;

    _acalcLune = false;
    _acalcSol = false;
    _ecl = false;
    _ext = false;
    _chr = ' ';
    _nbl = 0;
    _ageTLE = 0.;
    _ang0 = 0.;
    _crep = 0.;
    _haut = 0.;
    _mgn1 = 99.;
    _mgn2 = 99.;
    _pas0 = 0.;
    _seuilConjonction = 0.;

    /* Retour */
    return;
}

Conditions::Conditions(const bool acalculLune, const bool acalculSoleil, const bool pecEcart, const bool refraction, const bool systeme,
                       const int hauteur, const double age, const double seuilConj, const double jourJulien1,
                       const double jourJulien2, const double offsetUTC, const QString &ficEnt, const QString &ficOut,
                       const QString &unit)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _acalcLune = acalculLune;
    _acalcSol = acalculSoleil;
    _ecart = pecEcart;
    _refr = refraction;
    _syst = systeme;
    _haut = hauteur * DEG2RAD;
    _ageTLE = age;
    _seuilConjonction = seuilConj * DEG2RAD;
    _jj1 = jourJulien1;
    _jj2 = jourJulien2;
    _offset = offsetUTC;
    _fic = ficEnt;
    _out = ficOut;
    _unite = unit;

    _apassApogee = false;
    _apassNoeuds = false;
    _apassOmbre = false;
    _apassPso = false;
    _atransJn = false;
    _ecl = false;
    _ext = false;
    _chr = ' ';
    _nbl = -1;
    _ang0 = 0.;
    _crep = 0.;
    _mgn1 = 99.;
    _mgn2 = 99.;
    _pas0 = 0.;

    /* Retour */
    return;
}

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
        double tlemin = -DATE_INFINIE;
        double tlemax = DATE_INFINIE;

        QVectorIterator<TLE> it(tabtle);
        while (it.hasNext()) {
            const TLE tle = it.next();
            const double epok = tle.epoque().jourJulienUTC();
            if (epok > tlemin)
                tlemin = epok;
            if (epok < tlemax)
                tlemax = epok;
        }

        if (tlemax > conditions._jj1) {
            if (tlemin > tlemax) {
                const double tmp = tlemin;
                tlemin = tlemax;
                tlemax = tmp;
            }
        }

        ligne1 = QObject::tr("Age du TLE le plus récent : %1 jours (au %2)\nAge du TLE le plus ancien : %3 jours");
        ligne1 = ligne1.arg(fabs(conditions._jj1 - tlemin), 4, 'f', 2).
                arg(date.ToShortDate(FORMAT_COURT, (conditions._syst) ? SYSTEME_24H : SYSTEME_12H).trimmed()).
                arg(fabs(conditions._jj1 - tlemax), 4, 'f', 2);
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
    if (conditions._ecart) {
        if (fabs(conditions._offset) > EPSDBL100) {
            QTime heur;
            heur = heur.addSecs((int) (conditions._offset * NB_SEC_PAR_JOUR + EPS_DATES));
            chaine = chaine.append((conditions._offset > 0.) ? " + " : " - ").append(heur.toString("hh:mm"));
        }
    }
    flux << QString((conditions._ecart) ? ligne2.arg(chaine) : ligne2.arg(QObject::tr("Heure légale"))) << endl;

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


int Conditions::nbl() const
{
    return _nbl;
}

bool Conditions::chr() const
{
    return _chr;
}

bool Conditions::psol() const
{
    return _psol;
}

double Conditions::ang0() const
{
    return _ang0;
}

double Conditions::mgn2() const
{
    return _mgn2;
}

QStringList Conditions::tabStsIri() const
{
    return _tabStsIri;
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
