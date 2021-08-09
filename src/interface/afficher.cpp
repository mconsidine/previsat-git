/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2011
 *
 * Date de revision
 * >    5 octobre 2020
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "ui_afficher.h"
#pragma GCC diagnostic warning "-Wconversion"
#include <QTextStream>
#pragma GCC diagnostic ignored "-Wswitch-default"
#include "ui_onglets.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "afficher.h"
#include "ciel.h"
#include "onglets.h"
#include "configuration/configuration.h"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/maths/maths.h"

Q_DECLARE_METATYPE(QList<ResultatPrevisions>)


QList<Etoile> etoiles;
QList<Constellation> constellations;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Afficher::Afficher(const TypeCalcul &typeCalcul, const ConditionsPrevisions &conditions, const DonneesPrevisions &donnees,
                   const QMap<QString, QList<QList<ResultatPrevisions> > > &resultats, Onglets *onglets,
                   QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Afficher)
{
    /* Declarations des variables locales */
    QStringList titres;

    /* Initialisations */
    _typeCalcul = typeCalcul;
    _conditions = conditions;
    _donnees = donnees;
    _resultats = resultats;
    _onglets = onglets;
    _ciel = nullptr;
    scene = nullptr;

    /* Corps du constructeur */
#if (BUILD_TEST == false)
    ui->setupUi(this);

    Etoile::Initialisation(Configuration::instance()->dirCommonData(), etoiles);
    Constellation::Initialisation(Configuration::instance()->dirCommonData(), constellations);

    if (_typeCalcul == EVENEMENTS) {
        ui->actionEnregistrer->setVisible(false);
        ui->frameCiel->setVisible(false);
        setMinimumSize(620, 630);
    } else {
        QStyle * const styleBouton = QApplication::style();
        ui->actionEnregistrer->setIcon(styleBouton->standardIcon(QStyle::SP_DialogSaveButton));
        ui->actionEnregistrer->setVisible(true);
        ui->frameCiel->setVisible(true);
        setMinimumSize(1226, 630);
    }
    resize(minimumSize());

    ui->resultatsPrevisions->clear();
    ui->detailsTransit->setVisible(false);

    QFont fnt;
    fnt.setBold(true);
    ui->resultatsPrevisions->horizontalHeader()->setFont(fnt);
    ui->afficherCarte->setVisible(false);

    switch (_typeCalcul) {
    case PREVISIONS:
        setWindowTitle(tr("Prévisions de passage"));
        titres << tr("Satellite") << tr("Date de début", "Date and hour") << tr("Date de fin", "Date and hour")
               << tr("Hauteur max", "Maximal elevation") << tr("Magn", "Magnitude") << tr("Hauteur Soleil");
        break;

    case EVENEMENTS:
        setWindowTitle(tr("Évènements orbitaux"));
        titres << tr("Satellite") << tr("Date", "Date and hour") << tr("PSO", "In orbit position") << tr("Longitude") << tr("Latitude")
               << tr("Évènement");
        break;

    case FLASHS:
        setWindowTitle(tr("Flashs"));
        ui->afficherCarte->setVisible(true);
        titres << tr("Satellite") << tr("Date de début", "Date and hour") << tr("Date de fin", "Date and hour")
               << tr("Hauteur Max", "Maximal elevation") << tr("Magn", "Magnitude") << tr("Mir", "Mirror") << tr("Hauteur Soleil");
        break;

    case TRANSITS:
        setWindowTitle(tr("Transits ISS"));
        ui->afficherCarte->setVisible(true);
        titres << tr("Date de début", "Date and hour") << tr("Date de fin", "Date and hour") << tr("Cst", "Constellation") << tr("Angle")
               << tr("Type", "Transit or conjunction") << tr("Corps") << tr("Ill", "Illumination") << tr("Hauteur Soleil");

        if (_resultats.size() > 0) {
            ui->detailsTransit->setVisible(true);
        }
        break;

    case TELESCOPE:
    default:
        break;
    }

    ui->resultatsPrevisions->setColumnCount(titres.count());
    ui->resultatsPrevisions->setHorizontalHeaderLabels(titres);

    ChargementResultats();

    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
    activateWindow();

#endif

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Afficher::~Afficher()
{
    if (_ciel != nullptr) {
        delete _ciel;
        _ciel = nullptr;
    }

    if (_onglets != nullptr) {
        delete _onglets;
        _onglets = nullptr;
    }

    if (tableDetail != nullptr) {
        delete tableDetail;
        tableDetail = nullptr;
    }

    if (afficherDetail != nullptr) {
        delete afficherDetail;
        afficherDetail = nullptr;
    }

    if (scene != nullptr) {
        delete scene;
        scene = nullptr;
    }

    delete ui;
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
 */



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
void Afficher::on_resultatsPrevisions_itemDoubleClicked(QTableWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_typeCalcul != EVENEMENTS) {

        int j = 0;
        int m = 0;
        const QList<ResultatPrevisions> list = ui->resultatsPrevisions->item(item->row(), 0)->data(Qt::UserRole).value<QList<ResultatPrevisions> > ();

        tableDetail = new QTableWidget;

        switch (_typeCalcul) {

        case PREVISIONS:
            tableDetail->setColumnCount(12);
            tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date", "Date and hour")
                                                   << tr("Azimut Sat", "Satellite azimuth") << tr("Hauteur Sat", "Satellite elevation")
                                                   << tr("AD Sat", "Satellite right ascension") << tr("Decl Sat", "Satellite declination")
                                                   << tr("Const", "Constellation") << tr("Magn", "Magnitude") << tr("Altitude") << tr("Distance")
                                                   << tr("Az Soleil", "Solar azimuth") << tr("Haut Soleil", "Solar elevation"));
            break;

        case FLASHS:
            tableDetail->setColumnCount(18);
            tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date", "Date and hour")
                                                   << tr("Azimut Sat", "Satellite azimuth") << tr("Hauteur Sat", "Satellite elevation")
                                                   << tr("AD Sat", "Satellite right ascension") << tr("Decl Sat", "Satellite declination")
                                                   << tr("Const", "Constellation") << tr("Ang", "Angle") << tr("Mir", "Mirror")
                                                   << tr("Magn", "Magnitude") << tr("Altitude") << tr("Dist", "Range")
                                                   << tr("Az Soleil", "Solar azimuth") << tr("Haut Soleil", "Solar elevation")
                                                   << tr("Long Max", "Longitude of the maximum") << tr("Lat Max", "Latitude of the maximum")
                                                   << tr("Magn Max", "Magnitude at the maximum") << tr("Distance"));
            break;

        case TRANSITS:
            tableDetail->setColumnCount(17);
            tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Date", "Date and hour") << tr("Azimut Sat", "Satellite azimuth")
                                                   << tr("Hauteur Sat", "Satellite elevation") << tr("AD Sat", "Satellite right ascension")
                                                   << tr("Decl Sat", "Satellite declination") << tr("Cst", "Constellation") << tr("Ang", "Angle")
                                                   << tr("Type", "Transit or conjunction") << tr("Corps") << tr("Ill", "Illumination")
                                                   << tr("Alt", "Altitude") << tr("Dist", "Range") << tr("Az Soleil", "Solar azimuth")
                                                   << tr("Haut Soleil", "Solar elevation") << tr("Long Max", "Longitude of the maximum")
                                                   << tr("Lat Max", "Latitude of the maximum") << tr("Distance"));
            break;

        case TELESCOPE:
        case EVENEMENTS:
        default:
            break;
        }

        tableDetail->setSelectionMode(QTableWidget::NoSelection);
        tableDetail->setCornerButtonEnabled(false);
        tableDetail->verticalHeader()->setVisible(false);

        int kmax;

        QFont fnt;
        fnt.setBold(true);
        tableDetail->horizontalHeader()->setFont(fnt);

        QStringList elems;
        QTableWidgetItem * itm;
        QListIterator<ResultatPrevisions> it(list);
        while (it.hasNext()) {

            elems.clear();
            const ResultatPrevisions res = it.next();

            switch (_typeCalcul) {
            case PREVISIONS:
                elems = ElementsDetailsPrevisions(res);
                break;

            case FLASHS:
                elems = ElementsDetailsFlashs(res);
                break;

            case TRANSITS:
                if ((j > 0) && (j < 4)) {
                    elems = ElementsDetailsTransits(res);
                }
                break;

            case TELESCOPE:
            case EVENEMENTS:
            default:
                break;
            }

            // Ajout d'une ligne dans le tableau de resultats
            if (!elems.isEmpty()) {

                tableDetail->insertRow(m);
                tableDetail->setRowHeight(m, 16);

                kmax = elems.count();
                if ((_typeCalcul == FLASHS) && (j != 1)) {
                    kmax = elems.count() - 4;
                }

                for(int k=0; k<kmax; k++) {

                    // Remplissage des elements d'une ligne
                    itm = new QTableWidgetItem(elems.at(k).trimmed());
                    itm->setTextAlignment(Qt::AlignCenter);
                    itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);

                    if ((k == 0) && (_typeCalcul != TRANSITS)) {
                        itm->setToolTip(elems.at(0));
                    }

                    tableDetail->setItem(m, k, itm);
                    tableDetail->resizeColumnToContents(k);
                }
                m++;
            }

            j++;
        }

        tableDetail->setSelectionMode(QTableWidget::SingleSelection);
        tableDetail->setSelectionBehavior(QTableWidget::SelectRows);
