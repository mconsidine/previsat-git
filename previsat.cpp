/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
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
 * >    16 septembre 2012
 *
 */

#include <QClipboard>
#include <QtCore/qmath.h>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QGraphicsTextItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QSound>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/messages.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "previsions/conditions.h"
#include "previsions/iridium.h"
#include "afficher.h"
#include "apropos.h"
#include "gestionnairetle.h"
#include "telecharger.h"
#include "threadcalculs.h"
#include "zlib.h"
#include "previsat.h"
#include "ui_previsat.h"

// Repertoires
static QString dirCoo;
static QString dirDat;
static QString dirExe;
static QString dirMap;
static QString dirOut;
static QString dirTle;
static QString dirTmp;

// TLE par defaut
static QString nom;
static QString l1;
static QString l2;

// Liste de satellites
static bool htr;
static bool info;
static bool notif;
static bool old;
static int ind;
static int nbSat;
static QString nomfic;
static QString ficgz;
static QString nor;
static QVector<bool> bipSat;
static QVector<TLE> tles;
static QList<Satellite> satellites;
static QStringList liste;
static QStringList listeGroupeMaj;

// Date courante
static Date dateCourante;
static double offsetUTC;
static QDateTime tim;

// Lieux d'observation
static int selec;
static QList<Observateur> observateurs;
static QStringList ficObs;
static QStringList listeObs;
static QStringList mapObs;

// Cartes du monde
static int selec2;
static QStringList ficMap;
static double DEG2PXHZ;
static double DEG2PXVT;

// Soleil, Lune, etoiles, planetes
static Soleil soleil;
static Lune lune;
static QList<Etoile> etoiles;
static QList<Constellation> constellations;
static QList<LigneConstellation> lignesCst;
static QList<Planete> planetes;
static QList<QColor> couleurPlanetes;

// SAA
static double tabSAA[59][2] = { { -96.5, -29. }, { -95., -24.5 }, { -90., -16. }, { -85., -10. }, { -80., -6. },
                                { -75., -3.5 }, { -70., 0. }, { -65., 4. }, { -60., 6.5 }, { -55., 8. }, { -50., 9. },
                                { -45., 10. }, { -40., 11. }, { -35., 12. }, { -30., 13. }, { -25., 12. }, { -20., 9.5 },
                                { -15., 8. }, { -10., 7. }, { -5., 6. }, { 0., 4. }, { 5., 2. }, { 10., -3. }, { 15., -4. },
                                { 20., -5. }, { 25., -6. }, { 30., -8. }, { 35., -11.5 }, { 40., -14. }, { 42.5, -17.5 },
                                { 40., -22. }, { 35., -23.5 }, { 30., -25. }, { 25., -27. }, { 20., -29. }, { 15., -32. },
                                { 10., -33.5 }, { 5., -35. }, { 0., -36. }, { -5., -37. }, { -10., -38.5 }, { -15., -42.5 },
                                { -20., -44.5 }, { -25., -46. }, { -30., -47.5 }, { -35., -48.5 }, { -40., -49.5 },
                                { -45., -49. }, { -50., -48.5 }, { -55., -47.5 }, { -60., -47. }, { -65., -46.5 },
                                { -70., -45.5 }, { -75., -43.5 }, { -80., -42. }, { -85., -38.5 }, { -90., -36. },
                                { -95., -33. }, { -96.5, -29. } };


// Ecliptique
static double tabEcliptique[49][2] = { { 0., 0. }, { 0.5, 3.233 }, { 1., 6.4 }, { 1.5, 9.417 },  { 2., 12.217 },
                                       { 2.5, 14.783 }, { 3., 17. }, { 3.5, 18.983 }, { 4., 20.567 }, { 4.5, 21.817 },
                                       { 5., 22.75 }, { 5.5, 23.25 }, { 6., 23.433 }, { 6.5, 23.25 }, { 7., 22.75 },
                                       { 7.5, 21.817 }, { 8., 20.567 }, { 8.5, 18.983 }, { 9., 17. }, { 9.5, 14.783 },
                                       { 10., 12.217 }, { 10.5, 9.417 }, { 11., 6.4 }, { 11.5, 3.233 }, { 12., 0. },
                                       { 12.5, -3.233 }, { 13., -6.4 }, { 13.5, -9.417 }, { 14., -12.217 }, { 14.5, -14.783 },
                                       { 15., -17. }, { 15.5, -18.983 }, { 16., -20.567 }, { 16.5, -21.817 }, { 17., -22.75 },
                                       { 17.5, -23.25 }, { 18., -23.433 }, { 18.5, -23.25 }, { 19., -22.75 },
                                       { 19.5, -21.817 }, { 20., -20.567 }, { 20.5, -18.983 }, { 21., -17. },
                                       { 21.5, -14.783 }, { 22., -12.217 }, { 22.5, -9.417 }, { 23., -6.4 }, { 23.5, -3.233 },
                                       { 24., 0. } };

// Registre
static QSettings settings("Astropedia", "previsat");

// Telechargement
static bool amajDeb;
static bool amajInt;
static bool atrouve;
static bool aupdnow;
static QNetworkAccessManager mng;
static QQueue<QUrl> downQueue;
static QNetworkReply *rep;
static QFile ficDwn;
static QString dirDwn;

static QTimer *chronometre;
static ThreadCalculs *threadCalculs;
static Afficher *afficherResultats;
static GestionnaireTLE *gestionnaire;

// Interface graphique
QPalette paletteDefaut;
QGraphicsScene *scene;
QGraphicsScene *scene2;
QGraphicsScene *scene3;

QLabel *messagesStatut;
QLabel *messagesStatut2;
QLabel *messagesStatut3;
QLabel *modeFonctionnement;
QLabel *stsDate;
QLabel *stsHeure;

PreviSat::PreviSat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreviSat)
{
    ui->setupUi(this);
    PreviSat::move(0, 0);
    ui->onglets->setCurrentIndex(0);
    ui->ongletsOptions->setCurrentIndex(0);
    ui->ongletsOutils->setCurrentIndex(0);
}

PreviSat::~PreviSat()
{
    delete ui;
}

/*******************
 * Initialisations *
 ******************/

/*
 * Chargement de la configuration
 */
void PreviSat::ChargementConfig()
{
    /* Declarations des variables locales */
    QDir di;

    /* Initialisations */
    info = true;
    old = false;
    selec = -1;
    selec2 = 0;
    couleurPlanetes << Qt::gray << Qt::white << Qt::red << QColor("orange") << Qt::darkYellow << Qt::green << Qt::blue;
    paletteDefaut = PreviSat::palette();
    tim = QDateTime();

    // Repertoires
    dirExe = QCoreApplication::applicationDirPath();
#if defined (Q_OS_WIN)
    dirDat = dirExe + QDir::separator() + "data";
    dirOut = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() +
            QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    dirTle = dirExe + QDir::separator() + "tle";

#elif defined (Q_OS_LINUX)
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
    dirOut = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QDir::separator() +
            QCoreApplication::applicationName();
    dirTle = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "tle";

#elif defined (Q_OS_MAC)
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
    dirOut = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QDir::separator() +
            QCoreApplication::applicationName();
    dirTle = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "tle";

#else
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
    dirOut = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() +
            QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    dirTle = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "tle";
#endif

    dirCoo = dirDat + QDir::separator() + "coordonnees";
    dirMap = dirDat + QDir::separator() + "map";
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);

    DEG2PXHZ = 1. / 0.45;
    DEG2PXVT = 1. / 0.45;
    chronometre = new QTimer(this);

    /* Corps de la methode */
    // Verification et creation des arborescences
    di = QDir(dirDat);
    if (!di.exists()) {
        const QString message = tr("POSITION : Erreur rencontrée lors de l'initialisation\nLe répertoire %1 n'existe pas, veuillez réinstaller PreviSat");
        Messages::Afficher(message.arg(QDir::convertSeparators(dirDat)), ERREUR);
        exit(1);
    }

    QStringList listeDir;
    listeDir << dirMap << dirOut << dirTle << dirTmp;
    foreach(QString dir, listeDir) {
        di = QDir(dir);
        if (!di.exists())
            di.mkpath(dir);
    }

    // Verification de la presence des fichiers du repertoire data
    QStringList ficdata;
    ficdata << "chimes.wav" << "constellations.cst" << "constlabel.cst" << "constlines.cst" << "donnees.sat" <<
               "etoiles.str" << "gestionnaireTLE.gst" << "iridium.sts";
    QStringListIterator it1(ficdata);
    while (it1.hasNext()) {
        const QFile fi(dirDat + QDir::separator() + it1.next());
        if (!fi.exists()) {
            const QString message = tr("POSITION : Le fichier %1 n'existe pas, veuillez réinstaller PreviSat");
            Messages::Afficher(message.arg(fi.fileName()), ERREUR);
            exit(1);
        }
    }

    // Chargement des fichiers images de cartes du monde
    InitFicMap(false);

    // Recuperation des donnees en memoire
    // TLE par defaut (lors de la premiere utilisation de PreviSat, ces chaines de caracteres sont vides)
    nom = settings.value("TLE/nom", "").toString();
    l1 = settings.value("TLE/l1", "").toString();
    l2 = settings.value("TLE/l2", "").toString();

    nbSat = settings.value("TLE/nbsat", 2).toInt();
    liste = settings.value("TLE/liste", "25544&20580").toString().split("&");

    // Affichage des champs par defaut
    ui->pasReel->setCurrentIndex(settings.value("temps/pasreel", 1).toInt());
    ui->pasManuel->setCurrentIndex(settings.value("temps/pasmanuel", 1).toInt());
    ui->valManuel->setCurrentIndex(settings.value("temps/valmanuel", 0).toInt());
    nomfic = settings.value("fichier/nom", QDir::convertSeparators(dirTle + QDir::separator() + "visual.txt")).toString();

    ui->fichierAMettreAJour->setText(settings.value("fichier/fichierAMettreAJour", nomfic).toString());
    ui->fichierALire->setText(settings.value("fichier/fichierALire", "").toString());
    ui->fichierALireCreerTLE->setText(settings.value("fichier/fichierALireCreerTLE", "").toString());
    ui->nomFichierPerso->setText(settings.value("fichier/nomFichierPerso", "").toString());
    ui->fichierTLEIri->setText(settings.value("fichier/iridium", QDir::convertSeparators(dirTle + QDir::separator() +
                                                                                         "iridium.txt")).toString());
    ui->fichierTLETransit->setText(settings.value("fichier/fichierTLETransit", nomfic).toString());
    ui->affconst->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt()));
    ui->affcoord->setChecked(settings.value("affichage/affcoord", true).toBool());
    ui->affetoiles->setChecked(settings.value("affichage/affetoiles", true).toBool());
    ui->affgrille->setChecked(settings.value("affichage/affgrille", true).toBool());
    ui->affinvew->setChecked(settings.value("affichage/affinvew", false).toBool());
    ui->affinvns->setChecked(settings.value("affichage/affinvns", false).toBool());
    ui->afflune->setChecked(settings.value("affichage/afflune", true).toBool());
    ui->affnomlieu->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomlieu", Qt::Checked).toUInt()));
    ui->affnomsat->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomsat", Qt::Checked).toUInt()));
    ui->affnotif->setChecked(settings.value("affichage/affnotif", true).toBool());
    ui->affnuit->setChecked(settings.value("affichage/affnuit", true).toBool());
    ui->affphaselune->setChecked(settings.value("affichage/affphaselune", true).toBool());
    ui->affplanetes->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affplanetes", Qt::Checked).toUInt()));
    ui->affradar->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affradar", Qt::Checked).toUInt()));
    ui->affSAA->setChecked(settings.value("affichage/affSAA", false).toBool());
    ui->affsoleil->setChecked(settings.value("affichage/affsoleil", true).toBool());
    ui->afftraj->setChecked(settings.value("affichage/afftraj", true).toBool());
    ui->affvisib->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affvisib", Qt::Checked).toUInt()));
    ui->calJulien->setChecked(settings.value("affichage/calJulien", false).toBool());
    ui->extinctionAtmospherique->setChecked(settings.value("affichage/extinction", true).toBool());
    ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 40).toInt());
    ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
    ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 1).toUInt());
    ui->utcAuto->setChecked(settings.value("affichage/utcAuto", true).toBool());

    if (settings.value("affichage/utc", false).toBool())
        ui->utc->setChecked(true);
    else
        ui->heureLegale->setChecked(true);

    if (settings.value("affichage/unite", true).toBool())
        ui->unitesKm->setChecked(true);
    else
        ui->unitesMi->setChecked(true);

    settings.setValue("fichier/path", dirExe);

    // Affichage au demarrage
    QStyle *style = QApplication::style();
    ui->actionEnregistrer->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionOuvrir_fichier_TLE->setIcon(style->standardIcon(QStyle::SP_DirOpenIcon));
    ui->ciel->setVisible(false);
    ui->nord->setVisible(false);
    ui->sud->setVisible(false);
    ui->est->setVisible(false);
    ui->ouest->setVisible(false);

    ui->pasManuel->setVisible(false);
    ui->valManuel->setVisible(false);
    ui->dateHeure3->setVisible(false);
    ui->utcManuel->setVisible(false);
    ui->dateHeure4->setVisible(false);
    ui->utcManuel2->setVisible(false);
    ui->frameSimu->setVisible(false);
    ui->pause->setEnabled(false);

    ui->hauteurSatPrev->setCurrentIndex(0);
    ui->hauteurSoleilPrev->setCurrentIndex(1);
    ui->valHauteurSatPrev->setVisible(false);
    ui->valHauteurSoleilPrev->setVisible(false);
    ui->valMagnitudeMaxPrev->setVisible(false);
    ui->afficherPrev->setVisible(false);
    ui->annulerPrev->setVisible(false);

    ui->hauteurSatIri->setCurrentIndex(2);
    ui->hauteurSoleilIri->setCurrentIndex(1);
    ui->valHauteurSatIri->setVisible(false);
    ui->valHauteurSoleilIri->setVisible(false);
    ui->afficherIri->setVisible(false);
    ui->annulerIri->setVisible(false);

    ui->coordonnees->setVisible(false);
    ui->nouveauLieu->setVisible(false);
    ui->nouvelleCategorie->setVisible(false);
    ui->nvEw->setCurrentIndex(0);
    ui->nvNs->setCurrentIndex(0);

    ui->numeroNORADCreerTLE->setCurrentIndex(0);
    ui->ADNoeudAscendantCreerTLE->setCurrentIndex(0);
    ui->excentriciteCreerTLE->setCurrentIndex(0);
    ui->inclinaisonCreerTLE->setCurrentIndex(0);
    ui->argumentPerigeeCreerTLE->setCurrentIndex(0);
    ui->fichierTelechargement->setText("");
    ui->barreProgression->setValue(0);
    ui->frameBarreProgression->setVisible(false);
    ui->compteRenduMaj->setVisible(false);
    ui->frameADNA->setVisible(false);
    ui->frameArgumentPerigee->setVisible(false);
    ui->frameExcentricite->setVisible(false);
    ui->frameIncl->setVisible(false);
    ui->frameNORAD->setVisible(false);

    ui->afficherEvt->setVisible(false);
    ui->annulerEvt->setVisible(false);

    ui->hauteurSatTransit->setCurrentIndex(1);
    ui->pasGeneration->setCurrentIndex(5);
    ui->valHauteurSatTransit->setVisible(false);
    ui->afficherTransit->setVisible(false);
    ui->annulerTransit->setVisible(false);

    // Menu
    ui->barreMenu->setMenu(ui->menuOuvrir_fichier_TLE);
    ui->menuBar->setVisible(false);

    // Barre de statut
    messagesStatut = new QLabel("", this);
    messagesStatut->setFrameStyle(QFrame::NoFrame);
    messagesStatut->setIndent(3);
    messagesStatut->setMinimumSize(398, 0);
    messagesStatut->setToolTip(tr("Messages"));

    messagesStatut2 = new QLabel("", this);
    messagesStatut2->setFrameStyle(QFrame::NoFrame);
    messagesStatut2->setFixedWidth(140);
    messagesStatut2->setAlignment(Qt::AlignCenter);
    messagesStatut2->setVisible(false);

    messagesStatut3 = new QLabel("", this);
    messagesStatut3->setFrameStyle(QFrame::NoFrame);
    messagesStatut3->setFixedWidth(140);
    messagesStatut3->setAlignment(Qt::AlignCenter);
    messagesStatut3->setVisible(false);

    modeFonctionnement = new QLabel("", this);
    modeFonctionnement->setFrameStyle(QFrame::NoFrame);
    modeFonctionnement->setFixedWidth(110);
    modeFonctionnement->setAlignment(Qt::AlignCenter);
    modeFonctionnement->setToolTip(tr("Mode de fonctionnement"));

    stsDate = new QLabel("", this);
    stsDate->setFrameStyle(QFrame::NoFrame);
    stsDate->setFixedWidth(90);
    stsDate->setAlignment(Qt::AlignCenter);
    stsDate->setToolTip(tr("Date"));

    stsHeure = new QLabel("", this);
    stsHeure->setFrameStyle(QFrame::NoFrame);
    stsHeure->setFixedWidth(77);
    stsHeure->setAlignment(Qt::AlignCenter);
    stsHeure->setToolTip(tr("Heure"));

    ui->barreStatut->addPermanentWidget(messagesStatut, 1);
    ui->barreStatut->addPermanentWidget(messagesStatut2);
    ui->barreStatut->addPermanentWidget(messagesStatut3);
    ui->barreStatut->addPermanentWidget(modeFonctionnement);
    ui->barreStatut->addPermanentWidget(stsDate);
    ui->barreStatut->addPermanentWidget(stsHeure);


    // Initialisation du lieu d'observation
    AffichageLieuObs();

    // Initialisation de la gestion des fichiers de lieux d'observation
    InitFicObs(true);
    if (ui->fichiersObs->count() > 0) {
        ui->fichiersObs->setCurrentRow(0);
        ui->ajdfic->setCurrentIndex(0);
    }

    QResizeEvent *event;
    resizeEvent(event);

    /* Retour */
    return;
}

/*
 * Chargement du fichier TLE
 */
