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
 * >    23 decembre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QSoundEffect>
#include <QStandardPaths>
#include <QTimer>
#include <QVersionNumber>
#include "ui_options.h"
#include "ui_previsat.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wredundant-decls"
#pragma GCC diagnostic warning "-Wconversion"
#include "listwidgetitem.h"
#include "previsat.h"
#include "ui_carte.h"
#include "ui_coordiss.h"
#include "ui_general.h"
#include "ui_informations.h"
#include "ui_informationssatellite.h"
#include "ui_onglets.h"
#include "ui_osculateurs.h"
#include "apropos/apropos.h"
#include "carte/carte.h"
#include "carte/coordiss.h"
#include "ciel/ciel.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "informations/informations.h"
#include "logging/logging.h"
#include "onglets/antenne/antenne.h"
#include "onglets/donnees/informationsiss.h"
#include "onglets/donnees/informationssatellite.h"
#include "onglets/donnees/recherchesatellite.h"
#include "onglets/general/general.h"
#include "onglets/onglets.h"
#include "onglets/osculateurs/osculateurs.h"
#include "onglets/previsions/calculsevenementsorbitaux.h"
#include "onglets/previsions/calculsflashs.h"
#include "onglets/previsions/calculsprevisions.h"
#include "onglets/previsions/calculsstarlink.h"
#include "onglets/previsions/calculstransits.h"
#include "onglets/telescope/suivitelescope.h"
#include "options/options.h"
#include "outils/outils.h"
#include "radar/radar.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/systeme/telechargement.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);

// Couleurs GMT
static const std::array<QColor, 3> _coulGmt = { Qt::red, Qt::white, Qt::cyan };


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
PreviSat::PreviSat(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::PreviSat)
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
PreviSat::~PreviSat()
{
    EFFACE_OBJET(_informations);
    EFFACE_OBJET(_coordISS);
    EFFACE_OBJET(_gmt);
    EFFACE_OBJET(_carte);
    EFFACE_OBJET(_ciel);
    EFFACE_OBJET(_onglets);
    EFFACE_OBJET(_options);
    EFFACE_OBJET(_outils);
    EFFACE_OBJET(_radar);

    EFFACE_OBJET(_previsions);
    EFFACE_OBJET(_flashs);
    EFFACE_OBJET(_transits);
    EFFACE_OBJET(_evenements);
    EFFACE_OBJET(_starlink);
    EFFACE_OBJET(_informationsSatellite);
    EFFACE_OBJET(_recherche);
    EFFACE_OBJET(_station);
    EFFACE_OBJET(_captureEcran);
    EFFACE_OBJET(_etapePrec);
    EFFACE_OBJET(_etapeSuiv);

    EFFACE_OBJET(_messageStatut);
    EFFACE_OBJET(_messageStatut2);
    EFFACE_OBJET(_messageStatut3);
    EFFACE_OBJET(_modeFonctionnement);
    EFFACE_OBJET(_stsDate);
    EFFACE_OBJET(_stsHeure);

    EFFACE_OBJET(_dateCourante);
    EFFACE_OBJET(_chronometre);
    EFFACE_OBJET(_chronometreMs);
    EFFACE_OBJET(_timerStatut);

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
 * Chargement de la fenetre principale
 */
void PreviSat::ChargementConfiguration()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Fonction" << __FUNCTION__;

    // Chargement des elements des differentes fenetres
    _informations = new Informations(this);
    _onglets = new Onglets();
    _ui->layoutOnglets->addWidget(_onglets);

    _outils = new Outils(this);
    _radar = new Radar(_ui->frameRadar);

    _carte = new Carte(_ui->frameCarte);
    _ui->layoutCarte->addWidget(_carte);

    _ciel = new Ciel(_ui->frameCarte);
    _ui->layoutCarte->addWidget(_ciel, 0, Qt::AlignVCenter);
    _ciel->resize(_ui->frameCarte->height(), _ui->frameCarte->height());
    _ciel->setVisible(false);

    // Coordonnees de l'ISS
    _coordISS = new CoordISS(_carte->ui()->carte);
    _coordISS->setVisible(false);
    _gmt = new QLabel("", _carte->ui()->carte);
    _gmt->setVisible(false);

    // Creation des menus
    CreationMenus();

    // Creation des raccourcis clavier
    CreationRaccourcis();

    // Connexions signaux-slots
    ConnexionsSignauxSlots();

    // Gestion de la police
    GestionPolice();

    _isCarteMonde = true;
    Configuration::instance()->issLive() = settings.value("affichage/issLive").toBool();

    _ui->pasReel->setCurrentIndex(settings.value("temps/pasreel", 1).toInt());
    _ui->pasManuel->setCurrentIndex(settings.value("temps/pasmanuel", 1).toInt());
    _ui->valManuel->setCurrentIndex(settings.value("temps/valmanuel", 0).toInt());
    on_tempsReel_toggled(true);
    _ui->frameVideo->setVisible(_ui->issLive->isChecked());
    _radar->setVisible(!_ui->issLive->isChecked());

    // Barre de statut
    InitBarreStatut();

    // Liste des fichiers d'elements orbitaux
    InitFicGP();

    QCoreApplication::instance()->installEventFilter(this);

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

/*
 * Mise a jour des elements orbitaux lors du demarrage
 */
void PreviSat::MajGP()
{
    /* Declarations des variables locales */

    /* Initialisations */
    qInfo() << "Début Fonction" << __FUNCTION__;

    InitDate();

    /* Corps de la methode */
    // Mise a jour des elements orbitaux si necessaire
    if (Configuration::instance()->mapCategoriesMajElementsOrbitaux().isEmpty()) {

        VerifAgeGP();

    } else {

        if (settings.value("temps/ageMaxElementsOrbitaux", true).toBool()) {

            const double lastUpdate = settings.value("temps/lastUpdate", 0.).toDouble();
            const int ageMax = settings.value("temps/ageMax", 15).toInt();
            const QString noradDefaut = Configuration::instance()->noradDefaut();

            // Mise ajour des elements orbitaux anciens
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
    }

    // Telechargement des groupes Starlink
    TelechargementGroupesStarlink();

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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

        qInfo() << "Début Fonction" << __FUNCTION__;

        // Nom du fichier d'elements orbitaux
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

                    // Lecture du fichier TLE en entier
                    Configuration::instance()->setMapElementsOrbitaux(TLE::LectureFichier(nomfic, Configuration::instance()->donneesSatellites(),
                                                                                          Configuration::instance()->lgRec()));
                    qInfo() << "Lecture du fichier TLE" << ff.fileName() << "OK";

                } else {
                    qWarning() << QString("Fichier TLE %1 KO").arg(ff.fileName());
                }
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
                Configuration::instance()->noradDefaut() =  listeSatellites.first();
                GestionnaireXml::EcritureConfiguration();
            }

            // Affichage de la liste de satellites
            AfficherListeSatellites(ff.fileName());
        }

    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
    qInfo() << "Début Fonction" << __FUNCTION__;

    // Construction de la liste de satellites
    const QString noradDefaut = Configuration::instance()->noradDefaut();
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
    const QFileInfo ff(Configuration::instance()->dirElem() + QDir::separator() + Configuration::instance()->nomfic());

    if (!Configuration::instance()->mapElementsOrbitaux().isEmpty()) {

        satellites.clear();
        QStringListIterator it(Configuration::instance()->mapSatellitesFichierElem()[ff.fileName()]);
        while (it.hasNext()) {

            const QString norad = it.next();
            const ElementsOrbitaux elem = Configuration::instance()->mapElementsOrbitaux()[norad];

            if (norad == noradDefaut) {
                satellites.insert(0, Satellite(elem));
            } else {
                satellites.append(Satellite(elem));
            }
        }
    }

    // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
    EnchainementCalculs();

    // Affichage des donnees numeriques dans la barre d'onglets
    _onglets->show(*_dateCourante);
#if defined (Q_OS_WIN)
    _onglets->suiviTelescope()->CalculAosSatSuivi(*_dateCourante);
#endif

    _ui->issLive->setChecked(settings.value("affichage/issLive", false).toBool());

    // Affichage des elements sur les cartes et le radar
    AffichageCartesRadar();

    // Lancement du chronometre
    if (_chronometre == nullptr) {
        _chronometre = new QTimer(this);
        _chronometre->setInterval(_ui->pasReel->currentText().toInt() * 1000);
        _chronometre->setTimerType(Qt::PreciseTimer);
        connect(_chronometre, SIGNAL(timeout()), this, SLOT(GestionTempsReel()));

        // Mode sombre
        _ui->actionMode_sombre->setChecked(settings.value("affichage/modeSombre", false).toBool());
        on_actionMode_sombre_triggered();
    }

    // Lancement du chronometreMs
    if (_chronometreMs == nullptr) {
        _chronometreMs = new QTimer(this);
        _chronometreMs->setInterval(200);
        _chronometreMs->setTimerType(Qt::PreciseTimer);
        connect(_chronometreMs, SIGNAL(timeout()), this, SLOT(TempsReel()));
        _chronometreMs->start();
    }

    // Affichage de la fenetre d'informations
    const QUrl urlLastNews(QString("%1informations/").arg(DOMAIN_NAME) + "last_news_" + Configuration::instance()->locale() + ".html");

    if (!_chronometre->isActive() && settings.value("affichage/informationsDemarrage", true).toBool() && Telechargement::UrlExiste(urlLastNews)) {
        on_actionInformations_triggered();
        const QRect tailleEcran = QApplication::primaryScreen()->availableGeometry();
        _informations->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, _informations->size(), tailleEcran));
    }

    _chronometre->start();

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
 * Affichage des elements graphiques (carte du monde, carte du ciel, radar)
 */
void PreviSat::AffichageCartesRadar()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_isCarteMonde) {

        // Affichage de la carte du monde
        _carte->show();

        if (Configuration::instance()->issLive()) {
            AfficherCoordIssGmt();
        }

    } else {

        // Affichage de la carte du ciel
        _ciel->show(Configuration::instance()->observateur(),
                    Configuration::instance()->soleil(),
                    Configuration::instance()->lune(),
                    Configuration::instance()->lignesCst(),
                    Configuration::instance()->constellations(),
                    Configuration::instance()->etoiles(),
                    Configuration::instance()->planetes(),
                    Configuration::instance()->listeSatellites(),
                    Configuration::instance()->isCarteMaximisee());
    }

    // Affichage du radar
    const Qt::CheckState affradar = static_cast<Qt::CheckState> (settings.value("affichage/affradar", Qt::Checked).toUInt());
    const bool radarVisible = ((affradar == Qt::Checked) ||
                               ((affradar == Qt::PartiallyChecked) && Configuration::instance()->listeSatellites().first().isVisible()));
    if (radarVisible) {
        _radar->show();
    }

    _radar->setVisible(!_ui->issLive->isChecked() && radarVisible);

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
    qInfo() << "Début Fonction" << __FUNCTION__;

    QAction const * effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &PreviSat::on_filtreSatellites_returnPressed);
    }

    // Connexions avec la barre d'onglets
    connect(this, &PreviSat::AffichageListeSatellites, _onglets->previsions(), &CalculsPrevisions::AfficherListeSatellites);
    connect(this, &PreviSat::AffichageListeSatellites, _onglets->transits(), &CalculsTransits::AfficherListeSatellites);
    connect(this, &PreviSat::AffichageListeSatellites, _onglets->evenements(), &CalculsEvenementsOrbitaux::AfficherListeSatellites);

    connect(this, &PreviSat::InitAffichageListeSatellites, _onglets->previsions(), &CalculsPrevisions::InitAffichageListeSatellites);
    connect(this, &PreviSat::InitAffichageListeSatellites, _onglets->transits(), &CalculsTransits::InitAffichageListeSatellites);
    connect(this, &PreviSat::InitAffichageListeSatellites, _onglets->evenements(), &CalculsEvenementsOrbitaux::InitAffichageListeSatellites);

    connect(this, &PreviSat::TriAffichageListeSatellites, _onglets->previsions(), &CalculsPrevisions::TriAffichageListeSatellites);
    connect(this, &PreviSat::TriAffichageListeSatellites, _onglets->transits(), &CalculsTransits::TriAffichageListeSatellites);
    connect(this, &PreviSat::TriAffichageListeSatellites, _onglets->evenements(), &CalculsEvenementsOrbitaux::TriAffichageListeSatellites);