#if QT_VERSION >= 0x050000
        tableDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
        tableDetail->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

        afficherDetail = new QMainWindow;
        afficherDetail->setStyleSheet("QHeaderView::section { background-color:rgb(235, 235, 235) }");

        switch (_typeCalcul) {

        case PREVISIONS:
            afficherDetail->setWindowTitle(tr("Détail du passage"));
            break;

        case FLASHS:
            afficherDetail->setWindowTitle(tr("Détail du flash"));
            break;

        case TRANSITS:
            afficherDetail->setWindowTitle(tr("Détail du transit ou conjonction"));
            break;

        case TELESCOPE:
        case EVENEMENTS:
        default:
            break;
        }

        afficherDetail->setCentralWidget(tableDetail);
#if defined (Q_OS_LINUX)
        int lrg = 5;
#else
        int lrg = 2;
#endif
        if (tableDetail->rowCount() > 10) {
            lrg += 17;
        }
        for(int i=0; i<tableDetail->columnCount(); i++) {
            lrg += tableDetail->columnWidth(i);
        }

        afficherDetail->setFixedSize(lrg, tableDetail->horizontalHeader()->height() + tableDetail->rowHeight(1) * qMin(10, tableDetail->rowCount()));
        afficherDetail->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, afficherDetail->size(), geometry()));
        afficherDetail->show();
    }

    /* Retour */
    return;
}

