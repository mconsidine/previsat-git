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
 * >    options.cpp
 *
 * Localisation
 * >    interface.options
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 aout 2022
 *
 * Date de revision
 * >
 *
 */

#include "options.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_options.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
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
Options::Options(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Options)
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
Options::~Options()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Options *Options::ui()
{
    return _ui;
}

/*
 * Modificateurs
 */

/*
 * Methodes publiques
 */
/*
 * Initialisation de la fenetre Options
 */
void Options::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->listeOptions->setCurrentRow(0);
    _ui->enregistrerPref->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));
    _ui->ajoutLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowRight));
    _ui->supprLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowLeft));
    _ui->listeOptions->setFocus();

    const int index = settings.value("affichage/policeWCC", 0).toInt();
    _ui->policeWCC->clear();

#if defined (Q_OS_WIN)
    _ui->policeWCC->addItem("Lucida Console");
    _ui->policeWCC->addItem("MS Shell Dlg 2");

    const int taille = 10;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#elif defined (Q_OS_LINUX)
    _ui->policeWCC->addItem("FreeSans");
    _ui->policeWCC->addItem("Sans Serif");

    const int taille = 11;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille);

#elif defined (Q_OS_MAC)
    _ui->policeWCC->addItem("Lucida Grande");
    _ui->policeWCC->addItem("Marion");

    const int taille = 13;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#else
    const int taille = 11;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille);

#endif

    Configuration::instance()->setPoliceWcc(policeWcc);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Options::changeEvent(QEvent *evt)
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
void Options::on_listeOptions_currentRowChanged(int currentRow)
{
    _ui->stackedWidget_options->setCurrentIndex(currentRow);
}
