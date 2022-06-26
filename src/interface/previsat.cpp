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
 * >    26 juin 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include "ui_onglets.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "ui_previsat.h"
#pragma GCC diagnostic warning "-Wconversion"
#include "ui_coordiss.h"
#include "apropos.h"
#include "carte.h"
#include "ciel.h"
#include "coordiss.h"
#include "informations.h"
#include "listwidgetitem.h"
#include "onglets.h"
#include "previsat.h"
#include "radar.h"
#include "configuration/configuration.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/systeme/decompression.h"


static bool isMaximise;

// Registre
static QSettings settings("Astropedia", "PreviSat");

// Couleurs GMT
static const QColor coulGmt[3] = { Qt::red, Qt::white, Qt::cyan };


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
    ui(new Ui::PreviSat)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    ui->setupUi(this);

    _chronometre = nullptr;
    _chronometreMs = nullptr;
    _dateCourante = nullptr;
    _messageStatut = nullptr;
    _messageStatut2 = nullptr;
    _messageStatut3 = nullptr;
    _modeFonctionnement = nullptr;
    _stsDate = nullptr;
    _stsHeure = nullptr;
    _timerStatut = nullptr;

    ui->lancementVideoNasa->installEventFilter(this);

    try {

        // Initialisation de la configuration generale
        Configuration::instance()->Initialisation();

        // Chargement des elements de la fenetre
        ChargementFenetre();

    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
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

    delete ui;
}


/*
 * Accesseurs
 */


/*
 * Methodes publiques
 */
/*******************
 * Initialisations *
 ******************/
/*
 * Chargement de la configuration
 */
void PreviSat::ChargementConfig()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Gestion de la police
    GestionPolice();

    // Affichage au demarrage
    InitAffichageDemarrage();

    // Verification des mises a jour au demarrage
    InitVerificationsMAJ();

    // Initialisation des menus
    InitMenus();

    // Barre de statut
    InitBarreStatut();

    // Valeur des champs par defaut
    InitChampsDefaut();

    // Liste des fichiers TLE
    InitFicTLE();

    ConnexionsSignauxSlots();
    _onglets->InitChargementOnglets();

    /* Retour */
    return;
}

/*
 * Chargement du fichier TLE par defaut
 */
void PreviSat::ChargementTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Lecture du fichier TLE par defaut
    try {

        QString nomfic = Configuration::instance()->nomfic();
        QFileInfo fi(nomfic);

        if (!Configuration::instance()->listeFicTLE().isEmpty() && !Configuration::instance()->listeFicTLE().contains(fi.fileName())) {
            const QString fic = Configuration::instance()->listeFicTLE().at(0);
            nomfic = (fic.contains(QDir::separator())) ? fic : Configuration::instance()->dirTle() + QDir::separator() + fic;
            fi = QFileInfo(nomfic);
        }

        if (fi.exists() && (fi.size() != 0)) {

            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = Configuration::instance()->dirTmp() + QDir::separator() + fi.completeBaseName();

                if (Decompression::DecompressionFichierGz(Configuration::instance()->nomfic())) {
                    nomfic = fic;
                    fi = QFileInfo(nomfic);
                } else {
                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                    throw PreviSatException(msg.arg(nomfic), WARNING);
                }
            }

            // Verification du fichier TLE
            if (!nomfic.contains(Configuration::instance()->dirTle())) {
                AfficherMessageStatut(tr("Vérification du fichier TLE %1 ...").arg(fi.fileName()));
                TLE::VerifieFichier(nomfic, true);
                AfficherMessageStatut(tr("Fichier TLE %1 OK").arg(fi.fileName()), 5);
            }

            // Lecture du fichier TLE en entier
            Configuration::instance()->setMapTLE(TLE::LectureFichier(nomfic, Configuration::instance()->donneesSatellites(),
                                                                     Configuration::instance()->lgRec()));

            // Mise a jour de la liste de satellites
            QStringList listeSatellites = Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()];

            QStringListIterator it(Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()]);

            while (it.hasNext()) {
                const QString norad = it.next();
                if (!Configuration::instance()->mapTLE().keys().contains(norad)) {
                    listeSatellites.removeOne(norad);
                }
            }

            Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()] = listeSatellites;

            if (!listeSatellites.isEmpty()
                    && Configuration::instance()->tleDefaut().nomsat.isEmpty()
                    && Configuration::instance()->tleDefaut().l1.isEmpty()
                    && Configuration::instance()->tleDefaut().l2.isEmpty()) {

                const TLE tle = Configuration::instance()->mapTLE()[listeSatellites.at(0)];
                Configuration::instance()->tleDefaut().nomsat = tle.nom();
                Configuration::instance()->tleDefaut().l1 = tle.ligne1();
                Configuration::instance()->tleDefaut().l2 = tle.ligne2();
                EcritureTleDefautRegistre();
            }

            // Affichage de la liste de satellites
            AfficherListeSatellites(fi.fileName());
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Mise a jour des TLE lors du demarrage
 */
void PreviSat::MAJTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */
    InitDate();

    /* Corps de la methode */
    // Mise a jour des TLE si necessaire
    if (Configuration::instance()->listeCategoriesTLE().size() > 0) {

        const bool ageMaxTLE = settings.value("temps/ageMaxTLE", true).toBool();
        if (ageMaxTLE) {

            const double lastUpdate = settings.value("temps/lastUpdate", 0.).toDouble();
            const int ageMax = settings.value("temps/ageMax", 15).toInt();
            const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);

            if ((fabs(_dateCourante->jourJulienUTC() - lastUpdate) > ageMax) ||
                    ((_dateCourante->jourJulienUTC() - Configuration::instance()->mapTLE()[noradDefaut].epoque().jourJulienUTC()) > ageMax)) {
                MajWebTLE();
                settings.setValue("temps/lastUpdate", _dateCourante->jourJulienUTC());
            }
        } else {
            emit AfficherMessageStatut(tr("Mise à jour automatique des TLE"), 10);
            MajWebTLE();
            settings.setValue("temps/lastUpdate", _dateCourante->jourJulienUTC());
        }
    } else {
        VerifAgeTLE();
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
    _onglets->AffichageLieuObs();

    if (Configuration::instance()->etoiles().isEmpty()) {
        Etoile::Initialisation(Configuration::instance()->dirCommonData(), Configuration::instance()->etoiles());
    }

    if (Configuration::instance()->constellations().isEmpty()) {
        Constellation::Initialisation(Configuration::instance()->dirCommonData(), Configuration::instance()->constellations());
        LigneConstellation::Initialisation(Configuration::instance()->dirCommonData());
    }

    const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
    const QFileInfo fi(Configuration::instance()->nomfic());

    if (!Configuration::instance()->mapTLE().isEmpty()) {

        QStringListIterator it(Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()]);
        while (it.hasNext()) {

            const QString norad = it.next();
            const TLE tle = Configuration::instance()->mapTLE()[norad];

            if (norad == noradDefaut) {
                satellites.insert(0, Satellite(tle));
            } else {
                satellites.append(Satellite(tle));
            }
        }
    }

    // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
    EnchainementCalculs();

    // Affichage des donnees numeriques dans la barre d'onglets
    _onglets->show(*_dateCourante);
#if defined (Q_OS_WIN)
    _onglets->CalculAosSatSuivi();
#endif

    ui->mccISS->setChecked(settings.value("affichage/mccISS", false).toBool());

    if (Configuration::instance()->isCarteMonde()) {

        // Affichage des courbes sur la carte du monde
        _carte->show();

    } else {

        // Affichage de la carte du ciel
        _ciel->show(Configuration::instance()->observateur(),
                    Configuration::instance()->soleil(),
                    Configuration::instance()->lune(),
                    Configuration::instance()->lignesCst(),
                    Configuration::instance()->constellations(),
                    Configuration::instance()->etoiles(),
                    Configuration::instance()->planetes(),
                    Configuration::instance()->listeSatellites());
    }

    // Affichage du radar
    if (_onglets->ui()->affradar->isChecked()) {
        _radar->show();
    }

    // Affichage de la fenetre d'informations
    const QUrl urlLastNews(QString("%1%2/Qt/informations/").arg(DOMAIN_NAME).arg(QString(APP_NAME).toLower())
                           + "last_news_" + Configuration::instance()->locale() + ".html");

    if (settings.value("affichage/informationsDemarrage", true).toBool() && Informations::UrlExiste(urlLastNews)) {
        on_actionInformations_triggered();
        const QRect tailleEcran = QApplication::desktop()->availableGeometry();
        infos->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, infos->size(), tailleEcran));
    }

    // Lancement du chronometre
    if (_chronometre == nullptr) {
        _chronometre = new QTimer(this);
        _chronometre->setInterval(ui->pasReel->currentText().toInt() * 1000);
        connect(_chronometre, SIGNAL(timeout()), this, SLOT(GestionTempsReel()));
        _chronometre->start();
    }

    // Lancement du chronometreMs
    if (_chronometreMs == nullptr) {
        _chronometreMs = new QTimer(this);
        _chronometreMs->setInterval(200);
        connect(_chronometreMs, SIGNAL(timeout()), this, SLOT(TempsReel()));
        _chronometreMs->start();
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

/*******************
 * Initialisations *
 ******************/
/*
 * Gestion de la police
 */
void PreviSat::GestionPolice()
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool afin = false;
    int taillePolice = ui->secondes->font().pointSize();
    const int tailleLabelInit = ui->secondes->width();

    /* Corps de la methode */
    QFont police;
#if defined (Q_OS_WIN)
    police.setFamily("MS Shell Dlg 2");
#elif defined (Q_OS_LINUX)
    police.setFamily("Sans Serif");
#elif defined (Q_OS_MAC)
    police.setFamily("Marion");
#endif

    while (!afin) {

        ui->secondes->adjustSize();
        const int tailleLabel = ui->secondes->width();

        if (tailleLabel > tailleLabelInit - 1) {
            taillePolice--;
            setStyleSheet(QString("font: %1pt").arg(taillePolice));
        } else {
            afin = true;
        }
    }

    police.setPointSize(taillePolice);
    setFont(police);
    Configuration::instance()->setPolice(police);

    const int index = settings.value("affichage/policeWCC", 0).toInt();
    const bool etat = _onglets->ui()->policeWCC->blockSignals(true);
    _onglets->ui()->policeWCC->clear();

#if defined (Q_OS_WIN)
    _onglets->ui()->policeWCC->addItem("Lucida Console");
    _onglets->ui()->policeWCC->addItem("MS Shell Dlg 2");

    const int taille = 10;
    QFont policeWcc(_onglets->ui()->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#elif defined (Q_OS_LINUX)
    _onglets->ui()->policeWCC->addItem("FreeSans");
    _onglets->ui()->policeWCC->addItem("Sans Serif");

    const int taille = 11;
    QFont policeWcc(_onglets->ui()->policeWCC->itemText(index), taille);

#elif defined (Q_OS_MAC)
    _onglets->ui()->policeWCC->addItem("Lucida Grande");
    _onglets->ui()->policeWCC->addItem("Marion");

    const int taille = 13;
    QFont policeWcc(_onglets->ui()->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#else
    const int taille = 11;
    QFont policeWcc(_onglets->ui()->policeWCC->itemText(index), taille);
#endif

    Configuration::instance()->setPoliceWcc(policeWcc);
    _onglets->ui()->policeWCC->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Affichage au demarrage
 */
void PreviSat::InitAffichageDemarrage() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();

    /* Corps de la methode */
    ui->actionOuvrir_fichier_TLE->setIcon(styleIcones->standardIcon(QStyle::SP_DirOpenIcon));
    ui->actionEnregistrer->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionInformations->setIcon(styleIcones->standardIcon(QStyle::SP_MessageBoxInformation));

    ui->pasManuel->setVisible(false);
    ui->valManuel->setVisible(false);

    auto lineEdit = ui->chaineNasa->findChild<QLineEdit*>();
    lineEdit->setReadOnly(true);
    lineEdit->setFocusPolicy(Qt::NoFocus);
    connect(ui->chaineNasa, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->chaineNasa,
            [&, lineEdit](){lineEdit->deselect();}, Qt::QueuedConnection);

    ui->frameVideo->setVisible(ui->mccISS->isChecked());
    _radar->setVisible(!ui->mccISS->isChecked());

    ui->frameLatGauche->setVisible(false);

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

    ui->barreStatut->addPermanentWidget(_messageStatut, 1);
    ui->barreStatut->addPermanentWidget(_messageStatut2);
    ui->barreStatut->addPermanentWidget(_messageStatut3);
    ui->barreStatut->addPermanentWidget(_modeFonctionnement);
    ui->barreStatut->addPermanentWidget(_stsDate);
    ui->barreStatut->addPermanentWidget(_stsHeure);

    /* Retour */
    return;
}

/*
 * Initialisation des champs par defaut
 */
void PreviSat::InitChampsDefaut()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_pasReel_currentIndexChanged(0);
    ui->pasReel->setCurrentIndex(settings.value("temps/pasreel", 1).toInt());

    on_pasManuel_currentIndexChanged(0);
    ui->pasManuel->setCurrentIndex(settings.value("temps/pasmanuel", 1).toInt());
    ui->valManuel->setCurrentIndex(settings.value("temps/valmanuel", 0).toInt());

    ui->chaineNasa->setMaximum(Configuration::instance()->listeChainesNasa().size());

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
    double offsetUTC = (_onglets->ui()->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    _onglets->ui()->updown->setValue(sgn(offsetUTC) * (static_cast<int>(fabs(offsetUTC) * NB_SEC_PAR_JOUR + EPS_DATES)));
    offsetUTC = (_onglets->ui()->heureLegale->isChecked()) ? _onglets->ui()->updown->value() * NB_JOUR_PAR_SEC : 0.;

    // Date et heure locales
    _dateCourante = new Date(offsetUTC);

    /* Retour */
    return;
}

/*
 * Initialisation des menus
 */
void PreviSat::InitMenus() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->barreMenu->setMenu(ui->menuPrincipal);
    ui->don->setMenu(ui->menuDon);
    ui->menuBar->setVisible(false);

    /* Retour */
    return;
}

/*
 * Verification des mises a jour (logiciel, fichiers internes)
 */
void PreviSat::InitVerificationsMAJ()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
#if defined (Q_OS_MAC)
    settings.setValue("affichage/verifMAJ", false);
    _onglets->ui()->verifMAJ->setVisible(false);
#else
    if (_onglets->ui()->verifMAJ->isChecked()) {
        VerifMAJPreviSat();
    }
#endif

#if !defined (Q_OS_WIN)
    if (settings.value("fichier/dirHttpPreviDon", "").toString().isEmpty()) {

        _onglets->setDirDwn(Configuration::instance()->dirTmp());

        const QString fic("don");
        _onglets->TelechargementFichier(QString("%1%2/Qt/").arg(DOMAIN_NAME).arg(QString(APP_NAME).toLower()) + fic, false);

        QFile fi(_onglets->dirDwn() + QDir::separator() + fic);
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                settings.setValue("fichier/dirHttpPreviDon", QString(fi.readLine()));
            }
            fi.close();
        }
    }
