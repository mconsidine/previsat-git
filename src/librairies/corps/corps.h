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
 * >    12 mars 2023
 *
 */

#ifndef CORPS_H
#define CORPS_H

#include <QList>
#include <QPointF>
#include <QString>
#include "librairies/maths/vecteur3d.h"
#include "corpsconst.h"
#include "ephemerides.h"


class Date;
class Observateur;
enum class DateSysteme;

class Corps
{
public:

    /*
     *  Constructeurs
     */
    Corps() {
        _azimut = 0.;
        _hauteur = 0.;
        _distance = 0.;

        _ascensionDroite = 0.;
        _declinaison = 0.;

        _longitude = 0.;
        _latitude = 0.;
        _altitude = 0.;

        _visible = false;
        _rangeRate = 0.;

        _lonEcl = 0.;
        _latEcl = 0.;
        _ct = 0.;
        _r0 = 0.;
    }

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculAltitude Calcul de l'altitude du corps
     * @return altitude (km)
     */
    void CalculAltitude();

    /**
     * @brief CalculCoordEquat Calcul des coordonnees equatoriales
     * @param[in] observateur observateur
     * @param[in] determinationConstellation determination de la constellation
     * @throw Exception
     */
    void CalculCoordEquat(const Observateur &observateur,
                          const bool determinationConstellation = true);

    /**
     * @brief CalculCoordHoriz Calcul des coordonnees horizontales
     * @param[in] observateur observateur
     * @param[in] acalc calcul de l'azimut
     * @param[in] arefr prise en compte de la refraction
     * @param[in] aos gestion de la refraction dans le cas du calcul de lever/coucher
     */
    void CalculCoordHoriz(const Observateur &observateur,
                          const bool acalc/* = true*/,
                          const bool arefr = true,
                          const bool aos = false);

    // TODO
    /**
     * @brief CalculCoordHoriz Calcul des coordonnees horizontales (avec condition de visibilite)
     * @param[in] observateur observateur
     */
    void CalculCoordHoriz3(const Observateur &observateur);

    /**
     * @brief CalculCoordTerrestres Calcul des coordonnees terrestres du corps a la date courante
     * @param[in] observateur observateur
     */
    void CalculCoordTerrestres(const Observateur &observateur);

    /**
     * @brief CalculCoordTerrestres Calcul des coordonnees terrestres du corps a une date donnee
     * @param[in] date date
     */
    void CalculCoordTerrestres(const Date &date);

    /**
     * @brief CalculLatitude Calcul de la latitude geodesique du corps
     */
    void CalculLatitude();

    /**
     * @brief CalculLeverMeridienCoucher Calcul des lever/passage au meridien/coucher
     * @param[in] date date
     * @param[in] syst systeme horaire
     * @param[in] observateur observateur
     * @param[in] calculCrepuscules calcul des crepuscules dans le cas du Soleil
     */
    void CalculLeverMeridienCoucher(const Date &date,
                                    const DateSysteme &syst,
                                    const Observateur &observateur,
                                    const bool calculCrepuscules = true);

    /**
     * @brief CalculPosVit Calcul de la position-vitesse du corps
     * @param date date
     */
    virtual void CalculPosVit(const Date &date) {
        Q_UNUSED(date)
    }

    /**
     * @brief CalculPosVitECEF Calcul de la position et de la vitesse dans le repere ECEF
     * @param[in] date date
     * @param[out] positionECEF position dans le repere ECEF
     * @param[out] vitesseECEF vitesse dans le repere ECEF
     */
    void CalculPosVitECEF(const Date &date,
                          Vecteur3D &positionECEF,
                          Vecteur3D &vitesseECEF) const;

    /**
     * @brief CalculRefractionAtmospherique Calcul de la refraction atmospherique
     * @param[in] ht hauteur du corps (en radians)
     * @return refraction atmospherique (en radians)
     */
    double CalculRefractionAtmospherique(const double ht) const;

    /**
     * @brief CalculZoneVisibilite Calcul de la zone de visibilite du corps
     * @param[in] beta rayon apparent du corps
     */
    void CalculZoneVisibilite(const double beta);

    /**
     * @brief Initialisation Initialisation des elements relatifs aux corps (satellites, systeme solaire, etc.)
     * @param dirCommonData repertoire ou se trouve le fichier constellations.dat
     * @throw Exception
     */
    static void Initialisation(const QString &dirCommonData);

    /**
     * @brief Sph2Cart Conversion d'un vecteur en coordonnees ecliptiques spheriques en coordonnees cartesiennes equatoriales
     * @param[in] vecteur vecteur en coordonnees ecliptiques spheriques
     * @param[in] date date
     * @return vecteur en coordonnees cartesiennes equatoriales
     */
    Vecteur3D Sph2Cart(const Vecteur3D &vecteur,
                       const Date &date) const;


    /*
     * Accesseurs
     */
    double hauteur() const;
    double azimut() const;
    double distance() const;

    double ascensionDroite() const;
    double declinaison() const;
    QString constellation() const;

    double longitude() const;
    double latitude() const;
    double altitude() const;

    Vecteur3D position() const;
    Vecteur3D vitesse() const;
    Vecteur3D dist() const;

    double lonEcl() const;

    bool visible() const;
    double rangeRate() const;

    std::array<QPointF, 360> zone() const;

    QString dateLever() const;
    QString dateMeridien() const;
    QString dateCoucher() const;
    std::array<QString, 6> datesCrepuscules() const;


    /*
     * Modificateurs
     */
    void setPosition(const Vecteur3D &pos);


protected:

    /*
     * Variables protegees
     */
    // Coordonnees horizontales
    double _hauteur;
    double _azimut;
    double _distance;

    // Coordonnees equatoriales
    double _ascensionDroite;
    double _declinaison;
    QString _constellation;

    // Coordonnees terrestres
    double _longitude;
    double _latitude;
    double _altitude;

    // Coordonnees cartesiennes
    Vecteur3D _position;
    Vecteur3D _vitesse;
    Vecteur3D _dist;

    // Coordonnees ecliptiques
    double _lonEcl;
    double _latEcl;

    bool _visible;
    double _rangeRate;

    std::array<QPointF, 360> _zone;

    // Dates de lever, passage au meridien et coucher
    QList<Ephemerides> _ephem;
    QString _dateLever;
    QString _dateMeridien;
    QString _dateCoucher;

    // Dates de crepuscules
    std::array<QString, 6> _datesCrepuscules;

    /*
     * Methodes protegees
     */
    std::array<double, 3> CalculAnglesReductionEquat(const Date &date);

    virtual void CalculEphemLeverMeridienCoucher(const Date &date,
                                                 const Observateur &observateur) {
        Q_UNUSED(date)
        Q_UNUSED(observateur)
    }


private:

    /*
     * Variables privees
     */

    double _ct;
    double _r0;

    /*
     * Methodes privees
     */
    /**
     * @brief CalculLatitudeAltitude Calcul de la latitude et de l'altitude
     */
    void CalculLatitudeAltitude();

};

#endif // CORPS_H
