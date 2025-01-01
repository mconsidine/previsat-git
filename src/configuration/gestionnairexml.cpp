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
 * >    12 decembre 2024
 *
 */

#include <QMessageBox>
#include <QPushButton>
#include "configuration.h"
#include "gestionnairexml.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"
#include "librairies/systeme/telechargement.h"


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
 * Ecriture du fichier de gestionnaire d'elements orbitaux
 */
void GestionnaireXml::EcritureGestionnaireElementsOrbitaux()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + "gestionnaireElem.xml");

    if (!fi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        const QFileInfo ff(fi.fileName());
        qWarning() << QString("Erreur lors de l'écriture du fichier %1").arg(ff.fileName());
        throw Exception(QObject::tr("Erreur lors de l'écriture du fichier %1").arg(ff.fileName()), MessageType::WARNING);
    }

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

            EcritureCategoriesElementsOrbitaux(listeCategorie, cfg);
            cfg.writeEndElement();
        }
    }

    cfg.writeEndElement();
    cfg.writeEndDocument();
    fi.close();

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

    /* Corps de la methode */
    if (Configuration::instance()->satellitesStarlink().isEmpty()) {
        throw Exception();
    }

    const Date aujourdhui;

    // Liste des fichiers d'elements orbitaux a supprimer
    const QDir di(Configuration::instance()->dirStarlink());
    const QStringList filtres(QStringList () << "*.xml");
    QStringList listeElem = di.entryList(filtres, QDir::Files);

    QFile fi1(Configuration::instance()->dirCfg() + QDir::separator() + "pre-launch.xml");

    if (!fi1.open(QIODevice::WriteOnly | QIODevice::Text)) {
        const QFileInfo ff(fi1.fileName());
        qWarning() << QString("Erreur lors de l'écriture du fichier %1").arg(ff.fileName());
        throw Exception(QObject::tr("Erreur lors de l'écriture du fichier %1").arg(ff.fileName()), MessageType::WARNING);
    }

    QXmlStreamWriter cfg(&fi1);

    cfg.setAutoFormatting(true);
    cfg.writeStartDocument();
    cfg.writeStartElement("PreviSatStarlink");

    // Groupes Starlink
    QMapIterator it(Configuration::instance()->satellitesStarlink());
    while (it.hasNext()) {
        it.next();

        const QString groupe = it.key();
        const SatellitesStarlink starlink = it.value();
        const Date dateLancement = Date::ConversionDateIso(starlink.lancement);

        // Sauvegarde des elements starlink recents
        if (fabs(dateLancement.jourJulienUTC() - aujourdhui.jourJulienUTC()) < STARLINK::AGE_MAXIMAL_ELEM) {

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
    }

    cfg.writeEndDocument();
    fi1.close();

    // Suppression des fichiers d'elements orbitaux inutiles
    QFile fi2;
    QStringListIterator it2(listeElem);
    while (it2.hasNext()) {

        const QString fic = it2.next();

        if (fic.split("-").last().contains("b")) {
            fi2.setFileName(Configuration::instance()->dirStarlink() + QDir::separator() + fic);
            if (fi2.exists()) {
                fi2.remove();
            }
        }
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
QList<Observateur> GestionnaireXml::observateurs()
{
    return _observateurs;
}


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
 * Ecriture des categories d'elements orbitaux
 */
void GestionnaireXml::EcritureCategoriesElementsOrbitaux(const QList<CategorieElementsOrbitaux> &listeCategorie,
                                                         QXmlStreamWriter &cfg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
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

    /* Retour */
    return;
}

/*
 * Lecture des categories d'elements orbitaux
 */
QList<CategorieElementsOrbitaux> GestionnaireXml::LectureCategoriesElementsOrbitaux(const QDomNode &serveur,
                                                                                    QList<CategorieElementsOrbitaux> &categorieMajElem)
{
    /* Declarations des variables locales */
    QList<CategorieElementsOrbitaux> categorieElem;

    /* Initialisations */
    categorieMajElem.clear();

    /* Corps de la methode */
    if (!serveur.isNull()) {

        Qt::CheckState maj;
        QStringList fichiers;
        QMap<QString, QString> mapNom;
        const QDomNodeList listeCategories = serveur.toElement().elementsByTagName("Categorie");

        for(int i=0; i<listeCategories.count(); i++) {

            fichiers.clear();
            mapNom.clear();
            const QDomNode categorie = listeCategories.at(i);
            maj = static_cast<Qt::CheckState> (categorie.toElement().attribute("maj").toInt());

            // Noms de la categorie
            const QDomNodeList listeLangues = categorie.toElement().elementsByTagName("Langue");

            for(int j=0; j<listeLangues.count(); j++) {

                const QDomNode langue = listeLangues.at(j);
                const QString lang = langue.toElement().attribute("lang");
                const QString nom = langue.firstChildElement("Nom").text();

                if (!nom.isEmpty()) {
                    mapNom.insert(lang, nom);
                }
            }

            // Fichiers
            const QDomNodeList listeFichiers = categorie.firstChildElement("Fichiers").elementsByTagName("Fichier");

            for(int j=0; j<listeFichiers.count(); j++) {
                fichiers.append(listeFichiers.at(j).toElement().text());
            }

            categorieElem.append({ maj, mapNom, fichiers });

            if (maj == Qt::Checked) {
                categorieMajElem.append({ maj, mapNom, fichiers });
            }
        }
    }

    /* Retour */
    return categorieElem;
}

/*
 * Lecture du fichier de categories d'orbite
 */
QMap<QString, QString> GestionnaireXml::LectureCategoriesOrbite()
{
    /* Declarations des variables locales */
    QMap<QString, QString> mapCategoriesOrbite;

    /* Initialisations */
    const QString nomficXml = "categories.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatCategories") {
            throw Exception();
        }

        const QDomNodeList listeCategories = root.elementsByTagName("Categorie");

        for(int i=0; i<listeCategories.count(); i++) {

            const QDomNode categorie = listeCategories.at(i);
            if (!categorie.isNull()) {

                const QString acronyme = categorie.firstChildElement("Acronyme").text();
                const QString description = categorie.firstChildElement("Description").text();

                if (!acronyme.isEmpty()) {
                    mapCategoriesOrbite.insert(acronyme, description);
                }
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapCategoriesOrbite;
}

/*
 * Lecture du fichier contenant les frequences radio des satellites
 */
QMap<QString, QList<FrequenceRadio> > GestionnaireXml::LectureFrequencesRadio()
{
    /* Declarations des variables locales */
    QMap<QString, QList<FrequenceRadio> > mapFrequencesRadio;

    /* Initialisations */
    const QString nomficXml = "radio.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatRadio") {
            throw Exception();
        }

        const QDomNodeList listeSatellites = root.elementsByTagName("Satellite");

        for(int i=0; i<listeSatellites.count(); i++) {

            const QDomNode sat = listeSatellites.at(i);
            const QString norad = sat.firstChildElement("Norad").text();
            const QList<FrequenceRadio> listeFrequences = LectureSatellitesFrequences(sat);

            if (!norad.isEmpty()) {
                mapFrequencesRadio.insert(norad, listeFrequences);
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapFrequencesRadio;
}

/*
 * Lecture du fichier de gestionnaire d'elements orbitaux
 */
QMap<QString, QList<CategorieElementsOrbitaux> >
GestionnaireXml::LectureGestionnaireElementsOrbitaux(QString &versionCategorieElem,
                                                     QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesMajElementsOrbitaux)
{
    /* Declarations des variables locales */
    QMap<QString, QList<CategorieElementsOrbitaux> > mapCategoriesElementsOrbitaux;

    /* Initialisations */
    const QString nomficXml = "gestionnaireElem.xml";
    mapCategoriesMajElementsOrbitaux.clear();

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);
        versionCategorieElem = fi.version();

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatGestionElem") {
            throw Exception();
        }

        QList<CategorieElementsOrbitaux> categorieMajElem;
        const QDomNodeList listeServeurs = root.elementsByTagName("Categories");

        for(int i=0; i<listeServeurs.count(); i++) {

            const QDomNode serveur = listeServeurs.at(i);
            if (!serveur.isNull()) {

                const QString site = serveur.toElement().attribute("site");
                const QList<CategorieElementsOrbitaux> categorieElem = LectureCategoriesElementsOrbitaux(serveur, categorieMajElem);

                if (!site.isEmpty()) {
                    mapCategoriesElementsOrbitaux.insert(site, categorieElem);
                    mapCategoriesMajElementsOrbitaux.insert(site, categorieMajElem);
                }
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapCategoriesElementsOrbitaux;
}

/*
 * Lecture de la structure de lieux d'observations
 */
QList<Observateur> GestionnaireXml::LectureLieuxObservation(const QDomNode &obs)
{
    /* Declarations des variables locales */
    QList<Observateur> obser;

    /* Initialisations */

    /* Corps de la methode */
    if (!obs.isNull()) {

        const QDomNodeList listeObservateurs = obs.toElement().elementsByTagName("Observateur");

        for(int i=0; i<listeObservateurs.count(); i++) {

            const QDomNode observateur = listeObservateurs.at(i);
            const QString nom = observateur.firstChildElement("Nom").text();
            const double longitude = observateur.firstChildElement("Longitude").text().toDouble();
            const double latitude = observateur.firstChildElement("Latitude").text().toDouble();
            const double altitude = observateur.firstChildElement("Altitude").text().toDouble();

            if (!nom.isEmpty()) {
                obser.append(Observateur(nom, longitude, latitude, altitude));
            }
        }
    }

    /* Retour */
    return obser;
}

/*
 * Lecture des noms des constellations
 */
QMap<QString, QMap<QString, QString> > GestionnaireXml::LectureNomsConstellations()
{
    /* Declarations des variables locales */
    QMap<QString, QMap<QString, QString> > mapNomsConstellations;

    /* Initialisations */
    const QString nomficXml = "constnames.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatConstellations") {
            throw Exception();
        }

        QMap<QString, QString> mapNom;
        const QDomNodeList listeConstellations = root.elementsByTagName("Constellation");

        for(int i=0; i<listeConstellations.count(); i++) {

            const QDomNode constellation = listeConstellations.at(i);
            const QString acronyme = constellation.firstChildElement("Acronyme").text();

            mapNom.clear();
            const QDomNodeList listeLangues = constellation.toElement().elementsByTagName("Langue");

            for(int j=0; j<listeLangues.count(); j++) {

                const QDomNode langue = listeLangues.at(j);
                const QString lang = langue.toElement().attribute("lang");
                const QString nom = langue.firstChildElement("Nom").text();

                if (!nom.isEmpty()) {
                    mapNom.insert(lang, nom);
                }
            }

            mapNomsConstellations.insert(acronyme, mapNom);
        }

#if (!BUILD_TEST)
        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);
#endif

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapNomsConstellations;
}

/*
 * Lecture du fichier listant les pays et organisations
 */
QMap<QString, QString> GestionnaireXml::LecturePays()
{
    /* Declarations des variables locales */
    QMap<QString, QString> mapPays;

    /* Initialisations */
    const QString nomficXml = "pays.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatPays") {
            throw Exception();
        }

        const QDomNodeList listePays = root.elementsByTagName("Pays");

        for(int i=0; i<listePays.count(); i++) {

            const QDomNode pays = listePays.at(i);
            if (!pays.isNull()) {

                const QString acronyme = pays.firstChildElement("Acronyme").text();
                const QString description = pays.firstChildElement("Description").text();

                if (!acronyme.isEmpty()) {
                    mapPays.insert(acronyme, description);
                }
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapPays;
}

/*
 * Lecture du fichier Pre-Launch Starlink
 */
QMap<QString, SatellitesStarlink> GestionnaireXml::LecturePreLaunchStarlink()
{
    /* Declarations des variables locales */
    QMap<QString, SatellitesStarlink> mapSatellitesStarlink;

    /* Initialisations */
    const QString nomficXml = "pre-launch.xml";
    FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);

    /* Corps de la methode */
    try {

        if (Configuration::instance()->mapVerrous().contains("starlink")) {

            const QDomDocument document = fi.Ouverture(false);
            const QDomElement root = document.firstChildElement();

            if (root.nodeName() != "PreviSatStarlink") {
                throw Exception();
            }

            const QDomNodeList listeStarlink = root.elementsByTagName("Starlink");

            for(int i=0; i<listeStarlink.count(); i++) {

                const QDomNode starlink = listeStarlink.at(i);
                if (!starlink.isNull()) {

                    const QString fichier = starlink.firstChildElement("Fichier").text();
                    const QString groupe = starlink.firstChildElement("Groupe").text();
                    const QString lancement = starlink.firstChildElement("Lancement").text();
                    const QString deploiement = starlink.firstChildElement("Deploiement").text();

                    // Recuperation des informations Starlink
                    if (!groupe.isEmpty()) {
                        mapSatellitesStarlink.insert(groupe, { fichier, lancement, deploiement });
                    }
                }
            }

            qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);
        }

    } catch (Exception const &e) {
        if (fi.exists()) {
            qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                                .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }
    }

    /* Retour */
    return mapSatellitesStarlink;
}

/*
 * Lecture du fichier de statut des satellites produisant des flashs
 */
QMap<QString, SatellitesFlashs> GestionnaireXml::LectureSatellitesFlashs()
{
    /* Declarations des variables locales */
    QMap<QString, SatellitesFlashs> mapFlashs;

    /* Initialisations */
    const QString nomficXml = "flares.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatFlashs") {
            throw Exception();
        }

        const QDomNodeList listeSatellites = root.elementsByTagName("Satellite");

        for(int i=0; i<listeSatellites.count(); i++) {

            const QDomNode sat = listeSatellites.at(i);
            const SatellitesFlashs satelliteFlashs = LectureStatutSatellitesFlashs(sat);

            if (!satelliteFlashs.nomsat.isEmpty()) {
                mapFlashs.insert(satelliteFlashs.norad, satelliteFlashs);
            }
        }

#if (!BUILD_TEST)
        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);
