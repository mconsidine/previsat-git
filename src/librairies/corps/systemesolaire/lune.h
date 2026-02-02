/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    lune.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position de la Lune
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    24 mars 2025
 *
 */

#ifndef LUNE_H
#define LUNE_H

#include "librairies/corps/corps.h"
#include "luneconst.h"


class Date;
class Soleil;

class Lune : public Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Lune Constructeur par defaut
     */
    Lune();


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculDatesEclipses Calcul des dates des prochaines eclipses de Lune et de Soleil
     * @param[in] date date
     */
    void CalculDatesEclipses(const Date &date);

    /**
     * @brief CalculDatesPhases Calcul des dates des phases lunaires
     * @param[in] date date
     */
    void CalculDatesPhases(const Date &date);

    /**
     * @brief CalculMagnitude Calcul de la magnitude visuelle de la Lune
     * @param[in] soleil Soleil
     */
    void CalculMagnitude(const Soleil &soleil);

    /**
     * @brief CalculPhase Calcul de la phase actuelle de la Lune
     * @param[in] soleil
     */
    void CalculPhase(const Soleil &soleil);

    /**
     * @brief CalculPositionSimp Calcul simplifie de la position de la Lune
     * @cite Astronomical Algorithms 2nd edition de Jean Meeus, pp337-342
     * @param[in] date date
     */
    void CalculPositionSimp(const Date &date);

    /**
     * @brief CalculPosVit Calcul de la position-vitesse du corps
     * @param[in] date date
     */
    void CalculPosVit(const Date &date) override {
        CalculPositionSimp(date);
    }


    /*
     * Accesseurs
     */
    bool luneCroissante() const;
    double anglePhase() const;
    double fractionIlluminee() const;
    double magnitude() const;
    QString phase() const;
    std::array<QString, LUNE::NB_PHASES> datesPhases() const;

    QString dateEclipseSoleil() const;
    QString typeEclipseSoleil() const;
    QString tooltipEclipseSoleil() const;
    QString dateEclipseLune() const;
    QString typeEclipseLune() const;
    QString tooltipEclipseLune() const;


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
    bool _luneCroissante;
    double _anglePhase;
    double _fractionIlluminee;
    double _magnitude;

    double _dj;
    double _p;
    double _q;
    double _gamma;
    double _u;
    QString _dateEclipseSoleil;
    QString _typeEclipseSoleil;
    QString _tooltipEclipseSoleil;
    QString _dateEclipseLune;
    QString _typeEclipseLune;
    QString _tooltipEclipseLune;

    QString _phase;
    std::array<QString, LUNE::NB_PHASES> _datesPhases;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculCaracteristiquesEclipses Calcul des caracteristiques des eclipses de Soleil et de Lune
     * @param[in] phase phase (0 = Nouvelle Lune; 2 = Pleine Lune)
     * @param[in] jj jour julien de l'eclipse
     * @param[in/out] k indice de la Nouvelle Lune
     * @return
     */
    bool CalculCaracteristiquesEclipses(const unsigned int phase,
                                        const double jj,
                                        double &k);

    /**
     * @brief CalculElementsEclipses Calcul des elements communs aux 2 types d'eclipse (Soleil et Lune)
     * @param[in] k indice de la Nouvelle Lune
     * @param[in] f argument de latitude de la Lune
     * @param[in] phase phase (0 = Nouvelle Lune; 2 = Pleine Lune)
     */
    void CalculElementsEclipses(const double k,
                                const double f,
                                unsigned int phase);

    /**
     * @brief CalculEphemLeverMeridienCoucher Calcul des ephemerides de la Lune pour determiner les heures de lever/meriden/coucher
     * @param[in] date date
     * @param[in] observateur observateur
     */
    void CalculEphemLeverMeridienCoucher(const Date &date,
                                         const Observateur &observateur) override;

    /**
     * @brief CalculIndicePhase Calcul de l'indice de la Nouvelle Lune (0 pour la premiere Nouvelle Lune de 2000)
     * @param[in] annee annee
     * @param[in] phase phase (0 = Nouvelle Lune; 2 = Pleine Lune)
     * @return indice de la Nouvelle Lune
     */
    double CalculIndicePhase(const double annee,
                             const unsigned int phase);

    /**
     * @brief CalculJourJulienPhase Calcul du jour julien approximatif d'une phase lunaire
     * @param[in] k nombre de lunaison depuis le 1er janvier 2000 (sans partie fractionnaire pour la nouvelle lune, +0.25 pour le premier quartier, etc.)
     * @return jour julien approximatif de la phase
     */
    double CalculJourJulienPhase(const double k);


};

#endif // LUNE_H
