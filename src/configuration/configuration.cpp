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
 * >    18 aout 2022
 *
 */

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFileInfo>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QLocale>
#pragma GCC diagnostic warning "-Wconversion"
#include <QSettings>
#include <QStandardPaths>
#include <QXmlStreamWriter>
#include "configuration.h"
#include "librairies/exceptions/previsatexception.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);

Configuration *Configuration::_instance = nullptr;


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

QString Configuration::dirDox() const
{
    return _dirDox;
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

QString Configuration::dirLang() const
{
    return _dirLang;
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

QString &Configuration::locale()
{
    return _locale;
}

QStringList Configuration::listeFicLang() const
{
    return _listeFicLang;
}

QFont Configuration::police() const
{
    return _police;
}

QFont Configuration::policeWcc() const
{
    return _policeWcc;
}

QString Configuration::adresseAstropedia() const
{
    return _adresseAstropedia;
}

QString Configuration::adresseCelestrak() const
{
    return _adresseCelestrak;
}

QString Configuration::adresseCelestrakNorad() const
{
    return _adresseCelestrak + "NORAD/elements/";
}

QStringList Configuration::listeFicLocalData() const
{
    return _listeFicLocalData;
}

QStringList Configuration::listeFicMap() const
{
    return _listeFicMap;
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

TLEdefaut &Configuration::tleDefaut()
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

QMap<QString, QString> Configuration::mapCategoriesOrbite() const
{
    return _mapCategoriesOrbite;
}

QMap<QString, QString> Configuration::mapPays() const
{
    return _mapPays;
}

QMap<QString, Observateur> Configuration::mapSites() const
{
    return _mapSites;
}

QMap<QString, Observateur> Configuration::mapStations() const
{
    return _mapStations;
}

QMap<QString, Observateur> &Configuration::mapObs()
{
    return _mapObs;
}

QMap<int, SatelliteTDRS> Configuration::mapTDRS() const
{
    return _mapTDRS;
}

QMap<QString, SatellitesFlashs> Configuration::mapFlashs() const
{
    return _mapFlashs;
}

QString Configuration::noradStationSpatiale() const
{
    return _noradStationSpatiale;
}

QString Configuration::dateDebutISS() const
{
    return _dateDebutISS;
}

QList<double> Configuration::masseISS() const
{
    return _masseISS;
}

QStringList Configuration::evenementsISS() const
{
    return _evenementsISS;
}

QString Configuration::donneesSatellites() const
{
    return _donneesSatellites;
}

int Configuration::lgRec() const
{
    return _lgRec;
}

QList<CategorieTLE> &Configuration::listeCategoriesTLE()
{
    return _listeCategoriesTLE;
}

QList<CategorieTLE> &Configuration::listeCategoriesMajTLE()
{
    return _listeCategoriesMajTLE;
}

QMap<AdressesTelechargement, QString> Configuration::mapAdressesTelechargement() const
{
    return _mapAdressesTelechargement;
}

QStringList Configuration::listeChainesNasa() const
{
    return _listeChainesNasa;
}

bool &Configuration::issLive()
{
    return _issLive;
}

bool &Configuration::isCarteMonde()
{
    return _isCarteMonde;
}

bool &Configuration::isCarteMaximisee()
{
    return _isCarteMaximisee;
}

NotificationSonore &Configuration::notifAOS()
{
    return _notifAOS;
}

NotificationSonore &Configuration::notifFlashs()
{
    return _notifFlashs;
}


/*
 * Modificateurs
 */
void Configuration::setPolice(const QFont &p)
{
    _police = p;
}

void Configuration::setPoliceWcc(const QFont &p)
{
    _policeWcc = p;
}

void Configuration::setListeFicTLE(const QStringList &listeFic)
{
    _listeFicTLE = listeFic;
}

void Configuration::ajoutListeFicTLE(const QString &fic)
{
    _listeFicTLE.append(fic);
    _listeFicTLE.sort();
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
    const QFileInfo fi(_nomfic);
    const QString fic = _nomfic.contains(Configuration::instance()->dirTle()) ? fi.fileName() : _nomfic;

    if (!_mapSatellitesFicTLE[fic].contains(norad)) {
        _mapSatellitesFicTLE[fic].append(norad);
    }
}

void Configuration::suppressionSatelliteFicTLE(const QString &norad)
{
    const QFileInfo fi(_nomfic);
    const QString fic = _nomfic.contains(Configuration::instance()->dirTle()) ? fi.fileName() : _nomfic;

    if (_mapSatellitesFicTLE[fic].contains(norad)) {
        _mapSatellitesFicTLE[fic].removeOne(norad);
    }
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

        // Lecture du fichier de satellites TDRS
        LectureSatellitesTDRS();

        // Lecture du fichier des sites de lancement
        LectureSitesLancement();

        // Lecture du fichier de stations
        LectureStations();

        // Lecture du fichier de statut des satellites produisant des flashs
        LectureStatutSatellitesFlashs();

        // Lecture du fichier de donnees satellites
        LectureDonneesSatellites();

        // Lecture du fichier de gestionnaire de TLE
        LectureGestionnaireTLE();

        // Lecture de la configuration
        LectureConfiguration();

        // Lecture du fichier des chaines NASA
        LectureChainesNasa();

        // Cartes du monde
        InitFicMap();

        // Fichiers de preferences
        InitFicPref();

        // Fichiers TLE
        InitFicTLE();

        // Lecture du fichier taiutc.dat
        Date::Initialisation(_dirLocalData);

        // Lecture du fichier NASA des manoeuvres ISS
        LectureManoeuvresISS();

        // Lecture du fichier de constellations
        Corps::InitTabConstellations(_dirCommonData);

        const QString httpDir = QString("%1%2/Qt/commun/data/").arg(_adresseAstropedia).arg(QString(APP_NAME).toLower());
        _mapAdressesTelechargement.insert(COORDONNEES, httpDir + "coordinates/");
        _mapAdressesTelechargement.insert(CARTES, httpDir + "map/");
        _mapAdressesTelechargement.insert(NOTIFICATIONS, httpDir + "sound/");

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

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatConfiguration");
        cfg.writeAttribute("version", _versionCfg);

        // Numero NORAD de la station spatiale
        cfg.writeTextElement("NoradStationSpatiale", _noradStationSpatiale);

        // Adresse Celestrak
        cfg.writeTextElement("AdresseCelestrak", _adresseCelestrak);

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

            if (!itTLE.value().isEmpty()) {

                cfg.writeStartElement("Fichier");
                cfg.writeAttribute("nom", itTLE.key());

                QStringListIterator itNorad(itTLE.value());
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
 * Ecriture du fichier de lieu d'observation
 */
void Configuration::EcritureFicObs(const QString &ficObsXml)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(_dirCoord + QDir::separator() + ficObsXml);

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatObservateurs");

        // Observateurs
        QMapIterator<QString, Observateur> it(_mapObs);
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
 * Ecriture du fichier de gestionnaire des TLE
 */
void Configuration::EcritureGestionnaireTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(_dirCfg + QDir::separator() + "gestionnaireTLE.xml");

    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QXmlStreamWriter cfg(&fi);

        cfg.setAutoFormatting(true);
        cfg.writeStartDocument();
        cfg.writeStartElement("PreviSatGestionTLE");
        cfg.writeAttribute("version", _versionCategoriesTLE);

        // Categories
        QListIterator<CategorieTLE> it(_listeCategoriesTLE);
        while (it.hasNext()) {

            const CategorieTLE categorie = it.next();

            cfg.writeStartElement("Categorie");

            QMapIterator<QString, QString> it2(categorie.nom);
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
 * Lecture d'un fichier de lieux d'observation
 */
void Configuration::LectureFicObs(const QString &ficObsXml, const bool alarme)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _mapObs.clear();

    /* Corps de la methode */
    QFile fi1(_dirCoord + QDir::separator() + ficObsXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name() == "PreviSatObservateurs") {

                QString nom;
                double lon;
                double lat;
                double alt;

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

                        if (!nom.isEmpty()) {
                            _mapObs.insert(nom, Observateur(nom, lon, lat, alt));
                        }
                    } else {
                        cfg.skipCurrentElement();
                    }
                }
            } else {
                fi1.close();
                if (alarme) {
                    throw PreviSatException(QObject::tr("Le fichier ne contient pas de lieux d'observation"), WARNING);
                } else {
                    throw PreviSatException();
                }
            }
        }
        fi1.close();
    }

    // Verifications


    /* Retour */
    return;
}

/*
 * Lecture du fichier NASA contenant les manoeuvres de l'ISS
 */
void Configuration::LectureManoeuvresISS()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _masseISS.clear();
    _evenementsISS.clear();

    /* Corps de la methode */
    QFile fi1(_dirLocalData + QDir::separator() + "ISS.OEM_J2K_EPH.xml");

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

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
        fi1.close();
    }

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
    QString dirCommon;

    /* Initialisations */
    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();

    /* Corps de la methode */
    _dirExe = QCoreApplication::applicationDirPath();
    _adresseAstropedia = QCoreApplication::organizationDomain();

    const QStringList listeGenericDir = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory);
    const QString dir = listeGenericDir.at(0) + dirAstr + QDir::separator();
    _dirLocalData = dir + "data";
    _dirTle = dir + "tle";

    _dirOut = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory) + dirAstr;
    _dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);

    if (_dirTmp.endsWith("/")) {
        _dirTmp.resize(_dirTmp.size() - 1);
    }

