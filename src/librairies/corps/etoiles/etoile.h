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
 * >    etoile.h
 *
 * Localisation
 * >    librairies.corps.etoiles
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Definition d'une etoile
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

#ifndef ETOILE_H
#define ETOILE_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QList>
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/corps/corps.h"


class Etoile : public Corps
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Etoile Constructeur par defaut
     */
    Etoile();

    /**
     * @brief Etoile Definition a partir de ses composantes
     * @param[in] nomEtoile nom de l'etoile
     * @param[in] ascDroite ascension droite (heures)
     * @param[in] decl declinaison (degres)
     * @param[in] mag magnitude
     */
    Etoile(const QString &nomEtoile, const double ascDroite, const double decl, const double mag);


    /*
     * Methodes publiques
     */
    /**
     * @brief CalculPositionEtoiles Calcul de la position des etoiles
     * @param[in] observateur observateur
     */
    static void CalculPositionEtoiles(const Observateur &observateur, QList<Etoile> &etoiles);

    /**
     * @brief Initialisation Lecture du fichier d'etoiles
     * @param[in] dirCommonData chemin des donnees communes
     */
    /**
     * @brief Initialisation Lecture du fichier d'etoiles
     * @param[in] dirCommonData chemin des donnees communes
     * @param[out] etoiles tableau d'etoiles
     */
    static void Initialisation(const QString &dirCommonData, QList<Etoile> &etoiles);


    /*
     * Accesseurs
     */
    double magnitude() const;
    const QString &nom() const;


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
    double _magnitude;
    QString _nom;


    /*
     * Methodes privees
     */
    /**
     * @brief InitTabEtoiles Lecture du fichier d'etoiles
     * @param[out] etoiles tableau d'etoiles
     */
    static void InitTabEtoiles(QList<Etoile> &etoiles);


};

#endif // ETOILE_H
