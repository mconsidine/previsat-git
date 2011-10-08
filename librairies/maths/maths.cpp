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
 * >    maths.cpp
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
 * >
 *
 */

#include <QObject>
#include <cmath>
#include "maths.h"

/* Methodes */
/*
 * Arrondi avec n decimales
 */
double Maths::arrondi(const double x, const int n)
{
    /* Declarations des variables locales */
    double res;

    /* Initialisations */

    /* Corps de la methode */
    if (n < 0) {
        res = x;
    } else {

        const double y = fabs(x);
        const double p = pow(10., n);
        res = sgn(x) * floor(p * y + 0.5) / p;
    }

    /* Retour */
    return (res);
}

/*
 * Signe d'un nombre reel
 */
int Maths::sgn(const double x)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return ((x > 0.) ? 1 : (x < 0.) ? -1 : 0);
}

/*
 * Calcul d'un extremum par interpolation a l'ordre 3,
 * issu de l'Astronomical Algorithms 2nd edition, de Jean Meeus, pp23-25
 */
void Maths::CalculExtremumInterpolation3(const double xtab[], const double ytab[], double extremum[])
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double a = ytab[1] - ytab[0];
    const double b = ytab[2] - ytab[1];
    const double ci = (a + b) / (b - a);

    /* Corps de la methode */
    extremum[0] = xtab[1] - 0.5 * (xtab[1] - xtab[0]) * ci;
    extremum[1] = ytab[1] - 0.125 * (a + b) * ci;

    /* Retour */
    return;
}

/*
 * Calcul d'une valeur x pour une valeur y donnee, par interpolation a l'ordre 3,
 * issu de l'Astronomical Algorithms 2nd edition, de Jean Meeus, pp23-27
 */
double Maths::CalculValeurXInterpolation3(const double xtab[], const double ytab[], const double yval, const double epsilon)
{
    /* Declarations des variables locales */
    int iter;
    double dn0, n0;
    double yy[3];

    /* Initialisations */
    iter = 0;
    dn0 = 100000.;
    n0 = 0.;
    for (int i=0; i<3; i++)
        yy[i] = ytab[i] - yval;

    const double a = yy[1] - yy[0];
    const double b = yy[2] - yy[1];
    const double c = b - a;

    /* Corps de la methode */
    const double dy = 2. * yy[1];
    while (fabs(dn0) >= epsilon && iter < 10000) {
        double tmp1, tmp2;

        tmp1 = c * n0;
        tmp2 = a + b + tmp1;
        dn0 = -(dy + n0 * tmp2) / (tmp1 + tmp2);
        n0 += dn0;
        iter++;
    }

    /* Retour */
    return (xtab[1] + n0 * (xtab[1] - xtab[0]));
}

/*
 * Conversion d'un angle sous forme decimale en chaine de caracteres formattee
 */
QString Maths::ToSexagesimal(const double xdec, const AngleFormatType typeAngle, const int nbDeg, const int nbDecimales, const bool signe, const bool espace)
{
    /* Declarations des variables locales */
    bool tst1, tst2;
    int dec, deg, degr, min;
    double sec, xval, y;
    QString esp, res;

    /* Initialisations */
    switch (typeAngle) {
    case DEGRE:
        xval = xdec * RAD2DEG;
        break;

    case HEURE1:
    case HEURE2:
        xval = xdec * RAD2HEUR;
        break;

    case ARCSEC:
    case RADIAN:
    case NO_TYPE:
    default:
        xval = xdec;
    }
    y = fabs(xval) + EPSDBL100;

    degr = nbDeg;
    if (degr < 0)
        degr = 0;
    if (degr > 2 && (typeAngle == HEURE1 || typeAngle == HEURE2))
        degr = 2;

    dec = nbDecimales;
    if (dec < 0)
        dec = 0;

    tst1 = typeAngle == DEGRE || typeAngle == NO_TYPE;
    tst2 = typeAngle == HEURE1;
    esp = (espace && typeAngle != HEURE2) ? " " : "";

    /* Corps de la methode */
    deg = (int) y;
    min = (int) (ARCMIN_PAR_DEG * (y - deg));
    sec = ARCSEC_PAR_DEG * (y - deg) - ARCMIN_PAR_DEG * min;
    if (fabs(Maths::arrondi(sec, dec) - ARCSEC_PAR_MIN) < EPSDBL100) {
        sec = 0.;
        min++;
    }
    if (min == (int) (ARCMIN_PAR_DEG + EPSDBL100)) {
        min = 0;
        deg++;
    }

    // Signe
    res = res.append((Maths::sgn(xval) >= 0) ? ((signe && tst1) ? "+" : " ") : "-");

    // Degres (ou heures)
    res = res.append("%1");
    res = res.arg(deg, degr, 10, QChar('0'));
    res = res.append((tst1) ? "°" : (tst2) ? QObject::tr("h") : ":");
    res = res.append(esp);

    // Minutes
    res = res.append("%1");
    res = res.arg(min, 2, 10, QChar('0'));
    res = res.append((tst1) ? "\'" : (tst2) ? QObject::tr("m") : ":");
    res = res.append(esp);

    // Secondes
    res = res.append("%1");
    res = res.arg(sec, (dec == 0) ? 2 : dec + 3, 'f', dec, QChar('0'));
    res = res.append((tst1) ? "\"" : (tst2) ? QObject::tr("s") : "");

    /* Retour */
    return (res);
}
