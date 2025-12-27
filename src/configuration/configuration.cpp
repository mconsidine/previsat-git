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
 * >    16 decembre 2025
 *
 */

#include <QDir>
#include <QStandardPaths>
#include "configuration.h"
#include "evenementsstationspatiale.h"
#include "gestionnairexml.h"
#include "lancements.h"
#include "rentrees.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"


Configuration *Configuration::_instance = nullptr;


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
 * Chargement de la configuration generale
 */
void Configuration::Chargement()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "--";
        qInfo() << "Début Chargement Configuration";

        // Lecture du fichier de configuration generale
        LectureConfiguration();

        // Lecture du fichier de categories d'orbite
        _mapCategoriesOrbite = GestionnaireXml::LectureCategoriesOrbite();

        // Lecture du fichier de gestionnaire d'elements orbitaux
        _mapCategoriesElementsOrbitaux = GestionnaireXml::LectureGestionnaireElementsOrbitaux(_versionCategorieElem,
                                                                                              _mapCategoriesMajElementsOrbitaux);

        // Lecture du fichier listant les pays et organisations
        _mapPays = GestionnaireXml::LecturePays();

        // Lecture du fichier de satellites TDRS
        _mapTDRS = GestionnaireXml::LectureSatellitesTDRS();

        // Lecture du fichier des sites de lancement
        _mapSitesLancement = GestionnaireXml::LectureSitesLancement();

        // Lecture du fichier de stations
        _mapStations = GestionnaireXml::LectureStations();

        // Lecture du fichier de statut des satellites produisant des flashs
        _mapFlashs = GestionnaireXml::LectureSatellitesFlashs();

        // Lecture des frequences radio des satellites
        _mapFrequencesRadio = GestionnaireXml::LectureFrequencesRadio();

        // Lecture des noms des constellations
        _mapNomsConstellations = GestionnaireXml::LectureNomsConstellations();

        // Lecture du fichier NASA contenant les evenements de la Station Spatiale
        _evenementsStation = EvenementsStationSpatiale::LectureEvenementsStationSpatiale();

        // Lecture du fichier contenant les rentrees atmospheriques
        _rentreesAtmospheriques = Rentrees::LectureFichierRentrees();

        // Ouverture de la base de donnees satellites
        OuvertureBaseDonneesSatellites();

        // Lecture du fichier taiutc.dat
        Date::Initialisation(_dirLocalData);

        // Lecture du fichier de constellations
        Corps::Initialisation(_dirCommonData);

        // Initialisation du tableau d'etoiles
        _etoiles = Etoile::Initialisation(_dirCommonData);

        // Initialisation des tableaux de constellations
        _constellations = Constellation::Initialisation(_dirCommonData);
        LigneConstellation::Initialisation(_dirCommonData);

        _adresseCelestrakNorad = _adresseCelestrak + "NORAD/elements/gp.php?GROUP=%1&FORMAT=xml";
        _adresseCelestrakSupplementalNorad = _adresseCelestrak + "NORAD/elements/supplemental/";
        _adresseCelestrakSupplementalNoradFichier = _adresseCelestrakSupplementalNorad + "sup-gp.php?FILE=%1&FORMAT=xml";

        const QString httpDir = QString("%1data/").arg(DOMAIN_NAME);
        _mapAdressesTelechargement.insert(TypeTelechargement::COORDONNEES, httpDir + "coordinates/");
        _mapAdressesTelechargement.insert(TypeTelechargement::CARTES, httpDir + "map/");
        _mapAdressesTelechargement.insert(TypeTelechargement::NOTIFICATIONS, httpDir + "sound/");

        // Lecture du fichier de verrous
        _mapVerrous = GestionnaireXml::LectureVerrous();

        // Lecture du fichier contenant le calendrier des lancements
        _calendrierLancements = Lancements::LectureCalendrierLancements();

        // Lecture du fichier Pre-Launch Starlink
        _satellitesStarlink = GestionnaireXml::LecturePreLaunchStarlink();

        // Initialisation de la liste de fichiers d'elements orbitaux
        InitListeFichiersElem();

        // Initialisation de la liste de fichiers de cartes du monde
        InitListeFichiersMap();

        // Initialisation de la liste de fichiers de lieux d'observation
        InitListeFichiersObs();

        // Initialisation de la liste de fichiers de preferences
        InitListeFichiersPref();

        // Initialisation de la liste de fichiers de sons
        InitListeFichiersSon();

        // Ecriture d'informations dans le fichier de log
        qInfo().noquote() << QString("Lieu d'observation : %1 %2 %3")
                                 .arg(_observateurs.first().longitude() * MATHS::RAD2DEG, 0, 'f', 9)
                                 .arg(_observateurs.first().latitude() * MATHS::RAD2DEG, 0, 'f', 9)
                                 .arg(_observateurs.first().altitude() * 1.e3);

        qInfo().noquote() << "Nom du fichier d'éléments orbitaux :" << _nomfic;
        qInfo().noquote() << "Numéro NORAD par défaut :" << _noradDefaut;

        QListIterator it(_mapSatellitesFichierElem[_nomfic]);
        qInfo() << "Liste des numéros NORAD :";
        while (it.hasNext()) {
            qInfo().noquote() << "     " << it.next();
        }

        qInfo() << "Fin   Chargement Configuration";
        qInfo() << "--";

    } catch (Exception const &e) {
        qCritical() << "Erreur Chargement Configuration";
        throw Exception();
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
    QFile fi(Configuration::instance()->dirCfg() + QDir::separator() + "configuration.xml");

    if (!fi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        const QFileInfo ff(fi.fileName());
        qWarning().noquote() << QString("Erreur lors de l'écriture du fichier %1").arg(ff.fileName());
        throw Exception(QObject::tr("Erreur lors de l'écriture du fichier %1").arg(ff.fileName()), MessageType::WARNING);
    }

    QXmlStreamWriter cfg(&fi);

    cfg.setAutoFormatting(true);
    cfg.writeStartDocument();
    cfg.writeStartElement("PreviSatConfiguration");
    cfg.writeAttribute("version", _versionCfg);

    // Numero NORAD de la station spatiale
    cfg.writeTextElement("NoradStationSpatiale", _noradStationSpatiale);

    // Adresse du site Celestrak
    cfg.writeTextElement("AdresseCelestrak", _adresseCelestrak);

    // Adresse du site RocketLaunchLive
    cfg.writeTextElement("AdresseRocketLaunchLive", _adresseRocketLaunchLive);

    // Nom du fichier d'evenements de la Station Spatiale
    cfg.writeTextElement("NomFichierEvenementsStationSpatiale", _nomFichierEvenementsStationSpatiale);

    // Observateurs
    cfg.writeStartElement("Observateurs");
    QListIterator itObs(_observateurs);
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
    const QStringList listeNorad = _mapSatellitesFichierElem[_nomfic];
    if (!listeNorad.isEmpty()) {

        cfg.writeStartElement("Fichier");
        cfg.writeAttribute("nom", _nomfic);

        QStringListIterator itNorad(listeNorad);
        while (itNorad.hasNext()) {
            cfg.writeTextElement("Norad", QString("%1").arg(itNorad.next(), 6, '0'));
        }

        cfg.writeEndElement();
    }

    QMapIterator itElem(_mapSatellitesFichierElem);
    while (itElem.hasNext()) {

        itElem.next();

        if (!itElem.value().isEmpty() && (itElem.key() != _nomfic)) {

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

    /* Retour */
    return;
}

/*
 * Fermeture de la base de donnees satellites
 */
void Configuration::FermetureBaseDonneesSatellites()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomDb = _dbSatellites.connectionName();

    /* Corps de la methode */
    _dbSatellites = QSqlDatabase();
    _dbSatellites.removeDatabase(nomDb);

    /* Retour */
    return;
}

/*
 * Definition preliminaires pour le logiciel
 */
void Configuration::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Definition des arborescences
    DefinitionArborescences();

    // Determination de la locale et liste des langues disponibles
    DeterminationLocale();

    try {

        // Verification des arborescences
        VerificationArborescences();

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return;
}

/*
 * Initialisation de la liste de fichiers d'elements orbitaux
 */
void Configuration::InitListeFichiersElem()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirElem);
    const QStringList filtres(QStringList () << "*.xml" << "*.txt" << "*.tle");
    _listeFichiersElem = di.entryList(filtres, QDir::Files);

    /* Retour */
    return;
}

