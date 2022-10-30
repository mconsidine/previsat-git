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
 * >
 *
 */

#ifndef EVENEMENTSSTATIONSPATIALE_H
#define EVENEMENTSSTATIONSPATIALE_H

#include <QList>
#include <QStringList>


class QXmlStreamReader;
struct EvenementsStation;

class EvenementsStationSpatiale
{
public:

    /*
     *  Constructeurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief LectureEvenementsStationSpatiale Lecture du fichier NASA contenant les evenements de la Station Spatiale
     * @return evenements lies a la station spatiale
     */
    static EvenementsStation LectureEvenementsStationSpatiale();


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

    /*
     * Methodes privees
     */
    /**
     * @brief LectureBody Lecture de la section body du fichier Station Spatiale
     * @param cfg lecteur xml
     * @param evenements evenements
     */
    static void LectureBody(QXmlStreamReader &cfg, EvenementsStation &evenements);

    /**
     * @brief LectureData Lecture de la section data du fichier Station Spatiale
     * @param cfg lecteur xml
     * @param evenements evenements
     */
    static void LectureData(QXmlStreamReader &cfg, EvenementsStation &evenements);

    /**
     * @brief LectureMetadata Lecture de la section metadata du fichier Station Spatiale
     * @param cfg lecteur xml
     * @param evenements evenements
     */
    static void LectureMetadata(QXmlStreamReader &cfg, EvenementsStation &evenements);

};

#endif // EVENEMENTSSTATIONSPATIALE_H
