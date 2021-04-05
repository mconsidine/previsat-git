/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    configuration.cpp
 *
 * Localisation
 * >    configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 decembre 2019
 *
 * Date de revision
 * >
 *
 */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QLocale>
#include <QSettings>
#pragma GCC diagnostic warning "-Wconversion"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "configuration.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"


// Registre
static QSettings settings("Astropedia", "PreviSat");

Configuration *Configuration::_instance = nullptr;

bool Configuration::_isCarteMonde;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Accesseurs
 */
Configuration *Configuration::instance()
{
    if (_instance == nullptr) {
        _instance = new Configuration();
    }
    return _instance;
}

QString Configuration::dirExe() const
{
    return _dirExe;
}

QString Configuration::dirCoord() const
{
    return _dirCoord;
}

QString Configuration::dirCommonData() const
{
    return _dirCommonData;
}

QString Configuration::dirLocalData() const
{
    return _dirLocalData;
}

QString Configuration::dirMap() const
{
    return _dirMap;
}

QString Configuration::dirOut() const
{
    return _dirOut;
}

QString Configuration::dirPrf() const
{
    return _dirPrf;
}

QString Configuration::dirRsc() const
{
    return _dirRsc;
}

QString Configuration::dirSon() const
{
    return _dirSon;
}

QString Configuration::dirTle() const
{
    return _dirTle;
}

QString Configuration::dirTmp() const
{
    return _dirTmp;
}

QString Configuration::locale() const
{
    return _locale;
}

QFont Configuration::police() const
{
    return _police;
}

QString Configuration::adresseAstropedia() const
{
    return _adresseAstropedia;
}

QString Configuration::adresseCelestrak() const
{
    return "http://www.celestrak.com/";
}

QString Configuration::adresseCelestrakNorad() const
{
    return adresseCelestrak() + "NORAD/elements/";
}

QStringList Configuration::listeFicLocalData() const
{
    return _listeFicLocalData;
}

QStringList &Configuration::listeFicObs()
{
    return _listeFicObs;
}

QStringList Configuration::listeFicPref() const
{
    return _listeFicPref;
}

QStringList Configuration::listeFicTLE() const
{
    return _listeFicTLE;
}

QMap<QString, TLE> Configuration::mapTLE() const
{
    return _mapTLE;
}

QString &Configuration::nomfic()
{
    return _nomfic;
}

TLEdefaut Configuration::tleDefaut() const
{
    return _tleDefaut;
}

QList<Satellite> &Configuration::listeSatellites()
{
    return _listeSatellites;
}

QList<Observateur> &Configuration::observateurs()
{
    return _observateurs;
}

Observateur &Configuration::observateur()
{
    return _observateurs[0];
}

QMap<QString, QStringList> &Configuration::mapSatellitesFicTLE()
{
    return _mapSatellitesFicTLE;
}

Soleil &Configuration::soleil()
{
    return _soleil;
}

Lune &Configuration::lune()
{
    return _lune;
}

QList<Planete> &Configuration::planetes()
{
    return _planetes;
}

QList<Etoile> &Configuration::etoiles()
{
    return _etoiles;
}

QList<Constellation> &Configuration::constellations()
{
    return _constellations;
}

QList<LigneConstellation> &Configuration::lignesCst()
{
    return _lignesCst;
}

QMap<QString, QString> Configuration::mapCategories() const
{
    return _mapCategories;
}

QMap<QString, QString> Configuration::mapPays() const
{
    return _mapPays;
}

QMap<QString, Observateur> Configuration::mapSites() const
{
    return _mapSites;
}

QMap<QString, SatellitesFlashs> Configuration::mapFlashs() const
{
    return _mapFlashs;
}

QString Configuration::noradStationSpatiale() const
{
    return _noradStationSpatiale;
}

bool Configuration::isCarteMonde() const
{
    return _isCarteMonde;
}

QString Configuration::dateDebut() const
{
    return _dateDebut;
}

QList<double> Configuration::masseISS() const
{
    return _masseISS;
}

QStringList Configuration::evenementsISS() const
{
    return _evenementsISS;
}

QList<PositionISS> Configuration::positionsISS() const
{
    return _positionsISS;
}


/*
 * Modificateurs
 */
void Configuration::setPolice(const QFont &p)
{
    _police = p;
}

