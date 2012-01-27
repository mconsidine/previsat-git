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
 * >    matrice.cpp
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
 * >
 *
 */

#include "matrice.h"

/*
 * Constructeur par defaut
 */
Matrice::Matrice()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'une matrice
 */
Matrice::Matrice(const Matrice &matrice)
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
Matrice::Matrice(const Vecteur3D &vecteur1, const Vecteur3D &vecteur2, const Vecteur3D &vecteur3)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _vecteur1 = vecteur1;
    _vecteur2 = vecteur2;
    _vecteur3 = vecteur3;

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Matrice::~Matrice()
{
}

/*
 * Calcul de la matrice transposee
 */
Matrice Matrice::Transposee()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const Vecteur3D l(_vecteur1.getX(), _vecteur2.getX(), _vecteur3.getX());
    const Vecteur3D m(_vecteur1.getY(), _vecteur2.getY(), _vecteur3.getY());
    const Vecteur3D n(_vecteur1.getZ(), _vecteur2.getZ(), _vecteur3.getZ());

    /* Retour */
    return (Matrice(l, m, n));
}

/*
 * Produit d'une matrice avec un vecteur 3D
 */
Vecteur3D Matrice::operator *(const Vecteur3D &vecteur)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double x = _vecteur1.getX() * vecteur.getX() + _vecteur2.getX() * vecteur.getY() +
            _vecteur3.getX() * vecteur.getZ();
    const double y = _vecteur1.getY() * vecteur.getX() + _vecteur2.getY() * vecteur.getY() +
            _vecteur3.getY() * vecteur.getZ();
    const double z = _vecteur1.getZ() * vecteur.getX() + _vecteur2.getZ() * vecteur.getY() +
            _vecteur3.getZ() * vecteur.getZ();

    /* Retour */
    return (Vecteur3D(x, y, z));
}

/*
 * Produit de 2 matrices
 */
Matrice Matrice::operator *(const Matrice &matrice)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return (Matrice((*this) * matrice._vecteur1, (*this) * matrice._vecteur2, (*this) * matrice._vecteur3));
}

/* Accesseurs */
Vecteur3D Matrice::getVecteur1() const
{
    return _vecteur1;
}

Vecteur3D Matrice::getVecteur2() const
{
    return _vecteur2;
}

Vecteur3D Matrice::getVecteur3() const
{
    return _vecteur3;
}