#endif

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

    ui->retranslateUi(this);
    _onglets->ui()->retranslateUi(_onglets);

    /* Retour */
    return;
}

/*
 * Ecriture du TLE par defaut en registre
 */
void PreviSat::EcritureTleDefautRegistre()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    settings.setValue("TLE/nom", Configuration::instance()->tleDefaut().nomsat);
    settings.setValue("TLE/l1", Configuration::instance()->tleDefaut().l1);
    settings.setValue("TLE/l2", Configuration::instance()->tleDefaut().l2);

    /* Retour */
    return;
}

/*
 * Enchainement des calculs
 */
void PreviSat::EnchainementCalculs()
{
    /* Declarations des variables locales */

    /* Initialisations */

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

        if (Configuration::instance()->isCarteMonde()) {

            // Coordonnees terrestres
            soleil.CalculCoordTerrestres(observateur);

            if (!isMaximise) {
                // Coordonnees equatoriales
                soleil.CalculCoordEquat(observateur);
            }
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

        // Calcul de la magnitude de la Lune
        lune.CalculMagnitude(soleil);

        if (Configuration::instance()->isCarteMonde()) {

            // Coordonnees terrestres
            lune.CalculCoordTerrestres(observateur);

            if (!isMaximise) {
                // Coordonnees equatoriales
                lune.CalculCoordEquat(observateur);
            }
        }


        if (!Configuration::instance()->isCarteMonde()) {

            /*
             * Calcul de la position des planetes
             */
            if (_onglets->ui()->affplanetes->checkState() != Qt::Unchecked) {

                QList<Planete> &planetes = Configuration::instance()->planetes();
                planetes.clear();
                for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

                    planetes.append(Planete(static_cast<IndicePlanete>(iplanete)));
                    planetes.last().CalculPosition(*_dateCourante, soleil);
                    planetes.last().CalculCoordHoriz(observateur);
                }
            }

            /*
             * Calcul de la position du catalogue d'etoiles
             */
            Etoile::CalculPositionEtoiles(observateur, Configuration::instance()->etoiles());
            if (_onglets->ui()->affconst->isChecked()) {
                Constellation::CalculConstellations(observateur, Configuration::instance()->constellations());
            }

            if (_onglets->ui()->affconst->checkState() != Qt::Unchecked) {
                LigneConstellation::CalculLignesCst(Configuration::instance()->etoiles(), Configuration::instance()->lignesCst());
            }
        }

        QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
        if (satellites.isEmpty()) {

            const QString titre = "%1 %2";
            setWindowTitle(titre.arg(qApp->applicationName()).arg(QString(APPVER_MAJ)));

        } else {

            const QString titre = "%1 %2 - %3";
            setWindowTitle(titre.arg(qApp->applicationName()).arg(QString(APPVER_MAJ)).arg(Configuration::instance()->tleDefaut().nomsat));

            // Calculs specifiques lors de l'affichage du Wall Command Center
            const bool mcc = ui->mccISS->isChecked();

            // Nombre de traces au sol a afficher
            int nbTraces = _onglets->ui()->nombreTrajectoires->value();

            if (satellites.isEmpty() || !_onglets->ui()->afftraj->isChecked()) {
                nbTraces = 1;
            } else {
                if (mcc && satellites.at(0).tle().norad() == Configuration::instance()->noradStationSpatiale()) {
                    nbTraces = 3;
                }
            }

            Satellite::CalculPosVitListeSatellites(*_dateCourante,
                                                   observateur,
                                                   soleil,
                                                   lune,
                                                   nbTraces,
                                                   _onglets->ui()->eclipsesLune->isChecked(),
                                                   _onglets->ui()->effetEclipsesMagnitude->isChecked(),
                                                   _onglets->ui()->extinctionAtmospherique->isChecked(),
                                                   _onglets->ui()->refractionAtmospherique->isChecked(),
                                                   _onglets->ui()->afftraceCiel->isChecked(),
                                                   _onglets->ui()->affvisib->isChecked(),
                                                   satellites.at(0).tle().norad() == Configuration::instance()->noradStationSpatiale(),
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
 * Mise a jour du fichier TLE courant
 */
void PreviSat::MajFichierTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo fi(Configuration::instance()->nomfic());

    /* Corps de la methode */
    if (QDir::toNativeSeparators(fi.absolutePath()) == Configuration::instance()->dirTle()) {

        connect(_onglets, SIGNAL(TelechargementFini()), _onglets, SLOT(FinTelechargementTle()));

        _messageStatut->setText(tr("Mise à jour du fichier TLE %1 en cours...").arg(fi.fileName()));
        const QString adresse = (fi.fileName().contains("spctrk")) ?
                    Configuration::instance()->adresseAstropedia() + "previsat/tle/" : Configuration::instance()->adresseCelestrakNorad();
        const QString ficMaj = adresse + fi.fileName();

        _onglets->setDirDwn(Configuration::instance()->dirTle());
        _onglets->TelechargementFichier(ficMaj);
        settings.setValue("temps/lastUpdate", _dateCourante->jourJulienUTC());
    }

    /* Retour */
    return;
}

/*
 * Mise a jour automatique des TLE
 */
void PreviSat::MajWebTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _onglets->setDirDwn(Configuration::instance()->dirTle());

    /* Corps de la methode */
    QListIterator<CategorieTLE> it(Configuration::instance()->listeCategoriesMajTLE());
    while (it.hasNext()) {

        const CategorieTLE categorie = it.next();
        QString adresse = categorie.site;

        if (adresse.contains("celestrak")) {
            adresse = Configuration::instance()->adresseCelestrakNorad();
        }

        if (adresse.contains("astropedia")) {
            adresse = Configuration::instance()->adresseAstropedia() + "previsat/tle/";
        }

        if (!adresse.endsWith("/")) {
            adresse.append("/");
        }

        foreach (const QString fic, categorie.fichiers) {
            const QString ficMaj = adresse + fic;
            _onglets->TelechargementFichier(ficMaj, false);
        }
    }

    /* Retour */
    return;
}

/*
 * Mettre a jour un groupe de TLE
 */
void PreviSat::MettreAJourGroupeTLE(const QString &groupe)
{
    _onglets->MettreAJourGroupeTLE(groupe);
}

void PreviSat::MiseAJourFichiers(QAction *action, const QString &nomMAJ)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    action->setVisible(true);

    const QString msg = tr("Une mise à jour %1 est disponible. Souhaitez-vous la télécharger?");

    QMessageBox msgbox(tr("Information"), msg.arg(nomMAJ), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::Yes) {

        if (action == ui->actionTelecharger_la_mise_a_jour) {
            on_actionTelecharger_la_mise_a_jour_triggered();
            ui->actionTelecharger_la_mise_a_jour->setVisible(false);
        }

        if (action == ui->actionMettre_jour_les_fichiers_de_donnees) {
            on_actionMettre_jour_les_fichiers_de_donnees_triggered();
            ui->actionMettre_jour_les_fichiers_de_donnees->setVisible(false);
        }
    }

    /* Retour */
    return;
}

/*
 * Ouverture d'un fichier TLE
 */
void PreviSat::OuvertureFichierTLE(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Configuration::instance()->nomfic() = fichier;
    const QFileInfo fi(fichier);
    AfficherMessageStatut(tr("Ouverture du fichier TLE %1 ...").arg(fi.fileName()));
    ChargementTLE();
    AfficherMessageStatut(tr("Fichier TLE de %1 satellites").arg(Configuration::instance()->mapTLE().size()), 5);

    /* Retour */
    return;
}

/*
 * Verification de l'age d'un TLE
 */