void Configuration::setListeFicTLE(const QStringList &listeFic)
{
    _listeFicTLE = listeFic;
}

void Configuration::setMapTLE(const QMap<QString, TLE> &map)
{
    _mapTLE = map;
}

void Configuration::setNomfic(const QString &nom)
{
    _nomfic = nom;
}

void Configuration::ajoutObservateur(const Observateur &obs)
{
    bool ajout = true;
    QListIterator<Observateur> it(_observateurs);
    while (it.hasNext() && ajout) {
        if (it.next().nomlieu().trimmed().toLower() == obs.nomlieu().trimmed().toLower()) {
            ajout = false;
        }
    }
    if (ajout) {
        _observateurs.append(obs);
        EcritureConfiguration();
    }
}

void Configuration::suppressionObservateur(const Observateur &obs)
{
    int i = 0;
    bool suppression = false;

    QListIterator<Observateur> it(_observateurs);
    while (it.hasNext() && !suppression) {
        if (it.next().nomlieu().trimmed().toLower() == obs.nomlieu().trimmed().toLower()) {
            _observateurs.removeAt(i);
            suppression = true;
            EcritureConfiguration();
        }
        i++;
    }
}

void Configuration::setObservateurDefaut(const int index)
{
    if ((index != 0) && (index < _observateurs.size())) {
#if QT_VERSION < 0x050D00
        _observateurs.swap(index, 0);
#else
        _observateurs.swapItemsAt(index, 0);
#endif
    }
}

void Configuration::setObservateurs(const QList<Observateur> &obs)
{
    _observateurs = obs;
}

void Configuration::ajoutSatelliteFicTLE(const QString &norad)
{
    if (!_mapSatellitesFicTLE[_nomfic].contains(norad)) {
        _mapSatellitesFicTLE[_nomfic].append(norad);
    }
}

void Configuration::suppressionSatelliteFicTLE(const QString &norad)
{
    if (_mapSatellitesFicTLE[_nomfic].contains(norad)) {
        _mapSatellitesFicTLE[_nomfic].removeOne(norad);
    }
}

void Configuration::setIsCarteMonde(bool isCarteMonde)
{
    _isCarteMonde = isCarteMonde;
}


/*
 * Methodes publiques
 */
/*
 * Initialisation de la configuration generale
 */
void Configuration::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Determination de la locale
        DeterminationLocale();

        // Definition des arborescences
        DefinitionArborescences();

        // Verification des arborescences
        VerificationArborescences();

        // Lecture du fichier de categories d'orbite
        LectureCategoriesOrbite();

        // Lecture du fichier listant les pays ou organisations
        LecturePays();

        // Lecture du fichier des sites de lancement
        LectureSitesLancement();

        // Lecture du fichier de statut des satellites produisant des flashs
        LectureStatutSatellitesFlashs();

        // Lecture de la configuration
        LectureConfiguration();

        // Fichiers de preferences
        InitFicPref();

        // Fichiers TLE
        InitFicTLE();

        // Lecture du fichier taiutc.dat
        Date::Initialisation(_dirLocalData);

        // Lecture du fichier NASA des positions ISS
        LecturePositionsISS();

        // Lecture du fichier de constellations
        Corps::InitTabConstellations(_dirCommonData);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Ecriture de la configuration
 */
void Configuration::EcritureConfiguration()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(_dirCfg + QDir::separator() + "configuration.xml");

    fi.open(QIODevice::WriteOnly | QIODevice::Text);
    QXmlStreamWriter cfg(&fi);

    cfg.setAutoFormatting(true);
    cfg.writeStartDocument();
    cfg.writeStartElement("PreviSatConfiguration");
    cfg.writeAttribute("version", _versionCfg);

    // Numero NORAD de la station spatiale
    cfg.writeTextElement("NoradStationSpatiale", _noradStationSpatiale);

    // Observateurs
    cfg.writeStartElement("Observateurs");
    QListIterator<Observateur> itObs(_observateurs);
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

    // Listes de satellites selon le fichier TLE
    cfg.writeStartElement("FichiersTLE");
    QMapIterator<QString, QStringList> itTLE(_mapSatellitesFicTLE);
    while (itTLE.hasNext()) {
        itTLE.next();

        cfg.writeStartElement("Fichier");
        cfg.writeAttribute("nom", itTLE.key());

        QStringListIterator itNorad(itTLE.value());
        while (itNorad.hasNext()) {
            cfg.writeTextElement("TLE", itNorad.next());
        }
        cfg.writeEndElement();
    }
    cfg.writeEndElement();

    cfg.writeEndElement();
    cfg.writeEndDocument();
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
/*
 * Definition des arborescences
 */