/*
 * Initialisation de la liste de fichiers de lieux d'observation
 */
void Configuration::InitListeFichiersObs()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirCoord);
    const QStringList filtres(QStringList () << "*.xml");
    _listeFicObs = di.entryList(filtres, QDir::Files);

    /* Retour */
    return;
}

/*
 * Verification de la date d'expiration d'une fonctionnalite
 */
bool Configuration::VerifieDateExpiration(const QString &fonction)
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool res = false;

    /* Corps de la methode */
    if (_mapVerrous.contains(fonction)) {
        res = (QDate::currentDate() < _mapVerrous[fonction]);
    }

    /* Retour */
    return res;
}


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

// Repertoires
const QString Configuration::dirBnd() const
{
    return _dirBnd;
}

const QString &Configuration::dirCfg() const
{
    return _dirCfg;
}

const QString &Configuration::dirCommonData() const
{
    return _dirCommonData;
}

const QString &Configuration::dirCoord() const
{
    return _dirCoord;
}

const QString &Configuration::dirDox() const
{
    return _dirDox;
}

const QString &Configuration::dirElem() const
{
    return _dirElem;
}

const QString &Configuration::dirExe() const
{
    return _dirExe;
}

const QString &Configuration::dirHtml() const
{
    return _dirHtml;
}

