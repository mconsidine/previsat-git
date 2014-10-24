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
 * >    25 octobre 2013
 *
 */

#ifndef VECTEUR3D_H
#define VECTEUR3D_H

class Vecteur3D
{
public:

    /* Constructeurs */
    Vecteur3D();
    Vecteur3D(const Vecteur3D &vecteur);
    Vecteur3D(const double x, const double y, const double z);
    ~Vecteur3D();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    bool isNul() const;
    double Norme() const;
    double Angle(const Vecteur3D &vecteur) const;
    Vecteur3D Normalise() const;

    friend inline Vecteur3D operator - (const Vecteur3D &vecteur);
    friend inline Vecteur3D operator + (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);
    friend inline Vecteur3D operator - (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);
    friend inline Vecteur3D operator * (const Vecteur3D &vecteur1, const double scalaire);
    friend inline double operator * (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);
    friend inline Vecteur3D operator ^ (const Vecteur3D &vecteur1, const Vecteur3D &vecteur2);

    /* Accesseurs */
    double getX() const;
    double getY() const;
    double getZ() const;


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