void PreviSat::VerifAgeTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->mapTLE().count() > 0) {

        const int ageMax = settings.value("temps/ageMax", 15).toInt();
        const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);

        if ((fabs(_dateCourante->jourJulienUTC() - Configuration::instance()->mapTLE()[noradDefaut].epoque().jourJulienUTC()) > ageMax) &&
                ui->tempsReel->isChecked()) {

            const QString msg = tr("Les éléments orbitaux sont plus vieux que %1 jour(s). Souhaitez-vous les mettre à jour?");

            QMessageBox msgbox(tr("Information"), msg.arg(ageMax), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                               QMessageBox::No, QMessageBox::NoButton, this);
            msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
            msgbox.setButtonText(QMessageBox::No, tr("Non"));
            msgbox.exec();

            const int res = msgbox.result();
            if (res == QMessageBox::Yes) {
                MajFichierTLE();
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Verification des mises a jour (logiciel, fichiers internes)
 */
void PreviSat::VerifMAJPreviSat()
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool anewVersion = false;
    _onglets->setDirDwn(Configuration::instance()->dirTmp());
    const QString dirHttpPrevi = QString("%1%2/Qt/").arg(DOMAIN_NAME).arg(QString(APP_NAME).toLower());
    const QStringList listeFic(QStringList () << "versionPreviSat" << "majFicInt" << "majInfos");

    /* Corps de la methode */
    foreach(QString fic, listeFic) {

        const QString ficMaj = dirHttpPrevi + fic;
        _onglets->TelechargementFichier(ficMaj, false);

        QString ligne;
        QFile fi(_onglets->dirDwn() + QDir::separator() + fic);
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream flux(&fi);
                ligne = flux.readLine();
            }
            fi.close();
        }

        if (!ligne.isEmpty()) {

            if (fic == "versionPreviSat") {

                const QStringList newVersion = ligne.split(".");
                const QStringList oldVersion = settings.value("fichier/version", "").toString().split(".");

                int inew[4], iold[4];
                for(int i=0; i<4; i++) {
                    inew[i] = newVersion.at(i).toInt();
                    iold[i] = oldVersion.at(i).toInt();
                }

                anewVersion = std::lexicographical_compare(iold, iold + 4, inew, inew + 4);

                if (anewVersion) {
                    MiseAJourFichiers(ui->actionTelecharger_la_mise_a_jour, tr("de %1", "for downloading PreviSat revision")
                                      .arg(QCoreApplication::applicationName()));
                    settings.setValue("fichier/majPrevi", "1");
                } else {
                    ui->actionTelecharger_la_mise_a_jour->setVisible(false);
                }

            } else if (((fic == "majFicInt") || (fic == "majInfos")) && !anewVersion) {

                const int an = ligne.mid(0, 4).toInt();
                const int mo = ligne.mid(5, 2).toInt();
                const int jo = ligne.mid(8, 2).toInt();

                const QDateTime dateHttp(QDate(an, mo, jo), QTime(0, 0, 0));

                if (fic == "majFicInt") {

                    QDateTime dateMax;
                    for(int i=0; i<Configuration::instance()->listeFicLocalData().size(); i++) {

                        const QString fichier = Configuration::instance()->listeFicLocalData().at(i);
                        if (!fichier.contains("preferences")) {

                            const QString fich = Configuration::instance()->dirLocalData() + QDir::separator() + fichier;
                            const QFileInfo fi2(fich);

                            if (fi2.lastModified().date() > dateMax.date()) {
                                dateMax.setDate(fi2.lastModified().date());
                            }
                        }
                    }

                    const bool anew = (dateHttp > dateMax);

                    if (anew && !ui->actionTelecharger_la_mise_a_jour->isVisible()) {
                        MiseAJourFichiers(ui->actionMettre_jour_les_fichiers_de_donnees, tr("des fichiers internes",
                                                                                            "for downloading internal files"));
                        settings.setValue("fichier/majPrevi", "1");
                    } else {
                        ui->actionMettre_jour_les_fichiers_de_donnees->setVisible(false);
                    }
                }

                if (fic == "majInfos") {

                    //majInfosDate = ligne;
                    QFont fnt;

                    if (settings.value("affichage/informationsDemarrage", true).toBool()) {
                        fnt.setBold(false);
                        ui->actionInformations->setFont(fnt);

                    } else {
                        const QDateTime lastInfos = QDateTime::fromString(settings.value("temps/lastInfos", "").toString(), "yyyy-MM-dd");
                        if (dateHttp > lastInfos) {
                            fnt.setBold(true);
                            ui->actionInformations->setFont(fnt);
                        }
                    }
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Mise a jour de l'affichage suite a un changement de carte
 */
void PreviSat::ChangementCarte()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->isCarteMonde()) {

        // Passage en carte du ciel
        Configuration::instance()->setIsCarteMonde(false);
        _affichageCiel->setToolTip(tr("Carte du monde"));
        ui->frameCarteGenerale->setVisible(false);
        ui->frameLon->setVisible(false);
        ui->frameCarteLon->setContentsMargins(0, 0, 20, 0);

        // Ciel
        if (_ciel != nullptr) {
            _ciel->deleteLater();
            _ciel = nullptr;
        }
        _ciel = new Ciel(_onglets, ui->frameCarteLon);

        connect(_ciel, SIGNAL(AfficherMessageStatut(const QString &, const int)), this, SLOT(AfficherMessageStatut(const QString &, const int)));
        connect(_ciel, SIGNAL(AfficherMessageStatut2(const QString &)), this, SLOT(AfficherMessageStatut2(const QString &)));
        connect(_ciel, SIGNAL(AfficherMessageStatut3(const QString &)), this, SLOT(AfficherMessageStatut3(const QString &)));
        connect(_ciel, SIGNAL(EffacerMessageStatut()), this, SLOT(EffacerMessageStatut()));
        connect(_ciel, SIGNAL(EcritureTleDefautRegistre()), this, SLOT(EcritureTleDefautRegistre()));
        connect(_ciel, SIGNAL(RecalculerPositions()), this, SLOT(GestionTempsReel()));

        // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
        EnchainementCalculs();

        // Affichage de la carte du ciel
        _ciel->show(Configuration::instance()->observateur(),
                    Configuration::instance()->soleil(),
                    Configuration::instance()->lune(),
                    Configuration::instance()->lignesCst(),
                    Configuration::instance()->constellations(),
                    Configuration::instance()->etoiles(),
                    Configuration::instance()->planetes(),
                    Configuration::instance()->listeSatellites());

    } else {
        // Passage en carte du monde
        Configuration::instance()->setIsCarteMonde(true);
        _affichageCiel->setToolTip(tr("Carte du ciel"));
        ui->frameCarteGenerale->setVisible(true);
        ui->frameLon->setVisible(true);
        ui->frameCarteLon->setContentsMargins(0, 0, 0, 0);

        if (_ciel != nullptr) {
            _ciel->deleteLater();
            _ciel = nullptr;
        }
    }

    /* Retour */
    return;
}

/*
 * Mise a jour de l'affichage suite a un changement de date
 */
void PreviSat::ChangementDate(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_dateCourante != nullptr) {
        delete _dateCourante;
        _dateCourante = nullptr;
    }

    _dateCourante = new Date(date.jourJulienUTC(), date.offsetUTC());
    ChangementDate(_dateCourante->ToQDateTime(1));

    /* Retour */
    return;
}

/*
 * Mise a jour de l'affichage suite a un changement de date
 */
void PreviSat::ChangementDate(const QDateTime &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Date datp(date.date().year(), date.date().month(), date.date().day(),
                    date.time().hour(), date.time().minute(), date.time().second(), 0.);

    /* Corps de la methode */
    const double offset = _dateCourante->offsetUTC();
    if (_dateCourante != nullptr) {
        delete _dateCourante;
        _dateCourante = nullptr;
    }
    _dateCourante = new Date(datp.jourJulienUTC() - offset, offset);

    if (ui->modeManuel->isChecked()) {
        _onglets->setInfo(true);
        _onglets->setAcalcDN(true);
        _onglets->setAcalcAOS(true);
    }

    _onglets->ui()->dateHeure3->setDateTime(date);
    _onglets->ui()->dateHeure4->setDateTime(date);

    // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
    EnchainementCalculs();

    // Affichage des donnees numeriques dans la barre d'onglets
    _onglets->show(*_dateCourante);

    if (Configuration::instance()->isCarteMonde()) {

        // Affichage des courbes sur la carte du monde
        _carte->show();

    } else {

        // Affichage de la carte du ciel
        _ciel->show(Configuration::instance()->observateur(),
                    Configuration::instance()->soleil(),
                    Configuration::instance()->lune(),
                    Configuration::instance()->lignesCst(),
                    Configuration::instance()->constellations(),
                    Configuration::instance()->etoiles(),
                    Configuration::instance()->planetes(),
                    Configuration::instance()->listeSatellites());
    }

    // Affichage du radar
    if (_onglets->ui()->affradar->isChecked()) {
        _radar->show();
    }

    /* Retour */
    return;
}

/*
 * Changement dynamique de la langue
 */
void PreviSat::ChangementLangue(const int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Configuration::instance()->locale() = Configuration::instance()->listeFicLang().at(index);

    /* Corps de la methode */
    ChargementTraduction(Configuration::instance()->locale());

    _onglets->setInfo(true);
    _onglets->RechargerListes();
    InitFicTLE();
    GestionTempsReel();
    settings.setValue("affichage/langue", Configuration::instance()->locale());

    /* Retour */
    return;
}

/*
 * Mise a jour de l'affichage suite a un changement de zoom
 */
void PreviSat::ChangementZoom()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (isMaximise) {

        // Passage en affichage minimise
        isMaximise = false;
        _maximise->setToolTip(tr("Agrandir"));
        _maximise->setIcon(QIcon(":/resources/maxi.png"));

        ui->layoutCarteLon->setContentsMargins(0, 0, 0, 0);
        ui->frameModeListe->setVisible(true);
        ui->frameOngletsRadar->setVisible(true);

    } else {
        // Passage en affichage maximise
        isMaximise = true;
        _maximise->setToolTip(tr("Réduire"));
        _maximise->setIcon(QIcon(":/resources/mini.png"));

        ui->layoutCarteLon->setContentsMargins(0, 0, 6, 0);
        ui->frameModeListe->setVisible(false);
        ui->frameOngletsRadar->setVisible(false);
    }

    resizeEvent(NULL);

    /* Retour */
    return;
}


/*
 * Liste des fichiers TLE
 */
void PreviSat::InitFicTLE() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        bool defaut = false;
        int idx = 0;
        QStringList listeTLE = Configuration::instance()->listeFicTLE();

        const bool etat = ui->listeFichiersTLE->blockSignals(true);
        ui->listeFichiersTLE->clear();
        QStringListIterator it(Configuration::instance()->listeFicTLE());
        while (it.hasNext()) {

            const QString nom = it.next();
            const QString fic = QDir::toNativeSeparators(Configuration::instance()->dirTle() + QDir::separator() + nom);
            if (TLE::VerifieFichier(fic) > 0) {

                ui->listeFichiersTLE->addItem(nom);

                if (fic == Configuration::instance()->nomfic()) {
                    const int index = ui->listeFichiersTLE->count() - 1;
                    ui->listeFichiersTLE->setCurrentIndex(index);
                    ui->listeFichiersTLE->setItemData(index, QColor(Qt::gray), Qt::BackgroundRole);
                }
            } else {
                // Suppression dans la liste des fichiers qui ne sont pas des TLE
                if (fic == Configuration::instance()->nomfic()) {
                    defaut = true;
                }
                listeTLE.removeAt(idx);
            }
            idx++;
        }

        if (listeTLE.count() == 0) {
            ui->listeFichiersTLE->addItem("");
        } else {
            if (defaut) {
                Configuration::instance()->nomfic() = Configuration::instance()->dirTle() + QDir::separator() + listeTLE.at(0);
            }
        }
        ui->listeFichiersTLE->blockSignals(etat);

        // Mise a jour de la liste de fichiers TLE
        Configuration::instance()->setListeFicTLE(listeTLE);

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}


/*************
 * Affichage *
 ************/
