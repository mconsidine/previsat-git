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
 * >    date.cpp
 *
 * Localisation
 * >    librairies.dates
 *
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires sur les dates
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    25 octobre 2013
 *
 */

#include <cmath>
#include <ctime>
#include <fstream>
#include "date.h"
#include "librairies/maths/maths.h"

/* Constructeurs */
Date::Date()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _annee = 0;
    _mois = 0;
    _jour = 0;
    _heure = 0;
    _minutes = 0;
    _secondes = 0.;

    _jourJulien = 0.;
    _jourJulienUTC = 0.;
    _offsetUTC = 0.;

    /* Retour */
    return;
}

/*
 * Constructeur par defaut : obtention de la date systeme
 */
Date::Date(const double offsetUTC)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    const QDateTime dateSysteme = QDateTime::currentDateTime();

    _annee = dateSysteme.date().year();
    _mois = dateSysteme.date().month();
    _jour = dateSysteme.date().day();
    _heure = dateSysteme.time().hour();
    _minutes = dateSysteme.time().minute();
    _secondes = dateSysteme.time().second();

    _offsetUTC = offsetUTC;

    CalculJourJulien();

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une date
 */
Date::Date(const Date &date, const double offsetUTC)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _annee = date._annee;
    _mois = date._mois;
    _jour = date._jour;
    _heure = date._heure;
    _minutes = date._minutes;
    _secondes = date._secondes;

    _offsetUTC = offsetUTC;
    _jourJulien = date._jourJulien;
    _jourJulienUTC = _jourJulien - offsetUTC;

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un jour julien 2000
 */
Date::Date(const double jourJulien, const double offsetUTC, const bool acalc)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    if (acalc) {

        int a;
        const double j1 = jourJulien + 0.5;
        int z = (int) floor(j1);
        const double f = j1 - z;
        z += (int) (TJ2000 + EPSDBL100);

        if (z < 2299161) {
            a = z;
        } else {
            const int al = (int) floor((z - 1867216.25) / 36524.25);
            a = z + 1 + al - al / 4;
        }

        const int b = a + 1524;
        const int c = (int) floor((b - 122.1) * NB_ANJ_PAR_JOURS);
        const int d = (int) floor(NB_JOURS_PAR_ANJ * c);
        const int e = (int) floor((b - d) / 30.6001);
        const double j0 = b - d - floor(30.6001 * e) + f;

        _mois = (e < 14) ? e - 1 : e - 13;
        _annee = (_mois > 2) ? c - 4716 : c - 4715;
        _jour = (int) floor(j0);

        _heure = (int) floor(NB_HEUR_PAR_JOUR * (j0 - _jour));
        _minutes = (int) floor(NB_MIN_PAR_JOUR * (j0 - _jour) - NB_MIN_PAR_HEUR * _heure);
        _secondes = NB_SEC_PAR_JOUR * (j0 - _jour) - NB_SEC_PAR_HEUR * _heure - NB_SEC_PAR_MIN * _minutes;
    }

    _offsetUTC = offsetUTC;
    _jourJulien = jourJulien;
    _jourJulienUTC = _jourJulien - _offsetUTC;

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une date calendaire, ou le jour est exprime sous forme decimale
 */
Date::Date(const int annee, const int mois, const double xjour, const double offsetUTC)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _annee = annee;
    _mois = mois;
    _jour = (int) floor(xjour);
    _heure = (int) floor(NB_HEUR_PAR_JOUR * (xjour - _jour));
    _minutes = (int) floor(NB_MIN_PAR_JOUR * (xjour - _jour) - NB_MIN_PAR_HEUR * _heure);
    _secondes = NB_SEC_PAR_JOUR * (xjour - _jour) - NB_SEC_PAR_HEUR * _heure - NB_SEC_PAR_MIN * _minutes;

    _offsetUTC = offsetUTC;

    CalculJourJulien();

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une date calendaire
 */
Date::Date(const int annee, const int mois, const int jour, const int heure, const int minutes,
           const double secondes, const double offsetUTC)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _annee = annee;
    _mois = mois;
    _jour = jour;
    _heure = heure;
    _minutes = minutes;
    _secondes = secondes;

    _offsetUTC = offsetUTC;

    CalculJourJulien();

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Date::~Date()
{
}


/* Methodes */
/*
 * Calcul de l'ecart Heure locale - UTC
 */
double Date::CalculOffsetUTC(const QDateTime &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QDateTime utc(date);
    utc.setTimeSpec(Qt::UTC);

    /* Corps de la methode */

    /* Retour */
    return (date.secsTo(utc) * NB_JOUR_PAR_SEC);
}

