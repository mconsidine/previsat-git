/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre principale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    18 mai 2019
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QSettings>
#include <QTimer>
#include "ui_previsat.h"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include "ui_onglets.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "apropos.h"
#include "carte.h"
#include "configuration/configuration.h"
#include "listwidgetitem.h"
#include "onglets.h"
#include "previsat.h"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/systeme/decompression.h"


// Registre
static QSettings settings("Astropedia", "PreviSat");


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

    // Initialisation de la configuration generale
    Configuration::instance()->Initialisation();

    // Onglets
    _onglets = new Onglets(this);
    ui->layoutOnglets->addWidget(_onglets, 0, Qt::AlignVCenter);

    // Carte
    _carte = new Carte(_onglets, this);
    _carte->resize(811, 425);
    ui->layoutCarte->addWidget(_carte);

    connect(_onglets->ui()->langue, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangementLangue(const int)));
    connect(_onglets, SIGNAL(AffichageSiteLancement(const QString &, const Observateur &)),
            _carte, SLOT(AffichageSiteLancement(const QString &, const Observateur &)));
    connect(_onglets, SIGNAL(AfficherMessageStatut(const QString &, const int)), this, SLOT(AfficherMessageStatut(const QString &, const int)));

    /* Retour */
    return;
}

/*
 * Destructeur
 */
PreviSat::~PreviSat()
{
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

    // Initialisation des menus
    InitMenus();

    // Barre de statut
    InitBarreStatut();

    // Valeur des champs par defaut
    InitChampsDefaut();

    // Liste des fichiers TLE
    InitFicTLE();

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
        if (fi.exists() && (fi.size() != 0)) {

            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = Configuration::instance()->dirTmp() + QDir::separator() + fi.completeBaseName();

                //                if (DecompressionFichierGz(nomfic, fic)) {
                //                    ficgz = nomfic;
                //                    nomfic = fic;
                //                    fi = QFileInfo(nomfic);
                //                } else {
                //                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                //                    throw PreviSatException(msg.arg(nomfic), WARNING);
                //                }
            }

            // Verification du fichier TLE
            TLE::VerifieFichier(nomfic, true);

            // Lecture du fichier TLE en entier
            Configuration::instance()->setMapTLE(TLE::LectureFichier(Configuration::instance()->dirLocalData(), Configuration::instance()->nomfic()));

            // Mise a jour de la liste de satellites
            QStringList listeSatellites = Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()];
            QStringListIterator it(Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()]);
            while (it.hasNext()) {
                const QString norad = it.next();
                if (Configuration::instance()->mapTLE().keys().contains(norad)) {
                    listeSatellites.removeOne(norad);
                }
            }
            Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()] = listeSatellites;

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
    //    Telechargement tel(Configuration::instance()->dirTle());
    //    tel.TelechargementFichier("http://celestrak.com/NORAD/elements/visual.txt", false);



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

    Etoile::Initialisation(Configuration::instance()->dirCommonData(), Configuration::instance()->etoiles());

    const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
    const QFileInfo fi(Configuration::instance()->nomfic());

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

    // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
    EnchainementCalculs();

    // Affichage des donnees dans la barre d'onglets
    _onglets->show(*_dateCourante);

    _carte->AffichageCourbes();

    // Lancement du chronometre
    _chronometre = new QTimer(this);
    _chronometre->setInterval(ui->pasReel->currentText().toInt() * 1000);
    connect(_chronometre, SIGNAL(timeout()), this, SLOT(GestionTempsReel()));
    _chronometre->start();

    // Lancement du chronometreMs
    _chronometreMs = new QTimer(this);
    _chronometreMs->setInterval(200);
    connect(_chronometreMs, SIGNAL(timeout()), this, SLOT(TempsReel()));
    _chronometreMs->start();

    /* Retour */
    return;
}

/*
 * Changement dynamique de la langue
 */