const QString &Configuration::dirLang() const
{
    return _dirLang;
}

const QString &Configuration::dirLocalData() const
{
    return _dirLocalData;
}

const QString &Configuration::dirLog() const
{
    return _dirLog;
}

const QString &Configuration::dirMap() const
{
    return _dirMap;
}

const QString &Configuration::dirOut() const
{
    return _dirOut;
}

const QString &Configuration::dirPref() const
{
    return _dirPref;
}

const QString &Configuration::dirRsc() const
{
    return _dirRsc;
}

const QString &Configuration::dirSon() const
{
    return _dirSon;
}

const QString &Configuration::dirStarlink() const
{
    return _dirStarlink;
}

const QString &Configuration::dirTmp() const
{
    return _dirTmp;
}


// Locale
QString &Configuration::locale()
{
    return _locale;
}

const QStringList &Configuration::listeFicLang() const
{
    return _listeFicLang;
}

const QStringList &Configuration::listeFicLocalData() const
{
    return _listeFicLocalData;
}

// Polices
QFont &Configuration::police()
{
    return _police;
}

QFont &Configuration::policeWcc()
{
    return _policeWcc;
}

bool &Configuration::isCarteMaximisee()
{
    return _isCarteMaximisee;
}

bool &Configuration::issLive()
{
    return _issLive;
}


const QString &Configuration::versionCfg() const
{
    return _versionCfg;
}

const QString &Configuration::versionCategorieElem() const
{
    return _versionCategorieElem;
}


const QString &Configuration::adresseCelestrak() const
{
    return _adresseCelestrak;
}

const QString &Configuration::adresseCelestrakNorad() const
{
    return _adresseCelestrakNorad;
}

const QString &Configuration::adresseCelestrakSupplementalNorad() const
{
    return _adresseCelestrakSupplementalNorad;
}

const QString &Configuration::adresseCelestrakSupplementalNoradFichier() const
{
    return _adresseCelestrakSupplementalNoradFichier;
}

const QString Configuration::adresseRocketLaunchLive() const
{
    return _adresseRocketLaunchLive;
}

const QMap<QString, QString> &Configuration::mapCategoriesOrbite() const
{
    return _mapCategoriesOrbite;
}


const QString &Configuration::nomFichierEvenementsStationSpatiale() const
{
    return _nomFichierEvenementsStationSpatiale;
}

