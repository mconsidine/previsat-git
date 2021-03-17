/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    radar.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Radar
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    3 avril 2020
 *
 * Date de revision
 * >
 *
 */

#ifndef RADAR_H
#define RADAR_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFrame>
#pragma GCC diagnostic warning "-Wconversion"


class Onglets;
class QGraphicsScene;

namespace Ui {
class Radar;
}

class Radar : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Radar Constructeur par defaut
     * @param parent parent
     */
    explicit Radar(Onglets *onglets, QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Radar();


    /*
     * Accesseurs
     */

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
     * @brief show Affichage du radar
     */
    void show();


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
    Ui::Radar *ui;
    QGraphicsScene *scene;
    Onglets *_onglets;

    /*
     * Methodes privees
     */


};

#endif // RADAR_H
