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
 * >    suivitelescope.cpp
 *
 * Localisation
 * >    interface.onglets.telescope
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

#include "suivitelescope.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_suivitelescope.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
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
SuiviTelescope::SuiviTelescope(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::SuiviTelescope)
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
SuiviTelescope::~SuiviTelescope()
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
void SuiviTelescope::changeEvent(QEvent *evt)
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
 * Initialisation de la classe SuiviTelescope
 */
void SuiviTelescope::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->valHauteurSatSuivi->setVisible(false);
    _ui->hauteurSatSuivi->setCurrentIndex(settings.value("previsions/hauteurSatSuivi", 2).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservation5", 0).toInt());
    _ui->pasSuivi->setValue(settings.value("previsions/pasSuivi", 20).toInt());
    _ui->pecDelai->setChecked(settings.value("previsions/pecDelai", false).toBool());
    _ui->delaiTelescope->setValue(settings.value("previsions/delaiTelescope", 60).toInt());
    _ui->delaiTelescope->setEnabled(_ui->pecDelai->isChecked());
    _ui->demarrerSuiviTelescope->setChecked(settings.value("previsions/demarrerSuiviTelescope", false).toBool());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void SuiviTelescope::AffichageLieuObs()
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
void SuiviTelescope::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                             const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listeTelescope);
    elem->setData(Qt::UserRole, norad);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled);
    elem->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

    if (norad == noradDefaut) {
        _ui->listeTelescope->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void SuiviTelescope::InitAffichageListeSatellites()
{
    _ui->listeTelescope->clear();
    _ui->listeTelescope->scrollToTop();
}

/*
 * Tri dans l'affichage des satellites
 */
void SuiviTelescope::TriAffichageListeSatellites()
{
    _ui->listeTelescope->sortItems();
    _ui->listeTelescope->scrollToItem(_ui->listeTelescope->currentItem(), QAbstractItemView::PositionAtTop);
}

void SuiviTelescope::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeTelescope->count(); i++) {
        const QString elem = _ui->listeTelescope->item(i)->text();
        _ui->listeTelescope->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void SuiviTelescope::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void SuiviTelescope::on_parametrageDefautSuivi_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatSuivi->setCurrentIndex(2);
    _ui->pasSuivi->setValue(20);
    _ui->pecDelai->setChecked(false);
    _ui->delaiTelescope->setEnabled(false);
    _ui->delaiTelescope->setValue(60);
    _ui->demarrerSuiviTelescope->setChecked(false);

    /* Retour */
    return;
}
