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
 * >    satellite.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position des satellites
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    4 octobre 2020
 *
 */

#ifndef SATELLITE_H
#define SATELLITE_H

#include "librairies/corps/corps.h"
#include "conditioneclipse.h"
#include "elementsosculateurs.h"
#include "magnitude.h"
#include "phasage.h"
#include "sgp4.h"
#include "signal.h"
#include "tle.h"


struct ElementsTraceCiel {
    double hauteur;
    double azimut;
    double jourJulienUTC;
    bool eclipseTotale;
    bool eclipsePartielle;
};

struct ElementsTraceSol {
    double longitude;
    double latitude;
    double jourJulienUTC;
    bool eclipseTotale;
    bool eclipsePartielle;
};

class Satellite : public Corps
{
#if BUILD_TEST == true
    friend class SatelliteTest;
#endif
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Satellite Constructeur par defaut
     */
    Satellite();

    /**
     * @brief Satellite Constructeur a partir d'un TLE
     * @param xtle TLE
     */
    explicit Satellite(const TLE &xtle) :
        _tle(xtle) {

        _nbOrbites = 0;
        _ageTLE = 0.;
        _beta = 0.;
        _deltaNbOrb = -1;
    }

    /**
     * @brief Satellite Constructeur a partir d'un tableau de TLE
     * @param tabtle tableau de TLE
     */
    explicit Satellite(const QList<TLE> &tabtle);


    /*
     * Accesseurs
     */
    double ageTLE() const;
    double beta() const;

    char method() const;
    int deltaNbOrb() const;
    unsigned int nbOrbites() const;

    TLE tle() const;
    ConditionEclipse conditionEclipse() const;
    ElementsOsculateurs elements() const;
    Magnitude magnitude() const;
    Phasage phasage() const;
    Signal signal() const;
    QList<ElementsTraceSol> traceAuSol() const;
    QList<ElementsTraceCiel> traceCiel() const;


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
     * @brief CalculBeta Calcul de l'angle beta (angle entre le plan de l'orbite et la direction du Soleil)
     * @param soleil Soleil
     */
    void CalculBeta(const Soleil &soleil);

    /**
     * @brief CalculElementsOsculateurs Calcul des elements osculateurs
     * @param date date
     */
    void CalculElementsOsculateurs(const Date &date);

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
     * @param soleil soleil
     * @param lune lune
     * @param nbTracesAuSol nombre de traces au sol
     * @param acalcEclipseLune calcul des eclipses produites par la Lune
     * @param effetEclipsePartielle prise en compte de l'effet des eclipses partielles
     * @param extinction prise en compte de l'extinction atmospherique
     * @param refraction prise en compte de la refraction atmospherique
     * @param traceCiel calcul de la trace dans le ciel
     * @param visibilite calcul de la zone de visibilite
     * @param satellites liste de satellites
     */
    static void CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil, const Lune &lune,
                                            const int nbTracesAuSol, const bool acalcEclipseLune, const bool effetEclipsePartielle,
                                            const bool extinction, const bool refraction, const bool traceCiel, const bool visibilite,
                                            QList<Satellite> &satellites);

    /**
     * @brief CalculTraceCiel Calcul de la trace dans le ciel
     * @param date date
     * @param acalcEclipseLune calcul des eclipses produites par la Lune
     * @param refraction prise en compte de la refraction atmospherique
     * @param observateur observateur
     * @param sec pas en secondes
     */
    void CalculTraceCiel(const Date &date, const bool acalcEclipseLune, const bool refraction, const Observateur &observateur, const int sec = 0);

    /**
     * @brief CalculTracesAuSol Calcul des traces au sol
     * @param dateInit date de depart
     * @param nbOrb nombre d'orbites
     * @param acalcEclipseLune calcul des eclipses produites par la Lune
     * @param refraction prise en compte de la refraction atmospherique
     */
    void CalculTracesAuSol(const Date &dateInit, const int nbOrb, const bool acalcEclipseLune, const bool refraction);

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
    int _deltaNbOrb;
    unsigned int _nbOrbites;
    double _ageTLE;
    double _beta;

    SGP4 _sgp4;
    TLE _tle;
    QList<TLE> _tabtle;

    ElementsOsculateurs _elements;
    Phasage _phasage;
    Signal _signal;

    ConditionEclipse _conditionEclipse;
    Magnitude _magnitude;

    QList<ElementsTraceSol> _traceAuSol;
    QList<ElementsTraceCiel> _traceCiel;


    /*
     * Methodes privees
     */


};

#endif // SATELLITE_H