const QString &Configuration::noradStationSpatiale() const
{
    return _noradStationSpatiale;
}

Observateur &Configuration::observateur()
{
    return _observateurs[0];
}


QList<Observateur> &Configuration::observateurs()
{
    return _observateurs;
}

QMap<QString, Observateur> &Configuration::mapObs()
{
    return _mapObs;
}


QMap<QString, QStringList> &Configuration::mapSatellitesFichierElem()
{
    return _mapSatellitesFichierElem;
}

Soleil &Configuration::soleil()
{
    return _soleil;
}

Lune &Configuration::lune()
{
    return _lune;
}

std::array<Planete, PLANETE::NB_PLANETES> &Configuration::planetes()
{
    return _planetes;
}

const QMap<TypeTelechargement, QString> &Configuration::mapAdressesTelechargement() const
{
    return _mapAdressesTelechargement;
}


QMap<QString, QList<CategorieElementsOrbitaux> > &Configuration::mapCategoriesElementsOrbitaux()
{
    return _mapCategoriesElementsOrbitaux;
}

QMap<QString, QList<CategorieElementsOrbitaux> > &Configuration::mapCategoriesMajElementsOrbitaux()
{
    return _mapCategoriesMajElementsOrbitaux;
}


const QMap<QString, QString> &Configuration::mapPays() const
{
    return _mapPays;
}

const QMap<int, SatelliteTDRS> &Configuration::mapTDRS() const
{
    return _mapTDRS;
}

const QMap<QString, Observateur> &Configuration::mapSitesLancement() const
{
    return _mapSitesLancement;
}

const QMap<QString, Observateur> &Configuration::mapStations() const
{
    return _mapStations;
}

const QMap<QString, SatellitesFlashs> &Configuration::mapFlashs() const
{
    return _mapFlashs;
}


QSqlDatabase Configuration::dbSatellites() const
{
    return _dbSatellites;
}

QString &Configuration::nomfic()
{
    return _nomfic;
}

QString &Configuration::noradDefaut()
{
    return _noradDefaut;
}

QStringList &Configuration::listeFichiersElem()
{
    return _listeFichiersElem;
}

void Configuration::AjoutSatelliteFichierElem(const QString &norad)
{
    if (!_mapSatellitesFichierElem[_nomfic].contains(norad)) {
        _mapSatellitesFichierElem[_nomfic].append(norad);
    }
}

void Configuration::SuppressionSatelliteFichierElem(const QString &norad)
{
    if (_mapSatellitesFichierElem[_nomfic].contains(norad)) {
        _mapSatellitesFichierElem[_nomfic].removeOne(norad);
    }
}

QMap<QString, QStringList> &Configuration::mapFichierElemNorad()
{
    return _mapFichierElemNorad;
}

QList<Satellite> &Configuration::listeSatellites()
{
    return _listeSatellites;
}

QMap<QString, ElementsOrbitaux> &Configuration::mapElementsOrbitaux()
{
    return _mapElementsOrbitaux;
}

EvenementsStation &Configuration::evenementsStation()
{
    return _evenementsStation;
}

QList<CalendrierLancements> &Configuration::calendrierLancements()
{
    return _calendrierLancements;
}

QList<RentreesAtmospheriques> &Configuration::rentreesAtmospheriques()
{
    return _rentreesAtmospheriques;
}

const QMap<QString, QList<FrequenceRadio> > &Configuration::mapFrequencesRadio() const
{
    return _mapFrequencesRadio;
}

QList<Etoile> &Configuration::etoiles()
{
    return _etoiles;
}

QList<Constellation> &Configuration::constellations()
{
    return _constellations;
}

QMap<QString, QMap<QString, QString> > Configuration::mapNomsConstellations() const
{
    return _mapNomsConstellations;
}

QList<LigneConstellation> &Configuration::lignesCst()
{
    return _lignesCst;
}

const QStringList &Configuration::listeFicMap() const
{
    return _listeFicMap;
}

