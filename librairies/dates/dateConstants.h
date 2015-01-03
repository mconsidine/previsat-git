/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
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
 * >    dateConstants.h
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
 * >    14 juin 2013
 *
 */

#ifndef DATECONSTANTS_H
#define DATECONSTANTS_H

enum DateFormat {
    COURT,
    LONG
};

enum DateSysteme {
    SYSTEME_24H,
    SYSTEME_12H
};

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

static const double NB_JOUR_PAR_HEUR = 1. / NB_HEUR_PAR_JOUR;
static const double NB_JOUR_PAR_MIN = 1. / NB_MIN_PAR_JOUR;
static const double NB_JOUR_PAR_SEC = 1. / NB_SEC_PAR_JOUR;

static const double NB_MIN_PAR_SEC = 1. / NB_SEC_PAR_MIN;
static const double NB_HEUR_PAR_MIN = 1. / NB_MIN_PAR_HEUR;
static const double NB_HEUR_PAR_SEC = 1. / NB_SEC_PAR_HEUR;

static const double NB_ANJ_PAR_JOURS = 1. / NB_JOURS_PAR_ANJ;
static const double NB_SIECJ_PAR_JOURS = 1. / NB_JOURS_PAR_SIECJ;

#endif // DATECONSTANTS_H
