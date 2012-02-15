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
 * >
 *
 */
//#include <QMessageBox>
#include <QtCore/qmath.h>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QGraphicsTextItem>
#include <QSettings>
#include <QSound>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/messages.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "zlib/zlib.h"
#include "previsat.h"
#include "ui_previsat.h"

// Repertoires
static QString dirExe;
static QString dirDat;
static QString dirCoo;
static QString dirMap;
static QString dirOut;
static QString dirTle;

// TLE par defaut
static QString nom;
static QString l1;
static QString l2;

// Liste de satellites
static bool htr;
static bool info;
static bool notif;
static bool old;
static int nbSat;
static QString nomfic;
static QStringList liste;
static QVector<bool> bipSat;
static QVector<TLE> tles;
static QList<Satellite> satellites;
static QStringList mapSatellites;
static QStringList listeFicTLE;

// Date courante
static Date dateCourante;
static double offsetUTC;
static QDateTime tim;

// Lieux d'observation
static QList<Observateur> observateurs;
static QStringList ficObs;
static QStringList listeObs;

// Cartes du monde
static QStringList ficMap;
static double DEG2PXHZ;
static double DEG2PXVT;

// Soleil, Lune
static Soleil soleil;
static Lune lune;

// Registre
static QSettings settings("Astropedia", "previsat");

static QTimer *chronometre;

QLabel *messagesStatut;
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
 * Initialisations generales
 */
void PreviSat::Initialisations()
{
    /* Declarations des variables locales */
    QDir di;

    /* Initialisations */
    info = true;
    old = false;
    dirExe = QCoreApplication::applicationDirPath();
    dirDat = dirExe + QDir::separator() + "data";
    dirCoo = dirDat + QDir::separator() + "coordonnees";
    dirMap = dirDat + QDir::separator() + "map";
    dirOut = dirDat + QDir::separator() + "out";
    dirTle = dirExe + QDir::separator() + "tle";
    DEG2PXHZ = 1. / 0.45;
    DEG2PXVT = 1. / 0.45;
    chronometre = new QTimer(this);

    /* Corps de la methode */
    // Verifications preliminaires
//    di = QDir(dirOut);
//    if (di.exists())
//

    di = QDir(dirTle);
    if (!di.exists())
        di.mkdir(dirTle);

    di = QDir(dirDat);
    if (!di.exists()) {
        QString message = tr("POSITION : Erreur rencontr�e lors de l'initialisation\nLe r�pertoire %1 n'existe pas, veuillez r�installer PreviSat");
        message = message.arg(dirDat);
        Messages::Afficher(message, Messages::ERREUR);
        exit(1);
    }

    // Verification de la presence des fichiers du repertoire data
    QStringList ficdata;
    ficdata << "chimes.wav" << "constellations.cst" << "constlines.cst" << "cross.cur" << "donnees.sat" <<
               "etoiles.str" << "gestionnaireTLE" << "iridium.sts";
    QStringListIterator it1(ficdata);
    while (it1.hasNext()) {
        const QFile fi(dirDat + QDir::separator() + it1.next());
        if (!fi.exists()) {
            QString message = tr("POSITION : Le fichier %1 n'existe pas, veuillez r�installer PreviSat");
            message = message.arg(fi.fileName());
            Messages::Afficher(message, Messages::ERREUR);
            exit(1);
        }
    }

    // Chargement des fichiers images de cartes du monde
    di = QDir(dirMap);
    if (!di.exists())
        di.mkdir(dirMap);
    InitFicMap();

    // Verification de l'absence du fichier de mise a jour
    //    QFile fi(dirExe + QDir::separator() + "maj.exe");
    //    if (fi.exists())
    //        fi.remove();

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
    ui->nbJoursAgeMaxTLE->setValue(settings.value("temps/agemax", 15).toInt());
    ui->majTLEAutomatique->setChecked(settings.value("temps/majTLEAutomatique", false).toBool());
    ui->majTLEManuel->setChecked(settings.value("temps/majTLEManuel", true).toBool());
    ui->ageMaxTLE->setChecked(settings.value("temps/ageMaxTLE", true).toBool());
    nomfic = settings.value("fichier/nom", dirTle + QDir::separator() + "visual.txt").toString();
    ui->fichierAMettreAJour->setText(settings.value("fichier/fichierAMettreAJour", nomfic).toString());
    ui->fichierALire->setText(settings.value("fichier/fichierALire", "").toString());
    ui->fichierALireCreerTLE->setText(settings.value("fichier/fichierALireCreerTLE", "").toString());
    ui->nomFichierPerso->setText(settings.value("fichier/nomFichierPerso", "").toString());
    ui->fichierTLEIri->setText(settings.value("fichier/fichierTLEIri", dirTle + QDir::separator() + "iridium.txt").toString());
    ui->fichierTLETransit->setText(settings.value("fichier/fichierTLETransit", nomfic).toString());
    ui->affconst->setChecked(settings.value("affichage/affconst", true).toBool());
    ui->affcoord->setChecked(settings.value("affichage/affcoord", true).toBool());
    ui->affetoiles->setChecked(settings.value("affichage/affetoiles", true).toBool());
    ui->affgrille->setChecked(settings.value("affichage/affgrille", true).toBool());
    ui->affinvew->setChecked(settings.value("affichage/affinvew", false).toBool());
    ui->affinvns->setChecked(settings.value("affichage/affinvns", false).toBool());
    ui->afflune->setChecked(settings.value("affichage/afflune", true).toBool());
    ui->affnomlieu->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomlieu", Qt::PartiallyChecked).toUInt()));
    ui->affnomsat->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomsat", Qt::PartiallyChecked).toUInt()));
    ui->affnotif->setChecked(settings.value("affichage/affnotif", true).toBool());
    ui->affnuit->setChecked(settings.value("affichage/affnuit", true).toBool());
    ui->affphaselune->setChecked(settings.value("affichage/affphaselune", true).toBool());
    ui->affradar->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affradar", Qt::PartiallyChecked).toUInt()));
    ui->affsoleil->setChecked(settings.value("affichage/affsoleil", true).toBool());
    ui->afftraj->setChecked(settings.value("affichage/afftraj", true).toBool());
    ui->affvisib->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affvisib", Qt::PartiallyChecked).toUInt()));
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

    ui->nvEw->setCurrentIndex(0);
    ui->nvNs->setCurrentIndex(0);

    ui->numeroNORADCreerTLE->setCurrentIndex(0);
    ui->ADNoeudAscendantCreerTLE->setCurrentIndex(0);
    ui->excentriciteCreerTLE->setCurrentIndex(0);
    ui->inclinaisonCreerTLE->setCurrentIndex(0);
    ui->argumentPerigeeCreerTLE->setCurrentIndex(0);
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
    messagesStatut->setMinimumSize(778, 0);

    modeFonctionnement = new QLabel("", this);
    modeFonctionnement->setFrameStyle(QFrame::NoFrame);
    modeFonctionnement->setFixedWidth(110);
    modeFonctionnement->setAlignment(Qt::AlignCenter);

    stsDate = new QLabel("", this);
    stsDate->setFrameStyle(QFrame::NoFrame);
    stsDate->setFixedWidth(90);
    stsDate->setAlignment(Qt::AlignCenter);

    stsHeure = new QLabel("", this);
    stsHeure->setFrameStyle(QFrame::NoFrame);
    stsHeure->setFixedWidth(70);
    stsHeure->setAlignment(Qt::AlignCenter);

    ui->barreStatut->addPermanentWidget(messagesStatut, 1);
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

    // Ouverture du fichier TLE par defaut
    if (nbSat == 0) {
        tles.resize(1);
        nbSat = 1;
    } else {
        tles.resize(nbSat);
        bipSat.resize(nbSat);
    }

    // Lecture du fichier TLE par defaut
    const QFileInfo fi(nomfic);
    if (fi.exists()) {

        try {

            // Verification du fichier
            TLE::VerifieFichier(nomfic, true);

            // Lecture du fichier
            TLE::LectureFichier(nomfic, liste, tles);

        } catch (PreviSatException &e) {
        }

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
            tles[0] = TLE(l1, l2);

            // Ouverture du fichier TLE (pour placer dans la liste de l'interface graphique les satellites
            // contenus dans le fichier)
            AfficherListeSatellites(nomfic, liste);
            l1 = "";
            l2 = "";
        } else {
            AfficherListeSatellites(nomfic, liste);
        }
    } else {

        // Remplissage du premier element du tableau du TLE avec le satellite en memoire
        tles.clear();
        tles.append(TLE(l1, l2));
        liste.takeFirst();
        bipSat.resize(1);
        l1 = "";
        l2 = "";
        ui->liste1->clear();
        ui->liste2->clear();
        ui->liste3->clear();
    }

    // Recuperation des donnees satellites
    Satellite::LectureDonnees(liste, tles, satellites);


    // Initialisation de la date
    // Determination automatique de l'ecart heure locale - UTC
    const QDateTime dateLocale = QDateTime::currentDateTime();
    QDateTime dateUTC(dateLocale);
    dateUTC.setTimeSpec(Qt::UTC);
    const double ecart = dateLocale.secsTo(dateUTC) * NB_JOUR_PAR_SEC;
    offsetUTC = (ui->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    ui->updown->setValue((int) (offsetUTC * NB_MIN_PAR_JOUR + EPS_DATES));

    // Date et heure locales
    dateCourante = Date(offsetUTC);

    // Calcul de la position des etoiles
    observateurs[0].CalculPosVit(dateCourante);
    //...

    chronometre->setInterval(200);
    connect(chronometre, SIGNAL(timeout()), this, SLOT(GestionTempsReel()));
    chronometre->start();


    // Mise a jour des TLE si necessaire
    if (ui->majTLEAutomatique->isChecked()) {
        if (ui->ageMaxTLE->isChecked()) {
            const double lastUpdate = settings.value("temps/lastUpdate", 0.).toDouble();
            if (fabs(dateCourante.getJourJulienUTC() - lastUpdate) > ui->nbJoursAgeMaxTLE->value())
                MajWebTLE(false);
        } else {
            messagesStatut->setText(tr("Mise � jour automatique des TLE"));
            MajWebTLE(false);
        }
    }

    if (ui->majTLEManuel->isChecked() && ui->ageMaxTLE->isChecked())
        VerifAgeTLE();

    const int xmax = QApplication::desktop()->width();
    const int ymax = QApplication::desktop()->height();

    if (PreviSat::width() > xmax)
        PreviSat::setGeometry(0, 0, xmax, PreviSat::height());
    if (PreviSat::height() > ymax)
        PreviSat::setGeometry(0, 0, PreviSat::width(), ymax);

    EnchainementCalculs();
    AffichageDonnees();
    AffichageCourbes();

    tim = QDateTime::currentDateTime();

    /* Retour */
    return;
}

