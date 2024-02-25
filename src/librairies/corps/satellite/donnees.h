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
 * >    1er octobre 2023
 *
 */

#ifndef DONNEES_H
#define DONNEES_H

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
     * @param[in] donnee informations sur le satellite
     */
    explicit Donnees(const QString &donnee);


    /*
     * Methodes publiques
     */

    /*
     * Accesseurs
     */
    char methMagnitude() const;
    char stsDateRentree() const;
    char stsHeureRentree() const;
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
    char _methMagnitude;
    char _stsDateRentree;
    char _stsHeureRentree;
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


};

#endif // DONNEES_H