void PreviSat::ChargementTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Ouverture du fichier TLE par defaut
    if (nbSat == 0) {
        tles.resize(1);
        nbSat = 1;
    } else {
        tles.resize(nbSat);
        bipSat.resize(nbSat);
    }

    // Lecture du fichier TLE par defaut
    try {

        const QFileInfo fi(nomfic);
        if (fi.exists()) {

            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = dirTmp + QDir::separator() + fi.completeBaseName();

                if (DecompressionFichierGz(nomfic, fic)) {
                    ficgz = nomfic;
                    nomfic = fic;
                } else {
                    throw PreviSatException(tr("POSITION : Erreur rencontrée lors de la décompression du fichier") + " " +
                                            nomfic, WARNING);
                }
            }

            // Verification du fichier
            TLE::VerifieFichier(nomfic, true);

            // Lecture du fichier
            TLE::LectureFichier(nomfic, liste, tles);

            ui->nomFichierTLE->setText(fi.fileName());

            // Mise a jour de la liste de satellites
            int i = 0;
            liste.clear();
            bipSat.clear();
            QVectorIterator<TLE> it2(tles);
            while (it2.hasNext()) {
                const TLE tle = it2.next();
                if (tle.getNorad().isEmpty()) {
                    tles.remove(i);
                } else {
                    liste.append(tles.at(i).getNorad());
                    bipSat.append(false);
                    i++;
                }
            }
            nbSat = tles.size();

            if (nbSat == 0) {

                if (!l1.isEmpty() && !l2.isEmpty())
                    tles.append(TLE(l1, l2));

                // Ouverture du fichier TLE (pour placer dans la liste de l'interface graphique les satellites contenus dans le fichier)
                AfficherListeSatellites(nomfic, liste);
                l1 = "";
                l2 = "";
            } else {
                AfficherListeSatellites(nomfic, liste);
            }

            // Recuperation des donnees satellites
            Satellite::LectureDonnees(liste, tles, satellites);

        } else {
            nbSat = 0;
            tles.clear();
            liste.clear();
            bipSat.clear();
            l1 = "";
            l2 = "";
            ui->liste1->clear();
            ui->liste2->clear();
            ui->liste3->clear();
            ui->lbl_nomFichierTLE->setVisible(false);
            ui->nomFichierTLE->setVisible(false);
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
    amajDeb = false;

    /* Corps de la methode */
    // Initialisation de la date
    // Determination automatique de l'ecart heure locale - UTC
    const QDateTime dateLocale = QDateTime::currentDateTime();
    QDateTime dateUTC(dateLocale);
    dateUTC.setTimeSpec(Qt::UTC);
    const double ecart = dateLocale.secsTo(dateUTC) * NB_JOUR_PAR_SEC;
    offsetUTC = (ui->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    ui->updown->setValue(Maths::sgn(offsetUTC) * ((int) (fabs(offsetUTC) * NB_MIN_PAR_JOUR + EPS_DATES)));
    offsetUTC = (ui->heureLegale->isChecked()) ? ui->updown->value() * NB_JOUR_PAR_MIN : 0.;

    // Date et heure locales
    dateCourante = Date(offsetUTC);

    // Chargement des groupes de TLE
    AffichageGroupesTLE();

    // Mise a jour des TLE si necessaire
    if (listeGroupeMaj.count() > 0) {

        const bool ageMaxTLE = settings.value("temps/ageMaxTLE", true).toBool();
        if (ageMaxTLE) {
            const double lastUpdate = settings.value("temps/lastUpdate", 0.).toDouble();
            const int ageMax = settings.value("temps/ageMax", 15).toInt();
            if (fabs(dateCourante.getJourJulienUTC() - lastUpdate) > ageMax ||
                    (dateCourante.getJourJulienUTC() - tles.at(0).getEpoque().getJourJulienUTC()) > ageMax) {
                MajWebTLE();
                settings.setValue("temps/lastUpdate", dateCourante.getJourJulienUTC());
            }
        } else {
            messagesStatut->setText(tr("Mise à jour automatique des TLE"));
            MajWebTLE();
            settings.setValue("temps/lastUpdate", dateCourante.getJourJulienUTC());
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
    const int xmax = QApplication::desktop()->width();
    const int ymax = QApplication::desktop()->height();

    /* Corps de la methode */
    // Redimensionnement de la fenetre si necessaire
    if (PreviSat::width() > xmax)
        PreviSat::setGeometry(0, 0, xmax, PreviSat::height());
    if (PreviSat::height() > ymax)
        PreviSat::setGeometry(0, 0, PreviSat::width(), ymax);

    // Calcul de la position des etoiles
    observateurs[0].CalculPosVit(dateCourante);
    Etoile::CalculPositionEtoiles(observateurs.at(0), etoiles);
    Constellation::CalculConstellations(observateurs.at(0), constellations);
    LigneConstellation::CalculLignesCst(etoiles, lignesCst);


    // Demarrage du temps reel
    chronometre->setInterval(200);
    connect(chronometre, SIGNAL(timeout()), this, SLOT(GestionTempsReel()));
    chronometre->start();

    CalculsAffichage();

    tim = QDateTime::currentDateTime();

    /* Retour */
    return;
}

/*
 * Chargement de la liste de lieux d'observation
 */
void PreviSat::InitFicObs(const bool alarm) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->fichiersObs->clear();
    ui->ajdfic->clear();

    // Repertoire contenant les fichiers de lieux d'observation
    const QDir di = QDir(dirCoo);
    if (di.exists()) {

        // Nombre de fichiers contenus dans le repertoire 'coordonnees'
        if (di.entryList(QDir::Files).count() == 0) {
            if (alarm)
                Messages::Afficher(tr("POSITION : Erreur rencontrée lors de l'initialisation\nIl n'existe aucun fichier de lieux d'observation"), WARNING);
        } else {

            // Liste de fichiers de lieux d'observation
            ficObs.clear();
            foreach(QString fic, di.entryList(QDir::Files)) {

                try {
                    // Verification que le fichier est un fichier de lieux d'observation
                    const QString file = dirCoo + QDir::separator() + fic;
                    QFile fi(file);
                    if (fi.exists()) {

                        fi.open(QIODevice::ReadOnly | QIODevice::Text);
                        QTextStream flux(&fi);
                        while (!flux.atEnd()) {
                            const QString lieu = flux.readLine();
                            if (lieu.size() > 0) {
                                if (lieu.size() > 33) {
                                    if (lieu.at(14) != ' ' || lieu.at(28) != ' ' || lieu.at(33) != ' ')
                                        throw PreviSatException();

                                } else {
                                    throw PreviSatException();
                                }
                            }
                        }
                    }

                    ficObs.append(file);
                    if (fic == "aapreferes") {
                        ui->fichiersObs->addItem(tr("Mes Préférés"));
                        ui->ajdfic->addItem(tr("Mes Préférés"));
                    } else {
                        fic[0] = fic.at(0).toUpper();
                        ui->fichiersObs->addItem(fic);
                        ui->ajdfic->addItem(fic);
                    }
                } catch (PreviSatException &e) {
                }
            }
        }
    } else {
        if (alarm)
            Messages::Afficher(tr("POSITION : Erreur rencontrée lors de l'initialisation\nLe répertoire contenant les fichiers de lieux d'observation n'existe pas"), WARNING);
    }

    /* Retour */
    return;
}

/*
 * Chargement de la liste de cartes du monde
 */
void PreviSat::InitFicMap(const bool majAff) const
{
    /* Declarations des variables locales */
    QStringList filtres;

    /* Initialisations */
    const QDir di(dirMap);
    filtres << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";

    /* Corps de la methode */
    ui->listeMap->clear();
    ui->listeMap->addItem(tr("* Défaut"));
    ui->listeMap->setCurrentIndex(0);

    if (di.entryList(filtres, QDir::Files).count() == 0) {
        ui->listeMap->addItem(tr("Télécharger..."));
    } else {

        foreach(QString fic, di.entryList(filtres, QDir::Files)) {

            const QString file = dirMap + QDir::separator() + fic;
            ficMap.append(file);
            ui->listeMap->addItem(fic.at(0).toUpper() + fic.mid(1, fic.lastIndexOf(".")-1));
            if (settings.value("fichier/listeMap", "").toString() == file)
                ui->listeMap->setCurrentIndex(ficMap.indexOf(file)+1);
        }
        ui->listeMap->addItem(tr("Télécharger..."));
        if (ui->listeMap->currentIndex() > 0 && majAff)
            AffichageCourbes();
    }

    /* Retour */
    return;
}


/**************
 * Affichages *
 *************/

/*
 * Affichage des donnees numeriques
 */
void PreviSat::AffichageDonnees()
{
    /* Declarations des variables locales */
    QString chaine2;

    /* Initialisations */
    QString unite1 = (ui->unitesKm->isChecked()) ? QObject::tr("km") : QObject::tr("mi");
    const QString unite2 = (ui->unitesKm->isChecked()) ? QObject::tr("km/s") : QObject::tr("mi/s");

    /* Corps de la methode */
    if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {
        // Affichage maximise : on ne reactualise pas l'affichage des donnees numeriques
    } else {

        if (satellites.isEmpty()) {
            l1 = "";
            l2 = "";
        }
        if (l1.isEmpty() && l2.isEmpty()) {

            // Cas ou aucun satellite n'est selectionne dans la liste de satellites
            ui->satellite->setVisible(false);
            const QString msg = "PreviSat %1";
            PreviSat::setWindowTitle(msg.arg(QString(APPVER_MAJ)));

            if (ui->onglets->count() == 7) {
                ui->onglets->removeTab(1);
                ui->onglets->removeTab(1);
            }
        } else {
            ui->satellite->setVisible(true);
            if (ui->onglets->count() < 7) {
                ui->onglets->insertTab(1, ui->osculateurs, tr("Éléments osculateurs"));
                ui->onglets->insertTab(2, ui->informations, tr("Informations satellite"));
            }
        }

        /*
         * Affichage des donnees sur l'onglet General
         */
        QString chaine = dateCourante.ToLongDate().append("  ");
        if (fabs(dateCourante.getOffsetUTC()) > EPSDBL100) {
            QTime heur;
            heur = heur.addSecs(fabs(dateCourante.getOffsetUTC()) * NB_SEC_PAR_JOUR + EPS_DATES);
            QString chaineUTC = tr("UTC").append((dateCourante.getOffsetUTC() > 0.) ? " + " : " - ").
                    append(heur.toString("hh:mm"));
            chaine = chaine.append(chaineUTC);
            ui->utcManuel->setText(chaineUTC);
        } else {
            ui->utcManuel->setText(tr("UTC"));
        }
        ui->dateHeure1->setText(chaine);
        ui->dateHeure2->setText(chaine);


        /*
         * Affichage des donnees relatives au satellite par defaut
         */
        if (!l1.isEmpty() && !l2.isEmpty()) {

            // Nom
            ui->nomsat1->setText(nom);
            const QString msg = "PreviSat %1 - %2";
            PreviSat::setWindowTitle(msg.arg(QString(APPVER_MAJ)).arg(nom));

            // Temps ecoule depuis l'epoque
            chaine = tr("%1 jours");
            QBrush brush;
            QPalette palette;
            brush.setStyle(Qt::SolidPattern);

            // Indicateur de l'age du TLE
            if (fabs(satellites.at(0).getAgeTLE()) > 15.) {
                brush.setColor(Qt::red);
            } else if (fabs(satellites.at(0).getAgeTLE()) > 10.) {
                brush.setColor(QColor("orange"));
            } else if (fabs(satellites.at(0).getAgeTLE()) > 5.) {
                brush.setColor(Qt::darkYellow);
            } else {
                brush.setColor(QColor("forestgreen"));
            }
            palette.setBrush(QPalette::WindowText, brush);
            ui->ageTLE->setPalette(palette);
            ui->ageTLE->setText(chaine.arg(dateCourante.getJourJulienUTC() - tles.at(0).getEpoque().getJourJulienUTC(), 0,
                                           'f', 2));

            // Longitude/latitude/altitude
            const QString ews = (satellites.at(0).getLongitude() > 0.) ? tr("Ouest") : tr("Est");
            ui->longitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).getLongitude()), DEGRE, 3, 0, false,
                                                           true).append(" ").append(ews));
            const QString nss = (satellites.at(0).getLatitude() > 0.) ? tr("Nord") : tr("Sud");
            ui->latitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).getLatitude()), DEGRE, 2, 0, false,
                                                          true).append(" ").append(nss));
            chaine = "%1 " + unite1;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).getAltitude(), 0, 'f', 1);
                ui->altitudeSat->setText(chaine);
            } else {
                chaine = chaine.arg(satellites.at(0).getAltitude() * MILE_PAR_KM, 0, 'f', 1);
                ui->altitudeSat->setText(chaine);
            }

            // Hauteur/azimut/distance
            ui->hauteurSat->setText(Maths::ToSexagesimal(satellites.at(0).getHauteur(), DEGRE, 2, 0, true, true));
            ui->azimutSat->setText(Maths::ToSexagesimal(satellites.at(0).getAzimut(), DEGRE, 3, 0, false, true));
            chaine = "%1 " + unite1;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).getDistance(), 0, 'f', 1);
                ui->distanceSat->setText(chaine);
            } else {
                chaine = chaine.arg(satellites.at(0).getDistance() * MILE_PAR_KM, 0, 'f', 1);
                ui->distanceSat->setText(chaine);
            }

            // Ascension droite/declinaison/constellation
            ui->ascensionDroiteSat->setText(Maths::ToSexagesimal(satellites.at(0).getAscensionDroite(), HEURE1, 2, 0,
                                                                 false, true).trimmed());
            ui->declinaisonSat->setText(Maths::ToSexagesimal(satellites.at(0).getDeclinaison(), DEGRE, 2, 0, true,
                                                             true).trimmed());
            ui->constellationSat->setText(satellites.at(0).getConstellation());

            // Direction/vitesse/range rate
            ui->directionSat->setText((satellites.at(0).getVitesse().getZ() > 0.) ? tr("Ascendant") : tr("Descendant"));
            chaine = "%1 " + unite2;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).getVitesse().Norme(), 0, 'f', 3);
                ui->vitesseSat->setText(chaine);

                chaine = "%1%2 " + unite2;
                chaine = chaine.arg((satellites.at(0).getRangeRate() >= 0.) ? "+" : "-").
                        arg(fabs(satellites.at(0).getRangeRate()), 0, 'f', 3);
                ui->rangeRate->setText(chaine);

            } else {
                chaine = chaine.arg(satellites.at(0).getVitesse().Norme() * MILE_PAR_KM, 0, 'f', 3);
                ui->vitesseSat->setText(chaine);

                chaine = "%1%2 " + unite2;
                chaine = chaine.arg((satellites.at(0).getRangeRate() >= 0.) ? "+" : "-").
                        arg(fabs(satellites.at(0).getRangeRate() * MILE_PAR_KM), 0, 'f', 3);
                ui->rangeRate->setText(chaine);
            }

            // Magnitude/Illumination
            const double fractionilluminee = 100. * satellites.at(0).getFractionIlluminee();
            if (satellites.at(0).getMagnitudeStandard() > 98.) {

                // Magnitude standand inconnue
                if (satellites.at(0).isEclipse()) {
                    ui->magnitudeSat->setText(tr("Satellite non visible (Ombre)"));
                } else {
                    chaine = tr("Illumination : %1%");
                    chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                    if (satellites.at(0).isPenombre())
                        chaine = chaine.append(" ").append(tr("Pénombre"));
                    ui->magnitudeSat->setText(chaine);
                }
            } else {

                // Satellite au dessus de l'horizon
                if (satellites.at(0).isVisible()) {

                    // Satellite eclipse
                    if (satellites.at(0).isEclipse()) {
                        ui->magnitudeSat->setText(tr("Satellite non visible (Ombre)"));
                    } else {
                        chaine = tr("Magnitude (Illumination) : %1%2");
                        chaine = chaine.arg((satellites.at(0).getMagnitude() >= 0.) ? "+" : "-").
                                arg(fabs(satellites.at(0).getMagnitude()), 0, 'f', 1);
                        if (satellites.at(0).isPenombre())
                            chaine = chaine.append("*");
                        chaine = chaine.append((" (%1%)"));
                        chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                        ui->magnitudeSat->setText(chaine);
                    }
                } else {
                    chaine = tr("Satellite non visible");
                    if (satellites.at(0).isEclipse()) {
                        chaine = tr("Satellite non visible (Ombre)");
                    } else {
                        if (satellites.at(0).isPenombre())
                            chaine = chaine.append(" : ").append(tr("Pénombre"));
                        chaine = chaine.append(" (%1%)");
                        chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                    }
                    ui->magnitudeSat->setText(chaine);
                }
            }

            // Conditions d'observation
            const double ht = soleil.getHauteur() * RAD2DEG;
            if (ht >= 0.)
                chaine = tr("Jour");
            else if (ht >= -6.)
                chaine = tr("Crépuscule civil");
            else if (ht >= -12.)
                chaine = tr("Crépuscule nautique");
            else if (ht >= -18.)
                chaine = tr("Crépuscule astronomique");
            else
                chaine = tr("Nuit");
            ui->conditionsObservation->setText(chaine);

            // Nombre d'orbites du satellite
            chaine = "%1";
            ui->nbOrbitesSat->setText(chaine.arg(satellites.at(0).getNbOrbites()));
        }


        /*
         * Donnees relatives au Soleil
         */
        // Hauteur/azimut/distance
        ui->hauteurSoleil->setText(Maths::ToSexagesimal(soleil.getHauteur(), DEGRE, 2, 0, true, true));
        ui->azimutSoleil->setText(Maths::ToSexagesimal(soleil.getAzimut(), DEGRE, 3, 0, false, true));
        chaine = "%1 " + tr("UA");
        chaine = chaine.arg(soleil.getDistanceUA(), 0, 'f', 3);
        ui->distanceSoleil->setText(chaine);

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.getAscensionDroite(), HEURE1, 2, 0, false,
                                                                true).trimmed());
        ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.getDeclinaison(), DEGRE, 2, 0, true, true).trimmed());
        ui->constellationSoleil->setText(soleil.getConstellation());


        /*
         * Donnees relatives a la Lune
         */
        // Hauteur/azimut/distance
        ui->hauteurLune->setText(Maths::ToSexagesimal(lune.getHauteur(), DEGRE, 2, 0, true, true));
        ui->azimutLune->setText(Maths::ToSexagesimal(lune.getAzimut(), DEGRE, 3, 0, false, true));
        chaine = "%1 " + unite1;
        if (ui->unitesKm->isChecked()) {
            chaine = chaine.arg(lune.getDistance(), 0, 'f', 0);
            ui->distanceLune->setText(chaine);
        } else {
            chaine = chaine.arg(lune.getDistance() * MILE_PAR_KM, 0, 'f', 0);
            ui->distanceLune->setText(chaine);
        }

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteLune->setText(Maths::ToSexagesimal(lune.getAscensionDroite(), HEURE1, 2, 0, false,
                                                              true).trimmed());
        ui->declinaisonLune->setText(Maths::ToSexagesimal(lune.getDeclinaison(), DEGRE, 2, 0, true, true).trimmed());
        ui->constellationLune->setText(lune.getConstellation());

        // Illumination/Phase
        chaine = "%1%";
        chaine = chaine.arg(lune.getFractionIlluminee() * 100., 0, 'f', 0);
        ui->illuminationLune->setText(chaine);
        ui->phaseLune->setText(lune.getPhase());


        /*
         * Affichage des donnees sur l'onglet Elements Osculateurs
         */
        ui->utcManuel2->setText(ui->utcManuel->text());
        if (!l1.isEmpty() && !l2.isEmpty()) {

            ui->nomsat2->setText(nom);
            ui->ligne1->setText(l1);
            ui->ligne2->setText(l2);

            // Vecteur d'etat
            // Position
            Vecteur3D vect;
            vect = (ui->unitesKm->isChecked()) ? satellites.at(0).getPosition() : satellites.at(0).getPosition() * MILE_PAR_KM;
            chaine2 = "%1%2 " + unite1;
            chaine = chaine2.arg((vect.getX() >= 0.) ? "+" : "-").arg(fabs(vect.getX()), 0, 'f', 3);
            ui->xsat->setText(chaine);
            chaine = chaine2.arg((vect.getY() >= 0.) ? "+" : "-").arg(fabs(vect.getY()), 0, 'f', 3);
            ui->ysat->setText(chaine);
            chaine = chaine2.arg((vect.getZ() >= 0.) ? "+" : "-").arg(fabs(vect.getZ()), 0, 'f', 3);
            ui->zsat->setText(chaine);

            // Vitesse
            vect = (ui->unitesKm->isChecked()) ? satellites.at(0).getVitesse() : satellites.at(0).getVitesse() * MILE_PAR_KM;
            chaine2 = "%1%2 " + unite2;
            chaine = chaine2.arg((vect.getX() >= 0.) ? "+" : "-").arg(fabs(vect.getX()), 0, 'f', 6);
            ui->vxsat->setText(chaine);
            chaine = chaine2.arg((vect.getY() >= 0.) ? "+" : "-").arg(fabs(vect.getY()), 0, 'f', 6);
            ui->vysat->setText(chaine);
            chaine = chaine2.arg((vect.getZ() >= 0.) ? "+" : "-").arg(fabs(vect.getZ()), 0, 'f', 6);
            ui->vzsat->setText(chaine);

            // Elements osculateurs
            chaine = "%1 " + unite1;
            double xval = (ui->unitesKm->isChecked()) ? satellites.at(0).getElements().getDemiGrandAxe() :
                                                        satellites.at(0).getElements().getDemiGrandAxe() * MILE_PAR_KM;
            chaine = chaine.arg(xval, 0, 'f', 1);
            ui->demiGrandAxe->setText(chaine);

            chaine = "%1";
            chaine = chaine.arg(satellites.at(0).getElements().getExcentricite(), 0, 'f', 7);
            ui->excentricite->setText(chaine);

            chaine2 = "%1°";
            chaine = chaine2.arg(satellites.at(0).getElements().getInclinaison() * RAD2DEG, 0, 'f', 4);
            ui->inclinaison->setText(chaine);

            chaine = chaine2.arg(satellites.at(0).getElements().getAscensionDroiteNA() * RAD2DEG, 0, 'f', 4);
            ui->ADNoeudAscendant->setText(chaine);

            chaine = chaine2.arg(satellites.at(0).getElements().getArgumentPerigee() * RAD2DEG, 0, 'f', 4);
            ui->argumentPerigee->setText(chaine);

            chaine = chaine2.arg(satellites.at(0).getElements().getAnomalieMoyenne() * RAD2DEG, 0, 'f', 4);
            ui->anomalieMoyenne->setText(chaine);

            chaine = chaine2.arg(satellites.at(0).getElements().getAnomalieVraie() * RAD2DEG, 0, 'f', 4);
            ui->anomalieVraie->setText(chaine);

            chaine = chaine2.arg(satellites.at(0).getElements().getAnomalieExcentrique() * RAD2DEG, 0, 'f', 4);
            ui->anomalieExcentrique->setText(chaine);

            chaine = "±" + chaine2.arg(acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellites.at(0).getAltitude())) *
                                       RAD2DEG, 0, 'f', 2);
            ui->champDeVue->setText(chaine);

            // Apogee/perigee/periode orbitale
            chaine2 = "%2 %1 (%3 %1)";
            chaine2 = chaine2.arg(unite1);
            xval = (ui->unitesKm->isChecked()) ? satellites.at(0).getElements().getApogee() :
                                                 satellites.at(0).getElements().getApogee() * MILE_PAR_KM;
            chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval - RAYON_TERRESTRE, 0, 'f', 1);
            ui->apogee->setText(chaine);

            xval = (ui->unitesKm->isChecked()) ? satellites.at(0).getElements().getPerigee() :
                                                 satellites.at(0).getElements().getPerigee() * MILE_PAR_KM;
            chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval - RAYON_TERRESTRE, 0, 'f', 1);
            ui->perigee->setText(chaine);

            ui->periode->setText(Maths::ToSexagesimal(satellites.at(0).getElements().getPeriode() * HEUR2RAD, HEURE1,
                                                      1, 0, false, true));

            if (info) {
                // Affichage des donnees sur l'onglet Informations satellite

                // Nom du satellite
                ui->nomsat3->setText(nom);

                // Lignes du TLE
                ui->line1->setText(ui->ligne1->text());
                ui->line2->setText(ui->ligne2->text());

                // Numero NORAD
                ui->norad->setText(l1.mid(2, 5));

                // Designation COSPAR
                ui->cospar->setText(l1.mid(9, 8).trimmed());

                // Epoque du TLE
                ui->epoque->setText(tles.at(0).getEpoque().ToShortDate(COURT));

                // Nombre d'orbites a l'epoque
                ui->nbOrbitesEpoque->setText(l2.mid(63, 5).trimmed());

                // Anomalie moyenne
                ui->anomalieMoy->setText(l2.mid(43, 8).append("°"));

                // Inclinaison
                ui->inclinaisonMoy->setText(l2.mid(8, 8).append("°"));

                // Excentricite
                ui->excentriciteMoy->setText("0." + l2.mid(26, 7));

                // Ascension droite du noeud ascendant
                ui->ADNoeudAscendantMoy->setText(l2.mid(17, 8).append("°"));

                // Argument du perigee
                ui->argumentPerigeeMoy->setText(l2.mid(34, 8).append("°"));

                // Nombre de revolutions par jour
                ui->nbRev->setText(l2.mid(52, 11));

                // (derivee de nbRev) / 2
                chaine2 = "%1";
                chaine = chaine2.arg(QString(l1.mid(33, 1).trimmed() + "0" + l1.mid(34, 9)).toDouble(), 0, 'f', 8);
                ui->nbRev2->setText(chaine);

                // (derivee seconde de nbRev) / 6
                chaine = chaine2.arg(QString(l1.mid(44, 1) + "0." + l1.mid(45, 5)).toDouble() *
                                     pow(10., QString(l1.mid(50, 2)).toDouble()), 0, 'f', 8);
                ui->nbRev3->setText(chaine);

                // Coefficient pseudo-balistique
                chaine = chaine2.arg(QString(l1.mid(53, 1) + "0." + l1.mid(54, 5)).toDouble() *
                                     pow(10., QString(l1.mid(59, 2)).toDouble()));
                ui->bstar->setText(chaine);

                // Magnitude standard/maximale
                if (satellites.at(0).getMagnitudeStandard() > 98.) {
                    ui->magnitudeStdMax->setText("?/?");
                } else {

                    // Estimation de la magnitude maximale
                    xval = satellites.at(0).getMagnitudeStandard() - 15.75 + 5. *
                            log10(1.45 * (satellites.at(0).getElements().getDemiGrandAxe() *
                                          (1. - satellites.at(0).getElements().getExcentricite()) - RAYON_TERRESTRE));
                    chaine = "%1%2%3/%4%5";
                    chaine = chaine.arg((satellites.at(0).getMagnitudeStandard() >= 0.) ? "+" : "-").
                            arg(fabs(satellites.at(0).getMagnitudeStandard()), 0, 'f', 1).arg(satellites.at(0).getMethMagnitude()).
                            arg((xval >= 0.) ? "+" : "-").arg(fabs(xval), 0, 'f', 1);
                    ui->magnitudeStdMax->setText(chaine);
                }

                // Modele orbital
                chaine = (satellites.at(0).getMethod() == 'd') ? QObject::tr("SGP4 (DS)") : QObject::tr("SGP4 (NE)");
                ui->modele->setText(chaine);

                // Dimensions du satellite
                double t1 = satellites.at(0).getT1();
                double t2 = satellites.at(0).getT2();
                double t3 = satellites.at(0).getT3();
                double section = satellites.at(0).getSection();
                unite1 = QObject::tr("m");
                if (ui->unitesMi->isChecked()) {
                    t1 *= PIED_PAR_METRE;
                    t2 *= PIED_PAR_METRE;
                    t3 *= PIED_PAR_METRE;
                    section = Maths::arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
                    unite1 = QObject::tr("ft");
                }

                chaine2 = "%1 " + unite1;
                if (fabs(t2) < EPSDBL100 && fabs(t3) < EPSDBL100)
                    chaine = QObject::tr("Sphérique. R=").append(chaine2.arg(t1, 0, 'f', 1));
                if (fabs(t2) >= EPSDBL100 && fabs(t3) < EPSDBL100)
                    chaine = QObject::tr("Cylindrique. L=").append(chaine2.arg(t1, 0, 'f', 1)).
                            append(QObject::tr(", R=")).append(chaine2.arg(t2, 0, 'f', 1));
                if (fabs(t2) >= EPSDBL100 && fabs(t3) >= EPSDBL100) {
                    chaine = QObject::tr("Boîte.").append(" %1 x %2 x %3 ").append(unite1);
                    chaine = chaine.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1);
                }
                if (fabs(t1) < EPSDBL100)
                    chaine = QObject::tr("Inconnues");

                if (fabs(section) > EPSDBL100) {
                    chaine = chaine + " / %1 " + unite1;
                    chaine = chaine.arg(section, 0, 'f', 2);
                    ui->sq->setVisible(true);
                } else {
                    ui->sq->setVisible(false);
                }

                ui->dimensions->setText(chaine);
                ui->dimensions->adjustSize();
                ui->sq->move(ui->dimensions->x() + ui->dimensions->width() + 1, ui->sq->y());
                info = false;
            }
        }
    }
    if (scene != NULL)
        scene->deleteLater();
    if (scene2 != NULL)
        scene2->deleteLater();
    if (scene3 != NULL)
        scene3->deleteLater();

    /* Retour */
    return;
}

/*
 * Affichage des elements graphiques
 */
