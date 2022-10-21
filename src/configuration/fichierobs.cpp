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
 * >    fichierobs.cpp
 *
 * Localisation
 * >    configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    21 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QDir>
#include <QXmlStreamReader>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "configuration.h"
#include "fichierobs.h"
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
 * Ecriture du fichier de lieu d'observation
 */
void FichierObs::Ecriture(const QString &ficObsXml)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + ficObsXml);

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatObservateurs");

        // Observateurs
        QMapIterator it(Configuration::instance()->mapObs());
        while (it.hasNext()) {
            it.next();

            const Observateur obs = it.value();
            cfg.writeStartElement("Observateur");
            cfg.writeTextElement("Nom", obs.nomlieu());

            cfg.writeTextElement("Longitude", QString::number(obs.longitude() * RAD2DEG, 'f', 9));
            cfg.writeTextElement("Latitude", QString::number(obs.latitude() * RAD2DEG, 'f', 9));
            cfg.writeTextElement("Altitude", QString::number(obs.altitude() * 1000.));
            cfg.writeEndElement();
        }

        cfg.writeEndElement();
        cfg.writeEndDocument();
        fi.close();
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de lieu d'observation
 */
QMap<QString, Observateur> FichierObs::Lecture(const QString &ficObsXml, const bool alarme)
{
    /* Declarations des variables locales */
    QMap<QString, Observateur> mapObs;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(Configuration::instance()->dirCoord() + QDir::separator() + ficObsXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name().toString() == "PreviSatObservateurs") {

                QString nom;
                double lon;
                double lat;
                double alt;

                while (cfg.readNextStartElement()) {

                    if (cfg.name().toString() == "Observateur") {

                        nom = "";
                        lon = 0.;
                        lat = 0.;
                        alt = 0.;

                        while (cfg.readNextStartElement()) {

                            if (cfg.name().toString() == "Nom") {
                                nom = cfg.readElementText();
                            } else if (cfg.name().toString() == "Longitude") {
                                lon = cfg.readElementText().toDouble();
                            } else if (cfg.name().toString() == "Latitude") {
                                lat = cfg.readElementText().toDouble();
                            } else if (cfg.name().toString() == "Altitude") {
                                alt = cfg.readElementText().toDouble();
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        if (!nom.isEmpty()) {
                            mapObs.insert(nom, Observateur(nom, lon, lat, alt));
                        }
                    } else {
                        cfg.skipCurrentElement();
                    }
                }
            } else {

                fi1.close();

                qWarning() << "Le fichier ne contient pas de lieux d'observation";

                if (alarme) {
                    throw PreviSatException(QObject::tr("Le fichier ne contient pas de lieux d'observation"), MessageType::WARNING);
                } else {
                    throw PreviSatException();
                }
            }
        }
        fi1.close();
    }

    // Verifications


    /* Retour */
    return mapObs;
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

