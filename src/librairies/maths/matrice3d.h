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
 * >    25 fevrier 2023
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
     * @param[in] valeurInit valeur d'initialisation
     */
    explicit Matrice3D(const double valeurInit = 0.) :
        _vecteur1(valeurInit),
        _vecteur2(valeurInit),
        _vecteur3(valeurInit) {
    }

    /**
     * @brief Matrice3D Definition a partir d'une matrice
     * @param[in] matrice matrice
     */
    Matrice3D(const Matrice3D &matrice) = default;

    /**
     * @brief Matrice3D Definition d'une matrice de rotation
     * @param[in] axe axe de rotation
     * @param[in] ang angle de rotation
     */
    Matrice3D(const AxeType &axe, const double ang);

    /**
     * @brief Matrice3D Definition a partir des vecteurs colonne
     * @param[in] vec1 vecteur 1
     * @param[in] vec2 vecteur 2
     * @param[in] vec3 vecteur 3
     */
    Matrice3D(const Vecteur3D &vec1, const Vecteur3D &vec2, const Vecteur3D &vec3) :
        _vecteur1(vec1),
        _vecteur2(vec2),
        _vecteur3(vec3) {
    }


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
     * @param[in] matrice
     * @return matrice affectee
     */
    Matrice3D &operator = (const Matrice3D &matrice) = default;

    /**
     * @brief operator * produit d'une matrice 3x3 par un vecteur 3D
     * @param[in] matrice matrice 3x3
     * @param[in] vecteur vecteur 3D
     * @return vecteur 3D resultat
     */
    friend inline Vecteur3D operator * (const Matrice3D &matrice, const Vecteur3D &vecteur);

    /**
     * @brief operator * produit de 2 matrices 3x3
     * @param[in] matrice1 matrice 1
     * @param[in] matrice2 matrice 2
     * @return matrice 3D resultat
     */
    friend inline Matrice3D operator * (const Matrice3D &matrice1, const Matrice3D &matrice2);


    /*
     * Accesseurs
     */
    const Vecteur3D &vecteur1() const;
    const Vecteur3D &vecteur2() const;
    const Vecteur3D &vecteur3() const;


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
