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
 * >    gestionnairexml.cpp
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
#include <QDomDocument>
#include <QFile>
#include <QXmlStreamReader>
#include "categorieelementsorbitaux.h"
#include "configuration.h"
#include "gestionnairexml.h"
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
 * Ecriture de la configuration
 */
void GestionnaireXml::EcritureConfiguration()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + "configuration.xml");

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatConfiguration");
        cfg.writeAttribute("version", Configuration::instance()->versionCfg());

        // Numero NORAD de la station spatiale
        cfg.writeTextElement("NoradStationSpatiale", Configuration::instance()->noradStationSpatiale());

        // Adresse du site Celestrak
        cfg.writeTextElement("AdresseCelestrak", Configuration::instance()->adresseCelestrak());

        // Nom du fichier d'evenements de la Station Spatiale
        cfg.writeTextElement("NomFichierEvenementsStationSpatiale", Configuration::instance()->nomFichierEvenementsStationSpatiale());

        // Observateurs
        cfg.writeStartElement("Observateurs");
        QListIterator itObs(Configuration::instance()->observateurs());
        while (itObs.hasNext()) {

            const Observateur obs = itObs.next();
            cfg.writeStartElement("Observateur");
            cfg.writeTextElement("Nom", obs.nomlieu());

            cfg.writeTextElement("Longitude", QString::number(obs.longitude() * RAD2DEG, 'f', 9));
            cfg.writeTextElement("Latitude", QString::number(obs.latitude() * RAD2DEG, 'f', 9));
            cfg.writeTextElement("Altitude", QString::number(obs.altitude() * 1000.));
            cfg.writeEndElement();
        }
        cfg.writeEndElement();

        // Listes de satellites selon le fichier d'elements orbitaux
        cfg.writeStartElement("FichiersElem");

        // Numeros NORAD du fichier par defaut
        const QStringList listeNorad = Configuration::instance()->mapSatellitesFichierElem()[Configuration::instance()->nomfic()];
        if (!listeNorad.isEmpty()) {

            cfg.writeStartElement("Fichier");
            cfg.writeAttribute("nom", Configuration::instance()->nomfic());

            QStringListIterator itNorad(listeNorad);
            while (itNorad.hasNext()) {
                cfg.writeTextElement("Norad", itNorad.next());
            }
            cfg.writeEndElement();
        }

        QMapIterator itElem(Configuration::instance()->mapSatellitesFichierElem());
        while (itElem.hasNext()) {

            itElem.next();

            if (!itElem.value().isEmpty() && (itElem.key() != Configuration::instance()->nomfic())) {

                cfg.writeStartElement("Fichier");
                cfg.writeAttribute("nom", itElem.key());

                QStringListIterator itNorad(itElem.value());
                while (itNorad.hasNext()) {
                    cfg.writeTextElement("Norad", itNorad.next());
                }
                cfg.writeEndElement();
            }
        }
        cfg.writeEndElement();

        cfg.writeEndElement();
        cfg.writeEndDocument();
        fi.close();
    }

    /* Retour */
    return;
}

/*
 * Ecriture du fichier de gestionnaire d'elements orbitaux
 */