#if defined (Q_OS_WIN)
    connect(this, &PreviSat::AffichageListeSatellites, _onglets->suiviTelescope(), &SuiviTelescope::AfficherListeSatellites);
    connect(this, &PreviSat::InitAffichageListeSatellites, _onglets->suiviTelescope(), &SuiviTelescope::InitAffichageListeSatellites);
    connect(this, &PreviSat::TriAffichageListeSatellites, _onglets->suiviTelescope(), &SuiviTelescope::TriAffichageListeSatellites);
    connect(_onglets->suiviTelescope(), &SuiviTelescope::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
#endif

    connect(_onglets->previsions(), &CalculsPrevisions::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_onglets->flashs(), &CalculsFlashs::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_onglets->transits(), &CalculsTransits::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_onglets->starlink(), &CalculsStarlink::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_onglets->starlink(), &CalculsStarlink::MajElementsOrbitaux, this, &PreviSat::TelechargementGroupesStarlink);
    connect(_onglets->evenements(), &CalculsEvenementsOrbitaux::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_onglets->general(), &General::RecalculerPositions, _onglets, &Onglets::AffichageLieuObs);
    connect(_onglets->general(), &General::RecalculerPositions, this, &PreviSat::ReinitCalculEvenementsSoleilLune);
    connect(_onglets->general(), &General::RecalculerPositions, this, &PreviSat::GestionTempsReel);
    connect(_onglets->general(), &General::ModeManuel, this, &PreviSat::on_modeManuel_toggled);
    connect(_onglets->general(), &General::ModificationDate, this, &PreviSat::ModificationDate);
    connect(_onglets->osculateurs(), &Osculateurs::ModeManuel, this, &PreviSat::on_modeManuel_toggled);
    connect(_onglets->osculateurs(), &Osculateurs::ModificationDate, this, &PreviSat::ModificationDate);

    connect(_onglets->previsions(), &CalculsPrevisions::MajFichierGP, this, &PreviSat::MajFichierGP);
    connect(_onglets->evenements(), &CalculsEvenementsOrbitaux::MajFichierGP, this, &PreviSat::MajFichierGP);
    connect(_onglets->flashs(), &CalculsFlashs::ChargementGP, this, &PreviSat::ChargementGP);
    connect(_onglets->flashs(), &CalculsFlashs::DemarrageApplication, this, &PreviSat::DemarrageApplication);
    connect(_onglets->transits(), &CalculsTransits::MajFichierGP, this, &PreviSat::MajFichierGP);

    connect(this, &PreviSat::SauveOngletGeneral, _onglets->general(), &General::SauveOngletGeneral);
    connect(this, &PreviSat::SauveOngletElementsOsculateurs, _onglets->osculateurs(), &Osculateurs::SauveOngletElementsOsculateurs);
    connect(this, &PreviSat::SauveOngletInformations, _onglets->informationsSatellite(), &InformationsSatellite::SauveOngletInformations);
    connect(this, &PreviSat::SauveOngletRecherche, _onglets->rechercheSatellite(), &RechercheSatellite::SauveOngletRecherche);

    // Connexions avec l'onglet Informations satellite
    connect(_onglets->informationsSatellite(), &InformationsSatellite::AffichageSiteLancement, _carte, &Carte::AffichageSiteLancement);
    connect(_onglets->informationsSatellite(), &InformationsSatellite::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);

    // Connexions avec l'onglet Informations ISS
    connect(_onglets->informationsISS(), &InformationsISS::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);

    // Connexions avec l'onglet Recherche donnees
    connect(_onglets->rechercheSatellite(), &RechercheSatellite::AffichageSiteLancement, _carte, &Carte::AffichageSiteLancement);
    connect(_onglets->rechercheSatellite(), &RechercheSatellite::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);

    // Connexions avec l'onglet Antenne
    connect(this, &PreviSat::DeconnecterUdp, _onglets->antenne(), &Antenne::DeconnecterUdp);

    // Connexions avec l'instance de Carte
    connect(_carte, &Carte::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_carte, &Carte::AfficherMessageStatut2, this, &PreviSat::AfficherMessageStatut2);
    connect(_carte, &Carte::AfficherMessageStatut3, this, &PreviSat::AfficherMessageStatut3);
    connect(_carte, &Carte::EffacerMessageStatut, this, &PreviSat::EffacerMessageStatut);
    connect(_carte, &Carte::ReinitFlags, _onglets, &Onglets::ReinitFlags);
    connect(_carte, &Carte::RecalculerPositions, this, &PreviSat::GestionTempsReel);

    // Connexions avec l'instance de Ciel
    connect(_ciel, &Ciel::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_ciel, &Ciel::AfficherMessageStatut2, this, &PreviSat::AfficherMessageStatut2);
    connect(_ciel, &Ciel::AfficherMessageStatut3, this, &PreviSat::AfficherMessageStatut3);
    connect(_ciel, &Ciel::EffacerMessageStatut, this, &PreviSat::EffacerMessageStatut);
    connect(_ciel, &Ciel::ReinitFlags, _onglets, &Onglets::ReinitFlags);
    connect(_ciel, &Ciel::RecalculerPositions, this, &PreviSat::GestionTempsReel);

    // Connexions avec l'instance de Radar
    connect(_radar, &Radar::AfficherMessageStatut, this, &PreviSat::AfficherMessageStatut);
    connect(_radar, &Radar::AfficherMessageStatut2, this, &PreviSat::AfficherMessageStatut2);
    connect(_radar, &Radar::AfficherMessageStatut3, this, &PreviSat::AfficherMessageStatut3);
    connect(_radar, &Radar::RecalculerPositions, this, &PreviSat::GestionTempsReel);

    // Connexions avec la fenetre Options
    connect(_options, &Options::ChangementFuseauHoraire, this, &PreviSat::ChangementFuseauHoraire);
    connect(_options, &Options::RecalculerPositions, this, &PreviSat::ReinitCalculEvenementsSoleilLune);
    connect(_options, &Options::AfficherListeSatellites, this, &PreviSat::AfficherListeSatellites);
    connect(_options, &Options::AfficherListeSatellites, this, &PreviSat::AffichageCartesRadar);
    connect(_options, &Options::RecalculerPositions, this, &PreviSat::GestionTempsReel);
    connect(_options, &Options::RecalculerPositions, this, &PreviSat::on_actionMode_sombre_triggered);
    connect(_options, &Options::RecalculerPositions, _coordISS, &CoordISS::setPolice);
    connect(_options, &Options::RecalculerPositions, _onglets, &Onglets::AffichageLieuObs);
    connect(_options, &Options::ChargementCarteDuMonde, _carte, &Carte::ChargementCarteDuMonde);
    connect(_options, &Options::ChargementTraduction, this, &PreviSat::ChargementTraduction);
    connect(this, &PreviSat::AppliquerPreferences, _options, &Options::AppliquerPreferences);

    // Connexions avec la fenetre Outils
    connect(_outils, &Outils::ChargementGP, this, &PreviSat::ChargementGP);
    connect(_outils, &Outils::InitFicGP, this, &PreviSat::InitFicGP);

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
    qInfo() << "Début Fonction" << __FUNCTION__;

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

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
    qInfo() << "Début Fonction" << __FUNCTION__;

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

    // Raccourci Onglet Starlink
    _starlink = new QAction(this);
    _starlink->setShortcut(Qt::ALT | Qt::Key_K);
    connect(_starlink, &QAction::triggered, this, &PreviSat::RaccourciStarlink);
    this->addAction(_starlink);

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

    // Raccourci Capture ecran
    _captureEcran = new QAction(this);
    _captureEcran->setShortcut(Qt::Key_F8);
    connect(_captureEcran, &QAction::triggered, this, &PreviSat::CaptureEcran);
    this->addAction(_captureEcran);

    // Raccourci Etape precedente
    _etapePrec = new QAction(this);
#if defined (Q_OS_MAC)
    _etapePrec->setShortcut(Qt::Key_F6);
#else
    _etapePrec->setShortcut(Qt::Key_F11);
#endif
    connect(_etapePrec, &QAction::triggered, this, &PreviSat::EtapePrecedente);
    this->addAction(_etapePrec);

    // Raccourci Etape suivante
    _etapeSuiv = new QAction(this);
#if defined (Q_OS_MAC)
    _etapeSuiv->setShortcut(Qt::Key_F7);
#else
    _etapeSuiv->setShortcut(Qt::Key_F12);
#endif
    connect(_etapeSuiv, &QAction::triggered, this, &PreviSat::EtapeSuivante);
    this->addAction(_etapeSuiv);

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

/*
 * Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
 */
void PreviSat::EnchainementCalculs()
{
    /* Declarations des variables locales */
    static int jour;

    /* Initialisations */
    if (_reinitJour) {
        jour = 0;
        _reinitJour = false;
    }

    /* Corps de la methode */
    try {


        /*
         * Calcul de la position de l'observateur
         */
        Observateur &observateur = Configuration::instance()->observateur();
        observateur.CalculPosVit(*_dateCourante);


        /*
         * Position du Soleil
         */
        Soleil &soleil = Configuration::instance()->soleil();
        soleil.CalculPosition(*_dateCourante);

        // Position topocentrique
        soleil.CalculCoordHoriz(observateur);

        // Coordonnees terrestres
        soleil.CalculCoordTerrestres(observateur);

        if (_isCarteMonde && !Configuration::instance()->isCarteMaximisee()) {
            // Coordonnees equatoriales
            soleil.CalculCoordEquat(observateur);
        }


        /*
         * Position de la Lune
         */
        Lune &lune = Configuration::instance()->lune();
        lune.CalculPosition(*_dateCourante);

        // Calcul de la phase lunaire
        lune.CalculPhase(soleil);

        // Coordonnees topocentriques
        lune.CalculCoordHoriz(observateur);

        // Coordonnees terrestres
        lune.CalculCoordTerrestres(observateur);

        // Calcul de la magnitude de la Lune
        lune.CalculMagnitude(soleil);

        if (_isCarteMonde && !Configuration::instance()->isCarteMaximisee()) {
            // Coordonnees equatoriales
            lune.CalculCoordEquat(observateur);
        }

        // Evenements Soleil et Lune
        if (_dateCourante->jour() != jour) {

            // Heures de lever/passage au meridien/coucher/crepuscules
            const DateSysteme syst = (settings.value("affichage/systemeHoraire", true).toBool()) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H;
            soleil.CalculLeverMeridienCoucher(*_dateCourante, observateur, syst);

            // Heures de lever/passage au meridien/coucher
            lune.CalculLeverMeridienCoucher(*_dateCourante, observateur, syst);

            // Calcul des phases de la Lune
            lune.CalculDatesPhases(*_dateCourante);

            jour = _dateCourante->jour();
        }


        if (!_isCarteMonde) {

            /*
             * Calcul de la position des planetes
             */
            if (static_cast<Qt::CheckState> (settings.value("affichage/affplanetes", Qt::Checked).toUInt()) != Qt::Unchecked) {

                std::array<Planete, PLANETE::NB_PLANETES> &planetes = Configuration::instance()->planetes();

                for(unsigned int i=0; i<PLANETE::NB_PLANETES; i++) {

                    planetes[i] = Planete(static_cast<IndicePlanete> (i));
                    planetes[i].CalculPosition(*_dateCourante, soleil);
                    planetes[i].CalculCoordHoriz(observateur);
                }
            }

            /*
             * Calcul de la position du catalogue d'etoiles
             */
            Etoile::CalculPositionEtoiles(observateur, Configuration::instance()->etoiles());
            if (static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt()) == Qt::Checked) {
                Constellation::CalculConstellations(observateur, Configuration::instance()->constellations());
            }

            if (static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt()) != Qt::Unchecked) {
                LigneConstellation::CalculLignesCst(Configuration::instance()->etoiles(), Configuration::instance()->lignesCst());
            }
        }

        QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
        if (satellites.isEmpty()) {

            const QString titre = "%1 %2";
            setWindowTitle(titre.arg(APP_NAME).arg(APP_VER_MAJ));

        } else {

            const QString titre = "%1 %2 - %3";
            setWindowTitle(titre.arg(APP_NAME).arg(APP_VER_MAJ).arg(satellites.first().elementsOrbitaux().nom));

            // Calculs specifiques lors de l'affichage du Wall Command Center
            const bool mcc = _ui->issLive->isChecked();

            // Nombre de traces au sol a afficher
            int nbTraces = settings.value("affichage/nombreTrajectoires", 2).toInt();

            if (satellites.isEmpty()) {
                nbTraces = 1;
            } else if (mcc && satellites.first().elementsOrbitaux().norad == Configuration::instance()->noradStationSpatiale()) {
                nbTraces = 3;
            }

            const bool eclipsesLune = settings.value("affichage/eclipsesLune", true).toBool();
            const bool effetEclipsesMagnitude = settings.value("affichage/effetEclipsesMagnitude", true).toBool();
            const bool extinctionAtmospherique = settings.value("affichage/extinctionAtmospherique", true).toBool();
            const bool refractionAtmospherique = settings.value("affichage/refractionAtmospherique", true).toBool();
            const bool afftraceCiel = settings.value("affichage/afftraceCiel", true).toBool();
            const bool affvisib = settings.value("affichage/affvisib", true).toBool();

            Satellite::CalculPosVitListeSatellites(*_dateCourante,
                                                   observateur,
                                                   soleil,
                                                   lune,
                                                   nbTraces,
                                                   eclipsesLune,
                                                   effetEclipsesMagnitude,
                                                   extinctionAtmospherique,
                                                   refractionAtmospherique,
                                                   afftraceCiel,
                                                   affvisib,
                                                   satellites.first().elementsOrbitaux().norad == Configuration::instance()->noradStationSpatiale(),
                                                   mcc,
                                                   satellites);
        }

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

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
    qInfo() << "Début Fonction" << __FUNCTION__;

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

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
    _informations = nullptr;
    _carte = nullptr;
    _coordISS = nullptr;
    _ciel = nullptr;
    _gmt = nullptr;
    _onglets = nullptr;
    _options = nullptr;
    _outils = nullptr;
    _radar = nullptr;

    _previsions = nullptr;
    _flashs = nullptr;
    _transits = nullptr;
    _starlink = nullptr;
    _evenements = nullptr;
    _informationsSatellite = nullptr;
    _recherche = nullptr;
    _station = nullptr;
    _captureEcran = nullptr;
    _etapePrec = nullptr;
    _etapeSuiv = nullptr;

    _messageStatut = nullptr;
    _messageStatut2 = nullptr;
    _messageStatut3 = nullptr;
    _modeFonctionnement = nullptr;
    _stsDate = nullptr;
    _stsHeure = nullptr;
    _timerStatut = nullptr;

    _dateCourante = nullptr;
    _chronometre = nullptr;
    _chronometreMs = nullptr;

    _reinitJour = false;

    /* Corps de la methode */
    try {

        // Affichage des informations generales dans le fichier de log
        qInfo() << QString("%1 %2").arg(APP_NAME).arg(QString(APP_VERSION));
        qInfo() << QString("%1 %2 %3").arg(QSysInfo::productType()).arg(QSysInfo::productVersion()).arg(QSysInfo::currentCpuArchitecture());

        setWindowTitle(QString("%1 %2").arg(APP_NAME).arg(APP_VER_MAJ));
        restoreGeometry(settings.value("affichage/geometrie").toByteArray());
        restoreState(settings.value("affichage/etat").toByteArray());

        ChargementTraduction(settings.value("affichage/langue", "en").toString());

        // Chargement de la configuration generale
        Configuration::instance()->Chargement();

        qInfo() << "Début Initialisation" << metaObject()->className();

        _options = new Options(this);

        // Verification des mises a jour au demarrage
        InitVerificationsMAJ();

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
    qInfo() << "Début Fonction" << __FUNCTION__;

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

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
    qInfo() << "Début Fonction" << __FUNCTION__;

    // Determination automatique de l'ecart heure locale - UTC
    const double ecart = Date::CalculOffsetUTC(QDateTime::currentDateTime());
    double offsetUTC = (_options->ui()->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    _options->ui()->updown->setValue(sgn(offsetUTC) * (static_cast<int>(fabs(offsetUTC) * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES)));
    offsetUTC = (_options->ui()->heureLegale->isChecked()) ? _options->ui()->updown->value() * DATE::NB_JOUR_PAR_SEC : 0.;

    // Date et heure locales
    _dateCourante = new Date(offsetUTC);

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

/*
 * Verification des mises a jour au demarrage
 */
void PreviSat::InitVerificationsMAJ()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Fonction" << __FUNCTION__;

    const QVersionNumber versionPrec = QVersionNumber::fromString(settings.value("fichier/version").toString());
    if (!versionPrec.isNull()) {

        const QVersionNumber versionAct = QVersionNumber::fromString(QString(APP_VERSION));

        if (QVersionNumber::compare(versionPrec, versionAct) < 0) {

            const int nbl = atoi(NB_LIGNES);
            const QString nbLignes =
                (nbl == 0) ? "" : tr("Cette version comporte %1 lignes de code,\n" \
                                     "ce qui représente environ %2 pages.").arg(NB_LIGNES).arg(100 * qRound(nbl / 3000.)) + "\n\n";

            const QString msg = tr("Vous venez de mettre à jour %1.\n%2Souhaitez-vous faire un don pour soutenir son auteur ?");
            QMessageBox msgbox(QMessageBox::Question, tr("Information"), msg.arg(APP_NAME).arg(nbLignes));

            QPushButton * const paypal = msgbox.addButton("PayPal", QMessageBox::YesRole);
            const QPushButton * const tipeee = msgbox.addButton("Tipeee", QMessageBox::AcceptRole);
            msgbox.addButton(tr("Non"), QMessageBox::RejectRole);

            msgbox.setDefaultButton(paypal);
            msgbox.exec();

            if (msgbox.clickedButton() == paypal) {
                on_actionPayPal_triggered();

            } else if (msgbox.clickedButton() == tipeee) {
                on_actionTipeee_triggered();
            }
        }
    }

    settings.setValue("fichier/version", QString(APP_VERSION));

#if defined (Q_OS_MAC)
    settings.setValue("affichage/verifMAJ", false);
    _options->ui()->verifMAJ->setVisible(false);
#else
    if (_options->ui()->verifMAJ->isChecked()) {
        VerifMajPreviSat();
    }
#endif

    try {

        const QString liens = settings.value("fichier/dirHttpPreviDon", "").toString();

        if (liens.isEmpty() || (liens.count('\n') <= 1)) {

            const QUrl url(QString("%1/maj/don").arg(DOMAIN_NAME));
            Telechargement tel(Configuration::instance()->dirTmp());
            tel.TelechargementFichier(url, false, false);

            QFile fi(tel.dirDwn() + QDir::separator() + QFileInfo(url.path()).fileName());

            if (fi.exists() && (fi.size() != 0)) {

                if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    settings.setValue("fichier/dirHttpPreviDon", QString(fi.readAll()));
                }

                fi.close();
            }
        }
    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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
    } else if (!langue.endsWith("_fr")) {
        qWarning() << "Impossible de charger le fichier de traduction" << langue;
    }

    /* Retour */
    return;
}

/*
 * Lecture du fichier identifiant les groupes Starlink
 */
void PreviSat::LectureGroupesStarlink()
{
    /* Declarations des variables locales */
    QMap<QString, QStringList> mapGroupes;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirStarlink() + QDir::separator() + "starlink.txt");
    if (fi.exists() && (fi.size() != 0)) {

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            const QStringList contenu = QString(fi.readAll()).split("\n", Qt::SkipEmptyParts);

            QString groupe;
            QString groupePrec;
            QStringList elem;
            QStringList grp;

            QStringListIterator it(contenu);
            while (it.hasNext()) {

                elem = it.next().split(" ", Qt::SkipEmptyParts);
                groupe = elem.at(1);
                if (groupePrec.isEmpty()) {
                    groupePrec = groupe;
                }

                if (groupe == groupePrec) {

                    grp.append(elem.first());

                } else {

                    mapGroupes.insert(groupePrec, grp);

                    grp.clear();
                    grp.append(elem.first());

                    groupePrec = groupe;
                }
            }

            mapGroupes.insert(groupePrec, grp);
        }
    }

    fi.close();

    Configuration::instance()->groupesStarlink() = mapGroupes;

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

        QString adresse;
        Telechargement tel(Configuration::instance()->dirElem());

        QMapIterator it1(Configuration::instance()->mapCategoriesMajElementsOrbitaux());
        while (it1.hasNext()) {
            it1.next();

            adresse = it1.key();
            const QList<CategorieElementsOrbitaux> listeCategories = it1.value();

            QListIterator it2(listeCategories);
            while (it2.hasNext()) {

                const CategorieElementsOrbitaux categorie = it2.next();

                if (adresse.contains("celestrak")) {
                    adresse = Configuration::instance()->adresseCelestrakNorad();
                }

                if (adresse.contains("previsat")) {
                    adresse = QString(DOMAIN_NAME) + "elem/%1";
                }

                foreach (const QString fic, categorie.fichiers) {

                    const QString fichier = (adresse.contains("celestrak")) ? QFileInfo(fic).baseName() : fic;
                    tel.TelechargementFichier(QUrl(adresse.arg(fichier)));
                }
            }
        }
    } catch (PreviSatException const &e) {
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
    qInfo() << "Début Fonction" << __FUNCTION__;

    if (!Configuration::instance()->mapElementsOrbitaux().isEmpty()) {

        const int ageMax = settings.value("temps/ageMax", 15).toInt();
        const QString noradDefaut = Configuration::instance()->noradDefaut();

        // Verification de l'age des elements orbitaux
        if ((fabs(_dateCourante->jourJulienUTC() - Configuration::instance()->mapElementsOrbitaux()[noradDefaut].epoque.jourJulienUTC()) > ageMax) &&
                _ui->tempsReel->isChecked()) {

            const QString msg = tr("Les éléments orbitaux sont plus vieux que %1 jour(s). Souhaitez-vous les mettre à jour?");

            QMessageBox msgbox(QMessageBox::Question, tr("Information"), msg.arg(ageMax));
            QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
            msgbox.addButton(tr("Non"), QMessageBox::NoRole);
            msgbox.setDefaultButton(oui);
            msgbox.exec();

            if (msgbox.clickedButton() == oui) {
                MajFichierGP();
            }
        }
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

/*
 * Verification d'une mise a jour a partir d'une date
 */
bool PreviSat::VerifMajDate(const QString &fichier, const QStringList &listeFichierMaj, const QDate &dateMaj)
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool anew = false;

    /* Corps de la methode */
    try {

        qInfo() << "Début Fonction" << __FUNCTION__;

        Telechargement tel(Configuration::instance()->dirTmp());
        tel.TelechargementFichier(QString("%1/maj/%2").arg(DOMAIN_NAME).arg(fichier), false, false);

        QFile fi(tel.dirDwn() + QDir::separator() + fichier);
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

                const QString ligne = fi.readLine().trimmed();
                const QDate nouvelleDate = QDate::fromString(ligne, "yyyy-MM-dd");
                fi.close();

                if (!listeFichierMaj.isEmpty()) {

                    QDate dateMax;
                    QStringListIterator it(listeFichierMaj);
                    while (it.hasNext()) {

                        const QString fic = it.next();

                        if (!fic.contains("preferences")) {

                            const QString fich = Configuration::instance()->dirLocalData() + QDir::separator() + fic;
                            const QFileInfo ff(fich);

                            // Comparaison de la nouvelle date avec celle des fichiers du repertoire local
                            if (ff.lastModified().date() > dateMax) {
                                dateMax = QDate(ff.lastModified().date());
                            }
                        }
                    }

                    anew = (nouvelleDate > dateMax);
                }

                if (dateMaj.isValid()) {
                    anew = (nouvelleDate > dateMaj);
                    _majInfosDate = ligne;
                }
            }
        }
    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return anew;
}