#endif

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapFlashs;
}

/*
 * Lecture des structures des frequences radio
 */
QList<FrequenceRadio> GestionnaireXml::LectureSatellitesFrequences(const QDomNode &sat)
{
    /* Declarations des variables locales */
    QList<FrequenceRadio> frequences;

    /* Initialisations */

    /* Corps de la methode */
    if (!sat.isNull()) {

        QString nom;
        QString balise;
        QString mode;
        QString signalAppel;
        QString frequencesDescendantes;
        QString frequencesMontantes;

        const QDomNodeList listeFrequences = sat.toElement().elementsByTagName("Frequences");

        for(int i=0; i<listeFrequences.count(); i++) {

            const QDomNode frequence = listeFrequences.at(i);

            nom = frequence.firstChildElement("Nom").text();
            balise = frequence.firstChildElement("Balise").text();
            mode = frequence.firstChildElement("Mode").text();
            signalAppel = frequence.firstChildElement("SignalAppel").text();

            frequencesDescendantes = frequence.firstChildElement("FrequenceDescendante").text().remove("*");
            frequencesMontantes = frequence.firstChildElement("FrequenceMontante").text().remove("*");

            if (!nom.isEmpty()) {
                frequences.append({ nom, frequencesMontantes, frequencesDescendantes, balise, mode, signalAppel });
            }
        }
    }

    /* Retour */
    return frequences;
}