#if defined (Q_OS_WIN)
    dirCommon = listeGenericDir.at(1) + dirAstr;
#elif defined (Q_OS_LINUX)
    dirCommon = ((listeGenericDir.at(2).contains("local")) ? listeGenericDir.at(3) : listeGenericDir.at(2)) + dirAstr;
#elif defined (Q_OS_MAC)
    dirCommon = _dirExe;
    _dirLocalData = dirCommon + QDir::separator() + "data";
    _dirTle = _dirExe + QDir::separator() + "tle";
    _adresseAstropedia = "http://astropedia.free.fr/";
    _dirOut = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory) + QCoreApplication::applicationName();
#endif


    if (_dirTmp.trimmed().isEmpty()) {
        _dirTmp = _dirLocalData.mid(0, _dirLocalData.lastIndexOf(QDir::separator())) + QDir::separator() + "cache";
    }

    _dirOut = QDir::toNativeSeparators(_dirOut);
    _dirTle = QDir::toNativeSeparators(_dirTle);

    // Autres repertoires
    _dirCommonData = dirCommon + QDir::separator() + "data";
    _dirDox = _dirExe + QDir::separator() + "dox";
    _dirLang = _dirExe + QDir::separator() + "translations";
    _dirCoord = _dirLocalData + QDir::separator() + "coordinates";
    _dirCfg = _dirLocalData + QDir::separator() + "config";
    _dirHtml = _dirLocalData + QDir::separator() + "html";
    _dirMap = _dirLocalData + QDir::separator() + "map";
    _dirPrf = _dirLocalData + QDir::separator() + "preferences";
    _dirRsc = _dirLocalData + QDir::separator() + "resources";
    _dirSon = _dirLocalData + QDir::separator() + "sound";

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
    const QDir di(QCoreApplication::applicationDirPath() + QDir::separator() + "translations");
    const QStringList filtres(QStringList () << QCoreApplication::applicationName() + "_*.qm");
    _listeFicLang = di.entryList(filtres, QDir::Files).replaceInStrings(QCoreApplication::applicationName() + "_", "").replaceInStrings(".qm", "");
    _listeFicLang.insert(0, "fr");

    /* Corps de la methode */
    const QFile fi(di.path() + QDir::separator() + QCoreApplication::applicationName() + "_" + _locale + ".qm");
    if (!fi.exists() && (_locale != "fr")) {
        _locale = QLocale(QLocale::English, QLocale::UnitedStates).name().section('_', 0, 0);
    }

    /* Retour */
    return;
}