void Afficher::on_actionEnregistrerTxt_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
#if (BUILD_TEST == true)
    const QString fic = _conditions.ficRes;
#else
    const QString nomFicDefaut = _conditions.ficRes.split(QDir::separator()).last();
    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), Configuration::instance()->dirOut() + QDir::separator() +
                                                     nomFicDefaut, tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
#endif

    if (!fic.isEmpty()) {

        QFile fichier(_conditions.ficRes);
        if (fichier.exists()) {
            fichier.remove();
        }

        // Ecriture de l'entete du fichier de resultat
        EcrireEntete();

        // Ouverture du fichier de resultat
        fichier.open(QIODevice::Append | QIODevice::Text);
        QTextStream flux(&fichier);

        int i;
        int kmin;
        int kmax;
        QString ligne;
        QString nomsat;
        QStringList evts;
        QStringList elems;

        bool ecrireNomColonnes = true;
        QMapIterator<QString, QList<QList<ResultatPrevisions> > > it1(_resultats);
        while (it1.hasNext()) {
            it1.next();

            // Nom du satellite
            nomsat = it1.value().at(0).at(0).nom;
            if (nomsat.contains("R/B") || nomsat.contains(" DEB")) {
                nomsat += "  " + tr("(numéro NORAD : %1)").arg(it1.key().split(" ").last());
            }

            // Description des colonnes
            switch (_typeCalcul) {

            case PREVISIONS:
                flux << nomsat << endl;
                flux << tr("   Date      Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const Magn  Altitude  Distance  Az Soleil  Haut Soleil",
                           "Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, Constellation, " \
                           "Magnitude, Altitude, Range, Solar azimuth, Solar elevation")
                     << endl;
                break;

            case FLASHS:
                flux << tr("Satellite     Date      Heure      Azimut Sat Hauteur Sat  AD Sat    Decl Sat   Cst  Ang  Mir Magn" \
                           "       Alt      Dist  Az Soleil  Haut Soleil   Long Max    Lat Max    Magn Max  Distance",
                           "Satellite, Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, " \
                           "Constellation, Angle, Mirror, Magnitude, Altitude, Range, Solar azimuth, Solar elevation, Longitude of the maximum, " \
                           "Latitude of the maximum, Magnitude at the maximum, Range from the maximum")
                     << endl;
                break;

            case TRANSITS:
                if (ecrireNomColonnes) {
                    flux << tr("   Date      Heure      Azimut Sat Hauteur Sat  AD Sat    Decl Sat   Cst  Ang  Type Corps" \
                               " Ill    Alt    Dist  Az Soleil  Haut Soleil   Long Max    Lat Max     Distance",
                               "Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, Constellation, "
                               "Angle, Type, Body, Illumination, Altitude, Range, Solar azimuth, Solar elevation, Longitude of the maximum, " \
                               "Latitude of the maximum, Range from the maximum")
                         << endl;
                    ecrireNomColonnes = false;
                }
                break;

            case EVENEMENTS:
                flux << nomsat << endl;
                flux << tr("   Date      Heure      PSO    Longitude  Latitude  Évènements",
                           "Date, Hour, In orbit position, Longitude, Latitude, Events") << endl;
                break;

            case TELESCOPE:
            default:
                break;
            }

            QListIterator<QList<ResultatPrevisions> > it2(it1.value());
            while (it2.hasNext()) {

                i = 0;
                evts.clear();
                QListIterator<ResultatPrevisions> it3(it2.next());
                while (it3.hasNext()) {

                    kmin = 0;
                    elems.clear();
                    const ResultatPrevisions res = it3.next();

                    switch (_typeCalcul) {
                    case PREVISIONS:
                        kmin = 1;
                        elems = ElementsDetailsPrevisions(res);
                        break;

                    case FLASHS:
                        elems = ElementsDetailsFlashs(res);
                        break;

                    case TRANSITS:
                        if ((i > 0) && (i < 4)) {
                            elems = ElementsDetailsTransits(res);
                        }
                        break;

                    case EVENEMENTS:
                        kmin = 1;
                        elems = ElementsDetailsEvenements(res);
                        break;

                    case TELESCOPE:
                    default:
                        break;
                    }

                    if (!elems.isEmpty()) {

                        kmax = elems.count();
                        if ((_typeCalcul == FLASHS) && (i != 1)) {
                            kmax = elems.count() - 4;
                        }

                        ligne = "";
                        for(int k=kmin; k<kmax; k++) {
                            ligne += elems.at(k) + " ";
                        }

                        if (_typeCalcul == EVENEMENTS) {
                            evts.append(ligne);
                        } else {
                            flux << ligne.trimmed() << endl;
                        }
                    }
                    i++;
                }

                if (_typeCalcul == EVENEMENTS) {

                    evts.sort();
                    QStringListIterator it4(evts);
                    while (it4.hasNext()) {
                        flux << it4.next().trimmed() << endl;
                    }
                }
                flux << endl;
            }
        }

#if (BUILD_TEST == false)
        flux << endl << tr("Temps écoulé : %1s").arg(1.e-3 * static_cast<double> (_donnees.tempsEcoule), 0, 'f', 2) << endl;
#endif
        fichier.close();

#if (BUILD_TEST == false)
        QFile fi2(fic);
        fichier.copy(fi2.fileName());
#endif
    }

    /* Retour */
    return;
}

/*
 * Affichage du détail d'un transit ISS
 */
