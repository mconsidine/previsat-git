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
 * >    25 fevrier 2023
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
     * @brief CalculDatesPhases Calcul des dates des phases lunaires
     * @param date date
     */
    void CalculDatesPhases(const Date &date, const DateSysteme &syst);

    /**
     * @brief CalculLeverMeridienCoucher Calcul des heures de lever, passage au meridien et coucher
     * @param date date
     * @param observateur observateur
     */
    void CalculLeverMeridienCoucher(const Date &date, const Observateur &observateur, const DateSysteme &syst);

    /**
     * @brief CalculMagnitude Calcul de la magnitude visuelle de la Lune
     * @param soleil Soleil
     */
    void CalculMagnitude(const Soleil &soleil);

    /**
     * @brief CalculPhase Calcul de la phase actuelle de la Lune
     * @param soleil
     */
    void CalculPhase(const Soleil &soleil);

    /**
     * @brief CalculPosition Calcul de la position de la Lune avec le modele simplifie issu de
     * l'Astronomical Algorithms 2nd edition de Jean Meeus, pp337-342
     * @param date date
     */
    void CalculPosition(const Date &date);


    /*
     * Accesseurs
     */
    bool luneCroissante() const;
    double anglePhase() const;
    double fractionIlluminee() const;
    double magnitude() const;
    const QString &phase() const;
    const std::array<QString, LUNE::NB_PHASES> &datesPhases() const;


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
    QString _phase;
    std::array<QString, LUNE::NB_PHASES> _datesPhases;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculJourJulienPhase Calcul du jour julien approximatif d'une phase lunaire
     * @param k nombre de lunaison depuis le 1er janvier 2000 (sans partie fractionnaire pour la nouvelle lune, +0.25 pour le premier quartier, etc.)
     * @return jour julien approximatif de la phase
     */
    double CalculJourJulienPhase(const double k);


};

#endif // LUNE_H