/*
 * Verification des mises a jour (logiciel, fichiers internes)
 */
void PreviSat::VerifMajPreviSat()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Fonction" << __FUNCTION__;

    // Version du logiciel
    const bool anew = VerifMajVersion(QString("version%1").arg(APP_NAME));

    if (!anew) {

        // Fichiers internes
        if (VerifMajDate("majFichiersInternes", Configuration::instance()->listeFicLocalData())) {

            _ui->actionMettre_a_jour_les_fichiers_de_donnees->setVisible(true);

            const QString msg = tr("Une mise à jour %1 est disponible. Souhaitez-vous la télécharger?");

            QMessageBox msgbox(QMessageBox::Question, tr("Information"), msg.arg(tr("des fichiers internes")));
            QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
            msgbox.addButton(tr("Non"), QMessageBox::NoRole);
            msgbox.setDefaultButton(oui);
            msgbox.exec();

            if (msgbox.clickedButton() == oui) {

                on_actionMettre_a_jour_les_fichiers_de_donnees_triggered();
                _ui->actionMettre_a_jour_les_fichiers_de_donnees->setVisible(false);
            }
        } else {
            _ui->actionMettre_a_jour_les_fichiers_de_donnees->setVisible(false);
        }

        // Fichiers d'informations
        QFont fnt;
        if (settings.value("affichage/informationsDemarrage", true).toBool()) {
            fnt.setBold(false);
            _ui->actionInformations->setFont(fnt);

        } else {

            const QDate dateMaj = QDate::fromString(settings.value("temps/lastInfos").toString(), "yyyy-MM-dd");
            if (VerifMajDate("majInfos", QStringList(), dateMaj)) {

                fnt.setBold(true);
                _ui->actionInformations->setFont(fnt);
            }
        }
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

/*
 * Verification d'une mise a jour a partir d'un fichier de version
 */
bool PreviSat::VerifMajVersion(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */
    qInfo() << "Début Fonction" << __FUNCTION__;

    bool anew = false;

    /* Corps de la methode */
    try {

        bool majPrevi = settings.value("fichier/majPreviSat", true).toBool();
        Telechargement tel(Configuration::instance()->dirTmp());
        tel.TelechargementFichier(QString("%1/maj/%2").arg(DOMAIN_NAME).arg(fichier), false, false);

        QFile fi(tel.dirDwn() + QDir::separator() + fichier);
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

                // Verification du numero de version
                const QVersionNumber versionActuelle = QVersionNumber::fromString(settings.value("fichier/version", "").toString());
                const QVersionNumber versionNouvelle = QVersionNumber::fromString(fi.readLine());
                fi.close();

                anew = (!versionActuelle.isNull() && (QVersionNumber::compare(versionActuelle, versionNouvelle) < 0));

                if (anew && majPrevi) {

                    _ui->actionTelecharger_la_mise_a_jour->setVisible(true);

                    const QString msg = tr("Une mise à jour %1 est disponible. Souhaitez-vous la télécharger?");

                    QMessageBox msgbox(QMessageBox::Question, tr("Information"), msg.arg(APP_NAME));
                    QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);

                    msgbox.addButton(tr("Non"), QMessageBox::NoRole);
                    msgbox.setDefaultButton(oui);
                    msgbox.exec();

                    if (msgbox.clickedButton() == oui) {

                        on_actionTelecharger_la_mise_a_jour_triggered();
                        _ui->actionTelecharger_la_mise_a_jour->setVisible(false);
                    }

                    majPrevi = false;
                } else {
                    majPrevi = true;
                }

                settings.setValue("fichier/majPreviSat", majPrevi);

                if (!majPrevi) {
                    QFont police;
                    police.setBold(true);
                    _ui->actionTelecharger_la_mise_a_jour->setFont(police);
                }
            }
        }

        _ui->actionTelecharger_la_mise_a_jour->setVisible(!majPrevi);

    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return anew;
}

