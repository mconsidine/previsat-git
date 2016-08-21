/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2016  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    satellite.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >    Corps
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    21 aout 2016
 *
 */

#ifndef SATELLITE_H
#define SATELLITE_H

#include "elementsosculateurs.h"
#include "sgp4.h"
#include "tle.h"
#include "librairies/corps/corps.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"

class Satellite : public Corps
{
public:

    /* Constructeurs */
    /**
     * @brief Satellite Constructeur par defaut
     */
    Satellite();

    /**
     * @brief Satellite Constructeur a partir d'un TLE
     * @param xtle tle
     */
    explicit Satellite(const TLE &xtle) : _typeEclipseSoleil(NON_ECLIPSE), _typeEclipseLune(NON_ECLIPSE), _tle(xtle) {

        _eclipseTotale = true;
        _eclipsePartielle = false;
        _eclipseAnnulaire = false;
        _luminositeEclipseSoleil = 1.;
        _luminositeEclipseLune = 1.;
        _phiSoleil = 0.;
        _phiSoleilRefr = 0.;
        _phiTerre = 0.;
        _phiLune = 0.;
        _elongationSoleil = 0.;
        _elongationLune = 0.;

        _ieralt = true;
        _methMagnitude = 'v';
        _nbOrbites = 0;
        _ageTLE = 0.;
        _attenuation = 0.;
        _beta = 0.;
        _delai = 0.;
        _doppler = 0.;
        _fractionIlluminee = 0.;
        _magnitude = 99.;
        _magnitudeStandard = 99.;
        _section = 0.;
        _t1 = 0.;
        _t2 = 0.;
        _t3 = 0.;
    }

    ~Satellite();

    /* Constantes publiques */

    /* Variables publiques */
    static bool initCalcul;

    /* Methodes publiques */
    /**
     * @brief CalculBeta Calcul de l'angle beta (angle entre le plan de l'orbite et la direction du Soleil)
     * @param soleil Soleil
     */
    void CalculBeta(const Soleil &soleil);

    /**
     * @brief CalculCercleAcquisition Calcul du cercle d'acquisition d'une station
     * @param station sation
     */
    void CalculCercleAcquisition(const Observateur &station);

    /**
     * @brief CalculDateAOSSuiv Calcul de la date du prochain AOS ou LOS
     * @param dateCalcul date
     * @param observateur observateur
     * @param ctypeAOS type d'evenement (AOS ou LOS)
     * @param azimAOS azimut de l'evenement
     * @param aos vrai si le satellite peut apparaitre dans le ciel de l'observateur
     * @return date du prochain AOS/LOS
     */
    Date CalculDateAOSSuiv(const Date &dateCalcul, const Observateur &observateur, QString &ctypeAOS, double &azimAOS, bool &aos);

    /**
     * @brief CalculDateNoeudAscPrec Calcul de la date du noeud ascendant precedent a la date donnee
     * @param date date
     * @return date du noeud ascendant precedent
     */
    Date CalculDateNoeudAscPrec(const Date &date);

    /**
     * @brief CalculDateOmbrePenombreSuiv Calcul de la date du prochain passage ombre->penombre ou penombre->ombre
     * @param date date
     * @param nbTrajectoires nombre de traces au sol
     * @param acalcEclipseLune prise en compte des eclipses de Lune
     * @param refraction prise en compte de la refraction atmospherique
     * @return date du prochain passage ombre->penombre ou penombre->ombre
     */
    Date CalculDateOmbrePenombreSuiv(const Date &date, const int nbTrajectoires, const bool acalcEclipseLune, const bool refraction);

    /**
     * @brief CalculElementsOsculateurs Calcul des elements osculateurs et du numero d'orbite
     * @param date date
     */
    void CalculElementsOsculateurs(const Date &date);

    /**
     * @brief CalculMagnitude Calcul de la magnitude visuelle du satellite
     * @param observateur observateur
     * @param extinction prise en compte de l'extinction atmospherique
     * @param effetEclipsePartielle prise en compte de l'eclipse partielle
     */
    void CalculMagnitude(const Observateur &observateur, const bool extinction, const bool effetEclipsePartielle);

    /**
     * @brief CalculPosVit Calcul de la position et de la vitesse du satellite
     * Modele SGP4 : d'apres l'article "Revisiting Spacetrack Report #3: Rev 1" de David Vallado (2006)
     * @param date date
     */
    void CalculPosVit(const Date &date);

