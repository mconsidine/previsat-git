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
 * >    9 octobre 2022
 *
 */

#include <QDesktopServices>
#include "previsat.h"
#include "ui_onglets.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include "ui_previsat.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "apropos/apropos.h"
#include "configuration/configuration.h"
#include "informations/informations.h"
#include "onglets/onglets.h"
#include "options/options.h"
#include "outils/outils.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/previsatexception.h"
#include "configuration/gestionnairexml.h"


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
PreviSat::PreviSat(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::PreviSat)
{
    _ui->setupUi(this);

    try {

        _informations = nullptr;
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
    EFFACE_OBJET(_onglets);
    EFFACE_OBJET(_options);
    EFFACE_OBJET(_outils);

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

    // Menu deroulant
    _ui->actionDons->setMenu(_ui->menuDons);
    _ui->actionPartenaires->setMenu(_ui->menuPartenaires);
    _ui->actionElementsOrbitaux->setMenu(_ui->menuElementsOrbitaux);
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
    _previsions->setShortcut(Qt::ALT | Qt::Key_P);
    connect(_previsions, &QAction::triggered, this, &PreviSat::RaccourciPrevisions);
    this->addAction(_previsions);

    // Raccourci Onglet Flashs
    _flashs = new QAction(this);
    _flashs->setShortcut(Qt::ALT | Qt::Key_F);
    connect(_flashs, &QAction::triggered, this, &PreviSat::RaccourciFlashs);
    this->addAction(_flashs);

    // Raccourci Onglet Transits
    _transits = new QAction(this);
    _transits->setShortcut(Qt::ALT | Qt::Key_T);
    connect(_transits, &QAction::triggered, this, &PreviSat::RaccourciTransits);
    this->addAction(_transits);

    // Raccourci Onglet Evenements
    _evenements = new QAction(this);
    _evenements->setShortcut(Qt::ALT | Qt::Key_E);
    connect(_evenements, &QAction::triggered, this, &PreviSat::RaccourciEvenements);
    this->addAction(_evenements);

    // Raccourci Onglet Informations satellite
    _informationsSatellite = new QAction(this);
    _informationsSatellite->setShortcut(Qt::ALT | Qt::Key_S);
    connect(_informationsSatellite, &QAction::triggered, this, &PreviSat::RaccourciInformations);
    this->addAction(_informationsSatellite);

    // Raccourci Onglet Recherche satellite
    _recherche = new QAction(this);
    _recherche->setShortcut(Qt::ALT | Qt::Key_R);
    connect(_recherche, &QAction::triggered, this, &PreviSat::RaccourciRecherche);
    this->addAction(_recherche);

    // Raccourci Onglet Informations ISS
    _station = new QAction(this);
    _station->setShortcut(Qt::ALT | Qt::Key_I);
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
        qInfo() << QString("%1 %2").arg(APP_NAME).arg(QString(APP_VERSION));
        qInfo() << QString("%1 %2 %3").arg(QSysInfo::productType()).arg(QSysInfo::productVersion()).arg(QSysInfo::currentCpuArchitecture());

        setWindowTitle(QString("%1 %2").arg(APP_NAME).arg(APP_VER_MAJ));

        ChargementTraduction(Configuration::instance()->locale());

        // Chargement de la configuration generale
        Configuration::instance()->Chargement();

        qInfo() << "Début Initialisation" << metaObject()->className();

        _informations = new Informations(this);
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

/*
 * Raccourcis vers les fonctionnalites
 */
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

/*
 * Boutons de l'interface graphique
 */
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

void PreviSat::on_aide_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString ficDox = QString("%1/%2/html/index.html").arg(Configuration::instance()->dirDox()).arg(Configuration::instance()->locale());

    /* Corps de la methode */
    if (!QDesktopServices::openUrl("file:///" + ficDox)) {
        Message::Afficher(tr("Impossible d'afficher l'aide en ligne"), MessageType::WARNING);
    }

    /* Retour */
    return;
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

/*
 * Menu deroulant
 */
void PreviSat::on_actionImporter_fichier_TLE_GP_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Ouverture d'un fichier TLE
        const QString fichier = QFileDialog::getOpenFileName(this, tr("Importer fichier GP / TLE"),
                                                             QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                             tr("Fichiers GP (*.xml);;" \
                                                                "Fichiers TLE (*.txt *.tle);;" \
                                                                "Tous les fichiers (*.*)"));

        // Ouverture du fichier d'elements orbitaux
        if (!fichier.isEmpty()) {

            qInfo() << "Ouverture du fichier" << fichier;

            QFileInfo ff(fichier);
            if (ff.suffix() == "xml") {

                // Cas d'un fichier GP
                const QMap<QString, ElementsOrbitaux> mapElements = GPFormat::LectureFichier(fichier, Configuration::instance()->donneesSatellites(),
                                                                                             Configuration::instance()->lgRec());

                if (mapElements.isEmpty()) {

                    qWarning() << QString("Le fichier GP %1 ne contient pas d'éléments orbitaux").arg(ff.fileName());
                    throw PreviSatException(tr("Le fichier %1 ne contient pas d'éléments orbitaux").arg(ff.fileName()), MessageType::WARNING);

                } else {

                    // Le fichier contient des elements orbitaux, on le copie dans le repertoire d'elements orbitaux
                    QFile fi(fichier);
                    if (fi.copy(Configuration::instance()->instance()->dirElem() + QDir::separator() + ff.fileName())) {

                        qInfo() << "Import du fichier GP" << ff.fileName() << "OK";
                        // TODO mettre a jour la liste de fichiers (et eventuellement ouvrir le fichier importe)

                    } else {
                        qWarning() << "Import du fichier GP" << ff.fileName() << "KO";
                    }

                }

            } else {

                // Cas d'un fichier TLE
                const int nbElem = TLE::VerifieFichier(fichier);

                if (nbElem > 0) {

                    // Le fichier contient des elements orbitaux, on le copie dans le repertoire d'elements orbitaux
                    QFile fi(fichier);
                    if (fi.copy(Configuration::instance()->instance()->dirElem() + QDir::separator() + ff.fileName())) {

                        qInfo() << "Import du fichier TLE" << ff.fileName() << "OK";
                        // TODO mettre a jour la liste de fichiers (et eventuellement ouvrir le fichier importe)

                    } else {
                        qWarning() << "Import du fichier TLE" << ff.fileName() << "KO";
                    }

                } else {
                    qWarning() << QString("Le fichier TLE %1 ne contient pas d'éléments orbitaux").arg(ff.fileName());
                    throw PreviSatException(tr("Le fichier %1 ne contient pas d'éléments orbitaux").arg(ff.fileName()), MessageType::WARNING);
                }
            }
        }

    } catch (PreviSatException &ex) {
    }

    /* Retour */
    return;
}

