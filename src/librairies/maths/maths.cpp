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
 * >    maths.cpp
 *
 * Localisation
 * >    librairies.maths
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    17 octobre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QObject>
#include <QString>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "maths.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Methodes publiques
 */
/*
 * Calcul d'un extremum par interpolation a l'ordre 3
 */
QPair<double, double> Maths::CalculExtremumInterpolation3(const std::array<double, DEGRE_INTERPOLATION> &xtab,
                                                          const std::array<double, DEGRE_INTERPOLATION> &ytab)
{
    /* Declarations des variables locales */
    QPair<double, double> res;

    /* Initialisations */
    const double a = ytab[1] - ytab[0];
    const double b = ytab[2] - ytab[1];
    const double ci = (a + b) / (b - a);

    /* Corps de la methode */
    res.first = xtab[1] - 0.5 * (xtab[1] - xtab[0]) * ci;
    res.second = ytab[1] - 0.125 * (a + b) * ci;

    /* Retour */
    return res;
}

/*
 * Calcul d'une valeur x pour une valeur y donnee, par interpolation a l'ordre 3
 */
double Maths::CalculValeurXInterpolation3(const std::array<double, DEGRE_INTERPOLATION> &xtab,
                                          const std::array<double, DEGRE_INTERPOLATION> &ytab,
                                          const double yval,
                                          const double epsilon)
{
    /* Declarations des variables locales */
    std::array<double, DEGRE_INTERPOLATION> yy;

    /* Initialisations */
    unsigned int iter = 0;
    double dn0 = 100000.;
    double n0 = 0.;

    for (unsigned int i=0; i<DEGRE_INTERPOLATION; i++) {
        yy[i] = ytab[i] - yval;
    }

    const double a = yy[1] - yy[0];
    const double b = yy[2] - yy[1];
    const double c = b - a;

    /* Corps de la methode */
    const double dy = 2. * yy[1];
    while ((fabs(dn0) >= epsilon) && (iter < ITERATIONS_MAX)) {

        const double tmp1 = c * n0;
        const double tmp2 = a + b + tmp1;
        dn0 = -(dy + n0 * tmp2) / (tmp1 + tmp2);

        if (fabs(dn0) > 1.) {
            dn0 = sgn(dn0);
        }

        n0 += dn0;
        iter++;
    }

    /* Retour */
    return (xtab[1] + n0 * (xtab[1] - xtab[0]));
}

/*
 * Conversion d'un angle sous forme decimale en chaine de caracteres formattee
 */
QString Maths::ToSexagesimal(const double xdec, const AngleFormatType &typeAngle, const int nbDeg, const int nbDecimales,
                             const bool signe, const bool espace)
{
    /* Declarations des variables locales */
    double xval;

    /* Initialisations */
    switch (typeAngle) {
    case AngleFormatType::DEGRE:
        xval = xdec * RAD2DEG;
        break;

    case AngleFormatType::HEURE1:
    case AngleFormatType::HEURE2:
        xval = xdec * RAD2HEUR;
        break;

    case AngleFormatType::ARCSEC:
    case AngleFormatType::RADIAN:
    case AngleFormatType::NO_TYPE:
    default:
        xval = xdec;
    }

    double y = fabs(xval) + EPSDBL100;

    int degr = nbDeg;
    if (degr < 0) {
        degr = 0;
    }
    if ((degr > 2) && ((typeAngle == AngleFormatType::HEURE1) || (typeAngle == AngleFormatType::HEURE2))) {
        degr = 2;
    }

    int dec = nbDecimales;
    if (dec < 0) {
        dec = 0;
    }

    const bool tst1 = ((typeAngle == AngleFormatType::DEGRE) || (typeAngle == AngleFormatType::NO_TYPE));
    const bool tst2 = (typeAngle == AngleFormatType::HEURE1);
    const QChar chr = QChar('0');

    const QString esp = (espace && (typeAngle != AngleFormatType::HEURE2)) ? " " : "";
    const QString signe0 = (sgn(xval) >= 0) ? ((signe && tst1) ? "+" : " ") : "-";

    const QString unite1 = (tst1) ? "°" : (tst2) ? QObject::tr("h", "hour (angle)") : ":";
    const QString unite2 = (tst1) ? "\'" : (tst2) ? QObject::tr("m", "minute (angle)") : ":";
    const QString unite3 = (tst1) ? "\"" : (tst2) ? QObject::tr("s", "second (angle)") : "";

    /* Corps de la methode */
    int deg = static_cast<int> (y);
    int min = static_cast<int> (ARCMIN_PAR_DEG * (y - deg));
    double sec = ARCSEC_PAR_DEG * (y - deg) - ARCMIN_PAR_DEG * min;

    if (fabs(arrondi(sec, dec) - ARCSEC_PAR_MIN) < EPSDBL100) {
        sec = 0.;
        min++;
    }

    if (min == static_cast<int>(ARCMIN_PAR_DEG + EPSDBL100)) {
        min = 0;
        deg++;
    }

    const QString fmt = "%1%2%3%4%5%6%7%8%9";

    /* Retour */
    return (fmt.arg(signe0).arg(deg, degr, 10, chr).arg(unite1).arg(esp).arg(min, 2, 10, chr).arg(unite2).arg(esp)
            .arg(sec, (dec == 0) ? 2 : dec + 3, 'f', dec, chr).arg(unite3));
}


/*
 * Accesseurs
 */


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */

