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
 * >    configuration.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Configuration generale du logiciel
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 decembre 2019
 *
 * Date de revision
 * >    12 decembre 2021
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QFont>
#include <QMap>
#include <QString>
#include <QStringList>
#include "librairies/observateur/observateur.h"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/exceptions/message.h"


struct TLEdefaut
{
    QString nomsat;
    QString l1;
    QString l2;
};

struct SatellitesFlashs
{
    QString nomsat;
    QList<QPair<double, double> > angles;
};

struct SatelliteTDRS
{
    QString denomination;
    int rouge;
    int vert;
    int bleu;
};

struct PositionISS {
    double jourJulienUTC;
    Vecteur3D position;
    Vecteur3D vitesse;
};

struct CategorieTLE {
    QMap<QString, QString> nom;
    QString site;
    QStringList fichiers;
};

enum AdressesTelechargement {
    COORDONNEES = 0,
    CARTES,
    NOTIFICATIONS
};


class QFile;
class QXmlStreamReader;

class Configuration
{
#if (BUILD_TEST == true)
    friend class OngletsTest;
#endif

public:

    /*
     *  Constructeurs
     */
    Configuration(const Configuration &) = delete;

    /*
     * Accesseurs
     */
    static Configuration *instance();

    QString dirDox() const;
    QString dirExe() const;
    QString dirCoord() const;
    QString dirCommonData() const;
    QString dirLocalData() const;
    QString dirLang() const;
    QString dirMap() const;
    QString dirOut() const;
    QString dirPrf() const;
    QString dirRsc() const;
    QString dirSon() const;
    QString dirTle() const;
    QString dirTmp() const;

    QString &locale();
    QStringList listeFicLang() const;
    QFont police() const;
    QFont policeWcc() const;

    QString adresseAstropedia() const;
    QString adresseCelestrak() const;
    QString adresseCelestrakNorad() const;

    QStringList listeFicLocalData() const;
    QStringList listeFicMap() const;
    QStringList &listeFicObs();
    QStringList listeFicPref() const;
    QStringList listeFicTLE() const;

    QMap<QString, TLE> mapTLE() const;

    QString &nomfic();
    TLEdefaut &tleDefaut();

    QList<Satellite> &listeSatellites();

    QList<Observateur> &observateurs();
    Observateur &observateur();

    QMap<QString, QStringList> &mapSatellitesFicTLE();

    Soleil &soleil();
    Lune &lune();
    QList<Planete> &planetes();
    QList<Etoile> &etoiles();
    QList<Constellation> &constellations();
    QList<LigneConstellation> &lignesCst();

    QMap<QString, QString> mapCategoriesOrbite() const;
    QMap<QString, QString> mapPays() const;
    QMap<QString, Observateur> mapSites() const;
    QMap<QString, Observateur> mapStations() const;
    QMap<int, SatelliteTDRS> mapTDRS() const;
    QMap<QString, SatellitesFlashs> mapFlashs() const;

    QString noradStationSpatiale() const;

    bool isCarteMonde() const;

    QString dateDebutISS() const;
    QList<double> masseISS() const;
    QStringList evenementsISS() const;

    QString donneesSatellites() const;
    int lgRec() const;

    QList<CategorieTLE> &listeCategoriesTLE();
    QList<CategorieTLE> &listeCategoriesMajTLE();

    QMap<AdressesTelechargement, QString> mapAdressesTelechargement() const;

    QStringList listeChainesNasa() const;

    bool &issLive();


    /*
     * Modificateurs
     */
    void setPolice(const QFont &p);
    void setPoliceWcc(const QFont &p);

    void setListeFicTLE(const QStringList &listeFic);
    void ajoutListeFicTLE(const QString &fic);
    void setMapTLE(const QMap<QString, TLE> &map);
    void setNomfic(const QString &nom);

    void ajoutObservateur(const Observateur &obs);
    void suppressionObservateur(const Observateur &obs);
    void setObservateurDefaut(const int index);
    void setObservateurs(const QList<Observateur> &obs);

    void ajoutSatelliteFicTLE(const QString &norad);
    void suppressionSatelliteFicTLE(const QString &norad);

    void setIsCarteMonde(bool isCarteMonde);

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    Configuration& operator = (const Configuration &) = delete;

    /**
     * @brief Initialisation Initialisation de la configuration generale
     */
    void Initialisation();

    /**
     * @brief EcritureConfiguration Ecriture de la configuration
     */
    void EcritureConfiguration();

    /**
     * @brief EcritureGestionnaireTLE Ecriture du fichier de gestionnaire des TLE
     */
    void EcritureGestionnaireTLE();

    /**
     * @brief LectureManoeuvresISS Lecture du fichier NASA contenant les manoeuvres de l'ISS
     */
    void LectureManoeuvresISS();


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     *  Constructeurs
     */
    /**
     * @brief Configuration Creation du singleton
     */
    Configuration() {
    }

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    static Configuration *_instance;

    QString _versionCfg;

    // Repertoires
    QString _dirDox;
    QString _dirExe;
    QString _dirCoord;
    QString _dirCommonData;
    QString _dirLocalData;
    QString _dirCfg;
    QString _dirHtml;
    QString _dirLang;
    QString _dirMap;
    QString _dirOut;
    QString _dirPrf;
    QString _dirRsc;
    QString _dirSon;
    QString _dirTle;
    QString _dirTmp;

    // Locale
    QString _locale;
    QStringList _listeFicLang;

    // Adresses internet
    QString _adresseAstropedia;

    // Police
    QFont _police;
    QFont _policeWcc;

    // Fichiers du repertoire data local
    QStringList _listeFicLocalData;

    // Fichiers de preferences
    QStringList _listeFicPref;

    // Fichiers de lieux d'observation
    QStringList _listeFicObs;

