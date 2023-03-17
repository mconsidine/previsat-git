/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    17 mars 2023
 *
 */

#include <cmath>
#include "vecteur3d.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Vecteur3D::Vecteur3D(const double valeurInit)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _x = valeurInit;
    _y = valeurInit;
    _z = valeurInit;

    /* Retour */
    return;
}

/*
 * Definition du vecteur a partir de ses composantes
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
 * Methodes publiques
 */
/*
 * Angle entre 2 vecteurs
 */
double Vecteur3D::Angle(const Vecteur3D &vecteur) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    double res = 0.;
    const double norme1 = Norme();
    const double norme2 = vecteur.Norme();

    /* Corps de la methode */
    if ((norme1 > MATHS::EPSDBL) && (norme2 > MATHS::EPSDBL)) {
        const double cosang = (*this) * vecteur / (norme1 * norme2);
        res = (cosang < 1.) ? acos(cosang) : 0.;
    }

    /* Retour */
    return res;
}

/*
 * Determination du vecteur unitaire
 */
Vecteur3D Vecteur3D::Normalise() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double norme = Norme();
    const double val = (norme < MATHS::EPSDBL) ? 1. : 1. / norme;

    /* Corps de la methode */

    /* Retour */
    return ((*this) * val);
}

/*
 * Norme du vecteur
 */
double Vecteur3D::Norme() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return std::hypot(_x, _y, _z);
}

/*
 * Test si un vecteur est nul
 */
bool Vecteur3D::Nul() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return ((fabs(_x) < MATHS::EPSDBL) && (fabs(_y) < MATHS::EPSDBL) && (fabs(_z) < MATHS::EPSDBL));
}

/*
 * Rotation d'un vecteur autour d'un axe
 */
Vecteur3D Vecteur3D::Rotation(const AxeType &axe, const double angle) const
{
    /* Declarations des variables locales */
    Vecteur3D vecteur;

    /* Initialisations */
    const double cosang = cos(angle);
    const double sinang = sin(angle);

    /* Corps de la methode */
    switch (axe) {

    case AxeType::AXE_X:

        vecteur = Vecteur3D(_x, _y * cosang + _z * sinang, -_y * sinang + _z * cosang);
        break;

    case AxeType::AXE_Y:

        vecteur = Vecteur3D(_x * cosang - _z * sinang, _y, _x * sinang + _z * cosang);
        break;

    case AxeType::AXE_Z:

        vecteur = Vecteur3D(_x * cosang + _y * sinang, -_x * sinang + _y * cosang, _z);
        break;

    default:
        break;
    }

    /* Retour */
    return (vecteur);
}


/*
 * Accesseurs
 */
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

