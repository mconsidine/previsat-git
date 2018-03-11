/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2018  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    afficher.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre d'affichage des resultats
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2011
 *
 * Date de revision
 * >    11 mars 2018
 *
 */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wpacked"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDesktopWidget>
#include <QGraphicsSimpleTextItem>
#include "ui_afficher.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wpacked"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/etoile.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/maths/maths.h"
#include "afficher.h"

static QString dirLocalData;
static QString dirOut;
static QString dirTmp;
static QString map0;
static QString prev;
static QStringList res;
static QStringList tablonlat;
static QStringList tabres;
static QSettings settings("Astropedia", "previsat");
static Conditions cond;
static Observateur obs;

// Soleil, Lune, etoiles, planetes
static Soleil soleil;
static Lune lune;
static QList<Etoile> etoiles;
static QList<Constellation> constellations;
static QList<LigneConstellation> lignesCst;
static QList<Planete> planetes;
static const QList<QColor> couleurPlanetes(QList<QColor> () << Qt::gray << Qt::white << Qt::red << QColor("orange") << Qt::darkYellow <<
                                           Qt::green << Qt::blue);

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

QGraphicsScene *sceneSky;
QScrollArea *scrollAreaRes;

Afficher::Afficher(const Conditions &conditions, const Observateur &observateur, QStringList &result, QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::Afficher)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    ui->setupUi(this);

    const int xmax = QApplication::desktop()->availableGeometry().width();
    const int ymax = QApplication::desktop()->availableGeometry().height();
    int xAff = width();
    int yAff = height();

    if (x() < 0 || y() < 0)
        move(0, 0);

    if (!(xAff < xmax && yAff < ymax))
        resize(xAff, yAff);

    // Redimensionnement de la fenetre si necessaire
    if (xAff > xmax)
        xAff = xmax;
    if (yAff > ymax)
        yAff = ymax;

    if (xAff < width() || yAff < height()) {
        if (xmax < minimumWidth())
            setMinimumWidth(xmax);
        if (ymax < minimumHeight())
            setMinimumHeight(ymax);
        resize(xAff, yAff);
        scrollAreaRes = new QScrollArea(this);
        scrollAreaRes->setWidget(centralWidget());
        scrollAreaRes->setWidgetResizable(true);
        setCentralWidget(scrollAreaRes);
    } else {
        ui->horizontalLayoutFichier->setContentsMargins(0, 0, 0, 45);
        setMaximumSize(minimumSize());
    }

    QFont police;

#if defined (Q_OS_WIN)
    police.setFamily("MS Shell Dlg 2");
    police.setPointSize(8);

#elif defined (Q_OS_LINUX)
    police.setFamily("Sans Serif");
    police.setPointSize(7);

#elif defined (Q_OS_MAC)
    police.setFamily("Marion");
    police.setPointSize(11);
#else
#endif

    setFont(police);
    QStyle * const styleBouton = QApplication::style();
    ui->actionEnregistrer->setIcon(styleBouton->standardIcon(QStyle::SP_DialogSaveButton));

    if (settings.value("affichage/flagIntensiteVision", false).toBool()) {

        QPalette paletteWin, palList;
        const int red = settings.value("affichage/valIntensiteVision", 0).toInt();
        const QBrush alpha = QBrush(QColor::fromRgb(red, 0, 0, 255));
        const QColor coulList = QColor(red + 27, 0, 0);

        paletteWin.setBrush(this->backgroundRole(), alpha);
        palList.setColor(QPalette::Base, coulList);

        this->setPalette(paletteWin);
        ui->fichier->setPalette(palList);
        ui->listePrevisions->setPalette(palList);
    }

    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();

#if QT_VERSION >= 0x050000
    dirLocalData = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory).at(0) +
            dirAstr + QDir::separator() + "data";
    dirOut = settings.value("fichier/sauvegarde",
                            QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory) +
                            QDir::separator() + dirAstr).toString();
    dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);
#else
    dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                            QDir::separator() + dirAstr).toString();
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif

#if defined (Q_OS_MAC)
    dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#endif

    if (dirTmp.trimmed().isEmpty())
        dirTmp = dirLocalData.mid(0, dirLocalData.lastIndexOf(QDir::separator())) + QDir::separator() + "cache";

#if defined (Q_OS_LINUX) || defined (Q_OS_MAC)

