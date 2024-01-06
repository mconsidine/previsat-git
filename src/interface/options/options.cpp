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
 * >    options.cpp
 *
 * Localisation
 * >    interface.options
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 aout 2022
 *
 * Date de revision
 * >    23 decembre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QSettings>
#include <QTranslator>
#include "ui_options.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "options.h"
#include "telechargementoptions.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "configuration/fichierobs.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


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
Options::Options(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Options)
{
    _ui->setupUi(this);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), parent->geometry()));

    try {

        _creerCategorie = nullptr;
        _renommerCategorie = nullptr;
        _supprimerCategorie = nullptr;
        _telechargerCategorie = nullptr;

        _creerLieu = nullptr;
        _ajouterLieuMesPreferes = nullptr;
        _renommerLieu = nullptr;
        _modifierLieu = nullptr;
        _supprimerLieu = nullptr;

        _ui->configuration->setCurrentIndex(0);
        _ui->listeOptions->setCurrentRow(0);

        Initialisation();

        // Chargement des fichiers de preference
        ChargementPref();

        // Creation des menus contextuels
        CreerMenus();

        connect(_ui->listeBoutonsOptions->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &Options::AppliquerPreferences);

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Options::~Options()
{
    EFFACE_OBJET(_creerCategorie);
    EFFACE_OBJET(_renommerCategorie);
    EFFACE_OBJET(_supprimerCategorie);
    EFFACE_OBJET(_telechargerCategorie);

    EFFACE_OBJET(_creerLieu);
    EFFACE_OBJET(_ajouterLieuMesPreferes);
    EFFACE_OBJET(_renommerLieu);
    EFFACE_OBJET(_modifierLieu);
    EFFACE_OBJET(_supprimerLieu);

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Options *Options::ui()
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
 * Initialisation de la fenetre Options
 */
void Options::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->enregistrerPref->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));
    _ui->ajoutLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowRight));
    _ui->supprLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowLeft));
    _ui->listeOptions->setFocus();

    const QIcon ajout(":/resources/interface/ajout.png");
    _ui->creationCategorie->setIcon(ajout);
    _ui->creationCategorie->setToolTip(tr("Créer une catégorie"));
    _ui->creationLieu->setIcon(ajout);
    _ui->creationLieu->setToolTip(tr("Créer un nouveau lieu"));

    const QRegularExpressionValidator *valLon =
            new QRegularExpressionValidator(QRegularExpression("((0\\d\\d|1[0-7]\\d)°[0-5]\\d'[0-5]\\d\"|180°0?0'0?0\")"));
    _ui->nvLongitude->setValidator(valLon);

    const QRegularExpressionValidator *valLat =
            new QRegularExpressionValidator(QRegularExpression("((0\\d|[0-8]\\d)°[0-5]\\d'[0-5]\\d\"|90°0?0'0?0\")"));
    _ui->nvLatitude->setValidator(valLat);

    const QString unite = (_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot");
    QIntValidator *valAlt;

    if (_ui->unitesKm->isChecked()) {
        valAlt = new QIntValidator(-500, 8900);
        _ui->nvAltitude->setValidator(valAlt);
    } else {
        valAlt = new QIntValidator(-1640, 29200);
        _ui->nvAltitude->setValidator(valAlt);
    }

    const QString fmt = tr("L'altitude doit être comprise entre %1%2 et %3%2", "Observer altitude");
    _ui->nvAltitude->setToolTip(fmt.arg(valAlt->bottom()).arg(unite).arg(valAlt->top()));

    _ui->outilsLieuxObservation->setVisible(false);
    _ui->modeSombre->setVisible(false);

    const int offset = static_cast<int> (Date::CalculOffsetUTC(QDateTime::currentDateTime()) * DATE::NB_SEC_PAR_JOUR);
    _ui->updown->setValue(settings.value("affichage/updown", offset).toInt());

    // Affichage de la liste des fichiers sons
    InitFicSon();

    // Affichage de la liste des cartes du monde
    InitFicMap();

    // Affichage de la liste des langues disponibles
    InitFicLang();


    // Police Wall Command Center
    InitPoliceWCC();

    // Initialisation du Wall Command Center
    InitWallCommandCenter();

    // Chargement des stations
    InitChargementStations();


    // Chargement des fichiers de lieux d'observation
    InitFicObs();

    // Affichage des lieux d'observation selectionnes
    AffichageLieuObs();

    // Chargement des fichiers de preference
    InitFicPref();

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Appliquer les preferences d'affichage et enregistrer le fichier de preferences
 */
void Options::AppliquerPreferences()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    settings.setValue("affichage/affSAA", _ui->affSAA->isChecked());
    settings.setValue("affichage/affconst", _ui->affconst->checkState());
    settings.setValue("affichage/affcoord", _ui->affcoord->isChecked());
    settings.setValue("affichage/affetoiles", _ui->affetoiles->isChecked());
    settings.setValue("affichage/affgrille", _ui->affgrille->isChecked());
    settings.setValue("affichage/afficone", _ui->afficone->isChecked());
    settings.setValue("affichage/affinvew", _ui->affinvew->isChecked());
    settings.setValue("affichage/affinvns", _ui->affinvns->isChecked());
    settings.setValue("affichage/afflune", _ui->afflune->isChecked());
    settings.setValue("affichage/affnomlieu", _ui->affnomlieu->checkState());
    settings.setValue("affichage/affnomsat", _ui->affnomsat->checkState());
    settings.setValue("affichage/affnotif", _ui->affnotif->isChecked());
    settings.setValue("affichage/affnuit", _ui->affnuit->checkState());
    settings.setValue("affichage/affphaselune", _ui->affphaselune->isChecked());
    settings.setValue("affichage/affplanetes", _ui->affplanetes->checkState());
    settings.setValue("affichage/affradar", _ui->affradar->checkState());
    settings.setValue("affichage/affsoleil", _ui->affsoleil->isChecked());
    settings.setValue("affichage/afftraceCiel", _ui->afftraceCiel->isChecked());
    settings.setValue("affichage/afftraj", _ui->afftraj->isChecked());
    settings.setValue("affichage/affvisib", _ui->affvisib->checkState());
    settings.setValue("affichage/calJulien", _ui->calJulien->isChecked());
    settings.setValue("affichage/eclipsesLune", _ui->eclipsesLune->isChecked());
    settings.setValue("affichage/effetEclipsesMagnitude", _ui->effetEclipsesMagnitude->isChecked());
    settings.setValue("affichage/extinctionAtmospherique", _ui->extinctionAtmospherique->isChecked());
    settings.setValue("affichage/intensiteOmbre", _ui->intensiteOmbre->value());
    settings.setValue("affichage/langue", Configuration::instance()->listeFicLang().at(_ui->langue->currentIndex()));
    settings.setValue("affichage/modeSombre", _ui->modeSombre->isChecked());
    settings.setValue("affichage/magnitudeEtoiles", _ui->magnitudeEtoiles->value());
    settings.setValue("affichage/nombreTrajectoires", _ui->nombreTrajectoires->value());
    settings.setValue("affichage/notificationSonore", _ui->listeSons->currentIndex());
    settings.setValue("affichage/affichageFrontieres", _ui->affichageFrontieres->isChecked());
    settings.setValue("affichage/refractionAtmospherique", _ui->refractionAtmospherique->isChecked());
    settings.setValue("affichage/rotationIconeISS", _ui->rotationIconeISS->isChecked());
    settings.setValue("affichage/affNoradListes", _ui->affNoradListes->checkState());
    settings.setValue("affichage/rotationLune", _ui->rotationLune->isChecked());
    settings.setValue("affichage/systemeHoraire", _ui->syst24h->isChecked());
    settings.setValue("affichage/unite", _ui->unitesKm->isChecked());
    settings.setValue("affichage/updown", _ui->updown->value());
    settings.setValue("affichage/utc", _ui->utc->isChecked());
    settings.setValue("affichage/utcAuto", _ui->utcAuto->isChecked());
    settings.setValue("affichage/valeurZoomMap", _ui->valeurZoomMap->value());
    settings.setValue("affichage/verifMAJ", _ui->verifMAJ->isChecked());

    settings.setValue("affichage/affBetaWCC", _ui->affBetaWCC->isChecked());
    settings.setValue("affichage/affCerclesAcq", _ui->affCerclesAcq->isChecked());
    settings.setValue("affichage/affNbOrbWCC", _ui->affNbOrbWCC->isChecked());
    settings.setValue("affichage/affSAA_ZOE", _ui->affSAA_ZOE->isChecked());
    settings.setValue("affichage/styleWCC", _ui->styleWCC->isChecked());
    settings.setValue("affichage/coulGMT", _ui->coulGMT->currentIndex());
    settings.setValue("affichage/coulZOE", _ui->coulZOE->currentIndex());
    settings.setValue("affichage/coulCercleVisibilite", _ui->coulCercleVisibilite->currentIndex());
    settings.setValue("affichage/coulEquateur", _ui->coulEquateur->currentIndex());
    settings.setValue("affichage/coulTerminateur", _ui->coulTerminateur->currentIndex());
    settings.setValue("affichage/policeWCC", _ui->policeWCC->currentIndex());

    for(int i=0; i<_ui->listeStations->count(); i++) {
        settings.setValue("affichage/station" + _ui->listeStations->item(i)->data(Qt::UserRole).toString(),
                          _ui->listeStations->item(i)->checkState());
    }

    Configuration::instance()->setPoliceWcc(_ui->policeWCC->currentText());

    settings.setValue("fichier/nbFichiersLog", _ui->nbFichiersLog->value());

    // Ecriture du fichier de preferences
    if (_ui->preferences->currentIndex() < _ui->preferences->count() - 2) {

        const QString fichierPref = Configuration::instance()->dirPref() + QDir::separator() +
                Configuration::instance()->listeFicPref().at(_ui->preferences->currentIndex());

        SauvePreferences(fichierPref);
    }

    emit AfficherListeSatellites(Configuration::instance()->nomfic());

    /* Retour */
    return;
}

