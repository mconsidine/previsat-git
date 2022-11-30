/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    15 octobre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


QList<QPair<double, double> > Date::_ecartsTAI_UTC;


/**********
 * PUBLIC *
 **********/


/*
 * Constructeurs
 */
/*
 * Obtention de la date systeme
 */
Date::Date(const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

        const QDateTime dateSysteme = QDateTime::currentDateTimeUtc();

        _annee = dateSysteme.date().year();
        _mois = dateSysteme.date().month();
        _jour = dateSysteme.date().day();
        _heure = dateSysteme.time().hour();
        _minutes = dateSysteme.time().minute();
        _secondes = dateSysteme.time().second();

        _offsetUTC = offset;

        CalculJourJulien();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir d'un objet Date et de l'ecart heure legale - UTC
 */
Date::Date(const Date &date, const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

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

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir d'un jour julien 2000 et de l'ecart heure legale - UTC
 */
Date::Date(const double jourJulien2000, const double offset, const bool acalc)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

        if (acalc) {

            int a;
            const double j1 = jourJulien2000 + 0.5;
            int z = static_cast<int> (floor(j1));
            const double f = j1 - z;
            z += static_cast<int> (TJ2000 + EPSDBL100);

            if (z < 2299161) {
                a = z;
            } else {
                const int al = static_cast<int> (floor((z - 1867216.25) * (1. / 36524.25)));
                a = z + 1 + al - al / 4;
            }

            const int b = a + 1524;
            const int c = static_cast<int> (floor((b - 122.1) * NB_ANJ_PAR_JOURS));
            const int d = static_cast<int> (floor(NB_JOURS_PAR_ANJ * c));
            const int e = static_cast<int> (floor((b - d) * (1. / 30.6001)));
            const double j0 = b - d - floor(30.6001 * e) + f;

            _mois = (e < 14) ? e - 1 : e - 13;
            _annee = (_mois > 2) ? c - 4716 : c - 4715;
            _jour = static_cast<int> (floor(j0));

            _heure = static_cast<int> (floor(NB_HEUR_PAR_JOUR * (j0 - _jour)));
            _minutes = static_cast<int> (floor(NB_MIN_PAR_JOUR * (j0 - _jour) - NB_MIN_PAR_HEUR * _heure));
            _secondes = NB_SEC_PAR_JOUR * (j0 - _jour) - NB_SEC_PAR_HEUR * _heure - NB_SEC_PAR_MIN * _minutes;
        }

        _offsetUTC = offset;
        _jourJulienUTC = jourJulien2000;
        _jourJulien = _jourJulienUTC + _offsetUTC;
        getDeltaAT();
        _jourJulienTT = _jourJulienUTC + (NB_SEC_TT_TAI + _deltaAT) * NB_JOUR_PAR_SEC;

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir de l'annee, du mois et du jour (decimal)
 */
Date::Date(const int an, const int mo, const double xjour, const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

        _annee = an;
        _mois = mo;
        _jour = static_cast<int> (floor(xjour));
        _heure = static_cast<int> (floor(NB_HEUR_PAR_JOUR * (xjour - _jour)));
        _minutes = static_cast<int> (floor(NB_MIN_PAR_JOUR * (xjour - _jour) - NB_MIN_PAR_HEUR * _heure));
        _secondes = NB_SEC_PAR_JOUR * (xjour - _jour) - NB_SEC_PAR_HEUR * _heure - NB_SEC_PAR_MIN * _minutes;

        _offsetUTC = offset;

        CalculJourJulien();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir des elements du calendrier
 */
Date::Date(const int an, const int mo, const int j, const int h, const int min, const double sec, const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

        _annee = an;
        _mois = mo;
        _jour = j;
        _heure = h;
        _minutes = min;
        _secondes = sec;

        _offsetUTC = offset;

        CalculJourJulien();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
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
    return (static_cast<double> (date.secsTo(utc)) * NB_JOUR_PAR_SEC);
}

/*
 * Conversion d'une date au format ISO en Date
 */
Date Date::ConversionDateIso(const QString &dateFormatIso)
{
    /* Declarations des variables locales */
    Date date;

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (dateFormatIso.isEmpty()) {
#if (BUILD_TEST == false)
            qWarning() << "Date au format ISO vide";
#endif
            throw PreviSatException(QObject::tr("Date au format ISO vide"), MessageType::WARNING);
        }

        const QDateTime dateTime = QDateTime::fromString(dateFormatIso, Qt::ISODate);

        if (!dateTime.isValid()) {
#if (BUILD_TEST == false)
            qWarning() << QString("Date au format ISO invalide (%1)").arg(dateFormatIso);
#endif
            throw PreviSatException(QObject::tr("Date au format ISO invalide"), MessageType::WARNING);
        }

        const int annee = dateTime.date().year();
        const int mois = dateTime.date().month();
        const int jour = dateTime.date().day();
        const int heure = dateTime.time().hour();
        const int minutes = dateTime.time().minute();
        const double millisec = QString("0." + dateFormatIso.mid(20)).toDouble();
        const double secondes = dateTime.time().second() + millisec;

        date = Date(annee, mois, jour, heure, minutes, secondes, 0.);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return date;
}

/*
 * Conversion d'une date au format NASA en Date
 */
Date Date::ConversionDateNasa(const QString &dateFormatNasa)
{
    /* Declarations des variables locales */
    Date date;

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (dateFormatNasa.isEmpty()) {
#if (BUILD_TEST == false)
            qWarning() << "Date au format NASA vide";
#endif
            throw PreviSatException(QObject::tr("Date au format NASA vide"), MessageType::WARNING);
        }

        const QStringList dateNasa = dateFormatNasa.split("T", Qt::SkipEmptyParts);

        if (dateNasa.size() != 2) {
#if (BUILD_TEST == false)
            qWarning() << QString("Date au format NASA invalide (%1)").arg(dateFormatNasa);
#endif
            throw PreviSatException(QObject::tr("Date au format NASA invalide"), MessageType::WARNING);
        }

        const QStringList anneeNbJours = dateNasa.first().split("-", Qt::SkipEmptyParts);

        if (anneeNbJours.size() != 2) {
#if (BUILD_TEST == false)
            qWarning() << QString("Date au format NASA invalide (%1)").arg(dateFormatNasa);
#endif
            throw PreviSatException(QObject::tr("Date au format NASA invalide"), MessageType::WARNING);
        }

        const int an = anneeNbJours.first().toInt();
        const int nbJours = anneeNbJours.at(1).toInt();
        const QStringList heures = dateNasa.at(1).mid(0, dateNasa.at(1).length() - 1).split(":", Qt::SkipEmptyParts);

        if (heures.size() != 3) {
#if (BUILD_TEST == false)
            qWarning() << QString("Date au format NASA invalide (%1)").arg(dateFormatNasa);
#endif
            throw PreviSatException(QObject::tr("Date au format NASA invalide"), MessageType::WARNING);
        }

        const int hr = heures.first().toInt();
        const int mn = heures.at(1).toInt();
        const double sec = heures.at(2).toDouble();
        const double jours = nbJours + hr * NB_JOUR_PAR_HEUR + mn * NB_JOUR_PAR_MIN + sec * NB_JOUR_PAR_SEC;

        const Date date1(an, 1, 1., 0.);
        date = Date(date1.jourJulien() + jours - 1., 0., true);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return date;
}

/*
 * Lecture du fichier taiutc.dat
 */
void Date::Initialisation(const QString &dirLocalData)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString fic = dirLocalData + QDir::separator() + "taiutc.dat";

        QFile fi(fic);
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QTextStream flux(&fi);

                _ecartsTAI_UTC.clear();
                while (!flux.atEnd()) {

                    const QString ligne = flux.readLine();

                    if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#')) {
                        const QPair<double, double> pair(ligne.mid(0, 10).toDouble(), ligne.mid(11, 5).toDouble());
                        _ecartsTAI_UTC.append(pair);
                    }
                }
            }
            fi.close();

#if (COVERAGE_TEST == false)
            if (_ecartsTAI_UTC.isEmpty()) {
                const QFileInfo ff(fi.fileName());
                throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                                          .arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
            }
#endif

#if (BUILD_TEST == false)
            qInfo() << "Lecture fichier taiutc.dat OK";
#endif

        } else {
            throw PreviSatException(QObject::tr("Le fichier taiutc.dat n'existe pas"), MessageType::WARNING);
        }

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Conversion de la date en chaine de caracteres
 */
