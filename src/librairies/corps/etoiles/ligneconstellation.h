/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    ligneconstellation.h
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Heritage
 * >
 *
 * Description
 * >    Definition d'une ligne de constellation
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    4 decembre 2015
 *
 */

#ifndef LIGNECONSTELLATION_H
#define LIGNECONSTELLATION_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QList>
#include <QPair>
#pragma GCC diagnostic warning "-Wconversion"
#include "etoile.h"


class LigneConstellation
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief LigneConstellation Constructeur par defaut
     */
    LigneConstellation();

    /**
     * @brief LigneConstellation Defiinition a partie de 2 etoiles
     * @param[in] star1 etoile 1
     * @param[in] star2 etoile 2
     */
    LigneConstellation(const Etoile &star1, const Etoile &star2) :
        _etoile1(star1),
        _etoile2(star2) {
        _dessin = (_etoile1.isVisible() && _etoile2.isVisible());
    }


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculLignesCst Calcul des lignes de constellations
     * @param[in] etoiles tableau d'etoiles
     * @param[out] lignesCst tableau de lignes de constellation
     */
    static void CalculLignesCst(const QList<Etoile> &etoiles, QList<LigneConstellation> &lignesCst);

    /**
     * @brief Initialisation Lecture du fichier contenant les lignes de constellations
     * @param[in] dirCommonData chemin des donnees communes
     */
    static void Initialisation(const QString &dirCommonData);


    /*
     * Accesseurs
     */
    bool isDessin() const;
    const Etoile &etoile1() const;
    const Etoile &etoile2() const;


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */
    bool _dessin;
    Etoile _etoile1;
    Etoile _etoile2;
    static QList<QPair<int, int> > _tabLigCst;


    /*
     * Methodes privees
     */


};


#endif // LIGNECONSTELLATION_H
