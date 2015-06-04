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
 * >    matrice3d.cpp
 *
 * Localisation
 * >    librairies.maths
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul matriciel
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

#include "matrice3d.h"

/*
 * Constructeur par defaut
 */
Matrice3D::Matrice3D()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une Matrice3D
 */
Matrice3D::Matrice3D(const Matrice3D &matrice)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _vecteur1 = matrice._vecteur1;
    _vecteur2 = matrice._vecteur2;
    _vecteur3 = matrice._vecteur3;

    /* Retour */
    return;
}

/*
 * Constructeur a partir de 3 vecteurs colonnes
 */
Matrice3D::Matrice3D(const Vecteur3D &vect1, const Vecteur3D &vect2, const Vecteur3D &vect3)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _vecteur1 = vect1;
    _vecteur2 = vect2;
    _vecteur3 = vect3;

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Matrice3D::~Matrice3D()
{
}

/*
 * Calcul de la Matrice3D transposee
 */
Matrice3D Matrice3D::Transposee()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const Vecteur3D l(_vecteur1.x(), _vecteur2.x(), _vecteur3.x());
    const Vecteur3D m(_vecteur1.y(), _vecteur2.y(), _vecteur3.y());
    const Vecteur3D n(_vecteur1.z(), _vecteur2.z(), _vecteur3.z());

    /* Retour */
    return (Matrice3D(l, m, n));
}


/* Accesseurs */
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


