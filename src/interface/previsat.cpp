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
 * >    16 octobre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include "ui_options.h"
#include "ui_previsat.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include <QDesktopServices>
#include <QMessageBox>
#include "listwidgetitem.h"
#include "previsat.h"
#include "ui_onglets.h"
#include "apropos/apropos.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "informations/informations.h"
#include "onglets/onglets.h"
#include "onglets/previsions/calculsevenementsorbitaux.h"
#include "onglets/previsions/calculsprevisions.h"
#include "onglets/previsions/calculstransits.h"
#include "onglets/telescope/suivitelescope.h"
#include "options/options.h"
#include "outils/outils.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/systeme/telechargement.h"


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

        _messageStatut = nullptr;
        _messageStatut2 = nullptr;
        _messageStatut3 = nullptr;
        _modeFonctionnement = nullptr;
        _stsDate = nullptr;
        _stsHeure = nullptr;
        _timerStatut = nullptr;

        _dateCourante = nullptr;

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
    EFFACE_OBJET(_informations);
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
/*
 * Mise a jour des elements orbitaux lors du demarrage
 */
void PreviSat::MajGP()
{
    /* Declarations des variables locales */

    /* Initialisations */
    InitDate();

    /* Corps de la methode */
    // Mise a jour des elements orbitaux si necessaire
    if (Configuration::instance()->listeCategoriesElementsOrbitaux().size() > 0) {

        if (settings.value("temps/ageMaxElementsOrbitaux", true).toBool()) {

            const double lastUpdate = settings.value("temps/lastUpdate", 0.).toDouble();
            const int ageMax = settings.value("temps/ageMax", 15).toInt();
            const QString noradDefaut = Configuration::instance()->noradDefaut();

            if ((fabs(_dateCourante->jourJulienUTC() - lastUpdate) > ageMax) ||
                    ((_dateCourante->jourJulienUTC() - Configuration::instance()->mapElementsOrbitaux()[noradDefaut].epoque.jourJulienUTC()) > ageMax)) {
                MajWebGP();
                settings.setValue("temps/lastUpdate", _dateCourante->jourJulienUTC());
            }
        } else {
            emit AfficherMessageStatut(tr("Mise à jour automatique des éléments orbitaux"), 10);
            MajWebGP();
            settings.setValue("temps/lastUpdate", _dateCourante->jourJulienUTC());
        }
    } else {
        VerifAgeGP();
    }

    /* Retour */
    return;
}

/*
 * Demarrage de l'application apres le chargement de la configuration
 */
void PreviSat::DemarrageApplication()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

/*
 * Chargement du fichier d'elements orbitaux par defaut
 */
