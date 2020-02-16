/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    dateconst.h
 *
 * Localisation
 * >    librairies.dates
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes temporelles
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    27 decembre 2018
 *
 */

#ifndef DATESCONST_H
#define DATESCONST_H


/*
 * Enumerations
 */
enum DateFormat {
    FORMAT_COURT = 0,
    FORMAT_LONG = 1,
    FORMAT_MILLISEC = 3
};

enum DateSysteme {
    SYSTEME_24H,
    SYSTEME_12H
};


/*
 * Definitions des constantes
 */
static const int AN2000 = 2000;

static const double EPS_DATES = 1.e-7;
static const double TJ2000 = 2451545.;
static const double DATE_INFINIE = 9999999.;

static const double NB_HEUR_PAR_JOUR = 24.;
static const double NB_MIN_PAR_HEUR = 60.;
static const double NB_MIN_PAR_JOUR = 1440.;
static const double NB_SEC_PAR_MIN = 60.;
static const double NB_SEC_PAR_HEUR = 3600.;
static const double NB_SEC_PAR_JOUR = 86400.;

static const double NB_JOURS_PAR_ANJ = 365.25;
static const double NB_JOURS_PAR_SIECJ = 36525.;

// Ecart constant TAI-TT (en secondes)
static const double NB_SEC_TT_TAI = 32.184;

static const double NB_JOUR_PAR_HEUR = 1. / NB_HEUR_PAR_JOUR;
static const double NB_JOUR_PAR_MIN = 1. / NB_MIN_PAR_JOUR;
static const double NB_JOUR_PAR_SEC = 1. / NB_SEC_PAR_JOUR;

static const double NB_MIN_PAR_SEC = 1. / NB_SEC_PAR_MIN;
static const double NB_HEUR_PAR_MIN = 1. / NB_MIN_PAR_HEUR;
static const double NB_HEUR_PAR_SEC = 1. / NB_SEC_PAR_HEUR;

static const double NB_ANJ_PAR_JOURS = 1. / NB_JOURS_PAR_ANJ;
static const double NB_SIECJ_PAR_JOURS = 1. / NB_JOURS_PAR_SIECJ;


#endif // DATESCONST_H
