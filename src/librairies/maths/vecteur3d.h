/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    25 fevrier 2023
 *
 */

#ifndef VECTEUR3D_H
#define VECTEUR3D_H

#include "mathsconst.h"


class Vecteur3D
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Vecteur3D Constructeur par defaut
     * @param[in] valeurInit valeur d'initialisation
     */
    explicit Vecteur3D(const double valeurInit = 0.);

    /**
     * @brief Vecteur3D Definition a partir d'un vecteur
     * @param[in] vecteur vecteur
     */
    Vecteur3D(const Vecteur3D &vecteur) = default;

    /**
     * @brief Vecteur3D Definition du vecteur a partir de ses composantes
     * @param[in] xval composante x du vecteur
     * @param[in] yval composante y du vecteur
     * @param[in] zval composante z du vecteur
     */
    Vecteur3D(const double xval,
              const double yval,
              const double zval);


    /*
     * Methodes publiques
     */
    /**
     * @brief Angle Angle entre 2 vecteurs
     * @param[in] vecteur vecteur
     * @return angle
     */
    double Angle(const Vecteur3D &vecteur) const;

    /**
     * @brief Normalise Determination du vecteur unitaire
     * @return vecteur unitaire
     */
    Vecteur3D Normalise() const;

    /**
     * @brief Norme Norme du vecteur
     * @return Norme
     */
    double Norme() const;

    /**
     * @brief Rotation Rotation d'un vecteur autour d'un axe
     * @param[in] axe axe (AXE_X, AXE_Y ou AXE_Z)
     * @param[in] angle angle de rotation (rad)
     * @return vecteur issu de la rotation
     */
    Vecteur3D Rotation(const AxeType &axe,
                       const double angle) const;

    /**
     * @brief operator = Affectation d'un vecteur 3D
     * @param[in] vecteur vecteur
     * @return vecteur affecte
     */
    Vecteur3D &operator = (const Vecteur3D &vecteur) = default;

    /**
     * @brief operator - Oppose du vecteur 3D
     * @param[in] vecteur vecteur
     * @return Oppose du vecteur 3D
     */
    friend inline Vecteur3D operator - (const Vecteur3D &vecteur);

    /**
     * @brief operator + Somme de deux vecteurs 3D
     * @param[in] vecteur1 vecteur1
     * @param[in] vecteur2 vecteur2
     * @return Somme de deux vecteurs 3D
     */
    friend inline Vecteur3D operator + (const Vecteur3D &vecteur1,
                                        const Vecteur3D &vecteur2);

    /**
     * @brief operator - Difference de deux vecteurs 3D
     * @param[in] vecteur1 vecteur1
     * @param[in] vecteur2 vecteur2
     * @return Difference de deux vecteurs 3D
     */
    friend inline Vecteur3D operator - (const Vecteur3D &vecteur1,
                                        const Vecteur3D &vecteur2);

    /**
     * @brief operator * Produit d'un vecteur 3D par un scalaire
     * @param[in] vecteur1 vecteur
     * @param[in] scalaire grandeur scalaire
     * @return Produit d'un vecteur 3D par un scalaire
     */
    friend inline Vecteur3D operator * (const Vecteur3D &vecteur1,
                                        const double scalaire);

    /**
     * @brief operator *= Produit d'un vecteur avec un scalaire
     * @param[in/out] vecteur vecteur
     * @param[in] scalaire scalaire
     */
    friend inline void operator *= (Vecteur3D &vecteur,
                                    const double scalaire);

    /**
     * @brief operator * Produit scalaire de deux vecteurs 3D
     * @param[in] vecteur1 vecteur1
     * @param[in] vecteur2 vecteur2
     * @return Produit scalaire de deux vecteurs 3D
     */
    friend inline double operator * (const Vecteur3D &vecteur1,
                                     const Vecteur3D &vecteur2);

    /**
     * @brief operator ^ Produit vectoriel de deux vecteurs 3D
     * @param[in] vecteur1 vecteur1
     * @param[in] vecteur2 vecteur2
     * @return Produit vectoriel de deux vecteurs 3D
     */
    friend inline Vecteur3D operator ^ (const Vecteur3D &vecteur1,
                                        const Vecteur3D &vecteur2);


    /*
     * Accesseurs
     */
    double x() const;
    double y() const;
    double z() const;


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */
    // Composantes du vecteur
    double _x;
    double _y;
    double _z;

    /*
     * Methodes privees
     */


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
inline Vecteur3D operator + (const Vecteur3D &vecteur1,
                             const Vecteur3D &vecteur2)
{
    return (Vecteur3D(vecteur1._x + vecteur2._x, vecteur1._y + vecteur2._y, vecteur1._z + vecteur2._z));
}

/*
 * Difference de 2 vecteurs 3D
 */
inline Vecteur3D operator - (const Vecteur3D &vecteur1,
                             const Vecteur3D &vecteur2)
{
    return (Vecteur3D(vecteur1._x - vecteur2._x, vecteur1._y - vecteur2._y, vecteur1._z - vecteur2._z));
}

/*
 * Multiplication d'un vecteur 3D par un scalaire
 */
inline Vecteur3D operator * (const Vecteur3D &vecteur1,
                             const double scalaire)
{
    return (Vecteur3D(vecteur1._x * scalaire, vecteur1._y * scalaire, vecteur1._z * scalaire));
}

/*
 * Produit d'un vecteur par un scalaire
 */
inline void operator *= (Vecteur3D &vecteur,
                         const double scalaire)
{
    vecteur._x *= scalaire;
    vecteur._y *= scalaire;
    vecteur._z *= scalaire;
}

/*
 * Produit scalaire de 2 vecteurs 3D
 */
inline double operator * (const Vecteur3D &vecteur1,
                          const Vecteur3D &vecteur2)
{
    return (vecteur1._x * vecteur2._x + vecteur1._y * vecteur2._y + vecteur1._z * vecteur2._z);
}

/*
 * Produit vectoriel de 2 vecteurs 3D
 */
inline Vecteur3D operator ^ (const Vecteur3D &vecteur1,
                             const Vecteur3D &vecteur2)
{
    return (Vecteur3D(vecteur1._y * vecteur2._z - vecteur1._z * vecteur2._y,
                      vecteur1._z * vecteur2._x - vecteur1._x * vecteur2._z,
                      vecteur1._x * vecteur2._y - vecteur1._y * vecteur2._x));
}


#endif // VECTEUR3D_H
