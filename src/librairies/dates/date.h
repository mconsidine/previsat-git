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
 * >    date.h
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
 * >    21 septembre 2022
 *
 */

#ifndef DATE_H
#define DATE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDateTime>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include <QList>
#include "dateconst.h"


class QLocale;

class Date
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Date Obtention de la date systeme
     * @param offset ecart heure legale - UTC
     */
    explicit Date(const double offset = 0.);

    /**
     * @brief Date Definition de la date a partir d'un objet Date et de l'ecart heure legale - UTC
     * @param date date
     * @param offset ecart heure legale - UTC
     */
    Date(const Date &date, const double offset = 0.);

    /**
     * @brief Date Definition de la date a partir d'un jour julien 2000 et de l'ecart heure legale - UTC
     * @param jourJulien2000 jour julien 2000
     * @param offset ecart heure legale - UTC
     * @param acalc calcul des elements du calendrier
     */
    Date(const double jourJulien2000, const double offset, const bool acalc = true);

    /**
     * @brief Date Definition de la date a partir de l'annee, du mois et du jour (decimal)
     * @param an annee
     * @param mo mois
     * @param xjour jour decimal
     * @param offset ecart heure legale - UTC
     */
    Date(const int an, const int mo, const double xjour, const double offset);

    /**
     * @brief Date Definition de la date a partir des elements du calendrier
     * @param an annee
     * @param mo mois
     * @param j jour
     * @param h heure
     * @param min minutes
     * @param sec secondes
     * @param offset ecart heure legale - UTC
     */
    Date(const int an, const int mo, const int j, const int h, const int min, const double sec, const double offset);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculOffsetUTC Calcul de l'ecart heure legale - UTC
     * @param date date
     * @return ecart heure legale - UTC
     */
    static double CalculOffsetUTC(const QDateTime &date);

    /**
     * @brief ConversionDateIso Conversion d'une date au format ISO en Date
     * @param dateFormatIso date au format ISO
     * @return date
     */
    static Date ConversionDateIso(const QString &dateFormatIso);

    /**
     * @brief ConversionDateNasa Conversion d'une date au format NASA en Date
     * @param dateFormatNasa date au format NASA
     * @return date
     */
    static Date ConversionDateNasa(const QString &dateFormatNasa);

    /**
     * @brief Initialisation Lecture du fichier taiutc.dat
     * @param dirLocalData chemin ou se trouve le fichier
     */
    static void Initialisation(const QString &dirLocalData);

    /**
     * @brief ToLongDate Conversion en chaine de caracteres longue
     * @param locale locale
     * @param systeme systeme horaire (SYSTEME_12H ou SYSTEME_24H)
     * @return chaine de caracteres contenant la date au format long
     */
    QString ToLongDate(const QString &locale, const DateSysteme &systeme) const;

    /**
     * @brief ToQDateTime Conversion de la date en QDateTime
     * @param type type d'affichage des secondes (0 = secondes prises a zero; arrondi a l'entier le plus proche sinon)
     * @return date
     */
    QDateTime ToQDateTime(const int type) const;

    /**
     * @brief ToShortDate Conversion en chaine de caracteres courte
     * @param format format (COURT ou LONG)
     * @param systeme systeme horaire (SYSTEME_12H ou SYSTEME_24H)
     * @return chaine de caracteres contenant la date au format court
     */
    QString ToShortDate(const DateFormat &format, const DateSysteme &systeme) const;

    /**
     * @brief ToShortDateAMJ Conversion en chaine de caracteres courte (AAAA/MM/JJ)
     * @param format format (COURT ou LONG)
     * @param systeme systeme horaire (SYSTEME_12H ou SYSTEME_24H)
     * @return chaine de caracteres contenant la date au format court (AAAA/MM/JJ)
     */
    QString ToShortDateAMJ(const DateFormat &format, const DateSysteme &systeme) const;

    /**
     * @brief ToShortDateAMJmillisec Conversion en chaine de caracteres avec une precision a la milliseconde
     * @return chaine de caracteres avec une precision a la milliseconde
     */
    QString ToShortDateAMJmillisec() const;

    /**
     * @brief operator = Affectation d'une date
     * @param date date
     */
    Date &operator = (const Date &date);


    /*
     * Accesseurs
     */
    int annee() const;
    int heure() const;
    int jour() const;
    int minutes() const;
    int mois() const;
    double jourJulien() const;
    double jourJulienTT() const;
    double jourJulienUTC() const;
    double offsetUTC() const;
    double secondes() const;


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */
    int _annee;
    int _mois;
    int _jour;
    int _heure;
    int _minutes;
    double _secondes;

    double _jourJulien;
    double _jourJulienTT;
    double _jourJulienUTC;
    double _offsetUTC;

    // Ecart TAI-UTC (en secondes)
    double _deltaAT;

    // Table des ecarts TAI-UTC
    static QList<QPair<double, double> > _ecartsTAI_UTC;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculJourJulien Calcul du jour julien 2000 (pour des dates du calendrier gregorien)
     */
    void CalculJourJulien();

    /**
     * @brief getDeltaAT Obtention de l'ecart TAI-UTC
     */
    void getDeltaAT();

    /**
     * @brief getHrAmPm Obtention de l'heure AM/PM
     * @param heure heure
     * @param systeme systeme horaire (SYSTEME_12H ou SYSTEME_24H)
     * @return heure AM/PM
     */
    QPair<int, QString> getHrAmPm(const int heure, const DateSysteme &systeme) const;

};

#endif // DATE_H