void PreviSat::ChargementGP()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QString nomfic = Configuration::instance()->dirElem() + QDir::separator() + Configuration::instance()->nomfic();
        QFileInfo ff(nomfic);

        if (!Configuration::instance()->listeFichiersElem().isEmpty() && !Configuration::instance()->listeFichiersElem().contains(ff.fileName())) {
            nomfic = Configuration::instance()->listeFichiersElem().first();
            ff = QFileInfo(nomfic);
        }

        if (ff.exists() && (ff.size() != 0)) {

            if (ff.suffix() == "xml") {

                // Cas d'un fichier au format GP
                Configuration::instance()->setMapElementsOrbitaux(GPFormat::LectureFichier(nomfic, Configuration::instance()->donneesSatellites(),
                                                                                           Configuration::instance()->lgRec()));
                qInfo() << "Lecture du fichier GP" << ff.fileName() << "OK";

            } else {

                // Cas d'un fichier au format TLE
                qInfo() << "Vérification du fichier TLE" << ff.fileName();
                AfficherMessageStatut(tr("Vérification du fichier TLE %1 ...").arg(ff.fileName()));

                if (TLE::VerifieFichier(nomfic, true) > 0) {
                    qInfo() << QString("Fichier TLE %1 OK").arg(ff.fileName());
                    AfficherMessageStatut(tr("Fichier TLE %1 OK").arg(ff.fileName()));
                } else {
                    qWarning() << QString("Fichier TLE %1 KO").arg(ff.fileName());
                }

                // Lecture du fichier TLE en entier
                Configuration::instance()->setMapElementsOrbitaux(TLE::LectureFichier(nomfic, Configuration::instance()->donneesSatellites(),
                                                                                      Configuration::instance()->lgRec()));
                qInfo() << "Lecture du fichier TLE" << ff.fileName() << "OK";
            }

            // Mise a jour de la liste de satellites
            QStringList listeSatellites = Configuration::instance()->mapSatellitesFichierElem()[ff.fileName()];

            QStringListIterator it(Configuration::instance()->mapSatellitesFichierElem()[ff.fileName()]);
            while (it.hasNext()) {

                const QString norad = it.next();
                if (!Configuration::instance()->mapElementsOrbitaux().keys().contains(norad)) {
                    listeSatellites.removeOne(norad);
                }
            }

            Configuration::instance()->mapSatellitesFichierElem()[ff.fileName()] = listeSatellites;

            if (!listeSatellites.isEmpty()) {
                GestionnaireXml::EcritureConfiguration();
            }

            // Affichage de la liste de satellites
            AfficherListeSatellites(ff.fileName());
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
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

    if (_informations != nullptr) {
        _informations->changeEvent(&evt);
    }

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

/*
 * Connexions entre les differents elements de l'interface
 */
void PreviSat::ConnexionsSignauxSlots()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Connexions avec la barre d'onglets
    connect(this, SIGNAL(AffichageListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)),
            _onglets->previsions(), SLOT(AfficherListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)));

    connect(this, SIGNAL(AffichageListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)),
            _onglets->transits(), SLOT(AfficherListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)));

    connect(this, SIGNAL(AffichageListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)),
            _onglets->evenements(), SLOT(AfficherListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)));

    connect(this, SIGNAL(InitAffichageListeSatellites()), _onglets->previsions(), SLOT(InitAffichageListeSatellites()));
    connect(this, SIGNAL(InitAffichageListeSatellites()), _onglets->transits(), SLOT(InitAffichageListeSatellites()));
    connect(this, SIGNAL(InitAffichageListeSatellites()), _onglets->evenements(), SLOT(InitAffichageListeSatellites()));

    connect(this, SIGNAL(TriAffichageListeSatellites()), _onglets->previsions(), SLOT(TriAffichageListeSatellites()));
    connect(this, SIGNAL(TriAffichageListeSatellites()), _onglets->transits(), SLOT(TriAffichageListeSatellites()));
    connect(this, SIGNAL(TriAffichageListeSatellites()), _onglets->evenements(), SLOT(TriAffichageListeSatellites()));

#if defined (Q_OS_WIN)
    connect(this, SIGNAL(AffichageListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)),
            _onglets->suiviTelescope(), SLOT(AfficherListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool)));

    connect(this, SIGNAL(InitAffichageListeSatellites()), _onglets->suiviTelescope(), SLOT(InitAffichageListeSatellites()));

    connect(this, SIGNAL(TriAffichageListeSatellites()), _onglets->suiviTelescope(), SLOT(TriAffichageListeSatellites()));
#endif

    /* Retour */
    return;
}

/*
 * Creation des menus
 */
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
 * Gestion de la police
 */
void PreviSat::GestionPolice()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFont police;
#if defined (Q_OS_WIN)
    police.setFamily("Segoe UI");
#elif defined (Q_OS_LINUX)
    police.setFamily("Sans Serif");
#elif defined (Q_OS_MAC)
    police.setFamily("Marion");
#endif

    setFont(police);
    Configuration::instance()->setPolice(police);

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
        _options = new Options();
        _onglets = new Onglets(_options, _ui->frameOnglets);
        _outils = new Outils();

        CreationMenus();
        CreationRaccourcis();

        // Connexions signaux-slots
        ConnexionsSignauxSlots();

        // Gestion de la police
        GestionPolice();

        //on_pasReel_currentIndexChanged(0);
        _ui->pasReel->setCurrentIndex(settings.value("temps/pasreel", 1).toInt());

        //on_pasManuel_currentIndexChanged(0);
        _ui->pasManuel->setCurrentIndex(settings.value("temps/pasmanuel", 1).toInt());
        _ui->valManuel->setCurrentIndex(settings.value("temps/valmanuel", 0).toInt());
        on_tempsReel_toggled(true);
        _ui->frameVideo->setVisible(_ui->issLive->isChecked());
        //_radar->setVisible(!(_ui->issLive->isChecked());

        // Barre de statut
        InitBarreStatut();

        // Liste des fichiers d'elements orbitaux
        InitFicGP();

        qInfo() << "Fin   Initialisation" << metaObject()->className();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