void PreviSat::AffichageCourbes() const
{
    /* Declarations des variables locales */
    int indLune;

    /* Initialisations */
    scene = new QGraphicsScene;
    scene2 = new QGraphicsScene;
    scene3 = new QGraphicsScene;
    scene->setSceneRect(ui->carte->rect());

    bool ht = false;
    for(int j=0; j<nbSat; j++)
        if (satellites.at(j).isVisible())
            ht = true;

    if (!ht) {
        for(int j=0; j<nbSat; j++)
            bipSat[j] = false;
    }

    // Notification sonore
    if (notif && ui->affnotif->isChecked()) {
        if (ui->tempsReel->isChecked())
            QSound::play(dirDat + QDir::separator() + "chimes.wav");
        notif = false;
    }

    // Phase de la Lune
    if (ui->affphaselune->isChecked()) {

        const double ll = atan2(lune.getPosition().getY(), lune.getPosition().getX());
        const double ls = atan2(soleil.getPosition().getY(), soleil.getPosition().getX());

        double diff = (ll - ls) * RAD2DEG;
        if (diff < 0.)
            diff += T360;
        indLune = (int) (diff / 12.19075) + 1;
        if (indLune > 29)
            indLune = 1;
    } else {
        indLune = 15;
    }
    const QString src = ":/resources/lune%1.png";
    QPixmap pixlun;
    pixlun.load(src.arg(indLune, 2, 10, QChar('0')));
    pixlun = pixlun.scaled(17, 17);

    // Couleur du ciel
    QBrush bru(Qt::black);
    QBrush bru2(Qt::black);
    if (!ui->ciel->isHidden() || ui->affradar->isChecked()) {

        const double hts = soleil.getHauteur() * RAD2DEG;
        if (hts >= 0.) {
            // Jour
            bru = QBrush(QColor::fromRgb(213, 255, 254));

        } else {

            const int red = (int) (213.15126 / (1. + 0.0018199 * exp(-0.983684 * hts)) + 0.041477);
            const int green = (int) (qMax(qMin(256.928983 / (1. + 0.008251 * exp(-0.531535 * hts)) - 0.927648, 255.), 0.));

            // Algorithme special pour le bleu
            int blue;
            if (hts >= -6.) {
                blue = 254;
            } else if (hts >= -12.) {
                blue = (int) (-2.74359 * hts * hts - 31.551282 * hts + 163.461538);
            } else {
                blue = (int) (qMax(273.1116 / (1. + 0.0281866 * exp(-0.282853 * hts)) - 1.46635, 0.));
            }
            bru = QBrush(QColor::fromRgb(red, green, blue));
        }
    }
    const QColor crimson(220, 20, 60);
    const QPen noir(Qt::black);
    QPen crayon(Qt::white);

    /* Corps de la methode */
    QRect rect;
    const QString nomMap = (ui->listeMap->currentIndex() == 0) ? ":/resources/map.png" : ficMap.at(ui->listeMap->currentIndex()-1);
    scene->addPixmap(QPixmap(nomMap).scaled(ui->carte->size()));

    if (!ui->carte->isHidden()) {

        const int hcarte = ui->carte->height() - 3;
        const int lcarte = ui->carte->width() - 3;
        const int hcarte2 = qRound(hcarte * 0.5);
        const int lcarte2 = qRound(lcarte * 0.5);

        // Affichage de la grille de coordonnees
        if (ui->affgrille->isChecked()) {

            scene->addLine(0, hcarte2, lcarte, hcarte2, QPen(Qt::white));
            scene->addLine(lcarte2, 0, lcarte2, hcarte, QPen(Qt::white));

            QPen style(Qt::lightGray);
            const int tablat[] = { hcarte / 6, hcarte / 3, (int) (hcarte / 1.5), (int) (hcarte / 1.2) };
            const int tablon[] = { lcarte / 12, lcarte / 6, (int) (lcarte * 0.25), lcarte / 3, (int) (lcarte / 2.4),
                                   (int) (7. * lcarte / 12.), (int) (lcarte / 1.5), (int) (lcarte * 0.75), (int) (lcarte / 1.2),
                                   (int) (11. * lcarte / 12.) };

            for(int j=0; j<4; j++)
                scene->addLine(0, tablat[j], lcarte, tablat[j], style);

            for(int j=0; j<10; j++)
                scene->addLine(tablon[j], 0, tablon[j], hcarte, style);

            // Tropiques
            style.setStyle(Qt::DashLine);
            scene->addLine(0, 66.55 * DEG2PXVT, lcarte, 66.55 * DEG2PXVT, style);
            scene->addLine(0, 113.45 * DEG2PXVT, lcarte, 113.45 * DEG2PXVT, style);

            if (!ui->carte->isHidden()) {
                ui->frameLat->setVisible(true);
                ui->frameLon->setVisible(true);
            }
        } else {
            ui->frameLat->setVisible(false);
            ui->frameLon->setVisible(false);
        }

        // Affichage de la SAA
        if (ui->affSAA->isChecked()) {

            const QBrush alpha = QBrush(QColor::fromRgb(255, 0, 0, 50));
            QVector<QPoint> zoneSAA;
            zoneSAA.resize(59);

            for(int i=0; i<zoneSAA.size(); i++) {
                zoneSAA[i].setX(qRound((180. + tabSAA[i][0]) * DEG2PXHZ));
                zoneSAA[i].setY(qRound((90. - tabSAA[i][1]) * DEG2PXVT));
            }

            const QPolygonF poly1(zoneSAA);
            scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
        }

        // Affichage du Soleil
        const int lsol = qRound((180. - soleil.getLongitude() * RAD2DEG) * DEG2PXHZ);
        const int bsol = qRound((90. - soleil.getLatitude() * RAD2DEG) * DEG2PXVT);

        if (ui->affsoleil->isChecked()) {
            rect = QRect(lsol - 7, bsol - 7, 15, 15);
            scene->addEllipse(rect, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
        }

        // Zone d'ombre
        if (ui->affnuit->isChecked()) {

            int jmin = 0;
            int xmin = ui->carte->width() - 3;
            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, (int) (2.55 * ui->intensiteOmbre->value())));

            QVector<QPoint> zone;
            zone.resize(361);
            for(int j=0; j<361; j++) {
                zone[j].setX(qRound(soleil.getZone().at(j).x() * DEG2PXHZ));
                zone[j].setY(qRound(soleil.getZone().at(j).y() * DEG2PXVT));

                if (soleil.getLatitude() < 0.) {
                    if (zone[j].x() <= xmin) {
                        xmin = zone[j].x();
                        jmin = j;
                    }
                } else {
                    if (zone[j].x() < xmin) {
                        xmin = zone[j].x();
                        jmin = j;
                    }
                }
            }

            if (fabs(soleil.getLatitude()) > 0.002449 * DEG2RAD) {

                zone.resize(366);
                if (soleil.getLatitude() < 0.) {

                    for(int j=3; j<363; j++) {
                        zone[j].setX(qRound(soleil.getZone().at((j+jmin-2)%360).x() * DEG2PXHZ)+1);
                        zone[j].setY(qRound(soleil.getZone().at((j+jmin-2)%360).y() * DEG2PXVT)+1);
                    }
                    zone[0] = QPoint(ui->carte->width() - 1, 0);
                    zone[1] = QPoint(ui->carte->width() - 1, hcarte + 2);
                    zone[2] = QPoint(ui->carte->width() - 1, 1 + qRound(0.5 * (zone[3].y() + zone[362].y())));

                    zone[363] = QPoint(0, 1 + qRound(0.5 * (zone[3].y() + zone[362].y())));
                    zone[364] = QPoint(0, hcarte + 2);
                    zone[365] = QPoint(0, 0);

                } else {

                    for(int j=2; j<362; j++) {
                        zone[j].setX(qRound(soleil.getZone().at((j+jmin-2)%360).x() * DEG2PXHZ)+1);
                        zone[j].setY(qRound(soleil.getZone().at((j+jmin-2)%360).y() * DEG2PXVT)+1);
                    }

                    zone[0] = QPoint(0, 0);
                    zone[1] = QPoint(0, 1 + qRound(0.5 * (zone[2].y() + zone[361].y())));

                    zone[362] = QPoint(ui->carte->width() - 1, 1 + qRound(0.5 * (zone[2].y() + zone[361].y())));
                    zone[363] = QPoint(ui->carte->width() - 1, 0);
                    zone[364] = QPoint(ui->carte->width() - 1, hcarte + 2);
                    zone[365] = QPoint(0, hcarte + 2);
                }

                const QPolygonF poly(zone);
                scene->addPolygon(poly, QPen(Qt::NoBrush, 0), alpha);

            } else {

                QVector<QPoint> zone1;
                zone1.resize(4);
                const int x1 = qRound(qMin(soleil.getZone().at(90).x(), soleil.getZone().at(270).x()) * DEG2PXHZ) + 1;
                const int x2 = qRound(qMax(soleil.getZone().at(90).x(), soleil.getZone().at(270).x()) * DEG2PXHZ) + 1;

                zone1.resize(4);
                if (lsol > lcarte / 4 && lsol < (3 * lcarte) / 4) {

                    QVector<QPoint> zone2;

                    zone2.resize(4);

                    zone1[0] = QPoint(0, 0);
                    zone1[1] = QPoint(x1, 0);
                    zone1[2] = QPoint(x1, hcarte + 2);
                    zone1[3] = QPoint(0, hcarte + 2);

                    zone2[0] = QPoint(ui->carte->width() - 1, 0);
                    zone2[1] = QPoint(x2, 0);
                    zone2[2] = QPoint(x2, hcarte + 2);
                    zone2[3] = QPoint(lcarte + 1, hcarte + 2);

                    const QPolygonF poly1(zone1);
                    const QPolygonF poly2(zone2);
                    scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
                    scene->addPolygon(poly2, QPen(Qt::NoBrush, 0), alpha);

                } else {

                    zone1[0] = QPoint(x1, 0);
                    zone1[1] = QPoint(x1, hcarte + 2);
                    zone1[2] = QPoint(x2, hcarte + 2);
                    zone1[3] = QPoint(x2, 0);

                    const QPolygonF poly1(zone1);
                    scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
                }
            }
        }

        // Affichage de la Lune
        if (ui->afflune->isChecked()) {

            const int llun = qRound((180. - lune.getLongitude() * RAD2DEG) * DEG2PXHZ);
            const int blun = qRound((90. - lune.getLatitude() * RAD2DEG) * DEG2PXVT);

            QGraphicsPixmapItem *lun = scene->addPixmap(pixlun);
            QTransform transform;
            transform.translate(llun - 7, blun - 7);
            if (ui->rotationLune->isChecked() && observateurs.at(0).getLatitude() < 0.)
                transform.rotate(180.);
            lun->setTransform(transform);
        }

        // Lieux d'observation
        int nbMax;
        if (ui->affnomlieu->checkState() == Qt::Unchecked)
            nbMax = 0;
        if (ui->affnomlieu->checkState() == Qt::Checked || ui->affnomlieu->checkState() == Qt::PartiallyChecked)
            nbMax = ui->lieuxObservation1->count() - 1;

        for(int j=nbMax; j>=0; j--) {

            const int lobs = qRound((180. - observateurs.at(j).getLongitude() * RAD2DEG) * DEG2PXHZ);
            const int bobs = qRound((90. - observateurs.at(j).getLatitude() * RAD2DEG) * DEG2PXVT);

            scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
            scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

            if (j == 0 || ui->affnomlieu->checkState() == Qt::Checked) {

                QGraphicsSimpleTextItem *txtObs = new QGraphicsSimpleTextItem(observateurs.at(j).getNomlieu());
                const int lng = txtObs->boundingRect().width();
                const int xnobs = (lobs + 4 + lng > lcarte) ? lobs - lng - 1 : lobs + 4;
                const int ynobs = (bobs + 9 > hcarte) ? bobs - 12 : bobs;

                txtObs->setBrush(Qt::white);
                txtObs->setPos(xnobs, ynobs);
                scene->addItem(txtObs);
            }
        }

        // Affichage de la trace au sol du satellite par defaut
        if (ui->afftraj->isChecked()) {
            if (nbSat > 0) {
                if (!satellites.at(0).isIeralt()) {
                    int lsat1 = qRound(satellites.at(0).getTraceAuSol().at(0).at(0) * DEG2PXHZ);
                    int bsat1 = qRound(satellites.at(0).getTraceAuSol().at(0).at(1) * DEG2PXVT);

                    const QColor bleuClair(173, 216, 230);

                    for(int j=1; j<satellites.at(0).getTraceAuSol().size()-1; j++) {
                        int lsat2 = qRound(satellites.at(0).getTraceAuSol().at(j).at(0) * DEG2PXHZ);
                        int bsat2 = qRound(satellites.at(0).getTraceAuSol().at(j).at(1) * DEG2PXVT);
                        int ils = 99999;

                        if (fabs(lsat2 - lsat1) > lcarte2) {
                            if (lsat2 < lsat1)
                                lsat2 += lcarte;
                            else
                                lsat1 += lcarte;
                            ils = j;
                        }

                        crayon = (satellites.at(0).getTraceAuSol().at(j).at(2) == 0) ? bleuClair : crimson;
                        scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        if (ils == j) {
                            lsat1 -= lcarte;
                            lsat2 -= lcarte;
                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);
                            ils = 0;
                        }
                        lsat1 = qRound(satellites.at(0).getTraceAuSol().at(j).at(0) * DEG2PXHZ);
                        bsat1 = qRound(satellites.at(0).getTraceAuSol().at(j).at(1) * DEG2PXVT);
                    }
                }
            }
        }

        // Affichage de la zone de visibilite des satellites
        if (nbSat > 0) {
            if (ui->affvisib->isChecked()) {
                if (ui->affvisib->checkState() == Qt::PartiallyChecked)
                    nbMax = 1;
                if (ui->affvisib->checkState() == Qt::Checked)
                    nbMax = liste.size();

                crayon = QPen(Qt::white);
                for(int isat=0; isat<nbMax; isat++) {

                    if (!satellites.at(isat).isIeralt()) {

                        int lsat1 = qRound(satellites.at(isat).getZone().at(0).x() * DEG2PXHZ);
                        int bsat1 = qRound(satellites.at(isat).getZone().at(0).y() * DEG2PXVT);

                        for(int j=1; j<361; j++) {
                            int lsat2 = qRound(satellites.at(isat).getZone().at(j).x() * DEG2PXHZ);
                            int bsat2 = qRound(satellites.at(isat).getZone().at(j).y() * DEG2PXVT);
                            int ils = 99999;

                            if (fabs(lsat2 - lsat1) > lcarte2) {
                                if (lsat2 < lsat1)
                                    lsat2 += lcarte;
                                else
                                    lsat1 += lcarte;
                                ils = j;
                            }

                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                            if (ils == j) {
                                lsat1 -= lcarte + 1;
                                lsat2 -= lcarte + 1;
                                scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);
                                ils = 0;
                            }
                            lsat1 = qRound(satellites.at(isat).getZone().at(j).x() * DEG2PXHZ);
                            bsat1 = qRound(satellites.at(isat).getZone().at(j).y() * DEG2PXVT);
                        }
                    }
                }
            }
        }

        QColor col;
        for(int isat=nbSat-1; isat>=0; isat--) {

            // Dessin des satellites
            if (!satellites.at(isat).isIeralt()) {

                const int lsat = qRound((180. - satellites.at(isat).getLongitude() * RAD2DEG) * DEG2PXHZ);
                const int bsat = qRound((90. - satellites.at(isat).getLatitude() * RAD2DEG) * DEG2PXVT);

                rect = QRect(lsat - 3, bsat - 3, 6, 6);
                col = (satellites.at(isat).isEclipse()) ? crimson : Qt::yellow;
                scene->addEllipse(rect, noir, QBrush(col, Qt::SolidPattern));

                // Nom des satellites
                if (ui->affnomsat->isChecked()) {

                    if ((ui->affnomsat->checkState() == Qt::PartiallyChecked && isat == 0) ||
                            ui->affnomsat->checkState() == Qt::Checked) {
                        QGraphicsSimpleTextItem *txtSat = new QGraphicsSimpleTextItem(tles.at(isat).getNom());
                        const int lng = txtSat->boundingRect().width();
                        const int xnsat = (lsat + 4 + lng > lcarte) ? lsat - lng - 1 : lsat + 4;
                        const int ynsat = (bsat + 9 > hcarte) ? bsat - 12 : bsat;

                        txtSat->setBrush(Qt::white);
                        txtSat->setPos(xnsat, ynsat);
                        scene->addItem(txtSat);
                    }
                }
            }
        }
        ui->carte->setScene(scene);
        QGraphicsView view(scene);
        view.setRenderHints(QPainter::Antialiasing);

    } else {

        // Affichage de la carte du ciel
        rect = QRect(0, 0, ui->ciel->width()-1, ui->ciel->height()-1);
        scene3->setSceneRect(rect);

        scene3->setBackgroundBrush(QBrush(PreviSat::palette().background().color()));
        QPen pen(bru, Qt::SolidPattern);
        scene3->addEllipse(rect, pen, bru);
        const int lciel = qRound(0.5 * ui->ciel->width());
        const int hciel = qRound(0.5 * ui->ciel->height());

        // Affichage des constellations
        if (ui->affconst->checkState() != Qt::Unchecked) {

            QListIterator<LigneConstellation> it(lignesCst);
            while (it.hasNext()) {

                const LigneConstellation lig = it.next();
                if (lig.isDessin()) {

                    // Calcul des coordonnees radar des etoiles pour le dessin de la constellation
                    const int lstr1 = qRound(lciel - lciel * (1. - lig.getEtoile1().getHauteur() * DEUX_SUR_PI) *
                                             sin(lig.getEtoile1().getAzimut()));
                    const int bstr1 = qRound(hciel - hciel * (1. - lig.getEtoile1().getHauteur() * DEUX_SUR_PI) *
                                             cos(lig.getEtoile1().getAzimut()));

                    const int lstr2 = qRound(lciel - lciel * (1. - lig.getEtoile2().getHauteur() * DEUX_SUR_PI) *
                                             sin(lig.getEtoile2().getAzimut()));
                    const int bstr2 = qRound(hciel - hciel * (1. - lig.getEtoile2().getHauteur() * DEUX_SUR_PI) *
                                             cos(lig.getEtoile2().getAzimut()));

                    QColor col;
                    col.setNamedColor("deepskyblue");
                    crayon = QPen(col);
                    if ((lstr2 - lstr1) * (lstr2 - lstr1) + (bstr2 - bstr1) * (bstr2 - bstr1) < lciel * ui->ciel->height())
                        scene3->addLine(lstr1, bstr1, lstr2, bstr2, crayon);
                }
            }

            // Affichage du nom des constellations
            if (ui->affconst->checkState() == Qt::Checked) {

                if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {
                    QListIterator<Constellation> it2(constellations);
                    while (it2.hasNext()) {

                        const Constellation cst = it2.next();
                        if (cst.isVisible()) {

                            // Calcul des coordonnees radar du label
                            const int lcst = qRound(lciel - lciel * (1. - cst.getHauteur() * DEUX_SUR_PI) * sin(cst.getAzimut()));
                            const int bcst = qRound(hciel - hciel * (1. - cst.getHauteur() * DEUX_SUR_PI) * cos(cst.getAzimut()));

                            const int lst = lcst - lciel;
                            const int bst = hciel - bcst;

                            QGraphicsSimpleTextItem *txtCst = new QGraphicsSimpleTextItem(cst.getNom());
                            const int lng = txtCst->boundingRect().width();

                            const int xncst = (sqrt((lst + lng) * (lst + lng) + bst * bst) > lciel) ? lcst - lng - 1 : lcst + 1;
                            const int yncst = (bcst + 9 > ui->ciel->height()) ? bcst - 10 : bcst + 1;

                            txtCst->setBrush(QBrush(Qt::darkYellow));
                            txtCst->setPos(xncst, yncst);
                            txtCst->setFont(QFont(PreviSat::font().family(), 8));
                            scene3->addItem(txtCst);
                        }
                    }
                }
            }
        }

        // Affichage des etoiles
        QListIterator<Etoile> it1(etoiles);
        while (it1.hasNext()) {

            const Etoile etoile = it1.next();
            if (etoile.isVisible() && etoile.getMagnitude() <= ui->magnitudeEtoiles->value()) {

                const int lstr = qRound(lciel - lciel * (1. - etoile.getHauteur() * DEUX_SUR_PI) * sin(etoile.getAzimut()));
                const int bstr = qRound(hciel - hciel * (1. - etoile.getHauteur() * DEUX_SUR_PI) * cos(etoile.getAzimut()));

                rect = (etoile.getMagnitude() > 3.) ? QRect(lstr-1, bstr-1, 2, 2) : QRect(lstr-1., bstr-1., 2.5, 3.5);
                bru2 = (soleil.getHauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);
                scene3->addEllipse(rect, QPen(Qt::NoPen), bru2);

                // Nom des etoiles les plus brillantes
                if (ui->affetoiles->isChecked()) {
                    if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {
                        if (etoile.getMagnitude() < ui->magnitudeEtoiles->value() - 1.9) {
                            if (!etoile.getNom().isEmpty() && etoile.getNom().at(0).isUpper()) {

                                const int lst = lstr - lciel;
                                const int bst = hciel - bstr;
                                const QString nomstr = etoile.getNom().mid(0, 1) + etoile.getNom().mid(1).toLower();
                                QGraphicsSimpleTextItem *txtStr = new QGraphicsSimpleTextItem(nomstr);
                                const int lng = txtStr->boundingRect().width();

                                const int xnstr = (sqrt((lst + lng) * (lst + lng) + bst * bst) > lciel) ? lstr - lng - 1 : lstr + 1;
                                const int ynstr = (bstr + 9 > ui->ciel->height()) ? bstr - 10 : bstr + 1;

                                txtStr->setBrush(bru2);
                                txtStr->setPos(xnstr, ynstr);
                                txtStr->setFont(QFont(PreviSat::font().family(), 7));
                                scene3->addItem(txtStr);
                            }
                        }
                    }
                }
            }
        }

        if (ui->affplanetes->checkState() != Qt::Unchecked) {

            // Calcul des coordonnees radar des planetes
            for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

                if (planetes.at(iplanete).getHauteur() >= 0.) {

                    const int lpla = qRound(lciel - lciel * (1. - planetes.at(iplanete).getHauteur() * DEUX_SUR_PI) *
                                            sin(planetes.at(iplanete).getAzimut()));
                    const int bpla = qRound(hciel - hciel * (1. - planetes.at(iplanete).getHauteur() * DEUX_SUR_PI) *
                                            cos(planetes.at(iplanete).getAzimut()));

                    const QBrush bru3(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                    rect = QRect(lpla - 2, bpla - 2, 4, 4);
                    scene3->addEllipse(rect, QPen(couleurPlanetes[iplanete]), bru3);

                    if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored &&
                            ui->affplanetes->checkState() == Qt::Checked) {
                        const int lpl = lpla - lciel;
                        const int bpl = hciel - bpla;
                        const QString nompla = planetes.at(iplanete).getNom();
                        QGraphicsSimpleTextItem *txtPla = new QGraphicsSimpleTextItem(nompla);
                        const int lng = txtPla->boundingRect().width();

                        const int xnpla = (sqrt((lpl + lng) * (lpl + lng) + bpl * bpl) > lciel) ? lpla - lng - 1 : lpla + 3;
                        const int ynpla = (bpla + 9 > ui->ciel->height()) ? bpla - 10 : bpla + 2;

                        txtPla->setBrush(bru3);
                        txtPla->setPos(xnpla, ynpla);
                        txtPla->setFont(QFont(PreviSat::font().family(), 7));
                        scene3->addItem(txtPla);
                    }
                }
            }
        }

        if (ui->affsoleil->isChecked()) {

            // Dessin de l'ecliptique
            if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {

                const double ad1 = tabEcliptique[0][0] * HEUR2RAD;
                const double de1 = tabEcliptique[0][1] * DEG2RAD;
                const double cd1 = cos(de1);
                Vecteur3D vec = Vecteur3D(cos(ad1) * cd1, sin(ad1) * cd1, sin(de1));
                Vecteur3D vec1 = observateurs.at(0).getRotHz() * vec;

                double ht1 = asin(vec1.getZ());
                double az1 = atan2(vec1.getY(), -vec1.getX());
                if (az1 < 0.)
                    az1 += DEUX_PI;

                int lecl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                int becl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                for(int i=1; i<49; i++) {

                    const double ad2 = tabEcliptique[i][0] * HEUR2RAD;
                    const double de2 = tabEcliptique[i][1] * DEG2RAD;
                    const double cd2 = cos(de2);
                    vec = Vecteur3D(cos(ad2) * cd2, sin(ad2) * cd2, sin(de2));
                    Vecteur3D vec2 = observateurs.at(0).getRotHz() * vec;

                    const double ht2 = asin(vec2.getZ());

                    double az2 = atan2(vec2.getY(), -vec2.getX());
                    if (az2 < 0.)
                        az2 += DEUX_PI;

                    const int lecl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                    const int becl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                    if (ht1 >= 0. || ht2 >= 0.)
                        scene3->addLine(lecl1, becl1, lecl2, becl2, QPen(Qt::darkYellow));

                    lecl1 = lecl2;
                    becl1 = becl2;
                    ht1 = ht2;
                }
            }

            if (soleil.isVisible()) {
                // Calcul des coordonnees radar du Soleil
                const int lsol = qRound(lciel - lciel * (1. - soleil.getHauteur() * DEUX_SUR_PI) * sin(soleil.getAzimut()));
                const int bsol = qRound(hciel - hciel * (1. - soleil.getHauteur() * DEUX_SUR_PI) * cos(soleil.getAzimut()));

                rect = QRect(lsol - 7, bsol - 7, 15, 15);
                scene3->addEllipse(rect, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
            }
        }

        if (ui->afflune->isChecked() && lune.isVisible()) {

            // Calcul des coordonnees radar de la Lune
            const int llun = qRound(lciel - lciel * (1. - lune.getHauteur() * DEUX_SUR_PI) * sin(lune.getAzimut()));
            const int blun = qRound(hciel - hciel * (1. - lune.getHauteur() * DEUX_SUR_PI) * cos(lune.getAzimut()));

            QGraphicsPixmapItem *lun = scene3->addPixmap(pixlun);
            QTransform transform;
            transform.translate(llun - 7, blun - 7);
            if (ui->rotationLune->isChecked() && observateurs.at(0).getLatitude() < 0.)
                transform.rotate(180.);
            lun->setTransform(transform);
        }

        for(int isat=nbSat-1; isat>=0; isat--) {

            if (satellites.at(isat).isVisible() && !satellites.at(isat).isIeralt()) {

                // Affichage de la trace dans le ciel
                if (satellites.at(isat).getTraceCiel().size() > 0) {

                    const QList<QVector<double> > trace = satellites.at(isat).getTraceCiel();
                    const double ht1 = trace.at(0).at(0);
                    const double az1 = trace.at(0).at(1);
                    int lsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                    int bsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                    const QColor bleuClair(173, 216, 230);

                    for(int i=1; i<trace.size(); i++) {

                        double ht2 = trace.at(i).at(0);
                        double az2 = trace.at(i).at(1);

                        crayon = (trace.at(i).at(2) == 0) ? bleuClair : crimson;

                        const int lsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                        const int bsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                        scene3->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        lsat1 = lsat2;
                        bsat1 = bsat2;
                    }
                }

                // Calcul des coordonnees radar du satellite
                const int lsat = qRound(lciel - lciel * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                        sin(satellites.at(isat).getAzimut()));
                const int bsat = qRound(hciel - hciel * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                        cos(satellites.at(isat).getAzimut()));

                rect = QRect(lsat - 3, bsat - 3, 6, 6);
                const QColor col = (satellites.at(isat).isEclipse()) ? crimson : Qt::yellow;
                scene3->addEllipse(rect, noir, QBrush(col, Qt::SolidPattern));
            }
        }

        scene3->addEllipse(-10, -10, ui->ciel->width() + 20, ui->ciel->height() + 20,
                           QPen(QBrush(PreviSat::palette().background().color()), 20.6));
        scene3->addEllipse(0, 0, ui->ciel->width() - 1, ui->ciel->height() - 1, QPen(QBrush(Qt::gray), 2));

        ui->ciel->setScene(scene3);
        QGraphicsView view3(scene3);
        view3.setRenderHints(QPainter::Antialiasing);
    }

    // Radar
    htr = false;
    if (ui->affradar->checkState() == Qt::Checked || (ui->affradar->checkState() == Qt::PartiallyChecked && ht)) {

        ui->coordGeo1->setVisible(true);
        ui->coordGeo2->setVisible(true);
        ui->coordGeo3->setVisible(true);
        ui->coordGeo4->setVisible(true);
        ui->radar->setVisible(true);
        htr = true;

        // Dessin du fond du radar
        rect = QRect(0, 0, 200, 200);

        scene2->setBackgroundBrush(QBrush(ui->frameZone->palette().background().color()));
        QPen pen(bru, Qt::SolidPattern);
        scene2->addEllipse(rect, pen, bru);

        // Dessin des cercles concentriques
        scene2->addEllipse(33, 33, 133, 133, QPen(Qt::gray));
        scene2->addEllipse(67, 67, 67, 67, QPen(Qt::gray));

        // Dessin des segments
        scene2->addLine(0, 100, 200, 100, QPen(Qt::gray));
        scene2->addLine(100, 0, 100, 200, QPen(Qt::gray));
        scene2->addLine(13, 50, 187, 150, QPen(Qt::gray));
        scene2->addLine(13, 150, 187, 50, QPen(Qt::gray));
        scene2->addLine(50, 13, 150, 187, QPen(Qt::gray));
        scene2->addLine(50, 187, 150, 13, QPen(Qt::gray));

        // Inversion des coordonnees du Soleil et du satellite
        int xf, yf;
        if (ui->affinvns->isChecked()) {
            ui->coordGeo1->setText(tr("Sud"));
            ui->coordGeo2->setText(tr("Nord"));
            yf = -1;
        } else {
            ui->coordGeo1->setText(tr("Nord"));
            ui->coordGeo2->setText(tr("Sud"));
            yf = 1;
        }
        if (ui->affinvew->isChecked()) {
            ui->coordGeo3->setText(tr("Est"));
            ui->coordGeo4->setText(tr("Ouest"));
            xf = 1;
        } else {
            ui->coordGeo3->setText(tr("Ouest"));
            ui->coordGeo4->setText(tr("Est"));
            xf = -1;
        }

        if (ui->affsoleil->isChecked() && soleil.isVisible()) {

            // Calcul des coordonnees radar du Soleil
            const int lsol = qRound(100. - 100. * xf * (1. - soleil.getHauteur() * DEUX_SUR_PI) * sin(soleil.getAzimut()));
            const int bsol = qRound(100. - 100. * yf * (1. - soleil.getHauteur() * DEUX_SUR_PI) * cos(soleil.getAzimut()));

            rect = QRect(lsol - 7, bsol - 7, 15, 15);
            scene2->addEllipse(rect, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
        }

        if (ui->afflune->isChecked() && lune.isVisible()) {

            // Calcul des coordonnees radar de la Lune
            const int llun = qRound(100. - 100. * xf * (1. - lune.getHauteur() * DEUX_SUR_PI) * sin(lune.getAzimut()));
            const int blun = qRound(100. - 100. * yf * (1. - lune.getHauteur() * DEUX_SUR_PI) * cos(lune.getAzimut()));

            QGraphicsPixmapItem *lun = scene2->addPixmap(pixlun);
            QTransform transform;
            transform.translate(llun - 7, blun - 7);
            if (ui->rotationLune->isChecked() && observateurs.at(0).getLatitude() < 0.)
                transform.rotate(180.);
            lun->setTransform(transform);
        }

        for(int isat=nbSat-1; isat>=0; isat--) {

            if (satellites.at(isat).isVisible() && !satellites.at(isat).isIeralt()) {

                // Calcul des coordonnees radar du satellite
                const int lsat = qRound(100. - 100. * xf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) * sin(satellites.at(isat).getAzimut()));
                const int bsat = qRound(100. - 100. * yf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) * cos(satellites.at(isat).getAzimut()));

                rect = QRect(lsat - 3, bsat - 3, 6, 6);
                const QColor col = (satellites.at(isat).isEclipse()) ? crimson : Qt::yellow;
                scene2->addEllipse(rect, noir, QBrush(col, Qt::SolidPattern));
            }
        }

        // Cercle exterieur du radar
        scene2->addEllipse(0, 0, 200, 200, QPen(QBrush(Qt::gray), 2.1));
        scene2->addEllipse(-6, -6, 212, 212, QPen(QBrush(ui->frameZone->palette().background().color()), 11));
        ui->radar->setScene(scene2);
    } else {
        ui->coordGeo1->setVisible(false);
        ui->coordGeo2->setVisible(false);
        ui->coordGeo3->setVisible(false);
        ui->coordGeo4->setVisible(false);
        ui->radar->setVisible(false);
    }

    /* Retour */
    return;
}

/*
 * Affichage des groupes de TLE
 */
void PreviSat::AffichageGroupesTLE() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(dirDat + QDir::separator() + "gestionnaireTLE.gst");
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    while (!flux.atEnd()) {
        const QStringList ligne = flux.readLine().split("#");
        QString nomGroupe = ligne.at(0);
        nomGroupe[0] = nomGroupe[0].toUpper();
        ui->groupeTLE->addItem(nomGroupe);

        if (ligne.at(1) == "1")
            listeGroupeMaj.append(ligne.at(0) + "#" + ligne.at(2));
    }
    fi.close();
    ui->groupeTLE->setCurrentIndex(settings.value("affichage/groupeTLE", 0).toInt());

    /* Retour */
    return;
}

/*
 * Affichage du lieu d'observation sur l'interface graphique
 */
void PreviSat::AffichageLieuObs() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    listeObs = settings.value("observateur/lieu", "Paris#-002.345277778&+48.860833333&60").toString().split("$");

    /* Corps de la methode */
    ui->lieuxObservation1->clear();
    ui->lieuxObservation2->clear();
    ui->lieuxObservation3->clear();
    ui->lieuxObservation4->clear();
    ui->selecLieux->clear();
    observateurs.clear();
    QStringListIterator it(listeObs);
    while (it.hasNext()) {

        const QString obs = it.next();
        const int delim = obs.indexOf("#");
        const QString nomlieu = obs.mid(0, delim).trimmed();

        ui->lieuxObservation1->addItem(nomlieu);
        ui->lieuxObservation2->addItem(nomlieu);
        ui->lieuxObservation3->addItem(nomlieu);
        ui->lieuxObservation4->addItem(nomlieu);
        ui->selecLieux->addItem(nomlieu);

        // Longitude/Latitude/Altitude
        const QStringList coord = obs.mid(delim + 1).split("&");
        const double lo = coord.at(0).toDouble();
        const double la = coord.at(1).toDouble();
        const int atd = coord.at(2).toInt();

        // Creation du lieu d'observation
        observateurs.append(Observateur(nomlieu, lo, la, atd));

        if (obs == listeObs.first()) {

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");
            const QString alt = "%1 %2";

            // Affichage des coordonnees
            ui->longitudeObs->setText(Maths::ToSexagesimal(fabs(lo) * DEG2RAD, DEGRE, 3, 0, false, true).append(" ").append(ew));
            ui->latitudeObs->setText(Maths::ToSexagesimal(fabs(la) * DEG2RAD, DEGRE, 2, 0,false, true).append(" ").append(ns));
            ui->altitudeObs->setText(alt.arg((ui->unitesKm->isChecked()) ? atd : atd * PIED_PAR_METRE).
                                     arg((ui->unitesKm->isChecked()) ? tr("m") : tr("ft")));
        }
    }
    ui->lieuxObservation1->setCurrentIndex(0);
    ui->lieuxObservation2->setCurrentIndex(0);
    ui->lieuxObservation3->setCurrentIndex(0);
    ui->lieuxObservation4->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Affichage des noms des satellites dans les listes
 */
void PreviSat::AfficherListeSatellites(const QString &fichier, const QStringList &listeSat) const
{
    /* Declarations des variables locales */
    QString magn;

    /* Initialisations */
    nbSat = 0;
    QString nomsat = "---";
    QFile donneesSatellites(dirDat + QDir::separator() + "donnees.sat");
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&donneesSatellites);
        magn = flux.readAll();
    } else {
        magn = "";
    }
    donneesSatellites.close();

    /* Corps de la methode */
    // NB : le fichier doit etre verifie au prealable
    ui->liste1->clear();
    ui->liste2->clear();
    ui->liste3->clear();
    QFile fichierTLE(fichier);
    if (fichierTLE.exists()) {

        QString li1, li2;
        QListWidgetItem *elem0 = new QListWidgetItem();
        fichierTLE.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichierTLE);

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();

            if (ligne.at(0) == '1') {

                li1 = ligne;

                do {
                    li2 = flux.readLine();
                } while (li2.trimmed().length() == 0);

                const QString norad = li1.mid(2, 5);
                if (nomsat.at(0) == '1' || nomsat == "---") {

                    const int indx1 = magn.indexOf(norad);
                    if (indx1 >= 0) {
                        const int indx2 = magn.indexOf("\n", indx1) - indx1;
                        nomsat = magn.mid(indx1 + 36, indx2 - 36).trimmed();
                    } else {
                        nomsat = norad;
                    }
                }

                if (nomsat.length() > 25 && nomsat.mid(25).contains("."))
                    nomsat = nomsat.mid(0, 15).trimmed();
                if (nomsat.toLower() == "iss (zarya)")
                    nomsat = "ISS ";
                if (nomsat.toLower().contains("iridium") && nomsat.contains("["))
                    nomsat = nomsat.mid(0, nomsat.indexOf("[")).trimmed();

                bool check = false;
                for (int j=0; j<listeSat.length(); j++) {
                    if (norad == listeSat.at(j)) {
                        check = true;
                        nbSat++;
                        if (j == 0) {
                            settings.setValue("TLE/nom", nomsat);
                            settings.setValue("TLE/l1", li1);
                            settings.setValue("TLE/l2", li2);
                            nom = nomsat;

                            l1 = li1;
                            l2 = li2;
                        }
                        break;
                    }
                }

                // Ajout du satellite dans la liste de satellites
                const QString nomsat2 = nomsat + "\t\t\t\t#" + norad;
                QListWidgetItem *elem1 = new QListWidgetItem(nomsat2, ui->liste1);
                elem1->setFlags(Qt::ItemIsEnabled);
                elem1->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                QListWidgetItem *elem2 = new QListWidgetItem(nomsat2, ui->liste2);
                elem2->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                QListWidgetItem *elem3 = new QListWidgetItem(nomsat2, ui->liste3);
                elem3->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                if (nomsat == nom)
                    elem0 = elem1;
            }
            nomsat = ligne.trimmed();
        }
        if (l1.length() > 0) {
            li1 = l1;
            li2 = l2;
            ui->liste1->setCurrentItem(elem0);
            ui->liste2->setCurrentRow(ui->liste1->currentRow());
            ui->liste3->setCurrentRow(ui->liste1->currentRow());
            l1 = li1;
            l2 = li2;
        }
    }

    /* Retour */
    return;
}

/*
 * Enchainement des calculs et affichage
 */
void PreviSat::CalculsAffichage()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Satellite::initCalcul = false;

    // Enchainement de l'ensemble des calculs
    EnchainementCalculs();

    // Donnees numeriques
    AffichageDonnees();

    // Affichage des elements graphiques
    AffichageCourbes();

    /* Retour */
    return;
}


/***********
 * Calculs *
 **********/

/*
 * Enchainement des calculs
 */
