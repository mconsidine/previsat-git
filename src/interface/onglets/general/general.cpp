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
 * >    general.cpp
 *
 * Localisation
 * >    interface.onglets.general
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >
 *
 */

#include "general.h"
#include "ui_general.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
General::General(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::General)
{
    _ui->setupUi(this);
}


/*
 * Destructeur
 */
General::~General()
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
void General::on_soleilLunePrec_clicked()
{
    _ui->stackedWidget_soleilLune->setCurrentIndex((_ui->stackedWidget_soleilLune->currentIndex() + _ui->stackedWidget_soleilLune->count() - 1)
                                                  % _ui->stackedWidget_soleilLune->count());
}

void General::on_soleilLuneSuiv_clicked()
{
    _ui->stackedWidget_soleilLune->setCurrentIndex((_ui->stackedWidget_soleilLune->currentIndex() + 1) % _ui->stackedWidget_soleilLune->count());
}
