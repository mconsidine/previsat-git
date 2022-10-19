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
 * >    outils.cpp
 *
 * Localisation
 * >    interface.outils
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    14 aout 2022
 *
 * Date de revision
 * >    27 aout 2022
 *
 */

#include "outils.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_outils.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Outils::Outils(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Outils)
{
    _ui->setupUi(this);

    try {

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Outils::~Outils()
{
    settings.setValue("fichier/fichierAMettreAJour", _ui->fichierAMettreAJour->text());
    settings.setValue("fichier/fichierALire", _ui->fichierALire->text());
    settings.setValue("fichier/affichageMsgMAJ", _ui->affichageMsgMAJ->currentIndex());
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
/*
 * Initialisation de la fenetre Outils
 */
void Outils::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->listeOutils->setCurrentRow(0);
    _ui->listeOutils->setFocus();
    _ui->listeBoutonsOutils->button(QDialogButtonBox::Close)->setDefault(true);

    _ui->affichageMsgMAJ->setCurrentIndex(settings.value("fichier/affichageMsgMAJ", 1).toInt());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Outils::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}


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
void Outils::on_listeOutils_currentRowChanged(int currentRow)
{
    _ui->stackedWidget_outils->setCurrentIndex(currentRow);
}
