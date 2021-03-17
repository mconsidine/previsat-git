/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    30 decembre 2018
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "etoile.h"
#include "librairies/exceptions/previsatexception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Etoile::Etoile()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _magnitude = 0.;

    /* Retour */
    return;
}

/*
 * Definition a partir de ses composantes
 */
Etoile::Etoile(const QString &nomEtoile, const double ascDroite, const double decl, const double mag) :
    _nom(nomEtoile)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ascensionDroite = ascDroite * HEUR2RAD;
    _declinaison = decl * DEG2RAD;
    _magnitude = mag;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
double Etoile::magnitude() const
{
    return _magnitude;
}

QString Etoile::nom() const
{
    return _nom;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la position des etoiles
 */
void Etoile::CalculPositionEtoiles(const Observateur &observateur, QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */
    try {
        if (etoiles.isEmpty()) {
            throw PreviSatException(QObject::tr("Le tableau d'étoiles n'est pas initialisé"), WARNING);
        }
    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Corps de la methode */
    for (int i=0; i<etoiles.size(); i++) {
        etoiles[i].CalculCoordHoriz2(observateur);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier d'etoiles
 */
void Etoile::Initialisation(const QString &dirCommonData, QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "etoiles.dat";
    QFile fi(fic);
    if (fi.exists() && (fi.size() != 0)) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        double ascDte;
        double dec;
        double mag;
        QString nom;

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();
            ascDte = 0.;
            dec = 0.;
            mag = 99.;
            nom = "";

            if (ligne.length() > 34) {

                const int ad1 = ligne.mid(0, 2).toInt();
                const int ad2 = ligne.mid(2, 2).toInt();
                const double ad3 = ligne.mid(4, 4).toDouble();
                ascDte = ad1 + ad2 * DEG_PAR_ARCMIN + ad3 * DEG_PAR_ARCSEC;

                const int sgnd = (ligne.at(9) == '-') ? -1 : 1;
                const int de1 = ligne.mid(10, 2).toInt();
                const int de2 = ligne.mid(12, 2).toInt();
                const int de3 = ligne.mid(14, 2).toInt();
                dec = sgnd * (de1 + de2 * DEG_PAR_ARCMIN + de3 * DEG_PAR_ARCSEC);

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