const QStringList &Configuration::listeFicObs() const
{
    return _listeFicObs;
}

const QStringList &Configuration::listeFicPref() const
{
    return _listeFicPref;
}

const QStringList &Configuration::listeFicSon() const
{
    return _listeFicSon;
}

NotificationSonore &Configuration::notifAOS()
{
    return _notifAOS;
}

NotificationSonore &Configuration::notifFlashs()
{
    return _notifFlashs;
}


QMap<QString, SatellitesStarlink> &Configuration::satellitesStarlink()
{
    return _satellitesStarlink;
}

QMap<QString, QStringList> &Configuration::groupesStarlink()
{
    return _groupesStarlink;
}

void Configuration::AjoutDonneesSatellitesStarlink(const QString &groupe,
                                                   const QString &fichier,
                                                   const QString &lancement,
                                                   const QString &deploiement)
{
    if (!_satellitesStarlink.keys().contains(groupe)) {
        _satellitesStarlink.insert(groupe, { fichier, lancement, deploiement });
    }
}

QMap<QString, QDate> Configuration::mapVerrous() const
{
    return _mapVerrous;
}


/*
 * Modificateurs
 */
void Configuration::setObservateurDefaut(const int index)
{
    if ((index > 0) && (index < _observateurs.size())) {
        _observateurs.swapItemsAt(index, 0);
    }
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
    const QString dirAstr = QString(ORG_NAME) + QDir::separator() + APP_NAME;

    /* Corps de la methode */
    _dirExe = QCoreApplication::applicationDirPath();

#if (PORTABLE_BUILD)
    const QString dir = _dirExe + QDir::separator();
#else
    const QStringList listeGenericDir = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory);
    const QString dir = listeGenericDir.first() + dirAstr + QDir::separator();
#endif
    _dirLocalData = dir + "data";
    _dirElem = dir + "elem";
    _dirLog = dir + "log";
    _dirStarlink = dir + "starlink";

#if (PORTABLE_BUILD)
    _dirOut = _dirExe + QDir::separator() + dirAstr;
    _dirTmp = _dirExe + QDir::separator() + "cache";
#else
    _dirOut = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory) + dirAstr;
    _dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);
#endif

    if (_dirTmp.endsWith("/")) {
        _dirTmp.resize(_dirTmp.size() - 1);
    }

#if (PORTABLE_BUILD)
    const QString dirCommon = _dirExe;
#else

#if defined (Q_OS_WIN)
    const QString dirCommon = listeGenericDir.at(1) + dirAstr;
#elif defined (Q_OS_LINUX)
    const QString dirCommon = QString("/usr/share") + QDir::separator() + dirAstr;
#elif defined (Q_OS_MAC)
    const QString dirCommon = _dirExe;
    _dirLocalData = dirCommon + QDir::separator() + "data";
    _dirElem = _dirExe + QDir::separator() + "elem";
    _dirLog = _dirExe + QDir::separator() + "log";
    _dirOut = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory) + APP_NAME;
#endif
#endif

    if (_dirTmp.trimmed().isEmpty()) {
        _dirTmp = _dirLocalData.mid(0, _dirLocalData.lastIndexOf(QDir::separator())) + QDir::separator() + "cache";
    }

    _dirOut = QDir::toNativeSeparators(_dirOut);
    _dirElem = QDir::toNativeSeparators(_dirElem);

    // Autres repertoires
    _dirCommonData = dirCommon + QDir::separator() + "data";
    _dirBnd = _dirCommonData + QDir::separator() + "boundaries";
    _dirDox = _dirExe + QDir::separator() + "dox";
    _dirCfg = _dirLocalData + QDir::separator() + "config";
    _dirCoord = _dirLocalData + QDir::separator() + "coordinates";
    _dirHtml = _dirLocalData + QDir::separator() + "html";
    _dirLang = _dirExe + QDir::separator() + "translations";
    _dirMap = _dirLocalData + QDir::separator() + "map";
    _dirPref = _dirLocalData + QDir::separator() + "preferences";
    _dirRsc = _dirLocalData + QDir::separator() + "resources";
    _dirSon = _dirLocalData + QDir::separator() + "sound";

    /* Retour */
    return;
}