void PreviSat::EnchainementCalculs() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    // Nombre de traces au sol a afficher
    const int nbTraces = (ui->afftraj->isChecked()) ? ui->nombreTrajectoires->value() : 0;

    // Prise en compte de l'extinction atmospherique
    const bool extinction = ui->extinctionAtmospherique->isChecked();

    // Calcul de la zone de visibilite des satellites
    const bool visibilite = !ui->carte->isHidden();

    // Calcul de la trace dans le ciel
    const bool traceCiel = (ui->afftraj->isChecked() && ui->ciel->isVisible());

    /* Corps de la methode */
    /*
     * Calcul de la position de l'observateur
     */
    observateurs[0].CalculPosVit(dateCourante);


    /*
     * Position du Soleil
     */
    soleil.CalculPosition(dateCourante);

    // Position topocentrique
    soleil.CalculCoordHoriz(observateurs.at(0));

    if (!ui->carte->isHidden()) {

        // Coordonnees terrestres
        soleil.CalculCoordTerrestres(observateurs.at(0));

        // Zone d'ombre
        soleil.CalculZoneVisibilite();

        if (ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored)
            soleil.CalculCoordEquat(observateurs.at(0));
    }


    /*
     * Position de la Lune
     */
    if (ui->afflune->isChecked()) {

        lune.CalculPosition(dateCourante);
        lune.CalculPhase(soleil);

        // Coordonnees topocentriques
        lune.CalculCoordHoriz(observateurs.at(0));

        if (ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored) {
            // Coordonnees equatoriales
            lune.CalculCoordEquat(observateurs.at(0));

            // Coordonnees terrestres
            lune.CalculCoordTerrestres(observateurs.at(0));
        }
    }

    if (ui->ciel->isVisible()) {

        /*
         * Calcul de la position des planetes
         */
        if (ui->affplanetes->checkState() != Qt::Unchecked) {
            planetes.clear();
            for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

                Planete planete(iplanete);
                planete.CalculPosition(dateCourante, soleil);
                planete.CalculCoordHoriz(observateurs.at(0));
                planetes.append(planete);
            }
        }

        /*
         * Calcul de la position du catalogue d'etoiles
         */
        Etoile::CalculPositionEtoiles(observateurs.at(0), etoiles);
        if (ui->affconst->isChecked())
            Constellation::CalculConstellations(observateurs.at(0), constellations);
        if (ui->affconst->checkState() != Qt::Unchecked)
            LigneConstellation::CalculLignesCst(etoiles, lignesCst);
    }

    /*
     * Calcul de la position courante des satellites
     */
    if (nbSat > 0) {

        Satellite::CalculPosVitListeSatellites(dateCourante, observateurs.at(0), soleil, nbTraces, visibilite, extinction,
                                               traceCiel, satellites);

        for (int i=0; i<nbSat; i++) {
            if (satellites[i].isVisible() && !bipSat[i]) {
                notif = true;
                bipSat[i] = true;
            }
        }

        if (ui->onglets->count() == 7 && satellites[0].isIeralt()) {
            chronometre->stop();
            const QString msg = tr("POSITION : Erreur rencontrée lors de l'exécution\nLa position du satellite %1 (numéro NORAD : %2) ne peut pas être calculée (altitude négative)");
            Messages::Afficher(msg.arg(tles.at(0).getNom()).arg(tles.at(0).getNorad()), WARNING);
            chronometre->start();
            l1 = "";
            l2 = "";
        }

        if (ui->onglets->count() < 7 && !satellites[0].isIeralt()) {
            l1 = tles.at(0).getLigne1();
            l2 = tles.at(0).getLigne2();
        }
    }
    Satellite::initCalcul = true;

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
    amajDeb = true;
    amajInt = false;
    atrouve = false;
    aupdnow = false;
    dirDwn = dirTle;

    /* Corps de la methode */
    QStringListIterator it(listeGroupeMaj);
    while (it.hasNext()) {
        const QStringList ligne = it.next().split("#");
        QString adresse = ligne.at(0).split("@").at(1);
        const QStringList listeTLE = ligne.at(1).split(",");

        if (adresse.contains("celestrak"))
            adresse = "http://www.celestrak.com/NORAD/elements/";
        if (!adresse.startsWith("http://"))
            adresse.insert(0, "http://");
        if (!adresse.endsWith("/"))
            adresse.append("/");

        foreach(QString file, listeTLE) {

            const QUrl url(adresse + file);
            AjoutFichier(url);

            const QString fic = QDir::convertSeparators(dirTle + QDir::separator() + file);
            if (fic == nomfic)
                atrouve = true;

            QNetworkRequest requete(url);
            rep = mng.get(requete);

            // Creation d'une boucle pour rendre le telechargement synchrone
            QEventLoop loop;
            connect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
        }

        if (downQueue.isEmpty())
            QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
    }

    // Mise a jour du fichier courant
    if (!atrouve) {

        const QFile fi(nomfic);
        const QUrl url("http://www.celestrak.com/NORAD/elements/" + fi.fileName());
        AjoutFichier(url);

        if (downQueue.isEmpty()) {
            QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
        } else {
            settings.setValue("temps/lastUpdate", dateCourante.getJourJulienUTC());
        }
        atrouve = false;
    }

    /* Retour */
    return;
}

void PreviSat::AjoutFichier(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (downQueue.isEmpty())
        QTimer::singleShot(0, this, SLOT(TelechargementSuivant()));
    downQueue.enqueue(url);

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
    if (tles.count() > 0) {

        const int ageMax = settings.value("temps/ageMax", 15).toInt();
        if (fabs(dateCourante.getJourJulienUTC() - tles.at(0).getEpoque().getJourJulienUTC()) > ageMax) {
            const QString msg = tr("Les éléments orbitaux sont plus vieux que %1 jour(s). Souhaitez-vous les mettre à jour?");
            const int res = QMessageBox::question(this, tr("Information"), msg.arg(ageMax),
                                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

            if (res == QMessageBox::Yes) {
                const QFileInfo fi(nomfic);
                if (fi.absoluteDir() == dirTle) {
                    amajDeb = true;
                    amajInt = false;
                    atrouve = true;
                    aupdnow = false;
                    dirDwn = dirTle;
                    const QUrl url("http://www.celestrak.com/NORAD/elements/" + fi.fileName());
                    AjoutFichier(url);

                    QNetworkRequest requete(url);
                    rep = mng.get(requete);

                    // Creation d'une boucle pour rendre le telechargement synchrone
                    QEventLoop loop;
                    connect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
                    loop.exec();

                    if (downQueue.isEmpty())
                        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));

                    settings.setValue("temps/lastUpdate", dateCourante.getJourJulienUTC());
                } else {
                    QDesktopServices::openUrl(QUrl("http://www.celestrak.com/NORAD/elements/"));
                }
            }
            old = true;
        }
    }

    /* Retour */
    return;
}


/*************
 * Interface *
 ************/
/*
 * Sauvegarde des donnees de l'onglet General
 */
void PreviSat::SauveOngletGeneral(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile sw(fic);
    sw.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!sw.isWritable()) {
        const QString msg = tr("POSITION : Problème de droits d'écriture du fichier %1");
        throw PreviSatException(msg.arg(sw.fileName()), WARNING);
    }
    QTextStream flux(&sw);

    flux << "PreviSat " + QString(APPVER_MAJ) + " / Astropedia (c) 2005-2012" << endl << endl << endl;
    flux << tr("Date :") << " " << ui->dateHeure1->text() << endl << endl;

    flux << tr("Lieu d'observation :") << " " << ui->lieuxObservation1->currentText() << endl;
    QString chaine = tr("Longitude : %1\tLatitude : %2\tAltitude : %3");
    flux << chaine.arg(ui->longitudeObs->text()).arg(ui->latitudeObs->text()).arg(ui->altitudeObs->text()) << endl << endl << endl;

    if (ui->onglets->count() == 7) {

        // Donnees sur le satellite
        flux << tr("Nom du satellite :") + " " + ui->nomsat1->text() << endl << endl;

        chaine = tr("Longitude : %1\tHauteur    : %2\tAscension droite :  %3");
        flux << chaine.arg(ui->longitudeSat->text().trimmed()).arg(ui->hauteurSat->text())
                .arg(ui->ascensionDroiteSat->text().trimmed()) << endl;

        chaine = tr("Latitude  :  %1\tAzimut (N) : %2\tDéclinaison      : %3");
        flux << chaine.arg(ui->latitudeSat->text().trimmed()).arg(ui->azimutSat->text().trimmed())
                .arg(ui->declinaisonSat->text()) << endl;

        chaine = tr("Altitude  :  %1%2\tDistance   : %3%4\tConstellation    : %5");
        flux << chaine.arg(ui->altitudeSat->text()).arg(QString(18 - ui->altitudeSat->text().length(), QChar(' ')))
                .arg(ui->distanceSat->text()).arg(QString(18 - ui->distanceSat->text().length(), QChar(' ')))
                .arg(ui->constellationSat->text()) << endl << endl;

        chaine = tr("Direction          : %1  \t%2");
        flux << chaine.arg(ui->directionSat->text()).arg(ui->magnitudeSat->text()) << endl;

        chaine = tr("Vitesse orbitale   : %1%2  \tConditions d'observation : %3");
        flux << chaine.arg((ui->vitesseSat->text().length() < 11) ? " " : "").arg(ui->vitesseSat->text())
                .arg(ui->conditionsObservation->text()) << endl;

        chaine = tr("Variation distance : %1  \tOrbite n°%2");
        flux << chaine.arg(ui->rangeRate->text()).arg(ui->nbOrbitesSat->text()) << endl << endl << endl;
    }

    // Donnees sur le Soleil
    flux << tr("Coordonnées du Soleil :") << endl;
    chaine = tr("Hauteur    : %1\tAscension droite :  %2");
    flux << chaine.arg(ui->hauteurSoleil->text().trimmed()).arg(ui->ascensionDroiteSoleil->text()) << endl;

    chaine = tr("Azimut (N) : %1\tDéclinaison      : %2");
    flux << chaine.arg(ui->azimutSoleil->text().trimmed()).arg(ui->declinaisonSoleil->text()) << endl;

    chaine = tr("Distance   : %1   \tConstellation    : %2");
    flux << chaine.arg(ui->distanceSoleil->text()).arg(ui->constellationSoleil->text()) << endl << endl << endl;

    // Donnees sur la Lune
    flux << tr("Coordonnées de la Lune :") << endl;
    chaine = tr("Hauteur    : %1\tAscension droite :  %2");
    flux << chaine.arg(ui->hauteurLune->text().trimmed()).arg(ui->ascensionDroiteLune->text()) << endl;

    chaine = tr("Azimut (N) : %1\tDéclinaison      : %2");
    flux << chaine.arg(ui->azimutLune->text().trimmed()).arg(ui->declinaisonLune->text()) << endl;

    chaine = tr("Distance   : %1  \tConstellation    : %2");
    flux << chaine.arg(ui->distanceLune->text()).arg(ui->constellationLune->text()) << endl << endl;
    flux << tr("Phase        :") + " " + ui->phaseLune->text() << endl;
    flux << tr("Illumination :") + " " + ui->illuminationLune->text() << endl;

    sw.close();

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet Elements osculateurs
 */
void PreviSat::SauveOngletElementsOsculateurs(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile sw(fic);
    sw.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!sw.isWritable()) {
        const QString msg = tr("POSITION : Problème de droits d'écriture du fichier %1");
        throw PreviSatException(msg.arg(sw.fileName()), WARNING);
    }
    QTextStream flux(&sw);

    flux << "PreviSat " + QString(APPVER_MAJ) + " / Astropedia (c) 2005-2012" << endl << endl << endl;
    flux << tr("Date :") << " " << ui->dateHeure2->text() << endl << endl;

    // Donnees sur le satellite
    flux << tr("Nom du satellite :") + " " + ui->nomsat2->text() << endl;
    flux << ui->ligne1->text() << endl;
    flux << ui->ligne2->text() << endl << endl;

    flux << tr("Vecteur d'état (ECI) :\t\t\t\tÉléments osculateurs :") << endl;
    QString chaine = tr("x : %1%2\tvx : %3\tDemi-grand axe : %4\tAscension droite du noeud ascendant : %5%6");
    flux << chaine.arg(ui->xsat->text()).arg(QString(18 - ui->xsat->text().length(), QChar(' '))).arg(ui->vxsat->text())
            .arg(ui->demiGrandAxe->text()).arg(QString(9 - ui->ADNoeudAscendant->text().length(), QChar('0')))
            .arg(ui->ADNoeudAscendant->text()) << endl;

    chaine = tr("y : %1%2\tvy : %3\tExcentricité   : %4\tArgument du périgée                 : %5%6");
    flux << chaine.arg(ui->ysat->text()).arg(QString(18 - ui->ysat->text().length(), QChar(' '))).arg(ui->vysat->text())
            .arg(ui->excentricite->text()).arg(QString(9 - ui->argumentPerigee->text().length(), QChar('0')))
            .arg(ui->argumentPerigee->text()) << endl;

    chaine = tr("z : %1%2\tvz : %3\tInclinaison    : %4%5\tAnomalie moyenne                    : %6%7");
    flux << chaine.arg(ui->zsat->text()).arg(QString(18 - ui->zsat->text().length(), QChar(' '))).arg(ui->vzsat->text())
            .arg(QString(9 - ui->inclinaison->text().length(), QChar('0'))).arg(ui->inclinaison->text())
            .arg(QString(9 - ui->anomalieMoyenne->text().length(), QChar('0'))).arg(ui->anomalieMoyenne->text()) << endl << endl;


    chaine = tr("Anomalie vraie       : %1%2\t\tApogée  (Altitude) : %3");
    flux << chaine.arg(QString(9 - ui->anomalieVraie->text().length(), QChar('0'))).arg(ui->anomalieVraie->text())
            .arg(ui->apogee->text()) << endl;

    chaine = tr("Anomalie excentrique : %1%2\t\tPérigée (Altitude) : %3");
    flux << chaine.arg(QString(9 - ui->anomalieExcentrique->text().length(), QChar('0')))
            .arg(ui->anomalieExcentrique->text()).arg(ui->perigee->text()) << endl;

    chaine = tr("Champ de vue         : %1  \t\tPériode orbitale   : %2");
    flux << chaine.arg(ui->champDeVue->text()).arg(ui->periode->text().replace(" ", "")) << endl;

    sw.close();

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet Informations satellite
 */
void PreviSat::SauveOngletInformations(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile sw(fic);
    sw.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!sw.isWritable()) {
        const QString msg = tr("POSITION : Problème de droits d'écriture du fichier %1");
        throw PreviSatException(msg.arg(sw.fileName()), WARNING);
    }
    QTextStream flux(&sw);

    flux << "PreviSat " + QString(APPVER_MAJ) + " / Astropedia (c) 2005-2012" << endl << endl << endl;

    // Donnees sur le satellite
    flux << tr("Nom du satellite :") + " " + ui->nomsat3->text() << endl;
    flux << ui->line1->text() << endl;
    flux << ui->line2->text() << endl << endl;

    QString chaine = tr("Numéro NORAD            : %1 \t\tMoyen mouvement       : %2 rev/jour");
    flux << chaine.arg(ui->norad->text()).arg(ui->nbRev->text()) << endl;

    chaine = tr("Désignation COSPAR      : %1\t\tn'/2                  : %2%3 rev/jour^2");
    flux << chaine.arg(ui->cospar->text()).arg(QString(11 - ui->nbRev2->text().length(), QChar(' '))).arg(ui->nbRev2->text()) << endl;

    chaine = tr("Époque (UTC)            : %1\tn\"/6                  : %2%3 rev/jour^3");
    flux << chaine.arg(ui->epoque->text()).arg(QString(11 - ui->nbRev3->text().length(), QChar(' '))).arg(ui->nbRev3->text()) << endl;

    chaine = tr("Coeff pseudo-balistique : %1 (1/Re)\tNb orbites à l'époque : %2");
    flux << chaine.arg(ui->bstar->text()).arg(ui->nbOrbitesEpoque->text()) << endl << endl;

    chaine = tr("Inclinaison             : %1%2\t\tAnomalie moyenne      : %3%4");
    flux << chaine.arg(QString(9 - ui->inclinaisonMoy->text().trimmed().length(), QChar('0')))
            .arg(ui->inclinaisonMoy->text().trimmed()).arg(QString(9 - ui->anomalieMoy->text().trimmed().length(), QChar('0')))
            .arg(ui->anomalieMoy->text().trimmed()) << endl;

    chaine = tr("AD noeud ascendant      : %1%2\t\tMagnitude std/max     : %3");
    flux << chaine.arg(QString(9 - ui->ADNoeudAscendantMoy->text().trimmed().length(), QChar('0')))
            .arg(ui->ADNoeudAscendantMoy->text().trimmed()).arg(ui->magnitudeStdMax->text()) << endl;

    chaine = tr("Excentricité            : %1\t\tModèle orbital        : %2");
    flux << chaine.arg(ui->excentriciteMoy->text()).arg(ui->modele->text()) << endl;

    chaine = tr("Argument du périgée     : %1%2\t\tDimensions/Section    : %3^2");
    flux << chaine.arg(QString(9 - ui->argumentPerigeeMoy->text().trimmed().length(), QChar('0')))
            .arg(ui->argumentPerigeeMoy->text().trimmed()).arg(ui->dimensions->text()) << endl;
    sw.close();

    /* Retour */
    return;
}

void PreviSat::EcritureCompteRenduMaj(const QStringList &compteRendu, bool &aecr)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const int nbsup = compteRendu.at(compteRendu.count()-1).toInt();
    const int nbadd = compteRendu.at(compteRendu.count()-2).toInt();
    const int nbold = compteRendu.at(compteRendu.count()-3).toInt();
    const int nbmaj = compteRendu.at(compteRendu.count()-4).toInt();
    const QString fic = compteRendu.at(compteRendu.count()-5);

    ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + tr("Fichier").append(" ").append(fic).append(" :\n"));
    QString msgcpt = tr("TLE du satellite %1 (%2) non réactualisé");
    if (nbmaj < nbold && nbmaj > 0) {
        for(int i=0; i<compteRendu.count()-5; i++) {
            const QString nomsat = compteRendu.at(i).split("#").at(0);
            const QString norad = compteRendu.at(i).split("#").at(1);
            ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + msgcpt.arg(nomsat).arg(norad) + "\n");
        }

        msgcpt = tr("%1 TLE(s) sur %2 mis à jour");
        ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + msgcpt.arg(nbmaj).arg(nbold) + "\n");
    }

    if (nbmaj == nbold && nbold != 0) {
        msgcpt = tr("Mise à jour de tous les TLE effectuée (fichier de %1 satellite(s))");
        ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + msgcpt.arg(nbold) + "\n");
    }

    if (nbmaj == 0 && nbold != 0)
        ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + tr("Aucun TLE mis à jour") + "\n");

    if (nbsup > 0) {
        msgcpt = tr("Nombre de TLE(s) supprimés : %1");
        ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + msgcpt.arg(nbsup) + "\n");
    }

    if (nbadd > 0) {
        msgcpt = tr("Nombre de TLE(s) ajoutés : %1");
        ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + msgcpt.arg(nbadd) + "\n");
    }
    ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + "\n");

    aecr = nbold > 0 && (nbmaj > 0 || nbsup > 0 || nbadd > 0);

    /* Retour */
    return;
}

void PreviSat::ModificationOption()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->affradar->isChecked() || ui->affradar->checkState() == Qt::PartiallyChecked) {
        ui->affinvew->setEnabled(true);
        ui->affinvns->setEnabled(true);
    } else {
        ui->affinvew->setEnabled(false);
        ui->affinvns->setEnabled(false);
    }

    if (ui->afflune->isChecked()) {
        ui->affphaselune->setEnabled(true);
        ui->rotationLune->setEnabled(true);
    } else {
        ui->affphaselune->setEnabled(false);
        ui->rotationLune->setEnabled(false);
    }

    ui->intensiteOmbre->setEnabled(ui->affnuit->isChecked());
    ui->nombreTrajectoires->setEnabled(ui->afftraj->isChecked());

    if (ui->unitesKm->hasFocus() || ui->unitesMi->hasFocus()) {
        info = true;
        AffichageLieuObs();
        ui->lieuxObs->setCurrentRow(-1);
        ui->selecLieux->setCurrentRow(-1);
    }

    if (ui->zoneAffichage->isVisible()) {

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::AfficherLieuSelectionne(const int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString nomlieu = "";

    /* Corps de la methode */
    ui->nouveauLieu->setVisible(false);
    ui->nouvelleCategorie->setVisible(false);
    messagesStatut->setText("");

    // Recuperation du lieu d'observation de la liste
    if (ui->lieuxObs->hasFocus() && index >= 0) {
        nomlieu = mapObs.at(index);
        ui->selecLieux->setCurrentRow(-1);
    }
    if (ui->selecLieux->hasFocus() && index >= 0) {
        nomlieu = listeObs.at(index);
        ui->lieuxObs->setCurrentRow(-1);
    }

    if (!nomlieu.isEmpty()) {

        ui->coordonnees->setVisible(true);

        // Longitude/Latitude/Altitude
        const QStringList lieu = nomlieu.split("#");
        const QStringList coord = lieu.at(1).split("&");
        const double lo = coord.at(0).toDouble();
        const double la = coord.at(1).toDouble();
        const int atd = coord.at(2).toInt();

        const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
        const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");

        // Affichage des coordonnees du lieu d'observation
        ui->nLieu->setText(tr("Lieu :") + " " + lieu.at(0).trimmed());
        ui->nLongitude->setText(Maths::ToSexagesimal(fabs(lo), NO_TYPE, 3, 0, false, true) + " " + ew);
        ui->nLatitude->setText(Maths::ToSexagesimal(fabs(la), NO_TYPE, 2, 0, false, true) + " " + ns);
        const QString msg = "%1 ";
        ui->nAltitude->setText((ui->unitesKm->isChecked()) ? msg.arg(atd).append(tr("m")) :
                                                             msg.arg((int) (atd * PIED_PAR_METRE)).append(tr("ft")));
    }

    /* Retour */
    return;
}

/*
 * Compte du nombre de satellites coches dans une liste
 */
int PreviSat::getListeItemChecked(const QListWidget *listWidget) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    int k = 0;
    for (int i=0; i<listWidget->count(); i++) {
        if (listWidget->item(i)->checkState() == Qt::Checked)
            k++;
    }

    /* Retour */
    return (k);
}


/***********
 * Systeme *
 **********/
void PreviSat::EcritureListeRegistre() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString listeTLE = "";

    /* Corps de la methode */
    try {
        if (liste.length() > 0)
            listeTLE = liste.at(0);
        if (nbSat > 0) {

            for (int i=1; i<nbSat; i++)
                listeTLE.append("&").append(liste.at(i));

            // Recuperation des TLE de la liste
            TLE::LectureFichier(nomfic, liste, tles);
            for (int i=0; i<nbSat; i++)
                if (tles.at(i).getNorad().isEmpty())
                    throw PreviSatException();
            nom = tles.at(0).getNom();
            l1 = tles.at(0).getLigne1();
            l2 = tles.at(0).getLigne2();

            // Recuperation des donnees satellites
            Satellite::LectureDonnees(liste, tles, satellites);

            if (tles.at(0).getNom().isEmpty())
                settings.setValue("TLE/nom", nom);
            else
                settings.setValue("TLE/nom", tles.at(0).getNom());

            settings.setValue("TLE/l1", tles.at(0).getLigne1());
            settings.setValue("TLE/l2", tles.at(0).getLigne2());
            settings.setValue("TLE/nbsat", nbSat);
            settings.setValue("TLE/liste", listeTLE);

        }
    } catch (PreviSatException &e) {

        try {
            TLE::VerifieFichier(nomfic, true);

            AfficherListeSatellites(nomfic, liste);
            nbSat = getListeItemChecked(ui->liste1);

            nor = liste.at(0);

            int j = 0, k = 0;
            tles.clear();
            liste.clear();
            tles.resize(nbSat);
            bipSat.resize(nbSat);

            if (nbSat == 0) {
                l1 = "";
                l2 = "";
            }

            for (int i=0; i<nbSat; i++) {
                if (ui->liste1->item(i)->checkState() == Qt::Checked) {
                    liste[k] = ui->liste1->item(i)->text().split("#").at(1);
                    if (nor == liste.at(k))
                        j = k;
                    k++;
                }
            }
            if (j > 0) {
                liste[j] = liste.at(0);
                liste[0] = nor;
            }
            TLE::LectureFichier(nomfic, liste, tles);

        } catch (PreviSatException &ex) {
        }
    }

    /* Retour */
    return;
}

bool PreviSat::DecompressionFichierGz(const QString &fichierGz, const QString &fichierDecompresse) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool res = false;

    /* Corps de la methode */
    gzFile fichGz = gzopen(fichierGz.toStdString().c_str(), "rb");
    if (fichGz == NULL) {
        res = false;
    } else {

        QFile fichDec(fichierDecompresse);
        fichDec.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux(&fichDec);

        char buffer[8192];
        while (gzgets(fichGz, buffer, 8192) != NULL)
            flux << buffer;
        gzclose(fichGz);
        fichDec.close();
        res = true;
    }

    /* Retour */
    return (res);
}


/*
 * Gestion du temps reel
 */
void PreviSat::GestionTempsReel()
{
    /* Declarations des variables locales */
    double pas1, pas2;
    Date date1, date2;

    /* Initialisations */

    /* Corps de la methode */
    if (ui->tempsReel->isChecked()) {
        modeFonctionnement->setText(tr("Temps réel"));
        date1 = Date(dateCourante.getOffsetUTC());
        pas1 = ui->pasReel->currentText().toDouble();
        pas2 = 0.;
    } else {
        modeFonctionnement->setText(tr("Mode manuel"));
        date1 = dateCourante;
        if (ui->valManuel->currentIndex() < 3) {
            pas1 = ui->pasManuel->currentText().toDouble() * qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) *
                    NB_JOUR_PAR_SEC;
        } else {
            pas1 = ui->pasManuel->currentText().toDouble();
        }
        pas2 = (!ui->backward->isEnabled() || !ui->forward->isEnabled()) ? 1. : pas1 * NB_SEC_PAR_JOUR;
    }

    // Affichage du jour julien
    date2 = Date(date1.getAnnee(), 1, 1., 0.);
    if (ui->calJulien->isChecked()) {
        stsDate->setText(QString::number(date1.getJourJulien() + TJ2000, 'f', 5));
        stsHeure->setText(QString::number(date1.getJourJulien() - date2.getJourJulien() + 1., 'f', 5));
        stsDate->setToolTip(tr("Jour julien"));
        stsHeure->setToolTip(tr("Jour"));
    } else {
        stsDate->setText(QDate(date1.getAnnee(), date1.getMois(), date1.getJour()).toString(tr("dd/MM/yyyy")));
        stsHeure->setText(QTime(date1.getHeure(), date1.getMinutes(), date1.getSecondes()).toString("hh:mm:ss"));
        stsDate->setToolTip(tr("Date"));
        stsHeure->setToolTip(tr("Heure"));
    }

    // Lancement des calculs
    if (ui->tempsReel->isChecked() && tim.secsTo(QDateTime::currentDateTime()) >= pas1) {
        tim = (tim.addSecs(pas1) <= QDateTime::currentDateTime()) ? tim.addSecs(pas1) : QDateTime::currentDateTime();

        // Date actuelle
        dateCourante = Date(dateCourante.getOffsetUTC());

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    if (ui->modeManuel->isChecked() && fabs(tim.secsTo(QDateTime::currentDateTime()) >= pas2)) {
        tim = QDateTime::currentDateTime();

        if (ui->pause->isEnabled()) {

            if (!ui->pasManuel->view()->isVisible()) {

                double jd = dateCourante.getJourJulien();
                if (!ui->rewind->isEnabled() || !ui->backward->isEnabled())
                    jd -= pas1;
                if (!ui->play->isEnabled() || !ui->forward->isEnabled())
                    jd += pas1;

                dateCourante = Date(jd + EPS_DATES, dateCourante.getOffsetUTC());

                // Enchainement de l'ensemble des calculs
                EnchainementCalculs();

                if (ui->dateHeure4->isVisible())
                    ui->dateHeure4->setDateTime(dateCourante.ToQDateTime(1));
                else
                    ui->dateHeure3->setDateTime(dateCourante.ToQDateTime(1));
            }
        }
    }

    /* Retour */
    return;
}

void PreviSat::CalculsTermines()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QFileInfo fi(dirTmp + QDir::separator() + "prevision.txt");
    switch (threadCalculs->getTypeCalcul()) {
    case ThreadCalculs::PREVISION:
        ui->annulerPrev->setVisible(false);
        ui->calculsPrev->setVisible(true);
        if (fi.exists()) {
            ui->afficherPrev->setVisible(true);
            ui->afficherPrev->setFocus();
        }
        break;

    case ThreadCalculs::IRIDIUM:
        ui->annulerIri->setVisible(false);
        ui->calculsIri->setVisible(true);
        if (fi.exists()) {
            ui->afficherIri->setVisible(true);
            ui->afficherIri->setFocus();
            settings.setValue("fichier/iridium", QDir::convertSeparators(ui->fichierTLEIri->text()));
        }
        break;

    case ThreadCalculs::EVENEMENTS:
        ui->annulerEvt->setVisible(false);
        ui->calculsEvt->setVisible(true);
        if (fi.exists()) {
            ui->afficherEvt->setVisible(true);
            ui->afficherEvt->setFocus();
        }
        break;

    case ThreadCalculs::TRANSITS:
        ui->annulerTransit->setVisible(false);
        ui->calculsTransit->setVisible(true);
        if (fi.exists()) {
            ui->afficherTransit->setVisible(true);
            ui->afficherTransit->setFocus();
            settings.setValue("fichier/fichierTLETransit", QDir::convertSeparators(ui->fichierTLETransit->text()));
        }

    default:
        break;
    }
    threadCalculs->deleteLater();
    messagesStatut->setText((fi.exists()) ? tr("Terminé !") : "");

    /* Retour */
    return;
}

