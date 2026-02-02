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
 * >    20 juillet 2024
 *
 */

#ifndef GPFORMAT_H
#define GPFORMAT_H

#include <QMap>
#include "elementsorbitaux.h"
#include "librairies/systeme/fichierxml.h"


class QDomDocument;
class QDomNode;

class GPFormat
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief GPFormat Constructeur par defaut
     */
    GPFormat();


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
     * @param[in] db donnees satellites
     * @param[in] listeSatellites liste des numeros NORAD ou des designations COSPAR (si elle est vide on recupere tous les elements orbitaux)
     * @param[in] ajoutDonnees ajout des donnees satellite
     * @param[in] alarme affichage des messages d'erreurs ou de warnings
     * @return tableau d'elements orbitaux
     * @throw Exception
     */
    static QMap<QString, ElementsOrbitaux> Lecture(const QString &fichier,
                                                   const QSqlDatabase &db = QSqlDatabase(),
                                                   const QStringList &listeSatellites = QStringList(),
                                                   const bool ajoutDonnees = true,
                                                   const bool alarme = true);

    /**
     * @brief LectureFichierListeGP Lecture d'un fichier GP contenant une liste d'elements orbitaux pour un meme satellite
     * @param[in] fichier nom du fichier d'elements orbitaux
     * @param[in] db donnees satellites
     * @param[in] alarme affichage des messages d'erreurs ou de warnings
     * @return liste d'elements orbitaux
     * @throw Exception
     */
    static QList<ElementsOrbitaux> LectureListeGP(const QString &fichier,
                                                  const QSqlDatabase &db = QSqlDatabase(),
                                                  const bool alarme = false);

    /**
     * @brief RecupereNomsat Recupere le nom du satellite
     * @param[in] nomComplet nom complet du satellite
     * @return nom du satellite
     */
    static QString RecupereNomsat(const QString &nom);


    /*
     * Accesseurs
     */
    ElementsOrbitaux elements() const;


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
     * @brief LectureElements Lecture des elements orbitaux
     * @param[in] sat noeud du fichier xml correspondant a un satellite
     * @return elements orbitaux
     * @throw Exception
     */
    static ElementsOrbitaux LectureElements(const QDomNode &sat);

};

#endif // GPFORMAT_H