/*
 * Determination de la locale et liste des langues disponibles
 */
void Configuration::DeterminationLocale()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDir di(_dirLang);
    const QStringList filtres(QStringList () << QString(APP_NAME) + "_*.qm");

    /* Corps de la methode */
    _locale = QLocale::system().name().section('_', 0, 0);
    _listeFicLang = di.entryList(filtres, QDir::Files).replaceInStrings(QString(APP_NAME) + "_", "").replaceInStrings(".qm", "");

    if (!_listeFicLang.contains("fr")) {
        _listeFicLang.insert(0, "fr");
    }

    const QFile fi(di.path() + QDir::separator() + APP_NAME + "_" + _locale + ".qm");
    if (_dirLang.isEmpty() || !di.exists() || (!fi.exists() && (_locale != "fr"))) {

        _locale = QLocale(QLocale::English, QLocale::UnitedStates).name().section('_', 0, 0);

        if (!_listeFicLang.contains(_locale)) {
            _listeFicLang.append(_locale);
        }
    }

    /* Retour */
    return;
}

void Configuration::InitListeFichiersMap()
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
 * Initialisation de la liste de fichiers de preferences
 */
void Configuration::InitListeFichiersPref()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirPref);
    const QStringList filtres(QStringList () << "*.prf");
    _listeFicPref = di.entryList(filtres, QDir::Files);
    _listeFicPref.insert(0, "defaut");

    /* Retour */
    return;
}

/*
 * Initialisation de la liste de fichiers de sons
 */
void Configuration::InitListeFichiersSon()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di(_dirSon);
    const QStringList filtres(QStringList () << "aos-*.wav");

    if (di.entryList(filtres, QDir::Files).isEmpty()) {

        // Copie des fichiers sons par defaut
        QFile fi;
        const QString dirCommonSon = _dirCommonData + QDir::separator() + "sound";
        const QDir di2(dirCommonSon);

        foreach(const QString &fic, di2.entryList(QStringList () << "*.wav", QDir::Files)) {
            fi.setFileName(dirCommonSon + QDir::separator() + fic);
            fi.copy(_dirSon + QDir::separator() + fic);
        }
    }

    _listeFicSon = di.entryList(filtres, QDir::Files).replaceInStrings("aos-", "").replaceInStrings(".wav", "");

    /* Retour */
    return;
}

/*
 * Lecture du fichier de configuration generale
 */
