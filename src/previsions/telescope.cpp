/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    telescope.cpp
 *
 * Localisation
 * >    previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 octobre 2020
 *
 * Date de revision
 * >    25 fevrier 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include <cmath>
#include "configuration/configuration.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "telescope.h"


static ConditionsPrevisions _conditions;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Accesseurs
 */

/*
 * Modificateurs
 */
void Telescope::setConditions(const ConditionsPrevisions &conditions)
{
    _conditions = conditions;
}


/*
 * Methodes publiques
 */
/*
 * Calcul des coordonnees du satellite pour le suivi avec un telescope
 */
int Telescope::CalculSuiviTelescope(int &nombre)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(_conditions.ficRes);

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        if (fi.isWritable()) {

            QMap<QString, ElementsOrbitaux> tabElem;
            const QString fmt = "%1,%2,%3,%4";
            double jjmsec = floor(_conditions.jj1 * DATE::NB_MILLISEC_PAR_JOUR + _conditions.pas);
            Date date(jjmsec * DATE::NB_JOUR_PAR_MILLISEC, 0.);

            // Creation de la liste d'elements orbitaux
            const QFileInfo ff(_conditions.fichier);
            if (ff.suffix() == "xml") {
                tabElem = GPFormat::LectureFichier(_conditions.fichier, Configuration::instance()->donneesSatellites(),
                                                   Configuration::instance()->lgRec(), _conditions.listeSatellites);
            } else {
                tabElem = TLE::LectureFichier(_conditions.fichier, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec(),
                                              _conditions.listeSatellites);
            }

            // Satellite
            Satellite sat(tabElem.first());

            const QString entete("\"Time (UTCG)\",\"Range (km)\",\"Right Ascen (deg)\",\"Declination (deg)\"");

            QTextStream flux(&fi);
            flux << entete << Qt::endl;

            int i = 0;
            while (i < _conditions.nbIter) {

                // Position de l'observateur
                _conditions.observateur.CalculPosVit(date);

                // Position du satellite
                sat.CalculPosVit(date);

                // Position topocentrique du satellite
                sat.CalculCoordHoriz(_conditions.observateur, true, false);
                const double ht = sat.CalculRefractionAtmospherique(sat.hauteur());

                if (ht >= _conditions.hauteur) {

                    // Ascension droite, declinaison
                    sat.CalculCoordEquat(_conditions.observateur, false);

                    const QString ephem = fmt.arg(date.ToShortDateAMJmillisec()).arg(sat.distance(), 16, 'f', 6).
                            arg(sat.ascensionDroite() * MATHS::RAD2DEG, 16, 'f', 6).arg(sat.declinaison() * MATHS::RAD2DEG, 16, 'f', 6);

                    flux << ephem << Qt::endl;
                }

                jjmsec += _conditions.pas;
                date = Date(jjmsec * DATE::NB_JOUR_PAR_MILLISEC, 0.);
                i++;
            }
        }
    }
    fi.close();

    /* Retour */
    return nombre;
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