/*
 * Afficher les coordonnees ISS et le label GMT
 */
void PreviSat::AfficherCoordIssGmt()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!Configuration::instance()->listeSatellites().isEmpty()
        && (Configuration::instance()->listeSatellites().first().elementsOrbitaux().norad == Configuration::instance()->noradStationSpatiale())) {

        // Coordonnees ISS
        _coordISS->setPolice();
        if (settings.value("affichage/affNbOrbWCC", true).toBool() && settings.value("affichage/affBetaWCC", false).toBool()) {

            _coordISS->ui()->inclinaisonISS->move(5, 39);
            _coordISS->ui()->nextTransitionISS->move(112, 0);
            _coordISS->ui()->orbiteISS->move(112, 13);
            _coordISS->ui()->orbiteISS->setVisible(true);
            _coordISS->ui()->betaISS->move(112, 26);
            _coordISS->ui()->betaISS->setVisible(true);
            _coordISS->resize(223, 59);

        } else {

            _coordISS->ui()->inclinaisonISS->move(112, 0);
            _coordISS->ui()->nextTransitionISS->move(112, 13);
            _coordISS->ui()->orbiteISS->setVisible(false);
            _coordISS->ui()->betaISS->setVisible(false);

            if (settings.value("affichage/affNbOrbWCC", true).toBool()) {

                _coordISS->ui()->orbiteISS->move(112, 26);
                _coordISS->ui()->orbiteISS->setVisible(true);
                _coordISS->ui()->betaISS->setVisible(false);

            } else if (settings.value("affichage/affBetaWCC", false).toBool()) {

                _coordISS->ui()->betaISS->move(112, 26);
                _coordISS->ui()->betaISS->setVisible(true);
                _coordISS->ui()->orbiteISS->setVisible(false);
            }
            _coordISS->resize(223, 46);
        }

        _coordISS->show(*_dateCourante, General::dateEclipse());
        _coordISS->setVisible(true);

        // GMT
        const QString chaine = "GMT = %1/%2:%3:%4";
        const Date date2 = Date(_dateCourante->annee(), 1, 1., 0.);
        const double jourDsAnnee = _dateCourante->jourJulienUTC() - date2.jourJulienUTC() + 1.;
        const int numJour = (int) jourDsAnnee;
        const int heure = (int) floor(DATE::NB_HEUR_PAR_JOUR * (jourDsAnnee - numJour) + 0.00005);
        const int min = _dateCourante->minutes();
        const int sec = (int) floor(_dateCourante->secondes());
        const QString texte =
            chaine.arg(numJour, 3, 10, QChar('0')).arg(heure, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));

        QPalette coul;
        coul.setColor(_gmt->foregroundRole(), _coulGmt[settings.value("affichage/coulGMT", false).toUInt()]);

        _gmt->setText(texte);
        _gmt->setPalette(coul);

        QFont police = Configuration::instance()->policeWcc();
        police.setPointSize(12);
        police.setBold(true);

        _gmt->setFont(police);
        _gmt->adjustSize();
        _gmt->setGeometry((_carte->width() - _gmt->width()) / 2, 30, _gmt->width(), 16);
        _gmt->show();

    } else {
        _coordISS->setVisible(false);
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
    _ui->satellitesChoisis->setChecked(false);
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
        switch (static_cast<Qt::CheckState> (settings.value("affichage/affNoradListes", Qt::Unchecked).toUInt())) {
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
        elem->setData(Qt::CheckStateRole, (check) ? Qt::Checked : Qt::Unchecked);
        elem->setToolTip(tooltip);
        elem->setFlags(Qt::ItemIsEnabled);

        if (norad == noradDefaut) {
            elem->setData(Qt::BackgroundRole, QColor(160, 220, 240));
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
        connect(_timerStatut, &QTimer::timeout, this, &PreviSat::EffacerMessageStatut);
        _timerStatut->start();
    }

    /* Retour */
    return;
}

/*
 * Affichage d'un message dans la zone de statut 2
 */
void PreviSat::AfficherMessageStatut2(const QString &message)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (message.isEmpty()) {
        _messageStatut2->setVisible(false);
    } else {
        _messageStatut2->setText(message);
        _messageStatut2->repaint();
        _messageStatut2->setVisible(true);
    }

    /* Retour */
    return;
}

/*
 * Affichage d'un message dans la zone de statut 3
 */
void PreviSat::AfficherMessageStatut3(const QString &message)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (message.isEmpty()) {
        _messageStatut3->setVisible(false);
    } else {
        _messageStatut3->setText(message);
        _messageStatut3->repaint();
        _messageStatut3->setVisible(true);
    }

    /* Retour */
    return;
}

/*
 * Capture d'ecran de la fenetre
 */
void PreviSat::CaptureEcran()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Capture de la fenetre
    _chronometre->stop();
    _chronometreMs->stop();

    const QPixmap image = QWidget::grab(rect());
    if (_isCarteMonde) {
        _ciel->setVisible(false);
    } else {
        _carte->setVisible(false);
    }

    const QString nomFicDefaut = Configuration::instance()->dirOut() + QDir::separator() + "previsat_" +
                                 _dateCourante->ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).remove("/").remove(":")
                                     .replace(" ", "_") + "_" + _options->ui()->tuc->text().remove(" ").remove(":");

    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomFicDefaut,
                                                     tr("Fichiers PNG (*.png);;Fichiers JPEG (*.jpg *.jpeg);;Fichiers BMP (*.bmp);;" \
                                                        "Tous les fichiers (*.*)"));

    if (!fic.isEmpty()) {
        image.save(fic);
        const QFileInfo fi(fic);
        settings.setValue("fichier/sauvegarde", fi.absolutePath());
    }

    _chronometre->start();
    _chronometreMs->start();

    /* Retour */
    return;
}

/*
 * Changement de la date en mode manuel
 */
void PreviSat::ChangementDate(const QDateTime &dt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const bool etat1 = _onglets->general()->ui()->dateHeure2->blockSignals(true);
    const bool etat2 = _onglets->osculateurs()->ui()->dateHeure2->blockSignals(true);

    _onglets->general()->ui()->dateHeure2->setDateTime(dt);
    _onglets->osculateurs()->ui()->dateHeure2->setDateTime(dt);

    _onglets->general()->ui()->dateHeure2->blockSignals(etat1);
    _onglets->osculateurs()->ui()->dateHeure2->blockSignals(etat2);

    if (!_onglets->general()->ui()->pause->isEnabled()) {
        EnchainementCalculs();
    }

    /* Retour */
    return;
}

/*
 * Changement du fuseau horaire
 */
void PreviSat::ChangementFuseauHoraire(const int offset)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double jjutc = _dateCourante->jourJulienUTC();

    /* Corps de la methode */
    EFFACE_OBJET(_dateCourante);
    _dateCourante = new Date(jjutc, offset * DATE::NB_JOUR_PAR_SEC);

    /* Retour */
    return;
}

/*
 * Chargement de la traduction
 */
void PreviSat::ChargementTraduction(const QString &langue)
{
    /* Declarations des variables locales */

    /* Initialisations */
    qInfo() << "Locale :" << langue;

    /* Corps de la methode */
    InstallationTraduction(QString("%1_%2").arg(APP_NAME).arg(langue), _appTraduction);
    InstallationTraduction(QString("qt_%1").arg(langue), _qtTraduction);
    Configuration::instance()->locale() = settings.value("affichage/langue", "en").toString();

    _ui->retranslateUi(this);
    if (_onglets != nullptr) {

        _onglets->ReinitFlags();
        _onglets->ui()->retranslateUi(_onglets);

        _onglets->general()->ui()->retranslateUi(_onglets->general());
        _onglets->osculateurs()->ui()->retranslateUi(_onglets->osculateurs());
        _onglets->informationsSatellite()->ui()->retranslateUi(_onglets->informationsSatellite());

        _onglets->show(*_dateCourante);
    }

    if ((_carte != nullptr) && (_ciel != nullptr) && (_radar != nullptr)) {
        AffichageCartesRadar();
    }

    QEvent evt(QEvent::LanguageChange);

    if (_options != nullptr) {
        _options->changeEvent(&evt);
        _options->Initialisation();
    }

    if (_outils != nullptr) {
        _outils->changeEvent(&evt);
    }

    if (_informations != nullptr) {
        _informations->changeEvent(&evt);
    }

    /* Retour */
    return;
}

/*
 * Effacer la zone de message de statut
 */
void PreviSat::EffacerMessageStatut()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _messageStatut->setText("");

    /* Retour */
    return;
}

/*
 * Etape precedente en mode manuel
 */
void PreviSat::EtapePrecedente()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_ui->modeManuel->isChecked()) {
        _ui->modeManuel->setChecked(true);
    }

    /* Corps de la methode */
    const double jd = (_ui->valManuel->currentIndex() < 3) ?
                          _dateCourante->jourJulienUTC() - _ui->pasManuel->currentText().toDouble() *
                                                               qPow(DATE::NB_SEC_PAR_MIN, _ui->valManuel->currentIndex()) * DATE::NB_JOUR_PAR_SEC :
                          _dateCourante->jourJulienUTC() - _ui->pasManuel->currentText().toDouble();

    const double offset = _dateCourante->offsetUTC();
    EFFACE_OBJET(_dateCourante);
    _dateCourante = new Date(jd, offset);

    // Enchainement de l'ensemble des calculs
    EnchainementCalculs();

    const QString fmt = tr("dddd dd MMMM yyyy  HH:mm:ss") + ((_options->ui()->syst12h->isChecked()) ? "a" : "");

    _onglets->osculateurs()->ui()->dateHeure2->setDisplayFormat(fmt);
    _onglets->general()->ui()->dateHeure2->setDisplayFormat(fmt);
    ChangementDate(_dateCourante->ToQDateTime(1));

    _onglets->show(*_dateCourante);

    AffichageCartesRadar();

    /* Retour */
    return;
}