void Configuration::LectureConfiguration()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomficXml = "configuration.xml";

    try {

        const QString msg = QObject::tr("Le fichier de configuration de %1 a évolué.\n" \
                                        "Souhaitez-vous tenter de récupérer les lieux d'observation ?").arg(APP_NAME);
        GestionnaireXml::VerifieVersionXml(nomficXml, msg);

        FichierXml fi(Configuration::instance()->dirCfg() + QDir::separator() + nomficXml);
        const QDomDocument document = fi.Ouverture(false);
        _versionCfg = fi.version();

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatConfiguration") {
            throw Exception();
        }

        _noradStationSpatiale = root.firstChildElement("NoradStationSpatiale").text();
        _adresseCelestrak = root.firstChildElement("AdresseCelestrak").text();
        _adresseRocketLaunchLive = root.firstChildElement("AdresseRocketLaunchLive").text();
        _nomFichierEvenementsStationSpatiale = root.firstChildElement("NomFichierEvenementsStationSpatiale").text();

        // Lieux d'observation
        const QDomNode listeObservateurs = root.elementsByTagName("Observateurs").at(0);
        _observateurs = (GestionnaireXml::observateurs().isEmpty()) ? GestionnaireXml::LectureLieuxObservation(listeObservateurs) :
                            GestionnaireXml::observateurs();

        // Elements orbitaux
        QStringList listeNorad;
        const QDomNodeList listeFichiers = root.elementsByTagName("Fichier");

        for(int i=0; i<listeFichiers.count(); i++) {

            listeNorad.clear();
            const QDomNode fichier = listeFichiers.at(i);
            const QDomElement fic = fichier.toElement();
            const QString nom = fic.attribute("nom");

            const QDomNodeList norads = fic.elementsByTagName("Norad");

            for(int j=0; j<norads.count(); j++) {
                listeNorad.append(QString("%1").arg(norads.at(j).toElement().text(), 6, '0'));
            }

            _mapSatellitesFichierElem.insert(nom, listeNorad);

            if (_nomfic.isEmpty()) {
                _nomfic = nom;
                _noradDefaut = listeNorad.first();
            }
        }

        if (_nomfic.isEmpty()) {
            _nomfic = "visual.xml";
        }

        if (_noradDefaut.isEmpty()) {
            _noradDefaut = _noradStationSpatiale;
        }

        if (_nomFichierEvenementsStationSpatiale.isEmpty()) {
            _nomFichierEvenementsStationSpatiale = "ISS.OEM_J2K_EPH.xml";
        }

        if (_mapSatellitesFichierElem.isEmpty()) {
            const QStringList elem(QStringList() << _noradStationSpatiale << "20580");
            _mapSatellitesFichierElem.insert(_nomfic, elem);
        }

        if (_noradStationSpatiale.isEmpty() || _observateurs.isEmpty()) {

            qCritical().noquote() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
            throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                                .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
        }

        qInfo().noquote() << QString("Lecture fichier %1 OK").arg(nomficXml);

    } catch (Exception const &e) {
        qCritical().noquote() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(nomficXml).arg(APP_NAME);
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2")
                            .arg(nomficXml).arg(APP_NAME), MessageType::ERREUR);
    }

    /* Retour */
    return;
}

/*
 * OuvertureBaseDonneesSatellites Ouverture de la base de donnees satellites
 */