void GestionnaireXml::EcritureGestionnaireElementsOrbitaux()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + "gestionnaireElem.xml");

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatGestionElem");
        cfg.writeAttribute("version", Configuration::instance()->versionCategorieElem());

        // Categories
        QListIterator it(Configuration::instance()->listeCategoriesElementsOrbitaux());
        while (it.hasNext()) {

            const CategorieElementsOrbitaux categorie = it.next();

            cfg.writeStartElement("Categorie");

            QMapIterator it2(categorie.nom);
            while (it2.hasNext()) {
                it2.next();

                cfg.writeStartElement("Langue");
                cfg.writeAttribute("lang", it2.key());
                cfg.writeTextElement("Nom", it2.value().split("@").at(0).toLower());
                cfg.writeEndElement();
            }
            cfg.writeTextElement("Site", categorie.site);

            cfg.writeStartElement("Fichiers");
            QStringListIterator it3(categorie.fichiers);
            while (it3.hasNext()) {
                cfg.writeTextElement("Fichier", it3.next());
            }
            cfg.writeEndElement();

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
 * Lecture du fichier de categories d'orbite
 */
QMap<QString, QString> GestionnaireXml::LectureCategoriesOrbite()
{
    /* Declarations des variables locales */
    QString version;
    QMap<QString, QString> mapCategoriesOrbite;

    /* Initialisations */
    const QString nomficXml = "categories.xml";
    mapCategoriesOrbite.clear();

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, version, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatCategories") {

                    QString acronyme;
                    QString desc;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "Categorie") {

                            acronyme = "";
                            desc = "";

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Acronyme") {
                                    acronyme = cfg.readElementText();
                                } else if (cfg.name().toString() == "Description") {
                                    desc = cfg.readElementText();
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!acronyme.isEmpty()) {
                                mapCategoriesOrbite.insert(acronyme, desc);
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (mapCategoriesOrbite.isEmpty()) {
            const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucune catégorie d'orbite n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(msg.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return mapCategoriesOrbite;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier de configuration generale
 */
void GestionnaireXml::LectureConfiguration(QString &nomFichierEvenementsStationSpatiale,
                                           QString &noradStationSpatiale,
                                           QString &versionCfg,
                                           QString &adresseCelestrak,
                                           QString &nomfic,
                                           QString &noradDefaut,
                                           QList<Observateur> &observateurs,
                                           QMap<QString, QStringList> &mapSatellitesFichierElem)
{
    /* Declarations des variables locales */

    /* Initialisations */
    observateurs.clear();
    mapSatellitesFichierElem.clear();

    const QString nomficXml = "configuration.xml";

    try {

        const QString msg1 = QObject::tr("Le fichier de configuration de %1 a évolué.\n"
                                         "Certaines informations de configuration "
                                         "(par exemple les lieux d'observation sélectionnés) seront perdues.").arg(APP_NAME);
        const QString msg2 = QObject::tr("Le fichier %1 n'existe pas :\nUtilisation de la configuration par défaut");

        VerifieFichierXml(nomficXml, versionCfg, MessageType::WARNING, msg1, msg2);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatConfiguration") {

                    QString nom;
                    QStringList elements;

                    double lon;
                    double lat;
                    double alt;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "NoradStationSpatiale") {

                            noradStationSpatiale = cfg.readElementText();

                        } else if (cfg.name().toString() == "AdresseCelestrak") {

                            adresseCelestrak = cfg.readElementText();

                        } else if (cfg.name().toString() == "NomFichierEvenementsStationSpatiale") {

                            nomFichierEvenementsStationSpatiale = cfg.readElementText();

                        } else if (cfg.name().toString() == "Observateurs") {

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
                                        observateurs.append(Observateur(nom, lon, lat, alt));
                                    }

                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }
                        } else if (cfg.name().toString() == "FichiersElem") {

                            while (cfg.readNextStartElement()) {

                                nom = "";
                                if ((cfg.name().toString() == "Fichier") && (cfg.attributes().hasAttribute("nom"))) {
                                    nom = cfg.attributes().value("nom").toString();
                                } else {
                                    cfg.skipCurrentElement();
                                }

                                elements.clear();
                                while (cfg.readNextStartElement()) {
                                    if (cfg.name().toString() == "Norad") {
                                        elements.append(cfg.readElementText());
                                    } else {
                                        cfg.skipCurrentElement();
                                    }
                                }

                                if (!mapSatellitesFichierElem.contains(nom)) {

                                    mapSatellitesFichierElem.insert(nom, elements);

                                    if (nomfic.isEmpty()) {
                                        nomfic = nom;
                                        noradDefaut = elements.at(0);
                                    }
                                }
                            }

                        } else {
                            cfg.skipCurrentElement();
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (nomFichierEvenementsStationSpatiale.isEmpty()) {
            nomFichierEvenementsStationSpatiale = "ISS.OEM_J2K_EPH.xml";
        }

        if (noradStationSpatiale.isEmpty()) {
            noradStationSpatiale = "25544";
        }

        if (observateurs.isEmpty()) {
            observateurs.append(Observateur("Paris", -2.348640000, +48.853390000, 30));
        }

        if (mapSatellitesFichierElem.isEmpty() || nomfic.isEmpty() || noradDefaut.isEmpty()) {
            throw PreviSatException();
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier de gestionnaire d'elements orbitaux
 */
QList<CategorieElementsOrbitaux> GestionnaireXml::LectureGestionnaireElementsOrbitaux(QString &versionCategorieElem)
{
    /* Declarations des variables locales */
    QList<CategorieElementsOrbitaux> listeCategoriesElementsOrbitaux;

    /* Initialisations */
    listeCategoriesElementsOrbitaux.clear();
    const QString nomficXml = "gestionnaireElem.xml";

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, versionCategorieElem, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatGestionElem") {

                    QString langue;
                    QString nom;
                    QString site;
                    QStringList fichiers;
                    QMap<QString, QString> nomCategorie;

                    while (cfg.readNextStartElement()) {

                        fichiers.clear();
                        nomCategorie.clear();

                        if (cfg.name().toString() == "Categorie") {

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Langue") {

                                    // Nom de la categorie
                                    if (cfg.attributes().hasAttribute("lang")) {
                                        langue = cfg.attributes().value("lang").toString();

                                        while (cfg.readNextStartElement()) {

                                            if (cfg.name().toString() == "Nom") {
                                                nom = cfg.readElementText();
                                                nom[0] = nom[0].toUpper();
                                            } else {
                                                cfg.skipCurrentElement();
                                            }
                                        }

                                        if (!langue.isEmpty()) {
                                            nomCategorie.insert(langue, nom);
                                        }
                                    } else {
                                        cfg.skipCurrentElement();
                                    }

                                } else if (cfg.name().toString() == "Site") {

                                    // Site web
                                    site = cfg.readElementText();

                                } else if (cfg.name().toString() == "Fichiers") {

                                    // Nom des fichiers de la categorie
                                    while (cfg.readNextStartElement()) {

                                        if (cfg.name().toString() == "Fichier") {
                                            fichiers.append(cfg.readElementText());
                                        } else {
                                            cfg.skipCurrentElement();
                                        }
                                    }
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            QMapIterator it(nomCategorie);
                            while (it.hasNext()) {
                                it.next();
                                nomCategorie[it.key()] += "@" + site;
                            }

                            if (!nomCategorie.isEmpty()) {
                                listeCategoriesElementsOrbitaux.append({ nomCategorie, site, fichiers });
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);


        /* Retour */
        return listeCategoriesElementsOrbitaux;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier listant les pays et organisations
 */
QMap<QString, QString> GestionnaireXml::LecturePays()
{
    /* Declarations des variables locales */
    QString version;
    QMap<QString, QString> mapPays;

    /* Initialisations */
    const QString nomficXml = "pays.xml";

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, version, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatPays") {

                    QString acronyme;
                    QString desc;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "Pays") {

                            acronyme = "";
                            desc = "";

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Acronyme") {
                                    acronyme = cfg.readElementText();
                                } else if (cfg.name().toString() == "Description") {
                                    desc = cfg.readElementText();
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!acronyme.isEmpty()) {
                                mapPays.insert(acronyme, desc);
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (mapPays.isEmpty()) {
            const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun pays ou organisation n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(msg.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return mapPays;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier de satellites TDRS
 */
QMap<int, SatelliteTDRS> GestionnaireXml::LectureSatellitesTDRS()
{
    /* Declarations des variables locales */
    QString version;
    QMap<int, SatelliteTDRS> mapTDRS;

    /* Initialisations */
    mapTDRS.clear();
    const QString nomficXml = "tdrs.xml";

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, version, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatTDRS") {

                    int numero;
                    QString denomination;
                    int rouge;
                    int vert;
                    int bleu;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "TDRS") {

                            numero = 0;
                            denomination = "";
                            rouge = 0;
                            vert = 0;
                            bleu = 0;

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Numero") {
                                    numero = cfg.readElementText().toInt();
                                } else if (cfg.name().toString() == "Denomination") {
                                    denomination = cfg.readElementText();
                                } else if (cfg.name().toString() == "CouleurR") {
                                    rouge = cfg.readElementText().toInt();
                                } else if (cfg.name().toString() == "CouleurV") {
                                    vert = cfg.readElementText().toInt();
                                } else if (cfg.name().toString() == "CouleurB") {
                                    bleu = cfg.readElementText().toInt();
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!denomination.isEmpty()) {
                                mapTDRS.insert(numero, { denomination, rouge, vert, bleu });
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (mapTDRS.isEmpty()) {
            const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun satellite TDRS n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(msg.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return mapTDRS;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier des sites de lancement
 */
QMap<QString, Observateur> GestionnaireXml::LectureSitesLancement()
{
    /* Declarations des variables locales */
    QString version;
    QMap<QString, Observateur> mapSitesLancement;

    /* Initialisations */
    mapSitesLancement.clear();
    const QString nomficXml = "sites.xml";

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, version, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatSites") {

                    QString acronyme;
                    QString desc;
                    double lon;
                    double lat;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "Site") {

                            acronyme = "";
                            desc = "";
                            lon = 0.;
                            lat = 0.;

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Acronyme") {
                                    acronyme = cfg.readElementText();
                                } else if (cfg.name().toString() == "Description") {
                                    desc = cfg.readElementText();
                                } else if (cfg.name().toString() == "Longitude") {
                                    lon = cfg.readElementText().toDouble();
                                } else if (cfg.name().toString() == "Latitude") {
                                    lat = cfg.readElementText().toDouble();
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!acronyme.isEmpty()) {
                                mapSitesLancement.insert(acronyme, Observateur(desc, lon, lat));
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (mapSitesLancement.isEmpty()) {
            const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun site de lancement n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(msg.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return mapSitesLancement;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier de stations
 */
QMap<QString, Observateur> GestionnaireXml::LectureStations()
{
    /* Declarations des variables locales */
    QString version;
    QMap<QString, Observateur> mapStations;

    /* Initialisations */
    mapStations.clear();
    const QString nomficXml = "stations.xml";

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, version, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatStations") {

                    QString acronyme;
                    QString nom;
                    double lon;
                    double lat;
                    double alt;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "Station") {

                            acronyme = "";
                            nom = "";
                            lon = 0.;
                            lat = 0.;
                            alt = 0.;

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Acronyme") {
                                    acronyme = cfg.readElementText();
                                } else if (cfg.name().toString() == "Nom") {
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

                            if (!acronyme.isEmpty()) {
                                mapStations.insert(acronyme, Observateur(nom, lon, lat, alt));
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (mapStations.isEmpty()) {
            const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucune station n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(msg.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return mapStations;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Lecture du fichier de statut des satellites produisant des flashs
 */
QMap<QString, SatellitesFlashs> GestionnaireXml::LectureStatutSatellitesFlashs()
{
    /* Declarations des variables locales */
    QString version;
    QMap<QString, SatellitesFlashs> mapFlashs;

    /* Initialisations */
    mapFlashs.clear();
    const QString nomficXml = "flares.xml";

    try {

        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Le fichier %1 n'existe pas, veuillez réinstaller %2");

        VerifieFichierXml(nomficXml, version, MessageType::ERREUR, "", message);

        /* Corps de la methode */
        QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi1.exists() && (fi1.size() != 0)) {

            if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QXmlStreamReader cfg(&fi1);

                cfg.readNextStartElement();
                if (cfg.name().toString() == "PreviSatFlashs") {

                    QString norad;
                    QPair<double, double> angles;
                    SatellitesFlashs satelliteFlash;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString() == "Satellite") {

                            norad = "";
                            satelliteFlash.angles.clear();

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Nom") {
                                    satelliteFlash.nomsat = cfg.readElementText();
                                } else if (cfg.name().toString() == "Norad") {
                                    norad = cfg.readElementText();
                                } else if (cfg.name().toString() == "Angles") {

                                    while (cfg.readNextStartElement()) {

                                        if (cfg.name().toString() == "Yaw") {
                                            angles.first = cfg.readElementText().toDouble() * DEG2RAD;
                                        } else if (cfg.name().toString() == "Pitch") {
                                            angles.second = cfg.readElementText().toDouble() * DEG2RAD;
                                        } else {
                                            cfg.skipCurrentElement();
                                        }
                                    }
                                    satelliteFlash.angles.append(angles);

                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!norad.isEmpty()) {
                                mapFlashs.insert(norad, satelliteFlash);
                            }
                        }
                    }
                }
            }
            fi1.close();
        }

        // Verifications
        if (mapFlashs.isEmpty()) {
            const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun satellite produisant des flashs n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(msg.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

        /* Retour */
        return mapFlashs;

    } catch (PreviSatException &e) {
        throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                MessageType::ERREUR);
    }
}

/*
 * Verification du fichier xml
 */
void GestionnaireXml::VerifieFichierXml(const QString &nomficXml, QString &version, const MessageType &typeMessage, const QString &message1,
                                        const QString &message2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

#if defined (Q_OS_MAC)
    if (!fi1.exists()) {
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(message2.arg(ff.fileName()).arg(APP_NAME), typeMessage);
    }
#else
    fi1.open(QIODevice::ReadOnly | QIODevice::Text);

    QFile fi2(Configuration::instance()->dirCommonData() + QDir::separator() + "config" + QDir::separator() + nomficXml);

    if (!fi1.exists()) {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());

        } else {
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(message2.arg(ff.fileName()).arg(APP_NAME), typeMessage);
        }
    }

    VerifieVersionXml(fi1, fi2, version, message1);

    fi2.close();
#endif
    fi1.close();

    /* Retour */
    return;
}

/*
 * Verification du numero de version du fichier xml
 */
void GestionnaireXml::VerifieVersionXml(QFile &fi1, QFile &fi2, QString &version, const QString &msg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (fi2.exists()) {

        QDomDocument doc;
        if (fi2.open(QIODevice::ReadOnly | QIODevice::Text)) {

            doc.setContent(&fi2);
            const QString versionNew = doc.documentElement().attribute("version");

            if (fi1.exists()) {

                // Comparaison des numeros de version du fichier xml
                doc.clear();
                doc.setContent(&fi1);
                version = doc.documentElement().attribute("version");
                fi1.seek(0);

                if (versionNew != version) {

                    if (!msg.isEmpty()) {
                        Message::Afficher(msg, MessageType::WARNING);
                    }

                    // Copie du fichier xml
                    fi1.remove();
                    fi2.copy(fi1.fileName());
                    version = versionNew;
                }
            } else {

                // Copie du fichier xml
                fi2.copy(fi1.fileName());
                version = versionNew;
            }
        }
    }

    /* Retour */
    return;
}
