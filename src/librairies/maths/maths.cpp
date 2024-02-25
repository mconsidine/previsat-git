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
 * >    25 fevrier 2023
 *
 */

#include <QObject>
#include <QString>
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
QPointF Maths::CalculExtremumInterpolation3(const std::array<QPointF, MATHS::DEGRE_INTERPOLATION> &table)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double a = table[1].y() - table[0].y();
    const double b = table[2].y() - table[1].y();
    const double ci = (a + b) / (b - a);

    /* Corps de la methode */

    /* Retour */
    return QPointF(table[1].x() - 0.5 * (table[1].x() - table[0].x()) * ci, table[1].y() - 0.125 * (a + b) * ci);
}

/*
 * Calcul d'une valeur x pour une valeur y donnee, par interpolation a l'ordre 3
 */
double Maths::CalculValeurXInterpolation3(const std::array<QPointF, MATHS::DEGRE_INTERPOLATION> &table,
                                          const double yval,
                                          const double epsilon)
{
    /* Declarations des variables locales */
    std::array<double, MATHS::DEGRE_INTERPOLATION> yy;

    /* Initialisations */
    unsigned int iter = 0;
    double dn0 = MATHS::ITERATIONS_MAX;
    double n0 = 0.;

    for (unsigned int i=0; i<MATHS::DEGRE_INTERPOLATION; i++) {
        yy[i] = table[i].y() - yval;
    }

    const double a = yy[1] - yy[0];
    const double b = yy[2] - yy[1];
    const double c = b - a;

    /* Corps de la methode */
    const double dy = 2. * yy[1];
    while ((fabs(dn0) >= epsilon) && (iter < MATHS::ITERATIONS_MAX)) {

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
    return (table[1].x() + n0 * (table[1].x() - table[0].x()));
}

/*
 * Interpolation par polynome de Lagrange
 */
double Maths::InterpolationLagrange(const QVector<QPointF> &table,
                                    const double xval)
{
    /* Declarations des variables locales */
    double c;

    /* Initialisations */
    double res = 0.;

    /* Corps de la methode */
    for(unsigned int i=0; i<table.size(); i++) {

        c = 1.;
        for(unsigned int j=0; j<table.size(); j++) {

            if (j != i) {
                c *= ((xval - table[j].x()) / (table[i].x() - table[j].x()));
            }
        }

        res += (c * table[i].y());
    }

    /* Retour */
    return res;
}

/*
 * Conversion d'un angle sous forme decimale en chaine de caracteres formattee
 */
QString Maths::ToSexagesimal(const double xdec,
                             const AngleFormatType &typeAngle,
                             const int nbDeg,
                             const int nbDecimales,
                             const bool signe,
                             const bool espace)
{
    /* Declarations des variables locales */
    double xval;

    /* Initialisations */
    switch (typeAngle) {
    case AngleFormatType::DEGRE:
        xval = xdec * MATHS::RAD2DEG;
        break;

    case AngleFormatType::HEURE1:
    case AngleFormatType::HEURE2:
        xval = xdec * MATHS::RAD2HEUR;
        break;

    case AngleFormatType::ARCSEC:
    case AngleFormatType::RADIAN:
    case AngleFormatType::NO_TYPE:
    default:
        xval = xdec;
    }

    double y = fabs(xval) + MATHS::EPSDBL100;

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
    const QString unite2 = (tst1) ? R"(')" : (tst2) ? QObject::tr("m", "minute (angle)") : ":";
    const QString unite3 = (tst1) ? R"(")" : (tst2) ? QObject::tr("s", "second (angle)") : "";

    /* Corps de la methode */
    int deg = static_cast<int> (y);
    int min = static_cast<int> (MATHS::ARCMIN_PAR_DEG * (y - deg));
    double sec = MATHS::ARCSEC_PAR_DEG * (y - deg) - MATHS::ARCMIN_PAR_DEG * min;

    if (fabs(arrondi(sec, dec) - MATHS::ARCSEC_PAR_MIN) < MATHS::EPSDBL100) {
        sec = 0.;
        min++;
    }

    if (min == static_cast<int>(MATHS::ARCMIN_PAR_DEG + MATHS::EPSDBL100)) {
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