void Options::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void Options::show(QWidget *parent)
{
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), parent->geometry()));
    QDialog::show();
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
 * Affichage des lieux d'observation selectionnes
 */
void Options::AffichageLieuObs()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->selecLieux->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->observateurs());
    while (it.hasNext()) {
        const QString nomlieu = it.next().nomlieu();
        _ui->selecLieux->addItem(nomlieu);
    }

    /* Retour */
    return;
}

/*
 * Afficher les coordonnees du lieu selectionne
 */
void Options::AfficherLieu(const Observateur &obs)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->nLieu->setText(tr("Lieu : %1").arg(obs.nomlieu()));

    const double lo = obs.longitude();
    const double la = obs.latitude();
    const double atd = obs.altitude() * 1000.;

    const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
    const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");

    // Affichage des coordonnees
    const QString fmt = "%1 %2";
    _ui->nLongitude->setText(fmt.arg(Maths::ToSexagesimal(fabs(lo), AngleFormatType::DEGRE, 3, 0, false, true)).arg(ew));
    _ui->nLatitude->setText(fmt.arg(Maths::ToSexagesimal(fabs(la), AngleFormatType::DEGRE, 2, 0,false, true)).arg(ns));
    _ui->nAltitude->setText(fmt.arg((settings.value("affichage/unite").toBool()) ? atd : qRound(atd * TERRE::PIED_PAR_METRE + 0.5 * sgn(atd))).
                            arg((settings.value("affichage/unite").toBool()) ? tr("m", "meter") : tr("ft", "foot")));

    _ui->outilsLieuxObservation->setCurrentIndex(0);
    _ui->outilsLieuxObservation->setVisible(true);

    /* Retour */
    return;
}

/*
 * Chargement du fichier de preferences
 */