void Configuration::DefinitionArborescences()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();

    /* Corps de la methode */
    _dirExe = QCoreApplication::applicationDirPath();
    _adresseAstropedia = QCoreApplication::organizationDomain();

#if QT_VERSION >= 0x050000

    const QStringList listeGenericDir = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory);
    const QString dir = listeGenericDir.at(0) + dirAstr + QDir::separator();
    _dirLocalData = dir + "data";
    _dirTle = dir + "tle";

    _dirOut = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory) + dirAstr;
    _dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);

#if defined (Q_OS_WIN)
    _dirCommonData = listeGenericDir.at(1) + dirAstr + QDir::separator() + "data";
#elif defined (Q_OS_LINUX)
    _dirCommonData = listeGenericDir.at(2) + dirAstr + QDir::separator() + "data";
#endif

#else

    const QString listeGenericDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    _dirLocalData = listeGenericDir + QDir::separator() + "data";

    _dirOut = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + dirAstr;
    _dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    _dirTle = QDir::toNativeSeparators(listeGenericDir + QDir::separator() + "tle");

#if defined (Q_OS_WIN)
    _dirCommonData = QDir::rootPath() + "ProgramData" + QDir::separator() + dirAstr + QDir::separator() + "data";
#elif defined (Q_OS_LINUX)
    _dirCommonData = QString("/usr/share") + QDir::separator() + dirAstr + QDir::separator() + "data";
#endif

#endif

    // Cas particulier de Mac OS X
#if defined (Q_OS_MAC)
    _dirCommonData = _dirExe + QDir::separator() + "data";
    _dirLocalData = _dirCommonData;
    _dirTle = _dirExe + QDir::separator() + "tle";
    _adresseAstropedia = "http://astropedia.free.fr/";

#if QT_VERSION >= 0x050000
    _dirOut = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory) + QCoreApplication::applicationName();
#else
    _dirOut = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QDir::separator() + QCoreApplication::applicationName();
#endif
#endif


    if (_dirTmp.trimmed().isEmpty()) {
        _dirTmp = _dirLocalData.mid(0, _dirLocalData.lastIndexOf(QDir::separator())) + QDir::separator() + "cache";
    }

    // Autres repertoires
    _dirCoord = _dirLocalData + QDir::separator() + "coordinates";
    _dirCfg = _dirLocalData + QDir::separator() + "config";
    _dirMap = _dirLocalData + QDir::separator() + "map";
    _dirPrf = _dirLocalData + QDir::separator() + "preferences";
    _dirRsc = _dirLocalData + QDir::separator() + "resources";
    _dirSon = _dirLocalData + QDir::separator() + "sound";
    _dirOut = QDir::toNativeSeparators(_dirOut);

    Telechargement::setDirTmp(_dirTmp);

    /* Retour */
    return;
}

/*
 * Determination de la locale
 */
void Configuration::DeterminationLocale()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _locale = QLocale::system().name().section('_', 0, 0);

    /* Corps de la methode */
    const QFile fi(QCoreApplication::applicationDirPath() + QDir::separator() + QCoreApplication::applicationName() + "_" + _locale + ".qm");
    if (!fi.exists() && (_locale != "fr")) {
        _locale = QLocale(QLocale::English, QLocale::UnitedStates).name().section('_', 0, 0);
    }

    /* Retour */
    return;
}

/*
 * Fichiers de preferences
 */
void Configuration::InitFicPref()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirPrf);
    const QStringList filtres(QStringList () << "*.prf");
    _listeFicPref = di.entryList(filtres, QDir::Files);
    _listeFicPref.insert(0, "defaut");

    /* Retour */
    return;
}

/*
 * Fichiers TLE
 */
void Configuration::InitFicTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirTle);
    const QStringList filtres(QStringList () << "*.txt" << "*.tle");
    _listeFicTLE = di.entryList(filtres, QDir::Files);

    // TLE par defaut
    _nomfic = settings.value("fichier/nom", _dirTle + QDir::separator() + "visual.txt").toString();
    _tleDefaut.nomsat = settings.value("TLE/nom", "").toString();
    _tleDefaut.l1 = settings.value("TLE/l1", "").toString();
    _tleDefaut.l2 = settings.value("TLE/l2", "").toString();

    /* Retour */
    return;
}