/*
 * Cartes du monde
 */
void Configuration::InitFicMap()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirMap);
    const QStringList filtres(QStringList () << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png");
    _listeFicMap = di.entryList(filtres, QDir::Files);

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
    _nomfic = QDir::toNativeSeparators(settings.value("fichier/nom", _dirTle + QDir::separator() + "visual.txt").toString());
    _tleDefaut.nomsat = settings.value("TLE/nom", "").toString();
    _tleDefaut.l1 = settings.value("TLE/l1", "").toString();
    _tleDefaut.l2 = settings.value("TLE/l2", "").toString();

    /* Retour */
    return;
}

/*
 * Lecture de la section body du fichier ISS
 */
void Configuration::LectureBody(QXmlStreamReader &cfg)
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
 * Lecture de la configuration
 */
void Configuration::LectureConfiguration()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _observateurs.clear();
    _mapSatellitesFicTLE.clear();

    const QString nomficXml = "configuration.xml";
    const QString msg1 = QObject::tr("Le fichier de configuration de %1 a évolué.\n"
                                     "Certaines informations de configuration "
                                     "(par exemple les lieux d'observation sélectionnés) seront perdues.").arg(QCoreApplication::applicationName());
    const QString msg2 = QObject::tr("Le fichier %1 n'existe pas :\nUtilisation de la configuration par défaut");

    VerifieFichierXml(nomficXml, _versionCfg, WARNING, msg1, msg2);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

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

                    } else if (cfg.name() == "AdresseCelestrak") {

                        _adresseCelestrak = cfg.readElementText();

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

                                if (!nom.isEmpty()) {
                                    _observateurs.append(Observateur(nom, lon, lat, alt));
                                }

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
                                if (cfg.name() == "Norad") {
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
    }

    // Verifications
    if (_observateurs.isEmpty()) {
        _observateurs.append(Observateur("Paris", -2.348640000, +48.853390000, 30));
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
    _mapCategoriesOrbite.clear();

    const QString nomficXml = "categories.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name() == "PreviSatCategories") {

                QString acronyme;
                QString desc;

                while (cfg.readNextStartElement()) {

                    if (cfg.name() == "Categorie") {

                        acronyme = "";
                        desc = "";

                        while (cfg.readNextStartElement()) {

                            if (cfg.name() == "Acronyme") {
                                acronyme = cfg.readElementText();
                            } else if (cfg.name() == "Description") {
                                desc = cfg.readElementText();
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        if (!acronyme.isEmpty()) {
                            _mapCategoriesOrbite.insert(acronyme, desc);
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications
    if (_mapCategoriesOrbite.isEmpty()) {
        const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Aucune catégorie d'orbite n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(msg.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier des chaines NASA
 */
void Configuration::LectureChainesNasa()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fic = _dirHtml + QDir::separator() + "chaines.chnl";

    /* Corps de la methode */
    QFile fi(fic);

    if (fi.exists() && (fi.size() != 0)) {

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
            _listeChainesNasa = QString(fi.readAll()).split("\n", Qt::SkipEmptyParts);
        }
        fi.close();
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section data du fichier ISS
 */
void Configuration::LectureData(QXmlStreamReader &cfg)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString value;

    /* Corps de la methode */
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

    /* Retour */
    return;
}

/*
 * Lecture du fichier de donnees satellites
 */
void Configuration::LectureDonneesSatellites()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fic = _dirLocalData + QDir::separator() + "donnees.bin";
    _donneesSatellites = "";

    /* Corps de la methode */
    QFile fi(fic);

    if (fi.exists() && (fi.size() != 0)) {

        if (fi.open(QIODevice::ReadOnly)) {
            const QByteArray donneesCompressees = fi.readAll();
            _donneesSatellites = QString(qUncompress(donneesCompressees));
        }
        fi.close();
    }

    _lgRec = (_donneesSatellites.isEmpty()) ? -1 : _donneesSatellites.size() / _donneesSatellites.count('\n');

    /* Retour */
    return;
}

/*
 * Lecture du fichier de gestionnaire de TLE
 */
void Configuration::LectureGestionnaireTLE()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */
    _listeCategoriesTLE.clear();

    const QString nomficXml = "gestionnaireTLE.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name() == "PreviSatGestionTLE") {

                QString langue;
                QString nom;
                QString site;
                QStringList fichiers;
                QMap<QString, QString> nomCategorie;

                while (cfg.readNextStartElement()) {

                    fichiers.clear();
                    nomCategorie.clear();

                    if (cfg.name() == "Categorie") {

                        while (cfg.readNextStartElement()) {

                            if (cfg.name() == "Langue") {

                                // Nom de la categorie
                                if (cfg.attributes().hasAttribute("lang")) {
                                    langue = cfg.attributes().value("lang").toString();

                                    while (cfg.readNextStartElement()) {

                                        if (cfg.name() == "Nom") {
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

                            } else if (cfg.name() == "Site") {

                                // Site web
                                site = cfg.readElementText();

                            } else if (cfg.name() == "Fichiers") {

                                // Nom des fichiers de la categorie
                                while (cfg.readNextStartElement()) {

                                    if (cfg.name() == "Fichier") {
                                        fichiers.append(cfg.readElementText());
                                    } else {
                                        cfg.skipCurrentElement();
                                    }
                                }
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        QMapIterator<QString, QString> it(nomCategorie);
                        while (it.hasNext()) {
                            it.next();
                            nomCategorie[it.key()] += "@" + site;
                        }

                        if (!nomCategorie.isEmpty()) {
                            _listeCategoriesTLE.append({ nomCategorie, site, fichiers });
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications


    /* Retour */
    return;
}

/*
 * Lecture de la section data du fichier ISS
 */
void Configuration::LectureMetadata(QXmlStreamReader &cfg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (cfg.readNextStartElement()) {

        if (cfg.name().toString().toLower() == "start_time") {

            // Date de debut
            _dateDebutISS = cfg.readElementText();

        } else if (cfg.name().toString().toLower() == "stop_time") {

            // Date de fin
            _dateFinISS = cfg.readElementText();

        } else {
            cfg.skipCurrentElement();
        }
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
    _mapPays.clear();

    const QString nomficXml = "pays.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name() == "PreviSatPays") {

                QString acronyme;
                QString desc;

                while (cfg.readNextStartElement()) {

                    if (cfg.name() == "Pays") {

                        acronyme = "";
                        desc = "";

                        while (cfg.readNextStartElement()) {

                            if (cfg.name() == "Acronyme") {
                                acronyme = cfg.readElementText();
                            } else if (cfg.name() == "Description") {
                                desc = cfg.readElementText();
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        if (!acronyme.isEmpty()) {
                            _mapPays.insert(acronyme, desc);
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications
    if (_mapPays.isEmpty()) {
        const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Aucun pays ou organisation n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(msg.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de satellites TDRS
 */
void Configuration::LectureSatellitesTDRS()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */
    _mapTDRS.clear();

    const QString nomficXml = "tdrs.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name() == "PreviSatTDRS") {

                int numero;
                QString denomination;
                int rouge;
                int vert;
                int bleu;

                while (cfg.readNextStartElement()) {

                    if (cfg.name() == "TDRS") {

                        numero = 0;
                        denomination = "";
                        rouge = 0;
                        vert = 0;
                        bleu = 0;

                        while (cfg.readNextStartElement()) {

                            if (cfg.name() == "Numero") {
                                numero = cfg.readElementText().toInt();
                            } else if (cfg.name() == "Denomination") {
                                denomination = cfg.readElementText();
                            } else if (cfg.name() == "CouleurR") {
                                rouge = cfg.readElementText().toInt();
                            } else if (cfg.name() == "CouleurV") {
                                vert = cfg.readElementText().toInt();
                            } else if (cfg.name() == "CouleurB") {
                                bleu = cfg.readElementText().toInt();
                            } else {
                                cfg.skipCurrentElement();
                            }
                        }

                        if (!denomination.isEmpty()) {
                            _mapTDRS.insert(numero, { denomination, rouge, vert, bleu });
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications
    if (_mapTDRS.isEmpty()) {
        const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Aucun satellite TDRS n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(msg.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

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
    _mapSites.clear();

    const QString nomficXml = "sites.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

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

                        if (!acronyme.isEmpty()) {
                            _mapSites.insert(acronyme, Observateur(desc, lon, lat));
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications
    if (_mapSites.isEmpty()) {
        const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Aucun site de lancement n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(msg.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier de stations
 */
void Configuration::LectureStations()
{
    /* Declarations des variables locales */
    QString version;

    /* Initialisations */
    _mapStations.clear();

    const QString nomficXml = "stations.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader cfg(&fi1);

            cfg.readNextStartElement();
            if (cfg.name() == "PreviSatStations") {

                QString acronyme;
                QString nom;
                double lon;
                double lat;
                double alt;

                while (cfg.readNextStartElement()) {

                    if (cfg.name() == "Station") {

                        acronyme = "";
                        nom = "";
                        lon = 0.;
                        lat = 0.;
                        alt = 0.;

                        while (cfg.readNextStartElement()) {

                            if (cfg.name() == "Acronyme") {
                                acronyme = cfg.readElementText();
                            } else if (cfg.name() == "Nom") {
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

                        if (!acronyme.isEmpty()) {
                            _mapStations.insert(acronyme, Observateur(nom, lon, lat, alt));
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications
    if (_mapStations.isEmpty()) {
        const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Aucune station n'a été trouvée dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(msg.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
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
    _mapFlashs.clear();

    const QString nomficXml = "flares.xml";
    const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Le fichier %1 n'existe pas, veuillez réinstaller %2");

    VerifieFichierXml(nomficXml, version, ERREUR, "", message);

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

    if (fi1.exists() && (fi1.size() != 0)) {

        if (fi1.open(QIODevice::ReadOnly | QIODevice::Text)) {

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

                        if (!norad.isEmpty()) {
                            _mapFlashs.insert(norad, satelliteFlash);
                        }
                    }
                }
            }
        }
        fi1.close();
    }

    // Verifications
    if (_mapFlashs.isEmpty()) {
        const QString msg = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                        "Aucun satellite produisant des flashs n'a été trouvé dans le fichier %1, veuillez réinstaller %2");
        const QFileInfo ff(fi1.fileName());
        throw PreviSatException(msg.arg(ff.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
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

#if defined (Q_OS_LINUX)
        const QDir di = QDir(_dirLocalData);
        if (!di.exists()) {
            di.mkpath(_dirLocalData);
        }

        const QStringList listeDirOrig(QStringList () << _dirCommonData + QDir::separator() + "coordinates"
                                       << _dirCommonData + QDir::separator() + "html"
                                       << _dirCommonData + QDir::separator() + "preferences"
                                       << _dirCommonData + QDir::separator() + ".." + QDir::separator() + "tle");

        foreach(QString orig, listeDirOrig) {
            QDir dir(orig);
            const QString dest = orig.replace(_dirCommonData, _dirLocalData);
            dir.rename(dir.path(), dest);
        }

        const QStringList listeFics(QStringList () << _dirCommonData + QDir::separator() + "donnees.bin"
                                    << _dirCommonData + QDir::separator() + "ISS.OEM_J2K_EPH.xml"
                                    << _dirCommonData + QDir::separator() + "taiutc.dat");

        foreach(QString fic, listeFics) {
            QString file(fic);
            const QString dest = fic.replace(_dirCommonData, _dirLocalData);
            QFile fi;
            fi.rename(file, dest);
            fi.remove(file);
        }
#endif

        // Verification et creation des arborescences
        foreach(const QString dirDat, listeDirDat) {
            const QDir dir(dirDat);
            if (!dir.exists()) {
                const QString message = QObject::tr("Erreur rencontrée lors de l'initialisation :\n" \
                                                    "Le répertoire %1 n'existe pas, veuillez réinstaller %2");
                throw PreviSatException(message.arg(QDir::toNativeSeparators(dirDat)).arg(QCoreApplication::applicationName()), ERREUR);
            }
        }

        const QStringList listeDir(QStringList () << _dirCfg << _dirMap << _dirOut << _dirPrf << _dirRsc << _dirSon << _dirTle << _dirTmp);
        foreach(const QString dir, listeDir) {
            const QDir direc = QDir(dir);
            if (!direc.exists()) {
                direc.mkpath(dir);
            }
        }

        // Verification de la presence des fichiers du repertoire data
        // Fichiers du repertoire data commun
        const QString repSon = QString("sound") + QDir::separator();
        const QString repStr = QString("stars") + QDir::separator();
        const QStringList ficCommonData(QStringList () << repSon + "aos-default.wav" << repSon + "los-default.wav"
                                        << repStr + "constellations.dat" << repStr + "constlabel.dat"
                                        << repStr + "constlines.dat" << repStr + "etoiles.dat");

        VerifieFichiersData(_dirCommonData, ficCommonData);

        // Fichiers du repertoire data local
        const QString repHtm = QString("html") + QDir::separator();
        _listeFicLocalData << "donnees.bin" << "ISS.OEM_J2K_EPH.xml" << repHtm + "chaines.chnl" << repHtm + "meteo.map" << repHtm + "meteoNASA.html"
                           << repHtm + "resultat.map" << QString("preferences") + QDir::separator() + "defaut" << "taiutc.dat";

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
 * Verification du fichier xml
 */
void Configuration::VerifieFichierXml(const QString &nomficXml, QString &version, const MessageType &typeMessage, const QString &message1,
                                      const QString &message2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi1(_dirCfg + QDir::separator() + nomficXml);

#if defined (Q_OS_MAC)
    if (!fi1.exists()) {
        const QFileInfo ff(fi1.fileName());
        Message::Afficher(message2.arg(ff.fileName()).arg(QCoreApplication::applicationName()), typeMessage);
    }
#else
    fi1.open(QIODevice::ReadOnly | QIODevice::Text);

    QFile fi2(_dirCommonData + QDir::separator() + "config" + QDir::separator() + nomficXml);

    if (!fi1.exists()) {

        if (fi2.exists()) {

            // Copie du fichier xml
            fi2.copy(fi1.fileName());

        } else {
            const QFileInfo ff(fi1.fileName());
            Message::Afficher(message2.arg(ff.fileName()).arg(QCoreApplication::applicationName()), typeMessage);
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

            if (versionNew != version) {

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