void Afficher::AffichageDetailTransit(const Observateur &observateur, const Soleil &soleil, const Lune &lune, const QList<ResultatPrevisions> &list)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int lciel = qRound(0.5 * ui->detailsTransit->width());
    const int hciel = qRound(0.5 * ui->detailsTransit->height());

    // Determination de la couleur du ciel
    const double hts = soleil.hauteur() * RAD2DEG;
    const QBrush couleurCiel = Ciel::CalculCouleurCiel(hts);

    if (scene != nullptr) {
        delete scene;
        scene = nullptr;
    }

    scene = new QGraphicsScene;
    scene->setBackgroundBrush(Qt::white);

    // Affichage de la carte du ciel
    QRect rectangle(0, 0, ui->detailsTransit->width(), ui->detailsTransit->height());
    scene->setSceneRect(rectangle);

    /* Corps de la methode */
    // Dessin de la Lune ou du Soleil
    if (list.at(0).typeCorps == CORPS_SOLEIL) {

        QPixmap pixsol;
        pixsol.load(":/resources/soleil.png");
        pixsol = pixsol.scaled(100, 100);

        QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
        QTransform transform;
        transform.translate(lciel - 50, hciel - 50);
        sol->setTransform(transform);

    } else {

        // Angle horaire
        const double angleHoraire = observateur.tempsSideralGreenwich() - observateur.longitude() - lune.ascensionDroite();

        // Angle parallactique
        const double angleParallactique = RAD2DEG *
                atan(sin(angleHoraire) / (tan(observateur.latitude()) * cos(lune.declinaison()) - sin(lune.declinaison()) * cos(angleHoraire)));

        QPixmap pixlun;
        pixlun.load(":/resources/lune.png");
        pixlun = pixlun.scaled(100, 100);

        // Dessin de la Lune et rotations
        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        transform.translate(lciel, hciel);
        transform.rotate(angleParallactique);
        if (_onglets->ui()->rotationLune->isChecked() && (observateur.latitude() < 0.)) {
            transform.rotate(180.);
        }
        transform.translate(-50, -50);
        lun->setTransform(transform);

        // Dessin de la phase
        if (_onglets->ui()->affphaselune->isChecked()) {

            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 160));
            const QPen stylo(Qt::NoBrush, 0);
            const QPolygonF poly = Ciel::AffichagePhaseLune(lune, 51);

            QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);
            const QRectF rect = poly.boundingRect();
            transform.translate(-rect.x(), -rect.y());
            omb->setTransform(transform);
        }
    }

    double deltaAzimut;
    double deltaHauteur;
    QList<QPair<double, double> > coord;
    QListIterator<ResultatPrevisions> it(list);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Coordonnees relatives de l'ISS par rapport a la Lune ou au Soleil
        if (res.typeCorps == CORPS_SOLEIL) {

            deltaAzimut = RAD2DEG * (res.azimut - res.azimutSoleil);
            deltaHauteur = RAD2DEG * (res.hauteur - res.hauteurSoleil);

        } else {
            deltaAzimut = RAD2DEG * (res.azimut - lune.azimut());
            deltaHauteur = RAD2DEG * (res.hauteur - lune.hauteur());
        }

        const double lsat = lciel + static_cast<int> (200. * deltaAzimut);
        const double hsat = hciel - static_cast<int> (200. * deltaHauteur);
        const QPair<double, double> xy(lsat, hsat);
        coord.append(xy);
    }

    double lsat1 = coord.at(0).first;
    double hsat1 = coord.at(0).second;

    const QPen couleur((list.at(0).typeCorps == CORPS_SOLEIL) ? Qt::black : Qt::lightGray, list.at(0).typeCorps);

    QLineF lig1;
    QLineF lig2;

    QListIterator<QPair<double, double> > it2(coord);
    it2.next();
    while (it2.hasNext()) {

        const QPair<double, double> xy = it2.next();
        const QLineF lig(xy.first, xy.second, lsat1, hsat1);
        scene->addLine(lig, couleur);

        if ((fabs(xy.first - coord.last().first) < EPSDBL100) && (fabs(xy.second - coord.last().second) < EPSDBL100)) {

            lig1 = lig.normalVector();
            lig2 = lig1;

            lig1.setAngle(lig1.angle() - 60.);
            lig1.setLength(12.);
            lig2.setAngle(lig2.angle() - 120.);
            lig2.setLength(12.);

            scene->addLine(lig1, couleur);
            scene->addLine(lig2, couleur);
        }

        lsat1 = xy.first;
        hsat1 = xy.second;
    }

    ui->detailsTransit->setScene(scene);

    /* Retour */
    return;
}

/*
 * Chargement de la carte representant la trace du maximum
 */