void PreviSat::AfficherCoordIssGmt()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (_gmt != nullptr) {
        delete _gmt;
        _gmt = nullptr;
    }

    /* Corps de la methode */
    if (Configuration::instance()->issLive()) {

        // Coordonnees ISS
        _coordISS->setPolice();
        if (_onglets->ui()->affNbOrbWCC->isChecked() && _onglets->ui()->affBetaWCC->isChecked()) {

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

            if (_onglets->ui()->affNbOrbWCC->isChecked()) {

                _coordISS->ui()->orbiteISS->move(112, 26);
                _coordISS->ui()->orbiteISS->setVisible(true);
                _coordISS->ui()->betaISS->setVisible(false);

            } else if (_onglets->ui()->affBetaWCC->isChecked()) {

                _coordISS->ui()->betaISS->move(112, 26);
                _coordISS->ui()->betaISS->setVisible(true);
                _coordISS->ui()->orbiteISS->setVisible(false);
            }
            _coordISS->resize(223, 46);
        }

        _coordISS->show(*_dateCourante, Onglets::dateEclipse());

        // GMT
        const QString chaine = "GMT = %1/%2:%3:%4";
        const Date date2 = Date(_dateCourante->annee(), 1, 1., 0.);
        const double jourDsAnnee = _dateCourante->jourJulienUTC() - date2.jourJulienUTC() + 1.;
        const int numJour = (int) jourDsAnnee;
        const int heure = (int) floor(NB_HEUR_PAR_JOUR * (jourDsAnnee - numJour) + 0.00005);
        const int min = _dateCourante->minutes();
        const int sec = (int) floor(_dateCourante->secondes());
        const QString texte =
                chaine.arg(numJour, 3, 10, QChar('0')).arg(heure, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));

        QPalette coul;
        coul.setColor(QPalette::WindowText, coulGmt[_onglets->ui()->coulGMT->currentIndex()]);
        _gmt = new QLabel(texte, _carte);
        _gmt->setGeometry(340, 30, 151, 16);
        _gmt->setPalette(coul);

        QFont police = Configuration::instance()->policeWcc();
        police.setPointSize(12);
        police.setBold(true);
        _gmt->setFont(police);
        _gmt->adjustSize();
        _gmt->show();
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

    /* Initialisations */
    ui->liste1->clear();
    ui->liste1->scrollToTop();
    _onglets->ui()->liste2->clear();
    _onglets->ui()->liste2->scrollToTop();
    _onglets->ui()->liste3->clear();
    _onglets->ui()->liste3->scrollToTop();
#if defined (Q_OS_WIN)
    _onglets->ui()->liste4->clear();
    _onglets->ui()->liste4->scrollToTop();
#endif
    const QStringList &listeSatellites = Configuration::instance()->mapSatellitesFicTLE()[nomfic];
    const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);

    /* Corps de la methode */
    QString nomsatComplet;
    ListWidgetItem * elem1;
    ListWidgetItem * elem2;
    ListWidgetItem * elem3;
    ListWidgetItem * elem4;

    QMapIterator<QString, TLE> it(Configuration::instance()->mapTLE());
    while (it.hasNext()) {
        it.next();

        const QString nomsat = it.value().nom().trimmed();
        const QString norad = it.key();
        const bool check = listeSatellites.contains(norad);
        const QString tooltip = tr("%1\nNORAD : %2\nCOSPAR : %3").arg(nomsat).arg(norad).arg(it.value().donnees().cospar());

        switch (_onglets->ui()->affNoradListes->checkState()) {
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

        // Liste principale de satellites
        elem1 = new ListWidgetItem(nomsatComplet, ui->liste1);
        elem1->setData(Qt::UserRole, norad);
        elem1->setToolTip(tooltip);
        elem1->setFlags(Qt::ItemIsEnabled);
        elem1->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
        if (norad == noradDefaut) {
            ui->liste1->setCurrentItem(elem1);
        }

        if (majListesOnglets) {

            // Liste pour les previsions de passage
            elem2 = new ListWidgetItem(nomsatComplet, _onglets->ui()->liste2);
            elem2->setData(Qt::UserRole, norad);
            elem2->setToolTip(tooltip);
            elem2->setFlags(Qt::ItemIsEnabled);
            elem2->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

            // Liste pour les evenements orbitaux
            elem3 = new ListWidgetItem(nomsatComplet, _onglets->ui()->liste3);
            elem3->setData(Qt::UserRole, norad);
            elem3->setToolTip(tooltip);
            elem3->setFlags(Qt::ItemIsEnabled);
            elem3->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

#if defined (Q_OS_WIN)
            // Liste pour le suivi de telescope
            elem4 = new ListWidgetItem(nomsatComplet, _onglets->ui()->liste4);
            elem4->setData(Qt::UserRole, norad);
            elem4->setToolTip(tooltip);
            elem4->setFlags(Qt::ItemIsEnabled);
            elem4->setCheckState(((noradDefaut == norad) && check) ? Qt::Checked : Qt::Unchecked);
#endif

            if (norad == noradDefaut) {
                _onglets->ui()->liste2->setCurrentItem(elem2);
                _onglets->ui()->liste3->setCurrentItem(elem3);
#if defined (Q_OS_WIN)
                _onglets->ui()->liste4->setCurrentItem(elem4);
#endif
            }
        }
    }

    ui->liste1->sortItems();
    ui->liste1->scrollToItem(ui->liste1->currentItem(), QAbstractItemView::PositionAtTop);
    _onglets->ui()->liste2->sortItems();
    _onglets->ui()->liste2->scrollToItem(_onglets->ui()->liste2->currentItem(), QAbstractItemView::PositionAtTop);
    _onglets->ui()->liste3->sortItems();
    _onglets->ui()->liste3->scrollToItem(_onglets->ui()->liste3->currentItem(), QAbstractItemView::PositionAtTop);
#if defined (Q_OS_WIN)
    _onglets->ui()->liste4->sortItems();
    _onglets->ui()->liste4->scrollToItem(_onglets->ui()->liste4->currentItem(), QAbstractItemView::PositionAtTop);
#endif

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
 * Chargement des elements de la fenetre
 */
void PreviSat::ChargementFenetre()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ciel = nullptr;
    _coordISS = nullptr;
    _gmt = nullptr;

    /* Corps de la methode */
    // TEMP
    setMaximumSize(minimumSize());
    ui->actionVision_nocturne->setVisible(false);
    //

    // Bouton pour maximiser la carte
    isMaximise = false;
    _maximise = new QToolButton(ui->frameCarteLon);
    _maximise->setVisible(false); // TEMP
    _maximise->setMaximumSize(20, 20);
    _maximise->setToolTip(tr("Agrandir"));
    _maximise->setIcon(QIcon(":/resources/maxi.png"));
    _maximise->setShortcut(QKeySequence::fromString("Ctrl+M"));
    _maximise->setAutoRaise(true);
    connect(_maximise, SIGNAL(clicked()), this, SLOT(ChangementZoom()));

    // Bouton pour passer de la carte du monde a la carte du ciel (et vice versa)
    Configuration::instance()->setIsCarteMonde(true);
    _affichageCiel = new QToolButton(ui->frameCarteLon);
    _affichageCiel->setMaximumSize(20, 20);
    _affichageCiel->setToolTip(tr("Carte du ciel"));
    _affichageCiel->setIcon(QIcon(":/resources/globe.png"));
    _affichageCiel->setShortcut(QKeySequence::fromString("F9"));
    _affichageCiel->setAutoRaise(true);
    connect(_affichageCiel, SIGNAL(clicked()), this, SLOT(ChangementCarte()));

    // Onglets
    _onglets = new Onglets(this);
    ui->layoutOnglets->addWidget(_onglets, 0, Qt::AlignVCenter);

    // Carte
    _carte = new Carte(_onglets, ui->frameCarteLon);
    ui->layoutCarte->addWidget(_carte);

    // Coordonnees de l'ISS
    _coordISS = new CoordISS(_carte);
    _coordISS->setVisible(false);

    // Radar
    _radar = new Radar(_onglets, this);
    ui->layoutRadar->addWidget(_radar);

    /* Retour */
    return;
}

void PreviSat::ConnexionsSignauxSlots()
{
    // Connexions signaux-slots
    connect(_onglets->ui()->langue, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangementLangue(const int)));
    connect(_onglets, SIGNAL(AffichageSiteLancement(const QString &, const Observateur &)),
            _carte, SLOT(AffichageSiteLancement(const QString &, const Observateur &)));
    connect(_onglets, SIGNAL(MiseAJourCarte()), _carte, SLOT(show()));
    connect(_onglets, SIGNAL(MiseAJourCarte()), _coordISS, SLOT(setPolice()));
    connect(_onglets, SIGNAL(MiseAJourCarte()), this, SLOT(AfficherCoordIssGmt()));
    connect(_onglets, SIGNAL(AfficherMessageStatut(const QString &, const int)), this, SLOT(AfficherMessageStatut(const QString &, const int)));
    connect(_onglets, SIGNAL(EffacerMessageStatut()), this, SLOT(EffacerMessageStatut()));

    connect(_onglets, SIGNAL(ModeManuel(bool)), this, SLOT(on_modeManuel_toggled(bool)));
    connect(_onglets, SIGNAL(ChangementDate(const QDateTime &)), this, SLOT(ChangementDate(const QDateTime &)));
    connect(_onglets, SIGNAL(ChangementDate(const Date &)), this, SLOT(ChangementDate(const Date &)));
    connect(_onglets, SIGNAL(RechargerTLE()), this, SLOT(ChargementTLE()));
    connect(_onglets, SIGNAL(RecalculerPositions()), this, SLOT(GestionTempsReel()));
    connect(_onglets, SIGNAL(InitFicTLE()), this, SLOT(InitFicTLE()));

    connect(_carte, SIGNAL(AfficherMessageStatut(const QString &, const int)), this, SLOT(AfficherMessageStatut(const QString &, const int)));
    connect(_carte, SIGNAL(AfficherMessageStatut2(const QString &)), this, SLOT(AfficherMessageStatut2(const QString &)));
    connect(_carte, SIGNAL(AfficherMessageStatut3(const QString &)), this, SLOT(AfficherMessageStatut3(const QString &)));
    connect(_carte, SIGNAL(EffacerMessageStatut()), this, SLOT(EffacerMessageStatut()));
    connect(_carte, SIGNAL(EcritureTleDefautRegistre()), this, SLOT(EcritureTleDefautRegistre()));
    connect(_carte, SIGNAL(RecalculerPositions()), this, SLOT(GestionTempsReel()));

    connect(_radar, SIGNAL(AfficherMessageStatut(const QString &, const int)), this, SLOT(AfficherMessageStatut(const QString &, const int)));
    connect(_radar, SIGNAL(AfficherMessageStatut2(const QString &)), this, SLOT(AfficherMessageStatut2(const QString &)));
    connect(_radar, SIGNAL(AfficherMessageStatut3(const QString &)), this, SLOT(AfficherMessageStatut3(const QString &)));
    connect(_radar, SIGNAL(EffacerMessageStatut()), this, SLOT(EffacerMessageStatut()));
    connect(_radar, SIGNAL(EcritureTleDefautRegistre()), this, SLOT(EcritureTleDefautRegistre()));
    connect(_radar, SIGNAL(RecalculerPositions()), this, SLOT(GestionTempsReel()));

    ChargementTraduction(settings.value("affichage/langue", "en").toString());

    if (settings.value("fichier/sauvegarde").toString().isEmpty()) {
        settings.setValue("fichier/sauvegarde", Configuration::instance()->dirOut());
    }

    settings.setValue("fichier/path", Configuration::instance()->dirExe());
    settings.setValue("fichier/version", QString(APPVERSION));
    settings.setValue("affichage/flagIntensiteVision", false);
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
 * Gestion du temps reel
 */