/*
 * Lecture de la configuration
 */
void Configuration::LectureConfiguration()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + "configuration.xml");

#if !defined (Q_OS_MAC)

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    QFile fi2(_dirCommonData + QDir::separator() + "config" + QDir::separator() + "configuration.xml");

    if (fi1.exists()) {

        const QString msg = QObject::tr("Le fichier de configuration de %1 a évolué.\n"
                            "Certaines informations de configuration "
                            "(par exemple les lieux d'observation sélectionnés) seront perdues.").arg(QCoreApplication::applicationName());

        VerifieVersionXml(fi1, fi2, _versionCfg, msg);

    } else {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi1.remove();
            fi2.copy(fi1.fileName());

        } else {
            const QString message = QObject::tr("Le fichier %1 n'existe pas :\nUtilisation de la configuration par défaut");
            const QFileInfo ff(fi1.fileName());
            Message::Afficher(message.arg(ff.fileName()), WARNING);
        }
    }
    fi2.close();
    fi1.close();

#endif

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    if (fi1.exists()) {

        QXmlStreamReader cfg(&fi1);

        cfg.readNextStartElement();
        if (cfg.name() == "PreviSatConfiguration") {

            QString nom;
            QStringList elements;

            double lon;
            double lat;
            double alt;

            while (cfg.readNextStartElement()) {

                if (cfg.name() == "NoradStationSpatiale") {

                    _noradStationSpatiale = cfg.readElementText();

                } else if (cfg.name() == "Observateurs") {

                    while (cfg.readNextStartElement()) {

                        if (cfg.name() == "Observateur") {

                            nom = "";
                            lon = 0.;
                            lat = 0.;
                            alt = 0.;

                            while (cfg.readNextStartElement()) {

                                if (cfg.name() == "Nom") {
                                    nom = cfg.readElementText();
                                } else if (cfg.name() == "Longitude") {
                                    lon = cfg.readElementText().toDouble();
                                } else if (cfg.name() == "Latitude") {
                                    lat = cfg.readElementText().toDouble();
                                } else if (cfg.name() == "Altitude") {
                                    alt = cfg.readElementText().toDouble();
                                } else {
                                    cfg.skipCurrentElement();
                                }
                            }
                            _observateurs.append(Observateur(nom, lon, lat, alt));

                        } else {
                            cfg.skipCurrentElement();
                        }
                    }
                } else if (cfg.name() == "FichiersTLE") {

                    while (cfg.readNextStartElement()) {

                        nom = "";
                        if ((cfg.name() == "Fichier") && (cfg.attributes().hasAttribute("nom"))) {
                            nom = cfg.attributes().value("nom").toString();
                        } else {
                            cfg.skipCurrentElement();
                        }

                        elements.clear();
                        while (cfg.readNextStartElement()) {
                            if (cfg.name() == "TLE") {
                                elements.append(cfg.readElementText());
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        if (!_mapSatellitesFicTLE.contains(nom)) {
                            _mapSatellitesFicTLE.insert(nom, elements);
                        }
                    }

                } else {
                    cfg.skipCurrentElement();
                }
            }
        }
    }
    fi1.close();

    // Verifications
    if (_observateurs.isEmpty()) {
        _observateurs.append(Observateur("Paris", -002.348640000, +48.853390000, 30));
    }

    if (_mapSatellitesFicTLE.isEmpty()) {
        _mapSatellitesFicTLE.insert("visual.txt", QStringList() << "25544" << "20580");
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de categories d'orbite
 */
void Configuration::LectureCategoriesOrbite()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + "categories.xml");

#if !defined (Q_OS_MAC)

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    QFile fi2(_dirCommonData + QDir::separator() + "config" + QDir::separator() + "categories.xml");

    if (fi1.exists()) {
        VerifieVersionXml(fi1, fi2, version);
    } else {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());

        } else {
            const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                                "Le fichier %1 n'existe pas, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
        }
    }
    fi2.close();
    fi1.close();