/*
 * Etape suivante en mode manuel
 */
void PreviSat::EtapeSuivante()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (!_ui->modeManuel->isChecked()) {
        _ui->modeManuel->setChecked(true);
    }

    /* Corps de la methode */
    const double jd = (_ui->valManuel->currentIndex() < 3) ?
                          _dateCourante->jourJulienUTC() + _ui->pasManuel->currentText().toDouble() *
                                                               qPow(DATE::NB_SEC_PAR_MIN, _ui->valManuel->currentIndex()) * DATE::NB_JOUR_PAR_SEC :
                          _dateCourante->jourJulienUTC() + _ui->pasManuel->currentText().toDouble();

    const double offset = _dateCourante->offsetUTC();
    EFFACE_OBJET(_dateCourante);
    _dateCourante = new Date(jd, offset);

    // Enchainement de l'ensemble des calculs
    EnchainementCalculs();

    const QString fmt = tr("dddd dd MMMM yyyy  HH:mm:ss") + ((_options->ui()->syst12h->isChecked()) ? "a" : "");

    _onglets->osculateurs()->ui()->dateHeure2->setDisplayFormat(fmt);
    _onglets->general()->ui()->dateHeure2->setDisplayFormat(fmt);
    ChangementDate(_dateCourante->ToQDateTime(1));

    _onglets->show(*_dateCourante);

    AffichageCartesRadar();

    /* Retour */
    return;
}

/*
 * Gestion du temps reel
 */
void PreviSat::GestionTempsReel()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->tempsReel->isChecked()) {

        // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
        EnchainementCalculs();

        // Affichage des donnees numeriques dans la barre d'onglets
        _onglets->show(*_dateCourante);

        // Notifications sonores
        if (settings.value("affichage/affnotif", false).toBool() && !Configuration::instance()->listeSatellites().isEmpty()) {

            NotificationSonore &notif = Configuration::instance()->notifAOS();
            const Satellite sat = Configuration::instance()->listeSatellites().first();

            if (sat.isVisible() && (notif == NotificationSonore::ATTENTE_LOS)) {
                notif = NotificationSonore::NOTIFICATION_AOS;
            }

            if (!sat.isVisible() && (notif == NotificationSonore::ATTENTE_AOS)) {
                notif = NotificationSonore::NOTIFICATION_LOS;
            }

            if (notif == NotificationSonore::NOTIFICATION_AOS) {

                // Notification sonore pour l'AOS
                if (_options->ui()->listeSons->currentIndex() < _options->ui()->listeSons->count()) {

                    const QString nomSonAOS = (_options->ui()->listeSons->currentIndex() == 0) ?
                                Configuration::instance()->dirCommonData() + QDir::separator() + "sound" + QDir::separator() + "aos-default.wav" :
                                Configuration::instance()->dirSon() + QDir::separator() + "aos-" + _options->ui()->listeSons->currentText() + ".wav";

                    const QFileInfo ff(nomSonAOS);
                    if (ff.exists()) {
                        QSoundEffect son;
                        son.setSource(QUrl(nomSonAOS));
                        son.play();
                    }
                }

                notif = NotificationSonore::ATTENTE_AOS;
            }

            if (notif == NotificationSonore::NOTIFICATION_LOS) {

                // Notification sonore pour le LOS
                if (_options->ui()->listeSons->currentIndex() < _options->ui()->listeSons->count()) {

                    const QString nomSonLOS = (_options->ui()->listeSons->currentIndex() == 0) ?
                                Configuration::instance()->dirCommonData() + QDir::separator() + "sound" + QDir::separator() + "los-default.wav" :
                                Configuration::instance()->dirSon() + QDir::separator() + "los-" + _options->ui()->listeSons->currentText() + ".wav";

                    const QFileInfo ff(nomSonLOS);
                    if (ff.exists()) {
                        QSoundEffect son;
                        son.setSource(QUrl(nomSonLOS));
                        son.play();
                    }
                }

                notif = NotificationSonore::ATTENTE_LOS;
            }
        }

        AffichageCartesRadar();
    }

    if (_ui->modeManuel->isChecked()) {

        _onglets->setAcalcDN(true);
        _onglets->setAcalcAOS(true);

        if (_onglets->general()->ui()->pause->isEnabled()) {

            if (!_ui->pasManuel->view()->isVisible()) {

                double pas;
                if (_ui->valManuel->currentIndex() < 3) {
                    pas = _ui->pasManuel->currentText().toDouble() * qPow(DATE::NB_SEC_PAR_MIN, _ui->valManuel->currentIndex()) * DATE::NB_JOUR_PAR_SEC;
                } else {
                    pas = _ui->pasManuel->currentText().toDouble();
                }

                double jd = _dateCourante->jourJulienUTC();
                if (!_onglets->general()->ui()->rewind->isEnabled() || !_onglets->general()->ui()->backward->isEnabled()) {
                    jd -= pas;
                }
                if (!_onglets->general()->ui()->play->isEnabled() || !_onglets->general()->ui()->forward->isEnabled()) {
                    jd += pas;
                }

                const double offset = _dateCourante->offsetUTC();
                EFFACE_OBJET(_dateCourante);
                _dateCourante = new Date(jd, offset);

                const QString fmt = tr("dddd dd MMMM yyyy  HH:mm:ss") + ((_options->ui()->syst12h->isChecked()) ? "a" : "");

                _onglets->osculateurs()->ui()->dateHeure2->setDisplayFormat(fmt);
                _onglets->general()->ui()->dateHeure2->setDisplayFormat(fmt);

                // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
                EnchainementCalculs();
            }
        }

        ChangementDate(_dateCourante->ToQDateTime(1));

        _onglets->show(*_dateCourante);
        AffichageCartesRadar();
    }

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

        qInfo() << "Début Fonction" << __FUNCTION__;

        QString nomfic;
        QMap<QString, ElementsOrbitaux> mapElem;
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
                mapElem = GPFormat::LectureFichier(fic, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec(),
                                                   QStringList(), false);

                ajout = !mapElem.isEmpty();
                nomfic = ff.baseName();

            } else {

                // Cas des fichiers TLE
                ajout = (TLE::VerifieFichier(fic) > 0);
                if (ajout) {
                    mapElem = TLE::LectureFichier(fic, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec(),
                                                  QStringList(), false);
                    nomfic = ff.baseName() + " (TLE)";
                }
            }

            if (ajout) {

                // Ajout du fichier dans la liste
                nomfic[0] = nomfic[0].toUpper();
                _ui->listeFichiersElem->addItem(nomfic);

                if (nom == Configuration::instance()->nomfic()) {
                    const int index = _ui->listeFichiersElem->count() - 1;
                    _ui->listeFichiersElem->setCurrentIndex(index);
                    _ui->listeFichiersElem->setItemData(index, QColor(Qt::gray), Qt::BackgroundRole);
                }

                Configuration::instance()->mapFichierElemNorad()[nom] = mapElem.keys();

            } else {

                // Suppression dans la liste des fichiers qui ne sont pas des elements orbitaux
                if (nom == Configuration::instance()->nomfic()) {
                    defaut = true;
                }

                if (idx < listeElem.size()) {
                    listeElem.removeAt(idx);
                }
            }

            idx++;
        }

        if (listeElem.count() == 0) {
            _ui->listeFichiersElem->addItem("");
        } else if (defaut) {
            Configuration::instance()->nomfic() = Configuration::instance()->dirElem() + QDir::separator() + listeElem.first();
        }
        _ui->listeFichiersElem->blockSignals(etat);

        // Mise a jour de la liste de fichiers d'elements orbitaux
        Configuration::instance()->setListeFicElem(listeElem);

    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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

        ChargementGP();
        _onglets->ReinitFlags();
        DemarrageApplication();

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

/*
 * Mise a jour d'un groupe d'elements orbitaux
 */
void PreviSat::MettreAjourGroupeElem(const QString &groupe)
{
    /* Declarations des variables locales */
    QString adresse;

    /* Initialisations */
    qInfo() << "Début Fonction" << __FUNCTION__;

    AfficherMessageStatut(tr("Mise à jour du groupe d'éléments orbitaux \"%1\"...").arg(groupe));
    Telechargement tel(Configuration::instance()->dirElem());

    /* Corps de la methode */
    QMapIterator it1(Configuration::instance()->mapCategoriesElementsOrbitaux());
    while (it1.hasNext()) {
        it1.next();

        adresse = it1.key();

        if (adresse.contains("celestrak")) {
            adresse = Configuration::instance()->adresseCelestrakNorad();
        }

        if (adresse.contains("previsat")) {
            adresse = QString(DOMAIN_NAME) + "elem/%1";
        }

        QListIterator it2(it1.value());
        while (it2.hasNext()) {

            const CategorieElementsOrbitaux categorie = it2.next();
            if (groupe.contains(categorie.nom[Configuration::instance()->locale()], Qt::CaseInsensitive)) {

                foreach (const QString fic, categorie.fichiers) {

                    const QString fichier = (adresse.contains("celestrak")) ? QFileInfo(fic).baseName() : fic;
                    tel.TelechargementFichier(QUrl(adresse.arg(fichier)));
                }
            }
        }
    }

    Configuration::instance()->InitListeFichiersElem();
    InitFicGP();
    AfficherMessageStatut(tr("Mise à jour du groupe d'éléments orbitaux \"%1\" terminée").arg(groupe), 5);

    // Rechargement du fichier courant (qui a ete mis a jour)
    ChargementGP();
    _onglets->ReinitFlags();
    DemarrageApplication();

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

/*
 * Modification de la date en mode manuel
 */
void PreviSat::ModificationDate(const QDateTime &dt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const double offset = _dateCourante->offsetUTC();
    EFFACE_OBJET(_dateCourante);
    _dateCourante = new Date(dt, offset);

    // Enchainement de l'ensemble des calculs
    EnchainementCalculs();

    _onglets->show(*_dateCourante);

    AffichageCartesRadar();

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

void PreviSat::RaccourciStarlink()
{
    const int index = _onglets->ui()->stackedWidget_previsions->indexOf(_onglets->ui()->starlink);
    if (index != -1) {
        _onglets->setCurrentWidget(_onglets->ui()->previsions);
        _onglets->ui()->stackedWidget_previsions->setCurrentWidget(_onglets->ui()->starlink);
        _onglets->setIndexPrevisions(index);
    }
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
 * Reinitialisation du calcul des evenements Soleil/Lune
 */
void PreviSat::ReinitCalculEvenementsSoleilLune()
{
    _reinitJour = true;
}

/*
 * Telechargement des groupes Starlink
 */
void PreviSat::TelechargementGroupesStarlink()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "Telechargement des groupes Starlink";

        if (!settings.value("affichage/verifMAJ").toBool()) {
            throw PreviSatException();
        }

        // Telechargement du verrou Starlink
        Telechargement tel1(Configuration::instance()->dirTmp());
        tel1.TelechargementFichier(QString(DOMAIN_NAME) + "maj/verrouStarlink", false);

        if (VerifieDateExpirationStarlink()) {
            throw PreviSatException();
        }

        // Telechargement des informations generales des pre-launch Starlink
        Telechargement tel2(Configuration::instance()->dirStarlink());
        tel2.TelechargementFichier(QUrl(Configuration::instance()->adresseCelestrakSupplementalNorad() + "index.php"));

        // Telechargement du fichier d'elements orbitaux le plus a jour
        tel2.TelechargementFichier(QUrl(Configuration::instance()->adresseCelestrakSupplementalNoradFichier().arg("starlink")));

        // Telechargement des trains Starlink connus
        tel2.TelechargementFichier(QUrl(QString(DOMAIN_NAME) + "starlink/starlink.txt"));

        // Lecture du fichier starlink.txt
        LectureGroupesStarlink();

        // Recuperation des nouveaux groupes de lancement
        QFile fi(Configuration::instance()->dirStarlink() + QDir::separator() + "index.php");
        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QString fichier;
            QString groupe;
            unsigned int debf = 0;
            unsigned int debl = 0;

            const QString contenu = fi.readAll();
            const unsigned int nb = static_cast<unsigned int> (contenu.count("sup-gp.php?FILE=starlink-"));
            const QMap<QString, SatellitesStarlink> satellitesStarlink = Configuration::instance()->satellitesStarlink();

            if (nb > 0) {
                Configuration::instance()->satellitesStarlink().clear();
            }

            for(unsigned int i=0; i<nb; i++) {

                const unsigned int indf = static_cast<unsigned int> (contenu.indexOf("sup-gp.php?FILE=starlink-", debf)) + 16;
                const unsigned int finf = static_cast<unsigned int> (contenu.indexOf("&FORMAT", indf));
                const unsigned int indg = static_cast<unsigned int> (contenu.indexOf(">", indf) + 1);
                const unsigned int fing = static_cast<unsigned int> (contenu.indexOf("<", indg));
                const unsigned int indl = static_cast<unsigned int> (contenu.indexOf("Launch: ", debl));

                // Informations Starlink
                fichier = contenu.mid(indf, finf - indf);
                groupe = contenu.mid(indg, fing - indg);
                const QString lancement = contenu.mid(contenu.indexOf("Launch: ", indl) + 8, 19);
                const QString deploiement = contenu.mid(contenu.indexOf("Deployment: ", indl) + 12, 19);

                const QString nomGroupe = groupe.split(" ", Qt::SkipEmptyParts).at(1);
                if (Configuration::instance()->groupesStarlink().keys().contains(nomGroupe)) {

                    groupe = groupe.split(" ", Qt::SkipEmptyParts).first() + " " + nomGroupe;
                    fichier = "starlink";

                } else {

                    // Telechargement du fichier d'elements orbitaux correspondant
                    const QUrl url(Configuration::instance()->adresseCelestrakSupplementalNoradFichier().arg(fichier));
                    Telechargement tel3(Configuration::instance()->dirStarlink());
                    tel3.TelechargementFichier(url);
                }

                // Ajout du groupe dans la liste
                Configuration::instance()->AjoutDonneesSatellitesStarlink(groupe, fichier, lancement, deploiement);

                debf = finf + 1;
                debl = indl + 1;
            }

            fi.close();

            if (nb > 0) {

                QMapIterator it(satellitesStarlink);
                while (it.hasNext()) {
                    it.next();

                    const SatellitesStarlink starlink = it.value();
                    if (!starlink.fichier.split("-").last().contains("b")) {
                        Configuration::instance()->AjoutDonneesSatellitesStarlink(it.key(), starlink.fichier, starlink.lancement, starlink.deploiement);
                    }
                }

                if (Configuration::instance()->satellitesStarlink().isEmpty()) {
                    throw PreviSatException();
                }
            }

            _onglets->starlink()->show();
        }

    } catch (PreviSatException const &e) {

        if (Configuration::instance()->satellitesStarlink().isEmpty()) {
            _onglets->ui()->stackedWidget_previsions->removeWidget(_onglets->ui()->starlink);

        } else if (!VerifieDateExpirationStarlink()) {
            _onglets->starlink()->show();
        }
    }

    /* Retour */
    return;
}

