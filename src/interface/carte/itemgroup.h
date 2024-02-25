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
 * >    itemgroup.h
 *
 * Localisation
 * >    interface.carte
 *
 * Heritage
 * >    QGraphicsItemGroup
 *
 * Description
 * >    Groupe d'elements graphiques
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 mars 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <QGraphicsItemGroup>
#include <QObject>


class ItemGroup : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief ItemGroup Constructeur par defaut
     * @param[in] parent parent
     */
    explicit ItemGroup(QGraphicsItem *parent = nullptr);


    /*
     * Destructeur
     */

    /*
     * Accesseurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief setVisibleMin Positionnement du zoom minimum pour la visibilite des frontieres
     * @param[in] min valeur minimum du zoom
     */
    void setVisibleMin(const double min = 0.);


public slots:

    /**
     * @brief CheckVisibleMin Verifie la valeur du zoom
     * @param[in] v valeur du zoom
     */
    void CheckVisibleMin(const double v);


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
    double _minVisible;


    /*
     * Methodes privees
     */


};

#endif // ITEMGROUP_H
