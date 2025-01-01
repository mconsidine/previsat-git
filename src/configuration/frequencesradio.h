/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    frequencesradio.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Informations sur les frequences radio des satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 octobre 2022
 *
 * Date de revision
 * >    12 decembre 2024
 *
 */

#ifndef FREQUENCESRADIO_H
#define FREQUENCESRADIO_H

#include <QString>


/*
 * Definitions des structures
 */
struct FrequenceRadio {
    QString nom;
    QString frequenceMontante;
    QString frequenceDescendante;
    QString balise;
    QString mode;
    QString signalAppel;
};

#endif // FREQUENCESRADIO_H
