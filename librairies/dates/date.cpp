/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    8 decembre 2019
 *
 */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <cmath>
#include <ctime>
#include <fstream>
#include <QCoreApplication>
#include <QDir>
#include <QObject>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"

QList<QList<double> > Date::_ecartsTAI_UTC;


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
    _jourJulienTT = 0.;
    _jourJulienUTC = 0.;
    _offsetUTC = 0.;

    _deltaAT = 0.;

    /* Retour */
    return;
}

/*
 * Constructeur par defaut : obtention de la date systeme
 */
Date::Date(const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    const QDateTime dateSysteme = QDateTime::currentDateTimeUtc();

    _annee = dateSysteme.date().year();
    _mois = dateSysteme.date().month();
    _jour = dateSysteme.date().day();
    _heure = dateSysteme.time().hour();
    _minutes = dateSysteme.time().minute();
    _secondes = dateSysteme.time().second();

    _offsetUTC = offset;

    CalculJourJulien();

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une date
 */
Date::Date(const Date &date, const double offset)
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

    _offsetUTC = offset;
    _jourJulienUTC = date._jourJulienUTC;
    _jourJulien = _jourJulienUTC + offset;
    getDeltaAT();
    _jourJulienTT = _jourJulienUTC + (NB_SEC_TT_TAI + _deltaAT) * NB_JOUR_PAR_SEC;

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un jour julien 2000
 */
Date::Date(const double jourJulien2000, const double offset, const bool acalc)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    if (acalc) {

        int a;
        const double j1 = jourJulien2000 + 0.5;
        int z = (int) floor(j1);
        const double f = j1 - z;
        z += (int) (TJ2000 + EPSDBL100);

        if (z < 2299161) {
            a = z;
        } else {
            const int al = (int) floor((z - 1867216.25) * (1. / 36524.25));
            a = z + 1 + al - al / 4;
        }

        const int b = a + 1524;
        const int c = (int) floor((b - 122.1) * NB_ANJ_PAR_JOURS);
        const int d = (int) floor(NB_JOURS_PAR_ANJ * c);
        const int e = (int) floor((b - d) * (1. / 30.6001));
        const double j0 = b - d - floor(30.6001 * e) + f;

        _mois = (e < 14) ? e - 1 : e - 13;
        _annee = (_mois > 2) ? c - 4716 : c - 4715;
        _jour = (int) floor(j0);

        _heure = (int) floor(NB_HEUR_PAR_JOUR * (j0 - _jour));
        _minutes = (int) floor(NB_MIN_PAR_JOUR * (j0 - _jour) - NB_MIN_PAR_HEUR * _heure);
        _secondes = NB_SEC_PAR_JOUR * (j0 - _jour) - NB_SEC_PAR_HEUR * _heure - NB_SEC_PAR_MIN * _minutes;
    }

    _offsetUTC = offset;
    _jourJulienUTC = jourJulien2000;
    _jourJulien = _jourJulienUTC + _offsetUTC;
    getDeltaAT();
    _jourJulienTT = _jourJulienUTC + (NB_SEC_TT_TAI + _deltaAT) * NB_JOUR_PAR_SEC;

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une date calendaire, ou le jour est exprime sous forme decimale
 */
Date::Date(const int an, const int mo, const double xjour, const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _annee = an;
    _mois = mo;
    _jour = (int) floor(xjour);
    _heure = (int) floor(NB_HEUR_PAR_JOUR * (xjour - _jour));
    _minutes = (int) floor(NB_MIN_PAR_JOUR * (xjour - _jour) - NB_MIN_PAR_HEUR * _heure);
    _secondes = NB_SEC_PAR_JOUR * (xjour - _jour) - NB_SEC_PAR_HEUR * _heure - NB_SEC_PAR_MIN * _minutes;

    _offsetUTC = offset;

    CalculJourJulien();

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une date calendaire
 */
Date::Date(const int an, const int mo, const int j, const int h, const int min, const double sec, const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _annee = an;
    _mois = mo;
    _jour = j;
    _heure = h;
    _minutes = min;
    _secondes = sec;

    _offsetUTC = offset;

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
    return ((double) date.secsTo(utc) * NB_JOUR_PAR_SEC);
}

/*
 * Conversion d'une date au format NASA en Date
 */
Date Date::ConversionDateNasa(const QString &dateFormatNasa)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QStringList dateNasa = dateFormatNasa.split("/");
    const int annee = dateNasa.at(0).toInt();
    const int nbJours = dateNasa.at(1).toInt();

    const QStringList heures = dateNasa.at(2).split(":", QString::SkipEmptyParts);
    const int heure = heures.at(0).toInt();
    const int minutes = heures.at(1).toInt();
    const double secondes = heures.at(2).toDouble();

    const double jours = nbJours + heure * NB_JOUR_PAR_HEUR + minutes * NB_JOUR_PAR_MIN + secondes * NB_JOUR_PAR_SEC;

    const Date date(annee, 1, 1., 0.);

    /* Retour */
    return Date(date.jourJulien() + jours - 1., 0., true);
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
    return (QDateTime(QDate(_annee, _mois, _jour), QTime(_heure, _minutes, sec)).addSecs(qRound(_offsetUTC * NB_SEC_PAR_JOUR)));
}

QString Date::ToShortDate(const DateFormat &format, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int fmt = (format == FORMAT_COURT) ? 2 : format + 3;
    const double tmp = floor(_jourJulien);
    const QString chaine = " %1:%2:%3%4";

    /* Corps de la methode */
    const double jjsec = arrondi(NB_SEC_PAR_JOUR * (_jourJulien - tmp), format) * NB_JOUR_PAR_SEC + tmp + EPSDBL100;
    const Date date(jjsec, 0.);
    const QDateTime date2 = date.ToQDateTime(0);

    int hr = date2.time().hour();
    QString sys = " ";
    if (systeme == SYSTEME_12H) {
        hr = date2.time().hour()%12;
        sys = (hr >= 0 && date2.time().hour() < 12) ? "a" : "p";
        if (hr == 0) hr = 12;
    }

    const QString res = date2.toString(QObject::tr("dd/MM/yyyy")) + chaine.arg(hr, 2, 10, QChar('0')).
                        arg(date._minutes, 2, 10, QChar('0')).arg(date._secondes, fmt, 'f', format, QChar('0')).arg(sys);

    /* Retour */
    return (res);
}

QString Date::ToShortDateAMJ(const DateFormat &format, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double tmp = floor(_jourJulien);
    QString res = "%1/%2/%3 %4:%5:%6%7";

    /* Corps de la methode */
    const int fmt = (format == FORMAT_COURT) ? 2 : format + 3;
    const double jjsec = arrondi(NB_SEC_PAR_JOUR * (_jourJulien - tmp), format) * NB_JOUR_PAR_SEC + tmp + EPSDBL100;
    const Date date(jjsec, _offsetUTC);
    int hr = date._heure;
    QString sys = " ";

    if (systeme == SYSTEME_12H) {
        hr = date._heure%12;
        sys = (hr >= 0 && date._heure < 12) ? "a" : "p";
        if (hr == 0) hr = 12;
    }

    /* Retour */
    return (res.arg(date._annee, 4, 10, QChar('0')).arg(date._mois, 2, 10, QChar('0')).
            arg(date._jour, 2, 10, QChar('0')).arg(hr, 2, 10, QChar('0')).
            arg(date._minutes, 2, 10, QChar('0')).arg(date._secondes, fmt, 'f', format, QChar('0')).arg(sys));
}

/*
 * Conversion de la date en chaine de caracteres
 */
QString Date::ToLongDate(const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double offset = (fabs(_offsetUTC) > EPSDBL100) ? Date::CalculOffsetUTC(Date(*this, _offsetUTC).ToQDateTime(1)) : 0.;
    const QDateTime qdate = Date((*this).jourJulienUTC() + offset + EPS_DATES, 0.).ToQDateTime(1);

    /* Corps de la methode */
    QString res = qdate.toString(QObject::tr("dddd dd MMMM yyyy hh:mm:ss") + ((systeme == SYSTEME_12H) ? "a" : ""));
    res[0] = res[0].toUpper();

    /* Retour */
    return (res);
}

/*
 * Affectation d'une date
 */
Date &Date::operator = (const Date &date)
{
    _annee = date._annee;
    _mois = date._mois;
    _jour = date._jour;
    _heure = date._heure;
    _minutes = date._minutes;
    _secondes = date.secondes();

    _jourJulien = date._jourJulien;
    _jourJulienTT = date._jourJulienTT;
    _jourJulienUTC = date._jourJulienUTC;
    _offsetUTC = date._offsetUTC;

    _deltaAT = date._deltaAT;

    return (*this);
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
    const double xj = _jour + _heure * NB_JOUR_PAR_HEUR + _minutes * NB_JOUR_PAR_MIN + _secondes * NB_JOUR_PAR_SEC;

    /* Corps de la methode */
    if (n < 3) {
        d--;
        n += 12;
    }

    const int c = d / 100;
    const int b = 2 - c + c / 4;
    d -= AN2000;

    _jourJulienUTC = floor(NB_JOURS_PAR_ANJ * d) + floor(30.6001 * (n + 1)) + xj + b - 50.5;
    _jourJulien = _jourJulienUTC + _offsetUTC;
    getDeltaAT();
    _jourJulienTT = _jourJulienUTC + (NB_SEC_TT_TAI + _deltaAT) * NB_JOUR_PAR_SEC;

    /* Retour */
    return;
}

void Date::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

#if defined (Q_OS_MAC)
        const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else

#if QT_VERSION >= 0x050000
        const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
        const QString dirLocalData =
                QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory).at(0) +
                dirAstr + QDir::separator() + "data";
