/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    14 aout 2023
 *
 */

#include <QDir>
#include <QTextStream>
#include <QVector>
#include "date.h"
#include "librairies/exceptions/exception.h"
#include "librairies/maths/maths.h"


QVector<QPointF> Date::_ecartsTAI_UTC;
std::array<std::array<double, DATE::NB_PARAM_TAIUTC>, DATE::NB_LIGNES_TAIUTC> Date::_tabEcartsTAI_UTC;


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
        *this = Date(dateSysteme.addSecs(static_cast<quint64> (floor(offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES))), offset);

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir d'un objet Date et de l'ecart heure legale - UTC
 */
Date::Date(const Date &date,
           const double offset)
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
        _jourJulienTT = _jourJulienUTC + (DATE::NB_SEC_TT_TAI + _deltaAT) * DATE::NB_JOUR_PAR_SEC;

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir d'un objet QDateTime et de l'ecart heure legale - UTC
 */
Date::Date(const QDateTime &datetime,
           const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

        const QDateTime dt = datetime.addSecs(static_cast<qint64> (-floor(offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES)));
        *this = Date(dt.date().year(), dt.date().month(), dt.date().day(), dt.time().hour(), dt.time().minute(), dt.time().second(), offset);

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir d'un jour julien 2000 et de l'ecart heure legale - UTC
 */
Date::Date(const double jourJulien2000,
           const double offset,
           const bool acalc)
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
            z += static_cast<int> (DATE::TJ2000 + MATHS::EPSDBL100);

            if (z < 2299161) {
                a = z;
            } else {
                const int al = static_cast<int> (floor((z - 1867216.25) * (1. / 36524.25)));
                a = z + 1 + al - al / 4;
            }

            const int b = a + 1524;
            const int c = static_cast<int> (floor((b - 122.1) * DATE::NB_ANJ_PAR_JOURS));
            const int d = static_cast<int> (floor(DATE::NB_JOURS_PAR_ANJ * c));
            const int e = static_cast<int> (floor((b - d) * (1. / 30.6001)));
            const double j0 = b - d - floor(30.6001 * e) + f;

            _mois = (e < 14) ? e - 1 : e - 13;
            _annee = (_mois > 2) ? c - 4716 : c - 4715;
            _jour = static_cast<int> (floor(j0));

            _heure = static_cast<int> (floor(DATE::NB_HEUR_PAR_JOUR * (j0 - _jour)));
            _minutes = static_cast<int> (floor(DATE::NB_MIN_PAR_JOUR * (j0 - _jour) - DATE::NB_MIN_PAR_HEUR * _heure));
            _secondes = DATE::NB_SEC_PAR_JOUR * (j0 - _jour) - DATE::NB_SEC_PAR_HEUR * _heure - DATE::NB_SEC_PAR_MIN * _minutes;
        }

        _offsetUTC = offset;
        _jourJulienUTC = jourJulien2000;
        _jourJulien = _jourJulienUTC + _offsetUTC;
        getDeltaAT();
        _jourJulienTT = _jourJulienUTC + (DATE::NB_SEC_TT_TAI + _deltaAT) * DATE::NB_JOUR_PAR_SEC;

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir de l'annee, du mois et du jour (decimal)
 */
Date::Date(const int an,
           const int mo,
           const double xjour,
           const double offset)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    try {

        _annee = an;
        _mois = mo;
        _jour = static_cast<int> (floor(xjour));
        _heure = static_cast<int> (floor(DATE::NB_HEUR_PAR_JOUR * (xjour - _jour)));
        _minutes = static_cast<int> (floor(DATE::NB_MIN_PAR_JOUR * (xjour - _jour) - DATE::NB_MIN_PAR_HEUR * _heure));
        _secondes = DATE::NB_SEC_PAR_JOUR * (xjour - _jour) - DATE::NB_SEC_PAR_HEUR * _heure - DATE::NB_SEC_PAR_MIN * _minutes;

        _offsetUTC = offset;

        CalculJourJulien();

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return;
}

/*
 * Definition de la date a partir des elements du calendrier
 */
