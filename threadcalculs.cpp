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
 * >    threadcalculs.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QThread
 *
 * Description
 * >    Thread pour le lancement des calculs de prevision
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    3 mars 2012
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#include "threadcalculs.h"
#include "previsions/evenements.h"
#include "previsions/iridium.h"
#include "previsions/prevision.h"
#include "previsions/transitiss.h"

static Observateur observ;
static QStringList result;

ThreadCalculs::ThreadCalculs(const TypeCalcul typeCalc, const Conditions &cond)
{
    _typeCalcul = typeCalc;
    _conditions = cond;
}

ThreadCalculs::ThreadCalculs(const TypeCalcul typeCalc, const Conditions &cond, const Observateur &obs)
{
    _typeCalcul = typeCalc;
    _conditions = cond;
    observ = obs;
}

void ThreadCalculs::run()
{
    switch (_typeCalcul) {
    case PREVISION:
        Prevision::CalculPassages(_conditions, observ, result
                                  );
        break;

    case IRIDIUM:
        Iridium::CalculFlashsIridium(_conditions, observ, result);
        break;

    case EVENEMENTS:
        Evenements::CalculEvenements(_conditions);
        break;

    case TRANSITS:
        TransitISS::CalculTransitsISS(_conditions, observ, result);

    default:
        break;
    }
}

ThreadCalculs::TypeCalcul ThreadCalculs::typeCalcul() const
{
    return _typeCalcul;
}

Observateur ThreadCalculs::observateur() const
{
    return observ;
}

QStringList ThreadCalculs::res()
{
    return result;
}
