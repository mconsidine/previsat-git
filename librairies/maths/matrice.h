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
 * >    3 novembre 2013
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
    friend inline Vecteur3D operator * (const Matrice &matrice, const Vecteur3D &vecteur);
    friend inline Matrice operator * (const Matrice &matrice1, const Matrice &matrice2);

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

/*
 * Produit d'une matrice avec un vecteur 3D
 */
inline Vecteur3D operator * (const Matrice &matrice, const Vecteur3D &vecteur)
{
    const double x = matrice._vecteur1.getX() * vecteur.getX() + matrice._vecteur2.getX() * vecteur.getY() + matrice._vecteur3.getX() * vecteur.getZ();
    const double y = matrice._vecteur1.getY() * vecteur.getX() + matrice._vecteur2.getY() * vecteur.getY() + matrice._vecteur3.getY() * vecteur.getZ();
    const double z = matrice._vecteur1.getZ() * vecteur.getX() + matrice._vecteur2.getZ() * vecteur.getY() + matrice._vecteur3.getZ() * vecteur.getZ();

    return (Vecteur3D(x, y, z));
}

/*
 * Produit de 2 matrices
 */
inline Matrice operator * (const Matrice &matrice1, const Matrice &matrice2)
{
    return (Matrice(matrice1 * matrice2._vecteur1, matrice1 * matrice2._vecteur2, matrice1 * matrice2._vecteur3));
}

#endif // MATRICE_H
