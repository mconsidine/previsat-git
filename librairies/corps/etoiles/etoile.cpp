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
#include <QTextStream>
#include "etoile.h"
#include "librairies/maths/mathConstants.h"

bool Etoile::_initStar = false;

/*
 * Constructeurs
 */
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

/*
 * Destructeur
 */
Etoile::~Etoile()
{
}

/* Calcul de la position des etoiles */
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

/*
 * Lecture du catalogue d'etoiles
 */
void Etoile::InitTabEtoiles(QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    etoiles.clear();
    const QString fic = QCoreApplication::applicationDirPath() + QDir::separator() + "data" + QDir::separator() + "etoiles.str";
    QFile fi(fic);
    if (fi.exists()) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();
            double ascDte = 0.;
            double dec = 0.;
            double mag = 99.;
            QString nom = "";

            if (ligne.length() > 34) {

                const int ad1 = ligne.mid(0, 2).toInt();
                const int ad2 = ligne.mid(2, 2).toInt();
                const double ad3 = ligne.mid(4, 4).toDouble();
                ascDte = ad1 + ad2 * DEG_PAR_ARCMIN + ad3 * DEG_PAR_ARCSEC;

                const int sgn = (ligne.at(9) == '-') ? -1 : 1;
                const int de1 = ligne.mid(10, 2).toInt();
                const int de2 = ligne.mid(12, 2).toInt();
                const int de3 = ligne.mid(14, 2).toInt();
                dec = sgn * (de1 + de2 * DEG_PAR_ARCMIN + de3 * DEG_PAR_ARCSEC);

                mag = ligne.mid(31, 5).toDouble();
                nom = (ligne.length() > 37) ? ligne.mid(37, ligne.length()) : "";
            }
            etoiles.append(Etoile(nom, ascDte, dec, mag));
        }
    }
    fi.close();

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
