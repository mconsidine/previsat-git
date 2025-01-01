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
 * >    2 decembre 2024
 *
 */

#ifndef RADAR_H
#define RADAR_H

#include <QFrame>


namespace Ui {
class Radar;
}

class QGraphicsEllipseItem;
class QGraphicsPixmapItem;
class QGraphicsScene;

class Radar : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Radar Constructeur par defaut
     * @param[in] parent parent
     * @throw Exception
     */
    explicit Radar(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Radar();


    /*
     * Accesseurs
     */
    Ui::Radar *ui() const;


    /*
     * Methodes publiques
     */

public slots:

    void mouseMoveEvent(QMouseEvent *evt);
    void mousePressEvent(QMouseEvent *evt);

    /**
     * @brief show Affichage du radar
     */
    void show();


signals:

    void AfficherMessageStatut(const QString &message, const int secondes = -1);
    void AfficherMessageStatut2(const QString &message);
    void AfficherMessageStatut3(const QString &message);
    void EffacerMessageStatut();
    void RecalculerPositions();
    void ReinitFlags();


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
    Ui::Radar *_ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *_sol;
    QGraphicsPixmapItem *_lun;
    QList<QGraphicsEllipseItem *> _sat;


    /*
     * Methodes privees
     */


};

#endif // RADAR_H