/*
 * Initialisation de la barre de statut
 */
void PreviSat::InitBarreStatut()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _messageStatut = new QLabel("", this);
    _messageStatut->setFrameStyle(QFrame::NoFrame);
    _messageStatut->setIndent(3);
    _messageStatut->setMinimumSize(405, 0);
    _messageStatut->setToolTip(tr("Messages"));

    _messageStatut2 = new QLabel("", this);
    _messageStatut2->setFrameStyle(QFrame::NoFrame);
    _messageStatut2->setFixedWidth(140);
    _messageStatut2->setAlignment(Qt::AlignCenter);
    _messageStatut2->setVisible(false);

    _messageStatut3 = new QLabel("", this);
    _messageStatut3->setFrameStyle(QFrame::NoFrame);
    _messageStatut3->setFixedWidth(140);
    _messageStatut3->setAlignment(Qt::AlignCenter);
    _messageStatut3->setVisible(false);

    _modeFonctionnement = new QLabel("", this);
    _modeFonctionnement->setFrameStyle(QFrame::NoFrame);
    _modeFonctionnement->setFixedWidth(103);
    _modeFonctionnement->setAlignment(Qt::AlignCenter);
    _modeFonctionnement->setToolTip(tr("Mode de fonctionnement"));

    _stsDate = new QLabel("", this);
    _stsDate->setFrameStyle(QFrame::NoFrame);
    _stsDate->setFixedWidth(90);
    _stsDate->setAlignment(Qt::AlignCenter);
    _stsDate->setToolTip(tr("Date"));

    _stsHeure = new QLabel("", this);
    _stsHeure->setFrameStyle(QFrame::NoFrame);
    _stsHeure->setFixedWidth(77);
    _stsHeure->setAlignment(Qt::AlignCenter);
    _stsHeure->setToolTip(tr("Heure"));

    _ui->barreStatut->addPermanentWidget(_messageStatut, 1);
    _ui->barreStatut->addPermanentWidget(_messageStatut2);
    _ui->barreStatut->addPermanentWidget(_messageStatut3);
    _ui->barreStatut->addPermanentWidget(_modeFonctionnement);
    _ui->barreStatut->addPermanentWidget(_stsDate);
    _ui->barreStatut->addPermanentWidget(_stsHeure);

    /* Retour */
    return;
}

/*
 * Initialisation de la date
 */
void PreviSat::InitDate()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Determination automatique de l'ecart heure locale - UTC
    const double ecart = Date::CalculOffsetUTC(QDateTime::currentDateTime());
    double offsetUTC = (_options->ui()->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    _options->ui()->updown->setValue(sgn(offsetUTC) * (static_cast<int>(fabs(offsetUTC) * NB_SEC_PAR_JOUR + EPS_DATES)));
    offsetUTC = (_options->ui()->heureLegale->isChecked()) ? _options->ui()->updown->value() * NB_JOUR_PAR_SEC : 0.;

    // Date et heure locales
    _dateCourante = new Date(offsetUTC);

    /* Retour */
    return;
}

/*
 * Liste des fichiers d'elements orbitaux
 */
