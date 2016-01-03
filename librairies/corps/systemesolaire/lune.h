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
 * >    lune.h
 *
 * Localisation
 * >    librairies.corps.systemesolaire
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
 * >    3 juin 2015
 *
 */

#ifndef LUNE_H
#define LUNE_H

#include "librairies/corps/corps.h"
#include "LuneConstants.h"
#include "soleil.h"

class Lune : public Corps
{
public:

    /* Constructeurs */
    /**
     * @brief Lune Constructeur par defaut
     */
    Lune();

    ~Lune();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculPosition Calcul de la position de la Lune avec le modele simplifie issu de
     * l'Astronomical Algorithms 2nd edition de Jean Meeus, pp337-342
     * @param date date
     */
    void CalculPosition(const Date &date);

    /**
     * @brief CalculPhase Calcul de la phase de la Lune
     * @param soleil
     */
    void CalculPhase(const Soleil &soleil);

    /**
     * @brief CalculMagnitude Calcul de la magnitude visuelle de la Lune
     * @param soleil Soleil
     */
    void CalculMagnitude(const Soleil &soleil);

    /* Accesseurs */
    double fractionIlluminee() const;
    double magnitude() const;
    QString phase() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    double _fractionIlluminee;
    double _magnitude;
    QString _phase;

    /* Methodes privees */


};

#endif // LUNE_H
