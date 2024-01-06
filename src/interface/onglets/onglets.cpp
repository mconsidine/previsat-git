/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    27 decembre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#if defined (Q_OS_WIN)
#include "ui_suivitelescope.h"
#endif
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "onglets.h"
#include "ui_general.h"
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
#include "previsions/calculsstarlink.h"
#include "previsions/calculstransits.h"
#if defined (Q_OS_WIN)
#include "telescope/suivitelescope.h"
#endif


// Registre
static QSettings settings(ORG_NAME, APP_NAME);

static const char* _titresPrevisions[] = {
    QT_TRANSLATE_NOOP("Onglets", "Prévisions"),
    QT_TRANSLATE_NOOP("Onglets", "Flashs"),
    QT_TRANSLATE_NOOP("Onglets", "Transits"),
    QT_TRANSLATE_NOOP("Onglets", "Évènements orbitaux"),
    QT_TRANSLATE_NOOP("Onglets", "Starlink")
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

    ReinitFlags();

    try {

        _general = nullptr;
        _osculateurs = nullptr;

        _informationsSatellite = nullptr;
        _rechercheSatellite = nullptr;
        _informationsISS = nullptr;

        _previsions = nullptr;
        _flashs = nullptr;
        _transits = nullptr;
        _starlink = nullptr;
        _evenements = nullptr;

#if defined (Q_OS_WIN)
        _suiviTelescope = nullptr;
#endif
        _antenne = nullptr;

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
    EFFACE_OBJET(_starlink);
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

General *Onglets::general() const
{
    return _general;
}

Osculateurs *Onglets::osculateurs() const
{
    return _osculateurs;
}

InformationsSatellite *Onglets::informationsSatellite() const
{
    return _informationsSatellite;
}

InformationsISS *Onglets::informationsISS() const
{
    return _informationsISS;
}

RechercheSatellite *Onglets::rechercheSatellite() const
{
    return _rechercheSatellite;
}

CalculsPrevisions *Onglets::previsions() const
{
    return _previsions;
}

CalculsFlashs *Onglets::flashs() const
{
    return _flashs;
}

CalculsTransits *Onglets::transits() const
{
    return _transits;
}

CalculsStarlink *Onglets::starlink() const
{
    return _starlink;
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

Antenne *Onglets::antenne() const
{
    return _antenne;
}


/*
 * Modificateurs
 */
void Onglets::setAcalcAOS(const bool acalc)
{
    _general->setAcalcAOS(acalc);
}

void Onglets::setAcalcDN(const bool acalc)
{
    _general->setAcalcDN(acalc);
}

void Onglets::setIndexInformations(const unsigned int newIndexInformations)
{
    _indexInformations = newIndexInformations;
    AffichageOngletInformations();
}

void Onglets::setIndexPrevisions(const unsigned int newIndexPrevisions)
{
    _indexPrevisions = newIndexPrevisions;
    setTabText(indexOf(_ui->previsions), QCoreApplication::translate("Onglets", _titresPrevisions[_indexPrevisions]));
}

void Onglets::setInfo(const bool info)
{
    _info = info;
}


/*
 * Methodes publiques
 */
/*
 * Affichage des informations pour les onglets
 */
void Onglets::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Affichage des donnees de l'onglet General
    _general->show(date);

    if (Configuration::instance()->listeSatellites().isEmpty()) {

        removeTab(indexOf(_ui->elementsOsculateurs));
        _ui->stackedWidget_informations->removeWidget(_ui->informationsSat);

    } else {

        // Affichage des donnees de l'onglet Elements osculateurs
        if (indexOf(_ui->elementsOsculateurs) == -1) {
            insertTab(1, _ui->elementsOsculateurs, tr("Éléments osculateurs"));
        }

        if (_ui->stackedWidget_informations->indexOf(_ui->informationsSat) == -1) {
            _ui->stackedWidget_informations->insertWidget(0, _ui->informationsSat);
            _ui->stackedWidget_informations->setCurrentIndex(0);
        }

        _osculateurs->show(date);

        // Affichage des informations sur le satellite
        if (_info) {

            _informationsSatellite->show(Configuration::instance()->listeSatellites().first());
            _rechercheSatellite->on_noradDonneesSat_valueChanged(Configuration::instance()->noradDefaut().toInt());
            _informationsISS->show();
            _antenne->InitAffichageFrequences();
            _info = false;
        }

        _antenne->show(date);
    }

    AffichageOngletInformations();

    if (!_previsions->isVisible()) {
        _previsions->show(date);
    }

    if (!_flashs->isVisible()) {
        _flashs->show(date);
    }

    if (!_transits->isVisible()) {
        _transits->show(date);
    }

    if (!_evenements->isVisible()) {
        _evenements->show(date);
    }

#if defined (Q_OS_WIN)
    if (_ui->telescope->isVisible() && (_suiviTelescope->getListItemChecked(_suiviTelescope->ui()->listeTelescope) > 0)) {
        _suiviTelescope->CalculAosSatSuivi(date);
    }
#endif

    /* Retour */
    return;
}

/*
 * Reinitialisation des flags d'affichage
 */
void Onglets::ReinitFlags()
{
    setAcalcAOS(true);
    setAcalcDN(true);
    setInfo(true);
}

void Onglets::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {

        // Titre de l'onglet Previsions
        setTabText(indexOf(_ui->previsions), QCoreApplication::translate("Onglets", _titresPrevisions[_indexPrevisions]));

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
 * Gestion de l'affichage de l'onglet Informations
 */
void Onglets::AffichageOngletInformations()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const bool isInfo = (_ui->stackedWidget_informations->indexOf(_ui->informationsSat) != -1);

    /* Corps de la methode */
    if (isInfo) {

        if (_ui->informations->isVisible()) {

            setTabText(indexOf(_ui->informations), tr("Informations satellite"));

            _ui->infoPrec->setToolTip(tr("Informations ISS"));
            _ui->infoPrec->setVisible(true);
            _ui->infoSuiv->setToolTip(tr("Recherche données"));
                _ui->infoSuiv->setVisible(true);
        }

    } else {
        setTabText(indexOf(_ui->informations), tr("Recherche données"));
    }

    if (_ui->rechercheSat->isVisible()) {

        setTabText(indexOf(_ui->informations), tr("Recherche données"));
        _ui->infoSuiv->setToolTip(tr("Informations ISS"));
        _ui->infoSuiv->setVisible(true);

        if (isInfo) {

            _ui->infoPrec->setVisible(true);
            _ui->infoPrec->setToolTip(tr("Informations satellite"));

        } else {
            _ui->infoPrec->setVisible(false);
        }
    }

    if (_ui->informationsStationSpatiale->isVisible()) {

        setTabText(indexOf(_ui->informations), tr("Informations ISS"));
        _ui->infoPrec->setToolTip(tr("Recherche données"));
        _ui->infoPrec->setVisible(true);

        if (isInfo) {

            _ui->infoSuiv->setVisible(true);
            _ui->infoSuiv->setToolTip(tr("Informations satellite"));

        } else {
            _ui->infoSuiv->setVisible(false);
        }
    }

    /* Retour */
    return;
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

    setCurrentIndex(0);
    setStyleSheet("QTabWidget::pane { border: 5px solid #eeeeee; }");

    // Initialisation des onglets
    _osculateurs = new Osculateurs(_ui->elementsOsculateurs);
    _flashs = new CalculsFlashs(_ui->flashs);
    _general = new General(_flashs, _osculateurs, _ui->general);

    // Donnees satellite
    _informationsSatellite = new InformationsSatellite(_ui->informationsSat);

    _rechercheSatellite = new RechercheSatellite(_ui->rechercheSat);
    _rechercheSatellite->show();

    _informationsISS = new InformationsISS(_ui->informationsStationSpatiale);
    _informationsISS->show();

    // Calculs de previsions
    _previsions = new CalculsPrevisions(_ui->prevision);
    _transits = new CalculsTransits(_ui->transits);
    _starlink = new CalculsStarlink(_ui->starlink);
    _evenements = new CalculsEvenementsOrbitaux(_ui->evenementsOrbitaux);

#if defined (Q_OS_WIN)
    // Suivi avec un telescope
    _suiviTelescope = new SuiviTelescope(_ui->telescope);
    _suiviTelescope->show();
#else
    removeTab(indexOf(_ui->telescope));
#endif

    // Antenne
    _antenne = new Antenne(_ui->antenne);

    // Connexions signaux-slots
    connect(this, &Onglets::AffichageLieuObs, _general, &General::AffichageLieuObs);
    connect(this, &Onglets::AffichageLieuObs, _previsions, &CalculsPrevisions::AffichageLieuObs);
    connect(this, &Onglets::AffichageLieuObs, _flashs, &CalculsFlashs::AffichageLieuObs);
    connect(this, &Onglets::AffichageLieuObs, _transits, &CalculsTransits::AffichageLieuObs);
    connect(this, &Onglets::AffichageLieuObs, _starlink, &CalculsStarlink::AffichageLieuObs);
#if defined (Q_OS_WIN)
    connect(this, &Onglets::AffichageLieuObs, _suiviTelescope, &SuiviTelescope::AffichageLieuObs);
#endif
    AffichageLieuObservation();

    _ui->stackedWidget_informations->setCurrentIndex(_indexInformations);
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

void Onglets::on_Onglets_currentChanged(int index)
{
    Q_UNUSED(index)
    AffichageOngletInformations();
}

void Onglets::on_infoPrec_clicked()
{
    _indexInformations = (_ui->stackedWidget_informations->currentIndex() + _ui->stackedWidget_informations->count() - 1)
            % _ui->stackedWidget_informations->count();
    _ui->stackedWidget_informations->setCurrentIndex(_indexInformations);

    AffichageOngletInformations();
}

void Onglets::on_infoSuiv_clicked()
{
    _indexInformations = (_ui->stackedWidget_informations->currentIndex() + 1) % _ui->stackedWidget_informations->count();
    _ui->stackedWidget_informations->setCurrentIndex(_indexInformations);

    AffichageOngletInformations();
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

void Onglets::on_stackedWidget_previsions_currentChanged(int arg1)
{
    if (static_cast<unsigned int> (arg1) != _indexPrevisions) {
        _indexPrevisions = arg1;
    }

    const unsigned int ind1 = (_indexPrevisions + _ui->stackedWidget_previsions->count() - 1) % _ui->stackedWidget_previsions->count();
    const unsigned int ind2 = (_indexPrevisions + 1) % _ui->stackedWidget_previsions->count();

    _ui->previsionPrec->setToolTip(QCoreApplication::translate("Onglets", _titresPrevisions[ind1]));
    _ui->previsionSuiv->setToolTip(QCoreApplication::translate("Onglets", _titresPrevisions[ind2]));
}
