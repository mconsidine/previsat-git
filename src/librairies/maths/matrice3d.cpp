/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    matrice3d.cpp
 *
 * Localisation
 * >    librairies.maths
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

#include <cmath>
#include "matrice3d.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Definition d'une matrice de rotation
 */
Matrice3D::Matrice3D(const AxeType &axe, const double ang)
{
    /* Declarations des variables locales */
    Vecteur3D vec1, vec2, vec3;

    /* Initialisations */
    const double cosang = cos(ang);
    const double sinang = sin(ang);

    /* Corps du constructeur */
    switch (axe) {

    case AXE_X:
        vec1 = Vecteur3D(1., 0., 0.);
        vec2 = Vecteur3D(0., cosang, -sinang);
        vec3 = Vecteur3D(0., sinang, cosang);
        break;

    case AXE_Y:
        vec1 = Vecteur3D(cosang, 0., sinang);
        vec2 = Vecteur3D(0., 1., 0.);
        vec3 = Vecteur3D(-sinang, 0., cosang);
        break;

    case AXE_Z:
        vec1 = Vecteur3D(cosang, -sinang, 0.);
        vec2 = Vecteur3D(sinang, cosang, 0.);
        vec3 = Vecteur3D(0., 0., 1.);
        break;

    default:
        break;
    }

    _vecteur1 = vec1;
    _vecteur2 = vec2;
    _vecteur3 = vec3;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
Vecteur3D Matrice3D::vecteur1() const
{
    return _vecteur1;
}

Vecteur3D Matrice3D::vecteur2() const
{
    return _vecteur2;
}

Vecteur3D Matrice3D::vecteur3() const
{
    return _vecteur3;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la matrice 3x3 transposee
 */
Matrice3D Matrice3D::Transposee() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const Vecteur3D l(_vecteur1.x(), _vecteur2.x(), _vecteur3.x());
    const Vecteur3D m(_vecteur1.y(), _vecteur2.y(), _vecteur3.y());
    const Vecteur3D n(_vecteur1.z(), _vecteur2.z(), _vecteur3.z());

    /* Retour */
    return Matrice3D(l, m, n);
}


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */



