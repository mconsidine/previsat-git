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
 * >    calculsflashs.cpp
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

#include "calculsflashs.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_calculsflashs.h"
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
CalculsFlashs::CalculsFlashs(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsFlashs)
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
CalculsFlashs::~CalculsFlashs()
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
void CalculsFlashs::changeEvent(QEvent *evt)
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
 * Initialisation de la classe CalculsFlashs
 */
void CalculsFlashs::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->hauteurSatMetOp->setCurrentIndex(settings.value("previsions/hauteurSatMetOp", 2).toInt());
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->hauteurSoleilMetOp->setCurrentIndex(settings.value("previsions/hauteurSoleilMetOp", 1).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservation3", 0).toInt());
    _ui->ordreChronologiqueMetOp->setChecked(settings.value("previsions/ordreChronologiqueMetOp", true).toBool());
    _ui->magnitudeMaxMetOp->setValue(settings.value("previsions/magnitudeMaxMetOp", 2.).toDouble());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsFlashs::on_parametrageDefautMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatMetOp->setCurrentIndex(2);
    _ui->hauteurSoleilMetOp->setCurrentIndex(1);
    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->ordreChronologiqueMetOp->setChecked(true);
    _ui->magnitudeMaxMetOp->setValue(4.);
    if (!_ui->calculsFlashs->isEnabled()) {
        _ui->calculsFlashs->setEnabled(true);
    }

    /* Retour */
    return;
}