void PreviSat::TempsReel()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->tempsReel->isChecked()) {

        // Date actuelle
        const double offset = (_options->ui()->utcAuto->isChecked()) ? Date::CalculOffsetUTC(QDateTime::currentDateTime()) : _dateCourante->offsetUTC();
        EFFACE_OBJET(_dateCourante);

        _dateCourante = new Date(offset);
        _modeFonctionnement->setText(tr("Temps réel"));

    } else {
        _modeFonctionnement->setText(tr("Mode manuel"));
    }

    if (_options->ui()->calJulien->isChecked()) {

        // Affichage du jour julien dans la barre de statut
        const Date date1(_dateCourante->annee(), 1, 1, 0.);
        _stsDate->setText(QString::number(_dateCourante->jourJulien() + DATE::TJ2000, 'f', 5));
        _stsHeure->setText(QString::number(_dateCourante->jourJulien() - date1.jourJulien() + 1., 'f', 5));
        _stsDate->setToolTip(tr("Jour julien"));
        _stsHeure->setToolTip(tr("Jour"));

    } else {

        // Affichage de la date et l'heure dans la barre de statut
        const QDateTime d = _dateCourante->ToQDateTime(1);
        _stsDate->setText(d.toString(tr("dd/MM/yyyy", "date format")));
        _stsHeure->setText(QLocale(Configuration::instance()->locale()).toString(d, QString("HH:mm:ss") +
                                                                                 ((_options->ui()->syst12h->isChecked()) ? "a" : "")));
        _stsDate->setToolTip(tr("Date"));
        _stsHeure->setToolTip(tr("Heure"));
    }

    /* Retour */
    return;
}

/*
 * Verification de la date d'expiration Starlink
 */
bool PreviSat::VerifieDateExpirationStarlink()
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool res = false;

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + "verrouStarlink");
    if (fi.exists() && (fi.size() != 0)) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        const QDate date = QDate::fromString(fi.readAll(), Qt::ISODate);
        fi.close();

        if (QDate::currentDate() >= date) {
            Configuration::instance()->satellitesStarlink().clear();
            res = true;
        }
    }

    /* Retour */
    return res;
}

void PreviSat::closeEvent(QCloseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    // Suppression des fichiers du cache
    const QDir di = QDir(Configuration::instance()->dirTmp());
    const QStringList listeFic = di.entryList(QDir::Files);
    foreach(const QString fic, listeFic) {
        if (!(_options->ui()->verifMAJ->isChecked() && ((fic == "versionPreviSat") || (fic == "majFichiersInternes") || (fic == "verrouStarlink")))) {
            QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + fic);
            fi.remove();
        }
    }

    // Sauvegarde des donnees du logiciel
    settings.setValue("temps/valManuel", _ui->valManuel->currentIndex());
    settings.setValue("temps/pasManuel", _ui->pasManuel->currentIndex());
    settings.setValue("temps/pasReel", _ui->pasReel->currentIndex());
    settings.setValue("temps/dtu", _options->ui()->updown->value() * DATE::NB_JOUR_PAR_SEC);

    settings.setValue("affichage/geometrie", saveGeometry());
    settings.setValue("affichage/etat", saveState());
    settings.setValue("affichage/issLive", _ui->issLive->isChecked());

    emit AppliquerPreferences();
    GestionnaireXml::EcritureConfiguration();
    GestionnaireXml::EcriturePreLaunchStarlink();

    /* Retour */
    return;
}

bool PreviSat::eventFilter(QObject *watched, QEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QMouseEvent* const evt = static_cast<QMouseEvent*> (event);

    /* Corps de la methode */
    if ((event->type() == QEvent::MouseMove) || (event->type() == QEvent::HoverEnter)) {

        if (_carte->underMouse()) {
            _carte->mouseMoveEvent(evt);

        } else if ((_ciel != nullptr) && (_ciel->underMouse())) {
            _ciel->mouseMoveEvent(evt);

        } else if (_radar->underMouse()) {
            _radar->mouseMoveEvent(evt);
        }

    } else if (event->type() == QEvent::MouseButtonPress) {

        if (_carte->underMouse()) {
            _carte->mousePressEvent(evt);

        } else if ((_ciel != nullptr) && (_ciel->underMouse())) {
            _ciel->mousePressEvent(evt);

        } else if (_radar->underMouse()) {
            _radar->mousePressEvent(evt);
        }

    } else if ((event->type() == QEvent::Leave) || (event->type() == QEvent::HoverLeave)) {

        setCursor(Qt::ArrowCursor);
        setToolTip("");

        EffacerMessageStatut();
        AfficherMessageStatut2("");
        AfficherMessageStatut3("");
    }

    /* Retour */
    return QMainWindow::eventFilter(watched, event);
}

void PreviSat::keyPressEvent(QKeyEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (evt->key() == Qt::Key_F10) {

        // Bascule Temps reel/Mode manuel
        if (_ui->tempsReel->isChecked()) {
            _ui->modeManuel->setChecked(true);

        } else if (_ui->modeManuel->isChecked()) {
            _ui->tempsReel->setChecked(true);
        }

        GestionTempsReel();
    }

    /* Retour */
    return;
}

void PreviSat::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (_stsDate->underMouse() || _stsHeure->underMouse()) {
        _options->ui()->calJulien->setChecked(!_options->ui()->calJulien->isChecked());
        TempsReel();
    }

    /* Retour */
    return;
}


/*
 * Boutons de l'interface graphique
 */
void PreviSat::on_configuration_clicked()
{
    qInfo() << "Ouverture de la fenêtre de configuration";
    _options->Initialisation();
    _options->show(this);
}

void PreviSat::on_outils_clicked()
{
    qInfo() << "Ouverture de la fenêtre d'outils";
    _outils->Initialisation();
    _outils->show();
}

void PreviSat::on_aide_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString ficDox = QString("%1/%2/html/index.html").arg(Configuration::instance()->dirDox()).arg(Configuration::instance()->locale());

    /* Corps de la methode */
    if (QDesktopServices::openUrl("file:///" + ficDox)) {
        qInfo() << "Ouverture de l'aide en ligne";
    } else {
        qWarning() << "Impossible d'afficher l'aide en ligne";
        Message::Afficher(tr("Impossible d'afficher l'aide en ligne"), MessageType::WARNING);
    }

    /* Retour */
    return;
}

void PreviSat::on_tempsReel_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */
    qInfo() << "Fonction" << __FUNCTION__ << ":" << checked;

    /* Corps de la methode */
    if (checked && (_chronometre != nullptr)) {
        _chronometre->setInterval(_ui->pasReel->currentText().toInt() * 1000);
        TempsReel();
        GestionTempsReel();
    }

    _ui->pasManuel->setVisible(!checked);
    _ui->valManuel->setVisible(!checked);
    _ui->pasReel->setVisible(checked);
    _ui->secondes->setVisible(checked);
    _ui->tempsReel->setChecked(checked);

    _onglets->general()->ui()->frameSimu->setVisible(!checked);
    _onglets->general()->ui()->dateHeure->setCurrentIndex(0);
    _onglets->osculateurs()->ui()->dateHeure->setCurrentIndex(0);

    /* Retour */
    return;
}

void PreviSat::on_modeManuel_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */
    qInfo() << "Fonction" << __FUNCTION__ << ":" << checked;

    /* Corps de la methode */
    if (checked) {

        if (!_onglets->general()->ui()->rewind->isEnabled() || !_onglets->general()->ui()->play->isEnabled()) {

            double pas = 0.;
            if (_ui->valManuel->currentIndex() < 3) {
                pas = _ui->pasManuel->currentText().toDouble() * qPow(DATE::NB_SEC_PAR_MIN, _ui->valManuel->currentIndex()) * DATE::NB_JOUR_PAR_SEC;
            } else {
                pas = _ui->pasManuel->currentText().toDouble();
            }

            _chronometre->setInterval(static_cast<int> (pas * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES) * 1000);

        } else if (!_onglets->general()->ui()->backward->isEnabled() || !_onglets->general()->ui()->forward->isEnabled()) {
            _chronometre->setInterval(1000);
        }
    }

    _ui->pasReel->setVisible(!checked);
    _ui->secondes->setVisible(!checked);
    _ui->pasManuel->setVisible(checked);
    _ui->valManuel->setVisible(checked);
    _ui->modeManuel->setChecked(checked);

    _onglets->general()->ui()->frameSimu->setVisible(checked);
    _onglets->general()->ui()->dateHeure->setCurrentIndex(1);
    _onglets->osculateurs()->ui()->dateHeure->setCurrentIndex(1);

    const bool etat1 = _onglets->general()->ui()->dateHeure2->blockSignals(true);
    const bool etat2 = _onglets->osculateurs()->ui()->dateHeure2->blockSignals(true);

    _onglets->general()->ui()->dateHeure2->setDateTime(_dateCourante->ToQDateTime(1));
    _onglets->osculateurs()->ui()->dateHeure2->setDateTime(_onglets->general()->ui()->dateHeure2->dateTime());

    _onglets->general()->ui()->dateHeure2->blockSignals(etat1);
    _onglets->osculateurs()->ui()->dateHeure2->blockSignals(etat2);

    /* Retour */
    return;
}

void PreviSat::on_pasReel_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->secondes->setText((index == 0) ? tr("seconde") : tr("secondes"));
    if (_chronometre != nullptr) {
        _chronometre->setInterval(_ui->pasReel->currentText().toInt() * 1000);
    }

    /* Retour */
    return;
}

void PreviSat::on_pasManuel_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const bool aindx = (index == 0);

    /* Corps de la methode */
    _ui->valManuel->setItemText(0, (aindx) ? tr("seconde") : tr("secondes"));
    _ui->valManuel->setItemText(1, (aindx) ? tr("minute") : tr("minutes"));
    _ui->valManuel->setItemText(2, (aindx) ? tr("heure") : tr("heures"));
    _ui->valManuel->setItemText(3, (aindx) ? tr("jour") : tr("jours"));

    /* Retour */
    return;
}


