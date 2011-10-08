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
 * >
 *
 */

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include "ligneconstellation.h"

const int LigneConstellation::TABMAX;
bool LigneConstellation::_initLig = false;
int LigneConstellation::_tabLigCst[TABMAX][2];
QList<LigneConstellation> LigneConstellation::_lignesCst;

LigneConstellation::LigneConstellation()
{
}

LigneConstellation::LigneConstellation(const Etoile &etoile1, const Etoile &etoile2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _etoile1 = etoile1;
    _etoile2 = etoile2;
    _dessin = _etoile1.isVisible() && _etoile2.isVisible();

    /* Retour */
    return;
}

LigneConstellation::~LigneConstellation()
{
}

void LigneConstellation::CalculLignesCst(const QList<Etoile> etoiles)
{
    /* Declarations des variables locales */
    int ind1, ind2;

    /* Initialisations */
    if (!_initLig) {
        InitTabLignesCst();
        _initLig = true;
    }

    /* Corps de la methode */
    _lignesCst.clear();
    for (int i=0; i<TABMAX; i++) {
        ind1 = _tabLigCst[i][0] - 1;
        ind2 = _tabLigCst[i][1] - 1;
        _lignesCst[i] = LigneConstellation(etoiles[ind1], etoiles[ind2]);
    }

    /* Retour */
    return;
}

void LigneConstellation::InitTabLignesCst()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    int i = 0;
    QFile fichier("data/constlines.cst");
    if (fichier.exists()) {

        fichier.open(QIODevice::ReadOnly);
        QTextStream flux(&fichier);

        while (!flux.atEnd()) {

            QStringList ligne = flux.readLine().split(" ");
            _tabLigCst[i][0] = ligne[0].toInt();
            _tabLigCst[i][1] = ligne[1].toInt();
            i++;
        }
    }
    fichier.close();

    /* Retour */
    return;
}

QList<LigneConstellation> LigneConstellation::getLignesCst()
{
    return _lignesCst;
}
