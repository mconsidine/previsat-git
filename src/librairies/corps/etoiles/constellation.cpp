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
 * >    30 decembre 2018
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "constellation.h"
#include"librairies/exceptions/previsatexception.h"


QList<Constellation> Constellation::_constellations;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Constellation::Constellation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */

    /* Retour */
    return;
}

/*
 * Definition a partir des composantes
 */
Constellation::Constellation(const QString &nomConst, const double ascDroite, const double decl) :
    _nom(nomConst)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ascensionDroite = ascDroite * HEUR2RAD;
    _declinaison = decl * DEG2RAD;

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
QString Constellation::nom() const
{
    return _nom;
}

QList<Constellation> &Constellation::constellations()
{
    return _constellations;
}


/*
 * Methodes publiques
 */
void Constellation::CalculConstellations(const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        if (_constellations.isEmpty()) {
            throw PreviSatException(QObject::tr("Le tableau de constellations n'est pas initialisé"), WARNING);
        }

        for (int i=0; i<_constellations.size(); i++) {
            _constellations[i].CalculCoordHoriz2(observateur);
        }
    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de constellations
 */
void Constellation::Initialisation(const QString &dirCommonData)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "constlabel.dat";
    QFile fi(fic);
    if (fi.exists() && fi.size() != 0) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();
            const double asc = ligne.mid(0, 6).toDouble();
            const double dec = ligne.mid(7, 7).toDouble();
            _constellations.append(Constellation(ligne.mid(15, 3), asc, dec));
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



