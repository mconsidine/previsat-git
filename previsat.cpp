﻿/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    20 octobre 2019
 *
 */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

#include <QClipboard>
#include <QDesktopServices>
#include <QSound>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wpacked"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wundef"
#if QT_VERSION >= 0x050000
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QtNetwork>
#include <QWebView>
#include "ui_previsat.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wpacked"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wundef"
#if QT_VERSION >= 0x050000
#pragma GCC diagnostic warning "-Wfloat-conversion"
#endif
#include <QToolTip>
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "previsions/conditions.h"
#include "previsions/evenements.h"
#include "previsions/metop.h"
#include "previsions/prevision.h"
#include "previsions/transitiss.h"
#include "afficher.h"
#include "apropos.h"
#include "gestionnairetle.h"
#include "informations.h"
#include "previsat.h"
#include "telecharger.h"
#include "threadcalculs.h"
#include "zlib.h"

#define BOOL_STR(b) ((b) ? "true" : "false")

// Repertoires
static QString dirCoord;
static QString dirCommonData;
static QString dirLocalData;
static QString dirExe;
static QString dirMap;
static QString dirOut;
static QString dirPrf;
static QString dirRsc;
static QString dirSon;
static QString dirTle;
static QString dirTmp;

// TLE par defaut
static QString nom;
static QString l1;
static QString l2;

// Liste de satellites
static bool htr;
static bool info;
static bool notifAOS;
static bool notifLOS;
static bool old;
static bool acalcAOS;
static bool acalcDN;
static bool isAOS;
static bool isEcl;
static int chg;
static int ind;
static int idxf;
static int idxfi;
static int idxfm;
static int nbSat;
static int notifFlash;
static int deltaNbOrb;
static double azimAOS;
static double htSat;
static QString ctypeAOS;
static QString nomfic;
static QString ficgz;
static QString nor;
static QVector<bool> bipSatAOS;
static QVector<bool> bipSatLOS;
static QVector<TLE> tles;
static QVector<TLE> tab3le;
static QList<Satellite> satellites;
static QString liste;
static QString donneesSat;
static QStringList listeTLE;
static QStringList ficTLE;
static QStringList ficTLEMetOp;
static QStringList listeGroupeMaj;
static QStringList resultatsSatellitesTrouves;
static QStringList tabStatutMetOp;
static QStringList tabTDRS;

static QString ficRes;

// Date courante
static Date dateCourante;
static double offsetUTC;
static QDateTime tim;
static Date dateAOS;
static Date dateEcl;
static Date date_lct;

// Lieux d'observation
static int selec;
static QString ligneCoord;
static QList<Observateur> observateurs;
static QStringList ficObs;
static QStringList listeObs;
static QStringList mapObs;

// Stations
static QList<Observateur> stations;

// Cartes du monde
static int selec2;
static QStringList ficMap;
static double DEG2PXHZ = 1. / 0.45;
static double DEG2PXVT = 1. / 0.45;

// Fichiers sons
static int selec3;
static QStringList ficSonAOS;
static QStringList ficSonLOS;

// Fichiers de preferences
static QStringList ficPrf;
static int selec4;

// Soleil, Lune, etoiles, planetes
static Soleil soleil;
static Lune lune;
static QList<Etoile> etoiles;
static QList<Constellation> constellations;
static QList<LigneConstellation> lignesCst;
static QList<Planete> planetes;
static const QList<QColor> couleurPlanetes(QList<QColor> () << Qt::gray << Qt::white << Qt::red << QColor("orange") <<
                                           Qt::darkYellow << Qt::green << Qt::blue);

// SAA
static const double tabSAA[59][2] = { { -96.5, -29. }, { -95., -24.5 }, { -90., -16. }, { -85., -10. }, { -80., -6. },
                                      { -75., -3.5 }, { -70., 0. }, { -65., 4. }, { -60., 6.5 }, { -55., 8. },
                                      { -50., 9. }, { -45., 10. }, { -40., 11. }, { -35., 12. }, { -30., 13. },
                                      { -25., 12. }, { -20., 9.5 }, { -15., 8. }, { -10., 7. }, { -5., 6. }, { 0., 4. },
                                      { 5., 2. }, { 10., -3. }, { 15., -4. }, { 20., -5. }, { 25., -6. }, { 30., -8. },
                                      { 35., -11.5 }, { 40., -14. }, { 42.5, -17.5 }, { 40., -22. }, { 35., -23.5 },
                                      { 30., -25. }, { 25., -27. }, { 20., -29. }, { 15., -32. }, { 10., -33.5 },
                                      { 5., -35. }, { 0., -36. }, { -5., -37. }, { -10., -38.5 }, { -15., -42.5 },
                                      { -20., -44.5 }, { -25., -46. }, { -30., -47.5 }, { -35., -48.5 }, { -40., -49.5 },
                                      { -45., -49. }, { -50., -48.5 }, { -55., -47.5 }, { -60., -47. }, { -65., -46.5 },
                                      { -70., -45.5 }, { -75., -43.5 }, { -80., -42. }, { -85., -38.5 }, { -90., -36. },
                                      { -95., -33. }, { -96.5, -29. } };

// SAA pour la visualisation Wall Command Center
static const double tabSAA_ISS[16][2] = { { 55.5, -17.3 }, { 47., -17.3 }, { 34.3, -20. }, { 14.5, -28. }, { -16., -31.6 },
                                          { -26.5, -35.5 }, { -28.3, -40.6 }, { -21.6, -45.6 }, { 2.5, -53. }, { 42., -53. },
                                          { 54., -47.6 }, { 62.2, -36. }, { 63.3, -31. }, { 63.3, -24. }, { 60.5, -19.2 },
                                          { 55.5, -17.3 } };

// Couleurs GMT
static const QColor cgmt[3] = { Qt::red, Qt::white, Qt::cyan };

// Ecliptique
static const double tabEcliptique[49][2] = { { 0., 0. }, { 0.5, 3.233 }, { 1., 6.4 }, { 1.5, 9.417 },  { 2., 12.217 },
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
static bool aclickFicMaj;
static bool amajDeb;
static bool amajPrevi;
static bool atrouve;
static bool aupdnow;
static bool aupdateCF5;
static int cptCF5;
static QNetworkAccessManager mng;
static QQueue<QUrl> downQueue;
static QNetworkReply *rep;
static QFile ficDwn;
static QString dirDwn;
static QString adresseAstropedia;
static const QString adresseCelestrak = "http://www.celestrak.com/";
static const QString adresseCelestrakNorad = adresseCelestrak + "NORAD/elements/";
static QString majInfosDate;

static QTimer *chronometre;
static ThreadCalculs *threadCalculs;
static Afficher *afficherResultats;
static QMainWindow *afficherMeteo;
static QMainWindow *afficherManoeuvre;
static GestionnaireTLE *gestionnaire;
static QString localePreviSat;
static Conditions conditions;
static QStringList listeFicLocalData;
static QStringList tabManoeuvresISS;

// Interface graphique
static int nbOnglets;
static QPalette paletteDefaut;
static QGraphicsScene *scene;
static QGraphicsScene *scene2;
static QGraphicsScene *scene3;
static QScrollArea *scrollArea;

static QLabel *messagesStatut;
static QLabel *messagesStatut2;
static QLabel *messagesStatut3;
static QLabel *modeFonctionnement;
static QLabel *stsDate;
static QLabel *stsHeure;

// Meteo
static QWebView *viewMeteo;

// Meteo bases NASA
static bool iEtatMeteo;
static QWebView *viewMeteoNASA;

// Live ISS
static QStringList listeChaines;

// Detail des manoeuvres
static QTableWidget *tableMan;


PreviSat::PreviSat(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::PreviSat)
{
    ui->setupUi(this);
    nbOnglets = ui->onglets->count();
    ui->onglets->setCurrentIndex(0);
    ui->ongletsOptions->setCurrentIndex(0);
    ui->ongletsOutils->setCurrentIndex(0);
    mng.setNetworkAccessible(QNetworkAccessManager::Accessible);
}

/*
 * Chargement de la configuration
 */
void PreviSat::ChargementConfig()
{
    /* Declarations des variables locales */
    QDir di;

    /* Initialisations */
    info = true;
    acalcAOS = true;
    acalcDN = true;
    aclickFicMaj = true;
    htSat = 0.;
    old = false;
    selec = -1;
    selec2 = 0;
    selec3 = 0;
    selec4 = 0;
    deltaNbOrb = 0;
    paletteDefaut = palette();
    tim = QDateTime();

    const QString repFlr = QString("flares") + QDir::separator();
    const QString repHtm = QString("html") + QDir::separator();

    listeFicLocalData << "donnees.sat" << "gestionnaireTLE_" + localePreviSat + ".gst" << repFlr + "flares.sts" <<
                         repHtm + "chaines.chnl" << repHtm + "meteo.map" << repHtm + "meteoNASA.html" << repHtm + "resultat.map" <<
                         QString("preferences") + QDir::separator() + "defaut" << "stations.sta" << "taiutc.dat" << "tdrs.sat";

    // Definition des repertoires et de la police suivant la plateforme
    dirExe = QCoreApplication::applicationDirPath();
    QFont police;

    ui->policeWCC->clear();

    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    adresseAstropedia = QCoreApplication::organizationDomain();

#if QT_VERSION >= 0x050000
    const QStringList listeGenericDir = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(),
                                                                  QStandardPaths::LocateDirectory);
    dirCommonData = listeGenericDir.at(listeGenericDir.size() - 1) + dirAstr + QDir::separator() + "data";
    dirLocalData = listeGenericDir.at(0) + dirAstr + QDir::separator() + "data";
    dirOut = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory) + dirAstr;
    dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);
    dirTle = listeGenericDir.at(0) + dirAstr + QDir::separator() + "tle";

#else
    const QString listeGenericDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    dirCommonData = listeGenericDir + QDir::separator() + "data";
    dirLocalData = listeGenericDir + QDir::separator() + "data";
    dirOut = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + dirAstr;
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    dirTle = QDir::toNativeSeparators(listeGenericDir + QDir::separator() + "tle");
#endif


#if defined (Q_OS_WIN)

    police.setFamily("MS Shell Dlg 2");
    police.setPointSize(8);
    setFont(police);

    ui->policeWCC->addItem("Lucida Console");
    ui->policeWCC->addItem("MS Shell Dlg 2");

#elif defined (Q_OS_LINUX)

#if QT_VERSION >= 0x050000
    dirOut = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(),
                                    QStandardPaths::LocateDirectory) + QCoreApplication::applicationName();
#else
    dirCommonData = QString("/usr/share") + QDir::separator() + dirAstr + QDir::separator() + "data";
    dirOut = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QDir::separator() +
            QCoreApplication::applicationName();
#endif

    police.setFamily("Sans Serif");
    police.setPointSize(7);
    setFont(police);

    ui->policeWCC->addItem("FreeSans");
    ui->policeWCC->addItem("Sans Serif");

#elif defined (Q_OS_MAC)

    dirCommonData = dirExe + QDir::separator() + "data";
    dirLocalData = dirCommonData;
#if QT_VERSION >= 0x050000
    dirOut = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(),
                                    QStandardPaths::LocateDirectory) + QCoreApplication::applicationName();
#else
    dirOut = QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QDir::separator() +
            QCoreApplication::applicationName();
#endif
    dirTle = dirExe + QDir::separator() + "tle";

    police.setFamily("Marion");
    police.setPointSize(11);
    setFont(police);

    ui->policeWCC->addItem("Lucida Grande");
    ui->policeWCC->addItem("Marion");
    adresseAstropedia = "http://astropedia.free.fr/";

#else
#endif

    if (dirTmp.trimmed().isEmpty()) {
        dirTmp = dirLocalData.mid(0, dirLocalData.lastIndexOf(QDir::separator())) + QDir::separator() + "cache";
    }

#if !defined (Q_OS_WIN)
    settings.setValue("fichier/dirHttpPrevi", adresseAstropedia + "previsat/Qt/");
    ui->grpVecteurEtat->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
    ui->grpElementsOsculateurs->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
    ui->grpSignal->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
#endif

    ui->policeWCC->setCurrentIndex(settings.value("affichage/policeWCC", 0).toInt());

    dirCoord = dirLocalData + QDir::separator() + "coordinates";
    dirMap = dirLocalData + QDir::separator() + "map";
    dirPrf = dirLocalData + QDir::separator() + "preferences";
    dirRsc = dirLocalData + QDir::separator() + "resources";
    dirSon = dirLocalData + QDir::separator() + "sound";
    dirOut = QDir::toNativeSeparators(dirOut);

    chronometre = new QTimer(this);

    /* Corps de la methode */
    DeterminationLocale();

    // Verification et creation des arborescences
    const QStringList listeDirDat(QStringList () << dirCommonData << dirLocalData);
    foreach(QString dirDat, listeDirDat) {
        di = QDir(dirDat);
        if (!di.exists()) {
            const QString message = tr("Erreur rencontrée lors de l'initialisation\n" \
                                       "Le répertoire %1 n'existe pas, veuillez réinstaller %2");
            Message::Afficher(message.arg(QDir::toNativeSeparators(dirDat)).arg(QCoreApplication::applicationName()), ERREUR);
            exit(1);
        }
    }

    const QStringList listeDir(QStringList () << dirMap << dirOut << dirPrf << dirRsc << dirSon << dirTle << dirTmp);
    foreach(QString dir, listeDir) {
        di = QDir(dir);
        if (!di.exists()) di.mkpath(dir);
    }

    // Verification de la presence des fichiers du repertoire data
    const QString repSon = QString("sound") + QDir::separator();
    const QString repStr = QString("stars") + QDir::separator();
    const QStringList ficCommonData(QStringList () << repSon + "aos-default.wav" << repSon + "los-default.wav"
                                    << repStr + "constellations.dat" << repStr + "constlabel.dat" <<  repStr + "constlines.dat"
                                    << repStr + "etoiles.dat");

    // Fichiers du repertoire common
    VerifieFichiersData(dirCommonData, ficCommonData);

    // Fichiers du repertoire local
    VerifieFichiersData(dirLocalData, listeFicLocalData);

    // Chargement de la liste des chaines video
    InitChainesVideo();

    // Chargement des fichiers sons (pour les AOS et LOS)
    InitFicSon();

    // Chargement des fichiers images de cartes du monde
    InitFicMap(false);

    const QString nomMap = (ui->listeMap->currentIndex() == 0) ? ":/resources/map.png" : ficMap.at(ui->listeMap->currentIndex()-1);
    const int w = settings.value("affichage/largeur", width()).toInt() - ui->frameListe->width() - ui->frameLat->width() - 31;
    const int h = settings.value("affichage/hauteur", height()).toInt() - ui->frameOnglets->height() - ui->frameLon->height() - 52;

    scene = new QGraphicsScene;
    scene->addPixmap(QPixmap(nomMap).scaled(w, h));
    ui->carte->setScene(scene);

    scene2 = new QGraphicsScene;
    scene2->setBackgroundBrush(QBrush(ui->frameZone->palette().background().color()));
    ui->radar->setScene(scene2);

    // Recuperation des donnees en memoire
    // TLE par defaut (lors de la premiere utilisation de PreviSat, ces chaines de caracteres sont vides)
    nom = settings.value("TLE/nom", "").toString();
    l1 = settings.value("TLE/l1", "").toString();
    l2 = settings.value("TLE/l2", "").toString();

    nbSat = settings.value("TLE/nbsat", 2).toInt();
    if (!(liste = settings.value("TLE/liste", "visual.txt#25544&20580").toString()).contains("#"))
        liste = "visual.txt#25544&20580";

    // Affichage des champs par defaut
    InitChampsDefaut();

    // Chargement des preferences
    InitFicPref(false);
    ChargementPref();

    if (settings.value("affichage/utc", false).toBool()) {
        ui->utc->setChecked(true);
    } else {
        ui->heureLegale->setChecked(true);
    }

    if (settings.value("affichage/unite", true).toBool()) {
        ui->unitesKm->setChecked(true);
    } else {
        ui->unitesMi->setChecked(true);
    }

    if (settings.value("affichage/systemeHoraire", true).toBool()) {
        ui->syst24h->setChecked(true);
    } else {
        ui->syst12h->setChecked(true);
    }

    if (settings.value("fichier/sauvegarde").toString().isEmpty()) {
        settings.setValue("fichier/sauvegarde", dirOut);
    }
    settings.setValue("fichier/path", dirExe);
    settings.setValue("fichier/version", QString(APPVERSION));
    settings.setValue("affichage/flagIntensiteVision", false);

    // Affichage au demarrage
    InitAffichageDemarrage();

    // Menus
    InitMenus(police);

    // Barre de statut
    InitBarreStatut(police);

    // Verification des mises a jour (logiciel, fichiers internes)
#if defined (Q_OS_MAC)
    settings.setValue("affichage/verifMAJ", false);
    ui->verifMAJ->setVisible(false);
#else
    if (ui->verifMAJ->isChecked()) {
        VerifMAJPreviSat();
    }
#endif

#if !defined (Q_OS_WIN)
    if (settings.value("fichier/dirHttpPreviDon", "").toString().isEmpty()) {
        dirDwn = dirTmp;
        amajDeb = true;

        QString fic("don");
        TelechargementFichier(settings.value("fichier/dirHttpPrevi", "").toString() + fic, false);
        QFile fi(dirDwn + QDir::separator() + fic);
        if (fi.exists() && fi.size() != 0) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            settings.setValue("fichier/dirHttpPreviDon", flux.readLine());
            fi.close();
        }
        amajDeb = false;
    }
#endif

    // Chargement des satellites TDRS
    InitChargementTDRS();

    // Chargement des stations
    InitChargementStations();

    // Initialisation du lieu d'observation
    AffichageLieuObs();

    // Initialisation de la gestion des fichiers de lieux d'observation
    InitFicObs(true);
    if (ui->fichiersObs->count() > 0) {
        ui->fichiersObs->setCurrentRow(0);
        ui->ajdfic->setCurrentIndex(0);
    }

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
    // Chargement des fichiers TLE
    InitFicTLE();

    // Ouverture du fichier TLE par defaut
    if (nbSat == 0) {
        tles.resize(1);
        nbSat = 1;
    } else {
        tles.resize(nbSat);
        bipSatAOS.resize(nbSat);
        bipSatLOS.resize(nbSat);
    }

    // Lecture du fichier TLE par defaut
    try {

        const QFileInfo fi(nomfic);
        if (fi.exists() && fi.size() != 0) {

            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                const QString fic = dirTmp + QDir::separator() + fi.completeBaseName();

                if (DecompressionFichierGz(nomfic, fic)) {
                    ficgz = nomfic;
                    nomfic = fic;
                } else {
                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                    throw PreviSatException(msg.arg(nomfic), WARNING);
                }
            }

            // Verification du fichier
            TLE::VerifieFichier(nomfic, true);

            // Lecture du fichier
            TLE::LectureFichier(nomfic, listeTLE, tles);

            // Mise a jour de la liste de satellites
            int i = 0;
            listeTLE.clear();
            bipSatAOS.clear();
            bipSatLOS.clear();
            QVectorIterator<TLE> it2(tles);
            while (it2.hasNext()) {
                const TLE tle = it2.next();
                if (tle.norad().trimmed().isEmpty()) {
                    tles.remove(i);
                } else {
                    listeTLE.append(tles.at(i).norad());
                    bipSatAOS.append(false);
                    bipSatLOS.append(false);
                    i++;
                }
            }
            nbSat = tles.size();

            if (nbSat == 0) {

                if (!l1.isEmpty() && !l2.isEmpty()) {
                    tles.append(TLE(nom, l1, l2));
                }

                // Ouverture du fichier TLE (pour placer dans la liste de l'interface graphique les satellites
                // contenus dans le fichier)
                AfficherListeSatellites(nomfic, listeTLE);
                l1 = "";
                l2 = "";
            } else {
                AfficherListeSatellites(nomfic, listeTLE);
            }

            // Recuperation des donnees satellites
            Satellite::LectureDonnees(listeTLE, tles, satellites);
            Satellite::initCalcul = false;

        } else {
            nbSat = 0;
            tles.clear();
            listeTLE.clear();
            bipSatAOS.clear();
            bipSatLOS.clear();
            l1 = "";
            l2 = "";
            ui->liste1->clear();
            ui->liste2->clear();
            ui->liste3->clear();
        }
    } catch (PreviSatException &e) {
        throw PreviSatException();
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
    const double ecart = Date::CalculOffsetUTC(QDateTime::currentDateTime());
    offsetUTC = (ui->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    ui->updown->setValue(sgn(offsetUTC) * ((int) (fabs(offsetUTC) * NB_MIN_PAR_JOUR + EPS_DATES)));
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

            if (fabs(dateCourante.jourJulienUTC() - lastUpdate) > ageMax ||
                    (dateCourante.jourJulienUTC() - tles.at(0).epoque().jourJulienUTC()) > ageMax) {
                MajWebTLE();
                settings.setValue("temps/lastUpdate", dateCourante.jourJulienUTC());
            }
        } else {
            messagesStatut->setText(tr("Mise à jour automatique des TLE"));
            MajWebTLE();
            settings.setValue("temps/lastUpdate", dateCourante.jourJulienUTC());
        }
    } else {
        VerifAgeTLE();
    }

    // Verification du fichier iss.3le
    const QString ficHsf = dirTmp + QDir::separator() + ISS_TRAJECTOIRE_NASA.split("/", QString::SkipEmptyParts).last();
    const QString fichier3leIss = dirTle + QDir::separator() + "iss.3le";

    if (ui->verifMAJ->isChecked()) {

        const QFile fi(fichier3leIss);
        if (fi.exists()) {

            // Lecture du fichier iss.3le
            TLE::LectureFichier3le(fichier3leIss, tab3le);
        }

        const QFile fi2(ficHsf);

        const double jj3lemoy = (tab3le.isEmpty()) ?
                    0. : 0.5 * (tab3le.first().dateDebutValidite().jourJulienUTC() + tab3le.last().dateDebutValidite().jourJulienUTC());
        if (!fi2.exists() || tab3le.isEmpty() || (dateCourante.jourJulienUTC() >= jj3lemoy)) {

            // Telechargement du fichier Human Space Flight
            amajDeb = true;
            dirDwn = dirTmp;
            TelechargementFichier(ISS_TRAJECTOIRE_NASA, false);
            amajDeb = false;
        }
    }

    // Creation du fichier iss.3le (les lignes sont verifiees avant l'ecriture du fichier)
    TLE::LectureTrajectoryData(ficHsf, fichier3leIss, tabManoeuvresISS);

    // Affichage des manoeuvres ISS
    if (!tabManoeuvresISS.isEmpty()) {
        AffichageManoeuvresISS();
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
    const int xmax = QApplication::desktop()->availableGeometry().width();
    const int ymax = QApplication::desktop()->availableGeometry().height() - messagesStatut->height() - 7;
    const int w = settings.value("affichage/largeur", 1068).toInt();
    const int h = settings.value("affichage/hauteur", 690).toInt();
    int xPrevi = qMax(w, width());
    int yPrevi = qMax(h, height());

    /* Corps de la methode */
    move(0, 0);
    ui->frameCarte->resize(ui->frameCarte->minimumSize());
    ui->frameCarte->resize(width() - ui->frameListe->width(), height() - ui->frameOnglets->height() - 26);
    ui->frameCarteListe->resize(ui->frameCarte->size());

    if (settings.value("affichage/fenetreMax", false).toBool() && xPrevi <= xmax && yPrevi <= ymax) {
        showMaximized();
    } else {
        resize(xPrevi, yPrevi);
    }

    // Redimensionnement de la fenetre si necessaire
    if (xPrevi > xmax) xPrevi = xmax;
    if (yPrevi > ymax) yPrevi = ymax;
    if (xPrevi < width() || yPrevi < height()) {

        if (xmax < minimumWidth()) setMinimumWidth(xmax);
        if (ymax < minimumHeight()) setMinimumHeight(ymax);
        resize(xPrevi, yPrevi);

        scrollArea = new QScrollArea(this);
        scrollArea->setWidget(centralWidget());
        scrollArea->setWidgetResizable(true);
        setCentralWidget(scrollArea);
    }

    if (settings.value("affichage/affMax", false).toBool()) {
        on_maximise_clicked();
    }

    // Calcul de la position des etoiles
    observateurs[0].CalculPosVit(dateCourante);
    Etoile::CalculPositionEtoiles(observateurs.at(0), etoiles);
    Constellation::CalculConstellations(observateurs.at(0), constellations);
    LigneConstellation::CalculLignesCst(etoiles, lignesCst);

    // Affichage du Wall Command Center
    InitWallCommandCenter();

    resizeEvent(NULL);
    Message::setFenetreParent(this);

    // Demarrage du temps reel
    chronometre->setInterval(200);
    connect(chronometre, SIGNAL(timeout()), this, SLOT(GestionTempsReel()));
    chronometre->start();

    CalculsAffichage();

    const QUrl urlLastNews(settings.value("fichier/dirHttpPrevi", "").toString() + "informations/last_news_" + localePreviSat + ".html");
    if (settings.value("affichage/informationsDemarrage", true).toBool() && Informations::UrlExiste(urlLastNews)) {
        on_actionInformations_triggered();
    }

    tim = QDateTime::currentDateTimeUtc();

    /* Retour */
    return;
}

/*
 * Determination de la locale
 */
QString PreviSat::DeterminationLocale()
{
    /* Declarations des variables locales */

    /* Initialisations */
    localePreviSat = QLocale::system().name().section('_', 0, 0);

    /* Corps de la methode */
    const QFile fi(QCoreApplication::applicationDirPath() + QDir::separator() + QCoreApplication::applicationName() + "_" +
                   localePreviSat + ".qm");
    if (!fi.exists() && localePreviSat != "fr") {
        localePreviSat = QLocale(QLocale::English, QLocale::UnitedStates).name().section('_', 0, 0);
    }

    /* Retour */
    return (localePreviSat);
}

PreviSat::~PreviSat()
{
    if (afficherResultats != NULL) {
        afficherResultats->close();
    }

    if (afficherMeteo != NULL) {
        afficherMeteo->close();
    }

    delete ui;
}


/*******************
 * Initialisations *
 ******************/
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

    ui->actionFermerVideo->setIcon(styleIcones->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->fermerVideo->setDefaultAction(ui->actionFermerVideo);

    ui->actionAgrandirVideo->setIcon(styleIcones->standardIcon(QStyle::SP_TitleBarNormalButton));
    ui->agrandirVideo->setDefaultAction(ui->actionAgrandirVideo);

    ui->enregistrerPref->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));

    ui->lbl_ageTLE->adjustSize();
    ui->ageTLE->move(ui->lbl_ageTLE->x() + ui->lbl_ageTLE->width() + 7, ui->ageTLE->y());

    ui->lbl_nbOrbitesSat->adjustSize();
    ui->lbl_nbOrbitesSat->resize(ui->lbl_nbOrbitesSat->width(), 16);
    ui->nbOrbitesSat->move(ui->lbl_nbOrbitesSat->x() + ui->lbl_nbOrbitesSat->width() + 2, ui->nbOrbitesSat->y());

    ui->lbl_prochainAOS->adjustSize();
    ui->lbl_prochainAOS->resize(ui->lbl_prochainAOS->width(), 16);
    ui->dateAOS->move(ui->lbl_prochainAOS->x() + ui->lbl_prochainAOS->width() + 7, ui->dateAOS->y());
    ui->dateJN->move(ui->dateAOS->x(), ui->dateJN->y());
    ui->lbl_beta->move(ui->dateAOS->x() + ui->dateAOS->width() - ui->lbl_beta->width() + 2, ui->dateJN->y());

    ui->gmt->setVisible(false);
    ui->frameCoordISS->setVisible(false);
    ui->frameCtrlVideo->setVisible(false);
    ui->lbl_chaine->setVisible(false);
    ui->chaine->setVisible(false);
    ui->chaine->setValue(settings.value("affichage/chaine", 1).toInt());
    ui->fluxVideoHtml->setVisible(false);

    ui->ajoutLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowRight));
    ui->supprLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowLeft));

    const QRegExpValidator *valLon = new QRegExpValidator(QRegExp("([0-1]?[0-7]?\\d°[0-5]?\\d'[0-5]?\\d?\"?|180°0?0'0?0\"?)"));
    ui->nvLongitude->setValidator(valLon);

    const QRegExpValidator *valLat = new QRegExpValidator(QRegExp("([0-8]?\\d°[0-5]?\\d'[0-5]?\\d?\"?|90°0?0'0?0\"?)"));
    ui->nvLatitude->setValidator(valLat);

    if (ui->unitesKm->isChecked()) {
        const QIntValidator *valAlt = new QIntValidator(-500, 8900);
        ui->nvAltitude->setValidator(valAlt);
    } else {
        const QIntValidator *valAlt = new QIntValidator(-1640, 29200);
        ui->nvAltitude->setValidator(valAlt);
    }

    ui->ciel->setVisible(false);
    ui->nord->setVisible(false);
    ui->sud->setVisible(false);
    ui->est->setVisible(false);
    ui->ouest->setVisible(false);
    ui->coordGeo1->setVisible(false);
    ui->coordGeo2->setVisible(false);
    ui->coordGeo3->setVisible(false);
    ui->coordGeo4->setVisible(false);
    ui->fluxVideo->setVisible(false);
    ui->fluxVideo->raise();

    ui->pasManuel->setVisible(false);
    ui->valManuel->setVisible(false);
    ui->dateHeure3->setVisible(false);
    ui->utcManuel->setVisible(false);
    ui->dateHeure4->setVisible(false);
    ui->utcManuel2->setVisible(false);
    ui->frameSimu->setVisible(false);
    ui->pause->setEnabled(false);

    ui->lbl_prochainAOS->setVisible(false);
    ui->dateAOS->setVisible(false);

    ui->frameInformationsSatellite->setVisible(true);
    ui->frameDonneesSat->setVisible(false);

    QPalette pal;
    const QBrush coulLabel = QBrush(QColor::fromRgb(227, 227, 227));
    pal.setBrush(ui->lbl_coordonneesSoleil->backgroundRole(), coulLabel);
    ui->lbl_coordonneesSoleil->setPalette(pal);
    ui->lbl_coordonneesLune->setPalette(pal);
    ui->rechercheDonneesSat->setChecked(settings.value("affichage/rechercheDonneesSat", false).toBool());

    ui->pasGeneration->setCurrentIndex(settings.value("previsions/pasGeneration", 5).toInt());
    ui->lieuxObservation2->setCurrentIndex(settings.value("previsions/lieuxObservation2", 0).toInt());
    ui->valHauteurSatPrev->setVisible(false);
    ui->hauteurSatPrev->setCurrentIndex(settings.value("previsions/hauteurSatPrev", 0).toInt());
    ui->valHauteurSoleilPrev->setVisible(false);
    ui->hauteurSoleilPrev->setCurrentIndex(settings.value("previsions/hauteurSoleilPrev", 1).toInt());
    ui->illuminationPrev->setChecked(settings.value("previsions/illuminationPrev", true).toBool());
    ui->magnitudeMaxPrev->setChecked(settings.value("previsions/magnitudeMaxPrev", false).toBool());
    ui->valMagnitudeMaxPrev->setVisible(ui->magnitudeMaxPrev->isChecked());
    ui->afficherPrev->setEnabled(false);

    ui->coordonnees->setVisible(false);
    ui->nouveauLieu->setVisible(false);
    ui->nouvelleCategorie->setVisible(false);
    ui->nvEw->setCurrentIndex(0);
    ui->nvNs->setCurrentIndex(0);

    const QIcon suppr(":/resources/suppr.png");
    ui->actionSupprimerCategorie->setIcon(suppr);
    ui->actionSupprimerLieu->setIcon(suppr);
    ui->actionSupprimerLieuSelec->setIcon(suppr);

    const QIcon ajout(":/resources/ajout.png");
    ui->actionCreer_une_categorie->setIcon(ajout);
    ui->creationCategorie->setIcon(ajout);
    ui->creationCategorie->setToolTip(tr("Créer une catégorie"));
    ui->actionCreer_un_nouveau_lieu->setIcon(ajout);
    ui->creationLieu->setIcon(ajout);
    ui->creationLieu->setToolTip(tr("Créer un nouveau lieu"));

    ui->actionModifier_coordonnees->setIcon(QIcon(":/resources/editer.png"));
    ui->actionAjouter_Mes_Preferes->setIcon(QIcon(":/resources/pref.png"));

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

    ui->passageApogee->setChecked(settings.value("previsions/passageApogee", true).toBool());
    ui->passageNoeuds->setChecked(settings.value("previsions/passageNoeuds", true).toBool());
    ui->passageOmbre->setChecked(settings.value("previsions/passageOmbre", true).toBool());
    ui->passageQuadrangles->setChecked(settings.value("previsions/passageQuadrangles", true).toBool());
    ui->transitionJourNuit->setChecked(settings.value("previsions/transitionJourNuit", true).toBool());
    ui->afficherEvt->setEnabled(false);

    ui->valHauteurSatTransit->setVisible(false);
    ui->manoeuvresISS->setVisible(false);
    ui->hauteurSatTransit->setCurrentIndex(settings.value("previsions/hauteurSatTransit", 1).toInt());
    ui->lieuxObservation4->setCurrentIndex(settings.value("previsions/lieuxObservation4", 0).toInt());
    ui->ageMaxTLETransit->setValue(settings.value("previsions/ageMaxTLETransit", 2.).toDouble());
    ui->elongationMaxCorps->setValue(settings.value("previsions/elongationMaxCorps", 5.).toDouble());
    ui->afficherTransit->setEnabled(false);

    ui->valHauteurSatMetOp->setVisible(false);
    ui->hauteurSatMetOp->setCurrentIndex(settings.value("previsions/hauteurSatMetOp", 2).toInt());
    ui->valHauteurSoleilMetOp->setVisible(false);
    ui->hauteurSoleilMetOp->setCurrentIndex(settings.value("previsions/hauteurSoleilMetOp", 1).toInt());
    ui->lieuxObservation5->setCurrentIndex(settings.value("previsions/lieuxObservation5", 0).toInt());
    ui->ordreChronologiqueMetOp->setChecked(settings.value("previsions/ordreChronologiqueMetOp", true).toBool());
    ui->magnitudeMaxMetOp->setValue(settings.value("previsions/magnitudeMaxMetOp", 2.).toDouble());
    if (settings.value("previsions/affichage3lignesMetOp", true).toBool()) {
        ui->affichage3lignesMetOp->setChecked(true);
    } else {
        ui->affichage1ligneMetOp->setChecked(true);
    }
    ui->afficherMetOp->setEnabled(false);
    MetOp::LectureStatutMetOp(tabStatutMetOp);

    /* Retour */
    return;
}

/*
 * Initialisation de la barre de statut
 */
void PreviSat::InitBarreStatut(const QFont &police)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    messagesStatut = new QLabel("", this);
    messagesStatut->setFont(police);
    messagesStatut->setFrameStyle(QFrame::NoFrame);
    messagesStatut->setIndent(3);
    messagesStatut->setMinimumSize(405, 0);
    messagesStatut->setToolTip(tr("Messages"));

    messagesStatut2 = new QLabel("", this);
    messagesStatut2->setFont(police);
    messagesStatut2->setFrameStyle(QFrame::NoFrame);
    messagesStatut2->setFixedWidth(140);
    messagesStatut2->setAlignment(Qt::AlignCenter);
    messagesStatut2->setVisible(false);

    messagesStatut3 = new QLabel("", this);
    messagesStatut3->setFont(police);
    messagesStatut3->setFrameStyle(QFrame::NoFrame);
    messagesStatut3->setFixedWidth(140);
    messagesStatut3->setAlignment(Qt::AlignCenter);
    messagesStatut3->setVisible(false);

    modeFonctionnement = new QLabel("", this);
    modeFonctionnement->setFont(police);
    modeFonctionnement->setFrameStyle(QFrame::NoFrame);
    modeFonctionnement->setFixedWidth(103);
    modeFonctionnement->setAlignment(Qt::AlignCenter);
    modeFonctionnement->setToolTip(tr("Mode de fonctionnement"));

    stsDate = new QLabel("", this);
    stsDate->setFont(police);
    stsDate->setFrameStyle(QFrame::NoFrame);
    stsDate->setFixedWidth(90);
    stsDate->setAlignment(Qt::AlignCenter);
    stsDate->setToolTip(tr("Date"));

    stsHeure = new QLabel("", this);
    stsHeure->setFont(police);
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

    /* Retour */
    return;
}

/*
 * Chargement de la liste des chaines video
 */
void PreviSat::InitChainesVideo() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    listeChaines.clear();

    /* Corps de la methode */
    QFile ficChnl(dirLocalData + QDir::separator() + "html" + QDir::separator() + "chaines.chnl");
    ficChnl.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&ficChnl);

    while (!flux.atEnd()) {
        listeChaines.append(flux.readLine());
    }
    ficChnl.close();
    ui->chaine->setMaximum(listeChaines.size());

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
    nomfic = settings.value("fichier/nom", QDir::toNativeSeparators(dirTle + QDir::separator() + "visual.txt")).toString();

    QStringListIterator it(liste.split("$"));
    while (it.hasNext()) {
        const QString ficTLEs = it.next();
        const QString baseFicTLE = QDir::toNativeSeparators(ficTLEs.split("#").at(0));
        if (nomfic == dirTle + QDir::separator() + baseFicTLE || nomfic == baseFicTLE) {
            listeTLE = ficTLEs.split("#").at(1).split("&");
        }
    }
    if (listeTLE.isEmpty() || listeTLE.at(0).isEmpty()) {
        listeTLE = liste.split("$").at(0).split("#").at(1).split("&");
    }

    ui->fichierAMettreAJour->setText(settings.value("fichier/fichierAMettreAJour", nomfic).toString());
    ui->fichierALire->setText(settings.value("fichier/fichierALire", "").toString());
    ui->fichierALireCreerTLE->setText(settings.value("fichier/fichierALireCreerTLE", "").toString());
    ui->nomFichierPerso->setText(settings.value("fichier/nomFichierPerso", "").toString());

    // Fichier flashs MetOp et SkyMed
    ficTLEMetOp.clear();
    const QString nomFicMetOp = settings.value("fichier/fichierTLEMetOp", dirTle + QDir::separator() +
                                               "flares-spctrk.txt").toString().trimmed();
    const QFileInfo fim(nomFicMetOp);
    if (fim.exists() && fim.size() != 0) {
        ui->fichierTLEMetOp->addItem(fim.fileName());
        ui->fichierTLEMetOp->setItemData(0, QColor(Qt::gray), Qt::BackgroundRole);
        ficTLEMetOp.append(QDir::toNativeSeparators(nomFicMetOp));
    }
    idxfm = 0;
    if (ficTLEMetOp.isEmpty()) {
        ui->fichierTLEMetOp->addItem("");
    }
    ui->fichierTLEMetOp->addItem(tr("Parcourir..."));

    ui->affichageMsgMAJ->addItem(tr("Affichage des messages informatifs"));
    ui->affichageMsgMAJ->addItem(tr("Accepter ajout/suppression de TLE"));
    ui->affichageMsgMAJ->addItem(tr("Refuser ajout/suppression de TLE"));

    /* Retour */
    return;
}

/*
 * Chargement des stations
 */
void PreviSat::InitChargementStations() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile ficSta(dirLocalData + QDir::separator() + "stations.sta");
    ficSta.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux2(&ficSta);

    int ista = 0;
    while (!flux2.atEnd()) {

        const QStringList ligne = flux2.readLine().split(" ", QString::SkipEmptyParts);

        const double lo = ligne.at(0).toDouble();
        const double la = ligne.at(1).toDouble();
        const double alt = ligne.at(2).toDouble();
        const QString nomSta = ligne.at(3);
        const QString nomlieu = ligne.at(4);
        stations.append(Observateur(nomSta, lo, la, alt));

        const QString nomStation = "%1 (%2)";
        QListWidgetItem * const elem = new QListWidgetItem(nomStation.arg(nomlieu).arg(nomSta) , ui->listeStations);
        elem->setCheckState((static_cast<Qt::CheckState> (settings.value("affichage/station" + QString::number(ista), Qt::Checked).
                                                          toUInt())) ? Qt::Checked : Qt::Unchecked);
        ista++;
    }
    ficSta.close();

    /* Retour */
    return;
}

/*
 * Chargement des satellites TDRS
 */
void PreviSat::InitChargementTDRS() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fichier(dirLocalData + QDir::separator() + "tdrs.sat");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    while (!flux.atEnd()) {
        const QString ligne = flux.readLine();
        tabTDRS.append(ligne);
    }
    fichier.close();

    /* Retour */
    return;
}

/*
 * Chargement de la liste de cartes du monde
 */
void PreviSat::InitFicMap(const bool majAff) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDir di(dirMap);
    const QStringList filtres(QStringList () << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png");

    /* Corps de la methode */
    ui->listeMap->clear();
    ui->listeMap->addItem(tr("* Défaut"));
    ui->listeMap->setCurrentIndex(0);

    const QStringList listMap = di.entryList(filtres, QDir::Files);
    if (listMap.count() == 0) {
        ui->listeMap->addItem(tr("Télécharger..."));
    } else {

        foreach(QString fic, listMap) {

            const QString file = dirMap + QDir::separator() + fic;
            ficMap.append(file);
            ui->listeMap->addItem(fic.at(0).toUpper() + fic.mid(1, fic.lastIndexOf(".")-1));
            if (settings.value("fichier/listeMap", "").toString() == file) {
                ui->listeMap->setCurrentIndex(ficMap.indexOf(file)+1);
            }
        }
        ui->listeMap->addItem(tr("Télécharger..."));
        if (ui->listeMap->currentIndex() > 0 && majAff) {
            AffichageCourbes();
        }
    }

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
    const QDir di = QDir(dirCoord);
    if (di.exists()) {

        // Nombre de fichiers contenus dans le repertoire 'coordonnees'
        const QStringList listFic = di.entryList(QDir::Files);
        if (listFic.count() == 0) {
            if (alarm) {
                Message::Afficher(tr("Erreur rencontrée lors de l'initialisation\n" \
                                     "Il n'existe aucun fichier de lieux d'observation"), WARNING);
            }
        } else {

            // Liste de fichiers de lieux d'observation
            ficObs.clear();
            foreach(QString fic, listFic) {

                try {
                    // Verification que le fichier est un fichier de lieux d'observation
                    const QString file = dirCoord + QDir::separator() + fic;
                    QFile fi(file);
                    if (fi.exists()) {

                        fi.open(QIODevice::ReadOnly | QIODevice::Text);
                        QTextStream flux(&fi);
                        while (!flux.atEnd()) {
                            const QString lieu = flux.readLine();
                            if (lieu.size() > 0) {
                                if (lieu.size() > 33) {
                                    if (lieu.at(14) != ' ' || lieu.at(28) != ' ' || lieu.at(33) != ' ') {
                                        throw PreviSatException();
                                    }

                                } else {
                                    throw PreviSatException();
                                }
                            }
                        }
                    }

                    ficObs.append(file);
                    if (fic == "preferes") {
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

            const QString ficPref = dirCoord + QDir::separator() + "preferes";
            if (ficObs.contains(ficPref)) {

                const int indObs = ficObs.indexOf(ficPref);
                ficObs.insert(0, ficPref);
                ficObs.removeAt(indObs + 1);

                ui->fichiersObs->insertItem(0, ui->fichiersObs->takeItem(indObs));
                ui->ajdfic->insertItem(0, ui->ajdfic->itemText(indObs));
                ui->ajdfic->removeItem(indObs + 1);
            }

            if (ficObs.count() == 0) {
                if (alarm) {
                    Message::Afficher(tr("Erreur rencontrée lors de l'initialisation\n" \
                                         "Il n'existe aucun fichier de lieux d'observation"), WARNING);
                }
            }
        }
    } else {
        if (alarm) {
            Message::Afficher(tr("Erreur rencontrée lors de l'initialisation\n" \
                                 "Le répertoire contenant les fichiers de lieux d'observation n'existe pas"), WARNING);
        }
    }

    /* Retour */
    return;
}

/*
 * Chargement de la liste de fichiers de preferences
 */
void PreviSat::InitFicPref(const bool majAff) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDir di(dirPrf);
    const QStringList filtres(QStringList () << "*.prf");

    /* Corps de la methode */
    ui->preferences->clear();
    ui->preferences->addItem(tr("* Défaut"));
    ficPrf.clear();
    ficPrf.append(dirPrf + QDir::separator() + "defaut");

    const QStringList listPrf = di.entryList(filtres, QDir::Files);
    if (listPrf.count() == 0) {
        ui->preferences->addItem(tr("Enregistrer sous..."));
    } else {

        foreach(QString fic, listPrf) {

            if (fic != "defaut") {
                const QString file = dirPrf + QDir::separator() + fic;
                ficPrf.append(file);
                ui->preferences->addItem(fic.at(0).toUpper() + fic.mid(1, fic.lastIndexOf(".")-1));
                if (settings.value("fichier/preferences", dirPrf + QDir::separator() + "defaut").toString() == file) {
                    ui->preferences->setCurrentIndex(ficPrf.indexOf(file));
                }
            }
        }

        ui->preferences->addItem(tr("Enregistrer sous..."));
        if (ui->preferences->currentIndex() > 0 && majAff) {
            AffichageCourbes();
        }
    }
    ui->preferences->addItem(tr("Supprimer..."));

    /* Retour */
    return;
}

/*
 * Chargement de la liste des fichiers sons
 */
void PreviSat::InitFicSon() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDir di(dirSon);
    const QStringList filtresAOS(QStringList () << "aos-*.wav");
    const QStringList filtresLOS(QStringList () << "los-*.wav");

    /* Corps de la methode */
    ui->listeSons->clear();
    ui->listeSons->addItem(tr("* Défaut"));
    ui->listeSons->setCurrentIndex(0);

    QStringList listSonsAOS = di.entryList(filtresAOS, QDir::Files);
    listSonsAOS.removeAll("aos-default.wav");
    QStringList listSonsLOS = di.entryList(filtresLOS, QDir::Files);
    listSonsLOS.removeAll("los-default.wav");

    if (listSonsAOS.count() == 0 || listSonsLOS.count() == 0) {
        ui->listeSons->addItem(tr("Télécharger..."));
    } else {

        foreach(QString fic, listSonsAOS) {

            const QString file = dirSon + QDir::separator() + fic;
            ficSonAOS.append(file);
            ui->listeSons->addItem(fic.at(4).toUpper() + fic.mid(5, fic.mid(4).lastIndexOf(".")-1));
            if (settings.value("fichier/listeSon", "").toString() == file) {
                ui->listeSons->setCurrentIndex(ficSonAOS.indexOf(file)+1);
            }
        }

        foreach(QString fic, listSonsLOS) {
            const QString file = dirSon + QDir::separator() + fic;
            ficSonLOS.append(file);
        }
        ui->listeSons->addItem(tr("Télécharger..."));
    }

    /* Retour */
    return;
}

/*
 * Chargement de la liste de fichiers TLE
 */
void PreviSat::InitFicTLE() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        bool aNomficTrouve = false;
        const QDir di(dirTle);
        const QStringList filtres(QStringList () << "*.txt" << "*.tle");

        ficTLE.clear();
        ui->listeFichiersTLE->clear();
        const QStringList listFic = di.entryList(filtres, QDir::Files);
        if (listFic.count() != 0) {

            foreach(QString fic, listFic) {

                const QString file = QDir::toNativeSeparators(dirTle + QDir::separator() + fic);
                if (TLE::VerifieFichier(file, false) > 0) {

                    ficTLE.append(file);
                    const int idx = ficTLE.indexOf(file);
                    ui->listeFichiersTLE->addItem(fic);

                    if (QDir::toNativeSeparators(nomfic) == QDir::toNativeSeparators(file)) {
                        ui->listeFichiersTLE->setCurrentIndex(idx);
                        ui->listeFichiersTLE->setItemData(idx, QColor(Qt::gray), Qt::BackgroundRole);
                        idxf = idx;
                        aNomficTrouve = true;
                    }
                }
            }
        }

        if (!aNomficTrouve) {
            const QString fic = settings.value("fichier/nom", nomfic).toString();

            if (!fic.isEmpty()) {

                const QFileInfo fi(fic);
                if (fi.exists()) {

                    ficTLE.append(fic);
                    const int idx = ficTLE.indexOf(fic);

                    ui->listeFichiersTLE->addItem(fic.mid(fic.lastIndexOf(QDir::separator()) + 1));
                    ui->listeFichiersTLE->setCurrentIndex(idx);
                    ui->listeFichiersTLE->setItemData(idx, QColor(Qt::gray), Qt::BackgroundRole);
                    idxf = idx;
                }
            }
        }

        if (ficTLE.count() == 0) {
            ui->listeFichiersTLE->addItem("");
        }

        ui->listeFichiersTLE->addItem(tr("Parcourir..."));

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Initialisation des menus
 */
void PreviSat::InitMenus(const QFont &police) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->barreMenu->setMenu(ui->menuPrincipal);
    ui->menuBar->setVisible(false);
    ui->menuAjouter_selection_dans->setFont(police);
    ui->menuContextuelCategorie->setFont(police);
    ui->menuContextuelCompteRenduMaj->setFont(police);
    ui->menuContextuelLieux->setFont(police);
    ui->menuContextuelLieuxSelec->setFont(police);
    ui->menuContextuelListe1->setFont(police);
    ui->menuContextuelListes->setFont(police);

    /* Retour */
    return;
}

/*
 * Affichage du Wall Command Center au demarrage
 */
void PreviSat::InitWallCommandCenter()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const bool affWCC = ui->mccISS->isChecked() && !satellites.isEmpty() && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE &&
            !l1.isEmpty() && !l2.isEmpty() && !ui->ciel->isVisible();

    /* Corps de la methode */
    ui->mccISS->setChecked(settings.value("affichage/mccISS", false).toBool());
    ui->frameCoordISS->move(ui->carte->pos());
    ui->frameCoordISS->setVisible(affWCC);
    ui->gmt->setVisible(affWCC);
    ui->frameLat2->setVisible(affWCC);

    ui->affBetaWCC->setChecked(settings.value("affichage/affBetaWCC", false).toBool());
    ui->affCerclesAcq->setChecked(settings.value("affichage/affCerclesAcq", true).toBool());
    ui->affNbOrbWCC->setChecked(settings.value("affichage/affNbOrbWCC", true).toBool());
    ui->affSAA_ZOE->setChecked(settings.value("affichage/affSAA_ZOE", true).toBool());
    ui->styleWCC->setChecked(settings.value("affichage/styleWCC", true).toBool());
    ui->coulGMT->setCurrentIndex(settings.value("affichage/coulGMT", 0).toInt());
    ui->coulZOE->setCurrentIndex(settings.value("affichage/coulZOE", 0).toInt());
    ui->coulCercleVisibilite->setCurrentIndex(settings.value("affichage/coulCercleVisibilite", 0).toInt());
    ui->coulEquateur->setCurrentIndex(settings.value("affichage/coulEquateur", 0).toInt());
    ui->coulTerminateur->setCurrentIndex(settings.value("affichage/coulTerminateur").toInt());
    ui->proportionsCarte->setChecked(settings.value("affichage/proportionsCarte", true).toBool());
    ui->frameCarteListe->resize(width(), ui->frameCarteListe->height());

    isEcl = false;
    on_affBetaWCC_toggled(false);
    on_affNbOrbWCC_toggled(false);

    /* Retour */
    return;
}

/*
 * Verifie la presence des fichiers du repertoire data
 */
void PreviSat::VerifieFichiersData(const QString &dirData, const QStringList &listeFicData) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QStringListIterator it(listeFicData);
    while (it.hasNext()) {

        const QFile fi(dirData + QDir::separator() + it.next());

        // Le fichier n'existe pas
        if (!fi.exists()) {
            const QString message = tr("Le fichier %1 n'existe pas, veuillez réinstaller %2");
            Message::Afficher(message.arg(fi.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
            exit(1);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            const QString message = tr("Le fichier %1 est vide, veuillez réinstaller %2");
            Message::Afficher(message.arg(fi.fileName()).arg(QCoreApplication::applicationName()), ERREUR);
            exit(1);
        }
    }

    /* Retour */
    return;
}


/**************
 * Affichages *
 *************/
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
    for(int j=0; j<nbSat; j++) {
        if (satellites.at(j).isVisible()) {
            ht = true;
        }
    }

    if (!ht) {
        for(int j=0; j<nbSat; j++) {
            bipSatAOS[j] = false;
            bipSatLOS[j] = false;
        }
    }

    // Notification sonore
    if (ui->affnotif->isChecked() && ui->tempsReel->isChecked()) {

        if (notifAOS) {
            const QString nomSonAOS = (ui->listeSons->currentIndex() == 0) ?
                        dirCommonData + QDir::separator() + "sound" + QDir::separator() + "aos-default.wav" :
                        ficSonAOS.at(ui->listeSons->currentIndex()-1);
            QSound::play(nomSonAOS);
            notifAOS = false;
        }

        if (notifLOS) {
            const QString nomSonLOS = (ui->listeSons->currentIndex() == 0) ?
                        dirCommonData + QDir::separator() + "sound" + QDir::separator() + "los-default.wav" :
                        ficSonLOS.at(ui->listeSons->currentIndex()-1);
            QSound::play(nomSonLOS);
            notifLOS = false;
        }

        if (notifFlash%2 == 1) {
            const QString nomSonFlare = dirCommonData + QDir::separator() + "sound" + QDir::separator() + "flare.wav";
            const QFile fi(nomSonFlare);
            if (fi.exists()) {
                QSound::play(nomSonFlare);
            }
            if (notifFlash == 1)
                notifFlash = 2;
            if (notifFlash == 3)
                notifFlash = 0;
        }
    }

    // Phase de la Lune
    if (ui->affphaselune->isChecked()) {

        const double ll = atan2(lune.position().y(), lune.position().x());
        const double ls = atan2(soleil.position().y(), soleil.position().x());

        double diff = (ll - ls) * RAD2DEG;
        if (diff < 0.) diff += T360;
        indLune = (int) (diff / 12.190749) + 1;
        if (indLune > 29) indLune = 1;
    } else {
        indLune = 15;
    }
    const QString src = ":/resources/lune%1.png";
    QPixmap pixlun;
    pixlun.load(src.arg(indLune, 2, 10, QChar('0')));
    pixlun = pixlun.scaled(17, 17);

    // Couleur du ciel
    QBrush bru(Qt::black);
    if (!ui->ciel->isHidden() || ui->affradar->isChecked()) {

        const double hts = soleil.hauteur() * RAD2DEG;
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
    const QColor bleuClair(173, 216, 230);
    const QPen noir(Qt::black);
    QPen crayon(Qt::white);

    /* Corps de la methode */
    QRect rectangle;
    const QString nomMap = (ui->listeMap->currentIndex() == 0) ? ":/resources/map.png" : ficMap.at(ui->listeMap->currentIndex()-1);
    scene->addPixmap(QPixmap(nomMap).scaled(ui->carte->size()));

    if (!ui->carte->isHidden()) {

        const int hcarte = ui->carte->height() - 3;
        const int lcarte = ui->carte->width() - 3;
        const int hcarte2 = qRound(hcarte * 0.5);
        const int lcarte2 = qRound(lcarte * 0.5);
        const bool mcc = ui->mccISS->isChecked() && ui->styleWCC->isChecked();

        // Affichage du filtre de la carte en mode vision nocturne
        const QBrush alphaNuit = (ui->actionVision_nocturne->isChecked()) ? QBrush(QColor::fromRgb(128, 0, 0, 128)) :
                                                                            QBrush(Qt::NoBrush);
        const QRect rectCarte(0, 0, ui->carte->width(), ui->carte->height());
        scene->setSceneRect(rectCarte);
        scene->addRect(rectCarte, QPen(Qt::NoBrush, 0), alphaNuit);

        const QRect rectRadar(0, 0, ui->radar->width(), ui->radar->height());
        scene2->setSceneRect(rectRadar);
        scene2->addRect(rectRadar, QPen(Qt::NoBrush, 0), alphaNuit);

        // Affichage de la grille de coordonnees
        if (ui->affgrille->isChecked()) {

            const QPen pen = QPen((!satellites.isEmpty() && mcc && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) ?
                                      ((ui->coulEquateur->currentIndex() == 0) ? Qt::red : Qt::white) : Qt::white);
            scene->addLine(0, hcarte2, lcarte, hcarte2, pen);
            scene->addLine(lcarte2, 0, lcarte2, hcarte, QPen(Qt::white));

            QPen stylo(Qt::lightGray);
            QList<int> tabLat, tabLon;

            if (mcc) {
                tabLat << hcarte / 12 << hcarte / 6 << (int) (hcarte * 0.25) << hcarte / 3 << (int) (hcarte / 2.4) <<
                          (int) (7. * hcarte / 12.) << (int) (hcarte / 1.5) << (int) (hcarte * 0.75) << (int) (hcarte / 1.2) <<
                          (int) (11. * hcarte / 12.);
                tabLon << lcarte / 24 << lcarte / 12 << lcarte / 8 << lcarte / 6 << (int) (lcarte / 4.8) << (int) (lcarte * 0.25) <<
                          (int) (7. * lcarte / 24.) << lcarte / 3 << (int) (3. * lcarte / 8.) << (int) (lcarte / 2.4) <<
                          (int) (11. * lcarte / 24.) << (int) (13. * lcarte / 24.) << (int) (7. * lcarte / 12.) <<
                          (int) (15. * lcarte / 24.) << (int) (lcarte / 1.5) << (int) (17. * lcarte / 24.) << (int) (lcarte * 0.75) <<
                          (int) (19. * lcarte / 24.) << (int) (lcarte / 1.2) << (int) (21. * lcarte / 24.) <<
                          (int) (11. * lcarte / 12.) << (int) (23. * lcarte / 24.);

                ui->W150->setText("-150");
                ui->W120->setText("-120");
                ui->W90->setText("-90");
                ui->W60->setText("-60");
                ui->W30->setText("-30");

                QPalette coul;
                coul.setColor(QPalette::WindowText, Qt::white);
                ui->N60->setPalette(coul);
                ui->N30->setPalette(coul);
                ui->N0->setPalette(coul);
                ui->S30->setPalette(coul);
                ui->S60->setPalette(coul);
                ui->S30->setText("-30");
                ui->S60->setText("-60");
                ui->N60->setAlignment(Qt::AlignRight);
                ui->N30->setAlignment(Qt::AlignRight);
                ui->N0->setAlignment(Qt::AlignRight);

            } else {
                tabLat << hcarte / 6 << hcarte / 3 << (int) (hcarte / 1.5) << (int) (hcarte / 1.2);
                tabLon << lcarte / 12 << lcarte / 6 << (int) (lcarte * 0.25) << lcarte / 3 << (int) (lcarte / 2.4)
                       << (int) (7. * lcarte / 12.) << (int) (lcarte / 1.5) << (int) (lcarte * 0.75) << (int) (lcarte / 1.2) <<
                          (int) (11. * lcarte / 12.);

                ui->W150->setText("150");
                ui->W120->setText("120");
                ui->W90->setText("90");
                ui->W60->setText("60");
                ui->W30->setText("30");

                QPalette coul;
                coul.setColor(QPalette::WindowText, Qt::black);
                ui->N60->setPalette(coul);
                ui->N30->setPalette(coul);
                ui->N0->setPalette(coul);
                ui->S30->setPalette(coul);
                ui->S60->setPalette(coul);
                ui->S30->setText("30");
                ui->S60->setText("60");
                ui->N60->setAlignment(Qt::AlignLeft);
                ui->N30->setAlignment(Qt::AlignLeft);
                ui->N0->setAlignment(Qt::AlignLeft);

                // Tropiques
                stylo.setStyle(Qt::DashLine);
                scene->addLine(0, 66.55 * DEG2PXVT, lcarte, 66.55 * DEG2PXVT, stylo);
                scene->addLine(0, 113.45 * DEG2PXVT, lcarte, 113.45 * DEG2PXVT, stylo);
            }

            const int dec1 = (mcc) ? 12 : 8;
            const int dec2 = (mcc) ? 9 : 5;
            ui->W150->move((int) (lcarte / 12.) - dec1, 0);
            ui->W120->move((int) (lcarte / 6.) - dec1, 0);
            ui->W90->move((int) (lcarte / 4.) - dec2, 0);
            ui->W60->move((int) (lcarte / 3.) - dec2, 0);
            ui->W30->move((int) (lcarte / 2.4) - dec2, 0);

            stylo.setStyle(Qt::SolidLine);
            for(int j=0; j<tabLat.size(); j++) {
                scene->addLine(0, tabLat.at(j), lcarte, tabLat.at(j), stylo);
            }

            for(int j=0; j<tabLon.size(); j++) {
                scene->addLine(tabLon.at(j), 0, tabLon.at(j), hcarte, stylo);
            }

            if (!ui->carte->isHidden()) {
                ui->frameLat->setVisible(true);
                ui->frameLon->setVisible(true);
                if (mcc) {
                    ui->NN->setVisible(false);
                    ui->SS->setVisible(false);
                    ui->EE->setVisible(false);
                    ui->WW->setVisible(false);
                    ui->frameLat2->setVisible(true);
                } else {
                    ui->NN->setVisible(true);
                    ui->SS->setVisible(true);
                    ui->EE->setVisible(true);
                    ui->WW->setVisible(true);
                    ui->frameLat2->setVisible(false);
                }
            }

        } else {
            ui->frameLat->setVisible(false);
            ui->frameLat2->setVisible(false);
            ui->frameLon->setVisible(false);
        }

        // Affichage de la SAA
        if (ui->affSAA->isChecked() && !ui->mccISS->isChecked()) {

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
        const int lsol = qRound((180. - soleil.longitude() * RAD2DEG) * DEG2PXHZ);
        const int bsol = qRound((90. - soleil.latitude() * RAD2DEG) * DEG2PXVT);

        if (ui->affsoleil->isChecked()) {
            if (mcc) {

                const QString iconeSoleil = ":/resources/icones/soleil.png";
                QGraphicsPixmapItem *pm = scene->addPixmap(QPixmap(iconeSoleil));
                pm->setPos(lsol - 15, bsol - 10);

            } else {
                rectangle = QRect(lsol - 7, bsol - 7, 15, 15);
                scene->addEllipse(rectangle, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
            }
        }

        // Zone d'ombre
        if (ui->affnuit->isChecked()) {

            int jmin = 0;
            int xmin = ui->carte->width() - 3;
            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, (int) (2.55 * ui->intensiteOmbre->value())));
            const QPen stylo((mcc) ? ((ui->coulTerminateur->currentIndex() == 0) ?
                                          QPen(QColor::fromRgb(102, 50, 16), 2) : QPen(Qt::darkYellow, 2)) : QPen(Qt::NoBrush, 0));

            QVector<QPoint> zone;
            zone.resize(361);
            for(int j=0; j<361; j++) {
                zone[j].setX(qRound(soleil.zone().at(j).x() * DEG2PXHZ));
                zone[j].setY(qRound(soleil.zone().at(j).y() * DEG2PXVT));

                if (soleil.latitude() < 0.) {
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

            if (fabs(soleil.latitude()) > 0.002449 * DEG2RAD) {

                zone.resize(366);
                if (soleil.latitude() < 0.) {

                    for(int j=3; j<363; j++) {
                        zone[j].setX(qRound(soleil.zone().at((j+jmin-2)%360).x() * DEG2PXHZ)+1);
                        zone[j].setY(qRound(soleil.zone().at((j+jmin-2)%360).y() * DEG2PXVT)+1);
                    }
                    zone[0] = QPoint(ui->carte->width(), -1);
                    zone[1] = QPoint(ui->carte->width(), ui->carte->height());
                    zone[2] = QPoint(ui->carte->width(), 1 + qRound(0.5 * (zone[3].y() + zone[362].y())));

                    zone[363] = QPoint(-1, 1 + qRound(0.5 * (zone[3].y() + zone[362].y())));
                    zone[364] = QPoint(-1, ui->carte->height());
                    zone[365] = QPoint(-1, -1);

                } else {

                    for(int j=2; j<362; j++) {
                        zone[j].setX(qRound(soleil.zone().at((j+jmin-2)%360).x() * DEG2PXHZ)+1);
                        zone[j].setY(qRound(soleil.zone().at((j+jmin-2)%360).y() * DEG2PXVT)+1);
                    }

                    zone[0] = QPoint(-1, -1);
                    zone[1] = QPoint(-1, 1 + qRound(0.5 * (zone[2].y() + zone[361].y())));

                    zone[362] = QPoint(ui->carte->width(), 1 + qRound(0.5 * (zone[2].y() + zone[361].y())));
                    zone[363] = QPoint(ui->carte->width(), -1);
                    zone[364] = QPoint(ui->carte->width(), ui->carte->height());
                    zone[365] = QPoint(-1, ui->carte->height());
                }

                const QPolygonF poly(zone);
                scene->addPolygon(poly, stylo, alpha);

            } else {

                QVector<QPoint> zone1;
                zone1.resize(4);
                const int x1 = qRound(qMin(soleil.zone().at(90).x(), soleil.zone().at(270).x()) * DEG2PXHZ) + 1;
                const int x2 = qRound(qMax(soleil.zone().at(90).x(), soleil.zone().at(270).x()) * DEG2PXHZ) + 1;

                zone1.resize(4);
                if (lsol > lcarte / 4 && lsol < (3 * lcarte) / 4) {

                    QVector<QPoint> zone2;

                    zone2.resize(4);

                    zone1[0] = QPoint(-1, -1);
                    zone1[1] = QPoint(x1, -1);
                    zone1[2] = QPoint(x1, ui->carte->height());
                    zone1[3] = QPoint(-1, ui->carte->height());

                    zone2[0] = QPoint(ui->carte->width() - 1, -1);
                    zone2[1] = QPoint(x2, -1);
                    zone2[2] = QPoint(x2, ui->carte->height());
                    zone2[3] = QPoint(ui->carte->width() + 1, ui->carte->height());

                    const QPolygonF poly1(zone1);
                    const QPolygonF poly2(zone2);
                    scene->addPolygon(poly1, stylo, alpha);
                    scene->addPolygon(poly2, stylo, alpha);

                } else {

                    zone1[0] = QPoint(x1, -1);
                    zone1[1] = QPoint(x1, ui->carte->height());
                    zone1[2] = QPoint(x2, ui->carte->height());
                    zone1[3] = QPoint(x2, -1);

                    const QPolygonF poly1(zone1);
                    scene->addPolygon(poly1, stylo, alpha);
                }
            }
        }

        // Affichage de la ZOE et de la SAA pour le Wall Command Center
        if (ui->affSAA_ZOE->isChecked() && ui->mccISS->isChecked() && !satellites.isEmpty() &&
                satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE && !l1.isEmpty() && !l2.isEmpty() && !ui->ciel->isVisible()) {

            // Zone Of Exclusion (ZOE)
            QGraphicsSimpleTextItem * const txtZOE = new QGraphicsSimpleTextItem("ZOE");
            const double xnZOE = 252. * DEG2PXHZ;
            const double ynZOE = 66. * DEG2PXVT;

            txtZOE->setBrush((ui->coulZOE->currentIndex() == 0) ? Qt::black : Qt::white);
            const QFont policeZOE(ui->policeWCC->currentText(), 14);
            txtZOE->setFont(policeZOE);
            const int htt = (int) txtZOE->boundingRect().height();

            txtZOE->setPos(xnZOE, ynZOE - htt);
            scene->addItem(txtZOE);

            // South Atlantic Anomaly (SAA)
            QGraphicsSimpleTextItem * const txtSAA = new QGraphicsSimpleTextItem("SAA");
            const double xnSAA = 150. * DEG2PXHZ;
            const double ynSAA = 125. * DEG2PXVT;

            txtSAA->setBrush(Qt::white);
            const QFont policeSAA(ui->policeWCC->currentText(), 11);
            txtSAA->setFont(policeSAA);
            txtSAA->setPos(xnSAA, ynSAA);
            scene->addItem(txtSAA);

            // Dessin du contour de la SAA
            QVector<QPoint> zoneSAA_ISS;
            zoneSAA_ISS.resize(16);

            for(int i=0; i<zoneSAA_ISS.size(); i++) {
                zoneSAA_ISS[i].setX(qRound((180. - tabSAA_ISS[i][0]) * DEG2PXHZ));
                zoneSAA_ISS[i].setY(qRound((90. - tabSAA_ISS[i][1]) * DEG2PXVT));
            }

            const QPolygonF poly1(zoneSAA_ISS);
            scene->addPolygon(poly1, QPen(Qt::white, (ui->styleWCC->isChecked()) ? 2 : 1));
        }

        // Lieux d'observation
        const int nbMax = (ui->affnomlieu->checkState() == Qt::Unchecked) ? 0 : ui->lieuxObservation1->count() - 1;
        for(int j=nbMax; j>=0; j--) {

            const int lobs = qRound((180. - observateurs.at(j).longitude() * RAD2DEG) * DEG2PXHZ);
            const int bobs = qRound((90. - observateurs.at(j).latitude() * RAD2DEG) * DEG2PXVT);

            scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
            scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

            if (j == 0 || ui->affnomlieu->checkState() == Qt::Checked) {

                QGraphicsSimpleTextItem * const txtObs = new QGraphicsSimpleTextItem(observateurs.at(j).nomlieu());
                const int lng = (int) txtObs->boundingRect().width();
                const int xnobs = (lobs + 4 + lng > lcarte) ? lobs - lng - 1 : lobs + 4;
                const int ynobs = (bobs + 9 > hcarte) ? bobs - 12 : bobs;

                txtObs->setBrush(Qt::white);
                txtObs->setPos(xnobs, ynobs);
                scene->addItem(txtObs);
            }
        }

        // Stations
        if (ui->mccISS->isChecked()) {

            crayon = QPen(Qt::yellow);
            for(int j=0; j<ui->listeStations->count(); j++) {

                if (ui->listeStations->item(j)->checkState() == Qt::Checked) {

                    const int lsta = qRound((180. - stations.at(j).longitude() * RAD2DEG) * DEG2PXHZ);
                    const int bsta = qRound((90. - stations.at(j).latitude() * RAD2DEG) * DEG2PXVT);

                    scene->addLine(lsta-4, bsta, lsta+4, bsta, crayon);
                    scene->addLine(lsta, bsta-4, lsta, bsta+4, crayon);

                    QGraphicsSimpleTextItem * const txtSta = new QGraphicsSimpleTextItem(stations.at(j).nomlieu());
                    const QFont policeSta(ui->policeWCC->currentText(), 10);
                    txtSta->setFont(policeSta);

                    const int lng = (int) txtSta->boundingRect().width();
                    const int xnsta = lsta - lng / 2 + 1;
                    const int ynsta = (bsta > 16) ? bsta - 16 : bsta + 3;

                    txtSta->setBrush(Qt::yellow);
                    txtSta->setPos(xnsta, ynsta);
                    scene->addItem(txtSta);

                    if (ui->affCerclesAcq->isChecked() && !satellites.isEmpty() && !satellites.at(0).isIeralt() && !l1.isEmpty() &&
                            !l2.isEmpty() && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE && !ui->ciel->isVisible()) {

                        const QPen crayon2 = (ui->styleWCC->isChecked()) ? QPen(Qt::yellow, 2) : crayon;
                        Satellite sat = satellites.at(0);
                        sat.CalculCercleAcquisition(stations.at(j));

                        double lsat1 = sat.zone().at(0).x() * DEG2PXHZ;
                        double bsat1 = sat.zone().at(0).y() * DEG2PXVT + 1;

                        for(int k=1; k<361; k++) {
                            double lsat2 = sat.zone().at(k).x() * DEG2PXHZ;
                            double bsat2 = sat.zone().at(k).y() * DEG2PXVT + 1;
                            int ils = 99999;

                            if (fabs(lsat2 - lsat1) > lcarte2) {
                                if (lsat2 < lsat1) {
                                    lsat2 += lcarte;
                                } else {
                                    lsat1 += lcarte;
                                }
                                ils = k;
                            }

                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon2);

                            if (ils == k) {
                                lsat1 -= lcarte + 1;
                                lsat2 -= lcarte + 1;
                                scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon2);
                                ils = 0;
                            }
                            lsat1 = sat.zone().at(k).x() * DEG2PXHZ;
                            bsat1 = sat.zone().at(k).y() * DEG2PXVT + 1;
                        }
                    }
                }
            }
        }

        // Affichage de la Lune
        if (ui->afflune->isChecked()) {

            if (!mcc) {
                const int llun = qRound((180. - lune.longitude() * RAD2DEG) * DEG2PXHZ);
                const int blun = qRound((90. - lune.latitude() * RAD2DEG) * DEG2PXVT);

                QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
                QTransform transform;
                transform.translate(llun, blun);
                if (ui->rotationLune->isChecked() && observateurs.at(0).latitude() < 0.) {
                    transform.rotate(180.);
                }
                transform.translate(-7, -7);
                lun->setTransform(transform);
            }
        }

        // Affichage de la trace au sol du satellite par defaut
        if (ui->afftraj->isChecked() || mcc) {
            if (nbSat > 0) {
                if (!satellites.at(0).isIeralt()) {
                    double lsat1 = satellites.at(0).traceAuSol().at(0).at(0) * DEG2PXHZ;
                    double bsat1 = satellites.at(0).traceAuSol().at(0).at(1) * DEG2PXVT;

                    int nbOrb = 0;
                    for(int j=1; j<satellites.at(0).traceAuSol().size()-1; j++) {

                        double lsat2 = satellites.at(0).traceAuSol().at(j).at(0) * DEG2PXHZ;
                        double bsat2 = satellites.at(0).traceAuSol().at(j).at(1) * DEG2PXVT;
                        int ils = 99999;

                        if (fabs(lsat2 - lsat1) > lcarte2) {
                            if (lsat2 < lsat1) {
                                lsat2 += lcarte;
                            } else {
                                lsat1 += lcarte;
                            }
                            ils = j;
                        }

                        const QLineF lig = QLineF(lsat2, bsat2, lsat1, bsat1);

                        crayon = (fabs(satellites.at(0).traceAuSol().at(j).at(2)) <= EPSDBL100) ?
                                    bleuClair : (fabs(satellites.at(0).traceAuSol().at(j).at(2) - 2.) <= EPSDBL100) ?
                                        Qt::green : crimson;

                        if (ui->mccISS->isChecked()) {

                            if (satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) {

                                // Affichage du numero d'orbite
                                if (satellites.at(0).traceAuSol().at(j).at(1) < 90. &&
                                        satellites.at(0).traceAuSol().at(j-1).at(1) > 90. && nbOrb < 3) {

                                    nbOrb++;
                                    const Date dateISS(Date(satellites.at(0).traceAuSol().at(j+1).at(3), 0., false));
                                    const int numOrb = CalculNumeroOrbiteISS(dateISS);
                                    QGraphicsSimpleTextItem * const txtOrb = new QGraphicsSimpleTextItem(QString::number(numOrb));

                                    const QFont policeOrb(PreviSat::font().family(), 10, (ui->styleWCC->isChecked()) ?
                                                              QFont::Bold : QFont::Normal);
                                    txtOrb->setFont(policeOrb);
                                    txtOrb->setBrush(Qt::white);

                                    const int lng = (int) txtOrb->boundingRect().width();
                                    const double xnorb = (lsat2 - lng < 0) ? lsat2 + lcarte - lng - 8 : lsat2 - lng;
                                    txtOrb->setPos(xnorb, hcarte2 - 18);
                                    scene->addItem(txtOrb);
                                }

                                if (ui->styleWCC->isChecked()) {

                                    if (satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) {
                                        crayon = QPen(Qt::white, 2);
                                    }

                                    // Affichage des crochets des transitions jour/nuit
                                    const double ecl = satellites.at(0).traceAuSol().at(j).at(2);
                                    if (fabs(ecl - satellites.at(0).traceAuSol().at(j+1).at(2)) > 1.5) {

                                        const double ang = fmod(-fabs(lig.angle()), T360);
                                        const double ca = cos(ang * DEG2RAD);
                                        const double sa = sin(ang * DEG2RAD);

#if defined (Q_OS_MAC)
                                        const QFont policeOmb(PreviSat::font().family(), 24);
                                        const double fact = (ecl > EPSDBL100) ? 3. : 1.;
                                        const double xnc = lsat2 - fact * ca + 10. * sa;
                                        const double ync = bsat2 - fact * sa - 10. * ca;
#else
                                        const QFont policeOmb(PreviSat::font().family(), 14);
                                        const double fact = (ecl > EPSDBL100) ? 4. : 2.;
                                        const double xnc = lsat2 - fact * ca + 14. * sa;
                                        const double ync = bsat2 - fact * sa - 13. * ca;
#endif

                                        QGraphicsSimpleTextItem * const txtOmb =
                                                new QGraphicsSimpleTextItem((ecl > EPSDBL100) ? "[" : "]");

                                        txtOmb->setFont(policeOmb);
                                        txtOmb->setBrush(Qt::white);
                                        txtOmb->setPos(xnc, ync);
                                        txtOmb->setRotation(ang);
                                        scene->addItem(txtOmb);
                                    }
                                }
                            }
                        }
                        scene->addLine(lig, crayon);

                        if (ils == j) {
                            lsat2 -= lcarte;
                            scene->addLine(lig, crayon);
                            ils = 0;
                        }
                        lsat1 = satellites.at(0).traceAuSol().at(j).at(0) * DEG2PXHZ;
                        bsat1 = satellites.at(0).traceAuSol().at(j).at(1) * DEG2PXVT;
                    }
                }
            }
        }

        // Affichage de la zone de visibilite des satellites
        QList<bool> als;
        for(int i=0; i<nbSat; i++) {
            als.append(false);
        }

        if (nbSat > 0) {

            if (ui->affvisib->isChecked() || mcc) {
                const int nbMax2 = (ui->affvisib->checkState() == Qt::PartiallyChecked) ? 1 : listeTLE.size();

                for(int isat=0; isat<nbMax2; isat++) {

                    if (mcc) {

                        crayon = QPen((ui->coulCercleVisibilite->currentIndex() == 0) ? Qt::white : Qt::darkRed, 2);
                        if (satellites.at(isat).tle().nom().toLower().startsWith("tdrs")) {

                            const int numeroTDRS = satellites.at(isat).tle().nom().section(" ", 1).toInt();

                            QStringListIterator it(tabTDRS);
                            while (it.hasNext()) {

                                const QString ligne = it.next().trimmed();
                                if (ligne.section(" ", 0, 0).toInt() == numeroTDRS) {

                                    const QString nomTDRS = ligne.section(" ", 1, 1);
                                    const int r = ligne.section(" ", 2, 2).toInt();
                                    const int v = ligne.section(" ", 3, 3).toInt();
                                    const int b = ligne.section(" ", 4, 4).toInt();

                                    // Affichage du nom du satellite TDRS
                                    QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(nomTDRS);
                                    const QFont policeSat(ui->policeWCC->currentText(), 11);
                                    txtSat->setFont(policeSat);

                                    const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ);
                                    const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT);

                                    crayon = QPen(QColor(r, v, b), 2);
                                    txtSat->setBrush(crayon.color());

                                    const int lng = (int) txtSat->boundingRect().width();
                                    const int xnsat = lsat - lng / 2 + 1;
                                    txtSat->setPos(xnsat, bsat + 19);
                                    scene->addItem(txtSat);
                                }
                            }
                        }
                    } else {
                        crayon = QPen(Qt::white);
                    }

                    if (!satellites.at(isat).isIeralt()) {

                        double lsat1 = satellites.at(isat).zone().at(0).x() * DEG2PXHZ;
                        double bsat1 = satellites.at(isat).zone().at(0).y() * DEG2PXVT + 1;

                        for(int j=1; j<361; j++) {
                            double lsat2 = satellites.at(isat).zone().at(j).x() * DEG2PXHZ;
                            double bsat2 = satellites.at(isat).zone().at(j).y() * DEG2PXVT + 1;
                            int ils = 99999;

                            if (fabs(lsat2 - lsat1) > lcarte2) {
                                if (lsat2 < lsat1) {
                                    lsat2 += lcarte;
                                } else {
                                    lsat1 += lcarte;
                                }
                                ils = j;
                                als[isat] = true;
                            }

                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                            if (ils == j) {
                                lsat1 -= lcarte + 1;
                                lsat2 -= lcarte + 1;
                                scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);
                                ils = 0;
                            }
                            lsat1 = satellites.at(isat).zone().at(j).x() * DEG2PXHZ;
                            bsat1 = satellites.at(isat).zone().at(j).y() * DEG2PXVT + 1;
                        }
                    }
                }
            }
        }

        for(int isat=nbSat-1; isat>=0; isat--) {

            // Dessin des satellites
            if (!satellites.at(isat).isIeralt()) {

                const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ);
                const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT);

                if (mcc || ui->afficone->isChecked()) {

                    // Affichage de l'icone du satellite a partir du numero NORAD
                    const QStringList listeIcones(QStringList () << dirRsc + QDir::separator() + "%1.png" << ":/resources/icones/%1.png");
                    QPixmap img;

                    QStringListIterator it(listeIcones);
                    while (it.hasNext()) {

                        const QString nomIcone = it.next();
                        const int norad = satellites.at(isat).tle().norad().toInt();

                        img = QPixmap(nomIcone.arg(norad));

                        if (img.isNull()) {

                            // Affichage de l'icone du satellite a partir du nom du satellite
                            const QString nomsatm = satellites.at(isat).tle().nom().toLower();
                            if (!(nomsatm.contains(" deb") && nomsatm.contains("r/b"))) {
                                img = QPixmap(nomIcone.arg(nomsatm.section(QRegExp("[^a-z0-9]"), 0, 0)));
                            }
                        }

                        if (!img.isNull()) {
                            it.toBack();
                        }
                    }

                    if (img.isNull()) {
                        AffichageSatellite(isat, lsat, bsat, lcarte, hcarte);
                    } else {

                        img = img.scaled(qMin(lcarte / 12, img.width()), qMin(hcarte / 6, img.height()));
                        QGraphicsPixmapItem * const pm = scene->addPixmap(img);

                        QTransform transform;
                        transform.translate(lsat, bsat);

                        double angle = 0.;
                        if (ui->rotationIconeISS->isChecked() && satellites.at(isat).tle().norad() == NORAD_STATION_SPATIALE) {
                            const double vxsat = satellites.at(isat).vitesse().x();
                            const double vysat = satellites.at(isat).vitesse().y();
                            const double vzsat = satellites.at(isat).vitesse().z();
                            angle = RAD2DEG * (-atan(vzsat / sqrt(vxsat * vxsat + vysat * vysat)));
                            transform.rotate(angle);
                        }
                        transform.translate(-img.width() / 2, -img.height() / 2);
                        pm->setTransform(transform);

                        if (als.at(isat)) {
                            QGraphicsPixmapItem * const pm2 = scene->addPixmap(img);
                            transform.reset();

                            if (lsat > lcarte2) {
                                transform.translate(lsat - lcarte, bsat);
                            } else {
                                transform.translate(lsat + lcarte, bsat);
                            }

                            transform.rotate(angle);
                            transform.translate(-img.width() / 2, -img.height() / 2);
                            pm2->setTransform(transform);
                        }
                    }
                } else {
                    AffichageSatellite(isat, lsat, bsat, lcarte, hcarte);
                }
            }
        }

        ui->carte->setScene(scene);
        QGraphicsView gview(scene);
        gview.setRenderHints(QPainter::Antialiasing);

    } else {

        // Affichage de la carte du ciel
        rectangle = QRect(2, 2, ui->ciel->width() - 4, ui->ciel->height() - 4);
        scene3->setSceneRect(rectangle);
        scene3->setBackgroundBrush(QBrush(palette().background().color()));

        const QPen pen(bru, Qt::SolidPattern);
        scene3->addEllipse(rectangle, pen, bru);
        const int lciel = qRound(0.5 * ui->ciel->width());
        const int hciel = qRound(0.5 * ui->ciel->height());

        // Affichage des constellations
        if (ui->affconst->checkState() != Qt::Unchecked) {

            QListIterator<LigneConstellation> it(lignesCst);
            while (it.hasNext()) {

                const LigneConstellation lig = it.next();
                if (lig.isDessin()) {

                    // Calcul des coordonnees radar des etoiles pour le dessin de la constellation
                    const int lstr1 = qRound(lciel - lciel * (1. - lig.etoile1().hauteur() * DEUX_SUR_PI) *
                                             sin(lig.etoile1().azimut()));
                    const int bstr1 = qRound(hciel - hciel * (1. - lig.etoile1().hauteur() * DEUX_SUR_PI) *
                                             cos(lig.etoile1().azimut()));

                    const int lstr2 = qRound(lciel - lciel * (1. - lig.etoile2().hauteur() * DEUX_SUR_PI) *
                                             sin(lig.etoile2().azimut()));
                    const int bstr2 = qRound(hciel - hciel * (1. - lig.etoile2().hauteur() * DEUX_SUR_PI) *
                                             cos(lig.etoile2().azimut()));

                    crayon = QPen((soleil.hauteur() > -0.08) ?
                                      bleuClair : (soleil.hauteur() > -0.12) ? QColor("deepskyblue") : QColor(Qt::cyan));

                    if ((lstr2 - lstr1) * (lstr2 - lstr1) + (bstr2 - bstr1) * (bstr2 - bstr1) < lciel * ui->ciel->height()) {
                        scene3->addLine(lstr1, bstr1, lstr2, bstr2, crayon);
                    }
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
                            const int lcst = qRound(lciel - lciel * (1. - cst.hauteur() * DEUX_SUR_PI) * sin(cst.azimut()));
                            const int bcst = qRound(hciel - hciel * (1. - cst.hauteur() * DEUX_SUR_PI) * cos(cst.azimut()));

                            const int lst = lcst - lciel;
                            const int bst = hciel - bcst;

                            QGraphicsSimpleTextItem * const txtCst = new QGraphicsSimpleTextItem(cst.nom());
                            const int lng = (int) txtCst->boundingRect().width();

                            const int xncst = (sqrt((lst + lng) * (lst + lng) + bst * bst) > lciel) ? lcst - lng - 1 : lcst + 1;
                            const int yncst = (bcst + 9 > ui->ciel->height()) ? bcst - 10 : bcst + 1;

                            txtCst->setBrush(QBrush(Qt::darkYellow));
                            txtCst->setPos(xncst, yncst);
                            txtCst->setFont(QFont(font().family(), 8));
                            scene3->addItem(txtCst);
                        }
                    }
                }
            }
        }

        // Affichage des etoiles
        const QBrush bru2 = (soleil.hauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);
        QListIterator<Etoile> it1(etoiles);
        while (it1.hasNext()) {

            const Etoile etoile = it1.next();
            if (etoile.isVisible() && etoile.magnitude() <= ui->magnitudeEtoiles->value()) {

                const int lstr = qRound(lciel - lciel * (1. - etoile.hauteur() * DEUX_SUR_PI) * sin(etoile.azimut()));
                const int bstr = qRound(hciel - hciel * (1. - etoile.hauteur() * DEUX_SUR_PI) * cos(etoile.azimut()));

                if (etoile.magnitude() <= 3.) {
                    rectangle = QRect(lstr-1, bstr-1, 2, 3);
                    scene3->addEllipse(rectangle, QPen(Qt::NoPen), bru2);
                } else {
                    const QLine lin(lstr, bstr, lstr, bstr);
                    scene3->addLine(lin, bru2.color());
                }

                // Nom des etoiles les plus brillantes
                if (ui->affetoiles->isChecked()) {
                    if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {
                        if (!etoile.nom().isEmpty() && etoile.nom().at(0).isUpper()) {
                            if (etoile.magnitude() < ui->magnitudeEtoiles->value() - 1.9) {

                                const int lst = lstr - lciel;
                                const int bst = hciel - bstr;
                                const QString nomstr = etoile.nom().mid(0, 1) + etoile.nom().mid(1).toLower();
                                QGraphicsSimpleTextItem * const txtStr = new QGraphicsSimpleTextItem(nomstr);
                                const int lng = (int) txtStr->boundingRect().width();

                                const int xnstr = (sqrt((lst + lng) * (lst + lng) + bst * bst) > lciel) ? lstr - lng - 1 : lstr + 1;
                                const int ynstr = (bstr + 9 > ui->ciel->height()) ? bstr - 10 : bstr + 1;

                                txtStr->setBrush(bru2);
                                txtStr->setPos(xnstr, ynstr);
                                txtStr->setFont(font());
                                txtStr->setScale(0.9);
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

                if (planetes.at(iplanete).hauteur() >= 0.) {

                    if (((iplanete == MERCURE || iplanete == VENUS) && planetes.at(iplanete).distance() > soleil.distance()) ||
                            iplanete >= MARS) {

                        const int lpla = qRound(lciel - lciel * (1. - planetes.at(iplanete).hauteur() * DEUX_SUR_PI) *
                                                sin(planetes.at(iplanete).azimut()));
                        const int bpla = qRound(hciel - hciel * (1. - planetes.at(iplanete).hauteur() * DEUX_SUR_PI) *
                                                cos(planetes.at(iplanete).azimut()));

                        const QBrush bru3(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                        rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                        scene3->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), bru3);

                        if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored &&
                                ui->affplanetes->checkState() == Qt::Checked) {
                            const int lpl = lpla - lciel;
                            const int bpl = hciel - bpla;
                            const QString nompla = planetes.at(iplanete).nom();
                            QGraphicsSimpleTextItem * const txtPla = new QGraphicsSimpleTextItem(nompla);
                            const int lng = (int) txtPla->boundingRect().width();

                            const int xnpla = (sqrt((lpl + lng) * (lpl + lng) + bpl * bpl) > lciel) ? lpla - lng - 1 : lpla + 3;
                            const int ynpla = (bpla + 9 > ui->ciel->height()) ? bpla - 10 : bpla + 2;

                            txtPla->setBrush(bru3);
                            txtPla->setPos(xnpla, ynpla);
                            txtPla->setFont(font());
                            txtPla->setScale(0.9);
                            scene3->addItem(txtPla);
                        }
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
                const Vecteur3D vec(cos(ad1) * cd1, sin(ad1) * cd1, sin(de1));
                const Vecteur3D vec1 = observateurs.at(0).rotHz() * vec;

                double ht1 = asin(vec1.z());
                double az1 = atan2(vec1.y(), -vec1.x());
                if (az1 < 0.) az1 += DEUX_PI;

                int lecl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                int becl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                for(int i=1; i<49; i++) {

                    const double ad2 = tabEcliptique[i][0] * HEUR2RAD;
                    const double de2 = tabEcliptique[i][1] * DEG2RAD;
                    const double cd2 = cos(de2);
                    const Vecteur3D vec0(cos(ad2) * cd2, sin(ad2) * cd2, sin(de2));
                    const Vecteur3D vec2 = observateurs.at(0).rotHz() * vec0;

                    const double ht2 = asin(vec2.z());

                    double az2 = atan2(vec2.y(), -vec2.x());
                    if (az2 < 0.) az2 += DEUX_PI;

                    const int lecl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                    const int becl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                    if (ht1 >= 0. || ht2 >= 0.) {
                        scene3->addLine(lecl1, becl1, lecl2, becl2, QPen(Qt::darkYellow));
                    }

                    lecl1 = lecl2;
                    becl1 = becl2;
                    ht1 = ht2;
                }
            }

            if (soleil.isVisible()) {

                // Calcul des coordonnees radar du Soleil
                const int lsol = qRound(lciel - lciel * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
                const int bsol = qRound(hciel - hciel * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

                rectangle = QRect(lsol - 7, bsol - 7, 15, 15);
                scene3->addEllipse(rectangle, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
            }
        }

        if (ui->affplanetes->checkState() != Qt::Unchecked) {

            // Calcul des coordonnees radar des planetes Mercure et Venus
            for(int iplanete=MERCURE; iplanete<=VENUS; iplanete++) {

                if (planetes.at(iplanete).hauteur() >= 0.) {

                    if (planetes.at(iplanete).distance() < soleil.distance()) {

                        const int lpla = qRound(lciel - lciel * (1. - planetes.at(iplanete).hauteur() * DEUX_SUR_PI) *
                                                sin(planetes.at(iplanete).azimut()));
                        const int bpla = qRound(hciel - hciel * (1. - planetes.at(iplanete).hauteur() * DEUX_SUR_PI) *
                                                cos(planetes.at(iplanete).azimut()));

                        const QBrush bru3(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                        rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                        scene3->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), bru3);

                        if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored &&
                                ui->affplanetes->checkState() == Qt::Checked) {
                            const int lpl = lpla - lciel;
                            const int bpl = hciel - bpla;
                            const QString nompla = planetes.at(iplanete).nom();
                            QGraphicsSimpleTextItem * const txtPla = new QGraphicsSimpleTextItem(nompla);
                            const int lng = (int) txtPla->boundingRect().width();

                            const int xnpla = (sqrt((lpl + lng) * (lpl + lng) + bpl * bpl) > lciel) ? lpla - lng - 1 : lpla + 3;
                            const int ynpla = (bpla + 9 > ui->ciel->height()) ? bpla - 10 : bpla + 2;

                            txtPla->setBrush(bru3);
                            txtPla->setPos(xnpla, ynpla);
                            txtPla->setFont(font());
                            txtPla->setScale(0.9);
                            scene3->addItem(txtPla);
                        }
                    }
                }
            }
        }

        if (ui->afflune->isChecked() && lune.isVisible()) {

            // Calcul des coordonnees radar de la Lune
            const int llun = qRound(lciel - lciel * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
            const int blun = qRound(hciel - hciel * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

            QGraphicsPixmapItem * const lun = scene3->addPixmap(pixlun);
            QTransform transform;
            transform.translate(llun, blun);
            if (ui->rotationLune->isChecked() && observateurs.at(0).latitude() < 0.) {
                transform.rotate(180.);
            }
            transform.translate(-7, -7);
            lun->setTransform(transform);
        }

        for(int isat=nbSat-1; isat>=0; isat--) {

            if (satellites.at(isat).isVisible() && !satellites.at(isat).isIeralt()) {

                // Affichage de la trace dans le ciel
                const QList<QVector<double> > trace = satellites.at(isat).traceCiel();
                if (ui->afftraceCiel->isChecked() && trace.size() > 0) {

                    const double ht1 = trace.at(0).at(0);
                    const double az1 = trace.at(0).at(1);
                    int lsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                    int bsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                    for(int i=1; i<trace.size(); i++) {

                        const double ht2 = trace.at(i).at(0);
                        const double az2 = trace.at(i).at(1);

                        crayon = (fabs(trace.at(i).at(2)) <= EPSDBL100) ?
                                    ((soleil.hauteur() > -0.08) ?
                                         bleuClair : ((soleil.hauteur() > -0.12) ? QColor("deepskyblue") : QColor("cyan"))) :
                                    (fabs(trace.at(i).at(2) - 2.) <= EPSDBL100) ? Qt::green : crimson;

                        const int lsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                        const int bsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                        scene3->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        lsat1 = lsat2;
                        bsat1 = bsat2;
                    }
                }

                // Calcul des coordonnees radar du satellite
                const int lsat = qRound(lciel - lciel * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                        sin(satellites.at(isat).azimut()));
                const int bsat = qRound(hciel - hciel * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                        cos(satellites.at(isat).azimut()));

                rectangle = QRect(lsat - 3, bsat - 3, 6, 6);
                const QColor col = (satellites.at(isat).conditionEclipse().isEclipseTotale()) ?
                            crimson : (satellites.at(isat).conditionEclipse().isEclipsePartielle() ||
                                       satellites.at(isat).conditionEclipse().isEclipseAnnulaire()) ? Qt::green : Qt::yellow;
                scene3->addEllipse(rectangle, noir, QBrush(col, Qt::SolidPattern));
            }
        }

        scene3->addEllipse(-20, -20, ui->ciel->width() + 40, ui->ciel->height() + 40,
                           QPen(QBrush(palette().background().color()), 44));
        scene3->addEllipse(1, 1, ui->ciel->width() - 3, ui->ciel->height() - 3, QPen(QBrush(Qt::gray), 3));

        ui->ciel->setScene(scene3);
        QGraphicsView view3(scene3);
        view3.setRenderHints(QPainter::Antialiasing);
    }

    // Radar
    htr = false;
    if ((ui->affradar->checkState() == Qt::Checked || (ui->affradar->checkState() == Qt::PartiallyChecked && ht)) &&
            !ui->mccISS->isChecked()) {

        ui->coordGeo1->setVisible(true);
        ui->coordGeo2->setVisible(true);
        ui->coordGeo3->setVisible(true);
        ui->coordGeo4->setVisible(true);
        ui->radar->setVisible(true);
        htr = true;

        // Dessin du fond du radar
        rectangle = QRect(2, 2, 196, 196);

        scene2->setBackgroundBrush(QBrush(ui->frameZone->palette().background().color()));
        const QPen pen(bru, Qt::SolidPattern);
        scene2->addEllipse(rectangle, pen, bru);

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
            const int lsol = qRound(100. - 100. * xf * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
            const int bsol = qRound(100. - 100. * yf * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

            rectangle = QRect(lsol - 7, bsol - 7, 15, 15);
            scene2->addEllipse(rectangle, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
        }

        if (ui->afflune->isChecked() && lune.isVisible()) {

            // Calcul des coordonnees radar de la Lune
            const int llun = qRound(100. - 100. * xf * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
            const int blun = qRound(100. - 100. * yf * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

            QGraphicsPixmapItem * const lun = scene2->addPixmap(pixlun);
            QTransform transform;
            transform.translate(llun, blun);
            if (ui->rotationLune->isChecked() && observateurs.at(0).latitude() < 0.) {
                transform.rotate(180.);
            }
            transform.translate(-7, -7);
            lun->setTransform(transform);
        }

        for(int isat=nbSat-1; isat>=0; isat--) {

            if (satellites.at(isat).isVisible() && !satellites.at(isat).isIeralt()) {

                // Affichage de la trace dans le radar
                const QList<QVector<double> > trace = satellites.at(isat).traceCiel();
                if (ui->afftraceCiel->isChecked() && trace.size() > 0) {

                    const double ht1 = trace.at(0).at(0);
                    const double az1 = trace.at(0).at(1);
                    int lsat1 = qRound(100. - 100. * xf * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                    int bsat1 = qRound(100. - 100. * yf * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                    for(int i=1; i<trace.size(); i++) {

                        const double ht2 = trace.at(i).at(0);
                        const double az2 = trace.at(i).at(1);

                        crayon = (fabs(trace.at(i).at(2)) <= EPSDBL100) ?
                                    ((soleil.hauteur() > -0.08) ?
                                         bleuClair : ((soleil.hauteur() > -0.12) ? QColor("deepskyblue") : QColor("cyan"))) :
                                    (fabs(trace.at(i).at(2) - 2.) <= EPSDBL100) ? Qt::green : crimson;

                        const int lsat2 = qRound(100. - 100. * xf * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                        const int bsat2 = qRound(100. - 100. * yf * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                        scene2->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        lsat1 = lsat2;
                        bsat1 = bsat2;
                    }
                }

                // Calcul des coordonnees radar du satellite
                const int lsat = qRound(100. - 100. * xf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                        sin(satellites.at(isat).azimut()));
                const int bsat = qRound(100. - 100. * yf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                        cos(satellites.at(isat).azimut()));

                rectangle = QRect(lsat - 3, bsat - 3, 6, 6);
                const QColor col = (satellites.at(isat).conditionEclipse().isEclipseTotale()) ?
                            crimson : (satellites.at(isat).conditionEclipse().isEclipsePartielle() ||
                                       satellites.at(isat).conditionEclipse().isEclipseAnnulaire()) ? Qt::green : Qt::yellow;
                scene2->addEllipse(rectangle, noir, QBrush(col, Qt::SolidPattern));
            }
        }

        // Cercle exterieur du radar
        scene2->addEllipse(-26, -26, 251, 251, QPen(QBrush(ui->frameZone->palette().background().color()), 56));
        scene2->addEllipse(1, 1, 197, 197, QPen(QBrush(Qt::gray), 3));
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
 * Affichage des donnees numeriques
 */
void PreviSat::AffichageDonnees()
{
    /* Declarations des variables locales */
    QString chaine, chaine2;

    /* Initialisations */
    QString unite1 = (ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");
    const QString unite2 = (ui->unitesKm->isChecked()) ? tr("km/s") : tr("nmi/s");

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
            const QString msg = "%1 %2";
            setWindowTitle(msg.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)));

            if ((ui->onglets->count() == nbOnglets) || ((ui->onglets->count() == nbOnglets-1) && (ui->liste2->count() == 0))) {
                ui->onglets->removeTab(1);
                ui->rechercheDonneesSat->setChecked(true);
                ui->rechercheDonneesSat->setVisible(false);
            }
        } else {
            ui->satellite->setVisible(true);
            if (ui->onglets->count() < nbOnglets) {
                ui->onglets->insertTab(1, ui->osculateurs, tr("Éléments osculateurs"));
                ui->rechercheDonneesSat->setVisible(true);
            }
        }

        /*
         * Affichage des donnees sur l'onglet General
         */
        QString chaineUTC = tr("UTC");
        Date date;

        if (fabs(dateCourante.offsetUTC()) > EPSDBL100) {
            QTime heur(0, 0);
            heur = heur.addSecs(qRound(fabs(dateCourante.offsetUTC()) * NB_SEC_PAR_JOUR));
            chaineUTC = chaineUTC.append((dateCourante.offsetUTC() > 0.) ? " + " : " - ").append(heur.toString("hh:mm"));
            ui->utcManuel->setText(chaineUTC);
            date = dateCourante;
        } else {
            ui->utcManuel->setText(chaineUTC);
            date = Date(dateCourante.jourJulienUTC(), 0., true);
        }

        chaine = date.ToLongDate((ui->syst12h->isChecked()) ? SYSTEME_12H : SYSTEME_24H).append("  ");
        chaine = chaine.append(chaineUTC);
        ui->dateHeure1->setText(chaine);
        ui->dateHeure2->setText(chaine);

        // Conditions d'observation
        const double ht = soleil.hauteur() * RAD2DEG;
        if (ht >= 0.) {
            chaine = tr("Jour");
        } else if (ht >= -6.) {
            chaine = tr("Crépuscule civil");
        } else if (ht >= -12.) {
            chaine = tr("Crépuscule nautique");
        } else if (ht >= -18.) {
            chaine = tr("Crépuscule astronomique");
        } else {
            chaine = tr("Nuit");
        }
        ui->conditionsObservation->setText(chaine);

        if (info && !satellites.isEmpty()) {

            // Date de lancement
            const QString dateLancement = satellites.at(0).dateLancement();
            if (dateLancement.isEmpty()) {
                deltaNbOrb = 0;

            } else {
                const int annee_lct = dateLancement.mid(0, 4).toInt();
                const int mois_lct = dateLancement.mid(5, 2).toInt();
                const double jour_lct = dateLancement.mid(8, 2).toDouble();
                date_lct = Date(annee_lct, mois_lct, jour_lct, 0.);

                // Nombre theorique d'orbites a l'epoque
                const int nbOrbTheo = (int) (satellites.at(0).tle().no() * (satellites.at(0).tle().epoque().jourJulienUTC() -
                                                                            date_lct.jourJulienUTC()));
                int resteOrb = nbOrbTheo%100000;
                resteOrb += ((satellites.at(0).tle().nbOrbites() > 50000 && resteOrb < 50000) ? 100000 : 0);
                resteOrb -= ((satellites.at(0).tle().nbOrbites() < 50000 && resteOrb > 50000) ? 100000 : 0);
                deltaNbOrb = nbOrbTheo - resteOrb;
            }
        }


        /*
         * Affichage des donnees relatives au satellite par defaut
         */
        if (!l1.isEmpty() && !l2.isEmpty()) {

            // Nom
            ui->nomsat1->setText(nom);
            const QString msg = "%1 %2 - %3";
            setWindowTitle(msg.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(nom));

            // Temps ecoule depuis l'epoque
            chaine = tr("%1 jours");
            QBrush brush;
            QPalette paletteFond;
            brush.setStyle(Qt::SolidPattern);

            // Indicateur de l'age du TLE
            if (fabs(satellites.at(0).ageTLE()) > 15.) {
                brush.setColor(Qt::red);
            } else if (fabs(satellites.at(0).ageTLE()) > 10.) {
                brush.setColor(QColor("orange"));
            } else if (fabs(satellites.at(0).ageTLE()) > 5.) {
                brush.setColor(Qt::darkYellow);
            } else {
                brush.setColor(QColor("forestgreen"));
            }

            paletteFond.setBrush(QPalette::WindowText, brush);
            ui->ageTLE->setPalette(paletteFond);
            ui->ageTLE->setText(chaine.arg(satellites.at(0).ageTLE(), 0, 'f', 2));

            // Longitude/latitude/altitude
            const QString ews = (satellites.at(0).longitude() > 0.) ? tr("Ouest") : tr("Est");
            ui->longitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).longitude()), DEGRE, 3, 0, false,
                                                           true).append(" ").append(ews));
            const QString nss = (satellites.at(0).latitude() > 0.) ? tr("Nord") : tr("Sud");
            ui->latitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).latitude()), DEGRE, 2, 0, false,
                                                          true).append(" ").append(nss));
            chaine = "%1 " + unite1;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).altitude(), 0, 'f', 1);
                ui->altitudeSat->setText(chaine);
            } else {
                chaine = chaine.arg(satellites.at(0).altitude() * MILE_PAR_KM, 0, 'f', 1);
                ui->altitudeSat->setText(chaine);
            }

            // Hauteur/azimut/distance
            ui->hauteurSat->setText(Maths::ToSexagesimal(satellites.at(0).hauteur(), DEGRE, 2, 0, true, true));
            ui->azimutSat->setText(Maths::ToSexagesimal(satellites.at(0).azimut(), DEGRE, 3, 0, false, true));
            chaine = "%1 " + unite1;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).distance(), 0, 'f', 1);
                ui->distanceSat->setText(chaine);
            } else {
                chaine = chaine.arg(satellites.at(0).distance() * MILE_PAR_KM, 0, 'f', 1);
                ui->distanceSat->setText(chaine);
            }

            // Ascension droite/declinaison/constellation
            ui->ascensionDroiteSat->setText(Maths::ToSexagesimal(satellites.at(0).ascensionDroite(), HEURE1, 2, 0,
                                                                 false, true).trimmed());
            ui->declinaisonSat->setText(Maths::ToSexagesimal(satellites.at(0).declinaison(), DEGRE, 2, 0, true,
                                                             true).trimmed());
            ui->constellationSat->setText(satellites.at(0).constellation());

            // Direction/vitesse/range rate
            ui->directionSat->setText((satellites.at(0).vitesse().z() > 0.) ? tr("Ascendant") : tr("Descendant"));
            chaine = "%1 " + unite2;
            if (ui->unitesKm->isChecked()) {
                chaine = chaine.arg(satellites.at(0).vitesse().Norme(), 0, 'f', 3);
                ui->vitesseSat->setText(chaine);

                chaine = "%1%2 " + unite2;
                chaine = chaine.arg((satellites.at(0).rangeRate() >= 0.) ? "+" : "-").
                        arg(fabs(satellites.at(0).rangeRate()), 0, 'f', 3);
                ui->rangeRate->setText(chaine);

            } else {
                chaine = chaine.arg(satellites.at(0).vitesse().Norme() * MILE_PAR_KM, 0, 'f', 3);
                ui->vitesseSat->setText(chaine);

                chaine = "%1%2 " + unite2;
                chaine = chaine.arg((satellites.at(0).rangeRate() >= 0.) ? "+" : "-").
                        arg(fabs(satellites.at(0).rangeRate() * MILE_PAR_KM), 0, 'f', 3);
                ui->rangeRate->setText(chaine);
            }

            // Magnitude/Illumination
            const double fractionilluminee = 100. * satellites.at(0).magnitude().fractionIlluminee();
            chaine2 = (satellites.at(0).conditionEclipse().luminositeEclipseLune() <
                       satellites.at(0).conditionEclipse().luminositeEclipseSoleil()) ? " " + tr("Lune") : "";
            if (satellites.at(0).magnitudeStandard() > 98.) {

                // Magnitude standard inconnue
                if (satellites.at(0).conditionEclipse().isEclipseTotale()) {
                    ui->magnitudeSat->setText(tr("Satellite en éclipse totale%1").arg(chaine2));
                } else {
                    chaine = tr("Satellite non éclipsé");
                    if (satellites.at(0).conditionEclipse().isEclipsePartielle())
                        chaine = tr("Satellite en éclipse partielle%1").arg(chaine2);
                    if (satellites.at(0).conditionEclipse().isEclipseAnnulaire())
                        chaine = tr("Satellite en éclipse annulaire%1").arg(chaine2);
                    chaine = chaine.append(" (%1%)");
                    chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                    ui->magnitudeSat->setText(chaine);
                }
            } else {

                // Satellite au dessus de l'horizon
                if (satellites.at(0).isVisible()) {

                    // Satellite eclipse
                    if (satellites.at(0).conditionEclipse().isEclipseTotale()) {
                        ui->magnitudeSat->setText(tr("Satellite en éclipse totale%1").arg(chaine2));
                    } else {
                        chaine = tr("Magnitude (Illumination) : %1%2");
                        double magnitude = satellites.at(0).magnitude().magnitude();

                        // Le satellite est un MetOp ou un SkyMed, on calcule la veritable magnitude (flash)
                        if (tabStatutMetOp.join("").contains(satellites.at(0).tle().norad())) {
                            const double mag = MetOp::CalculMagnitudeMetOp(ui->extinctionAtmospherique->isChecked(),
                                                                           ui->effetEclipsesMagnitude->isChecked(), tabStatutMetOp,
                                                                           satellites.at(0), soleil, satellites.at(0).conditionEclipse(),
                                                                           observateurs.at(0));
                            magnitude = qMin(magnitude, mag);

                            double crep = 0.;
                            if (ui->hauteurSoleilMetOp->currentIndex() <= 3) {
                                crep = -6. * ui->hauteurSoleilMetOp->currentIndex();
                            } else if (ui->hauteurSoleilMetOp->currentIndex() == 4) {
                                crep = 90.;
                            } else if (ui->hauteurSoleilMetOp->currentIndex() == 5) {
                                crep = ui->valHauteurSoleilMetOp->text().toInt();
                            } else {
                            }
                            crep *= DEG2RAD;

                            if (notifFlash == 0 && (magnitude <= ui->magnitudeMaxMetOp->value() && soleil.hauteur() <= crep)) {
                                notifFlash = 1;
                            }

                            if (notifFlash == 2 && (magnitude >= ui->magnitudeMaxMetOp->value() && soleil.hauteur() <= crep)) {
                                notifFlash = 3;
                            }
                        }

                        chaine = chaine.arg((magnitude >= 0.) ? "+" : "-").arg(fabs(magnitude), 0, 'f', 1);

                        chaine = chaine.append((" (%1%)"));
                        chaine = chaine.arg(fractionilluminee, 0, 'f', 0);

                        if (satellites.at(0).conditionEclipse().isEclipseAnnulaire() || satellites.at(0).conditionEclipse().isEclipsePartielle()) {
                            chaine = chaine.append(" %1/%2");
                            chaine = chaine.arg((chaine2.length() > 0) ? tr("L") : tr("T"));
                        }

                        if (satellites.at(0).conditionEclipse().isEclipsePartielle()) {
                            chaine = chaine.arg(tr("P"));
                        }

                        if (satellites.at(0).conditionEclipse().isEclipseAnnulaire()) {
                            chaine = chaine.arg(tr("A"));
                        }

                        ui->magnitudeSat->setText(chaine);
                    }
                } else {
                    if (satellites.at(0).conditionEclipse().isEclipseTotale()) {
                        chaine = tr("Satellite en éclipse totale%1").arg(chaine2);
                    } else {
                        chaine = tr("Satellite non éclipsé");
                        if (satellites.at(0).conditionEclipse().isEclipsePartielle()) {
                            chaine = tr("Satellite en éclipse partielle%1").arg(chaine2);
                        }

                        if (satellites.at(0).conditionEclipse().isEclipseAnnulaire()) {
                            chaine = tr("Satellite en éclipse annulaire%1").arg(chaine2);
                        }
                        chaine = chaine.append(" (%1%)");
                        chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                    }
                    ui->magnitudeSat->setText(chaine);
                }
            }

            // Nombre d'orbites du satellite
            chaine = "%1";
            ui->nbOrbitesSat->setText(chaine.arg(satellites.at(0).nbOrbites() + deltaNbOrb));

            // Calcul de la prochaine transition J/N
            if (!(isEcl && satellites.at(0).conditionEclipse().isEclipseTotale())) {
                acalcDN = true;
            }

            if (acalcDN) {
                CalculDN();
                isEcl = satellites.at(0).conditionEclipse().isEclipseTotale();
            }

            const Date dateCrt = (ui->tempsReel->isChecked()) ? Date(offsetUTC) : Date(dateCourante, offsetUTC);
            double delai = dateEcl.jourJulienUTC() - dateCrt.jourJulienUTC();
            if (delai >= -EPS_DATES && dateEcl.jourJulienUTC() < satellites.at(0).traceAuSol().last().at(3)) {

                chaine = tr("Prochain %1 :");
                ui->lbl_prochainJN->setText(chaine.arg((satellites.at(0).conditionEclipse().isEclipseTotale()) ? tr("N>J") : tr("J>N")));

                // Delai de l'evenement
                chaine = tr("%1 (dans %2).");
                const Date delaiEcl = Date(delai - 0.5 + EPS_DATES, 0.);
                const QString cDelaiEcl = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                            delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5).replace(":", tr("h").append(" ")).append(tr("min")) :
                            delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5).replace(":", tr("min").append(" ")).append(tr("s"));

                ui->dateJN->setText(chaine.arg(dateEcl.ToShortDate(FORMAT_COURT, ((ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).
                                    arg(cDelaiEcl));

                ui->lbl_prochainJN->setVisible(true);
                ui->dateJN->setVisible(true);
                ui->magnitudeSat->move(333, 78);
            } else {
                ui->lbl_prochainJN->setVisible(false);
                ui->dateJN->setVisible(false);
                ui->magnitudeSat->move(177, 93);
            }

            // Prochain AOS/LOS
            if (htSat * satellites.at(0).hauteur() <= 0.) {
                acalcAOS = true;
            }

            if (acalcAOS) {
                isAOS = CalculAOS();
                htSat = satellites.at(0).hauteur();
            }

            if (isAOS) {

                // Type d'evenement (AOS ou LOS)
                chaine = tr("Prochain %1 :");
                ui->lbl_prochainAOS->setText(chaine.arg(ctypeAOS));

                // Delai de l'evenement
                chaine = tr("%1 (dans %2). Azimut : %3");
                delai = dateAOS.jourJulienUTC() - dateCrt.jourJulienUTC();
                const Date delaiAOS = Date(delai - 0.5 + EPS_DATES, 0.);
                const QString cDelaiAOS = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                            delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5).replace(":", tr("h").append(" ")).append(tr("min")) :
                            delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5).replace(":", tr("min").append(" ")).append(tr("s"));

                ui->dateAOS->setText(chaine.arg(dateAOS.ToShortDate(FORMAT_COURT, ((ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).
                                     arg(cDelaiAOS).arg(Maths::ToSexagesimal(azimAOS, DEGRE, 3, 0, false, true).mid(0, 9)));

                ui->lbl_prochainAOS->setVisible(true);
                ui->dateAOS->adjustSize();
                ui->dateAOS->resize(ui->dateAOS->width(), 16);
                ui->dateAOS->setVisible(true);

            } else {
                ui->lbl_prochainAOS->setVisible(false);
                ui->dateAOS->setVisible(false);
            }

            // Angle beta
            chaine = tr("Beta : %1");
            ui->lbl_beta->setText(chaine.arg(Maths::ToSexagesimal(satellites.at(0).beta(), DEGRE, 2, 0, false, true).mid(0, 9)));

            ui->magnitudeSat->adjustSize();
            if (ui->magnitudeSat->x() == 177) {

                const int posMag = ui->magnitudeSat->x() + ui->magnitudeSat->width();
                ui->lbl_beta->move((posMag > 321) ? posMag + 10 : 333, ui->dateJN->y());
                if (!isAOS) {
                    ui->lbl_beta->move(ui->lbl_prochainAOS->pos());
                }

            } else {
                ui->lbl_beta->adjustSize();
                ui->lbl_beta->move(ui->dateAOS->x() + ui->dateAOS->width() - ui->lbl_beta->width() + 2, ui->dateJN->y());
            }
        }


        /*
         * Donnees relatives au Soleil
         */
        // Hauteur/azimut/distance
        ui->hauteurSoleil->setText(Maths::ToSexagesimal(soleil.hauteur(), DEGRE, 2, 0, true, true));
        ui->azimutSoleil->setText(Maths::ToSexagesimal(soleil.azimut(), DEGRE, 3, 0, false, true));
        chaine = "%1 " + tr("UA");
        chaine = chaine.arg(soleil.distanceUA(), 0, 'f', 3);
        ui->distanceSoleil->setText(chaine);

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.ascensionDroite(), HEURE1, 2, 0, false,
                                                                true).trimmed());
        ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.declinaison(), DEGRE, 2, 0, true, true).trimmed());
        ui->constellationSoleil->setText(soleil.constellation());


        /*
         * Donnees relatives a la Lune
         */
        // Hauteur/azimut/distance
        ui->hauteurLune->setText(Maths::ToSexagesimal(lune.hauteur(), DEGRE, 2, 0, true, true));
        ui->azimutLune->setText(Maths::ToSexagesimal(lune.azimut(), DEGRE, 3, 0, false, true));
        chaine = "%1 " + unite1;
        if (ui->unitesKm->isChecked()) {
            chaine = chaine.arg(lune.distance(), 0, 'f', 0);
            ui->distanceLune->setText(chaine);
        } else {
            chaine = chaine.arg(lune.distance() * MILE_PAR_KM, 0, 'f', 0);
            ui->distanceLune->setText(chaine);
        }

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteLune->setText(Maths::ToSexagesimal(lune.ascensionDroite(), HEURE1, 2, 0, false, true).trimmed());
        ui->declinaisonLune->setText(Maths::ToSexagesimal(lune.declinaison(), DEGRE, 2, 0, true, true).trimmed());
        ui->constellationLune->setText(lune.constellation());

        // Illumination/Phase
        chaine = "%1%";
        chaine = chaine.arg(lune.fractionIlluminee() * 100., 0, 'f', 0);
        ui->illuminationLune->setText(chaine);
        ui->phaseLune->setText(lune.phase());

        // Magnitude de la Lune
        chaine = "%1";
        chaine = chaine.arg(lune.magnitude(), 0, 'f', 2);
        ui->magnitudeLune->setText(chaine);

        /*
         * Affichage des donnees sur l'onglet Elements Osculateurs
         */
        ui->utcManuel2->setText(ui->utcManuel->text());
        if (!l1.isEmpty() && !l2.isEmpty()) {

            ui->nomsat2->setText(nom.trimmed());
            ui->ligne1->setText(l1);
            ui->ligne2->setText(l2);

            // Elements osculateurs
            AffichageElementsOsculateurs();

            if (info) {
                // Affichage des donnees sur l'onglet Informations satellite

                // Nom du satellite
                ui->nomsat3->setText(nom.trimmed());

                // Lignes du TLE
                ui->line1->setText(ui->ligne1->text());
                ui->line2->setText(ui->ligne2->text());

                // Numero NORAD
                ui->norad->setText(l1.mid(2, 5));

                // Designation COSPAR
                chaine = "%1-%2";
                int annee = l1.mid(9, 2).trimmed().toInt();
                annee += (annee < 57) ? AN2000 : 1900;
                ui->cospar->setText(chaine.arg(annee).arg(l1.mid(11, 6).trimmed()));

                // Epoque du TLE
                ui->epoque->setText(tles.at(0).epoque().
                                    ToShortDate(FORMAT_COURT, ((ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H)));

                // Nombre d'orbites a l'epoque
                chaine2 = "%1";
                chaine = chaine2.arg(l2.mid(63, 5).trimmed().toInt() + deltaNbOrb);
                ui->nbOrbitesEpoque->setText(chaine);

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
                if (satellites.at(0).magnitudeStandard() > 98.) {
                    ui->magnitudeStdMax->setText("?/?");
                } else {

                    // Estimation de la magnitude maximale
                    const double xval = satellites.at(0).magnitudeStandard() - 15.75 + 5. *
                            log10(1.45 * (satellites.at(0).elements().demiGrandAxe() *
                                          (1. - satellites.at(0).elements().excentricite()) - RAYON_TERRESTRE));
                    chaine = "%1%2%3/%4%5";
                    chaine = chaine.arg((satellites.at(0).magnitudeStandard() >= 0.) ? "+" : "-").
                            arg(fabs(satellites.at(0).magnitudeStandard()), 0, 'f', 1).
                            arg(satellites.at(0).methMagnitude()).arg((xval >= 0.) ? "+" : "-").
                            arg(fabs(xval), 0, 'f', 1);
                    ui->magnitudeStdMax->setText(chaine);
                }

                // Modele orbital
                chaine = (satellites.at(0).method() == 'd') ? tr("SGP4 (DS)") : tr("SGP4 (NE)");
                ui->modele->setText(chaine);

                // Dimensions du satellite
                double t1 = satellites.at(0).t1();
                double t2 = satellites.at(0).t2();
                double t3 = satellites.at(0).t3();
                double section = satellites.at(0).section();
                unite1 = tr("m");
                if (ui->unitesMi->isChecked()) {
                    t1 *= PIED_PAR_METRE;
                    t2 *= PIED_PAR_METRE;
                    t3 *= PIED_PAR_METRE;
                    section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
                    unite1 = tr("ft");
                }

                chaine2 = "%1 " + unite1;
                if (fabs(t2) < EPSDBL100 && fabs(t3) < EPSDBL100) {
                    chaine = tr("Sphérique. R=").append(chaine2.arg(t1, 0, 'f', 1));
                }
                if (fabs(t2) >= EPSDBL100 && fabs(t3) < EPSDBL100) {
                    chaine = tr("Cylindrique. L=").append(chaine2.arg(t1, 0, 'f', 1)).
                            append(tr(", R=")).append(chaine2.arg(t2, 0, 'f', 1));
                }
                if (fabs(t2) >= EPSDBL100 && fabs(t3) >= EPSDBL100) {
                    chaine = tr("Boîte.").append(" %1 x %2 x %3 ").append(unite1);
                    chaine = chaine.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1);
                }
                if (fabs(t1) < EPSDBL100) {
                    chaine = tr("Inconnues");
                }

                if (fabs(section) > EPSDBL100) {
                    chaine = chaine + " / %1 " + unite1;
                    chaine = chaine.arg(section, 0, 'f', 2);
                    ui->sq->setVisible(true);
                } else {
                    ui->sq->setVisible(false);
                }

                ui->dimensions->setText(chaine);
                ui->dimensions->adjustSize();
                ui->dimensions->setFixedHeight(16);
                ui->sq->move(ui->dimensions->x() + ui->dimensions->width() + 1, ui->sq->y());
                info = false;

                // Date de lancement
                if (satellites.at(0).dateLancement().length() > 0) {
                    ui->dateLancement->setText(date_lct.ToShortDate(FORMAT_COURT, SYSTEME_24H).left(10));
                } else {
                    ui->dateLancement->setText(tr("Non connue"));
                }

                // Categorie de l'orbite
                ui->categorieOrbite->setText((satellites.at(0).categorieOrbite().isEmpty()) ?
                                                 tr("Indéterminée") : satellites.at(0).categorieOrbite());

                // Pays ou organisation
                ui->pays->setText((satellites.at(0).pays().isEmpty()) ? tr("Indéterminé") : satellites.at(0).pays());

                // Site de lancement
                ui->siteLancement->setText((satellites.at(0).siteLancement().isEmpty()) ? tr("Inconnu") : satellites.at(0).siteLancement());
            }
        }
    }


    /*
     * Donnees ISS sur le Wall Command Center
     */
    if (!satellites.isEmpty()) {
        if (ui->mccISS->isChecked() && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE && !l1.isEmpty() && !l2.isEmpty() &&
                !ui->ciel->isVisible()) {

            // Prochaine transition J/N
            chaine = "D/N : %1";
            const double delai = dateEcl.jourJulienUTC() - dateCourante.jourJulienUTC();
            const Date delaiEcl(delai - 0.5, 0.);
            const QString cDelai = (delai >= 0.) ? delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(12, 7) : "0:00:00";
            ui->nextTransitionISS->setText(chaine.arg(cDelai));

            const int numOrb = CalculNumeroOrbiteISS(dateCourante);

            // Affichage des donnees du blackboard
            chaine = "LAT = %1";
            ui->latitudeISS->setText(chaine.arg(satellites.at(0).latitude() * RAD2DEG, 0, 'f', 1));
            chaine = "ALT = %1";
            ui->altitudeISS->setText(chaine.arg(satellites.at(0).altitude() * MILE_PAR_KM, 0, 'f', 1));
            chaine = "LON = %1";
            ui->longitudeISS->setText(chaine.arg(-satellites.at(0).longitude() * RAD2DEG, 0, 'f', 1));
            chaine = "INC = %1";
            ui->inclinaisonISS->setText(chaine.arg(satellites.at(0).elements().inclinaison() * RAD2DEG, 0, 'f', 1));
            chaine = "ORB = %1";
            ui->orbiteISS->setText(chaine.arg(numOrb));
            chaine = "BETA = %1";
            ui->betaISS->setText(chaine.arg(satellites.at(0).beta() * RAD2DEG, 0, 'f', 1));

            // Calcul et affichage du jour et de l'heure GMT
            chaine = "GMT = %1/%2:%3:%4";
            const Date date2 = Date(dateCourante.annee(), 1, 1., 0.);
            const double jourDsAnnee = dateCourante.jourJulienUTC() - date2.jourJulienUTC() + 1.;
            const int numJour = (int) jourDsAnnee;
            const int heure = (int) floor(NB_HEUR_PAR_JOUR * (jourDsAnnee - numJour) + 0.00005);
            const int min = dateCourante.minutes();
            const int sec = (int) floor(dateCourante.secondes());

            QPalette coul;
            coul.setColor(QPalette::WindowText, cgmt[ui->coulGMT->currentIndex()]);
            ui->gmt->setPalette(coul);
            ui->gmt->setText(chaine.arg(numJour, 3, 10, QChar('0')).arg(heure, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0'))
                             .arg(sec, 2, 10, QChar('0')));
            ui->frameCoordISS->setVisible(true);
            ui->gmt->setVisible(true);

        } else {
            ui->frameCoordISS->setVisible(false);
            ui->gmt->setVisible(false);
        }
    } else {
        ui->frameCoordISS->setVisible(false);
        ui->gmt->setVisible(false);
    }

    if (scene != NULL) {
        scene->deleteLater();
    }
    if (scene2 != NULL) {
        scene2->deleteLater();
    }
    if (scene3 != NULL) {
        scene3->deleteLater();
    }

    /* Retour */
    return;
}

/*
 * Affichage des elements osculateurs
 */
void PreviSat::AffichageElementsOsculateurs() const
{
    /* Declarations des variables locales */
    QString chaine, chaine2;

    /* Initialisations */
    const QString unite1 = (ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");
    const QString unite2 = (ui->unitesKm->isChecked()) ? tr("km/s") : tr("nmi/s");

    /* Corps de la methode */
    // Vecteur d'etat
    Vecteur3D position, vitesse;
    if (ui->typeRepere->currentIndex() == 0) {
        position = satellites.at(0).position();
        vitesse = satellites.at(0).vitesse();
    } else {
        satellites.at(0).CalculPosVitECEF(dateCourante, position, vitesse);
    }

    if (ui->unitesMi->isChecked()) {
        position = position * MILE_PAR_KM;
        vitesse = vitesse * MILE_PAR_KM;
    }

    // Position
    chaine2 = "%1%2 " + unite1;
    chaine = chaine2.arg((position.x() >= 0.) ? "+" : "-").arg(fabs(position.x()), 0, 'f', 3);
    ui->xsat->setText(chaine);
    chaine = chaine2.arg((position.y() >= 0.) ? "+" : "-").arg(fabs(position.y()), 0, 'f', 3);
    ui->ysat->setText(chaine);
    chaine = chaine2.arg((position.z() >= 0.) ? "+" : "-").arg(fabs(position.z()), 0, 'f', 3);
    ui->zsat->setText(chaine);

    // Vitesse
    chaine2 = "%1%2 " + unite2;
    chaine = chaine2.arg((vitesse.x() >= 0.) ? "+" : "-").arg(fabs(vitesse.x()), 0, 'f', 6);
    ui->vxsat->setText(chaine);
    chaine = chaine2.arg((vitesse.y() >= 0.) ? "+" : "-").arg(fabs(vitesse.y()), 0, 'f', 6);
    ui->vysat->setText(chaine);
    chaine = chaine2.arg((vitesse.z() >= 0.) ? "+" : "-").arg(fabs(vitesse.z()), 0, 'f', 6);
    ui->vzsat->setText(chaine);

    chaine = "%1 " + unite1;
    double xval = satellites.at(0).elements().demiGrandAxe() * ((ui->unitesKm->isChecked()) ? 1. : MILE_PAR_KM);

    chaine = chaine.arg(xval, 0, 'f', 1);
    ui->demiGrandAxe->setText(chaine);

    ui->frameCirculaire->setVisible(false);
    ui->frameCirculaireEquatorial->setVisible(false);
    ui->frameEquatorial->setVisible(false);
    ui->frameKeplerien->setVisible(false);

    switch (ui->typeParametres->currentIndex()) {
    case 0:
        // Parametres kepleriens
        ui->frameKeplerien->setVisible(true);

        chaine = "%1";
        chaine = chaine.arg(satellites.at(0).elements().excentricite(), 0, 'f', 7);
        ui->excentricite->setText(chaine);

        chaine2 = "%1°";
        chaine = chaine2.arg(satellites.at(0).elements().inclinaison() * RAD2DEG, 0, 'f', 4);
        ui->inclinaison->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().ascensionDroiteNA() * RAD2DEG, 0, 'f', 4);
        ui->ADNoeudAscendant->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().argumentPerigee() * RAD2DEG, 0, 'f', 4);
        ui->argumentPerigee->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().anomalieMoyenne() * RAD2DEG, 0, 'f', 4);
        ui->anomalieMoyenne->setText(chaine);
        break;

    case 1:
        // Parametres circulaires
        ui->frameCirculaire->setVisible(true);

        chaine2 = "%1";
        chaine = chaine2.arg(satellites.at(0).elements().exCirc(), 0, 'f', 7);
        ui->ex1->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().eyCirc(), 0, 'f', 7);
        ui->ey1->setText(chaine);

        chaine2 = "%1°";
        chaine = chaine2.arg(satellites.at(0).elements().inclinaison() * RAD2DEG, 0, 'f', 4);
        ui->inclinaison2->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().ascensionDroiteNA() * RAD2DEG, 0, 'f', 4);
        ui->ADNoeudAscendant2->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().pso() * RAD2DEG, 0, 'f', 4);
        ui->positionSurOrbite->setText(chaine);
        break;

    case 2:
        // Parametres equatoriaux
        ui->frameEquatorial->setVisible(true);

        chaine2 = "%1";
        chaine = chaine2.arg(satellites.at(0).elements().excentricite(), 0, 'f', 7);
        ui->excentricite2->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().ix(), 0, 'f', 7);
        ui->ix1->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().iy(), 0, 'f', 7);
        ui->iy1->setText(chaine);

        chaine2 = "%1°";
        chaine = chaine2.arg((satellites.at(0).elements().ascensionDroiteNA() + satellites.at(0).elements().
                              argumentPerigee()) * RAD2DEG, 0, 'f', 4);
        ui->longitudePerigee->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().anomalieMoyenne() * RAD2DEG, 0, 'f', 4);
        ui->anomalieMoyenne2->setText(chaine);
        break;

    case 3:
        // Parametres circulaires equatoriaux
        ui->frameCirculaireEquatorial->setVisible(true);

        chaine2 = "%1";
        chaine = chaine2.arg(satellites.at(0).elements().exCEq(), 0, 'f', 7);
        ui->ex2->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().eyCEq(), 0, 'f', 7);
        ui->ey2->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().ix(), 0, 'f', 7);
        ui->ix2->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).elements().iy(), 0, 'f', 7);
        ui->iy2->setText(chaine);

        chaine2 = "%1°";
        chaine = chaine2.arg(satellites.at(0).elements().argumentLongitudeVraie() * RAD2DEG, 0, 'f', 4);
        ui->argumentLongitudeVraie2->setText(chaine);
        break;

    default:
        break;
    }

    chaine = chaine2.arg(satellites.at(0).elements().anomalieVraie() * RAD2DEG, 0, 'f', 4);
    ui->anomalieVraie->setText(chaine);

    chaine = chaine2.arg(satellites.at(0).elements().anomalieExcentrique() * RAD2DEG, 0, 'f', 4);
    ui->anomalieExcentrique->setText(chaine);

    chaine = "±" + chaine2.arg(acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellites.at(0).altitude())) *
                               RAD2DEG, 0, 'f', 2);
    ui->champDeVue->setText(chaine);

    // Apogee/perigee/periode orbitale
    chaine2 = "%2 %1 (%3 %1)";
    chaine2 = chaine2.arg(unite1);
    const double ap = satellites.at(0).elements().apogee() - RAYON_TERRESTRE;
    xval = (ui->unitesKm->isChecked()) ? satellites.at(0).elements().apogee() : satellites.at(0).elements().apogee() * MILE_PAR_KM;
    double xval2 = (ui->unitesKm->isChecked()) ? ap : ap * MILE_PAR_KM;
    chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval2, 0, 'f', 1);
    ui->apogee->setText(chaine);

    const double per = satellites.at(0).elements().perigee() - RAYON_TERRESTRE;
    xval = (ui->unitesKm->isChecked()) ? satellites.at(0).elements().perigee() : satellites.at(0).elements().perigee() * MILE_PAR_KM;
    xval2 = (ui->unitesKm->isChecked()) ? per : per * MILE_PAR_KM;
    chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval2, 0, 'f', 1);
    ui->perigee->setText(chaine);

    ui->periode->setText(Maths::ToSexagesimal(satellites.at(0).elements().periode() * HEUR2RAD, HEURE1, 1, 0, false, true));

    // Proprietes du signal (Doppler@100MHz, attenuation@100MHz, delai en millisecondes)
    chaine = "%1";
    ui->doppler->setText(((satellites.at(0).signal().doppler() >= 0.) ? "+" : "-") +
                         chaine.arg(fabs(satellites.at(0).signal().doppler()), 0, 'f', 0) + " Hz");
    ui->attenuation->setText(chaine.arg(satellites.at(0).signal().attenuation(), 0, 'f', 2) + " dB");
    ui->delai->setText(chaine.arg(satellites.at(0).signal().delai(), 0, 'f', 2) + " ms");

    // Triplet de phasage
    chaine = "[ %1; %2; %3 ] %4";
    const int nu0 = satellites.at(0).phasage().nu0();
    const int dt0 = satellites.at(0).phasage().dt0();
    const int ct0 = satellites.at(0).phasage().ct0();
    const int nbOrb = satellites.at(0).phasage().nbOrb();
    if (nu0 < -98 || dt0 < -98 || ct0 < -98 || nbOrb < -98) {
        ui->phasage->setText(tr("N/A"));
    } else {
        ui->phasage->setText(chaine.arg(nu0).arg(dt0).arg(ct0).arg(nbOrb));
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
    QFile fi(dirLocalData + QDir::separator() + "gestionnaireTLE_" + localePreviSat + ".gst");
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    while (!flux.atEnd()) {
        const QStringList ligne = flux.readLine().split("#");
        QString nomGroupe = ligne.at(0);
        nomGroupe[0] = nomGroupe[0].toUpper();
        ui->groupeTLE->addItem(nomGroupe);

#if !defined (Q_OS_MAC)
        if (settings.value("TLE/" + nomGroupe, 0).toInt() == 1)
            listeGroupeMaj.append(ligne.at(0) + "#" + ligne.at(2));
#endif
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
    listeObs = settings.value("observateur/lieu", "Paris #-002.348640000&+48.853390000&30").toString().split("$");

    /* Corps de la methode */
    ui->lieuxObservation1->clear();
    ui->lieuxObservation2->clear();
    ui->lieuxObservation4->clear();
    ui->lieuxObservation5->clear();
    ui->selecLieux->clear();

    observateurs.clear();
    QStringListIterator it(listeObs);
    while (it.hasNext()) {

        const QString obs = it.next();
        const int delim = obs.indexOf("#");
        const QString nomlieu = obs.mid(0, delim).trimmed();

        ui->lieuxObservation1->addItem(nomlieu);
        ui->lieuxObservation2->addItem(nomlieu);
        ui->lieuxObservation4->addItem(nomlieu);
        ui->lieuxObservation5->addItem(nomlieu);
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

            // Affichage des coordonnees
            const QString fmt = "%1 %2";
            ui->longitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(lo) * DEG2RAD, DEGRE, 3, 0, false, true)).arg(ew));
            ui->latitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(la) * DEG2RAD, DEGRE, 2, 0,false, true)).arg(ns));
            ui->altitudeObs->setText(fmt.arg((ui->unitesKm->isChecked()) ? atd : qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd))).
                                     arg((ui->unitesKm->isChecked()) ? tr("m") : tr("ft")));
        }
    }
    ui->lieuxObservation1->setCurrentIndex(0);
    ui->lieuxObservation2->setCurrentIndex(0);
    ui->lieuxObservation4->setCurrentIndex(0);
    ui->lieuxObservation5->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Affichage des manoeuvres ISS
 */
void PreviSat::AffichageManoeuvresISS() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->manoeuvresISS->setRowCount(0);
    ui->manoeuvresISS->setVisible(true);
#if !defined (Q_OS_LINUX)
    QFont fnt;
    fnt.setPointSize(8);
    ui->manoeuvresISS->setFont(fnt);
#endif

    /* Corps de la methode */
    for(int i=0; i<tabManoeuvresISS.count(); i++) {

        const QStringList man = tabManoeuvresISS.at(i).split(" ", QString::SkipEmptyParts);
        ui->manoeuvresISS->insertRow(i);
        ui->manoeuvresISS->setRowHeight(i, 16);

        for(int k=0; k<man.count(); k++) {

            QString elem = man.at(k);
            QTableWidgetItem * const item = new QTableWidgetItem();

            // Date
            if (k == 0) {
                elem = Date(elem.toDouble(), 0.).ToShortDateAMJ(FORMAT_COURT, (ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H).trimmed();
                item->setToolTip("UTC");
            }

            // Masse
            if (k == 2) {
                if (ui->unitesKm->isChecked()) {
                    elem = QString("%1").arg(elem.toDouble() * KG_PAR_LIVRE, 0, 'f', 1);
                    item->setToolTip(tr("kg"));
                } else {
                    item->setToolTip(tr("lb"));
                }
            }

            // Apogee, perigee
            if (k == 3 || k == 4) {
                if (ui->unitesKm->isChecked()) {
                    elem = QString("%1").arg(elem.toDouble() / MILE_PAR_KM, 0, 'f', 1);
                    item->setToolTip(tr("km"));
                } else {
                    item->setToolTip(tr("nmi"));
                }
            }

            // DeltaV
            if (k == 5) {
                if (fabs(elem.toDouble()) < EPSDBL100) {
                    elem = "-";
                } else {
                    if (ui->unitesKm->isChecked()) {
                        elem = QString("%1").arg(elem.toDouble() / PIED_PAR_METRE, 0, 'f', 2);
                        item->setToolTip(tr("m/s"));
                    } else {
                        item->setToolTip(tr("fps"));
                    }
                }
            }

            // Duree
            if (k == 6) {
                if (fabs(elem.toDouble()) < EPSDBL100) {
                    elem = "-";
                } else {
                    item->setToolTip(tr("s"));
                }
            }

            item->setText(elem);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->manoeuvresISS->setItem(i, k, item);
            if (k < 6 || !ui->manoeuvresISS->verticalScrollBar()->isVisible()) {
                ui->manoeuvresISS->resizeColumnToContents(k);
            }
        }
    }

    ui->manoeuvresISS->setStyleSheet("QHeaderView::section { background-color:rgb(235, 235, 235) }");
    ui->manoeuvresISS->horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
    ui->manoeuvresISS->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    ui->manoeuvresISS->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    ui->manoeuvresISS->setAlternatingRowColors(true);

    /* Retour */
    return;
}

/*
 * Affichage d'un satellite sur la carte du monde
 */
void PreviSat::AffichageSatellite(const int isat, const int lsat, const int bsat, const int lcarte, const int hcarte) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Dessin du satellite
    const QColor crimson(220, 20, 60);
    const QPen noir(Qt::black);
    const QRect rectangle = QRect(lsat - 3, bsat - 3, 6, 6);
    const QColor col = (satellites.at(isat).conditionEclipse().isEclipseTotale()) ?
                crimson : (satellites.at(isat).conditionEclipse().isEclipsePartielle() ||
                           satellites.at(isat).conditionEclipse().isEclipseAnnulaire()) ? Qt::green : Qt::yellow;
    scene->addEllipse(rectangle, noir, QBrush(col, Qt::SolidPattern));

    // Nom des satellites
    if (ui->affnomsat->isChecked()) {

        if ((ui->affnomsat->checkState() == Qt::PartiallyChecked && isat == 0) ||
                ui->affnomsat->checkState() == Qt::Checked) {
            QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(tles.at(isat).nom());
            const int lng = (int) txtSat->boundingRect().width();
            const int xnsat = (lsat + 4 + lng > lcarte) ? lsat - lng - 1 : lsat + 4;
            const int ynsat = (bsat + 9 > hcarte) ? bsat - 12 : bsat;

            txtSat->setBrush(Qt::white);
            txtSat->setPos(xnsat, ynsat);
            scene->addItem(txtSat);
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage des noms des satellites dans les listes
 */
void PreviSat::AfficherListeSatellites(const QString &fichier, const QStringList &listeSat, const bool aMajListesSecondaires) const
{
    /* Declarations des variables locales */
    QString magn;

    /* Initialisations */
    nbSat = 0;
    QString nomsat = "---";
    QFile donneesSatellites(dirLocalData + QDir::separator() + "donnees.sat");
    if (donneesSatellites.exists() && donneesSatellites.size() != 0) {
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
    if (aMajListesSecondaires) {
        ui->liste2->clear();
        ui->liste3->clear();
    }
    QFile fichierTLE(fichier);
    if (fichierTLE.exists() && fichierTLE.size() != 0) {

        QString li1, li2;
        fichierTLE.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichierTLE);

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();

            if (ligne.mid(0, 2) == "1 ") {

                li1 = ligne;

                do {
                    li2 = flux.readLine();
                } while (li2.trimmed().length() == 0);

                const QString norad = li1.mid(2, 5);
                if (nomsat.mid(0, 2) == "1 " || nomsat == "---" || nomsat.isEmpty()) {

                    const int indx1 = magn.indexOf("\n" + norad) + 1;
                    if (indx1 > 0) {
                        const int indx2 = magn.indexOf("\n", indx1) - indx1;
                        nomsat = magn.mid(indx1 + 123, indx2 - 123).trimmed();
                    } else {
                        nomsat = norad;
                    }
                }

                if (nomsat.length() > 25 && nomsat.mid(25).contains(".")) {
                    nomsat = nomsat.mid(0, 15).trimmed();
                }

                if (nomsat.mid(0, 2) == "0 ") {
                    nomsat = nomsat.mid(2);
                }

                if (nomsat.toLower() == "iss (zarya)") {
                    nomsat = "ISS ";
                }

                if (nomsat.toLower().contains("iridium") && nomsat.contains("[")) {
                    nomsat = nomsat.mid(0, nomsat.indexOf("[")).trimmed();
                }

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
                QListWidgetItem * const elem1 = new QListWidgetItem(nomsat2, ui->liste1);
                elem1->setFlags(Qt::ItemIsEnabled);
                elem1->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                if (aMajListesSecondaires) {
                    QListWidgetItem * const elem2 = new QListWidgetItem(nomsat2, ui->liste2);
                    elem2->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                    QListWidgetItem * const elem3 = new QListWidgetItem(nomsat2, ui->liste3);
                    elem3->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
                }
            }
            nomsat = ligne.trimmed();
        }

        if (l1.length() > 0) {
            li1 = l1;
            li2 = l2;

            int ind0 = -1;
            if (!listeSat.isEmpty()) {
                for(int i=0; i<ui->liste1->count(); i++) {
                    const QString norad = ui->liste1->item(i)->text().split("#").at(1);
                    if (norad == listeSat.at(0)) {
                        ind0 = i;
                        break;
                    }
                }
            }

            ui->liste1->setCurrentRow(ind0);
            if (aMajListesSecondaires) {
                ui->liste2->setCurrentRow(ind0);
                ui->liste3->setCurrentRow(ind0);
            }
            l1 = li1;
            l2 = li2;
        }
        fichierTLE.close();
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
 * Calcul du prochain AOS/LOS
 */
bool PreviSat::CalculAOS() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool res = false;
    Satellite satellite = satellites.at(0);

    /* Corps de la methode */
    dateAOS = Date(satellite.CalculDateAOSSuiv(dateCourante, observateurs.at(0), ctypeAOS, azimAOS, res), offsetUTC);
    acalcAOS = false;

    /* Retour */
    return (res);
}

/*
 * Calcul de l'age du TLE de l'ISS pour le calcul des transits
 */
void PreviSat::CalculAgeTLETransitISS() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fichier = dirTle + QDir::separator() + "iss.3le";
    const int nbt = TLE::VerifieFichier(fichier, false);

    /* Corps de la methode */
    if (nbt > 0) {

        // Calcul de l'age du TLE de l'ISS pour l'onglet Transits ISS
        QVector<TLE> tleISS;
        TLE::LectureFichier3le(fichier, tleISS);
        const double jjcour = dateCourante.jourJulienUTC();
        const double age1 = tleISS.first().dateDebutValidite().jourJulienUTC() - jjcour;
        const double age2 = jjcour - tleISS.last().dateDebutValidite().jourJulienUTC();
        const double ageISS = qMax(age1, age2);

        if (ageISS > 0.) {

            const QString chaine = tr("%1 jours");
            QBrush brush;
            QPalette paletteTLE;
            brush.setStyle(Qt::SolidPattern);

            // Indicateur de l'age du TLE
            if (fabs(ageISS) > ui->ageMaxTLETransit->value() + 8.) {
                brush.setColor(Qt::red);
            } else if (fabs(ageISS) > ui->ageMaxTLETransit->value() + 4.) {
                brush.setColor(QColor("orange"));
            } else if (fabs(ageISS) > ui->ageMaxTLETransit->value()) {
                brush.setColor(Qt::darkYellow);
            } else {
                brush.setColor(QColor("forestgreen"));
            }

            paletteTLE.setBrush(QPalette::WindowText, brush);
            ui->ageTLETransit->setPalette(paletteTLE);

            if (fabs(ageISS - age1) < EPSDBL100) {
                ui->lbl_ageTLETransit->setText(tr("Age du premier TLE :"));
            } else {
                ui->lbl_ageTLETransit->setText(tr("Age du dernier TLE :"));
            }

            ui->ageTLETransit->setText(chaine.arg(ageISS, 0, 'f', 2));
            ui->lbl_ageTLETransit->setVisible(true);
            ui->ageTLETransit->setVisible(true);
        } else {
            ui->lbl_ageTLETransit->setVisible(false);
            ui->ageTLETransit->setVisible(false);
        }
    } else {
        ui->lbl_ageTLETransit->setVisible(false);
        ui->ageTLETransit->setVisible(false);
    }

    /* Retour */
    return;
}

/*
 * Calcul de la prochaine date d'eclipse de l'ISS
 */
void PreviSat::CalculDN() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    dateEcl = Date(satellites[0].CalculDateOmbrePenombreSuiv(dateCourante, satellites[0].conditionEclipse(), ui->nombreTrajectoires->value(),
            ui->eclipsesLune->isChecked(), ui->refractionPourEclipses->isChecked()), offsetUTC);
    acalcDN = false;

    /* Retour */
    return;
}

/*
 * Calcul du numero d'orbite de l'ISS
 */
int PreviSat::CalculNumeroOrbiteISS(const Date &date) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    Satellite sat = satellites.at(0);
    int numOrbite = 0;

    /* Corps de la methode */
    sat.CalculPosVit(date);
    sat.CalculElementsOsculateurs(date);
    Date dateCalcul(date.jourJulienUTC() + sat.elements().periode() * NB_JOUR_PAR_HEUR, 0., false);

    sat.CalculPosVit(dateCalcul);
    sat.CalculCoordTerrestres(dateCalcul);

    Date dateNA = sat.CalculDateNoeudAscPrec(dateCalcul);
    sat.CalculPosVit(dateNA);
    sat.CalculCoordTerrestres(dateNA);
    double lon1 = sat.longitude();

    bool atrouveOrb = false;
    while (!atrouveOrb) {

        dateCalcul = Date(dateNA.jourJulienUTC() - NB_JOUR_PAR_MIN, 0., false);
        sat.CalculPosVit(dateCalcul);
        sat.CalculCoordTerrestres(dateCalcul);

        dateNA = sat.CalculDateNoeudAscPrec(dateCalcul);
        sat.CalculPosVit(dateNA);
        sat.CalculCoordTerrestres(dateNA);
        double lon2 = sat.longitude();

        atrouveOrb = (lon2 < 0. && lon1 > 0.);
        numOrbite++;
        lon1 = lon2;
    }

    /* Retour */
    return (numOrbite);
}

/*
 * Enchainement des calculs
 */
void PreviSat::EnchainementCalculs() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    // Calculs specifiques lors de l'affichage du Wall Command Center
    const bool mcc = ui->mccISS->isChecked();

    // Prise en compte de la refraction atmospherique
    const bool refraction = ui->refractionPourEclipses->isChecked();

    // Calcul des eclipses produites par la Lune
    const bool acalcEclipseLune = ui->eclipsesLune->isChecked();

    // Nombre de traces au sol a afficher
    const int nbTraces = (satellites.isEmpty()) ? 0 : (mcc && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) ?
                                                      3 : (ui->afftraj->isChecked()) ? ui->nombreTrajectoires->value() : 0;

    // Prise en compte de l'extinction atmospherique
    const bool extinction = ui->extinctionAtmospherique->isChecked();

    // Prise en compte de l'effet des eclipses partielles sur la magnitude
    const bool effetEclipsePartielle = ui->effetEclipsesMagnitude->isChecked();

    // Calcul de la zone de visibilite des satellites
    const bool visibilite = ui->affvisib->isChecked() || (mcc && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE);

    // Calcul de la trace dans le ciel
    const bool traceCiel = (ui->afftraceCiel->isChecked() && ui->ciel->isVisible()) || ui->affradar->isChecked();

    /* Corps de la methode */
    try {

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
            const double beta = acos((RAYON_TERRESTRE - 15.) / soleil.distance()) - REFRACTION_HZ;
            soleil.CalculZoneVisibilite(beta);

            if (ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored) {
                // Coordonnees equatoriales
                soleil.CalculCoordEquat(observateurs.at(0));
            }
        }


        /*
         * Position de la Lune
         */
        lune.CalculPosition(dateCourante);

        // Calcul de la phase lunaire
        lune.CalculPhase(soleil);

        // Coordonnees topocentriques
        lune.CalculCoordHoriz(observateurs.at(0));

        // Calcul de la magnitude de la Lune
        lune.CalculMagnitude(soleil);

        if (!ui->carte->isHidden()) {

            // Coordonnees terrestres
            lune.CalculCoordTerrestres(observateurs.at(0));

            if (ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored) {
                // Coordonnees equatoriales
                lune.CalculCoordEquat(observateurs.at(0));
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
            if (ui->affconst->isChecked()) {
                Constellation::CalculConstellations(observateurs.at(0), constellations);
            }
            if (ui->affconst->checkState() != Qt::Unchecked) {
                LigneConstellation::CalculLignesCst(etoiles, lignesCst);
            }
        }

        /*
         * Calcul de la position courante des satellites
         */
        if (nbSat > 0) {

            Satellite::CalculPosVitListeSatellites(dateCourante, observateurs.at(0), soleil, lune, nbTraces, visibilite, extinction,
                                                   traceCiel, mcc, refraction, acalcEclipseLune, effetEclipsePartielle, satellites);

            if (ui->tempsReel->isChecked()) {
                for (int i=0; i<nbSat; i++) {

                    if (!satellites[i].isVisible() && !bipSatLOS[i] && bipSatAOS[i]) {
                        notifLOS = true;
                        bipSatLOS[i] = true;
                    }

                    if (satellites[i].isVisible() && !bipSatAOS[i]) {
                        notifAOS = true;
                        bipSatAOS[i] = true;
                    }
                }
            }

            if ((ui->onglets->count() == nbOnglets) && satellites[0].isIeralt()) {
                chronometre->stop();
                const QString msg = tr("Erreur rencontrée lors de l'exécution\nLa position du satellite %1 (numéro NORAD : %2) " \
                                       "ne peut pas être calculée (altitude négative)");
                Message::Afficher(msg.arg(tles.at(0).nom()).arg(tles.at(0).norad()), WARNING);
                chronometre->start();
                l1 = "";
                l2 = "";
            }

            if ((ui->onglets->count() < nbOnglets) && !satellites[0].isIeralt()) {
                l1 = tles.at(0).ligne1();
                l2 = tles.at(0).ligne2();
            }
        }
        Satellite::initCalcul = true;

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}


/******************************
 * Telechargement de fichiers *
 *****************************/
/*
 * Ajout d'un fichier dans la liste de fichiers a telecharger
 */
void PreviSat::AjoutFichier(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (downQueue.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(TelechargementSuivant()));
    }
    downQueue.enqueue(url);

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

    /* Corps de la methode */
    const QFileInfo fi(nomfic);
    if (fi.absoluteDir() == dirTle) {

        amajDeb = false;
        amajPrevi = false;
        atrouve = true;
        aupdnow = false;
        aupdateCF5 = false;
        dirDwn = dirTle;

        messagesStatut->setText(tr("Mise à jour du fichier TLE %1 en cours...").arg(fi.fileName()));
        const QString adresse = (fi.fileName().contains("spctrk")) ?
                    adresseAstropedia + "previsat/tle/" : adresseCelestrakNorad;
        const QString ficMaj = adresse + fi.fileName();
        TelechargementFichier(ficMaj, true);

        if (downQueue.isEmpty()) {
            QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
        }

        settings.setValue("temps/lastUpdate", dateCourante.jourJulienUTC());
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
    amajDeb = true;
    amajPrevi = false;
    atrouve = false;
    aupdnow = false;
    dirDwn = dirTle;

    /* Corps de la methode */
    QStringListIterator it(listeGroupeMaj);
    while (it.hasNext()) {
        const QStringList ligne = it.next().split("#");
        QString adresse = ligne.at(0).split("@").at(1);
        const QStringList listeTLEs = ligne.at(1).split(",");

        if (adresse.contains("celestrak")) {
            adresse = adresseCelestrakNorad;
        }
        if (adresse.contains("astropedia")) {
            adresse = adresseAstropedia + "previsat/tle/";
        }
        if (!adresse.startsWith("http://")) {
            adresse.insert(0, "http://");
        }
        if (!adresse.endsWith("/")) {
            adresse.append("/");
        }

        foreach(QString file, listeTLEs) {

            const QString ficMaj = adresse + file;
            const QString fic = QDir::toNativeSeparators(dirTle + QDir::separator() + file);
            if (fic == nomfic) atrouve = true;

            TelechargementFichier(ficMaj, false);
        }

        if (downQueue.isEmpty()) {
            QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
        }
    }
    amajDeb = false;

    /* Retour */
    return;
}

/*
 * Mise a jour des fichiers internes et du logiciel
 */
void PreviSat::MiseAJourFichiers(QAction *action, const QString &typeMAJ)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    action->setVisible(true);

    const QString msg = tr("Une mise à jour %1 est disponible. Souhaitez-vous la télécharger?");

    QMessageBox msgbox(tr("Information"), msg.arg(typeMAJ), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
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
        if (action == ui->actionMettre_jour_fichiers_internes) {
            aclickFicMaj = false;
            on_actionMettre_jour_fichiers_internes_triggered();
            ui->actionMettre_jour_fichiers_internes->setVisible(false);
        }
    }

    /* Retour */
    return;
}

/*
 * Telechargement d'un fichier
 */
void PreviSat::TelechargementFichier(const QString &ficHttp, const bool async)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QUrl url(ficHttp);

    /* Corps de la methode */
    AjoutFichier(url);

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    const QNetworkRequest requete(url);
    rep = mng.get(requete);

    if (!async) {

        // Creation d'une boucle pour rendre le telechargement synchrone
        QEventLoop loop;
        connect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        rep->deleteLater();
    }

    if (downQueue.isEmpty()) {
        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
    }

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
        if (fabs(dateCourante.jourJulienUTC() - tles.at(0).epoque().jourJulienUTC()) > ageMax && ui->tempsReel->isChecked()) {
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
            old = true;
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
    const QString dirHttpPrevi = adresseAstropedia + "previsat/Qt/";

    /* Corps de la methode */
    const QStringList listeFic(QStringList () << "versionPreviSat" << "majFicInt" << "majInfos");
    amajDeb = true;
    amajPrevi = true;
    dirDwn = dirTmp;

    foreach(QString fic, listeFic) {

        const QString ficMaj = dirHttpPrevi + fic;
        TelechargementFichier(ficMaj, false);

        QString ligne;
        QFile fi(dirDwn + QDir::separator() + fic);
        if (fi.exists() && fi.size() != 0) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            ligne = flux.readLine();
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
                    MiseAJourFichiers(ui->actionTelecharger_la_mise_a_jour, tr("de ") + QCoreApplication::applicationName());
                    settings.setValue("fichier/majPrevi", "1");
                } else {
                    ui->actionTelecharger_la_mise_a_jour->setVisible(false);
                }

            } else if ((fic == "majFicInt" || fic == "majInfos") && !anewVersion) {

                const int an = ligne.mid(0, 4).toInt();
                const int mo = ligne.mid(5, 2).toInt();
                const int jo = ligne.mid(8, 2).toInt();

                const QDateTime dateHttp(QDate(an, mo, jo), QTime(0, 0, 0));

                if (fic == "majFicInt") {

                    dirDwn = dirLocalData;

                    QDateTime dateMax;
                    for(int i=0; i<listeFicLocalData.size(); i++) {

                        const QString fichier = listeFicLocalData.at(i);
                        if (!fichier.contains("gestionnaireTLE_") && !fichier.contains("preferences")) {
                            const QString fich = dirLocalData + QDir::separator() + fichier;
                            const QFileInfo fi2(fich);

                            if (fi2.lastModified().date() > dateMax.date()) {
                                dateMax.setDate(fi2.lastModified().date());
                            }
                        }
                    }

                    const bool anew = (dateHttp > dateMax);

                    if (anew && !ui->actionTelecharger_la_mise_a_jour->isVisible()) {
                        MiseAJourFichiers(ui->actionMettre_jour_fichiers_internes, tr("des fichiers internes"));
                        settings.setValue("fichier/majPrevi", "1");
                    } else {
                        ui->actionMettre_jour_fichiers_internes->setVisible(false);
                    }
                }

                if (fic == "majInfos") {

                    majInfosDate = ligne;
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

            } else {
            }
        }
    }
    amajDeb = false;

    /* Retour */
    return;
}


/*************
 * Interface *
 *************/
/*
 * Affichage des fichiers TLE dans les listes deroulantes
 */
void PreviSat::AffichageListeFichiersTLE(const QString &fichier, QComboBox *comboBox, QStringList &listeFicTLEs)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        const QFileInfo fi(fichier);
        if (listeFicTLEs.contains(QDir::toNativeSeparators(fi.absoluteFilePath()))) {
            comboBox->setCurrentIndex(listeFicTLEs.indexOf(QDir::toNativeSeparators(fi.filePath())));
        } else {
            listeFicTLEs.append(QDir::toNativeSeparators(fi.absoluteFilePath()));
            if (comboBox->itemText(0).isEmpty()) {
                comboBox->setItemText(0, fi.fileName());
            } else {
                comboBox->removeItem(comboBox->count() - 1);
                comboBox->addItem(QDir::toNativeSeparators(fi.fileName()));
                comboBox->setCurrentIndex(comboBox->count() - 1);
                comboBox->addItem(tr("Parcourir..."));
            }
        }
        on_actionVision_nocturne_toggled(ui->actionVision_nocturne->isChecked());
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Affichage des donnees du lieu selectionne lorsqu'on clique sur son nom
 */
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
                                                             msg.arg(qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd))).append(tr("ft")));
    }

    /* Retour */
    return;
}

/*
 * Chargement des preferences
 */
void PreviSat::ChargementPref() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomPref = ficPrf.at(ui->preferences->currentIndex());

    /* Corps de la methode */
    QFile fichier(nomPref);
    if (fichier.exists() && fichier.size() != 0) {

        // Lecture du fichier de preferences
        fichier.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichier);

        QStringList listePrf;
        while (!flux.atEnd()) {
            listePrf.append(flux.readLine());
        }
        fichier.close();

        QStringListIterator it(listePrf);
        while (it.hasNext()) {
            const QStringList item = it.next().split(" ", QString::SkipEmptyParts);

            if (item.at(1) == "true") {
                settings.setValue(item.at(0), true);

            } else if (item.at(1) == "false") {
                settings.setValue(item.at(0), false);

            } else {
                if (item.at(0) == "affichage/magnitudeEtoiles") {
                    settings.setValue(item.at(0), item.at(1).toDouble());

                } else if (item.at(0) == "affichage/affconst" || item.at(0) == "affichage/affnomlieu" || item.at(0) == "affichage/affnomsat" ||
                           item.at(0) == "affichage/affplanetes" || item.at(0) == "affichage/affradar" || item.at(0) == "affichage/affvisib" ||
                           item.at(0) == "affichage/intensiteOmbre" || item.at(0) == "affichage/intensiteVision") {
                    settings.setValue(item.at(0), item.at(1).toUInt());
                } else {
                }
            }
        }

        ui->affconst->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt()));
        ui->affcoord->setChecked(settings.value("affichage/affcoord", true).toBool());
        ui->affetoiles->setChecked(settings.value("affichage/affetoiles", true).toBool());
        ui->affgrille->setChecked(settings.value("affichage/affgrille", true).toBool());
        ui->afficone->setChecked(settings.value("affichage/afficone", true).toBool());
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
        ui->afftraceCiel->setChecked(settings.value("affichage/afftraceCiel", true).toBool());
        ui->afftraj->setChecked(settings.value("affichage/afftraj", true).toBool());
        ui->affvisib->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affvisib", Qt::Checked).toUInt()));
        ui->calJulien->setChecked(settings.value("affichage/calJulien", false).toBool());
        ui->extinctionAtmospherique->setChecked(settings.value("affichage/extinction", true).toBool());
        ui->refractionPourEclipses->setChecked(settings.value("affichage/refractionPourEclipses", true).toBool());
        ui->effetEclipsesMagnitude->setChecked(settings.value("affichage/effetEclipsesMagnitude", true).toBool());
        ui->eclipsesLune->setChecked(settings.value("affichage/eclipsesLune", true).toBool());
        ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 40).toInt());
        ui->intensiteVision->setValue(settings.value("affichage/intensiteVision", 50).toInt());
        ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
        ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 2).toInt());
        ui->rotationIconeISS->setChecked(settings.value("affichage/rotationIconeISS", true).toBool());
        ui->rotationLune->setChecked(settings.value("affichage/rotationLune", false).toBool());
        ui->utcAuto->setChecked(settings.value("affichage/utcAuto", true).toBool());
        ui->typeRepere->setCurrentIndex(settings.value("affichage/typeRepere", 0).toInt());
        ui->typeParametres->setCurrentIndex(settings.value("affichage/typeParametres", 0).toInt());
        ui->affichageMsgMAJ->setCurrentIndex(settings.value("fichier/affichageMsgMAJ", 1).toInt());
        ui->verifMAJ->setChecked(settings.value("affichage/verifMAJ", false).toBool());
    }

    /* Retour */
    return;
}

/*
 * Ecriture du compte-rendu de mise a jour des TLE
 */
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

    if (!ui->compteRenduMaj->toPlainText().isEmpty()) {
        if (!ui->compteRenduMaj->toPlainText().split("\n").last().trimmed().isEmpty()) {
            ui->compteRenduMaj->appendPlainText("");
        }
    }

    ui->compteRenduMaj->appendPlainText(QString(tr("Fichier %1 :").arg(fic)));

    QString msgcpt;
    if (nbmaj < nbold && nbmaj > 0) {

        msgcpt = tr("TLE du satellite %1 (%2) non réactualisé");

        for(int i=0; i<compteRendu.count()-5; i++) {
            const QString nomsat = compteRendu.at(i).split("#").at(0);
            const QString norad = compteRendu.at(i).split("#").at(1);
            ui->compteRenduMaj->appendPlainText(msgcpt.arg(nomsat).arg(norad));
        }
    }

    if (nbsup > 0) {
        msgcpt = tr("Nombre de TLE(s) supprimés : %1");
        ui->compteRenduMaj->appendPlainText(msgcpt.arg(nbsup));
    }

    if (nbadd > 0) {
        msgcpt = tr("Nombre de TLE(s) ajoutés : %1");
        ui->compteRenduMaj->appendPlainText(msgcpt.arg(nbadd));
    }

    if (nbmaj < nbold && nbmaj > 0) {
        msgcpt = tr("%1 TLE(s) sur %2 mis à jour");
        ui->compteRenduMaj->appendPlainText(msgcpt.arg(nbmaj).arg(nbold));
    }

    if (nbmaj == nbold && nbold != 0) {
        msgcpt = tr("Mise à jour de tous les TLE effectuée (fichier de %1 satellite(s))");
        ui->compteRenduMaj->appendPlainText(msgcpt.arg(nbold));
    }

    if (nbmaj == 0 && nbold != 0) {
        ui->compteRenduMaj->appendPlainText(tr("Aucun TLE mis à jour"));
    }

    ui->compteRenduMaj->appendPlainText("");
    ui->compteRenduMaj->verticalScrollBar()->setValue(ui->compteRenduMaj->blockCount());

    aecr = nbold > 0 && (nbmaj > 0 || nbsup > 0 || nbadd > 0);

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
        if (listWidget->item(i)->checkState() == Qt::Checked) {
            k++;
        }
    }

    /* Retour */
    return (k);
}

/*
 * Modification d'une option d'affichage
 */
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

    ui->rotationIconeISS->setEnabled(ui->afficone->isChecked());

    ui->intensiteOmbre->setEnabled(ui->affnuit->isChecked());
    ui->nombreTrajectoires->setEnabled(ui->afftraj->isChecked());

    const QString fmt = tr("dddd dd MMMM yyyy  hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "");
    ui->dateHeure3->setDisplayFormat(fmt);
    ui->dateHeure3->setDateTime(dateCourante.ToQDateTime(1));
    ui->dateHeure4->setDisplayFormat(fmt);
    ui->dateHeure4->setDateTime(dateCourante.ToQDateTime(1));

    if (ui->unitesKm->hasFocus() || ui->unitesMi->hasFocus() || ui->syst12h->hasFocus() || ui->syst24h->hasFocus()) {
        info = true;
        AffichageLieuObs();
        ui->lieuxObs->setCurrentRow(-1);
        ui->selecLieux->setCurrentRow(-1);
    }

    acalcAOS = true;
    acalcDN = true;

    ui->afficherPrev->setEnabled(false);
    ui->afficherEvt->setEnabled(false);
    ui->afficherTransit->setEnabled(false);
    ui->afficherMetOp->setEnabled(false);

    if (ui->zoneAffichage->isVisible() || ui->ongletsOptions->isVisible()) {

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

/*
 * Ouverture d'un fichier TLE
 */
void PreviSat::OuvertureFichierTLE(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        int nsat;
        bool agz = false;
        old = false;
        QString fich = fichier;
        ficgz = "";

        QFileInfo fi(fich);
        if (fi.suffix() == "gz") {

            // Cas d'un fichier compresse au format gz
            const QString fic = dirTmp + QDir::separator() + fi.completeBaseName();

            if (DecompressionFichierGz(fich, fic)) {

                nsat = TLE::VerifieFichier(fic, false);
                if (nsat == 0) {
                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                    throw PreviSatException(msg.arg(fichier), WARNING);
                }
                ficgz = fich;
                fich = fic;
                agz = true;
            } else {
                const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                throw PreviSatException(msg.arg(fich), WARNING);
            }
        }

        messagesStatut->setText(tr("Ouverture du fichier TLE..."));

        // Verification du fichier TLE
        nsat = TLE::VerifieFichier(fich, true);
        nomfic = fich;

        const QString chaine = tr("Fichier TLE OK : %1 satellites");
        messagesStatut->setText(chaine.arg(nsat));

        if (agz) {
            settings.setValue("fichier/nom", ficgz);
        } else {
            settings.setValue("fichier/nom", nomfic);
        }
        settings.setValue("fichier/repTLE", fi.absolutePath());

        if (QDir::toNativeSeparators(fi.absolutePath()) == dirTle) {
            ui->listeFichiersTLE->setCurrentIndex(ficTLE.indexOf(QDir::toNativeSeparators(fi.filePath())));

            QStringListIterator it(liste.split("$"));
            while (it.hasNext()) {

                const QString ficTLEs = it.next();
                if (nomfic.contains(dirTle)) {
                    if (nomfic == dirTle + QDir::separator() + ficTLEs.split("#").at(0)) {
                        listeTLE = ficTLEs.split("#").at(1).split("&");
                        nbSat = listeTLE.count();
                        Satellite::initCalcul = false;
                    }
                }
            }
        } else {
            if (!ficTLE.contains(fi.absoluteFilePath())) {
                if (ui->listeFichiersTLE->itemText(0).isEmpty()) {
                    ui->listeFichiersTLE->setItemText(0, fi.fileName());
                } else {
                    ui->listeFichiersTLE->removeItem(ui->listeFichiersTLE->count() - 1);
                    ui->listeFichiersTLE->addItem(QDir::toNativeSeparators(fi.fileName()));
                    ui->listeFichiersTLE->addItem(tr("Parcourir..."));
                }
                ficTLE.append(fi.absoluteFilePath());
                ui->listeFichiersTLE->setCurrentIndex(ficTLE.indexOf(fi.absoluteFilePath()));
            }
        }

        // Ouverture du fichier TLE
        AfficherListeSatellites(nomfic, listeTLE);
        notifLOS = false;
        notifFlash = false;

        // Mise a jour de la liste de satellites selectionnes
        if (nbSat > 0) {
            nor = listeTLE.at(0);
            nbSat = getListeItemChecked(ui->liste1);

            if (nbSat > 0) {
                listeTLE.clear();
                tles.clear();
                tles.resize(nbSat);
                bipSatAOS.resize(nbSat);
                bipSatLOS.resize(nbSat);

                int j = -1;
                for (int i=0; i<ui->liste1->count(); i++) {
                    if (ui->liste1->item(i)->checkState() == Qt::Checked) {
                        listeTLE.append(ui->liste1->item(i)->text().split("#").at(1));
                        if (listeTLE.last() == nor) {
                            j = listeTLE.size() - 1;
                        }
                    }
                }

                if (j > 0) {
                    listeTLE[j] = listeTLE[0];
                    listeTLE[0] = nor;
                }

                info = true;
                acalcAOS = true;
                acalcDN = true;

                EcritureListeRegistre();

                if (nbSat == 0) {
                    l1 = "";
                    l2 = "";
                    listeTLE.clear();
                }

                Satellite::initCalcul = false;

                Satellite::LectureDonnees(listeTLE, tles, satellites);

                CalculsAffichage();

                // Verification de l'age du TLE
                if (!l1.isEmpty() && !l2.isEmpty()) {
                    if (!old) {
                        VerifAgeTLE();
                    }
                }
            }
        } else {

            // Aucun satellite de la liste n'est dans le nouveau fichier
            ui->liste1->setCurrentRow(0);
            l1 = "";
            l2 = "";
            listeTLE.clear();

            // Enchainement de l'ensemble des calculs
            EnchainementCalculs();

            // Affichage des donnees numeriques
            AffichageDonnees();

            //Affichage des elements sur la carte du monde et le radar
            AffichageCourbes();
        }
        on_actionVision_nocturne_toggled(ui->actionVision_nocturne->isChecked());

    } catch (PreviSatException &e) {
        messagesStatut->setText("");
        throw PreviSatException();
    }

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
    try {
        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
                arg(QString(APP_ANNEES_DEV)) << endl << endl << endl;
        flux << tr("Date :") << " " << ui->dateHeure2->text() << endl << endl;

        // Donnees sur le satellite
        flux << tr("Nom du satellite :") + " " + ui->nomsat2->text() << endl;
        flux << ui->ligne1->text() << endl;
        flux << ui->ligne2->text() << endl << endl;

        flux << tr("Vecteur d'état") << " (" << ui->typeRepere->currentText() << ") :" << endl;
        QString chaine = tr("x : %1%2\tvx : %3");
        flux << chaine.arg(QString(13 - ui->xsat->text().length(), QChar(' '))).arg(ui->xsat->text()).
                arg(ui->vxsat->text()) << endl;

        chaine = tr("y : %1%2\tvy : %3");
        flux << chaine.arg(QString(13 - ui->ysat->text().length(), QChar(' '))).arg(ui->ysat->text()).
                arg(ui->vysat->text()) << endl;

        chaine = tr("z : %1%2\tvz : %3");
        flux << chaine.arg(QString(13 - ui->zsat->text().length(), QChar(' '))).arg(ui->zsat->text()).
                arg(ui->vzsat->text()) << endl << endl;


        flux << tr("Éléments osculateurs :") << endl;
        switch (ui->typeParametres->currentIndex()) {

        case 0:
            // Parametres kepleriens
            chaine = tr("Demi-grand axe       : %1\tAscension droite du noeud ascendant : %2%3");
            flux << chaine.arg(ui->demiGrandAxe->text()).
                    arg(QString(9 - ui->ADNoeudAscendant->text().length(), QChar('0'))).
                    arg(ui->ADNoeudAscendant->text()) << endl;

            chaine = tr("Excentricité         : %1\tArgument du périgée                 : %2%3");
            flux << chaine.arg(ui->excentricite->text()).
                    arg(QString(9 - ui->argumentPerigee->text().length(), QChar('0'))).
                    arg(ui->argumentPerigee->text()) << endl;

            chaine = tr("Inclinaison          : %1%2\tAnomalie moyenne                    : %3%4");
            flux << chaine.arg(QString(9 - ui->inclinaison->text().length(), QChar('0'))).arg(ui->inclinaison->text()).
                    arg(QString(9 - ui->anomalieMoyenne->text().length(), QChar('0'))).arg(ui->anomalieMoyenne->text())
                 << endl << endl;
            break;

        case 1:
            // Parametres circulaires
            chaine = tr("Demi-grand axe       : %1\tAscension droite du noeud ascendant : %2%3");
            flux << chaine.arg(ui->demiGrandAxe->text()).
                    arg(QString(9 - ui->ADNoeudAscendant2->text().length(), QChar('0'))).
                    arg(ui->ADNoeudAscendant2->text()) << endl;

            chaine = tr("Ex                   : %1\tInclinaison                         : %2%3");
            flux << chaine.arg(ui->ex1->text()).arg(QString(9 - ui->inclinaison2->text().length(), QChar('0'))).
                    arg(ui->inclinaison2->text()) << endl;

            chaine = tr("Ey                   : %1\tPosition sur orbite                 : %2%3");
            flux << chaine.arg(ui->ey1->text()).arg(QString(9 - ui->positionSurOrbite->text().length(), QChar('0'))).
                    arg(ui->positionSurOrbite->text())
                 << endl << endl;
            break;

        case 2:
            // Parametres equatoriaux
            chaine = tr("Demi-grand axe       : %1\tIx                 : %2");
            flux << chaine.arg(ui->demiGrandAxe->text()).arg(ui->ix1->text()) << endl;

            chaine = tr("Excentricité         : %1\tIy                 : %2");
            flux << chaine.arg(ui->excentricite2->text()).arg(ui->iy1->text()) << endl;

            chaine = tr("Longitude du périgée : %1%2\tAnomalie moyenne   : %3%4");
            flux << chaine.arg(QString(9 - ui->longitudePerigee->text().length(), QChar('0'))).
                    arg(ui->longitudePerigee->text()).
                    arg(QString(9 - ui->anomalieMoyenne2->text().length(), QChar('0'))).
                    arg(ui->anomalieMoyenne2->text()) << endl << endl;
            break;

        case 3:
            // Parametres circulaires equatoriaux
            chaine = tr("Demi-grand axe       : %1\tIx                          : %2");
            flux << chaine.arg(ui->demiGrandAxe->text()).arg(ui->ix2->text()) << endl;

            chaine = tr("Ex                   : %1\tIy                          : %2");
            flux << chaine.arg(ui->ex2->text()).arg(ui->iy2->text()) << endl;

            chaine = tr("Ey                   : %1\tArgument de longitude vraie : %2%3");
            flux << chaine.arg(ui->ey2->text()).arg(QString(9 - ui->argumentLongitudeVraie2->text().length(), QChar('0'))).
                    arg(ui->argumentLongitudeVraie2->text()) << endl << endl;
            break;

        default:
            break;
        }

        chaine = tr("Anomalie vraie       : %1%2\tApogée  (Altitude) : %3");
        flux << chaine.arg(QString(9 - ui->anomalieVraie->text().length(), QChar('0'))).arg(ui->anomalieVraie->text())
                .arg(ui->apogee->text()) << endl;

        chaine = tr("Anomalie excentrique : %1%2\tPérigée (Altitude) : %3");
        flux << chaine.arg(QString(9 - ui->anomalieExcentrique->text().length(), QChar('0')))
                .arg(ui->anomalieExcentrique->text()).arg(ui->perigee->text()) << endl;

        chaine = tr("Champ de vue         : %1  \tPériode orbitale   : %2");
        flux << chaine.arg(ui->champDeVue->text()).arg(ui->periode->text().replace(" ", "")) << endl << endl;


        flux << tr("Divers :") << endl;
        chaine = tr("Doppler @ 100 MHz    : %1");
        flux << chaine.arg(ui->doppler->text()) << endl;

        chaine = tr("Atténuation          : %1");
        flux << chaine.arg(ui->attenuation->text()) << endl;

        chaine = tr("Délai                : %1");
        flux << chaine.arg(ui->delai->text()) << endl << endl;

        chaine = tr("Phasage              : %1");
        flux << chaine.arg(ui->phasage->text()) << endl;

        sw.close();
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet General
 */
void PreviSat::SauveOngletGeneral(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
                arg(QString(APP_ANNEES_DEV)) << endl << endl << endl;
        flux << tr("Date :") << " " << ui->dateHeure1->text() << endl << endl;

        flux << tr("Lieu d'observation :") << " " << ui->lieuxObservation1->currentText() << endl;
        QString chaine = tr("Longitude  : %1\tLatitude : %2\tAltitude : %3");
        flux << chaine.arg(ui->longitudeObs->text()).arg(ui->latitudeObs->text()).arg(ui->altitudeObs->text()) << endl;
        chaine = tr("Conditions : %1");
        flux << chaine.arg(ui->conditionsObservation->text()) << endl << endl << endl;

        if (ui->onglets->count() == nbOnglets) {

            // Donnees sur le satellite
            flux << tr("Nom du satellite :") + " " + ui->nomsat1->text() << endl << endl;

            chaine = tr("Longitude : %1\t\tHauteur    : %2\tAscension droite :  %3");
            flux << chaine.arg(ui->longitudeSat->text().trimmed()).arg(ui->hauteurSat->text())
                    .arg(ui->ascensionDroiteSat->text().trimmed()) << endl;

            chaine = tr("Latitude  :  %1\t\tAzimut (N) : %2\tDéclinaison      : %3");
            flux << chaine.arg(ui->latitudeSat->text().trimmed()).arg(ui->azimutSat->text().trimmed())
                    .arg(ui->declinaisonSat->text()) << endl;

            chaine = tr("Altitude  :  %1%2\t\tDistance   : %3%4\tConstellation    : %5");
            flux << chaine.arg(ui->altitudeSat->text()).arg(QString(13 - ui->altitudeSat->text().length(), QChar(' ')))
                    .arg(ui->distanceSat->text()).arg(QString(13 - ui->distanceSat->text().length(), QChar(' ')))
                    .arg(ui->constellationSat->text()) << endl << endl;

            chaine = tr("Direction          : %1  \tOrbite n°%2      \t\t%3");
            flux << chaine.arg(ui->directionSat->text()).arg(ui->nbOrbitesSat->text())
                    .arg((ui->magnitudeSat->x() == 333) ? ui->magnitudeSat->text() : "").trimmed() << endl;

            chaine = tr("Vitesse orbitale   : %1%2  \t%3\t%4");
            flux << chaine.arg((ui->vitesseSat->text().length() < 11) ? " " : "").arg(ui->vitesseSat->text())
                    .arg((ui->dateJN->isVisible()) ? ui->lbl_prochainJN->text() + " " + ui->dateJN->text() + " " : ui->magnitudeSat->text())
                    .arg((ui->dateAOS->isVisible()) ? ui->lbl_beta->text() : "").trimmed() << endl;

            chaine = tr("Variation distance : %1  \t%2");
            flux << chaine.arg(ui->rangeRate->text())
                    .arg((ui->dateAOS->isVisible()) ? ui->lbl_prochainAOS->text() + " " + ui->dateAOS->text() : ui->lbl_beta->text())
                    .trimmed() << endl << endl << endl;
        }

        // Donnees sur le Soleil
        flux << tr("Coordonnées du Soleil :") << endl;
        chaine = tr("Hauteur    : %1\t\tAscension droite :  %2");
        flux << chaine.arg(ui->hauteurSoleil->text().trimmed()).arg(ui->ascensionDroiteSoleil->text()) << endl;

        chaine = tr("Azimut (N) : %1\t\tDéclinaison      : %2");
        flux << chaine.arg(ui->azimutSoleil->text().trimmed()).arg(ui->declinaisonSoleil->text()) << endl;

        chaine = tr("Distance   : %1   \t\tConstellation    : %2");
        flux << chaine.arg(ui->distanceSoleil->text()).arg(ui->constellationSoleil->text()) << endl << endl << endl;

        // Donnees sur la Lune
        flux << tr("Coordonnées de la Lune :") << endl;
        chaine = tr("Hauteur    : %1\t\tAscension droite :  %2");
        flux << chaine.arg(ui->hauteurLune->text().trimmed()).arg(ui->ascensionDroiteLune->text()) << endl;

        chaine = tr("Azimut (N) : %1\t\tDéclinaison      : %2");
        flux << chaine.arg(ui->azimutLune->text().trimmed()).arg(ui->declinaisonLune->text()) << endl;

        chaine = tr("Distance   : %1  \t\tConstellation    : %2");
        flux << chaine.arg(ui->distanceLune->text()).arg(ui->constellationLune->text()) << endl << endl;
        flux << tr("Phase        :") + " " + ui->phaseLune->text() << endl;
        flux << tr("Illumination :") + " " + ui->illuminationLune->text() << endl;
        flux << tr("Magnitude    :") + " " + ui->magnitudeLune->text() << endl;

        sw.close();
    } catch (PreviSatException &e) {
    }

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
    try {
        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
                arg(QString(APP_ANNEES_DEV)) << endl << endl << endl;

        if (ui->frameInformationsSatellite->isVisible()) {

            // Donnees sur le satellite
            flux << tr("Nom du satellite :") + " " + ui->nomsat3->text() << endl;
            flux << ui->line1->text() << endl;
            flux << ui->line2->text() << endl << endl;

            QString chaine = tr("Numéro NORAD            : %1 \t\tMoyen mouvement       : %2 rev/jour\t Date de lancement  : %3");
            flux << chaine.arg(ui->norad->text()).arg(ui->nbRev->text()).arg(ui->dateLancement->text()) << endl;

            chaine = tr("Désignation COSPAR      : %1\t\tn'/2                  : %2%3 rev/jour^2\t Catégorie d'orbite : %4");
            flux << chaine.arg(ui->cospar->text()).arg(QString(11 - ui->nbRev2->text().length(), QChar(' '))).arg(ui->nbRev2->text()).
                    arg(ui->categorieOrbite->text()) << endl;

            chaine = tr("Époque (UTC)            : %1\tn\"/6                  : %2%3 rev/jour^3\t Pays/Organisation  : %4");
            flux << chaine.arg(ui->epoque->text()).arg(QString(11 - ui->nbRev3->text().length(), QChar(' '))).arg(ui->nbRev3->text()).
                    arg(ui->pays->text()) << endl;

            chaine = tr("Coeff pseudo-balistique : %1 (1/Re)\tNb orbites à l'époque : %2\t\t\t Site de lancement  : %3");
            flux << chaine.arg(ui->bstar->text()).arg(ui->nbOrbitesEpoque->text()).arg(ui->siteLancement->text()) << endl << endl;

            chaine = tr("Inclinaison             : %1%2\t\tAnomalie moyenne      : %3%4");
            flux << chaine.arg(QString(9 - ui->inclinaisonMoy->text().trimmed().length(), QChar('0')))
                    .arg(ui->inclinaisonMoy->text().trimmed()).arg(QString(9 - ui->anomalieMoy->text().trimmed().length(), QChar('0')))
                    .arg(ui->anomalieMoy->text().trimmed()) << endl;

            chaine = tr("AD noeud ascendant      : %1%2\t\tMagnitude std/max     : %3");
            flux << chaine.arg(QString(9 - ui->ADNoeudAscendantMoy->text().trimmed().length(), QChar('0')))
                    .arg(ui->ADNoeudAscendantMoy->text().trimmed()).arg(ui->magnitudeStdMax->text()) << endl;

            chaine = tr("Excentricité            : %1\t\tModèle orbital        : %2");
            flux << chaine.arg(ui->excentriciteMoy->text()).arg(ui->modele->text()) << endl;

            chaine = tr("Argument du périgée     : %1%2\t\tDimensions/Section    : %3%4");
            flux << chaine.arg(QString(9 - ui->argumentPerigeeMoy->text().trimmed().length(), QChar('0')))
                    .arg(ui->argumentPerigeeMoy->text().trimmed()).arg(ui->dimensions->text())
                    .arg((ui->dimensions->text() == tr("Inconnues")) ? "" : "^2") << endl;

        } else {

            // Donnees sur le satellite
            flux << tr("Nom                :") + " " + ui->nomsat->text() << endl;

            QString chaine = tr("Numéro NORAD       : %1\t\tMagnitude std/max  : %2");
            flux << chaine.arg(ui->numNorad->text()).arg(ui->magnitudeStdMaxDonneesSat->text()) << endl;

            chaine = tr("Désignation COSPAR : %1\t\tModèle orbital     : %2");
            flux << chaine.arg(ui->desigCospar->text()).arg(ui->modele->text()) << endl;

            chaine = tr("Dimensions/Section : %1%2");
            flux << chaine.arg(ui->dimensionsDonneesSat->text()).
                    arg((ui->dimensionsDonneesSat->text() == tr("Inconnues")) ? "" : "^2") << endl << endl;

            chaine = tr("Date de lancement  : %1\t\tApogée  (Altitude) : %2");
            flux << chaine.arg(ui->dateLancementDonneesSat->text()).arg(ui->apogeeDonneesSat->text()) << endl;

            chaine = (ui->dateRentree->isVisible()) ? tr("Date de rentrée    : %1\t\t").arg(ui->dateRentree->text()) :
                                                      tr("Catégorie d'orbite : %1\t\t").arg(ui->categorieOrbiteDonneesSat->text());
            flux << chaine + tr("Périgée (Altitude) : %1").arg(ui->perigeeDonneesSat->text()) << endl;

            chaine = (ui->dateRentree->isVisible()) ? tr("Catégorie d'orbite : %1\t\t").arg(ui->categorieOrbiteDonneesSat->text()) :
                                                      tr("Pays/Organisation  : %1\t\t").arg(ui->paysDonneesSat->text());
            flux << chaine + tr("Période orbitale   : %1").arg(ui->periodeDonneesSat->text()) << endl;

            chaine = (ui->dateRentree->isVisible()) ? tr("Pays/Organisation  : %1\t\t").arg(ui->paysDonneesSat->text()) :
                                                      tr("Site de lancement  : %1\t\t").arg(ui->siteLancement->text());
            flux << chaine + tr("Inclinaison        : %1").arg(ui->inclinaisonDonneesSat->text()) << endl;

            if (ui->dateRentree->isVisible())
                flux << tr("Site de lancement  : %1").arg(ui->siteLancementDonneesSat->text());
        }
        sw.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Sauvegarde des preferences
 */
void PreviSat::SauvePreferences(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        flux << "affichage/rechercheDonneesSat " << BOOL_STR(ui->rechercheDonneesSat->isChecked()) << endl <<
                "affichage/affMax " << BOOL_STR(ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) << endl <<
                "affichage/affSAA " << BOOL_STR(ui->affSAA->isChecked()) << endl <<
                "affichage/affconst " << ui->affconst->checkState() << endl <<
                "affichage/affcoord " << BOOL_STR(ui->affcoord->isChecked()) << endl <<
                "affichage/affetoiles " << BOOL_STR(ui->affetoiles->isChecked()) << endl <<
                "affichage/affgrille " << BOOL_STR(ui->affgrille->isChecked()) << endl <<
                "affichage/afficone " << BOOL_STR(ui->afficone->isChecked()) << endl <<
                "affichage/affinvew " << BOOL_STR(ui->affinvew->isChecked()) << endl <<
                "affichage/affinvns " << BOOL_STR(ui->affinvns->isChecked()) << endl <<
                "affichage/afflune " << BOOL_STR(ui->afflune->isChecked()) << endl <<
                "affichage/affnomlieu " << ui->affnomlieu->checkState() << endl <<
                "affichage/affnomsat " << ui->affnomsat->checkState() << endl <<
                "affichage/affnotif " << BOOL_STR(ui->affnotif->isChecked()) << endl <<
                "affichage/affnuit " << BOOL_STR(ui->affnuit->isChecked()) << endl <<
                "affichage/affphaselune " << BOOL_STR(ui->affphaselune->isChecked()) << endl <<
                "affichage/affplanetes " << ui->affplanetes->checkState() << endl <<
                "affichage/affradar " << ui->affradar->checkState() << endl <<
                "affichage/affsoleil " << BOOL_STR(ui->affsoleil->isChecked()) << endl <<
                "affichage/afftraceCiel " << BOOL_STR(ui->afftraceCiel->isChecked()) << endl <<
                "affichage/afftraj " << BOOL_STR(ui->afftraj->isChecked()) << endl <<
                "affichage/affvisib " << ui->affvisib->checkState() << endl <<
                "affichage/calJulien " << BOOL_STR(ui->calJulien->isChecked()) << endl <<
                "affichage/eclipsesLune " << BOOL_STR(ui->calJulien->isChecked()) << endl <<
                "affichage/effetEclipsesMagnitude " << BOOL_STR(ui->effetEclipsesMagnitude->isChecked()) << endl <<
                "affichage/extinction " << BOOL_STR(ui->extinctionAtmospherique->isChecked()) << endl <<
                "affichage/fenetreMax " << BOOL_STR(isMaximized()) << endl <<
                "affichage/groupeTLE " << ui->groupeTLE->currentIndex() << endl <<
                "affichage/hauteur " << height() << endl <<
                "affichage/intensiteOmbre " << ui->intensiteOmbre->value() << endl <<
                "affichage/intensiteVision " << ui->intensiteVision->value() << endl <<
                "affichage/largeur " << width() << endl <<
                "affichage/magnitudeEtoiles " << ui->magnitudeEtoiles->value() << endl <<
                "affichage/nombreTrajectoires " << ui->nombreTrajectoires->value() << endl <<
                "affichage/proportionsCarte " << BOOL_STR(ui->proportionsCarte->isChecked()) << endl <<
                "affichage/refractionPourEclipses " << BOOL_STR(ui->refractionPourEclipses->isChecked()) << endl <<
                "affichage/rotationIconeISS " << BOOL_STR(ui->rotationIconeISS->isChecked()) << endl <<
                "affichage/rotationLune " << BOOL_STR(ui->rotationLune->isChecked()) << endl <<
                "affichage/systemeHoraire " << BOOL_STR(ui->syst24h->isChecked()) << endl <<
                "affichage/typeParametres " << ui->typeParametres->currentIndex() << endl <<
                "affichage/typeRepere " << ui->typeRepere->currentIndex() << endl <<
                "affichage/unite " << BOOL_STR(ui->unitesKm->isChecked()) << endl <<
                "affichage/utc " << BOOL_STR(ui->utc->isChecked()) << endl <<
                "affichage/utcAuto " << BOOL_STR(ui->utcAuto->isChecked()) << endl <<
                "affichage/verifMAJ " << BOOL_STR(ui->verifMAJ->isChecked()) << endl <<
                "fichier/affichageMsgMAJ " << ui->affichageMsgMAJ->currentIndex() << endl <<
                "affichage/mccISS " << BOOL_STR(ui->mccISS->isChecked()) << endl <<
                "affichage/affBetaWCC " << BOOL_STR(ui->affBetaWCC->isChecked()) << endl <<
                "affichage/affCerclesAcq " << BOOL_STR(ui->affCerclesAcq->isChecked()) << endl <<
                "affichage/affNbOrbWCC " << BOOL_STR(ui->affNbOrbWCC->isChecked()) << endl <<
                "affichage/aff_ZOE " << BOOL_STR(ui->affSAA_ZOE->isChecked()) << endl <<
                "affichage/chaine " << ui->chaine->value() << endl <<
                "affichage/styleWCC " << BOOL_STR(ui->styleWCC->isChecked()) << endl <<
                "affichage/coulGMT " << ui->coulGMT->currentIndex() << endl <<
                "affichage/coulZOE " << ui->coulZOE->currentIndex() << endl <<
                "affichage/coulCercleVisibilite " << ui->coulCercleVisibilite->currentIndex() << endl <<
                "affichage/coulEquateur " << ui->coulEquateur->currentIndex() << endl <<
                "affichage/coulTerminateur " << ui->coulTerminateur->currentIndex() << endl <<
                "affichage/policeWCC " << ui->policeWCC->currentIndex() << endl;

        for(int i=0; i<ui->listeStations->count(); i++) {
            flux << "affichage/station" + QString::number(i) + " " + settings.value("affichage/station" + QString::number(i)).toString() << endl;
        }

        sw.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}


/***********
 * Systeme *
 **********/
/*
 * Decompression d'un fichier TLE au format gz
 */
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
        while (gzgets(fichGz, buffer, 8192) != NULL) {
            flux << buffer;
        }
        gzclose(fichGz);
        fichDec.close();
        res = true;
    }

    /* Retour */
    return (res);
}

/*
 * Ecriture de la liste de satellites dans la base de registre
 */
void PreviSat::EcritureListeRegistre() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString listeTLEs = "";

    /* Corps de la methode */
    try {
        if (listeTLE.length() > 0) {
            listeTLEs = listeTLE.at(0);
        }

        if (nbSat > 0) {

            for (int i=1; i<nbSat; i++) {
                listeTLEs.append("&").append(listeTLE.at(i));
            }

            // Recuperation des TLE de la liste
            TLE::LectureFichier(nomfic, listeTLE, tles);
            for (int i=0; i<nbSat; i++) {
                if (tles.at(i).norad().isEmpty()) {
                    throw PreviSatException();
                }
            }
            nom = tles.at(0).nom();
            l1 = tles.at(0).ligne1();
            l2 = tles.at(0).ligne2();

            // Recuperation des donnees satellites
            Satellite::LectureDonnees(listeTLE, tles, satellites);

            if (tles.at(0).nom().isEmpty()) {
                settings.setValue("TLE/nom", nom);
            } else {
                settings.setValue("TLE/nom", tles.at(0).nom());
            }

            settings.setValue("TLE/l1", tles.at(0).ligne1());
            settings.setValue("TLE/l2", tles.at(0).ligne2());
            settings.setValue("TLE/nbsat", nbSat);
        }

        QStringListIterator it(liste.split("$"));
        while (it.hasNext()) {

            const QString ficTle = it.next();

            const QString fic = nomfic.mid(nomfic.lastIndexOf(QDir::separator())+1);
            if (nomfic == dirTle + QDir::separator() + ficTle.split("#").at(0)) {
                const int ind1 = liste.indexOf(fic);
                const int ind2 = (liste.indexOf("$", ind1) == -1) ? liste.length() : liste.indexOf("$", ind1) - ind1;
                liste = liste.replace(ind1, ind2, fic + "#" + listeTLEs);
            } else {
                if (!liste.contains(fic)) {
                    liste.append("$" + fic + "#" + listeTLEs);
                }
            }
        }
        settings.setValue("TLE/liste", liste);

    } catch (PreviSatException &e) {

        try {
            TLE::VerifieFichier(nomfic, true);

            AfficherListeSatellites(nomfic, listeTLE);
            nbSat = getListeItemChecked(ui->liste1);

            nor = listeTLE.at(0);

            int j = 0, k = 0;
            tles.clear();
            listeTLE.clear();
            tles.resize(nbSat);
            bipSatAOS.resize(nbSat);
            bipSatLOS.resize(nbSat);

            if (nbSat == 0) {
                l1 = "";
                l2 = "";
            }

            for (int i=0; i<nbSat; i++) {
                if (ui->liste1->item(i)->checkState() == Qt::Checked) {
                    listeTLE[k] = ui->liste1->item(i)->text().split("#").at(1);
                    if (nor == listeTLE.at(k)) {
                        j = k;
                    }
                    k++;
                }
            }
            if (j > 0) {
                listeTLE[j] = listeTLE.at(0);
                listeTLE[0] = nor;
            }
            TLE::LectureFichier(nomfic, listeTLE, tles);

        } catch (PreviSatException &ex) {
        }
    }

    /* Retour */
    return;
}

/*
 * Fin des calculs de previsions
 */
void PreviSat::CalculsTermines()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QFileInfo fi(ficRes);
    switch (threadCalculs->typeCalcul()) {

    case PREVISION:

        ui->calculsPrev->setEnabled(true);
        if (fi.exists()) {
            ui->calculsPrev->setDefault(false);
            ui->afficherPrev->setEnabled(true);
            ui->afficherPrev->setDefault(true);
            ui->afficherPrev->setFocus();
        }
        break;

    case EVENEMENTS:

        ui->calculsEvt->setEnabled(true);
        if (fi.exists()) {
            ui->calculsEvt->setDefault(false);
            ui->afficherEvt->setEnabled(true);
            ui->afficherEvt->setDefault(true);
            ui->afficherEvt->setFocus();
        }
        break;

    case TRANSITS:

        ui->calculsTransit->setEnabled(true);
        if (fi.exists()) {
            ui->calculsTransit->setDefault(false);
            ui->afficherTransit->setEnabled(true);
            ui->afficherTransit->setDefault(true);
            ui->afficherTransit->setFocus();
        }
        break;

    case METOP:

        ui->calculsMetOp->setEnabled(true);
        if (fi.exists()) {
            ui->calculsMetOp->setDefault(false);
            ui->afficherMetOp->setEnabled(true);
            ui->afficherMetOp->setDefault(true);
            ui->afficherMetOp->setFocus();
            settings.setValue("fichier/fichierTLEMetOp",
                              QDir::toNativeSeparators(ficTLEMetOp.at(ui->fichierTLEMetOp->currentIndex())));
        }
        break;

    default:
        break;
    }
    threadCalculs->deleteLater();
    threadCalculs = NULL;
    messagesStatut->setText((fi.exists()) ? tr("Terminé !") : "");

    /* Retour */
    return;
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
    if (afficherResultats != NULL && !afficherResultats->isVisible()) {

        if (threadCalculs != NULL) {
            threadCalculs->deleteLater();
            threadCalculs = NULL;
        }

        afficherResultats->deleteLater();
        afficherResultats = NULL;
    }

    if (afficherMeteo != NULL && !afficherMeteo->isVisible()) {

        if (viewMeteo != NULL) {
            viewMeteo->deleteLater();
            viewMeteo = NULL;
        }

        if (viewMeteoNASA != NULL) {
            viewMeteoNASA->deleteLater();
            viewMeteoNASA = NULL;
        }

        afficherMeteo->deleteLater();
        afficherMeteo = NULL;
    }

    if (afficherManoeuvre != NULL && afficherManoeuvre->isHidden()) {

        if (tableMan != NULL) {
            tableMan->deleteLater();
            tableMan = NULL;
        }

        afficherManoeuvre->deleteLater();
        afficherManoeuvre = NULL;
    }

    /* Corps de la methode */
    if (ui->tempsReel->isChecked()) {

        modeFonctionnement->setText(tr("Temps réel"));
        const double offset = Date::CalculOffsetUTC(QDateTime::currentDateTime());
        offsetUTC = (fabs(offsetUTC - offset) < EPSDBL100) ? offset : offsetUTC;
        if (ui->heureLegale->isChecked()) {
            ui->updown->setValue(sgn(offsetUTC) * (qRound(fabs(offsetUTC) * NB_MIN_PAR_JOUR)));
        }
        date1 = Date(dateCourante.offsetUTC());
        pas1 = ui->pasReel->currentText().toDouble();
        pas2 = 0.;
    } else {

        modeFonctionnement->setText(tr("Mode manuel"));
        date1 = dateCourante;
        if (ui->valManuel->currentIndex() < 3) {
            pas1 = ui->pasManuel->currentText().toDouble() * qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) * NB_JOUR_PAR_SEC;
        } else {
            pas1 = ui->pasManuel->currentText().toDouble();
        }
        pas2 = (!ui->backward->isEnabled() || !ui->forward->isEnabled()) ? 0. : pas1 * NB_SEC_PAR_JOUR;
    }

    const QDateTime dateTime = Date(date1.jourJulienUTC() + date1.offsetUTC() + EPS_DATES, 0.).ToQDateTime(1);
    date2 = Date(dateTime.date().year(), 1, 1., 0.);

    if (ui->calJulien->isChecked()) {

        // Affichage du jour julien
        stsDate->setText(QString::number(date1.jourJulien() + TJ2000, 'f', 5));
        stsHeure->setText(QString::number(date1.jourJulien() - date2.jourJulien() + 1., 'f', 5));
        stsDate->setToolTip(tr("Jour julien"));
        stsHeure->setToolTip(tr("Jour"));

    } else {

        // Affichage de la date et l'heure
        stsDate->setText(dateTime.date().toString(tr("dd/MM/yyyy")));
        stsHeure->setText(dateTime.time().toString(QString("hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "")));
        stsDate->setToolTip(tr("Date"));
        stsHeure->setToolTip(tr("Heure"));
    }

    // Lancement des calculs
    if (ui->tempsReel->isChecked() && tim.secsTo(QDateTime::currentDateTimeUtc()) >= pas1) {

        tim = (tim.addSecs((int) pas1) <= QDateTime::currentDateTimeUtc()) ? tim.addSecs((int) pas1) : QDateTime::currentDateTimeUtc();

        // Date actuelle
        dateCourante = Date(dateCourante.offsetUTC());

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        // Donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    if (ui->modeManuel->isChecked() && fabs((double) tim.secsTo(QDateTime::currentDateTimeUtc())) >=  pas2 - EPSDBL100) {

        tim = QDateTime::currentDateTimeUtc();
        info = true;
        acalcAOS = true;
        acalcDN = true;

        if (ui->pause->isEnabled()) {

            if (!ui->pasManuel->view()->isVisible()) {

                double jd = dateCourante.jourJulienUTC();
                if (!ui->rewind->isEnabled() || !ui->backward->isEnabled()) {
                    jd -= pas1;
                }
                if (!ui->play->isEnabled() || !ui->forward->isEnabled()) {
                    jd += pas1;
                }

                dateCourante = Date(jd + EPS_DATES, dateCourante.offsetUTC());

                // Enchainement de l'ensemble des calculs
                EnchainementCalculs();

                const QString fmt = tr("dddd dd MMMM yyyy  hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "");
                if (ui->dateHeure4->isVisible()) {
                    ui->dateHeure4->setDisplayFormat(fmt);
                    ui->dateHeure4->setDateTime(dateCourante.ToQDateTime(1));
                } else {
                    ui->dateHeure3->setDisplayFormat(fmt);
                    ui->dateHeure3->setDateTime(dateCourante.ToQDateTime(1));
                }
            }
        }
    }

#if !((defined (Q_OS_MAC) || defined (Q_OS_WIN)) && QT_VERSION < 0x500000)
    ui->lbl_chaine->setVisible(ui->fluxVideoHtml->isVisible());
    ui->chaine->setVisible(ui->fluxVideoHtml->isVisible());
    ui->frameCtrlVideo->setVisible(ui->fluxVideoHtml->isVisible());
#endif

    /* Retour */
    return;
}


/******************************
 * Telechargement de fichiers *
 *****************************/
/*
 * Ecriture du fichier telecharge
 */
void PreviSat::EcritureFichier()
{
    ficDwn.write(rep->readAll());
}

/*
 * Gestion de l'enregistrement des fichiers telecharges
 */
void PreviSat::FinEnregistrementFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        bool atr = false;
        bool aup = false;
        ficDwn.close();

        QFile fd(ficDwn.fileName());
        fd.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flx(&fd);
        const QString lg = flx.readLine();
        fd.close();
        if (lg.contains("DOCTYPE")) {
            atr = true;
        }

        const QFileInfo ff(ficDwn.fileName());
        if ((!amajPrevi && (rep->error() || atr)) || lg.isEmpty()) {

            // Erreur survenue lors du telechargement
            ui->frameBarreProgression->setVisible(false);

            if (fd.exists()) {
                fd.remove();
            }

            if (aupdateCF5) {
                cptCF5++;
            } else {
                const QString fic = ff.fileName();
                if (fic != "versionPreviSat" && fic != "majFicInt") {
                    QString msg = tr("Erreur lors du téléchargement du fichier %1");
                    if (rep->error()) {
                        msg += " : " + rep->errorString();
                    }
                    Message::Afficher(msg.arg(ff.fileName()), WARNING);
                }
                ui->compteRenduMaj->setVisible(false);
            }

        } else {

            // Mise a jour des TLE
            if (atrouve) {

                if (QDir::toNativeSeparators(ficDwn.fileName()) == nomfic) {

                    const int nb = TLE::VerifieFichier(nomfic, false);
                    if (nb == 0) {
                        const QString msg = tr("Erreur lors du téléchargement du fichier %1");
                        Message::Afficher(msg.arg(nomfic), WARNING);
                        ui->compteRenduMaj->setVisible(false);
                    } else {

                        // Recuperation des TLE de la liste
                        TLE::LectureFichier(nomfic, listeTLE, tles);

                        info = true;
                        acalcAOS = true;
                        acalcDN = true;
                        Satellite::initCalcul = false;
                        Satellite::LectureDonnees(listeTLE, tles, satellites);

                        AfficherListeSatellites(nomfic, listeTLE, false);

                        CalculsAffichage();
                    }
                }
            }

            // Mise a jour des fichiers TLE selectionnes
            if (aupdnow && !lg.contains("HTML")) {

                QString fichierALire = QDir::toNativeSeparators(ficDwn.fileName());
                QFileInfo ff2(fichierALire);
                QString fichierAMettreAJour = QDir::toNativeSeparators(dirTle + QDir::separator() + ff2.fileName());

                QFile fi(fichierAMettreAJour);
                if (fi.exists() && fi.size() > 0) {

                    QStringList compteRendu;
                    const int affMsg = ui->affichageMsgMAJ->currentIndex();
                    try {
                        TLE::MiseAJourFichier(fichierAMettreAJour, fichierALire, affMsg, compteRendu);
                        bool aecr = false;
                        EcritureCompteRenduMaj(compteRendu, aecr);
                        aup = true;

                    } catch (PreviSatException &ex) {
                    }

                } else {
                    const QString msg = (fi.exists() && (fi.size() == 0)) ? tr("Remplacement du fichier %1") : tr("Ajout du fichier %1");
                    if (fi.size() == 0) fi.remove();
                    fi.copy(fichierALire, fichierAMettreAJour);
                    ui->compteRenduMaj->appendPlainText(msg.arg(ff.fileName()));
                    InitFicTLE();
                }

                if (fichierAMettreAJour == nomfic) {

                    const int nb = TLE::VerifieFichier(nomfic, false);
                    if (nb == 0) {
                        const QString msg = tr("Erreur lors du téléchargement du fichier %1");
                        Message::Afficher(msg.arg(ff.fileName()), WARNING);
                    } else {

                        // Recuperation des TLE de la liste
                        TLE::LectureFichier(nomfic, listeTLE, tles);

                        info = true;
                        acalcAOS = true;
                        acalcDN = true;
                        Satellite::initCalcul = false;
                        Satellite::LectureDonnees(listeTLE, tles, satellites);

                        AfficherListeSatellites(nomfic, listeTLE, false);

                        CalculsAffichage();
                    }
                }
            }

            const QString hsf = ISS_TRAJECTOIRE_NASA.split("/", QString::SkipEmptyParts).last();
            const QString ficHsf = dirTmp + QDir::separator() + hsf;
            if (ff.fileName() == hsf) {

                // Creation du fichier iss.3le (les lignes sont verifiees avant l'ecriture du fichier)
                const QString fichier3leIss = dirTle + QDir::separator() + "iss.3le";
                TLE::LectureTrajectoryData(ficHsf, fichier3leIss, tabManoeuvresISS);

                // Affichage des manoeuvres ISS
                if (!tabManoeuvresISS.isEmpty()) {
                    AffichageManoeuvresISS();
                }
            }
        }

        if (aup) {
            rep->deleteLater();
        }
        TelechargementSuivant();

    } catch (PreviSatException &e) {
        ui->frameBarreProgression->setVisible(false);
        ui->majMaintenant->setEnabled(true);
        ui->compteRenduMaj->setVisible(false);
    }

    /* Retour */
    return;
}

/*
 * Progression du telechargement
 */
void PreviSat::ProgressionTelechargement(qint64 recu, qint64 total) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (total != -1) {
        ui->barreProgression->setRange(0, (int) total);
        ui->barreProgression->setValue((int) recu);
    }

    /* Retour */
    return;
}

/*
 * Demarrage du telechargement suivant
 */
void PreviSat::TelechargementSuivant()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (downQueue.isEmpty()) {

        emit TelechargementFini();

        if (aupdateCF5 && cptCF5 > 0) {
            QString msg = tr("Erreur lors du téléchargement des fichiers TLE");
            if (rep->error()) {
                msg += " : " + rep->errorString();
            }
            Message::Afficher(msg, WARNING);
        }

        atrouve = false;
        aupdnow = false;
        ui->majMaintenant->setEnabled(true);
        ui->frameBarreProgression->setVisible(false);
        ui->affichageMsgMAJ->setVisible(true);
        messagesStatut->setText(tr("Terminé !"));

    } else {

        const QUrl url = downQueue.dequeue();
        const QString dirHttpPrevi =
                settings.value("fichier/dirHttpPrevi", "").toString().trimmed().replace(adresseAstropedia, "/") + "commun/data/";
        const QString fic = (url.path().contains(dirHttpPrevi)) ? url.path().replace(dirHttpPrevi, "") : QFileInfo(url.path()).fileName();

        if (fic.endsWith("txt")) {
            ui->fichierTelechargement->setText(fic.mid(fic.indexOf("/") + 1));
        } else {
            ui->fichierTelechargement->setText("TLE ISS");
        }
        if (ui->miseAJourTLE->isVisible() && aupdnow) {
            ui->barreProgression->setValue(0);
            ui->frameBarreProgression->setVisible(true);
            ui->compteRenduMaj->setVisible(true);
        }
        ficDwn.setFileName(dirDwn + QDir::separator() + fic);

        if (ficDwn.open(QIODevice::WriteOnly)) {

            QNetworkProxyFactory::setUseSystemConfiguration(true);
            const QNetworkRequest requete(url);
            if (!amajDeb) {
                rep = mng.get(requete);
            }

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


/*
 * Fin de l'application
 */
void PreviSat::closeEvent(QCloseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    const QDir di = QDir(dirTmp);
    const QStringList listeFic = di.entryList(QDir::Files);
    if (listeFic.count() > 0) {
        foreach(QString fic, listeFic) {
            if ((ui->verifMAJ->isChecked() && (fic == "versionPreviSat" || fic == "majFicInt")) ||
                    fic == ISS_TRAJECTOIRE_NASA.split("/", QString::SkipEmptyParts).last()) {
                continue;
            } else {
                QFile fi(dirTmp + QDir::separator() + fic);
                fi.remove();
            }
        }
    }

    // Sauvegarde des donnees du logiciel
    settings.setValue("temps/valManuel", ui->valManuel->currentIndex());
    settings.setValue("temps/pasManuel", ui->pasManuel->currentIndex());
    settings.setValue("temps/pasReel", ui->pasReel->currentIndex());
    settings.setValue("temps/dtu", ui->updown->value() * NB_JOUR_PAR_MIN);

    settings.setValue("affichage/rechercheDonneesSat", ui->rechercheDonneesSat->isChecked());
    settings.setValue("affichage/affMax", ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored);
    settings.setValue("affichage/affSAA", ui->affSAA->isChecked());
    settings.setValue("affichage/affconst", ui->affconst->checkState());
    settings.setValue("affichage/affcoord", ui->affcoord->isChecked());
    settings.setValue("affichage/affetoiles", ui->affetoiles->isChecked());
    settings.setValue("affichage/affgrille", ui->affgrille->isChecked());
    settings.setValue("affichage/afficone", ui->afficone->isChecked());
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
    settings.setValue("affichage/affsoleil", ui->affsoleil->isChecked());
    settings.setValue("affichage/afftraceCiel", ui->afftraceCiel->isChecked());
    settings.setValue("affichage/afftraj", ui->afftraj->isChecked());
    settings.setValue("affichage/affvisib", ui->affvisib->checkState());
    settings.setValue("affichage/calJulien", ui->calJulien->isChecked());
    settings.setValue("affichage/eclipsesLune", ui->eclipsesLune->isChecked());
    settings.setValue("affichage/effetEclipsesMagnitude", ui->effetEclipsesMagnitude->isChecked());
    settings.setValue("affichage/extinction", ui->extinctionAtmospherique->isChecked());
    settings.setValue("affichage/fenetreMax", isMaximized());
    settings.setValue("affichage/groupeTLE", ui->groupeTLE->currentIndex());
    settings.setValue("affichage/hauteur", height());
    settings.setValue("affichage/intensiteOmbre", ui->intensiteOmbre->value());
    settings.setValue("affichage/intensiteVision", ui->intensiteVision->value());
    settings.setValue("affichage/largeur", width());
    settings.setValue("affichage/magnitudeEtoiles", ui->magnitudeEtoiles->value());
    settings.setValue("affichage/nombreTrajectoires", ui->nombreTrajectoires->value());
    settings.setValue("affichage/proportionsCarte", ui->proportionsCarte->isChecked());
    settings.setValue("affichage/refractionPourEclipses", ui->refractionPourEclipses->isChecked());
    settings.setValue("affichage/rotationIconeISS", ui->rotationIconeISS->isChecked());
    settings.setValue("affichage/rotationLune", ui->rotationLune->isChecked());
    settings.setValue("affichage/systemeHoraire", ui->syst24h->isChecked());
    settings.setValue("affichage/typeParametres", ui->typeParametres->currentIndex());
    settings.setValue("affichage/typeRepere", ui->typeRepere->currentIndex());
    settings.setValue("affichage/unite", ui->unitesKm->isChecked());
    settings.setValue("affichage/utc", ui->utc->isChecked());
    settings.setValue("affichage/utcAuto", ui->utcAuto->isChecked());
    settings.setValue("affichage/verifMAJ", ui->verifMAJ->isChecked());

    settings.setValue("affichage/mccISS", ui->mccISS->isChecked());
    settings.setValue("affichage/affBetaWCC", ui->affBetaWCC->isChecked());
    settings.setValue("affichage/affCerclesAcq", ui->affCerclesAcq->isChecked());
    settings.setValue("affichage/affNbOrbWCC", ui->affNbOrbWCC->isChecked());
    settings.setValue("affichage/affSAA_ZOE", ui->affSAA_ZOE->isChecked());
    settings.setValue("affichage/chaine", ui->chaine->value());
    settings.setValue("affichage/styleWCC", ui->styleWCC->isChecked());
    settings.setValue("affichage/coulGMT", ui->coulGMT->currentIndex());
    settings.setValue("affichage/coulZOE", ui->coulZOE->currentIndex());
    settings.setValue("affichage/coulCercleVisibilite", ui->coulCercleVisibilite->currentIndex());
    settings.setValue("affichage/coulEquateur", ui->coulEquateur->currentIndex());
    settings.setValue("affichage/coulTerminateur", ui->coulTerminateur->currentIndex());
    settings.setValue("affichage/policeWCC", ui->policeWCC->currentIndex());

    for(int i=0; i<ui->listeStations->count(); i++) {
        settings.setValue("affichage/station" + QString::number(i), ui->listeStations->item(i)->checkState());
    }

    settings.setValue("fichier/listeMap", (ui->listeMap->currentIndex() > 0) ?
                          ficMap.at(qMax(0, ui->listeMap->currentIndex() - 1)) : "");
    settings.setValue("fichier/listeSon", (ui->listeSons->currentIndex() > 0) ?
                          ficSonAOS.at(qMax(0, ui->listeSons->currentIndex() - 1)) : "");
    settings.setValue("fichier/nom", (ficgz.isEmpty()) ? QDir::toNativeSeparators(nomfic) : QDir::toNativeSeparators(ficgz));
    settings.setValue("fichier/fichierAMettreAJour", ui->fichierAMettreAJour->text());
    settings.setValue("fichier/fichierALire", ui->fichierALire->text());
    settings.setValue("fichier/affichageMsgMAJ", ui->affichageMsgMAJ->currentIndex());
    settings.setValue("fichier/fichierALireCreerTLE", ui->fichierALireCreerTLE->text());
    settings.setValue("fichier/nomFichierPerso", ui->nomFichierPerso->text());

    settings.setValue("previsions/pasGeneration", ui->pasGeneration->currentIndex());
    settings.setValue("previsions/lieuxObservation2", ui->lieuxObservation2->currentIndex());
    settings.setValue("previsions/hauteurSatPrev", ui->hauteurSatPrev->currentIndex());
    settings.setValue("previsions/valHauteurSatPrev", ui->valHauteurSatPrev->text().toInt());
    settings.setValue("previsions/hauteurSoleilPrev", ui->hauteurSoleilPrev->currentIndex());
    settings.setValue("previsions/valHauteurSoleilPrev", ui->valHauteurSoleilPrev->text().toInt());
    settings.setValue("previsions/illuminationPrev", ui->illuminationPrev->isChecked());
    settings.setValue("previsions/magnitudeMaxPrev", ui->magnitudeMaxPrev->isChecked());

    settings.setValue("previsions/passageApogee", ui->passageApogee->isChecked());
    settings.setValue("previsions/passageNoeuds", ui->passageNoeuds->isChecked());
    settings.setValue("previsions/passageOmbre", ui->passageOmbre->isChecked());
    settings.setValue("previsions/passageQuadrangles", ui->passageQuadrangles->isChecked());
    settings.setValue("previsions/transitionJourNuit", ui->transitionJourNuit->isChecked());

    settings.setValue("previsions/hauteurSatTransit", ui->hauteurSatTransit->currentIndex());
    settings.setValue("previsions/valHauteurSatTransit", ui->valHauteurSatTransit->text().toInt());
    settings.setValue("previsions/lieuxObservation4", ui->lieuxObservation4->currentIndex());
    settings.setValue("previsions/ageMaxTLETransit", ui->ageMaxTLETransit->value());
    settings.setValue("previsions/elongationMaxCorps", ui->elongationMaxCorps->value());

    settings.setValue("previsions/hauteurSatMetOp", ui->hauteurSatMetOp->currentIndex());
    settings.setValue("previsions/hauteurSoleilMetOp", ui->hauteurSoleilMetOp->currentIndex());
    settings.setValue("previsions/lieuxObservation5", ui->lieuxObservation5->currentIndex());
    settings.setValue("previsions/ordreChronologiqueMetOp", ui->ordreChronologiqueMetOp->isChecked());
    settings.setValue("previsions/magnitudeMaxMetOp", ui->magnitudeMaxMetOp->value());
    settings.setValue("previsions/affichage3lignesMetOp", ui->affichage3lignesMetOp->isChecked());

    if (!ui->verifMAJ->isChecked()) {
        settings.setValue("fichier/majPrevi", "0");
    }

    EcritureListeRegistre();

    if (ui->preferences->currentIndex() < ui->preferences->count() - 2) {
        SauvePreferences(ficPrf.at(ui->preferences->currentIndex()));
    }

    if (afficherResultats != NULL) {
        afficherResultats->close();
    }

    if (afficherMeteo != NULL) {
        afficherMeteo->close();
    }

    if (afficherManoeuvre != NULL) {
        afficherManoeuvre->close();
    }

    donneesSat = "";

    /* Retour */
    return;
}

/*
 * Redimensionnement de la fenetre
 */
void PreviSat::resizeEvent(QResizeEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)
    const bool wcc = ui->mccISS->isChecked() && ui->styleWCC->isChecked();

    /* Corps de la methode */
    if (ui->frameCarte->height() >= height()) {
        ui->frameCarte->resize(ui->frameCarte->width(), height() - 23);
    }

    if (ui->frameCarte->width() != ui->frameCarteListe->width() - ui->frameListe->width() - 5 &&
            ui->frameListe->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored) {
        ui->frameCarte->resize(ui->frameCarteListe->width() - ui->frameListe->width() - 5, ui->frameCarte->height());
    }

    if (ui->proportionsCarte->isChecked()) {
        const int href = 2 * (ui->frameCarte->height() - 26);
        const int lref = ui->frameCarte->width() - 50;
        const int lc = qMin(href, lref);
        const int hc = lc / 2;
        ui->carte->setGeometry((ui->frameCarte->width() - lc) / 2 - 19, 6, lc, hc);
    } else {
        ui->carte->setGeometry(6, 6, ui->frameCarte->width() - 47, ui->frameCarte->height() - 23);
    }

    ui->frameCarte2->setGeometry(ui->carte->geometry());
    if (ui->frameCoordISS->isVisible()) {
        ui->frameCoordISS->move(ui->carte->pos());
    }

    ui->maximise->move(5 + ui->carte->x() + ui->carte->width(), 5);
    ui->affichageCiel->move(5 + ui->carte->x() + ui->carte->width(), 32);

    const int hcarte = ui->carte->height() - 3;
    const int lcarte = ui->carte->width() - 3;

    DEG2PXHZ = lcarte * (1. / T360);
    DEG2PXVT = hcarte * (2. / T360);

    ui->gmt->adjustSize();
    ui->gmt->move((ui->carte->width() - ui->gmt->width()) / 2, (int) (15. * DEG2PXVT) + 2);

    ui->liste1->resize(211, hcarte - 147);

    ui->S60->move(5, (int) (hcarte * (1. / 1.2)) - 1);
    ui->S30->move(5, (int) (hcarte * (1. / 1.5)) - 1);
    ui->SS->move(5, (int) (hcarte * (7. / 12.)) - 1);
    ui->N0->move(5, (int) (hcarte * 0.5) - 1);
    ui->NN->move(5, (int) (hcarte * (1. / 2.4)) - 1);
    ui->N30->move(5, (int) (hcarte * (1. / 3.)) - 1);
    ui->N60->move(5, (int) (hcarte * (1. / 6.)) - 1);
    const int xLat = (wcc) ? -26 : 1;
    ui->frameLat->setGeometry(xLat + ui->carte->x() + ui->carte->width(), 0, ui->frameLat->width(), ui->carte->height());

    if (ui->mccISS->isChecked()) {
        ui->frameLat2->setGeometry(ui->carte->x(), 0, ui->frameLat->width(), ui->carte->height());
        ui->S60b->move(5, ui->S60->y());
        ui->S30b->move(5, ui->S30->y());
        ui->N0b->move(5, ui->N0->y());
        ui->N30b->move(5, ui->N30->y());
        ui->N60b->move(5, ui->N60->y());
        ui->frameCoordISS->move(ui->carte->pos());
        const bool chk = ui->affBetaWCC->isChecked() && ui->affNbOrbWCC->isChecked();
        QFont police(ui->policeWCC->currentText());

#if defined (Q_OS_WIN)
        const int taille = 10;

#elif defined (Q_OS_LINUX)
        const int taille = 11;

#elif defined (Q_OS_MAC)
        const int taille = 13;

#else
        const int taille = 11;
#endif

        int w2 = 0;
        if (ui->frameListe->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored) {

            const int w = qMax(ui->frameCoordISS->minimumWidth(), (int) (lcarte * 0.25) + 1);
            const int h = qMax(ui->frameCoordISS->minimumHeight(), hcarte / 8 + 1);
            ui->frameCoordISS->resize(w, h);

            const int h2 = (int) ((chk) ? 0.23 * h : 0.32 * h);
            const int h3 = 2 * h2;
            const int h4 = 3 * h2;
            w2 = w / 2;

            police.setPointSize((chk) ? taille : taille + (int) (h * 0.125) - 7);

            ui->altitudeISS->move(5, h2);
            ui->longitudeISS->move(5, h3);

            ui->inclinaisonISS->move((chk) ? QPoint(5, h4) : QPoint(w2, 0));
            ui->nextTransitionISS->move((chk) ? QPoint(w2, 0) : QPoint(w2, h2));
            ui->orbiteISS->move((chk) ? QPoint(w2, h2) : QPoint(w2, h3));
            ui->betaISS->move(w2, h3);

        } else {

            police.setPointSize(taille);
            ui->frameCoordISS->resize(ui->frameCoordISS->minimumWidth(), (chk) ? 59 : 46);
            w2 = ui->frameCoordISS->width() / 2;

            ui->altitudeISS->move(5, 13);
            ui->longitudeISS->move(5, 26);

            ui->inclinaisonISS->move((chk) ? QPoint(5, 39) : QPoint(112, 0));
            ui->nextTransitionISS->move((chk) ? QPoint(112, 0) : QPoint(112, 13));
            ui->orbiteISS->move((chk) ? QPoint(112, 13) : QPoint(112, 26));
            ui->betaISS->move(112, 26);
        }

        ui->latitudeISS->resize(w2, ui->latitudeISS->height());
        ui->altitudeISS->resize(w2, ui->altitudeISS->height());
        ui->longitudeISS->resize(w2, ui->longitudeISS->height());
        ui->inclinaisonISS->resize(w2, ui->inclinaisonISS->height());
        ui->nextTransitionISS->resize(w2, ui->nextTransitionISS->height());
        ui->orbiteISS->resize(w2, ui->orbiteISS->height());
        ui->betaISS->resize(w2, ui->betaISS->height());

        ui->altitudeISS->setFont(police);
        ui->betaISS->setFont(police);
        ui->inclinaisonISS->setFont(police);
        ui->latitudeISS->setFont(police);
        ui->longitudeISS->setFont(police);
        ui->nextTransitionISS->setFont(police);
        ui->orbiteISS->setFont(police);
    }

    const int dec1 = (wcc) ? 12 : 8;
    const int dec2 = (wcc) ? 9 : 5;
    ui->W150->move((int) (lcarte * (1. / 12.)) - dec1, 0);
    ui->W120->move((int) (lcarte * (1. / 6.)) - dec1, 0);
    ui->W90->move((int) (lcarte * (1. / 4.)) - dec2, 0);
    ui->W60->move((int) (lcarte * (1. / 3.)) - dec2, 0);
    ui->W30->move((int) (lcarte * (1. / 2.4)) - dec2, 0);
    ui->WW->move((int) (lcarte * (11. / 24.)) - 2, 0);
    ui->W0->move((int) (lcarte * 0.5) - 2, 0);
    ui->EE->move((int) (lcarte * (13. / 24.)) - 2, 0);
    ui->E30->move((int) (lcarte * (7. / 12.)) - 5, 0);
    ui->E60->move((int) (lcarte * (1. / 1.5)) - 5, 0);
    ui->E90->move((int) (lcarte * 0.75) - 5, 0);
    ui->E120->move((int) (lcarte * (1. / 1.2)) - 8, 0);
    ui->E150->move((int) (lcarte * (11. / 12.)) - 8, 0);
    const int yLon = (wcc) ? -10 : 1 + ui->carte->y();
    ui->frameLon->setGeometry(ui->carte->x(), yLon + ui->carte->height(), ui->carte->width(), ui->frameLon->height());

    const int xOng = (ui->mccISS->isChecked()) ? 6 : ui->frameCarte->width() / 2 - 411;
    ui->onglets->move(xOng, 0);

    ui->frameZone->resize(width() - ui->onglets->width(), ui->frameZone->height());
    ui->frameZoneVideo->resize(ui->frameZone->width() - 18, ui->frameZone->height() - 24);
    ui->fluxVideoHtml->resize(ui->frameZoneVideo->width(), ui->frameZoneVideo->height() - 20);
    ui->frameCtrlVideo->move(ui->frameZoneVideo->width() - ui->frameCtrlVideo->width() + 2, 0);
    ui->fluxVideo->move((ui->frameZoneVideo->width() - ui->fluxVideo->width()) / 2,
                        (ui->frameZone->height() - ui->fluxVideo->height()) / 2);
    ui->lbl_video->move(ui->fluxVideoHtml->pos());
    ui->lbl_video->resize(ui->fluxVideoHtml->size());
    ui->lbl_chaine->move((ui->frameZoneVideo->width() - ui->lbl_chaine->width() - ui->chaine->width()) / 2, ui->lbl_chaine->y());
    ui->chaine->move(ui->lbl_chaine->x() + 45, ui->chaine->y());

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
void PreviSat::keyPressEvent(QKeyEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt = tr("dddd dd MMMM yyyy  hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "");

#if defined (Q_OS_WIN)
    const QString nomRepDefaut = settings.value("fichier/sauvegarde", dirOut).toString().replace(QDir::separator(), "\\");
    const Qt::Key etape_av = Qt::Key_F11;
    const Qt::Key etape_ap = Qt::Key_F12;

#elif defined (Q_OS_LINUX)
    const QString nomRepDefaut = settings.value("fichier/sauvegarde", dirOut).toString();
    const Qt::Key etape_av = Qt::Key_F11;
    const Qt::Key etape_ap = Qt::Key_F12;

#elif defined (Q_OS_MAC)
    const QString nomRepDefaut = settings.value("fichier/sauvegarde", dirOut).toString();
    const Qt::Key etape_av = Qt::Key_F6;
    const Qt::Key etape_ap = Qt::Key_F7;

#else
    const QString nomRepDefaut = settings.value("fichier/sauvegarde", dirOut).toString();
    const Qt::Key etape_av = Qt::Key_F11;
    const Qt::Key etape_ap = Qt::Key_F12;
#endif

    /* Corps de la methode */
    if (evt->key() == Qt::Key_F5) {

        // Mise a jour du fichier TLE courant
        MajFichierTLE();

    } else if (evt->key() == Qt::Key_F8) {

        // Capture de la fenetre
        chronometre->stop();
        const QPixmap image = QPixmap::grabWidget(QApplication::activeWindow());

        const QString nomFicDefaut = nomRepDefaut + QDir::separator() + "previsat_" +
                dateCourante.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").remove(":").replace(" ", "_") + "_" +
                ui->tuc->text().remove(" ").remove(":");

        const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), nomFicDefaut,
                                                         tr("Fichiers PNG (*.png);;Fichiers JPEG (*.jpg);;Fichiers BMP (*.bmp);;" \
                                                            "Tous les fichiers (*)"));
        if (!fic.isEmpty()) {
            image.save(fic);
            const QFileInfo fi(fic);
            settings.setValue("fichier/sauvegarde", fi.absolutePath());
        }
        chronometre->start();

    } else if (evt->key() == Qt::Key_F10) {

        // Bascule temps reel / mode manuel
        if (ui->tempsReel->isChecked()) {
            ui->modeManuel->setChecked(true);
        } else {

            ui->tempsReel->setChecked(true);
            tim = QDateTime::currentDateTimeUtc();

            // Date actuelle
            dateCourante = Date(offsetUTC);

            const Date date(dateCourante.jourJulienUTC() + EPS_DATES, offsetUTC);

            if (ui->dateHeure4->isVisible()) {
                ui->dateHeure4->setDisplayFormat(fmt);
                ui->dateHeure4->setDateTime(date.ToQDateTime(1));
            } else {
                ui->dateHeure3->setDisplayFormat(fmt);
                ui->dateHeure3->setDateTime(date.ToQDateTime(1));
                ui->dateHeure3->setFocus();
            }
        }

    } else if (evt->key() == etape_av || evt->key() == etape_ap) {

        // Etape precedente/suivante (mode manuel)
        if (!ui->modeManuel->isChecked()) {
            ui->modeManuel->setChecked(true);
        }
        const int sgnk = (evt->key() == etape_av) ? -1 : 1;

        const double jd = (ui->valManuel->currentIndex() < 3) ? dateCourante.jourJulienUTC() +
                                                                sgnk * ui->pasManuel->currentText().toDouble() *
                                                                qPow(NB_SEC_PAR_MIN, ui->valManuel->currentIndex()) * NB_JOUR_PAR_SEC :
                                                                dateCourante.jourJulienUTC() + sgnk * ui->pasManuel->currentText().
                                                                toDouble();

        const Date date(jd + EPS_DATES, offsetUTC);

        if (ui->dateHeure4->isVisible()) {
            ui->dateHeure4->setDisplayFormat(fmt);
            ui->dateHeure4->setDateTime(date.ToQDateTime(1));
        } else {
            ui->dateHeure3->setDisplayFormat(fmt);
            ui->dateHeure3->setDateTime(date.ToQDateTime(1));
            ui->dateHeure3->setFocus();
        }

    } else {
        QMainWindow::keyPressEvent(evt);
    }

    /* Retour */
    return;
}

/*
 * Clic avec la souris
 */
void PreviSat::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (evt->button() == Qt::LeftButton) {

        // Selection du satellite sur la carte du monde
        if (!ui->carte->isHidden()) {

            const int xCur = evt->x() - ui->carte->x();
            const int yCur = evt->y() - ui->carte->y();
            for (int isat=0; isat<nbSat; isat++) {

                const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ);
                const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT);

                // Distance au carre du curseur au satellite
                const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

                // Le curseur est au-dessus d'un satellite
                if (dt <= 16) {
                    const QString norad = listeTLE.at(isat);
                    listeTLE[isat] = listeTLE.at(0);
                    listeTLE[0] = norad;

                    bool bip = bipSatAOS.at(isat);
                    bipSatAOS[isat] = bipSatAOS.at(0);
                    bipSatAOS[0] = bip;

                    bip = bipSatLOS.at(isat);
                    bipSatLOS[isat] = bipSatLOS.at(0);
                    bipSatLOS[0] = bip;

                    const TLE tle = tles.at(isat);
                    tles[isat] = tles.at(0);
                    tles[0] = tle;

                    const Satellite sat = satellites.at(isat);
                    satellites[isat] = satellites.at(0);
                    satellites[0] = sat;

                    // Ecriture des cles de registre
                    EcritureListeRegistre();

                    info = true;
                    acalcAOS = true;
                    acalcDN = true;

                    CalculsAffichage();

                    const bool affWCC = ui->mccISS->isChecked() && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE && !l1.isEmpty() &&
                            !l2.isEmpty() && !ui->ciel->isVisible();
                    ui->frameCoordISS->setVisible(affWCC);
                    ui->gmt->setVisible(affWCC);
                }
            }
        }

        // Selection du satellite sur le radar
        if (!ui->radar->isHidden()) {

            const int xCur = evt->x() - ui->frameZone->x() - ui->radar->x() - 100;
            const int yCur = evt->y() - ui->frameListe->height() - ui->radar->y() - 100;

            if (xCur * xCur + yCur * yCur < 10000 && htr) {

                const int xf = (ui->affinvew->isChecked()) ? 1 : -1;
                const int yf = (ui->affinvns->isChecked()) ? -1 : 1;

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound(-100. * xf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                            sin(satellites.at(isat).azimut()));
                    const int bsat = qRound(-100. * yf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                            cos(satellites.at(isat).azimut()));

                    // Distance au carre du satellite au curseur
                    const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

                    // Le curseur est au dessus d'un satellite
                    if (dt <= 16) {
                        const QString norad = listeTLE.at(isat);
                        listeTLE[isat] = listeTLE.at(0);
                        listeTLE[0] = norad;

                        bool bip = bipSatAOS.at(isat);
                        bipSatAOS[isat] = bipSatAOS.at(0);
                        bipSatAOS[0] = bip;

                        bip = bipSatLOS.at(isat);
                        bipSatLOS[isat] = bipSatLOS.at(0);
                        bipSatLOS[0] = bip;

                        const TLE tle = tles.at(isat);
                        tles[isat] = tles.at(0);
                        tles[0] = tle;

                        const Satellite sat = satellites.at(isat);
                        satellites[isat] = satellites.at(0);
                        satellites[0] = sat;

                        // Ecriture des cles de registre
                        EcritureListeRegistre();

                        info = true;
                        acalcAOS = true;
                        acalcDN = true;

                        CalculsAffichage();
                    }
                }
            }
        }

        if (!ui->ciel->isHidden()) {

            const int lciel = qRound(0.5 * ui->ciel->width());
            const int hciel = qRound(0.5 * ui->ciel->height());
            const int x1 = evt->x() - ui->ciel->x() - lciel;
            const int y1 = evt->y() - ui->ciel->y() - hciel;

            if (x1 * x1 + y1 * y1 < hciel * lciel) {

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound(-0.5 * ui->ciel->width() * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                            sin(satellites.at(isat).azimut()));
                    const int bsat = qRound(-0.5 * ui->ciel->height() * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                            cos(satellites.at(isat).azimut()));

                    // Distance au carre du satellite au curseur
                    const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                    if (dt <= 16) {
                        const QString norad = listeTLE.at(isat);
                        listeTLE[isat] = listeTLE.at(0);
                        listeTLE[0] = norad;

                        bool bip = bipSatAOS.at(isat);
                        bipSatAOS[isat] = bipSatAOS.at(0);
                        bipSatAOS[0] = bip;

                        bip = bipSatLOS.at(isat);
                        bipSatLOS[isat] = bipSatLOS.at(0);
                        bipSatLOS[0] = bip;

                        const TLE tle = tles.at(isat);
                        tles[isat] = tles.at(0);
                        tles[0] = tle;

                        const Satellite sat = satellites.at(isat);
                        satellites[isat] = satellites.at(0);
                        satellites[0] = sat;

                        // Ecriture des cles de registre
                        EcritureListeRegistre();

                        info = true;
                        acalcAOS = true;
                        acalcDN = true;

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
void PreviSat::mouseMoveEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    setCursor(Qt::ArrowCursor);
    setToolTip("");
    ui->liste1->setToolTip("");

    messagesStatut->setText("");
    messagesStatut2->setVisible(false);
    messagesStatut3->setVisible(false);

    /* Corps de la methode */
    if (ui->affcoord->isChecked()) {

        // Deplacement de la souris sur la carte du monde
        if (!ui->carte->isHidden()) {

            const int xCur = evt->x() - ui->carte->x();
            const int yCur = evt->y() - ui->carte->y();

            if (xCur > 0 && xCur < ui->carte->width() - 1 && yCur > 0 && yCur < ui->carte->height() - 1) {

                // Longitude
                const double lo0 = 180. - xCur / DEG2PXHZ;
                const QString ews = (lo0 < 0.) ? tr("Est") : tr("Ouest");

                // Latitude
                const double la0 = 90. - yCur / DEG2PXVT;
                const QString nss = (la0 < 0.) ? tr("Sud") : tr("Nord");

                const QString msg = " : %1° %2";
                messagesStatut2->setText(tr("Longitude") + msg.arg(fabs(lo0), 6, 'f', 2, QChar('0')).arg(ews));
                messagesStatut3->setText(tr("Latitude") + msg.arg(fabs(la0), 5, 'f', 2, QChar('0')).arg(nss));
                messagesStatut2->setVisible(true);
                messagesStatut3->setVisible(true);

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ);
                    const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT);

                    // Distance au carre du curseur au satellite
                    const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

                    // Le curseur est au-dessus d'un satellite
                    if (dt <= 16 && !satellites.at(isat).isIeralt()) {
                        const QString msg2 = tr("%1 (numéro NORAD : %2)");
                        messagesStatut->setText(msg2.arg(tles.at(isat).nom()).arg(tles.at(isat).norad()));
                        setToolTip(messagesStatut->text());
                        setCursor(Qt::CrossCursor);
                        break;
                    } else {
                        if (messagesStatut->text().contains("NORAD")) {
                            messagesStatut->setText("");
                        }
                        setCursor(Qt::ArrowCursor);
                    }
                }

                if (ui->affsoleil->isChecked()) {

                    const int lsol = qRound((180. - soleil.longitude() * RAD2DEG) * DEG2PXHZ);
                    const int bsol = qRound((90. - soleil.latitude() * RAD2DEG) * DEG2PXVT);

                    // Distance au carre du curseur au Soleil
                    const int dt = (xCur - lsol) * (xCur - lsol) + (yCur - bsol) * (yCur - bsol);

                    // Le curseur est au-dessus du Soleil
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Soleil"));
                        setToolTip(messagesStatut->text());
                        setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains("Soleil")) {
                            messagesStatut->setText("");
                        }
                    }
                }

                if (ui->afflune->isChecked()) {

                    const int llun = qRound((180. - lune.longitude() * RAD2DEG) * DEG2PXHZ);
                    const int blun = qRound((90. - lune.latitude() * RAD2DEG) * DEG2PXVT);

                    // Distance au carre du curseur a la Lune
                    const int dt = (xCur - llun) * (xCur - llun) + (yCur - blun) * (yCur - blun);

                    //Le curseur est au-dessus de la Lune
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Lune"));
                        setToolTip(messagesStatut->text());
                        setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains("Lune")) {
                            messagesStatut->setText("");
                        }
                    }
                }
            }
        }

        if (!ui->radar->isHidden()) {

            const int x0 = ui->frameZone->x() + ui->radar->x();
            const int y0 = ui->frameListe->height() + ui->radar->y();

            if (evt->x() > x0 + 1 && evt->x() < x0 + ui->radar->width() - 1 &&
                    evt->y() > y0 + 1 && evt->y() < y0 + ui->radar->height() - 1) {

                const int x1 = evt->x() - x0 - 100;
                const int y1 = evt->y() - y0 - 100;

                if (x1 * x1 + y1 * y1 < 10000 && htr) {

                    const int xf = (ui->affinvew->isChecked()) ? 1 : -1;
                    const int yf = (ui->affinvns->isChecked()) ? -1 : 1;

                    const double x2 = -x1 / (100. * xf);
                    const double y2 = -y1 / (100. * yf);

                    const double ht = 90. * (1. - sqrt(x2 * x2 + y2 * y2));
                    double az = atan2(x2, y2) * RAD2DEG;
                    if (az < 0.) az += T360;

                    const QString msg = " : %1°";
                    messagesStatut2->setText(tr("Azimut") + msg.arg(fabs(az), 6, 'f', 2, QChar('0')));
                    messagesStatut3->setText(tr("Hauteur") + msg.arg(fabs(ht), 5, 'f', 2, QChar('0')));
                    messagesStatut2->setVisible(true);
                    messagesStatut3->setVisible(true);

                    for(int isat=0; isat<nbSat; isat++) {

                        const int lsat = qRound(-100. * xf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                                sin(satellites.at(isat).azimut()));
                        const int bsat = qRound(-100. * yf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                                cos(satellites.at(isat).azimut()));

                        const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                        // Le curseur est au-dessus du satellite
                        if (dt <= 16 && !satellites.at(isat).isIeralt()) {
                            QString msg2 = tr("%1 (numéro NORAD : %2)");
                            messagesStatut->setText(msg2.arg(tles.at(isat).nom()).arg(tles.at(isat).norad()));
                            setToolTip(messagesStatut->text());
                            setCursor(Qt::CrossCursor);
                            break;
                        } else {
                            if (messagesStatut->text().contains("NORAD")) {
                                messagesStatut->setText("");
                            }
                            setCursor(Qt::ArrowCursor);
                        }
                    }

                    // Soleil
                    if (ui->affsoleil->isChecked()) {

                        const int lsol = qRound(-100. * xf * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
                        const int bsol = qRound(-100. * yf * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

                        const int dt = (x1 - lsol) * (x1 - lsol) + (y1 - bsol) * (y1 - bsol);

                        // Le curseur est au-dessus du Soleil
                        if (dt <= 81) {
                            messagesStatut->setText(tr("Soleil"));
                            setToolTip(messagesStatut->text());
                            setCursor(Qt::CrossCursor);
                        } else {
                            if (messagesStatut->text().contains(tr("Soleil"))) {
                                messagesStatut->setText("");
                            }
                        }
                    }

                    // Lune
                    if (ui->afflune->isChecked()) {

                        const int llun = qRound(-100. * xf * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
                        const int blun = qRound(-100. * yf * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

                        const int dt = (x1 - llun) * (x1 - llun) + (y1 - blun) * (y1 - blun);

                        // Le curseur est au-dessus de la Lune
                        if (dt <= 81) {
                            messagesStatut->setText(tr("Lune"));
                            setToolTip(messagesStatut->text());
                            setCursor(Qt::CrossCursor);
                        } else {
                            if (messagesStatut->text().contains(tr("Lune"))) {
                                messagesStatut->setText("");
                            }
                        }
                    }
                }
            }
        }

        if (!ui->ciel->isHidden()) {

            const int lciel = qRound(0.5 * ui->ciel->width());
            const int hciel = qRound(0.5 * ui->ciel->height());
            const int x1 = evt->x() - ui->ciel->x() - lciel;
            const int y1 = evt->y() - ui->ciel->y() - hciel;

            if (x1 * x1 + y1 * y1 < hciel * lciel && htr) {

                const double x2 = -x1 / (double) lciel;
                const double y2 = -y1 / (double) hciel;

                const double ht = PI_SUR_DEUX * (1. - sqrt(x2 * x2 + y2 * y2));
                double az = atan2(x2, y2);
                if (az < 0.) az += DEUX_PI;

                const double ch = cos(ht);
                const Vecteur3D vec1(-cos(az) * ch, sin(az) * ch, sin(ht));
                const Vecteur3D vec2(observateurs.at(0).rotHz().Transposee() * vec1);

                // Declinaison
                const double dec = asin(vec2.z());

                // Ascension droite
                double ad = atan2(vec2.y(), vec2.x());
                if (ad < 0.) ad += DEUX_PI;

                messagesStatut2->setText(tr("Ascension droite :") + " " +
                                         Maths::ToSexagesimal(ad, HEURE1, 2, 0, false, false).mid(0, 7));
                messagesStatut3->setText(tr("Déclinaison :") + " " +
                                         Maths::ToSexagesimal(dec, DEGRE, 2, 0, true, false).mid(0, 7));
                messagesStatut2->setVisible(true);
                messagesStatut3->setVisible(true);

                for(int isat=0; isat<nbSat; isat++) {

                    const int lsat = qRound(-0.5 * ui->ciel->width() * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                            sin(satellites.at(isat).azimut()));
                    const int bsat = qRound(-0.5 * ui->ciel->height() * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) *
                                            cos(satellites.at(isat).azimut()));

                    // Distance au carre du satellite au curseur
                    const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                    if (dt <= 16 && !satellites.at(isat).isIeralt()) {
                        QString msg2 = tr("%1 (numéro NORAD : %2)");
                        messagesStatut->setText(msg2.arg(tles.at(isat).nom()).arg(tles.at(isat).norad()));
                        setToolTip(messagesStatut->text());
                        setCursor(Qt::CrossCursor);
                        break;
                    } else {
                        if (messagesStatut->text().contains("NORAD")) {
                            messagesStatut->setText("");
                        }
                        setCursor(Qt::ArrowCursor);
                    }
                }

                if (ui->affplanetes->checkState() != Qt::Unchecked) {

                    for(int ipla=MERCURE; ipla<=NEPTUNE; ipla++) {

                        const int lpla = qRound(-0.5 * ui->ciel->width() * (1. - planetes.at(ipla).hauteur() * DEUX_SUR_PI) *
                                                sin(planetes.at(ipla).azimut()));
                        const int bpla = qRound(-0.5 * ui->ciel->height() * (1. - planetes.at(ipla).hauteur() * DEUX_SUR_PI) *
                                                cos(planetes.at(ipla).azimut()));

                        // Distance au carre du Soleil au curseur
                        const int dt = (x1 - lpla) * (x1 - lpla) + (y1 - bpla) * (y1 - bpla);

                        // Le curseur est au-dessus d'une planete
                        if (dt <= 16) {
                            messagesStatut->setText(planetes.at(ipla).nom());
                            setToolTip(messagesStatut->text());
                            setCursor(Qt::CrossCursor);
                        } else {
                            if (messagesStatut->text().contains(planetes.at(ipla).nom())) {
                                messagesStatut->setText("");
                            }
                        }
                    }
                }

                if (ui->affsoleil->isChecked()) {

                    const int lsol = qRound(-0.5 * ui->ciel->width() * (1. - soleil.hauteur() * DEUX_SUR_PI) *
                                            sin(soleil.azimut()));
                    const int bsol = qRound(-0.5 * ui->ciel->height() * (1. - soleil.hauteur() * DEUX_SUR_PI) *
                                            cos(soleil.azimut()));

                    // Distance au carre du Soleil au curseur
                    const int dt = (x1 - lsol) * (x1 - lsol) + (y1 - bsol) * (y1 - bsol);

                    // Le curseur est au-dessus du Soleil
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Soleil"));
                        setToolTip(messagesStatut->text());
                        setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains(tr("Soleil"))) {
                            messagesStatut->setText("");
                        }
                    }
                }

                if (ui->afflune->isChecked()) {

                    const int llun = qRound(-0.5 * ui->ciel->width() * (1. - lune.hauteur() * DEUX_SUR_PI) *
                                            sin(lune.azimut()));
                    const int blun = qRound(-0.5 * ui->ciel->height() * (1. - lune.hauteur() * DEUX_SUR_PI) *
                                            cos(lune.azimut()));

                    // Distance au carre de la Lune au curseur
                    const int dt = (x1 - llun) * (x1 - llun) + (y1 - blun) * (y1 - blun);

                    // Le curseur est au-dessus de la Lune
                    if (dt <= 81) {
                        messagesStatut->setText(tr("Lune"));
                        setToolTip(messagesStatut->text());
                        setCursor(Qt::CrossCursor);
                    } else {
                        if (messagesStatut->text().contains(tr("Lune"))) {
                            messagesStatut->setText("");
                        }
                    }
                }
            }
        }
    }

    if (ui->listeFichiersTLE->underMouse()) {
        const QFileInfo fi(nomfic);
        if (QDir::toNativeSeparators(fi.absolutePath()) != dirTle) {
            ui->listeFichiersTLE->setToolTip(nomfic);
        }
    }

    if (ui->fluxVideo->isVisible()) {
        setCursor((ui->fluxVideo->underMouse()) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }

    /* Retour */
    return;
}

/*
 * Double-clic sur l'interface
 */
void PreviSat::mouseDoubleClickEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    setCursor(Qt::ArrowCursor);

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
        const int xCur = evt->x();
        const int yCur = evt->y();
        if (xCur > ui->carte->x() && xCur < ui->carte->x() + ui->carte->width() &&
                yCur > ui->carte->y() && yCur < ui->carte->y() + ui->carte->height()) {
            on_maximise_clicked();
        }
    }

    if (!ui->ciel->isHidden()) {
        const int xCur = evt->x() - ui->ciel->x();
        const int yCur = evt->y() - ui->ciel->y();
        const int x0 = qRound(0.5 * ui->ciel->width());
        const int y0 = qRound(0.5 * ui->ciel->height());

        if ((xCur - x0) * (xCur - x0) + (yCur - y0) * (yCur - y0) <= x0 * y0) {
            on_maximise_clicked();
        }
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
        ui->frameCarte->resize(width(), height() - 23);
        ui->maximise->setIcon(QIcon(":/resources/mini.png"));
        ui->maximise->setToolTip(tr("Réduire"));
    } else {

        // Carte minimisee
        ui->frameListe->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        ui->frameCarte->resize(ui->frameCarteListe->width() - ui->frameListe->width() - 5,
                               height() - ui->frameOngletsZone->height() - ((ui->ciel->isHidden()) ? 23 : 39));
        ui->maximise->setIcon(QIcon(":/resources/maxi.png"));
        ui->maximise->setToolTip(tr("Agrandir"));

        EnchainementCalculs();
    }

    resizeEvent(NULL);

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
        if (ui->mccISS->isChecked()) {
            ui->frameCoordISS->setVisible(true);
            ui->gmt->setVisible(true);
        }
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
        ui->frameCoordISS->setVisible(false);
        ui->gmt->setVisible(false);
        ui->affichageCiel->setToolTip(tr("Carte du monde"));
    }

    // Enchainement de l'ensemble des calculs et affichage
    CalculsAffichage();

    resizeEvent(NULL);

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


void PreviSat::on_faireDon_clicked()
{
    on_actionFaire_triggered();
}

void PreviSat::on_meteo_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherMeteo != NULL) {

        if (viewMeteoNASA != NULL) {
            viewMeteoNASA->deleteLater();
            viewMeteoNASA = NULL;
        }

        if (viewMeteo != NULL) {
            viewMeteo->deleteLater();
            viewMeteo = NULL;
        }

        afficherMeteo->deleteLater();
        afficherMeteo = NULL;
    }

    /* Corps de la methode */
    afficherMeteo = new QMainWindow;
    afficherMeteo->resize(1000, 750);

    const int xmax = QApplication::desktop()->availableGeometry().width();
    const int ymax = QApplication::desktop()->availableGeometry().height() - 40;
    int xAff = afficherMeteo->width();
    int yAff = afficherMeteo->height();

    if (afficherMeteo->x() < 0 || afficherMeteo->y() < 0) {
        afficherMeteo->move(0, 0);
    }

    // Redimensionnement de la fenetre si necessaire
    if (xAff > xmax)
        xAff = xmax;
    if (yAff > ymax)
        yAff = ymax;

    if (xAff < afficherMeteo->width() || yAff < afficherMeteo->height()) {
        afficherMeteo->resize(xAff, yAff);
    }

    afficherMeteo->setWindowTitle(QString("%1 %2 - Météo").arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)));

    viewMeteo = new QWebView;
    viewMeteo->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    viewMeteo->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    viewMeteo->settings()->globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    viewMeteo->settings()->globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    viewMeteo->settings()->setObjectCacheCapacities(0, 0, 0);
    viewMeteo->triggerPageAction(QWebPage::ReloadAndBypassCache);

    // Definition de raccourcis
    const QShortcut * const shortcut = new QShortcut(QKeySequence(Qt::Key_F5), afficherMeteo);
    connect(shortcut, SIGNAL(activated()), this, SLOT(ActualiseMeteo()));

    const QShortcut * const shortcut2 = new QShortcut(QKeySequence(Qt::Key_F11), afficherMeteo);
    connect(shortcut2, SIGNAL(activated()), this, SLOT(MeteoPleinEcran()));

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    // Affichage de la map
    QString map0;
    const QString fic = dirLocalData + QDir::separator() + "html" + QDir::separator() + "meteo.map";
    QFile fi(fic);

    if (fi.exists() && fi.size() != 0) {
        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);
        map0 = flux.readAll();
    }
    fi.close();

    const QString lon(QString::number(-observateurs.at(0).longitude() * RAD2DEG));
    const QString lat(QString::number(observateurs.at(0).latitude() * RAD2DEG));
    map0 = map0.replace("LONGITUDE_CENTRE", lon).replace("LATITUDE_CENTRE", lat)
            .replace("UNITE_TEMP", (ui->unitesKm->isChecked()) ? "C" : "F")
            .replace("UNITE_VENT", (ui->unitesKm->isChecked()) ? "kmh" : "mph");

    QFile fi2(dirTmp + QDir::separator() + "meteo.html");
    fi2.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fi2);
    flux << map0;
    fi2.close();

    // Chargement de la meteo
    const QUrl url("file:///" + fi2.fileName());

    viewMeteo->load(url);
    afficherMeteo->setCentralWidget(viewMeteo);
    afficherMeteo->showNormal();

    /* Retour */
    return;
}

void PreviSat::on_meteoBasesNASA_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherMeteo != NULL) {

        if (viewMeteoNASA != NULL) {
            viewMeteoNASA->deleteLater();
            viewMeteoNASA = NULL;
        }

        if (viewMeteo != NULL) {
            viewMeteo->deleteLater();
            viewMeteo = NULL;
        }

        afficherMeteo->deleteLater();
        afficherMeteo = NULL;
    }

    /* Corps de la methode */
    afficherMeteo = new QMainWindow;
    afficherMeteo->resize(1280, 960);

    const int xmax = QApplication::desktop()->availableGeometry().width();
    const int ymax = QApplication::desktop()->availableGeometry().height() - 40;
    int xAff = afficherMeteo->width();
    int yAff = afficherMeteo->height();

    if (afficherMeteo->x() < 0 || afficherMeteo->y() < 0) {
        afficherMeteo->move(0, 0);
    }

    // Redimensionnement de la fenetre si necessaire
    if (xAff > xmax)
        xAff = xmax;
    if (yAff > ymax)
        yAff = ymax;

    if (xAff < afficherMeteo->width() || yAff < afficherMeteo->height()) {
        afficherMeteo->resize(xAff, yAff);
    }

    afficherMeteo->setWindowTitle(QString("%1 %2 - Météo des bases de la NASA").arg(QCoreApplication::applicationName())
                                  .arg(QString(APPVER_MAJ)));

    viewMeteoNASA = new QWebView;
    viewMeteoNASA->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    viewMeteoNASA->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    viewMeteoNASA->settings()->setObjectCacheCapacities(0, 0, 0);
    viewMeteoNASA->triggerPageAction(QWebPage::ReloadAndBypassCache);

    // Definition de raccourcis
    const QShortcut * const shortcut = new QShortcut(QKeySequence(Qt::Key_F5), afficherMeteo);
    connect(shortcut, SIGNAL(activated()), this, SLOT(ActualiseMeteoNASA()));

    const QShortcut * const shortcut2 = new QShortcut(QKeySequence(Qt::Key_F11), afficherMeteo);
    connect(shortcut2, SIGNAL(activated()), this, SLOT(MeteoPleinEcran()));

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    // Chargement de la meteo
    const QString fic("file:///" + dirLocalData + QDir::separator() + "html" + QDir::separator() + "meteoNASA.html");
    const QUrl url(fic);

    viewMeteoNASA->load(url);
    afficherMeteo->setCentralWidget(viewMeteoNASA);
    afficherMeteo->showNormal();

    /* Retour */
    return;
}


void PreviSat::ActualiseMeteo()
{
    viewMeteo->reload();
}

void PreviSat::ActualiseMeteoNASA()
{
    viewMeteoNASA->reload();
}

void PreviSat::MeteoPleinEcran()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (afficherMeteo->isFullScreen()) {

        if (iEtatMeteo) {
            afficherMeteo->showMaximized();
        } else {
            afficherMeteo->showNormal();
        }

    } else {
        iEtatMeteo = afficherMeteo->isMaximized();
        afficherMeteo->showFullScreen();
    }

    /* Retour */
    return;
}


void PreviSat::on_mccISS_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {

        // Affichage du bouton pour activer le flux video
        ui->frameRadar->setVisible(false);
        ui->coordGeo1->setVisible(false);
        ui->coordGeo2->setVisible(false);
        ui->coordGeo3->setVisible(false);
        ui->coordGeo4->setVisible(false);
        ui->radar->setVisible(false);
        ui->lbl_chaine->setVisible(true);
        ui->chaine->setVisible(true);
        ui->fluxVideo->setText(tr("Cliquez ici pour activer\nle flux vidéo"));

        ui->frameZone->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        ui->frameOnglets->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui->frameZone->adjustSize();

    } else {

        // Fermeture de la video
        ui->fluxVideo->raise();
        ui->frameRadar->setVisible(true);
        ui->coordGeo1->setVisible(true);
        ui->coordGeo2->setVisible(true);
        ui->coordGeo3->setVisible(true);
        ui->coordGeo4->setVisible(true);
        ui->radar->setVisible(true);
        ui->lbl_chaine->setVisible(false);
        ui->chaine->setVisible(false);

        ui->frameZone->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        ui->frameOnglets->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }
    ui->fluxVideo->setVisible(checked);

    // Affichage du blackboard
    const bool tstSat = (satellites.isEmpty()) ? false : satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE;
    if (checked && tstSat && !l1.isEmpty() && !l2.isEmpty() && !ui->ciel->isVisible()) {
        ui->frameCoordISS->setVisible(true);
        ui->frameLat2->setVisible(true);
        ui->gmt->setVisible(true);
    } else {
        ui->frameCoordISS->setVisible(false);
        ui->frameLat2->setVisible(false);
        ui->gmt->setVisible(false);
    }

    CalculsAffichage();

    resizeEvent(NULL);

    /* Retour */
    return;
}

void PreviSat::on_chaine_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    if (ui->frameCtrlVideo->isVisible()) {
        on_fluxVideo_clicked();
    }
}

void PreviSat::on_fluxVideo_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString fic = listeChaines.at(ui->chaine->value()-1);

        // Verification de la connexion
        QTcpSocket socket;
        QString adresse = adresseAstropedia;
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


void PreviSat::on_directHelp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString aide = QString("file:///%1%2doc%3%4_%5.pdf").arg(dirExe).arg(QDir::separator()).arg(QDir::separator()).
            arg(QCoreApplication::applicationName()).arg(localePreviSat);

    /* Corps de la methode */
    if (!QDesktopServices::openUrl(aide)) {
        const QFileInfo fi(aide);
        const QString msg = tr("Impossible d'ouvrir le fichier d'aide %1");
        Message::Afficher(msg.arg(fi.fileName()), WARNING);
    }

    /* Retour */
    return;
}

QString PreviSat::getText(QWidget *fenetreParent, const QString &titre, const QString &label, const QString &texteOk, const QString &texteAnnule,
                          QLineEdit::EchoMode mode, const QString &texte, Qt::WindowFlags flags, Qt::InputMethodHints hints)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QInputDialog input(fenetreParent, flags);
    input.setWindowTitle(titre);
    input.setLabelText(label);
    input.setTextValue(texte);
    input.setTextEchoMode(mode);
    input.setInputMethodHints(hints);
    input.setOkButtonText(texteOk);
    input.setCancelButtonText(texteAnnule);

    const int ret = input.exec();

    /* Retour */
    return ((ret) ? input.textValue() : QString());
}


/*
 * Menu deroulant
 */
void PreviSat::on_actionOuvrir_fichier_TLE_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Ouverture d'un fichier TLE
    const QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                         settings.value("fichier/repTLE", dirTle).toString(),
                                                         tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;" \
                                                            "Fichiers gz (*.gz)"));

    try {
        if (fichier.isEmpty()) {
            if (!ui->listeFichiersTLE->currentText().isEmpty()) {
                ui->listeFichiersTLE->setCurrentIndex(ficTLE.indexOf(nomfic));
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
    if (ui->actionEnregistrer->isVisible() && ui->onglets->currentIndex() < 3) {

        const QStringList listeNoms(QStringList() << tr("onglet_general") << tr("onglet_elements") <<
                                    tr("onglet_informations"));

#if defined (Q_OS_WIN)
        const QString nomFicDefaut = settings.value("fichier/sauvegarde", dirOut).toString().replace(QDir::separator(), "\\") +
                QDir::separator() + listeNoms.at(ui->onglets->currentIndex()) + ".txt";
#else
        const QString nomFicDefaut = settings.value("fichier/sauvegarde", dirOut).toString() +
                QDir::separator() + listeNoms.at(ui->onglets->currentIndex()) + ".txt";
#endif
        const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomFicDefaut,
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

void PreviSat::on_actionImprimer_carte_triggered()
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
        const int xPrt = (pixscl.width() == pixmap.width()) ? printer.pageRect().width() / 2 - pixscl.width() / 2 : 50;
        p.drawPixmap(xPrt, 50, pixscl);
        p.end();
    }

    /* Retour */
    return;
}


void PreviSat::on_actionVision_nocturne_toggled(bool arg1)
{
    /* Declarations des variables locales */
    QPalette paletteWin, palLbl, palList;

    /* Initialisations */
    const QBrush alpha = (arg1) ? QBrush(QColor::fromRgb(178 - ui->intensiteVision->value(), 0, 0, 255)) : QBrush(Qt::NoBrush);
    const QBrush coulLbl = (arg1) ? QBrush(QColor::fromRgb(165 - ui->intensiteVision->value(), 0, 0)) :
                                    QBrush(QColor::fromRgb(227, 227, 227));
    const QColor coulList = (arg1) ? QColor(205 - ui->intensiteVision->value(), 0, 0) : QColor(255, 255, 255);
    settings.setValue("affichage/flagIntensiteVision", arg1);
    settings.setValue("affichage/valIntensiteVision", 178 - ui->intensiteVision->value());

    /* Corps de la methode */
    if (arg1) {
        paletteWin.setBrush(this->backgroundRole(), alpha);
    } else {
        paletteWin = paletteDefaut;
    }

    this->setPalette(paletteWin);

    palLbl.setBrush(ui->lbl_coordonneesSoleil->backgroundRole(), coulLbl);
    ui->lbl_coordonneesSoleil->setPalette(palLbl);
    ui->lbl_coordonneesLune->setPalette(palLbl);

    palList.setColor(QPalette::Base, coulList);
    ui->liste1->setPalette(palList);
    ui->liste2->setPalette(palList);
    ui->liste3->setPalette(palList);
    ui->listeStations->setPalette(palList);

    ui->dateHeure3->setPalette(palList);
    ui->dateHeure4->setPalette(palList);
    ui->lieuxObservation1->setPalette(palList);

    ui->barreMenu->setPalette(palList);
    ui->menuPrincipal->setPalette(palList);
    ui->pasManuel->setPalette(palList);
    ui->pasReel->setPalette(palList);
    ui->valManuel->setPalette(palList);
    ui->listeFichiersTLE->setPalette(palList);
    ui->typeParametres->setPalette(palList);

    ui->dateInitialePrev->setPalette(palList);
    ui->dateFinalePrev->setPalette(palList);
    ui->pasGeneration->setPalette(palList);
    ui->lieuxObservation2->setPalette(palList);
    ui->hauteurSoleilPrev->setPalette(palList);
    ui->valHauteurSoleilPrev->setPalette(palList);
    ui->hauteurSatPrev->setPalette(palList);
    ui->valHauteurSatPrev->setPalette(palList);
    ui->valMagnitudeMaxPrev->setPalette(palList);

    ui->fichiersObs->setPalette(palList);
    ui->lieuxObs->setPalette(palList);
    ui->selecLieux->setPalette(palList);
    ui->nvCategorie->setPalette(palList);
    ui->nvLieu->setPalette(palList);
    ui->nvLongitude->setPalette(palList);
    ui->nvLatitude->setPalette(palList);
    ui->nvAltitude->setPalette(palList);
    ui->ajdfic->setPalette(palList);

    ui->nombreTrajectoires->setPalette(palList);
    ui->listeMap->setPalette(palList);
    ui->magnitudeEtoiles->setPalette(palList);

    ui->groupeTLE->setPalette(palList);
    ui->affichageMsgMAJ->setPalette(palList);
    ui->fichierAMettreAJour->setPalette(palList);
    ui->fichierALire->setPalette(palList);
    ui->compteRenduMaj->setPalette(palList);

    ui->fichierALireCreerTLE->setPalette(palList);
    ui->numeroNORADCreerTLE->setPalette(palList);
    ui->noradMin->setPalette(palList);
    ui->noradMax->setPalette(palList);
    ui->ADNoeudAscendantCreerTLE->setPalette(palList);
    ui->ADNAMin->setPalette(palList);
    ui->ADNAMax->setPalette(palList);
    ui->excentriciteCreerTLE->setPalette(palList);
    ui->excMin->setPalette(palList);
    ui->excMax->setPalette(palList);
    ui->inclinaisonCreerTLE->setPalette(palList);
    ui->inclMin1->setPalette(palList);
    ui->inclMax1->setPalette(palList);
    ui->inclMin2->setPalette(palList);
    ui->inclMax2->setPalette(palList);
    ui->revMin->setPalette(palList);
    ui->revMax->setPalette(palList);
    ui->argumentPerigeeCreerTLE->setPalette(palList);
    ui->argMin->setPalette(palList);
    ui->argMax->setPalette(palList);
    ui->magnitudeMaxCreerTLE->setPalette(palList);
    ui->nomFichierPerso->setPalette(palList);

    ui->dateInitialeEvt->setPalette(palList);
    ui->dateFinaleEvt->setPalette(palList);

    ui->dateInitialeTransit->setPalette(palList);
    ui->dateFinaleTransit->setPalette(palList);
    ui->lieuxObservation4->setPalette(palList);
    ui->ageMaxTLETransit->setPalette(palList);
    ui->hauteurSatTransit->setPalette(palList);
    ui->valHauteurSatTransit->setPalette(palList);
    ui->elongationMaxCorps->setPalette(palList);

    ui->dateInitialeMetOp->setPalette(palList);
    ui->dateFinaleMetOp->setPalette(palList);
    ui->lieuxObservation5->setPalette(palList);
    ui->hauteurSoleilMetOp->setPalette(palList);
    ui->valHauteurSoleilMetOp->setPalette(palList);
    ui->hauteurSatMetOp->setPalette(palList);
    ui->valHauteurSatMetOp->setPalette(palList);
    ui->fichierTLEMetOp->setPalette(palList);
    ui->magnitudeMaxMetOp->setPalette(palList);

    AffichageCourbes();

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
    const QUrl urlLastNews(settings.value("fichier/dirHttpPrevi", "").toString() + "informations/last_news_" + localePreviSat + ".html");

    if (Informations::UrlExiste(urlLastNews)) {

        Informations * const infos = new Informations(localePreviSat, this);
        infos->setWindowModality(Qt::ApplicationModal);
        infos->show();

        QFont fnt;
        fnt.setBold(false);
        ui->actionInformations->setFont(fnt);

        if (!majInfosDate.isEmpty()) {
            settings.setValue("temps/lastInfos", majInfosDate);
        }

    } else {
        if (!majInfosDate.isEmpty()) {
            Message::Afficher(tr("Pas d'informations à afficher"), INFO);
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionFaire_triggered()
{
    QDesktopServices::openUrl(QUrl(settings.value("fichier/dirHttpPreviDon", "").toString()));
}

void PreviSat::on_actionPrevisat_sourceforge_net_triggered()
{
    QDesktopServices::openUrl(QUrl("http://previsat.sourceforge.net/"));
}

void PreviSat::on_actionTelecharger_la_mise_a_jour_triggered()
{
    QDesktopServices::openUrl(QUrl("http://sourceforge.net/projects/previsat/files/latest/download"));
}

void PreviSat::on_actionMettre_jour_fichiers_internes_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    dirDwn = dirLocalData;
    atrouve = false;
    aupdnow = false;
    const QString dirHttpPrevi = settings.value("fichier/dirHttpPrevi", adresseAstropedia + "previsat/Qt/").toString().trimmed();

    /* Corps de la methode */
    foreach(QString fic, listeFicLocalData) {
        if (!fic.contains("gestionnaireTLE_") && !fic.contains("preferences")) {
            const QString ficMaj = dirHttpPrevi + "commun/data/" + fic.replace("\\", "/");
            TelechargementFichier(ficMaj, aclickFicMaj);
        }
    }

    QFile fi(dirTmp + QDir::separator() + "majFicInt");
    if (fi.exists()) {
        fi.remove();
    }
    ui->actionMettre_jour_fichiers_internes->setVisible(false);
    settings.setValue("fichier/majPrevi", "0");

    /* Retour */
    return;
}

void PreviSat::on_actionRapport_de_bug_triggered()
{
    QDesktopServices::openUrl(QUrl(adresseAstropedia + "rapport.html"));
}

void PreviSat::on_actionWww_celestrak_com_triggered()
{
    QDesktopServices::openUrl(QUrl(adresseCelestrak));
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


/*
 * Gestion de la liste principale de satellites
 */
void PreviSat::on_listeFichiersTLE_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->listeFichiersTLE->itemText(ui->listeFichiersTLE->count() - 1) == tr("Parcourir...")) {

        if (index == ui->listeFichiersTLE->count() - 1) {
            on_actionOuvrir_fichier_TLE_triggered();
        } else {
            ui->listeFichiersTLE->setItemData(idxf, QColor(Qt::white), Qt::BackgroundRole);
            ui->listeFichiersTLE->setItemData(index, QColor(Qt::gray), Qt::BackgroundRole);
            OuvertureFichierTLE(ficTLE.at(index));
            idxf = index;
        }
        on_rechercheDonneesSat_toggled(ui->rechercheDonneesSat->isChecked());
    }

    /* Retour */
    return;
}

void PreviSat::on_actionDefinir_par_defaut_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    int j = -1;

    /* Corps de la methode */
    for(int i=0; i<nbSat; i++) {
        if (listeTLE.at(i) == nor) {
            j = i;
            break;
        }
    }

    // Le satellite selectionne fait deja partie de la liste
    if (j > 0) {
        listeTLE[j] = listeTLE.at(0);
        listeTLE[0] = nor;
        nor = "";
    }

    // Le satellite selectionne ne fait pas partie de la liste : on l'ajoute dans la liste
    if (j == -1) {

        nbSat++;
        listeTLE.append("");
        tles.resize(nbSat);
        bipSatAOS.resize(nbSat);
        bipSatLOS.resize(nbSat);

        for(int i=nbSat-1; i>0; i--) {
            listeTLE[i] = listeTLE.at(i-1);
        }
        listeTLE[0] = nor;
        nor = "";

        ui->liste1->setCurrentRow(ind);
        ui->liste2->setCurrentRow(ind);
        if (ui->liste1->currentItem()->checkState() == Qt::Unchecked) {
            ui->liste1->currentItem()->setCheckState(Qt::Checked);
        }
        ind = -1;
    }

    Satellite::initCalcul = false;

    // Ecriture des cles de registre
    EcritureListeRegistre();

    info = true;
    acalcAOS = true;
    acalcDN = true;

    CalculsAffichage();

    /* Retour */
    return;
}

void PreviSat::on_actionNouveau_fichier_TLE_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStringList listeSat = listeTLE;
    listeSat.sort();

    /* Corps de la methode */
    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), dirTle,
                                                     tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));

    QFile fichier(fic);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    for(int j=0; j<nbSat; j++) {
        for(int i=0; i<nbSat; i++) {
            if (tles.at(i).norad() == listeSat.at(j)) {
                flux << tles.at(i).nom()    << endl <<
                        tles.at(i).ligne1() << endl <<
                        tles.at(i).ligne2() << endl;
                break;
            }
        }
    }
    fichier.close();

    InitFicTLE();

    const QString msg = tr("Fichier %1 créé");
    messagesStatut->setText(msg.arg(fichier.fileName()));

    /* Retour */
    return;
}
void PreviSat::on_actionFichier_TLE_existant_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString fic = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"), dirTle,
                                                         tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));


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

        for(int i=0; i<listeTLE.size(); i++) {
            bool atrouve2 = false;
            for(int j=0; j<nsat; j++) {
                if (listeTLE.at(i) == tabtle.at(j).norad()) {
                    atrouve2 = true;
                    break;
                }
            }
            if (!atrouve2) {
                flux << tles.at(i).nom()    << endl <<
                        tles.at(i).ligne1() << endl <<
                        tles.at(i).ligne2() << endl;
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
        for(int i=0; i<nsat; i++) {
            listeSat.append(tabtle.at(i).norad());
        }

        listeSat.sort();

        fichier.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&fichier);

        for(int j=0; j<nsat; j++) {
            for(int i=0; i<nsat; i++) {
                if (tabtle.at(i).norad() == listeSat.at(j)) {
                    flux2 << tabtle.at(i).nom()    << endl <<
                             tabtle.at(i).ligne1() << endl <<
                             tabtle.at(i).ligne2() << endl;
                    break;
                }
            }
        }
        fichier.close();

        QFile fi2(ficTmp);
        fi2.remove();

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
    Q_UNUSED(index)
    QFile fi(nomfic);
    if (!fi.exists()) {
        const QString msg = tr("Le fichier %1 n'existe pas");
        Message::Afficher(msg.arg(nomfic), WARNING);

        // Remplissage du premier element du tableau de TLE avec le satellite en memoire
        nbSat = 0;
        tles.clear();
        tles.append(TLE(nom, l1, l2));
        listeTLE.clear();
        listeTLE.append("");
        bipSatAOS.resize(1);
        bipSatLOS.resize(1);
        l1 = "";
        l2 = "";
        ui->liste1->clear();
        ui->liste2->clear();
        ui->liste3->clear();
    }

    /* Corps de la methode */
    if (ui->liste1->hasFocus()) {
        ind = ui->liste1->currentRow();
        if (ind >= 0) {
            nor = ui->liste1->item(ind)->text().split("#").at(1);
        }

        if (ui->liste1->currentItem()->checkState() == Qt::Checked) {

            // Suppression d'un satellite de la liste
            for(int i=0; i<listeTLE.size(); i++) {
                if (ui->liste1->item(ind)->text().split("#").at(1) == listeTLE.at(i)) {
                    ui->liste1->currentItem()->setCheckState(Qt::Unchecked);
                    listeTLE.removeAt(i);
                    tles.remove(i);
                    bipSatAOS.remove(i);
                    bipSatLOS.remove(i);
                    ui->liste2->item(ind)->setCheckState(Qt::Unchecked);
                    ui->liste3->item(ind)->setCheckState(Qt::Unchecked);
                    break;
                }
            }
            nbSat--;

        } else {

            // Ajout d'un satellite dans la liste
            listeTLE.append(ui->liste1->item(ind)->text().split("#").at(1));
            nbSat++;
            tles.resize(nbSat);
            bipSatAOS.resize(nbSat);
            bipSatLOS.resize(nbSat);
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
        acalcAOS = true;
        acalcDN = true;

        // Ecriture des cles de registre
        EcritureListeRegistre();

        // Enchainement de l'ensemble des calculs
        EnchainementCalculs();

        if (satellites.at(0).isIeralt() && ui->liste1->currentItem()->checkState() == Qt::Checked) {
            chronometre->stop();
            const QString msg = tr("Erreur rencontrée lors de l'exécution\n" \
                                   "La position du satellite %1 (numéro NORAD : %2) ne peut pas être calculée (altitude négative)");
            Message::Afficher(msg.arg(tles.at(0).nom()).arg(tles.at(0).norad()), WARNING);
            chronometre->start();
            l1 = "";
            l2 = "";
        }

        if (ui->mccISS->isChecked() && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE && !l1.isEmpty() && !l2.isEmpty() &&
                !ui->ciel->isVisible()) {
            ui->frameCoordISS->setVisible(true);
            ui->gmt->setVisible(true);
        } else {
            ui->frameCoordISS->setVisible(false);
            ui->gmt->setVisible(false);
        }

        // Affichage des donnees numeriques
        AffichageDonnees();

        // Affichage des elements graphiques
        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_liste1_customContextMenuRequested(const QPoint &position)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(position)

    /* Corps de la methode */
    if (ui->liste1->currentRow() >= 0) {
        ui->menuContextuelListe1->exec(QCursor::pos());
    }

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
        messagesStatut->setText(ui->liste1->toolTip());
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
            if (i > 0) {
                nomlieu += "$";
            }
            nomlieu += listeObs.at(i);
        }
        settings.setValue("observateur/lieu", nomlieu);
        acalcAOS = true;

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

        htSat = 0.;
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

        tim = QDateTime::currentDateTimeUtc();
        ui->pasManuel->setVisible(false);
        ui->valManuel->setVisible(false);
        ui->dateHeure1->setVisible(true);
        ui->dateHeure2->setVisible(true);
        ui->dateHeure3->setVisible(false);
        ui->dateHeure4->setVisible(false);
        ui->utcManuel->setVisible(false);
        ui->utcManuel2->setVisible(false);
        on_pasReel_currentIndexChanged(ui->pasReel->currentIndex());
        ui->pasReel->setVisible(true);
        ui->secondes->setVisible(true);
        ui->frameSimu->setVisible(false);
        if (ui->pause->isEnabled()) {
            on_pause_clicked();
        }
        acalcAOS = true;
        acalcDN = true;
        CalculAgeTLETransitISS();
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

        const QString fmt = tr("dddd dd MMMM yyyy  hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "");
        ui->pasReel->setVisible(false);
        ui->secondes->setVisible(false);
        on_pasManuel_currentIndexChanged(ui->pasManuel->currentIndex());
        ui->pasManuel->setVisible(true);
        ui->valManuel->setVisible(true);
        ui->dateHeure3->setDateTime((ui->utc->isChecked()) ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
        ui->dateHeure3->setDisplayFormat(fmt);
        ui->dateHeure3->setVisible(true);
        ui->dateHeure4->setDateTime(ui->dateHeure3->dateTime());
        ui->dateHeure4->setDisplayFormat(fmt);
        ui->dateHeure4->setVisible(true);
        ui->dateHeure1->setVisible(false);
        ui->dateHeure2->setVisible(false);
        ui->utcManuel->setVisible(true);
        ui->utcManuel2->setVisible(true);
        ui->frameSimu->setVisible(true);
        ui->pasManuel->setFocus();
        CalculAgeTLETransitISS();

        htSat = 0.;
        acalcAOS = true;
        acalcDN = true;
        notifAOS = false;
        notifLOS = false;
        notifFlash = false;
    }

    /* Retour */
    return;
}

void PreviSat::on_dateHeure3_dateTimeChanged(const QDateTime &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Date datp(date.date().year(), date.date().month(), date.date().day(),
                    date.time().hour(), date.time().minute(), date.time().second(), 0.);

    /* Corps de la methode */
    dateCourante = Date(datp.jourJulienUTC() - dateCourante.offsetUTC(), dateCourante.offsetUTC());

    if (ui->modeManuel->isChecked()) {
        info = true;
        acalcAOS = true;
        acalcDN = true;
    }

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
    const Date datp(date.date().year(), date.date().month(), date.date().day(),
                    date.time().hour(), date.time().minute(), date.time().second(), 0.);

    /* Corps de la methode */
    dateCourante = Date(datp.jourJulienUTC() - dateCourante.offsetUTC(), dateCourante.offsetUTC());

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


void PreviSat::on_rechercheDonneesSat_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->noradDonneesSat->setValue(0);

    /* Corps de la methode */
    if (checked) {
        ui->frameDonneesSat->setVisible(true);
        ui->frameInformationsSatellite->setVisible(false);

        const QString ficData = dirLocalData + QDir::separator() + "donnees.sat";

        QFile donneesSatellites(ficData);
        if (donneesSatellites.exists() && donneesSatellites.size() != 0) {
            donneesSatellites.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&donneesSatellites);
            donneesSat = flux.readAll().toLower();
        } else {
            donneesSat = "";
        }
        donneesSatellites.close();

        chg = true;
        ui->frameResultats->setVisible(false);
        if (!satellites.isEmpty()) {
            ui->noradDonneesSat->setValue(satellites.at(0).tle().norad().toInt());
        }
        ui->rechercheDonneesSat->setText(tr("Informations satellite"));
        ui->nom->setFocus();
    } else {
        ui->frameDonneesSat->setVisible(false);
        ui->frameInformationsSatellite->setVisible(true);
        ui->rechercheDonneesSat->setText(tr("Recherche données satellite..."));
        donneesSat = "";
    }

    /* Retour */
    return;
}

void PreviSat::on_nom_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomsat = ui->nom->text();

    /* Corps de la methode */
    if (nomsat.length() >= 3) {

        int indx1 = 127;
        int indx2 = 0;
        chg = false;
        resultatsSatellitesTrouves.clear();

        // Recherche dans le tableau de donnees a partir du nom de l'objet
        do {
            indx1 = donneesSat.indexOf(nomsat.toLower().trimmed(), indx1 + indx2);
            if (indx1 >= 0) {

                int indx3 = donneesSat.lastIndexOf("\n", indx1) + 1;
                indx2 = donneesSat.indexOf("\n", indx3) - indx3;
                if (indx1 - indx3 >= 123) {
                    const QString ligne = donneesSat.mid(indx3, indx2);
                    if (ligne.length() > 0) {
                        resultatsSatellitesTrouves.append(ligne);
                    }
                    indx1 = indx3;
                } else {
                    indx1 += nomsat.trimmed().length() - indx2;
                }
            }
        } while (indx1 >= 0);

        if (resultatsSatellitesTrouves.count() > 0) {
            ui->cosparDonneesSat->setText(resultatsSatellitesTrouves.at(0).mid(6, 11).toUpper().trimmed());
            ui->noradDonneesSat->setValue(resultatsSatellitesTrouves.at(0).mid(0, 5).toInt());
        }

        chg = true;
        AffichageResultats();
    } else {
        QToolTip::showText(ui->nom->mapToGlobal(QPoint(0, 0)), ui->nom->toolTip());
    }

    /* Retour */
    return;
}

void PreviSat::on_noradDonneesSat_valueChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (chg) {
        resultatsSatellitesTrouves.clear();

        if (arg1 > 0) {

            int indx1 = 0;
            const QString chaine = "%1 ";
            const QString norad = chaine.arg(arg1, 5, 10, QChar('0'));

            // Recherche dans le tableau de donnees a partir du numero NORAD
            do {
                indx1 = donneesSat.indexOf(norad, indx1 + 1);
            } while (indx1 >= 0 && donneesSat.at(qMax(0, indx1 - 1)) != '\n');

            if (indx1 >= 0) {

                int indx2 = donneesSat.indexOf("\n", indx1) - indx1;

                const QString ligne = donneesSat.mid(indx1, indx2).trimmed();
                if (ligne.length() > 0) {
                    resultatsSatellitesTrouves.append(ligne);
                }

                QString nomsat = ligne.mid(123).trimmed();
                if (nomsat == "iss (zarya)") {
                    nomsat = "ISS";
                }
                ui->nom->setText(nomsat.toUpper());
                ui->cosparDonneesSat->setText(ligne.mid(6, 11).toUpper());
            }

            AffichageResultats();
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_cosparDonneesSat_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->cosparDonneesSat->text().length() > 1 && ui->cosparDonneesSat->text().contains("-")) {

        int indx1 = 0;
        int indx2 = 1;
        chg = false;
        resultatsSatellitesTrouves.clear();

        // Recherche dans le tableau de donnees a partir de la designation COSPAR
        do {
            indx1 = donneesSat.indexOf(ui->cosparDonneesSat->text().toLower().trimmed(), indx1 + indx2);
            if (indx1 >= 0 && donneesSat.at(qMax(0, indx1 - 7)) == '\n') {

                indx1 = donneesSat.lastIndexOf("\n", indx1) + 1;
                indx2 = donneesSat.indexOf("\n", indx1) - indx1;
                const QString ligne = donneesSat.mid(indx1, indx2);
                if (ligne.length() > 0) {
                    resultatsSatellitesTrouves.append(ligne);
                }
            }
        } while (indx1 >= 0);

        if (resultatsSatellitesTrouves.count() > 0) {
            ui->nom->setText(resultatsSatellitesTrouves.at(0).mid(123).toUpper().trimmed());
            ui->noradDonneesSat->setValue(resultatsSatellitesTrouves.at(0).mid(0, 5).toInt());
        }

        chg = true;
        AffichageResultats();
    }

    /* Retour */
    return;
}

void PreviSat::on_satellitesTrouves_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */
    QString chaine;

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        double xval, xval2;
        const QString ligne = resultatsSatellitesTrouves.at(currentRow).toUpper();
        const double magnitudeStandard = ligne.mid(34, 4).toDouble();

        const QString dateRentree = ligne.mid(59, 10).trimmed();
        const QString periode = ligne.mid(70, 10).trimmed();
        const QString perigee = ligne.mid(81, 7).trimmed();
        const QString apogee = ligne.mid(89, 7).trimmed();

        const double ap = apogee.toDouble() + RAYON_TERRESTRE;
        const double per = perigee.toDouble() + RAYON_TERRESTRE;

        // Nom du satellite
        QString nomsat = ligne.mid(123).trimmed();
        if (nomsat.toLower() == "iss (zarya)") {
            nomsat = "ISS";
        }
        ui->nomsat->setText((nomsat.isEmpty()) ? tr("Inconnu") : nomsat);

        // Numero NORAD
        const QString norad = ligne.mid(0, 5);
        ui->numNorad->setText(norad);

        // Designation COSPAR
        const QString cospar = ligne.mid(6, 11).trimmed();
        ui->desigCospar->setText((cospar.isEmpty()) ? tr("Inconnue") : cospar);

        // Magnitude standard/maximale
        if (magnitudeStandard > 98. || perigee.isEmpty() || apogee.isEmpty()) {
            ui->magnitudeStdMaxDonneesSat->setText("?/?");
        } else {

            // Estimation de la magnitude maximale
            const double demiGrandAxe = 0.5 * (ap + per);
            const double exc = 2. * ap / (ap + per) - 1.;
            xval = magnitudeStandard - 15.75 + 5. * log10(1.45 * (demiGrandAxe * (1. - exc) - RAYON_TERRESTRE));
            char methMagnitude = ligne.at(39).toLower().toLatin1();

            chaine = "%1%2%3/%4%5";
            chaine = chaine.arg((magnitudeStandard >= 0.) ? "+" : "-").arg(fabs(magnitudeStandard), 0, 'f', 1).
                    arg(methMagnitude).arg((xval >= 0.) ? "+" : "-").arg(fabs(xval), 0, 'f', 1);
            ui->magnitudeStdMaxDonneesSat->setText(chaine);
        }

        // Modele orbital
        const QString modele = (periode.toDouble() > 225.) ? tr("SGP4 (DS)") : tr("SGP4 (NE)");
        ui->modeleDonneesSat->setText((periode.isEmpty()) ? tr("Non applicable") : modele);

        // Dimensions du satellite
        double t1 = ligne.mid(18, 5).toDouble();
        double t2 = ligne.mid(24, 4).toDouble();
        double t3 = ligne.mid(29, 4).toDouble();
        double section = ligne.mid(41, 6).toDouble();
        QString unite1 = tr("m");
        const bool unite = settings.value("affichage/unite", true).toBool();
        if (!unite) {
            t1 *= PIED_PAR_METRE;
            t2 *= PIED_PAR_METRE;
            t3 *= PIED_PAR_METRE;
            section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
            unite1 = tr("ft");
        }

        QString chaine2 = "%1 " + unite1;
        if (fabs(t2) < EPSDBL100 && fabs(t3) < EPSDBL100) {
            chaine = tr("Sphérique. R=").append(chaine2.arg(t1, 0, 'f', 1));
        }

        if (fabs(t2) >= EPSDBL100 && fabs(t3) < EPSDBL100) {
            chaine = tr("Cylindrique. L=").append(chaine2.arg(t1, 0, 'f', 1)).
                    append(tr(", R=")).append(chaine2.arg(t2, 0, 'f', 1));
        }

        if (fabs(t2) >= EPSDBL100 && fabs(t3) >= EPSDBL100) {
            chaine = tr("Boîte.").append(" %1 x %2 x %3 ").append(unite1);
            chaine = chaine.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1);
        }

        if (fabs(t1) < EPSDBL100) {
            chaine = tr("Inconnues");
        }

        if (fabs(section) > EPSDBL100) {
            chaine = chaine + " / %1 " + unite1;
            chaine = chaine.arg(section, 0, 'f', 2);
            ui->sqDonneesSat->setVisible(true);
        } else {
            ui->sqDonneesSat->setVisible(false);
        }

        ui->dimensionsDonneesSat->setText(chaine);
        ui->dimensionsDonneesSat->adjustSize();
        ui->dimensionsDonneesSat->setFixedHeight(16);
        ui->sqDonneesSat->move(ui->dimensionsDonneesSat->x() + ui->dimensionsDonneesSat->width() + 1, ui->sqDonneesSat->y());

        // Apogee/perigee/periode orbitale
        unite1 = (unite) ? tr("km") : tr("nmi");
        if (apogee.isEmpty()) {
            ui->apogeeDonneesSat->setText(tr("Inconnu"));

        } else {
            chaine2 = "%2 %1 (%3 %1)";
            chaine2 = chaine2.arg(unite1);
            xval = (unite) ? ap : ap * MILE_PAR_KM;
            xval2 = (unite) ? apogee.toDouble() : apogee.toDouble() * MILE_PAR_KM;
            chaine = chaine2.arg(xval, 0, 'f', 0).arg(xval2, 0, 'f', 0);
            ui->apogeeDonneesSat->setText(chaine);
        }

        if (perigee.isEmpty() || perigee == "0") {
            ui->perigeeDonneesSat->setText(tr("Inconnu"));
        } else {
            xval = (unite) ? per : per * MILE_PAR_KM;
            xval2 = (unite) ? perigee.toDouble() : perigee.toDouble() * MILE_PAR_KM;
            chaine = chaine2.arg(xval, 0, 'f', 0).arg(xval2, 0, 'f', 0);
            ui->perigeeDonneesSat->setText(chaine);
        }

        chaine = (periode.isEmpty()) ?
                    tr("Inconnue") : Maths::ToSexagesimal(periode.toDouble() * NB_HEUR_PAR_MIN * HEUR2RAD, HEURE1, 1, 0, false, true);
        ui->periodeDonneesSat->setText(chaine.trimmed());

        // Inclinaison
        const QString inclinaison = ligne.mid(97, 6).trimmed();
        ui->inclinaisonDonneesSat->setText((inclinaison.isEmpty()) ? tr("Inconnue") : inclinaison + "°");

        // Date de lancement
        const QString dateLancement = ligne.mid(48, 10).trimmed();
        const int annee_lct = dateLancement.mid(0, 4).toInt();
        const int mois_lct = dateLancement.mid(5, 2).toInt();
        const double jour_lct = dateLancement.mid(8, 2).toDouble();
        const Date date_lancement(annee_lct, mois_lct, jour_lct, 0.);

        ui->dateLancementDonneesSat->setText((dateLancement.isEmpty()) ?
                                                 tr("Inconnue") : date_lancement.ToShortDate(FORMAT_COURT, SYSTEME_24H).left(10));

        // Date de rentree
        if (dateRentree.isEmpty()) {
            ui->lbl_dateRentree->setVisible(false);
            ui->dateRentree->setVisible(false);

            ui->lbl_categorieOrbiteDonneesSat->move(3, 70);
            ui->categorieOrbiteDonneesSat->move(114, 70);

            ui->lbl_paysDonneesSat->move(3, 85);
            ui->paysDonneesSat->move(114, 85);

            ui->lbl_siteLancementDonneesSat->move(3, 100);
            ui->siteLancementDonneesSat->move(114, 100);
        } else {

            const int annee_rentree = dateRentree.mid(0, 4).toInt();
            const int mois_rentree = dateRentree.mid(5, 2).toInt();
            const double jour_rentree = dateRentree.mid(8, 2).toDouble();
            const Date date_rentree(annee_rentree, mois_rentree, jour_rentree, 0.);

            ui->dateRentree->setText(date_rentree.ToShortDate(FORMAT_COURT, SYSTEME_24H).left(10));
            ui->lbl_dateRentree->setVisible(true);
            ui->dateRentree->setVisible(true);

            ui->lbl_categorieOrbiteDonneesSat->move(3, 85);
            ui->categorieOrbiteDonneesSat->move(114, 85);

            ui->lbl_paysDonneesSat->move(3, 100);
            ui->paysDonneesSat->move(114, 100);

            ui->lbl_siteLancementDonneesSat->move(3, 115);
            ui->siteLancementDonneesSat->move(114, 115);
        }

        // Categorie d'orbite
        const QString categorie = ligne.mid(104, 6).trimmed();
        ui->categorieOrbiteDonneesSat->setText((categorie.isEmpty()) ? tr("Inconnue") : categorie);

        // Pays/Organisation
        const QString pays = ligne.mid(111, 5).trimmed();
        ui->paysDonneesSat->setText((pays.isEmpty()) ? tr("Inconnu") : pays);

        // Site de lancement
        const QString siteLancement = ligne.mid(117, 5).trimmed();
        ui->siteLancementDonneesSat->setText((siteLancement.isEmpty()) ? tr("Inconnu") : siteLancement);

        // Recherche des fichiers TLE dans lesquels le satellite est present
        ui->fichiersTle->clear();
        const QDir di(dirTle);
        const QStringList filtres(QStringList () << "*.txt");
        const QStringList listeFicTle = di.entryList(filtres, QDir::Files);

        foreach(const QString fic, listeFicTle) {

            QVector<TLE> tabtle;
            //TLE::LectureFichier(dirTle + QDir::separator() + fic, norad, tabtle);
            QFile fi(dirTle + QDir::separator() + fic);
            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);

            bool atr = false;
            while (!flux.atEnd() && !atr) {

                const QString lig = flux.readLine();
                if (lig.mid(0, 2) == "1 ") {
                    if (lig.mid(2, 5) == norad) {
                        atr = true;
                    }
                }
            }
            fi.close();

            if (atr) {
                ui->fichiersTle->addItem(fic);
            }
        }

        if (ui->fichiersTle->count() > 0) {
            ui->lbl_fichiersTle->setVisible(true);
            ui->fichiersTle->setVisible(true);
        } else {
            ui->lbl_fichiersTle->setVisible(false);
            ui->fichiersTle->setVisible(false);
        }
    }

    /* Retour */
    return;
}

void PreviSat::AffichageResultats()
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->satellitesTrouves->clear();
    ui->frameResultats->setVisible(false);
    ui->lbl_fichiersTle->setVisible(false);
    ui->fichiersTle->setVisible(false);

    /* Corps de la methode */
    if (resultatsSatellitesTrouves.count() == 0) {
        ui->lbl_satellitesTrouves->setText(tr("Objets trouvés :"));
    } else {

        const QString chaine = tr("Objets trouvés (%1) :");
        ui->lbl_satellitesTrouves->setText(chaine.arg(resultatsSatellitesTrouves.count()));

        // Remplissage de la liste de resultats
        QStringListIterator it(resultatsSatellitesTrouves);
        while (it.hasNext()) {
            const QString item = it.next().toUpper();
            QString nomsat = item.mid(123).trimmed();

            if (nomsat.length() == 0) {
                nomsat = item.mid(0, 5);
            }

            ui->satellitesTrouves->addItem(nomsat);
        }
        ui->satellitesTrouves->setCurrentRow(0);
        ui->frameResultats->setVisible(true);
    }

    /* Retour */
    return;
}


/*
 * Gestion des options d'affichage
 */
void PreviSat::on_preferences_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->optionConfig->isVisible() && selec4 == 0) {

        if (index == ui->preferences->count() - 2) {
            selec4 = -1;

            // Sauvegarde d'un fichier de preferences
            QString fichier = getText(this, tr("Enregistrer sous..."), tr("Nom du fichier :"), tr("OK"), tr("Annuler"), QLineEdit::Normal,
                                      QString(), Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

            if (fichier.isEmpty()) {
                const QFileInfo fi(settings.value("fichier/preferences", dirPrf + QDir::separator() + "defaut").toString());
                const QString fic = fi.completeBaseName().at(0).toUpper() + fi.completeBaseName().mid(1).toLower();
                ui->preferences->setCurrentIndex(qMax(0, ui->preferences->findText(fic)));
                ChargementPref();

            } else {
                fichier = dirPrf + QDir::separator() + fichier.toLower() + ".prf";
                const QFileInfo fi(fichier);
                bool ok = true;
                if (fi.exists()) {

                    QMessageBox msgbox(tr("Information"), tr("Le fichier existe déjà. Voulez-vous l'écraser?"), QMessageBox::Question,
                                       QMessageBox::Yes, QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
                    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
                    msgbox.setButtonText(QMessageBox::No, tr("Non"));
                    msgbox.exec();
                    ok = (msgbox.result() == QMessageBox::Yes);

                } else {
                    ok = true;
                }

                if (ok) {
                    SauvePreferences(fichier);
                    InitFicPref(true);
                    const QString fic = fi.completeBaseName().at(0).toUpper() + fi.completeBaseName().mid(1);
                    ui->preferences->setCurrentIndex(ui->preferences->findText(fic));
                }
            }
            selec4 = 0;

        } else if (index == ui->preferences->count() - 1) {
            selec4 = -1;

            // Suppression d'un fichier de preferences
            const QDir di(dirPrf);
            const QStringList filtres(QStringList () << "*.prf");
            const QStringList items = di.entryList(filtres, QDir::Files).replaceInStrings(".prf", "");

            if (items.count() > 0) {
                QStringList listePrf;
                foreach(QString item, items) {
                    listePrf.append(item.at(0).toUpper() + item.mid(1));
                }

                const QString text(listePrf.value(ui->preferences->currentIndex()));
                QInputDialog input(this, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                input.setWindowTitle(tr("Supprimer un fichier..."));
                input.setLabelText(tr("Fichier :"));
                input.setComboBoxItems(listePrf);
                input.setTextValue(text);
                input.setComboBoxEditable(false);
                input.setOkButtonText(tr("Supprimer"));
                input.setCancelButtonText(tr("Annuler"));
                input.setOption(QInputDialog::UseListViewForComboBoxItems, true);
                const int ret = input.exec();
                const QString fichier = (ret) ? input.textValue() : "";

                if (fichier.isEmpty()) {
                    const QFileInfo fi(settings.value("fichier/preferences", dirPrf + QDir::separator() + "defaut").toString());
                    const QString fic = fi.completeBaseName().at(0).toUpper() + fi.completeBaseName().mid(1);
                    ui->preferences->setCurrentIndex(qMax(0, ui->preferences->findText(fic)));

                } else {
                    if (ret && fichier != "defaut") {
                        const QString fic = dirPrf + QDir::separator() + fichier.toLower() + ".prf";
                        QFile fi(fic);
                        fi.remove();
                        InitFicPref(true);
                        ui->preferences->setCurrentIndex(qMax(0, ui->preferences->currentIndex() - 1));
                        ChargementPref();
                    }
                }
            } else {
                ui->preferences->setCurrentIndex(0);
            }
            selec4 = 0;

        } else {
            ChargementPref();
            settings.setValue("fichier/preferences", ficPrf.at(qMax(0, ui->preferences->currentIndex())));
        }

        AffichageCourbes();
    }

    /* Retour */
    return;
}

void PreviSat::on_enregistrerPref_clicked()
{
    if (ui->preferences->currentIndex() < ui->preferences->count() - 2) {
        SauvePreferences(ficPrf.at(ui->preferences->currentIndex()));
    }
}

void PreviSat::on_affsoleil_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affnuit_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
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
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_afflune_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affphaselune_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_rotationIconeISS_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_rotationLune_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affnomsat_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affvisib_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_afftraceCiel_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_afftraj_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_nombreTrajectoires_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affradar_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}


void PreviSat::on_afficone_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affinvns_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affinvew_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affnomlieu_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affnotif_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}


void PreviSat::on_calJulien_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affcoord_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    if (messagesStatut2 != NULL) {
        messagesStatut2->setVisible(false);
    }
    if (messagesStatut2 != NULL) {
        messagesStatut3->setVisible(false);
    }
    ModificationOption();
}

void PreviSat::on_extinctionAtmospherique_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_refractionPourEclipses_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_effetEclipsesMagnitude_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_eclipsesLune_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affetoiles_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affconst_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_magnitudeEtoiles_valueChanged(double arg1)
{
    settings.setValue("affichage/magnitudeEtoiles", arg1);
    ModificationOption();
}

void PreviSat::on_affSAA_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_affplanetes_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ModificationOption();
}

void PreviSat::on_intensiteVision_valueChanged(int value)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->intensiteVision->isVisible()) {
        on_actionVision_nocturne_toggled(ui->actionVision_nocturne->isChecked());
        ui->intensiteVision->setToolTip(QString::number(value));
    }

    /* Retour */
    return;
}

void PreviSat::on_unitesKm_toggled(bool checked)
{
    Q_UNUSED(checked)

    if (checked) {
        ui->nvAltitude->setText("0000");
        ui->nvAltitude->setInputMask("####");
        const QIntValidator *valAlt = new QIntValidator(-500, 8900);
        ui->nvAltitude->setValidator(valAlt);
    } else {
        ui->nvAltitude->setText("00000");
        ui->nvAltitude->setInputMask("#####");
        const QIntValidator *valAlt = new QIntValidator(-1640, 29200);
        ui->nvAltitude->setValidator(valAlt);
    }

    ModificationOption();

    // Affichage des manoeuvres ISS
    if (!tabManoeuvresISS.isEmpty()) {
        AffichageManoeuvresISS();
    }
}

void PreviSat::on_unitesMi_toggled(bool checked)
{
    Q_UNUSED(checked)

    if (checked) {
        ui->nvAltitude->setText("00000");
        ui->nvAltitude->setInputMask("#####");
        const QIntValidator *valAlt = new QIntValidator(-1640, 29200);
        ui->nvAltitude->setValidator(valAlt);
    } else {
        ui->nvAltitude->setText("0000");
        ui->nvAltitude->setInputMask("####");
        const QIntValidator *valAlt = new QIntValidator(-500, 8900);
        ui->nvAltitude->setValidator(valAlt);
    }

    ModificationOption();

    // Affichage des manoeuvres ISS
    if (!tabManoeuvresISS.isEmpty()) {
        AffichageManoeuvresISS();
    }
}

void PreviSat::on_syst24h_toggled(bool checked)
{
    Q_UNUSED(checked)
    ModificationOption();
}

void PreviSat::on_syst12h_toggled(bool checked)
{
    Q_UNUSED(checked)
    ModificationOption();
}

void PreviSat::on_proportionsCarte_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    resizeEvent(NULL);
}

void PreviSat::on_typeRepere_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    if (ui->typeRepere->isVisible()) {
        AffichageElementsOsculateurs();
    }
}

void PreviSat::on_typeParametres_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    if (ui->typeParametres->isVisible()) {
        AffichageElementsOsculateurs();
    }
}

void PreviSat::on_heureLegale_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->options->isVisible() && checked) {

        offsetUTC = ui->updown->value() * NB_JOUR_PAR_MIN;
        acalcAOS = true;
        acalcDN = true;
        dateCourante = Date(dateCourante, offsetUTC);

        ModificationOption();
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

        offsetUTC = 0.;
        acalcAOS = true;
        acalcDN = true;
        dateCourante = Date(dateCourante, 0.);

        ModificationOption();
    }

    /* Retour */
    return;
}

void PreviSat::on_updown_valueChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QTime heur(0, 0);
    heur = heur.addSecs((int) fabs(arg1 * NB_SEC_PAR_MIN));
    const QString sgnh = (arg1 >= 0) ? " + " : " - ";
    ui->tuc->setText(tr("UTC") + sgnh + heur.toString("hh:mm"));
    offsetUTC = ui->updown->value() * NB_JOUR_PAR_MIN;

    if (ui->options->isVisible()) {

        if (ui->heureLegale->isChecked()) {

            acalcAOS = true;
            acalcDN = true;
            dateCourante = Date(dateCourante.jourJulien() - offsetUTC, offsetUTC);

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
    Q_UNUSED(arg1)

    /* Corps de la methode */
    if (ui->utcAuto->isChecked()) {

        const QDateTime dateLocale = QDateTime::currentDateTime();
        QDateTime dateUTC(dateLocale);
        dateUTC.setTimeSpec(Qt::UTC);

        const int ecart = (int) ((double) dateLocale.secsTo(dateUTC) * NB_MIN_PAR_SEC + EPS_DATES);
        ui->updown->setValue(ecart);

        offsetUTC = ui->updown->value() * NB_JOUR_PAR_MIN;
        dateCourante = Date(dateCourante, offsetUTC);

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
    if (ui->optionConfig->isVisible() && selec2 == 0) {
        if (index == 0) {
            settings.setValue("fichier/listeMap", "");
        } else {
            if (index == ui->listeMap->count() - 1) {
                selec2 = -1;

                Telecharger * const telecharger = new Telecharger(2);
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

void PreviSat::on_listeSons_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->optionConfig->isVisible() && selec3 == 0) {
        if (index == 0) {
            settings.setValue("fichier/listeSon", "");
        } else {
            if (index == ui->listeSons->count() - 1) {
                selec3 = -1;

                Telecharger * const telecharger = new Telecharger(3);
                telecharger->setWindowModality(Qt::ApplicationModal);
                telecharger->show();
                ui->listeSons->setCurrentIndex(ui->listeSons->findText(settings.value("fichier/listeSon", "").toString()));
                InitFicSon();
                selec3 = 0;
            } else {
                settings.setValue("fichier/listeSon", (index == 0) ? "" : ficSonAOS.at(qMax(0, ui->listeSons->currentIndex()-1)));
            }
        }
    }

    /* Retour */
    return;
}


void PreviSat::on_affBetaWCC_toggled(bool checked)
{
    if (checked) {
        if (ui->orbiteISS->isVisible()) {
            ui->inclinaisonISS->move(5, 39);
            ui->nextTransitionISS->move(112, 0);
            ui->orbiteISS->move(112, 13);
            ui->frameCoordISS->resize(ui->frameCoordISS->width(), 59);
        } else {
            ui->frameCoordISS->resize(ui->frameCoordISS->width(), 46);
        }
        ui->betaISS->move(112, 26);

    } else {
        ui->inclinaisonISS->move(112, 0);
        ui->nextTransitionISS->move(112, 13);
        ui->orbiteISS->move(112, 26);
        ui->frameCoordISS->resize(ui->frameCoordISS->width(), 46);
    }
    ui->betaISS->setVisible(checked);
}

void PreviSat::on_affCerclesAcq_toggled(bool checked)
{
    Q_UNUSED(checked)
    ModificationOption();
}

void PreviSat::on_affNbOrbWCC_toggled(bool checked)
{
    if (ui->affBetaWCC->isChecked()) {
        if (checked) {
            ui->inclinaisonISS->move(5, 39);
            ui->nextTransitionISS->move(112, 0);
            ui->orbiteISS->move(112, 13);
            ui->frameCoordISS->resize(ui->frameCoordISS->width(), 59);
        } else {
            ui->inclinaisonISS->move(112, 0);
            ui->nextTransitionISS->move(112, 13);
            ui->frameCoordISS->resize(ui->frameCoordISS->width(), 46);
        }
        ui->betaISS->move(112, 26);
    } else {
        ui->betaISS->move(112, 26);
        ui->frameCoordISS->resize(ui->frameCoordISS->width(), 46);
    }
    ui->orbiteISS->setVisible(checked);
}

void PreviSat::on_affSAA_ZOE_toggled(bool checked)
{
    Q_UNUSED(checked)
    ModificationOption();
}

void PreviSat::on_listeStations_clicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    ModificationOption();
}

void PreviSat::on_listeStations_customContextMenuRequested(const QPoint &position)
{
    Q_UNUSED(position)
    ui->menuContextuelListes->exec(QCursor::pos());
}

void PreviSat::on_styleWCC_toggled(bool checked)
{
    Q_UNUSED(checked)
    resizeEvent(NULL);
    ModificationOption();
}

void PreviSat::on_coulGMT_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ModificationOption();
}

void PreviSat::on_coulZOE_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ModificationOption();
}

void PreviSat::on_coulEquateur_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ModificationOption();
}

void PreviSat::on_coulTerminateur_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ModificationOption();
}

void PreviSat::on_coulCercleVisibilite_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ModificationOption();
}

void PreviSat::on_policeWCC_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
#if defined (Q_OS_WIN)
    const int taille = 10;
    QFont police(ui->policeWCC->itemText(index), taille, ((ui->policeWCC->currentIndex() == 0) ? QFont::Normal : QFont::Bold));

#elif defined (Q_OS_LINUX)
    const int taille = 11;
    QFont police(ui->policeWCC->itemText(index), taille);

#elif defined (Q_OS_MAC)
    const int taille = 13;
    QFont police(ui->policeWCC->itemText(index), taille, ((ui->policeWCC->currentIndex() == 0) ? QFont::Normal : QFont::Bold));

#else
    const int taille = 11;
    QFont police(ui->policeWCC->itemText(index), taille);
#endif

    ui->altitudeISS->setFont(police);
    ui->betaISS->setFont(police);
    ui->inclinaisonISS->setFont(police);
    ui->latitudeISS->setFont(police);
    ui->longitudeISS->setFont(police);
    ui->nextTransitionISS->setFont(police);
    ui->orbiteISS->setFont(police);

    police.setPointSize(12);
    police.setBold(true);
    ui->gmt->setFont(police);
    ui->gmt->adjustSize();

    ModificationOption();

    /* Retour */
    return;
}


/*
 * Gestion des lieux d'observation
 */
void PreviSat::on_actionCreer_une_categorie_triggered()
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

void PreviSat::on_creationCategorie_clicked()
{
    on_actionCreer_une_categorie_triggered();
}

void PreviSat::on_actionSupprimerCategorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = ui->fichiersObs->currentItem()->text().toLower();
    const QString categorie = ui->fichiersObs->currentItem()->text();
    QString msg = tr("Voulez-vous vraiment supprimer la catégorie \"%1\"?");

    QMessageBox msgbox(tr("Information"), msg.arg(categorie), QMessageBox::Question, QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::No) {
        messagesStatut->setText("");
    } else {
        QFile fi(dirCoord + QDir::separator() + fic);
        fi.remove();
        ui->lieuxObs->clear();
        InitFicObs(false);
        ui->fichiersObs->setCurrentRow(0);
        msg = tr("La catégorie \"%1\" a été supprimée");
        messagesStatut->setText(msg.arg(categorie));
    }

    /* Retour */
    return;
}

void PreviSat::on_actionTelechargerCategorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Telecharger * const telecharger = new Telecharger(1);
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
    if (ui->nvCategorie->text().trimmed().isEmpty()) {
        Message::Afficher(tr("Le nom de la catégorie n'est pas spécifié"), WARNING);
    } else {
        int cpt = 0;
        for(int i=0; i<ui->fichiersObs->count(); i++) {
            if (ui->nvCategorie->text().toLower() == ui->fichiersObs->item(i)->text().toLower())
                cpt++;
        }
        if (cpt == 0) {
            QDir di(dirCoord);
            if (!di.exists())
                di.mkdir(dirCoord);

            QFile fi(dirCoord + QDir::separator() + ui->nvCategorie->text().toLower());
            fi.open(QIODevice::WriteOnly | QIODevice::Text);
            fi.write("");
            fi.close();
            InitFicObs(false);

            if (ui->fichiersObs->count() > 0) {
                ui->fichiersObs->setCurrentRow(0);
            }
            messagesStatut->setText(tr("La nouvelle catégorie de lieux d'observation a été créée"));
            ui->nouvelleCategorie->setVisible(false);

        } else {
            Message::Afficher(tr("La catégorie spécifiée existe déjà"), WARNING);
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionRenommerCategorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString nvNomCateg = getText(this, tr("Catégorie"), tr("Nouveau nom de la catégorie :"), tr("OK"), tr("Annuler"),
                                       QLineEdit::Normal, ui->fichiersObs->currentItem()->text(), Qt::WindowTitleHint | Qt::WindowCloseButtonHint);


    if (!nvNomCateg.trimmed().isEmpty()) {

        bool ok = true;
        QFile fi(dirCoord + QDir::separator() + ui->fichiersObs->currentItem()->text().toLower());

        if (fi.exists()) {

            QMessageBox msgbox(tr("Information"), tr("La catégorie existe déjà. Voulez-vous l'écraser?"), QMessageBox::Question, QMessageBox::Yes,
                               QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
            msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
            msgbox.setButtonText(QMessageBox::No, tr("Non"));
            msgbox.exec();
            ok = (msgbox.result() == QMessageBox::Yes);

        } else {
            ok = true;
        }

        if (ok) {
            fi.rename(dirCoord + QDir::separator() + nvNomCateg.trimmed().toLower());
            InitFicObs(false);
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_fichiersObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (ui->nouveauLieu->isVisible() && !ui->ajdfic->isVisible()) {
        ui->nouveauLieu->setVisible(false);
        on_validerObs_clicked();
    }

    /* Corps de la methode */
    ui->coordonnees->setVisible(false);
    ui->selecLieux->setCurrentRow(-1);
    if (selec != currentRow) {
        selec = currentRow;
        if (selec < 0) {
            return;
        }
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
        mapObs.append(ligne.mid(34).trimmed() + " #" + ligne.mid(0, 33).replace(" ", "&"));
        for (int i=0; i<listeObs.size(); i++) {
            if (mapObs.last().mid(0, mapObs.last().indexOf("#")) == listeObs.at(i).mid(0, listeObs.at(i).indexOf("#"))) {
                if (mapObs.last() != listeObs.at(i)) {
                    listeObs[i] = mapObs.last();
                }
            }
        }
    }
    mapObs.sort();

    QString nomlieu = "";
    for(int i=0; i<listeObs.count(); i++) {
        if (i > 0) nomlieu += "$";
        nomlieu += listeObs.at(i);
    }
    settings.setValue("observateur/lieu", nomlieu);

    int i = 0;
    while (i < mapObs.length()) {
        ui->lieuxObs->addItem(mapObs.at(i).mid(0, mapObs.at(i).indexOf("#")).trimmed());
        i++;
    }

    /* Retour */
    return;
}

void PreviSat::on_fichiersObs_customContextMenuRequested(const QPoint &position)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(position)

    /* Corps de la methode */
    ui->actionSupprimerCategorie->setVisible(ui->fichiersObs->currentRow() > 0);
    ui->actionRenommerCategorie->setVisible(ui->fichiersObs->currentRow() > 0);
    ui->menuContextuelCategorie->exec(QCursor::pos());

    /* Retour */
    return;
}

void PreviSat::on_lieuxObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->nouveauLieu->isVisible() && !ui->ajdfic->isVisible()) {
        ui->nouveauLieu->setVisible(false);
        on_validerObs_clicked();
    }
    AfficherLieuSelectionne(currentRow);

    /* Retour */
    return;
}

void PreviSat::on_lieuxObs_customContextMenuRequested(const QPoint &position)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->lieuxObs->indexAt(position).row() < 0) {
        ui->actionRenommerLieu->setVisible(false);
        ui->actionModifier_coordonnees->setVisible(false);
        ui->actionSupprimerLieu->setVisible(false);
        ui->actionAjouter_Mes_Preferes->setVisible(false);
    } else {
        ui->actionRenommerLieu->setVisible(true);
        ui->actionModifier_coordonnees->setVisible(true);
        ui->actionSupprimerLieu->setVisible(true);
        const QFileInfo fi(dirCoord + QDir::separator() + "preferes");
        ui->actionAjouter_Mes_Preferes->setVisible(fi.exists() ? ui->fichiersObs->currentRow() > 0 :
                                                                 ui->fichiersObs->currentRow() >= 0);
    }
    ui->actionCreer_un_nouveau_lieu->setVisible(ui->fichiersObs->count() > 0);
    ui->menuContextuelLieux->exec(QCursor::pos());

    /* Retour */
    return;
}

void PreviSat::on_selecLieux_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->nouveauLieu->isVisible() && !ui->ajdfic->isVisible()) {
        ui->nouveauLieu->setVisible(false);
        on_validerObs_clicked();
    }
    AfficherLieuSelectionne(currentRow);

    /* Retour */
    return;
}

void PreviSat::on_selecLieux_customContextMenuRequested(const QPoint &position)
{
    Q_UNUSED(position)
    if (ui->selecLieux->count() > 1 && ui->selecLieux->indexAt(position).row() >= 0)
        ui->menuContextuelLieuxSelec->exec(QCursor::pos());
}

void PreviSat::on_actionCreer_un_nouveau_lieu_triggered()
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

    if (ui->unitesKm->isChecked()) {
        ui->nvAltitude->setText("0000");
        ui->nvAltitude->setInputMask("####");
    } else {
        ui->nvAltitude->setText("00000");
        ui->nvAltitude->setInputMask("#####");
    }

    ui->lbl_nvUnite->setText((ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
    ui->lbl_ajouterDans->setVisible(true);
    ui->ajdfic->setVisible(true);
    ui->ajdfic->setCurrentIndex(ui->fichiersObs->currentRow());
    messagesStatut->setText("");
    ui->nvLieu->setFocus();

    /* Retour */
    return;
}

void PreviSat::on_creationLieu_clicked()
{
    on_actionCreer_un_nouveau_lieu_triggered();
}

void PreviSat::on_actionAjouter_Mes_Preferes_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString fic = ficObs.at(0);
    if (!fic.contains("preferes")) {
        // Le fichier preferes n'existe pas, on le cree
        ficObs.insert(0, dirCoord + QDir::separator() + "preferes");
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
        Message::Afficher(msg.arg(nomlieu.split("#").at(0)), WARNING);
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

        const QString msg = tr("Le lieu d'observation \"%1\" a été ajouté dans la catégorie \"Mes Préférés\"");
        messagesStatut->setText(msg.arg(lieu.at(0).toUpper() + lieu.mid(1)));
    }

    /* Retour */
    return;
}

void PreviSat::on_actionModifier_coordonnees_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->nouvelleCategorie->setVisible(false);
    ui->nouveauLieu->setVisible(true);
    ui->coordonnees->setVisible(false);
    if (ui->unitesKm->isChecked()) {
        ui->nvAltitude->setInputMask("####");
    } else {
        ui->nvAltitude->setInputMask("#####");
    }

    const QString obs = mapObs.at(ui->lieuxObs->currentRow());
    ui->nvLieu->setText(obs.mid(0, obs.indexOf("#")).trimmed());

    const QStringList coord = obs.mid(obs.indexOf("#") + 1).split("&");
    const double lo = coord.at(0).toDouble();
    const double la = coord.at(1).toDouble();
    const int atd = coord.at(2).toInt();

    ui->nvLongitude->setText(Maths::ToSexagesimal(fabs(lo) * DEG2RAD, DEGRE, 3, 0, false, true));
    ui->nvLongitude->setPalette(QPalette());
    ui->nvEw->setCurrentIndex((lo <= 0.) ? 0 : 1);

    ui->nvLatitude->setText(Maths::ToSexagesimal(fabs(la) * DEG2RAD, DEGRE, 2, 0,false, true));
    ui->nvLatitude->setPalette(QPalette());
    ui->nvNs->setCurrentIndex((la >= 0.) ? 0 : 1);

    const QString alt = "%1";
    if (ui->unitesKm->isChecked()) {
        ui->nvAltitude->setText(alt.arg(atd, 4, 10, QChar('0')));
    } else {
        ui->nvAltitude->setText(alt.arg(qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd)), 5, 10, QChar('0')));
    }
    ui->nvAltitude->setPalette(QPalette());
    ui->lbl_nvUnite->setText((ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
    ligneCoord = QString("%1&%2&%3").arg(ui->nvLongitude->text()).arg(ui->nvLatitude->text()).arg(alt.arg(atd, 4, 10, QChar('0')));

    ui->lbl_ajouterDans->setVisible(false);
    ui->ajdfic->setVisible(false);

    // Suppression du lieu du fichier
    const QStringList lieu = mapObs.at(ui->lieuxObs->currentRow()).split("#");
    const QString msg2 = "%1%2 %3%4 %5 %6";
    const QString ligne =  msg2.arg((lo >= 0.) ? "+" : "-").arg(fabs(lo), 13, 'f', 9, QChar('0')).
            arg((la >= 0.) ? "+" : "-").arg(fabs(la), 12, 'f', 9, QChar('0')).arg(atd, 4, 10, QChar('0')).arg(lieu.at(0)).trimmed();

    const QString fic = (ui->fichiersObs->currentRow() == 0) ?
                dirCoord + QDir::separator() + "preferes" :
                dirCoord + QDir::separator() + ui->fichiersObs->currentItem()->text();

    QFile sr(fic);
    sr.open(QIODevice::ReadOnly | QIODevice::Text);

    QFile sw(dirTmp + QDir::separator() + "obs.tmp");
    sw.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream flux(&sr);
    QTextStream flux2(&sw);
    while (!flux.atEnd()) {
        const QString ligne2 = flux.readLine();
        if (ligne.mid(34).trimmed() != ligne2.mid(34).trimmed()) {
            flux2 << ligne2 << endl;
        }
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
    try {
        QString nomlieu = ui->nvLieu->text().trimmed();
        if (nomlieu.isEmpty()) {
            throw PreviSatException(tr("Le nom du lieu d'observation n'est pas spécifié"), WARNING);
        } else {

            ui->nouveauLieu->setVisible(false);
            nomlieu[0] = nomlieu.at(0).toUpper();

            // Recuperation de la longitude
            const int lo1 = ui->nvLongitude->text().mid(0, 3).toInt();
            const int lo2 = ui->nvLongitude->text().mid(4, 2).toInt();
            const int lo3 = ui->nvLongitude->text().mid(7, 2).toInt();

            if (lo1 < 0 || lo1 > 180 || lo2 < 0 || lo2 > 59 || lo3 < 0 || lo3 > 59) {
                throw PreviSatException(tr("Erreur dans la saisie de la longitude"), WARNING);
            }

            // Recuperation de la latitude
            const int la1 = ui->nvLatitude->text().mid(0, 2).toInt();
            const int la2 = ui->nvLatitude->text().mid(3, 2).toInt();
            const int la3 = ui->nvLatitude->text().mid(6, 2).toInt();

            if (la1 < 0 || la1 > 90 || la2 < 0 || la2 > 59 || la3 < 0 || la3 > 59) {
                throw PreviSatException(tr("Erreur dans la saisie de la latitude"), WARNING);
            }

            // Recuperation de l'altitude
            int atd = ui->nvAltitude->text().mid(0, 5).toInt();
            if (ui->unitesMi->isChecked()) {
                atd = qRound(atd / PIED_PAR_METRE);
            }
            if (atd < -500 || atd > 8900) {
                throw PreviSatException(tr("Erreur dans la saisie de l'altitude"), WARNING);
            }

            const QString fic = ficObs.at(ui->ajdfic->currentIndex());

            // Ajout du lieu d'observation dans le fichier de coordonnees selectionne
            QFile fi(fic);
            fi.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream flux(&fi);
            while (!flux.atEnd()) {
                const QString ligne = flux.readLine();
                if (ligne.mid(34).toLower().trimmed() == nomlieu.toLower().trimmed() && ui->ajdfic->isVisible()) {
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
            AffichageLieuObs();
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
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->nouveauLieu->isVisible() && !ui->ajdfic->isVisible()) {
        ui->nouveauLieu->setVisible(false);

        if (!ligneCoord.isEmpty()) {
            const QStringList list = ligneCoord.split("&");
            ui->nvLongitude->setText(list.at(0));
            ui->nvLatitude->setText(list.at(1));
            ui->nvAltitude->setText(list.at(2));
        }
        on_validerObs_clicked();
    }
    ui->nouveauLieu->setVisible(false);

    /* Retour */
    return;
}

void PreviSat::on_actionRenommerLieu_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString nvNomLieu = getText(this, tr("Lieu d'observation"), tr("Nouveau nom du lieu d'observation :"), tr("OK"), tr("Annuler"),
                                      QLineEdit::Normal, ui->lieuxObs->item(ui->lieuxObs->currentRow())->text(),
                                      Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    if (!nvNomLieu.trimmed().isEmpty()) {

        const QString fic = (ui->fichiersObs->currentRow() == 0) ?
                    dirCoord + QDir::separator() + "preferes" :
                    dirCoord + QDir::separator() + ui->fichiersObs->currentItem()->text();

        QFile sr(fic);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);

        QFile sw(dirTmp + QDir::separator() + "obs.tmp");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream flux(&sr);
        QTextStream flux2(&sw);
        while (!flux.atEnd()) {
            const QString ligne = flux.readLine();
            if (ligne.mid(34).toLower().trimmed() == ui->lieuxObs->currentItem()->text().toLower().trimmed()) {
                flux2 << ligne.mid(0, 34) << nvNomLieu.trimmed() << endl;
            } else {
                flux2 << ligne << endl;
            }
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

void PreviSat::on_actionSupprimerLieu_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = ficObs.at(ui->fichiersObs->currentRow());
    const QString nomlieu = ui->lieuxObs->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer \"%1\" de la catégorie \"%2\"?");

    QMessageBox msgbox(tr("Avertissement"), msg.arg(nomlieu).arg(ui->fichiersObs->currentItem()->text()), QMessageBox::Question,
                       QMessageBox::Yes, QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

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
            if (ligne.trimmed() != ligne2.trimmed()) {
                flux2 << ligne2 << endl;
            }
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

void PreviSat::on_actionSupprimerLieuSelec_triggered()
{
    on_supprLieu_clicked();
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

        acalcAOS = true;
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
    ui->actionEnregistrer->setVisible(((ui->onglets->currentIndex() < 3) && (ui->onglets->count() == nbOnglets))
                                      || (ui->onglets->currentIndex() < 1));
}

void PreviSat::on_onglets_currentChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (messagesStatut != NULL) {
        messagesStatut->setText("");
    }
    ui->compteRenduMaj->setVisible(false);
    const QString fmt = tr("dd/MM/yyyy hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "");

    /* Corps de la methode */
    if (index == ui->onglets->indexOf(ui->osculateurs)) {

        if (ui->modeManuel->isChecked()) {
            ui->dateHeure4->setDisplayFormat(tr("dddd dd MMMM yyyy  hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : ""));
            ui->dateHeure4->setDateTime(ui->dateHeure3->dateTime());
        }

    } else if (index == ui->onglets->indexOf(ui->informations)) {

        on_rechercheDonneesSat_toggled(ui->rechercheDonneesSat->isChecked());

    } else if (index == ui->onglets->indexOf(ui->previsions)) {

        const Date date(dateCourante.jourJulien() + EPS_DATES, 0.);
        ui->dateInitialePrev->setDateTime(date.ToQDateTime(0));
        ui->dateInitialePrev->setDisplayFormat(fmt);
        ui->dateFinalePrev->setDateTime(ui->dateInitialePrev->dateTime().addDays(7));
        ui->dateFinalePrev->setDisplayFormat(fmt);

        ui->afficherPrev->setDefault(false);
        ui->calculsPrev->setDefault(true);
        ui->calculsPrev->setFocus();

    } else if (index == ui->onglets->indexOf(ui->flashs)) {

        const Date date(dateCourante.jourJulien() + EPS_DATES, 0.);
        ui->dateInitialeMetOp->setDateTime(date.ToQDateTime(0));
        ui->dateInitialeMetOp->setDisplayFormat(fmt);
        ui->dateFinaleMetOp->setDateTime(ui->dateInitialeMetOp->dateTime().addDays(7));
        ui->dateFinaleMetOp->setDisplayFormat(fmt);

        ui->afficherMetOp->setDefault(false);
        ui->calculsMetOp->setDefault(true);
        ui->calculsMetOp->setFocus();

    } else if (index == ui->onglets->indexOf(ui->transits_ISS)) {

        const Date date(dateCourante.jourJulien() + EPS_DATES, 0.);
        ui->dateInitialeTransit->setDateTime(date.ToQDateTime(0));
        ui->dateInitialeTransit->setDisplayFormat(fmt);
        ui->dateFinaleTransit->setDateTime(ui->dateInitialeTransit->dateTime().addDays(12));
        ui->dateFinaleTransit->setDisplayFormat(fmt);

        ui->afficherTransit->setDefault(false);
        ui->calculsTransit->setDefault(true);
        ui->calculsTransit->setFocus();

        if (tab3le.isEmpty()) {
            ui->ageTLETransit->setVisible(false);
            ui->lbl_ageTLETransit->setVisible(false);
        } else {
            CalculAgeTLETransitISS();
        }

    } else {
    }

    if (ui->miseAJourTLE->isVisible()) {
        ui->majMaintenant->setDefault(true);
        ui->majMaintenant->setFocus();
    }

    if (ui->evenementsOrbitaux->isVisible()) {
        ui->afficherEvt->setDefault(false);
        ui->calculsEvt->setDefault(true);
        ui->calculsEvt->setFocus();
    }

    if (ui->transits_ISS->isVisible()) {
        ui->afficherTransit->setDefault(false);
        ui->calculsTransit->setDefault(true);
        ui->calculsTransit->setFocus();

        if (tab3le.isEmpty()) {
            ui->ageTLETransit->setVisible(false);
            ui->lbl_ageTLETransit->setVisible(false);
        } else {
            CalculAgeTLETransitISS();
        }
    }

    if (ui->flashs->isVisible()) {
        ui->afficherMetOp->setDefault(false);
        ui->calculsMetOp->setDefault(true);
        ui->calculsMetOp->setFocus();
    }

    /* Retour */
    return;
}

void PreviSat::on_ongletsOutils_currentChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt = tr("dd/MM/yyyy hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : "");

    /* Corps de la methode */
    if (index == ui->ongletsOutils->indexOf(ui->evenementsOrbitaux)) {

        const Date date(dateCourante.jourJulien() + EPS_DATES, 0.);
        ui->dateInitialeEvt->setDateTime(date.ToQDateTime(0));
        ui->dateInitialeEvt->setDisplayFormat(fmt);
        ui->dateFinaleEvt->setDateTime(ui->dateInitialeEvt->dateTime().addDays(7));
        ui->dateFinaleEvt->setDisplayFormat(fmt);

        ui->afficherEvt->setDefault(false);
        ui->calculsEvt->setDefault(true);
        ui->calculsEvt->setFocus();

    } else {
    }

    /* Retour */
    return;
}


/*
 * Mise a jour des TLE
 */
void PreviSat::on_groupeTLE_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->majMaintenant->setFocus();
}

void PreviSat::on_majMaintenant_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    aupdnow = true;
    dirDwn = dirTmp;
    const QString groupeTLE = ui->groupeTLE->currentText().toLower();
    ui->majMaintenant->setEnabled(false);
    ui->compteRenduMaj->clear();
    ui->compteRenduMaj->setVisible(true);

    /* Corps de la methode */
    // Telechargement des fichiers
    QFile fi(dirLocalData + QDir::separator() + "gestionnaireTLE_" + localePreviSat + ".gst");
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    const QString chaine = tr("Mise à jour du groupe de TLE \"%1\" (à partir de %2) en cours...");
    const QStringList list = ui->groupeTLE->currentText().split("@", QString::SkipEmptyParts);
    messagesStatut->setText(chaine.arg(list.at(0)).arg(list.at(1)));

    ui->affichageMsgMAJ->setVisible(false);
    ui->frameBarreProgression->setVisible(true);
    while (!flux.atEnd()) {
        const QStringList ligne = flux.readLine().split("#", QString::SkipEmptyParts);
        if (ligne.at(0) == groupeTLE) {

            QString adresse = ligne.at(0).split("@", QString::SkipEmptyParts).at(1);
            if (adresse.contains("celestrak")) {
                adresse = adresseCelestrakNorad;
            }
            if (adresse.contains("astropedia")) {
                adresse = adresseAstropedia + "previsat/tle/";
            }
            if (!adresse.startsWith("http://")) {
                adresse.insert(0, "http://");
            }
            if (!adresse.endsWith("/")) {
                adresse.append("/");
            }

            const QStringList listeTLEs = ligne.at(2).split(",", QString::SkipEmptyParts);
            foreach(QString file, listeTLEs) {
                AjoutFichier(QUrl(adresse + file.trimmed()));
            }

            if (downQueue.isEmpty()) {
                QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
            }
        }
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
    aupdateCF5 = true;
    cptCF5 = 0;
    on_majMaintenant_clicked();
}

void PreviSat::MettreAJourGroupesTLE(const QString &groupe)
{
    /* Declarations des variables locales */

    /* Initialisations */
    aupdnow = true;
    aupdateCF5 = true;
    cptCF5 = 0;
    dirDwn = dirTmp;
    ui->majMaintenant->setEnabled(false);
    ui->compteRenduMaj->clear();
    ui->compteRenduMaj->setVisible(true);

    /* Corps de la methode */
    QFile fi(dirLocalData + QDir::separator() + "gestionnaireTLE_" + localePreviSat + ".gst");
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    while (!flux.atEnd()) {

        const QStringList ligne = flux.readLine().split("#", QString::SkipEmptyParts);
        if (ligne.at(0).startsWith(groupe + "@")) {

            for(int i=0; i<ui->groupeTLE->count(); i++) {

                const QString groupeTLE = ui->groupeTLE->itemText(i).toLower();
                if (ligne.at(0).toLower() == groupeTLE) {

                    messagesStatut->setText(tr("Mise à jour des groupes de TLE en cours..."));
                    ui->affichageMsgMAJ->setVisible(false);
                    ui->frameBarreProgression->setVisible(true);

                    QString adresse = ligne.at(0).split("@", QString::SkipEmptyParts).at(1);
                    if (adresse.contains("celestrak")) {
                        adresse = adresseCelestrakNorad;
                    }
                    if (adresse.contains("astropedia")) {
                        adresse = adresseAstropedia + "previsat/tle/";
                    }
                    if (!adresse.startsWith("http://")) {
                        adresse.insert(0, "http://");
                    }
                    if (!adresse.endsWith("/")) {
                        adresse.append("/");
                    }

                    const QStringList listeTLEs = ligne.at(2).split(",", QString::SkipEmptyParts);
                    foreach(QString file, listeTLEs) {
                        AjoutFichier(QUrl(adresse + file.trimmed()));
                    }

                    if (downQueue.isEmpty()) {
                        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
                    }
                }
            }
        }
    }

    /* Retour */
    return;
}

void PreviSat::on_actionMettre_jour_TLE_communs_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Mise a jour des groupes de TLE communs
    const QString groupe = tr("commun");
    MettreAJourGroupesTLE(groupe);

    /* Retour */
    return;
}

void PreviSat::on_actionMettre_jour_tous_les_groupes_de_TLE_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Mise a jour du TLE de l'ISS pour les transits
    TelechargementFichier(ISS_TRAJECTOIRE_NASA, true);

    // Creation du fichier iss.3le (les lignes sont verifiees avant l'ecriture du fichier)
    const QString ficHsf = dirTmp + QDir::separator() + ISS_TRAJECTOIRE_NASA.split("/", QString::SkipEmptyParts).last();
    const QString fichier3leIss = dirTle + QDir::separator() + "iss.3le";
    TLE::LectureTrajectoryData(ficHsf, fichier3leIss, tabManoeuvresISS);

    // Affichage des manoeuvres ISS
    if (!tabManoeuvresISS.isEmpty()) {
        AffichageManoeuvresISS();
    }

    // Mise a jour de tous les groupes de TLE
    const QString groupe = tr("tous");
    MettreAJourGroupesTLE(groupe);

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
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
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
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
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
    ui->compteRenduMaj->clear();
    ui->compteRenduMaj->setVisible(false);

    try {
        bool agz = false;
        if (ui->fichierAMettreAJour->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à mettre à jour n'est pas spécifié"), WARNING);
        }

        if (ui->fichierALire->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à lire n'est pas spécifié"), WARNING);
        }

        // Fichier a lire au format gz
        QFileInfo fi(ui->fichierALire->text());
        QString fic;
        if (fi.suffix() == "gz") {

            // Cas d'un fichier compresse au format gz
            fic = dirTmp + QDir::separator() + fi.completeBaseName();

            if (DecompressionFichierGz(ui->fichierALire->text(), fic)) {

                const int nsat = TLE::VerifieFichier(fic, false);
                if (nsat == 0) {
                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                    throw PreviSatException(msg.arg(ui->fichierALire->text()), WARNING);
                }
                agz = true;
            } else {
                const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                throw PreviSatException(msg.arg(ui->fichierALire->text()), WARNING);
            }
        } else {
            fic = QDir::toNativeSeparators(fi.absoluteFilePath());
        }

        const QStringList listeFic(QStringList () << ui->fichierAMettreAJour->text() << fic);
        foreach(QString file, listeFic) {
            fi = QFileInfo(file);
            if (!fi.exists()) {
                const QString msg = tr("Le fichier %1 n'existe pas");
                throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
            }
        }

        QStringList compteRendu;
        const int affMsg = ui->affichageMsgMAJ->currentIndex();
        TLE::MiseAJourFichier(ui->fichierAMettreAJour->text(), fic, affMsg, compteRendu);

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
            TLE::LectureFichier(nomfic, listeTLE, tles);

            l1 = tles.at(0).ligne1();
            l2 = tles.at(0).ligne2();

            if (nbSat > 0) {

                AfficherListeSatellites(nomfic, listeTLE, false);

                Satellite::initCalcul = false;

                // Lecture des donnees satellite
                Satellite::LectureDonnees(listeTLE, tles, satellites);

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

void PreviSat::on_compteRenduMaj_customContextMenuRequested(const QPoint &position)
{
    Q_UNUSED(position)
    ui->menuContextuelCompteRenduMaj->exec(QCursor::pos());
}

void PreviSat::on_actionCopier_dans_le_presse_papier_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QClipboard * const clipboard = QApplication::clipboard();
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
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz)"));
    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
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
#if defined (Q_OS_WIN)
    const QString nomRepDefaut = settings.value("fichier/sauvegarde", dirOut).toString().replace(QDir::separator(), "\\");
#else
    const QString nomRepDefaut = settings.value("fichier/sauvegarde", dirOut).toString();
#endif

    QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomRepDefaut, tr("Fichiers texte (*.txt)"));

    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
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
        if (ui->fichierALireCreerTLE->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à lire n'est pas spécifié"), WARNING);
        }

        if (ui->nomFichierPerso->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier personnel n'est pas spécifié"), WARNING);
        }

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
                    if (nsat == 0) {
                        const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                        throw PreviSatException(msg.arg(ficlu), WARNING);
                    }
                    ficlu = fic;
                } else {
                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                    throw PreviSatException(msg.arg(ficlu), WARNING);
                }
            }
        } else {
            const QString msg = tr("Le fichier %1 n'existe pas");
            throw PreviSatException(msg.arg(ficlu), WARNING);
        }

        fi = QFileInfo(ui->nomFichierPerso->text());
        QDir di(fi.absolutePath());
        if (!di.exists()) {
            di.mkpath(fi.absolutePath());
        }

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
            const QString msg = tr("Erreur rencontrée lors du chargement du fichier %1");
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
            listeSat.append(tle.norad());
            sats.append(Satellite(tle));
        }

        // Lecture du fichier de donnees satellite
        Satellite::LectureDonnees(listeSat, tabtle, sats);

        QFile sw(ui->nomFichierPerso->text());
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        for(int isat=0; isat<nbs; isat++) {

            // Numero NORAD
            const int norad = tabtle.at(isat).norad().toInt();
            bool aecr = (norad >= nrdmin && norad <= nrdmax);

            // Ascension droite du noeud ascendant
            aecr = aecr && (tabtle.at(isat).omegao() >= ascmin && tabtle.at(isat).omegao() <= ascmax);

            // Excentricite
            aecr = aecr && (tabtle.at(isat).ecco() >= exmin && tabtle.at(isat).ecco() <= exmax);

            // Inclinaison
            aecr = aecr && ((tabtle.at(isat).inclo() >= imin1 && tabtle.at(isat).inclo() <= imax1) ||
                            (tabtle.at(isat).inclo() >= imin2 && tabtle.at(isat).inclo() <= imax2 &&
                             ui->inclinaisonCreerTLE->currentIndex() == 1));

            // Moyen mouvement
            aecr = aecr && (tabtle.at(isat).no() >= nbrevmin && tabtle.at(isat).no() <= nbrevmax);

            // Argument du perigee
            aecr = aecr && (tabtle.at(isat).argpo() >= argmin && tabtle.at(isat).argpo() <= argmax);

            // Magnitude
            double mag;
            if (sats.at(isat).magnitudeStandard() < 98.9) {
                const double ax = RAYON_TERRESTRE * qPow(KE / (tabtle.at(isat).no() * DEUX_PI * NB_JOUR_PAR_MIN), DEUX_TIERS);
                mag = sats.at(isat).magnitudeStandard() - 15.75 + 5. * log10(1.45 * (ax * 1. - tabtle.at(isat).ecco()));
            } else {
                mag = -10.;
            }
            aecr = aecr && (mag <= mgmax) ;

            // Ecriture du TLE
            if (aecr) {
                if (tabtle.at(isat).nom() != tabtle.at(isat).norad())
                    flux << tabtle.at(isat).nom() << endl;
                flux << tabtle.at(isat).ligne1() << endl;
                flux << tabtle.at(isat).ligne2() << endl;
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

void PreviSat::on_liste2_customContextMenuRequested(const QPoint &position)
{
    Q_UNUSED(position)
    if (ui->liste2->currentRow() >= 0) {
        ui->menuContextuelListes->exec(QCursor::pos());
    }
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

void PreviSat::on_actionTous_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste2->hasFocus()) {
        for(int i=0; i<ui->liste2->count(); i++) {
            ui->liste2->item(i)->setCheckState(Qt::Checked);
        }
    }

    if (ui->liste3->hasFocus()) {
        for(int i=0; i<ui->liste3->count(); i++) {
            ui->liste3->item(i)->setCheckState(Qt::Checked);
        }
    }

    if (ui->listeStations->hasFocus()) {
        for(int i=0; i<ui->listeStations->count(); i++) {
            ui->listeStations->item(i)->setCheckState(Qt::Checked);
        }
        ModificationOption();
    }

    /* Retour */
    return;
}

void PreviSat::on_actionAucun_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->liste2->hasFocus()) {
        for(int i=0; i<ui->liste2->count(); i++) {
            ui->liste2->item(i)->setCheckState(Qt::Unchecked);
        }
    }

    if (ui->liste3->hasFocus()) {
        for(int i=0; i<ui->liste3->count(); i++) {
            ui->liste3->item(i)->setCheckState(Qt::Unchecked);
        }
    }

    if (ui->listeStations->hasFocus()) {
        for(int i=0; i<ui->listeStations->count(); i++) {
            ui->listeStations->item(i)->setCheckState(Qt::Unchecked);
        }
        ModificationOption();
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
        ui->valHauteurSatPrev->setText(settings.value("previsions/valHauteurSatPrev", 0).toString());
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
        ui->valHauteurSoleilPrev->setText(settings.value("previsions/valHauteurSoleilPrev", 0).toString());
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

void PreviSat::on_parametrageDefautPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_onglets_currentChanged(ui->onglets->indexOf(ui->previsions));
    ui->pasGeneration->setCurrentIndex(5);
    ui->lieuxObservation2->setCurrentIndex(0);
    ui->hauteurSatPrev->setCurrentIndex(0);
    ui->hauteurSoleilPrev->setCurrentIndex(1);
    ui->valHauteurSatPrev->setVisible(false);
    ui->valHauteurSoleilPrev->setVisible(false);
    ui->valMagnitudeMaxPrev->setVisible(false);
    ui->illuminationPrev->setChecked(true);
    ui->magnitudeMaxPrev->setChecked(false);
    if (!ui->calculsPrev->isEnabled() && !ui->afficherPrev->isEnabled() && threadCalculs == NULL) {
        ui->calculsPrev->setEnabled(true);
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL) {
        afficherResultats->deleteLater();
        afficherResultats = NULL;
    }
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherEvt->setEnabled(false);
    ui->afficherTransit->setEnabled(false);
    ui->afficherMetOp->setEnabled(false);

    try {
        if (ui->liste2->count() == 0) {
            throw PreviSatException();
        }

        const int nsat = getListeItemChecked(ui->liste2);
        if (nsat == 0 && ui->liste2->count() > 0) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), WARNING);
        }

        ui->afficherPrev->setEnabled(false);

        // Ecart heure locale - UTC
        const bool ecart = (fabs(offsetUTC - Date::CalculOffsetUTC(dateCourante.ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateInitialePrev->dateTime());
        const double offset2 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateFinalePrev->dateTime());

        // Date et heure initiales
        const Date date1(ui->dateInitialePrev->date().year(), ui->dateInitialePrev->date().month(), ui->dateInitialePrev->date().day(),
                         ui->dateInitialePrev->time().hour(), ui->dateInitialePrev->time().minute(),
                         ui->dateInitialePrev->time().second(), 0.);

        // Jour julien initial
        double jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(ui->dateFinalePrev->date().year(), ui->dateFinalePrev->date().month(), ui->dateFinalePrev->date().day(),
                         ui->dateFinalePrev->time().hour(), ui->dateFinalePrev->time().minute(), ui->dateFinalePrev->time().second(), 0.);

        // Jour julien final
        double jj2 = date2.jourJulien() - offset2;

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
                    abs(ui->valHauteurSatPrev->text().toInt()) : 5 * ui->hauteurSatPrev->currentIndex();

        // Hauteur maximale du Soleil
        int crep = 0;
        if (ui->hauteurSoleilPrev->currentIndex() <= 3) {
            crep = -6 * ui->hauteurSoleilPrev->currentIndex();
        } else if (ui->hauteurSoleilPrev->currentIndex() == 4) {
            crep = 90;
        } else if (ui->hauteurSoleilPrev->currentIndex() == 5) {
            crep = ui->valHauteurSoleilPrev->text().toInt();
        } else {
        }

        // Prise en compte de l'extinction atmospherique
        const bool ext = ui->extinctionAtmospherique->isChecked();

        // Prise en compte de la refraction atmospherique
        const bool refr = ui->refractionPourEclipses->isChecked();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        const bool effetEclPartielle = ui->effetEclipsesMagnitude->isChecked();

        // Prise en compte des eclipses de Lune
        const bool acalcEclipseLune = ui->eclipsesLune->isChecked();

        // Prise en compte du systeme horaire
        const bool syst = ui->syst24h->isChecked();

        // Liste des numeros NORAD
        QStringList listeSat;
        for (int i=0; i<ui->liste2->count(); i++) {
            if (ui->liste2->item(i)->checkState() == Qt::Checked)
                listeSat.append(ui->liste2->item(i)->text().split("#").at(1));
        }

        // Nom du fichier resultat
        const QString chaine = tr("previsions") + "_%1_%2.txt";
        ficRes = dirTmp + QDir::separator() + chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

        QFile fi(ficRes);
        if (fi.exists()) {
            fi.remove();
        }
        QDir di(dirTmp);
        if (!di.exists()) {
            di.mkpath(dirTmp);
        }

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsPrev->setEnabled(false);

        // Lancement des calculs
        conditions = Conditions(PREVISION, ecart, ecl, ext, refr, acalcEclipseLune, effetEclPartielle, syst, crep, haut, pas0, jj1, jj2,
                                offset1, mag, nomfic, ficRes, unite, listeSat);
        const Observateur obser(observateurs.at(ui->lieuxObservation2->currentIndex()));

        threadCalculs = new ThreadCalculs(conditions, obser);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_afficherPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QStringList result = threadCalculs->res();
    afficherResultats = new Afficher(conditions, threadCalculs->observateur(), result);
    afficherResultats->setWindowTitle(QString("%1 %2 - ").arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)) +
                                      tr("Prévisions de passage des satellites"));
    afficherResultats->show(ficRes);
    result.clear();

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

void PreviSat::on_liste3_customContextMenuRequested(const QPoint &position)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(position)

    /* Corps de la methode */
    if (ui->liste3->currentRow() >= 0) {
        ui->menuContextuelListes->exec(QCursor::pos());
    }

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

void PreviSat::on_parametrageDefautEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_ongletsOutils_currentChanged(ui->ongletsOutils->indexOf(ui->evenementsOrbitaux));
    ui->passageApogee->setChecked(true);
    ui->passageNoeuds->setChecked(true);
    ui->passageOmbre->setChecked(true);
    ui->passageQuadrangles->setChecked(true);
    ui->transitionJourNuit->setChecked(true);
    if (!ui->calculsEvt->isEnabled() && ! ui->afficherEvt->isEnabled() && threadCalculs == NULL) {
        ui->calculsEvt->setEnabled(true);
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL) {
        afficherResultats->deleteLater();
        afficherResultats = NULL;
    }
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherPrev->setEnabled(false);
    ui->afficherTransit->setEnabled(false);
    ui->afficherMetOp->setEnabled(false);

    try {
        if (ui->liste3->count() == 0) {
            throw PreviSatException();
        }

        const int nsat = getListeItemChecked(ui->liste3);
        if (nsat == 0 && ui->liste3->count() > 0) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), WARNING);
        }

        ui->afficherEvt->setEnabled(false);

        // Ecart heure locale - UTC
        const bool ecart = (fabs(offsetUTC - Date::CalculOffsetUTC(dateCourante.ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateInitialeEvt->dateTime());
        const double offset2 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateFinaleEvt->dateTime());

        // Date et heure initiales
        const Date date1(ui->dateInitialeEvt->date().year(), ui->dateInitialeEvt->date().month(), ui->dateInitialeEvt->date().day(),
                         ui->dateInitialeEvt->time().hour(), ui->dateInitialeEvt->time().minute(), ui->dateInitialeEvt->time().second(), 0.);

        // Jour julien initial
        double jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(ui->dateFinaleEvt->date().year(), ui->dateFinaleEvt->date().month(), ui->dateFinaleEvt->date().day(),
                         ui->dateFinaleEvt->time().hour(), ui->dateFinaleEvt->time().minute(), ui->dateFinaleEvt->time().second(), 0.);

        // Jour julien final
        double jj2 = date2.jourJulien() - offset2;

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
            throw PreviSatException(tr("Aucun évènement sélectionné"), WARNING);

        // Prise en compte de la refraction atmospherique
        const bool refr = ui->refractionPourEclipses->isChecked();

        // Prise en compte des eclipses de Lune
        const bool acalcEclipseLune = ui->eclipsesLune->isChecked();

        // Prise en compte du systeme horaire
        const bool syst = ui->syst24h->isChecked();

        // Liste des numeros NORAD
        QStringList listeSat;
        for (int i=0; i<ui->liste3->count(); i++) {
            if (ui->liste3->item(i)->checkState() == Qt::Checked)
                listeSat.append(ui->liste3->item(i)->text().split("#").at(1));
        }

        // Nom du fichier resultat
        const QString chaine = tr("evenements") + "_%1_%2.txt";
        ficRes = dirTmp + QDir::separator() + chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

        QFile fi2(ficRes);
        if (fi2.exists()) {
            fi2.remove();
        }
        QDir di(dirTmp);
        if (!di.exists()) {
            di.mkpath(dirTmp);
        }

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsEvt->setEnabled(false);


        // Lancement des calculs
        conditions = Conditions(EVENEMENTS, apogee, noeuds, ombre, quadr, jourNuit, ecart, refr, acalcEclipseLune, syst, jj1, jj2, offset1,
                                nomfic, ficRes, unite, listeSat);

        threadCalculs = new ThreadCalculs(conditions);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_afficherEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QStringList result = threadCalculs->res();
    afficherResultats = new Afficher(conditions, threadCalculs->observateur(), result);
    afficherResultats->setWindowTitle(QString("%1 %2 - ").arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)) +
                                      tr("Évènements orbitaux"));
    afficherResultats->show(ficRes);
    result.clear();

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

void PreviSat::on_hauteurSatTransit_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSatTransit->count() - 1) {
        ui->valHauteurSatTransit->setText(settings.value("previsions/valHauteurSatTransit", 0).toString());
        ui->valHauteurSatTransit->setVisible(true);
        ui->valHauteurSatTransit->setCursorPosition(0);
        ui->valHauteurSatTransit->setFocus();
    } else {
        ui->valHauteurSatTransit->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_parametrageDefautTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_ongletsOutils_currentChanged(ui->ongletsOutils->indexOf(ui->transits_ISS));
    ui->hauteurSatTransit->setCurrentIndex(1);
    ui->valHauteurSatTransit->setVisible(false);
    ui->lieuxObservation4->setCurrentIndex(0);
    ui->ageMaxTLETransit->setValue(2.);
    ui->elongationMaxCorps->setValue(5.);
    if (!ui->calculsTransit->isEnabled() && ! ui->afficherTransit->isEnabled() && threadCalculs == NULL) {
        ui->calculsTransit->setEnabled(true);
    }

    /* Retour */
    return;
}

void PreviSat::on_majTleIss_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    dirDwn = dirTmp;

    /* Corps de la methode */
    // Mise a jour du fichier iss.3le
    messagesStatut->setText(tr("Mise à jour du TLE de l'ISS en cours..."));
    TelechargementFichier(ISS_TRAJECTOIRE_NASA, true);

    /* Retour */
    return;
}

void PreviSat::on_manoeuvresISS_itemDoubleClicked(QTableWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QStringList manoeuvre = tabManoeuvresISS.at(item->row()).split(" ", QString::SkipEmptyParts);
    const double deltaV = manoeuvre.at(5).toDouble();

    /* Corps de la methode */
    if (fabs(deltaV) > EPSDBL100) {

        if (afficherManoeuvre != NULL) {

            if (tableMan != NULL) {
                tableMan->close();
                tableMan->deleteLater();
                tableMan = NULL;
            }

            afficherManoeuvre->close();
            afficherManoeuvre->deleteLater();
            afficherManoeuvre = NULL;
        }

        // Entete
        tableMan = new QTableWidget;
        tableMan->insertRow(0);
        tableMan->setColumnCount(5);
        tableMan->setHorizontalHeaderLabels(QStringList() << tr("Date") << tr("Durée") << "ΔV" << "ΔV (M50)" << "ΔV (LVLH)");
        QFont fnt;
        fnt.setBold(true);
        tableMan->horizontalHeader()->setFont(fnt);
#if defined (Q_OS_LINUX)
        fnt.setPointSize(7);
        tableMan->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 7pt; }");
#else
        fnt.setPointSize(9);
#endif
        fnt.setBold(false);
        tableMan->setFont(fnt);
        tableMan->setSelectionMode(QTableWidget::NoSelection);
        tableMan->setCornerButtonEnabled(false);
        tableMan->verticalHeader()->setVisible(false);

        // Date
        const QString dateMan = Date(manoeuvre.at(0).toDouble(), 0.)
                .ToShortDateAMJ(FORMAT_MILLISEC, (ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H).trimmed();
        QTableWidgetItem *itm = new QTableWidgetItem(dateMan);
        itm->setTextAlignment(Qt::AlignCenter);
        itm->setFlags(item->flags() & ~Qt::ItemIsEditable);
        itm->setToolTip("UTC");
        tableMan->setItem(0, 0, itm);

        // Duree
        itm = new QTableWidgetItem(manoeuvre.at(6));
        itm->setTextAlignment(Qt::AlignCenter);
        itm->setToolTip(tr("s"));
        tableMan->setItem(0, 1, itm);

        // DeltaV total
        itm = new QTableWidgetItem(ui->manoeuvresISS->item(item->row(), 5)->text());
        itm->setTextAlignment(Qt::AlignCenter);
        itm->setToolTip((ui->unitesKm->isChecked()) ? tr("m/s") : tr("fps"));
        tableMan->setItem(0, 2, itm);

        // DeltaV (M50)
        const QString fmt("%1\n%2\n%3");
        itm = new QTableWidgetItem();
        QString dv = fmt.arg(manoeuvre.at(7)).arg(manoeuvre.at(8)).arg(manoeuvre.at(9));
        if (ui->unitesKm->isChecked()) {
            dv = fmt.arg(manoeuvre.at(7).toDouble() / PIED_PAR_METRE, 0, 'f', 2).arg(manoeuvre.at(8).toDouble() / PIED_PAR_METRE, 0, 'f', 2)
                    .arg(manoeuvre.at(9).toDouble() / PIED_PAR_METRE, 0, 'f', 2);
            itm->setToolTip(tr("m/s"));
        } else {
            itm->setToolTip(tr("fps"));
        }
        itm->setText(dv);
        itm->setTextAlignment(Qt::AlignCenter);
        tableMan->setItem(0, 3, itm);

        // DeltaV (LVLH)
        itm = new QTableWidgetItem();
        dv = fmt.arg(manoeuvre.at(10)).arg(manoeuvre.at(11)).arg(manoeuvre.at(12));
        if (ui->unitesKm->isChecked()) {
            dv = fmt.arg(manoeuvre.at(10).toDouble() / PIED_PAR_METRE, 0, 'f', 2).arg(manoeuvre.at(11).toDouble() / PIED_PAR_METRE, 0, 'f', 2)
                    .arg(manoeuvre.at(12).toDouble() / PIED_PAR_METRE, 0, 'f', 2);
            itm->setToolTip(tr("m/s"));
        } else {
            itm->setToolTip(tr("fps"));
        }
        itm->setText(dv);
        itm->setTextAlignment(Qt::AlignCenter);
        tableMan->setItem(0, 4, itm);

        tableMan->resizeRowsToContents();
        tableMan->resizeColumnsToContents();
        tableMan->viewport()->setFocusPolicy(Qt::NoFocus);
#if QT_VERSION >= 0x050000
        tableMan->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
        tableMan->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

        afficherManoeuvre = new QMainWindow;
        afficherManoeuvre->setStyleSheet("QHeaderView::section { background-color:rgb(235, 235, 235) }");
        afficherManoeuvre->setWindowTitle(tr("Détail de la manoeuvre"));
        afficherManoeuvre->setCentralWidget(tableMan);
#if defined (Q_OS_LINUX)
        int lrg = 5;
#else
        int lrg = 2;
#endif
        for(int i=0; i<tableMan->columnCount(); i++) {
            lrg += tableMan->columnWidth(i);
        }
        afficherManoeuvre->resize(lrg, tableMan->horizontalHeader()->height()+tableMan->rowHeight(0));

        afficherManoeuvre->setMinimumSize(afficherManoeuvre->size());
        afficherManoeuvre->setMaximumSize(afficherManoeuvre->size());
        afficherManoeuvre->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, afficherManoeuvre->size(), geometry()));
        afficherManoeuvre->show();
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL) {
        afficherResultats->deleteLater();
        afficherResultats = NULL;
    }
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherPrev->setEnabled(false);
    ui->afficherEvt->setEnabled(false);
    ui->afficherMetOp->setEnabled(false);

    try {

        const QString fichier3leIss = dirTle + QDir::separator() + "iss.3le";
        const QFileInfo fi(fichier3leIss);
        if (!fi.exists()) {
            throw PreviSatException(tr("Le fichier TLE n'existe pas"), WARNING);
        }

        ui->afficherTransit->setEnabled(false);

        // Ecart heure locale - UTC
        const bool ecart = (fabs(offsetUTC - Date::CalculOffsetUTC(dateCourante.ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateInitialeTransit->dateTime());
        const double offset2 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateFinaleTransit->dateTime());

        // Date et heure initiales
        const Date date1(ui->dateInitialeTransit->date().year(), ui->dateInitialeTransit->date().month(),
                         ui->dateInitialeTransit->date().day(), ui->dateInitialeTransit->time().hour(),
                         ui->dateInitialeTransit->time().minute(), ui->dateInitialeTransit->time().second(), 0.);

        // Jour julien initial
        double jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(ui->dateFinaleTransit->date().year(), ui->dateFinaleTransit->date().month(),
                         ui->dateFinaleTransit->date().day(), ui->dateFinaleTransit->time().hour(),
                         ui->dateFinaleTransit->time().minute(), ui->dateFinaleTransit->time().second(), 0.);

        // Jour julien final
        double jj2 = date2.jourJulien() - offset2;

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
                    abs(ui->valHauteurSatTransit->text().toInt()) : 5 * ui->hauteurSatTransit->currentIndex();

        // Elongation maximale
        const double elong = ui->elongationMaxCorps->value();

        // Selection des corps
        const bool calcSol = ui->soleilTransit->isChecked();
        const bool calcLune = ui->luneTransit->isChecked();

        // Prise en compte de la refraction atmospherique
        const bool refr = ui->refractionPourEclipses->isChecked();

        // Prise en compte des eclipses de Lune
        const bool acalcEclipseLune = ui->eclipsesLune->isChecked();

        // Prise en compte du systeme horaire
        const bool syst = ui->syst24h->isChecked();

        // Nom du fichier resultat
        const QString chaine = tr("transits") + "_%1_%2.txt";
        ficRes = dirTmp + QDir::separator() + chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

        QFile fi2(ficRes);
        if (fi2.exists()) {
            fi2.remove();
        }
        QDir di(dirTmp);
        if (!di.exists()) {
            di.mkpath(dirTmp);
        }

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");

        QVector<TLE> tabtle;

        // Verification du fichier TLE
        if (TLE::VerifieFichier(fi.absoluteFilePath(), false) == 0) {
            const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                   "Le fichier %1 n'est pas un TLE");
            throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
        }

        // Lecture du TLE
        TLE::LectureFichier3le(fi.absoluteFilePath(), tabtle);
        if (tabtle.at(0).norad() != NORAD_STATION_SPATIALE) {
            const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                   "Le fichier %1 ne contient pas le TLE de l'ISS");
            throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
        }

        // Age des TLE
        const double agePremierTLE = tabtle.first().epoque().jourJulienUTC();
        const double ageDernierTLE = tabtle.last().epoque().jourJulienUTC();
        double age1 = 0.;
        if (jj1 < agePremierTLE) age1 = agePremierTLE - jj1;
        if (jj1 > ageDernierTLE) age1 = jj1 - ageDernierTLE;
        double age2 = 0.;
        if (jj2 < agePremierTLE) age2 = agePremierTLE - jj2;
        if (jj2 > ageDernierTLE) age2 = jj2 - ageDernierTLE;

        if ((age1 > ageTLE + 0.05) || (age2 > ageTLE + 0.05)) {
            const QString msg = tr("L'âge du TLE de l'ISS (%1 jours) est supérieur à %2 jours");
            Message::Afficher(msg.arg(fabs(qMax(age1, age2)), 0, 'f', 1).arg(ageTLE, 0, 'f', 1), INFO);
        }

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsTransit->setEnabled(false);

        // Lancement des calculs
        conditions = Conditions(TRANSITS, calcLune, calcSol, ecart, refr, acalcEclipseLune, syst, haut, ageTLE, elong, jj1, jj2, offset1,
                                fi.absoluteFilePath(), ficRes, unite);
        const Observateur obser(observateurs.at(ui->lieuxObservation4->currentIndex()));

        threadCalculs = new ThreadCalculs(conditions, obser);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_afficherTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QStringList result = threadCalculs->res();
    afficherResultats = new Afficher(conditions, threadCalculs->observateur(), result);
    afficherResultats->setWindowTitle(QString("%1 %2 - ").arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)) +
                                      tr("Transits ISS"));
    afficherResultats->show(ficRes);
    result.clear();

    /* Retour */
    return;
}


/*
 * Calcul des flashs MetOp
 */
void PreviSat::on_effacerHeuresMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->dateInitialeMetOp->setTime(QTime(0, 0, 0));
    ui->dateFinaleMetOp->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void PreviSat::on_fichierTLEMetOp_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->fichierTLEMetOp->setToolTip("");

    /* Corps de la methode */
    try {
        if (ui->fichierTLEMetOp->itemText(ui->fichierTLEMetOp->count() - 1) == tr("Parcourir...")) {

            if (index == ui->fichierTLEMetOp->count() - 1) {

                const QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                                     settings.value("fichier/fichierTLEMetOp", dirTle).toString(),
                                                                     tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));

                if (fichier.isEmpty()) {
                    if (!ui->fichierTLEMetOp->currentText().isEmpty())
                        ui->fichierTLEMetOp->setCurrentIndex(idxfi);
                } else {
                    AffichageListeFichiersTLE(fichier, ui->fichierTLEMetOp, ficTLEMetOp);
                    const int idx = qMax(ficTLEMetOp.size() - 1, 0);
                    ui->fichierTLEMetOp->setItemData(idxfm, QColor(Qt::white), Qt::BackgroundRole);
                    ui->fichierTLEMetOp->setItemData(idx, QColor(Qt::gray), Qt::BackgroundRole);
                    idxfm = idx;
                }
            } else {
                if (!ficTLEMetOp.isEmpty()) {
                    const int idx = qMax(index - 1, 0);
                    ui->fichierTLEMetOp->setItemData(idxfm, QColor(Qt::white), Qt::BackgroundRole);
                    ui->fichierTLEMetOp->setItemData(idx, QColor(Qt::gray), Qt::BackgroundRole);
                    AffichageListeFichiersTLE(ficTLEMetOp.at(index), ui->fichierTLEMetOp, ficTLEMetOp);
                    idxfm = idx;
                }
            }
            if (!ficTLEMetOp.isEmpty()) {
                const QFileInfo fi(ficTLEMetOp.at(idxfm));
                ui->fichierTLEMetOp->setCurrentIndex(idxfm);
                ui->fichierTLEMetOp->setToolTip((QDir::toNativeSeparators(fi.absolutePath()) == dirTle) ? "" : fi.absoluteFilePath());
            }
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSatMetOp_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSatMetOp->count() - 1) {
        ui->valHauteurSatMetOp->setText(settings.value("previsions/valHauteurSatMetOp", 0).toString());
        ui->valHauteurSatMetOp->setVisible(true);
        ui->valHauteurSatMetOp->setCursorPosition(0);
        ui->valHauteurSatMetOp->setFocus();
    } else {
        ui->valHauteurSatMetOp->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_hauteurSoleilMetOp_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == ui->hauteurSoleilMetOp->count() - 1) {
        ui->valHauteurSoleilMetOp->setText(settings.value("previsions/valHauteurSoleilMetOp", 0).toString());
        ui->valHauteurSoleilMetOp->setVisible(true);
        ui->valHauteurSoleilMetOp->setCursorPosition(0);
        ui->valHauteurSoleilMetOp->setFocus();
    } else {
        ui->valHauteurSoleilMetOp->setVisible(false);
    }

    /* Retour */
    return;
}

void PreviSat::on_parametrageDefautMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_ongletsOutils_currentChanged(ui->onglets->indexOf(ui->flashs));
    ui->hauteurSatMetOp->setCurrentIndex(2);
    ui->hauteurSoleilMetOp->setCurrentIndex(1);
    ui->valHauteurSatMetOp->setVisible(false);
    ui->valHauteurSoleilMetOp->setVisible(false);
    ui->lieuxObservation5->setCurrentIndex(0);
    ui->ordreChronologiqueMetOp->setChecked(true);
    ui->magnitudeMaxMetOp->setValue(4.);
    ui->affichage3lignesMetOp->setChecked(true);
    if (!ui->calculsMetOp->isEnabled() && ! ui->afficherMetOp->isEnabled() && threadCalculs == NULL) {
        ui->calculsMetOp->setEnabled(true);
    }

    /* Retour */
    return;
}

void PreviSat::on_calculsMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (afficherResultats != NULL) {
        afficherResultats->deleteLater();
        afficherResultats = NULL;
    }
    messagesStatut->setText("");

    /* Corps de la methode */
    ui->afficherPrev->setEnabled(false);
    ui->afficherEvt->setEnabled(false);
    ui->afficherTransit->setEnabled(false);

    try {
        if (ui->fichierTLEMetOp->currentText().trimmed().isEmpty() || ui->fichierTLEMetOp->currentText() == tr("Parcourir...")) {
            throw PreviSatException(tr("Le nom du fichier TLE n'est pas spécifié"), WARNING);
        }

        const QFileInfo fi(ficTLEMetOp.at(ui->fichierTLEMetOp->currentIndex()));
        if (!fi.exists()) {
            ui->fichierTLEMetOp->removeItem(ui->fichierTLEMetOp->currentIndex());
            throw PreviSatException(tr("Le nom du fichier TLE est incorrect"), WARNING);
        }
        ui->afficherMetOp->setEnabled(false);

        // Ecart heure locale - UTC
        const bool ecart = (fabs(offsetUTC - Date::CalculOffsetUTC(dateCourante.ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateInitialeMetOp->dateTime());
        const double offset2 = (ecart) ? offsetUTC : Date::CalculOffsetUTC(ui->dateFinaleMetOp->dateTime());

        // Date et heure initiales
        const Date date1(ui->dateInitialeMetOp->date().year(), ui->dateInitialeMetOp->date().month(),
                         ui->dateInitialeMetOp->date().day(), ui->dateInitialeMetOp->time().hour(),
                         ui->dateInitialeMetOp->time().minute(), ui->dateInitialeMetOp->time().second(), 0.);

        // Jour julien initial
        double jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(ui->dateFinaleMetOp->date().year(), ui->dateFinaleMetOp->date().month(),
                         ui->dateFinaleMetOp->date().day(), ui->dateFinaleMetOp->time().hour(),
                         ui->dateFinaleMetOp->time().minute(), ui->dateFinaleMetOp->time().second(), 0.);

        // Jour julien final
        double jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (jj1 > jj2) {
            const double tmp = jj2;
            jj2 = jj1;
            jj1 = tmp;
        }

        // Hauteur minimale du satellite
        const int haut = (ui->hauteurSatMetOp->currentIndex() == 5) ?
                    abs(ui->valHauteurSatMetOp->text().toInt()) : 5 * ui->hauteurSatMetOp->currentIndex();

        // Hauteur maximale du Soleil
        int crep = 0;
        if (ui->hauteurSoleilMetOp->currentIndex() <= 3) {
            crep = -6 * ui->hauteurSoleilMetOp->currentIndex();
        } else if (ui->hauteurSoleilMetOp->currentIndex() == 4) {
            crep = 90;
        } else if (ui->hauteurSoleilMetOp->currentIndex() == 5) {
            crep = ui->valHauteurSoleilMetOp->text().toInt();
        } else {
        }

        // Choix du tri par ordre chronologique
        const bool chr = (ui->ordreChronologiqueMetOp->isChecked());

        // Choix du nombre de lignes par flash
        const int nbl = (ui->affichage3lignesMetOp->isChecked()) ? 3 : 1;

        // Magnitude maximale (nuit)
        const double mgn1 = ui->magnitudeMaxMetOp->value();

        // Prise en compte de l'extinction atmospherique
        const bool ext = ui->extinctionAtmospherique->isChecked();

        // Prise en compte de la refraction atmospherique
        const bool refr = ui->refractionPourEclipses->isChecked();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        const bool effetEclPartielle = ui->effetEclipsesMagnitude->isChecked();

        // Prise en compte des eclipses de Lune
        const bool acalcEclipseLune = ui->eclipsesLune->isChecked();

        // Prise en compte du systeme horaire
        const bool syst = ui->syst24h->isChecked();

        // Nom du fichier resultat
        const QString chaine = tr("flashs") + "_%1_%2.txt";
        ficRes = dirTmp + QDir::separator() + chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

        QFile fi2(ficRes);
        if (fi2.exists()) {
            fi2.remove();
        }
        QDir di(dirTmp);
        if (!di.exists()) {
            di.mkpath(dirTmp);
        }

        // Unite pour les distances
        const QString unite = (ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");

        // Lecture du fichier de statut des satellites MetOp
        QStringList tabStsMetOp;
        tabStsMetOp.clear();
        MetOp::LectureStatutMetOp(tabStsMetOp);
        if (tabStsMetOp.isEmpty()) {
            throw PreviSatException(tr("Erreur rencontrée lors de l'exécution\n" \
                                       "Aucun satellite MetOp ou SkyMed susceptible de produire des flashs dans le fichier de statut"), WARNING);
        }

        // Creation de la liste de satellites
        QStringList listeSatellites;
        QStringListIterator it1(tabStsMetOp);
        while (it1.hasNext()) {
            const QString item = it1.next();
            listeSatellites.append(item.split(" ", QString::SkipEmptyParts).at(1));
        }

        // Verification du fichier TLE
        if (TLE::VerifieFichier(fi.absoluteFilePath(), false) == 0) {
            const QString msg = tr("Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE");
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
            if (tle.norad().isEmpty()) {
                tabStsMetOp.removeAt(i);
            } else {
                listeSatellites.append(tle.norad());
                tabtle2.append(tle);
                i++;
            }
        }

        // Il n'y a aucun satellite produisant des flashs dans le fichier TLE
        if (listeSatellites.size() == 0) {
            throw PreviSatException(tr("Erreur rencontrée lors de l'exécution\n" \
                                       "Aucun satellite produisant des flashs n'a été trouvé dans le fichier TLE"), WARNING);
        }

        messagesStatut->setText(tr("Calculs en cours. Veuillez patienter..."));
        ui->calculsMetOp->setEnabled(false);


        // Lancement des calculs
        conditions = Conditions(METOP, ecart, ext, refr, acalcEclipseLune, effetEclPartielle, syst, chr, crep, haut, nbl, jj1, jj2, offset1,
                                mgn1, fi.absoluteFilePath(), ficRes, unite, tabStsMetOp, tabtle2);
        Observateur obser(observateurs.at(ui->lieuxObservation5->currentIndex()));

        threadCalculs = new ThreadCalculs(conditions, obser);
        connect(threadCalculs, SIGNAL(finished()), this, SLOT(CalculsTermines()));
        threadCalculs->start();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void PreviSat::on_afficherMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QStringList result = threadCalculs->res();
    afficherResultats = new Afficher(conditions, threadCalculs->observateur(), result);
    afficherResultats->setWindowTitle(QString("%1 %2 - ").arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)) +
                                      tr("Prévisions des flashs"));
    afficherResultats->show(ficRes);
    result.clear();

    /* Retour */
    return;
}