void PreviSat::on_zoomCarte_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->isCarteMaximisee()) {

        // Passage en affichage minimise
        _ui->zoomCarte->setToolTip(tr("Agrandir"));
        _ui->zoomCarte->setIcon(QIcon(":/resources/interface/maxi.png"));

        _ui->frameModeListe->setVisible(true);
        _ui->frameOngletsRadar->setVisible(true);

    } else {

        // Passage en affichage maximise
        _ui->zoomCarte->setToolTip(tr("Réduire"));
        _ui->zoomCarte->setIcon(QIcon(":/resources/interface/mini.png"));

        _ui->frameModeListe->setVisible(false);
        _ui->frameOngletsRadar->setVisible(false);
    }

    Configuration::instance()->isCarteMaximisee() = !Configuration::instance()->isCarteMaximisee();

    AffichageCartesRadar();

    /* Retour */
    return;
}

void PreviSat::on_changerCarte_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_isCarteMonde) {

        // Passage en carte du ciel
        _isCarteMonde = false;
        _ui->changerCarte->setToolTip(tr("Carte du monde"));
        _carte->setVisible(false);
        _ciel->setVisible(true);

    } else {

        // Passage en carte du monde
        _isCarteMonde = true;
        _ui->changerCarte->setToolTip(tr("Carte du ciel"));
        _carte->setVisible(true);
        _ciel->setVisible(false);
    }

    DemarrageApplication();

    /* Retour */
    return;
}

void PreviSat::on_issLive_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Configuration::instance()->issLive() = checked;
    settings.setValue("affichage/issLive", checked);
    GestionTempsReel();

    _ui->frameVideo->setVisible(checked);

    _coordISS->setVisible(checked);
    _gmt->setVisible(checked);

    if (_ui->modeManuel->isChecked()) {
        _radar->setVisible(!checked);
    }

    /* Retour */
    return;
}

void PreviSat::on_meteoBasesNasa_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + Configuration::instance()->dirLocalData() + QDir::separator() + "html" +
                                   QDir::separator() + "meteoNASA.html"));
}

void PreviSat::on_meteo_clicked()
{
    /* Declarations des variables locales */
    static QString map0;

    /* Initialisations */

    /* Corps de la methode */
    // Chargemment du fichier
    if (map0.isEmpty()) {

        const QString fic = Configuration::instance()->dirLocalData() + QDir::separator() + "html" + QDir::separator() + "meteo.map";
        QFile fi(fic);

        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                map0 = fi.readAll();
            }

            fi.close();
        }
    }

    // Remplacement des coordonnees
    const QString lon(QString::number(-Configuration::instance()->observateur().longitude() * MATHS::RAD2DEG));
    const QString lat(QString::number(Configuration::instance()->observateur().latitude() * MATHS::RAD2DEG));
    const QString map = map0.replace("LONGITUDE_CENTRE", lon).replace("LATITUDE_CENTRE", lat)
                            .replace("UNITE_TEMP", (_options->ui()->unitesKm->isChecked()) ? "C" : "F")
                            .replace("UNITE_VENT", (_options->ui()->unitesKm->isChecked()) ? "kmh" : "mph")
                            .replace("VALEUR_ZOOM", QString::number(_options->ui()->valeurZoomMap->value()));

    QFile fi2(Configuration::instance()->dirTmp() + QDir::separator() + "meteo.html");
    if (fi2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fi2);
        flux << map;
    }
    fi2.close();

    // Chargement de la meteo
    QDesktopServices::openUrl(QUrl("file:///" + fi2.fileName()));

    /* Retour */
    return;
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

        qInfo() << "Début Fonction" << __FUNCTION__;

        // Ouverture d'un fichier GP / TLE
        const QString fichier = QFileDialog::getOpenFileName(this, tr("Importer fichier GP / TLE..."),
                                                             QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                             tr("Fichiers GP (*.xml);;Fichiers TLE (*.txt *.tle);;Tous les fichiers (*.*)"));

        // Ouverture du fichier d'elements orbitaux
        if (!fichier.isEmpty()) {

            qInfo() << "Ouverture du fichier" << fichier;

            QFileInfo ff(fichier);
            if (ff.suffix() == "xml") {

                // Cas d'un fichier GP
                const QMap<QString, ElementsOrbitaux> mapElements =
                        GPFormat::LectureFichier(fichier, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec(),
                                                 QStringList(), true, true);

                if (mapElements.isEmpty()) {

                    qWarning() << QString("Le fichier GP %1 ne contient pas d'éléments orbitaux").arg(ff.fileName());
                    throw PreviSatException(tr("Le fichier %1 ne contient pas d'éléments orbitaux").arg(ff.fileName()), MessageType::WARNING);

                } else {

                    QFile fi(fichier);

                    if (fi.exists()) {

                        qWarning() << "Le fichier GP existe déjà";
                        throw PreviSatException(tr("Le fichier %1 existe déjà").arg(ff.fileName()), MessageType::WARNING);

                    } else {

                        // Le fichier contient des elements orbitaux, on le copie dans le repertoire d'elements orbitaux
                        if (fi.copy(Configuration::instance()->instance()->dirElem() + QDir::separator() + ff.fileName())) {

                            qInfo() << "Import du fichier GP" << ff.fileName() << "OK";

                            Configuration::instance()->InitListeFichiersElem();
                            InitFicGP();

                        } else {
                            qWarning() << "Import du fichier GP" << ff.fileName() << "KO";
                        }
                    }
                }

            } else {

                // Cas d'un fichier TLE
                const int nbElem = TLE::VerifieFichier(fichier);

                if (nbElem > 0) {

                    QFile fo(Configuration::instance()->instance()->dirElem() + QDir::separator() + ff.fileName());

                    if (fo.exists()) {

                        qWarning() << "Le fichier TLE existe déjà";
                        throw PreviSatException(tr("Le fichier %1 existe déjà").arg(ff.fileName()), MessageType::WARNING);

                    } else {

                        QFile fi(fichier);

                        // Le fichier contient des elements orbitaux, on le copie dans le repertoire d'elements orbitaux
                        if (fi.copy(fo.fileName())) {

                            qInfo() << "Import du fichier TLE" << ff.fileName() << "OK";

                            Configuration::instance()->InitListeFichiersElem();
                            InitFicGP();

                        } else {
                            qWarning() << "Import du fichier TLE" << ff.fileName() << "KO";
                        }
                    }

                } else {
                    qWarning() << QString("Le fichier TLE %1 ne contient pas d'éléments orbitaux").arg(ff.fileName());
                    throw PreviSatException(tr("Le fichier %1 ne contient pas d'éléments orbitaux").arg(ff.fileName()), MessageType::WARNING);
                }
            }
        }

    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

void PreviSat::on_actionEnregistrer_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Fonction" << __FUNCTION__;

    if (_ui->actionEnregistrer->isVisible() && (_onglets->currentIndex() < 3) && !_onglets->ui()->informationsStationSpatiale->isVisible()) {

        const QStringList listeNoms(QStringList() << tr("onglet_general", "file name (without accent)")
                                    << tr("onglet_elements", "file name (without accent)") << tr("onglet_informations", "file name (without accent)"));

        const QString nomFicDefaut = settings.value("fichier/sauvegarde", Configuration::instance()->dirOut()).toString() + QDir::separator() +
                listeNoms.at(_onglets->currentIndex()) + ".txt";

        const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomFicDefaut,
                                                             tr("Fichiers texte (*.txt);;Tous les fichiers (*.*)"));

        if (!fichier.isEmpty()) {

            switch (_onglets->currentIndex()) {
            case 0:
                // Sauvegarde de l'onglet General
                emit SauveOngletGeneral(fichier);
                break;

            case 1:
                // Sauvegarde de l'onglet Elements osculateurs
                emit SauveOngletElementsOsculateurs(fichier);
                break;

            case 2:
                // Sauvegarde de l'onglet Informations satellite ou Recherche satellite
                if (_onglets->ui()->informationsSat->isVisible()) {
                    emit SauveOngletInformations(fichier);
                } else if (_onglets->ui()->rechercheSat->isVisible()) {
                    emit SauveOngletRecherche(fichier);
                }
                break;

            default:
                break;
            }

            const QFileInfo fi(fichier);
            settings.setValue("fichier/sauvegarde", fi.absolutePath());
        }
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;


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
    qInfo() << "Début Fonction" << __FUNCTION__;

    const QUrl urlLastNews(QString("%1informations/").arg(DOMAIN_NAME) + "last_news_" + Configuration::instance()->locale() + ".html");

    if (Telechargement::UrlExiste(urlLastNews)) {

        _informations->setWindowModality(Qt::ApplicationModal);
        _informations->show();

        QFont fnt;
        fnt.setBold(false);
        _ui->actionInformations->setFont(fnt);

        if (!_majInfosDate.isEmpty()) {
            settings.setValue("temps/lastInfos", _majInfosDate);
        }

    } else if (!_majInfosDate.isEmpty()) {
        Message::Afficher(tr("Pas d'informations à afficher"), MessageType::INFO);
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

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

void PreviSat::on_actionMode_sombre_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    settings.setValue("affichage/modeSombre", _ui->actionMode_sombre->isChecked());
    _options->ui()->modeSombre->setChecked(_ui->actionMode_sombre->isChecked());

#if defined (Q_OS_WIN)
    qApp->setStyle("windowsvista");
#elif defined (Q_OS_LINUX)
    qApp->setStyle("Windows");
#elif defined (Q_OS_MAC)
    qApp->setStyle("macos");
#endif

    /* Corps de la methode */
    if (settings.value("affichage/modeSombre", false).toBool()) {

        // Pour l'affichage avec Qt6
        QFile fi(":/resources/interface/darkstyle.qss");
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

                // Activation du mode sombre (inspire de https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle)
                QPalette palette;
                palette.setColor(QPalette::Window, QColor(53, 53, 53));
                palette.setColor(QPalette::WindowText, Qt::white);
                palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));

                palette.setColor(QPalette::Base, QColor(42, 42, 42));
                palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));

                palette.setColor(QPalette::ToolTipBase, Qt::white);
                palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));

                palette.setColor(QPalette::Text, Qt::white);
                palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));

                palette.setColor(QPalette::Dark, QColor(35, 35, 35));
                palette.setColor(QPalette::Shadow, QColor(20, 20, 20));

                palette.setColor(QPalette::Button, QColor(53, 53, 53));
                palette.setColor(QPalette::ButtonText, Qt::white);
                palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(53, 53, 53));

                palette.setColor(QPalette::BrightText, Qt::red);

                palette.setColor(QPalette::Link, QColor(42, 130, 218));
                palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
                palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));

                palette.setColor(QPalette::HighlightedText, Qt::white);
                palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

                qApp->setPalette(palette);

                qApp->setStyleSheet(QString(fi.readAll()));
            }
        }

        fi.close();

    } else {

        // Palette par defaut
        qApp->setPalette(qApp->style()->standardPalette());
        qApp->setStyleSheet("QTabBar { qproperty-drawBase: 0; }");
    }

    GestionTempsReel();

    /* Retour */
    return;
}

void PreviSat::on_actionMettre_a_jour_GP_courant_triggered()
{
    MajFichierGP();
}

void PreviSat::on_actionMettre_a_jour_GP_communs_triggered()
{
    MettreAjourGroupeElem(tr("Commun", "common orbital elements groups"));
}

void PreviSat::on_actionMettre_a_jour_tous_les_groupes_de_GP_triggered()
{
    MettreAjourGroupeElem(tr("Tous", "all orbital elements groups"));
}

void PreviSat::on_actionTelecharger_la_mise_a_jour_triggered()
{
    QDesktopServices::openUrl(QUrl("https://sourceforge.net/projects/previsat/files/latest/download"));
}

