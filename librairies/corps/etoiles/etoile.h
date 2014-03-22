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
 * >    22 mars 2014
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
    Etoile(const QString &nom, const double ascensionDroite, const double declinaison, const double magnitude);
    ~Etoile();

    /* Constantes publiques */

    /* Variables publiques */
    static bool initStar;

    /* Methodes publiques */
    static void CalculPositionEtoiles(const Observateur &observateur, QList<Etoile> &etoiles);

    /* Accesseurs */
    double getMagnitude() const;
    QString getNom() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    double _magnitude;
    QString _nom;

    /* Methodes privees */
    static void InitTabEtoiles(QList<Etoile> &etoiles);

};

#endif // ETOILE_H
