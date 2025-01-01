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
 * >    22 aout 2024
 *
 */

#ifndef GESTIONNAIREXML_H
#define GESTIONNAIREXML_H

#include <QMap>

#include "librairies/observateur/observateur.h"
#include "categorieelementsorbitaux.h"
#include "frequencesradio.h"
#include "satellitesflashs.h"
#include "satellitesstarlink.h"
#include "satellitetdrs.h"


class QDate;
class QDomNode;
class QString;
class QXmlStreamWriter;

class GestionnaireXml
{
    friend class Configuration;
    friend class FichierObs;
#if (BUILD_TEST)
    friend class FlashsTest;
    friend class GeneralTest;
#endif
public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief EcritureGestionnaireElementsOrbitaux Ecriture du fichier de gestionnaire d'elements orbitaux
     * @throw Exception
     */
    static void EcritureGestionnaireElementsOrbitaux();

    /**
     * @brief EcriturePreLaunchStarlink Ecriture du fichier Pre-Launch Starlink
     * @throw Exception
     */
    static void EcriturePreLaunchStarlink();


    /*
     * Accesseurs
     */
    static QList<Observateur> observateurs();


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
     * @brief EcritureCategoriesElementsOrbitaux Ecriture des categories d'elements orbitaux
     * @param[in] listeCategorie Liste des categories d'elements orbitaux
     * @param[in] cfg structure du fichier xml
     */
    static void EcritureCategoriesElementsOrbitaux(const QList<CategorieElementsOrbitaux> &listeCategorie,
                                                   QXmlStreamWriter &cfg);

    /**
     * @brief LectureCategoriesElementsOrbitaux Lecture des categories d'elements orbitaux
     * @param[in] serveur nom du serveur
     * @param[out] categorieMajElem categories d'elements orbitaux a mettre a jour
     * @return categories d'elements orbitaux
     */
    static QList<CategorieElementsOrbitaux> LectureCategoriesElementsOrbitaux(const QDomNode &serveur,
                                                                              QList<CategorieElementsOrbitaux> &categorieMajElem);

    /**
     * @brief LectureCategoriesOrbite Lecture du fichier de categories d'orbite
     * @return map contenant les categories d'orbite
     * @throw Exception
     */
    static QMap<QString, QString> LectureCategoriesOrbite();

    /**
     * @brief LectureFrequencesRadio Lecture du fichier contenant les frequences radio des satellites
     * @return map contenant les frequences radio des satellites
     * @throw Exception
     */
    static QMap<QString, QList<FrequenceRadio> > LectureFrequencesRadio();

    /**
     * @brief LectureGestionnaireElementsOrbitaux Lecture du fichier de gestionnaire d'elements orbitaux
     * @param[out] versionCategorieElem
     * @return map contenant les categories d'elements orbitaux
     * @throw Exception
     */
    static QMap<QString, QList<CategorieElementsOrbitaux> >
    LectureGestionnaireElementsOrbitaux(QString &versionCategorieElem,
                                        QMap<QString, QList<CategorieElementsOrbitaux> > &mapCategoriesMajElementsOrbitaux);

    /**
     * @brief LectureLieuxObservation Lecture de la structure de lieux d'observations
     * @param[in] obs noeud du fichier xml correspondant a un observateur
     * @return lieux d'observation
     */
    static QList<Observateur> LectureLieuxObservation(const QDomNode &obs);
    
    /**
     * @brief LectureNomsConstellations Lecture des noms des constellations
     * @return noms des constellations
     */
    static QMap<QString, QMap<QString, QString> > LectureNomsConstellations();

    /**
     * @brief LecturePays Lecture du fichier listant les pays et organisations
     * @return map contenant les pays et organisations
     * @throw Exception
     */
    static QMap<QString, QString> LecturePays();

    /**
     * @brief LecturePreLaunchStarlink Lecture du fichier Pre-Launch Starlink
     * @return map contenant les informations pre-launch des satellites Starlink
     * @throw Exception
     */
    static QMap<QString, SatellitesStarlink> LecturePreLaunchStarlink();

    /**
     * @brief LectureSatellitesFlashs Lecture du fichier de statut des satellites produisant des flashs
     * @return map contenant les caracteristiques des satellites produisant des flashs
     * @throw Exception
     */
    static QMap<QString, SatellitesFlashs> LectureSatellitesFlashs();

    /**
     * @brief LectureSatellitesFrequences Lecture des structures des frequences radio
     * @param[in] sat structure pour chaque satellite
     * @return liste des frequences radio
     */
    static QList<FrequenceRadio> LectureSatellitesFrequences(const QDomNode &sat);
    /**
     * @brief LectureSatellitesTDRS Lecture du fichier de satellites TDRS
     * @return map contenant les couleurs des satellites TDRS
     * @throw Exception
     */
    static QMap<int, SatelliteTDRS> LectureSatellitesTDRS();

    /**
     * @brief LectureSitesLancement Lecture du fichier des sites de lancement
     * @return map contenant la denomination des sites de lancement
     * @throw Exception
     */
    static QMap<QString, Observateur> LectureSitesLancement();

    /**
     * @brief LectureStations Lecture du fichier de stations
     * @return map contenant la description des stations
     * @throw Exception
     */
    static QMap<QString, Observateur> LectureStations();

    /**
     * @brief LectureStatutSatellitesFlashs Lecture des statuts des satellites produisant des flashs
     * @param[in] sat noeud du fichier xml correspondant a un satellite
     * @return statut des satellites produisant des flashs
     */
    static SatellitesFlashs LectureStatutSatellitesFlashs(const QDomNode &sat);

    /**
     * @brief LectureVerrous Lecture du fichier de verrous
     * @return map contenant les dates d'expiration des fonctionnalites
     */
    static QMap<QString, QDate> LectureVerrous();

    /**
     * @brief VerifieVersionXml Verification du numero de version du fichier xml
     * @param[in] nomfic nom du fichier
     * @param[in] msg message
     */
    static void VerifieVersionXml(const QString &nomfic, const QString &msg = QString());

};

#endif // GESTIONNAIREXML_H
