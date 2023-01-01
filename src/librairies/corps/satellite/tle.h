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
 * >    tle.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires lies a la manipulation des TLE
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    18 octobre 2022
 *
 */

#ifndef TLE_H
#define TLE_H

#include <QMap>
#include "librairies/dates/date.h"
#include "donnees.h"
#include "elementsorbitaux.h"


class TLE
{
#if (BUILD_TEST == true)
    friend class OngletsTest;
    friend class SatelliteTest;
    friend class TLETest;
#endif
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief TLE Constructeur par defaut
     */
    TLE();

    /**
     * @brief TLE Definition a partir des composantes du TLE
     * @param lig0 ligne 0 du TLE (nom du satellite)
     * @param lig1 ligne 1 du TLE
     * @param lig2 ligne 2 du TLE
     */
    TLE(const QString &lig0, const QString &lig1, const QString &lig2);


    /*
     * Methodes publiques
     */
    /**
     * @brief LectureFichier Lecture du fichier TLE
     * @param nomFichier nom du fichier TLE
     * @param donneesSat donnees satellites
     * @param lgRec longueur d'une ligne dans les donnees satellite
     * @param listeSatellites liste des numeros NORAD (si elle est vide on recupere tous les TLE)
     * @param ajoutDonnees ajout des donnees satellite
     * @return tableau d'elements orbitaux
     */
    static QMap<QString, ElementsOrbitaux> LectureFichier(const QString &nomFichier, const QString &donneesSat, const int lgRec,
                                             const QStringList &listeSatellites = QStringList(), const bool ajoutDonnees = true);

    /**
     * @brief LectureFichier3le Lecture du fichier 3le
     * @param nomFichier3le nom du fichier 3le
     * @return tableau d'elements orbitaux
     */
    static QList<ElementsOrbitaux> LectureFichier3le(const QString &nomFichier3le);

    /**
     * @brief MiseAJourFichier Mise a jour du fichier TLE
     * @param ficOld fichier avec les anciens TLE
     * @param ficNew fichier avec les nouveaux TLE
     * @param donneesSat donnees satellites
     * @param lgRec longueur d'une ligne dans les donnees satellite
     * @param affMsg affichage des messages
     * @param compteRendu compte rendu de mise a jour
     */
    static void MiseAJourFichier(const QString &ficOld, const QString &ficNew, const QString &donneesSat, const int lgRec, const int affMsg,
                                 QStringList &compteRendu);

    /**
     * @brief VerifieFichier Verification du fichier TLE
     * @param nomFichier nom du fichier TLE
     * @param alarme affichage d'une boite de message
     * @return nombre de satellites dans le fichier
     */
    static int VerifieFichier(const QString &nomFichier, const bool alarme = false);


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
    QString _ligne0;
    QString _ligne1;
    QString _ligne2;

    ElementsOrbitaux _elements;

    static QMap<QString, TLE> _mapTLE;


    /*
     * Methodes privees
     */
    /**
     * @brief CheckSum Verification du checksum d'une ligne de TLE
     * @param ligne ligne d'un TLE
     * @return vrai si le checksum est correct
     */
    static bool CheckSum(const QString &ligne);

    /**
     * @brief VerifieLignes Verification des lignes d'un TLE
     * @param li1 ligne 1 du TLE
     * @param li2 ligne 2 du TLE
     * @param nomsat nom du satellite
     * @param alarme affichage d'une boite de message
     */
    static void VerifieLignes(const QString &li1, const QString &li2, const QString &nomsat, const bool alarme);


};

#endif // TLE_H
