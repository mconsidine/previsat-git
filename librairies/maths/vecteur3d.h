/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
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
    double Angle(const Vecteur3D vecteur);
    Vecteur3D Normalise();

    Vecteur3D operator - ();
    Vecteur3D operator + (const Vecteur3D &vecteur);
    Vecteur3D operator - (const Vecteur3D &vecteur);
    Vecteur3D operator * (const double scalaire);
    double operator * (const Vecteur3D &vecteur);
    Vecteur3D operator ^ (const Vecteur3D &vecteur);

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

#endif // VECTEUR3D_H