Date::Date(const int an,
           const int mo,
           const int j,
           const int h,
           const int min,
           const double sec,
           const double offset)
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

    } catch (Exception const &e) {
        throw Exception();
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
    return (static_cast<double> (date.secsTo(utc)) * DATE::NB_JOUR_PAR_SEC);
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
    if (dateFormatIso.isEmpty()) {
#if (!BUILD_TEST)
        qWarning() << "Date au format ISO vide";
#endif
        throw Exception(QObject::tr("Date au format ISO vide"), MessageType::WARNING);
    }

    const QDateTime dateTime = QDateTime::fromString(dateFormatIso, Qt::ISODate);

    if (!dateTime.isValid()) {
#if (!BUILD_TEST)
        qWarning().noquote() << QString("Date au format ISO invalide (%1)").arg(dateFormatIso);
#endif
        throw Exception(QObject::tr("Date au format ISO invalide"), MessageType::WARNING);
    }

    const int an = dateTime.date().year();
    const int m = dateTime.date().month();
    const int j = dateTime.date().day();
    const int h = dateTime.time().hour();
    const int min = dateTime.time().minute();
    const double millisec = QString("0." + dateFormatIso.mid(20)).toDouble();
    const double sec = dateTime.time().second() + millisec;

    date = Date(an, m, j, h, min, sec, 0.);

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
    if (dateFormatNasa.isEmpty()) {
#if (!BUILD_TEST)
        qWarning() << "Date au format NASA vide";
#endif
        throw Exception(QObject::tr("Date au format NASA vide"), MessageType::WARNING);
    }

    const QStringList dateNasa = dateFormatNasa.split("T", Qt::SkipEmptyParts);

    if (dateNasa.size() != 2) {
#if (!BUILD_TEST)
        qWarning().noquote() << QString("Date au format NASA invalide (%1)").arg(dateFormatNasa);
#endif
        throw Exception(QObject::tr("Date au format NASA invalide"), MessageType::WARNING);
    }

    const QStringList anneeNbJours = dateNasa.first().split("-", Qt::SkipEmptyParts);

    if (anneeNbJours.size() != 2) {
#if (!BUILD_TEST)
        qWarning().noquote() << QString("Date au format NASA invalide (%1)").arg(dateFormatNasa);
#endif
        throw Exception(QObject::tr("Date au format NASA invalide"), MessageType::WARNING);
    }

    const int an = anneeNbJours.first().toInt();
    const int nbJours = anneeNbJours.at(1).toInt();

    QString heures = dateNasa.at(1);
    heures.chop(1);
    const QTime hrs = QTime::fromString(heures, Qt::ISODateWithMs);

    if (!hrs.isValid()) {
#if (!BUILD_TEST)
        qWarning().noquote() << QString("Date au format NASA invalide (%1)").arg(dateFormatNasa);
#endif
        throw Exception(QObject::tr("Date au format NASA invalide"), MessageType::WARNING);
    }

    const int hr = hrs.hour();
    const int mn = hrs.minute();
    const double sec = hrs.second() + hrs.msec() * 1.e-3;
    const double jours = nbJours + hr * DATE::NB_JOUR_PAR_HEUR + mn * DATE::NB_JOUR_PAR_MIN + sec * DATE::NB_JOUR_PAR_SEC;

    const Date date1(an, 1, 1., 0.);
    date = Date(date1.jourJulien() + jours - 1., 0., true);

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
    const QString fic = dirLocalData + QDir::separator() + "taiutc.dat";

    QFile fi(fic);
    if (!fi.exists() || (fi.size() == 0)) {
        const QFileInfo ff(fi.fileName());
        throw Exception(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME),
                        MessageType::ERREUR);
    }

    if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
#if (!COVERAGE_TEST)
        const QFileInfo ff(fi.fileName());
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName()), MessageType::ERREUR);
#endif
    }

    /* Corps de la methode */
    const QStringList contenu = QString(fi.readAll()).split("\n", Qt::SkipEmptyParts);
    fi.close();

    unsigned int i = 0;
    _ecartsTAI_UTC.clear();

    QStringListIterator it(contenu);
    while (it.hasNext()) {

        const QString ligne = it.next();

        if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#')) {

            if (i < DATE::NB_LIGNES_TAIUTC) {

                const QStringList params = ligne.split(" ", Qt::SkipEmptyParts);

                for(unsigned int j=0; j<DATE::NB_PARAM_TAIUTC; j++) {
                    _tabEcartsTAI_UTC[i][j] = params.at(j).toDouble();
                }

            } else {
                _ecartsTAI_UTC.append(QPointF(ligne.mid(0, 10).toDouble(), ligne.mid(11, 5).toDouble()));
            }

            i++;
        }
    }

#if (!BUILD_TEST)
    qInfo() << "Lecture fichier taiutc.dat OK";
#endif

    /* Retour */
    return;
}

/*
 * Conversion de la date en chaine de caracteres
 */
QString Date::ToLongDate(const QString &locale,
                         const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double offset = (fabs(_offsetUTC) > MATHS::EPSDBL100) ?
                              Date::CalculOffsetUTC(Date(*this, _offsetUTC).ToQDateTime(DateFormatSec::FORMAT_SEC)) : 0.;
    const QDateTime qdate = Date((*this).jourJulien() - offset + DATE::EPS_DATES, offset).ToQDateTime(DateFormatSec::FORMAT_SEC);

    /* Corps de la methode */
    QString res = QLocale(locale).toString(qdate, QObject::tr("dddd dd MMMM yyyy HH:mm:ss", "Date format") +
                                                      ((systeme == DateSysteme::SYSTEME_12H) ? "a" : ""));
    res[0] = res[0].toUpper();

    /* Retour */
    return (res);
}

