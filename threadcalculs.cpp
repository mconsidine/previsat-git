/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include "threadcalculs.h"
#include "previsions/evenements.h"
#include "previsions/iridium.h"
#include "previsions/prevision.h"
#include "previsions/transitiss.h"

ThreadCalculs::ThreadCalculs(const TypeCalcul typeCalcul, const Conditions &conditions)
{
    _typeCalcul = typeCalcul;
    _conditions = conditions;
}

ThreadCalculs::ThreadCalculs(const TypeCalcul typeCalcul, const Conditions &conditions, const Observateur &observateur)
{
    _typeCalcul = typeCalcul;
    _conditions = conditions;
    _observateur = observateur;
}

void ThreadCalculs::run()
{
    switch (_typeCalcul) {
    case PREVISION:
        Prevision::CalculPassages(_conditions, _observateur);
        break;

    case IRIDIUM:
        Iridium::CalculFlashsIridium(_conditions, _observateur);
        break;

    case EVENEMENTS:
        Evenements::CalculEvenements(_conditions);
        break;

    case TRANSITS:
        TransitISS::CalculTransitsISS(_conditions, _observateur);

    default:
        break;
    }
}

ThreadCalculs::TypeCalcul ThreadCalculs::getTypeCalcul() const
{
    return _typeCalcul;
}
