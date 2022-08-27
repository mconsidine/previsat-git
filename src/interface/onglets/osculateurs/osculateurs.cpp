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
 * >    osculateurs.cpp
 *
 * Localisation
 * >    interface.onglets.osculateurs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
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
#include "osculateurs.h"
#include "ui_osculateurs.h"
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
Osculateurs::Osculateurs(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Osculateurs)
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
Osculateurs::~Osculateurs()
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
void Osculateurs::changeEvent(QEvent *evt)
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
 * Affichage des elements osculateurs
 */
void Osculateurs::AffichageElementsOsculateurs()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

/*
 * Affichage du vecteur d'etat
 */
void Osculateurs::AffichageVecteurEtat()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

/*
 * Initialisation de la classe Osculateurs
 */
void Osculateurs::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    const bool etat1 = _ui->typeRepere->blockSignals(true);
    _ui->typeRepere->setCurrentIndex(settings.value("affichage/typeRepere", 0).toInt());
    _ui->typeRepere->blockSignals(etat1);

    const bool etat2 = _ui->typeParametres->blockSignals(true);
    _ui->typeParametres->setCurrentIndex(settings.value("affichage/typeParametres", 0).toInt());
    _ui->typeParametres->blockSignals(etat2);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Osculateurs::on_typeRepere_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AffichageVecteurEtat();
}


void Osculateurs::on_typeParametres_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AffichageElementsOsculateurs();
}
