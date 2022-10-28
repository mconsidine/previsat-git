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
 * >    28 octobre 2022
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFont>
#include <QMap>
#include <QString>
#pragma GCC diagnostic warning "-Wswitch-default"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/elementsorbitaux.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/observateur/observateur.h"
#include "categorieelementsorbitaux.h"
#include "configurationconst.h"
#include "frequencesradio.h"
#include "satellitesflashs.h"
#include "satellitetdrs.h"


class Configuration
{
#if (BUILD_TEST == true)
    friend class EvenementsOrbitauxTest;
    friend class FlashsTest;
    friend class GPFormatTest;
    friend class PrevisionTest;
    friend class SatelliteTest;
    friend class TelescopeTest;
    friend class TLETest;
#endif

public:

    /*
     *  Constructeurs
     */
    Configuration(const Configuration &) = delete;


    /*
     * Methodes publiques
     */
    Configuration& operator = (const Configuration &) = delete;

    /**
     * @brief Chargement Chargement de la configuration generale
     */
    void Chargement();

    /**
     * @brief Initialisation Definitions preliminaires pour le logiciel
     */
    void Initialisation();

    /**
     * @brief InitListeFichiersElem Initialisation de la liste de fichiers d'elements orbitaux
     */
    void InitListeFichiersElem();


    /*
     * Accesseurs
     */
    static Configuration *instance();

    // Repertoires
    const QString &dirCfg() const;
    const QString &dirCommonData() const;
    const QString &dirCoord() const;
    const QString &dirDox() const;
    const QString &dirElem() const;
    const QString &dirExe() const;
    const QString &dirHtml() const;
    const QString &dirLang() const;
    const QString &dirLocalData() const;
    const QString &dirLog() const;
    const QString &dirMap() const;
    const QString &dirOut() const;
    const QString &dirPref() const;
    const QString &dirRsc() const;
    const QString &dirSon() const;
    const QString &dirTmp() const;

    // Locale
    const QString &locale() const;
    const QStringList &listeFicLang() const;

    const QStringList &listeFicLocalData() const;

    // Polices
    const QFont &police() const;
    const QFont &policeWcc() const;

    bool isCarteMaximisee();
    bool &issLive();


    // Versions des fichiers de configuration
    const QString &versionCfg() const;
    const QString &versionCategorieElem() const;

    // Adresses internet
    const QString &adresseCelestrak() const;
    const QString &adresseCelestrakNorad() const;

    // Evenements Station Spatiale
    const QString &nomFichierEvenementsStationSpatiale() const;
    const QString &noradStationSpatiale() const;

    // Lieux d'observation
    Observateur &observateur();
    const QList<Observateur> &observateurs() const;
    QMap<QString, Observateur> &mapObs();

    // Map des satellites de tous les fichiers d'elements orbitaux
    const QMap<QString, QStringList> &mapSatellitesFichierElem() const;

    // Soleil, Lune, planetes
    Soleil &soleil();
    Lune &lune();
    std::array<Planete, NB_PLANETES> &planetes();


    const QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesElementsOrbitaux() const;
    const QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesMajElementsOrbitaux() const;

    const QMap<int, SatelliteTDRS> &mapTDRS() const;

    const QMap<QString, Observateur> &mapStations() const;

    // Satellites produisant des flashs
    const QMap<QString, SatellitesFlashs> &mapFlashs() const;

    // Donnees satellites
    const QString &donneesSatellites() const;
    int lgRec() const;

    // Nom du fichier d'elements orbitaux par defaut
    QString &nomfic();

    // Numero NORAD par defaut
    QString &noradDefaut();

    // Liste des fichiers d'elements orbitaux
    const QStringList &listeFichiersElem() const;

    QList<Satellite> &listeSatellites();

    const QMap<QString, ElementsOrbitaux> &mapElementsOrbitaux() const;

    // Frequences radio des satellites
    const QMap<QString, QList<FrequenceRadio> > &mapFrequencesRadio() const;

    QList<Etoile> &etoiles();
    QList<Constellation> &constellations();
    QList<LigneConstellation> &lignesCst();

    const QStringList &listeFicMap() const;
    const QStringList &listeFicObs() const;

    const QStringList &listeFicPref() const;
    const QStringList &listeFicSon() const;

    // Notifications sonores
    NotificationSonore &notifAOS();
    NotificationSonore &notifFlashs();


    /*
     * Modificateurs
     */
    void setPolice(const QFont &p);
    void setPoliceWcc(const QFont &p);
    void setMapElementsOrbitaux(const QMap<QString, ElementsOrbitaux> &map);
    void setListeFicElem(const QStringList &listeFic);


protected:

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
     * Variables privees
     */
    static Configuration *_instance;

