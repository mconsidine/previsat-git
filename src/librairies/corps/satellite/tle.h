/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    16 novembre 2018
 *
 */

#ifndef TLE_H
#define TLE_H

#include <QMap>
#include <QStringList>
#include "librairies/dates/date.h"
#include "donnees.h"


class QString;

class TLE
{
#if BUILD_TEST == true
    friend class SatelliteTest;
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
     * @param dateDebValid date de debut de validite du TLE
     */
    TLE(const QString &lig0, const QString &lig1, const QString &lig2, const Date &dateDebValid = Date(-DATE_INFINIE, 0.));


    /*
     * Accesseurs
     */
    unsigned int nbOrbitesEpoque() const;
    Date dateDebutValidite() const;

    double argpo() const;
    double bstar() const;
    double ecco() const;
    Date epoque() const;
    double inclo() const;
    QString ligne0() const;
    QString ligne1() const;
    QString ligne2() const;
    double mo() const;
    double no() const;
    QString nom() const;
    QString norad() const;
    QString cospar() const;
    double omegao() const;

    double ndt20() const;
    double ndd60() const;

    Donnees donnees() const;


    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief VerifieFichier Verification du fichier TLE
     * @param nomFichier nom du fichier TLE
     * @param alarme affichage d'une boite de message
     * @return nombre de satellites dans le fichier
     */
    static int VerifieFichier(const QString &nomFichier, const bool alarme = false);

    /**
     * @brief LectureFichier Lecture du fichier TLE
     * @param dirLocalData chemin de donnees locales
     * @param nomFichier nom du fichier TLE
     * @param listeSatellites liste des numeros NORAD (si elle est vide on recupere tous les TLE)
     * @return tableau de TLE
     */
    static QMap<QString, TLE> LectureFichier(const QString &dirLocalData, const QString &nomFichier, const QStringList &listeSatellites = QStringList());

    /**
     * @brief LectureFichier3le Lecture du fichier 3le
     * @param nomFichier3le nom du fichier 3le
     * @param tabtle tableau de TLE
     */
    static QList<TLE> LectureFichier3le(const QString &nomFichier3le);

    /**
     * @brief LectureTrajectoryData Lecture du fichier Human Space Flight
     * @param fichierHsf nom du fichier html
     * @param fichier3le nom du fichier 3le
     * @param tabManoeuvres tableau des manoeuvres
     */
    static void LectureTrajectoryData(const QString &fichierHsf, const QString &fichier3le, QStringList &tabManoeuvres);

    /**
     * @brief MiseAJourFichier Mise a jour du fichier TLE
     * @param dirLocalData chemin de donnees locales
     * @param ficOld fichier avec les anciens TLE
     * @param ficNew fichier avec les nouveaux TLE
     * @param affMsg affichage des messages
     * @param compteRendu compte rendu de mise a jour
     */
    static void MiseAJourFichier(const QString &dirLocalData, const QString &ficOld, const QString &ficNew, const int affMsg, QStringList &compteRendu);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    unsigned int _nbOrbitesEpoque;
    Date _dateDebutValidite;

    double _argpo;
    double _bstar;
    double _ecco;
    double _inclo;
    double _mo;
    double _no;
    double _omegao;
    Date _epoque;

    QString _nom;
    QString _ligne0;
    QString _ligne1;
    QString _ligne2;
    QString _norad;
    QString _cospar;

    double _ndt20;
    double _ndd60;

    Donnees _donnees;


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
     * @brief RecupereNomsat Recupere le nom du satellite
     * @param lig0 ligne 0 du TLE
     * @return nom du satellite
     */
    static QString RecupereNomsat(const QString &lig0);

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
