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
 * >    25 fevrier 2023
 *
 */

#include <QDir>
#include "etoile.h"
#include "librairies/exceptions/exception.h"
#include "librairies/maths/maths.h"


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
Etoile::Etoile(const QString &nomEtoile,
               const double ascDroite,
               const double decl,
               const double mag) :
    _nom(nomEtoile)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ascensionDroite = ascDroite * MATHS::HEUR2RAD;
    _declinaison = decl * MATHS::DEG2RAD;
    _magnitude = mag;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul de la position des etoiles
 */
void Etoile::CalculPositionEtoiles(const Observateur &observateur,
                                   QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (etoiles.isEmpty()) {
        throw Exception(QObject::tr("Le tableau d'étoiles n'est pas initialisé"), MessageType::WARNING);
    }

    /* Corps de la methode */
    for (int i=0; i<etoiles.size(); i++) {
        etoiles[i].CalculCoordHoriz3(observateur);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier d'etoiles
 */
QList<Etoile> Etoile::Initialisation(const QString &dirCommonData)
{
    /* Declarations des variables locales */
    QList<Etoile> etoiles;

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "etoiles.dat";
    QFile fi(fic);
    if (!fi.exists() || (fi.size() == 0)) {
        const QFileInfo ff(fi.fileName());
        throw Exception(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME),
                        MessageType::ERREUR);
    }

    if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QFileInfo ff(fi.fileName());
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName()), MessageType::ERREUR);
    }

    const QStringList contenu = QString(fi.readAll()).split("\n", Qt::SkipEmptyParts);
    fi.close();

    QStringListIterator it(contenu);
    while (it.hasNext()) {

        const QString ligne = it.next();

        const int ad1 = ligne.mid(0, 2).toInt();
        const int ad2 = ligne.mid(2, 2).toInt();
        const double ad3 = ligne.mid(4, 4).toDouble();
        const double ad = ad1 + ad2 * MATHS::DEG_PAR_ARCMIN + ad3 * MATHS::DEG_PAR_ARCSEC;

        const int sgnd = (ligne.at(9) == '-') ? -1 : 1;
        const int de1 = ligne.mid(10, 2).toInt();
        const int de2 = ligne.mid(12, 2).toInt();
        const int de3 = ligne.mid(14, 2).toInt();
        const double dec = sgnd * (de1 + de2 * MATHS::DEG_PAR_ARCMIN + de3 * MATHS::DEG_PAR_ARCSEC);

        const double mag = ligne.mid(31, 5).toDouble();
        const QString nomEtoile = (ligne.length() > 37) ? ligne.mid(37, ligne.length()) : "";

        etoiles.append(Etoile(nomEtoile, ad, dec, mag));
    }

    qInfo() << "Lecture fichier etoiles.dat OK";

    /* Retour */
    return etoiles;
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