void PreviSat::ChangementLangue(const int index)
{
    /* Declarations des variables locales */
    QString langue;

    /* Initialisations */

    /* Corps de la methode */
    switch (index) {
    case 0:
        langue = "fr";
        break;

    default:
    case 1:
        langue = "en";
        break;
    }

    InstallationTraduction(QString("%1_%2").arg(qApp->applicationName()).arg(langue), _appTraduction);
    InstallationTraduction(QString("qtbase_%1").arg(langue), _qtTraduction);
    ui->retranslateUi(this);
    _onglets->ui()->retranslateUi(_onglets);

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
    _onglets->ui()->updown->setValue(sgn(offsetUTC) * (static_cast<int>(fabs(offsetUTC) * NB_MIN_PAR_JOUR + EPS_DATES)));
    offsetUTC = (_onglets->ui()->heureLegale->isChecked()) ? _onglets->ui()->updown->value() * NB_JOUR_PAR_MIN : 0.;

    // Date et heure locales
    _dateCourante = new Date(offsetUTC);

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
    int idx = 0;
    QStringList listeTLE = Configuration::instance()->listeFicTLE();

    /* Corps de la methode */
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
            listeTLE.removeAt(idx);
        }
        idx++;
    }

    if (listeTLE.count() == 0) {
        ui->listeFichiersTLE->addItem("");
    }
    ui->listeFichiersTLE->addItem(tr("Parcourir..."));

    // Mise a jour de la liste de fichiers TLE
    Configuration::instance()->setListeFicTLE(listeTLE);

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
    ui->menuBar->setVisible(false);

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
    if (traduction.load(langue, qApp->applicationDirPath())) {
        qApp->installTranslator(&traduction);
    }

    /* Retour */
    return;
}


/*************
 * Affichage *
 ************/
/*
 * Afficher les noms des satellites dans les listes
 */
void PreviSat::AfficherListeSatellites(const QString &nomfic, const bool majListesOnglets)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QStringList listeSatellites = Configuration::instance()->mapSatellitesFicTLE()[nomfic];

    /* Corps de la methode */
    QMapIterator<QString, TLE> it(Configuration::instance()->mapTLE());
    while (it.hasNext()) {
        it.next();

        const QString nomsat = it.value().nom().trimmed();
        const QString norad = Configuration::instance()->tleDefaut().l1.mid(2, 5);
        const bool check = listeSatellites.contains(it.key());
        const QString tooltip = tr("%1\nNORAD : %2\nCOSPAR : %3").arg(nomsat).arg(it.key()).arg(it.value().donnees().cospar());

        // Liste principale de satellites
        ListWidgetItem * const elem1 = new ListWidgetItem(nomsat, ui->liste1);
        elem1->setData(Qt::UserRole, it.key());
        elem1->setToolTip(tooltip);
        elem1->setFlags(Qt::ItemIsEnabled);
        elem1->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
        if (it.key() == norad) {
            ui->liste1->setCurrentItem(elem1);
        }

        if (majListesOnglets) {

            // Liste pour les previsions de passage
            ListWidgetItem * const elem2 = new ListWidgetItem(nomsat, _onglets->ui()->liste2);
            elem2->setData(Qt::UserRole, it.key());
            elem2->setToolTip(tooltip);
            elem2->setFlags(Qt::ItemIsEnabled);
            elem2->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

            // Liste pour les evenements orbitaux
            ListWidgetItem * const elem3 = new ListWidgetItem(nomsat, _onglets->ui()->liste3);
            elem3->setData(Qt::UserRole, it.key());
            elem3->setToolTip(tooltip);
            elem3->setFlags(Qt::ItemIsEnabled);
            elem3->setCheckState((check) ? Qt::Checked : Qt::Unchecked);

#if defined (Q_OS_WIN)
            // Liste pour le suivi de telescope
            ListWidgetItem * const elem4 = new ListWidgetItem(nomsat, _onglets->ui()->liste4);
            elem4->setData(Qt::UserRole, it.key());
            elem4->setToolTip(tooltip);
            elem4->setFlags(Qt::ItemIsEnabled);
            elem4->setCheckState(((nomsat == Configuration::instance()->tleDefaut().nomsat) && check) ? Qt::Checked : Qt::Unchecked);
#endif

            if (it.key() == norad) {
                _onglets->ui()->liste2->setCurrentItem(elem2);
                _onglets->ui()->liste3->setCurrentItem(elem3);
#if defined (Q_OS_WIN)
                _onglets->ui()->liste4->setCurrentItem(elem4);
#endif
            }
        }
    }

    ui->liste1->sortItems();
    _onglets->ui()->liste2->sortItems();
    _onglets->ui()->liste3->sortItems();
#if defined (Q_OS_WIN)
    _onglets->ui()->liste4->sortItems();
