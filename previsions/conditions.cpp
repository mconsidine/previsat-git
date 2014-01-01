/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2014  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    17 decembre 2013
 *
 */

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
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
    _chr = ' ';
    _nbl = 0;

    _ageTLE = 0.;
    _ang0 = 0.;
    _crep = 0.;
    _jj1 = 0.;
    _jj2 = 0.;
    _haut = 0.;
    _mgn1 = 99.;
    _mgn2 = 99.;
    _pas0 = 0.;
    _seuilConjonction = 0.;

    _fic = "";
    _out = "";
    _unite = "";
    _listeSatellites = QStringList("");

    /* Retour */
    return;
}

Conditions::Conditions(const bool ecart, const bool ecl, const bool ext, const int crep, const int haut, const int pas0, const double jj1,
                       const double jj2, const double offset, const double mgn1, const QString &fic, const QString &out,
                       const QString &unite, const QStringList &listeSatellites)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ecart = ecart;
    _ecl = ecl;
    _ext = ext;
    _crep = crep * DEG2RAD;
    _haut = haut * DEG2RAD;
    _pas0 = pas0 * NB_JOUR_PAR_SEC;
    _jj1 = jj1;
    _jj2 = jj2;
    _offset = offset;
    _mgn1 = mgn1;
    _fic = fic;
    _out = out;
    _unite = unite;
    _listeSatellites = listeSatellites;

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

Conditions::Conditions(const bool ecart, const bool ext, const int crep, const int haut, const int nbl, const char chr, const double ang0,
                       const double jj1, const double jj2, const double offset, const double mgn1, const double mgn2, const QString &fic,
                       const QString &out, const QString &unite, const QStringList &tabStsIri, const QVector<TLE> &tabtle)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ecart = ecart;
    _ext = ext;
    _crep = crep * DEG2RAD;
    _haut = haut * DEG2RAD;
    _nbl = nbl;
    _chr = chr;
    _ang0 = ang0 * DEG2RAD;
    _jj1 = jj1;
    _jj2 = jj2;
    _offset = offset;
    _mgn1 = mgn1;
    _mgn2 = mgn2;
    _fic = fic;
    _out = out;
    _unite = unite;
    _tabStsIri = tabStsIri;
    _tabtle = tabtle;

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

Conditions::Conditions(const bool apassApogee, const bool apassNoeuds, const bool apassOmbre, const bool apassPso, const bool atransJn,
                       const bool ecart, const double jj1, const double jj2, const double offset, const QString &fic,
                       const QString &out, const QString &unite, const QStringList &listeSatellites)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _apassApogee = apassApogee;
    _apassNoeuds = apassNoeuds;
    _apassOmbre = apassOmbre;
    _apassPso = apassPso;
    _atransJn = atransJn;
    _ecart = ecart;
    _jj1 = jj1;
    _jj2 = jj2;
    _offset = offset;
    _fic = fic;
    _out = out;
    _unite = unite;
    _listeSatellites = listeSatellites;

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