/*
 * Chargement de la liste de lieux d'observation
 */
void PreviSat::InitFicObs(const bool alarm)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->coordonnees->setVisible(false);
    ui->nouveauLieu->setVisible(false);
    ui->nouvelleCategorie->setVisible(false);

    /* Corps de la methode */
    ui->fichiersObs->clear();
    ui->ajdfic->clear();

    // Repertoire contenant les fichiers de lieux d'observation
    const QDir di = QDir(dirCoo);
    if (di.exists()) {

        // Nombre de fichiers contenus dans le repertoire 'coordonnees'
        if (di.entryList(QDir::Files).count() == 0) {
            if (alarm)
                Messages::Afficher(tr("POSITION : Erreur rencontr�e lors de l'initialisation\nIl n'existe aucun fichier de lieux d'observation"), Messages::WARNING);
        } else {

            int sts;
            // Liste de fichiers de lieux d'observation
            foreach(QString fic, di.entryList(QDir::Files)) {

                sts = 0;

                // Verification que le fichier est un fichier de lieux d'observation
                const QString file = dirCoo + QDir::separator() + fic;
                QFile fi(file);
                if (fi.exists()) {

                    fi.open(QIODevice::ReadOnly);
                    QTextStream flux(&fi);
                    while (!flux.atEnd()) {
                        const QString lieu = flux.readLine();
                        if (lieu.size() > 0) {
                            if (lieu.size() > 33) {
                                if (lieu.at(14) == ' ' && lieu.at(28) == ' ' && lieu.at(33) == ' ') {
                                    sts = 0;
                                } else {
                                    sts = 1;
                                    break;
                                }
                            } else {
                                sts = 1;
                                break;
                            }
                        }
                    }
                }

                if (sts == 0) {
                    ficObs.append(file);
                    if (fic == "aapreferes") {
                        ui->fichiersObs->addItem(tr("Mes Pr�f�r�s"));
                        ui->ajdfic->addItem(tr("Mes Pr�f�r�s"));
                    } else {
                        ui->fichiersObs->addItem(fic.at(0).toUpper() + fic.mid(1));
                        ui->ajdfic->addItem(fic.at(0).toUpper() + fic.mid(1));
                    }
                }
            }
        }
    } else {
        if (alarm)
            Messages::Afficher(tr("POSITION : Erreur rencontr�e lors de l'initialisation\nLe r�pertoire contenant les fichiers de lieux d'observation n'existe pas"), Messages::WARNING);
    }

    /* Retour */
    return;
}

/*
 * Chargement de la liste de cartes du monde
 */
