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
 * >    onglets.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 decembre 2019
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
#include "onglets.h"
#include "ui_onglets.h"
#include "general/general.h"
#include "librairies/exceptions/previsatexception.h"
#include "osculateurs/osculateurs.h"
#include "donnees/informationsiss.h"
#include "donnees/informationssatellite.h"
#include "donnees/recherchesatellite.h"
#include "previsions/evenementsorbitaux.h"
#include "previsions/flashs.h"
#include "previsions/previsionspassage.h"
#include "previsions/transits.h"
#if defined (Q_OS_WIN)
#include "telescope/suivitelescope.h"
#endif


// Registre
static QSettings settings(ORG_NAME, APP_NAME);

static const char* _titresInformations[] = {
    QT_TRANSLATE_NOOP("Onglets", "Informations satellite"),
    QT_TRANSLATE_NOOP("Onglets", "Recherche données"),
    QT_TRANSLATE_NOOP("Onglets", "Informations ISS")
};

static const char* _titresPrevisions[] = {
    QT_TRANSLATE_NOOP("Onglets", "Prévisions"),
    QT_TRANSLATE_NOOP("Onglets", "Flashs"),
    QT_TRANSLATE_NOOP("Onglets", "Transits"),
    QT_TRANSLATE_NOOP("Onglets", "Évènements orbitaux")
};


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Onglets::Onglets(QWidget *parent) :
    QTabWidget(parent),
    _ui(new Ui::Onglets)
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
Onglets::~Onglets()
{
    if (_general != nullptr) {
        delete _general;
        _general = nullptr;
    }

    if (_osculateurs != nullptr) {
        delete _osculateurs;
        _osculateurs = nullptr;
    }

    if (_informationsSatellite != nullptr) {
        delete _informationsSatellite;
        _informationsSatellite = nullptr;
    }

    if (_rechercheSatellite != nullptr) {
        delete _rechercheSatellite;
        _rechercheSatellite = nullptr;
    }

    if (_informationsISS != nullptr) {
        delete _informationsISS;
        _informationsISS = nullptr;
    }

    if (_previsionsPassage != nullptr) {
        delete _previsionsPassage;
        _previsionsPassage = nullptr;
    }

    if (_flashs != nullptr) {
        delete _flashs;
        _flashs = nullptr;
    }

    if (_transits != nullptr) {
        delete _transits;
        _transits = nullptr;
    }

    if (_evenements != nullptr) {
        delete _evenements;
        _evenements = nullptr;
    }

#if defined (Q_OS_WIN)
    if (_suiviTelescope != nullptr) {
        delete _suiviTelescope;
        _suiviTelescope = nullptr;
    }
#endif

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Onglets *Onglets::ui()
{
    return _ui;
}


/*
 * Modificateurs
 */
void Onglets::setIndexInformations(unsigned int newIndexInformations)
{
    _indexInformations = newIndexInformations;
    setTabText(indexOf(_ui->informations), QCoreApplication::translate("Onglets", _titresInformations[_indexInformations]));
}

void Onglets::setIndexPrevisions(unsigned int newIndexPrevisions)
{
    _indexPrevisions = newIndexPrevisions;
    setTabText(indexOf(_ui->previsions), QCoreApplication::translate("Onglets", _titresPrevisions[_indexPrevisions]));
}


/*
 * Methodes publiques
 */
void Onglets::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {

        _ui->retranslateUi(this);

        _general->changeEvent(evt);
        _osculateurs->changeEvent(evt);

        _informationsSatellite->changeEvent(evt);
        _rechercheSatellite->changeEvent(evt);
        _informationsISS->changeEvent(evt);

        _previsionsPassage->changeEvent(evt);
        _flashs->changeEvent(evt);
        _transits->changeEvent(evt);
        _evenements->changeEvent(evt);

        _ui->infoPrec->setToolTip(
                    QCoreApplication::translate("Onglets", _titresInformations[(_indexInformations + _ui->stackedWidget_informations->count() - 1)
                % _ui->stackedWidget_informations->count()]));
        _ui->infoSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresInformations[(_indexInformations + 1) %
                                  _ui->stackedWidget_informations->count()]));

        _ui->previsionPrec->setToolTip(
                    QCoreApplication::translate("Onglets", _titresPrevisions[(_indexPrevisions + _ui->stackedWidget_previsions->count() - 1)
                % _ui->stackedWidget_previsions->count()]));
        _ui->previsionSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresPrevisions[(_indexPrevisions + 1) %
                                       _ui->stackedWidget_previsions->count()]));
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
 * Initialisation de la classe Onglets
 */
