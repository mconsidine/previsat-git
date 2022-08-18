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

#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QCoreApplication>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QDir>
#include <QSettings>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QStandardPaths>
#include "configuration.h"
#include "evenementsstationspatiale.h"
#include "gestionnairexml.h"
#include "librairies/corps/corps.h"
#include "librairies/dates/date.h"
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
 * Methodes publiques
 */
/*
 * Definition du repertoire dirLog
 */
void Configuration::DefinitionDirLog()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString dirAstr = QString(ORG_NAME) + QDir::separator() + APP_NAME;

    /* Corps de la methode */
    const QStringList listeGenericDir = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory);
    const QString dir = listeGenericDir.at(0) + dirAstr + QDir::separator();
    _dirLog = dir + "log";

    /* Retour */
    return;
}

/*
 * Initialisation de la configuration generale
 */
void Configuration::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "--";
        qInfo() << "Début Initialisation Configuration";

        // Definition des arborescences
        DefinitionArborescences();

        // Verification des arborescences
        VerificationArborescences();

        // Determination de la locale et liste des langues disponibles
        DeterminationLocale();

        // Lecture du fichier de configuration generale
        GestionnaireXml::LectureConfiguration(_nomFichierEvenementsStationSpatiale,
                                              _noradStationSpatiale,
                                              _versionCfg,
                                              _adresseCelestrak,
                                              _nomfic,
                                              _noradDefaut,
                                              _observateurs,
                                              _mapSatellitesFichierElem);

        // Lecture du fichier de categories d'orbite
        _mapCategoriesOrbite = GestionnaireXml::LectureCategoriesOrbite();

        // Lecture du fichier de gestionnaire d'elements orbitaux
        _listeCategoriesElementsOrbitaux = GestionnaireXml::LectureGestionnaireElementsOrbitaux(_versionCategorieElem);

        // Lecture du fichier listant les pays et organisations
        _mapPays = GestionnaireXml::LecturePays();

        // Lecture du fichier de satellites TDRS
        _mapTDRS = GestionnaireXml::LectureSatellitesTDRS();

        // Lecture du fichier des sites de lancement
        _mapSitesLancement = GestionnaireXml::LectureSitesLancement();

        // Lecture du fichier de stations
        _mapStations = GestionnaireXml::LectureStations();

        // Lecture du fichier de statut des satellites produisant des flashs
        _mapFlashs = GestionnaireXml::LectureStatutSatellitesFlashs();

        // Lecture du fichier NASA contenant les evenements de la Station Spatiale
        EvenementsStationSpatiale::LectureEvenementsStationSpatiale(_dateDebutStationSpatiale,
                                                                    _dateFinStationSpatiale,
                                                                    _masseStationSpatiale,
                                                                    _surfaceTraineeAtmospherique,
                                                                    _coefficientTraineeAtmospherique,
                                                                    _evenementsStationSpatiale);

        // Lecture du fichier de donnees satellites
        LectureDonneesSatellites();

        // Lecture du fichier des chaines NASA
        LectureChainesNasa();

        // Lecture du fichier taiutc.dat
        Date::Initialisation(_dirLocalData);

        // Lecture du fichier de constellations
        Corps::Initialisation(_dirCommonData);

        const QString httpDir = QString("%1%2/Qt/commun/data/").arg(DOMAIN_NAME).arg(QString(APP_NAME).toLower());
        _mapAdressesTelechargement.insert(AdressesTelechargement::COORDONNEES, httpDir + "coordinates/");
        _mapAdressesTelechargement.insert(AdressesTelechargement::CARTES, httpDir + "map/");
        _mapAdressesTelechargement.insert(AdressesTelechargement::NOTIFICATIONS, httpDir + "sound/");

        // Initialisation de la liste de fichiers d'elements orbitaux
        InitListeFichiersElem();

        // Initialisation de la liste de fichiers de cartes du monde
        InitListeFichiersMap();

        // Initialisation de la liste de fichiers de preferences
        InitListeFichiersPref();

        qInfo() << QString("Lieu d'observation : %1 %2 %3")
                   .arg(_observateurs.at(0).longitude() * RAD2DEG, 0, 'f', 9)
                   .arg(_observateurs.at(0).latitude() * RAD2DEG, 0, 'f', 9)
                   .arg(_observateurs.at(0).altitude() * 1.e3);

        qInfo() << "Nom du fichier d'éléments orbitaux :" << _nomfic;
        qInfo() << "Numéro NORAD par défaut :" << _noradDefaut;

        QListIterator it(_mapSatellitesFichierElem[_nomfic]);
        qInfo() << "Liste des numéros NORAD :";
        while (it.hasNext()) {
            qInfo() << "     " << it.next();
        }

        qInfo() << "Fin   Initialisation Configuration";
        qInfo() << "--";

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation Configuration";
        throw PreviSatException();
    }

    /* Retour */
    return;
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

const QString &Configuration::dirTmp() const
{
    return _dirTmp;
}


