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
 * >    maths.h
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

#ifndef MATHS_H
#define MATHS_H

#include <QString>
#include "mathConstants.h"

class Maths
{
public:

    /* Constructeurs */

    /* Enumerations */
    enum AngleFormatType { DEGRE, HEURE1, HEURE2, RADIAN, ARCSEC, NO_TYPE };

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    static double arrondi(const double x, const int n);

    inline static double modulo(const double x, const double y)
    {
        /* Declarations des variables locales */
        double z;

        /* Initialisations */
        z = x;

        /* Corps de la methode */
        z = fmod(x, y);
        if (z < 0.)
            z += y;

        /* Retour */
        return (z);
    }

    static int sgn(const double x);
    static void CalculExtremumInterpolation3(const double xtab[], const double ytab[], double extremum[]);
    static double CalculValeurXInterpolation3(const double xtab[], const double ytab[], const double yval,
                                              const double epsilon);
    static QString ToSexagesimal(const double xdec, const AngleFormatType typeAngle, const int nbDeg,
                                 const int nbDecimales, const bool signe, const bool espace);

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */

};

#endif // MATHS_H