void PreviSat::InitFicMap()
{
    /* Declarations des variables locales */
    QStringList filtres;

    /* Initialisations */
    QDir di(dirMap);
    filtres << "*.bmp" << "*.jpg" << "*.png";

    /* Corps de la methode */
    ui->listeMap->clear();
    ui->listeMap->addItem(tr("* D�faut"));
    ui->listeMap->setCurrentIndex(0);

    if (di.entryList(filtres, QDir::Files).count() == 0) {
        ui->listeMap->addItem(tr("T�l�charger..."));
    } else {

        foreach(QString fic, di.entryList(filtres, QDir::Files)) {

            const QString file = dirMap + QDir::separator() + fic;
            ficMap.append(file);
            ui->listeMap->addItem(fic.at(0).toUpper() + fic.mid(1, fic.lastIndexOf(".")-1));
            if (settings.value("fichier/listeMap", "").toString() == file)
                ui->listeMap->setCurrentIndex(ficMap.indexOf(file));
        }
        ui->listeMap->addItem(tr("T�l�charger..."));
        if (ui->listeMap->currentIndex() > 0)
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
        if (l1 == "" && l2 == "") {

            // Cas ou aucun satellite n'est selectionne dans la liste de satellites
            ui->satellite->setVisible(false);
            PreviSat::setWindowTitle("PreviSat 3.0");

            if (ui->onglets->count() == 7) {
                ui->onglets->removeTab(1);
                ui->onglets->removeTab(1);
            }
        } else {
            ui->satellite->setVisible(true);
            if (ui->onglets->count() < 7) {
                ui->onglets->insertTab(1, ui->osculateurs, tr("�l�ments osculateurs"));
                ui->onglets->insertTab(2, ui->informations, tr("Informations satellite"));
            }
        }

        /*
     * Affichage des donnees sur l'onglet General
     */
        QString chaine = dateCourante.ToLongDate().append("  ");//.append(tr("UTC"));
        if (fabs(dateCourante.getOffsetUTC()) > EPSDBL100) {
            QTime heur;
            heur = heur.addSecs(NB_SEC_PAR_JOUR * fabs(dateCourante.getOffsetUTC() + EPS_DATES));
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
        if (l1 != "" && l2 != "") {

            // Nom
            ui->nomsat1->setText(nom);
            PreviSat::setWindowTitle("PreviSat 3.0 - " + nom);

            // Temps ecoule depuis l'epoque
            chaine = tr("%1 jours");
            QBrush brush;
            QColor col;
            QPalette palette;
            brush.setStyle(Qt::SolidPattern);

            // Indicateur de l'age du TLE
            if (fabs(satellites.at(0).getAgeTLE()) > 15.) {
                brush.setColor(Qt::red);
            } else if (fabs(satellites.at(0).getAgeTLE()) > 10.) {
                col.setNamedColor("orange");
                brush.setColor(col);
            } else if (fabs(satellites.at(0).getAgeTLE()) > 5.) {
                col.setNamedColor("goldenrod");
                brush.setColor(col);
            } else {
                col.setNamedColor("forestgreen");
                brush.setColor(col);
            }
            palette.setBrush(QPalette::WindowText, brush);
            ui->ageTLE->setPalette(palette);
            ui->ageTLE->setText(chaine.arg(dateCourante.getJourJulienUTC() - tles.at(0).getEpoque().
                                           getJourJulienUTC(), 0, 'f', 2));

            // Longitude/latitude/altitude
            const QString ews = (satellites.at(0).getLongitude() > 0.) ? tr("Ouest") : tr("Est");
            ui->longitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).getLongitude()), Maths::DEGRE,
                                                           3, 0, false, true).append(" ").append(ews));
            const QString nss = (satellites.at(0).getLatitude() > 0.) ? tr("Nord") : tr("Sud");
            ui->latitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).getLatitude()), Maths::DEGRE,
                                                          2, 0, false, true).append(" ").append(nss));
            chaine = "%1 " + unite1;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).getAltitude(), 0, 'f', 1);
                ui->altitudeSat->setText(chaine);
            } else {
                chaine = chaine.arg(satellites.at(0).getAltitude() * MILE_PAR_KM, 0, 'f', 1);
                ui->altitudeSat->setText(chaine);
            }

            // Hauteur/azimut/distance
            ui->hauteurSat->setText(Maths::ToSexagesimal(satellites.at(0).getHauteur(), Maths::DEGRE, 2, 0,
                                                         true, true));
            ui->azimutSat->setText(Maths::ToSexagesimal(satellites.at(0).getAzimut(), Maths::DEGRE, 3, 0,
                                                        false, true));
            chaine = "%1 " + unite1;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).getDistance(), 0, 'f', 1);
                ui->distanceSat->setText(chaine);
            } else {
                chaine = chaine.arg(satellites.at(0).getDistance() * MILE_PAR_KM, 0, 'f', 1);
                ui->distanceSat->setText(chaine);
            }

            // Ascension droite/declinaison/constellation
            ui->ascensionDroiteSat->setText(Maths::ToSexagesimal(satellites.at(0).getAscensionDroite(),
                                                                 Maths::HEURE1, 2, 0, false, true).trimmed());
            ui->declinaisonSat->setText(Maths::ToSexagesimal(satellites.at(0).getDeclinaison(), Maths::DEGRE,
                                                             2, 0, true, true).trimmed());
            ui->constellationSat->setText(satellites.at(0).getConstellation());

            // Direction/vitesse/range rate
            ui->directionSat->setText((satellites.at(0).getVitesse().getZ() > 0.) ? tr("Ascendant") :
                                                                                    tr("Descendant"));
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
                        chaine = chaine.append(" ").append(tr("P�nombre"));
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
                        chaine = chaine.append(("(%1%)"));
                        chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                        ui->magnitudeSat->setText(chaine);
                    }
                } else {
                    chaine = tr("Satellite non visible");
                    if (satellites.at(0).isPenombre())
                        chaine = chaine.append(" : ").append(tr("P�nombre"));
                    chaine = chaine.append(" (%1%)");
                    chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                    if (satellites.at(0).isEclipse() && !satellites.at(0).isPenombre())
                        chaine = tr("Satellite non visible (Ombre)");
                    ui->magnitudeSat->setText(chaine);
                }
            }

            // Conditions d'observation
            const double ht = soleil.getHauteur() * RAD2DEG;
            if (ht >= 0.)
                chaine = tr("Jour");
            else if (ht >= -6.)
                chaine = tr("Cr�puscule civil");
            else if (ht >= -12.)
                chaine = tr("Cr�puscule nautique");
            else if (ht >= -18.)
                chaine = tr("Cr�puscule astronomique");
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
        ui->hauteurSoleil->setText(Maths::ToSexagesimal(soleil.getHauteur(), Maths::DEGRE, 2, 0, true, true));
        ui->azimutSoleil->setText(Maths::ToSexagesimal(soleil.getAzimut(), Maths::DEGRE, 3, 0, false, true));
        chaine = "%1 " + tr("UA");
        chaine = chaine.arg(soleil.getDistanceUA(), 0, 'f', 3);
        ui->distanceSoleil->setText(chaine);

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.getAscensionDroite(), Maths::HEURE1,
                                                                2, 0, false, true).trimmed());
        ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.getDeclinaison(), Maths::DEGRE,
                                                            2, 0, true, true).trimmed());
        ui->constellationSoleil->setText(soleil.getConstellation());


        /*
     * Donnees relatives a la Lune
     */
        // Hauteur/azimut/distance
        ui->hauteurLune->setText(Maths::ToSexagesimal(lune.getHauteur(), Maths::DEGRE, 2, 0, true, true));
        ui->azimutLune->setText(Maths::ToSexagesimal(lune.getAzimut(), Maths::DEGRE, 3, 0, false, true));
        chaine = "%1 " + unite1;
        if (ui->unitesKm->isChecked()) {
            chaine = chaine.arg(lune.getDistance(), 0, 'f', 0);
            ui->distanceLune->setText(chaine);
        } else {
            chaine = chaine.arg(lune.getDistance() * MILE_PAR_KM, 0, 'f', 0);
            ui->distanceLune->setText(chaine);
        }

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteLune->setText(Maths::ToSexagesimal(lune.getAscensionDroite(), Maths::HEURE1, 2, 0,
                                                              false, true).trimmed());
        ui->declinaisonLune->setText(Maths::ToSexagesimal(lune.getDeclinaison(), Maths::DEGRE, 2, 0,
                                                          true, true).trimmed());
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
        if (l1 != "" && l2 != "") {

            ui->nomsat2->setText(nom);
            ui->ligne1->setText(l1);
            ui->ligne2->setText(l2);

            // Vecteur d'etat
            // Position
            Vecteur3D vect;
            vect = (ui->unitesKm->isChecked()) ? satellites.at(0).getPosition() :
                                                 satellites.at(0).getPosition() * MILE_PAR_KM;
            chaine2 = "%1%2 " + unite1;
            chaine = chaine2.arg((vect.getX() >= 0.) ? "+" : "-").arg(fabs(vect.getX()), 0, 'f', 3);
            ui->xsat->setText(chaine);
            chaine = chaine2.arg((vect.getY() >= 0.) ? "+" : "-").arg(fabs(vect.getY()), 0, 'f', 3);
            ui->ysat->setText(chaine);
            chaine = chaine2.arg((vect.getZ() >= 0.) ? "+" : "-").arg(fabs(vect.getZ()), 0, 'f', 3);
            ui->zsat->setText(chaine);

            // Vitesse
            vect = (ui->unitesKm->isChecked()) ? satellites.at(0).getVitesse() :
                                                 satellites.at(0).getVitesse() * MILE_PAR_KM;
            chaine2 = "%1%2 " + unite2;
            chaine = chaine2.arg((vect.getX() >= 0.) ? "+" : "-").arg(fabs(vect.getX()), 0, 'f', 6);
            ui->vxsat->setText(chaine);
            chaine = chaine2.arg((vect.getY() >= 0.) ? "+" : "-").arg(fabs(vect.getY()), 0, 'f', 6);
            ui->vysat->setText(chaine);
            chaine = chaine2.arg((vect.getZ() >= 0.) ? "+" : "-").arg(fabs(vect.getZ()), 0, 'f', 6);
            ui->vzsat->setText(chaine);

            // Elements osculateurs
            chaine = "%1 " + unite1;
            double xval = (ui->unitesKm->isChecked()) ?
                        satellites.at(0).getElements().getDemiGrandAxe() :
                        satellites.at(0).getElements().getDemiGrandAxe() * MILE_PAR_KM;
            chaine = chaine.arg(xval, 0, 'f', 1);
            ui->demiGrandAxe->setText(chaine);

            chaine = "%1";
            chaine = chaine.arg(satellites.at(0).getElements().getExcentricite(), 0, 'f', 7);
            ui->excentricite->setText(chaine);

            chaine2 = "%1�";
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

            chaine = "�" +
                    chaine2.arg(acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellites.at(0).getAltitude())) *
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

            ui->periode->setText(Maths::ToSexagesimal(satellites.at(0).getElements().getPeriode() * HEUR2RAD,
                                                      Maths::HEURE1, 1, 0, false, true));

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
                ui->epoque->setText(tles.at(0).getEpoque().ToShortDate(Date::COURT));

                // Nombre d'orbites a l'epoque
                ui->nbOrbitesEpoque->setText(l2.mid(63, 5).trimmed());

                // Anomalie moyenne
                ui->anomalieMoy->setText(l2.mid(43, 8).append("�"));

                // Inclinaison
                ui->inclinaisonMoy->setText(l2.mid(8, 8).append("�"));

                // Excentricite
                ui->excentriciteMoy->setText("0." + l2.mid(26, 7));

                // Ascension droite du noeud ascendant
                ui->ADNoeudAscendantMoy->setText(l2.mid(17, 8).append("�"));

                // Argument du perigee
                ui->argumentPerigeeMoy->setText(l2.mid(34, 8).append("�"));

                // Nombre de revolutions par jour
                ui->nbRev->setText(l2.mid(52, 11));

                // (derivee de nbRev) / 2
                chaine2 = "%1";
                chaine = chaine2.arg(QString(l1.mid(33, 1).trimmed() + "0" + l1.mid(34, 9)).
                                     toDouble(), 0, 'f', 8);
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
                                          (1. - satellites.at(0).getElements().getExcentricite()) -
                                          RAYON_TERRESTRE));
                    chaine = "%1%2/%3%4";
                    chaine = chaine.arg((satellites.at(0).getMagnitudeStandard() >= 0.) ? "+" : "-").
                            arg(fabs(satellites.at(0).getMagnitudeStandard()), 0, 'f', 1).
                            arg((xval >= 0.) ? "+" : "-").arg(fabs(xval), 0, 'f', 1);
                    ui->magnitudeStdMax->setText(chaine);
                }

                // Modele orbital
                chaine = (satellites.at(0).getMethod() == 'd') ? QObject::tr("SGP4 (DS)") :
                                                                 QObject::tr("SPG4 (NE)");
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
                    chaine = QObject::tr("Sph�rique. R=").append(chaine2.arg(t1, 0, 'f', 1));
                if (fabs(t2) >= EPSDBL100 && fabs(t3) < EPSDBL100)
                    chaine = QObject::tr("Cylindrique. L=").append(chaine2.arg(t1, 0, 'f', 1)).
                            append(QObject::tr(", R=")).append(chaine2.arg(t2, 0, 'f', 1));
                if (fabs(t2) >= EPSDBL100 && fabs(t3) >= EPSDBL100) {
                    chaine = QObject::tr("Bo�te.").append(" %1 x %2 x %3 ").append(unite1);
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
                ui->sq->move(ui->dimensions->x() + ui->dimensions->width() - 4, ui->sq->y());
                info = false;
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage des elements graphiques
 */
void PreviSat::AffichageCourbes()
{
    /* Declarations des variables locales */
    int ind;

    /* Initialisations */
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
        ind = (int) (diff / 12.857) + 2;
        if (ind > 28)
            ind = 0;
    } else {
        ind = 14;
    }
    QString src = ":/resources/lune%1.gif";
    QGraphicsScene *scnlun = new QGraphicsScene;
    QPixmap pixlun;
    pixlun.load(src.arg(ind, 2, 10, QChar('0')));
    pixlun = pixlun.scaled(ui->imglun->size());
    scnlun->addPixmap(pixlun);

    // Couleur du ciel
    QBrush bru(Qt::black);
    QBrush bru2(Qt::black);
    //if ()
    const double hts = soleil.getHauteur() * RAD2DEG;
    if (hts >= 0.) {
        // Jour
        bru = QBrush(QColor::fromRgb(213, 255, 254));

    } else {
        const int red = (int) (213.15126 / (1. + 0.0018199 * exp(-0.983684 * hts)) + 0.041477);
        const int green = (int) (qMax(qMin(256.928983 / (1. + 0.008251 * exp(-0.531535 * hts)) - 0.927648,
                                           255.), 0.));

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
    //}
    const QColor crimson(220, 20, 60);
    const QPen noir(Qt::black);

    /* Corps de la methode */
    QRect rect;
    QGraphicsScene *scene = new QGraphicsScene;
    QPixmap pixmap;
    const QString nomMap = (ui->listeMap->currentIndex() == 0) ?
                ":/resources/map.jpg" :
                ficMap.at(ui->listeMap->currentIndex());

    pixmap.load(nomMap);
    pixmap = pixmap.scaled(ui->carte->size());
    scene->addPixmap(pixmap);

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
            const int tablon[] = { lcarte / 12, lcarte / 6, (int) (lcarte * 0.25), lcarte / 3,
                                   (int) (lcarte / 2.4), (int) (7. * lcarte / 12.), (int) (lcarte / 1.5),
                                   (int) (lcarte * 0.75), (int) (lcarte / 1.2), (int) (11. * lcarte / 12.) };

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
                        zone[j].setX(qRound(soleil.getZone().at((j+jmin-2)%360).x() * DEG2PXHZ));
                        zone[j].setY(qRound(soleil.getZone().at((j+jmin-2)%360).y() * DEG2PXVT));
                    }
                    zone[0] = QPoint(ui->carte->width() - 1, 0);
                    zone[1] = QPoint(ui->carte->width() - 1, hcarte + 1);
                    zone[2] = QPoint(ui->carte->width() - 1, qRound(0.5 * (zone[3].y() + zone[362].y())));

                    zone[363] = QPoint(0, qRound(0.5 * (zone[3].y() + zone[362].y())));
                    zone[364] = QPoint(0, hcarte + 1);
                    zone[365] = QPoint(0, 0);

                } else {

                    for(int j=2; j<362; j++) {
                        zone[j].setX(qRound(soleil.getZone().at((j+jmin-2)%360).x() * DEG2PXHZ));
                        zone[j].setY(qRound(soleil.getZone().at((j+jmin-2)%360).y() * DEG2PXVT));
                    }

                    zone[0] = QPoint(0, 0);
                    zone[1] = QPoint(0, qRound(0.5 * (zone[2].y() + zone[361].y())));

                    zone[362] = QPoint(ui->carte->width() - 1, qRound(0.5 * (zone[2].y() + zone[361].y())));
                    zone[363] = QPoint(ui->carte->width() - 1, 0);
                    zone[364] = QPoint(ui->carte->width() - 1, hcarte + 1);
                    zone[365] = QPoint(0, hcarte + 1);
                }

                const QPolygonF poly(zone);
                scene->addPolygon(poly, QPen(Qt::NoBrush, 0), alpha);
            } else {

                QVector<QPoint> zone1;
                zone1.resize(4);
                const int x1 = qRound(qMin(soleil.getZone().at(90).x(), soleil.getZone().at(270).x()) *
                                      DEG2PXHZ);
                const int x2 = qRound(qMax(soleil.getZone().at(90).x(), soleil.getZone().at(270).x()) *
                                      DEG2PXHZ);

                if (lsol > lcarte / 4 && lsol < (4 * lcarte) / 3) {

                    QVector<QPoint> zone2;

                    zone1[0] = QPoint(0, 0);
                    zone1[1] = QPoint(x1, 0);
                    zone1[2] = QPoint(x1, hcarte + 1);
                    zone1[3] = QPoint(0, hcarte + 1);

                    zone2[0] = QPoint(ui->carte->width() - 1, 0);
                    zone2[1] = QPoint(x2, 0);
                    zone2[2] = QPoint(x2, hcarte + 1);
                    zone2[3] = QPoint(lcarte + 1, hcarte + 1);

                    const QPolygonF poly1(zone1);
                    const QPolygonF poly2(zone2);
                    scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
                    scene->addPolygon(poly2, QPen(Qt::NoBrush, 0), alpha);

                } else {

                    zone1[0] = QPoint(x1, 0);
                    zone1[1] = QPoint(x1, hcarte + 1);
                    zone1[2] = QPoint(x2, hcarte + 1);
                    zone1[3] = QPoint(x2, 0);

                    const QPolygonF poly1(zone1);
                    scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
                }
            }
        }

        // Affichage de la Lune
        if (ui->afflune->isChecked()) {

            const int llun = qRound((180. - lune.getLongitude() * RAD2DEG) * DEG2PXHZ) +
                    ui->carte->geometry().left() - 1;
            const int blun = qRound((90. - lune.getLatitude() * RAD2DEG) * DEG2PXVT) +
                    ui->carte->geometry().top() - 1;

            ui->imglun->setStyleSheet("background: transparent; border: none");
            ui->imglun->setScene(scnlun);
            if (ui->rotationLune->isChecked() && observateurs.at(0).getLatitude() < 0.)
                ui->imglun->rotate(180.);

            ui->imglun->setGeometry(llun-7, blun-7, 17, 17);

        }

        // Lieux d'observation
        int nbMax;
        if (ui->affnomlieu->checkState() == Qt::Unchecked)
            nbMax = 0;
        if (ui->affnomlieu->checkState() == Qt::Checked ||
                ui->affnomlieu->checkState() == Qt::PartiallyChecked)
            nbMax = ui->lieuxObservation1->count() - 1;

        QPen crayon(Qt::white);
        for(int j=nbMax; j>=0; j--) {

            const int lobs = qRound((180. - observateurs.at(j).getLongitude() * RAD2DEG) * DEG2PXHZ);
            const int bobs = qRound((90. - observateurs.at(j).getLatitude() * RAD2DEG) * DEG2PXVT);

            scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
            scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

            if (j == 0 || ui->affnomlieu->checkState() == Qt::PartiallyChecked) {

                QGraphicsSimpleTextItem *txtObs = new QGraphicsSimpleTextItem(observateurs.at(j).
                                                                              getNomlieu());
                const int lng = txtObs->boundingRect().width();
                const int xnobs = (lobs + 4 + lng > lcarte) ? lobs - lng - 1 : lobs + 4;
                const int ynobs = (bobs + 9 > hcarte) ? bobs - 12 : bobs;

                txtObs->setBrush(Qt::white);
                txtObs->setPos(xnobs, ynobs);
                scene->addItem(txtObs);
            }
        }

        // Affichage de la trace au sol du satellite par defaut
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

        // Affichage de la zone de visibilite des satellites
        if (ui->affvisib->isChecked()) {
            if (ui->affvisib->checkState() == Qt::Checked)
                nbMax = 1;
            if (ui->affvisib->checkState() == Qt::PartiallyChecked)
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

                    if ((ui->affnomsat->checkState() == Qt::Checked && isat == 0) ||
                            ui->affnomsat->checkState() == Qt::PartiallyChecked) {
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
        //...
    }

    // Radar
    htr = false;
    if (ui->affradar->checkState() == Qt::Checked || (ui->affradar->checkState() == Qt::PartiallyChecked && ht)) {

        ui->frameZone->setVisible(true);
        QGraphicsScene *scene2 = new QGraphicsScene;
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

        ui->imglun2->setStyleSheet("background: transparent; border: none");
        if (ui->afflune->isChecked() && lune.isVisible()) {

            // Calcul des coordonnees radar de la Lune
            const int llun = qRound(100. - 100. * xf * (1. - lune.getHauteur() * DEUX_SUR_PI) * sin(lune.getAzimut()));
            const int blun = qRound(100. - 100. * yf * (1. - lune.getHauteur() * DEUX_SUR_PI) * cos(lune.getAzimut()));

            ui->imglun2->setScene(scnlun);
            if (ui->rotationLune->isChecked() && observateurs.at(0).getLatitude() < 0.)
                ui->imglun2->rotate(180.);

            ui->imglun2->setGeometry(llun-7, blun-7, 17, 17);
        }

        for(int isat=nbSat-1; isat>=0; isat--) {

            if (satellites.at(isat).isVisible() && !satellites.at(isat).isIeralt()) {

                // Calcul des coordonnees radar du satellite
                const int lsat = qRound(100. - 100. * xf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) * sin(satellites.at(isat).getAzimut()));
                const int bsat = qRound(100. - 100. * yf * (1. - satellites.at(isat).getHauteur() * DEUX_SUR_PI) * cos(satellites.at(isat).getAzimut()));

                rect = QRect(lsat - 3, bsat - 3, 6, 6);
                QColor col = (satellites.at(isat).isEclipse()) ? crimson : Qt::yellow;
                scene2->addEllipse(rect, noir, QBrush(col, Qt::SolidPattern));
            }
        }

        // Cercle exterieur du radar
        scene2->addEllipse(0, 0, 200, 200, QPen(QBrush(Qt::gray), 2.1));
        scene2->addEllipse(-4, -4, 208, 208, QPen(QBrush(ui->frameZone->palette().background().color()), 6));
        ui->radar->setScene(scene2);
    } else {
        ui->frameZone->setVisible(false);
    }

    /* Retour */
    return;
}

