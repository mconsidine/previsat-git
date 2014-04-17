/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2014  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    Fenetre d'affichage des resultats ou du fichier d'aide
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2011
 *
 * Date de revision
 * >    17 avril 2014
 *
 */

#if defined QT_NO_DEBUG
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsSimpleTextItem>
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
#include "ui_afficher.h"
#include "afficher.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"

#include <QMessageBox>

static QString dirDat;
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
    }

#if defined (Q_OS_WIN)
    dirDat = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                            QDir::separator() + QCoreApplication::organizationName() + QDir::separator() +
                            QCoreApplication::applicationName()).toString();

#elif defined (Q_OS_LINUX)
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
                            QDir::separator() + QCoreApplication::applicationName()).toString();

#elif defined (Q_OS_MAC)
    dirDat = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
                            QDir::separator() + QCoreApplication::applicationName()).toString();

#else
    dirDat = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                            QDir::separator() + QCoreApplication::organizationName() + QDir::separator() +
                            QCoreApplication::applicationName()).toString();
#endif

    dirOut = QDir::convertSeparators(dirOut);
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);

    res = result;
    result.clear();
    cond = conditions;
    obs = observateur;

    police.setWeight(QFont::Bold);
    ui->listePrevisions->horizontalHeader()->setFont(police);
    if (cond.getNbl() == 0)
        ui->ongletsResultats->setTabText(1, tr("Prévisions de passage"));
    else
        ui->ongletsResultats->setTabText(1, tr("Flashs Iridium"));

    if (cond.getApassApogee() || cond.getApassNoeuds() || cond.getApassOmbre() || cond.getApassPso() || cond.getAtransJn()) {
        ui->ongletsResultats->removeTab(1);
        ui->ongletsResultats->setTabText(0, tr("Évènements orbitaux"));
    }

    if (cond.getAcalcLune() || cond.getAcalcSol())
        ui->ongletsResultats->setTabText(1, tr("Transits ISS"));

    Etoile::initStar = false;
    LigneConstellation::initLig = false;

    load();

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
    if (cond.getNbl() < 0) {
        ui->listePrevisions->horizontalHeaderItem(3)->setText(tr("Angle"));
        ui->listePrevisions->horizontalHeaderItem(4)->setText(tr("Type"));
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
            idate = (cond.getNbl() == 0) ? ligne.indexOf(tr("Date")) - 3 : 0;
            iht = ligne.indexOf(tr("Hauteur Sat")) + 1;
            imagn = ligne.indexOf(tr("Magn")) - 1;
            ihtsol = ligne.indexOf(tr("Haut Soleil"));
            if (it.hasNext())
                ligne = it.next();
        }

        if (cond.getNbl() > 0) {
            const QString fmt2 = "  %1 %2  ";
            nomsat = fmt2.arg(tr("Iridium")).arg(ligne.mid(165, 4).trimmed());
        }

        if (cond.getNbl() < 0)
            nomsat = "ISS";

        if (!ligne.contains(tr("Date"))) {

            QString debut = ligne;

            // Maximums et fin du passage
            QString maxMag = debut;
            QString maxHt = debut;
            QString fin = debut;

            bool afin = false;
            while (!afin) {
                ligne = it.next().toLatin1();
                if (ligne.isEmpty()) {
                    afin = true;
                } else {
                    if (cond.getNbl() >= 0) {

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
                    fin = ligne;
                }
            }

            // Ajout d'une ligne dans le tableau des resultats
            ui->listePrevisions->insertRow(j);
            ui->listePrevisions->setRowHeight(j, 16);

            const int lngDate = (cond.getNbl() == 0) ? 20 : 22;
            const QStringList items(QStringList () << nomsat << debut.mid(idate, lngDate) << fin.mid(idate, lngDate) <<
                                    ((cond.getNbl() >= 0) ? maxHt.mid(iht, 9) : maxHt.mid(71, 5)) <<
                                    ((cond.getNbl() >= 0) ? maxMag.mid(imagn, 5) : debut.mid(79, 1)) << maxHt.mid(ihtsol, 10));
            for(int k=0; k<items.count(); k++) {
                QTableWidgetItem * const item = new QTableWidgetItem(items.at(k));
                item->setTextAlignment(Qt::AlignCenter);
                ui->listePrevisions->setItem(j, k, item);
                if ((k > 0 && cond.getNbl() == 0) || (k >= 0 && cond.getNbl() != 0))
                    ui->listePrevisions->resizeColumnToContents(k);
            }
            tabres.append(fmt.arg((cond.getNbl() >= 0) ? debut.right(5) : "25544").arg(debut.mid(idate, lngDate))
                          .arg(maxMag.mid(idate, lngDate)).arg(fin.mid(idate, lngDate)));

            // Dans le cas des flashs Iridium ou des transits ISS, determination de la ligne ou se produit le maximum
            if (cond.getNbl() != 0) {

                const int debt = (cond.getNbl() > 0) ? 121 : 128;
                if (debut.mid(debt, 44).trimmed().isEmpty() && fin.mid(debt, 44).trimmed().isEmpty()) {
                    tablonlat.append("0. 0. 0. 0.");
                } else {

                    bool tst = debut.mid(debt, 44).trimmed().isEmpty();
                    const QString deb = (tst) ? maxMag : debut;
                    tst = fin.mid(debt, 44).trimmed().isEmpty();
                    const QString end = (tst) ? maxMag : fin;

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

    if (cond.getNbl() == 0) {
        // Masquage de la map
        ui->frame->setVisible(false);
        ui->listePrevisions->resize(ui->listePrevisions->width(), ui->ongletsResultats->height() - 58);
    } else {

        // Affichage de la map
        const QString fic = dirDat + QDir::separator() + "resultat.map";
        QFile fi(fic);

        if (fi.exists()) {
            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            map0 = flux.readAll();
        }
        fi.close();

        const QString lon(QString::number(-obs.getLongitude() * RAD2DEG));
        const QString lat(QString::number(obs.getLatitude() * RAD2DEG));
        map0 = map0.replace("NOMLIEU_CENTRE", obs.getNomlieu()).replace("LONGITUDE_CENTRE", lon).replace("LATITUDE_CENTRE", lat).
                replace("CHAINE_LONGITUDE", tr("Longitude")).replace("CHAINE_LATITUDE", tr("Latitude"));

        ui->frame->setVisible(true);
    }
    ui->ongletsResultats->setCurrentIndex(settings.value("affichage/ongletPrevisions", 0).toInt());
    ui->listePrevisions->selectRow(0);
    ui->listePrevisions->setFocus();

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
#if defined (Q_OS_WIN)
    prev = fi.readAll();
#else
    prev = fi.trUtf8(fi.readAll());
#endif
    ui->fichier->setText(prev);
    setVisible(true);
    prev = "";

    /* Retour */
    return;
}

void Afficher::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    settings.setValue("affichage/ongletPrevisions", ui->ongletsResultats->currentIndex());
    ui->fichier->clear();
}

void Afficher::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt)
    ui->ongletsResultats->resize(width(), height() - ui->barreOutils->height());
    ui->fichier->setGeometry(0, 0, width() - 4, height() - ui->barreOutils->height() - 24);
}