void Options::ChargementPref()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomPref = Configuration::instance()->dirPref() + QDir::separator() +
            Configuration::instance()->listeFicPref().at(_ui->preferences->currentIndex());

    /* Corps de la methode */
    QFile fichier(nomPref);
    if (fichier.exists() && (fichier.size() != 0)) {

        // Lecture du fichier de preferences
        if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {

            const QStringList listePrf = QString(fichier.readAll()).split("\n", Qt::SkipEmptyParts);
            fichier.close();

            QStringListIterator it(listePrf);
            while (it.hasNext()) {

                const QStringList item = it.next().split(" ", Qt::SkipEmptyParts);

                if (item.at(1) == "true") {
                    settings.setValue(item.first(), true);

                } else if (item.at(1) == "false") {
                    settings.setValue(item.first(), false);

                } else {
                    if (item.first() == "affichage/magnitudeEtoiles") {
                        settings.setValue(item.first(), item.at(1).toDouble());

                    } else if ((item.first() == "affichage/affconst") || (item.first() == "affichage/affnomlieu")
                               || (item.first() == "affichage/affnomsat") || (item.first() == "affichage/affplanetes")
                               || (item.first() == "affichage/affradar") || (item.first() == "affichage/affvisib")
                               || (item.first() == "affichage/intensiteOmbre") || (item.first() == "affichage/intensiteVision")) {
                        settings.setValue(item.first(), item.at(1).toUInt());
                    }
                }
            }
        }

        _ui->affconst->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt()));
        _ui->affcoord->setChecked(settings.value("affichage/affcoord", true).toBool());
        _ui->affetoiles->setChecked(settings.value("affichage/affetoiles", true).toBool());
        _ui->affgrille->setChecked(settings.value("affichage/affgrille", true).toBool());
        _ui->afficone->setChecked(settings.value("affichage/afficone", true).toBool());
        _ui->affinvew->setChecked(settings.value("affichage/affinvew", false).toBool());
        _ui->affinvns->setChecked(settings.value("affichage/affinvns", false).toBool());
        _ui->afflune->setChecked(settings.value("affichage/afflune", true).toBool());
        _ui->affnomlieu->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomlieu", Qt::Checked).toUInt()));
        _ui->affnomsat->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomsat", Qt::Checked).toUInt()));
        _ui->affnotif->setChecked(settings.value("affichage/affnotif", true).toBool());
        _ui->affnuit->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnuit", Qt::Checked).toUInt()));
        _ui->affphaselune->setChecked(settings.value("affichage/affphaselune", true).toBool());
        _ui->affplanetes->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affplanetes", Qt::Checked).toUInt()));
        _ui->affradar->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affradar", Qt::Checked).toUInt()));
        _ui->affSAA->setChecked(settings.value("affichage/affSAA", false).toBool());
        _ui->affsoleil->setChecked(settings.value("affichage/affsoleil", true).toBool());
        _ui->afftraceCiel->setChecked(settings.value("affichage/afftraceCiel", true).toBool());
        _ui->afftraj->setChecked(settings.value("affichage/afftraj", true).toBool());
        _ui->affvisib->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affvisib", Qt::Checked).toUInt()));
        _ui->calJulien->setChecked(settings.value("affichage/calJulien", false).toBool());
        _ui->extinctionAtmospherique->setChecked(settings.value("affichage/extinctionAtmospherique", true).toBool());
        _ui->refractionAtmospherique->setChecked(settings.value("affichage/refractionAtmospherique", true).toBool());
        _ui->effetEclipsesMagnitude->setChecked(settings.value("affichage/effetEclipsesMagnitude", true).toBool());
        _ui->eclipsesLune->setChecked(settings.value("affichage/eclipsesLune", true).toBool());
        _ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 30).toInt());
        _ui->modeSombre->setChecked(settings.value("affichage/intensiteVision", false).toBool());
        _ui->langue->setCurrentIndex(static_cast<int> (Configuration::instance()->listeFicLang()
                                                       .indexOf(settings.value("affichage/langue", "en").toString())));
        _ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
        _ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 2).toInt());
        _ui->affichageFrontieres->setChecked(settings.value("affichage/affichageFrontieres", true).toBool());
        _ui->rotationIconeISS->setChecked(settings.value("affichage/rotationIconeISS", true).toBool());
        _ui->affNoradListes->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affNoradListes", 0).toUInt()));
        _ui->rotationLune->setChecked(settings.value("affichage/rotationLune", false).toBool());
        _ui->utc->setChecked(settings.value("affichage/utc", false).toBool());
        _ui->utcAuto->setChecked(settings.value("affichage/utcAuto", true).toBool());
        _ui->verifMAJ->setChecked(settings.value("affichage/verifMAJ", false).toBool());
    }

    /* Retour */
    return;
}

/*
 * Creation des menus contextuels
 */
void Options::CreerMenus()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _creerCategorie = new QAction(tr("Créer une catégorie"), this);
    _creerCategorie->setIcon(QIcon(":/resources/interface/ajout.png"));
    connect(_creerCategorie, &QAction::triggered, this, &Options::CreerCategorie);

    _renommerCategorie = new QAction(tr("Renommer"), this);
    connect(_renommerCategorie, &QAction::triggered, this, &Options::RenommerCategorie);

    _supprimerCategorie = new QAction(tr("Supprimer"), this);
    _supprimerCategorie->setIcon(QIcon(":/resources/interface/suppr.png"));
    connect(_supprimerCategorie, &QAction::triggered, this, &Options::SupprimerCategorie);

    _telechargerCategorie = new QAction(tr("Télécharger..."), this);
    connect(_telechargerCategorie, &QAction::triggered, this, &Options::TelechargerCategorie);


    _creerLieu = new QAction(tr("Créer un nouveau lieu"), this);
    _creerLieu->setIcon(QIcon(":/resources/interface/ajout.png"));
    connect(_creerLieu, &QAction::triggered, this, &Options::CreerLieu);

    _ajouterLieuMesPreferes = new QAction(tr("Ajouter à Mes Préférés"), this);
    _ajouterLieuMesPreferes->setIcon(QIcon(":/resources/interface/pref.png"));
    connect(_ajouterLieuMesPreferes, &QAction::triggered, this, &Options::AjouterLieuMesPreferes);

    _renommerLieu = new QAction(tr("Renommer"), this);
    connect(_renommerLieu, &QAction::triggered, this, &Options::RenommerLieu);

    _modifierLieu = new QAction(tr("Modifier"), this);
    _modifierLieu->setIcon(QIcon(":/resources/interface/editer.png"));
    connect(_modifierLieu, &QAction::triggered, this, &Options::ModifierLieu);

    _supprimerLieu = new QAction(tr("Supprimer"), this);
    _supprimerLieu->setIcon(QIcon(":/resources/interface/suppr.png"));
    connect(_supprimerLieu, &QAction::triggered, this, &Options::SupprimerLieu);

    /* Retour */
    return;
}

/*
 * Chargement de la liste des stations
 */
void Options::InitChargementStations()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->listeStations->clear();

    /* Corps de la methode */
    QMapIterator it(Configuration::instance()->mapStations());
    while (it.hasNext()) {
        it.next();

        const QString acronyme = it.key();
        const QString nom = it.value().nomlieu();

        QListWidgetItem * const station = new QListWidgetItem(QString("%1 (%2)").arg(nom).arg(acronyme), _ui->listeStations);
        station->setCheckState((static_cast<Qt::CheckState> (settings.value("affichage/station" + acronyme, Qt::Checked).
                                                             toUInt())) ? Qt::Checked : Qt::Unchecked);
        station->setData(Qt::UserRole, acronyme);
    }

    _ui->listeStations->sortItems();

    /* Retour */
    return;
}

/*
 * Chargement de la liste des langues disponibles
 */