    /**
     * @brief CalculPosVitListeSatellites Calcul de la position d'une liste de satellites
     * @param date date
     * @param observateur observateur
     * @param soleil Soleil
     * @param nbTracesAuSol nombre de traces au sol
     * @param visibilite Calcul de la zone de visibilite des satellites
     * @param extinction Prise en compte de l'extinction atmospherique
     * @param traceCiel Calcul de la trace du satellite dans le ciel de l'observateur
     * @param mcc Calcul des cercles des satellites TDRS
     * @param refraction Prise en compte de la refraction atmospherique
     * @param acalcEclipseLune prise en compte des eclipses de Lune
     * @param effetEclipsePartielle prise en compte de l'eclipse partielle pour le calcul de la magnitude
     * @param satellites liste de satellites
     */
    static void CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil, const Lune &lune,
                                            const int nbTracesAuSol, const bool visibilite, const bool extinction, const bool traceCiel,
                                            const bool mcc, const bool refraction, const bool acalcEclipseLune,
                                            const bool effetEclipsePartielle, QList<Satellite> &satellites);

    /**
     * @brief CalculSatelliteEclipse Determination de la condition d'eclipse du satellite
     * @param soleil Soleil
     * @param lune Lune
     * @param acalcEclipseLune prise en compte des eclipses de Lune
     * @param refraction Prise en compte de la refraction atmospherique
     */
    void CalculSatelliteEclipse(const Soleil &soleil, const Lune &lune, const bool acalcEclipseLune, const bool refraction = true);

    /**
     * @brief CalculSatelliteEclipseCorps Calcul de la condition d'eclipse du satellite par un corps occultant
     * @param soleil Soleil
     * @param corpsOccultant corps occultant
     * @param lune Lune
     * @param refraction Prise en compte de la refraction atmospherique
     */
    void CalculSatelliteEclipseCorps(const Soleil &soleil, const CorpsOccultant &corpsOccultant, const Lune &lune = Lune(),
                                     const bool refraction = true);

    /**
     * @brief CalculTraceCiel Calcul de la trace dans le ciel
     * @param date date
     * @param acalcEclipseLune prise en compte des eclipses de Lune
     * @param refraction Prise en compte de la refraction atmospherique
     * @param observateur observateur
     * @param sec parametre pour les satellites geostationnaires
     */
    void CalculTraceCiel(const Date &date, const bool acalcEclipseLune, const bool refraction, const Observateur &observateur,
                         const int sec = 0);

    /**
     * @brief hasAOS Determination si le satellite peut se lever (ou se coucher) dans le ciel de l'observateur
     * @param observateur observateur
     * @return vrai si le satellite peut se lever (ou se coucher) dans le ciel de l'observateur
     */
    bool hasAOS(const Observateur &observateur) const;

    /**
     * @brief isGeo Determination si le satellite est geosynchrone
     * @return vrai si le satellite est geosynchrone
     */
    bool isGeo() const;

    /**
     * @brief LectureDonnees Lecture du fichier de donnees des satellites
     * @param listeSatellites liste des numeros NORAD
     * @param tabtle tableau des TLE correspondants
     * @param satellites liste de satellites
     */
    static void LectureDonnees(const QStringList &listeSatellites, const QVector<TLE> &tabtle, QList<Satellite> &satellites);

    /* Accesseurs */
    bool isEclipseTotale() const;
    bool isEclipsePartielle() const;
    bool isEclipseAnnulaire() const;
    TypeEclipse typeEclipseSoleil() const;
    TypeEclipse typeEclipseLune() const;
    double luminositeEclipseSoleil() const;
    double luminositeEclipseLune() const;
    double elongationSoleil() const;
    double elongationLune() const;
    double phiSoleil() const;
    double phiSoleilRefr() const;
    double phiTerre() const;
    double phiLune() const;

    bool isIeralt() const;
    double ageTLE() const;
    double attenuation() const;
    double beta() const;
    double delai() const;
    double doppler() const;
    double fractionIlluminee() const;
    double magnitude() const;
    double magnitudeStandard() const;
    char methMagnitude() const;
    char method() const;
    int nbOrbites() const;
    double section() const;
    double t1() const;
    double t2() const;
    double t3() const;
    QString dateLancement() const;
    QString categorieOrbite() const;
    QString pays() const;
    QString siteLancement() const;

    TLE tle() const;
    ElementsOsculateurs elements() const;
    QList<QVector<double> > traceAuSol() const;
    QList<QVector<double> > traceCiel() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    bool _eclipseTotale;
    bool _eclipsePartielle;
    bool _eclipseAnnulaire;
    TypeEclipse _typeEclipseSoleil;
    TypeEclipse _typeEclipseLune;
    double _luminositeEclipseSoleil;
    double _luminositeEclipseLune;
    double _phiSoleil;
    double _phiSoleilRefr;
    double _phiTerre;
    double _phiLune;
    double _elongationSoleil;
    double _elongationLune;

    bool _ieralt;
    int _nbOrbites;
    double _ageTLE;
    double _beta;
    double _fractionIlluminee;
    double _magnitude;

    char _methMagnitude;
    double _doppler;
    double _attenuation;
    double _delai;
    double _magnitudeStandard;
    double _section;
    double _t1;
    double _t2;
    double _t3;
    QString _cospar;
    QString _dateLancement;
    QString _periode;
    QString _perigee;
    QString _apogee;
    QString _inclinaison;
    QString _categorieOrbite;
    QString _pays;
    QString _siteLancement;

    SGP4 _sgp4;
    TLE _tle;
    ElementsOsculateurs _elements;
    QList<QVector<double> > _traceAuSol;
    QList<QVector<double> > _traceCiel;

    /* Methodes privees */
    /**
     * @brief CalculSignal Calcul des proprietes du signal
     */
    void CalculSignal();

    /**
     * @brief CalculTracesAuSol Calcul de la trace au sol du satellite
     * @param date date
     * @param nbOrbites nombre de traces au sol
     * @param acalcEclipseLune prise en compte des eclipses de Lune
     * @param refraction Prise en compte de la refraction atmospherique
     */
    void CalculTracesAuSol(const Date &date, const int nbOrbites, const bool acalcEclipseLune, const bool refraction);

};

#endif // SATELLITE_H
