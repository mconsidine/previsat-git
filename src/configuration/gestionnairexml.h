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
 * >    gestionnairexml.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Lecture/ecriture des fichiers xml de configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    19 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef GESTIONNAIREXML_H
#define GESTIONNAIREXML_H

#include <QMap>
#include "categorieelementsorbitaux.h"
#include "satellitetdrs.h"
#include "librairies/exceptions/message.h"
#include "librairies/observateur/observateur.h"


class QFile;
class QString;
class QXmlStreamReader;

class GestionnaireXml
{
    friend class Configuration;
#if (BUILD_TEST == true)
    friend class FlashsTest;
#endif

public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief EcritureConfiguration Ecriture de la configuration
     */
    static void EcritureConfiguration();

    /**
     * @brief EcritureGestionnaireElementsOrbitaux Ecriture du fichier de gestionnaire d'elements orbitaux
     */
    static void EcritureGestionnaireElementsOrbitaux();


    /*
     * Accesseurs
     */

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
     * Variables privees
     */
    static QList<Observateur> _observateurs;


    /*
     * Methodes privees
     */
    /**
     * @brief LectureCategoriesOrbite Lecture du fichier de categories d'orbite
     * @return map contenant les categories d'orbite
     */
    static QMap<QString, QString> LectureCategoriesOrbite();

    /**
     * @brief LectureConfiguration Lecture du fichier de configuration generale
     * @param nomFichierEvenementsStationSpatiale nom du fichier d'evenements de la Station Spatiale
     * @param noradStationSpatiale numero NORAD de la Station Spatiale
     * @param versionCfg version du fichier de configuration
     * @param adresseCelestrak adresse du site Celestrak
     * @param nomfic nomdu fichier d'elements orbitaux par defaut
     * @param noradDefaut numero NORAD par defaut
     * @param observateurs liste des lieux d'observation
     * @param mapSatellitesFichierElem liste des satellites par fichier d'elements orbitaux
     */
    static void LectureConfiguration(QString &nomFichierEvenementsStationSpatiale,
                                     QString &noradStationSpatiale,
                                     QString &versionCfg,
                                     QString &adresseCelestrak,
                                     QString &nomfic,
                                     QString &noradDefaut,
                                     QList<Observateur> &observateurs,
                                     QMap<QString, QStringList> &mapSatellitesFichierElem);

    /**
     * @brief LectureFrequencesRadio Lecture du fichier contenant les frequences radio des satellites
     * @return map contenant les frequences radio des satellites
     */
    static QMap<QString, QList<FrequenceRadio> > LectureFrequencesRadio();

    /**
     * @brief LectureGestionnaireElementsOrbitaux Lecture du fichier de gestionnaire d'elements orbitaux
     * @param versionCategorieElem version du fichier de categories d'elements orbitaux
     * @return liste contenant les categories d'elements orbitaux
     */
    static QMap<QString, QList<CategorieElementsOrbitaux> > LectureGestionnaireElementsOrbitaux(QString &versionCategorieElem);

    /**
     * @brief LectureLieuxObservation Lecture de la structure de lieux d'observations
     * @param cfg lecteur xml
     * @return liste de lieux d'observations
     */
    static QList<Observateur> LectureLieuxObservation(QXmlStreamReader &cfg);

    /**
     * @brief LecturePays Lecture du fichier listant les pays et organisations
     * @return map contenant les pays et organisations
     */
    static QMap<QString, QString> LecturePays();

    /**
     * @brief LectureSatellitesTDRS Lecture du fichier de satellites TDRS
     * @return map contenant les couleurs des satellites TDRS
     */
    static QMap<int, SatelliteTDRS> LectureSatellitesTDRS();

    /**
     * @brief LectureSitesLancement Lecture du fichier des sites de lancement
     * @return map contenant la denomination des sites de lancement
     */
    static QMap<QString, Observateur> LectureSitesLancement();

    /**
     * @brief LectureStations Lecture du fichier de stations
     * @return map contenant la description des stations
     */
    static QMap<QString, Observateur> LectureStations();

    /**
     * @brief LectureStatutSatellitesFlashs Lecture du fichier de statut des satellites produisant des flashs
     * @return map contenant les caracteristiques des satellites produisant des flashs
     */
    static QMap<QString, SatellitesFlashs> LectureStatutSatellitesFlashs();

    /**
     * @brief VerifieFichierXml Verification du fichier xml
     * @param nomficXml nom du fichier xml
     * @param version numero de version
     * @param message message a afficher
     */
    static void VerifieFichierXml(const QString &nomficXml, QString &version, const QString &message = QString());

    /**
     * @brief VerifieVersionXml Verification du numero de version du fichier xml
     * @param fi1 fichier xml du repertoire commun
     * @param fi2 fichier xml du repertoire local
     * @param msg message a afficher
     */
    static void VerifieVersionXml(QFile &fi1, QFile &fi2, QString &version, const QString &msg = QString());


};

#endif // GESTIONNAIREXML_H