void Options::InitFicLang()
{
    /* Declarations des variables locales */
    QTranslator trad;

    /* Initialisations */
    QPixmap fond(30, 20);
    fond.fill(_ui->langue->palette().window().color());
    _ui->langue->clear();
    _ui->langue->addItem(QIcon(":/resources/drapeaux/fr.png"), "Français");

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicLang());
    it.next();
    while (it.hasNext()) {

        const QString locale = it.next();
        const QString nomFicTrad = QString("%1_%2.qm").arg(APP_NAME).arg(locale);

        // Nom de la langue dans le fichier de traduction
        if (trad.load(Configuration::instance()->dirLang() + QDir::separator() + nomFicTrad)) {

            const QString langue = trad.translate("Options", "Langue", "Translate by the name of language, for example : English, Français, Español");

            const QFileInfo fi(":/resources/drapeaux/" + locale + ".png");
            const QIcon drapeau = (fi.exists()) ? QIcon(fi.filePath()) : fond;

            _ui->langue->addItem(drapeau, langue);
        }
    }

    _ui->langue->setCurrentIndex(static_cast<int> (Configuration::instance()->listeFicLang()
                                                      .indexOf(settings.value("affichage/langue", "en").toString())));

    /* Retour */
    return;
}

/*
 * Chargement de la liste des cartes du monde
 */
void Options::InitFicMap()
{
    /* Declarations des variables locales */
    QString nomFicMap;

    /* Initialisations */
    const bool etat = _ui->listeMap->blockSignals(true);
    _ui->listeMap->clear();
    _ui->listeMap->addItem(tr("* Défaut"));

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicMap());
    while (it.hasNext()) {

        nomFicMap = it.next().section(".", -2, -2);
        nomFicMap[0] = nomFicMap[0].toUpper();
        _ui->listeMap->addItem(nomFicMap);
    }

    _ui->listeMap->addItem(tr("Télécharger..."));
    const QString map = settings.value("fichier/nomMap").toString().trimmed();
    _ui->listeMap->setCurrentIndex(static_cast<int> ((map.isEmpty()) ? 0 : Configuration::instance()->listeFicMap().indexOf(map)+1));
    _ui->listeMap->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Chargement de la liste des fichiers de lieux d'observation
 */
void Options::InitFicObs()
{
    /* Declarations des variables locales */
    QListWidgetItem *elem;
    QString nomFicObs;
    QString nomPays;

    /* Initialisations */
    _ui->categoriesObs->clear();
    _ui->ajdfic->clear();

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicObs());
    while (it.hasNext()) {

        nomFicObs = it.next();
        if (nomFicObs == "preferes.xml") {

            elem = new QListWidgetItem(tr("Mes Préférés"));
            elem->setData(Qt::UserRole, nomFicObs);
            _ui->categoriesObs->insertItem(0, elem);
            _ui->ajdfic->insertItem(0, elem->text(), elem->data(Qt::UserRole));

        } else {

            nomPays = nomFicObs.section(".", -2, -2);
            nomPays[0] = nomPays[0].toUpper();

            elem = new QListWidgetItem(nomPays);
            elem->setData(Qt::UserRole, nomFicObs);
            _ui->categoriesObs->addItem(elem);
            _ui->ajdfic->addItem(nomPays, elem->data(Qt::UserRole));
        }
    }

    _ui->categoriesObs->setCurrentRow(0);

    /* Retour */
    return;
}

void Options::InitFicPref()
{
    /* Declarations des variables locales */
    QString fichier;

    /* Initialisations */
    _ui->preferences->clear();

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicPref());
    while (it.hasNext()) {

        const QString fic = Configuration::instance()->dirPref() + QDir::separator() + it.next();
        const QFileInfo fi(fic);
        fichier = fi.completeBaseName();
        fichier[0] = fichier[0].toUpper();

        _ui->preferences->addItem((fi.completeBaseName() == "defaut") ? tr("* Défaut") : fichier);
        if (settings.value("fichier/preferences", Configuration::instance()->dirPref() + QDir::separator() + "defaut").toString() == fic) {
            _ui->preferences->setCurrentIndex(_ui->preferences->count() - 1);
        }
    }

    _ui->preferences->addItem(tr("Enregistrer sous..."));
    _ui->preferences->addItem(tr("Supprimer..."));

    /* Retour */
    return;
}

/*
 * Chargement de la liste des fichiers de notification sonore
 */
void Options::InitFicSon()
{
    /* Declarations des variables locales */
    QString nomFicSon;

    /* Initialisations */
    const bool etat = _ui->listeSons->blockSignals(true);
    _ui->listeSons->clear();

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicSon());
    while (it.hasNext()) {

        nomFicSon = it.next();
        if (nomFicSon == "default") {
            nomFicSon = tr("* Défaut");
        } else {
            nomFicSon[0] = nomFicSon[0].toUpper();
        }
        _ui->listeSons->addItem(nomFicSon);
    }
    _ui->listeSons->addItem(tr("Télécharger..."));
    _ui->listeSons->setCurrentIndex(settings.value("affichage/notificationSonore", 0).toInt());
    _ui->listeSons->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Initialisation de la police du Wall Command Center
 */
void Options::InitPoliceWCC()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const int index = settings.value("affichage/policeWCC", 0).toInt();
    _ui->policeWCC->clear();

#if defined (Q_OS_WIN)
    _ui->policeWCC->addItem("Lucida Console");
    _ui->policeWCC->addItem("MS Shell Dlg 2");

    const int taille = 10;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#elif defined (Q_OS_LINUX)
    _ui->policeWCC->addItem("FreeSans");
    _ui->policeWCC->addItem("Sans Serif");

    const int taille = 11;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille);

#elif defined (Q_OS_MAC)
    _ui->policeWCC->addItem("Lucida Grande");
    _ui->policeWCC->addItem("Marion");

    const int taille = 13;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#else
    const int taille = 11;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille);

#endif

    _ui->policeWCC->setCurrentIndex(index);
    Configuration::instance()->setPoliceWcc(policeWcc);

    /* Retour */
    return;
}

/*
 * Initialisation des options du Wall Command Center
 */
void Options::InitWallCommandCenter()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->affBetaWCC->setChecked(settings.value("affichage/affBetaWCC", false).toBool());
    _ui->affCerclesAcq->setChecked(settings.value("affichage/affCerclesAcq", true).toBool());
    _ui->affNbOrbWCC->setChecked(settings.value("affichage/affNbOrbWCC", true).toBool());
    _ui->affSAA_ZOE->setChecked(settings.value("affichage/affSAA_ZOE", true).toBool());
    _ui->styleWCC->setChecked(settings.value("affichage/styleWCC", true).toBool());
    _ui->coulGMT->setCurrentIndex(settings.value("affichage/coulGMT", 0).toInt());
    _ui->coulZOE->setCurrentIndex(settings.value("affichage/coulZOE", 0).toInt());
    _ui->coulCercleVisibilite->setCurrentIndex(settings.value("affichage/coulCercleVisibilite", 0).toInt());
    _ui->coulEquateur->setCurrentIndex(settings.value("affichage/coulEquateur", 0).toInt());
    _ui->coulTerminateur->setCurrentIndex(settings.value("affichage/coulTerminateur").toInt());

    /* Retour */
    return;
}

