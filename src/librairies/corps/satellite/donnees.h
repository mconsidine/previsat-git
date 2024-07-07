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
 * >    donnees.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Donnees des satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    20 novembre 2019
 *
 * Date de revision
 * >    7 juillet 2024
 *
 */

#ifndef DONNEES_H
#define DONNEES_H

#include <QtSql>
#include <QString>
#include "librairies/dates/date.h"


class Donnees
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Donnees Constructeur par defaut
     */
    Donnees();

    /**
     * @brief Donnees Definition a partir des informations
     * @param[in] req requete
     */
    Donnees(const QSqlQuery &req);


    /*
     * Methodes publiques
     */
    /**
     * @brief RequeteCospar Recherche des donnees satellites par designation COSPAR
     * @param[in] db base de donnees
     * @param[in] cospar designation COSPAR
     * @return Liste des donnees satellites
     */
    static QList<Donnees> RequeteCospar(const QSqlDatabase &db, const QString &cospar);

    /**
     * @brief RequeteNom Recherche des donnees satellites par nom de satellite
     * @param[in] db base de donnees
     * @param[in] nom nom
     * @return Liste des donnees satellites
     */
    static QList<Donnees> RequeteNom(const QSqlDatabase &db, const QString &nom);

    /**
     * @brief RequeteNorad Recherche des donnees satellites par numero NORAD
     * @param[in] db base de donnees
     * @param[in] norad numero NORAD
     * @return Liste des donnees satellites
     */
    static QList<Donnees> RequeteNorad(const QSqlDatabase &db, const QString &norad);


    /*
     * Accesseurs
     */
    QString methMagnitude() const;
    QString stsDateRentree() const;
    QString stsHeureRentree() const;
    double magnitudeStandard() const;
    double longueur() const;
    double diametre() const;
    double envergure() const;
    QString periode() const;
    double perigee() const;
    double apogee() const;
    QString inclinaison() const;
    QString norad() const;
    QString cospar() const;
    QString dateLancement() const;
    QString dateRentree() const;
    QString masseSec() const;
    QString masseTot() const;
    QString forme() const;
    QString classe() const;
    QString categorie() const;
    QString discipline() const;
    QString orbite() const;
    QString pays() const;
    QString siteLancement() const;
    QString nom() const;


    /*
     * Modificateurs
     */
    void setMagnitudeStandard(const double m);


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
    QString _methMagnitude;
    QString _stsDateRentree;
    QString _stsHeureRentree;
    double _magnitudeStandard;
    double _longueur;
    double _diametre;
    double _envergure;
    QString _periode;
    double _perigee;
    double _apogee;
    QString _inclinaison;
    QString _norad;
    QString _cospar;
    QString _dateLancement;
    QString _dateRentree;
    QString _masseSec;
    QString _masseTot;
    QString _forme;
    QString _classe;
    QString _categorie;
    QString _discipline;
    QString _orbite;
    QString _pays;
    QString _siteLancement;
    QString _nom;


    /*
     * Methodes privees
     */
    /**
     * @brief Requete Requete generique dans la base de donnees
     * @param db base de donnees
     * @param requete requete
     * @return Liste des donnees satellites
     */
    static QList<Donnees> Requete(const QSqlDatabase &db, const QString &requete);


};

#endif // DONNEES_H