/*
 * Lecture du fichier de satellites TDRS
 */
QMap<int, SatelliteTDRS> GestionnaireXml::LectureSatellitesTDRS()
{
    /* Declarations des variables locales */
    QMap<int, SatelliteTDRS> mapTDRS;

    /* Initialisations */
    const QString nomficXml = "tdrs.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatTDRS") {
            throw Exception();
        }

        const QDomNodeList listeTDRS = root.elementsByTagName("TDRS");

        for(int i=0; i<listeTDRS.count(); i++) {

            const QDomNode tdrs = listeTDRS.at(i);
            if (!tdrs.isNull()) {

                const int numero = tdrs.firstChildElement("Numero").text().toInt();
                const QString denomination = tdrs.firstChildElement("Denomination").text();
                const int rouge = tdrs.firstChildElement("CouleurR").text().toInt();
                const int vert = tdrs.firstChildElement("CouleurV").text().toInt();
                const int bleu = tdrs.firstChildElement("CouleurB").text().toInt();

                if (!denomination.isEmpty()) {
                    mapTDRS.insert(numero, { denomination, rouge, vert, bleu });
                }
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
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
    const QString nomficXml = "sites.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatSites") {
            throw Exception();
        }

        const QDomNodeList listeSites = root.elementsByTagName("Site");

        for(int i=0; i<listeSites.count(); i++) {

            const QDomNode site = listeSites.at(i);
            if (!site.isNull()) {

                const QString acronyme = site.firstChildElement("Acronyme").text();
                const QString description = site.firstChildElement("Description").text();
                const double longitude = site.firstChildElement("Longitude").text().toDouble();
                const double latitude = site.firstChildElement("Latitude").text().toDouble();

                if (!acronyme.isEmpty()) {
                    mapSitesLancement.insert(acronyme, Observateur(description, longitude, latitude));
                }
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
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
    const QString nomficXml = "stations.xml";

    try {

        VerifieVersionXml(nomficXml);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatStations") {
            throw Exception();
        }

        const QDomNodeList listeStations = root.elementsByTagName("Station");

        for(int i=0; i<listeStations.count(); i++) {

            const QDomNode station = listeStations.at(i);
            if (!station.isNull()) {

                const QString acronyme = station.firstChildElement("Acronyme").text();
                const QString nom = station.firstChildElement("Nom").text();
                const double longitude = station.firstChildElement("Longitude").text().toDouble();
                const double latitude = station.firstChildElement("Latitude").text().toDouble();
                const double altitude = station.firstChildElement("Altitude").text().toDouble();

                if (!acronyme.isEmpty()) {
                    mapStations.insert(acronyme, Observateur(nom, longitude, latitude, altitude));
                }
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return mapStations;
}

/*
 * Lecture des statuts des satellites produisant des flashs
 */
SatellitesFlashs GestionnaireXml::LectureStatutSatellitesFlashs(const QDomNode &sat)
{
    /* Declarations des variables locales */
    SatellitesFlashs satelliteFlashs;

    /* Initialisations */

    /* Corps de la methode */
    if (!sat.isNull()) {

        QList<QPair<double, double> > angles;

        const QString nom = sat.firstChildElement("Nom").text();
        const QString norad = sat.firstChildElement("Norad").text();

        const QDomNodeList listeAngles = sat.toElement().elementsByTagName("Angles");

        for(int i=0; i<listeAngles.count(); i++) {

            const QDomNode angle = listeAngles.at(i);
            const double yaw = angle.firstChildElement("Yaw").text().toDouble() * MATHS::DEG2RAD;
            const double pitch = angle.firstChildElement("Pitch").text().toDouble() * MATHS::DEG2RAD;

            angles.append({ yaw, pitch });
        }

        if (!norad.isEmpty()) {
            satelliteFlashs = { nom, norad, angles };
        }
    }

    /* Retour */
    return satelliteFlashs;
}

/*
 * Lecture du fichier de verrous
 */
QMap<QString, QDate> GestionnaireXml::LectureVerrous()
{
    /* Declarations des variables locales */
    QMap<QString, QDate> mapVerrous;

    /* Initialisations */

    /* Corps de la methode */
    try {

        FichierXml fi(Configuration::instance()->dirTmp() + QDir::separator() + "verrou.xml");

        if (!fi.exists()) {

            Telechargement tel(Configuration::instance()->dirTmp());
            tel.TelechargementFichier(QUrl(QString("%1maj/verrou.xml").arg(DOMAIN_NAME)), false);
        }

        // Lecture du fichier
        const QDomDocument document = fi.Ouverture(false);

        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatVerrous") {
            throw Exception();
        }

        const QDomNodeList elems = root.elementsByTagName("Verrou");

        for(int i=0; i<elems.count(); i++) {

            const QDomNode elem = elems.at(i);
            if (!elem.isNull()) {

                const QString fonction = elem.firstChildElement("Fonction").text();
                const QDate dateExpiration = QDate::fromString(elem.firstChildElement("DateExpiration").text(), Qt::ISODate);
                qInfo() << "Verrou" << fonction << dateExpiration.toString(Qt::ISODate);
                mapVerrous.insert(fonction, dateExpiration);
            }
        }

        if (fi.exists()) {
            fi.remove();
        }

    } catch (Exception const &e) {
    }

    /* Retour */
    return mapVerrous;
}

/*
 * Verification du numero de version du fichier xml
 */
void GestionnaireXml::VerifieVersionXml(const QString &nomfic, const QString &msg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        _observateurs.clear();
        FichierXml ficLocal(Configuration::instance()->dirCfg() + QDir::separator() + nomfic);
        FichierXml ficCommon(Configuration::instance()->dirCommonData() + QDir::separator() + "config" + QDir::separator() + nomfic);

        if (ficLocal.exists()) {

            ficCommon.Ouverture(false);
            const QDomDocument document = ficLocal.Ouverture(false);

            const QString versionNew = ficCommon.version();
            const QString version = ficLocal.version();

            if ((versionNew != version) && !msg.isEmpty()) {

                QMessageBox msgbox(QMessageBox::Question, QObject::tr("Avertissement"), msg);
                QPushButton * const oui = msgbox.addButton(QObject::tr("Oui"), QMessageBox::YesRole);
                msgbox.addButton(QObject::tr("Non"), QMessageBox::NoRole);
                msgbox.setDefaultButton(oui);
                msgbox.exec();

                if (msgbox.clickedButton() == oui) {

                    // Tentative de recuperation des lieux d'observation
                    qInfo() << "Tentative de recuperation des lieux d'observation";

                    const QDomNode obs = document.elementsByTagName("Observateurs").at(0);
                    _observateurs = LectureLieuxObservation(obs);

                    if (!_observateurs.isEmpty()) {
                        qInfo() << "Recuperation de" << _observateurs.size() << "lieux d'observation";
                    }
                }

                // Remplacement du fichier xml dans le repertoire local
                ficLocal.remove();
                ficCommon.copy(ficLocal.fileName());
            }
        } else {

            // Copie du fichier xml dans le repertoire local
            ficCommon.copy(ficLocal.fileName());
        }

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}
