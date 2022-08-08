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
 * >    previsat.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    8 aout 2022
 *
 */

#include "previsat.h"
#include "ui_onglets.h"
#include "ui_previsat.h"
#include "onglets/onglets.h"
#include "configuration/configuration.h"
#include "librairies/exceptions/previsatexception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
PreviSat::PreviSat(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::PreviSat)
{
    _ui->setupUi(this);

    try {

        Initialisation();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
PreviSat::~PreviSat()
{
    if (_onglets != nullptr) {
        delete _onglets;
        _onglets = nullptr;
    }

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
 * Chargement de la traduction
 */
void PreviSat::ChargementTraduction(const QString &langue)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    InstallationTraduction(QString("%1_%2").arg(qApp->applicationName()).arg(langue), _appTraduction);
    InstallationTraduction(QString("qt_%1").arg(langue), _qtTraduction);

    _ui->retranslateUi(this);
    _onglets->ui()->retranslateUi(_onglets);

    /* Retour */
    return;
}

/*
 * Creation des raccourcis clavier
 */
void PreviSat::CreationRaccourcis()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Onglet Previsions
    _previsions = new QAction(this);
    _previsions->setShortcut(Qt::ALT + Qt::Key_P);
    connect(_previsions, &QAction::triggered, this, &PreviSat::RaccourciPrevisions);
    this->addAction(_previsions);

    // Onglet Flashs
    _flashs = new QAction(this);
    _flashs->setShortcut(Qt::ALT + Qt::Key_F);
    connect(_flashs, &QAction::triggered, this, &PreviSat::RaccourciFlashs);
    this->addAction(_flashs);

    // Onglet Transits
    _transits = new QAction(this);
    _transits->setShortcut(Qt::ALT + Qt::Key_T);
    connect(_transits, &QAction::triggered, this, &PreviSat::RaccourciTransits);
    this->addAction(_transits);

    // Onglet Evenements
    _evenements = new QAction(this);
    _evenements->setShortcut(Qt::ALT + Qt::Key_E);
    connect(_evenements, &QAction::triggered, this, &PreviSat::RaccourciEvenements);
    this->addAction(_evenements);

    // Onglet Informations satellite
    _informations = new QAction(this);
    _informations->setShortcut(Qt::ALT + Qt::Key_I);
    connect(_informations, &QAction::triggered, this, &PreviSat::RaccourciInformations);
    this->addAction(_informations);

    // Onglet Recherche satellite
    _recherche = new QAction(this);
    _recherche->setShortcut(Qt::ALT + Qt::Key_R);
    connect(_recherche, &QAction::triggered, this, &PreviSat::RaccourciRecherche);
    this->addAction(_recherche);

    // Onglet Informations ISS
    _station = new QAction(this);
    _station->setShortcut(Qt::ALT + Qt::Key_S);
    connect(_station, &QAction::triggered, this, &PreviSat::RaccourciStation);
    this->addAction(_station);

    /* Retour */
    return;
}

/*
 * Initialisation de la fenetre principale
 */
void PreviSat::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Affichage des informations generales dans le fichier de log
    qInfo() << QString("%1 %2").arg(APP_NAME).arg(QString(APPVERSION));
    qInfo() << QString("%1 %2 %3").arg(QSysInfo::productType()).arg(QSysInfo::productVersion()).arg(QSysInfo::currentCpuArchitecture());

    setWindowTitle(QString("%1 %2").arg(APP_NAME).arg(APPVER_MAJ));

    // Initialisation de la configuration generale
    Configuration::instance()->Initialisation();

    _onglets = new Onglets(_ui->frameOnglets);

    CreationRaccourcis();

    //ChargementTraduction(Configuration::instance()->locale());

    /* Retour */
    return;
}

/*
 * Installation de la traduction
 */
void PreviSat::InstallationTraduction(const QString &langue, QTranslator &traduction)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qApp->removeTranslator(&traduction);
    if (traduction.load(langue, Configuration::instance()->dirLang())) {
        qApp->installTranslator(&traduction);
    }

    /* Retour */
    return;
}

void PreviSat::RaccourciPrevisions()
{
    _onglets->setCurrentWidget(_onglets->ui()->previsions);
    _onglets->ui()->stackedWidget_previsions->setCurrentWidget(_onglets->ui()->prevision);
    const unsigned int index = _onglets->ui()->stackedWidget_previsions->indexOf(_onglets->ui()->prevision);
    _onglets->setIndexPrevisions(index);
}

void PreviSat::RaccourciFlashs()
{
    _onglets->setCurrentWidget(_onglets->ui()->previsions);
    _onglets->ui()->stackedWidget_previsions->setCurrentWidget(_onglets->ui()->flashs);
    const unsigned int index = _onglets->ui()->stackedWidget_previsions->indexOf(_onglets->ui()->flashs);
    _onglets->setIndexPrevisions(index);
}

void PreviSat::RaccourciTransits()
{
    _onglets->setCurrentWidget(_onglets->ui()->previsions);
    _onglets->ui()->stackedWidget_previsions->setCurrentWidget(_onglets->ui()->transits);
    const unsigned int index = _onglets->ui()->stackedWidget_previsions->indexOf(_onglets->ui()->transits);
    _onglets->setIndexPrevisions(index);
}

void PreviSat::RaccourciEvenements()
{
    _onglets->setCurrentWidget(_onglets->ui()->previsions);
    _onglets->ui()->stackedWidget_previsions->setCurrentWidget(_onglets->ui()->evenementsOrbitaux);
    const unsigned int index = _onglets->ui()->stackedWidget_previsions->indexOf(_onglets->ui()->evenementsOrbitaux);
    _onglets->setIndexPrevisions(index);
}

void PreviSat::RaccourciInformations()
{
    _onglets->setCurrentWidget(_onglets->ui()->informations);
    _onglets->ui()->stackedWidget_informations->setCurrentWidget(_onglets->ui()->informationsSat);
    const unsigned int index = _onglets->ui()->stackedWidget_informations->indexOf(_onglets->ui()->informationsSat);
    _onglets->setIndexInformations(index);
}

void PreviSat::RaccourciRecherche()
{
    _onglets->setCurrentWidget(_onglets->ui()->informations);
    _onglets->ui()->stackedWidget_informations->setCurrentWidget(_onglets->ui()->rechercheSat);
    const unsigned int index = _onglets->ui()->stackedWidget_informations->indexOf(_onglets->ui()->rechercheSat);
    _onglets->setIndexInformations(index);
}

void PreviSat::RaccourciStation()
{
    _onglets->setCurrentWidget(_onglets->ui()->informations);
    _onglets->ui()->stackedWidget_informations->setCurrentWidget(_onglets->ui()->informationsStationSpatiale);
    const unsigned int index = _onglets->ui()->stackedWidget_informations->indexOf(_onglets->ui()->informationsStationSpatiale);
    _onglets->setIndexInformations(index);
}
