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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "configuration.h"
#include "evenementsstation.h"
#include "evenementsstationspatiale.h"
#include "librairies/exceptions/previsatexception.h"


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

    /* Corps de la methode */
    try {
        QFile fi(Configuration::instance()->dirLocalData() + QDir::separator() +
                 Configuration::instance()->nomFichierEvenementsStationSpatiale());

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

            cfg.readNextStartElement();
            if (cfg.name().toString().toLower() == "ndm") {

                QString version;

                while (cfg.readNextStartElement()) {

                    if (cfg.name().toString().toLower() == "oem") {

                        version = cfg.attributes().value("version").toString();

                        while (cfg.readNextStartElement()) {

                            if (cfg.name().toString().toLower() == "body") {

                                LectureBody(cfg, evenements);

                            } else {
                                cfg.skipCurrentElement();
                            }
                        }
                    }
                }
            }

            fi.close();

            if (evenements.dateDebutEvenementsStationSpatiale.isEmpty()
                    || evenements.dateFinEvenementsStationSpatiale.isEmpty()
                    || (evenements.masseStationSpatiale < 0.)
                    || (evenements.surfaceTraineeAtmospherique < 0.)
                    || (evenements.coefficientTraineeAtmospherique < 0.)) {

                const QFileInfo ff(fi.fileName());
                qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME);
                throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                        .arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
            }

            qInfo() << QString("Lecture fichier %1 OK").arg(Configuration::instance()->nomFichierEvenementsStationSpatiale());
        }

    } catch (PreviSatException &e) {
        throw PreviSatException();
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
/*
 * Lecture de la section body du fichier Station Spatiale
 */
void EvenementsStationSpatiale::LectureBody(QXmlStreamReader &cfg, EvenementsStation &evenements)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        if (cfg.name().toString().toLower() == "segment") {

            while (cfg.readNextStartElement()) {

                if (cfg.name().toString().toLower() == "metadata") {

                    LectureMetadata(cfg, evenements);

                } else if (cfg.name().toString().toLower() == "data") {

                    LectureData(cfg, evenements);
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section data du fichier Station Spatiale
 */
void EvenementsStationSpatiale::LectureData(QXmlStreamReader &cfg, EvenementsStation &evenements)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString value;

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        value = cfg.readElementText();

        if (value.toLower().contains("mass")) {

            // Masse (en kg)
            evenements.masseStationSpatiale = value.split("=").last().toDouble();

        } else if (value.toLower().contains("drag_area")) {

            // Surface de trainee atmospherique
            evenements.surfaceTraineeAtmospherique = value.split("=").last().toDouble();

        } else if (value.toLower().contains("drag_coeff")) {

            // Coefficient de trainee atmospherique
            evenements.coefficientTraineeAtmospherique = value.split("=").last().toDouble();

        } else if (value.contains("===")) {

            // Recuperation des evenements
            value = "";
            while (cfg.readNextStartElement() && !value.contains("===")) {

                value = cfg.readElementText();
                if (!value.contains("===") && !value.isEmpty() && !value.contains("(")) {
                    value.replace(26, 1, "T");
                    evenements.evenementsStationSpatiale.append(value);
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section metadata du fichier Station Spatiale
 */
void EvenementsStationSpatiale::LectureMetadata(QXmlStreamReader &cfg, EvenementsStation &evenements)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        if (cfg.name().toString().toLower() == "start_time") {

            // Date de debut
            evenements.dateDebutEvenementsStationSpatiale = cfg.readElementText();

        } else if (cfg.name().toString().toLower() == "stop_time") {

            // Date de fin
            evenements.dateFinEvenementsStationSpatiale = cfg.readElementText();

        } else {
            cfg.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}