void Configuration::OuvertureBaseDonneesSatellites()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fichierDb = _dirLocalData + QDir::separator() + "satellites.db";

    /* Corps de la methode */
    // Verification de l'existence du fichier
    QFile fi(fichierDb);
    const QFileInfo ff(fi.fileName());

    if (!fi.exists() || (fi.size() == 0)) {
        qCritical().noquote() << QString("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME);
        throw Exception(QObject::tr("Le fichier %1 n'existe pas ou est vide, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME),
                        MessageType::ERREUR);
    }

    // Ouverture de la base de donnees
    _dbSatellites = QSqlDatabase::addDatabase("QSQLITE");
    _dbSatellites.setDatabaseName(fichierDb);

    if (!_dbSatellites.open()) {
        qCritical().noquote() << "Erreur lors de l'ouverture de la base de données satellites : " + _dbSatellites.lastError().text();
        throw Exception(QObject::tr("Erreur lors de l'ouverture de la base de données satellites, veuillez réinstaller %1").arg(APP_NAME),
                        MessageType::ERREUR);
    }

#if (!BUILD_TEST)
    qInfo() << "Ouverture base de données satellites.db OK";
#endif

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
                                                     << _dirCommonData + QDir::separator() + "preferences");

        foreach(QString orig, listeDirOrig) {
            QDir dir(orig);
            const QString dest = orig.replace(_dirCommonData, _dirLocalData);
            if (!QDir(dest).exists()) {
                dir.rename(dir.path(), dest);
            }
        }

        QDir dirElm(_dirExe + QDir::separator() + "elem");
        if (!QDir(_dirElem).exists()) {
            dirElm.rename(dirElm.path(), _dirElem);
        }

        const QStringList listeFics(QStringList ()
                                    << _dirCommonData + QDir::separator() + "satellites.db"
                                    << _dirCommonData + QDir::separator() + _nomFichierEvenementsStationSpatiale
                                    << _dirCommonData + QDir::separator() + "radio.xml"
                                    << _dirCommonData + QDir::separator() + "taiutc.dat");

        foreach(QString fic, listeFics) {

            const QString file(fic);
            const QString dest = fic.replace(_dirCommonData, _dirLocalData);

            QFileInfo ff(dest);
            if (!ff.exists()) {
                QFile fi(file);
                fi.copy(dest);
            }
        }
#endif

        // Copie des fichiers de lieux d'observation
        QDir dirCrd(_dirCoord);
        if (!dirCrd.exists()) {
            dirCrd.mkpath(_dirCoord);
        }

        const QDir dirCmnCrd(_dirCommonData + QDir::separator() + "coordinates");
        const QStringList filtres(QStringList () << "*.xml");
        const QStringList listeCoord = dirCmnCrd.entryList(filtres, QDir::Files);

        foreach(const QString &fic, listeCoord) {

            const QString file = dirCmnCrd.absolutePath() + QDir::separator() + fic;
            const QString dest = _dirCoord + QDir::separator() + fic;

            QFileInfo ff(dest);
            if (!ff.exists()) {
                QFile fi(file);
                fi.copy(dest);
            }
        }

        // Verification et creation des arborescences
        foreach(const QString &dirDat, listeDirDat) {
            const QDir dir(dirDat);
            if (!dir.exists()) {
                qCritical().noquote() << QString("Le répertoire %1 n'existe pas, veuillez réinstaller %2").arg(QDir::toNativeSeparators(dirDat)).arg(APP_NAME);
                throw Exception(
                    QObject::tr("Le répertoire %1 n'existe pas, veuillez réinstaller %2").arg(QDir::toNativeSeparators(dirDat)).arg(APP_NAME), MessageType::ERREUR);
            }
        }

        const QStringList listeDir(QStringList ()
#if defined (Q_OS_LINUX)
                                   << _dirElem
#endif
                                   << _dirCfg << _dirLog << _dirMap << _dirOut << _dirPref << _dirRsc << _dirSon << _dirStarlink << _dirTmp);

        foreach(const QString &dir, listeDir) {
            const QDir direc = QDir(dir);
            if (!direc.exists()) {
                direc.mkpath(dir);
            }
        }

        // Verification de la presence des fichiers du repertoire data
        // Fichiers du repertoire data commun
        const QString repPref = QString("preferences") + QDir::separator();
        const QString repSon = QString("sound") + QDir::separator();
        const QString repStr = QString("stars") + QDir::separator();
        const QStringList ficCommonData(QStringList () << repPref + "defaut" << repSon + "aos-default.wav" << repSon + "los-default.wav"
                                                      << repStr + "constellations.dat" << repStr + "constlabel.dat"
                                                      << repStr + "constlines.dat" << repStr + "etoiles.dat");

        VerifieFichiersData(_dirCommonData, ficCommonData);

        // Copie des fichiers du repertoire commun vers le repertoire local
        const QStringList listeFics(QStringList () << _dirCommonData + QDir::separator() + repPref + "defaut");

        foreach(QString fic, listeFics) {

            const QString file(fic);
            const QString dest = fic.replace(_dirCommonData, _dirLocalData);

            QFileInfo ff(dest);
            if (!ff.exists()) {
                QFile fi(file);
                fi.copy(dest);
            }
        }

        // Fichiers du repertoire data local
        const QString repHtm = QString("html") + QDir::separator();
        _listeFicLocalData << "satellites.db" << repHtm + "meteo.map" << repHtm + "meteoNASA.html" << repHtm + "resultat.map" << "taiutc.dat";

        VerifieFichiersData(_dirLocalData, _listeFicLocalData);

        // Fichiers non obligatoires pour le fonctionnement de PreviSat
        _listeFicLocalData << _nomFichierEvenementsStationSpatiale << "radio.xml";

    } catch (Exception const &e) {
        throw Exception();
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
        const QFileInfo ff(fi.fileName());

        // Le fichier n'existe pas
        if (!fi.exists()) {
            qCritical().noquote() << QString("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME);
            throw Exception(QObject::tr("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            qCritical().noquote() << QString("Le fichier %1 est vide, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME);
            throw Exception(QObject::tr("Le fichier %1 est vide, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }
    }

    /* Retour */
    return;
}
