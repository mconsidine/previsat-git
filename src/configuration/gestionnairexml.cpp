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
 * >    6 octobre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QXmlStreamReader>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "categorieelementsorbitaux.h"
#include "configuration.h"
#include "gestionnairexml.h"
#include "librairies/exceptions/previsatexception.h"


QList<Observateur> GestionnaireXml::_observateurs;


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

            cfg.writeTextElement("Longitude", QString::number(obs.longitude() * MATHS::RAD2DEG, 'f', 9));
            cfg.writeTextElement("Latitude", QString::number(obs.latitude() * MATHS::RAD2DEG, 'f', 9));
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
                cfg.writeTextElement("Norad", QString("%1").arg(itNorad.next(), 6, '0'));
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
                    cfg.writeTextElement("Norad", QString("%1").arg(itNorad.next(), 6, '0'));
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
        QMapIterator it1(Configuration::instance()->mapCategoriesElementsOrbitaux());
        while (it1.hasNext()) {
            it1.next();

            const QString site = it1.key();

            if (!site.isEmpty()) {

                const QList<CategorieElementsOrbitaux> listeCategorie = it1.value();

                cfg.writeStartElement("Categories");
                cfg.writeAttribute("site", site);

                QListIterator it2(listeCategorie);
                while (it2.hasNext()) {

                    const CategorieElementsOrbitaux categorie = it2.next();

                    cfg.writeStartElement("Categorie");
                    cfg.writeAttribute("maj", QString::number(categorie.miseAjour));

                    QMapIterator it3(categorie.nom);
                    while (it3.hasNext()) {
                        it3.next();

                        cfg.writeStartElement("Langue");
                        cfg.writeAttribute("lang", it3.key());
                        cfg.writeTextElement("Nom", it3.value());
                        cfg.writeEndElement();
                    }

                    cfg.writeStartElement("Fichiers");
                    QStringListIterator it4(categorie.fichiers);
                    while (it4.hasNext()) {
                        cfg.writeTextElement("Fichier", it4.next());
                    }
                    cfg.writeEndElement();
                    cfg.writeEndElement();
                }

                cfg.writeEndElement();
            }
        }

        cfg.writeEndElement();
        cfg.writeEndDocument();
        fi.close();
    }

    /* Retour */
    return;
}

/*
 * Ecriture du fichier Pre-Launch Starlink
 */
