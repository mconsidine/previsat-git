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
 * >    vecteur3d.cpp
 *
 * Localisation
 * >    librairies.maths
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul vectoriel
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    22 avril 2014
 *
 */

#include <cmath>
#include "mathConstants.h"
#include "vecteur3d.h"

/* Constructeurs */
/*
 * Constructeur par defaut
 */
Vecteur3D::Vecteur3D()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _x = 0.;
    _y = 0.;
    _z = 0.;

    /* Retour */
    return;
}

/*
 * Constructeur a partir d'un vecteur 3D
 */
Vecteur3D::Vecteur3D(const Vecteur3D &vecteur)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _x = vecteur._x;
    _y = vecteur._y;
    _z = vecteur._z;

    /* Retour */
    return;
}

/*
 * Constructeur a partir de 3 nombres reels
 */
Vecteur3D::Vecteur3D(const double x, const double y, const double z)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _x = x;
    _y = y;
    _z = z;

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Vecteur3D::~Vecteur3D()
{
}

/* Methodes */
/*
 * Test si un vecteur est nul
 */
bool Vecteur3D::isNul() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return (_x < EPSDBL100 && _y < EPSDBL100 && _z < EPSDBL100);
}

/*
 * Calcul de la norme du vecteur 3D
 */
double Vecteur3D::Norme() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return (sqrt(_x * _x + _y * _y + _z * _z));
}

/*
 * Calcul de l'angle entre 2 vecteurs
 */
double Vecteur3D::Angle(const Vecteur3D &vecteur)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double norme1 = Norme();
    const double norme2 = vecteur.Norme();
    const double cosang = (*this) * vecteur / (norme1 * norme2);
    const double res = (cosang < 1.) ? acos(cosang) : (fabs(cosang - 1.) < EPSDBL) ? 0. : -1.;

    /* Retour */
    return ((norme1 < EPSDBL || norme2 < EPSDBL) ? 0. : res);
}

/*
 * Calcul du vecteur 3D unitaire
 */
Vecteur3D Vecteur3D::Normalise()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double norme = Norme();
    const double val = (norme < EPSDBL100) ? 1. : 1. / norme;

    /* Corps de la methode */

    /* Retour */
    return ((*this) * val);
}


/* Accesseurs */
double Vecteur3D::getX() const
{
    return _x;
}

double Vecteur3D::getY() const
{
    return _y;
}

double Vecteur3D::getZ() const
{
    return _z;
}