void PreviSat::TelechargementSuivant()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (downQueue.isEmpty()) {
        emit TelechargementFini();
        if (ui->miseAJourTLE->isVisible())
            ui->frameBarreProgression->setVisible(false);
        messagesStatut->setText(tr("Terminé !"));
    } else {

        const QUrl url = downQueue.dequeue();

        const QString fic = QFileInfo(url.path()).fileName();
        ui->fichierTelechargement->setText(fic);
        if (ui->miseAJourTLE->isVisible()) {
            ui->barreProgression->setValue(0);
            ui->frameBarreProgression->setVisible(true);
            ui->compteRenduMaj->setVisible(true);
        }
        ficDwn.setFileName(dirDwn + QDir::separator() + fic);

        if (ficDwn.open(QIODevice::WriteOnly)) {

            QNetworkRequest requete(url);
            if (!amajDeb)
                rep = mng.get(requete);

            connect(rep, SIGNAL(downloadProgress(qint64,qint64)), SLOT(ProgressionTelechargement(qint64,qint64)));
            connect(rep, SIGNAL(finished()), SLOT(FinEnregistrementFichier()));
            connect(rep, SIGNAL(readyRead()), SLOT(EcritureFichier()));

        } else {
            const QString msg = tr("Erreur lors du téléchargement du fichier %1");
            messagesStatut->setText(msg.arg(ficDwn.fileName()));
        }
    }

    /* Retour */
    return;
}

void PreviSat::FinEnregistrementFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool atr = false;
    bool aup = false;
    ficDwn.close();

    /* Corps de la methode */
    QFile fd(ficDwn.fileName());
    fd.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flx(&fd);
    const QString lg = flx.readLine();
    fd.close();
    if (lg.contains("DOCTYPE"))
        atr = true;

    if (rep->error() || atr) {

        // Erreur survenue lors du telechargement
        ui->frameBarreProgression->setVisible(false);
        const QFileInfo ff(ficDwn.fileName());

        if (fd.exists())
            fd.remove();

        QString msg = tr("Erreur lors du téléchargement du fichier %1");
        if (rep->error())
            msg += " : " + rep->errorString();
        Messages::Afficher(msg.arg(ff.fileName()), WARNING);

    } else {

        // Mise a jour des TLE
        if (atrouve) {

            if (QDir::convertSeparators(ficDwn.fileName()) == nomfic) {

                const int nb = TLE::VerifieFichier(nomfic, false);
                if (nb == 0) {
                    const QString msg = tr("Erreur lors du téléchargement du fichier %1");
                    Messages::Afficher(msg.arg(nomfic), WARNING);
                } else {

                    // Recuperation des TLE de la liste
                    TLE::LectureFichier(nomfic, liste, tles);

                    Satellite::initCalcul = false;
                    Satellite::LectureDonnees(liste, tles, satellites);

                    CalculsAffichage();
                }
            }
        }

        // Mise a jour des fichiers internes
        if (amajInt) {

            amajInt = false;
            if (ficDwn.exists()) {

                ficDwn.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream flux(&ficDwn);
                const QStringList ligne = flux.readLine().split("-");
                ficDwn.close();
                rep->deleteLater();
                TelechargementSuivant();

                const int an = ligne.at(0).toInt();
                const int mo = ligne.at(1).toInt();
                const int jo = ligne.at(2).toInt();
                const QDateTime dateHttp(QDate(an, mo, jo), QTime(0, 0, 0));

                const QString httpDirDat = "http://astropedia.free.fr/previsat/Qt/data/";
                QStringList fichiers;
                fichiers << "donnees.sat" << "iridium.sts";
                dirDwn = dirDat;

                for(int i=0; i<fichiers.size(); i++) {

                    const QString fic = dirDat + QDir::separator() + fichiers.at(i);
                    const QFileInfo fi(fic);

                    if (fi.lastModified() < dateHttp) {
                        AjoutFichier(QUrl(httpDirDat + fichiers.at(i)));
                        aup = true;
                    }
                }
                if (downQueue.isEmpty())
                    QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));

                if (aup)
                    QMessageBox::information(0, tr("Information"), tr("Mise à jour des fichiers internes effectuée"));
                else
                    QMessageBox::information(0, tr("Information"), tr("Aucune mise à jour des fichiers internes est nécessaire"));
            }
        }

        // Mise a jour des fichiers TLE selectionnes
        if (aupdnow) {

            QString fichierALire = QDir::convertSeparators(ficDwn.fileName());
            QFileInfo ff(fichierALire);
            QString fichierAMettreAJour = QDir::convertSeparators(dirTle + QDir::separator() + ff.fileName());

            QFile fi(fichierAMettreAJour);
            if (fi.exists()) {

                QStringList compteRendu;
                TLE::MiseAJourFichier(fichierAMettreAJour, fichierALire, compteRendu);

                bool aecr = false;
                EcritureCompteRenduMaj(compteRendu, aecr);
                aup = true;

            } else {
                fi.copy(fichierALire, fichierAMettreAJour);
                const QString msg = tr("Ajout du fichier %1") + "\n";
                ui->compteRenduMaj->setPlainText(ui->compteRenduMaj->toPlainText() + msg.arg(ff.fileName()));
            }

            if (fichierAMettreAJour == nomfic) {

                const int nb = TLE::VerifieFichier(nomfic, false);
                if (nb == 0) {
                    const QString msg = tr("Erreur lors du téléchargement du fichier %1");
                    Messages::Afficher(msg.arg(ff.fileName()), WARNING);
                } else {

                    // Recuperation des TLE de la liste
                    TLE::LectureFichier(nomfic, liste, tles);

                    Satellite::initCalcul = false;
                    Satellite::LectureDonnees(liste, tles, satellites);

                    CalculsAffichage();
                }
            }
        }
    }

    if (aup)
        rep->deleteLater();
    TelechargementSuivant();

    /* Retour */
    return;
}

void PreviSat::EcritureFichier()
{
    ficDwn.write(rep->readAll());
}

void PreviSat::ProgressionTelechargement(qint64 recu, qint64 total) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (total != -1) {
        ui->barreProgression->setRange(0, total);
        ui->barreProgression->setValue(recu);
    }

    /* Retour */
    return;
}


/*
 * Fin de l'application
 */
void PreviSat::closeEvent(QCloseEvent *)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QDir di = QDir(dirTmp);
    if (di.entryList(QDir::Files).count() > 0) {
        foreach(QString fic, di.entryList(QDir::Files)) {
            QFile fi(dirTmp + QDir::separator() + fic);
            fi.remove();
        }
    }

    // Sauvegarde des donnees du logiciel
    settings.setValue("temps/valManuel", ui->valManuel->currentIndex());
    settings.setValue("temps/pasManuel", ui->pasManuel->currentIndex());
    settings.setValue("temps/pasReel", ui->pasReel->currentIndex());
    settings.setValue("temps/dtu", ui->updown->value() * NB_JOUR_PAR_MIN);

    settings.setValue("affichage/groupeTLE", ui->groupeTLE->currentIndex());
    settings.setValue("affichage/affcoord", ui->affcoord->isChecked());
    settings.setValue("affichage/affgrille", ui->affgrille->isChecked());
    settings.setValue("affichage/affinvew", ui->affinvew->isChecked());
    settings.setValue("affichage/affinvns", ui->affinvns->isChecked());
    settings.setValue("affichage/afflune", ui->afflune->isChecked());
    settings.setValue("affichage/affnomlieu", ui->affnomlieu->checkState());
    settings.setValue("affichage/affnomsat", ui->affnomsat->checkState());
    settings.setValue("affichage/affnotif", ui->affnotif->isChecked());
    settings.setValue("affichage/affnuit", ui->affnuit->isChecked());
    settings.setValue("affichage/affphaselune", ui->affphaselune->isChecked());
    settings.setValue("affichage/affplanetes", ui->affplanetes->checkState());
    settings.setValue("affichage/affradar", ui->affradar->checkState());
    settings.setValue("affichage/affSAA", ui->affSAA->isChecked());
    settings.setValue("affichage/affsoleil", ui->affsoleil->isChecked());
    settings.setValue("affichage/afftracesol", ui->afftraj->isChecked());
    settings.setValue("affichage/affconst", ui->affconst->checkState());
    settings.setValue("affichage/affetoiles", ui->affetoiles->isChecked());
    settings.setValue("affichage/nombreTrajectoires", ui->nombreTrajectoires->value());
    settings.setValue("affichage/magnitudeEtoiles", ui->magnitudeEtoiles->value());
    settings.setValue("affichage/affvisib", ui->affvisib->checkState());
    settings.setValue("affichage/utcAuto", ui->utcAuto->isChecked());
    settings.setValue("affichage/calJulien", ui->calJulien->isChecked());
    settings.setValue("affichage/extinction", ui->extinctionAtmospherique->isChecked());
    settings.setValue("affichage/intensiteOmbre", ui->intensiteOmbre->value());
    settings.setValue("affichage/utc", ui->utc->isChecked());
    settings.setValue("affichage/unite", ui->unitesKm->isChecked());

    settings.setValue("fichier/listeMap", (ui->listeMap->currentIndex() > 0) ? ficMap.at(qMax(0, ui->listeMap->currentIndex()-1)) : "");
    settings.setValue("fichier/nom", (ficgz.isEmpty()) ? QDir::convertSeparators(nomfic) : QDir::convertSeparators(ficgz));
    settings.setValue("fichier/iridium", ui->fichierTLEIri->text());
    settings.setValue("fichier/fichierAMettreAJour", ui->fichierAMettreAJour->text());
    settings.setValue("fichier/fichierALire", ui->fichierALire->text());
    settings.setValue("fichier/fichierALireCreerTLE", ui->fichierALireCreerTLE->text());
    settings.setValue("fichier/nomFichierPerso", ui->nomFichierPerso->text());
    settings.setValue("fichier/fichierTLETransit", ui->fichierTLETransit->text());

    EcritureListeRegistre();

    /* Retour */
    return;
}

/*
 * Redimensionnement de la fenetre
 */
void PreviSat::resizeEvent(QResizeEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->frameCarte->height() >= PreviSat::height())
        ui->frameCarte->setGeometry(0, 0, ui->frameCarte->width(), PreviSat::height() - 23);
    ui->carte->setGeometry(6, 6, ui->frameCarte->width() - 47, ui->frameCarte->height() - 23);
    ui->frameCarte2->setGeometry(ui->carte->geometry());
    ui->maximise->move(11 + ui->carte->width(), 5);
    ui->affichageCiel->move(11 + ui->carte->width(), 32);

    const int hcarte = ui->carte->height() - 3;
    const int lcarte = ui->carte->width() - 3;

    DEG2PXHZ = lcarte / T360;
    DEG2PXVT = hcarte * 2. / T360;

    ui->liste1->setGeometry(20, 136, 200, hcarte - 127);

    ui->S60->move(5, hcarte / 1.2 - 1);
    ui->S30->move(5, hcarte / 1.5 - 1);
    ui->SS->move(5, hcarte * 7. / 12. - 1);
    ui->N0->move(5, hcarte * 0.5 - 1);
    ui->NN->move(5, hcarte / 2.4 - 1);
    ui->N30->move(5, hcarte / 3. - 1);
    ui->N60->move(5, hcarte / 6. - 1);
    ui->frameLat->setGeometry(7 + ui->carte->width(), 0, ui->frameLat->width(), ui->carte->height());

    ui->W150->move(lcarte / 12. - 8, 0);
    ui->W120->move(lcarte / 6. - 8, 0);
    ui->W90->move(lcarte / 4. - 5, 0);
    ui->W60->move(lcarte / 3. - 5, 0);
    ui->W30->move(lcarte / 2.4 - 5, 0);
    ui->WW->move(lcarte * 11. / 24. - 2, 0);
    ui->W0->move(lcarte * 0.5 - 2, 0);
    ui->EE->move(lcarte * 13. / 24. - 2, 0);
    ui->E30->move(lcarte * 7. / 12. - 5, 0);
    ui->E60->move(lcarte / 1.5 - 5, 0);
    ui->E90->move(lcarte * 0.75 - 5, 0);
    ui->E120->move(lcarte / 1.2 - 8, 0);
    ui->E150->move(lcarte * 11. / 12. - 8, 0);
    ui->frameLon->setGeometry(6, 7 + ui->carte->height(), ui->carte->width(), ui->frameLon->height());

    ui->onglets->move(ui->frameCarte->width() * 0.5 - 411, 0);

    ui->ciel->setGeometry(qRound(0.5 * (ui->frameCarte->width() - ui->frameCarte->height() + 30)), 20,
                          ui->frameCarte->height() - 44, ui->frameCarte->height() - 44);
    ui->sud->move(qRound(0.5 * ui->frameCarte->width()) - 20, ui->ciel->y() + ui->ciel->height());
    ui->nord->move(ui->sud->x(), 5);
    ui->est->move(ui->ciel->x() - ui->est->width() - 2, qRound(0.5 * ui->ciel->height()) + 10);
    ui->ouest->move(ui->ciel->x() + ui->ciel->width() + 2, ui->est->y());

    if (Satellite::initCalcul) {

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des courbes
        AffichageCourbes();
    }

    /* Retour */
    return;
}


/*
 * Appui sur une touche
 */
void PreviSat::keyPressEvent(QKeyEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (event->key() == Qt::Key_F8) {

        // Capture de la fenetre
        chronometre->stop();
        const QPixmap image = QPixmap::grabWidget(QApplication::activeWindow());
        const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), settings.value("fichier/sauvegarde", dirOut).toString(),
                                                         tr("Fichiers PNG (*.png);;Fichiers JPEG (*.jpg);;Fichiers BMP (*.bmp);;Tous les fichiers (*)"));
        if (!fic.isEmpty())
            image.save(fic);
        chronometre->start();
        const QFileInfo fi(fic);
        settings.setValue("fichier/sauvegarde", fi.absolutePath());

    } else if (event->key() == Qt::Key_F10) {

        // Bascule temps reel / mode manuel
        if (ui->tempsReel->isChecked()) {
            ui->modeManuel->setChecked(true);
        } else {

            ui->tempsReel->setChecked(true);
            tim = QDateTime::currentDateTime();

            // Date actuelle
            dateCourante = Date(offsetUTC);

            const Date date = Date(dateCourante.getJourJulien() + EPS_DATES, 0.);

            if (ui->dateHeure4->isVisible()) {
                ui->dateHeure4->setDateTime(date.ToQDateTime(1));
            } else {
                ui->dateHeure3->setDateTime(date.ToQDateTime(1));
                ui->dateHeure3->setFocus();
            }
        }

    } else if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_F12) {

        // Etape precedente/suivante (mode manuel)
        int sgn;
        if (!ui->modeManuel->isChecked())
            ui->modeManuel->setChecked(true);
        if (event->key() == Qt::Key_F11)
            sgn = -1;
        if (event->key() == Qt::Key_F12)
            sgn = 1;

        double jd = dateCourante.getJourJulien();
        jd = (ui->valManuel->currentIndex() < 3) ? jd + sgn * ui->pasManuel->currentText().toDouble() *
                                                   qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) * NB_JOUR_PAR_SEC :
                                                   jd + sgn * ui->pasManuel->currentText().toDouble();

        const Date date = Date(jd + EPS_DATES, 0.);

        if (ui->dateHeure4->isVisible()) {
            ui->dateHeure4->setDateTime(date.ToQDateTime(1));
        } else {
            ui->dateHeure3->setDateTime(date.ToQDateTime(1));
            ui->dateHeure3->setFocus();
        }
    }

    /* Retour */
    return;
}

/*
 * Clic avec la souris
 */
void PreviSat::mousePressEvent(QMouseEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (event->button() == Qt::LeftButton) {

        // Selection du satellite sur la carte du monde
        if (!ui->carte->isHidden()) {

            const int x = event->x() - 6;
            const int y = event->y() - 6;
            for (int isat=0; isat<nbSat; isat++) {

                const int lsat = qRound((180. - satellites.at(isat).getLongitude() * RAD2DEG) * DEG2PXHZ);
                const int bsat = qRound((90. - satellites.at(isat).getLatitude() * RAD2DEG) * DEG2PXVT);

                // Distance au carre du curseur au satellite
                const int dt = (x - lsat) * (x - lsat) + (y - bsat) * (y - bsat);

                // Le curseur est au-dessus d'un satellite
                if (dt <= 16) {
                    const QString norad = liste.at(isat);
                    liste[isat] = liste.at(0);
                    liste[0] = norad;

                    bool bip = bipSat.at(isat);
                    bipSat[isat] = bipSat.at(0);
                    bipSat[0] = bip;

                    const TLE tle = tles.at(isat);
                    tles[isat] = tles.at(0);
                    tles[0] = tle;

                    const Satellite sat = satellites.at(isat);
                    satellites[isat] = satellites.at(0);
                    satellites[0] = sat;

                    // Ecriture des cles de registre
                    EcritureListeRegistre();

                    info = true;

                    CalculsAffichage();
                }
            }
        }

        // Selection du satellite sur le radar
        if (!ui->radar->isHidden()) {

            const int x = event->x() - ui->frameZone->x() - ui->radar->x() - 100;
            const int y = event->y() - ui->frameListe->height() - ui->radar->y() - 100;

            if (x * x + y * y < 10000 && htr) {

                const int xf = (ui->affinvew->isChecked()) ? 1 : -1;
                const int yf = (ui->affinvns->isChecked()) ? -1 : 1;

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound(-100. * xf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                            sin(satellites.at(isat).getAzimut()));
                    const int bsat = qRound(-100. * yf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                            cos(satellites.at(isat).getAzimut()));

                    // Distance au carre du satellite au curseur
                    const int dt = (x - lsat) * (x - lsat) + (y - bsat) * (y - bsat);

                    // Le curseur est au dessus d'un satellite
                    if (dt <= 16) {
                        const QString norad = liste.at(isat);
                        liste[isat] = liste.at(0);
                        liste[0] = norad;

                        const bool bip = bipSat.at(isat);
                        bipSat[isat] = bipSat.at(0);
                        bipSat[0] = bip;

                        const TLE tle = tles.at(isat);
                        tles[isat] = tles.at(0);
                        tles[0] = tle;

                        const Satellite sat = satellites.at(isat);
                        satellites[isat] = satellites.at(0);
                        satellites[0] = sat;

                        // Ecriture des cles de registre
                        EcritureListeRegistre();

                        info = true;

                        CalculsAffichage();
                    }
                }
            }
        }

        if (!ui->ciel->isHidden()) {

            const int lciel = qRound(0.5 * ui->ciel->width());
            const int hciel = qRound(0.5 * ui->ciel->height());
            const int x1 = event->x() - ui->ciel->x() - lciel;
            const int y1 = event->y() - ui->ciel->y() - hciel;

            if (x1 * x1 + y1 * y1 < hciel * lciel) {

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound(-0.5 * ui->ciel->width() * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                            sin(satellites.at(isat).getAzimut()));
                    const int bsat = qRound(-0.5 * ui->ciel->height() * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                            cos(satellites.at(isat).getAzimut()));

                    // Distance au carre du satellite au curseur
                    const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                    if (dt <= 16) {
                        const QString norad = liste.at(isat);
                        liste[isat] = liste.at(0);
                        liste[0] = norad;

                        const bool bip = bipSat.at(isat);
                        bipSat[isat] = bipSat.at(0);
                        bipSat[0] = bip;

                        const TLE tle = tles.at(isat);
                        tles[isat] = tles.at(0);
                        tles[0] = tle;

                        const Satellite sat = satellites.at(isat);
                        satellites[isat] = satellites.at(0);
                        satellites[0] = sat;

                        // Ecriture des cles de registre
                        EcritureListeRegistre();

                        info = true;

                        CalculsAffichage();
                    }
                }
            }
        }


        if (modeFonctionnement->underMouse()) {
            if (ui->tempsReel->isChecked()) {
                ui->modeManuel->setChecked(true);
                return;
            }
            if (ui->modeManuel->isChecked()) {
                ui->tempsReel->setChecked(true);
                return;
            }
        }

        if (stsDate->underMouse() || stsHeure->underMouse()) {
            ui->calJulien->setChecked(!ui->calJulien->isChecked());
            CalculsAffichage();
        }
    }

    /* Retour */
    return;
}

/*
 * Deplacement de la souris sur les elements graphiques
 */
void PreviSat::mouseMoveEvent(QMouseEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */
    PreviSat::setToolTip("");
    ui->liste1->setToolTip("");
    messagesStatut->setText("");
    messagesStatut2->setVisible(false);
    messagesStatut3->setVisible(false);

    /* Corps de la methode */
    if (ui->affcoord->isChecked()) {

        // Deplacement de la souris sur la carte du monde
        if (!ui->carte->isHidden()) {

            const int x = event->x() - 6;
            const int y = event->y() - 6;

            if (x > 0 && x < ui->carte->width() - 1 && y > 0 && y < ui->carte->height() - 1) {

                // Longitude
                const double lo0 = 180. - x / DEG2PXHZ;
                const QString ews = (lo0 < 0.) ? tr("Est") : tr("Ouest");

                // Latitude
                const double la0 = 90. - y /DEG2PXVT;
                const QString nss = (la0 < 0.) ? tr("Sud") : tr("Nord");

                const QString msg = " : %1° %2";
                messagesStatut2->setText(tr("Longitude") + msg.arg(fabs(lo0), 6, 'f', 2, QChar('0')).arg(ews));
                messagesStatut3->setText(tr("Latitude") + msg.arg(fabs(la0), 5, 'f', 2, QChar('0')).arg(nss));
                messagesStatut2->setVisible(true);
                messagesStatut3->setVisible(true);

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound((180. - satellites.at(isat).getLongitude() * RAD2DEG) * DEG2PXHZ);
                    const int bsat = qRound((90. - satellites.at(isat).getLatitude() * RAD2DEG) * DEG2PXVT);

                    // Distance au carre du curseur au satellite
                    const int dt = (x - lsat) * (x - lsat) + (y - bsat) * (y - bsat);

                    // Le curseur est au-dessus d'un satellite
                    if (dt <= 16 && !satellites.at(isat).isIeralt()) {
                        const QString msg2 = tr("%1 (numéro NORAD : %2)");
                        messagesStatut->setText(msg2.arg(tles.at(isat).getNom()).arg(tles.at(isat).getNorad()));
                        PreviSat::setToolTip(messagesStatut->text());
                        PreviSat::setCursor(Qt::CrossCursor);
                        break;
                    } else {
                        if (messagesStatut->text().contains("NORAD"))
                            messagesStatut->setText("");
                        PreviSat::setCursor(Qt::ArrowCursor);
                    }
                }

                if (ui->affsoleil->isChecked()) {

                    const int lsol = qRound((180. - soleil.getLongitude() * RAD2DEG) * DEG2PXHZ);
                    const int bsol = qRound((90. - soleil.getLatitude() * RAD2DEG) * DEG2PXVT);

                    // Distance au carre du curseur au Soleil
                    const int dt = (x - lsol) * (x - lsol) + (y - bsol) * (y - bsol);

                    // Le curseur est au-dessus du Soleil
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Soleil"));
                        PreviSat::setToolTip(messagesStatut->text());
                        PreviSat::setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains("Soleil"))
                            messagesStatut->setText("");
                    }
                }

                if (ui->afflune->isChecked()) {

                    const int llun = qRound((180. - lune.getLongitude() * RAD2DEG) * DEG2PXHZ);
                    const int blun = qRound((90. - lune.getLatitude() * RAD2DEG) * DEG2PXVT);

                    // Distance au carre du curseur a la Lune
                    const int dt = (x - llun) * (x - llun) + (y - blun) * (y - blun);

                    //Le curseur est au-dessus de la Lune
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Lune"));
                        PreviSat::setToolTip(messagesStatut->text());
                        PreviSat::setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains("Lune"))
                            messagesStatut->setText("");
                    }
                }
            }
        }

        if (!ui->radar->isHidden()) {

            const int x0 = ui->frameZone->x() + ui->radar->x();
            const int y0 = ui->frameListe->height() + ui->radar->y();

            if (event->x() > x0 + 1 && event->x() < x0 + ui->radar->width() - 1 &&
                    event->y() > y0 + 1 && event->y() < y0 + ui->radar->height() - 1) {

                const int x1 = event->x() - x0 - 100;
                const int y1 = event->y() - y0 - 100;

                if (x1 * x1 + y1 * y1 < 10000 && htr) {

                    const int xf = (ui->affinvew->isChecked()) ? 1 : -1;
                    const int yf = (ui->affinvns->isChecked()) ? -1 : 1;

                    const double x2 = -x1 / (100. * xf);
                    const double y2 = -y1 / (100. * yf);

                    const double ht = 90. * (1. - sqrt(x2 * x2 + y2 * y2));
                    double az = atan2(x2, y2) * RAD2DEG;
                    if (az < 0.)
                        az += T360;

                    const QString msg = " : %1°";
                    messagesStatut2->setText(tr("Azimut") + msg.arg(fabs(az), 6, 'f', 2, QChar('0')));
                    messagesStatut3->setText(tr("Hauteur") + msg.arg(fabs(ht), 5, 'f', 2, QChar('0')));
                    messagesStatut2->setVisible(true);
                    messagesStatut3->setVisible(true);

                    for(int isat=0; isat<nbSat; isat++) {

                        const int lsat = qRound(-100. * xf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                                sin(satellites.at(isat).getAzimut()));
                        const int bsat = qRound(-100. * yf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                                cos(satellites.at(isat).getAzimut()));

                        const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                        // Le curseur est au-dessus du satellite
                        if (dt <= 16 && !satellites.at(isat).isIeralt()) {
                            QString msg2 = tr("%1 (numéro NORAD : %2)");
                            messagesStatut->setText(msg2.arg(tles.at(isat).getNom()).arg(tles.at(isat).getNorad()));
                            PreviSat::setToolTip(messagesStatut->text());
                            PreviSat::setCursor(Qt::CrossCursor);
                            break;
                        } else {
                            if (messagesStatut->text().contains("NORAD"))
                                messagesStatut->setText("");
                            PreviSat::setCursor(Qt::ArrowCursor);
                        }
                    }

                    // Soleil
                    if (ui->affsoleil->isChecked()) {

                        const int lsol = qRound(-100. * xf * (1. - soleil.getHauteur() * DEUX_SUR_PI) * sin(soleil.getAzimut()));
                        const int bsol = qRound(-100. * yf * (1. - soleil.getHauteur() * DEUX_SUR_PI) * cos(soleil.getAzimut()));

                        const int dt = (x1 - lsol) * (x1 - lsol) + (y1 - bsol) * (y1 - bsol);

                        // Le curseur est au-dessus du Soleil
                        if (dt <= 81) {
                            messagesStatut->setText(tr("Soleil"));
                            PreviSat::setToolTip(messagesStatut->text());
                            PreviSat::setCursor(Qt::CrossCursor);
                        } else {
                            if (messagesStatut->text().contains(tr("Soleil")))
                                messagesStatut->setText("");
                        }
                    }

                    // Lune
                    if (ui->afflune->isChecked()) {

                        const int llun = qRound(-100. * xf * (1. - lune.getHauteur() * DEUX_SUR_PI) * sin(lune.getAzimut()));
                        const int blun = qRound(-100. * yf * (1. - lune.getHauteur() * DEUX_SUR_PI) * cos(lune.getAzimut()));

                        const int dt = (x1 - llun) * (x1 - llun) + (y1 - blun) * (y1 - blun);

                        // Le curseur est au-dessus de la Lune
                        if (dt <= 81) {
                            messagesStatut->setText(tr("Lune"));
                            PreviSat::setToolTip(messagesStatut->text());
                            PreviSat::setCursor(Qt::CrossCursor);
                        } else {
                            if (messagesStatut->text().contains(tr("Lune")))
                                messagesStatut->setText("");
                        }
                    }
                }
            }
        }

        if (!ui->ciel->isHidden()) {

            const int lciel = qRound(0.5 * ui->ciel->width());
            const int hciel = qRound(0.5 * ui->ciel->height());
            const int x1 = event->x() - ui->ciel->x() - lciel;
            const int y1 = event->y() - ui->ciel->y() - hciel;

            if (x1 * x1 + y1 * y1 < hciel * lciel && htr) {

                const double x2 = -x1 / (double) lciel;
                const double y2 = -y1 / (double) hciel;

                const double ht = PI_SUR_DEUX * (1. - sqrt(x2 * x2 + y2 * y2));
                double az = atan2(x2, y2);
                if (az < 0.)
                    az += DEUX_PI;

                const double ch = cos(ht);
                const Vecteur3D vec1 = Vecteur3D(-cos(az) * ch, sin(az) * ch, sin(ht));
                const Vecteur3D vec2 = Vecteur3D(observateurs.at(0).getRotHz().Transposee() * vec1);

                // Declinaison
                const double dec = asin(vec2.getZ());

                // Ascension droite
                double ad = atan2(vec2.getY(), vec2.getX());
                if (ad < 0.)
                    ad += DEUX_PI;

                messagesStatut2->setText(tr("Ascension droite :") + " " +
                                         Maths::ToSexagesimal(ad, HEURE1, 2, 0, false, false).mid(0, 7));
                messagesStatut3->setText(tr("Déclinaison :") + " " +
                                         Maths::ToSexagesimal(dec, DEGRE, 2, 0, true, false).mid(0, 7));
                messagesStatut2->setVisible(true);
                messagesStatut3->setVisible(true);

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound(-0.5 * ui->ciel->width() * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                            sin(satellites.at(isat).getAzimut()));
                    const int bsat = qRound(-0.5 * ui->ciel->height() * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) *
                                            cos(satellites.at(isat).getAzimut()));

                    // Distance au carre du satellite au curseur
                    const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                    if (dt <= 16 && !satellites.at(isat).isIeralt()) {
                        QString msg2 = tr("%1 (numéro NORAD : %2)");
                        messagesStatut->setText(msg2.arg(tles.at(isat).getNom()).arg(tles.at(isat).getNorad()));
                        PreviSat::setToolTip(messagesStatut->text());
                        PreviSat::setCursor(Qt::CrossCursor);
                        break;
                    } else {
                        if (messagesStatut->text().contains("NORAD"))
                            messagesStatut->setText("");
                        PreviSat::setCursor(Qt::ArrowCursor);
                    }
                }

                if (ui->affplanetes->checkState() != Qt::Unchecked) {

                    for(int ipla=MERCURE; ipla<=NEPTUNE; ipla++) {

                        const int lpla = qRound(-0.5 * ui->ciel->width() * (1. - planetes.at(ipla).getHauteur() * DEUX_SUR_PI) *
                                                sin(planetes.at(ipla).getAzimut()));
                        const int bpla = qRound(-0.5 * ui->ciel->height() * (1. - planetes.at(ipla).getHauteur() * DEUX_SUR_PI) *
                                                cos(planetes.at(ipla).getAzimut()));

                        // Distance au carre du Soleil au curseur
                        const int dt = (x1 - lpla) * (x1 - lpla) + (y1 - bpla) * (y1 - bpla);

                        // Le curseur est au-dessus d'une planete
                        if (dt <= 16) {
                            messagesStatut->setText(planetes.at(ipla).getNom());
                            PreviSat::setToolTip(messagesStatut->text());
                            PreviSat::setCursor(Qt::CrossCursor);
                        } else {
                            if (messagesStatut->text().contains(planetes.at(ipla).getNom()))
                                messagesStatut->setText("");
                        }
                    }
                }

                if (ui->affsoleil->isChecked()) {

                    const int lsol = qRound(-0.5 * ui->ciel->width() * (1. - soleil.getHauteur() * DEUX_SUR_PI) *
                                            sin(soleil.getAzimut()));
                    const int bsol = qRound(-0.5 * ui->ciel->height() * (1. - soleil.getHauteur() * DEUX_SUR_PI) *
                                            cos(soleil.getAzimut()));

                    // Distance au carre du Soleil au curseur
                    const int dt = (x1 - lsol) * (x1 - lsol) + (y1 - bsol) * (y1 - bsol);

                    // Le curseur est au-dessus du Soleil
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Soleil"));
                        PreviSat::setToolTip(messagesStatut->text());
                        PreviSat::setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains(tr("Soleil")))
                            messagesStatut->setText("");
                    }
                }

                if (ui->afflune->isChecked()) {

                    const int llun = qRound(-0.5 * ui->ciel->width() * (1. - lune.getHauteur() * DEUX_SUR_PI) *
                                            sin(lune.getAzimut()));
                    const int blun = qRound(-0.5 * ui->ciel->height() * (1. - lune.getHauteur() * DEUX_SUR_PI) *
                                            cos(lune.getAzimut()));

                    // Distance au carre de la Lune au curseur
                    const int dt = (x1 - llun) * (x1 - llun) + (y1 - blun) * (y1 - blun);

                    // Le curseur est au-dessus de la Lune
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Lune"));
                        PreviSat::setToolTip(messagesStatut->text());
                        PreviSat::setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains(tr("Lune")))
                            messagesStatut->setText("");
                    }
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Double-clic sur l'interface
 */