#if QT_VERSION >= 0x050000
    dirOut = settings.value("fichier/sauvegarde",
                            QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory) +
                            QDir::separator() + dirAstr).toString();
#else
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
                            QDir::separator() + QCoreApplication::applicationName()).toString();
#endif

#endif

    dirOut = QDir::toNativeSeparators(dirOut);

    const int ind = (result.size() > 1000 || result.isEmpty()) ? 0 : 1;

    if (ind == 1) {
        res = result;
    } else {
        ui->ongletsResultats->removeTab(1);
    }
    result.clear();
    cond = conditions;
    obs = observateur;

    police.setWeight(QFont::Bold);
    ui->listePrevisions->horizontalHeader()->setFont(police);

    switch (cond.typeCalcul()) {
    case PREVISION:
        ui->ongletsResultats->setTabText(ind, tr("Prévisions de passage"));
        break;

    case IRIDIUM:
        ui->ongletsResultats->setTabText(ind, tr("Flashs Iridium"));
        break;

    case EVENEMENTS:
        ui->ongletsResultats->removeTab(1);
        ui->ongletsResultats->setTabText(0, tr("Évènements orbitaux"));
        break;

    case TRANSITS:
        ui->ongletsResultats->setTabText(ind, tr("Transits ISS"));
        break;

    case METOP:
        ui->ongletsResultats->setTabText(ind, tr("Flashs MetOp et SkyMed"));
        break;

    default:
        break;
    }

    if (ind == 1) {
        Constellation::initCst = false;
        Etoile::initStar = false;
        LigneConstellation::initLig = false;
        load();
    }

    QResizeEvent *evt = NULL;
    resizeEvent(evt);

    /* Retour */
    return;
}

