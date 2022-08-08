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
 * >    evenementsstationspatiale.h
 *
 * Localisation
 * >    configuration
 *
 * Heritage
 * >
 *
 * Description
 * >    Lecture du fichier xml contenant les evenements de la station spatiale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    19 juin 2022
 *
 * Date de revision
 * >    8 aout 2022
 *
 */

#ifndef EVENEMENTSSTATIONSPATIALE_H
#define EVENEMENTSSTATIONSPATIALE_H

#include <QList>
#include <QStringList>


class QXmlStreamReader;

class EvenementsStationSpatiale
{
    friend class Configuration;

public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief LectureEvenementsStationSpatiale Lecture du fichier NASA contenant les evenements de la Station Spatiale
     * @param dateDebutEvenementsStationSpatiale date de debut des evenements contenus dans le fichier
     * @param dateFinEvenementsStationSpatiale date de fin des evenements contenus dans le fichier
     * @param masseStationSpatiale masse de la Station Spatiale
     * @param evenementsStationSpatiale evenements lies a la Station Spatiale
     */
    static void LectureEvenementsStationSpatiale(QString &dateDebutEvenementsStationSpatiale,
                                                 QString &dateFinEvenementsStationSpatiale,
                                                 double &masseStationSpatiale,
                                                 double &surfaceTraineeAtmospherique,
                                                 double &coefficientTraineeAtmospherique,
                                                 QStringList &evenementsStationSpatiale);


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
    static QString _dateDebutStationSpatiale;
    static QString _dateFinStationSpatiale;
    static double _masseStationSpatiale;
    static double _surfaceTraineeAtmospherique;
    static double _coefficientTraineeAtmospherique;
    static QStringList _evenementsStationSpatiale;

    /*
     * Methodes privees
     */
    /**
     * @brief LectureBody Lecture de la section body du fichier Station Spatiale
     * @param cfg lecteur xml
     */
    static void LectureBody(QXmlStreamReader &cfg);

    /**
     * @brief LectureData Lecture de la section data du fichier Station Spatiale
     * @param cfg lecteur xml
     */
    static void LectureData(QXmlStreamReader &cfg);

    /**
     * @brief LectureMetadata Lecture de la section metadata du fichier Station Spatiale
     * @param cfg lecteur xml
     */
    static void LectureMetadata(QXmlStreamReader &cfg);

};

#endif // EVENEMENTSSTATIONSPATIALE_H
