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
 * >
 *
 */

#include <QFile>
#include <QTextStream>
#include "conditions.h"
#include "librairies/maths/maths.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

Conditions::Conditions(const bool ecl, const bool ext, const int crep, const int haut, const int pas0, const double dtu, const double jj1, const double jj2, const double mgn1, const QString fic, const QString out, const QString unite, const QStringList listeSatellites)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ecl = ecl;
    _ext = ext;
    _crep = crep * DEG2RAD;
    _haut = haut * DEG2RAD;
    _pas0 = pas0 * NB_JOUR_PAR_SEC;
    _dtu = dtu;
    _jj1 = jj1;
    _jj2 = jj2;
    _mgn1 = mgn1;
    _fic = fic;
    _out = out;
    _unite = unite;
    _listeSatellites = listeSatellites;

    /* Retour */
    return;
}

Conditions::Conditions(const bool ext, const int crep, const int haut, const int nbl, const char chr, const char ope, const double ang0, const double dtu, const double jj1, const double jj2, const double mgn1, const double mgn2, const QString fic, const QString out, const QString unite)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ext = ext;
    _crep = crep * DEG2RAD;
    _haut = haut * DEG2RAD;
    _nbl = nbl;
    _chr = chr;
    _ope = ope;
    _ang0 = ang0 * DEG2RAD;
    _dtu = dtu;
    _jj1 = jj1;
    _jj2 = jj2;
    _mgn1 = mgn1;
    _mgn2 = mgn2;
    _fic = fic;
    _out = out;
    _unite = unite;

    /* Retour */
    return;
}

Conditions::Conditions(const bool apassApogee, const bool apassNoeuds, const bool apassOmbre, const bool apassPso, const bool atransJn, const double dtu, const double jj1, const double jj2, const QString fic, const QString out, const QString unite, const QStringList listeSatellites)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _apassApogee = apassApogee;
    _apassNoeuds = apassNoeuds;
    _apassOmbre = apassOmbre;
    _apassPso = apassPso;
    _atransJn = atransJn;
    _dtu = dtu;
    _jj1 = jj1;
    _jj2 = jj2;
    _fic = fic;
    _out = out;
    _unite = unite;
    _listeSatellites = listeSatellites;

    /* Retour */
    return;
}

Conditions::Conditions(const bool acalcLune, const bool acalcSoleil, const int haut, const double ageTLE, const double seuilConjonction, const double dtu, const double jj1, const double jj2, const QString fic, const QString out, const QString unite)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _acalcLune = acalcLune;
    _acalcSol = acalcSoleil;
    _haut = haut * DEG2RAD;
    _ageTLE = ageTLE;
    _seuilConjonction = seuilConjonction * DEG2RAD;
    _dtu = dtu;
    _jj1 = jj1;
    _jj2 = jj2;
    _fic = fic;
    _out = out;
    _unite = unite;

    /* Retour */
    return;
}

/*
 * Ecriture de l'entete du fichier de resultats
 */
void Conditions::EcrireEntete(const Observateur &observateur, const Conditions &conditions, QVector<TLE> &tabtle, const bool itransit)
{
    /* Declarations des variables locales */
    double tlemin, tlemax, tmp;
    QString ligne1, ligne2;

    /* Initialisations */
    ligne1 = "";
    const QString lon = Maths::ToSexagesimal(fabs(observateur.getLongitude()), Maths::DEGRE, 3, 0, false, false);
    const QString ew = (observateur.getLongitude() >= 0.) ? QObject::tr("Ouest") : QObject::tr("Est");
    const QString lat = Maths::ToSexagesimal(fabs(observateur.getLatitude()), Maths::DEGRE, 2, 0, false, false);
    const QString ns = (observateur.getLatitude() >= 0.) ? QObject::tr("Nord") : QObject::tr("Sud");
    const double alt = (conditions._unite == QObject::tr("km")) ? observateur.getAltitude() : observateur.getAltitude() * PIED_PAR_METRE;
    const QString unit = (conditions._unite == QObject::tr("km")) ? QObject::tr("m") : QObject::tr("ft");
    const QString cond1 = QObject::tr("Conditions d'observations : ");
    const QString cond2 = QObject::tr("Hauteur minimale du satellite = %1�");

    /* Corps de la methode */
    const Date date = Date(conditions._jj1 + conditions._dtu + EPS_DATES, 0.);

    // Calcul de l'age des TLE
    if (tabtle.size() == 1) {
        ligne1 = QObject::tr("Age du TLE                : %1 jours (au %2)");
        ligne1 = ligne1.arg(fabs(conditions._jj1 - tabtle.at(0).getEpoque().getJourJulienUTC()), 4, 'f', 2).arg(date.ToShortDate(Date::COURT));

    } else {
        tlemin = -DATE_INFINIE;
        tlemax = DATE_INFINIE;

        QVectorIterator<TLE> it(tabtle);
        while (it.hasNext()) {
            TLE tle = it.next();
            double epok = tle.getEpoque().getJourJulienUTC();
            if (epok > tlemin)
                tlemin = epok;
            if (epok < tlemax)
                tlemax = epok;
        }

        if (tlemax > conditions._jj1) {
            if (tlemin > tlemax) {
                tmp = tlemin;
                tlemin = tlemax;
                tlemax = tmp;
            }
        }

        ligne1 = QObject::tr("Age du TLE le plus r�cent : %1 jours (au %2)\nAge du TLE le plus ancien : %3 jours");
        ligne1 = ligne1.arg(fabs(conditions._jj1 - tlemin), 4, 'f', 2).arg(date.ToShortDate(Date::COURT)).arg(fabs(conditions._jj1 - tlemax), 4, 'f', 2);
    }

    QFile fichier(conditions._out);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    flux << QString("PreviSat 3.0 / Astropedia (c) 2005-2011") << endl << endl;
    ligne2 = QObject::tr("Lieu d'observation        : %1     %2 %3   %4 %5   %6 %7");
    ligne2 = ligne2.arg(observateur.getNomlieu()).arg(lon).arg(ew).arg(lat).arg(ns).arg(1000. * alt).arg(unit);
    flux << ligne2 << endl;

    ligne2 = QObject::tr("Fuseau horaire            : %1 %2%3");
    ligne2 = ligne2.arg(QObject::tr("UTC")).arg((conditions._dtu >= 0.) ? "+" : "-").arg(Maths::ToSexagesimal(NB_HEUR_PAR_JOUR * HEUR2RAD * fabs(conditions._dtu), Maths::HEURE1, 2, 0, false, false).mid(0, 6));
    flux << ligne2 << endl;

    if (itransit) {
        flux << (cond1 + cond2).arg(conditions._haut * RAD2DEG) << endl;

    } else {
        flux << cond1 + QObject::tr("Hauteur maximale du Soleil = %1�").arg(conditions._crep * RAD2DEG) << endl;
        flux << QString(cond1.size(), ' ') << cond2.arg(conditions._haut * RAD2DEG) << endl;
    }

    flux << QObject::tr("Unit� de distance         : %1").arg(conditions._unite) << endl << endl;
    flux << ligne1 << endl << endl << endl;
    fichier.close();

    /* Retour */
    return;
}

/* Accesseurs */
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

double Conditions::getDtu() const
{
    return _dtu;
}

double Conditions::getJj1() const
{
    return _jj1;
}

double Conditions::getJj2() const
{
    return _jj2;
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

char Conditions::getOpe() const
{
    return _ope;
}

double Conditions::getAng0() const
{
    return _ang0;
}

double Conditions::getMgn2() const
{
    return _mgn2;
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