void PreviSat::mouseDoubleClickEvent(QMouseEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->dateHeure1->underMouse() && ui->dateHeure1->isVisible()) {
        ui->modeManuel->setChecked(true);
        ui->dateHeure3->setFocus();
    }

    if (ui->dateHeure2->underMouse() && ui->dateHeure2->isVisible()) {
        ui->modeManuel->setChecked(true);
        ui->dateHeure4->setFocus();
    }

    if (!ui->carte->isHidden()) {
        const int x = event->x();
        const int y = event->y();
        if (x > 6 && x < 6 + ui->carte->width() && y > 6 && y < 6 + ui->carte->height())
            on_maximise_clicked();
    }

    if (!ui->ciel->isHidden()) {
        const int x = event->x() - ui->ciel->x();
        const int y = event->y() - ui->ciel->y();
        const int x0 = qRound(0.5 * ui->ciel->width());
        const int y0 = qRound(0.5 * ui->ciel->height());

        if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= x0 * y0)
            on_maximise_clicked();
    }

    /* Retour */
    return;
}

/*
 * Minimisation/maximisation de la carte du monde ou de la carte du ciel
 */
void PreviSat::on_maximise_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->frameOngletsZone->sizePolicy().verticalPolicy() == QSizePolicy::Preferred) {
        ui->frameOngletsZone->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
        ui->onglets->setVisible(false);
    } else {
        ui->frameOngletsZone->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        ui->onglets->setVisible(true);
    }

    if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Preferred) {

        // Carte maximisee
        ui->frameListe->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred));
        ui->frameCarte->setGeometry(0, 0, PreviSat::width(), PreviSat::height() - 23);
        ui->maximise->setIcon(QIcon(":/resources/mini.png"));
        ui->maximise->setToolTip(tr("Réduire"));
    } else {

        // Carte minimisee
        ui->frameListe->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        ui->frameCarte->setGeometry(0, 0, ui->frameCarteListe->width() - ui->frameListe->width(),
                                    PreviSat::height() - ui->frameOngletsZone->height() - 23);
        ui->maximise->setIcon(QIcon(":/resources/maxi.png"));
        ui->maximise->setToolTip(tr("Agrandir"));
    }

    QResizeEvent *event;
    resizeEvent(event);

    /* Retour */
    return;
}

void PreviSat::on_affichageCiel_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->ciel->isVisible()) {
        ui->carte->setVisible(true);
        ui->frameLat->setVisible(true);
        ui->frameLon->setVisible(true);
        ui->ciel->setVisible(false);
        ui->nord->setVisible(false);
        ui->sud->setVisible(false);
        ui->est->setVisible(false);
        ui->ouest->setVisible(false);
        ui->affichageCiel->setToolTip(tr("Carte du ciel"));
    } else {
        ui->carte->setVisible(false);
        ui->frameLat->setVisible(false);
        ui->frameLon->setVisible(false);
        ui->ciel->setVisible(true);
        ui->nord->setVisible(true);
        ui->sud->setVisible(true);
        ui->est->setVisible(true);
        ui->ouest->setVisible(true);
        ui->affichageCiel->setToolTip(tr("Carte du monde"));
    }

    // Enchainement de l'ensemble des calculs
    EnchainementCalculs();

    QResizeEvent *event;
    resizeEvent(event);

    /* Retour */
    return;
}

void PreviSat::on_directHelp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString locale = QLocale::system().name().section('_', 0, 0);
    const QString aide = "file:///" + dirExe + QDir::separator() + "aide" + QDir::separator() + "PreviSat_" + locale + ".pdf";

    /* Corps de la methode */
    if (!QDesktopServices::openUrl(aide)) {
        const QFileInfo fi(aide);
        const QString msg = tr("Impossible d'ouvrir le fichier d'aide %1");
        Messages::Afficher(msg.arg(fi.fileName()), WARNING);
    }

    /* Retour */
    return;
}


/*
 * Menu deroulant
 */
void PreviSat::on_actionOuvrir_fichier_TLE_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Ouverture d'un fichier TLE
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), settings.value("fichier/repTLE", dirTle).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz);;Tous les fichiers (*)"));

    try {
        if (!fichier.isEmpty()) {

            bool agz = false;
            int nsat;
            ficgz = "";
            QFileInfo fi(fichier);
            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = dirTmp + QDir::separator() + fi.completeBaseName();

                if (DecompressionFichierGz(fichier, fic)) {

                    nsat = TLE::VerifieFichier(fic, false);
                    if (nsat == 0)
                        throw PreviSatException(tr("POSITION : Erreur rencontrée lors de la décompression du fichier") + " " +
                                                fichier, WARNING);
                    ficgz = fichier;
                    fichier = fic;
                    agz = true;
                } else {
                    throw PreviSatException(tr("POSITION : Erreur rencontrée lors de la décompression du fichier") + " " +
                                            fichier, WARNING);
                }
            }

            messagesStatut->setText(tr("Ouverture du fichier TLE..."));

            // Verification du fichier TLE
            try {
                nsat = TLE::VerifieFichier(fichier, true);
                nomfic = fichier;
            } catch (PreviSatException &e) {
                messagesStatut->setText("");
                throw PreviSatException();
            }

            ui->nomFichierTLE->setText(fi.fileName());
            ui->lbl_nomFichierTLE->setVisible(true);
            ui->nomFichierTLE->setVisible(true);
            const QString chaine = tr("Fichier TLE OK : %1 satellites");
            messagesStatut->setText(chaine.arg(nsat));

            if (agz)
                settings.setValue("fichier/nom", ficgz);
            else
                settings.setValue("fichier/nom", nomfic);
            settings.setValue("fichier/repTLE", fi.absolutePath());

            // Ouverture du fichier TLE
            AfficherListeSatellites(nomfic, liste);

            // Mise a jour de la liste de satellites selectionnes
            if (nbSat > 0) {
                nor = liste.at(0);
                nbSat = getListeItemChecked(ui->liste1);

                if (nbSat > 0) {
                    liste.clear();
                    tles.clear();
                    tles.resize(nbSat);
                    bipSat.resize(nbSat);

                    int j = -1;
                    for (int i=0; i<ui->liste1->count(); i++) {
                        if (ui->liste1->item(i)->checkState() == Qt::Checked) {
                            liste.append(ui->liste1->item(i)->text().split("#").at(1));
                            if (liste.last() == nor)
                                j = liste.size() - 1;
                        }
                    }

                    if (j > 0) {
                        liste[j] = liste[0];
                        liste[0] = nor;
                    }

                    info = true;

                    EcritureListeRegistre();

                    if (nbSat == 0) {
                        l1 = "";
                        l2 = "";
                        liste.clear();
                    }

                    Satellite::initCalcul = false;

                    Satellite::LectureDonnees(liste, tles, satellites);

                    CalculsAffichage();

                    // Verification de l'age du TLE
                    if (!l1.isEmpty() && !l2.isEmpty()) {
                        old = false;
                        VerifAgeTLE();
                    }
                }
            } else {
                // Aucun satellite de la liste n'est dans le nouveau fichier
                ui->liste1->setCurrentRow(0);
                l1 = "";
                l2 = "";
                liste.clear();

                // Enchainement de l'ensemble des calculs
                EnchainementCalculs();

                // Affichage des donnees numeriques
                AffichageDonnees();

                //Affichage des elements sur la carte du monde et le radar
                AffichageCourbes();
            }
        }
    } catch (PreviSatException &ex) {
    }

    /* Retour */
    return;
}

void PreviSat::on_actionEnregistrer_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->actionEnregistrer->isVisible() && ui->onglets->currentIndex() < 3) {
        const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), settings.value("fichier/sauvegarde", dirOut).toString(),
                                                             tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
        if (!fichier.isEmpty()) {
            switch (ui->onglets->currentIndex()) {
            case 0:
                // Sauvegarde de l'onglet General
                SauveOngletGeneral(fichier);
                break;
            case 1:
                // Sauvegarde de l'onglet Elements osculateurs
                SauveOngletElementsOsculateurs(fichier);
                break;
            case 2:
                // Sauvegarde de l'onglet Informations satellite
                SauveOngletInformations(fichier);
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

void PreviSat::on_actionImprimer_carte_activated()
{
    /* Declarations des variables locales */
    QPrinter printer;

    /* Initialisations */

    /* Corps de la methode */
    printer.setOrientation((ui->carte->isVisible()) ? QPrinter::Landscape : QPrinter::Portrait);
    QPrintDialog dial(&printer, this);
    if (dial.exec() == QDialog::Accepted) {
        printer.newPage();
        QPainter p(&printer);

        const QPixmap pixmap = QPixmap::grabWidget((ui->carte->isVisible()) ? ui->carte : ui->ciel);
        const QPixmap pixscl = (pixmap.width() > printer.pageRect().width()) ?
                    pixmap.scaledToWidth(printer.pageRect().width()) : pixmap;
        const int x = (pixscl.width() == pixmap.width()) ? printer.pageRect().width() / 2 - pixscl.width() / 2 : 50;
        p.drawPixmap(x, 50, pixscl);
        p.end();
    }

    /* Retour */
    return;
}

void PreviSat::on_actionFichier_d_aide_activated(int arg1)
{
    on_directHelp_clicked();
}

void PreviSat::on_actionAstropedia_free_fr_activated()
{
    QDesktopServices::openUrl(QUrl("http://astropedia.free.fr/"));
}

void PreviSat::on_actionDonnez_votre_avis_activated()
{
    QDesktopServices::openUrl(QUrl("http://astropedia.free.fr/contacts/avis/avis.html"));
}

void PreviSat::on_actionTelecharger_les_mises_jour_activated()
{
    QDesktopServices::openUrl(QUrl("http://sourceforge.net/projects/previsat/"));
}

void PreviSat::on_actionRapport_de_bug_activated()
{
    QDesktopServices::openUrl(QUrl("http://astropedia.free.fr/contacts/rapport/rapport.html"));
}

void PreviSat::on_actionWww_celestrak_com_activated()
{
    QDesktopServices::openUrl(QUrl("http://www.celestrak.com"));
}

void PreviSat::on_actionWww_space_track_org_activated()
{
    QDesktopServices::openUrl(QUrl("http://www.space-track.org"));
}

void PreviSat::on_actionA_propos_activated(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Apropos *apropos = new Apropos;
    apropos->setWindowModality(Qt::ApplicationModal);
    apropos->show();

    /* Retour */
    return;
}


/*
 * Gestion de la liste principale de satellites
 */
void PreviSat::on_actionDefinir_par_defaut_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */
    int j = -1;

    /* Corps de la methode */
    for(int i=0; i<nbSat; i++) {
        if (liste.at(i) == nor) {
            j = i;
            break;
        }
    }

    // Le satellite selectionne fait deja partie de la liste
    if (j > 0) {
        liste[j] = liste.at(0);
        liste[0] = nor;
        nor = "";
    }

    // Le satellite selectionne ne fait pas partie de la liste : on l'ajoute dans la liste
    if (j == -1) {

        nbSat++;
        liste.append("");
        tles.resize(nbSat);
        bipSat.resize(nbSat);

        for(int i=nbSat-1; i>0; i--)
            liste[i] = liste.at(i-1);
        liste[0] = nor;
        nor = "";

        ui->liste1->setCurrentRow(ind);
        ui->liste2->setCurrentRow(ind);
        if (ui->liste1->currentItem()->checkState() == Qt::Unchecked)
            ui->liste1->currentItem()->setCheckState(Qt::Checked);
        ind = -1;
    }

    Satellite::initCalcul = false;

    // Ecriture des cles de registre
    EcritureListeRegistre();

    info = true;

    CalculsAffichage();

    /* Retour */
    return;
}

void PreviSat::on_actionNouveau_fichier_TLE_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStringList listeSat = liste;
    listeSat.sort();

    /* Corps de la methode */
    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), settings.value("fichier/sauvegarde", dirOut).toString(),
                                                     tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Tous les fichiers (*)"));

    QFile fichier(fic);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    for(int j=0; j<nbSat; j++) {
        for(int i=0; i<nbSat; i++) {
            if (tles.at(i).getNorad() == listeSat.at(j)) {
                flux << tles.at(i).getNom()    << endl <<
                        tles.at(i).getLigne1() << endl <<
                        tles.at(i).getLigne2() << endl;
                break;
            }
        }
    }
    fichier.close();

    const QFileInfo fi(fic);
    settings.setValue("fichier/sauvegarde", fi.absolutePath());

    const QString msg = tr("Fichier %1 créé");
    messagesStatut->setText(msg.arg(fichier.fileName()));

    /* Retour */
    return;
}
void PreviSat::on_actionFichier_TLE_existant_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), dirTle,
                                                     tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Tous les fichiers (*)"));

    try {
        bool atrouve2;
        // Verification que le fichier est un TLE
        int nsat = TLE::VerifieFichier(fic, true);

        // Lecture du TLE
        QStringList listeSat;
        QVector<TLE> tabtle;
        tabtle.resize(nsat);
        TLE::LectureFichier(fic, listeSat, tabtle);

        // Ajout des satellites selectionnes dans le tableau de TLE
        QFile fichier(fic);
        fichier.open(QIODevice::Append | QIODevice::Text);
        QTextStream flux(&fichier);

        for(int i=0; i<liste.size(); i++) {
            atrouve2 = false;
            for(int j=0; j<nsat; j++) {
                if (liste.at(i) == tabtle.at(j).getNorad()) {
                    atrouve2 = true;
                    break;
                }
            }
            if (!atrouve2) {
                flux << tles.at(i).getNom()    << endl <<
                        tles.at(i).getLigne1() << endl <<
                        tles.at(i).getLigne2() << endl;
            }
        }
        fichier.close();

        // Reecriture du fichier en triant par numero NORAD
        QFile fi(fic);
        const QString ficTmp = fic + ".tmp";
        fi.rename(ficTmp);

        nsat = TLE::VerifieFichier(ficTmp, false);
        tabtle.resize(nsat);
        TLE::LectureFichier(ficTmp, listeSat, tabtle);
        for(int i=0; i<nsat; i++)
            listeSat.append(tabtle.at(i).getNorad());

        listeSat.sort();

        fichier.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&fichier);

        for(int j=0; j<nsat; j++) {
            for(int i=0; i<nsat; i++) {
                if (tabtle.at(i).getNorad() == listeSat.at(j)) {
                    flux2 << tles.at(i).getNom()    << endl <<
                             tles.at(i).getLigne1() << endl <<
                             tles.at(i).getLigne2() << endl;
                    break;
                }
            }
        }
        fichier.close();

        const QString msg = tr("Fichier %1 augmenté de nouveaux satellites");
        messagesStatut->setText(msg.arg(fichier.fileName()));

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_liste1_clicked(const QModelIndex &index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QFile fi(nomfic);
    if (!fi.exists()) {
        const QString msg = tr("POSITION : Le fichier %1 n'existe pas");
        Messages::Afficher(msg.arg(nomfic), WARNING);

        // Remplissage du premier element du tableau de TLE avec le satellite en memoire
        nbSat = 0;
        tles.clear();
        tles.append(TLE(l1, l2));
        liste.clear();
        liste.append("");
        bipSat.resize(1);
        l1 = "";
        l2 = "";
        ui->liste1->clear();
        ui->liste2->clear();
        ui->liste3->clear();
    }

    /* Corps de la methode */
    if (ui->liste1->hasFocus()) {
        ind = ui->liste1->currentRow();
        if (ind >= 0)
            nor = ui->liste1->item(ind)->text().split("#").at(1);

        if (ui->liste1->currentItem()->checkState() == Qt::Checked) {

            // Suppression d'un satellite de la liste
            for(int i=0; i<liste.size(); i++) {
                if (ui->liste1->item(ind)->text().split("#").at(1) == liste.at(i)) {
                    ui->liste1->currentItem()->setCheckState(Qt::Unchecked);
                    liste.removeAt(i);
                    tles.remove(i);
                    bipSat.remove(i);
                    ui->liste2->item(ind)->setCheckState(Qt::Unchecked);
                    ui->liste3->item(ind)->setCheckState(Qt::Unchecked);
                    break;
                }
            }
            nbSat--;

        } else {

            // Ajout d'un satellite dans la liste
            liste.append(ui->liste1->item(ind)->text().split("#").at(1));
            nbSat++;
            tles.resize(nbSat);
            bipSat.resize(nbSat);
            ui->liste1->currentItem()->setCheckState(Qt::Checked);
            ui->liste2->item(ind)->setCheckState(Qt::Checked);
            ui->liste3->item(ind)->setCheckState(Qt::Checked);
        }

        if (nbSat == 0) {
            l1 = "";
            l2 = "";
        }

        Satellite::initCalcul = false;
        info = true;

        // Ecriture des cles de registre
        EcritureListeRegistre();

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        if (satellites.at(0).isIeralt() && ui->liste1->currentItem()->checkState() == Qt::Checked) {
            chronometre->stop();
            const QString msg = tr("POSITION : Erreur rencontrée lors de l'exécution\nLa position du satellite %1 (numéro NORAD : %2) ne peut pas être calculée (altitude négative)");
            Messages::Afficher(msg.arg(tles.at(0).getNom()).arg(tles.at(0).getNorad()), WARNING);
            chronometre->start();
            l1 = "";
            l2 = "";
        }

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_liste1_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste1->currentRow() >= 0)
        ui->menuContextuelListe1->exec(QCursor::pos());

    /* Retour */
    return;
}

void PreviSat::on_liste1_entered(const QModelIndex &index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->liste1->setToolTip("");

    /* Corps de la methode */
    const int r = index.row();
    const QStringList sat = ui->liste1->item(r)->text().split("#");
    const QString nomsat = sat.at(0).trimmed();
    const QString norad = sat.at(1);
    if (nomsat != norad) {
        const QString msg = tr("%1 (numéro NORAD : %2)");
        ui->liste1->setToolTip(msg.arg(nomsat).arg(norad));
    }

    /* Retour */
    return;
}


/*
 * Gestion de l'onglet General
 */
void PreviSat::on_lieuxObservation1_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->lieuxObservation1->hasFocus() && index > 0) {

        const QString chaine = listeObs.at(0);
        listeObs[0] = listeObs.at(index);
        listeObs[index] = chaine;

        QString nomlieu = "";
        for(int i=0; i<listeObs.count(); i++) {
            if (i > 0)
                nomlieu += "$";
            nomlieu += listeObs.at(i);
        }
        settings.setValue("observateur/lieu", nomlieu);

        AffichageLieuObs();

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_tempsReel_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {

        if (tim.isValid()) {

            // Date actuelle
            dateCourante = Date(offsetUTC);

            // Enchainement de l'ensemble des calculs
            EnchainementCalculs();

            // Affichage des donnees numeriques
            AffichageDonnees();

            // Affichage des elements graphiques
            AffichageCourbes();
        }

        tim = QDateTime::currentDateTime();
        ui->pasManuel->setVisible(false);
        ui->valManuel->setVisible(false);
        ui->dateHeure1->setVisible(true);
        ui->dateHeure2->setVisible(true);
        ui->dateHeure3->setVisible(false);
        ui->dateHeure4->setVisible(false);
        ui->utcManuel->setVisible(false);
        ui->utcManuel2->setVisible(false);
        ui->pasReel->setVisible(true);
        ui->secondes->setVisible(true);
        ui->frameSimu->setVisible(false);
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
        ui->pasReel->setVisible(false);
        ui->secondes->setVisible(false);
        ui->pasManuel->setVisible(true);
        ui->valManuel->setVisible(true);
        ui->dateHeure3->setDateTime(QDateTime::currentDateTime());
        ui->dateHeure3->setVisible(true);
        ui->dateHeure4->setDateTime(ui->dateHeure3->dateTime());
        ui->dateHeure4->setVisible(true);
        ui->dateHeure1->setVisible(false);
        ui->dateHeure2->setVisible(false);
        ui->utcManuel->setVisible(true);
        ui->utcManuel2->setVisible(true);
        ui->frameSimu->setVisible(true);
        ui->pasManuel->setFocus();
    }

    /* Retour */
    return;
}

void PreviSat::on_dateHeure3_dateTimeChanged(const QDateTime &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    dateCourante = Date(date.date().year(), date.date().month(), date.date().day(),
                        date.time().hour(), date.time().minute(), date.time().second(),
                        dateCourante.getOffsetUTC());

    if (ui->modeManuel->isChecked())
        info = true;

    // Enchainement de l'ensemble des calculs
    EnchainementCalculs();

    // Affichage des donnees numeriques
    AffichageDonnees();

    // Affichage des elements graphiques
    AffichageCourbes();

    /* Retour */
    return;
}

void PreviSat::on_dateHeure4_dateTimeChanged(const QDateTime &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    dateCourante = Date(date.date().year(), date.date().month(), date.date().day(),
                        date.time().hour(), date.time().minute(), date.time().second(), dateCourante.getOffsetUTC());

    ui->dateHeure3->setDateTime(date);

    /* Retour */
    return;
}

void PreviSat::on_play_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->play->setEnabled(!ui->play->isEnabled());
    const bool enb = !ui->play->isEnabled();
    ui->pause->setEnabled(enb);
    ui->rewind->setEnabled(enb);
    ui->forward->setEnabled(enb);
    ui->backward->setEnabled(enb);
    ui->frameSimu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_pause_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->pause->setEnabled(!ui->pause->isEnabled());
    const bool enb = !ui->pause->isEnabled();
    ui->play->setEnabled(enb);
    ui->rewind->setEnabled(enb);
    ui->forward->setEnabled(enb);
    ui->backward->setEnabled(enb);
    ui->frameSimu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_rewind_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->rewind->setEnabled(!ui->rewind->isEnabled());
    const bool enb = !ui->rewind->isEnabled();
    ui->play->setEnabled(enb);
    ui->pause->setEnabled(enb);
    ui->forward->setEnabled(enb);
    ui->backward->setEnabled(enb);
    ui->frameSimu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_forward_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->forward->setEnabled(!ui->forward->isEnabled());
    const bool enb = !ui->forward->isEnabled();
    ui->play->setEnabled(enb);
    ui->pause->setEnabled(enb);
    ui->rewind->setEnabled(enb);
    ui->backward->setEnabled(enb);
    ui->frameSimu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_backward_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->backward->setEnabled(!ui->backward->isEnabled());
    const bool enb = !ui->backward->isEnabled();
    ui->play->setEnabled(enb);
    ui->pause->setEnabled(enb);
    ui->rewind->setEnabled(enb);
    ui->forward->setEnabled(enb);
    ui->frameSimu->setFocus();

    /* Retour */
    return;
}


/*
 * Gestion des options d'affichage
 */
void PreviSat::on_affsoleil_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affnuit_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_intensiteOmbre_valueChanged(int value)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->intensiteOmbre->isVisible()) {
        AffichageCourbes();
        ui->intensiteOmbre->setToolTip(QString::number(value));
    }

    /* Retour */
    return;
}

void PreviSat::on_affgrille_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_afflune_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affphaselune_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_rotationLune_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affnomsat_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affvisib_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_afftraj_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_nombreTrajectoires_valueChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affradar_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affinvns_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affinvew_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affnomlieu_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affnotif_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_calJulien_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affcoord_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_extinctionAtmospherique_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affetoiles_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affconst_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_magnitudeEtoiles_valueChanged(double arg1)
{
    ModificationOption();
}

void PreviSat::on_affSAA_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_affplanetes_stateChanged(int arg1)
{
    ModificationOption();
}

void PreviSat::on_unitesKm_toggled(bool checked)
{
    ModificationOption();
}

void PreviSat::on_unitesMi_toggled(bool checked)
{
    ModificationOption();
}

void PreviSat::on_heureLegale_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->options->isVisible() && checked) {

        const double offset = ui->updown->value() * NB_JOUR_PAR_MIN;
        dateCourante = Date(dateCourante, offset);

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_utc_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->options->isVisible() && checked) {

        dateCourante = Date(dateCourante, 0.);

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_updown_valueChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QTime heur;
    heur = heur.addSecs(fabs(arg1 * NB_SEC_PAR_MIN));
    const QString sgn = (arg1 >= 0) ? " + " : " - ";
    ui->tuc->setText(tr("UTC") + sgn + heur.toString("hh:mm"));
    const double offset = ui->updown->value() * NB_JOUR_PAR_MIN;

    if (ui->options->isVisible()) {
        if (ui->heureLegale->isChecked()) {
            dateCourante = Date(dateCourante, offset);

            // Enchainement de l'ensemble des calculs
            EnchainementCalculs();

            // Affichage des donnees numeriques
            AffichageDonnees();

            // Affichage des elements graphiques
            AffichageCourbes();
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_utcAuto_stateChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->utcAuto->isChecked()) {

        const QDateTime dateLocale = QDateTime::currentDateTime();
        QDateTime dateUTC(dateLocale);
        dateUTC.setTimeSpec(Qt::UTC);

        const int ecart = (int) (dateLocale.secsTo(dateUTC) * NB_MIN_PAR_SEC + EPS_DATES);
        ui->updown->setValue(ecart);

        const double offset = ui->updown->value() * NB_JOUR_PAR_MIN;
        dateCourante = Date(dateCourante, offset);
        CalculsAffichage();
    }

    /* Retour */
    return;
}

void PreviSat::on_listeMap_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->optionAffichage->isVisible() && selec2 == 0) {
        if (index == 0) {
            settings.setValue("fichier/listeMap", "");
        } else {
            if (index == ui->listeMap->count() - 1) {
                selec2 = -1;

                Telecharger *telecharger = new Telecharger(2);
                telecharger->setWindowModality(Qt::ApplicationModal);
                telecharger->show();
                ui->listeMap->setCurrentIndex(ui->listeMap->findText(settings.value("fichier/listeMap", "").toString()));
                InitFicMap(true);
                selec2 = 0;
            } else {
                settings.setValue("fichier/listeMap", (index == 0) ? "" : ficMap.at(qMax(0, ui->listeMap->currentIndex()-1)));
            }
        }
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_majFicPrevisat_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    dirDwn = dirTmp;
    amajInt = true;
    atrouve = false;
    aupdnow = false;

    /* Corps de la methode */
    // Recuperation des informations de mise a jour
    AjoutFichier(QUrl("http://astropedia.free.fr/previsat/Qt/maj"));
    if (downQueue.isEmpty())
        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));

    /* Retour */
    return;
}


/*
 * Gestion des lieux d'observation
 */