#endif

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    if (fi1.exists()) {

        QXmlStreamReader cfg(&fi1);

        cfg.readNextStartElement();
        if (cfg.name() == "PreviSatCategories") {

            QString acronyme;
            QString desc;

            while (cfg.readNextStartElement()) {

                if (cfg.name() == "Categorie") {

                    while (cfg.readNextStartElement()) {

                        acronyme = "";
                        desc = "";

                        if (cfg.name() == "Acronyme") {
                            acronyme = cfg.readElementText();
                        } else if (cfg.name() == "Description") {
                            desc = cfg.readElementText();
                        } else {
                            cfg.skipCurrentElement();
                        }
                    }
                    _mapCategories.insert(acronyme, desc);
                }
            }
        }
    }
    fi1.close();

    // Verifications
    if (_mapCategories.isEmpty()) {
        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucune catégorie d'orbite n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier listant les pays ou organisations
 */
void Configuration::LecturePays()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + "pays.xml");

#if !defined (Q_OS_MAC)

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    QFile fi2(_dirCommonData + QDir::separator() + "config" + QDir::separator() + "pays.xml");

    if (fi1.exists()) {
        VerifieVersionXml(fi1, fi2, version);
    } else {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());

        } else {
            const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                                "Le fichier %1 n'existe pas, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
        }
    }
    fi2.close();
    fi1.close();

