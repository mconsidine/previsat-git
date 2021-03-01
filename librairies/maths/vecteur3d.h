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
 * >    vecteur3d.h
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
 * >    2 avril 2018
 *
 */

#ifndef VECTEUR3D_H
#define VECTEUR3D_H

#include "mathsConstants.h"


class Vecteur3D
{
public:

    /* Constructeurs */
    /**
     * @brief Vecteur3D Constructeur par defaut : creation d'un vecteur nul
     */
    Vecteur3D();

    /**
     * @brief Vecteur3D Constructeur a partir d'un objet Vecteur3d
     * @param vecteur vecteur 3D
     */
    Vecteur3D(const Vecteur3D &vecteur);

    /**
     * @brief Vecteur3D Constructeur a partir des composantes du vecteur
     * @param xval Composante x
     * @param yval Composante y
     * @param zval Composante z
     */
    Vecteur3D(const double xval, const double yval, const double zval);

    ~Vecteur3D();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief Angle Calcul de l'angle entre 2 vecteurs
     * @param vecteur vecteur
     * @return angle entre les deux vecteurs
     */
    double Angle(const Vecteur3D &vecteur) const;

    /**
     * @brief Normalise Determination du vecteur unitaire
     * @return vecteur unitaire
     */
    Vecteur3D Normalise() const;

    /**
     * @brief isNul Verifie si le vecteur est nul
     * @return vrai si le vecteur est nul
     */
    bool isNul() const;

    /**
     * @brief Norme Calcul de la norme du vecteur 3D
     * @return norme du vecteur
     */
    double Norme() const;

    /**
     * @brief Rotation Rotation d'un vecteur autour d'un axe
     * @param axe axe (AXE_X, AXE_Y ou AXE_Z)
     * @param angle angle de rotation (rad)
     * @return vecteur issu de la rotation
     */
    Vecteur3D Rotation(AxeType axe, double angle) const;

    /**
     * @brief operator = Affectation d'un vecteur 3D
     * @param vecteur vecteur
     */
    Vecteur3D &operator = (const Vecteur3D &vecteur);

    /**
     * @brief operator - Oppose du vecteur 3D
     * @param vecteur vecteur
     * @return Oppose du vecteur 3D
     */
    friend inline Vecteur3D operator - (const Vecteur3D &vecteur);

    /**
     * @brief operator + Somme de deux vecteurs 3D
     * @param vecteur1 vecteur1
     * @param vecteur2 vecteur2
     * @return Somme de deux vecteurs 3D
     */
    friend inline Vecteur3D operator + (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);

    /**
     * @brief operator - Difference de deux vecteurs 3D
     * @param vecteur1 vecteur1
     * @param vecteur2 vecteur2
     * @return Difference de deux vecteurs 3D
     */
    friend inline Vecteur3D operator - (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);

    /**
     * @brief operator * Produit d'un vecteur 3D par un scalaire
     * @param vecteur1 vecteur
     * @param scalaire grandeur scalaire
     * @return Produit d'un vecteur 3D par un scalaire
     */
    friend inline Vecteur3D operator * (const Vecteur3D &vecteur1, const double scalaire);

    /**
     * @brief operator * Produit scalaire de deux vecteurs 3D
     * @param vecteur1 vecteur1
     * @param vecteur2 vecteur2
     * @return Produit scalaire de deux vecteurs 3D
     */
    friend inline double operator * (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);

    /**
     * @brief operator ^ Produit vectoriel de deux vecteurs 3D
     * @param vecteur1 vecteur1
     * @param vecteur2 vecteur2
     * @return Produit vectoriel de deux vecteurs 3D
     */
    friend inline Vecteur3D operator ^ (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);

    /* Accesseurs */
    double x() const;
    double y() const;
    double z() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    double _x;
    double _y;
    double _z;

    /* Methodes privees */

};


/*
 * Oppose du vecteur
 */
inline Vecteur3D operator - (const Vecteur3D &vecteur)
{
    return (Vecteur3D(-vecteur._x, -vecteur._y, -vecteur._z));
}

/*
 * Somme de 2 vecteurs 3D
 */
inline Vecteur3D operator + (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2)
{
    return (Vecteur3D(vecteur1._x + vecteur2._x, vecteur1._y + vecteur2._y, vecteur1._z + vecteur2._z));
}

/*
 * Difference de 2 vecteurs 3D
 */
inline Vecteur3D operator - (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2)
{
    return (Vecteur3D(vecteur1._x - vecteur2._x, vecteur1._y - vecteur2._y, vecteur1._z - vecteur2._z));
}

/*
 * Multiplication d'un vecteur 3D par un scalaire
 */
inline Vecteur3D operator * (const Vecteur3D &vecteur1, const double scalaire)
{
    return (Vecteur3D(vecteur1._x * scalaire, vecteur1._y * scalaire, vecteur1._z * scalaire));
}

/*
 * Produit scalaire de 2 vecteurs 3D
 */
inline double operator * (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2)
{
    return (vecteur1._x * vecteur2._x + vecteur1._y * vecteur2._y + vecteur1._z * vecteur2._z);
}

/*
 * Produit vectoriel de 2 vecteurs 3D
 */
inline Vecteur3D operator ^ (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2)
{
    return (Vecteur3D(vecteur1._y * vecteur2._z - vecteur1._z * vecteur2._y,
                      vecteur1._z * vecteur2._x - vecteur1._x * vecteur2._z,
                      vecteur1._x * vecteur2._y - vecteur1._y * vecteur2._x));
}

#endif // VECTEUR3D_H