#else
        QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

#endif

#if defined (Q_OS_LINUX)
    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    dirLocalData = QString("/usr/share") + QDir::separator() + dirAstr + QDir::separator() + "data";
#endif

        const QString fic = dirLocalData + QDir::separator() + "taiutc.dat";

        QFile fi(fic);
        if (fi.exists() && fi.size() != 0) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);

            _ecartsTAI_UTC.clear();
            while (!flux.atEnd()) {
                const QString ligne = flux.readLine();
                if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#')) {
                    const QList<double> list(QList<double> () << ligne.mid(0, 10).toDouble() << ligne.mid(11, 5).toDouble());
                    _ecartsTAI_UTC.append(list);
                }
            }
        } else {
            throw PreviSatException(QObject::tr("Fichier %1 absent").arg(QDir::toNativeSeparators(fic)), WARNING);
        }
        fi.close();
    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

void Date::getDeltaAT()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _deltaAT = 0.;

    /* Corps de la methode */
    try {

        if (_ecartsTAI_UTC.count() == 0) {
            Initialisation();
        }

        if (_jourJulienUTC < _ecartsTAI_UTC.at(0).at(0) - TJ2000) {

            const double mjd = _jourJulienUTC + 51544.5;

            if (mjd >= 37300. && mjd < 37512.) {
                // Intervalle 01/01/1961 - 01/08/1961
                _deltaAT = 1.422818 + (mjd - 37300.) * 0.001296;

            } else if (mjd >= 37512. && mjd < 37665.) {
                // Intervalle 01/08/1961 - 01/01/1962
                _deltaAT = 1.372818 + (mjd - 37300.) * 0.001296;

            } else if (mjd >= 37665. && mjd < 38334.) {
                // Intervalle 01/01/1962 - 01/11/1963
                _deltaAT = 1.845858 + (mjd - 37665.) * 0.0011232;

            } else if (mjd >= 38334. && mjd < 38395.) {
                // Intervalle 01/11/1963 - 01/01/1964
                _deltaAT = 1.945858 + (mjd - 37665.) * 0.0011232;

            } else if (mjd >= 38395. && mjd < 38486.) {
                // Intervalle 01/01/1964 - 01/04/1964
                _deltaAT = 3.240130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 38486. && mjd < 38639.) {
                // Intervalle 01/04/1964 - 01/09/1964
                _deltaAT = 3.340130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 38639. && mjd < 38761.) {
                // Intervalle 01/09/1964 - 01/01/1965
                _deltaAT = 3.440130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 38761. && mjd < 38820.) {
                // Intervalle 01/01/1965 - 01/03/1965
                _deltaAT = 3.540130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 38820. && mjd < 38942.) {
                // Intervalle 01/03/1965 - 01/07/1965
                _deltaAT = 3.640130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 38942. && mjd < 39004.) {
                // Intervalle 01/07/1965 - 01/09/1965
                _deltaAT = 3.740130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 39004. && mjd < 39126.) {
                // Intervalle 01/09/1965 - 01/01/1966
                _deltaAT = 3.840130 + (mjd - 38761.) * 0.001296;

            } else if (mjd >= 39126. && mjd < 39887.) {
                // Intervalle 01/01/1966 - 01/02/1968
                _deltaAT = 4.313170 + (mjd - 39126.) * 0.002592;

            } else if (mjd >= 39887. && mjd < 41317.) {
                // Intervalle 01/02/1968 - 01/01/1972
                _deltaAT = 4.213170 + (mjd - 39126.) * 0.002592;

            } else {
                _deltaAT = 0.;
            }
        } else {

            // Dates ulterieures au 1er janvier 1972
            QListIterator<QList<double> > it(_ecartsTAI_UTC);
            it.toBack();
            while (it.hasPrevious()) {
                const QList<double> list = it.previous();
                if (_jourJulienUTC >= list.at(0) - TJ2000) {
                    _deltaAT = list.at(1);
                    it.toFront();
                }
            }
        }
    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}


/* Accesseurs */
int Date::annee() const
{
    return _annee;
}

int Date::heure() const
{
    return _heure;
}

int Date::jour() const
{
    return _jour;
}

double Date::jourJulien() const
{
    return _jourJulien;
}

double Date::jourJulienTT() const
{
    return _jourJulienTT;
}

double Date::jourJulienUTC() const
{
    return _jourJulienUTC;
}

int Date::minutes() const
{
    return _minutes;
}

int Date::mois() const
{
    return _mois;
}

double Date::offsetUTC() const
{
    return _offsetUTC;
}

double Date::secondes() const
{
    return _secondes;
}