    // Liste des lieux d'observation selectionnes
    QList<Observateur> _observateurs;

    // Listes de satellites selon le fichier TLE
    QMap<QString, QStringList> _mapSatellitesFicTLE;

    // Liste des fichiers TLE
    QStringList _listeFicTLE;

    // Liste des cartes du monde
    QStringList _listeFicMap;

    // Map des TLE d'un fichier
    QMap<QString, TLE> _mapTLE;

    QList<Satellite> _listeSatellites;

    // Categories d'orbite
    QMap<QString, QString> _mapCategoriesOrbite;

    // Pays ou organisations
    QMap<QString, QString> _mapPays;

    // Satellites TDRS
    QMap<int, SatelliteTDRS> _mapTDRS;

    // Sites de lancement
    QMap<QString, Observateur> _mapSites;

    // Stations
    QMap<QString, Observateur> _mapStations;

    // Satellites produisant des flashs
    QMap<QString, SatellitesFlashs> _mapFlashs;

    // Fichier TLE par defaut
    QString _nomfic;

    // TLE par defaut
    TLEdefaut _tleDefaut;

    // Soleil, Lune, planetes
    Soleil _soleil;
    Lune _lune;
    QList<Planete> _planetes;

    // Etoiles
    QList<Etoile> _etoiles;
    QList<Constellation> _constellations;
    QList<LigneConstellation> _lignesCst;

    // Numero NORAD de la station spatiale
    QString _noradStationSpatiale;

    // Evenements ISS
    QString _dateDebutISS;
    QString _dateFinISS;
    QList<double> _masseISS;
    QStringList _evenementsISS;

    // Donnees satellites
    QString _donneesSatellites;
    int _lgRec;

    // Pour le gestionnaire de fichiers TLE
    QString _versionCategoriesTLE;
    QList<CategorieTLE> _listeCategoriesTLE;
    QList<CategorieTLE> _listeCategoriesMajTLE;

    // Adresses de telechargement
    QMap<AdressesTelechargement, QString> _mapAdressesTelechargement;

    // Liste des chaines de la NASA
    QStringList _listeChainesNasa;

    bool _issLive;

    static bool _isCarteMonde;

    /*
     * Methodes privees
     */
    /**
     * @brief DefinitionArborescences Definition des arborescences
     */
    void DefinitionArborescences();

    /**
     * @brief DeterminationLocale Determination de la locale
     */
    void DeterminationLocale();

    /**
     * @brief InitFicMap Cartes du monde
     */
    void InitFicMap();

    /**
     * @brief InitFicPref Fichiers de preferences
     */
    void InitFicPref();

    /**
     * @brief InitFicTLE Fichiers TLE
     */
    void InitFicTLE();

    /**
     * @brief LectureBody Lecture de la section body du fichier ISS
     * @param cfg lecteur xml
     */
    void LectureBody(QXmlStreamReader &cfg);

    /**
     * @brief LectureConfiguration Lecture de la configuration
     */
    void LectureConfiguration();

    /**
     * @brief LectureCategoriesOrbite Lecture du fichier de categories d'orbite
     */
    void LectureCategoriesOrbite();

    /**
     * @brief LectureChainesNasa Lecture du fichier des chaines NASA
     */
    void LectureChainesNasa();

    /**
     * @brief LectureData Lecture de la section data du fichier ISS
     * @param cfg lecteur xml
     */
    void LectureData(QXmlStreamReader &cfg);

    /**
     * @brief LectureDonneesSatellites Lecture du fichier de donnees satellites
     */
    void LectureDonneesSatellites();

    /**
     * @brief LectureGestionnaireTLE Lecture du fichier de gestionnaire de TLE
     */
    void LectureGestionnaireTLE();

    /**
     * @brief LectureMetadata Lecture de la section metadata du fichier ISS
     * @param cfg lecteur xml
     */
    void LectureMetadata(QXmlStreamReader &cfg);

    /**
     * @brief LecturePays Lecture du fichier listant les pays ou organisations
     */
    void LecturePays();

    /**
     * @brief LectureSatellitesTDRS Lecture du fichier de satellites TDRS
     */
    void LectureSatellitesTDRS();

    /**
     * @brief LectureSitesLancement Lecture du fichier des sites de lancement
     */
    void LectureSitesLancement();

    /**
     * @brief LectureStations Lecture du fichier de stations
     */
    void LectureStations();

    /**
     * @brief LectureStatutSatellitesFlashs Lecture du fichier de statut des satellites produisant des flashs
     */
    void LectureStatutSatellitesFlashs();

    /**
     * @brief VerificationArborescences Verification des arborescences
     */
    void VerificationArborescences();

    /**
     * @brief VerifieFichiersData Verifie la presence des fichiers du repertoire data
     * @param dirData repertoire data
     * @param listeFicData liste de fichiers du repertoire data
     */
    void VerifieFichiersData(const QString &dirData, const QStringList &listeFicData) const;

    /**
     * @brief VerifieFichierXml Verification du fichier xml
     * @param nomficXml nom du fichier xml
     * @param version numero de version
     * @param typeMessage type de message
     * @param message1 message 1 a afficher
     * @param message2 message 2 a afficher
     */
    void VerifieFichierXml(const QString &nomficXml, QString &version, const MessageType &typeMessage, const QString &message1 = QString(),
                           const QString &message2 = QString());

    /**
     * @brief VerifieVersionXml Verification du numero de version du fichier xml
     * @param fi1 fichier xml du repertoire commun
     * @param fi2 fichier xml du repertoire local
     * @param msg message a afficher
     */
    void VerifieVersionXml(QFile &fi1, QFile &fi2, QString &version, const QString &msg = QString());

};

#endif // CONFIGURATION_H
