/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    1er janvier 2025
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
#include "elementsorbitaux.h"
#include "satelliteconst.h"


class Satellite : public Corps
{
#if (BUILD_TEST)
    friend class GeneralTest;
    friend class OsculateursTest;
    friend class SatelliteTest;
    friend class EvenementsTest;
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
     * @brief Satellite Constructeur a partir des elements orbitaux
     * @param[in] elem elements orbitaux
     */
    explicit Satellite(const ElementsOrbitaux &elem) :
        _elementsOrbitaux(elem) {

        _nbOrbites = 0;
        _ageElementsOrbitaux = 0.;
        _beta = 0.;
        _sgp4.setInit(false);
    }

    /**
     * @brief Satellite Constructeur a partir d'un tableau d'elements orbitaux
     * @param[in] listElements tableau d'elements orbitaux
     */
    explicit Satellite(const QList<ElementsOrbitaux> &listElements);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculBeta Calcul de l'angle beta (angle entre le plan de l'orbite et la direction du Soleil)
     * @param[in] soleil Soleil
     */
    void CalculBeta(const Soleil &soleil);

    /**
     * @brief CalculCercleAcquisition Calcul du cercle d'acquisition d'une station
     * @param[in] station sation
     */
    void CalculCercleAcquisition(const Observateur &station);

    /**
     * @brief CalculElementsOsculateurs Calcul des elements osculateurs
     * @param[in] date date
     */
    void CalculElementsOsculateurs(const Date &date);

    /**
     * @brief CalculPosVit Calcul de la position et de la vitesse du satellite
     * Modele SGP4 : d'apres l'article "Revisiting Spacetrack Report #3: Rev 3" de David Vallado (2006)
     * @param[in] date date
     */
    void CalculPosVit(const Date &date) override;

    /**
     * @brief CalculPosVitListeSatellites Calcul de la position d'une liste de satellites
     * @param[in] date date
     * @param[in] observateur observateur
     * @param[in] soleil soleil
     * @param[in] lune lune
     * @param[in] nbTracesAuSol nombre de traces au sol
     * @param[in] acalcEclipseLune calcul des eclipses produites par la Lune
     * @param[in] effetEclipsePartielle prise en compte de l'effet des eclipses partielles
     * @param[in] extinction prise en compte de l'extinction atmospherique
     * @param[in] refractionAtmospherique prise en compte de la refraction atmospherique
     * @param[in] traceCiel calcul de la trace dans le ciel
     * @param[in] visibilite calcul de la zone de visibilite
     * @param[in/out] satellites liste de satellites
     */
    static void CalculPosVitListeSatellites(const Date &date,
                                            const Observateur &observateur,
                                            const Soleil &soleil,
                                            const Lune &lune,
                                            const int nbTracesAuSol,
                                            const bool acalcEclipseLune,
                                            const bool effetEclipsePartielle,
                                            const bool extinction,
                                            const bool refractionAtmospherique,
                                            const bool traceCiel,
                                            const bool visibilite,
                                            const bool isISS,
                                            const bool mcc,
                                            QList<Satellite> &satellites);

    /**
     * @brief CalculTraceCiel Calcul de la trace dans le ciel
     * @param[in] date0 date initiale
     * @param[in] acalcEclipseLune calcul des eclipses produites par la Lune
     * @param[in] refraction prise en compte de la refraction atmospherique
     * @param[in] observateur observateur
     * @param[in] sec pas en secondes
     * @param[in] jj1 jour julien maximal
     */
    void CalculTraceCiel(const Date &date0,
        const bool acalcEclipseLune,
        const bool refraction,
        const Observateur &observateur,
        const int sec = 0,
        const double jj1 = DATE::DATE_INFINIE);

    /**
     * @brief CalculTracesAuSol Calcul des traces au sol
     * @param[in] dateInit date de depart
     * @param[in] nbOrb nombre d'orbites
     * @param[in] acalcEclipseLune calcul des eclipses produites par la Lune
     * @param[in] refraction prise en compte de la refraction atmospherique
     */
    void CalculTracesAuSol(const Date &dateInit,
                           const int nbOrb,
                           const bool acalcEclipseLune,
                           const bool refraction);

    /**
     * @brief hasAOS Determination si le satellite peut se lever (ou se coucher) dans le ciel de l'observateur
     * @param[in] observateur observateur
     * @return vrai si le satellite peut se lever (ou se coucher) dans le ciel de l'observateur
     */
    bool hasAOS(const Observateur &observateur) const;

    /**
     * @brief isGeo Determination si le satellite est geosynchrone
     * @return vrai si le satellite est geosynchrone
     */
    bool isGeo() const;


    /*
     * Accesseurs
     */
    double ageElementsOrbitaux() const;
    double beta() const;

    char method() const;
    int deltaNbOrb() const;
    unsigned int nbOrbites() const;

    ElementsOrbitaux elementsOrbitaux() const;
    ConditionEclipse conditionEclipse() const;
    ElementsOsculateurs elementsOsculateurs() const;
    Magnitude magnitude() const;
    Phasage phasage() const;
    Signal signal() const;
    QList<ElementsTraceSol> traceAuSol() const;
    QList<ElementsTraceCiel> traceCiel() const;


    /*
     * Modificateurs
     */
    void setConditionEclipse(const ConditionEclipse &condEcl);


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
    unsigned int _nbOrbites;
    double _ageElementsOrbitaux;
    double _beta;

    SGP4 _sgp4;
    ElementsOrbitaux _elementsOrbitaux;
    QList<ElementsOrbitaux> _listElements;

    ElementsOsculateurs _elementsOsculateurs;
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
