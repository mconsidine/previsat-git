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
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    2 avril 2018
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


struct ElementsTraceSol {
    double longitude;
    double latitude;
    double jourJulienUTC;
    bool eclipseTotale;
    bool eclipsePartielle;
};

class Satellite : public Corps
{
#if BUILDTEST == true
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
    explicit Satellite(const QVector<TLE> &tabtle);


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
//    QList<QVector<double> > traceCiel() const;


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
     * @brief CalculPosVit Calcul de la position et de la vitesse du satellite
     * Modele SGP4 : d'apres l'article "Revisiting Spacetrack Report #3: Rev 1" de David Vallado (2006)
     * @param date date
     */
    void CalculPosVit(const Date &date);

    static void CalculPosVitListeSatellites(const Date &date, const Observateur &observateur, const Soleil &soleil, const Lune &lune,
                                            const int nbTracesAuSol, const bool acalcEclipseLune, const bool effetEclipsePartielle,
                                            const bool extinction, const bool refraction, const bool visibilite, QList<Satellite> &satellites);

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
    QVector<TLE> _tabtle;

    ElementsOsculateurs _elements;
    Phasage _phasage;
    Signal _signal;

    ConditionEclipse _conditionEclipse;
    Magnitude _magnitude;

    QList<ElementsTraceSol> _traceAuSol;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculElementsOsculateurs Calcul des elements osculateurs
     * @param date date
     */
    void CalculElementsOsculateurs(const Date &date);


};

#endif // SATELLITE_H
