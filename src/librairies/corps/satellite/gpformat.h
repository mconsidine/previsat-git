/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    11 novembre 2023
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
     * @param[in] elem elements orbitaux
     */
    explicit GPFormat(const ElementsOrbitaux &elem);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculNombreOrbitesEpoque Calcul du nombre d'orbites a l'epoque (cas depassant 100000 orbites)
     * @param[in] elements elements orbitaux
     * @return nombre d'orbites a l'epoque
     */
    static int CalculNombreOrbitesEpoque(const ElementsOrbitaux &elements);

    /**
     * @brief LectureFichier Lecture d'un fichier au format GP
     * @param[in] fichier nom du fichier d'elements orbitaux
     * @param[in] donneesSat donnees satellites
     * @param[in] lgRec longueur d'une ligne dans les donnees satellite
     * @param[in] listeSatellites liste des numeros NORAD ou COSPAR (si elle est vide on recupere tous les elements orbitaux du fichier)
     * @param[in] ajoutDonnees ajout des donnees satellite
     * @param[in] alarme affichage des messages d'erreurs ou de warnings
     * @return tableau d'elements orbitaux
     */
    static QMap<QString, ElementsOrbitaux> LectureFichier(const QString &nomFichier, const QString &donneesSat, const int lgRec,
                                                          const QStringList &listeSatellites = QStringList(), const bool ajoutDonnees = true,
                                                          const bool alarme = false);

    /**
     * @brief LectureFichierListeGP Lecture d'un fichier GP contenant une liste d'elements orbitaux pour un meme satellite
     * @param[in] fichier nom du fichier d'elements orbitaux
     * @param[in] donneesSat donnees satellites
     * @param[in] lgRec longueur d'une ligne dans les donnees satellite
     * @param[in] alarme affichage des messages d'erreurs ou de warnings
     * @return liste d'elements orbitaux
     */
    static QList<ElementsOrbitaux> LectureFichierListeGP(const QString &nomFichier, const QString &donneesSat, const int lgRec,
                                                         const bool alarme = false);

    /**
     * @brief RecupereNomsat Recupere le nom du satellite
     * @param[in] nomComplet nom complet du satellite
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
     * @param[in/out] gp contenu du fichier
     * @param[in/out] elem elements orbitaux
     */
    static void LectureSectionBody(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionData Lecture de la section Data du fichier d'elements orbitaux
     * @param[in/out] gp contenu du fichier
     * @param[in/out] elem elements orbitaux
     */
    static void LectureSectionData(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionMeanElements Lecture de la section MeanElements du fichier d'elements orbitaux
     * @param[in/out] gp contenu du fichier
     * @param[in/out] elem elements orbitaux
     */
    static void LectureSectionMeanElements(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionMetaData Lecture de la section MetaData du fichier d'elements orbitaux
     * @param[in/out] gp contenu du fichier
     * @param[in/out] elem elements orbitaux
     */
    static void LectureSectionMetaData(QXmlStreamReader &gp, ElementsOrbitaux &elem);

    /**
     * @brief LectureSectionTleParameters Lecture de la section TleParameters du fichier d'elements orbitaux
     * @param[in/out] gp contenu du fichier
     * @param[in/out] elem elements orbitaux
     */
    static void LectureSectionTleParameters(QXmlStreamReader &gp, ElementsOrbitaux &elem);

};

#endif // GPFORMAT_H