/*
 * Sauvegarde des preferences d'affichage
 */
void Options::SauvePreferences(const QString &fichierPref)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile fi(fichierPref);

        if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

            if (!fi.isWritable()) {
                const QString msg = tr("Problème de droits d'écriture du fichier %1");
                throw PreviSatException(msg.arg(fi.fileName()), MessageType::WARNING);
            }

            QTextStream flux(&fi);

            flux << "affichage/affSAA " << QVariant(_ui->affSAA->isChecked()).toString() << Qt::endl
                 << "affichage/affconst " << _ui->affconst->checkState() << Qt::endl
                 << "affichage/affcoord " << QVariant(_ui->affcoord->isChecked()).toString() << Qt::endl
                 << "affichage/affetoiles " << QVariant(_ui->affetoiles->isChecked()).toString() << Qt::endl
                 << "affichage/affgrille " << QVariant(_ui->affgrille->isChecked()).toString() << Qt::endl
                 << "affichage/afficone " << QVariant(_ui->afficone->isChecked()).toString() << Qt::endl
                 << "affichage/affinvew " << QVariant(_ui->affinvew->isChecked()).toString() << Qt::endl
                 << "affichage/affinvns " << QVariant(_ui->affinvns->isChecked()).toString() << Qt::endl
                 << "affichage/afflune " << QVariant(_ui->afflune->isChecked()).toString() << Qt::endl
                 << "affichage/affnomlieu " << _ui->affnomlieu->checkState() << Qt::endl
                 << "affichage/affnomsat " << _ui->affnomsat->checkState() << Qt::endl
                 << "affichage/affNoradListes " << _ui->affNoradListes->checkState() << Qt::endl
                 << "affichage/affnotif " << QVariant(_ui->affnotif->isChecked()).toString() << Qt::endl
                 << "affichage/affnuit " << _ui->affnuit->checkState() << Qt::endl
                 << "affichage/affphaselune " << QVariant(_ui->affphaselune->isChecked()).toString() << Qt::endl
                 << "affichage/affplanetes " << _ui->affplanetes->checkState() << Qt::endl
                 << "affichage/affradar " << _ui->affradar->checkState() << Qt::endl
                 << "affichage/affsoleil " << QVariant(_ui->affsoleil->isChecked()).toString() << Qt::endl
                 << "affichage/afftraceCiel " << QVariant(_ui->afftraceCiel->isChecked()).toString() << Qt::endl
                 << "affichage/afftraj " << QVariant(_ui->afftraj->isChecked()).toString() << Qt::endl
                 << "affichage/affvisib " << _ui->affvisib->checkState() << Qt::endl
                 << "affichage/calJulien " << QVariant(_ui->calJulien->isChecked()).toString() << Qt::endl
                 << "affichage/eclipsesLune " << QVariant(_ui->eclipsesLune->isChecked()).toString() << Qt::endl
                 << "affichage/effetEclipsesMagnitude " << QVariant(_ui->effetEclipsesMagnitude->isChecked()).toString() << Qt::endl
                 << "affichage/extinctionAtmospherique " << QVariant(_ui->extinctionAtmospherique->isChecked()).toString() << Qt::endl
                 << "affichage/intensiteOmbre " << _ui->intensiteOmbre->value() << Qt::endl
                 << "affichage/modeSombre " << QVariant(_ui->modeSombre->isChecked()).toString() << Qt::endl
                 << "affichage/magnitudeEtoiles " << _ui->magnitudeEtoiles->value() << Qt::endl
                 << "affichage/nombreTrajectoires " << _ui->nombreTrajectoires->value() << Qt::endl
                 << "affichage/affichageFrontieres " << QVariant(_ui->affichageFrontieres->isChecked()).toString() << Qt::endl
                 << "affichage/refractionAtmospherique " << QVariant(_ui->refractionAtmospherique->isChecked()).toString() << Qt::endl
                 << "affichage/rotationIconeISS " << QVariant(_ui->rotationIconeISS->isChecked()).toString() << Qt::endl
                 << "affichage/rotationLune " << QVariant(_ui->rotationLune->isChecked()).toString() << Qt::endl
                 << "affichage/systemeHoraire " << QVariant(_ui->syst24h->isChecked()).toString() << Qt::endl
                 << "affichage/unite " << QVariant(_ui->unitesKm->isChecked()).toString() << Qt::endl
                 << "affichage/utc " << QVariant(_ui->utc->isChecked()).toString() << Qt::endl
                 << "affichage/utcAuto " << QVariant(_ui->utcAuto->isChecked()).toString() << Qt::endl
                 << "affichage/valeurZoomMap " << _ui->valeurZoomMap->value() << Qt::endl
                 << "affichage/verifMAJ " << QVariant(_ui->verifMAJ->isChecked()).toString() << Qt::endl

                 << "affichage/affBetaWCC " << QVariant(_ui->affBetaWCC->isChecked()).toString() << Qt::endl
                 << "affichage/affCerclesAcq " << QVariant(_ui->affCerclesAcq->isChecked()).toString() << Qt::endl
                 << "affichage/affNbOrbWCC " << QVariant(_ui->affNbOrbWCC->isChecked()).toString() << Qt::endl
                 << "affichage/aff_ZOE " << QVariant(_ui->affSAA_ZOE->isChecked()).toString() << Qt::endl
                 << "affichage/styleWCC " << QVariant(_ui->styleWCC->isChecked()).toString() << Qt::endl
                 << "affichage/coulGMT " << _ui->coulGMT->currentIndex() << Qt::endl
                 << "affichage/coulZOE " << _ui->coulZOE->currentIndex() << Qt::endl
                 << "affichage/coulCercleVisibilite " << _ui->coulCercleVisibilite->currentIndex() << Qt::endl
                 << "affichage/coulEquateur " << _ui->coulEquateur->currentIndex() << Qt::endl
                 << "affichage/coulTerminateur " << _ui->coulTerminateur->currentIndex() << Qt::endl
                 << "affichage/policeWCC " << _ui->policeWCC->currentIndex() << Qt::endl;

            for(const QString &station : Configuration::instance()->mapStations().keys()) {
                flux << "affichage/station" + station + " " + QVariant(settings.value("affichage/station" + station).toString()).toString() << Qt::endl;
            }

            fi.close();
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Options::CreerCategorie()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->outilsLieuxObservation->setCurrentWidget(_ui->nouvelleCategorie);
    _ui->outilsLieuxObservation->setVisible(true);
    _ui->validerCategorie->setDefault(true);
    _ui->nvCategorie->setText("");
    _ui->nvCategorie->setFocus();

    /* Retour */
    return;
}