void Afficher::ChargementCarte(const Observateur &observateur, const QList<ResultatPrevisions> &list)
{
    /* Declarations des variables locales */
    static QString map;

    /* Initialisations */
    const QString lon(QString::number(-observateur.longitude() * RAD2DEG));
    const QString lat(QString::number(observateur.latitude() * RAD2DEG));
    const QString unite((_conditions.unite == tr("km", "Kilometer")) ? tr("m", "meter") : tr("ft", "foot"));
    const QString alt(QString::number((int) (observateur.altitude() * 1000. * ((unite == tr("m", "meter")) ? 1. : PIED_PAR_METRE) + EPSDBL100)) +
                      " " + unite);

    /* Corps de la methode */
    // Lecture du fichier balise
    if (map.isEmpty()) {

        const QString ficMap = Configuration::instance()->dirLocalData() + QDir::separator() + "html" + QDir::separator() + "resultat.map";
        QFile fi(ficMap);

        if (fi.exists() && (fi.size() != 0)) {
            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            map = flux.readAll();
            fi.close();
        }
    }

    // Remplacement des balises par les donnees
    _mapResultats = map;
    _mapResultats = _mapResultats
            .replace("NOMLIEU_CENTRE", observateur.nomlieu().toUtf8())
            .replace("LONGITUDE_CENTRE", lon)
            .replace("LATITUDE_CENTRE", lat)
            .replace("ALTITUDE_CENTRE", alt)
            .replace("CHAINE_LONGITUDE", tr("Longitude"))
            .replace("CHAINE_LATITUDE", tr("Latitude"))
            .replace("CHAINE_ALTITUDE", tr("Altitude"))
            .replace("LONGITUDE1", QString::number(-list.first().obsmax.longitude() * RAD2DEG))
            .replace("LONGITUDE2", QString::number(-list.last().obsmax.longitude() * RAD2DEG))
            .replace("LATITUDE1", QString::number(list.first().obsmax.latitude() * RAD2DEG))
            .replace("LATITUDE2", QString::number(list.last().obsmax.latitude() * RAD2DEG));

    // Creation du fichier html
    QFile fr(Configuration::instance()->dirTmp() + QDir::separator() + "resultat.html");
    fr.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fr);
    flux << _mapResultats;
    fr.close();

    /* Retour */
    return;
}

/*
 * Chargement des resultats
 */
void Afficher::ChargementResultats()
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */
    int j = 0;
    int imax = 1;

    /* Corps de la methode */
    QTableWidgetItem * item;
    QMapIterator<QString, QList<QList<ResultatPrevisions> > > it1(_resultats);
    while (it1.hasNext()) {
        it1.next();

        QListIterator<QList<ResultatPrevisions> > it2(it1.value());
        while (it2.hasNext()) {

            const QList<ResultatPrevisions> list = it2.next();

            elems.clear();
            switch (_typeCalcul) {

            case PREVISIONS:
                elems = ElementsPrevisions(list);
                break;

            case FLASHS:
                elems = ElementsFlashs(list);
                break;

            case TRANSITS:
                elems = ElementsTransits(list);
                break;

            case EVENEMENTS:
                imax = list.size();
                break;

            case TELESCOPE:
            default:
                break;
            }

            for(int i=0; i<imax; i++) {

                if (_typeCalcul == EVENEMENTS) {
                    elems = ElementsEvenements(i, list);
                }

                // Ajout d'une ligne dans le tableau de resultats
                ui->resultatsPrevisions->insertRow(j);
                ui->resultatsPrevisions->setRowHeight(j, 16);

                for(int k=0; k<elems.count(); k++) {

                    // Remplissage des elements d'une ligne
                    item = new QTableWidgetItem(elems.at(k));
                    item->setTextAlignment(((_typeCalcul == EVENEMENTS) && (k == (elems.count() - 1))) ? Qt::AlignLeft : Qt::AlignCenter);
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

                    if (k == 0) {
                        item->setToolTip(elems.at(0));
                        item->setData(Qt::UserRole, QVariant::fromValue<QList<ResultatPrevisions> > (list));
                    }

                    ui->resultatsPrevisions->setItem(j, k, item);
                    if ((k > 0) || (_typeCalcul == TRANSITS)) {
                        ui->resultatsPrevisions->resizeColumnToContents(k);
                    }
                }
                j++;
            }
        }
    }

    ui->resultatsPrevisions->setStyleSheet("QHeaderView::section { background-color:rgb(235, 235, 235) }");
    ui->resultatsPrevisions->horizontalHeader()->setStretchLastSection(true);
    ui->resultatsPrevisions->setToolTip(tr("Double-cliquez sur une ligne pour afficher plus de détails"));
#if QT_VERSION >= 0x050000
    ui->resultatsPrevisions->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    ui->resultatsPrevisions->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    ui->resultatsPrevisions->setAlternatingRowColors(true);
    if (_onglets->ui()->ordreChronologiqueMetOp->isChecked()) {
        ui->resultatsPrevisions->sortItems(1);
    }
    ui->resultatsPrevisions->selectRow(0);

    /* Retour */
    return;
}

/*
 * Ecriture de l'entete du fichier de resultats
 */
void Afficher::EcrireEntete() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString ligne;
    QFile fi(_conditions.ficRes);
    if (fi.exists()) {
        fi.remove();
    }

    QDir di(Configuration::instance()->dirTmp());
    if (!di.exists()) {
        di.mkpath(Configuration::instance()->dirTmp());
    }

    /* Corps de la methode */
    QFile fichier(_conditions.ficRes);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    // Ligne d'entete
#if (BUILD_TEST == false)
    flux << QString("%1 %2 / %3 (c) %4").arg(QCoreApplication::applicationName()).arg(QString(APPVERSION)).arg(QCoreApplication::organizationName()).
            arg(QString(APP_ANNEES_DEV)) << endl << endl;