/*
 * Conversion de la date en QDateTime
 */
QDateTime Date::ToQDateTime(const int type) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const int sec = (type == 0) ? 0 : qRound(_secondes);

    /* Retour */
    return (QDateTime(QDate(_annee, _mois, _jour), QTime(_heure, _minutes, sec)));
}

QString Date::ToShortDate(const DateFormat &format, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDateTime date = QDateTime(QDate(_annee, _mois, _jour), QTime(0, 0, 0));
    const int fmt = (format == COURT) ? 0 : 1;
    const double tmp = floor(_jourJulien);
    const QString chaine = " %1:%2:%3%4";

    /* Corps de la methode */
    const double jjsec = Maths::arrondi(NB_SEC_PAR_JOUR * (_jourJulien - tmp), fmt) * NB_JOUR_PAR_SEC + tmp + EPSDBL100;
    Date date2 = Date(jjsec, _offsetUTC);

    int heure = date2._heure;
    QString sys = " ";
    if (systeme == SYSTEME_12H) {
        heure = date2._heure%12;
        sys = (heure >= 0 && date2._heure < 12) ? "a" : "p";
        if (heure == 0)
            heure = 12;
    }

    const QString res = date.toString(QObject::tr("dd/MM/yyyy")) + chaine.arg(heure, 2, 10, QChar('0')).
                        arg(date2._minutes, 2, 10, QChar('0')).arg(date2._secondes, 2 * (fmt + 1), 'f', fmt, QChar('0')).arg(sys);

    /* Retour */
    return (res);
}

QString Date::ToShortDateAMJ(const DateFormat &format, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int fmt = (format == COURT) ? 0 : 1;
    const double tmp = floor(_jourJulien);
    QString res = "%1/%2/%3 %4:%5:%6%7";

    /* Corps de la methode */
    const double jjsec = Maths::arrondi(NB_SEC_PAR_JOUR * (_jourJulien - tmp), fmt) * NB_JOUR_PAR_SEC + tmp + EPSDBL100;
    Date date = Date(jjsec, _offsetUTC);
    int heure = date._heure;
    QString sys = " ";

    if (systeme == SYSTEME_12H) {
        heure = date._heure%12;
        sys = (heure >= 0 && date._heure < 12) ? "a" : "p";
        if (heure == 0)
            heure = 12;
    }

    /* Retour */
    return (res.arg(date._annee, 4, 10, QChar('0')).arg(date._mois, 2, 10, QChar('0')).
            arg(date._jour, 2, 10, QChar('0')).arg(heure, 2, 10, QChar('0')).
            arg(date._minutes, 2, 10, QChar('0')).arg(date._secondes, 2 * (fmt + 1), 'f', fmt, QChar('0')).arg(sys));
}

/*
 * Conversion de la date en chaine de caracteres
 */
QString Date::ToLongDate(const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDateTime date = QDateTime(QDate(_annee, _mois, _jour), QTime(_heure, _minutes, (int) (_secondes + EPS_DATES)));

    /* Corps de la methode */
    QString res = date.toString(QObject::tr("dddd dd MMMM yyyy hh:mm:ss") + ((systeme == SYSTEME_12H) ? "a" : ""));
    res[0] = res[0].toUpper();

    /* Retour */
    return (res);
}

/*
 * Calcul du jour julien 2000
 */
void Date::CalculJourJulien()
{
    /* Declarations des variables locales */

    /* Initialisations */
    int d = _annee;
    int n = _mois;
    const double xj = _jour + _heure * NB_JOUR_PAR_HEUR + _minutes * NB_JOUR_PAR_MIN + _secondes *
            NB_JOUR_PAR_SEC;

    /* Corps de la methode */
    if (n < 3) {
        d--;
        n+=12;
    }

    const int c = d / 100;
    const int b = 2 - c + c / 4;
    d -= AN2000;

    _jourJulien = floor(NB_JOURS_PAR_ANJ * d) + floor(30.6001 * (n + 1)) + xj + b - 50.5;
    _jourJulienUTC = _jourJulien - _offsetUTC;

    /* Retour */
    return;
}

/* Accesseurs */
int Date::getAnnee() const
{
    return _annee;
}

int Date::getHeure() const
{
    return _heure;
}

int Date::getJour() const
{
    return _jour;
}

double Date::getJourJulien() const
{
    return _jourJulien;
}

double Date::getJourJulienUTC() const
{
    return _jourJulienUTC;
}

int Date::getMinutes() const
{
    return _minutes;
}

int Date::getMois() const
{
    return _mois;
}

double Date::getOffsetUTC() const
{
    return _offsetUTC;
}

double Date::getSecondes() const
{
    return _secondes;
}