/*
 * Affichage du lieu d'observation sur l'interface graphique
 */
void PreviSat::AffichageLieuObs()
{
    /* Declarations des variables locales */
    double atd, la, lo;

    /* Initialisations */
    listeObs = settings.value("observateur/lieu", "Paris#-002.345277778&+48.860833333&60").toString().split("$");

    /* Corps de la methode */
    ui->lieuxObservation1->clear();
    ui->lieuxObservation2->clear();
    ui->lieuxObservation3->clear();
    ui->lieuxObservation4->clear();
    ui->selecLieux->clear();
    QStringListIterator it(listeObs);
    while (it.hasNext()) {

        const QString obs = it.next();
        int delim = obs.indexOf("#");
        const QString nomlieu = obs.mid(0, delim).trimmed();

        ui->lieuxObservation1->addItem(nomlieu);
        ui->lieuxObservation2->addItem(nomlieu);
        ui->lieuxObservation3->addItem(nomlieu);
        ui->lieuxObservation4->addItem(nomlieu);
        ui->selecLieux->addItem(nomlieu);

        // Longitude/Latitude/Altitude
        const QStringList coord = obs.mid(delim + 1).split("&");
        lo = coord.at(0).toDouble();
        la = coord.at(1).toDouble();
        atd = coord.at(2).toInt();

        // Creation du lieu d'observation
        observateurs.append(Observateur(nomlieu, lo, la, atd));

        if (obs == listeObs.first()) {

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");
            QString alt = "%1 %2";

            // Affichage des coordonnees
            ui->longitudeObs->setText(Maths::ToSexagesimal(fabs(lo) * DEG2RAD, Maths::DEGRE, 3, 0,
                                                           false, true).append(" ").append(ew));
            ui->latitudeObs->setText(Maths::ToSexagesimal(fabs(la) * DEG2RAD, Maths::DEGRE, 2, 0,
                                                          false, true).append(" ").append(ns));
            alt = alt.arg((ui->unitesKm->isChecked()) ? atd : atd * PIED_PAR_METRE).
                    arg((ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
            ui->altitudeObs->setText(alt);
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
void PreviSat::AfficherListeSatellites(const QString fichier, const QStringList listeSat)
{
    /* Declarations des variables locales */
    QString ligne, li1, li2, magn, nomsat, norad;

    /* Initialisations */
    nbSat = 0;
    nomsat = "---";
    mapSatellites.clear();
    QFile donneesSatellites("data/donnees.sat");
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly);
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

        QListWidgetItem *elem0 = new QListWidgetItem();
        fichierTLE.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichierTLE);

        while (!flux.atEnd()) {

            ligne = flux.readLine();

            if (ligne.at(0) == '1') {

                li1 = ligne;
                do {
                    li2 = flux.readLine();
                } while (li2.trimmed().length() == 0);

                norad = li1.mid(2, 5);
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
                    nomsat = "ISS";
                if (nomsat.toLower().contains("iridium") && nomsat.contains("["))
                    nomsat = nomsat.mid(0, nomsat.indexOf("[")).trimmed();
                if (nomsat.contains(" DEB") || nomsat.contains("R/B"))
                    nomsat = nomsat.append("  (").append(norad).append(")");

                bool check = false;
                for (int j=0; j<listeSat.length(); j++) {
                    if (norad == listeSat.at(j)) {
                        check = true;
                        nbSat++;
                        if (j == 0) {
                            settings.setValue("TLE/nom", nomsat);
                            settings.setValue("TLE/lg1", li1);
                            settings.setValue("TLE/lg2", li2);
                            nom = nomsat;

                            l1 = li1;
                            l2 = li2;
                        }
                        break;
                    }
                }

                // Ajout du satellite dans la liste de satellites
                mapSatellites.append(nomsat + "#" + norad);
                QListWidgetItem *elem1 = new QListWidgetItem(nomsat, ui->liste1);
                elem1->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                QListWidgetItem *elem2 = new QListWidgetItem(nomsat, ui->liste2);
                elem2->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                QListWidgetItem *elem3 = new QListWidgetItem(nomsat, ui->liste3);
                elem3->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                if (nomsat == nom)
                    elem0 = elem1;
            }
            nomsat = ligne.trimmed();
        }
        mapSatellites.sort();
        if (li1.length() > 0) {
            ui->liste1->setCurrentItem(elem0);
            ui->liste2->setCurrentRow(ui->liste1->currentRow());
            ui->liste3->setCurrentRow(ui->liste1->currentRow());
        }
    }

    /* Retour */
    return;
}


/***********
 * Calculs *
 **********/

/*
 * Enchainement des calculs
 */
void PreviSat::EnchainementCalculs()
{
    /* Declarations des variables locales */
    bool extinction, visibilite;
    int nbTraces;

    /* Initialisations */
    // Nombre de traces au sol a afficher
    nbTraces = (ui->afftraj->isChecked()) ? ui->nombreTrajectoires->value() : 0;

    // Prise en compte de l'extinction atmospherique
    extinction = ui->extinctionAtmospherique->isChecked();

    // Calcul de la zone de visibilite des satellites
    visibilite = !ui->carte->isHidden();

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


    /*
     * Calcul de la position du catalogue d'etoiles
     */
    //if
    //...

    /*
     * Calcul de la position courante des satellites
     */
    Satellite::CalculPosVitListeSatellites(dateCourante, observateurs.at(0), soleil, nbTraces, visibilite,
                                           extinction, satellites);

    for (int i=0; i<nbSat; i++) {
        if (satellites[i].isVisible() && !bipSat[i]) {
            notif = true;
            bipSat[i] = true;
        }
    }

    if (nbSat > 0) {
        if (ui->onglets->count() == 7 && satellites[0].isIeralt()) {
            //...
            QString msg = tr("POSITION : Erreur rencontr�e lors de l'ex�cution\nLa position du satellite %1 (num�ro NORAD : %2) ne peut pas �tre calcul�e (altitude n�gative)");
            msg = msg.arg(tles.at(0).getNom()).arg(tles.at(0).getNorad());
            Messages::Afficher(msg, Messages::WARNING);
            //...
            l1 = "";
            l2 = "";
        }
        if (ui->onglets->count() < 7 && !satellites[0].isIeralt()) {
            l1 = tles.at(0).getLigne1();
            l2 = tles.at(0).getLigne2();
        }
    }

    /* Retour */
    return;
}

void PreviSat::MajWebTLE(const bool alarm) const
{

}

void PreviSat::VerifAgeTLE() const
{

}


/*************
 * Interface *
 ************/

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
        modeFonctionnement->setText(tr("Temps r�el"));
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
        stsDate->setText(QDate(date1.getAnnee(), date1.getMois(), date1.getJour()).toString("dd/MM/yyyy"));
        stsHeure->setText(QTime(date1.getHeure(), date1.getMinutes(), date1.getSecondes()).toString("hh:mm:ss"));
        stsDate->setToolTip(tr("Date"));
        stsHeure->setToolTip(tr("Heure"));
    }

    // Lancement des calculs
    if (ui->tempsReel->isChecked() && tim.secsTo(QDateTime::currentDateTime()) >= pas1) {
        tim = (tim.addSecs(pas1) <= QDateTime::currentDateTime()) ? tim.addSecs(pas1) : QDateTime::currentDateTime();

        // Date actuelle
        dateCourante = Date(offsetUTC);

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Donnees numeriques
        AffichageDonnees();

        // Affichage des courbes
        AffichageCourbes();
    }

    if (ui->modeManuel->isChecked() && fabs(tim.secsTo(QDateTime::currentDateTime()) >= pas2)) {
        tim = QDateTime::currentDateTime();

        if (ui->pause->isEnabled()) {

            if (ui->pasManuel->view()->isVisible()) {

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

/*
 * Fin de l'application
 */
void PreviSat::closeEvent(QCloseEvent *)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QDir di = QDir(dirOut);
    if (di.exists())
        di.rmdir(dirOut);

    // Sauvegarde des donnees du logiciel
    settings.setValue("temps/valManuel", ui->valManuel->currentIndex());
    settings.setValue("temps/pasManuel", ui->pasManuel->currentIndex());
    settings.setValue("temps/pasReel", ui->pasReel->currentIndex());
    settings.setValue("temps/ageMax", ui->nbJoursAgeMaxTLE->value());
    settings.setValue("temps/majTLEAutomatique", ui->majTLEAutomatique->isChecked());
    settings.setValue("temps/majTLEManuel", ui->majTLEManuel->isChecked());
    settings.setValue("temps/ageMaxTLE", ui->ageMaxTLE->isChecked());
    settings.setValue("temps/dtu", ui->updown->value() * NB_JOUR_PAR_MIN);

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
    settings.setValue("affichage/affradar", ui->affradar->checkState());
    settings.setValue("affichage/affsoleil", ui->affsoleil->isChecked());
    settings.setValue("affichage/afftracesol", ui->afftraj->isChecked());
    settings.setValue("affichage/affconst", ui->affconst->isChecked());
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

    settings.setValue("fichier/listeMap", (ui->listeMap->currentIndex() == 0) ? "" : ficMap.at(qMax(0, ui->listeMap->currentIndex()-1)));
    settings.setValue("fichier/nom", nomfic);
    settings.setValue("fichier/iridium", ui->fichierTLEIri->text());
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
    ui->carte->setGeometry(6, 6, ui->frameCarte->width() - 47, ui->frameCarte->height() - 23);
    ui->maximise->move(11 + ui->carte->width(), 5);

    const int hcarte = ui->carte->height() - 3;
    const int lcarte = ui->carte->width() - 3;

    DEG2PXHZ = lcarte / T360;
    DEG2PXVT = hcarte * 2. / T360;

    ui->liste1->setGeometry(10, 149, 200, hcarte - 140);

    ui->S60->move(5, hcarte / 6. - 8);
    ui->S30->move(5, hcarte / 3. - 8);
    ui->SS->move(5, hcarte / 2.4 - 8);
    ui->N0->move(5, hcarte * 0.5 - 8);
    ui->NN->move(5, hcarte * 7. / 12. - 8);
    ui->N30->move(5, hcarte / 1.5 - 8);
    ui->N60->move(5, hcarte / 1.2 - 8);
    ui->frameLat->setGeometry(7 + ui->carte->width(), 0, ui->frameLat->width(), ui->carte->height());

    ui->W150->move(lcarte / 12. - 8, 0);
    ui->W120->move(lcarte / 6. - 8, 0);
    ui->W90->move(lcarte / 4. - 4, 0);
    ui->W60->move(lcarte / 3. - 4, 0);
    ui->W30->move(lcarte / 2.4 - 4, 0);
    ui->WW->move(lcarte * 11. / 24. - 2, 0);
    ui->W0->move(lcarte * 0.5 - 1, 0);
    ui->EE->move(lcarte * 13. / 24. - 2, 0);
    ui->E30->move(lcarte * 7. / 12. - 4, 0);
    ui->E60->move(lcarte / 1.5, 0);
    ui->E90->move(lcarte * 0.75 - 4, 0);
    ui->E120->move(lcarte / 1.2 - 8, 0);
    ui->E150->move(lcarte * 11. / 12. - 8, 0);
    ui->frameLon->setGeometry(6, 7 + ui->carte->height(), ui->carte->width(), ui->frameLon->height());

    ui->onglets->move(ui->frameCarte->width() * 0.5 - 419, 0);
    ui->enregistrerOnglet->move(ui->onglets->x() + 774, 3);
    //...

    if (Satellite::initCalcul) {

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des courbes
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::keyPressEvent(QKeyEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Capture de la fenetre
    if (event->key() == Qt::Key_F8) {

        chronometre->stop();
        const QPixmap image = QPixmap::grabWidget(QApplication::activeWindow());
        const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"),
                                                         nomfic.mid(0, qMax(0, nomfic.lastIndexOf(QDir::separator()))),
                                                         tr("Fichiers JPEG (*.jpg);;Fichiers BMP (*.bmp);;Tous les fichiers (*)"));
        if (!fic.isEmpty())
            image.save(fic);
        chronometre->start();

    } else {

        Qt::KeyboardModifiers mod = QApplication::keyboardModifiers();
        if (mod.testFlag(Qt::ControlModifier)) {

            if (ui->liste1->hasFocus())
                ui->carte->setFocus();

            // Agrandissement de la carte
            if (event->key() == Qt::Key_M)
                MaximiseCarte();

        } else {

            double jd = dateCourante.getJourJulien();

            // Etape precedente/suivante (mode manuel)
            if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_F12) {

                int sgn;
                if (!ui->modeManuel->isChecked())
                    ui->modeManuel->setChecked(true);
                if (event->key() == Qt::Key_F11)
                    sgn = -1;
                if (event->key() == Qt::Key_F12)
                    sgn = 1;

                jd = (ui->valManuel->currentIndex() < 3) ? jd + sgn * ui->pasManuel->currentText().toDouble() *
                                                           qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) *
                                                           NB_JOUR_PAR_SEC :
                                                           jd + sgn * ui->pasManuel->currentText().toDouble();

                Date date = Date(jd + EPS_DATES, 0.);

                if (ui->dateHeure4->isVisible()) {
                    ui->dateHeure4->setDateTime(date.ToQDateTime(1));
                } else {
                    ui->dateHeure3->setDateTime(date.ToQDateTime(1));
                    ui->dateHeure3->setFocus();
                }

                // Affichage de la carte du ciel ou de la carte du monde
                if (event->key() == Qt::Key_F9)
                    AffichageCiel();
            }
        }
    }

    /* Retour */
    return;
}

void PreviSat::mousePressEvent(QMouseEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (event->button() == Qt::LeftButton) {

        // Selection du satellite sur la carte du monde
        if (event->x() > 6 && event->x() < 809 && event->y() > 6 && event->y() < 409) {

            const int x = event->x() - 6;
            const int y = event->y() - 6;
            for (int isat=0; isat<nbSat; isat++) {

                const int lsat = qRound((180. - satellites.at(isat).getLongitude() * RAD2DEG) * DEG2PXHZ);
                const int bsat = qRound((90. - satellites.at(isat).getLatitude() * RAD2DEG) * DEG2PXVT);

                // Distance au carre du curseur au satellite
                const int dt = (x - lsat) * (x - lsat) + (y - bsat) * (y - bsat);

                // Le cuseur est au-dessus d'un satellite
                if (dt <= 16) {
                    const QString norad = liste.at(isat);
                    liste[isat] = liste.at(0);
                    liste[0] = norad;

                    bool bip = bipSat.at(isat);
                    bipSat[isat] = bipSat.at(0);
                    bipSat[0] = bip;

                    Satellite::initCalcul = false;

                    // Ecriture des cles de registre
                    EcritureListeRegistre();

                    info = true;

                    // Enchainement de l'ensemble des calculs
                    EnchainementCalculs();

                    // Affichage des donnees numeriques
                    AffichageDonnees();

                    // Affichage des elements sur la carte du monde et le radar
                    AffichageCourbes();
                }
            }
        }
    }

    /* Retour */
    return;
}

void PreviSat::AffichageCiel() const
{

}

void PreviSat::EnregistrerOnglet() const
{

}

void PreviSat::MaximiseCarte() const
{

}


/***********
 * Systeme *
 **********/
void PreviSat::EcritureListeRegistre()
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

            const QString nor = liste.at(0);

            int j = 0, k = 0;
            tles.clear();
            liste.clear();
            tles.resize(nbSat);
            bipSat.resize(nbSat);

            if (nbSat == 0) {
                l1 = "";
                l2 = "";
            } else {
                liste.reserve(nbSat);
            }

            for (int i=0; i<mapSatellites.size(); i++) {
                QListWidgetItem *elem1 = new QListWidgetItem(mapSatellites.at(i));
                if (elem1->checkState() == Qt::Checked) {
                    liste[k] = mapSatellites.at(i).mid(mapSatellites.indexOf("#"));
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

void PreviSat::on_maximise_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->frameOngletsZone->sizePolicy().verticalPolicy() == QSizePolicy::Preferred) {
        ui->frameOngletsZone->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
        ui->onglets->setVisible(false);
        ui->enregistrerOnglet->setVisible(false);
    } else {
        ui->frameOngletsZone->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        ui->onglets->setVisible(true);
        ui->enregistrerOnglet->setVisible(true);
    }

    if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Preferred) {

        // Carte maximisee
        ui->frameListe->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred));
        ui->frameCarte->setGeometry(0, 0, PreviSat::width(), PreviSat::height() - 23);
        ui->maximise->setIcon(QIcon(":/resources/mini.png"));
        ui->maximise->setToolTip(tr("R�duire"));
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

void PreviSat::SauveOngletGeneral()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

void PreviSat::SauveOngletElementsOsculateurs()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}