void PreviSat::on_actionMettre_a_jour_les_fichiers_de_donnees_triggered()
{
    /* Declarations des variables locales */
    QFile fi;
    QFile fi2;

    /* Initialisations */
    qInfo() << "Début Fonction" << __FUNCTION__;

    Telechargement tel(Configuration::instance()->dirTmp());

    /* Corps de la methode */
    foreach (const QString &fic, Configuration::instance()->listeFicLocalData()) {

        if (!fic.contains("preferences", Qt::CaseInsensitive)) {

            const QString fichier = QString("%1data/%2").arg(DOMAIN_NAME).arg(fic).replace(QDir::separator(), "/");
            const QUrl url(fichier);
            tel.TelechargementFichier(url, false, false);

            fi.setFileName(Configuration::instance()->dirTmp() + QDir::separator() + QFileInfo(fic).fileName());

            if (fi.exists() && (fi.size() > 0)) {

                fi2.setFileName(Configuration::instance()->dirLocalData() + QDir::separator() + fic);
                fi2.remove();
                fi.rename(fi2.fileName());
            }
        }
    }

    Configuration::instance()->Chargement();

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

void PreviSat::on_actionExporter_fichier_log_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Ouverture de la fenêtre d'export de fichier log";

    Logging * const logging = new Logging(this);
    QEvent evt(QEvent::LanguageChange);

    logging->changeEvent(&evt);
    logging->setWindowModality(Qt::ApplicationModal);
    logging->show();
    logging->setVisible(true);

    /* Retour */
    return;
}

void PreviSat::on_actionPayPal_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QStringList listeDon = settings.value("fichier/dirHttpPreviDon", "").toString().split("\n", Qt::SkipEmptyParts);

    /* Corps de la methode */
    if (!listeDon.isEmpty()) {

        bool ok;
        const int idx = static_cast<int> (Configuration::instance()->listeFicLang().indexOf(settings.value("affichage/langue").toString()));

        QVector<QString> devises(QVector<QString>() << "Euro (€)" << "USD ($)" << "JPY (¥)");
        devises.resize(Configuration::instance()->listeFicLang().size());

        QInputDialog input(this);
        input.setWindowTitle(tr("Devise"));
        input.setLabelText(tr("Choisissez la devise :"));

        const QString item = input.getItem(this, tr("Devise"), tr("Choisissez la devise :"), devises.toList(), idx, false, &ok,
                                           Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

        if (ok && !item.isEmpty()) {
            QDesktopServices::openUrl(QUrl(listeDon.at(devises.indexOf(item))));
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionTipeee_triggered()
{
    Message::Afficher(tr("Attention : Il est possible d'effectuer un don PayPal via Tipeee, mais ceci induira des frais supplémentaires"),
                      MessageType::INFO);
    QDesktopServices::openUrl(QUrl("https://tipeee.com/previsat"));
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
    QDesktopServices::openUrl(QUrl("mailto:" + QString(MAILTO)));
}

void PreviSat::on_actionApropos_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Apropos * const apropos = new Apropos(this);
    QEvent evt(QEvent::LanguageChange);

    disconnect(apropos, &Apropos::VerifMajVersion, this, &PreviSat::VerifMajVersion);
    connect(apropos, &Apropos::VerifMajVersion, this, &PreviSat::VerifMajVersion);

    apropos->changeEvent(&evt);
    apropos->setWindowModality(Qt::ApplicationModal);
    apropos->show();
    apropos->setVisible(true);

    /* Retour */
    return;
}

void PreviSat::on_listeFichiersElem_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo fi(Configuration::instance()->nomfic());
    const int idx = static_cast<int> (Configuration::instance()->listeFichiersElem().indexOf(fi.fileName()));
    _ui->listeFichiersElem->setItemData(idx, QColor(Qt::white), Qt::BackgroundRole);

    /* Corps de la methode */
    Configuration::instance()->nomfic() = Configuration::instance()->listeFichiersElem().at(index);
    const QFileInfo ff(Configuration::instance()->nomfic());
    Configuration::instance()->listeSatellites().clear();
    _ui->listeFichiersElem->setItemData(index, QColor(Qt::gray), Qt::BackgroundRole);

    AfficherMessageStatut(tr("Ouverture du fichier d'éléments orbitaux %1 ...").arg(ff.fileName()));
    ChargementGP();
    AfficherMessageStatut(tr("Fichier d'éléments orbitaux de %1 satellites").arg(Configuration::instance()->mapElementsOrbitaux().size()), 5);

    const QString noradDefaut = Configuration::instance()->noradDefaut();
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();

    if (!Configuration::instance()->mapElementsOrbitaux().isEmpty()) {

        QStringListIterator it(Configuration::instance()->mapSatellitesFichierElem()[ff.fileName()]);
        while (it.hasNext()) {

            const QString norad = it.next();
            const ElementsOrbitaux elem = Configuration::instance()->mapElementsOrbitaux()[norad];

            if (norad == noradDefaut) {
                satellites.insert(0, Satellite(elem));
            } else {
                satellites.append(Satellite(elem));
            }
        }
    }

    _onglets->ReinitFlags();
    DeconnecterUdp();
    Configuration::instance()->notifAOS() = NotificationSonore::ATTENTE_LOS;

    GestionTempsReel();

    /* Retour */
    return;
}

void PreviSat::on_filtreSatellites_textChanged(const QString &arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!arg1.isEmpty()) {
        const bool etat = _ui->satellitesChoisis->blockSignals(true);
        _ui->satellitesChoisis->setChecked(false);
        _ui->satellitesChoisis->blockSignals(etat);
    }

    for(int i=0; i<_ui->listeSatellites->count(); i++) {
        const QString elem = _ui->listeSatellites->item(i)->text();
        _ui->listeSatellites->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }

    /* Retour */
    return;
}

void PreviSat::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    _ui->listeSatellites->sortItems();
    _ui->listeSatellites->scrollToItem(_ui->listeSatellites->currentItem(), QAbstractItemView::PositionAtTop);
}

void PreviSat::on_satellitesChoisis_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {
        _ui->filtreSatellites->clear();
        for(int i=0; i<_ui->listeSatellites->count(); i++) {
            const bool chk = !(_ui->listeSatellites->item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked);
            _ui->listeSatellites->item(i)->setHidden(chk);
        }
    } else {
        on_filtreSatellites_textChanged("");
        on_filtreSatellites_returnPressed();
    }

    /* Retour */
    return;
}

void PreviSat::on_listeSatellites_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->listeSatellites->hasFocus() && (_ui->listeSatellites->currentRow() >= 0)) {

        const QString norad = item->data(Qt::UserRole).toString();
        const ElementsOrbitaux elem = Configuration::instance()->mapElementsOrbitaux()[norad];

        if (item->checkState() == Qt::Checked) {

            // Suppression d'un satellite dans la liste
            item->setData(Qt::CheckStateRole, Qt::Unchecked);
            item->setData(Qt::BackgroundRole, QColor(Qt::transparent));

            QList<Satellite> &listeSatellites = Configuration::instance()->listeSatellites();
            const auto sat = std::find_if(listeSatellites.begin(), listeSatellites.end(), [norad](Satellite s) {
                    return (s.elementsOrbitaux().norad == norad);
            });

            if (sat != listeSatellites.end()) {
                listeSatellites.erase(sat);
                Configuration::instance()->SuppressionSatelliteFichierElem(norad);
            }

            if (!listeSatellites.isEmpty()) {

                bool atrouve = false;
                const QString noradDefaut = listeSatellites.first().elementsOrbitaux().norad;

                for(int j=0; j<_ui->listeSatellites->count() && !atrouve; j++) {
                    if (_ui->listeSatellites->item(j)->data(Qt::UserRole).toString() == noradDefaut) {
                        _ui->listeSatellites->item(j)->setData(Qt::BackgroundRole, QColor(160, 220, 240));
                        atrouve = true;
                    }
                }
            }

            Configuration::instance()->notifAOS() = NotificationSonore::ATTENTE_LOS;
            Configuration::instance()->notifFlashs() = NotificationSonore::ATTENTE_LOS;

        } else {

            // Ajout d'un satellite dans la liste
            item->setData(Qt::CheckStateRole, Qt::Checked);
            Configuration::instance()->listeSatellites().append(Satellite(elem));
            Configuration::instance()->AjoutSatelliteFichierElem(norad);
        }

        _onglets->ReinitFlags();
        emit DeconnecterUdp();

        // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
        EnchainementCalculs();

        // Affichage des donnees numeriques dans la barre d'onglets
        _onglets->show(*_dateCourante);
#if defined (Q_OS_WIN)
        _onglets->suiviTelescope()->CalculAosSatSuivi(*_dateCourante);
#endif

        _ui->issLive->setChecked(settings.value("affichage/issLive", false).toBool());

        AffichageCartesRadar();

        GestionnaireXml::EcritureConfiguration();
    }

    /* Retour */
    return;
}

void PreviSat::on_listeSatellites_itemEntered(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomsat = item->text();
    const QString norad = item->data(Qt::UserRole).toString();
    const QString cospar = item->toolTip().split(":").last().trimmed();

    /* Corps de la methode */
    if (nomsat != norad) {
        AfficherMessageStatut(tr("<b>%1</b> (numéro NORAD : <b>%2</b>  -  COSPAR : %3)").arg(nomsat).arg(norad).arg(cospar), 5);
    }

    /* Retour */
    return;
}

void PreviSat::on_listeSatellites_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    if (_ui->listeSatellites->currentRow() >= 0) {
        _ui->menuListeSatellites->exec(QCursor::pos());
    }

    /* Retour */
    return;
}

void PreviSat::on_actionDefinir_par_defaut_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    qInfo() << "Début Fonction" << __FUNCTION__;

    const QString norad = _ui->listeSatellites->currentItem()->data(Qt::UserRole).toString();
    const QFileInfo fi(Configuration::instance()->nomfic());
    const bool atrouve = Configuration::instance()->mapSatellitesFichierElem().value(fi.fileName()).contains(norad);
    const ElementsOrbitaux elem = Configuration::instance()->mapElementsOrbitaux()[norad];

    /* Corps de la methode */
    if (atrouve) {

        // Le satellite fait partie de la liste de satellites, on intervertit
        QList<Satellite> &listeSatellites = Configuration::instance()->listeSatellites();
        const auto sat = std::find_if(listeSatellites.begin(), listeSatellites.end(), [norad](Satellite s) {
                return (s.elementsOrbitaux().norad == norad);
        });

        if (sat != listeSatellites.end()) {
            const int idx = static_cast<int> (std::distance(listeSatellites.begin(), sat));
            Configuration::instance()->listeSatellites().swapItemsAt(idx, 0);
        }

    } else {

        // Le satellite choisi ne fait pas partie de la liste de satellites, on l'ajoute
        Configuration::instance()->listeSatellites().insert(0, Satellite(elem));
        Configuration::instance()->AjoutSatelliteFichierElem(norad);
    }

    qInfo() << "Numéro NORAD par défaut :" << norad;

    // On definit le satellite choisi comme satellite par defaut
    _ui->listeSatellites->currentItem()->setData(Qt::CheckStateRole, Qt::Checked);
    for(int i=0; i<_ui->listeSatellites->count(); i++) {
        _ui->listeSatellites->item(i)->setData(Qt::BackgroundRole, QColor(Qt::transparent));
    }

    _ui->listeSatellites->currentItem()->setData(Qt::BackgroundRole, QColor(160, 220, 240));
    _onglets->ReinitFlags();
    emit DeconnecterUdp();
    Configuration::instance()->notifAOS() = NotificationSonore::ATTENTE_LOS;

    GestionTempsReel();

    GestionnaireXml::EcritureConfiguration();

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

void PreviSat::on_lancementVideoNasa_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "Début Fonction" << __FUNCTION__;

        QString val;
        QStringListIterator it(Configuration::instance()->listeChainesNasa());
        while (it.hasNext()) {
            if (!val.isEmpty()) {
                val += ",";
            }
            val += "'" + it.next() + "'";
        }

        QString tab = QString("var tab = [ %1 ];").arg(val);

        static QString html0;
        if (html0.isEmpty()) {

            const QString ficHtml = Configuration::instance()->dirLocalData() + QDir::separator() + "html" + QDir::separator() + "chainesNASA.ht";
            QFile fi(ficHtml);

            if (fi.exists() && (fi.size() != 0)) {

                if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    html0 = fi.readAll();
                }

                fi.close();
            }
        }

        QString html = html0;
        html = html.replace("CHANNEL_MAX", QString::number(Configuration::instance()->listeChainesNasa().count())).replace("CHANNEL_TAB", tab);

        // Creation du fichier html
        QFile fr(Configuration::instance()->dirTmp() + QDir::separator() + "chainesNASA.html");
        if (fr.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream flux(&fr);
            flux << html;
        }
        fr.close();

        // Ouverture du fichier html
        QDesktopServices::openUrl(QUrl("file:///" + fr.fileName()));

    } catch (PreviSatException const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}