void PreviSat::GestionTempsReel()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->tempsReel->isChecked()) {

        // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
        EnchainementCalculs();

        // Affichage des donnees numeriques dans la barre d'onglets
        _onglets->show(*_dateCourante);

        if (Configuration::instance()->isCarteMonde()) {

            // Affichage des courbes sur la carte du monde
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
                        Configuration::instance()->listeSatellites());
        }

        // Affichage du radar
        if (_onglets->ui()->affradar->isChecked()) {
            _radar->show();
        }
        _radar->setVisible(!ui->mccISS->isChecked());

    }

    if (ui->modeManuel->isChecked()) {

        _onglets->setInfo(true);
        _onglets->setAcalcDN(true);
        _onglets->setAcalcAOS(true);

        if (_onglets->ui()->pause->isEnabled()) {

            if (!ui->pasManuel->view()->isVisible()) {

                double pas;
                if (ui->valManuel->currentIndex() < 3) {
                    pas = ui->pasManuel->currentText().toDouble() * qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) * NB_JOUR_PAR_SEC;
                } else {
                    pas = ui->pasManuel->currentText().toDouble();
                }

                double jd = _dateCourante->jourJulienUTC();
                if (!_onglets->ui()->rewind->isEnabled() || !_onglets->ui()->backward->isEnabled()) {
                    jd -= pas;
                }
                if (!_onglets->ui()->play->isEnabled() || !_onglets->ui()->forward->isEnabled()) {
                    jd += pas;
                }

                const double offset = _dateCourante->offsetUTC();
                if (_dateCourante != nullptr) {
                    delete _dateCourante;
                    _dateCourante = nullptr;
                }
                _dateCourante = new Date(jd, offset);

                // Enchainement de l'ensemble des calculs
                EnchainementCalculs();

                _onglets->show(*_dateCourante);
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage en temps reel
 */
void PreviSat::TempsReel()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->tempsReel->isChecked()) {

        // Date actuelle
        const double offset = _dateCourante->offsetUTC();
        if (_dateCourante != nullptr) {
            delete _dateCourante;
            _dateCourante = nullptr;
        }

        _dateCourante = new Date(offset);
        _modeFonctionnement->setText(tr("Temps réel"));

    } else {
        _modeFonctionnement->setText(tr("Mode manuel"));
    }

    if (_onglets->ui()->calJulien->isChecked()) {

        // Affichage du jour julien dans la barre de statut
        const Date date1(_dateCourante->annee(), 1, 1, 0.);
        _stsDate->setText(QString::number(_dateCourante->jourJulien() + TJ2000, 'f', 5));
        _stsHeure->setText(QString::number(_dateCourante->jourJulien() - date1.jourJulien() + 1., 'f', 5));
        _stsDate->setToolTip(tr("Jour julien"));
        _stsHeure->setToolTip(tr("Jour"));

    } else {

        // Affichage de la date et l'heure dans la barre de statut
        const QDateTime d = _dateCourante->ToQDateTime(1);
        _stsDate->setText(d.toString(tr("dd/MM/yyyy", "date format")));
        _stsHeure->setText(d.toString("hh:mm:ss") + ((_onglets->ui()->syst12h->isChecked()) ? "a" : ""));
        _stsDate->setToolTip(tr("Date"));
        _stsHeure->setToolTip(tr("Heure"));
    }

    /* Retour */
    return;
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
        if (!(_onglets->ui()->verifMAJ->isChecked() && (fic == "versionPreviSat" || fic == "majFicInt"))) {
            QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + fic);
            fi.remove();
        }
    }

    // Sauvegarde des donnees du logiciel
    settings.setValue("temps/valManuel", ui->valManuel->currentIndex());
    settings.setValue("temps/pasManuel", ui->pasManuel->currentIndex());
    settings.setValue("temps/pasReel", ui->pasReel->currentIndex());
    settings.setValue("temps/dtu", _onglets->ui()->updown->value() * NB_JOUR_PAR_SEC);

    settings.setValue("affichage/affSAA", _onglets->ui()->affSAA->isChecked());
    settings.setValue("affichage/affconst", _onglets->ui()->affconst->checkState());
    settings.setValue("affichage/affcoord", _onglets->ui()->affcoord->isChecked());
    settings.setValue("affichage/affetoiles", _onglets->ui()->affetoiles->isChecked());
    settings.setValue("affichage/affgrille", _onglets->ui()->affgrille->isChecked());
    settings.setValue("affichage/afficone", _onglets->ui()->afficone->isChecked());
    settings.setValue("affichage/affinvew", _onglets->ui()->affinvew->isChecked());
    settings.setValue("affichage/affinvns", _onglets->ui()->affinvns->isChecked());
    settings.setValue("affichage/afflune", _onglets->ui()->afflune->isChecked());
    settings.setValue("affichage/affnomlieu", _onglets->ui()->affnomlieu->checkState());
    settings.setValue("affichage/affnomsat", _onglets->ui()->affnomsat->checkState());
    settings.setValue("affichage/affnotif", _onglets->ui()->affnotif->isChecked());
    settings.setValue("affichage/affnuit", _onglets->ui()->affnuit->isChecked());
    settings.setValue("affichage/affphaselune", _onglets->ui()->affphaselune->isChecked());
    settings.setValue("affichage/affplanetes", _onglets->ui()->affplanetes->checkState());
    settings.setValue("affichage/affradar", _onglets->ui()->affradar->checkState());
    settings.setValue("affichage/affsoleil", _onglets->ui()->affsoleil->isChecked());
    settings.setValue("affichage/afftraceCiel", _onglets->ui()->afftraceCiel->isChecked());
    settings.setValue("affichage/afftraj", _onglets->ui()->afftraj->isChecked());
    settings.setValue("affichage/affvisib", _onglets->ui()->affvisib->checkState());
    settings.setValue("affichage/calJulien", _onglets->ui()->calJulien->isChecked());
    settings.setValue("affichage/eclipsesLune", _onglets->ui()->eclipsesLune->isChecked());
    settings.setValue("affichage/effetEclipsesMagnitude", _onglets->ui()->effetEclipsesMagnitude->isChecked());
    settings.setValue("affichage/extinction", _onglets->ui()->extinctionAtmospherique->isChecked());
    settings.setValue("affichage/fenetreMax", isMaximized());
    settings.setValue("affichage/groupeTLE", _onglets->ui()->groupeTLE->currentIndex());
    settings.setValue("affichage/hauteur", height());
    settings.setValue("affichage/intensiteOmbre", _onglets->ui()->intensiteOmbre->value());
    settings.setValue("affichage/intensiteVision", _onglets->ui()->intensiteVision->value());
    settings.setValue("affichage/largeur", width());
    settings.setValue("affichage/magnitudeEtoiles", _onglets->ui()->magnitudeEtoiles->value());
    settings.setValue("affichage/nombreTrajectoires", _onglets->ui()->nombreTrajectoires->value());
    settings.setValue("affichage/proportionsCarte", _onglets->ui()->proportionsCarte->isChecked());
    settings.setValue("affichage/refractionAtmospherique", _onglets->ui()->refractionAtmospherique->isChecked());
    settings.setValue("affichage/rotationIconeISS", _onglets->ui()->rotationIconeISS->isChecked());
    settings.setValue("affichage/affNoradListes", _onglets->ui()->affNoradListes->isChecked());
    settings.setValue("affichage/rotationLune", _onglets->ui()->rotationLune->isChecked());
    settings.setValue("affichage/systemeHoraire", _onglets->ui()->syst24h->isChecked());
    settings.setValue("affichage/typeParametres", _onglets->ui()->typeParametres->currentIndex());
    settings.setValue("affichage/typeRepere", _onglets->ui()->typeRepere->currentIndex());
    settings.setValue("affichage/unite", _onglets->ui()->unitesKm->isChecked());
    settings.setValue("affichage/utc", _onglets->ui()->utc->isChecked());
    settings.setValue("affichage/utcAuto", _onglets->ui()->utcAuto->isChecked());
    settings.setValue("affichage/valeurZoomMap", _onglets->ui()->valeurZoomMap->value());
    settings.setValue("affichage/verifMAJ", _onglets->ui()->verifMAJ->isChecked());

    settings.setValue("affichage/mccISS", ui->mccISS->isChecked());
    settings.setValue("affichage/affBetaWCC", _onglets->ui()->affBetaWCC->isChecked());
    settings.setValue("affichage/affCerclesAcq", _onglets->ui()->affCerclesAcq->isChecked());
    settings.setValue("affichage/affNbOrbWCC", _onglets->ui()->affNbOrbWCC->isChecked());
    settings.setValue("affichage/affSAA_ZOE", _onglets->ui()->affSAA_ZOE->isChecked());
    settings.setValue("affichage/chaine", ui->chaineNasa->value());
    settings.setValue("affichage/styleWCC", _onglets->ui()->styleWCC->isChecked());
    settings.setValue("affichage/coulGMT", _onglets->ui()->coulGMT->currentIndex());
    settings.setValue("affichage/coulZOE", _onglets->ui()->coulZOE->currentIndex());
    settings.setValue("affichage/coulCercleVisibilite", _onglets->ui()->coulCercleVisibilite->currentIndex());
    settings.setValue("affichage/coulEquateur", _onglets->ui()->coulEquateur->currentIndex());
    settings.setValue("affichage/coulTerminateur", _onglets->ui()->coulTerminateur->currentIndex());
    settings.setValue("affichage/policeWCC", _onglets->ui()->policeWCC->currentIndex());

    for(int i=0; i<_onglets->ui()->listeStations->count(); i++) {
        settings.setValue("affichage/station" + _onglets->ui()->listeStations->item(i)->data(Qt::UserRole).toString(),
                          _onglets->ui()->listeStations->item(i)->checkState());
    }

    settings.setValue("fichier/listeMap", (_onglets->ui()->listeMap->currentIndex() > 0) ?
                          Configuration::instance()->listeFicMap().at(qMax(0, _onglets->ui()->listeMap->currentIndex() - 1)) : "");
    //    settings.setValue("fichier/listeSon", (_onglets->ui()->listeSons->currentIndex() > 0) ?
    //                          ficSonAOS.at(qMax(0, ui->listeSons->currentIndex() - 1)) : "");
    settings.setValue("fichier/nom", QDir::toNativeSeparators(Configuration::instance()->nomfic()));
    settings.setValue("fichier/fichierAMettreAJour", _onglets->ui()->fichierAMettreAJour->text());
    settings.setValue("fichier/fichierALire", _onglets->ui()->fichierALire->text());
    settings.setValue("fichier/affichageMsgMAJ", _onglets->ui()->affichageMsgMAJ->currentIndex());
    settings.setValue("fichier/fichierALireCreerTLE", _onglets->ui()->fichierALireCreerTLE->text());
    settings.setValue("fichier/nomFichierPerso", _onglets->ui()->nomFichierPerso->text());

    settings.setValue("previsions/pasGeneration", _onglets->ui()->pasGeneration->currentIndex());
    settings.setValue("previsions/lieuxObservation2", _onglets->ui()->lieuxObservation2->currentIndex());
    settings.setValue("previsions/hauteurSatPrev", _onglets->ui()->hauteurSatPrev->currentIndex());
    settings.setValue("previsions/valHauteurSatPrev", _onglets->ui()->valHauteurSatPrev->text().toInt());
    settings.setValue("previsions/hauteurSoleilPrev", _onglets->ui()->hauteurSoleilPrev->currentIndex());
    settings.setValue("previsions/valHauteurSoleilPrev", _onglets->ui()->valHauteurSoleilPrev->text().toInt());
    settings.setValue("previsions/illuminationPrev", _onglets->ui()->illuminationPrev->isChecked());
    settings.setValue("previsions/magnitudeMaxPrev", _onglets->ui()->magnitudeMaxPrev->isChecked());

    settings.setValue("previsions/passageApogee", _onglets->ui()->passageApogee->isChecked());
    settings.setValue("previsions/passageNoeuds", _onglets->ui()->passageNoeuds->isChecked());
    settings.setValue("previsions/passageOmbre", _onglets->ui()->passageOmbre->isChecked());
    settings.setValue("previsions/passageQuadrangles", _onglets->ui()->passageQuadrangles->isChecked());
    settings.setValue("previsions/transitionJourNuit", _onglets->ui()->transitionJourNuit->isChecked());

    settings.setValue("previsions/hauteurSatTransit", _onglets->ui()->hauteurSatTransit->currentIndex());
    settings.setValue("previsions/valHauteurSatTransit", _onglets->ui()->valHauteurSatTransit->text().toInt());
    settings.setValue("previsions/lieuxObservation4", _onglets->ui()->lieuxObservation4->currentIndex());
    settings.setValue("previsions/ageMaxTLETransit", _onglets->ui()->ageMaxTLETransit->value());
    settings.setValue("previsions/elongationMaxCorps", _onglets->ui()->elongationMaxCorps->value());
    settings.setValue("previsions/calcTransitLunaireJour", _onglets->ui()->calcTransitLunaireJour->isChecked());

    settings.setValue("previsions/hauteurSatMetOp", _onglets->ui()->hauteurSatMetOp->currentIndex());
    settings.setValue("previsions/hauteurSoleilMetOp", _onglets->ui()->hauteurSoleilMetOp->currentIndex());
    settings.setValue("previsions/lieuxObservation5", _onglets->ui()->lieuxObservation5->currentIndex());
    settings.setValue("previsions/ordreChronologiqueMetOp", _onglets->ui()->ordreChronologiqueMetOp->isChecked());
    settings.setValue("previsions/magnitudeMaxMetOp", _onglets->ui()->magnitudeMaxMetOp->value());

