/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    etoile.cpp
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Heritage
 * >
 *
 * Description
 * >    Definition d'une etoile
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

#include <QCoreApplication>
#include <QDir>
#include <stdio.h>
#include "etoile.h"
#include "librairies/maths/mathConstants.h"

bool Etoile::_initStar = false;

Etoile::Etoile()
{
    _magnitude = 0.;
}

Etoile::Etoile(const QString nom, const double ascensionDroite, const double declinaison, const double magnitude)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nom = nom;
    _ascensionDroite = ascensionDroite * HEUR2RAD;
    _declinaison = declinaison * DEG2RAD;
    _magnitude = magnitude;

    /* Retour */
    return;
}

Etoile::~Etoile()
{
}

void Etoile::CalculPositionEtoiles(const Observateur &observateur, QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_initStar) {
        InitTabEtoiles(etoiles);
        _initStar = true;
    }

    /* Corps de la methode */
    for (int i=0; i<etoiles.size(); i++)
        etoiles[i].CalculCoordHoriz2(observateur);

    /* Retour */
    return;
}

void Etoile::InitTabEtoiles(QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    etoiles.clear();
    FILE *fstr = NULL;
    const QString fic = QCoreApplication::applicationDirPath() + QDir::separator() + "data" + QDir::separator() +
            "etoiles.str";
    if ((fstr = fopen(fic.toStdString().c_str(), "r")) != NULL) {

        char ligne1[4096];
        std::string ligne2;

        while (fgets(ligne1, 4096, fstr) != NULL) {

            int ad1, ad2, de1, de2, de3;
            double ad3;

            QString nom = "";
            double ascDte = 0.;
            double dec = 0.;
            double mag = 99.;
            ligne2.assign(ligne1);

            if (ligne2.length() > 34) {
                sscanf(ligne1, "%2d%2d%4lf%*1s%2d%2d%2d%*7f%*7f%6lf", &ad1, &ad2, &ad3, &de1, &de2, &de3, &mag);

                ascDte = ad1 + ad2 * DEG_PAR_ARCMIN + ad3 * DEG_PAR_ARCSEC;
                const int sgn = (ligne2.at(9) == '-') ? -1 : 1;
                dec = sgn * (de1 + de2 * DEG_PAR_ARCMIN + de3 * DEG_PAR_ARCSEC);

                if (ligne2.length() > 37)
                    nom = QString::fromStdString(ligne2.substr(37, ligne2.length()));
            }

            etoiles.append(Etoile(nom, ascDte, dec, mag));
        }
    }
    fclose(fstr);

    /* Retour */
    return;
}

/* Accesseurs */
double Etoile::getMagnitude() const
{
    return (_magnitude);
}

QString Etoile::getNom() const
{
    return (_nom);
}
