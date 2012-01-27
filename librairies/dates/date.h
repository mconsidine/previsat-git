/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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

#ifndef DATE_H
#define DATE_H

#include <QString>
#include "dateConstants.h"

class Date
{
public:

    /* Constructeurs */
    Date();
    Date(const double offsetUTC);
    Date(const Date &date, const double offsetUTC = 0.);
    Date(const double jourJulien, const double offsetUTC, const bool acalc = true);
    Date(const int annee, const int mois, const double xjour, const double offsetUTC);
    Date(const int annee, const int mois, const int jour, const int heure, const int minutes,
         const double secondes, const double offsetUTC);
    ~Date();

    /* Enumerations */
    enum DateFormat { COURT, LONG };

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    Date ToLocalDate() const;
    Date ToLocalDate(const double offsetUTC) const;
    QString ToShortDate(const DateFormat format) const;
    QString ToLongDate() const;

    /* Accesseurs */
    int getAnnee() const;
    int getHeure() const;
    int getJour() const;
    int getMinutes() const;
    int getMois() const;
    double getJourJulien() const;
    double getJourJulienUTC() const;
    double getOffsetUTC() const;
    double getSecondes() const;

protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    int _annee;
    int _mois;
    int _jour;
    int _heure;
    int _minutes;
    double _secondes;

    double _jourJulien;
    double _jourJulienUTC;
    double _offsetUTC;

    /* Methodes privees */
    void CalculJourJulien();

};

#endif // DATE_H