#endif

    // Lieu d'observation
    if (_typeCalcul != EVENEMENTS) {

        ligne = QObject::tr("Lieu d'observation        : %1     %2 %3   %4 %5   %6 %7");
        const QString lon = Maths::ToSexagesimal(fabs(_conditions.observateur.longitude()), DEGRE, 3, 0, false, false);
        const QString ew = (_conditions.observateur.longitude() >= 0.) ? QObject::tr("Ouest") : QObject::tr("Est");

        const QString lat = Maths::ToSexagesimal(fabs(_conditions.observateur.latitude()), DEGRE, 2, 0, false, false);
        const QString ns = (_conditions.observateur.latitude() >= 0.) ? QObject::tr("Nord") : QObject::tr("Sud");

        const double alt = (_conditions.unite == QObject::tr("km", "kilometer")) ? _conditions.observateur.altitude() :
                                                                                   _conditions.observateur.altitude() * PIED_PAR_METRE;
        const QString unite = (_conditions.unite == QObject::tr("km", "kilometer")) ? QObject::tr("m", "meter") : QObject::tr("ft", "foot");

        ligne = ligne.arg(_conditions.observateur.nomlieu()).arg(lon).arg(ew).arg(lat).arg(ns).arg(1000. * alt, 0, 'f', 0).arg(unite);
        flux << ligne << endl;
    }

    // Fuseau horaire
    ligne = QObject::tr("Fuseau horaire            : %1");
    QString chaine = QObject::tr("UTC", "Universal Time Coordinated");
    if (_conditions.ecart) {
        if (fabs(_conditions.offset) > EPSDBL100) {
            QTime heur(0, 0);
            heur = heur.addSecs((int) (_conditions.offset * NB_SEC_PAR_JOUR + EPS_DATES));
            chaine = chaine.append((_conditions.offset > 0.) ? " + " : " - ").append(heur.toString("hh:mm"));
        }
    }
    flux << ligne.arg(chaine) << endl;

    if (_typeCalcul != EVENEMENTS) {

        const QString cond1 = QObject::tr("Conditions d'observations :") + " ";
        const QString cond2 = QObject::tr("Hauteur minimale du satellite = %1°");

        // Conditions d'observations
        if (_typeCalcul == TRANSITS) {
            flux << (cond1 + cond2).arg(_conditions.hauteur * RAD2DEG) << endl;
        } else {
            flux << cond1 + QObject::tr("Hauteur maximale du Soleil = %1°").arg(_conditions.crepuscule * RAD2DEG) << endl;
            flux << QString(cond1.size(), ' ') << cond2.arg(_conditions.hauteur * RAD2DEG) << endl;
        }

        // Unite de distance
        flux << tr("Unité de distance         : %1").arg(_conditions.unite) << endl;
    }

    // Age des TLE
    const QString date = Date(_conditions.jj1 + _conditions.offset + EPS_DATES, 0.)
            .ToShortDate(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H).trimmed();

    if (_donnees.ageTle.count() == 1) {
        flux << endl << tr("Age du TLE                : %1 jours (au %2)").arg(_donnees.ageTle.at(0), 4, 'f', 2).arg(date) << endl << endl << endl;
    } else {

        flux << endl << tr("Age du TLE le plus récent : %1 jours (au %2)\nAge du TLE le plus ancien : %3 jours")
                .arg(_donnees.ageTle.at(0), 4, 'f', 2).arg(date).arg(_donnees.ageTle.at(1), 4, 'f', 2) << endl << endl << endl;
    }

    /* Retour */
    return;
}

/*
 * Elements des evenements a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsEvenements(const int index, const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    elems.append(ElementsDetailsEvenements(liste.at(index)));

    /* Retour */
    return elems;
}

/*
 * Elements des evenements pour la sauvegarde dans un fichier texte
 */
QStringList Afficher::ElementsDetailsEvenements(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // PSO
    elems.append(QString("%1°").arg(res.pso * RAD2DEG, 6, 'f', 2, QChar('0')));

    // Longitude, latitude
    elems.append(QString("  %1° %2")
                 .arg(fabs(res.longitude * RAD2DEG), 6, 'f', 2, QChar('0')).arg((res.longitude >= 0.) ?
                                                                                    tr("W", "West") : tr("E", "East")));
    elems.append(QString(" %1° %2 ")
                 .arg(fabs(res.latitude * RAD2DEG), 5, 'f', 2, QChar('0')).arg((res.latitude >= 0.) ?
                                                                                   tr("N", "North") : tr("S", "South")));

    // Type d'evenement
    elems.append(res.typeEvenement);

    /* Retour */
    return elems;
}