void PreviSat::on_actionCreer_une_categorie_activated(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->nouvelleCategorie->setVisible(true);
    ui->nouveauLieu->setVisible(false);
    ui->coordonnees->setVisible(false);
    ui->nvCategorie->setText("");
    ui->nvCategorie->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_actionSupprimerCategorie_activated(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = ui->fichiersObs->currentItem()->text().toLower();
    QString msg = tr("Voulez-vous vraiment supprimer la catégorie \"%1\"?");
    const int res = QMessageBox::question(this, tr("Avertissement"), msg.arg(ui->fichiersObs->currentItem()->text()),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (res == QMessageBox::No) {
        messagesStatut->setText("");
    } else {
        QFile fi(dirCoo + QDir::separator() + fic);
        fi.remove();
        ui->lieuxObs->clear();
        InitFicObs(false);
        ui->fichiersObs->setCurrentRow(0);
        msg = tr("La catégorie \"%1\" a été supprimée");
        messagesStatut->setText(msg.arg(ui->fichiersObs->currentItem()->text()));
    }

    /* Retour */
    return;
}

void PreviSat::on_actionTelechargerCategorie_activated(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Telecharger *telecharger = new Telecharger(1);
    telecharger->setWindowModality(Qt::ApplicationModal);
    telecharger->show();

    /* Retour */
    return;
}

void PreviSat::on_annulerCategorie_clicked()
{
    ui->nouvelleCategorie->setVisible(false);
}

void PreviSat::on_validerCategorie_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->nvCategorie->text().isEmpty()) {
        Messages::Afficher(tr("Le nom de la catégorie n'est pas spécifié"), WARNING);
    } else {
        int cpt = 0;
        for(int i=0; i<ui->fichiersObs->count(); i++) {
            if (ui->nvCategorie->text().toLower() == ui->fichiersObs->item(i)->text().toLower())
                cpt++;
        }
        if (cpt == 0) {
            QDir di(dirCoo);
            if (!di.exists())
                di.mkdir(dirCoo);

            QFile fi(dirCoo + QDir::separator() + ui->nvCategorie->text().toLower());
            fi.open(QIODevice::WriteOnly | QIODevice::Text);
            fi.write("");
            fi.close();
            InitFicObs(false);

            if (ui->fichiersObs->count() > 0)
                ui->fichiersObs->setCurrentRow(0);
            messagesStatut->setText(tr("La nouvelle catégorie de lieux d'observation a été créée"));
            ui->nouvelleCategorie->setVisible(false);

        } else {
            Messages::Afficher(tr("La catégorie spécifiée existe déjà"), WARNING);
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionRenommerCategorie_activated()
{
    /* Declarations des variables locales */
    bool ok;

    /* Initialisations */

    /* Corps de la methode */
    const QString nvNomCateg = QInputDialog::getText(0, tr("Catégorie"), tr("Nouveau nom de la catégorie :"),
                                                     QLineEdit::Normal, ui->fichiersObs->currentItem()->text(), &ok);

    if (ok && !nvNomCateg.trimmed().isEmpty()) {
        QFile fi(dirCoo + QDir::separator() + ui->fichiersObs->currentItem()->text().toLower());
        fi.rename(dirCoo + QDir::separator() + nvNomCateg.trimmed().toLower());
        InitFicObs(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_fichiersObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->coordonnees->setVisible(false);
    ui->selecLieux->setCurrentRow(-1);
    if (selec != currentRow) {
        selec = currentRow;
        if (selec < 0)
            return;
    }
    messagesStatut->setText("");

    // Affichage des lieux d'observations contenus dans le fichier
    ui->lieuxObs->clear();
    mapObs.clear();
    QFile fi(ficObs.at(selec));
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux (&fi);
    while (!flux.atEnd()) {
        QString ligne = "";
        do {
            ligne = flux.readLine();
        } while (ligne.isEmpty());
        ui->lieuxObs->addItem(ligne.mid(34));
        mapObs.append(ligne.mid(34).trimmed() + " #" + ligne.mid(0, 33).replace(" ", "&"));
    }
    mapObs.sort();

    /* Retour */
    return;
}

void PreviSat::on_fichiersObs_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->actionSupprimerCategorie->setVisible(ui->fichiersObs->currentRow() > 0);
    ui->actionRenommerCategorie->setVisible(ui->fichiersObs->currentRow() > 0);
    ui->menuContextuelCategorie->exec(QCursor::pos());

    /* Retour */
    return;
}

void PreviSat::on_lieuxObs_currentRowChanged(int currentRow)
{
    AfficherLieuSelectionne(currentRow);
}

void PreviSat::on_lieuxObs_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->lieuxObs->indexAt(pos).row() < 0) {
        ui->actionRenommerLieu->setVisible(false);
        ui->actionModifier_coordonnees->setVisible(false);
        ui->actionSupprimerLieu->setVisible(false);
        ui->actionAjouter_Mes_Preferes->setVisible(false);
    } else {
        ui->actionRenommerLieu->setVisible(true);
        ui->actionModifier_coordonnees->setVisible(true);
        ui->actionSupprimerLieu->setVisible(true);
        ui->actionAjouter_Mes_Preferes->setVisible(ui->fichiersObs->currentRow() > 0);
    }
    ui->actionCreer_un_nouveau_lieu->setVisible(ui->fichiersObs->count() > 0);

    ui->menuContextuelLieux->exec(QCursor::pos());

    /* Retour */
    return;
}

void PreviSat::on_selecLieux_currentRowChanged(int currentRow)
{
    AfficherLieuSelectionne(currentRow);
}

void PreviSat::on_actionCreer_un_nouveau_lieu_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->nouvelleCategorie->setVisible(false);
    ui->nouveauLieu->setVisible(true);
    ui->coordonnees->setVisible(false);
    ui->nvLieu->setText("");
    ui->nvLongitude->setText("000°00'00\"");
    ui->nvLatitude->setText("000°00'00\"");
    ui->nvAltitude->setText("0000");
    ui->lbl_nvUnite->setText((ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
    ui->lbl_ajouterDans->setVisible(true);
    ui->ajdfic->setVisible(true);
    ui->ajdfic->setCurrentIndex(0);
    messagesStatut->setText("");
    ui->nvLieu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_actionAjouter_Mes_Preferes_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString fic = ficObs.at(0);
    if (!fic.contains("aapreferes")) {
        // Le fichier aapreferes n'existe pas, on le cree
        ficObs.insert(0, dirCoo + QDir::separator() + "aapreferes");
        fic = ficObs.at(0);

        QFile fi(fic);
        fi.open(QIODevice::WriteOnly | QIODevice::Text);
        fi.write("");
        fi.close();
        InitFicObs(false);
    }

    /* Corps de la methode */
    const QString nomlieu = mapObs.at(ui->lieuxObs->currentRow());
    const QString lieu = nomlieu.split("#").at(0).toLower().trimmed();

    // Verification que le lieu d'observation n'existe pas deja dans Mes Preferes
    QFile fichier(fic);
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);
    bool atrouve2 = false;
    while (!flux.atEnd() && !atrouve2) {
        const QString ligne = flux.readLine().mid(34).toLower().trimmed();
        atrouve2 = (lieu == ligne);
    }

    if (atrouve2) {
        const QString msg = tr("Le lieu d'observation \"%1\" fait déjà partie de \"Mes Préférés\"");
        Messages::Afficher(msg.arg(nomlieu.split("#").at(0)), WARNING);
    } else {

        const QStringList lieux = nomlieu.split("#");
        const QStringList coord = lieux.at(1).split("&");

        QString ligne = "%1 %2 %3 %4";
        ligne = ligne.arg(coord.at(0)).arg(coord.at(1)).arg(coord.at(2)).arg(lieux.at(0));

        // Ajout du lieu d'observation dans Mes Preferes
        QFile fich(fic);
        fich.open(QIODevice::Append | QIODevice::Text);
        QTextStream flux2(&fich);
        flux2 << ligne << endl;
        fich.close();

        const QString msg = tr("Le lieu d'observation \"%1\" a été ajouté dans la catégorie \"Mes Preferes\"");
        messagesStatut->setText(msg.arg(lieu.at(0)));
    }

    /* Retour */
    return;
}

void PreviSat::on_actionModifier_coordonnees_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->nouvelleCategorie->setVisible(false);
    ui->nouveauLieu->setVisible(true);
    ui->coordonnees->setVisible(false);

    const QString obs = mapObs.at(ui->lieuxObs->currentRow());
    ui->nvLieu->setText(obs.mid(0, obs.indexOf("#")).trimmed());

    const QStringList coord = obs.mid(obs.indexOf("#") + 1).split("&");
    const double lo = coord.at(0).toDouble();
    const double la = coord.at(1).toDouble();
    const int atd = coord.at(2).toInt();

    ui->nvLongitude->setText(Maths::ToSexagesimal(fabs(lo) * DEG2RAD, DEGRE, 3, 0, false, true));
    ui->nvEw->setCurrentIndex((lo <= 0.) ? 0 : 1);
    ui->nvLatitude->setText(Maths::ToSexagesimal(fabs(la) * DEG2RAD, DEGRE, 2, 0,false, true));
    ui->nvNs->setCurrentIndex((la >= 0.) ? 0 : 1);
    const QString alt = "%1";
    ui->nvAltitude->setText(alt.arg((ui->unitesKm->isChecked()) ? atd : (int) (atd * PIED_PAR_METRE), 4, 10, QChar('0')));
    ui->lbl_nvUnite->setText((ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
    ui->lbl_ajouterDans->setVisible(false);
    ui->ajdfic->setVisible(false);

    // Suppression du lieu du fichier
    const QStringList lieu = mapObs.at(ui->lieuxObs->currentRow()).split("#");
    const QString msg2 = "%1%2 %3%4 %5 %6";
    const QString ligne =  msg2.arg((lo >= 0.) ? "+" : "-").arg(fabs(lo), 13, 'f', 9, QChar('0'))
            .arg((la >= 0.) ? "+" : "-").arg(fabs(la), 12, 'f', 9, QChar('0')).arg(atd, 4, 10, QChar('0'))
            .arg(lieu.at(0)).trimmed();

    const QString fic = (ui->fichiersObs->currentRow() == 0) ?
                dirCoo + QDir::separator() + "aapreferes" :
                dirCoo + QDir::separator() + ui->fichiersObs->currentItem()->text();

    QFile sr(fic);
    sr.open(QIODevice::ReadOnly | QIODevice::Text);

    QFile sw(dirTmp + QDir::separator() + "obs.tmp");
    sw.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream flux(&sr);
    QTextStream flux2(&sw);
    while (!flux.atEnd()) {
        const QString ligne2 = flux.readLine();
        if (ligne != ligne2)
            flux2 << ligne2 << endl;
    }
    sw.close();
    sr.close();

    sr.remove();
    sw.rename(fic);

    messagesStatut->setText("");
    ui->nvLieu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_validerObs_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString nomlieu = ui->nvLieu->text().trimmed();
    try {
        if (nomlieu.isEmpty()) {
            throw PreviSatException(tr("Le nom du lieu d'observation n'est pas spécifié"), WARNING);
        } else {

            // Recuperation de la longitude
            const int lo1 = ui->nvLongitude->text().mid(0, 3).toInt();
            const int lo2 = ui->nvLongitude->text().mid(4, 2).toInt();
            const int lo3 = ui->nvLongitude->text().mid(7, 2).toInt();

            if (lo1 < 0 || lo1 > 180 || lo2 < 0 || lo2 > 59 || lo3 < 0 || lo3 > 59)
                throw PreviSatException(tr("Erreur dans la saisie de la longitude"), WARNING);

            // Recuperation de la latitude
            const int la1 = ui->nvLatitude->text().mid(0, 2).toInt();
            const int la2 = ui->nvLatitude->text().mid(3, 2).toInt();
            const int la3 = ui->nvLatitude->text().mid(6, 2).toInt();

            if (la1 < 0 || la1 > 90 || la2 < 0 || la2 > 59 || la3 < 0 || la3 > 59)
                throw PreviSatException(tr("Erreur dans la saisie de la latitude"), WARNING);

            // Recuperation de l'altitude
            int atd = ui->nvAltitude->text().mid(0, 4).toInt();
            if (ui->unitesMi->isChecked())
                atd = qRound(atd / PIED_PAR_METRE);
            if (atd < -500 || atd > 8900)
                throw PreviSatException(tr("Erreur dans la saisie de l'altitude"), WARNING);

            const QString fic = ficObs.at(ui->ajdfic->currentIndex());

            // Ajout du lieu d'observation dans le fichier de coordonnees selectionne
            QFile fi(fic);
            fi.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream flux(&fi);
            while (!flux.atEnd()) {
                const QString ligne = flux.readLine();
                if (ligne.mid(34).toLower().trimmed() == nomlieu.toLower().trimmed()) {
                    const QString msg = tr("Le lieu existe déjà dans la catégorie \"%1\"");
                    throw PreviSatException(msg.arg(ui->ajdfic->currentText()), WARNING);
                }
            }
            const double x1 = ((ui->nvEw->currentText() == tr("Est")) ? -1 : 1) * (lo1 + lo2 * DEG_PAR_ARCMIN + lo3 * DEG_PAR_ARCSEC);
            const double x2 = ((ui->nvNs->currentText() == tr("Sud")) ? -1 : 1) * (la1 + la2 * DEG_PAR_ARCMIN + la3 * DEG_PAR_ARCSEC);

            const QString msg = "%1%2 %3%4 %5 %6";
            flux << msg.arg((x1 >= 0.) ? "+" : "-").arg(fabs(x1), 13, 'f', 9, QChar('0'))
                    .arg((x2 >= 0.) ? "+" : "-").arg(fabs(x2), 12, 'f', 9, QChar('0')).arg(atd, 4, 10, QChar('0'))
                    .arg(nomlieu) << endl;
            fi.close();

            on_fichiersObs_currentRowChanged(0);
            ui->nouveauLieu->setVisible(false);
            selec = -1;
        }
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_annulerObs_clicked()
{
    ui->nouveauLieu->setVisible(false);
}

void PreviSat::on_actionRenommerLieu_activated()
{
    /* Declarations des variables locales */
    bool ok;

    /* Initialisations */

    /* Corps de la methode */
    const QString nvNomLieu = QInputDialog::getText(0, tr("Lieu d'observation"), tr("Nouveau nom du lieu d'observation :"),
                                                    QLineEdit::Normal, ui->lieuxObs->currentItem()->text(), &ok);

    if (ok && !nvNomLieu.trimmed().isEmpty()) {

        const QString fic = (ui->fichiersObs->currentRow() == 0) ?
                    dirCoo + QDir::separator() + "aapreferes" :
                    dirCoo + QDir::separator() + ui->fichiersObs->currentItem()->text();

        QFile sr(fic);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);

        QFile sw(dirTmp + QDir::separator() + "obs.tmp");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream flux(&sr);
        QTextStream flux2(&sw);
        while (!flux.atEnd()) {
            const QString ligne = flux.readLine();
            if (ligne.mid(34).toLower().trimmed() == ui->lieuxObs->currentItem()->text().toLower().trimmed())
                flux2 << ligne.mid(0, 34) << nvNomLieu.trimmed() << endl;
            else
                flux2 << ligne << endl;
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(fic);
        on_fichiersObs_currentRowChanged(ui->fichiersObs->currentRow());
    }

    /* Retour */
    return;
}

void PreviSat::on_actionSupprimerLieu_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = ficObs.at(ui->fichiersObs->currentRow());
    const QString nomlieu = ui->lieuxObs->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer \"%1\" de la catégorie \"%2\"?");
    const int res = QMessageBox::question(this, tr("Avertissement"), msg.arg(nomlieu).arg(ui->fichiersObs->currentItem()->text()),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (res == QMessageBox::Yes) {

        const QStringList lieu = mapObs.at(ui->lieuxObs->currentRow()).split("#");
        const QStringList coord = lieu.at(1).split("&");
        const double lo = coord.at(0).toDouble();
        const double la = coord.at(1).toDouble();
        const int atd = coord.at(2).toInt();
        const QString msg2 = "%1%2 %3%4 %5 %6";
        const QString ligne =  msg2.arg((lo >= 0.) ? "+" : "-").arg(fabs(lo), 13, 'f', 9, QChar('0'))
                .arg((la >= 0.) ? "+" : "-").arg(fabs(la), 12, 'f', 9, QChar('0')).arg(atd, 4, 10, QChar('0'))
                .arg(lieu.at(0));

        QFile sr(fic);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);

        QFile sw(dirTmp + QDir::separator() + "obs.tmp");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream flux(&sr);
        QTextStream flux2(&sw);
        while (!flux.atEnd()) {
            const QString ligne2 = flux.readLine();
            if (ligne.trimmed() != ligne2.trimmed())
                flux2 << ligne2 << endl;
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(fic);
        on_fichiersObs_currentRowChanged(0);
        selec = -1;
        const QString msg3 = tr("Le lieu d'observation \"%1\" a été supprimé de la catégorie \"%2\"");
        messagesStatut->setText(msg3.arg(nomlieu).arg(ui->fichiersObs->currentItem()->text()));
    } else {
        messagesStatut->setText("");
    }

    /* Retour */
    return;
}

void PreviSat::on_ajoutLieu_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        if (ui->lieuxObs->currentRow() >= 0) {
            for(int i=0; i<ui->selecLieux->count(); i++) {
                if (ui->selecLieux->item(i)->text() == ui->lieuxObs->currentItem()->text()) {
                    messagesStatut->setText(tr("Lieu d'observation déjà sélectionné"));
                    throw PreviSatException();
                }
            }
            ui->selecLieux->addItem(ui->lieuxObs->currentItem()->text());
            listeObs.append(mapObs.at(ui->lieuxObs->currentRow()));

            QString nomslieux = "";
            QStringListIterator it(listeObs);
            while (it.hasNext()) {
                nomslieux += it.next() + "$";
            }
            nomslieux.remove(nomslieux.length() - 1, 1);
            settings.setValue("observateur/lieu", nomslieux);
            AffichageLieuObs();
            AffichageCourbes();
            ui->lieuxObs->setFocus();
        }
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_supprLieu_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->selecLieux->currentRow() >= 0 && ui->selecLieux->count() > 1) {
        listeObs.removeAt(ui->selecLieux->currentRow());
        ui->selecLieux->removeItemWidget(ui->selecLieux->currentItem());

        QString nomslieux = "";
        QStringListIterator it(listeObs);
        while (it.hasNext()) {
            nomslieux += it.next() + "$";
        }
        nomslieux.remove(nomslieux.length() - 1, 1);
        settings.setValue("observateur/lieu", nomslieux);

        AffichageLieuObs();
        EnchainementCalculs();
        AffichageDonnees();
        AffichageCourbes();
        ui->coordonnees->setVisible(false);
    }

    /* Retour */
    return;
}


/*
 * Gestion des onglets
 */
void PreviSat::on_barreMenu_pressed()
{
    ui->actionEnregistrer->setVisible((ui->onglets->currentIndex() < 3 && ui->onglets->count() == 7) ||
                                      ui->onglets->currentIndex() < 1);
}

void PreviSat::on_onglets_currentChanged(QWidget *arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */
    messagesStatut->setText("");
    ui->compteRenduMaj->setVisible(false);

    /* Corps de la methode */
    if (arg1 == ui->osculateurs) {
        if (ui->modeManuel->isChecked())
            ui->dateHeure4->setDateTime(ui->dateHeure3->dateTime());

    } else if (arg1 == ui->previsions) {
        const Date date(dateCourante.getJourJulien() + EPS_DATES, 0.);
        ui->dateInitialePrev->setDateTime(date.ToQDateTime(0));
        ui->dateFinalePrev->setDateTime(ui->dateInitialePrev->dateTime().addDays(7));

    } else if (arg1 == ui->iridium) {
        const Date date(dateCourante.getJourJulien() + EPS_DATES, 0.);
        ui->dateInitialeIri->setDateTime(date.ToQDateTime(0));
        ui->dateFinaleIri->setDateTime(ui->dateInitialeIri->dateTime().addDays(7));

    } else {
    }

    /* Retour */
    return;
}

void PreviSat::on_ongletsOutils_currentChanged(QWidget *arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (arg1 == ui->evenementsOrbitaux) {
        const Date date(dateCourante.getJourJulien() + EPS_DATES, 0.);
        ui->dateInitialeEvt->setDateTime(date.ToQDateTime(0));
        ui->dateFinaleEvt->setDateTime(ui->dateInitialeEvt->dateTime().addDays(7));

    } else if (arg1 == ui->transitsISS) {
        const Date date(dateCourante.getJourJulien() + EPS_DATES, 0.);
        ui->dateInitialeTransit->setDateTime(date.ToQDateTime(0));
        ui->dateFinaleTransit->setDateTime(ui->dateInitialeTransit->dateTime().addDays(7));

    } else {
    }

    /* Retour */
    return;
}


/*
 * Mise a jour des TLE
 */
void PreviSat::on_MajMaintenant_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    aupdnow = true;
    dirDwn = dirTmp;
    const QString groupeTLE = ui->groupeTLE->currentText().toLower();
    ui->compteRenduMaj->clear();
    ui->compteRenduMaj->setVisible(true);

    /* Corps de la methode */
    // Telechargement des fichiers
    QFile fi(dirDat + QDir::separator() + "gestionnaireTLE.gst");
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    ui->frameBarreProgression->setVisible(true);
    while (!flux.atEnd()) {
        const QStringList ligne = flux.readLine().split("#");
        if (ligne.at(0) == groupeTLE) {

            QString adresse = ligne.at(0).split("@").at(1);
            if (adresse.contains("celestrak"))
                adresse = "http://www.celestrak.com/NORAD/elements/";
            if (!adresse.startsWith("http://"))
                adresse.insert(0, "http://");
            if (!adresse.endsWith("/"))
                adresse.append("/");

            const QStringList listeTLE = ligne.at(2).split(",");
            foreach(QString file, listeTLE)
                AjoutFichier(QUrl(adresse + file));

            if (downQueue.isEmpty())
                QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_parcourirMaj1_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierMaj", dirTle).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::convertSeparators(fichier);
        ui->fichierAMettreAJour->setText(fichier);
    }

    /* Retour */
    return;
}

void PreviSat::on_parcourirMaj2_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierALire", dirTle).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::convertSeparators(fichier);
        ui->fichierALire->setText(fichier);
    }

    /* Retour */
    return;
}

void PreviSat::on_mettreAJourTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    messagesStatut->setText("");
    ui->compteRenduMaj->setVisible(false);

    try {
        bool agz = false;
        if (ui->fichierAMettreAJour->text().isEmpty())
            throw PreviSatException(tr("MISE A JOUR : Le nom du fichier à mettre à jour n'est pas spécifié"), WARNING);

        if (ui->fichierALire->text().isEmpty())
            throw PreviSatException(tr("MISE A JOUR : Le nom du fichier à lire n'est pas spécifié"), WARNING);

        // Fichier a lire au format gz
        QFileInfo fi(ui->fichierALire->text());
        QString fic;
        if (fi.suffix() == "gz") {

            // Cas d'un fichier compresse au format gz
            fic = dirTmp + QDir::separator() + fi.completeBaseName();

            if (DecompressionFichierGz(ui->fichierALire->text(), fic)) {

                const int nsat = TLE::VerifieFichier(fic, false);
                if (nsat == 0)
                    throw PreviSatException(tr("MISE A JOUR : Erreur rencontrée lors de la décompression du fichier") + " " +
                                            ui->fichierALire->text(), WARNING);
                agz = true;
            } else {
                throw PreviSatException(tr("MISE A JOUR : Erreur rencontrée lors de la décompression du fichier") + " " +
                                        ui->fichierALire->text(), WARNING);
            }
        } else {
            fic = QDir::convertSeparators(fi.absoluteFilePath());
        }

        QStringList listeFic;
        listeFic << ui->fichierAMettreAJour->text() << fic;
        foreach(QString file, listeFic) {
            fi = QFileInfo(file);
            if (!fi.exists()) {
                const QString msg = tr("MISE A JOUR : Le fichier %1 n'existe pas");
                throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
            }
        }

        QStringList compteRendu;
        TLE::MiseAJourFichier(ui->fichierAMettreAJour->text(), fic, compteRendu);

        bool aecr = false;
        EcritureCompteRenduMaj(compteRendu, aecr);

        if (agz) {
            QFile fich(fic);
            fich.remove();
        }

        messagesStatut->setText(tr("Terminé !"));
        ui->compteRenduMaj->setVisible(true);

        if (nomfic == ui->fichierAMettreAJour->text().trimmed() && aecr) {

            // Recuperation des TLE de la liste
            TLE::LectureFichier(nomfic, liste, tles);

            l1 = tles.at(0).getLigne1();
            l2 = tles.at(0).getLigne2();

            if (nbSat > 0) {

                AfficherListeSatellites(nomfic, liste);

                Satellite::initCalcul = false;

                // Lecture des donnees satellite
                Satellite::LectureDonnees(liste, tles, satellites);

                CalculsAffichage();
            }
        }
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_gestionnaireMajTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    gestionnaire = new GestionnaireTLE;
    gestionnaire->setWindowModality(Qt::ApplicationModal);
    gestionnaire->show();

    /* Retour */
    return;
}

void PreviSat::on_compteRenduMaj_customContextMenuRequested(const QPoint &pos)
{
    ui->menuContextuelCompteRenduMaj->exec(QCursor::pos());
}

void PreviSat::on_actionCopier_dans_le_presse_papier_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->compteRenduMaj->toPlainText());

    /* Retour */
    return;
}


/*
 * Extraction d'un fichier TLE
 */
void PreviSat::on_numeroNORADCreerTLE_currentIndexChanged(int index)
{
    ui->frameNORAD->setVisible(index != 0);
}

void PreviSat::on_ADNoeudAscendantCreerTLE_currentIndexChanged(int index)
{
    ui->frameADNA->setVisible(index != 0);
}

void PreviSat::on_excentriciteCreerTLE_currentIndexChanged(int index)
{
    ui->frameExcentricite->setVisible(index != 0);
}

void PreviSat::on_inclinaisonCreerTLE_currentIndexChanged(int index)
{
    ui->frameIncl->setVisible(index != 0);
}

void PreviSat::on_argumentPerigeeCreerTLE_currentIndexChanged(int index)
{
    ui->frameArgumentPerigee->setVisible(index != 0);
}

void PreviSat::on_parcourir1CreerTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierALireCreerTLE", dirTle).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::convertSeparators(fichier);
        ui->fichierALireCreerTLE->setText(fichier);
    }

    /* Retour */
    return;
}

void PreviSat::on_parcourir2CreerTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."),
                                                   settings.value("fichier/sauvegarde", dirOut).toString(),
                                                   tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::convertSeparators(fichier);
        ui->nomFichierPerso->setText(fichier);

        const QFileInfo fi(fichier);
        settings.setValue("fichier/sauvegarde", fi.absolutePath());
    }

    /* Retour */
    return;
}

