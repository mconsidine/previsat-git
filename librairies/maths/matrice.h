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
 * >    matrice.h
 *
 * Localisation
 * >    librairies.maths
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

#ifndef MATRICE_H
#define MATRICE_H

#include "librairies/maths/vecteur3d.h"

class Matrice
{
public:

    /* Constructeurs */
    Matrice();
    Matrice(const Matrice &matrice);
    Matrice(const Vecteur3D &vecteur1, const Vecteur3D &vecteur2, const Vecteur3D &vecteur3);
    ~Matrice();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    Matrice Transposee();
    Vecteur3D operator * (const Vecteur3D &vecteur);
    Matrice operator * (const Matrice &matrice);

    /* Accesseurs */
    Vecteur3D getVecteur1() const;
    Vecteur3D getVecteur2() const;
    Vecteur3D getVecteur3() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    Vecteur3D _vecteur1;
    Vecteur3D _vecteur2;
    Vecteur3D _vecteur3;

    /* Methodes privees */

};

#endif // MATRICE_H
