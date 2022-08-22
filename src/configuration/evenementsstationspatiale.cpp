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
 * >    22 aout 2022
 *
 */

#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QDir>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QFile>
#include <QXmlStreamReader>
#include "configuration.h"
#include "evenementsstationspatiale.h"
#include "librairies/exceptions/previsatexception.h"


QString EvenementsStationSpatiale::_dateDebutStationSpatiale;
QString EvenementsStationSpatiale::_dateFinStationSpatiale;
double EvenementsStationSpatiale::_masseStationSpatiale;
double EvenementsStationSpatiale::_surfaceTraineeAtmospherique;
double EvenementsStationSpatiale::_coefficientTraineeAtmospherique;
QStringList EvenementsStationSpatiale::_evenementsStationSpatiale;


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
void EvenementsStationSpatiale::LectureEvenementsStationSpatiale(QString &dateDebutEvenementsStationSpatiale,
                                                                 QString &dateFinEvenementsStationSpatiale,
                                                                 double &masseStationSpatiale,
                                                                 double &surfaceTraineeAtmospherique,
                                                                 double &coefficientTraineeAtmospherique,
                                                                 QStringList &evenementsStationSpatiale)
{
    try {

        /* Declarations des variables locales */

        /* Initialisations */
        _evenementsStationSpatiale.clear();

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirLocalData() + QDir::separator() + Configuration::instance()->nomFichierEvenementsStationSpatiale());

        if (fi.exists() && (fi.size() != 0)) {

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

                                    LectureBody(cfg);

                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }
                        }
                    }
                }
            }
            fi.close();

        } else {
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas").arg(Configuration::instance()->nomFichierEvenementsStationSpatiale()),
                                    MessageType::WARNING);
        }

        dateDebutEvenementsStationSpatiale = _dateDebutStationSpatiale;
        dateFinEvenementsStationSpatiale = _dateFinStationSpatiale;
        masseStationSpatiale = _masseStationSpatiale;
        surfaceTraineeAtmospherique = _surfaceTraineeAtmospherique;
        coefficientTraineeAtmospherique = _coefficientTraineeAtmospherique;
        evenementsStationSpatiale = _evenementsStationSpatiale;

        qInfo() << QString("Lecture fichier %1 OK").arg(Configuration::instance()->nomFichierEvenementsStationSpatiale());

    } catch (PreviSatException &e) {
        qCritical() << QString("Lecture fichier %1 KO").arg(Configuration::instance()->nomFichierEvenementsStationSpatiale());
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                .arg(Configuration::instance()->nomFichierEvenementsStationSpatiale()).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return;
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
void EvenementsStationSpatiale::LectureBody(QXmlStreamReader &cfg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        if (cfg.name().toString().toLower() == "segment") {

            while (cfg.readNextStartElement()) {

                if (cfg.name().toString().toLower() == "metadata") {

                    LectureMetadata(cfg);

                } else if (cfg.name().toString().toLower() == "data") {

                    LectureData(cfg);

                } else {
                    cfg.skipCurrentElement();
                }
            }
        } else {
            cfg.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section data du fichier Station Spatiale
 */
void EvenementsStationSpatiale::LectureData(QXmlStreamReader &cfg)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString value;

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        value = cfg.readElementText();

        if (value.toLower().contains("mass")) {

            // Masse (en kg)
            _masseStationSpatiale = value.split("=").last().toDouble();

        } else if (value.toLower().contains("drag_area")) {

            // Surface de trainee atmospherique
            _surfaceTraineeAtmospherique = value.split("=").last().toDouble();

        } else if (value.toLower().contains("drag_coeff")) {

            // Coefficient de trainee atmospherique
            _coefficientTraineeAtmospherique = value.split("=").last().toDouble();

        } else if (value.contains("===")) {

            // Recuperation des evenements
            value = "";
            while (cfg.readNextStartElement() && !value.contains("===")) {

                value = cfg.readElementText();
                if (!value.contains("===") && !value.isEmpty() && !value.contains("(")) {
                    value.replace(26, 1, "T");
                    _evenementsStationSpatiale.append(value);
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
void EvenementsStationSpatiale::LectureMetadata(QXmlStreamReader &cfg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        if (cfg.name().toString().toLower() == "start_time") {

            // Date de debut
            _dateDebutStationSpatiale = cfg.readElementText();

        } else if (cfg.name().toString().toLower() == "stop_time") {

            // Date de fin
            _dateFinStationSpatiale = cfg.readElementText();

        } else {
            cfg.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}