Conditions::Conditions(const bool acalcLune, const bool acalcSoleil, const bool ecart, const int haut,
                       const double ageTLE, const double seuilConjonction, const double jj1, const double jj2, const double offset,
                       const QString &fic, const QString &out, const QString &unite)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _acalcLune = acalcLune;
    _acalcSol = acalcSoleil;
    _ecart = ecart;
    _haut = haut * DEG2RAD;
    _ageTLE = ageTLE;
    _seuilConjonction = seuilConjonction * DEG2RAD;
    _jj1 = jj1;
    _jj2 = jj2;
    _offset = offset;
    _fic = fic;
    _out = out;
    _unite = unite;

    _apassApogee = false;
    _apassNoeuds = false;
    _apassOmbre = false;
    _apassPso = false;
    _atransJn = false;
    _ecl = false;
    _ext = false;
    _chr = ' ';
    _nbl = 0;
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
    const QString lon = Maths::ToSexagesimal(fabs(observateur.getLongitude()), DEGRE, 3, 0, false, false);
    const QString ew = (observateur.getLongitude() >= 0.) ? QObject::tr("Ouest") : QObject::tr("Est");

    const QString lat = Maths::ToSexagesimal(fabs(observateur.getLatitude()), DEGRE, 2, 0, false, false);
    const QString ns = (observateur.getLatitude() >= 0.) ? QObject::tr("Nord") : QObject::tr("Sud");

    const double alt = (conditions._unite == QObject::tr("km")) ? observateur.getAltitude() :
                                                                  observateur.getAltitude() * PIED_PAR_METRE;

    const QString unit = (conditions._unite == QObject::tr("km")) ? QObject::tr("m") : QObject::tr("ft");
    const QString cond1 = QObject::tr("Conditions d'observations :") + " ";
    const QString cond2 = QObject::tr("Hauteur minimale du satellite = %1°");

    /* Corps de la methode */
    const double offset = (conditions._ecart) ? conditions._offset :
                                                Date::CalculOffsetUTC(Date(conditions._jj1, 0.).ToQDateTime(1).toLocalTime());
    const Date date = Date(conditions._jj1 + offset + EPS_DATES, 0.);

    // Calcul de l'age des TLE
    if (tabtle.size() == 1) {
        ligne1 = QObject::tr("Age du TLE                : %1 jours (au %2)");
        ligne1 = ligne1.arg(fabs(conditions._jj1 - tabtle.at(0).getEpoque().getJourJulienUTC()), 4, 'f', 2).
                arg(date.ToShortDate(COURT));

    } else {
        double tlemin = -DATE_INFINIE;
        double tlemax = DATE_INFINIE;

        QVectorIterator<TLE> it(tabtle);
        while (it.hasNext()) {
            const TLE tle = it.next();
            const double epok = tle.getEpoque().getJourJulienUTC();
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
        ligne1 = ligne1.arg(fabs(conditions._jj1 - tlemin), 4, 'f', 2).arg(date.ToShortDate(COURT)).
                arg(fabs(conditions._jj1 - tlemax), 4, 'f', 2);
    }

    QFile fichier(conditions._out);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    const QString titre = "%1 %2 / %3 (c) %4";
    flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
            arg(QString(APP_ANNEES_DEV)) << endl << endl;

    QString ligne2 = QObject::tr("Lieu d'observation        : %1     %2 %3   %4 %5   %6 %7");
    ligne2 = ligne2.arg(observateur.getNomlieu()).arg(lon).arg(ew).arg(lat).arg(ns).arg(1000. * alt, 0, 'f', 0).arg(unit);
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
bool Conditions::getEcart() const
{
    return _ecart;
}

bool Conditions::getEcl() const
{
    return _ecl;
}

bool Conditions::getExt() const
{
    return _ext;
}

double Conditions::getCrep() const
{
    return _crep;
}

double Conditions::getHaut() const
{
    return _haut;
}

double Conditions::getPas0() const
{
    return _pas0;
}

double Conditions::getJj1() const
{
    return _jj1;
}

double Conditions::getJj2() const
{
    return _jj2;
}

double Conditions::getOffset() const
{
    return _offset;
}

double Conditions::getMgn1() const
{
    return _mgn1;
}

QString Conditions::getFic() const
{
    return _fic;
}

QString Conditions::getOut() const
{
    return _out;
}

QString Conditions::getUnite() const
{
    return _unite;
}

QStringList Conditions::getListeSatellites() const
{
    return _listeSatellites;
}


int Conditions::getNbl() const
{
    return _nbl;
}

char Conditions::getChr() const
{
    return _chr;
}

double Conditions::getAng0() const
{
    return _ang0;
}

double Conditions::getMgn2() const
{
    return _mgn2;
}

QStringList Conditions::getTabStsIri() const
{
    return _tabStsIri;
}

QVector<TLE> Conditions::getTabtle() const
{
    return _tabtle;
}

bool Conditions::getApassApogee() const
{
    return _apassApogee;
}

bool Conditions::getApassNoeuds() const
{
    return _apassNoeuds;
}

bool Conditions::getApassOmbre() const
{
    return _apassOmbre;
}

bool Conditions::getApassPso() const
{
    return _apassPso;
}

bool Conditions::getAtransJn() const
{
    return _atransJn;
}


bool Conditions::getAcalcLune() const
{
    return _acalcLune;
}

bool Conditions::getAcalcSol() const
{
    return _acalcSol;
}

double Conditions::getAgeTLE() const
{
    return _ageTLE;
}

double Conditions::getSeuilConjonction() const
{
    return _seuilConjonction;
}
