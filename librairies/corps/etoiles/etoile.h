/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2018  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
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

#include <QList>
#include <QString>
#include "librairies/corps/corps.h"

class Etoile : public Corps
{
public:

    /* Constructeurs */
    /**
     * @brief Etoile Constructeur par defaut
     */
    Etoile();

    /**
     * @brief Etoile Definition d'un objet Etoile a partir de ses composantes
     * @param nomEtoile nom de l'etoile
     * @param ascDroite ascension droite (heures)
     * @param decl declinaison (degres)
     * @param mag magnitude
     */
    Etoile(const QString &nomEtoile, const double ascDroite, const double decl, const double mag);

    ~Etoile();

    /* Constantes publiques */

    /* Variables publiques */
    static bool initStar;

    /* Methodes publiques */
    /**
     * @brief CalculPositionEtoiles Calcul de la position des etoiles
     * @param observateur observateur
     * @param etoiles tableau d'etoiles
     */
    static void CalculPositionEtoiles(const Observateur &observateur, QList<Etoile> &etoiles);

    /* Accesseurs */
    double magnitude() const;
    QString nom() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    double _magnitude;
    QString _nom;

    /* Methodes privees */
    /**
     * @brief InitTabEtoiles Lecture du fichier d'etoiles
     * @param etoiles tableau d'etoiles
     */
    static void InitTabEtoiles(QList<Etoile> &etoiles);

};

#endif // ETOILE_H
