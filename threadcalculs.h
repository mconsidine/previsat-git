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
 * >    threadcalculs.h
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QThread
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    3 mars 2012
 *
 * Date de revision
 * >
 *
 */

#ifndef THREADCALCULS_H
#define THREADCALCULS_H

#include <QThread>
#include "librairies/observateur/observateur.h"
#include "previsions/conditions.h"

class ThreadCalculs : public QThread
{
public:
    enum TypeCalcul {
        PREVISION,
        IRIDIUM,
        EVENEMENTS,
        TRANSITS
    };

    ThreadCalculs(const TypeCalcul typeCalcul, const Conditions &conditions);
    ThreadCalculs(const TypeCalcul typeCalcul, const Conditions &conditions, const Observateur &observateur);
    void run();

    TypeCalcul getTypeCalcul() const;

private:
    TypeCalcul _typeCalcul;
    Conditions _conditions;
    Observateur _observateur;
};

#endif // THREADCALCULS_H