void PreviSat::on_actionEnregistrer_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

void PreviSat::on_actionImprimer_carte_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

void PreviSat::on_actionFichier_d_aide_triggered()
{
    on_aide_clicked();
}

void PreviSat::on_actionInformations_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QUrl urlLastNews(QString("%1/data/informations/").arg(DOMAIN_NAME) + "last_news_" + Configuration::instance()->locale() + ".html");

    if (Informations::UrlExiste(urlLastNews)) {

        _informations->setWindowModality(Qt::ApplicationModal);
        _informations->show();

        QFont fnt;
        fnt.setBold(false);
        _ui->actionInformations->setFont(fnt);

        if (!_majInfosDate.isEmpty()) {
            settings.setValue("temps/lastInfos", _majInfosDate);
        }

    } else {
        if (!_majInfosDate.isEmpty()) {
            Message::Afficher(tr("Pas d'informations à afficher"), MessageType::INFO);
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionOptions_triggered()
{
    on_configuration_clicked();
}

void PreviSat::on_actionOutils_triggered()
{
    on_outils_clicked();
}

void PreviSat::on_actionMettre_a_jour_GP_courant_triggered()
{

}

void PreviSat::on_actionMettre_a_jour_groupe_GP_courant_triggered()
{

}

void PreviSat::on_actionMettre_a_jour_GP_communs_triggered()
{

}

void PreviSat::on_actionMettre_a_jour_tous_les_groupes_de_GP_triggered()
{

}

void PreviSat::on_actionMettre_a_jour_les_fichiers_de_donnees_triggered()
{

}

void PreviSat::on_actionExporter_fichier_log_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString ficlog = Configuration::instance()->dirOut() + QDir::separator() + APP_NAME + ".log";

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirLog() + QDir::separator() + APP_NAME + "-prev.log");
    if (!fi.exists()) {
        fi.setFileName(Configuration::instance()->dirLog() + QDir::separator() + APP_NAME + ".log");
    }

    if (fi.exists()) {

        const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), ficlog, tr("Fichiers log (*.log)"));

        if (!fichier.isEmpty()) {

            // Sauvegarde du fichier log
            if (fi.copy(ficlog)) {
                qInfo() << "Export du fichier" << fi.fileName() << "OK";
            } else {
                qWarning() << "Export du fichier" << fi.fileName() << "KO";
            }
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionPayPal_triggered()
{
    QDesktopServices::openUrl(QUrl(settings.value("fichier/dirHttpPreviDon", "").toString()));
}

void PreviSat::on_actionTipeee_triggered()
{
    Message::Afficher(tr("Attention : Il est possible d'effectuer un don PayPal via Tipeee, mais ceci induira des frais supplémentaires"),
                      MessageType::INFO);
    QDesktopServices::openUrl(QUrl("https://tipeee.com/previsat"));
}

void PreviSat::on_actionUtip_triggered()
{
    Message::Afficher(tr("Attention : Il est possible d'effectuer un don PayPal via Utip, mais ceci induira des frais supplémentaires"),
                      MessageType::INFO);
    QDesktopServices::openUrl(QUrl("https://utip.io/previsat/"));
}

void PreviSat::on_actionSkywatcher_triggered()
{
    QDesktopServices::openUrl(QUrl("http://skywatcher.com/"));
}

void PreviSat::on_actionPianetaRadio_triggered()
{
    QDesktopServices::openUrl(QUrl("https://www.pianetaradio.it/"));
}

void PreviSat::on_actionCelestrak_triggered()
{
    QDesktopServices::openUrl(QUrl(Configuration::instance()->adresseCelestrak()));
}

void PreviSat::on_actionSpaceTrack_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.space-track.org"));
}

void PreviSat::on_actionContact_triggered()
{
    QDesktopServices::openUrl(QUrl("mailto:previsat.app@gmail.com"));
}

void PreviSat::on_actionApropos_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Apropos * const apropos = new Apropos(this);
    QEvent evt(QEvent::LanguageChange);

    apropos->changeEvent(&evt);
    apropos->setWindowModality(Qt::ApplicationModal);
    apropos->show();

    /* Retour */
    return;
}
