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
 * >    8 aout 2022
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QMap>
#include <QString>
#include "librairies/observateur/observateur.h"
#include "categorieelementsorbitaux.h"
#include "configurationconst.h"
#include "satellitesflashs.h"
#include "satellitetdrs.h"


class Configuration
{
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
     * @brief DefinitionDirLog Definition du repertoire dirLog
     */
    void DefinitionDirLog();

    /**
     * @brief Initialisation Initialisation de la configuration generale
     */
    void Initialisation();


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

    // Versions des fichiers de configuration
    const QString &versionCfg() const;
    const QString &versionCategorieElem() const;

    // Adresses internet
    const QString &adresseCelestrak() const;

    // Evenements Station Spatiale
    const QString &nomFichierEvenementsStationSpatiale() const;
    const QString &noradStationSpatiale() const;

    // Lieux d'observation
    const QList<Observateur> &observateurs() const;
    const QMap<QString, Observateur> &mapObs() const;

    // Liste des satellites de tous les fichiers d'elements orbitaux
    const QList<QPair<QString, QStringList> > &listeSatellitesFichierElem() const;

    const QList<CategorieElementsOrbitaux> &listeCategoriesElementsOrbitaux() const;

    // Donnees satellites
    const QString &donneesSatellites() const;
    int lgRec() const;


    /*
     * Modificateurs
     */


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


    // Fichiers xml de configuration
    // Versions des fichiers de configuration
    QString _versionCfg;
    QString _versionCategorieElem;

    // Adresses internet
    QString _adresseCelestrak;

    // Categories d'orbite
    QMap<QString, QString> _mapCategoriesOrbite;

    // Categories d'elements orbitaux
    QList<CategorieElementsOrbitaux> _listeCategoriesElementsOrbitaux;

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


    // Lieux d'observation
    QList<Observateur> _observateurs;

    // Lieux d'observation contenus dans un fichier
    QMap<QString, Observateur> _mapObs;

    // Liste des satellites de tous les fichiers d'elements orbitaux
    QList<QPair<QString, QStringList> > _listeSatellitesFichierElem;


    // Adresses de telechargement
    QMap<AdressesTelechargement, QString> _mapAdressesTelechargement;


    // Elements orbitaux
    // Nom du fichier d'elements orbitaux courant
    QString _nomfic;

    // Numero NORAD par defaut
    QString _noradDefaut;

    // Liste des fichiers d'elements orbitaux
    QStringList _listeFichiersElem;

    // Liste des cartes du monde
    QStringList _listeFicMap;

    // Fichiers de preferences
    QStringList _listeFicPref;


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
     * @brief InitListeFichiersElem Initialisation de la liste de fichiers d'elements orbitaux
     */
    void InitListeFichiersElem();

    /**
     * @brief InitListeFichiersMap Initialisation de la liste de fichiers de cartes du monde
     */
    void InitListeFichiersMap();

    /**
     * @brief InitListeFichiersPref Initialisation de la liste de fichiers de preferences
     */
    void InitListeFichiersPref();

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