void Afficher::load()
{
    /* Declarations des variables locales */
    QString nomsat;

    /* Initialisations */
    int idate = 0;
    int imagn = 0;
    int iht = 0;
    int ihtsol = 0;
    int j = 0;

    QString fmt = "%1@%2@%3@%4";
    tablonlat.clear();
    tabres.clear();
    if (cond.nbl() < 0) {
        ui->listePrevisions->horizontalHeaderItem(3)->setText(tr("Angle"));
        ui->listePrevisions->horizontalHeaderItem(4)->setText(tr("Type"));
        ui->listePrevisions->horizontalHeaderItem(5)->setText(tr("Corps"));
    }

    if (cond.nbl() == 0) {
        ui->listePrevisions->removeColumn(5);
        ui->listePrevisions->setColumnWidth(0, 120);
    } else {
        ui->listePrevisions->setColumnWidth(0, 100);
    }


    /* Corps de la methode */
    QStringListIterator it(res);
    while (it.hasNext()) {

        QString ligne = it.next();

        // Nom du satellite et debut du passage
        if (ligne.length() < 60) {
            nomsat = ligne;
            if (it.hasNext())
                ligne = it.next();
        }

        if (ligne.contains(tr("Date"))) {
            idate = (cond.nbl() == 0) ? ligne.indexOf(tr("Date")) - 3 : 0;
            iht = ligne.indexOf(tr("Hauteur Sat")) - 1;
            imagn = ligne.indexOf(tr("Magn")) - 1;
            ihtsol = ligne.indexOf(tr("Haut Soleil")) - 1;
            if (it.hasNext())
                ligne = it.next();
        }

        if (cond.typeCalcul() == IRIDIUM) {
            const QString fmt2 = "  %1 %2  ";
            nomsat = fmt2.arg(tr("Iridium")).arg(ligne.mid(166, 4).trimmed());
        }

        if (cond.typeCalcul() == METOP) {
            nomsat = ligne.mid(166, 10).trimmed();
        }

        if (cond.typeCalcul() == TRANSITS)
            nomsat = "ISS";

        if (!ligne.contains(tr("Date"))) {

            QString debut = ligne;
            QString debut1 = debut;
            if (cond.typeCalcul() == TRANSITS)
                debut1 = it.next();

            // Maximums et fin du passage
            QString maxMag = debut;
            QString maxHt = debut;
            QString fin = debut;
            QString fin1 = debut;

            bool afin = false;
            while (!afin) {
                ligne = it.next();
                if (ligne.isEmpty()) {
                    afin = true;
                } else {
                    if (cond.nbl() >= 0) {

                        // Magnitude max
                        if (ligne.mid(imagn+1, 4) != "----")
                            if (ligne.mid(imagn, 5).toDouble() <= maxMag.mid(imagn, 5).toDouble() || maxMag.mid(imagn+1, 4) == "----")
                                maxMag = ligne;

                        // Hauteur max
                        if (QString::compare(ligne.mid(iht, 9), maxHt.mid(iht, 9)) > 0)
                            maxHt = ligne;
                    } else {
                        // Angle minimum
                        if (ligne.mid(71, 5).toDouble() <= maxHt.mid(71, 5).toDouble())
                            maxHt = ligne;
                    }
                    fin1 = fin;
                    fin = ligne;
                }
            }

            if (cond.typeCalcul() == IRIDIUM)
                fin1 = fin;

            // Ajout d'une ligne dans le tableau des resultats
            ui->listePrevisions->insertRow(j);
            ui->listePrevisions->setRowHeight(j, 16);

            const int lngDate = (cond.nbl() == 0) ? 21 : 22;
            QStringList items(QStringList () << nomsat << debut.mid(idate, lngDate).trimmed() << fin.mid(idate, lngDate).trimmed() <<
                              ((cond.nbl() >= 0) ? maxHt.mid(iht, 11).trimmed() : maxHt.mid(71, 5)) <<
                              ((cond.nbl() >= 0) ? maxMag.mid(imagn, 6).trimmed() : debut.mid(79, 1)) <<
                              maxHt.mid(ihtsol, maxHt.lastIndexOf("\"") - ihtsol + 1).trimmed());

            if (cond.nbl() != 0)
                items.insert(5, (cond.nbl() > 0) ? maxMag.mid(imagn - 2, 1) : maxHt.mid(84, 1));

            for(int k=0; k<items.count(); k++) {
                QTableWidgetItem * const item = new QTableWidgetItem(items.at(k));
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                if (k == 0)
                    item->setToolTip(items.at(0));

                ui->listePrevisions->setItem(j, k, item);
                if (k > 0)
                    ui->listePrevisions->resizeColumnToContents(k);
            }
            tabres.append(fmt.arg((cond.nbl() >= 0) ? debut.right(5) : NORAD_STATION_SPATIALE).arg(debut.mid(idate, lngDate))
                          .arg(maxMag.mid(idate, lngDate)).arg(fin.mid(idate, lngDate)));

            // Dans le cas des flashs ou des transits ISS, determination de la ligne ou se produit le maximum
            if (cond.nbl() != 0) {

                const int debt = (cond.nbl() > 0) ? 122 : 128;
                if (debut1.mid(debt, 44).trimmed().isEmpty() && fin1.mid(debt, 44).trimmed().isEmpty()) {
                    tablonlat.append("0. 0. 0. 0.");
                } else {

                    bool tst = debut1.mid(debt, 44).trimmed().isEmpty();
                    const QString deb = (tst) ? maxMag : debut1;
                    tst = fin1.mid(debt, 44).trimmed().isEmpty();
                    const QString end = (tst) ? maxMag : fin1;

                    if (deb == end) {
                        tablonlat.append("0. 0. 0. 0.");
                    } else {

                        // Longitudes et latitudes ou passe le maximum
                        const QString lon1 = ((deb.mid(debt + 12, 1) == tr("W")) ? "-" : "+") +
                                QString::number(deb.mid(debt + 3, 8).toDouble());
                        const QString lat1 = ((deb.mid(debt + 23, 1) == tr("S")) ? "-" : "+") +
                                QString::number(deb.mid(debt + 15, 7).toDouble());
                        const QString lon2 = ((end.mid(debt + 12, 1) == tr("W")) ? "-" : "+") +
                                QString::number(end.mid(debt + 3, 8).toDouble());
                        const QString lat2 = ((end.mid(debt + 23, 1) == tr("S")) ? "-" : "+") +
                                QString::number(end.mid(debt + 15, 7).toDouble());
                        const QString fmtll = "%1 %2 %3 %4";
                        tablonlat.append(fmtll.arg(lon1).arg(lat1).arg(lon2).arg(lat2));
                    }
                }
            }
            j++;
        }
    }
    if (tabres.isEmpty())
        ui->ongletsResultats->removeTab(1);
    ui->listePrevisions->horizontalHeader()->setStretchLastSection(true);
    ui->listePrevisions->setAlternatingRowColors(true);

    if (cond.nbl() == 0) {
        // Masquage de la map
        ui->frame->setVisible(false);
        ui->listePrevisions->resize(ui->listePrevisions->width(), ui->ongletsResultats->height() - 4);
    } else {

        // Affichage de la map
        const QString fic = dirLocalData + QDir::separator() + "html" + QDir::separator() + "resultat.map";
        QFile fi(fic);

        if (fi.exists() && fi.size() != 0) {
            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            map0 = flux.readAll();
        }
        fi.close();

        const QString lon(QString::number(-obs.longitude() * RAD2DEG));
        const QString lat(QString::number(obs.latitude() * RAD2DEG));
        const QString unite((cond.unite() == tr("km")) ? tr("m") : tr("ft"));
        const QString alt(QString::number((int) (obs.altitude() * 1000. * ((unite == tr("m")) ? 1. : PIED_PAR_METRE) + EPSDBL100)) +
                          " " + unite);
        map0 = map0.replace("NOMLIEU_CENTRE", obs.nomlieu().toUtf8()).replace("LONGITUDE_CENTRE", lon).replace("LATITUDE_CENTRE", lat).
                replace("ALTITUDE_CENTRE", alt).replace("CHAINE_LONGITUDE", tr("Longitude")).
                replace("CHAINE_LATITUDE", tr("Latitude")).replace("CHAINE_ALTITUDE", tr("Altitude"));

        ui->frame->setVisible(true);
        ui->webView->setContextMenuPolicy(Qt::CustomContextMenu);
    }

    ui->ongletsResultats->setCurrentIndex(settings.value("affichage/ongletPrevisions", 0).toInt());
    ui->listePrevisions->horizontalHeader()->setHighlightSections(false);
    ui->listePrevisions->selectRow(0);
    ui->listePrevisions->setFocus();
    res.clear();

    /* Retour */
    return;
}

