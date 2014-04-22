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
 * >    constellation.h
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Heritage
 * >    Corps
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 mars 2012
 *
 * Date de revision
 * >    22 avril 2014
 *
 */

#ifndef CONSTELLATION_H
#define CONSTELLATION_H

#include <QString>
#include "librairies/corps/corps.h"
#include "librairies/observateur/observateur.h"

class Constellation : public Corps
{
public:

    /* Constructeurs */
    Constellation();
    Constellation(const QString &nom, const double ascensionDroite, const double declinaison);
    ~Constellation();

    /* Constantes publiques */

    /* Variables publiques */
    static bool initCst;

    /* Methodes publiques */
    static void CalculConstellations(const Observateur &observateur, QList<Constellation> &constellations);

    /* Accesseurs */
    QString getNom() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    QString _nom;

    /* Methodes privees */
    static void InitTabCst(QList<Constellation> &constellations);

};

#endif // CONSTELLATION_H