void PreviSat::InitFicGP()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QString nomfic;
        bool ajout = false;
        bool defaut = false;
        int idx = 0;
        QStringList listeElem = Configuration::instance()->listeFichiersElem();

        const bool etat = _ui->listeFichiersElem->blockSignals(true);
        _ui->listeFichiersElem->clear();

        QStringListIterator it(Configuration::instance()->listeFichiersElem());
        while (it.hasNext()) {

            const QString nom = it.next();
            const QString fic = QDir::toNativeSeparators(Configuration::instance()->dirElem() + QDir::separator() + nom);

            const QFileInfo ff(fic);
            if (ff.suffix() == "xml") {

                // Cas des fichiers GP
                const QMap<QString, ElementsOrbitaux> mapElem = GPFormat::LectureFichier(fic, Configuration::instance()->donneesSatellites(),
                                                                                         Configuration::instance()->lgRec());

                ajout = (mapElem.size() != 0);
                nomfic = ff.baseName();

            } else {

                // Cas des fichiers TLE
                ajout = (TLE::VerifieFichier(fic) > 0);
                nomfic = nom;
            }

            if (ajout) {

                // Ajout du fichier dans la liste
                _ui->listeFichiersElem->addItem(nomfic);

                if (nom == Configuration::instance()->nomfic()) {
                    const int index = _ui->listeFichiersElem->count() - 1;
                    _ui->listeFichiersElem->setCurrentIndex(index);
                    _ui->listeFichiersElem->setItemData(index, QColor(Qt::gray), Qt::BackgroundRole);
                }
            } else {

                // Suppression dans la liste des fichiers qui ne sont pas des elements orbitaux
                if (nom == Configuration::instance()->nomfic()) {
                    defaut = true;
                }
                listeElem.removeAt(idx);
            }

            idx++;
        }

        if (listeElem.count() == 0) {
            _ui->listeFichiersElem->addItem("");
        } else {
            if (defaut) {
                Configuration::instance()->nomfic() = Configuration::instance()->dirElem() + QDir::separator() + listeElem.first();
            }
        }
        _ui->listeFichiersElem->blockSignals(etat);

        // Mise a jour de la liste de fichiers d'elements orbitaux
        Configuration::instance()->setListeFicElem(listeElem);

    } catch (PreviSatException &e) {
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
 * Mise a jour du fichier GP courant
 */
void PreviSat::MajFichierGP()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "Mise a jour du fichier GP courant";

        const QFileInfo ff(Configuration::instance()->nomfic());

        Telechargement tel(Configuration::instance()->dirElem());
        _messageStatut->setText(tr("Mise à jour du fichier GP %1 en cours...").arg(ff.fileName()));

        const QString ficMaj = (ff.fileName().contains("spctrk")) ?
                    QString("%1elem/%2").arg(DOMAIN_NAME).arg(ff.fileName()) : Configuration::instance()->adresseCelestrakNorad().arg(ff.baseName());

        tel.TelechargementFichier(QUrl(ficMaj));
        AfficherMessageStatut(tr("Téléchargement terminé"), 10);
        settings.setValue("temps/lastUpdate", _dateCourante->jourJulienUTC());

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Mise a jour automatique des elements orbitaux
 */
void PreviSat::MajWebGP()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "Mise a jour des fichiers GP";

        Telechargement tel(Configuration::instance()->dirElem());

        QListIterator it(Configuration::instance()->listeCategoriesMajElementsOrbitaux());
        while (it.hasNext()) {

            const CategorieElementsOrbitaux categorie = it.next();
            QString adresse = categorie.site;

            if (adresse.contains("celestrak")) {
                adresse = Configuration::instance()->adresseCelestrakNorad();
            }

            if (adresse.contains("previsat")) {
                adresse = QString(DOMAIN_NAME) + "elem/%1";
            }

            if (!adresse.endsWith("/")) {
                adresse.append("/");
            }

            foreach (const QString fic, categorie.fichiers) {

                const QString fichier = (adresse.contains("celestrak")) ? QFileInfo(fic).baseName() : fic;
                const QString ficMaj = adresse.arg(fichier);
                tel.TelechargementFichier(QUrl(ficMaj));
            }
        }
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Verification de l'age des elements orbitaux d'un satellite
 */
void PreviSat::VerifAgeGP()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->mapElementsOrbitaux().count() > 0) {

        const int ageMax = settings.value("temps/ageMax", 15).toInt();
        const QString noradDefaut = Configuration::instance()->noradDefaut();

        if ((fabs(_dateCourante->jourJulienUTC() - Configuration::instance()->mapElementsOrbitaux()[noradDefaut].epoque.jourJulienUTC()) > ageMax) &&
                _ui->tempsReel->isChecked()) {

            const QString msg = tr("Les éléments orbitaux sont plus vieux que %1 jour(s). Souhaitez-vous les mettre à jour?");

            QMessageBox msgbox(tr("Information"), msg.arg(ageMax), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                               QMessageBox::No, QMessageBox::NoButton, this);
            msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
            msgbox.setButtonText(QMessageBox::No, tr("Non"));
            msgbox.exec();

            const int res = msgbox.result();
            if (res == QMessageBox::Yes) {
                MajFichierGP();
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Afficher les noms des satellites dans les listes
 */
void PreviSat::AfficherListeSatellites(const QString &nomfic, const bool majListesOnglets)
{
    /* Declarations des variables locales */
    QString nomsatComplet;
    ListWidgetItem *elem;

    /* Initialisations */
    _ui->listeSatellites->clear();
    _ui->listeSatellites->scrollToTop();
    emit InitAffichageListeSatellites();

    const QStringList &listeSatellites = Configuration::instance()->mapSatellitesFichierElem()[nomfic];
    const QString noradDefaut = Configuration::instance()->noradDefaut();

    /* Corps de la methode */
    QMapIterator it(Configuration::instance()->mapElementsOrbitaux());
    while (it.hasNext()) {
        it.next();

        const QString nomsat = it.value().nom.trimmed();
        const QString norad = it.key();
        const bool check = listeSatellites.contains(norad);
        const QString tooltip = tr("<font color='blue'><b>%1</b></font><br />NORAD : <b>%2</b><br />COSPAR : <b>%3</b>")
                .arg(nomsat).arg(norad).arg(it.value().donnees.cospar());

        // Affichage du numero NORAD
        switch (_options->ui()->affNoradListes->checkState()) {
        default:
        case Qt::Unchecked:
            nomsatComplet = nomsat;
            break;

        case Qt::Checked:
            nomsatComplet = QString("%1     (%2)").arg(nomsat).arg(norad);
            break;

        case Qt::PartiallyChecked:
            nomsatComplet = (nomsat.contains("R/B") || nomsat.contains(" DEB")) ? QString("%1     (%2)").arg(nomsat).arg(norad) : nomsat;
            break;
        }

        // Ajout dans la liste principale
        elem = new ListWidgetItem(nomsatComplet, _ui->listeSatellites);
        elem->setData(Qt::UserRole, norad);
        elem->setToolTip(tooltip);
        elem->setFlags(Qt::ItemIsEnabled);
        elem->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
        if (norad == noradDefaut) {
            _ui->listeSatellites->setCurrentItem(elem);
        }

        if (majListesOnglets) {
            // Ajout dans les listes de la barre d'onglets
            emit AffichageListeSatellites(nomsatComplet, norad, noradDefaut, tooltip, check);
        }
    }

    _ui->listeSatellites->sortItems();
    _ui->listeSatellites->scrollToItem(_ui->listeSatellites->currentItem(), QAbstractItemView::PositionAtTop);

    emit TriAffichageListeSatellites();




    /* Retour */
    return;
}

/*
 * Affichage d'un message dans la zone de statut
 */
void PreviSat::AfficherMessageStatut(const QString &message, const int secondes)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _messageStatut->setText(message);
    _messageStatut->repaint();

    if ((_timerStatut != nullptr) && (_timerStatut->interval() > 0)) {

        if (_timerStatut->isActive()) {
            _timerStatut->stop();
        }

        _timerStatut->deleteLater();
        _timerStatut = nullptr;
    }

    if (secondes > 0) {

        _timerStatut = new QTimer(this);
        _timerStatut->setInterval(secondes * 1000);
        connect(_timerStatut, SIGNAL(timeout()), this, SLOT(EffacerMessageStatut()));
        _timerStatut->start();
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

void PreviSat::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeSatellites->count(); i++) {
        const QString elem = _ui->listeSatellites->item(i)->text();
        _ui->listeSatellites->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void PreviSat::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    _ui->listeSatellites->sortItems();
    _ui->listeSatellites->scrollToItem(_ui->listeSatellites->currentItem(), QAbstractItemView::PositionAtTop);
}
