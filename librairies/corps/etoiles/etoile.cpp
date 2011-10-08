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

#include <stdio.h>
#include "etoile.h"
#include "librairies/maths/mathConstants.h"

static const int TABMAX = 9110;
bool Etoile::_initStar = false;
QList<Etoile> Etoile::_etoiles;

Etoile::Etoile()
{
}

Etoile::Etoile(const QString nom, const double ascensionDroite, const double declinaison, const double magnitude)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nom = nom;
    _ascensionDroite = ascensionDroite;
    _declinaison = declinaison;
    _magnitude = magnitude;

    /* Retour */
    return;
}

Etoile::~Etoile()
{
}

void Etoile::CalculPositionEtoiles(const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_initStar) {
        InitTabEtoiles();
        _initStar = true;
    }

    /* Corps de la methode */
    for (int i=0; i<TABMAX; i++)
        _etoiles[i].CalculCoordHoriz(observateur);

    /* Retour */
    return;
}

void Etoile::CalculCoordHoriz(const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _visible = false;
    _hauteur = -PI;
    const double cd = cos(_declinaison);
    Vecteur3D vec1 = Vecteur3D(cos(_ascensionDroite) * cd, sin(_ascensionDroite) * cd, sin(_declinaison));
    Vecteur3D vec2 = observateur.getRotHz() * vec1;

    /* Corps de la methode */
    // Hauteur
    const double ht = asin(vec2.getZ());

    if (ht > -DEG2RAD) {

        // Prise en compte de la refraction atmospherique
        const double htd = ht * RAD2DEG;
        const double refraction = DEG2RAD * 1.02 / (ARCMIN_PAR_DEG * tan(DEG2RAD * (htd + 10.3 / (htd + 5.11))));

        _hauteur = ht + refraction;
        if (_hauteur >= 0.) {
            // Azimut
            _azimut = atan2(vec2.getY(), vec2.getX());
            if (_azimut < 0.)
                _azimut += DEUX_PI;
            _visible = true;
        } else {
            _visible = false;
            _hauteur = ht;
        }
    }

    /* Retour */
    return;
}

void Etoile::InitTabEtoiles()
{
    /* Declarations des variables locales */
    int ad1, ad2, de1, de2, de3, i, sgn;
    double ad3, ascDte, dec, mag;
    char ligne1[4096];
    std::string ligne2;
    QString nom;

    /* Initialisations */
    i = 0;

    /* Corps de la methode */
    FILE *fstr = NULL;
    if ((fstr = fopen("data/etoiles.str", "r")) != NULL) {

        while (fgets(ligne1, 4096, fstr) != NULL) {

            nom = "";
            ascDte = 0.;
            dec = 0.;
            mag = 99.;
            ligne2.assign(ligne1);

            if (ligne2.length() > 34) {
                sscanf(ligne1, "%2d%2d%4lf%*2s%2d%2d%2d%*7f%*7f%6lf", &ad1, &ad2, &ad3, &de1, &de2, &de3, &mag);

                ascDte = ad1 + ad2 * DEG_PAR_ARCMIN + ad3 * DEG_PAR_ARCSEC;
                sgn = (ligne2.at(9) == '-') ? -1 : 1;
                dec = sgn * (de1 + de2 * DEG_PAR_ARCMIN + de3 * DEG_PAR_ARCSEC);

                if (ligne2.length() > 37)
                    nom = QString::fromStdString(ligne2.substr(37, ligne2.length()));
            }

            _etoiles[i] = Etoile(nom, ascDte, dec, mag);
            i++;
        }
    }
    fclose(fstr);

    /* Retour */
    return;
}

QList<Etoile> Etoile::getEtoiles()
{
    return _etoiles;
}