void Options::RenommerCategorie()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QInputDialog input(this, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    input.setWindowTitle(tr("Catégorie"));
    input.setLabelText(tr("Nouveau nom de la catégorie :"));
    input.setTextValue(_ui->categoriesObs->currentItem()->text());
    input.setTextEchoMode(QLineEdit::Normal);
    input.setOkButtonText(tr("OK"));
    input.setCancelButtonText(tr("Annuler"));

    const int ret = input.exec();

    if (ret != 0) {

        const QString nvNomCategorie = input.textValue();

        if (!nvNomCategorie.trimmed().isEmpty()) {

            bool ok = true;
            QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + _ui->categoriesObs->currentItem()->text().toLower());

            if (fi.exists()) {

                QMessageBox msgbox(QMessageBox::Question, tr("Information"), tr("La catégorie existe déjà. Voulez-vous l'écraser ?"));
                const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
                QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
                msgbox.setDefaultButton(non);
                msgbox.exec();

                ok = (msgbox.clickedButton() == oui);

            } else {
                ok = true;
            }

            if (ok) {
                fi.rename(Configuration::instance()->dirCoord() + QDir::separator() + nvNomCategorie.trimmed().toLower());
                InitFicObs();
            }
        }
    }

    /* Retour */
    return;
}

void Options::SupprimerCategorie()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString categorie = _ui->categoriesObs->currentItem()->text();
    const QString fic = categorie.toLower();

    /* Corps de la methode */
    QMessageBox msgbox(QMessageBox::Question, tr("Information"), tr("Voulez-vous vraiment supprimer la catégorie <b>%1</b> ?").arg(categorie));
    const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
    msgbox.setDefaultButton(non);
    msgbox.exec();

    if (msgbox.clickedButton() == oui) {

        QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + fic);
        fi.remove();

        _ui->lieuxObs->clear();
        InitFicObs();

        _ui->categoriesObs->setCurrentRow(0);
    }

    /* Retour */
    return;
}

void Options::TelechargerCategorie()
{
    TelechargementOptions * const telechargementOptions = new TelechargementOptions(TypeTelechargement::COORDONNEES, this);
    telechargementOptions->setWindowModality(Qt::ApplicationModal);
    telechargementOptions->show();
}


