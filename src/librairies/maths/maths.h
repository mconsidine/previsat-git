/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * Description
 * >    Utilitaires mathematiques
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    29 octobre 2023
 *
 */

#ifndef MATHS_H
#define MATHS_H

#include <cmath>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QPointF>
#pragma GCC diagnostic ignored "-Wconversion"
#include "mathsconst.h"


class QString;

class Maths
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculExtremumInterpolation3 Calcul d'un extremum par interpolation a l'ordre 3,
     * issu de l'Astronomical Algorithms 2nd edition, de Jean Meeus, pp23-25
     * @param[in] xtab Tableau des abscisses
     * @param[in] ytab Tableau des ordonnees
     * @return Coordonnees de l'extremum
     */
    static QPointF CalculExtremumInterpolation3(const std::array<double, MATHS::DEGRE_INTERPOLATION> &xtab,
                                                const std::array<double, MATHS::DEGRE_INTERPOLATION> &ytab);

    /**
     * @brief CalculValeurXInterpolation3 Calcul d'une valeur x pour une valeur y donnee, par interpolation a l'ordre 3,
     * issu de l'Astronomical Algorithms 2nd edition, de Jean Meeus, pp23-27
     * @param[in] xtab Tableau des abscisses
     * @param[in] ytab Tableau des ordonnees
     * @param[in] yval Valeur de l'ordonnee
     * @param[in] Epsilon Seuil de comparaison
     * @return valeur x correspondante
     */
    static double CalculValeurXInterpolation3(const std::array<double, MATHS::DEGRE_INTERPOLATION> &xtab,
                                              const std::array<double, MATHS::DEGRE_INTERPOLATION> &ytab,
                                              const double yval,
                                              const double epsilon);

    /**
     * @brief InterpolationLagrange Interpolation par polynome de Lagrange
     * @param[in] table tableau des coordonnees
     * @param[in] xval valeur de l'abscisse
     * @return valeur de l'ordonnee interpolee
     */
    static double InterpolationLagrange(const QVector<QPointF> &table, const double xval);

    /**
     * @brief ToSexagesimal Conversion d'un angle sous forme decimale en chaine de caracteres formattee
     * @param[in] xdec Valeur de l'angle
     * @param[in] typeAngle Type de l'angle (DEGRE, HEURE1, HEURE2, RADIAN, ARCSEC, NO_TYPE)
     * @param[in] nbDeg Nombre de chiffres pour les degres
     * @param[in] nbDecimales Nombre de decimales a afficher
     * @param[in] signe Affichage du signe +
     * @param[in] espace Affichage d'un espace entre les composantes de l'angle
     * @return chaine de caracteres contenant la valeur angulaire
     */
    static QString ToSexagesimal(const double xdec, const AngleFormatType &typeAngle, const int nbDeg, const int nbDecimales, const bool signe,
                                 const bool espace);


    /*
     * Accesseurs
     */


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

    /*
     * Methodes privees
     */


};


/**
 * @brief sgn signe d'un reel
 * @param[in] x reel
 * @return signe du reel
 */
static inline int sgn(const double x)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return ((x > 0.) - (x < 0.));
}

/**
 * @brief arrondi Arrondi d'un nombre reel avec un nombre de decimales
 * @param[in] x nombre reel
 * @param[in] nbDec nombre de decimales
 * @return Arrondi
 */
static inline double arrondi(const double x, const unsigned int nbDec)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double y = fabs(x);
    const double p = pow(10., nbDec);

    /* Retour */
    return (sgn(x) * floor(p * y + 0.5) / p);
}

/**
 * @brief modulo Calcul du modulo
 * @param[in] x valeur a moduler
 * @param[in] y valeur modulante
 * @return valeur modulee
 */
static inline double modulo(const double x, const double y)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double z = fmod(x, y);

    /* Retour */
    return ((z < 0.) ? z + y : z);
}

#endif // MATHS_H
