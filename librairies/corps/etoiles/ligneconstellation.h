/*
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
 * >    ligneconstellation.h
 *
 * Localisation
 * >    librairies.corps.etoiles
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

#ifndef LIGNECONSTELLATION_H
#define LIGNECONSTELLATION_H

#include "etoile.h"

class LigneConstellation
{
public:

    /* Constructeurs */
    /**
     * @brief LigneConstellation Constructeur par defaut
     */
    LigneConstellation();

    /**
     * @brief LigneConstellation Defiinition a partie de 2 etoiles
     * @param star1 etoile 1
     * @param star2 etoile 2
     */
    LigneConstellation(const Etoile &star1, const Etoile &star2);

    ~LigneConstellation();

    /* Constantes publiques */

    /* Variables publiques */
    static bool initLig;

    /* Methodes publiques */
    /**
     * @brief CalculLignesCst Calcul des lignes de constellations
     * @param etoiles tableau d'etoiles
     * @param lignesCst tableau des constellations
     */
    static void CalculLignesCst(const QList<Etoile> &etoiles, QList<LigneConstellation> &lignesCst);

    /* Accesseurs */
    bool isDessin() const;
    Etoile etoile1() const;
    Etoile etoile2() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    bool _dessin;
    Etoile _etoile1;
    Etoile _etoile2;
    static QList<QVector<int> > _tabLigCst;

    /* Methodes privees */
    static void InitTabLignesCst();

};

#endif // LIGNECONSTELLATION_H