/*
 * Elements des flashs a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsFlashs(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(liste.first().nom);

    // Date de debut
    const Date dateDeb(liste.first().date, Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1)));
    elems.append(dateDeb.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Date de fin
    const Date dateFin(liste.last().date, Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1)));
    elems.append(dateFin.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    double htMax = -1.;
    double magnMax = 99.;
    double htSolMax = 0.;
    bool penombre = false;
    QString miroir;

    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Hauteur max
        if (res.hauteur >= htMax) {
            htMax = res.hauteur;
            htSolMax = res.hauteurSoleil;
        }

        // Magnitude maximale
        if (res.magnitude <= magnMax) {
            magnMax = res.magnitude;
            penombre = res.penombre;
            miroir = res.miroir;
        }
    }

    // Hauteur maximale du satellite
    elems.append(Maths::ToSexagesimal(htMax, DEGRE, 2, 0, false, false));

    // Magnitude
    const QString fmagn = "%1%2%3";
    const QString signe = (magnMax >= 0.) ? "+" : "-";
    const QString pen = (penombre) ? "*" : " ";
    elems.append(fmagn.arg(signe).arg(fabs(magnMax), 0, 'f', 1, QChar('0')).arg(pen));

    // Panneau reflechissant
    elems.append(miroir);

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des flashs pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList Afficher::ElementsDetailsFlashs(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(QString("%1").arg(res.nom, -10));

    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Angle de reflexion, miroir
    elems.append(QString("%1  ").arg(res.angleReflexion * RAD2DEG, 5, 'f', 2));
    elems.append(QString("%1 ").arg(res.miroir));

    // Magnitude
    const QString fmagn = "%1%2%3";
    QString signe = (res.magnitude >= 0.) ? "+" : "-";
    QString pen = (res.penombre) ? "*" : " ";
    elems.append(fmagn.arg(signe).arg(fabs(res.magnitude), 0, 'f', 1, QChar('0')).arg(pen));

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi", "nautical mile")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, DEGRE, 2, 0, true, false));

    if (!res.obsmax.nomlieu().isEmpty()) {

        const QString ew = (res.obsmax.longitude() >= 0.) ? QObject::tr("W", "West") : QObject::tr("E", "East");
        const QString ns = (res.obsmax.latitude() >= 0.) ? QObject::tr("N", "North") : QObject::tr("S", "South");

        // Longitude du maximum
        elems.append(QString("  %1 %2").arg(fabs(res.obsmax.longitude() * RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew));

        // Latitude du maximum
        elems.append(QString(" %1 %2    ").arg(fabs(res.obsmax.latitude() * RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns));

        // Magnitude au maximum
        signe = (res.magnitudeMax >= 0.) ? "+" : "-";
        pen = (res.penombreMax) ? "*" : " ";
        elems.append(fmagn.arg(signe).arg(fabs(res.magnitudeMax), 0, 'f', 1, QChar('0')).arg(pen));

        // Distance au maximum et cap
        elems.append(QString(" %1 (%2)").arg(res.distanceObs, 6, 'f', 1).arg(res.cap));
    }

    /* Retour */
    return elems;
}

/*
 * Elements des previsions de passage a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsPrevisions(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(liste.first().nom);

    // Date de debut
    const Date dateDeb(liste.first().date, Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1)));
    elems.append(dateDeb.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Date de fin
    const Date dateFin(liste.last().date, Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1)));
    elems.append(dateFin.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    double htMax = -1.;
    double magnMax = 99.;
    double magnStd = 99.;
    double htSolMax = 0.;
    bool penombre = false;

    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Hauteur max
        if (res.hauteur >= htMax) {
            htMax = res.hauteur;
            htSolMax = res.hauteurSoleil;
        }

        // Magnitude maximale
        if (res.magnitude <= magnMax) {
            magnMax = res.magnitude;
            magnStd = res.magnitudeStd;
            penombre = res.penombre;
        }
    }

    // Hauteur maximale du satellite
    elems.append(Maths::ToSexagesimal(htMax, DEGRE, 2, 0, false, false));

    // Magnitude
    if (magnMax > 98.) {
        elems.append((_conditions.eclipse || (magnStd > 98.)) ? "????" : "----");
    } else {
        const QString fmagn = "%1%2%3";
        const QString signe = (magnMax >= 0.) ? "+" : "-";
        const QString pen = (penombre) ? "*" : " ";
        elems.append(fmagn.arg(signe).arg(fabs(magnMax), 0, 'f', 1, QChar('0')).arg(pen));
    }

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des previsions de passage pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList Afficher::ElementsDetailsPrevisions(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Magnitude
    if (res.magnitude > 98.) {
        elems.append((_conditions.eclipse || (res.magnitudeStd > 98.)) ? " ???? " : " ---- ");
    } else {
        const QString fmagn = "%1%2%3%4";
        const QString esp = (res.magnitude < 9.95) ? " " : "";
        const QString signe = (res.magnitude >= 0.) ? "+" : "-";
        const QString pen = (res.penombre) ? "*" : " ";
        elems.append(fmagn.arg(esp).arg(signe).arg(fabs(res.magnitude), 0, 'f', 1, QChar('0')).arg(pen));
    }

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi", "nautical mile")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des transits ISS a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsTransits(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Date de debut
    const Date dateDeb(liste.first().date, Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1)));
    elems.append(dateDeb.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Date de fin
    const Date dateFin(liste.last().date, Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1)));
    elems.append(dateFin.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    bool eclipse = false;
    bool penombre = false;
    double angMin = PI;
    QString cst;
    QString type;
    QString corps;
    double htSolMax = 0.;

    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Hauteur max
        if (res.angle <= angMin) {
            angMin = res.angle;
            htSolMax = res.hauteurSoleil;
            cst = res.constellation;
            eclipse = res.eclipse;
            penombre = res.penombre;
            type = (res.transit) ? tr("T", "transit") : tr("C", "conjunction");
            corps = (res.typeCorps == CORPS_SOLEIL) ? tr("S", "Sun") : tr("L", "Moon");
        }
    }

    // Constellation
    elems.append(cst);

    // Angle minimal
    elems.append(QString("%1").arg(angMin * RAD2DEG, 5, 'f', 2));

    // Type, corps
    elems.append(type);
    elems.append(corps);

    // Illumination
    QString illumination = tr("Lum", "Lit");
    if (eclipse) {
        illumination = tr("Omb", "Shadow");
    }

    if (penombre) {
        illumination = tr("Pen", "Penumbra");
    }

    elems.append(illumination);

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des transits ISS pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList Afficher::ElementsDetailsTransits(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Angle de reflexion, type, corps
    elems.append(QString("%1  ").arg(res.angle * RAD2DEG, 5, 'f', 2));
    elems.append(QString("%1   ").arg((res.transit) ? tr("T", "transit") : tr("C", "conjunction")));
    elems.append(QString("%1  ").arg((res.typeCorps == CORPS_SOLEIL) ? tr("S", "Sun") : tr("L", "Moon")));

    // Illumination
    QString illumination = tr("Lum", "Lit");
    if (res.eclipse) {
        illumination = tr("Omb", "Shadow");
    }

    if (res.penombre) {
        illumination = tr("Pen", "Penumbra");
    }

    elems.append(illumination);

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi", "nautical mile")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 6, 'f', 1));
    elems.append(QString("%1").arg(distance, 7, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, DEGRE, 2, 0, true, false));

    if (!res.obsmax.nomlieu().isEmpty()) {

        const QString ew = (res.obsmax.longitude() >= 0.) ? QObject::tr("W", "West") : QObject::tr("E", "East");
        const QString ns = (res.obsmax.latitude() >= 0.) ? QObject::tr("N", "North") : QObject::tr("S", "South");

        // Longitude du maximum
        elems.append(QString("  %1 %2").arg(fabs(res.obsmax.longitude() * RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew));

        // Latitude du maximum
        elems.append(QString(" %1 %2 ").arg(fabs(res.obsmax.latitude() * RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns));

        // Distance au maximum et cap
        elems.append(QString(" %1 (%2)").arg(res.distanceObs, 6, 'f', 1).arg(res.cap));
    }

    /* Retour */
    return elems;
}