void Onglets::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _indexInformations = settings.value("affichage/indexInformations", 0).toUInt();
    _indexPrevisions = settings.value("affichage/indexPrevisions", 0).toUInt();

    setStyleSheet("QTabWidget::pane { border: 5px solid #eeeeee; }");

    // Initialisation des onglets
    _general = new General(_ui->general);
    _general->show();

    _osculateurs = new Osculateurs(_ui->elementsOsculateurs);
    _osculateurs->show();


    _informationsSatellite = new InformationsSatellite(_ui->informationsSat);
    _informationsSatellite->show();

    _rechercheSatellite = new RechercheSatellite(_ui->rechercheSat);
    _rechercheSatellite->show();

    _informationsISS = new InformationsISS(_ui->informationsStationSpatiale);
    _informationsISS->show();


    _previsionsPassage = new PrevisionsPassage(_ui->prevision);
    _previsionsPassage->show();

    _flashs = new Flashs(_ui->flashs);
    _flashs->show();

    _transits = new Transits(_ui->transits);
    _transits->show();

    _evenements = new EvenementsOrbitaux(_ui->evenementsOrbitaux);
    _evenements->show();


#if defined (Q_OS_WIN)
    _suiviTelescope = new SuiviTelescope(_ui->telescope);
    _suiviTelescope->show();
#else
    removeTab(indexOf(_ui->telescope));
#endif


    _ui->stackedWidget_informations->setCurrentIndex(_indexInformations);

    _ui->infoPrec->setToolTip(
                QCoreApplication::translate("Onglets", _titresInformations[(_indexInformations + _ui->stackedWidget_informations->count() - 1)
            % _ui->stackedWidget_informations->count()]));
    _ui->infoSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresInformations[(_indexInformations + 1) %
                              _ui->stackedWidget_informations->count()]));


    _ui->stackedWidget_previsions->setCurrentIndex(_indexPrevisions);

    _ui->previsionPrec->setToolTip(
                QCoreApplication::translate("Onglets", _titresPrevisions[(_indexPrevisions + _ui->stackedWidget_previsions->count() - 1)
            % _ui->stackedWidget_previsions->count()]));
    _ui->previsionSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresPrevisions[(_indexPrevisions + 1) %
                                   _ui->stackedWidget_previsions->count()]));

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Onglets::on_infoPrec_clicked()
{
    _indexInformations = (_ui->stackedWidget_informations->currentIndex() + _ui->stackedWidget_informations->count() - 1)
            % _ui->stackedWidget_informations->count();
    _ui->stackedWidget_informations->setCurrentIndex(_indexInformations);

    setTabText(indexOf(_ui->informations), QCoreApplication::translate("Onglets", _titresInformations[_indexInformations]));
}

void Onglets::on_infoSuiv_clicked()
{
    _indexInformations = (_ui->stackedWidget_informations->currentIndex() + 1) % _ui->stackedWidget_informations->count();
    _ui->stackedWidget_informations->setCurrentIndex(_indexInformations);
    setTabText(indexOf(_ui->informations), QCoreApplication::translate("Onglets", _titresInformations[_indexInformations]));
}

void Onglets::on_previsionPrec_clicked()
{
    _indexPrevisions = (_ui->stackedWidget_previsions->currentIndex() + _ui->stackedWidget_previsions->count() - 1)
            % _ui->stackedWidget_previsions->count();
    _ui->stackedWidget_previsions->setCurrentIndex(_indexPrevisions);
    setTabText(indexOf(_ui->previsions), QCoreApplication::translate("Onglets", _titresPrevisions[_indexPrevisions]));
}

void Onglets::on_previsionSuiv_clicked()
{
    _indexPrevisions = (_ui->stackedWidget_previsions->currentIndex() + 1) % _ui->stackedWidget_previsions->count();
    _ui->stackedWidget_previsions->setCurrentIndex(_indexPrevisions);
    setTabText(indexOf(_ui->previsions), QCoreApplication::translate("Onglets", _titresPrevisions[_indexPrevisions]));
}

void Onglets::on_stackedWidget_informations_currentChanged(int arg1)
{
    Q_UNUSED(arg1)

    _ui->infoPrec->setToolTip(
                QCoreApplication::translate("Onglets", _titresInformations[(_indexInformations + _ui->stackedWidget_informations->count() - 1)
            % _ui->stackedWidget_informations->count()]));
    _ui->infoSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresInformations[(_indexInformations + 1) %
                              _ui->stackedWidget_informations->count()]));
}

void Onglets::on_stackedWidget_previsions_currentChanged(int arg1)
{
    Q_UNUSED(arg1)

    _ui->previsionPrec->setToolTip(
                QCoreApplication::translate("Onglets", _titresPrevisions[(_indexPrevisions + _ui->stackedWidget_previsions->count() - 1)
            % _ui->stackedWidget_previsions->count()]));
    _ui->previsionSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresPrevisions[(_indexPrevisions + 1) %
                                   _ui->stackedWidget_previsions->count()]));
}