void PreviSat::SauveOngletInformations()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    return;
}



void PreviSat::on_actionOuvrir_fichier_TLE_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Ouverture d'un fichier TLE
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), dirTle,
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz);;Tous les fichiers (*)"));

    try {
        if (!fichier.isEmpty()) {

            int nsat;
            QFileInfo fi(fichier);
            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = fi.canonicalPath() + QDir::separator() + fi.completeBaseName();

                if (DecompressionFichierGz(fichier, fic)) {
                    fichier = fic;
                } else {
                    throw PreviSatException(tr("POSITION : Erreur rencontr�e lors de la d�compression du fichier ") + fichier, Messages::WARNING);
                }
            }

            messagesStatut->setText(tr("Ouverture du fichier TLE..."));

            // Verification du fichier TLE
            try {
                nsat = TLE::VerifieFichier(fichier, true);
                nomfic = (fi.absoluteDir() == dirTle) ? fi.baseName() : fichier;
            } catch (PreviSatException &e) {
                messagesStatut->setText("");
                throw PreviSatException();
            }

            ui->nomFichierTLE->setText(nomfic);
            const QString chaine = tr("Fichier TLE OK : %1 satellites");
            messagesStatut->setText(chaine.arg(nsat));
            listeFicTLE.insert(0, nomfic);
            EcritureListeRegistre();

            // Ouverture du fichier TLE
            AfficherListeSatellites(nomfic, liste);

            // Mise a jour de la liste de satellites selectionnes
            if (nbSat > 0) {
                const QString nor = liste.at(0);
                nbSat = 0;
                for (int i=0; i<mapSatellites.size(); i++) {
                    QListWidgetItem *elem1 = new QListWidgetItem(mapSatellites.at(i));
                    if (elem1->checkState() == Qt::Checked)
                        nbSat++;
                }

                if (nbSat > 0) {
                    liste.clear();
                    liste.reserve(nbSat);
                    tles.clear();
                    tles.resize(nbSat);
                    bipSat.resize(nbSat);

                    int j = -1;
                    for (int i=0; i<nbSat; i++) {
                        QListWidgetItem *elem1 = new QListWidgetItem(mapSatellites.at(i));
                        if (elem1->checkState() == Qt::Checked) {
                            liste[i] = mapSatellites.at(i).split("#").at(0);
                            if (liste.at(i) == nor)
                                j = i;
                        }
                    }

                    if (j > 0) {
                        liste[j] = liste[0];
                        liste[0] = nor;
                    }

                    info = true;
                    Satellite::initCalcul = false;

                    EcritureListeRegistre();

                    if (nbSat == 0) {
                        l1 = "";
                        l2 = "";
                        liste.clear();
                    }

                    // Enchainement de l'ensemble des calculs
                    EnchainementCalculs();

                    // Affichage des donnees numeriques
                    AffichageDonnees();

                    //Affichage des elements sur la carte du monde et le radar
                    AffichageCourbes();

                    // Verification de l'age du TLE
                    if (!l1.isEmpty() && !l2.isEmpty()) {
                        old = false;
                        VerifAgeTLE();
                    }
                } else {
                    // Aucun satellite de la liste n'est dans le nouveau fichier
                    ui->liste1->setCurrentRow(0);
                    l1 = "";
                    l2 = "";

                    // Enchainement de l'ensemble des calculs
                    EnchainementCalculs();

                    // Affichage des donnees numeriques
                    AffichageDonnees();

                    //Affichage des elements sur la carte du monde et le radar
                    AffichageCourbes();
                }
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
    const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), dirExe,
                                                         tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        switch (ui->onglets->currentIndex()) {
        case 0:
            // Sauvegarde de l'onglet General
            SauveOngletGeneral();
            break;
        case 1:
            // Sauvegarde de l'onglet Elements osculateurs
            SauveOngletElementsOsculateurs();
            break;
        case 2:
            // Sauvegarde de l'onglet Informations satellite
            SauveOngletInformations();
            break;
        default:
            break;
        }
    }

    /* Retour */
    return;
}