void GestionnaireXml::EcriturePreLaunchStarlink()
{
    /* Declarations des variables locales */

    /* Initialisations */
    // Liste des fichiers d'elements orbitaux a supprimer
    const QDir di(Configuration::instance()->dirStarlink());
    const QStringList filtres(QStringList () << "*.xml");
    QStringList listeElem = di.entryList(filtres, QDir::Files);

    /* Corps de la methode */
    QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + "pre-launch.xml");

    if (fi1.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi1);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatStarlink");
        cfg.writeAttribute("version", Configuration::instance()->versionStarlink());

        // Groupes Starlink
        QMapIterator it(Configuration::instance()->satellitesStarlink());
        while (it.hasNext()) {
            it.next();

            const QString groupe = it.key();
            const SatellitesStarlink starlink = it.value();

            // Ecriture des informations pour les fichiers pre-launch
            cfg.writeStartElement("Starlink");

            // Informations du groupe Starlink
            cfg.writeTextElement("Fichier", starlink.fichier);
            cfg.writeTextElement("Groupe", groupe);
            cfg.writeTextElement("Lancement", starlink.lancement);
            cfg.writeTextElement("Deploiement", starlink.deploiement);
            cfg.writeEndElement();

            listeElem.removeOne(starlink.fichier + ".xml");
        }

        cfg.writeEndDocument();
    }

    fi1.close();

    // Suppression des fichiers d'elements orbitaux inutiles
    QFile fi2;
    QStringListIterator it2(listeElem);
    while (it2.hasNext()) {

        fi2.setFileName(Configuration::instance()->dirStarlink() + QDir::separator() + it2.next());
        if (fi2.exists()) {
            fi2.remove();
        }
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
    QMap<QString, QString> mapCategoriesOrbite;

    /* Initialisations */
    mapCategoriesOrbite.clear();

    try {

        QString version;
        const QString nomficXml = "categories.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

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
        fi.close();

        // Verifications
        if (mapCategoriesOrbite.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapCategoriesOrbite;
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
    _observateurs.clear();
    observateurs.clear();
    mapSatellitesFichierElem.clear();

    try {

        const QString nomficXml = "configuration.xml";

        const QString message = QObject::tr("Le fichier de configuration de %1 a évolué.\n" \
                                            "Souhaitez-vous tenter de récupérer les lieux d'observation ?").arg(APP_NAME);

        VerifieFichierXml(nomficXml, versionCfg, message);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

            cfg.readNextStartElement();
            if (cfg.name().toString() == "PreviSatConfiguration") {

                QString nom;
                QStringList elements;

                while (cfg.readNextStartElement()) {

                    if (cfg.name().toString() == "NoradStationSpatiale") {

                        noradStationSpatiale = QString("%1").arg(cfg.readElementText(), 6, '0');

                    } else if (cfg.name().toString() == "AdresseCelestrak") {

                        adresseCelestrak = cfg.readElementText();

                    } else if (cfg.name().toString() == "NomFichierEvenementsStationSpatiale") {

                        nomFichierEvenementsStationSpatiale = cfg.readElementText();

                    } else if (cfg.name().toString() == "Observateurs") {

                        observateurs = LectureLieuxObservation(cfg);

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
                                    elements.append(QString("%1").arg(cfg.readElementText(), 6, '0'));
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!mapSatellitesFichierElem.contains(nom)) {

                                mapSatellitesFichierElem.insert(nom, elements);

                                if (nomfic.isEmpty()) {
                                    nomfic = nom;
                                    noradDefaut = elements.first();
                                }
                            }
                        }

                    } else {
                        cfg.skipCurrentElement();
                    }
                }
            }
        }
        fi.close();

        // Verifications
        if (!_observateurs.isEmpty()) {
            observateurs = _observateurs;
        }

        if (nomfic.isEmpty()) {
            nomfic = "visual.xml";
        }

        if (noradDefaut.isEmpty()) {
            noradDefaut = noradStationSpatiale;
        }

        if (nomFichierEvenementsStationSpatiale.isEmpty()) {
            nomFichierEvenementsStationSpatiale = "ISS.OEM_J2K_EPH.xml";
        }

        if (mapSatellitesFichierElem.isEmpty()) {
            const QStringList elem(QStringList() << noradStationSpatiale << "20580");
            mapSatellitesFichierElem.insert(nomfic, elem);
        }

        if (noradStationSpatiale.isEmpty() || observateurs.isEmpty()) {

            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier contenant les frequences radio des satellites
 */
QMap<QString, QList<FrequenceRadio> > GestionnaireXml::LectureFrequencesRadio()
{
    /* Declarations des variables locales */
    QMap<QString, QList<FrequenceRadio> > mapFrequencesRadio;

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString nomficXml = "radio.xml";

        QFile fi(Configuration::instance()->dirLocalData() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

            cfg.readNextStartElement();
            if (cfg.name().toString() == "PreviSatRadio") {

                QString norad;
                FrequenceRadio frequences;
                QList<FrequenceRadio> listeFrequences;

                while (cfg.readNextStartElement()) {

                    if (cfg.name().toString() == "Satellite") {

                        listeFrequences.clear();

                        while (cfg.readNextStartElement()) {

                            if (cfg.name().toString() == "Norad") {
                                norad = cfg.readElementText();

                            } else if (cfg.name().toString() == "Nom") {
                                frequences.nom = cfg.readElementText();

                            } else if (cfg.name().toString() == "Frequences") {

                                frequences.frequenceDescendante.clear();
                                frequences.frequenceMontante.clear();
                                frequences.balise = "";
                                frequences.mode = "";
                                frequences.signalAppel = "";

                                while (cfg.readNextStartElement()) {

                                    if (cfg.name().toString() == "FrequenceMontante") {
                                        frequences.frequenceMontante.append(cfg.readElementText().remove("*"));

                                    } else if (cfg.name().toString() == "FrequenceDescendante") {
                                        frequences.frequenceDescendante.append(cfg.readElementText().remove("*"));

                                    } else if (cfg.name().toString() == "Balise") {
                                        frequences.balise = cfg.readElementText();

                                    } else if (cfg.name().toString() == "Mode") {
                                        frequences.mode = cfg.readElementText();

                                    } else if (cfg.name().toString() == "SignalAppel") {
                                        frequences.signalAppel = cfg.readElementText();

                                    } else {
                                        cfg.skipCurrentElement();
                                    }
                                }

                                if (!frequences.nom.isEmpty()) {
                                    listeFrequences.append(frequences);
                                }
                            }
                        }

                        if (!listeFrequences.isEmpty() && !mapFrequencesRadio.contains(norad)) {
                            mapFrequencesRadio.insert(norad, listeFrequences);
                        }
                    } else {
                        cfg.skipCurrentElement();
                    }
                }
            }
        }

        fi.close();

        // Verifications

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapFrequencesRadio;
}

/*
 * Lecture du fichier de gestionnaire d'elements orbitaux
 */
QMap<QString, QList<CategorieElementsOrbitaux> > GestionnaireXml::LectureGestionnaireElementsOrbitaux(
        QString &versionCategorieElem, QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesMajElementsOrbitaux)
{
    /* Declarations des variables locales */
    QMap<QString, QList<CategorieElementsOrbitaux> > mapCategoriesElementsOrbitaux;

    /* Initialisations */
    mapCategoriesMajElementsOrbitaux.clear();

    try {

        const QString nomficXml = "gestionnaireElem.xml";

        VerifieFichierXml(nomficXml, versionCategorieElem);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

            cfg.readNextStartElement();
            if (cfg.name().toString() == "PreviSatGestionElem") {

                Qt::CheckState miseAjour;
                QString langue;
                QString nom;
                QString site;
                QStringList fichiers;
                QMap<QString, QString> nomCategorie;
                QList<CategorieElementsOrbitaux> listeCategories;
                QList<CategorieElementsOrbitaux> listeCategoriesMaj;

                while (cfg.readNextStartElement()) {

                    // Categorie d'elements orbitaux
                    if (cfg.name().toString() == "Categories") {

                        listeCategories.clear();
                        listeCategoriesMaj.clear();

                        if (cfg.attributes().hasAttribute("site")) {

                            site = cfg.attributes().value("site").toString();

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Categorie") {

                                    miseAjour = Qt::Unchecked;
                                    fichiers.clear();
                                    nomCategorie.clear();

                                    // Flag de mise a jour de la categorie
                                    if (cfg.attributes().hasAttribute("maj")) {

                                        miseAjour = static_cast<Qt::CheckState> (cfg.attributes().value("maj").toInt());

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

                                                        if (!nom.isEmpty()) {
                                                            nomCategorie.insert(langue, nom);
                                                        }
                                                    }
                                                }
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

                                        if (!nomCategorie.isEmpty()) {
                                            listeCategories.append({ miseAjour, nomCategorie, fichiers });

                                            if (miseAjour) {
                                                listeCategoriesMaj.append({ miseAjour, nomCategorie, fichiers });
                                            }
                                        }
                                    }
                                }
                            }

                            if (!listeCategories.isEmpty()) {
                                mapCategoriesElementsOrbitaux.insert(site, listeCategories);
                            }

                            if (!listeCategoriesMaj.isEmpty()) {
                                mapCategoriesMajElementsOrbitaux.insert(site, listeCategoriesMaj);
                            }
                        }
                    } else {
                        cfg.skipCurrentElement();
                    }
                }
            } else {
                cfg.skipCurrentElement();
            }
        }

        fi.close();

        if (mapCategoriesElementsOrbitaux.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        // Verifications
        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapCategoriesElementsOrbitaux;
}

