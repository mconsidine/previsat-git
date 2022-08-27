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
 * >    previsionspassage.cpp
 *
 * Localisation
 * >    interface.onglets.previsions
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

#include "previsionspassage.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_previsionspassage.h"
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
PrevisionsPassage::PrevisionsPassage(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::PrevisionsPassage)
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
PrevisionsPassage::~PrevisionsPassage()
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
void PrevisionsPassage::changeEvent(QEvent *evt)
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
/*
 * Initialisation de la classe PrevisionsPassage
 */
void PrevisionsPassage::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->pasGeneration->setCurrentIndex(settings.value("previsions/pasGeneration", 5).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservation2", 0).toInt());
    _ui->valHauteurSatPrev->setVisible(false);
    _ui->hauteurSatPrev->setCurrentIndex(settings.value("previsions/hauteurSatPrev", 0).toInt());
    _ui->valHauteurSoleilPrev->setVisible(false);
    _ui->hauteurSoleilPrev->setCurrentIndex(settings.value("previsions/hauteurSoleilPrev", 1).toInt());
    _ui->illuminationPrev->setChecked(settings.value("previsions/illuminationPrev", true).toBool());
    _ui->magnitudeMaxPrev->setChecked(settings.value("previsions/magnitudeMaxPrev", false).toBool());
    _ui->valMagnitudeMaxPrev->setVisible(_ui->magnitudeMaxPrev->isChecked());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void PrevisionsPassage::on_parametrageDefautPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->pasGeneration->setCurrentIndex(5);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->hauteurSatPrev->setCurrentIndex(0);
    _ui->hauteurSoleilPrev->setCurrentIndex(1);
    _ui->valHauteurSatPrev->setVisible(false);
    _ui->valHauteurSoleilPrev->setVisible(false);
    _ui->valMagnitudeMaxPrev->setVisible(false);
    _ui->illuminationPrev->setChecked(true);
    _ui->magnitudeMaxPrev->setChecked(false);
    if (!_ui->calculsPrev->isEnabled()) {
        _ui->calculsPrev->setEnabled(true);
    }

    /* Retour */
    return;
}
