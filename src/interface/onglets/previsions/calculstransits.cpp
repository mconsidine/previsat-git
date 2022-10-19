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
 * >    calculstransits.cpp
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

#include "calculstransits.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_calculstransits.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
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
CalculsTransits::CalculsTransits(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsTransits)
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
CalculsTransits::~CalculsTransits()
{
    settings.setValue("previsions/hauteurSatTransit", _ui->hauteurSatTransit->currentIndex());
    settings.setValue("previsions/valHauteurSatTransit", _ui->valHauteurSatTransit->text().toInt());
    settings.setValue("previsions/lieuxObservationTransit", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/ageMaxTLETransit", _ui->ageMaxTLETransit->value());
    settings.setValue("previsions/elongationMaxCorps", _ui->elongationMaxCorps->value());
    settings.setValue("previsions/calcTransitLunaireJour", _ui->calcTransitLunaireJour->isChecked());
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
 * Affichage des lieux d'observation dans la liste deroulante
 */
void CalculsTransits::AffichageLieuObs()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->lieuxObservation->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->observateurs());
    while (it.hasNext()) {
        const QString nomlieu = it.next().nomlieu();
        _ui->lieuxObservation->addItem(nomlieu);
    }

    _ui->lieuxObservation->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Affichage des satellites dans la liste
 */
void CalculsTransits::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                              const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listeTransits);
    elem->setData(Qt::UserRole, norad);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled);
    elem->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

    if (norad == noradDefaut) {
        _ui->listeTransits->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void CalculsTransits::InitAffichageListeSatellites()
{
    _ui->listeTransits->clear();
    _ui->listeTransits->scrollToTop();
}

/*
 * Tri dans l'affichage des satellites
 */
void CalculsTransits::TriAffichageListeSatellites()
{
    _ui->listeTransits->sortItems();
    _ui->listeTransits->scrollToItem(_ui->listeTransits->currentItem(), QAbstractItemView::PositionAtTop);
}

void CalculsTransits::changeEvent(QEvent *evt)
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
 * Initialisation de la classe CalculsTransits
 */
void CalculsTransits::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->valHauteurSatTransit->setVisible(false);
    _ui->hauteurSatTransit->setCurrentIndex(settings.value("previsions/hauteurSatTransit", 1).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationTransit", 0).toInt());
    _ui->ageMaxTLETransit->setValue(settings.value("previsions/ageMaxTLETransit", 2.).toDouble());
    _ui->elongationMaxCorps->setValue(settings.value("previsions/elongationMaxCorps", 5.).toDouble());

    QAction* effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &CalculsTransits::on_filtreSatellites_returnPressed);
    }

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsTransits::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeTransits->count(); i++) {
        const QString elem = _ui->listeTransits->item(i)->text();
        _ui->listeTransits->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void CalculsTransits::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsTransits::on_parametrageDefautTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatTransit->setCurrentIndex(1);
    _ui->valHauteurSatTransit->setVisible(false);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->ageMaxTLETransit->setValue(2.);
    _ui->elongationMaxCorps->setValue(5.);
    if (!_ui->calculsTransit->isEnabled()) {
        _ui->calculsTransit->setEnabled(true);
    }

    /* Retour */
    return;
}
