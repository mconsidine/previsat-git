﻿/*
 *     PreviSat, Satellite tracking software
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
 * >    4 decembre 2015
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

    /* Constructeurs */
    /**
     * @brief ThreadCalculs Constructeur avec conditions
     * @param cond conditions d'observation
     */
    explicit ThreadCalculs(const Conditions &cond) :
        _conditions(cond) { }

    /**
     * @brief ThreadCalculs Constructeur avec conditions et lieu d'observation
     * @param cond conditions d'observation
     * @param obs lieu d'observation
     */
    ThreadCalculs(const Conditions &cond, const Observateur &obs) :
        _conditions(cond) {
        _observ = obs;
    }

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief run Lancement du thread
     */
    void run();

    /* Accesseurs */
    static Observateur observateur();
    QStringList res();
    TypeCalcul typeCalcul() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    Conditions _conditions;
    static Observateur _observ;

    /* Methodes privees */


};

#endif // THREADCALCULS_H