// Locale
const QString &Configuration::locale() const
{
    return _locale;
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


const QString &Configuration::nomFichierEvenementsStationSpatiale() const
{
    return _nomFichierEvenementsStationSpatiale;
}

const QString &Configuration::noradStationSpatiale() const
{
    return _noradStationSpatiale;
}


const QList<Observateur> &Configuration::observateurs() const
{
    return _observateurs;
}

const QMap<QString, Observateur> &Configuration::mapObs() const
{
    return _mapObs;
}


const QMap<QString, QStringList> &Configuration::mapSatellitesFichierElem() const
{
    return _mapSatellitesFichierElem;
}

const QList<CategorieElementsOrbitaux> &Configuration::listeCategoriesElementsOrbitaux() const
{
    return _listeCategoriesElementsOrbitaux;
}

const QString &Configuration::donneesSatellites() const
{
    return _donneesSatellites;
}

int Configuration::lgRec() const
{
    return _lgRec;
}

const QString &Configuration::nomfic() const
{
    return _nomfic;
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
 * Definition des arborescences
 */
void Configuration::DefinitionArborescences()
{
    /* Declarations des variables locales */
    QString dirCommon;

    /* Initialisations */
    const QString dirAstr = QString(ORG_NAME) + QDir::separator() + APP_NAME;

    /* Corps de la methode */
    _dirExe = QCoreApplication::applicationDirPath();

    const QStringList listeGenericDir = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory);
    const QString dir = listeGenericDir.at(0) + dirAstr + QDir::separator();
    _dirLocalData = dir + "data";
    _dirElem = dir + "elem";

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
    _dirElem = _dirExe + QDir::separator() + "elem";
    _dirLog = _dirExe + QDir::separator() + "log";
    _dirOut = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory) + APP_NAME;
#endif


    if (_dirTmp.trimmed().isEmpty()) {
        _dirTmp = _dirLocalData.mid(0, _dirLocalData.lastIndexOf(QDir::separator())) + QDir::separator() + "cache";
    }

    _dirOut = QDir::toNativeSeparators(_dirOut);
    _dirElem = QDir::toNativeSeparators(_dirElem);

    // Autres repertoires
    _dirCommonData = dirCommon + QDir::separator() + "data";
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
    _locale = QLocale::system().name().section('_', 0, 0);
    const QDir di(_dirLang);
    const QStringList filtres(QStringList () << QString(APP_NAME) + "_*.qm");
    _listeFicLang = di.entryList(filtres, QDir::Files).replaceInStrings(QString(APP_NAME) + "_", "").replaceInStrings(".qm", "");
    _listeFicLang.insert(0, "fr");

    /* Corps de la methode */
    const QFile fi(di.path() + QDir::separator() + APP_NAME + "_" + _locale + ".qm");
    if (!fi.exists() && (_locale != "fr")) {
        _locale = QLocale(QLocale::English, QLocale::UnitedStates).name().section('_', 0, 0);
    }

    qInfo() << "Locale :" << _locale;

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

        qInfo() << "Lecture fichier chaines.chnl OK";

    } else {
        qCritical() << "Lecture fichier chaines.chnl KO";
        throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(fic).arg(APP_NAME), MessageType::ERREUR);
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

        qInfo() << "Lecture fichier donnees.bin OK";

    } else {
        qCritical() << "Lecture fichier donnees.bin KO";
        throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(fic).arg(APP_NAME), MessageType::ERREUR);
    }

    _lgRec = static_cast<int> ((_donneesSatellites.isEmpty()) ? -1 : _donneesSatellites.size() / _donneesSatellites.count('\n'));

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
                                       << _dirCommonData + QDir::separator() + ".." + QDir::separator() + "elem");

        foreach(QString orig, listeDirOrig) {
            QDir dir(orig);
            const QString dest = orig.replace(_dirCommonData, _dirLocalData);
            dir.rename(dir.path(), dest);
        }

        const QStringList listeFics(QStringList () << _dirCommonData + QDir::separator() + "donnees.bin"
                                    << _dirCommonData + QDir::separator() + _nomFichierEvenementsStationSpatiale
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

                qCritical() << QString("Le répertoire %1 n'existe pas").arg(dirDat);
                throw PreviSatException(message.arg(QDir::toNativeSeparators(dirDat)).arg(APP_NAME), MessageType::ERREUR);
            }
        }

        const QStringList listeDir(QStringList () << _dirCfg << _dirElem << _dirLog << _dirMap << _dirOut << _dirPref << _dirRsc << _dirSon << _dirTmp);
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

            qCritical() << QString("Le fichier %1 n'existe pas").arg(ff.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            const QString message = QObject::tr("Le fichier %1 est vide, veuillez réinstaller %2");
            const QFileInfo ff(fi.fileName());

            qCritical() << QString("Le fichier %1 est vide").arg(ff.fileName());
            throw PreviSatException(message.arg(ff.fileName()).arg(APP_NAME), MessageType::ERREUR);
        }
    }

    /* Retour */
    return;
}