/*
 * Conversion de la date en QDateTime
 */
QDateTime Date::ToQDateTime(const DateFormatSec &type) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const int sec = (type == DateFormatSec::FORMAT_SEC_ZERO) ? 0 : qRound(_secondes);

    /* Retour */
    return (QDateTime(QDate(_annee, _mois, _jour), QTime(_heure, _minutes, sec)).addSecs(qRound(_offsetUTC * DATE::NB_SEC_PAR_JOUR)));
}

/*
 * Conversion en chaine de caracteres courte
 */
QString Date::ToShortDate(const DateFormat &format,
                          const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int fmt = (format == DateFormat::FORMAT_COURT) ? 2 : static_cast<int> (format) + 3;
    const double tmp = floor(_jourJulien);
    const QString chaine = " %1:%2:%3%4";

    /* Corps de la methode */
    const double jjsec =
        arrondi(DATE::NB_SEC_PAR_JOUR * (_jourJulien - tmp) + DATE::EPS_DATES, static_cast<int> (format)) * DATE::NB_JOUR_PAR_SEC + tmp;
    const Date date(jjsec + MATHS::EPSDBL100, 0.);
    const QDateTime date2 = date.ToQDateTime(DateFormatSec::FORMAT_SEC_ZERO);
    const QPair<int, QString> hr = getHrAmPm(date2.time().hour(), systeme);

    const QString res = date2.toString(QObject::tr("dd/MM/yyyy", "Date format")) +
                        chaine.arg(hr.first, 2, 10, QChar('0')).arg(date._minutes, 2, 10, QChar('0'))
                            .arg(date._secondes, fmt, 'f', static_cast<int> (format), QChar('0')).arg(hr.second);

    /* Retour */
    return (res);
}

/*
 * Conversion en chaine de caracteres courte (AAAA/MM/JJ)
 */
QString Date::ToShortDateAMJ(const DateFormat &format,
                             const DateSysteme &systeme) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int fmt = (format == DateFormat::FORMAT_COURT) ? 2 : static_cast<int> (format) + 3;
    const double tmp = floor(_jourJulien);
    QString res = "%1/%2/%3 %4:%5:%6%7";

    /* Corps de la methode */
    const double jjsec =
        arrondi(DATE::NB_SEC_PAR_JOUR * (_jourJulien - tmp) + DATE::EPS_DATES, static_cast<int> (format)) * DATE::NB_JOUR_PAR_SEC + tmp;
    const Date date(jjsec + MATHS::EPSDBL100, _offsetUTC);
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
    const int sec = static_cast<int> (_secondes + DATE::EPS_DATES);
    const int msec = static_cast<int> (floor(1000. * (_secondes - sec) + 0.5));
    const QDateTime date(QDate(_annee, _mois, _jour), QTime(_heure, _minutes, sec, msec));

    /* Retour */
    return date.toString(fmtDate);
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
    const double xj = _jour + _heure * DATE::NB_JOUR_PAR_HEUR + _minutes * DATE::NB_JOUR_PAR_MIN + _secondes * DATE::NB_JOUR_PAR_SEC;

    /* Corps de la methode */
    try {

        if (n < 3) {
            d--;
            n += 12;
        }

        const int c = d / 100;
        const int b = 2 - c + c / 4;
        d -= DATE::AN2000;

        _jourJulienUTC = floor(DATE::NB_JOURS_PAR_ANJ * d) + floor(30.6001 * (n + 1)) + xj + b - 50.5;
        _jourJulien = _jourJulienUTC + _offsetUTC;
        getDeltaAT();
        _jourJulienTT = _jourJulienUTC + (DATE::NB_SEC_TT_TAI + _deltaAT) * DATE::NB_JOUR_PAR_SEC;

    } catch (Exception const &e) {
        throw Exception();
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
    if (_ecartsTAI_UTC.isEmpty()) {
        throw Exception(QObject::tr("Ecarts TAI-UTC non initialisés"), MessageType::WARNING);
    }

    if (_jourJulienUTC < (_ecartsTAI_UTC.first().x() - DATE::TJ2000)) {

        const double mjd = _jourJulienUTC + 51544.5;
        for(const auto &params : _tabEcartsTAI_UTC) {

            const double mjd1 = params[0];
            const double mjd2 = params[1];

            if ((mjd >= mjd1) && (mjd < mjd2)) {
                _deltaAT = params[2] + (mjd - params[3]) * params[4];
            }
        }
    } else {

        // Dates ulterieures au 1er janvier 1972
        QVectorIterator it(_ecartsTAI_UTC);
        it.toBack();

        while (it.hasPrevious()) {

            const QPointF pair = it.previous();
            if (_jourJulienUTC >= (pair.x() - DATE::TJ2000)) {
                _deltaAT = pair.y();
                it.toFront();
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Obtention de l'heure AM/PM
 */
QPair<int, QString> Date::getHrAmPm(const int heure,
                                    const DateSysteme &systeme) const
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