void Afficher::on_actionEnregistrer_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_WIN)
    const QString nomRepDefaut = dirOut.replace(QDir::separator(), "\\");
#else
    const QString nomRepDefaut = dirOut;
#endif
    const QString nomFicDefaut = _fichier.split(QDir::separator()).last();

    /* Corps de la methode */
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
    QUrl url(fi.fileName());
    url.setScheme("");
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
    const bool rotationLune = settings.value("affichage/rotationLune", true).toBool();
    const double magnitudeEtoiles = settings.value("affichage/magnitudeEtoiles", 4.0).toDouble();
    sceneSky = new QGraphicsScene;

    const QStringList tab = tabres.at(j).split("@");

    const QStringList listeTLEs(tab.at(0));
    QVector<TLE> tabtle;
    TLE::LectureFichier(cond.getFic(), listeTLEs, tabtle);
    Satellite sat(tabtle.at(0));

    // Date de debut du passage
    QString date = tab.at(1);
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
    double offset = (cond.getEcart()) ? cond.getOffset() : Date::CalculOffsetUTC(Date(dateI.getJourJulienUTC(), 0.).ToQDateTime(1));
    Date dateDeb(dateI.getJourJulienUTC(), offset);

    Date dateInit = Date(dateDeb, offset);
    bool atrouve = false;
    while (!atrouve) {
        obs.CalculPosVit(dateInit);

        sat.CalculPosVit(dateInit);
        sat.CalculCoordHoriz(obs, false);
        if (sat.getHauteur() < 0.) {
            atrouve = true;
        } else {
            dateInit = Date(dateInit.getJourJulien() - NB_JOUR_PAR_SEC * 10., offset);
        }
    }
    dateInit = Date(dateInit.getJourJulien() + NB_JOUR_PAR_SEC * 10., offset);

    Date dateMax, dateFin;
    if (cond.getNbl() == 3) {

        date = tab.at(2);
        QStringList max = date.replace("/", " ").replace(":", " ").split(" ");
        const Date dateM(max.at(0).toInt(), max.at(1).toInt(), max.at(2).toInt(), max.at(3).toInt(), max.at(4).toInt(),
                         max.at(5).toDouble(), 0.);
        offset = (cond.getEcart()) ? cond.getOffset() :
                                     Date::CalculOffsetUTC(Date(dateM.getJourJulienUTC(), 0.).ToQDateTime(1));
        dateMax = Date(dateM.getJourJulienUTC(), offset);

        date = tab.at(3);
        QStringList fin = date.replace("/", " ").replace(":", " ").split(" ");
        const Date dateF(fin.at(0).toInt(), fin.at(1).toInt(), fin.at(2).toInt(), fin.at(3).toInt(), fin.at(4).toInt(),
                         fin.at(5).toDouble(), 0.);
        offset = (cond.getEcart()) ? cond.getOffset() :
                                     Date::CalculOffsetUTC(Date(dateF.getJourJulienUTC(), 0.).ToQDateTime(1));
        dateFin = Date(dateF.getJourJulienUTC(), offset);
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
    if (affconst)
        Constellation::CalculConstellations(obs, constellations);
    if (affconst != Qt::Unchecked)
        LigneConstellation::CalculLignesCst(etoiles, lignesCst);

    // Position du satellite
    sat.CalculTraceCiel(dateInit, refraction, obs, 1);

    // Preparations pour l'affichage de la carte du ciel
    // Phase de la Lune
    if (affphaselune) {

        const double ll = atan2(lune.getPosition().getY(), lune.getPosition().getX());
        const double ls = atan2(soleil.getPosition().getY(), soleil.getPosition().getX());

        double diff = (ll - ls) * RAD2DEG;
        if (diff < 0.)
            diff += T360;
        indLune = (int) (diff / 12.190749) + 1;
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

    const QColor crimson(220, 20, 60);
    QPen crayon(Qt::white);

    // Affichage de la carte du ciel
    QRect rectangle(0, 0, ui->ciel->width()-1, ui->ciel->height()-1);
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
                    const int lcst = qRound(lciel - lciel * (1. - cst.getHauteur() * DEUX_SUR_PI) * sin(cst.getAzimut()));
                    const int bcst = qRound(hciel - hciel * (1. - cst.getHauteur() * DEUX_SUR_PI) * cos(cst.getAzimut()));

                    const int lst = lcst - lciel;
                    const int bst = hciel - bcst;

                    QGraphicsSimpleTextItem * const txtCst = new QGraphicsSimpleTextItem(cst.getNom());
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
    const QBrush bru2 = (soleil.getHauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);
    QListIterator<Etoile> it1(etoiles);
    while (it1.hasNext()) {

        const Etoile etoile = it1.next();
        if (etoile.isVisible() && etoile.getMagnitude() <= magnitudeEtoiles) {

            const int lstr = qRound(lciel - lciel * (1. - etoile.getHauteur() * DEUX_SUR_PI) * sin(etoile.getAzimut()));
            const int bstr = qRound(hciel - hciel * (1. - etoile.getHauteur() * DEUX_SUR_PI) * cos(etoile.getAzimut()));

            rectangle = (etoile.getMagnitude() > 3.) ? QRect(lstr-1, bstr-1, 2, 2) : QRect(lstr-1, bstr-1, 2, 3);
            sceneSky->addEllipse(rectangle, QPen(Qt::NoPen), bru2);

            // Nom des etoiles les plus brillantes
            if (affetoiles) {
                if (!etoile.getNom().isEmpty() && etoile.getNom().at(0).isUpper()) {
                    if (etoile.getMagnitude() < magnitudeEtoiles - 1.9) {

                        const int lst = lstr - lciel;
                        const int bst = hciel - bstr;
                        const QString nomstr = etoile.getNom().mid(0, 1) + etoile.getNom().mid(1).toLower();
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

            if (planetes.at(iplanete).getHauteur() >= 0.) {

                if (((iplanete == MERCURE || iplanete == VENUS) && planetes.at(iplanete).getDistance() > soleil.getDistance()) ||
                        iplanete >= MARS) {

                    const int lpla = qRound(lciel - lciel * (1. - planetes.at(iplanete).getHauteur() * DEUX_SUR_PI) *
                                            sin(planetes.at(iplanete).getAzimut()));
                    const int bpla = qRound(hciel - hciel * (1. - planetes.at(iplanete).getHauteur() * DEUX_SUR_PI) *
                                            cos(planetes.at(iplanete).getAzimut()));

                    const QBrush bru3(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                    rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                    sceneSky->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), bru3);

                    if (affplanetes == Qt::Checked) {
                        const int lpl = lpla - lciel;
                        const int bpl = hciel - bpla;
                        const QString nompla = planetes.at(iplanete).getNom();
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
        const Vecteur3D vec = Vecteur3D(cos(ad1) * cd1, sin(ad1) * cd1, sin(de1));
        const Vecteur3D vec1 = obs.getRotHz() * vec;

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
            const Vecteur3D vec0 = Vecteur3D(cos(ad2) * cd2, sin(ad2) * cd2, sin(de2));
            const Vecteur3D vec2 = obs.getRotHz() * vec0;

            const double ht2 = asin(vec2.getZ());

            double az2 = atan2(vec2.getY(), -vec2.getX());
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
            const int lsol = qRound(lciel - lciel * (1. - soleil.getHauteur() * DEUX_SUR_PI) * sin(soleil.getAzimut()));
            const int bsol = qRound(hciel - hciel * (1. - soleil.getHauteur() * DEUX_SUR_PI) * cos(soleil.getAzimut()));

            rectangle = QRect(lsol - 7, bsol - 7, 15, 15);
            sceneSky->addEllipse(rectangle, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
        }
    }

    if (affplanetes != Qt::Unchecked) {

        // Calcul des coordonnees radar des planetes Mercure et Venus
        for(int iplanete=MERCURE; iplanete<=VENUS; iplanete++) {

            if (planetes.at(iplanete).getHauteur() >= 0.) {

                if (planetes.at(iplanete).getDistance() < soleil.getDistance()) {

                    const int lpla = qRound(lciel - lciel * (1. - planetes.at(iplanete).getHauteur() * DEUX_SUR_PI) *
                                            sin(planetes.at(iplanete).getAzimut()));
                    const int bpla = qRound(hciel - hciel * (1. - planetes.at(iplanete).getHauteur() * DEUX_SUR_PI) *
                                            cos(planetes.at(iplanete).getAzimut()));

                    const QBrush bru3(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                    rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                    sceneSky->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), bru3);

                    if (affplanetes == Qt::Checked) {
                        const int lpl = lpla - lciel;
                        const int bpl = hciel - bpla;
                        const QString nompla = planetes.at(iplanete).getNom();
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
        const int llun = qRound(lciel - lciel * (1. - lune.getHauteur() * DEUX_SUR_PI) * sin(lune.getAzimut()));
        const int blun = qRound(hciel - hciel * (1. - lune.getHauteur() * DEUX_SUR_PI) * cos(lune.getAzimut()));

        QGraphicsPixmapItem * const lun = sceneSky->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun - 7, blun - 7);
        if (rotationLune && obs.getLatitude() < 0.)
            transform.rotate(180.);
        lun->setTransform(transform);
    }

    // Affichage de la trace dans le ciel
    if (sat.getTraceCiel().size() > 0) {

        const QList<QVector<double> > trace = sat.getTraceCiel();
        const double ht1 = trace.at(0).at(0);
        const double az1 = trace.at(0).at(1);
        int lsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
        int bsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

        const QColor bleuClair(173, 216, 230);

        bool aecr = false;
        bool adeb = false;
        bool amax = false;
        int min = -1;
        for(int i=1; i<trace.size(); i++) {

            const double ht2 = trace.at(i).at(0);
            const double az2 = trace.at(i).at(1);

            crayon = (fabs(trace.at(i).at(2)) <= EPSDBL100) ? QPen(bleuClair, 1) : QPen(crimson, 1);

            const int lsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
            const int bsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

            const QLineF lig = QLineF(lsat2, bsat2, lsat1, bsat1);

            // Determination des dates a afficher sur la carte du ciel
            Date dateTrace(trace.at(i).at(3) + offset, offset);
            if (dateTrace.getMinutes() != min) {
                aecr = true;
                min = dateTrace.getMinutes();
            }

            if (cond.getNbl() > 0) {

                if (dateTrace.getJourJulienUTC() > dateMax.getJourJulienUTC() && !adeb) {
                    adeb = true;
                    amax = true;
                    aecr = true;
                }
                if (dateTrace.getJourJulienUTC() >= dateDeb.getJourJulienUTC() &&
                        dateTrace.getJourJulienUTC() <= dateFin.getJourJulienUTC()) {
                    crayon = QPen(crayon.color(), 4);
                }
            }

            // Affichage de l'heure
            if (aecr) {

                aecr = false;
                const double ht3 = (i + 10 < trace.size()) ? trace.at(i+10).at(0) : trace.at(i-10).at(0);
                const double az3 = (i + 10 < trace.size()) ? trace.at(i+10).at(1) : trace.at(i-10).at(1);
                const int lsat3 = qRound(lciel - lciel * (1. - ht3 * DEUX_SUR_PI) * sin(az3));
                const int bsat3 = qRound(lciel - lciel * (1. - ht3 * DEUX_SUR_PI) * cos(az3));

                // Dessin d'une petite ligne correspondant a la date
                QLineF lig2 = QLineF(lsat2, bsat2, lsat3, bsat3).normalVector();
                lig2.setLength(4);
                sceneSky->addLine(lig2, QPen(bru2, 1.));

                QString sdate = "";
                if (amax) {
                    amax = false;
                    sdate = tr("Flash Iridium");
                } else {
                    if (dateTrace.getJourJulienUTC() < dateDeb.getJourJulienUTC() ||
                            dateTrace.getJourJulienUTC() > dateFin.getJourJulienUTC()) {
                        const DateSysteme sys = (cond.getSyst()) ? SYSTEME_24H : SYSTEME_12H;
                        sdate = dateTrace.ToShortDate(COURT, sys);
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
    }

    sceneSky->addEllipse(-20, -20, ui->ciel->width() + 40, ui->ciel->height() + 40,
                         QPen(QBrush(palette().background().color()), 40.6));
    sceneSky->addEllipse(0, 0, ui->ciel->width() - 1, ui->ciel->height() - 1, QPen(QBrush(Qt::gray), 2));

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
    if (cond.getNbl() != 0)
        loadMap(currentRow);
    loadSky(currentRow);
}

void Afficher::on_ongletsResultats_currentChanged(int index)
{
    Q_UNUSED(index)
    ui->listePrevisions->setFocus();
}
