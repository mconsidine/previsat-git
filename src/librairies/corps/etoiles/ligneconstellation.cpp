/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >
 *
 * Description
 * >    Definition d'une ligne de constellation
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    5 mars 2016
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"
#include "ligneconstellation.h"


QList<LigneConstellation> LigneConstellation::_lignesCst;
QList<QPair<int, int> > LigneConstellation::_tabLigCst;

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

QList<LigneConstellation> &LigneConstellation::lignesCst()
{
    return _lignesCst;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des lignes de constellations
 */
void LigneConstellation::CalculLignesCst(const QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        if (_tabLigCst.isEmpty()) {
            throw PreviSatException(QObject::tr("Le tableau de lignes de constellation n'est pas initialisé"), WARNING);
        }

        if (etoiles.isEmpty()) {
            throw PreviSatException(QObject::tr("Le tableau d'étoiles n'est pas initialisé"), WARNING);
        }

        _lignesCst.clear();
        for (int i=0; i<_tabLigCst.size(); i++) {
            const int ind1 = _tabLigCst.at(i).first - 1;
            const int ind2 = _tabLigCst.at(i).second - 1;
            _lignesCst.append(LigneConstellation(etoiles.at(ind1), etoiles.at(ind2)));
        }

    } catch (PreviSatException &e) {
        throw PreviSatException();
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
    QFile fichier(ficLig);
    if (fichier.exists() && (fichier.size() != 0)) {

        fichier.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichier);

        _tabLigCst.clear();
        while (!flux.atEnd()) {

            const QStringList ligne = flux.readLine().split(" ");
            QPair<int, int> lig(ligne.at(0).toInt(), ligne.at(1).toInt());
            _tabLigCst.append(lig);
        }
    }
    fichier.close();

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
