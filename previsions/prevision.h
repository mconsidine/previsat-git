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
 * >    prevision.h
 *
 * Localisation
 * >    previsions
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
 * >    3 juin 2015
 *
 */

#ifndef PREVISION_H
#define PREVISION_H

#include <QVector>
#include "conditions.h"
#include "librairies/observateur/observateur.h"

class Prevision
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculPassages Calcul des previsions de passage
     * @param conditions conditions d'observation
     * @param observateur observateur
     * @param res tableau des resultats
     */
    static void CalculPassages(const Conditions &conditions, Observateur &observateur, QStringList &res);

    /**
     * @brief FinTraitement Finalisation du traitement (liberation memoire)
     */
    static void FinTraitement();

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    /**
     * @brief CalculEphemSoleilObservateur Calcul des ephemerides du Soleil et de l'observateur
     * @param conditions conditions d'observation
     * @param observateur observateur
     */
    static void CalculEphemSoleilObservateur(const Conditions &conditions, Observateur &observateur);

};

#endif // PREVISION_H
