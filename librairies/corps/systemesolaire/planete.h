/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
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
 * >    planete.h
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
 * >    28 janvier 2012
 *
 * Date de revision
 * >    22 avril 2014
 *
 */

#ifndef PLANETE_H
#define PLANETE_H

#include "librairies/corps/corps.h"
#include "planeteConstants.h"
#include "soleil.h"

class Planete : public Corps
{

public:

    /* Constructeurs */
    Planete(const int iplanete);

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void CalculPosition(const Date &date, const Soleil &soleil);

    /* Accesseurs */
    QString getNom() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    static bool _init;
    int _iplanete;
    double _elem[6];
    Vecteur3D _positionSph;
    QString _nom;

    /* Methodes privees */
    void CalculElements(const Date &date);
    void CalculCoordonneesSpheriques();

};

#endif // PLANETE_H
