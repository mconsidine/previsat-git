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
 * >    donneessatellite.h
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre Donnees satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    18 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#ifndef DONNEESSATELLITE_H
#define DONNEESSATELLITE_H

#include <QMainWindow>


namespace Ui {
class DonneesSatellite;
}

class Donnees;

class DonneesSatellite : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief DonneesSatellite Constructeur par defaut
     * @param[in] parent fenetre parent
     */
    explicit DonneesSatellite(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~DonneesSatellite();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */

public slots:

    void changeEvent(QEvent *evt);
    void show(const Donnees &donnees);


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
    Ui::DonneesSatellite *_ui;


    /*
     * Methodes privees
     */
    bool eventFilter(QObject *watched, QEvent *event);


};

#endif // DONNEESSATELLITE_H
