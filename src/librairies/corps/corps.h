/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    corps.h
 *
 * Localisation
 * >    librairies.corps
 *
 * Heritage
 * >
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    19 juin 2022
 *
 */

#ifndef CORPS_H
#define CORPS_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QPointF>
#pragma GCC diagnostic warning "-Wconversion"
#include <QString>
#include <QVector>
#include "librairies/maths/vecteur3d.h"


class Date;
class Observateur;

class Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Corps Constructeur par defaut
     */
    Corps();

    /*
     * Accesseurs
     */
    double altitude() const;
    double ascensionDroite() const;
    double azimut() const;
    QString constellation() const;
    double declinaison() const;
    Vecteur3D dist() const;
    double distance() const;
    double hauteur() const;
    double latitude() const;
    double longitude() const;
    double lonEcl() const;
    Vecteur3D position() const;
    double rangeRate() const;
    bool isVisible() const;
    Vecteur3D vitesse() const;
    QVector<QPointF> zone() const;


    /*
     * Modificateurs
     */
    void setPosition(const Vecteur3D &pos);


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
     * @brief CalculAltitude Calcul de l'altitude du corps
     * @param pos position cartesienne du corps
     * @return altitude (km)
     */
    double CalculAltitude(const Vecteur3D &pos);

    /**
     * @brief CalculCoordEquat Calcul des coordonnees equatoriales
     * @param observateur observateur
     * @param determinationConstellation determination de la constellation
     */
    void CalculCoordEquat(const Observateur &observateur, const bool determinationConstellation = true);

    /**
     * @brief CalculCoordHoriz Calcul des coordonnees horizontales
     * @param observateur observateur
     * @param acalc calcul de l'azimut
     * @param arefr prise en compte de la refraction
     * @param aos gestion de la refraction dans le cas du calcul de lever/coucher
     */
    void CalculCoordHoriz(const Observateur &observateur, const bool acalc = true, const bool arefr = true, const bool aos = false);

    /**
     * @brief CalculCoordHoriz2 Calcul des coordonnees horizontales (avec condition de visibilite)
     * @param observateur observateur
     */
    void CalculCoordHoriz2(const Observateur &observateur);

    /**
     * @brief CalculCoordTerrestres Calcul des coordonnees terrestres du corps a la date courante
     * @param observateur observateur
     */
    void CalculCoordTerrestres(const Observateur &observateur);

    /**
     * @brief CalculCoordTerrestres Calcul des coordonnees terrestres du corps a une date donnee
     * @param date date
     */
    void CalculCoordTerrestres(const Date &date);

    /**
     * @brief CalculLatitude Calcul de la latitude geodesique du corps
     * @param pos position cartesienne du corps
     * @return latitude (radian)
     */
    double CalculLatitude(const Vecteur3D &pos);

    /**
     * @brief CalculPosVitECEF Calcul de la position et de la vitesse dans le repere ECEF
     * @param date date
     * @param positionECEF position dans le repere ECEF
     * @param vitesseECEF vitesse dans le repere ECEF
     */
    void CalculPosVitECEF(const Date &date, Vecteur3D &positionECEF, Vecteur3D &vitesseECEF) const;

    /**
     * @brief CalculRefractionAtmospherique Calcul de la refraction atmospherique
     * @param ht hauteur du corps (en radians)
     * @return refraction atmospherique (en radians)
     */
    double CalculRefractionAtmospherique(const double ht) const;

    /**
     * @brief CalculZoneVisibilite Calcul de la zone de visibilite du corps
     * @param beta rayon apparent du corps
     */
    void CalculZoneVisibilite(const double beta);

    /**
     * @brief InitTabConstellations Lecture du fichier de constellations
     * @param dirCommonData chemin du fichier de constellations
     */
    static void InitTabConstellations(const QString &dirCommonData);

    /**
     * @brief Sph2Cart Conversion d'un vecteur en coordonnees ecliptiques spheriques en coordonnees cartesiennes equatoriales
     * @param vecteur vecteur en coordonnees ecliptiques spheriques
     * @param date date
     * @return vecteur en coordonnees cartesiennes equatoriales
     */
    Vecteur3D Sph2Cart(const Vecteur3D &vecteur, const Date &date);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */
    // Coordonnees horizontales
    double _azimut;
    double _hauteur;
    double _distance;

    // Coordonnees equatoriales
    double _ascensionDroite;
    double _declinaison;
    QString _constellation;

    // Coordonnees terrestres
    double _longitude;
    double _latitude;
    double _altitude;

    // Coordonnees ecliptiques
    double _lonEcl;
    double _latEcl;

    bool _visible;
    double _rangeRate;

    // Coordonnees cartesiennes
    Vecteur3D _position;
    Vecteur3D _vitesse;
    Vecteur3D _dist;

    // Zone de visibilite
    QVector<QPointF> _zone;

    double _r0;
    double _ct;


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
    Vecteur3D _vec1;

    /*
     * Methodes privees
     */
    /**
     * @brief CalculLatitudeAltitude Calcul de la latitude et de l'altitude
     */
    void CalculLatitudeAltitude();

};


#endif // CORPS_H
