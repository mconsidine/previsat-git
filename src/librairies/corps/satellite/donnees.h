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
 * >    14 aout 2023
 *
 */

#ifndef DONNEES_H
#define DONNEES_H

#include <QString>


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
    double magnitudeStandard() const;
    double section() const;
    double t1() const;
    double t2() const;
    double t3() const;
    QString norad() const;
    const QString &cospar() const;
    const QString &dateLancement() const;
    const QString &dateRentree() const;
    const QString &periode() const;
    const QString &perigee() const;
    const QString &apogee() const;
    const QString &inclinaison() const;
    const QString &categorieOrbite() const;
    const QString &pays() const;
    const QString &siteLancement() const;
    QString nom() const;


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
    double _magnitudeStandard;
    double _section;
    double _t1;
    double _t2;
    double _t3;
    QString _norad;
    QString _cospar;
    QString _dateLancement;
    QString _dateRentree;
    QString _periode;
    QString _perigee;
    QString _apogee;
    QString _inclinaison;
    QString _categorieOrbite;
    QString _pays;
    QString _siteLancement;
    QString _nom;


    /*
     * Methodes privees
     */


};

#endif // DONNEES_H
