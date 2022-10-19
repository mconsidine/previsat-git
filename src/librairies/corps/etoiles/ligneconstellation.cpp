/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    15 octobre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"
#include "ligneconstellation.h"


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
    try {
        if (_tabLigCst.isEmpty()) {
            throw PreviSatException(QT_TRANSLATE_NOOP("LigneConstellation", "Le tableau de lignes de constellation n'est pas initialisé"),
                                    MessageType::WARNING);
        }

        if (etoiles.isEmpty()) {
            throw PreviSatException(QT_TRANSLATE_NOOP("LigneConstellation", "Le tableau d'étoiles n'est pas initialisé"), MessageType::WARNING);
        }

        lignesCst.clear();
        for (int i=0; i<_tabLigCst.size(); i++) {
            const int ind1 = _tabLigCst.at(i).first - 1;
            const int ind2 = _tabLigCst.at(i).second - 1;
            lignesCst.append(LigneConstellation(etoiles.at(ind1), etoiles.at(ind2)));
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

        if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream flux(&fichier);

            _tabLigCst.clear();
            while (!flux.atEnd()) {

                const QStringList ligne = flux.readLine().split(" ");
                const QPair<int, int> lig(ligne.first().toInt(), ligne.at(1).toInt());
                _tabLigCst.append(lig);
            }
        }
        fichier.close();
    }

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

const Etoile &LigneConstellation::etoile1() const
{
    return _etoile1;
}

const Etoile &LigneConstellation::etoile2() const
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

