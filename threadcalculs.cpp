/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    18 novembre 2018
 *
 */

#include "threadcalculs.h"
#include "previsions/evenements.h"
#include "previsions/metop.h"
#include "previsions/prevision.h"
#include "previsions/transitiss.h"

Observateur ThreadCalculs::_observ;
static QStringList result;

void ThreadCalculs::run()
{
    switch (_conditions.typeCalcul()) {
    case PREVISION:
        Prevision::CalculPassages(_conditions, _observ, result);
        break;

    case EVENEMENTS:
        Evenements::CalculEvenements(_conditions);
        break;

    case TRANSITS:
        TransitISS::CalculTransitsISS(_conditions, _observ, result);
        break;

    case METOP:
        MetOp::CalculFlashsMetOp(_conditions, _observ, result);
        break;

    default:
        break;
    }
}

TypeCalcul ThreadCalculs::typeCalcul() const
{
    return _conditions.typeCalcul();
}

Observateur ThreadCalculs::observateur()
{
    return _observ;
}

QStringList ThreadCalculs::res()
{
    return result;
}
