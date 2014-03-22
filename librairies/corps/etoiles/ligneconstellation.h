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
 * >    22 mars 2014
 *
 */

#ifndef LIGNECONSTELLATION_H
#define LIGNECONSTELLATION_H

#include "librairies/corps/etoiles/etoile.h"

class LigneConstellation
{
public:

    /* Constructeurs */
    LigneConstellation();
    LigneConstellation(const Etoile &etoile1, const Etoile &etoile2);
    ~LigneConstellation();

    /* Constantes publiques */

    /* Variables publiques */
    static bool initLig;

    /* Methodes publiques */
    static void CalculLignesCst(const QList<Etoile> &etoiles, QList<LigneConstellation> &lignesCst);

    /* Accesseurs */
    bool isDessin() const;
    Etoile getEtoile1() const;
    Etoile getEtoile2() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */
    //static const int TABMAX = 646;

    /* Variables privees */
    bool _dessin;
    Etoile _etoile1;
    Etoile _etoile2;
    static QList<QVector<int> > _tabLigCst;

    /* Methodes privees */
    static void InitTabLignesCst();

};

#endif // LIGNECONSTELLATION_H
