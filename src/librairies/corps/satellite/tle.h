/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    7 juillet 2024
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
#if (BUILD_TEST)
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
     * @param[in] lig0 ligne 0 du TLE (nom du satellite)
     * @param[in] lig1 ligne 1 du TLE
     * @param[in] lig2 ligne 2 du TLE
     */
    TLE(const QString &lig0,
        const QString &lig1,
        const QString &lig2);


    /*
     * Methodes publiques
     */
    /**
     * @brief Lecture Lecture du fichier TLE
     * @param[in] nomFichier nom du fichier TLE
     * @param[in] db donnees satellites
     * @param[in] listeSatellites liste des numeros NORAD (si elle est vide on recupere tous les TLE)
     * @param[in] ajoutDonnees ajout des donnees satellite
     * @return tableau d'elements orbitaux
     * @throw Exception
     */
    static QMap<QString, ElementsOrbitaux> Lecture(const QString &nomFichier,
                                                   const QSqlDatabase &db = QSqlDatabase(),
                                                   const QStringList &listeSatellites = QStringList(),
                                                   const bool ajoutDonnees = true);

    /**
     * @brief MiseAJourFichier Mise a jour du fichier TLE
     * @param[in] ficOld fichier avec les anciens TLE
     * @param[in] ficNew fichier avec les nouveaux TLE
     * @param[in] donneesSat donnees satellites
     * @param[in] affMsg affichage des messages
     * @return compte rendu de mise a jour
     * @throw Exception
     */
    static QStringList MiseAJourFichier(const QString &ficOld,
                                        const QString &ficNew,
                                        const QSqlDatabase &donneesSat,
                                        const int affMsg);

    /**
     * @brief VerifieFichier Verification du fichier TLE
     * @param[in] nomFichier nom du fichier TLE
     * @param[in] alarme affichage d'une boite de message
     * @return nombre de satellites dans le fichier
     * @throw Exception
     */
    static int VerifieFichier(const QString &nomFichier,
                              const bool alarme = false);


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
     * @param[in] ligne ligne d'un TLE
     * @param[out] checksum valeur calculee du checksum
     * @return vrai si le checksum est correct
     */
    static bool CheckSum(const QString &ligne,
                         int &checksum);

    /**
     * @brief VerifieLignes Verification des lignes d'un TLE
     * @param[in] li1 ligne 1 du TLE
     * @param[in] li2 ligne 2 du TLE
     * @param[in] nomsat nom du satellite
     * @param[in] alarme affichage d'une boite de message
     * @throw Exception
     */
    static void VerifieLignes(const QString &li1,
                              const QString &li2,
                              const QString &nomsat,
                              const bool alarme);


};

#endif // TLE_H