#endif

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    if (fi1.exists()) {

        QXmlStreamReader cfg(&fi1);

        cfg.readNextStartElement();
        if (cfg.name() == "PreviSatPays") {

            QString acronyme;
            QString desc;

            while (cfg.readNextStartElement()) {

                if (cfg.name() == "Pays") {

                    while (cfg.readNextStartElement()) {

                        acronyme = "";
                        desc = "";

                        if (cfg.name() == "Acronyme") {
                            acronyme = cfg.readElementText();
                        } else if (cfg.name() == "Description") {
                            desc = cfg.readElementText();
                        } else {
                            cfg.skipCurrentElement();
                        }
                    }
                    _mapPays.insert(acronyme, desc);
                }
            }
        }
    }
    fi1.close();

    // Verifications
    if (_mapPays.isEmpty()) {
        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun pays ou organisation n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier NASA contenant les positions de l'ISS
 */
void Configuration::LecturePositionsISS()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirTmp + QDir::separator() + "ISS.OEM_J2K_EPH.xml");

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    if (fi1.exists()) {

        QXmlStreamReader cfg(&fi1);

        cfg.readNextStartElement();
        if (cfg.name().toString().toLower() == "ndm") {

            double x = 0.;
            double y = 0.;
            double z = 0.;
            double vx = 0.;
            double vy = 0.;
            double vz = 0.;
            QString epoque;
            QString version;
            QString value;
            PositionISS position;

            while (cfg.readNextStartElement()) {

                if (cfg.name().toString().toLower() == "oem") {

                    version = cfg.attributes().value("version").toString();

                    while (cfg.readNextStartElement()) {

                        if (cfg.name().toString().toLower() == "body") {

                            while (cfg.readNextStartElement()) {

                                if (cfg.name().toString().toLower() == "segment") {

                                    while (cfg.readNextStartElement()) {

                                        if (cfg.name().toString().toLower() == "metadata") {

                                            while (cfg.readNextStartElement()) {

                                                if (cfg.name().toString().toLower() == "start_time") {

                                                    // Date de debut
                                                    _dateDebut = cfg.readElementText();

                                                } else if (cfg.name().toString().toLower() == "stop_time") {

                                                    // Date de fin
                                                    _dateFin = cfg.readElementText();

                                                } else {
                                                    cfg.skipCurrentElement();
                                                }
                                            }

                                        } else if (cfg.name().toString().toLower() == "data") {

                                            while (cfg.readNextStartElement()) {

                                                value = cfg.readElementText();

                                                if (value.toLower().contains("mass")) {

                                                    // Masse (en kg)
                                                    _masseISS.append(value.split("=").last().toDouble()); 

                                                } else if (value.contains("===")) {

                                                    // Recuperation des evenements
                                                    value = "";
                                                    while (cfg.readNextStartElement() && !value.contains("===")) {

                                                        value = cfg.readElementText();
                                                        if (!value.contains("===") && !value.isEmpty() && !value.contains("(")) {
                                                            value.replace(26, 1, "T");
                                                            _evenementsISS.append(value);
                                                        }
                                                    }
                                                }
                                            }

                                            // Recuperation des positions
                                            while (cfg.readNextStartElement()) {

                                                if (cfg.name().toString().toLower() == "statevector") {

                                                    while (cfg.readNextStartElement()) {

                                                        if (cfg.name().toString().toLower() == "epoch") {
                                                            epoque = cfg.readElementText();
                                                        } else if (cfg.name().toString().toLower() == "x") {
                                                            x = cfg.readElementText().toDouble();
                                                        } else if (cfg.name().toString().toLower() == "y") {
                                                            y = cfg.readElementText().toDouble();
                                                        } else if (cfg.name().toString().toLower() == "z") {
                                                            z = cfg.readElementText().toDouble();
                                                        } else if (cfg.name().toString().toLower() == "x_dot") {
                                                            vx = cfg.readElementText().toDouble();
                                                        } else if (cfg.name().toString().toLower() == "y_dot") {
                                                            vy = cfg.readElementText().toDouble();
                                                        } else if (cfg.name().toString().toLower() == "z_dot") {
                                                            vz = cfg.readElementText().toDouble();
                                                        } else {
                                                            cfg.skipCurrentElement();
                                                        }
                                                    }

                                                    const Vecteur3D pos(x, y, z);
                                                    const Vecteur3D vit(vx, vy, vz);

                                                    position.jourJulienUTC = Date::ConversionDateNasa(epoque).jourJulienUTC();
                                                    position.position = pos;
                                                    position.vitesse = vit;

                                                    _positionsISS.append(position);
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
                        } else {
                            cfg.skipCurrentElement();
                        }
                    }
                }
            }
        }
    }
    fi1.close();

    /* Retour */
    return;
}

/*
 * Lecture du fichier des sites de lancement
 */
void Configuration::LectureSitesLancement()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + "sites.xml");

#if !defined (Q_OS_MAC)

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    QFile fi2(_dirCommonData + QDir::separator() + "config" + QDir::separator() + "sites.xml");

    if (fi1.exists()) {
        VerifieVersionXml(fi1, fi2, version);
    } else {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());

        } else {
            const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                                "Le fichier %1 n'existe pas, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
        }
    }
    fi2.close();
    fi1.close();

#endif

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    if (fi1.exists()) {

        QXmlStreamReader cfg(&fi1);

        cfg.readNextStartElement();
        if (cfg.name() == "PreviSatSites") {

            QString acronyme;
            QString desc;
            double lon;
            double lat;

            while (cfg.readNextStartElement()) {

                if (cfg.name() == "Site") {

                    acronyme = "";
                    desc = "";
                    lon = 0.;
                    lat = 0.;

                    while (cfg.readNextStartElement()) {

                        if (cfg.name() == "Acronyme") {
                            acronyme = cfg.readElementText();
                        } else if (cfg.name() == "Description") {
                            desc = cfg.readElementText();
                        } else if (cfg.name() == "Longitude") {
                            lon = cfg.readElementText().toDouble();
                        } else if (cfg.name() == "Latitude") {
                            lat = cfg.readElementText().toDouble();
                        } else {
                            cfg.skipCurrentElement();
                        }
                    }
                    _mapSites.insert(acronyme, Observateur(desc, lon, lat));
                }
            }
        }
    }
    fi1.close();

    // Verifications
    if (_mapSites.isEmpty()) {
        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun site de lancement n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de statut des satellites produisant des flashs
 */
void Configuration::LectureStatutSatellitesFlashs()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + "flares.xml");

#if !defined (Q_OS_MAC)

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    QFile fi2(_dirCommonData + QDir::separator() + "config" + QDir::separator() + "flares.xml");

    if (fi1.exists()) {
        VerifieVersionXml(fi1, fi2, version);
    } else {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());

        } else {
            const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                                "Le fichier %1 n'existe pas, veuillez réinstaller %2");
            const QFileInfo ff(fi1.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
        }
    }
    fi2.close();
    fi1.close();

