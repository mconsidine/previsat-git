/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    etoile.h
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
 * >
 *
 */

#ifndef ETOILE_H
#define ETOILE_H

#include <QList>
#include <QString>
#include "librairies/corps/corps.h"

class Etoile : public Corps
{
public:

    /* Constructeurs */
    Etoile();
    Etoile(const QString nom, const double ascensionDroite, const double declinaison, const double magnitude);
    ~Etoile();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    static void CalculPositionEtoiles(const Observateur &observateur);
    void CalculCoordHoriz(const Observateur &observateur);

    /* Accesseurs */
    static QList<Etoile> getEtoiles();


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    static bool _initStar;
    double _magnitude;
    QString _nom;
    static QList<Etoile> _etoiles;

    /* Methodes privees */
    static void InitTabEtoiles();

};

#endif // ETOILE_H