void Options::CreerLieu()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->outilsLieuxObservation->setCurrentWidget(_ui->nouveauLieu);
    _ui->outilsLieuxObservation->setVisible(true);

    _ui->nvLieu->setText("");
    _ui->nvLieu->setReadOnly(false);
    _ui->nvLongitude->setText("000°00'00\"");
    _ui->nvLatitude->setText("000°00'00\"");

    if (_ui->unitesKm->isChecked()) {
        _ui->nvAltitude->setText("0000");
        _ui->nvAltitude->setInputMask("####");
    } else {
        _ui->nvAltitude->setText("00000");
        _ui->nvAltitude->setInputMask("#####");
    }

    _ui->lbl_nvUnite->setText((_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot"));
    _ui->lbl_ajouterDans->setVisible(true);
    _ui->ajdfic->setVisible(true);
    _ui->ajdfic->setCurrentIndex(_ui->categoriesObs->currentRow());
    _ui->validerObs->setDefault(true);
    _ui->nvLieu->setFocus();

    /* Retour */
    return;
}

void Options::AjouterLieuMesPreferes()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QMap<QString, Observateur> mapObs = Configuration::instance()->mapObs();
        const QMap<QString, Observateur> mapObsFic = FichierObs::Lecture(_ui->categoriesObs->currentItem()->data(Qt::UserRole).toString(), false);
        const Observateur lieu = mapObsFic.value(_ui->lieuxObs->currentItem()->text());

        QMap<QString, Observateur> mapObsPref = FichierObs::Lecture("preferes.xml", false);

        // Verification que le lieu d'observation n'existe pas deja dans Mes Preferes
        const QString nomlieu = lieu.nomlieu();
        if (mapObsPref.contains(nomlieu)) {

            Message::Afficher(tr("Le lieu d'observation <b>%1</b> fait déjà partie de <b>Mes Préférés</b>").arg(nomlieu), MessageType::WARNING);

        } else {

            mapObsPref.insert(nomlieu, lieu);
            Configuration::instance()->mapObs() = mapObsPref;
            FichierObs::Ecriture("preferes.xml");
            Configuration::instance()->mapObs() = mapObs;

            InitFicObs();
            _ui->categoriesObs->setCurrentRow(0);
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Options::RenommerLieu()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QInputDialog input(this, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    input.setWindowTitle(tr("Lieu d'observation"));
    input.setLabelText(tr("Nouveau nom du lieu d'observation :"));
    input.setTextValue(_ui->lieuxObs->currentItem()->text());
    input.setTextEchoMode(QLineEdit::Normal);
    input.setOkButtonText(tr("OK"));
    input.setCancelButtonText(tr("Annuler"));

    const int ret = input.exec();

    if (ret != 0) {

        const QString nvNomLieu = input.textValue();

        if (!nvNomLieu.trimmed().isEmpty()) {

            const QString fic = _ui->categoriesObs->currentItem()->data(Qt::UserRole).toString();
            Configuration::instance()->mapObs() = FichierObs::Lecture(fic, false);
            QMap<QString, Observateur> &mapObs = Configuration::instance()->mapObs();

            const Observateur obs = mapObs.value(_ui->lieuxObs->currentItem()->text());
            const QString nomlieu = obs.nomlieu();

            const Observateur nvObs(nvNomLieu, obs.longitude(), obs.latitude(), obs.altitude());
            mapObs.remove(nomlieu);
            mapObs.insert(nvNomLieu, nvObs);
            FichierObs::Ecriture(fic);

            on_categoriesObs_currentRowChanged(_ui->categoriesObs->currentRow());
        }
    }

    /* Retour */
    return;
}

void Options::ModifierLieu()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->outilsLieuxObservation->setCurrentWidget(_ui->nouveauLieu);
    _ui->outilsLieuxObservation->setVisible(true);
    _ui->lbl_ajouterDans->setVisible(false);
    _ui->ajdfic->setVisible(false);

    if (_ui->unitesKm->isChecked()) {
        _ui->nvAltitude->setInputMask("####");
    } else {
        _ui->nvAltitude->setInputMask("#####");
    }

    try {

        Configuration::instance()->mapObs() = FichierObs::Lecture(_ui->categoriesObs->currentItem()->data(Qt::UserRole).toString(), false);
        const Observateur obs = Configuration::instance()->mapObs().value(_ui->lieuxObs->currentItem()->text());

        _ui->nvLieu->setText(obs.nomlieu().trimmed());
        _ui->nvLieu->setReadOnly(true);

        _ui->nvLongitude->setText(Maths::ToSexagesimal(fabs(obs.longitude()), AngleFormatType::DEGRE, 3, 0, false, true));
        _ui->nvLongitude->setPalette(QPalette());
        _ui->nvEw->setCurrentIndex((obs.longitude() <= 0.) ? 0 : 1);

        _ui->nvLatitude->setText(Maths::ToSexagesimal(fabs(obs.latitude()), AngleFormatType::DEGRE, 2, 0,false, true));
        _ui->nvLatitude->setPalette(QPalette());
        _ui->nvNs->setCurrentIndex((obs.latitude() >= 0.) ? 0 : 1);

        const QString alt = "%1";
        const int atd = static_cast<int> (qRound(obs.altitude() * 1.e3));
        if (_ui->unitesKm->isChecked()) {
            _ui->nvAltitude->setText(alt.arg(atd, 4, 10, QChar('0')));
        } else {
            _ui->nvAltitude->setText(alt.arg(qRound(atd * TERRE::PIED_PAR_METRE + 0.5 * sgn(atd)), 5, 10, QChar('0')));
        }

        _ui->nvAltitude->setPalette(QPalette());
        _ui->lbl_nvUnite->setText((_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot"));

        _ui->nvLieu->setFocus();

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Options::SupprimerLieu()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString nomlieu = _ui->lieuxObs->currentItem()->text();
        const QString msg = tr("Voulez-vous vraiment supprimer <b>%1</b> de la catégorie <b>%2</b> ?");

        QMessageBox msgbox(QMessageBox::Question, tr("Avertissement"), msg.arg(nomlieu).arg(_ui->categoriesObs->currentItem()->text()));
        const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
        QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
        msgbox.setDefaultButton(non);
        msgbox.exec();

        if (msgbox.clickedButton() == oui) {

            const QString fic = _ui->categoriesObs->currentItem()->data(Qt::UserRole).toString();
            Configuration::instance()->mapObs() = FichierObs::Lecture(fic, false);

            QMap<QString, Observateur> &mapObs = Configuration::instance()->mapObs();
            mapObs.remove(nomlieu);

            FichierObs::Ecriture(fic);
            _ui->outilsLieuxObservation->setVisible(false);

            on_categoriesObs_currentRowChanged(_ui->categoriesObs->currentRow());
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Options::closeEvent(QCloseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    AppliquerPreferences();
    settings.setValue("fichier/listeMap", (_ui->listeMap->currentIndex() > 0) ?
                          Configuration::instance()->dirMap() + QDir::separator() +
                          Configuration::instance()->listeFicMap().at(qMax(0, _ui->listeMap->currentIndex() - 1)) : "");

    if (!_ui->verifMAJ->isChecked()) {
        settings.setValue("fichier/majPrevi", "0");
    }

    /* Retour */
    return;
}

void Options::on_listeOptions_currentRowChanged(int currentRow)
{
    _ui->stackedWidget_options->setCurrentIndex(currentRow);
}

void Options::on_listeBoutonsOptions_accepted()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    AppliquerPreferences();

    const QString langue = Configuration::instance()->listeFicLang().at(_ui->langue->currentIndex());
    Configuration::instance()->locale() = langue;

    emit ChargementTraduction(langue);
    emit ChargementCarteDuMonde();
    emit ChangementFuseauHoraire((_ui->utc->isChecked()) ? 0 : _ui->updown->value());

    /* Retour */
    return;
}

void Options::on_listeBoutonsOptions_rejected()
{
    // Rechargement du fichier de preferences si une option a ete modifiee et ne doit pas etre sauvegardee
    ChargementPref();
}


void Options::on_creationCategorie_clicked()
{
    CreerCategorie();
}

void Options::on_categoriesObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        _ui->lieuxObs->clear();

        // Affichage des lieux d'observation contenus dans le fichier
        const QString ficObs = _ui->categoriesObs->item(currentRow)->data(Qt::UserRole).toString();

        _mapObs = FichierObs::Lecture(ficObs, false);

        QStringListIterator it(_mapObs.keys());
        while (it.hasNext()) {
            _ui->lieuxObs->addItem(it.next());
        }
    }

    /* Retour */
    return;
}

void Options::on_categoriesObs_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QListWidgetItem const *item = _ui->categoriesObs->itemAt(pos);

    /* Corps de la methode */
    QMenu menu(this);
    menu.addAction(_creerCategorie);

    if ((item != nullptr) && (_ui->categoriesObs->currentItem()->data(Qt::UserRole).toString() != "preferes.xml")) {
        menu.addAction(_renommerCategorie);
        menu.addAction(_supprimerCategorie);
    }

    menu.addAction(_telechargerCategorie);
    menu.exec(QCursor::pos());

    /* Retour */
    return;
}

void Options::on_validerCategorie_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->nvCategorie->text().trimmed().isEmpty()) {
        Message::Afficher(tr("Le nom de la catégorie n'est pas spécifié"), MessageType::WARNING);
    } else {

        if (_ui->categoriesObs->findItems(_ui->nvCategorie->text(), Qt::MatchContains).isEmpty()) {

            QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + _ui->nvCategorie->text().toLower());
            if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {
                fi.write("");
            }
            fi.close();

            InitFicObs();

            if (_ui->categoriesObs->count() > 0) {
                _ui->categoriesObs->setCurrentRow(0);
            }

            _ui->outilsLieuxObservation->setVisible(false);

        } else {
            Message::Afficher(tr("La catégorie spécifiée existe déjà"), MessageType::WARNING);
        }
    }

    /* Retour */
    return;
}

void Options::on_annulerCategorie_clicked()
{
    _ui->outilsLieuxObservation->setVisible(false);
}


void Options::on_filtreLieuxObs_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->lieuxObs->count(); i++) {
        const QString elem = _ui->lieuxObs->item(i)->text();
        _ui->lieuxObs->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void Options::on_lieuxObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        // Affichage des coordonnees du lieu d'observation
        const Observateur obs = _mapObs[_ui->lieuxObs->currentItem()->text()];
        AfficherLieu(obs);
    }

    /* Retour */
    return;
}

