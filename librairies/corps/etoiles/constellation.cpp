/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >    Corps
 *
 * Description
 * >    Definition d'une constellation
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 mars 2012
 *
 * Date de revision
 * >    10 octobre 2015
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "constellation.h"

bool Constellation::initCst = false;

/*
 * Constructeurs
 */
Constellation::Constellation()
{
}

Constellation::Constellation(const QString &nomConst, const double ascDroite, const double decl)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nom = nomConst;
    _ascensionDroite = ascDroite * HEUR2RAD;
    _declinaison = decl * DEG2RAD;

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Constellation::~Constellation()
{
}

/*
 * Calcul des positions des noms des constellations pour la carte du ciel
 */
void Constellation::CalculConstellations(const Observateur &observateur, QList<Constellation> &constellations)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!initCst) {
        InitTabCst(constellations);
        initCst = true;
    }

    /* Corps de la methode */
    for (int i=0; i<constellations.size(); i++)
        constellations[i].CalculCoordHoriz2(observateur);

    /* Retour */
    return;
}

/*
 * Lecture des noms et positions des constellations
 */
void Constellation::InitTabCst(QList<Constellation> &constellations)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirCommonData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#elif defined (Q_OS_LINUX)
    const QString dirCommonData = "/usr/share" + QDir::separator() + dirAstr;
#else
    const QString dirCommonData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

    /* Corps de la methode */
    const QString fic = dirCommonData + QDir::separator() + "stars" + QDir::separator() + "constlabel.dat";
    QFile fi(fic);
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    while (!flux.atEnd()) {
        const QString ligne = flux.readLine();
        const double asc = ligne.mid(0, 6).toDouble();
        const double dec = ligne.mid(7, 7).toDouble();
        constellations.append(Constellation(ligne.mid(15, 3), asc, dec));
    }
    fi.close();

    /* Retour */
    return;
}

/* Accesseurs */
QString Constellation::nom() const
{
    return _nom;
}