#endif

    fi1.open(QIODevice::ReadOnly | QIODevice::Text);
    if (fi1.exists()) {

        QXmlStreamReader cfg(&fi1);

        cfg.readNextStartElement();
        if (cfg.name() == "PreviSatFlashs") {

            QString norad;
            QPair<double, double> angles;
            SatellitesFlashs satelliteFlash;

            while (cfg.readNextStartElement()) {

                if (cfg.name() == "Satellite") {

                    norad = "";
                    satelliteFlash.angles.clear();

                    while (cfg.readNextStartElement()) {

                        if (cfg.name() == "Nom") {
                            satelliteFlash.nomsat = cfg.readElementText();
                        } else if (cfg.name() == "Norad") {
                            norad = cfg.readElementText();
                        } else if (cfg.name() == "Angles") {

                            while (cfg.readNextStartElement()) {

                                if (cfg.name() == "Yaw") {
                                    angles.first = cfg.readElementText().toDouble() * DEG2RAD;
                                } else if (cfg.name() == "Pitch") {
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
                    _mapFlashs.insert(norad, satelliteFlash);
                }
            }
        }
    }
    fi1.close();

    // Verifications
    if (_mapFlashs.isEmpty()) {
        const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                            "Aucun satellite produisant des flashs n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Verification des arborescences
 */
void Configuration::VerificationArborescences()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QStringList listeDirDat(QStringList () << _dirCommonData << _dirLocalData);

    /* Corps de la methode */
    try {

        // Verification et creation des arborescences
        foreach(const QString dirDat, listeDirDat) {
            const QDir di(dirDat);
            if (!di.exists()) {
                const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                                    "Le répertoire %1 n'existe pas, veuillez réinstaller %2");
                throw PreviSatException(message.arg(QDir::toNativeSeparators(dirDat)).arg(QCoreApplication::applicationName()), ERREUR);
            }
        }

        const QStringList listeDir(QStringList () << _dirCfg << _dirMap << _dirOut << _dirPrf << _dirRsc << _dirSon << _dirTle << _dirTmp);
        foreach(const QString dir, listeDir) {
            const QDir di = QDir(dir);
            if (!di.exists()) {
                di.mkpath(dir);
            }
        }

        // Verification de la presence des fichiers du repertoire data
        // Fichiers du repertoire data commun
        const QString repSon = QString("sound") + QDir::separator();
        const QString repStr = QString("stars") + QDir::separator();
        const QStringList ficCommonData(QStringList () << "gestionnaireTLE_" + _locale + ".xml" << repSon + "aos-default.wav"
                                        << repSon + "los-default.wav" << repStr + "constellations.dat" << repStr + "constlabel.dat"
                                        << repStr + "constlines.dat" << repStr + "etoiles.dat");

        VerifieFichiersData(_dirCommonData, ficCommonData);

        // Fichiers du repertoire data local
        const QString repFlr = QString("flares") + QDir::separator();
        const QString repHtm = QString("html") + QDir::separator();
        _listeFicLocalData << "donnees.sat" << repFlr + "flares.sts" << repHtm + "chaines.chnl" << repHtm + "meteo.map" << repHtm + "meteoNASA.html"
                           << repHtm + "resultat.map" << QString("preferences") + QDir::separator() + "defaut" << "stations.sta" << "taiutc.dat"
                           << "tdrs.sat";

        VerifieFichiersData(_dirLocalData, _listeFicLocalData);

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Verifie la presence des fichiers du repertoire data
 */
void Configuration::VerifieFichiersData(const QString &dirData, const QStringList &listeFicData) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QStringListIterator it(listeFicData);
    while (it.hasNext()) {

        const QFile fi(dirData + QDir::separator() + it.next());

        // Le fichier n'existe pas
        if (!fi.exists()) {
            const QString message = QObject::tr("Le fichier %1 n'existe pas, veuillez réinstaller %2");
            const QFileInfo ff(fi.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            const QString message = QObject::tr("Le fichier %1 est vide, veuillez réinstaller %2");
            const QFileInfo ff(fi.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
        }
    }

    /* Retour */
    return;
}

/*
 * Verification du numero de version du fichier xml
 */
void Configuration::VerifieVersionXml(QFile &fi1, QFile &fi2, QString &version, const QString &msg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (fi2.exists()) {

        QDomDocument doc;
        doc.setContent(&fi2);
        const QString versionNew = doc.documentElement().attribute("version");

        if (fi1.exists()) {

            // Comparaison des numeros de version du fichier xml
            doc.clear();
            doc.setContent(&fi1);
            version = doc.documentElement().attribute("version");
            fi1.seek(0);

            if (versionNew != _versionCfg) {

                if (!msg.isEmpty()) {
                    Message::Afficher(msg, WARNING);
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

    /* Retour */
    return;
}