QString Date::ToLongDate(const QString &locale, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double offset = (fabs(_offsetUTC) > EPSDBL100) ? Date::CalculOffsetUTC(Date(*this, _offsetUTC).ToQDateTime(1)) : 0.;
    const QDateTime qdate = Date((*this).jourJulien() - offset + EPS_DATES, offset).ToQDateTime(1);

    /* Corps de la methode */
    QString res = QLocale(locale).toString(qdate, QObject::tr("dddd dd MMMM yyyy hh:mm:ss", "Date format") +
                                           ((systeme == DateSysteme::SYSTEME_12H) ? "a" : ""));
    res[0] = res[0].toUpper();

    /* Retour */
    return (res);
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

/*
 * Conversion en chaine de caracteres courte
 */
QString Date::ToShortDate(const DateFormat &format, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int fmt = (format == DateFormat::FORMAT_COURT) ? 2 : static_cast<int> (format) + 3;
    const double tmp = floor(_jourJulien);
    const QString chaine = " %1:%2:%3%4";

    /* Corps de la methode */
    const double jjsec = arrondi(NB_SEC_PAR_JOUR * (_jourJulien - tmp) + EPS_DATES, static_cast<int> (format)) * NB_JOUR_PAR_SEC + tmp;
    const Date date(jjsec + EPSDBL100, 0.);
    const QDateTime date2 = date.ToQDateTime(0);
    const QPair<int, QString> hr = getHrAmPm(date2.time().hour(), systeme);

    const QString res = date2.toString(QObject::tr("dd/MM/yyyy", "Date format")) + chaine.arg(hr.first, 2, 10, QChar('0'))
            .arg(date._minutes, 2, 10, QChar('0'))
            .arg(date._secondes, fmt, 'f', static_cast<int> (format), QChar('0')).arg(hr.second);

    /* Retour */
    return (res);
}

/*
 * Conversion en chaine de caracteres courte (AAAA/MM/JJ)
 */
QString Date::ToShortDateAMJ(const DateFormat &format, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int fmt = (format == DateFormat::FORMAT_COURT) ? 2 : static_cast<int> (format) + 3;
    const double tmp = floor(_jourJulien);
    QString res = "%1/%2/%3 %4:%5:%6%7";

    /* Corps de la methode */
    const double jjsec = arrondi(NB_SEC_PAR_JOUR * (_jourJulien - tmp) + EPS_DATES, static_cast<int> (format)) * NB_JOUR_PAR_SEC + tmp;
    const Date date(jjsec + EPSDBL100, _offsetUTC);
    const QPair<int, QString> hr = getHrAmPm(date._heure, systeme);

    /* Retour */
    return (res.arg(date._annee, 4, 10, QChar('0'))
            .arg(date._mois, 2, 10, QChar('0'))
            .arg(date._jour, 2, 10, QChar('0'))
            .arg(hr.first, 2, 10, QChar('0'))
            .arg(date._minutes, 2, 10, QChar('0'))
            .arg(date._secondes, fmt, 'f', static_cast<int> (format), QChar('0')).arg(hr.second));
}

/*
 * Conversion en chaine de caracteres avec une precision a la milliseconde (UTC)
 */
QString Date::ToShortDateAMJmillisec() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmtDate = "yyyy/MM/dd HH:mm:ss.zzz";

    /* Corps de la methode */
    const int sec = static_cast<int> (_secondes + EPS_DATES);
    const int msec = static_cast<int> (floor(1000. * (_secondes - sec) + 0.5));
    const QDateTime date(QDate(_annee, _mois, _jour), QTime(_heure, _minutes, sec, msec));

    /* Retour */
    return date.toString(fmtDate);
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
    _secondes = date._secondes;

    _jourJulien = date._jourJulien;
    _jourJulienTT = date._jourJulienTT;
    _jourJulienUTC = date._jourJulienUTC;
    _offsetUTC = date._offsetUTC;

    _deltaAT = date._deltaAT;

    return (*this);
}


