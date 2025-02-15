/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    constellation.cpp
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 mars 2012
 *
 * Date de revision
 * >    3 aout 2024
 *
 */

#include <QDir>
#include "constellation.h"
#include "librairies/exceptions/exception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Definition a partir des composantes
 */
Constellation::Constellation(const QString &nomConst,
                             const double ascDroite,
                             const double decl) :
    _nom(nomConst)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ascensionDroite = ascDroite * MATHS::HEUR2RAD;
    _declinaison = decl * MATHS::DEG2RAD;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des positions des noms des constellations pour la carte du ciel
 */
void Constellation::CalculConstellations(const Observateur &observateur,
                                         QList<Constellation> &constellations)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (constellations.isEmpty()) {
        throw Exception(QObject::tr("Le tableau de constellations n'est pas initialisé"), MessageType::WARNING);
    }

    for (int i=0; i<constellations.size(); i++) {
        constellations[i].CalculCoordHoriz(observateur);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de constellations
 */
QList<Constellation> Constellation::Initialisation(const QString &dirCommonData)
{
    /* Declarations des variables locales */
    QList<Constellation> constellations;

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "constlabel.dat";
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

        const QStringList ligne = it.next().split(" ", Qt::SkipEmptyParts);

        const double ad = ligne.first().toDouble();
        const double de = ligne.at(1).toDouble();
        constellations.append(Constellation(ligne.at(2), ad, de));
    }

    qInfo() << "Lecture fichier constlabel.dat OK";

    /* Retour */
    return constellations;
}


/*
 * Accesseurs
 */
QString Constellation::nom() const
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

