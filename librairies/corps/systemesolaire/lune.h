/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    lune.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
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

#ifndef LUNE_H
#define LUNE_H

#include "librairies/corps/corps.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/corps/systemesolaire/LuneConstants.h"

class Lune : public Corps
{
public:

    /* Constructeurs */
    Lune();
    ~Lune();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void CalculPosition(const Date &date);
    void CalculPhase(const Soleil &soleil);

    /* Accesseurs */
    double getFractionIlluminee() const;
    QString getPhase() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    double _fractionIlluminee;
    QString _phase;

    /* Methodes privees */

};

#endif // LUNE_H
