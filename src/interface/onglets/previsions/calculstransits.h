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
 * >    calculstransits.h
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet CalculsTransits
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    26 juin 2022
 *
 * Date de revision
 * >    27 aout 2022
 *
 */

#ifndef CALCULSTRANSITS_H
#define CALCULSTRANSITS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class CalculsTransits;
}

class CalculsTransits : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief CalculsTransits Constructeur par defaut
     * @param parent parent
     */
    explicit CalculsTransits(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CalculsTransits();


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
    Ui::CalculsTransits *_ui;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe CalculsTransits
     */
    void Initialisation();


private slots:

    void on_parametrageDefautTransit_clicked();


};

#endif // CALCULSTRANSITS_H