void Afficher::on_resultatsPrevisions_itemSelectionChanged()
{
    /* Declarations des variables locales */
    Date dateMax;
    Soleil soleil;
    Lune lune;
    QList<LigneConstellation> lignesCst;
    QList<Planete> planetes;
    QList<Satellite> satellites;

    /* Initialisations */
    const QList<ResultatPrevisions> list = ui->resultatsPrevisions->item(ui->resultatsPrevisions->currentRow(), 0)->data(Qt::UserRole)
            .value<QList<ResultatPrevisions> > ();
    const Date dateDeb = Date(list.first().date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));
    const Date dateFin = Date(list.last().date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));

    if (_typeCalcul == FLASHS) {
        dateMax = Date(list.at(1).date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));
    } else if (_typeCalcul == TRANSITS) {
        dateMax = Date(list.at(2).date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));
    } else {
        dateMax = dateDeb;
    }

    /* Corps de la methode */
    // Calcul de la position de l'observateur
    Observateur observateur = _conditions.observateur;
    observateur.CalculPosVit(dateDeb);

    // Calcul de la position du Soleil
    soleil.CalculPosition(dateDeb);
    soleil.CalculCoordHoriz(observateur);

    // Calcul de la position de la Lune
    lune.CalculPosition(dateDeb);
    lune.CalculPhase(soleil);
    lune.CalculCoordHoriz(observateur);
    lune.CalculCoordEquat(observateur, false);

    // Calcul de la position du catalogue d'etoiles
    Etoile::CalculPositionEtoiles(observateur, etoiles);
    if (_onglets->ui()->affconst->isChecked()) {
        Constellation::CalculConstellations(observateur, constellations);
    }
    if (_onglets->ui()->affconst->checkState() != Qt::Unchecked) {
        LigneConstellation::CalculLignesCst(etoiles, lignesCst);
    }

    // Calcul de la position des planetes
    for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

        planetes.append(Planete(static_cast<IndicePlanete>(iplanete)));
        planetes.last().CalculPosition(dateDeb, soleil);
        planetes.last().CalculCoordHoriz(observateur);
    }

    // Satellite selectionne
    Satellite sat(list.at(0).tle);
    sat.CalculPosVit(dateMax);
    sat.CalculCoordHoriz(observateur);

    const Date dateLever = Evenements::CalculAOS(dateMax, sat, observateur, false).date;
    sat.CalculTraceCiel(dateLever, true, _onglets->ui()->refractionPourEclipses->isChecked(), observateur, 1);
    satellites.append(sat);

    // Chargement du ciel
    if (_ciel != nullptr) {
        delete _ciel;
        _ciel = nullptr;
    }
    _ciel = new Ciel(_onglets, ui->frameCiel);
    _ciel->resize(ui->frameCiel->width(), ui->frameCiel->width() - 50);

    // Affichage de la carte du ciel
    _ciel->show(observateur, soleil, lune, lignesCst, constellations, etoiles, planetes, satellites, (_typeCalcul == FLASHS), true, dateDeb, dateMax,
                dateFin);

    // Affichage du detail du transit ISS
    if (_typeCalcul == TRANSITS) {

        ui->detailsTransit->setVisible(list.at(2).transit);

        if (list.at(2).transit) {
            AffichageDetailTransit(observateur, soleil, lune, list);
        }
    }

    if ((_typeCalcul == FLASHS) || (_typeCalcul == TRANSITS)) {
        ChargementCarte(observateur, list);
    }

    /* Retour */
    return;
}


void Afficher::on_afficherCarte_clicked()
{
    // Ouverture du navigateur
    QFile fi(Configuration::instance()->dirTmp() + "resultat.html");
    if (fi.exists() && (fi.size() != 0)) {
        QDesktopServices::openUrl(QUrl("file:///" + fi.fileName()));
    }
}
