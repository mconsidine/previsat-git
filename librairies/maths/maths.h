/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    3 juin 2015
 *
 */

#ifndef MATHS_H
#define MATHS_H

#include <QString>
#include "mathsConstants.h"

class Maths
{
public:

    /* Constructeurs */

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculExtremumInterpolation3 Calcul d'un extremum par interpolation a l'ordre 3,
     * issu de l'Astronomical Algorithms 2nd edition, de Jean Meeus, pp23-25
     * @param xtab Tableau des abscisses
     * @param ytab Tableau des ordonnees
     * @param extremum Coordonnees de l'extremum
     */
    static void CalculExtremumInterpolation3(const double xtab[], const double ytab[], double extremum[]);

    /**
     * @brief CalculValeurXInterpolation3 Calcul d'une valeur x pour une valeur y donnee, par interpolation a l'ordre 3,
     * issu de l'Astronomical Algorithms 2nd edition, de Jean Meeus, pp23-27
     * @param xtab Tableau des abscisses
     * @param ytab Tableau des ordonnees
     * @param yval Valeur de l'ordonnee
     * @param Epsilon Epsilon de comparaison
     * @return valeur x correspondante
     */
    static double CalculValeurXInterpolation3(const double xtab[], const double ytab[], const double yval, const double epsilon);

    /**
     * @brief ToSexagesimal Conversion d'un angle sous forme decimale en chaine de caracteres formattee
     * @param xdec Valeur de l'angle
     * @param typeAngle Type de l'angle (DEGRE, HEURE1, HEURE2, RADIAN, ARCSEC, NO_TYPE)
     * @param nbDeg Nombre de chiffres pour les degres
     * @param nbDecimales Nombre de decimales a afficher
     * @param signe Affichage du signe +
     * @param espace Affichage d'un espace entre les composantes de l'angle
     * @return chaine de caracteres contenant la valeur angulaire
     */
    static QString ToSexagesimal(const double xdec, const AngleFormatType typeAngle, const int nbDeg, const int nbDecimales,
                                 const bool signe, const bool espace);

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


/**
 * @brief arrondi Arrondi d'un nombre reel avec un nombre de decimales
 * @param x nombre reel
 * @param nbDec nombre de decimales
 * @return Arrondi
 */
static inline double arrondi(const double x, const int nbDec);

/**
 * @brief modulo Calcul du modulo
 * @param x valeur a moduler
 * @param y valeur modulante
 * @return valeur modulee
 */
static inline double modulo(const double x, const double y);

/**
 * @brief sgn signe d'un reel
 * @param x reel
 * @return signe du reel
 */
static inline int sgn(const double x);


static inline double arrondi(const double x, const int nbDec)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double y = fabs(x);
    const double p = pow(10., nbDec);

    /* Retour */
    return (sgn(x) * floor(p * y + 0.5) / p);
}

static inline double modulo(const double x, const double y)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    double z = fmod(x, y);
    if (z < 0.)
        z += y;

    /* Retour */
    return (z);
}

static inline int sgn(const double x)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return ((x > 0.) ? 1 : (x < 0.) ? -1 : 0);
}

#endif // MATHS_H
