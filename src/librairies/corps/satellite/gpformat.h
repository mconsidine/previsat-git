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
 * >    gpformat.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires lies a la manipulation des elements orbitaux au format GP
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef GPFORMAT_H
#define GPFORMAT_H

#include <QString>
#include "librairies/dates/date.h"
#include "elementsorbitaux.h"


class QXmlStreamReader;

class GPFormat
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief GPFormat Constructeur a partir des elements orbitaux
     * @param elem elements orbitaux
     */
    GPFormat(const ElementsOrbitaux &elem);


    /*
     * Methodes publiques
     */
    /**
     * @brief LectureFichier Lecture d'un fichier au format GP
     * @param fichier nom du fichier d'elements orbitaux
     * @param donneesSat donnees satellites
     * @param lgRec longueur d'une ligne dans les donnees satellite
     * @param listeSatellites liste des numeros NORAD (si elle est vide on recupere tous les elements orbitaux)
     * @param ajoutDonnees ajout des donnees satellite
     * @return tableau d'elements orbitaux
     */
    static QMap<QString, ElementsOrbitaux> LectureFichier(const QString &nomFichier, const QString &donneesSat, const int lgRec,
                                                  const QStringList &listeSatellites = QStringList(), const bool ajoutDonnees = true);

    /**
     * @brief RecupereNomsat Recupere le nom du satellite
     * @param nomComplet nom complet du satellite
     * @return nom du satellite
     */
    static QString RecupereNomsat(const QString &nomsat);


    /*
     * Accesseurs
     */
    const ElementsOrbitaux &elements() const;


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
    // Elements orbitaux
    ElementsOrbitaux _elements;


    /*
     * Methodes privees
     */
    /**
     * @brief LectureSectionBody Lecture de la section Body du fichier d'elements orbitaux
     * @param gp contenu du fichier
     * @param elem elements orbitaux
     */
    static void LectureSectionBody(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionData Lecture de la section Data du fichier d'elements orbitaux
     * @param gp contenu du fichier
     * @param elem elements orbitaux
     */
    static void LectureSectionData(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionMeanElements Lecture de la section MeanElements du fichier d'elements orbitaux
     * @param gp contenu du fichier
     * @param elem elements orbitaux
     */
    static void LectureSectionMeanElements(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionMetaData Lecture de la section MetaData du fichier d'elements orbitaux
     * @param gp contenu du fichier
     * @param elem elements orbitaux
     */
    static void LectureSectionMetaData(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionTleParameters Lecture de la section TleParameters du fichier d'elements orbitaux
     * @param gp contenu du fichier
     * @param elem elements orbitaux
     */
    static void LectureSectionTleParameters(QXmlStreamReader &gp, ElementsOrbitaux &elem);

};

#endif // GPFORMAT_H
