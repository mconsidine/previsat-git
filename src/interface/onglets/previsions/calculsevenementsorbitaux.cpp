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
 * >    calculsevenementsorbitaux.cpp
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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_calculsevenementsorbitaux.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "calculsevenementsorbitaux.h"
#include "interface/listwidgetitem.h"
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
CalculsEvenementsOrbitaux::CalculsEvenementsOrbitaux(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsEvenementsOrbitaux)
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
CalculsEvenementsOrbitaux::~CalculsEvenementsOrbitaux()
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
void CalculsEvenementsOrbitaux::changeEvent(QEvent *evt)
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
 * Initialisation de la classe CalculsEvenementsOrbitaux
 */
void CalculsEvenementsOrbitaux::Initialisation()
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

/*
 * Affichage des satellites dans la liste
 */
void CalculsEvenementsOrbitaux::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                                const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listeEvenements);
    elem->setData(Qt::UserRole, norad);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled);
    elem->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

    if (norad == noradDefaut) {
        _ui->listeEvenements->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void CalculsEvenementsOrbitaux::InitAffichageListeSatellites()
{
    _ui->listeEvenements->clear();
    _ui->listeEvenements->scrollToTop();
}

/*
 * Tri dans l'affichage des satellites
 */
void CalculsEvenementsOrbitaux::TriAffichageListeSatellites()
{
    _ui->listeEvenements->sortItems();
    _ui->listeEvenements->scrollToItem(_ui->listeEvenements->currentItem(), QAbstractItemView::PositionAtTop);
}

void CalculsEvenementsOrbitaux::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeEvenements->count(); i++) {
        const QString elem = _ui->listeEvenements->item(i)->text();
        _ui->listeEvenements->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void CalculsEvenementsOrbitaux::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsEvenementsOrbitaux::on_parametrageDefautEvt_clicked()
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
