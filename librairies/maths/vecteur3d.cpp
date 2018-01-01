/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2018  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    14 juin 2015
 *
 */

#include <cmath>
#include "librairies/exceptions/previsatexception.h"
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
Vecteur3D::Vecteur3D(const double xval, const double yval, const double zval)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _x = xval;
    _y = yval;
    _z = zval;

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
 * Calcul de l'angle entre 2 vecteurs
 */
double Vecteur3D::Angle(const Vecteur3D &vecteur) const
{
    try {

        /* Declarations des variables locales */

        /* Initialisations */

        /* Corps de la methode */
        const double norme1 = Norme();
        const double norme2 = vecteur.Norme();
        const double cosang = (*this) * vecteur / (norme1 * norme2);
        const double res = (cosang < 1.) ? acos(cosang) : (fabs(cosang - 1.) < EPSDBL) ? 0. : -1.;

        /* Retour */
        return ((norme1 < EPSDBL || norme2 < EPSDBL) ? 0. : res);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }
}

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

Vecteur3D Vecteur3D::Normalise() const
{
    try {

        /* Declarations des variables locales */

        /* Initialisations */
        const double norme = Norme();
        const double val = (norme < EPSDBL100) ? 1. : 1. / norme;

        /* Corps de la methode */

        /* Retour */
        return ((*this) * val);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }
}

Vecteur3D Vecteur3D::Rotation(AxeType axe, double angle) const
{
    /* Declarations des variables locales */
    Vecteur3D vecteur;

    /* Initialisations */
    const double cosang = cos(angle);
    const double sinang = sin(angle);

    /* Corps de la methode */
    switch (axe) {

    case AXE_X:

        vecteur = Vecteur3D(_x, _y * cosang + _z * sinang, -_y * sinang + _z * cosang);
        break;

    case AXE_Y:

        vecteur = Vecteur3D(_x * cosang - _z * sinang, _y, _x * sinang + _z * cosang);
        break;

    case AXE_Z:

        vecteur = Vecteur3D(_x * cosang + _y * sinang, -_x * sinang + _y * cosang, _z);
        break;

    default:
        break;
    }

    /* Retour */
    return (vecteur);
}


/* Accesseurs */
double Vecteur3D::x() const
{
    return _x;
}

double Vecteur3D::y() const
{
    return _y;
}

double Vecteur3D::z() const
{
    return _z;
}
