/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    16 decembre 2025
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QFont>
#include <QList>
#include <QMap>
#include <QString>
#include "calendrierlancements.h"
#include "categorieelementsorbitaux.h"
#include "configurationconst.h"
#include "evenementsstation.h"
#include "frequencesradio.h"
#include "rentreesatmospheriques.h"
#include "satellitesflashs.h"
#include "satellitesstarlink.h"
#include "satellitetdrs.h"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/elementsorbitaux.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/observateur/observateur.h"


class Configuration
{
#if (BUILD_TEST)
    friend class PrevisionTest;
    friend class TransitsTest;
    friend class FlashsTest;
    friend class GeneralTest;
    friend class InformationsTest;
    friend class EvenementsOrbitauxTest;
    friend class OsculateursTest;
    friend class StarlinkTest;
    friend class TelescopeTest;
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
     * @throw Exception
     */
    void Chargement();

    /**
     * @brief EcritureConfiguration Ecriture de la configuration
     * @throw Exception
     */
    void EcritureConfiguration();

    /**
     * @brief FermetureBaseDonneesSatellites Fermeture de la base de donnees satellites
     */
    void FermetureBaseDonneesSatellites();

    /**
     * @brief Initialisation Definitions preliminaires pour le logiciel
     * @throw Exception
     */
    void Initialisation();

    /**
     * @brief InitListeFichiersElem Initialisation de la liste de fichiers d'elements orbitaux
     */
    void InitListeFichiersElem();

    /**
     * @brief InitListeFichiersObs Initialisation de la liste de fichiers de lieux d'observation
     */
    void InitListeFichiersObs();

    /**
     * @brief VerifieDateExpiration Verification de la date d'expiration d'une fonctionnalite
     * @param fonction nom de la fonctionnalite
     * @return vrai si la fonctionnalite doit etre affichee dans l'interface
     */
    bool VerifieDateExpiration(const QString &fonction);


    /*
     * Accesseurs
     */
    static Configuration *instance();

    // Repertoires
    const QString dirBnd() const;
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
    const QString &dirStarlink() const;
    const QString &dirTmp() const;

    // Locale
    QString &locale();
    const QStringList &listeFicLang() const;

    const QStringList &listeFicLocalData() const;

    // Polices
    QFont &police();
    QFont &policeWcc();

    bool &isCarteMaximisee();
    bool &issLive();


    // Versions des fichiers de configuration
    const QString &versionCfg() const;
    const QString &versionCategorieElem() const;

    // Adresses internet
    const QString &adresseCelestrak() const;
    const QString &adresseCelestrakNorad() const;
    const QString &adresseCelestrakSupplementalNorad() const;
    const QString &adresseCelestrakSupplementalNoradFichier() const;
    const QString adresseRocketLaunchLive() const;

    // Categories d'orbite
    const QMap<QString, QString> &mapCategoriesOrbite() const;

    // Evenements Station Spatiale
    const QString &nomFichierEvenementsStationSpatiale() const;
    const QString &noradStationSpatiale() const;

    // Lieux d'observation
    Observateur &observateur();
    QList<Observateur> &observateurs();
    QMap<QString, Observateur> &mapObs();

    // Map des satellites de tous les fichiers d'elements orbitaux
    QMap<QString, QStringList> &mapSatellitesFichierElem();

    // Soleil, Lune, planetes
    Soleil &soleil();
    Lune &lune();
    std::array<Planete, PLANETE::NB_PLANETES> &planetes();

    const QMap<TypeTelechargement, QString> &mapAdressesTelechargement() const;


    QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesElementsOrbitaux();
    QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesMajElementsOrbitaux();

    const QMap<QString, QString> &mapPays() const;
    const QMap<int, SatelliteTDRS> &mapTDRS() const;
    const QMap<QString, Observateur> &mapSitesLancement() const;
    const QMap<QString, Observateur> &mapStations() const;

    // Satellites produisant des flashs
    const QMap<QString, SatellitesFlashs> &mapFlashs() const;

    // Donnees satellites
    QSqlDatabase dbSatellites() const;

    // Nom du fichier d'elements orbitaux par defaut
    QString &nomfic();

    // Numero NORAD par defaut
    QString &noradDefaut();

    // Liste des fichiers d'elements orbitaux
    QStringList &listeFichiersElem();
    void AjoutSatelliteFichierElem(const QString &norad);
    void SuppressionSatelliteFichierElem(const QString &norad);
    QMap<QString, QStringList> &mapFichierElemNorad();

    QList<Satellite> &listeSatellites();

