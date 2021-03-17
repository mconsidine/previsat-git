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
 * >    constellation.h
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Definition d'une constellation
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 mars 2012
 *
 * Date de revision
 * >    3 juin 2015
 *
 */

#ifndef CONSTELLATION_H
#define CONSTELLATION_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QList>
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/corps/corps.h"


class Constellation : public Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Constellation Constructeur par defaut
     */
    Constellation();

    /**
     * @brief Constellation Definition a partir des composantes
     * @param nomConst nom de la constellation
     * @param ascDroite ascension droite moyenne de la constellation
     * @param decl declinaison myenne de la declinaison
     */
    Constellation(const QString &nomConst, const double ascDroite, const double decl);


    /*
     * Accesseurs
     */
    QString nom() const;
    static QList<Constellation> &constellations();


    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculConstellations Calcul des positions des noms des constellations pour la carte du ciel
     * @param observateur observateur
     * @param constellations tableau des constellations
     */
    static void CalculConstellations(const Observateur &observateur);

    /**
     * @brief Initialisation Lecture du fichier de constellations
     * @param dirCommonData chemin des donnees communes
     */
    static void Initialisation(const QString &dirCommonData);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    QString _nom;
    static QList<Constellation> _constellations;

    /*
     * Methodes privees
     */


};


#endif // CONSTELLATION_H