void Options::on_lieuxObs_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QListWidgetItem const *item = _ui->lieuxObs->itemAt(pos);

    /* Corps de la methode */
    QMenu menu(this);
    menu.addAction(_creerLieu);

    if (_ui->categoriesObs->currentItem()->data(Qt::UserRole).toString() != "preferes.xml") {
        menu.addAction(_ajouterLieuMesPreferes);
    }

    if (item != nullptr) {
        menu.addAction(_renommerLieu);
        menu.addAction(_modifierLieu);
        menu.addAction(_supprimerLieu);
    }

    menu.exec(QCursor::pos());

    /* Retour */
    return;
}

void Options::on_filtreSelecLieux_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->selecLieux->count(); i++) {
        const QString elem = _ui->selecLieux->item(i)->text();
        _ui->selecLieux->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void Options::on_selecLieux_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        // Affichage des coordonnees du lieu d'observation
        const Observateur obs = Configuration::instance()->observateurs().at(currentRow);
        AfficherLieu(obs);
    }

    /* Retour */
    return;
}

void Options::on_creationLieu_clicked()
{
    CreerLieu();
}

void Options::on_validerObs_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Nom du lieu d'observation
        QString nomlieu = _ui->nvLieu->text().trimmed();

        if (nomlieu.isEmpty()) {
            throw PreviSatException(tr("Le nom du lieu d'observation n'est pas spécifié"), MessageType::WARNING);
        }

        const QString fic = _ui->ajdfic->currentData(Qt::UserRole).toString();
        Configuration::instance()->mapObs() = FichierObs::Lecture(fic, false);

        nomlieu[0] = nomlieu.at(0).toUpper();

        if (Configuration::instance()->mapObs().contains(nomlieu)) {
            throw PreviSatException(tr("Le lieu existe déjà dans la catégorie <b>%1</b>").arg(_ui->ajdfic->currentText()), MessageType::WARNING);
        }

        // Recuperation de la longitude
        const QStringList lon = _ui->nvLongitude->text().split(QRegularExpression("[°'\"]"), Qt::SkipEmptyParts);
        const int lo1 = lon.at(0).toInt();
        const int lo2 = lon.at(1).toInt();
        const int lo3 = lon.at(2).toInt();

        // Recuperation de la latitude
        const QStringList lat = _ui->nvLatitude->text().split(QRegularExpression("[°'\"]"), Qt::SkipEmptyParts);
        const int la1 = lat.at(0).toInt();
        const int la2 = lat.at(1).toInt();
        const int la3 = lat.at(2).toInt();

        // Recuperation de l'altitude
        int atd = _ui->nvAltitude->text().toInt();
        if (_ui->unitesMi->isChecked()) {
            atd = qRound(atd / TERRE::PIED_PAR_METRE);
        }

        const double longitude = ((_ui->nvEw->currentText() == tr("Est")) ? -1. : 1.) *
                (lo1 + lo2 * MATHS::DEG_PAR_ARCMIN + lo3 * MATHS::DEG_PAR_ARCSEC);
        const double latitude = ((_ui->nvNs->currentText() == tr("Sud")) ? -1. : 1.) *
                (la1 + la2 * MATHS::DEG_PAR_ARCMIN + la3 * MATHS::DEG_PAR_ARCSEC);

        const Observateur obs(nomlieu, longitude, latitude, atd);
        Configuration::instance()->mapObs().insert(nomlieu, obs);

        FichierObs::Ecriture(fic);
        InitFicObs();

        on_categoriesObs_currentRowChanged(_ui->categoriesObs->currentRow());
        _ui->outilsLieuxObservation->setVisible(false);

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Options::on_annulerObs_clicked()
{
    _ui->outilsLieuxObservation->setVisible(false);
}

void Options::on_ajoutLieu_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (_ui->lieuxObs->currentRow() >= 0) {

            const QString nomlieu = _ui->lieuxObs->currentItem()->text();

            if (!_ui->selecLieux->findItems(nomlieu, Qt::MatchExactly).isEmpty()) {
                throw PreviSatException(tr("Lieu d'observation déjà sélectionné"), MessageType::WARNING);
            }

            const Observateur obs = _mapObs[_ui->lieuxObs->currentItem()->text()];
            Configuration::instance()->observateurs().append(obs);
            GestionnaireXml::EcritureConfiguration();

            AffichageLieuObs();
            _ui->lieuxObs->setFocus();
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Options::on_supprLieu_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if ((_ui->selecLieux->currentRow() >= 0) && (_ui->selecLieux->count() > 1)) {

        Configuration::instance()->observateurs().removeAt(_ui->selecLieux->currentRow());
        AffichageLieuObs();
        GestionnaireXml::EcritureConfiguration();
        _ui->outilsLieuxObservation->setVisible(false);
        AffichageLieuObs();
    }

    /* Retour */
    return;
}


void Options::on_afficone_toggled(bool checked)
{
    _ui->rotationIconeISS->setEnabled(checked);
}


void Options::on_listeMap_currentIndexChanged(int index)
{
    if (index == _ui->listeMap->count() - 1) {

        TelechargementOptions * const telechargementOptions = new TelechargementOptions(TypeTelechargement::CARTES, this);
        telechargementOptions->setWindowModality(Qt::ApplicationModal);
        telechargementOptions->show();

    } else {
        settings.setValue("fichier/nomMap", (index == 0) ? "" : Configuration::instance()->listeFicMap().at(index - 1));
    }
}

void Options::on_listeSons_currentIndexChanged(int index)
{
    if (index == _ui->listeSons->count() - 1) {

        TelechargementOptions * const telechargementOptions = new TelechargementOptions(TypeTelechargement::NOTIFICATIONS, this);
        telechargementOptions->setWindowModality(Qt::ApplicationModal);
        telechargementOptions->show();
    }
}

void Options::on_updown_valueChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QTime heur(0, 0);
    heur = heur.addSecs(abs(arg1));
    const QString sgnh = (arg1 >= 0) ? " + " : " - ";
    _ui->tuc->setText(tr("UTC", "Universal Time Coordinated") + sgnh + heur.toString("hh:mm"));

    const int offset = static_cast<int> (Date::CalculOffsetUTC(QDateTime::currentDateTime()) * DATE::NB_SEC_PAR_JOUR);
    if (_ui->utcAuto->isChecked() && (offset != arg1)) {
        _ui->utcAuto->setChecked(false);
    }

    /* Retour */
    return;
}

void Options::on_utcAuto_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {
        const int offset = static_cast<int> (Date::CalculOffsetUTC(QDateTime::currentDateTime()) * DATE::NB_SEC_PAR_JOUR);
        _ui->updown->setValue(offset);
    }

    /* Retour */
    return;
}

void Options::on_heureLegale_toggled(bool checked)
{
    _ui->utc->setChecked(!checked);
}

void Options::on_utc_toggled(bool checked)
{
    _ui->heureLegale->setChecked(!checked);
}
