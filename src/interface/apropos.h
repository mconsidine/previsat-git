/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    apropos.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre A propos
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 mars 2012
 *
 * Date de revision
 * >    3 octobre 2015
 *
 */

#ifndef APROPOS_H
#define APROPOS_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
    class Apropos;
}

class Apropos : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    explicit Apropos(QWidget *fenetreParent = 0);

    /*
     * Destructeur
     */
    ~Apropos();

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
    Ui::Apropos *ui;


    /*
     * Methodes privees
     */


private slots:

    void on_ok_clicked();


};

#endif // APROPOS_H
