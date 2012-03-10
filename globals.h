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
 * >    globals.h
 *
 * Localisation
 * >
 *
 * Heritage
 * >
 *
 * Description
 * >     Variables globales
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 mars 2012
 *
 * Date de revision
 * >
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include "librairies/corps/satellite/tle.h"

// Repertoires
static QString dirDat;
static QString dirOut;
static QString dirTle;
static QString dirTmp;

// Satellites
static QString nomfic;
static QStringList liste;
static QVector<TLE> tles;

#endif // GLOBALS_H