#endif

    //ui->liste1->scrollToItem(ui->liste1->currentItem());
    //_onglets->ui()->liste2->scrollToItem(_onglets->ui()->liste2->currentItem());

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

    if (_timerStatut->isActive()) {
        _timerStatut->stop();
    }
    _timerStatut = new QTimer(this);
    _timerStatut->setInterval(secondes * 1000);
    connect(_timerStatut, SIGNAL(timeout()), this, SLOT(EffacerMessageStatut()));
    _timerStatut->start();

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

        //        //if (!ui->carte->isHidden()) {

        // Coordonnees terrestres
        soleil.CalculCoordTerrestres(observateur);

        //if (ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored) {
        // Coordonnees equatoriales
        soleil.CalculCoordEquat(observateur);
        //}
        //}


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

        //if (!ui->carte->isHidden()) {

        // Coordonnees terrestres
        lune.CalculCoordTerrestres(observateur);

        //if (ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored) {
        // Coordonnees equatoriales
        lune.CalculCoordEquat(observateur);
        //}
        //}


        //if (ui->ciel->isVisible()) {

        /*
                     * Calcul de la position des planetes
                     */
        //if (ui->affplanetes->checkState() != Qt::Unchecked) {

        //                QList<Planete> &planetes = Configuration::instance()->planetes();
        //                planetes.clear();
        //                for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

        //                    Planete planete(static_cast<IndicePlanete>(iplanete));
        //                    planete.CalculPosition(_dateCourante, soleil);
        //                    planete.CalculCoordHoriz(observateur);
        //                    planetes.append(planete);
        //                }
        //}

        //            /*
        //             * Calcul de la position du catalogue d'etoiles
        //             */
        //            Etoile::CalculPositionEtoiles(observateur, Configuration::instance()->etoiles());
        //if (ui->affconst->isChecked()) {
        //                Constellation::CalculConstellations(observateur);
        //}
        //if (ui->affconst->checkState() != Qt::Unchecked) {
        //                LigneConstellation::CalculLignesCst(Configuration::instance()->etoiles());
        //}
        //}

        QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
        if (satellites.isEmpty()) {

            const QString titre = "%1 %2";
            setWindowTitle(titre.arg(qApp->applicationName()).arg(QString(APPVER_MAJ)));

        } else {

            const QString titre = "%1 %2 - %3";
            setWindowTitle(titre.arg(qApp->applicationName()).arg(QString(APPVER_MAJ)).arg(Configuration::instance()->tleDefaut().nomsat));

            Satellite::CalculPosVitListeSatellites(*_dateCourante, observateur, soleil, lune, _onglets->ui()->nombreTrajectoires->value(),
                                                   _onglets->ui()->eclipsesLune->isChecked(), _onglets->ui()->effetEclipsesMagnitude->isChecked(),
                                                   _onglets->ui()->extinctionAtmospherique->isChecked(),
                                                   _onglets->ui()->refractionPourEclipses->isChecked(), _onglets->ui()->affvisib, satellites);
        }


    } catch (PreviSatException &e) {
        throw PreviSatException();
    }

    /* Retour */
    return;
}

void PreviSat::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

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

void PreviSat::resizeEvent(QResizeEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
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


    /* Retour */
    return;
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

void PreviSat::on_pasReel_currentIndexChanged(int index)
{

}

void PreviSat::on_pasManuel_currentIndexChanged(int index)
{

}

void PreviSat::on_liste1_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste1->hasFocus() && (ui->liste1->currentRow() >= 0)) {

        const QString norad = item->data(Qt::UserRole).toString();
        if (ui->liste1->currentItem()->checkState() == Qt::Checked) {

            // Suppression d'un satellite dans la liste
            ui->liste1->currentItem()->setCheckState(Qt::Unchecked);
            Configuration::instance()->suppressionSatelliteFicTLE(norad);

        } else {

            // Ajout d'un satellite dans la liste
            ui->liste1->currentItem()->setCheckState(Qt::Checked);
            Configuration::instance()->ajoutSatelliteFicTLE(norad);

        }
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

    /* Corps de la methode */
    if (nomsat != norad) {
        AfficherMessageStatut(tr("%1 (numéro NORAD : %2)").arg(nomsat).arg(norad), 10);
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

void PreviSat::GestionTempsReel()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->tempsReel->isChecked()) {

        // Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
        EnchainementCalculs();

        // Affichage des donnees dans la barre d'onglets
        _onglets->show(*_dateCourante);

        _carte->AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::TempsReel()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->tempsReel->isChecked()) {
        // Date actuelle
        _dateCourante = new Date(_dateCourante->offsetUTC());
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
        _stsDate->setText(d.toString(tr("dd/MM/yyyy")));
        _stsHeure->setText(d.toString("hh:mm:ss") + ((_onglets->ui()->syst12h->isChecked()) ? "a" : ""));
        _stsDate->setToolTip(tr("Date"));
        _stsHeure->setToolTip(tr("Heure"));
    }

    /* Retour */
    return;
}
