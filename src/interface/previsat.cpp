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
 * >    21 septembre 2022
 *
 */

#include "previsat.h"
#include "ui_onglets.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include "ui_previsat.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "onglets/onglets.h"
#include "options/options.h"
#include "outils/outils.h"
#include "librairies/exceptions/previsatexception.h"
#include "configuration/gestionnairexml.h"


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

        _onglets = nullptr;
        _options = nullptr;
        _outils = nullptr;

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
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

    if (_options != nullptr) {
        delete _options;
        _options = nullptr;
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
    qInfo() << "Locale :" << langue;

    /* Corps de la methode */
    if (langue != "fr") {
        InstallationTraduction(QString("%1_%2").arg(APP_NAME).arg(langue), _appTraduction);
    }
    InstallationTraduction(QString("qt_%1").arg(langue), _qtTraduction);

    _ui->retranslateUi(this);
    QEvent evt(QEvent::LanguageChange);

    if (_onglets != nullptr) {
        _onglets->changeEvent(&evt);
    }

    if (_options != nullptr) {
        _options->changeEvent(&evt);
    }

    if (_outils != nullptr) {
        _outils->changeEvent(&evt);
    }

    /* Retour */
    return;
}

void PreviSat::CreationMenus()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();

    /* Corps de la methode */
    _ui->barreMenu->setVisible(false);
    _ui->boutonMenu->setMenu(_ui->menuPrincipal);
    _ui->boutonDons->setMenu(_ui->menuDons);

    _ui->actionDons->setMenu(_ui->menuDons);
    _ui->actionImporter_fichier_TLE_GP->setIcon(styleIcones->standardIcon(QStyle::SP_DialogOpenButton));
    _ui->actionEnregistrer->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));
    _ui->actionInformations->setIcon(styleIcones->standardIcon(QStyle::SP_MessageBoxInformation));

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
    // Raccourci Onglet Previsions
    _previsions = new QAction(this);
    _previsions->setShortcut(Qt::ALT + Qt::Key_P);
    connect(_previsions, &QAction::triggered, this, &PreviSat::RaccourciPrevisions);
    this->addAction(_previsions);

    // Raccourci Onglet Flashs
    _flashs = new QAction(this);
    _flashs->setShortcut(Qt::ALT + Qt::Key_F);
    connect(_flashs, &QAction::triggered, this, &PreviSat::RaccourciFlashs);
    this->addAction(_flashs);

    // Raccourci Onglet Transits
    _transits = new QAction(this);
    _transits->setShortcut(Qt::ALT + Qt::Key_T);
    connect(_transits, &QAction::triggered, this, &PreviSat::RaccourciTransits);
    this->addAction(_transits);

    // Raccourci Onglet Evenements
    _evenements = new QAction(this);
    _evenements->setShortcut(Qt::ALT + Qt::Key_E);
    connect(_evenements, &QAction::triggered, this, &PreviSat::RaccourciEvenements);
    this->addAction(_evenements);

    // Raccourci Onglet Informations satellite
    _informations = new QAction(this);
    _informations->setShortcut(Qt::ALT + Qt::Key_S);
    connect(_informations, &QAction::triggered, this, &PreviSat::RaccourciInformations);
    this->addAction(_informations);

    // Raccourci Onglet Recherche satellite
    _recherche = new QAction(this);
    _recherche->setShortcut(Qt::ALT + Qt::Key_R);
    connect(_recherche, &QAction::triggered, this, &PreviSat::RaccourciRecherche);
    this->addAction(_recherche);

    // Raccourci Onglet Informations ISS
    _station = new QAction(this);
    _station->setShortcut(Qt::ALT + Qt::Key_I);
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
    try {

        // Affichage des informations generales dans le fichier de log
        qInfo() << QString("%1 %2").arg(APP_NAME).arg(QString(APPVERSION));
        qInfo() << QString("%1 %2 %3").arg(QSysInfo::productType()).arg(QSysInfo::productVersion()).arg(QSysInfo::currentCpuArchitecture());

        setWindowTitle(QString("%1 %2").arg(APP_NAME).arg(APPVER_MAJ));

        ChargementTraduction(Configuration::instance()->locale());

        // Chargement de la configuration generale
        Configuration::instance()->Chargement();

        qInfo() << "Début Initialisation" << metaObject()->className();

        _onglets = new Onglets(_ui->frameOnglets);
        _options = new Options();
        _outils = new Outils();

        CreationMenus();
        CreationRaccourcis();

        on_tempsReel_toggled(true);

        qInfo() << "Fin   Initialisation" << metaObject()->className();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

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
    } else {
        qWarning() << "Impossible de charger le fichier de traduction" << langue;
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

void PreviSat::on_configuration_clicked()
{
    _options->Initialisation();
    _options->show();
}

void PreviSat::on_outils_clicked()
{
    _outils->Initialisation();
    _outils->show();
}

void PreviSat::on_tempsReel_toggled(bool checked)
{
    _ui->pasManuel->setVisible(!checked);
    _ui->valManuel->setVisible(!checked);
    _ui->pasReel->setVisible(checked);
    _ui->secondes->setVisible(checked);
}

void PreviSat::on_modeManuel_toggled(bool checked)
{
    _ui->pasReel->setVisible(!checked);
    _ui->secondes->setVisible(!checked);
    _ui->pasManuel->setVisible(checked);
    _ui->valManuel->setVisible(checked);
}

void PreviSat::on_actionOptions_triggered()
{
    on_configuration_clicked();
}

void PreviSat::on_actionOutils_triggered()
{
    on_outils_clicked();
}