void PreviSat::on_rechercheCreerTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    messagesStatut->setText("");

    /* Corps de la methode */
    try {
        if (ui->fichierALireCreerTLE->text().isEmpty())
            throw PreviSatException(tr("PERSONNEL : Le nom du fichier à lire n'est pas spécifié"), WARNING);

        if (ui->nomFichierPerso->text().isEmpty())
            throw PreviSatException(tr("PERSONNEL : Le nom du fichier personnel n'est pas spécifié"), WARNING);

        // Fichier a lire
        QString ficlu = ui->fichierALireCreerTLE->text();

        // Verification du fichier
        QFileInfo fi(ficlu);
        if (fi.exists()) {
            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = dirTmp + QDir::separator() + fi.completeBaseName();

                if (DecompressionFichierGz(ficlu, fic)) {

                    const int nsat = TLE::VerifieFichier(fic, false);
                    if (nsat == 0)
                        throw PreviSatException(tr("PERSONNEL : Erreur rencontrée lors de la décompression du fichier") + " " +
                                                ficlu, WARNING);
                    ficlu = fic;
                } else {
                    throw PreviSatException(tr("PERSONNEL : Erreur rencontrée lors de la décompression du fichier") + " " +
                                            ficlu, WARNING);
                }
            }
        } else {
            const QString msg = tr("PERSONNEL : Le fichier %1 n'existe pas");
            throw PreviSatException(msg.arg(ficlu), WARNING);
        }

        fi = QFileInfo(ui->nomFichierPerso->text());
        QDir di(fi.absolutePath());
        if (!di.exists())
            di.mkpath(fi.absolutePath());

        ui->rechercheCreerTLE->setEnabled(false);

        // Recuperation des valeurs de l'interface graphique
        // Numero NORAD
        const int nrdmin = (ui->numeroNORADCreerTLE->currentIndex() == 0) ? 1 : qMin(ui->noradMin->value(), ui->noradMax->value());
        const int nrdmax = (ui->numeroNORADCreerTLE->currentIndex() == 0) ? 99999 : qMax(ui->noradMin->value(), ui->noradMax->value());

        // Ascension droite du noeud ascendant
        const double ascmin = (ui->ADNoeudAscendantCreerTLE->currentIndex() == 0) ? 0. : qMin(ui->ADNAMin->value(), ui->ADNAMax->value());
        const double ascmax = (ui->ADNoeudAscendantCreerTLE->currentIndex() == 0) ? T360 : qMax(ui->ADNAMin->value(), ui->ADNAMax->value());

        // Excentricite
        double exmin = 0.;
        double exmax = 1.;
        if (ui->excentriciteCreerTLE->currentIndex() == 1) {
            exmin = qMax(ui->excMin->text().toDouble(), 0.);
            exmax = qMin(ui->excMax->text().toDouble(), 1.);
            exmin = qMin(exmin, exmax);
            exmax = qMax(exmin, exmax);
        }

        // Inclinaison
        double imin1 = qMin(ui->inclMin1->value(), ui->inclMax1->value());
        double imax1 = qMax(ui->inclMin1->value(), ui->inclMax1->value());
        double imin2 = 0.;
        double imax2 = 180.;
        if (ui->inclinaisonCreerTLE->currentIndex() == 1) {
            imin2 = qMin(ui->inclMin2->value(), ui->inclMax2->value());
            imax2 = qMax(ui->inclMin2->value(), ui->inclMax2->value());
        }

        // Nombre de revolutions par jour
        double nbrevmin = qMax(ui->revMin->text().toDouble(), 0.);
        double nbrevmax = qMin(ui->revMax->text().toDouble(), 18.);
        nbrevmin = qMin(nbrevmin, nbrevmax);
        nbrevmax = qMax(nbrevmin, nbrevmax);

        // Argument du perigee
        const double argmin = (ui->argumentPerigeeCreerTLE->currentIndex() == 0) ? 0. : qMin(ui->argMin->value(), ui->argMax->value());
        const double argmax = (ui->argumentPerigeeCreerTLE->currentIndex() == 0) ? T360 : qMax(ui->argMin->value(), ui->argMax->value());

        // Magnitude maximale
        const double mgmax = qMin((double) ui->magnitudeMaxCreerTLE->value(), 99.);


        // Verification du fichier TLE
        const int nbs = TLE::VerifieFichier(ficlu, false);
        if (nbs == 0) {
            const QString msg = tr("PERSONNEL : Erreur rencontrée lors du chargement du fichier %1");
            throw PreviSatException(msg.arg(ficlu), WARNING);
        }

        // Lecture du fichier TLE
        QStringList listeSat;
        QList<Satellite> sats;
        QVector<TLE> tabtle;
        TLE::LectureFichier(ficlu, listeSat, tabtle);

        QVectorIterator<TLE> it(tabtle);
        while (it.hasNext()) {
            const TLE tle = it.next();
            listeSat.append(tle.getNorad());
            sats.append(Satellite(tle));
        }

        // Lecture du fichier de donnees satellite
        Satellite::LectureDonnees(listeSat, tabtle, sats);

        QFile sw(ui->nomFichierPerso->text());
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!sw.isWritable()) {
            const QString msg = tr("PERSONNEL : Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        for(int isat=0; isat<nbs; isat++) {

            // Numero NORAD
            const int norad = tabtle.at(isat).getNorad().toInt();
            bool aecr = (norad >= nrdmin && norad <= nrdmax);

            // Ascension droite du noeud ascendant
            aecr = aecr && (tabtle.at(isat).getOmegao() >= ascmin && tabtle.at(isat).getOmegao() <= ascmax);

            // Excentricite
            aecr = aecr && (tabtle.at(isat).getEcco() >= exmin && tabtle.at(isat).getEcco() <= exmax);

            // Inclinaison
            aecr = aecr && ((tabtle.at(isat).getInclo() >= imin1 && tabtle.at(isat).getInclo() <= imax1) ||
                            (tabtle.at(isat).getInclo() >= imin2 && tabtle.at(isat).getInclo() <= imax2 &&
                             ui->inclinaisonCreerTLE->currentIndex() == 1));

            // Moyen mouvement
            aecr = aecr && (tabtle.at(isat).getNo() >= nbrevmin && tabtle.at(isat).getNo() <= nbrevmax);

            // Argument du perigee
            aecr = aecr && (tabtle.at(isat).getArgpo() >= argmin && tabtle.at(isat).getArgpo() <= argmax);

            // Magnitude
            double mag;
            if (sats.at(isat).getMagnitudeStandard() < 98.9) {
                const double ax = RAYON_TERRESTRE * qPow(KE / (tabtle.at(isat).getNo() * DEUX_PI *
                                                               NB_JOUR_PAR_MIN), DEUX_TIERS);
                mag = sats.at(isat).getMagnitudeStandard() - 15.75 + 5. *
                        log10(1.45 * (ax * 1. - tabtle.at(isat).getEcco()));
            } else {
                mag = -10.;
            }
            aecr = aecr && (mag <= mgmax) ;

            // Ecriture du TLE
            if (aecr) {
                if (tabtle.at(isat).getNom() != tabtle.at(isat).getNorad())
                    flux << tabtle.at(isat).getNom() << endl;
                flux << tabtle.at(isat).getLigne1() << endl;
                flux << tabtle.at(isat).getLigne2() << endl;
            }
        }
        sw.close();
        if (sw.size() > 0) {
            const QString msg = tr("Fichier %1 écrit");
            messagesStatut->setText(msg.arg(sw.fileName()));
        }
        sats.clear();
        listeSat.clear();
        tabtle.clear();

    } catch (PreviSatException &e) {
    }
    ui->rechercheCreerTLE->setEnabled(true);

    /* Retour */
    return;
}


/*
 * Calcul des previsions de passage
 */
void PreviSat::on_effacerHeuresPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->dateInitialePrev->setTime(QTime(0, 0, 0));
    ui->dateFinalePrev->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void PreviSat::on_liste2_customContextMenuRequested(const QPoint &pos)
{
    if (ui->liste2->currentRow() >= 0)
        ui->menuContextuelListes->exec(QCursor::pos());
}

void PreviSat::on_liste2_entered(const QModelIndex &index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->liste2->setToolTip("");

    /* Corps de la methode */
    const int r = index.row();
    const QStringList sat = ui->liste2->item(r)->text().split("#");
    const QString nomsat = sat.at(0).trimmed();
    const QString norad = sat.at(1);
    if (nomsat != norad) {
        const QString msg = tr("%1 (numéro NORAD : %2)");
        ui->liste2->setToolTip(msg.arg(nomsat).arg(norad));
    }

    /* Retour */
    return;
}

void PreviSat::on_actionTous_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste2->hasFocus()) {
        for(int i=0; i<ui->liste2->count(); i++)
            ui->liste2->item(i)->setCheckState(Qt::Checked);
    }

    if (ui->liste3->hasFocus()) {
        for(int i=0; i<ui->liste3->count(); i++)
            ui->liste3->item(i)->setCheckState(Qt::Checked);
    }

    /* Retour */
    return;
}

void PreviSat::on_actionAucun_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste2->hasFocus()) {
        for(int i=0; i<ui->liste2->count(); i++)
            ui->liste2->item(i)->setCheckState(Qt::Unchecked);
    }

    if (ui->liste3->hasFocus()) {
        for(int i=0; i<ui->liste3->count(); i++)
            ui->liste3->item(i)->setCheckState(Qt::Unchecked);
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSatPrev_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSatPrev->count() - 1) {
        ui->valHauteurSatPrev->setVisible(true);
        ui->valHauteurSatPrev->setCursorPosition(0);
        ui->valHauteurSatPrev->setFocus();
    } else {
        ui->valHauteurSatPrev->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSoleilPrev_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSoleilPrev->count() - 1) {
        ui->valHauteurSoleilPrev->setVisible(true);
        ui->valHauteurSoleilPrev->setCursorPosition(0);
        ui->valHauteurSoleilPrev->setFocus();
    } else {
        ui->valHauteurSoleilPrev->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_magnitudeMaxPrev_toggled(bool checked)
{
    ui->valMagnitudeMaxPrev->setVisible(checked);
}

void PreviSat::on_calculsPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL)
        afficherResultats->close();
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherIri->setVisible(false);
    ui->afficherEvt->setVisible(false);
    ui->afficherTransit->setVisible(false);

    try {
        const int nsat = getListeItemChecked(ui->liste2);
        if (nsat == 0)
            throw PreviSatException(tr("PREVISION : Aucun satellite n'est sélectionné dans la liste"), WARNING);

        ui->afficherPrev->setVisible(false);

        // Ecart heure locale - UTC
        const double dtu = dateCourante.getOffsetUTC();

        // Date et heure initiales
        const Date date1(ui->dateInitialePrev->date().year(), ui->dateInitialePrev->date().month(), ui->dateInitialePrev->date().day(),
                         ui->dateInitialePrev->time().hour(), ui->dateInitialePrev->time().minute(), ui->dateInitialePrev->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien initial
        double jj1 = date1.getJourJulien() - dtu;

        // Date et heure finales
        const Date date2(ui->dateFinalePrev->date().year(), ui->dateFinalePrev->date().month(), ui->dateFinalePrev->date().day(),
                         ui->dateFinalePrev->time().hour(), ui->dateFinalePrev->time().minute(), ui->dateFinalePrev->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien final
        double jj2 = date2.getJourJulien() - dtu;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (jj1 > jj2) {
            const double tmp = jj2;
            jj2 = jj1;
            jj1 = tmp;
        }

        // Pas de generation
        int pas0 = 0;
        switch (ui->pasGeneration->currentIndex()) {
        case 0:
            pas0 = 1;
            break;
        case 1:
            pas0 = 5;
            break;
        case 2:
            pas0 = 10;
            break;
        case 3:
            pas0 = 20;
            break;
        case 4:
            pas0 = 30;
            break;
        case 5:
            pas0 = 60;
            break;
        case 6:
            pas0 = 120;
            break;
        case 7:
            pas0 = 300;
        default:
            break;
        }

        // Conditions d'eclairement du satellite
        const bool ecl = ui->illuminationPrev->isChecked();

        // Magnitude maximale
        const double mag = (ui->magnitudeMaxPrev->isChecked()) ? ui->valMagnitudeMaxPrev->value() : 99.;

        // Hauteur minimale du satellite
        const int haut = (ui->hauteurSatPrev->currentIndex() == 5) ?
                    fabs(ui->valHauteurSatPrev->text().toInt()) : 5 * ui->hauteurSatPrev->currentIndex();

        // Hauteur maximale du Soleil
        int crep = 0;
        if (ui->hauteurSoleilPrev->currentIndex() <= 3) {
            crep = -6 * ui->hauteurSoleilPrev->currentIndex();
        } else if (ui->hauteurSoleilPrev->currentIndex() == 4) {
            crep = 90;
        } else if (ui->hauteurSoleilPrev->currentIndex() == 5) {
            crep = ui->valHauteurSoleilPrev->text().toInt();
        }

        // Prise en compte de l'extinction atmospherique
        const bool ext = ui->extinctionAtmospherique->isChecked();

        // Liste des numeros NORAD
        QStringList listeSat;
        for (int i=0; i<ui->liste2->count(); i++) {
            if (ui->liste2->item(i)->checkState() == Qt::Checked)
                listeSat.append(ui->liste2->item(i)->text().split("#").at(1));
        }

        // Nom du fichier resultat
        const QString out = dirTmp + QDir::separator() + "prevision.txt";
        QFile fi(out);
        if (fi.exists())
            fi.remove();
        QDir di(dirTmp);
        if (!di.exists())
            di.mkpath(dirTmp);

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("mi");

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsPrev->setVisible(false);
        ui->annulerPrev->setVisible(true);
        ui->annulerPrev->setFocus();


        // Lancement des calculs
        const Conditions conditions(ecl, ext, crep, haut, pas0, dtu, jj1, jj2, mag, nomfic, out, unite, listeSat);
        const Observateur obser(observateurs.at(ui->lieuxObservation2->currentIndex()));

        threadCalculs = new ThreadCalculs(ThreadCalculs::PREVISION, conditions, obser);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_annulerPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Trouver methode alternative a terminate
    threadCalculs->terminate();
    threadCalculs->wait();

    ui->annulerPrev->setVisible(false);
    ui->afficherPrev->setVisible(false);
    messagesStatut->setText(tr("Annulation du calcul des prévisions de passage"));
    QFile fi(dirTmp + QDir::separator() + "prevision.txt");
    if (fi.exists())
        fi.remove();

    /* Retour */
    return;
}

void PreviSat::on_afficherPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    afficherResultats = new Afficher;
    afficherResultats->setWindowTitle(tr("Prévisions de passage des satellites"));
    afficherResultats->show(dirTmp + QDir::separator() + "prevision.txt");

    /* Retour */
    return;
}


/*
 * Calcul des flashs Iridium
 */
void PreviSat::on_effacerHeuresIri_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->dateInitialeIri->setTime(QTime(0, 0, 0));
    ui->dateFinaleIri->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void PreviSat::on_parcourirIri_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), settings.value("fichier/iridium", dirTle).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::convertSeparators(fichier);
        ui->fichierTLEIri->setText(fichier);
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSatIri_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSatIri->count() - 1) {
        ui->valHauteurSatIri->setVisible(true);
        ui->valHauteurSatIri->setCursorPosition(0);
        ui->valHauteurSatIri->setFocus();
    } else {
        ui->valHauteurSatIri->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSoleilIri_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSoleilIri->count() - 1) {
        ui->valHauteurSoleilIri->setVisible(true);
        ui->valHauteurSoleilIri->setCursorPosition(0);
        ui->valHauteurSoleilIri->setFocus();
    } else {
        ui->valHauteurSoleilIri->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsIri_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL)
        afficherResultats->close();
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherPrev->setVisible(false);
    ui->afficherEvt->setVisible(false);
    ui->afficherTransit->setVisible(false);

    try {
        if (ui->fichierTLEIri->text().isEmpty())
            throw PreviSatException(tr("IRIDIUM : Le nom du fichier TLE n'est pas spécifié"), WARNING);

        const QFileInfo fi(ui->fichierTLEIri->text());
        if (!fi.exists()) {
            ui->fichierTLEIri->setText("");
            throw PreviSatException(tr("IRIDIUM : Le nom du fichier TLE est incorrect"), WARNING);
        }
        ui->afficherIri->setVisible(false);

        // Ecart heure locale - UTC
        const double dtu = dateCourante.getOffsetUTC();

        // Date et heure initiales
        const Date date1(ui->dateInitialeIri->date().year(), ui->dateInitialeIri->date().month(), ui->dateInitialeIri->date().day(),
                         ui->dateInitialeIri->time().hour(), ui->dateInitialeIri->time().minute(), ui->dateInitialeIri->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien initial
        double jj1 = date1.getJourJulien() - dtu;

        // Date et heure finales
        const Date date2(ui->dateFinaleIri->date().year(), ui->dateFinaleIri->date().month(), ui->dateFinaleIri->date().day(),
                         ui->dateFinaleIri->time().hour(), ui->dateFinaleIri->time().minute(), ui->dateFinaleIri->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien final
        double jj2 = date2.getJourJulien() - dtu;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (jj1 > jj2) {
            const double tmp = jj2;
            jj2 = jj1;
            jj1 = tmp;
        }

        // Hauteur minimale du satellite
        const int haut = (ui->hauteurSatIri->currentIndex() == 5) ?
                    fabs(ui->valHauteurSatIri->text().toInt()) : 5 * ui->hauteurSatIri->currentIndex();

        // Hauteur maximale du Soleil
        int crep = 0;
        if (ui->hauteurSoleilIri->currentIndex() <= 3) {
            crep = -6 * ui->hauteurSoleilIri->currentIndex();
        } else if (ui->hauteurSoleilIri->currentIndex() == 4) {
            crep = 90;
        } else if (ui->hauteurSoleilIri->currentIndex() == 5) {
            crep = ui->valHauteurSoleilIri->text().toInt();
        }

        // Prise en compte des satellites operationnels uniquement
        const char ope = (ui->satellitesOperationnels->isChecked()) ? 'o' : 'n';

        // Choix du tri par ordre chronologique
        const char chr = (ui->ordreChronologique->isChecked()) ? 'o' : 'n';

        // Choix du nombre de lignes par flash
        const int nbl = (ui->affichage3lignesIri->isChecked()) ? 3 : 1;

        // Angle maximal de reflexion
        const double ang0 = ui->angleMaxReflexionIri->value();

        // Magnitude maximale (nuit)
        const double mgn1 = ui->magnitudeMaxNuitIri->value();

        // Magnitude maximale (nuit)
        const double mgn2 = ui->magnitudeMaxJourIri->value();

        // Prise en compte de l'extinction atmospherique
        const bool ext = ui->extinctionAtmospherique->isChecked();

        // Nom du fichier resultat
        const QString out = dirTmp + QDir::separator() + "prevision.txt";
        QFile fi2(out);
        if (fi2.exists())
            fi2.remove();
        QDir di(dirTmp);
        if (!di.exists())
            di.mkpath(dirTmp);

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("mi");

        // Lecture du fichier de statut des satellites Iridium
        QStringList tabStsIri;
        const int nb = Iridium::LectureStatutIridium(ope, tabStsIri);
        if (nb == 0)
            throw PreviSatException(QObject::tr("IRIDIUM : Erreur rencontrée lors de l'exécution\nAucun satellite Iridium susceptible de produire des flashs dans le fichier de statut"), WARNING);

        // Creation de la liste de satellites
        QStringList listeSatellites;
        QStringListIterator it1(tabStsIri);
        while (it1.hasNext()) {
            listeSatellites.append(it1.next().mid(4, 5));
        }

        // Verification du fichier TLE
        if (TLE::VerifieFichier(fi.absoluteFilePath(), false) == 0) {
            const QString msg = QObject::tr("IRIDIUM : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE");
            throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
        }

        QVector<TLE> tabtle;

        // Lecture du fichier TLE
        TLE::LectureFichier(fi.absoluteFilePath(), listeSatellites, tabtle);

        // Mise a jour de la liste de satellites et creation du tableau de satellites
        int i = 0;
        listeSatellites.clear();
        QVectorIterator<TLE> it2(tabtle);
        QVector<TLE> tabtle2;
        while (it2.hasNext()) {
            const TLE tle = it2.next();
            if (tle.getNorad().isEmpty()) {
                tabStsIri.removeAt(i);
            } else {
                listeSatellites.append(tle.getNorad());
                tabtle2.append(tle);
                i++;
            }
        }

        // Il n'y a aucun satellite Iridium dans le fichier TLE
        if (listeSatellites.size() == 0)
            throw PreviSatException(QObject::tr("IRIDIUM : Erreur rencontrée lors de l'exécution\nAucun satellite Iridium n'a été trouvé dans le fichier TLE"), WARNING);

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsIri->setVisible(false);
        ui->annulerIri->setVisible(true);
        ui->annulerIri->setFocus();


        // Lancement des calculs
        const Conditions conditions(ext, crep, haut, nbl, chr, ang0, dtu, jj1, jj2, mgn1, mgn2, fi.absoluteFilePath(), out,
                                    unite, tabStsIri, tabtle2);
        Observateur obser(observateurs.at(ui->lieuxObservation3->currentIndex()));

        threadCalculs = new ThreadCalculs(ThreadCalculs::IRIDIUM, conditions, obser);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_annulerIri_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Trouver methode alternative a terminate
    threadCalculs->terminate();
    threadCalculs->wait();

    ui->annulerIri->setVisible(false);
    ui->afficherIri->setVisible(false);
    messagesStatut->setText(tr("Annulation du calcul des flashs Iridium"));
    QFile fi(dirTmp + QDir::separator() + "prevision.txt");
    if (fi.exists())
        fi.remove();

    /* Retour */
    return;
}

void PreviSat::on_afficherIri_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    afficherResultats = new Afficher;
    afficherResultats->setWindowTitle(tr("Prévisions des flashs Iridium"));
    afficherResultats->show(dirTmp + QDir::separator() + "prevision.txt");
    afficherResultats->setGeometry(afficherResultats->x(), afficherResultats->y(), 1210, afficherResultats->height());

    /* Retour */
    return;
}


/*
 * Calcul des evenements orbitaux
 */
void PreviSat::on_effacerHeuresEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->dateInitialeEvt->setTime(QTime(0, 0, 0));
    ui->dateFinaleEvt->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void PreviSat::on_liste3_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste3->currentRow() >= 0)
        ui->menuContextuelListes->exec(QCursor::pos());

    /* Retour */
    return;
}

void PreviSat::on_liste3_entered(const QModelIndex &index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->liste3->setToolTip("");

    /* Corps de la methode */
    const int r = index.row();
    const QStringList sat = ui->liste3->item(r)->text().split("#");
    const QString nomsat = sat.at(0).trimmed();
    const QString norad = sat.at(1);
    if (nomsat != norad) {
        const QString msg = tr("%1 (numéro NORAD : %2)");
        ui->liste3->setToolTip(msg.arg(nomsat).arg(norad));
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL)
        afficherResultats->close();
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherPrev->setVisible(false);
    ui->afficherIri->setVisible(false);
    ui->afficherTransit->setVisible(false);

    try {
        const int nsat = getListeItemChecked(ui->liste3);
        if (nsat == 0)
            throw PreviSatException(tr("EVENEMENTS : Aucun satellite n'est sélectionné dans la liste"), WARNING);

        ui->afficherEvt->setVisible(false);

        // Ecart heure locale - UTC
        const double dtu = dateCourante.getOffsetUTC();

        // Date et heure initiales
        const Date date1(ui->dateInitialeEvt->date().year(), ui->dateInitialeEvt->date().month(), ui->dateInitialeEvt->date().day(),
                         ui->dateInitialeEvt->time().hour(), ui->dateInitialeEvt->time().minute(), ui->dateInitialeEvt->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien initial
        double jj1 = date1.getJourJulien() - dtu;

        // Date et heure finales
        const Date date2(ui->dateFinaleEvt->date().year(), ui->dateFinaleEvt->date().month(), ui->dateFinaleEvt->date().day(),
                         ui->dateFinaleEvt->time().hour(), ui->dateFinaleEvt->time().minute(), ui->dateFinaleEvt->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien final
        double jj2 = date2.getJourJulien() - dtu;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (jj1 > jj2) {
            const double tmp = jj2;
            jj2 = jj1;
            jj1 = tmp;
        }

        // Passages aux noeuds
        const bool noeuds = ui->passageNoeuds->isChecked();

        // Passages ombre/penombre/lumiere
        const bool ombre = ui->passageOmbre->isChecked();

        // Passages apogee/perigee
        const bool apogee = ui->passageApogee->isChecked();

        // Transitions jour/nuit
        const bool jourNuit = ui->transitionJourNuit->isChecked();

        // Passages aux quadrangles
        const bool quadr = ui->passageQuadrangles->isChecked();

        const bool eve = noeuds || ombre || apogee || jourNuit || quadr;
        if (!eve)
            throw PreviSatException(tr("EVENEMENTS : Aucun évènement sélectionné"), WARNING);

        // Liste des numeros NORAD
        QStringList listeSat;
        for (int i=0; i<ui->liste3->count(); i++) {
            if (ui->liste3->item(i)->checkState() == Qt::Checked)
                listeSat.append(ui->liste3->item(i)->text().split("#").at(1));
        }

        // Nom du fichier resultat
        const QString out = dirTmp + QDir::separator() + "prevision.txt";
        QFile fi2(out);
        if (fi2.exists())
            fi2.remove();
        QDir di(dirTmp);
        if (!di.exists())
            di.mkpath(dirTmp);

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("mi");

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsEvt->setVisible(false);
        ui->annulerEvt->setVisible(true);
        ui->annulerEvt->setFocus();


        // Lancement des calculs
        const Conditions conditions(apogee, noeuds, ombre, quadr, jourNuit, dtu, jj1, jj2, nomfic, out, unite, listeSat);

        threadCalculs = new ThreadCalculs(ThreadCalculs::EVENEMENTS, conditions);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_annulerEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Trouver methode alternative a terminate
    threadCalculs->terminate();
    threadCalculs->wait();

    ui->annulerEvt->setVisible(false);
    ui->afficherEvt->setVisible(false);
    messagesStatut->setText(tr("Annulation du calcul des évènements orbitaux"));
    QFile fi(dirTmp + QDir::separator() + "prevision.txt");
    if (fi.exists())
        fi.remove();

    /* Retour */
    return;
}

void PreviSat::on_afficherEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    afficherResultats = new Afficher;
    afficherResultats->setWindowTitle(tr("Évènements orbitaux"));
    afficherResultats->show(dirTmp + QDir::separator() + "prevision.txt");

    /* Retour */
    return;
}


/*
 * Calcul des transits ISS
 */
void PreviSat::on_effacerHeuresTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->dateInitialeTransit->setTime(QTime(0, 0, 0));
    ui->dateFinaleTransit->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void PreviSat::on_parcourirTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), settings.value("fichier/fichierTLETransit", dirTle).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::convertSeparators(fichier);
        ui->fichierTLETransit->setText(fichier);
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSatTransit_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSatTransit->count() - 1) {
        ui->valHauteurSatTransit->setVisible(true);
        ui->valHauteurSatTransit->setCursorPosition(0);
        ui->valHauteurSatTransit->setFocus();
    } else {
        ui->valHauteurSatTransit->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL)
        afficherResultats->close();
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherPrev->setVisible(false);
    ui->afficherIri->setVisible(false);
    ui->afficherEvt->setVisible(false);

    try {
        if (ui->fichierTLETransit->text().isEmpty())
            throw PreviSatException(tr("TRANSIT : Le nom du fichier TLE n'est pas spécifié"), WARNING);

        const QFileInfo fi(ui->fichierTLETransit->text());
        if (!fi.exists()) {
            ui->fichierTLETransit->setText("");
            throw PreviSatException(tr("TRANSIT : Le nom du fichier TLE est incorrect"), WARNING);
        }

        ui->afficherTransit->setVisible(false);

        // Ecart heure locale - UTC
        const double dtu = dateCourante.getOffsetUTC();

        // Date et heure initiales
        const Date date1(ui->dateInitialeTransit->date().year(), ui->dateInitialeTransit->date().month(), ui->dateInitialeTransit->date().day(),
                         ui->dateInitialeTransit->time().hour(), ui->dateInitialeTransit->time().minute(), ui->dateInitialeTransit->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien initial
        double jj1 = date1.getJourJulien() - dtu;

        // Date et heure finales
        const Date date2(ui->dateFinaleTransit->date().year(), ui->dateFinaleTransit->date().month(), ui->dateFinaleTransit->date().day(),
                         ui->dateFinaleTransit->time().hour(), ui->dateFinaleTransit->time().minute(), ui->dateFinaleTransit->time().second(),
                         dateCourante.getOffsetUTC());

        // Jour julien final
        double jj2 = date2.getJourJulien() - dtu;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (jj1 > jj2) {
            const double tmp = jj2;
            jj2 = jj1;
            jj1 = tmp;
        }

        // Age maximal du TLE
        const double ageTLE = ui->ageMaxTLETransit->value();

        // Hauteur minimale du satellite
        const int haut = (ui->hauteurSatTransit->currentIndex() == 5) ?
                    fabs(ui->valHauteurSatTransit->text().toInt()) : 5 * ui->hauteurSatTransit->currentIndex();

        // Elongation maximale
        const double elong = ui->elongationMaxCorps->value();

        // Selection des corps
        const bool calcSol = ui->soleilTransit->isChecked();
        const bool calcLune = ui->luneTransit->isChecked();

        // Nom du fichier resultat
        const QString out = dirTmp + QDir::separator() + "prevision.txt";
        QFile fi2(out);
        if (fi2.exists())
            fi2.remove();
        QDir di(dirTmp);
        if (!di.exists())
            di.mkpath(dirTmp);

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("mi");

        const QStringList listeTLE("25544");
        QVector<TLE> tabtle;

        // Verification du fichier TLE
        if (TLE::VerifieFichier(fi.absoluteFilePath(), false) == 0) {
            const QString msg = QObject::tr("TRANSIT : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE");
            throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
        }

        // Lecture du TLE
        TLE::LectureFichier(fi.absoluteFilePath(), listeTLE, tabtle);
        if (tabtle.at(0).getNorad().isEmpty()) {
            const QString msg = QObject::tr("TRANSIT : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 ne contient pas le TLE de l'ISS");
            throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
        }

        // Age du TLE
        const double age = fabs(jj1 - tabtle.at(0).getEpoque().getJourJulienUTC());
        if (age > ageTLE + 0.05) {
            const QString msg = QObject::tr("TRANSIT : L'âge du TLE de l'ISS (%1 jours) est supérieur à %2 jours");
            Messages::Afficher(msg.arg(age, 0, 'f', 1).arg(ageTLE, 0, 'f', 1), INFO);
        }

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsTransit->setVisible(false);
        ui->annulerTransit->setVisible(true);
        ui->annulerTransit->setFocus();

        // Lancement des calculs
        const Conditions conditions(calcLune, calcSol, haut, ageTLE, elong, dtu, jj1, jj2, fi.absoluteFilePath(), out, unite);
        const Observateur obser(observateurs.at(ui->lieuxObservation4->currentIndex()));

        threadCalculs = new ThreadCalculs(ThreadCalculs::TRANSITS, conditions, obser);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_annulerTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Trouver methode alternative a terminate
    threadCalculs->terminate();
    threadCalculs->wait();

    ui->annulerTransit->setVisible(false);
    ui->afficherTransit->setVisible(false);
    messagesStatut->setText(tr("Annulation du calcul des transits ISS"));
    QFile fi(dirTmp + QDir::separator() + "prevision.txt");
    if (fi.exists())
        fi.remove();

    /* Retour */
    return;
}

void PreviSat::on_afficherTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    afficherResultats = new Afficher;
    afficherResultats->setWindowTitle(tr("Transits ISS"));
    afficherResultats->show(dirTmp + QDir::separator() + "prevision.txt");
    afficherResultats->setGeometry(afficherResultats->x(), afficherResultats->y(), 1210, afficherResultats->height());

    /* Retour */
    return;
}