    // Configuration logicielle
    // Repertoires
    QString _dirCfg;
    QString _dirCommonData;
    QString _dirCoord;
    QString _dirDox;
    QString _dirElem;
    QString _dirExe;
    QString _dirHtml;
    QString _dirLang;
    QString _dirLocalData;
    QString _dirLog;
    QString _dirMap;
    QString _dirOut;
    QString _dirPref;
    QString _dirRsc;
    QString _dirSon;
    QString _dirTmp;

    // Locale
    QString _locale;
    QStringList _listeFicLang;

    // Fichiers du repertoire data local
    QStringList _listeFicLocalData;

    // Polices
    QFont _police;
    QFont _policeWcc;

    bool _isCarteMaximisee;
    bool _issLive;


    // Fichiers xml de configuration
    // Versions des fichiers de configuration
    QString _versionCfg;
    QString _versionCategorieElem;

    // Adresses internet
    QString _adresseCelestrak;
    QString _adresseCelestrakNorad;

    // Categories d'orbite
    QMap<QString, QString> _mapCategoriesOrbite;

    // Categories d'elements orbitaux
    QMap<QString, QList<CategorieElementsOrbitaux> > _mapCategoriesElementsOrbitaux;
    QMap<QString, QList<CategorieElementsOrbitaux> > _mapCategoriesMajElementsOrbitaux;

    // Pays ou organisations
    QMap<QString, QString> _mapPays;

    // Satellites TDRS
    QMap<int, SatelliteTDRS> _mapTDRS;

    // Sites de lancement
    QMap<QString, Observateur> _mapSitesLancement;

    // Stations
    QMap<QString, Observateur> _mapStations;

    // Satellites produisant des flashs
    QMap<QString, SatellitesFlashs> _mapFlashs;


    // Donnees provenant d'autres fichiers de configuration
    // Donnees satellite
    QString _donneesSatellites;
    int _lgRec;

    // Liste des chaines de la NASA
    QStringList _listeChainesNasa;

    // Evenements Station Spatiale
    QString _nomFichierEvenementsStationSpatiale;
    QString _noradStationSpatiale;
    QString _dateDebutStationSpatiale;
    QString _dateFinStationSpatiale;
    double _masseStationSpatiale;
    double _surfaceTraineeAtmospherique;
    double _coefficientTraineeAtmospherique;
    QStringList _evenementsStationSpatiale;

    // Frequences radio des satellites
    QMap<QString, QList<FrequenceRadio> > _mapFrequencesRadio;

    // Etoiles
    QList<Etoile> _etoiles;
    QList<Constellation> _constellations;
    QList<LigneConstellation> _lignesCst;


    // Lieux d'observation
    QList<Observateur> _observateurs;

    // Lieux d'observation contenus dans un fichier
    QMap<QString, Observateur> _mapObs;

    // Map des satellites de tous les fichiers d'elements orbitaux
    QMap<QString, QStringList> _mapSatellitesFichierElem;


    // Soleil, Lune, planetes
    Soleil _soleil;
    Lune _lune;
    std::array<Planete, NB_PLANETES> _planetes;


    // Adresses de telechargement
    QMap<AdressesTelechargement, QString> _mapAdressesTelechargement;


    // Elements orbitaux
    // Nom du fichier d'elements orbitaux courant
    QString _nomfic;

    // Numero NORAD par defaut
    QString _noradDefaut;

    // Liste des fichiers d'elements orbitaux
    QStringList _listeFichiersElem;

    // Map des elements orbitaux d'un fichier
    QMap<QString, ElementsOrbitaux> _mapElementsOrbitaux;

    QList<Satellite> _listeSatellites;


    // Autres
    // Liste des cartes du monde
    QStringList _listeFicMap;

    QStringList _listeFicObs;

    // Fichiers de preferences
    QStringList _listeFicPref;

    // Fichiers de notification sonore
    QStringList _listeFicSon;

    // Notifications sonores
    NotificationSonore _notifAOS;
    NotificationSonore _notifFlashs;


    /*
     * Methodes privees
     */
    /**
      * @brief DefinitionArborescences Definition des arborescences
      */
    void DefinitionArborescences();

    /**
     * @brief DeterminationLocale Determination de la locale et liste des langues disponibles
     */
    void DeterminationLocale();

    /**
     * @brief InitListeFichiersMap Initialisation de la liste de fichiers de cartes du monde
     */
    void InitListeFichiersMap();

    /**
     * @brief InitListeFichiersObs Initialisation de la liste de fichiers de lieux d'observation
     */
    void InitListeFichiersObs();

    /**
     * @brief InitListeFichiersPref Initialisation de la liste de fichiers de preferences
     */
    void InitListeFichiersPref();

    /**
     * @brief InitListeFichiersSon Initialisation de la liste de fichiers de sons
     */
    void InitListeFichiersSon();

    /**
     * @brief LectureChainesNasa Lecture du fichier des chaines NASA
     */
    void LectureChainesNasa();

    /**
     * @brief LectureDonneesSatellites Lecture du fichier de donnees satellites
     */
    void LectureDonneesSatellites();

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


};

#endif // CONFIGURATION_H