#if defined (Q_OS_WIN)
    settings.setValue("previsions/hauteurSatSuivi", _onglets->ui()->hauteurSatSuivi->currentIndex());
    settings.setValue("previsions/pasSuivi", _onglets->ui()->pasSuivi->value());
#endif

    if (!_onglets->ui()->verifMAJ->isChecked()) {
        settings.setValue("fichier/majPrevi", "0");
    }

    Configuration::instance()->EcritureConfiguration();

    if (_onglets->ui()->preferences->currentIndex() < _onglets->ui()->preferences->count() - 2) {
        const QString fichierPref = Configuration::instance()->dirPrf() + QDir::separator() +
                Configuration::instance()->listeFicPref().at(_onglets->ui()->preferences->currentIndex());
        _onglets->SauvePreferences(fichierPref);
    }

    if (_carte != nullptr) {
        delete _carte;
        _carte = nullptr;
    }

    if (_ciel != nullptr) {
        delete _ciel;
        _ciel = nullptr;
    }

    if (_radar != nullptr) {
        delete _radar;
        _radar = nullptr;
    }

    if (_maximise != nullptr) {
        delete _maximise;
        _maximise = nullptr;
    }

    if (_affichageCiel != nullptr) {
        delete _affichageCiel;
        _affichageCiel = nullptr;
    }

    if (_messageStatut != nullptr) {
        delete _messageStatut;
        _messageStatut = nullptr;
    }

    if (_messageStatut2 != nullptr) {
        delete _messageStatut2;
        _messageStatut2 = nullptr;
    }

    if (_messageStatut3 != nullptr) {
        delete _messageStatut3;
        _messageStatut3 = nullptr;
    }

    if (_modeFonctionnement != nullptr) {
        delete _modeFonctionnement;
        _modeFonctionnement = nullptr;
    }

    if (_stsDate != nullptr) {
        delete _stsDate;
        _stsDate = nullptr;
    }

    if (_stsHeure != nullptr) {
        delete _stsHeure;
        _stsHeure = nullptr;
    }

    if (_dateCourante != nullptr) {
        delete _dateCourante;
        _dateCourante = nullptr;
    }

    if (_chronometre != nullptr) {
        delete _chronometre;
        _chronometre = nullptr;
    }

    if (_chronometreMs != nullptr) {
        delete _chronometreMs;
        _chronometreMs = nullptr;
    }

    if (_timerStatut != nullptr) {
        delete _timerStatut;
        _timerStatut = nullptr;
    }

    /* Retour */
    return;
}

bool PreviSat::eventFilter(QObject *object, QEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (evt->type() == QEvent::MouseMove) {

        if (ui->lancementVideoNasa->underMouse()) {
            setCursor(Qt::PointingHandCursor);
        }
    } else if (evt->type() == QEvent::Leave) {
        setCursor(Qt::ArrowCursor);
    }

    /* Retour */
    return QMainWindow::eventFilter(object, evt);
}

void PreviSat::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (_modeFonctionnement->underMouse()) {

        if (ui->tempsReel->isChecked()) {
            ui->modeManuel->setChecked(true);
        } else {
            ui->tempsReel->setChecked(true);
        }

        GestionTempsReel();
    }

    if (_stsDate->underMouse() || _stsHeure->underMouse()) {
        _onglets->ui()->calJulien->setChecked(!_onglets->ui()->calJulien->isChecked());
        TempsReel();
    }

    /* Retour */
    return;
}

void PreviSat::keyPressEvent(QKeyEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const Qt::Key etapeAvant = Qt::Key_F6;
    const Qt::Key etapeApres = Qt::Key_F7;

#else
    const Qt::Key etapeAvant = Qt::Key_F11;
    const Qt::Key etapeApres = Qt::Key_F12;
#endif

    /* Corps de la methode */
    if (evt->key() == Qt::Key_F10) {

        // Bascule Temps reel/Mode manuel
        if (ui->tempsReel->isChecked()) {
            ui->modeManuel->setChecked(true);

        } else if (ui->modeManuel->isChecked()) {
            ui->tempsReel->setChecked(true);
        }

    } else if (evt->key() == Qt::Key_F8) {

        // Capture de la fenetre
        _chronometre->stop();
        _chronometreMs->stop();
        const QPixmap image = QPixmap::grabWidget(QApplication::activeWindow());

        const QString nomFicDefaut = Configuration::instance()->dirOut() + QDir::separator() + "previsat_" +
                _dateCourante->ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").remove(":").replace(" ", "_") + "_" +
                _onglets->ui()->tuc->text().remove(" ").remove(":");

        const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomFicDefaut,
                                                         tr("Fichiers PNG (*.png);;Fichiers JPEG (*.jpg);;Fichiers BMP (*.bmp);;" \
                                                            "Tous les fichiers (*)"));

        if (!fic.isEmpty()) {
            image.save(fic);
            const QFileInfo fi(fic);
            settings.setValue("fichier/sauvegarde", fi.absolutePath());
        }

        _chronometre->start();
        _chronometreMs->start();

    } else if ((evt->key() == etapeAvant) || (evt->key() == etapeApres)) {

        // Etape precedente/suivante (mode manuel)
        if (!ui->modeManuel->isChecked()) {
            ui->modeManuel->setChecked(true);
        }

        const int sgnk = (evt->key() == etapeAvant) ? -1 : 1;

        const double jd = (ui->valManuel->currentIndex() < 3) ? _dateCourante->jourJulienUTC() +
                                                                sgnk * ui->pasManuel->currentText().toDouble() *
                                                                qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) * NB_JOUR_PAR_SEC :
                                                                _dateCourante->jourJulienUTC() + sgnk * ui->pasManuel->currentText().toDouble();

        const Date date(jd + EPS_DATES, _dateCourante->offsetUTC());
        const QString fmt = tr("dddd dd MMMM yyyy  hh:mm:ss") + ((_onglets->ui()->syst12h->isChecked()) ? "a" : "");

        if (_onglets->ui()->dateHeure4->isVisible()) {
            _onglets->ui()->dateHeure4->setDisplayFormat(fmt);
            _onglets->ui()->dateHeure4->setDateTime(date.ToQDateTime(1));
        } else {
            _onglets->ui()->dateHeure3->setDisplayFormat(fmt);
            _onglets->ui()->dateHeure3->setDateTime(date.ToQDateTime(1));
            _onglets->ui()->dateHeure3->setFocus();
        }

    } else {
        QMainWindow::keyPressEvent(evt);
    }

    /* Retour */
    return;
}

void PreviSat::resizeEvent(QResizeEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (Configuration::instance()->isCarteMonde()) {

        if (_onglets->ui()->proportionsCarte->isChecked()) {
            ui->layoutCarte->setEnabled(false);
        }
        _carte->resizeEvent(evt);

        ui->frameCarteGenerale->setGeometry(ui->frameCarteGenerale->x(), ui->frameCarteGenerale->y(), ui->frameCarteGenerale->width(),
                                            _carte->height());
        ui->frameCarteLon->setGeometry(ui->frameCarteLon->x(), ui->frameCarteLon->y(), ui->frameCarteLon->width(),
                                       _carte->height());

        const double hcarte6 = _carte->height() / 6.;
        ui->S60->move(5, static_cast<int> (5 * hcarte6) - 6);
        ui->S30->move(5, static_cast<int> (4 * hcarte6) - 6);
        ui->SS->move(5, static_cast<int> (3.5 * hcarte6) - 7);
        ui->N0->move(5, static_cast<int> (3 * hcarte6) - 7);
        ui->NN->move(5, static_cast<int> (2.5 * hcarte6) - 7);
        ui->N30->move(5, static_cast<int> (2 * hcarte6) - 7);
        ui->N60->move(5, static_cast<int> (hcarte6) - 7);

        const double lcarte12 = _carte->width() / 12.;
        ui->W150->move(static_cast<int> (lcarte12) - 7, 0);
        ui->W120->move(static_cast<int> (2. * lcarte12) - 7, 0);
        ui->W90->move(static_cast<int> (3. * lcarte12) - 5, 0);
        ui->W60->move(static_cast<int> (4. * lcarte12) - 5, 0);
        ui->W30->move(static_cast<int> (5. * lcarte12) - 5, 0);
        ui->WW->move(static_cast<int> (5.5 * lcarte12) - 3, 0);
        ui->W0->move(static_cast<int> (6. * lcarte12) - 1, 0);
        ui->EE->move(static_cast<int> (6.5 * lcarte12) - 3, 0);
        ui->E30->move(static_cast<int> (7. * lcarte12) - 5, 0);
        ui->E60->move(static_cast<int> (8. * lcarte12) - 5, 0);
        ui->E90->move(static_cast<int> (9. * lcarte12) - 5, 0);
        ui->E120->move(static_cast<int> (10. * lcarte12) - 7, 0);
        ui->E150->move(static_cast<int> (11. * lcarte12) - 7, 0);

    } else {
        _ciel->resizeEvent(evt);
    }

    if (isMaximise) {
        _maximise->setGeometry(ui->centralWidget->width() - 30, 0, 20, 20);
        _affichageCiel->setGeometry(ui->centralWidget->width() - 30, 28, 20, 20);

    } else {
        _maximise->setGeometry(qMax(815, ui->frameCarteLon->width() - 19), 0, 20, 20);
        _affichageCiel->setGeometry(qMax(815, ui->frameCarteLon->width() - 19), 28, 20, 20);
    }

    // TEMP
    _affichageCiel->setGeometry(qMax(815, ui->frameCarteLon->width() - 19), 0, 20, 20);

    /* Retour */
    return;
}

void PreviSat::on_directHelp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString ficDox = QString("%1/%2/html/index.html").arg(Configuration::instance()->dirDox()).arg(Configuration::instance()->locale());

    /* Corps de la methode */
    if (!QDesktopServices::openUrl("file:///" + ficDox)) {
        Message::Afficher(tr("Impossible d'afficher l'aide en ligne"), WARNING);
    }

    /* Retour */
    return;
}

