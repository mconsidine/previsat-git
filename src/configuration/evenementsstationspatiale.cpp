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
 * >    evenementsstationspatiale.cpp
 *
 * Localisation
 * >    configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    19 juin 2022
 *
 * Date de revision
 * >    18 aout 2024
 *
 */

#include <QDir>
#include <QFile>
#include "configuration.h"
#include "evenementsstation.h"
#include "evenementsstationspatiale.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"
#include "librairies/systeme/telechargement.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Methodes publiques
 */
/*
 * Lecture du fichier NASA contenant les evenements de la Station Spatiale
 */
EvenementsStation EvenementsStationSpatiale::LectureEvenementsStationSpatiale()
{
    /* Declarations des variables locales */
    EvenementsStation evenements;

    /* Initialisations */
    evenements.masseStationSpatiale = -1.;
    evenements.coefficientTraineeAtmospherique = -1.;
    evenements.surfaceTraineeAtmospherique = -1.;
    FichierXml fi(Configuration::instance()->dirLocalData() + QDir::separator() + Configuration::instance()->nomFichierEvenementsStationSpatiale());

    if (!fi.exists()) {

        // Telechargement du fichier
        Telechargement tel(Configuration::instance()->dirLocalData());
        tel.TelechargementFichier(QUrl(QString("%1data/%2").arg(DOMAIN_NAME).arg(Configuration::instance()->nomFichierEvenementsStationSpatiale())));
    }

    /* Corps de la methode */
    try {

        QString val;
        const QDomDocument document = fi.Ouverture(false);

        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "ndm") {
            throw Exception();
        }

        const QDomNode metadata = root.elementsByTagName("metadata").at(0);
        evenements.dateDebutEvenementsStationSpatiale = metadata.firstChildElement("START_TIME").text();
        evenements.dateFinEvenementsStationSpatiale = metadata.firstChildElement("STOP_TIME").text();

        const QDomNode data = root.elementsByTagName("data").at(0);
        const QDomNodeList valeurs = data.toElement().elementsByTagName("COMMENT");

        int i = 0;
        while (i < valeurs.count()) {

            const QString valeur = valeurs.at(i).toElement().text();

            if (valeur.toLower().contains("mass")) {

                // Masse (en kg)
                evenements.masseStationSpatiale = valeur.split("=").last().toDouble();

            } else if (valeur.toLower().contains("drag_area")) {

                // Surface de trainee atmospherique
                evenements.surfaceTraineeAtmospherique = valeur.split("=").last().toDouble();

            } else if (valeur.toLower().contains("drag_coeff")) {

                // Coefficient de trainee atmospherique
                evenements.coefficientTraineeAtmospherique = valeur.split("=").last().toDouble();

            } else if (valeur.contains("===")) {

                // Recuperation des evenements
                val = "";
                while (!val.contains("===")) {

                    val = valeurs.at(++i).toElement().text();
                    if (!val.contains("===") && !val.isEmpty() && !val.contains("(")) {
                        val.replace(26, 1, "T");
                        evenements.evenementsStationSpatiale.append(val);
                    }
                }
            }

            i++;
        }

       qInfo().noquote() << QString("Lecture fichier %1 OK").arg(fi.nomfic());

    } catch (Exception const &e) {
        qCritical().noquote() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(fi.nomfic()).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(fi.nomfic()).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return evenements;
}


/*
 * Accesseurs
 */

/*
 * Modificateurs
 */


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