/*
 * Lecture de la structure de lieux d'observations
 */
QList<Observateur> GestionnaireXml::LectureLieuxObservation(QXmlStreamReader &cfg)
{
    /* Declarations des variables locales */
    QString nom;
    double lon;
    double lat;
    double alt;
    QList<Observateur> obs;

    /* Initialisations */

    /* Corps de la methode */
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
                obs.append(Observateur(nom, lon, lat, alt));
            }
        } else {
            cfg.skipCurrentElement();
        }
    }

    /* Retour */
    return obs;
}

/*
 * Lecture du fichier listant les pays et organisations
 */
QMap<QString, QString> GestionnaireXml::LecturePays()
{
    /* Declarations des variables locales */
    QMap<QString, QString> mapPays;

    /* Initialisations */

    try {

        QString version;
        const QString nomficXml = "pays.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

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
        fi.close();

        // Verifications
        if (mapPays.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapPays;
}

/*
 * Lecture du fichier Pre-Launch Starlink
 */
QMap<QString, SatellitesStarlink> GestionnaireXml::LecturePreLaunchStarlink(QString &version)
{
    /* Declarations des variables locales */
    QMap<QString, SatellitesStarlink> mapSatellitesStarlink;

    /* Initialisations */
    try {

        const QString nomficXml = "pre-launch.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

            cfg.readNextStartElement();
            if (cfg.name().toString() == "PreviSatStarlink") {

                QString groupe;
                QString fichier;
                QString lancement;
                QString deploiement;

                while (cfg.readNextStartElement()) {

                    if (cfg.name().toString() == "Starlink") {

                        groupe = "";
                        fichier = "";
                        lancement = "";
                        deploiement = "";

                        while (cfg.readNextStartElement()) {

                            if (cfg.name().toString() == "Fichier") {
                                fichier = cfg.readElementText();
                            } else if (cfg.name().toString() == "Groupe") {
                                groupe = cfg.readElementText();
                            } else if (cfg.name().toString() == "Lancement") {
                                lancement = cfg.readElementText();
                            } else if (cfg.name().toString() == "Deploiement") {
                                deploiement = cfg.readElementText();
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        // Recuperation des informations Starlink
                        if (!groupe.isEmpty()) {
                            mapSatellitesStarlink.insert(groupe, { fichier, lancement, deploiement });
                        }
                    }
                }
            }
        }

        fi.close();

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapSatellitesStarlink;
}

/*
 * Lecture du fichier de satellites TDRS
 */
QMap<int, SatelliteTDRS> GestionnaireXml::LectureSatellitesTDRS()
{
    /* Declarations des variables locales */
    QMap<int, SatelliteTDRS> mapTDRS;

    /* Initialisations */
    mapTDRS.clear();

    try {

        QString version;
        const QString nomficXml = "tdrs.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

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
        fi.close();

        // Verifications
        if (mapTDRS.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapTDRS;
}

/*
 * Lecture du fichier des sites de lancement
 */
QMap<QString, Observateur> GestionnaireXml::LectureSitesLancement()
{
    /* Declarations des variables locales */
    QMap<QString, Observateur> mapSitesLancement;

    /* Initialisations */
    mapSitesLancement.clear();

    try {

        QString version;
        const QString nomficXml = "sites.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

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
        fi.close();

        // Verifications
        if (mapSitesLancement.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapSitesLancement;
}

/*
 * Lecture du fichier de stations
 */
QMap<QString, Observateur> GestionnaireXml::LectureStations()
{
    /* Declarations des variables locales */
    QMap<QString, Observateur> mapStations;

    /* Initialisations */
    mapStations.clear();

    try {

        QString version;
        const QString nomficXml = "stations.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

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
        fi.close();

        // Verifications
        if (mapStations.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapStations;
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

    try {

        const QString nomficXml = "flares.xml";

        VerifieFichierXml(nomficXml, version);

        /* Corps de la methode */
        QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

        if (!fi.exists() || (fi.size() == 0)) {
            qCritical() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME),
                                    MessageType::ERREUR);
        }

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi);

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

                            // Nom du satellite et numero NORAD
                            if (cfg.name().toString() == "Nom") {
                                satelliteFlash.nomsat = cfg.readElementText();

                            } else if (cfg.name().toString() == "Norad") {
                                norad = cfg.readElementText();

                            } else if (cfg.name().toString() == "Angles") {

                                // Lecture des angles
                                while (cfg.readNextStartElement()) {

                                    if (cfg.name().toString() == "Yaw") {
                                        angles.first = cfg.readElementText().toDouble() * MATHS::DEG2RAD;

                                    } else if (cfg.name().toString() == "Pitch") {
                                        angles.second = cfg.readElementText().toDouble() * MATHS::DEG2RAD;

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
        fi.close();

        // Verifications
        if (mapFlashs.isEmpty()) {
            qCritical() << QString("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw PreviSatException(QObject::tr("Erreur lors de la lecture du fichier %1, veuillez réinstaller %2")
                                    .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

#if (BUILD_TEST == false)
        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);
#endif

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return mapFlashs;
}

/*
 * Verification du fichier xml
 */
void GestionnaireXml::VerifieFichierXml(const QString &nomficXml, QString &version, const QString &message)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

#if defined (Q_OS_MAC)
    if (!fi1.exists()) {
        const QFileInfo ff(fi1.fileName());
        qCritical() << QString("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(fic).arg(APP_NAME);
        throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(fic).arg(APP_NAME),
                                MessageType::ERREUR);
    }
#else
    fi1.open(QIODevice::ReadOnly | QIODevice::Text);

    QFile fi2(Configuration::instance()->dirCommonData() + QDir::separator() + "config" + QDir::separator() + nomficXml);

    if (!fi1.exists()) {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());
        }
    }

    VerifieVersionXml(fi1, fi2, version, message);

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

                        QMessageBox msgbox(QMessageBox::Question, QObject::tr("Avertissement"), msg);
                        QPushButton * const oui = msgbox.addButton(QObject::tr("Oui"), QMessageBox::YesRole);
                        msgbox.addButton(QObject::tr("Non"), QMessageBox::NoRole);
                        msgbox.setDefaultButton(oui);
                        msgbox.exec();

                        if (msgbox.clickedButton() == oui) {

                            // Tentative de recuperation des lieux d'observation
                            qInfo() << "Tentative de recuperation des lieux d'observation";
                            QXmlStreamReader cfg(&fi1);
                            cfg.readNextStartElement();

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString() == "Observateurs") {
                                    _observateurs = LectureLieuxObservation(cfg);
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }

                            if (!_observateurs.isEmpty()) {
                                qInfo() << "Recuperation de" << _observateurs.size() << "lieux d'observation";
                            }
                        }
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