void PreviSat::on_actionPaypal_triggered()
{
    on_actionFaire_triggered();
}

void PreviSat::on_actionTipeee_triggered()
{
    Message::Afficher(tr("Attention : Il est possible d'effectuer un don PayPal via Tipeee, mais ceci induira des frais supplémentaires"), INFO);
    QDesktopServices::openUrl(QUrl("https://tipeee.com/previsat"));
}

void PreviSat::on_actionUtip_triggered()
{
    Message::Afficher(tr("Attention : Il est possible d'effectuer un don PayPal via Utip, mais ceci induira des frais supplémentaires"), INFO);
    QDesktopServices::openUrl(QUrl("https://utip.io/previsat/"));
}

void PreviSat::on_mccISS_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Configuration::instance()->issLive() = checked;
    GestionTempsReel();
    _carte->show();
    _radar->setVisible(!checked);
    ui->frameVideo->setVisible(checked);
    if (checked) {
        AfficherCoordIssGmt();
    }
    _coordISS->setVisible(checked);
    _gmt->setVisible(checked);

    /* Retour */
    return;
}

void PreviSat::on_meteoBasesNASA_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + Configuration::instance()->dirLocalData() + QDir::separator() + "html" +
                                   QDir::separator() + "meteoNASA.html"));
}

void PreviSat::on_meteo_clicked()
{
    /* Declarations des variables locales */
    QString map;

    /* Initialisations */

    /* Corps de la methode */
    // Affichage de la map
    const QString fic = Configuration::instance()->dirLocalData() + QDir::separator() + "html" + QDir::separator() + "meteo.map";
    QFile fi(fic);

    if (fi.exists() && (fi.size() != 0)) {
        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream flux(&fi);
            map = flux.readAll();
        }
        fi.close();
    }


    const QString lon(QString::number(-Configuration::instance()->observateur().longitude() * RAD2DEG));
    const QString lat(QString::number(Configuration::instance()->observateur().latitude() * RAD2DEG));
    map = map.replace("LONGITUDE_CENTRE", lon).replace("LATITUDE_CENTRE", lat)
            .replace("UNITE_TEMP", (_onglets->ui()->unitesKm->isChecked()) ? "C" : "F")
            .replace("UNITE_VENT", (_onglets->ui()->unitesKm->isChecked()) ? "kmh" : "mph")
            .replace("VALEUR_ZOOM", QString::number(_onglets->ui()->valeurZoomMap->value()));

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
void PreviSat::on_actionOuvrir_fichier_TLE_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Ouverture d'un fichier TLE
        const QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                             settings.value("fichier/repTLE", Configuration::instance()->dirTle()).toString(),
                                                             tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;" \
                                                                "Fichiers gz (*.gz)"));

        if (fichier.isEmpty()) {
            if (!ui->listeFichiersTLE->currentText().isEmpty()) {
                ui->listeFichiersTLE->setCurrentIndex(Configuration::instance()->listeFicTLE().indexOf(Configuration::instance()->nomfic()));
            }
        } else {
            OuvertureFichierTLE(fichier);
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
    if (ui->actionEnregistrer->isVisible() && (_onglets->ui()->barreOnglets->currentIndex() < 3)) {

        const QStringList listeNoms(QStringList() << tr("onglet_general", "file name (without accent)")
                                    << tr("onglet_elements", "file name (without accent)") << tr("onglet_informations", "file name (without accent)"));

        const QString nomFicDefaut = settings.value("fichier/sauvegarde", Configuration::instance()->dirOut()).toString() + QDir::separator() +
                listeNoms.at(_onglets->ui()->barreOnglets->currentIndex()) + ".txt";

        const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomFicDefaut,
                                                             tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));

        if (!fichier.isEmpty()) {
            switch (_onglets->ui()->barreOnglets->currentIndex()) {
            case 0:
                // Sauvegarde de l'onglet General
                _onglets->SauveOngletGeneral(fichier);
                break;

            case 1:
                // Sauvegarde de l'onglet Elements osculateurs
                _onglets->SauveOngletElementsOsculateurs(fichier);
                break;

            case 2:
                // Sauvegarde de l'onglet Informations satellite
                _onglets->SauveOngletInformations(fichier);
                break;

            default:
                break;
            }

            const QFileInfo fi(fichier);
            settings.setValue("fichier/sauvegarde", fi.absolutePath());
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionImprimer_carte_triggered()
{
    /* Declarations des variables locales */
    QPrinter printer;

    /* Initialisations */

    /* Corps de la methode */
    printer.setOrientation((_carte->isVisible()) ? QPrinter::Landscape : QPrinter::Portrait);
    QPrintDialog dial(&printer, this);

    if (dial.exec() == QDialog::Accepted) {

        printer.newPage();
        QPainter p(&printer);

        QRect rect;

        if (_carte->isVisible()) {
            rect = _carte->rect();
            rect.moveTo(6, 6);
        } else {
            rect = _ciel->rect();
            rect.moveTo((ui->frameCarteLon->width() - _ciel->width()) / 2 - 20, 6);
        }

        const QPixmap pixmap = QWidget::grab(rect);
        const QPixmap pixscl = (pixmap.width() > printer.pageRect().width()) ? pixmap.scaledToWidth(printer.pageRect().width()) : pixmap;
        const int xPrt = (pixscl.width() == pixmap.width()) ? (printer.pageRect().width() - pixscl.width()) / 2 : 50;

        p.drawPixmap(xPrt, 50, pixscl);
        p.end();
    }

    /* Retour */
    return;
}


void PreviSat::on_actionMettre_jour_TLE_courant_triggered()
{
    MajFichierTLE();
}

void PreviSat::on_actionMettre_jour_groupe_TLE_triggered()
{
    _onglets->on_majMaintenant_clicked();
}

void PreviSat::on_actionMettre_jour_TLE_communs_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Mise a jour des groupes de TLE communs
    _messageStatut->setText(tr("Mise à jour des fichiers TLE communs..."));
    MettreAJourGroupeTLE(tr("commun", "do not translate for now"));

    /* Retour */
    return;
}

void PreviSat::on_actionMettre_jour_tous_les_groupes_de_TLE_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Mise a jour de tous les groupes de TLE
    _messageStatut->setText(tr("Mise à jour des groupes de TLE en cours..."));
    MettreAJourGroupeTLE(tr("tous", "do not translate for now"));

    /* Retour */
    return;
}

void PreviSat::on_actionMettre_jour_les_fichiers_de_donnees_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _onglets->setDirDwn(Configuration::instance()->dirLocalData());
    if (_messageStatut != nullptr) {
        _messageStatut->setText(tr("Mise à jour du fichier de données en cours..."));
        connect(_onglets, SIGNAL(TelechargementFini()), _onglets, SLOT(FinTelechargementDonnees()));
    }

    foreach(QString fic, Configuration::instance()->listeFicLocalData()) {

        if (!fic.contains("gestionnaireTLE") && !fic.contains("preferences")) {
            const QString ficMaj = Configuration::instance()->adresseAstropedia() + "previsat/Qt/commun/data/" + fic.replace("\\", "/");
            _onglets->TelechargementFichier(ficMaj, false);
        }
    }

    QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + "majFicInt");
    if (fi.exists()) {
        fi.remove();
    }

    ui->actionMettre_jour_les_fichiers_de_donnees->setVisible(false);
    settings.setValue("fichier/majPrevi", "0");

    /* Retour */
    return;
}

void PreviSat::on_actionFichier_d_aide_triggered()
{
    on_directHelp_clicked();
}

void PreviSat::on_actionInformations_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QUrl urlLastNews(QString("%1%2/Qt/informations/").arg(DOMAIN_NAME).arg(QString(APP_NAME).toLower())
                           + "last_news_" + Configuration::instance()->locale() + ".html");

    if (Informations::UrlExiste(urlLastNews)) {

        infos = new Informations(this, _onglets);
        infos->setWindowModality(Qt::ApplicationModal);
        infos->show();

        QFont fnt;
        fnt.setBold(false);
        ui->actionInformations->setFont(fnt);

        if (!_majInfosDate.isEmpty()) {
            settings.setValue("temps/lastInfos", _majInfosDate);
        }

    } else {
        if (!_majInfosDate.isEmpty()) {
            Message::Afficher(tr("Pas d'informations à afficher"), INFO);
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionTelecharger_la_mise_a_jour_triggered()
{
    QDesktopServices::openUrl(QUrl("https://sourceforge.net/projects/previsat/files/latest/download"));
}

void PreviSat::on_actionFaire_triggered()
{
    QDesktopServices::openUrl(QUrl(settings.value("fichier/dirHttpPreviDon", "").toString()));
}

void PreviSat::on_actionDonation_Tipeee_triggered()
{
    on_actionTipeee_triggered();
}

void PreviSat::on_actionDonation_Utip_triggered()
{
    on_actionUtip_triggered();
}

void PreviSat::on_actionPrevisat_sourceforge_net_triggered()
{
    QDesktopServices::openUrl(QUrl("http://previsat.sourceforge.net/"));
}

void PreviSat::on_actionRapport_de_bug_triggered()
{
    QDesktopServices::openUrl(QUrl(Configuration::instance()->adresseAstropedia() + "rapport_" + Configuration::instance()->locale() + ".html"));
}

void PreviSat::on_actionSkywatcher_com_triggered()
{
    QDesktopServices::openUrl(QUrl("http://skywatcher.com/"));
}

void PreviSat::on_actionWww_celestrak_com_triggered()
{
    QDesktopServices::openUrl(QUrl(Configuration::instance()->adresseCelestrak()));
}

void PreviSat::on_actionWww_space_track_org_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.space-track.org"));
}

void PreviSat::on_actionA_propos_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Apropos * const apropos = new Apropos(this);
    apropos->setWindowModality(Qt::ApplicationModal);
    apropos->show();

    /* Retour */
    return;
}

void PreviSat::on_tempsReel_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {

        // Positionnement de date actuelle et enchainement des calculs
        _chronometre->setInterval(ui->pasReel->currentText().toInt() * 1000);

        TempsReel();
        GestionTempsReel();

        ui->pasManuel->setVisible(false);
        ui->valManuel->setVisible(false);

        _onglets->ui()->dateHeure1->setVisible(true);
        _onglets->ui()->dateHeure2->setVisible(true);
        _onglets->ui()->dateHeure3->setVisible(false);
        _onglets->ui()->dateHeure4->setVisible(false);
        _onglets->ui()->utcManuel->setVisible(false);
        _onglets->ui()->utcManuel2->setVisible(false);
        _onglets->ui()->frameSimu->setVisible(false);
//        if (_onglets->ui()->pause->isEnabled()) {
//            _onglets->on_pause_clicked();
//        }

        on_pasReel_currentIndexChanged(ui->pasReel->currentIndex());
        ui->pasReel->setVisible(true);
        ui->secondes->setVisible(true);

        _onglets->setAcalcDN(true);
        _onglets->setAcalcAOS(true);

        // TODO
        //CalculAgeTLETransitISS();
    }

    /* Retour */
    return;
}

