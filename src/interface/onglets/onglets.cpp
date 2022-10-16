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
 * >    16 octobre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "onglets.h"
#include "ui_onglets.h"
#include "antenne/antenne.h"
#include "configuration/configuration.h"
#include "general/general.h"
#include "librairies/exceptions/previsatexception.h"
#include "osculateurs/osculateurs.h"
#include "donnees/informationsiss.h"
#include "donnees/informationssatellite.h"
#include "donnees/recherchesatellite.h"
#include "previsions/calculsevenementsorbitaux.h"
#include "previsions/calculsflashs.h"
#include "previsions/calculsprevisions.h"
#include "previsions/calculstransits.h"
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
Onglets::Onglets(Options *options, QWidget *parent) :
    QTabWidget(parent),
    _ui(new Ui::Onglets)
{
    _ui->setupUi(this);

    _options = options;

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
    EFFACE_OBJET(_general);
    EFFACE_OBJET(_osculateurs);
    EFFACE_OBJET(_informationsSatellite);
    EFFACE_OBJET(_rechercheSatellite);
    EFFACE_OBJET(_informationsISS);
    EFFACE_OBJET(_previsions);
    EFFACE_OBJET(_flashs);
    EFFACE_OBJET(_transits);
    EFFACE_OBJET(_evenements);

#if defined (Q_OS_WIN)
    EFFACE_OBJET(_suiviTelescope);
#endif

    EFFACE_OBJET(_antenne);

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Onglets *Onglets::ui()
{
    return _ui;
}

CalculsPrevisions *Onglets::previsions() const
{
    return _previsions;
}

CalculsTransits *Onglets::transits() const
{
    return _transits;
}

CalculsEvenementsOrbitaux *Onglets::evenements() const
{
    return _evenements;
}

#if defined (Q_OS_WIN)
SuiviTelescope *Onglets::suiviTelescope() const
{
    return _suiviTelescope;
}
#endif


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

        _previsions->changeEvent(evt);
        _flashs->changeEvent(evt);
        _transits->changeEvent(evt);
        _evenements->changeEvent(evt);

#if defined (Q_OS_WIN)
        _suiviTelescope->changeEvent(evt);
#endif

        _antenne->changeEvent(evt);

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
 * Affichage du lieu d'observation
 */
void Onglets::AffichageLieuObservation()
{
    emit AffichageLieuObs();
}

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
    _general = new General(_options, _ui->general);
    _general->show();

    _osculateurs = new Osculateurs(_ui->elementsOsculateurs);
    _osculateurs->show();


    _informationsSatellite = new InformationsSatellite(_ui->informationsSat);
    _informationsSatellite->show();

    _rechercheSatellite = new RechercheSatellite(_ui->rechercheSat);
    _rechercheSatellite->show();

    _informationsISS = new InformationsISS(_ui->informationsStationSpatiale);
    _informationsISS->show();


    _previsions = new CalculsPrevisions(_ui->prevision);
    _previsions->show();

    _flashs = new CalculsFlashs(_ui->flashs);
    _flashs->show();

    _transits = new CalculsTransits(_ui->transits);
    _transits->show();

    _evenements = new CalculsEvenementsOrbitaux(_ui->evenementsOrbitaux);
    _evenements->show();


#if defined (Q_OS_WIN)
    _suiviTelescope = new SuiviTelescope(_ui->telescope);
    _suiviTelescope->show();
#else
    removeTab(indexOf(_ui->telescope));
#endif

    _antenne = new Antenne(_ui->antenne);
    _antenne->show();

    connect(this, SIGNAL(AffichageLieuObs()), _general, SLOT(AffichageLieuObs()));
    connect(this, SIGNAL(AffichageLieuObs()), _previsions, SLOT(AffichageLieuObs()));
    connect(this, SIGNAL(AffichageLieuObs()), _flashs, SLOT(AffichageLieuObs()));
    connect(this, SIGNAL(AffichageLieuObs()), _transits, SLOT(AffichageLieuObs()));
    connect(this, SIGNAL(AffichageLieuObs()), _suiviTelescope, SLOT(AffichageLieuObs()));

    AffichageLieuObservation();

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