/*
 * Accesseurs
 */
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


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */
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
    try {

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

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Obtention de l'ecart TAI-UTC
 */
void Date::getDeltaAT()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _deltaAT = 0.;

    /* Corps de la methode */
    try {

        if (_ecartsTAI_UTC.isEmpty()) {
            throw PreviSatException(QObject::tr("Ecarts TAI-UTC non initialisés"), MessageType::WARNING);
        }

        if (_jourJulienUTC < (_ecartsTAI_UTC.first().first - TJ2000)) {
#if (COVERAGE_TEST == false)
            const double mjd = _jourJulienUTC + 51544.5;

            if ((mjd >= 37300.) && (mjd < 37512.)) {
                // Intervalle 01/01/1961 - 01/08/1961
                _deltaAT = 1.422818 + (mjd - 37300.) * 0.001296;

            } else if ((mjd >= 37512.) && (mjd < 37665.)) {
                // Intervalle 01/08/1961 - 01/01/1962
                _deltaAT = 1.372818 + (mjd - 37300.) * 0.001296;

            } else if ((mjd >= 37665.) && (mjd < 38334.)) {
                // Intervalle 01/01/1962 - 01/11/1963
                _deltaAT = 1.845858 + (mjd - 37665.) * 0.0011232;

            } else if ((mjd >= 38334.) && (mjd < 38395.)) {
                // Intervalle 01/11/1963 - 01/01/1964
                _deltaAT = 1.945858 + (mjd - 37665.) * 0.0011232;

            } else if ((mjd >= 38395.) && (mjd < 38486.)) {
                // Intervalle 01/01/1964 - 01/04/1964
                _deltaAT = 3.240130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 38486.) && (mjd < 38639.)) {
                // Intervalle 01/04/1964 - 01/09/1964
                _deltaAT = 3.340130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 38639.) && (mjd < 38761.)) {
                // Intervalle 01/09/1964 - 01/01/1965
                _deltaAT = 3.440130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 38761.) && (mjd < 38820.)) {
                // Intervalle 01/01/1965 - 01/03/1965
                _deltaAT = 3.540130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 38820.) && (mjd < 38942.)) {
                // Intervalle 01/03/1965 - 01/07/1965
                _deltaAT = 3.640130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 38942.) && (mjd < 39004.)) {
                // Intervalle 01/07/1965 - 01/09/1965
                _deltaAT = 3.740130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 39004.) && (mjd < 39126.)) {
                // Intervalle 01/09/1965 - 01/01/1966
                _deltaAT = 3.840130 + (mjd - 38761.) * 0.001296;

            } else if ((mjd >= 39126.) && (mjd < 39887.)) {
                // Intervalle 01/01/1966 - 01/02/1968
                _deltaAT = 4.313170 + (mjd - 39126.) * 0.002592;

            } else if ((mjd >= 39887.) && (mjd < 41317.)) {
                // Intervalle 01/02/1968 - 01/01/1972
                _deltaAT = 4.213170 + (mjd - 39126.) * 0.002592;

            } else {
                _deltaAT = 0.;
            }
#endif
        } else {

            // Dates ulterieures au 1er janvier 1972
            QListIterator it(_ecartsTAI_UTC);
            it.toBack();

            while (it.hasPrevious()) {
                const QPair<double, double> pair = it.previous();
                if (_jourJulienUTC >= (pair.first - TJ2000)) {
                    _deltaAT = pair.second;
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

/*
 * Obtention de l'heure AM/PM
 */
QPair<int, QString> Date::getHrAmPm(const int heure, const DateSysteme &systeme) const
{
    /* Declarations des variables locales */
    QPair<int, QString> res;

    /* Initialisations */
    res.first = heure;
    res.second = " ";

    /* Corps de la methode */
    if (systeme == DateSysteme::SYSTEME_12H) {
        res.first = heure % 12;
        res.second = ((res.first >= 0) && (heure < 12)) ? "a" : "p";
        if (res.first == 0) {
            res.first = 12;
        }
    }

    /* Retour */
    return res;
}
