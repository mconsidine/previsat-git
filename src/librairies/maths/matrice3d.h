/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    matrice3d.h
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
 * >    17 septembre 2022
 *
 */

#ifndef MATRICE3D_H
#define MATRICE3D_H

#include "vecteur3d.h"


class Matrice3D
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Matrice3D Constructeur par defaut
     * @param valeurInit valeur d'initialisation
     */
    explicit Matrice3D(const double valeurInit = 0.) :
        _vecteur1(valeurInit),
        _vecteur2(valeurInit),
        _vecteur3(valeurInit) {
    }

    /**
     * @brief Matrice3D Definition a partir d'une matrice
     * @param matrice matrice
     */
    Matrice3D(const Matrice3D &matrice) :
        _vecteur1(matrice._vecteur1),
        _vecteur2(matrice._vecteur2),
        _vecteur3(matrice._vecteur3) {
    }

    /**
     * @brief Matrice3D Definition d'une matrice de rotation
     * @param axe axe de rotation
     * @param ang angle de rotation
     */
    Matrice3D(const AxeType &axe, const double ang);

    /**
     * @brief Matrice3D Definition a partir des vecteurs colonne
     * @param vec1 vecteur 1
     * @param vec2 vecteur 2
     * @param vec3 vecteur 3
     */
    Matrice3D(const Vecteur3D &vec1, const Vecteur3D &vec2, const Vecteur3D &vec3) :
        _vecteur1(vec1),
        _vecteur2(vec2),
        _vecteur3(vec3) {
    }

    /*
     * Accesseurs
     */
    Vecteur3D vecteur1() const;
    Vecteur3D vecteur2() const;
    Vecteur3D vecteur3() const;

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Transposee Calcul de la matrice 3x3 transposee
     * @return transposee de la matrice
     */
    Matrice3D Transposee() const;

    /**
     * @brief operator = Affectation d'une matrice 3D
     * @param matrice
     * @return matrice affectee
     */
    Matrice3D &operator = (const Matrice3D &matrice);

    /**
     * @brief operator * produit d'une matrice 3x3 par un vecteur 3D
     * @param matrice matrice 3x3
     * @param vecteur 3D
     * @return vecteur 3D resultat
     */
    friend inline Vecteur3D operator * (const Matrice3D &matrice, const Vecteur3D &vecteur);

    /**
     * @brief operator * produit de 2 matrices 3x3
     * @param matrice1 matrice 1
     * @param matrice2 matrice 2
     * @return matrice 3D resultat
     */
    friend inline Matrice3D operator * (const Matrice3D &matrice1, const Matrice3D &matrice2);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    // Vecteurs colonnes
    Vecteur3D _vecteur1;
    Vecteur3D _vecteur2;
    Vecteur3D _vecteur3;

    /*
     * Methodes privees
     */


};


/*
 * Produit d'une Matrice3D avec un vecteur 3D
 */
inline Vecteur3D operator * (const Matrice3D &matrice, const Vecteur3D &vecteur)
{
    const double x = matrice._vecteur1.x() * vecteur.x() + matrice._vecteur2.x() * vecteur.y() + matrice._vecteur3.x() * vecteur.z();
    const double y = matrice._vecteur1.y() * vecteur.x() + matrice._vecteur2.y() * vecteur.y() + matrice._vecteur3.y() * vecteur.z();
    const double z = matrice._vecteur1.z() * vecteur.x() + matrice._vecteur2.z() * vecteur.y() + matrice._vecteur3.z() * vecteur.z();

    return (Vecteur3D(x, y, z));
}

/*
 * Produit de 2 matrices
 */
inline Matrice3D operator * (const Matrice3D &matrice1, const Matrice3D &matrice2)
{
    return (Matrice3D(matrice1 * matrice2._vecteur1, matrice1 * matrice2._vecteur2, matrice1 * matrice2._vecteur3));
}


#endif // MATRICE3D_H
