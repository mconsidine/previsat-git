/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    testtools.h
 *
 * Localisation
 * >    test
 *
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires de tests
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    18 juin 2019
 *
 * Date de revision
 * >
 *
 */

#ifndef TESTTOOLS_H
#define TESTTOOLS_H

#include "librairies/dates/date.h"
#include "librairies/maths/matrice3d.h"
#include "librairies/maths/vecteur3d.h"


namespace TestTools {

    void CompareDates(const Date &date1, const Date &date2);
    void CompareMatrices3D(const Matrice3D &mat1, const Matrice3D &mat2);
    void CompareVecteurs3D(const Vecteur3D &vec1, const Vecteur3D &vec2);
    void CompareFichiers(const QString &ficRes, const QString &ficRef);

}

#endif // TESTTOOLS_H
