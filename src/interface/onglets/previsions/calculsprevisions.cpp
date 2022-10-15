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
 * >    calculsprevisions.cpp
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

#include "calculsprevisions.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_calculsprevisions.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "interface/listWidgetItem.h"
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
CalculsPrevisions::CalculsPrevisions(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsPrevisions)
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
CalculsPrevisions::~CalculsPrevisions()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::CalculsPrevisions *CalculsPrevisions::ui() const
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
 * Affichage des satellites dans la liste
 */
void CalculsPrevisions::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                                const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listePrevisions);
    elem->setData(Qt::UserRole, norad);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled);
    elem->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

    if (norad == noradDefaut) {
        _ui->listePrevisions->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void CalculsPrevisions::InitAffichageListeSatellites()
{
    _ui->listePrevisions->clear();
    _ui->listePrevisions->scrollToTop();
}

/*
 * Tri dans l'affichage des satellites
 */
void CalculsPrevisions::TriAffichageListeSatellites()
{
    _ui->listePrevisions->sortItems();
    _ui->listePrevisions->scrollToItem(_ui->listePrevisions->currentItem(), QAbstractItemView::PositionAtTop);
}

void CalculsPrevisions::changeEvent(QEvent *evt)
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
 * Initialisation de la classe CalculsPrevisions
 */
void CalculsPrevisions::Initialisation()
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

void CalculsPrevisions::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listePrevisions->count(); i++) {
        const QString elem = _ui->listePrevisions->item(i)->text();
        _ui->listePrevisions->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void CalculsPrevisions::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsPrevisions::on_parametrageDefautPrev_clicked()
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
