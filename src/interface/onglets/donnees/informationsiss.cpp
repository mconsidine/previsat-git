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
 * >    informationsiss.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    26 juin 2022
 *
 * Date de revision
 * >    8 aout 2022
 *
 */

#include "informationsiss.h"
#include "ui_informationsiss.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
InformationsISS::InformationsISS(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::InformationsISS)
{
    _ui->setupUi(this);

    _ui->evenementsISS->setStyleSheet("QHeaderView::section {" \
                                      "background-color:rgb(235, 235, 235);" \
                                      "border-top: 0px solid grey;" \
                                      "border-bottom: 1px solid grey;" \
                                      "border-right: 1px solid grey;" \
                                      "font-size: 12px;" \
                                      "font-weight: 600 }");

    _ui->evenementsISS->resizeColumnsToContents();

}


/*
 * Destructeur
 */
InformationsISS::~InformationsISS()
{
    delete _ui;
}


/*
 * Accesseurs
 */


/*
 * Modificateurs
 */


/*
 * Methodes publiques
 */


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */

