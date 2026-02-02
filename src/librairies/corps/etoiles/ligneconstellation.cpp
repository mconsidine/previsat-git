/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    ligneconstellation.cpp
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
#include "librairies/exceptions/exception.h"
#include "ligneconstellation.h"


QList<QPoint> LigneConstellation::_tabLigCst;

/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
LigneConstellation::LigneConstellation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _dessin = false;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des lignes de constellations
 */
void LigneConstellation::CalculLignesCst(const QList<Etoile> &etoiles, QList<LigneConstellation> &lignesCst)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_tabLigCst.isEmpty()) {
        throw Exception(QObject::tr("Le tableau de lignes de constellation n'est pas initialisé"), MessageType::WARNING);
    }

    if (etoiles.isEmpty()) {
        throw Exception(QObject::tr("Le tableau d'étoiles n'est pas initialisé"), MessageType::WARNING);
    }

    lignesCst.clear();
    for (int i=0; i<_tabLigCst.size(); i++) {
        const int ind1 = _tabLigCst.at(i).x() - 1;
        const int ind2 = _tabLigCst.at(i).y() - 1;
        lignesCst.append(LigneConstellation(etoiles.at(ind1), etoiles.at(ind2)));
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier contenant les lignes de constellations
 */
void LigneConstellation::Initialisation(const QString &dirCommonData)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString ficLig = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "constlines.dat";
    QFile fi(ficLig);
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
        _tabLigCst.append(QPoint(ligne.first().toInt(), ligne.last().toInt()));
    }

    qInfo() << "Lecture fichier constlines.dat OK";

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
bool LigneConstellation::isDessin() const
{
    return _dessin;
}

Etoile LigneConstellation::etoile1() const
{
    return _etoile1;
}

Etoile LigneConstellation::etoile2() const
{
    return _etoile2;
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