Afficher::~Afficher()
{
    delete ui;
}

void Afficher::show(const QString &fic)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _fichier = fic;

    /* Corps de la methode */
    QFile fi(_fichier);
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    prev = fi.trUtf8(fi.readAll());
    ui->fichier->setText(prev);
    setVisible(true);
    prev = "";
    fi.close();

    /* Retour */
    return;
}

void Afficher::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    settings.setValue("affichage/ongletPrevisions", ui->ongletsResultats->currentIndex());
    ui->fichier->clear();
    if (sceneSky != NULL) {
        sceneSky->deleteLater();
        sceneSky = NULL;
    }

    if (scrollAreaRes != NULL) {
        scrollAreaRes->deleteLater();
        scrollAreaRes = NULL;
    }
}

void Afficher::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt)
    if (baseSize() == size()) {
        ui->ongletsResultats->resize(width(), height() - ui->barreOutils->height());
        if (cond.nbl() == 0)
            ui->listePrevisions->resize(ui->listePrevisions->width(), ui->ongletsResultats->height() - 30);
    }
}

void Afficher::on_actionEnregistrer_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_WIN)
    const QString nomRepDefaut = dirOut.replace(QDir::separator(), "\\");
#else
    const QString nomRepDefaut = dirOut;
#endif

    /* Corps de la methode */
    if (ui->fichier_txt->isVisible()) {

        const QString nomFicDefaut = _fichier.split(QDir::separator()).last();
        const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), nomRepDefaut + QDir::separator() +
                                                             nomFicDefaut, tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
        if (!fichier.isEmpty()) {
            QFile fi(fichier);
            if (fi.exists())
                fi.remove();

            QFile fi2(_fichier);
            fi2.copy(fi.fileName());
            QFileInfo fi3(fichier);
            settings.setValue("fichier/sauvegarde", fi3.absolutePath());
        }
    } else {

        const QPixmap image = QPixmap::grabWidget(ui->ongletsResultats);
        const QString nomFicDefaut = nomRepDefaut + QDir::separator() + _fichier.split(QDir::separator()).last().remove(".txt");

        const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), nomFicDefaut,
                                                         tr("Fichiers PNG (*.png);;Fichiers JPEG (*.jpg);;Fichiers BMP (*.bmp);;" \
                                                            "Tous les fichiers (*)"));
        if (!fic.isEmpty()) {
            image.save(fic);
            const QFileInfo fi(fic);
            settings.setValue("fichier/sauvegarde", fi.absolutePath());
        }
    }

    /* Retour */
    return;
}

