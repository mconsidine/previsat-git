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
 * >    constellation.h
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Definition d'une constellation
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 mars 2012
 *
 * Date de revision
 * >    3 aout 2024
 *
 */

#ifndef CONSTELLATION_H
#define CONSTELLATION_H

#include <QList>
#include "librairies/corps/corps.h"


class Constellation : public Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Constellation Constructeur par defaut
     */
    Constellation() {
    }

    /**
     * @brief Constellation Definition a partir des composantes
     * @param[in] nomConst nom de la constellation
     * @param[in] ascDroite ascension droite moyenne de la constellation
     * @param[in] decl declinaison moyenne de la constellation
     */
    Constellation(const QString &nomConst,
                  const double ascDroite,
                  const double decl);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculConstellations Calcul des positions des noms des constellations pour la carte du ciel
     * @param[in] observateur observateur
     * @param[out] constellations tableau des constellations
     * @throw Exception
     */
    static void CalculConstellations(const Observateur &observateur,
                                     QList<Constellation> &constellations);

    /**
     * @brief Initialisation Lecture du fichier de constellations
     * @param[in] dirCommonData chemin des donnees communes
     * @return tableau de constellations
     * @throw Exception
     */
    static QList<Constellation> Initialisation(const QString &dirCommonData);


    /*
     * Accesseurs
     */
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
    QString _nom;
    QList<Constellation> _constellations;


    /*
     * Methodes privees
     */
    void CalculPosVit(const Date &date) override {
        Q_UNUSED(date)
    }

};


#endif // CONSTELLATION_H