void PreviSat::on_modeManuel_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {

        if (!_onglets->ui()->rewind->isEnabled() || !_onglets->ui()->play->isEnabled()) {

            double pas = 0.;
            if (ui->valManuel->currentIndex() < 3) {
                pas = ui->pasManuel->currentText().toDouble() * qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) * NB_JOUR_PAR_SEC;
            } else {
                pas = ui->pasManuel->currentText().toDouble();
            }
            _chronometre->setInterval(static_cast<int> (pas * NB_SEC_PAR_JOUR + EPS_DATES) * 1000);

        } else if (!_onglets->ui()->backward->isEnabled() || !_onglets->ui()->forward->isEnabled()) {
            _chronometre->setInterval(0);
        }

        ui->modeManuel->setChecked(true);
        ui->pasReel->setVisible(false);
        ui->secondes->setVisible(false);
        on_pasManuel_currentIndexChanged(ui->pasManuel->currentIndex());
        ui->pasManuel->setVisible(true);
        ui->valManuel->setVisible(true);

        const QString fmt = tr("dddd dd MMMM yyyy  hh:mm:ss", "date format") + ((_onglets->ui()->syst12h->isChecked()) ? "a" : "");
        _onglets->ui()->dateHeure3->setDateTime((_onglets->ui()->utc->isChecked()) ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
        _onglets->ui()->dateHeure3->setDisplayFormat(fmt);
        _onglets->ui()->dateHeure3->setVisible(true);
        if (_onglets->ui()->general->isVisible()) {
            _onglets->ui()->dateHeure3->setFocus();
        }

        _onglets->ui()->dateHeure4->setDateTime((_onglets->ui()->utc->isChecked()) ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
        _onglets->ui()->dateHeure4->setDisplayFormat(fmt);
        _onglets->ui()->dateHeure4->setVisible(true);
        if (_onglets->ui()->osculateurs->isVisible()) {
            _onglets->ui()->dateHeure4->setFocus();
        }

        _onglets->ui()->dateHeure1->setVisible(false);
        _onglets->ui()->dateHeure2->setVisible(false);

        _onglets->ui()->utcManuel->setVisible(true);
        _onglets->ui()->utcManuel2->setVisible(true);
        //_onglets->ui()->frameSimu->setVisible(true);
        //ui->pasManuel->setFocus();

        _onglets->setAcalcDN(true);
        _onglets->setAcalcAOS(true);

        // TODO
        //        notifAOS = false;
        //        notifLOS = false;
        //        notifFlash = false;
    }

    /* Retour */
    return;
}

void PreviSat::on_pasReel_currentIndexChanged(int index)
{
    ui->secondes->setText((index == 0) ? tr("seconde") : tr("secondes"));
}

void PreviSat::on_pasManuel_currentIndexChanged(int index)
{
    const bool aindx = (index == 0);
    ui->valManuel->setItemText(0, (aindx) ? tr("seconde") : tr("secondes"));
    ui->valManuel->setItemText(1, (aindx) ? tr("minute") : tr("minutes"));
    ui->valManuel->setItemText(2, (aindx) ? tr("heure") : tr("heures"));
    ui->valManuel->setItemText(3, (aindx) ? tr("jour") : tr("jours"));
}

void PreviSat::on_listeFichiersTLE_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo fi(Configuration::instance()->nomfic());
    const int idx = Configuration::instance()->listeFicTLE().indexOf(fi.fileName());
    ui->listeFichiersTLE->setItemData(idx, QColor(Qt::white), Qt::BackgroundRole);

    /* Corps de la methode */
    Configuration::instance()->nomfic() = Configuration::instance()->dirTle() + QDir::separator() + Configuration::instance()->listeFicTLE().at(index);
    Configuration::instance()->listeSatellites().clear();
    ui->listeFichiersTLE->setItemData(index, QColor(Qt::gray), Qt::BackgroundRole);

    AfficherMessageStatut(tr("Ouverture du fichier TLE %1 ...").arg(fi.fileName()));
    ChargementTLE();
    AfficherMessageStatut(tr("Fichier TLE de %1 satellites").arg(Configuration::instance()->mapTLE().size()), 5);

    const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
    const QFileInfo ff(Configuration::instance()->nomfic());

    if (!Configuration::instance()->mapTLE().isEmpty()) {

        QStringListIterator it(Configuration::instance()->mapSatellitesFicTLE()[ff.fileName()]);
        while (it.hasNext()) {

            const QString norad = it.next();
            const TLE tle = Configuration::instance()->mapTLE()[norad];

            if (norad == noradDefaut) {
                satellites.insert(0, Satellite(tle));
            } else {
                satellites.append(Satellite(tle));
            }
        }
    }

    GestionTempsReel();

    /* Retour */
    return;
}

void PreviSat::on_liste1_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste1->hasFocus() && (ui->liste1->currentRow() >= 0)) {

        const QString norad = item->data(Qt::UserRole).toString();
        const TLE tle = Configuration::instance()->mapTLE()[norad];

        if (ui->liste1->currentItem()->checkState() == Qt::Checked) {

            // Suppression d'un satellite dans la liste
            ui->liste1->currentItem()->setCheckState(Qt::Unchecked);

            QList<Satellite> &listeSatellites = Configuration::instance()->listeSatellites();
            bool afin = false;
            int i;
            for(i = 0; i < listeSatellites.size() && !afin; i++) {
                afin = (listeSatellites.at(i).tle().norad() == norad);
            }

            listeSatellites.removeAt(--i);
            Configuration::instance()->suppressionSatelliteFicTLE(norad);

            if ((i == 0) && !listeSatellites.isEmpty()) {
                // On definit le satellite suivant dans la liste comme satellite par defaut
                Configuration::instance()->tleDefaut().nomsat = listeSatellites.at(0).tle().nom();
                Configuration::instance()->tleDefaut().l1 = listeSatellites.at(0).tle().ligne1();
                Configuration::instance()->tleDefaut().l2 = listeSatellites.at(0).tle().ligne2();
                EcritureTleDefautRegistre();

            } else if (listeSatellites.isEmpty()) {
                settings.setValue("TLE/nom", "");
                settings.setValue("TLE/l1", "");
                settings.setValue("TLE/l2", "");
            }

        } else {

            // Ajout d'un satellite dans la liste
            ui->liste1->currentItem()->setCheckState(Qt::Checked);
            Configuration::instance()->listeSatellites().append(Satellite(tle));
            Configuration::instance()->ajoutSatelliteFicTLE(norad);
        }

        GestionTempsReel();
        Configuration::instance()->EcritureConfiguration();
    }

    /* Retour */
    return;
}

void PreviSat::on_liste1_itemEntered(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomsat = item->text();
    const QString norad = item->data(Qt::UserRole).toString();
    const QString cospar = item->toolTip().split("\n").last();

    /* Corps de la methode */
    if (nomsat != norad) {
        AfficherMessageStatut(tr("%1 (numéro NORAD : %2  -  %3)").arg(nomsat).arg(norad).arg(cospar), 5);
    }

    /* Retour */
    return;
}

void PreviSat::on_liste1_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    if (ui->liste1->currentRow() >= 0) {
        ui->menuContextuelListe1->exec(QCursor::pos());
    }

    /* Retour */
    return;
}

void PreviSat::on_actionDefinir_par_defaut_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString norad = ui->liste1->currentItem()->data(Qt::UserRole).toString();
    const QFileInfo fi(Configuration::instance()->nomfic());
    const bool atrouve = Configuration::instance()->mapSatellitesFicTLE().value(fi.fileName()).contains(norad);
    const TLE tle = Configuration::instance()->mapTLE()[norad];

    /* Corps de la methode */
    if (atrouve) {

        bool afin = false;
        int i;
        for(i = 1; i < Configuration::instance()->listeSatellites().size() && !afin; i++) {
            afin = (Configuration::instance()->listeSatellites().at(i).tle().norad() == norad);
        }

#if QT_VERSION < 0x050D00
        Configuration::instance()->listeSatellites().swap(--i, 0);
#else
        Configuration::instance()->listeSatellites().swapItemsAt(--i, 0);
#endif

    } else {
        // Le satellite choisi ne fait pas partie de la liste de satellites, on l'ajoute
        Configuration::instance()->listeSatellites().insert(0, Satellite(tle));
        Configuration::instance()->ajoutSatelliteFicTLE(norad);
    }

    // On definit le satellite choisi comme satellite par defaut
    Configuration::instance()->tleDefaut().nomsat = tle.nom();
    Configuration::instance()->tleDefaut().l1 = tle.ligne1();
    Configuration::instance()->tleDefaut().l2 = tle.ligne2();

    ui->liste1->currentItem()->setCheckState(Qt::Checked);

    GestionTempsReel();

    Configuration::instance()->EcritureConfiguration();
    EcritureTleDefautRegistre();

    /* Retour */
    return;
}

void PreviSat::on_actionNouveau_fichier_TLE_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo fich(Configuration::instance()->nomfic());
    QStringList listeNorad = Configuration::instance()->mapSatellitesFicTLE().value(fich.fileName());
    listeNorad.sort();

    /* Corps de la methode */
    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), Configuration::instance()->dirTle(),
                                                     tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));

    QFile fi(fic);
    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fi);

        QStringListIterator it(listeNorad);
        while (it.hasNext()) {

            const TLE tle = Configuration::instance()->mapTLE()[it.next()];
            flux << tle.ligne0() << endl
                 << tle.ligne1() << endl
                 << tle.ligne2() << endl;
        }
    }
    fi.close();

    // Ajout du fichier dans la liste de fichiers TLE
    const QFileInfo f(fic);
    const QString fichier = (QDir::toNativeSeparators(fic).contains(Configuration::instance()->dirTle()) ? f.fileName() : fic);
    Configuration::instance()->ajoutListeFicTLE(fichier);
    InitFicTLE();

    AfficherMessageStatut(tr("Fichier %1 créé").arg(fichier), 10);

    /* Retour */
    return;
}

void PreviSat::on_actionFichier_TLE_existant_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo fich(Configuration::instance()->nomfic());
    QStringList listeNorad = Configuration::instance()->mapSatellitesFicTLE().value(fich.fileName());
    listeNorad.sort();

    /* Corps de la methode */
    try {

        const QString fic = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), Configuration::instance()->dirTle(),
                                                         tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));

        // Verification que le fichier est un TLE
        TLE::VerifieFichier(fic, true);
        QMap<QString, TLE> tabtle = TLE::LectureFichier(fic, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec(),
                                                        QStringList(), false);

        // Age des nouveaux numeros NORAD dans la liste
        int nb = 0;
        QStringListIterator it(listeNorad);
        while (it.hasNext()) {

            const QString norad = it.next();
            if (!tabtle.keys().contains(norad)) {

                const TLE tle = Configuration::instance()->mapTLE()[norad];
                tabtle.insert(norad, tle);
                nb++;
            }
        }

        // Ecriture du fichier
        QFile fi(fic);
        if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

            QTextStream flux(&fi);

            QMapIterator<QString, TLE> it2(tabtle);
            while (it2.hasNext()) {
                it2.next();

                flux << it2.value().ligne0() << endl;
                flux << it2.value().ligne1() << endl;
                flux << it2.value().ligne2() << endl;
            }
        }
        fi.close();

        const QFileInfo f(fic);
        const QString fichier = (QDir::toNativeSeparators(fic).contains(Configuration::instance()->dirTle()) ? f.fileName() : fic);

        if (nb == 0) {
            AfficherMessageStatut(tr("Aucun satellite ajouté dans le fichier %1").arg(fichier), 10);
        } else {
            AfficherMessageStatut(tr("Fichier %1 augmenté de %2 nouveaux satellites").arg(fichier).arg(nb), 10);
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_lancementVideoNasa_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString fic = Configuration::instance()->listeChainesNasa().at(ui->chaineNasa->value() - 1);

        // Verification de la connexion
        QTcpSocket socket;
        QString adresse = Configuration::instance()->adresseAstropedia();
        socket.connectToHost(adresse.remove("http://").remove("/"), 80);
        if (!socket.waitForConnected(1000)) {
            throw PreviSatException(tr("Impossible de lancer le flux vidéo : " \
                                       "essayez de nouveau et/ou vérifiez votre connexion Internet"), WARNING);
        }

        setCursor(Qt::ArrowCursor);
        QDesktopServices::openUrl(QUrl(fic));

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}