void Afficher::loadMap(const int i)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString map = map0;
    const QStringList tab = tablonlat.at(i).split(" ");

    /* Corps de la methode */
    map.replace("LONGITUDE1", tab.at(0)).replace("LATITUDE1", tab.at(1))
            .replace("LONGITUDE2", tab.at(2)).replace("LATITUDE2", tab.at(3));

    QFile fi(dirTmp + QDir::separator() + "resultat.html");
    fi.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fi);
    flux << map;
    fi.close();

    // Chargement de la map
    const QUrl url("file:///" + fi.fileName());
    ui->webView->load(url);

    /* Retour */
    return;
}

void Afficher::loadSky(const int j)
{
    /* Declarations des variables locales */
    int indLune;

    /* Initialisations */
    const Qt::CheckState affconst = static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt());
    const Qt::CheckState affplanetes = static_cast<Qt::CheckState> (settings.value("affichage/affplanetes", Qt::Checked).toUInt());
    const bool affetoiles = settings.value("affichage/affetoiles", true).toBool();
    const bool afflune = settings.value("affichage/afflune", true).toBool();
    const bool affphaselune = settings.value("affichage/affphaselune", true).toBool();
    const bool affsoleil = settings.value("affichage/affsoleil", true).toBool();
    const bool refraction = settings.value("affichage/refractionPourEclipses", true).toBool();
    const bool eclipsesLune = settings.value("affichage/eclipsesLune", true).toBool();
    const bool rotationLune = settings.value("affichage/rotationLune", true).toBool();
    const double magnitudeEtoiles = settings.value("affichage/magnitudeEtoiles", 4.0).toDouble();
    sceneSky = new QGraphicsScene;

    const QStringList tab = tabres.at(j).split("@");

    const QStringList listeTLEs(tab.at(0));
    QVector<TLE> tabtle;
    TLE::LectureFichier(cond.fic(), listeTLEs, tabtle);
    Satellite sat(tabtle.at(0));

    // Date de debut du passage
    QString date = tab.at(1).trimmed();
    QStringList deb = date.replace("/", " ").replace(":", " ").split(" ");

    int heure = deb.at(3).toInt();
    if (date.right(1).contains(QRegExp("[a-z]"))) {
        if (date.right(1) == "p") {
            heure += 12;
        } else {
            if (heure == 12)
                heure = 0;
        }
    }
    const Date dateI(deb.at(0).toInt(), deb.at(1).toInt(), deb.at(2).toInt(), heure, deb.at(4).toInt(),
                     deb.at(5).left(2).toDouble(), 0.);
    double offset = (cond.ecart()) ? cond.offset() : Date::CalculOffsetUTC(Date(dateI.jourJulienUTC(), 0.).ToQDateTime(1));
    Date dateDeb(dateI.jourJulienUTC() - offset, offset);

    Date dateInit(dateDeb, offset);
    Date dateMax, dateFin;

    if (!sat.isGeo()) {
        bool atrouve = false;
        while (!atrouve) {
            obs.CalculPosVit(dateInit);

            sat.CalculPosVit(dateInit);
            sat.CalculCoordHoriz(obs, false);
            if (sat.hauteur() < 0.) {
                atrouve = true;
            } else {
                dateInit = Date(dateInit.jourJulienUTC() - NB_JOUR_PAR_SEC * 10., offset);
            }
        }
        dateInit = Date(dateInit.jourJulienUTC() + NB_JOUR_PAR_SEC * 10., offset);

        if (cond.nbl() == 3) {

            date = tab.at(2);
            QStringList max = date.replace("/", " ").replace(":", " ").split(" ");
            const Date dateM(max.at(0).toInt(), max.at(1).toInt(), max.at(2).toInt(), max.at(3).toInt(), max.at(4).toInt(),
                             max.at(5).toDouble(), 0.);
            offset = (cond.ecart()) ? cond.offset() : Date::CalculOffsetUTC(Date(dateM.jourJulienUTC(), 0.).ToQDateTime(1));
            dateMax = Date(dateM.jourJulienUTC() - offset, offset);

            date = tab.at(3);
            QStringList fin = date.replace("/", " ").replace(":", " ").split(" ");
            const Date dateF(fin.at(0).toInt(), fin.at(1).toInt(), fin.at(2).toInt(), fin.at(3).toInt(), fin.at(4).toInt(),
                             fin.at(5).toDouble(), 0.);
            offset = (cond.ecart()) ? cond.offset() :
                                      Date::CalculOffsetUTC(Date(dateF.jourJulienUTC(), 0.).ToQDateTime(1));
            dateFin = Date(dateF.jourJulienUTC() - offset, offset);
        }
    }

    /* Corps de la methode */
    // Position de l'observateur a la date de debut
    obs.CalculPosVit(dateInit);

    // Position du Soleil a la date de debut
    soleil.CalculPosition(dateInit);
    soleil.CalculCoordHoriz(obs);

    // Position de la Lune
    lune.CalculPosition(dateInit);
    lune.CalculPhase(soleil);
    lune.CalculCoordHoriz(obs);

    // Position des planetes
    if (affplanetes != Qt::Unchecked) {
        planetes.clear();
        for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

            Planete planete(iplanete);
            planete.CalculPosition(dateInit, soleil);
            planete.CalculCoordHoriz(obs);
            planetes.append(planete);
        }
    }

    // Position des etoiles
    Etoile::CalculPositionEtoiles(obs, etoiles);
    if (affconst == Qt::Checked)
        Constellation::CalculConstellations(obs, constellations);
    if (affconst != Qt::Unchecked)
        LigneConstellation::CalculLignesCst(etoiles, lignesCst);

    // Position du satellite
    sat.CalculTraceCiel(dateInit, eclipsesLune, refraction, obs, 1);

    // Preparations pour l'affichage de la carte du ciel
    // Phase de la Lune
    if (affphaselune) {

        const double ll = atan2(lune.position().y(), lune.position().x());
        const double ls = atan2(soleil.position().y(), soleil.position().x());

        double diff = (ll - ls) * RAD2DEG;
        if (diff < 0.)
            diff += T360;
        indLune = (int) (diff  * (1. / 12.190749)) + 1;
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

    const QColor bleuClair(173, 216, 230);
    const QColor crimson(220, 20, 60);
    QPen crayon(Qt::white);

    // Affichage de la carte du ciel
    QRect rectangle(2, 2, ui->ciel->width() - 4, ui->ciel->height() - 4);
    sceneSky->setSceneRect(rectangle);

    sceneSky->setBackgroundBrush(QBrush(palette().background().color()));
    const QPen pen(bru, Qt::SolidPattern);
    sceneSky->addEllipse(rectangle, pen, bru);
    const int lciel = qRound(0.5 * ui->ciel->width());
    const int hciel = qRound(0.5 * ui->ciel->height());

    // Affichage des constellations
    if (affconst != Qt::Unchecked) {

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

                if ((lstr2 - lstr1) * (lstr2 - lstr1) + (bstr2 - bstr1) * (bstr2 - bstr1) < lciel * ui->ciel->height())
                    sceneSky->addLine(lstr1, bstr1, lstr2, bstr2, crayon);
            }
        }

        // Affichage du nom des constellations
        if (affconst == Qt::Checked) {

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
                    sceneSky->addItem(txtCst);
                }
            }
        }
    }

    // Affichage des etoiles
    const QBrush bru2 = (soleil.hauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);
    QListIterator<Etoile> it1(etoiles);
    while (it1.hasNext()) {

        const Etoile etoile = it1.next();
        if (etoile.isVisible() && etoile.magnitude() <= magnitudeEtoiles) {

            const int lstr = qRound(lciel - lciel * (1. - etoile.hauteur() * DEUX_SUR_PI) * sin(etoile.azimut()));
            const int bstr = qRound(hciel - hciel * (1. - etoile.hauteur() * DEUX_SUR_PI) * cos(etoile.azimut()));

            rectangle = (etoile.magnitude() > 3.) ? QRect(lstr-1, bstr-1, 2, 2) : QRect(lstr-1, bstr-1, 2, 3);
            sceneSky->addEllipse(rectangle, QPen(Qt::NoPen), bru2);

            // Nom des etoiles les plus brillantes
            if (affetoiles) {
                if (!etoile.nom().isEmpty() && etoile.nom().at(0).isUpper()) {
                    if (etoile.magnitude() < magnitudeEtoiles - 1.9) {

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
                        sceneSky->addItem(txtStr);
                    }
                }
            }
        }
    }

    if (affplanetes != Qt::Unchecked) {

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
                    sceneSky->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), bru3);

                    if (affplanetes == Qt::Checked) {
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
                        sceneSky->addItem(txtPla);
                    }
                }
            }
        }
    }

    if (affsoleil) {

        // Dessin de l'ecliptique
        const double ad1 = tabEcliptique[0][0] * HEUR2RAD;
        const double de1 = tabEcliptique[0][1] * DEG2RAD;
        const double cd1 = cos(de1);
        const Vecteur3D vec(cos(ad1) * cd1, sin(ad1) * cd1, sin(de1));
        const Vecteur3D vec1 = obs.rotHz() * vec;

        double ht1 = asin(vec1.z());
        double az1 = atan2(vec1.y(), -vec1.x());
        if (az1 < 0.)
            az1 += DEUX_PI;

        int lecl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
        int becl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

        for(int i=1; i<49; i++) {

            const double ad2 = tabEcliptique[i][0] * HEUR2RAD;
            const double de2 = tabEcliptique[i][1] * DEG2RAD;
            const double cd2 = cos(de2);
            const Vecteur3D vec0(cos(ad2) * cd2, sin(ad2) * cd2, sin(de2));
            const Vecteur3D vec2 = obs.rotHz() * vec0;

            const double ht2 = asin(vec2.z());

            double az2 = atan2(vec2.y(), -vec2.x());
            if (az2 < 0.)
                az2 += DEUX_PI;

            const int lecl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
            const int becl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

            if (ht1 >= 0. || ht2 >= 0.)
                sceneSky->addLine(lecl1, becl1, lecl2, becl2, QPen(Qt::darkYellow));

            lecl1 = lecl2;
            becl1 = becl2;
            ht1 = ht2;
        }

        if (soleil.isVisible()) {

            // Calcul des coordonnees radar du Soleil
            const int lsol = qRound(lciel - lciel * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
            const int bsol = qRound(hciel - hciel * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

            rectangle = QRect(lsol - 7, bsol - 7, 15, 15);
            sceneSky->addEllipse(rectangle, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
        }
    }

    if (affplanetes != Qt::Unchecked) {

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
                    sceneSky->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), bru3);

                    if (affplanetes == Qt::Checked) {
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
                        sceneSky->addItem(txtPla);
                    }
                }
            }
        }
    }

    if (afflune && lune.isVisible()) {

        // Calcul des coordonnees radar de la Lune
        const int llun = qRound(lciel - lciel * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
        const int blun = qRound(hciel - hciel * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

        QGraphicsPixmapItem * const lun = sceneSky->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun, blun);
        if (rotationLune && obs.latitude() < 0.)
            transform.rotate(180.);
        transform.translate(-7, -7);
        lun->setTransform(transform);
    }

    // Affichage de la trace dans le ciel
    if (sat.traceCiel().size() > 0) {

        const QList<QVector<double> > trace = sat.traceCiel();
        const double ht1 = trace.at(0).at(0);
        const double az1 = trace.at(0).at(1);
        int lsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
        int bsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

        bool aecr = false;
        bool adeb = false;
        bool amax = false;
        int min = -1;
        int lsat0 = 10000;
        int bsat0 = 10000;
        int lsat3 = 0;
        int bsat3 = 0;

        for(int i=1; i<trace.size(); i++) {

            const double ht2 = trace.at(i).at(0);
            const double az2 = trace.at(i).at(1);

            crayon = (fabs(trace.at(i).at(2)) <= EPSDBL100) ?
                        ((soleil.hauteur() > -0.08) ?
                             bleuClair : ((soleil.hauteur() > -0.12) ? QColor("deepskyblue") : QColor("cyan"))) :
                        (fabs(trace.at(i).at(2) - 2.) <= EPSDBL100) ? Qt::green : crimson;

            const int lsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
            const int bsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

            const QLineF lig = QLineF(lsat2, bsat2, lsat1, bsat1);

            // Determination des dates a afficher sur la carte du ciel
            const Date dateTrace(trace.at(i).at(3), offset);
            const double norm = sqrt((lsat1 - lsat0) * (lsat1 - lsat0) + (bsat1 - bsat0) * (bsat1 - bsat0));

            if (dateTrace.minutes() != min && norm > 12.) {
                aecr = true;
                min = dateTrace.minutes();
                lsat3 = (i == 1) ? lsat1 : lsat0;
                bsat3 = (i == 1) ? bsat1 : bsat0;
                lsat0 = lsat1;
                bsat0 = bsat1;
            }

            if (cond.nbl() > 0) {

                if (dateTrace.jourJulienUTC() > dateMax.jourJulienUTC() && !adeb) {
                    adeb = true;
                    amax = true;
                    aecr = true;
                }
                if (dateTrace.jourJulienUTC() >= dateDeb.jourJulienUTC() &&
                        dateTrace.jourJulienUTC() <= dateFin.jourJulienUTC()) {
                    crayon = QPen(crayon.color(), 4);
                }
            }

            // Affichage de l'heure
            if (aecr) {

                aecr = false;

                // Dessin d'une petite ligne correspondant a la date
                QLineF lig2 = QLineF(lsat2, bsat2, lsat3, bsat3).normalVector();
                lig2.setLength(4);
                sceneSky->addLine(lig2, QPen(bru2, 1.));

                QString sdate = "";
                if (amax) {
                    amax = false;
                    if (cond.typeCalcul() == IRIDIUM)
                        sdate = tr("Flash Iridium");

                    if (cond.typeCalcul() == METOP) {
                        QString nomFlash = sat.tle().nom().section(QRegExp("[ -]"), 0, 0).toLower();
                        nomFlash[0] = nomFlash[0].toUpper();
                        nomFlash[3] = nomFlash[3].toUpper();
                        sdate = tr("Flash %1").arg(nomFlash);
                    }
                } else {
                    if (dateTrace.jourJulienUTC() < dateDeb.jourJulienUTC() ||
                            dateTrace.jourJulienUTC() > dateFin.jourJulienUTC()) {
                        const DateSysteme sys = (cond.syst()) ? SYSTEME_24H : SYSTEME_12H;
                        sdate = dateTrace.ToShortDate(FORMAT_COURT, sys);
                        sdate = (sys == SYSTEME_12H) ? sdate.mid(11, 5) + sdate.right(1) : sdate.mid(11, 5);
                    }
                }

                if (!sdate.isEmpty()) {
                    QGraphicsSimpleTextItem * const txtTrace = new QGraphicsSimpleTextItem(sdate);
                    txtTrace->setBrush(bru2);

                    const double ang = -lig2.angle();
                    const double ca = cos(ang * DEG2RAD);
                    const double sa = sin(ang * DEG2RAD);
                    const double xnc = lsat2 + 6. * ca + 11. * sa;
                    const double ync = bsat2 + 6. * sa - 11. * ca;

                    txtTrace->setPos(xnc, ync);
                    txtTrace->setRotation(ang);
                    sceneSky->addItem(txtTrace);
                }
            }

            sceneSky->addLine(lig, crayon);

            lsat1 = lsat2;
            bsat1 = bsat2;
        }
    } else {

        // Calcul des coordonnees radar du satellite
        sat.CalculPosVit(dateInit);
        sat.CalculCoordHoriz(obs);
        const int lsat = qRound(lciel - lciel * (1. - sat.hauteur() * DEUX_SUR_PI) * sin(sat.azimut()));
        const int bsat = qRound(hciel - hciel * (1. - sat.hauteur() * DEUX_SUR_PI) * cos(sat.azimut()));

        rectangle = QRect(lsat - 3, bsat - 3, 6, 6);
        const QPen noir(Qt::black);
        const QColor col = (sat.conditionEclipse().isEclipseTotale()) ?
                    crimson : (sat.conditionEclipse().isEclipsePartielle() || sat.conditionEclipse().isEclipseAnnulaire()) ? Qt::green : Qt::yellow;
        sceneSky->addEllipse(rectangle, noir, QBrush(col, Qt::SolidPattern));
    }

    sceneSky->addEllipse(-20, -20, ui->ciel->width() + 40, ui->ciel->height() + 40,
                         QPen(QBrush(palette().background().color()), 44));
    sceneSky->addEllipse(1, 1, ui->ciel->width() - 3, ui->ciel->height() - 3, QPen(QBrush(Qt::gray), 3));

    ui->ciel->setScene(sceneSky);
    QGraphicsView view(sceneSky);
    view.setRenderHints(QPainter::Antialiasing);

    /* Retour */
    return;
}

void Afficher::on_listePrevisions_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn)
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)
    if (cond.nbl() != 0)
        loadMap(currentRow);
    loadSky(currentRow);
}

void Afficher::on_ongletsResultats_currentChanged(int index)
{
    Q_UNUSED(index)
    ui->listePrevisions->setFocus();
}