bool PreviSat::DecompressionFichierGz(const QString fichierGz, const QString fichierDecompresse)
{
    /* Declarations des variables locales */
    char buffer[8192];

    /* Initialisations */
    bool res = false;

    /* Corps de la methode */
    gzFile ficGz = gzopen(fichierGz.toStdString().c_str(), "rb");
    if (ficGz == NULL) {
        res = false;
    } else {

        QFile ficDec(fichierDecompresse);
        ficDec.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux(&ficDec);

        while (gzgets(ficGz, buffer, 8192) != NULL)
            flux << buffer;
        gzclose(ficGz);
        ficDec.close();
        res = true;
    }

    /* Retour */
    return (res);
}

void PreviSat::on_actionAstropedia_free_fr_activated()
{
    QDesktopServices::openUrl(QUrl("http://astropedia.free.fr/"));
}

void PreviSat::on_actionRapport_de_bug_activated()
{
    QDesktopServices::openUrl(QUrl("http://astropedia.free.fr/contacts/rapport/rapport.html"));
}

void PreviSat::on_actionTelecharger_les_mises_jour_activated()
{
    QDesktopServices::openUrl(QUrl("http://sourceforge.net/projects/previsat/"));
}

void PreviSat::on_actionDonnez_votre_avis_activated()
{
    QDesktopServices::openUrl(QUrl("http://astropedia.free.fr/contacts/avis/avis.html"));
}

void PreviSat::on_actionWww_celestrak_com_activated()
{
    QDesktopServices::openUrl(QUrl("http://www.celestrak.com"));
}

void PreviSat::on_actionWww_space_track_org_activated()
{
    QDesktopServices::openUrl(QUrl("http://www.space-track.org"));
}






