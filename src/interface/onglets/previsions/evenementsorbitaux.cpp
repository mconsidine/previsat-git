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
 * >    evenementsorbitaux.cpp
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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "evenementsorbitaux.h"
#include "ui_evenementsorbitaux.h"
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
EvenementsOrbitaux::EvenementsOrbitaux(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::EvenementsOrbitaux)
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
EvenementsOrbitaux::~EvenementsOrbitaux()
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
void EvenementsOrbitaux::changeEvent(QEvent *evt)
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
 * Initialisation de la classe EvenementsOrbitaux
 */
void EvenementsOrbitaux::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->passageApogee->setChecked(settings.value("previsions/passageApogee", true).toBool());
    _ui->passageNoeuds->setChecked(settings.value("previsions/passageNoeuds", true).toBool());
    _ui->passageOmbre->setChecked(settings.value("previsions/passageOmbre", true).toBool());
    _ui->passageQuadrangles->setChecked(settings.value("previsions/passageQuadrangles", true).toBool());
    _ui->transitionJourNuit->setChecked(settings.value("previsions/transitionJourNuit", true).toBool());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void EvenementsOrbitaux::on_parametrageDefautEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->passageApogee->setChecked(true);
    _ui->passageNoeuds->setChecked(true);
    _ui->passageOmbre->setChecked(true);
    _ui->passageQuadrangles->setChecked(true);
    _ui->transitionJourNuit->setChecked(true);
    if (!_ui->calculsEvt->isEnabled()) {
        _ui->calculsEvt->setEnabled(true);
    }

    /* Retour */
    return;
}