    QMap<QString, ElementsOrbitaux> &mapElementsOrbitaux();

    EvenementsStation &evenementsStation();

    QList<CalendrierLancements> &calendrierLancements();
    QList<RentreesAtmospheriques> &rentreesAtmospheriques();

    // Frequences radio des satellites
    const QMap<QString, QList<FrequenceRadio> > &mapFrequencesRadio() const;

    QList<Etoile> &etoiles();
    QList<Constellation> &constellations();
    QMap<QString, QMap<QString, QString> > mapNomsConstellations() const;
    QList<LigneConstellation> &lignesCst();

    const QStringList &listeFicMap() const;
    const QStringList &listeFicObs() const;

    const QStringList &listeFicPref() const;
    const QStringList &listeFicSon() const;

    // Notifications sonores
    NotificationSonore &notifAOS();
    NotificationSonore &notifFlashs();

    // Satellites Starlink
    QMap<QString, SatellitesStarlink> &satellitesStarlink();
    QMap<QString, QStringList> &groupesStarlink();
    void AjoutDonneesSatellitesStarlink(const QString &groupe,
                                        const QString &fichier,
                                        const QString &lancement,
                                        const QString &deploiement);

    QMap<QString, QDate> mapVerrous() const;


    /*
     * Modificateurs
     */
    void setObservateurDefaut(const int index);


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
        _isCarteMaximisee = false;
        _issLive = false;
        _notifAOS = NotificationSonore::ATTENTE_LOS;
        _notifFlashs = NotificationSonore::ATTENTE_LOS;
    }


    /*
     * Variables privees
     */
    static Configuration *_instance;

    // Configuration logicielle
    // Repertoires
    QString _dirBnd;
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
    QString _dirStarlink;
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
    QString _adresseCelestrakSupplementalNorad;
    QString _adresseCelestrakSupplementalNoradFichier;
    QString _adresseRocketLaunchLive;

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

    // Donnees satellites
    QSqlDatabase _dbSatellites;

    // Evenements Station Spatiale
    QString _nomFichierEvenementsStationSpatiale;
    QString _noradStationSpatiale;
    EvenementsStation _evenementsStation {};

    QList<CalendrierLancements> _calendrierLancements;
    QList<RentreesAtmospheriques> _rentreesAtmospheriques;

    // Frequences radio des satellites
    QMap<QString, QList<FrequenceRadio> > _mapFrequencesRadio;

    // Etoiles
    QList<Etoile> _etoiles;
    QList<Constellation> _constellations;
    QMap<QString, QMap<QString, QString> > _mapNomsConstellations;
    QList<LigneConstellation> _lignesCst;


    // Lieux d'observation
    QList<Observateur> _observateurs;

    // Lieux d'observation contenus dans un fichier
    QMap<QString, Observateur> _mapObs;

    // Map des satellites de tous les fichiers d'elements orbitaux
    QMap<QString, QStringList> _mapSatellitesFichierElem;
    QMap<QString, QStringList> _mapFichierElemNorad;


    // Soleil, Lune, planetes
    Soleil _soleil;
    Lune _lune;
    std::array<Planete, PLANETE::NB_PLANETES> _planetes;


    // Adresses de telechargement
    QMap<TypeTelechargement, QString> _mapAdressesTelechargement;


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

    // Satellites Starlink
    QMap<QString, SatellitesStarlink> _satellitesStarlink;
    QMap<QString, QStringList> _groupesStarlink;

    QMap<QString, QDate> _mapVerrous;


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
     * @brief InitListeFichiersPref Initialisation de la liste de fichiers de preferences
     */
    void InitListeFichiersPref();

    /**
     * @brief InitListeFichiersSon Initialisation de la liste de fichiers de sons
     */
    void InitListeFichiersSon();

    /**
     * @brief LectureConfiguration Lecture du fichier de configuration generale
     * @throw Exception
     */
    void LectureConfiguration();

    /**
     * @brief OuvertureBaseDonneesSatellites Ouverture de la base de donnees satellites
     * @throw Exception
     */
    void OuvertureBaseDonneesSatellites();

    /**
     * @brief VerificationArborescences Verification des arborescences
     * @throw Exception
     */
    void VerificationArborescences();

    /**
     * @brief VerifieFichiersData Verifie la presence des fichiers du repertoire data
     * @param[in] dirData repertoire data
     * @param[in] listeFicData liste de fichiers du repertoire data
     * @throw Exception
     */
    void VerifieFichiersData(const QString &dirData,
                             const QStringList &listeFicData) const;


};

#endif // CONFIGURATION_H
