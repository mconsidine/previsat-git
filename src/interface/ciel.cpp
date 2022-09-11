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
 * >    ciel.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    3 avril 2020
 *
 * Date de revision
 * >    11 septembre 2022
 *
 */

#include "configuration/configuration.h"
#include "ciel.h"
#include "onglets.h"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/maths/maths.h"
#include <QGraphicsPixmapItem>
#pragma GCC diagnostic ignored "-Wconversion"
#include "ui_ciel.h"
#pragma GCC diagnostic warning "-Wconversion"
#include "ui_onglets.h"


// Couleur des planetes
static const QList<QColor> couleurPlanetes(QList<QColor> () << Qt::gray << Qt::white << Qt::red << QColor("orange") << Qt::darkYellow << Qt::green
                                           << Qt::blue);

// Ecliptique
static const double tabEcliptique[49][2] = { { 0., 0. }, { 0.5, 3.233 }, { 1., 6.4 }, { 1.5, 9.417 },  { 2., 12.217 }, { 2.5, 14.783 }, { 3., 17. },
                                             { 3.5, 18.983 }, { 4., 20.567 }, { 4.5, 21.817 }, { 5., 22.75 }, { 5.5, 23.25 }, { 6., 23.433 },
                                             { 6.5, 23.25 }, { 7., 22.75 }, { 7.5, 21.817 }, { 8., 20.567 }, { 8.5, 18.983 }, { 9., 17. },
                                             { 9.5, 14.783 }, { 10., 12.217 }, { 10.5, 9.417 }, { 11., 6.4 }, { 11.5, 3.233 }, { 12., 0. },
                                             { 12.5, -3.233 }, { 13., -6.4 }, { 13.5, -9.417 }, { 14., -12.217 }, { 14.5, -14.783 }, { 15., -17. },
                                             { 15.5, -18.983 }, { 16., -20.567 }, { 16.5, -21.817 }, { 17., -22.75 }, { 17.5, -23.25 }, { 18., -23.433 },
                                             { 18.5, -23.25 }, { 19., -22.75 }, { 19.5, -21.817 }, { 20., -20.567 }, { 20.5, -18.983 }, { 21., -17. },
                                             { 21.5, -14.783 }, { 22., -12.217 }, { 22.5, -9.417 }, { 23., -6.4 }, { 23.5, -3.233 }, { 24., 0. } };


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Ciel::Ciel(Onglets *onglets, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Ciel)
{
    ui->setupUi(this);
    scene = nullptr;
    _onglets = onglets;
    _fenetreMax = false;

    ui->vueCiel->installEventFilter(this);
    ui->vueCiel->viewport()->installEventFilter(this);
}

/*
 * Destructeur
 */
Ciel::~Ciel()
{
    if (scene != nullptr) {
        delete scene;
        scene = nullptr;
    }

    delete ui;
}


/*
 * Accesseurs
 */
bool Ciel::fenetreMax() const
{
    return _fenetreMax;
}

/*
 * Modificateurs
 */
void Ciel::setFenetreMax(bool f)
{
    _fenetreMax = f;
}


/*
 * Methodes publiques
 */
/*
 * Affichage du ciel
 */
void Ciel::show(const Observateur &observateur,
                const Soleil &soleil,
                const Lune &lune,
                const QList<LigneConstellation> &lignesCst,
                const QList<Constellation> &constellations,
                const QList<Etoile> &etoiles,
                const QList<Planete> &planetes,
                const QList<Satellite> &satellites,
                const bool fenetreMax,
                const bool maxFlash,
                const bool labelHeure,
                const Date &dateDeb,
                const Date &dateMax,
                const Date &dateFin)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _labelHeure = labelHeure;
    _satellites = satellites;
    _planetes = planetes;
    _observateur = observateur;
    _soleil = soleil;
    _lune = lune;

    const QColor crimson(220, 20, 60);
    const QColor bleuClair(173, 216, 230);

    QPen noir(Qt::black);
    noir.setCosmetic(true);

    QPen crayon(Qt::white);
    crayon.setCosmetic(true);
    QWidget::show();

    // Determination de la couleur du ciel
    const double hts = soleil.hauteur() * RAD2DEG;
    const QBrush couleurCiel = CalculCouleurCiel(hts);

    if (scene != nullptr) {
        scene->deleteLater();
    }

    scene = new QGraphicsScene;
    scene->setBackgroundBrush(QBrush(palette().window().color()));

    /* Corps de la methode */
    // Affichage de la carte du ciel
    QRect rectangle(2, 2, ui->vueCiel->width() - 4, ui->vueCiel->height() - 4);
    scene->setSceneRect(rectangle);

    QPen pen(couleurCiel, Qt::SolidPattern);
    pen.setCosmetic(true);

    scene->addEllipse(rectangle, pen, couleurCiel);
    const int lciel = qRound(0.5 * ui->vueCiel->width());
    const int hciel = qRound(0.5 * ui->vueCiel->height());

    // Affichage des constellations
    if (_onglets->ui()->affconst->checkState() != Qt::Unchecked) {

        QListIterator<LigneConstellation> it(lignesCst);
        while (it.hasNext()) {

            const LigneConstellation lig = it.next();
            if (lig.isDessin()) {

                // Calcul des coordonnees radar des etoiles pour le dessin de la constellation
                const int lstr1 = qRound(lciel - lciel * (1. - lig.etoile1().hauteur() * DEUX_SUR_PI) * sin(lig.etoile1().azimut()));
                const int bstr1 = qRound(hciel - hciel * (1. - lig.etoile1().hauteur() * DEUX_SUR_PI) * cos(lig.etoile1().azimut()));

                const int lstr2 = qRound(lciel - lciel * (1. - lig.etoile2().hauteur() * DEUX_SUR_PI) * sin(lig.etoile2().azimut()));
                const int bstr2 = qRound(hciel - hciel * (1. - lig.etoile2().hauteur() * DEUX_SUR_PI) * cos(lig.etoile2().azimut()));

                if (soleil.hauteur() > -0.08) {
                    crayon = bleuClair;
                } else if (soleil.hauteur() > -0.12) {
                    crayon = QColor("deepskyblue");
                } else {
                    crayon = QColor(Qt::cyan);
                }
                crayon.setCosmetic(true);

                if (((lstr2 - lstr1) * (lstr2 - lstr1) + (bstr2 - bstr1) * (bstr2 - bstr1)) < (lciel * height())) {
                    scene->addLine(lstr1, bstr1, lstr2, bstr2, crayon);
                }
            }
        }

        // Affichage du nom des constellations
        if (_onglets->ui()->affconst->checkState() == Qt::Checked) {

            if (Configuration::instance()->isCarteMaximisee() || fenetreMax) {

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
                        const int lng = static_cast<int> (txtCst->boundingRect().width());

                        const int xncst = (sqrt((lst + lng) * (lst + lng) + bst * bst) > lciel) ? lcst - lng - 1 : lcst + 1;
                        const int yncst = (bcst + 9 > ui->vueCiel->height()) ? bcst - 10 : bcst + 1;

                        txtCst->setBrush(QBrush(Qt::darkYellow));
                        txtCst->setPos(xncst, yncst);
                        txtCst->setFont(QFont(font().family(), 8));
                        scene->addItem(txtCst);
                    }
                }
            }
        }
    }

    // Affichage des etoiles
    const QBrush couleurEtoiles = (soleil.hauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);
    QGraphicsSimpleTextItem * txtStr;
    QListIterator<Etoile> it1(etoiles);
    while (it1.hasNext()) {

        const Etoile etoile = it1.next();
        if (etoile.isVisible() && (etoile.magnitude() <= _onglets->ui()->magnitudeEtoiles->value())) {

            const int lstr = qRound(lciel - lciel * (1. - etoile.hauteur() * DEUX_SUR_PI) * sin(etoile.azimut()));
            const int bstr = qRound(hciel - hciel * (1. - etoile.hauteur() * DEUX_SUR_PI) * cos(etoile.azimut()));

            if (etoile.magnitude() <= 3.) {
                rectangle = QRect(lstr-1, bstr-1, 2, 3);
                scene->addEllipse(rectangle, QPen(Qt::NoPen), couleurEtoiles);
            } else {
                const QLine lin(lstr, bstr, lstr, bstr);
                scene->addLine(lin, couleurEtoiles.color());
            }

            // Nom des etoiles les plus brillantes
            if (_onglets->ui()->affetoiles->isChecked()) {

                if (Configuration::instance()->isCarteMaximisee() || fenetreMax) {

                    if (!etoile.nom().isEmpty() && etoile.nom().at(0).isUpper()) {
                        if (etoile.magnitude() < (_onglets->ui()->magnitudeEtoiles->value() - 1.9)) {

                            const int lst = lstr - lciel;
                            const int bst = hciel - bstr;
                            const QString nomstr = etoile.nom().mid(0, 1) + etoile.nom().mid(1).toLower();

                            txtStr = new QGraphicsSimpleTextItem(nomstr);
                            const int lng = static_cast<int> (txtStr->boundingRect().width());

                            const int xnstr = (sqrt((lst + lng) * (lst + lng) + bst * bst) > lciel) ? lstr - lng - 1 : lstr + 1;
                            const int ynstr = (bstr + 9 > ui->vueCiel->height()) ? bstr - 10 : bstr + 1;

                            txtStr->setBrush(couleurEtoiles);
                            txtStr->setPos(xnstr, ynstr);
                            txtStr->setFont(font());
                            txtStr->setScale(0.9);
                            scene->addItem(txtStr);
                        }
                    }
                }
            }
        }
    }

    // Affichage des planetes
    if (_onglets->ui()->affplanetes->checkState() != Qt::Unchecked) {

        // Calcul des coordonnees radar des planetes
        QGraphicsSimpleTextItem * txtPla;
        for(int iplanete=MERCURE; iplanete<=NEPTUNE; iplanete++) {

            if (_planetes.at(iplanete).hauteur() >= 0.) {

                if ((((iplanete == MERCURE) || (iplanete == VENUS)) && (_planetes.at(iplanete).distance() > soleil.distance())) || (iplanete >= MARS)) {

                    const int lpla = qRound(lciel - lciel * (1. - _planetes.at(iplanete).hauteur() * DEUX_SUR_PI)
                                            * sin(_planetes.at(iplanete).azimut()));
                    const int bpla = qRound(hciel - hciel * (1. - _planetes.at(iplanete).hauteur() * DEUX_SUR_PI)
                                            * cos(_planetes.at(iplanete).azimut()));

                    const QBrush coulPlanete(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                    rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                    scene->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), coulPlanete);

                    if ((Configuration::instance()->isCarteMaximisee() || fenetreMax) && (_onglets->ui()->affplanetes->checkState() == Qt::Checked)) {

                        const int lpl = lpla - lciel;
                        const int bpl = hciel - bpla;
                        const QString nompla = _planetes.at(iplanete).nom();
                        txtPla = new QGraphicsSimpleTextItem(nompla);
                        const int lng = static_cast<int> (txtPla->boundingRect().width());

                        const int xnpla = (sqrt((lpl + lng) * (lpl + lng) + bpl * bpl) > lciel) ? lpla - lng - 1 : lpla + 3;
                        const int ynpla = (bpla + 9 > ui->vueCiel->height()) ? bpla - 10 : bpla + 2;

                        txtPla->setBrush(coulPlanete);
                        txtPla->setPos(xnpla, ynpla);
                        txtPla->setFont(font());
                        txtPla->setScale(0.9);
                        scene->addItem(txtPla);
                    }
                }
            }
        }
    }

    // Affichage du Soleil
    if (_onglets->ui()->affsoleil->isChecked()) {

        // Dessin de l'ecliptique
        if (Configuration::instance()->isCarteMaximisee() || fenetreMax) {

            const double ad1 = tabEcliptique[0][0] * HEUR2RAD;
            const double de1 = tabEcliptique[0][1] * DEG2RAD;
            const double cd1 = cos(de1);
            const Vecteur3D vec(cos(ad1) * cd1, sin(ad1) * cd1, sin(de1));
            const Vecteur3D vec1 = observateur.rotHz() * vec;

            double ht1 = asin(vec1.z());
            double az1 = atan2(vec1.y(), -vec1.x());
            if (az1 < 0.) {
                az1 += DEUX_PI;
            }

            int lecl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
            int becl1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

            double az2;

            for(int i=1; i<49; i++) {

                const double ad2 = tabEcliptique[i][0] * HEUR2RAD;
                const double de2 = tabEcliptique[i][1] * DEG2RAD;
                const double cd2 = cos(de2);
                const Vecteur3D vec0(cos(ad2) * cd2, sin(ad2) * cd2, sin(de2));
                const Vecteur3D vec2 = observateur.rotHz() * vec0;

                const double ht2 = asin(vec2.z());

                az2 = atan2(vec2.y(), -vec2.x());
                if (az2 < 0.) {
                    az2 += DEUX_PI;
                }

                const int lecl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                const int becl2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                if ((ht1 >= 0.) || (ht2 >= 0.)) {
                    scene->addLine(lecl1, becl1, lecl2, becl2, QPen(Qt::darkYellow));
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

            QPixmap pixsol;
            pixsol.load(":/resources/soleil.png");
            pixsol = pixsol.scaled(17, 17);

            QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
            QTransform transform;
            transform.translate(lsol, bsol);
            transform.translate(-7, -7);
            sol->setTransform(transform);
        }
    }

    // Affichage des planetes
    if (_onglets->ui()->affplanetes->checkState() != Qt::Unchecked) {

        // Calcul des coordonnees radar des planetes Mercure et Venus
        QGraphicsSimpleTextItem * txtPla;
        for(int iplanete=MERCURE; iplanete<=VENUS; iplanete++) {

            if (_planetes.at(iplanete).hauteur() >= 0.) {

                if (_planetes.at(iplanete).distance() < soleil.distance()) {

                    const int lpla = qRound(lciel - lciel * (1. - _planetes.at(iplanete).hauteur() * DEUX_SUR_PI)
                                            * sin(_planetes.at(iplanete).azimut()));
                    const int bpla = qRound(hciel - hciel * (1. - _planetes.at(iplanete).hauteur() * DEUX_SUR_PI)
                                            * cos(_planetes.at(iplanete).azimut()));

                    const QBrush coulPlanete(QBrush(couleurPlanetes[iplanete], Qt::SolidPattern));
                    rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                    scene->addEllipse(rectangle, QPen(couleurPlanetes[iplanete]), coulPlanete);

                    if ((Configuration::instance()->isCarteMaximisee() || fenetreMax) && (_onglets->ui()->affplanetes->checkState() == Qt::Checked)) {

                        const int lpl = lpla - lciel;
                        const int bpl = hciel - bpla;
                        const QString nompla = _planetes.at(iplanete).nom();
                        txtPla = new QGraphicsSimpleTextItem(nompla);
                        const int lng = static_cast<int> (txtPla->boundingRect().width());

                        const int xnpla = (sqrt((lpl + lng) * (lpl + lng) + bpl * bpl) > lciel) ? lpla - lng - 1 : lpla + 3;
                        const int ynpla = (bpla + 9 > ui->vueCiel->height()) ? bpla - 10 : bpla + 2;

                        txtPla->setBrush(coulPlanete);
                        txtPla->setPos(xnpla, ynpla);
                        txtPla->setFont(font());
                        txtPla->setScale(0.9);
                        scene->addItem(txtPla);
                    }
                }
            }
        }
    }

    // Affichage de la Lune
    if (_onglets->ui()->afflune->isChecked() && lune.isVisible()) {

        // Calcul des coordonnees radar de la Lune
        const int llun = qRound(lciel - lciel * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
        const int blun = qRound(hciel - hciel * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

        const int lpol = lciel;
        const int bpol = qRound(hciel - hciel * (1. - observateur.latitude() * DEUX_SUR_PI));

        QPixmap pixlun;
        pixlun.load(":/resources/lune.png");
        pixlun = pixlun.scaled(17, 17);

        // Dessin de la Lune et rotations
        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun, blun);
        transform.rotate(180. - QLineF(llun, blun, lpol, bpol).normalVector().angle());
        if (_onglets->ui()->rotationLune->isChecked() && (observateur.latitude() < 0.)) {
            transform.rotate(180.);
        }
        transform.translate(-7, -7);
        lun->setTransform(transform);

        // Dessin de la phase
        if (_onglets->ui()->affphaselune->isChecked()) {

            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 160));
            QPen stylo(Qt::NoBrush, 0);
            stylo.setCosmetic(true);
            const QPolygonF poly = AffichagePhaseLune(lune, 9);

            QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);
            omb->setTransform(transform);
        }
    }

    // Affichage des satellites
    bool aecr;
    bool adeb = false;
    bool amax = false;
    int min;
    int lsat0;
    int bsat0;
    int lsat1;
    int bsat1;
    int lsat3 = 0;
    int bsat3 = 0;
    QColor couleur;

    for(int isat=satellites.size()-1; isat>=0; isat--) {

        if (satellites.at(isat).isVisible() && (satellites.at(isat).altitude() >= 0.)) {

            // Affichage de la trace dans le ciel
            const QList<ElementsTraceCiel> trace = satellites.at(isat).traceCiel();
            if (_onglets->ui()->afftraceCiel->isChecked() && !trace.isEmpty()) {

                const double ht1 = trace.at(0).hauteur;
                const double az1 = trace.at(0).azimut;
                lsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                bsat1 = qRound(lciel - lciel * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                aecr = false;
                adeb = false;
                min = -1;
                lsat0 = 10000;
                bsat0 = 10000;
                lsat3 = 0;
                bsat3 = 0;
                QLineF lig2;
                QString sdate;
                QString nomFlash;
                QGraphicsSimpleTextItem * txtTrace;

                for(int i=1; i<trace.size(); i++) {

                    const double ht2 = trace.at(i).hauteur;
                    const double az2 = trace.at(i).azimut;

                    if (trace.at(i).eclipseTotale) {
                        crayon = crimson;

                    } else if (trace.at(i).eclipsePartielle) {
                        crayon = QPen(Qt::green);

                    } else {

                        const double hauteurSoleil = soleil.hauteur();
                        if (hauteurSoleil > -0.08) {
                            crayon = bleuClair;

                        } else if (hauteurSoleil > -0.12) {
                            crayon = QPen(QColor("deepskyblue"));

                        } else {
                            crayon = QPen(QColor("cyan"));
                        }
                    }
                    crayon.setCosmetic(true);

                    const int lsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                    const int bsat2 = qRound(lciel - lciel * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                    const QLineF lig(lsat2, bsat2, lsat1, bsat1);

                    if (labelHeure) {

                        // Determination des dates a afficher sur la carte du ciel
                        const Date dateTrace(trace.at(i-1).jourJulienUTC, dateDeb.offsetUTC());
                        const double norm = sqrt((lsat1 - lsat0) * (lsat1 - lsat0) + (bsat1 - bsat0) * (bsat1 - bsat0));

                        if ((dateTrace.minutes() != min) && (norm > 12.)) {
                            aecr = true;
                            min = dateTrace.minutes();
                            lsat3 = (i == 1) ? lsat1 : lsat0;
                            bsat3 = (i == 1) ? bsat1 : bsat0;
                            lsat0 = lsat1;
                            bsat0 = bsat1;
                        }

                        if (maxFlash) {

                            if ((dateTrace.jourJulienUTC() > dateMax.jourJulienUTC()) && !adeb) {
                                adeb = true;
                                amax = true;
                                aecr = true;
                            }

                            if (dateTrace.jourJulienUTC() >= dateDeb.jourJulienUTC() &&
                                    dateTrace.jourJulienUTC() <= dateFin.jourJulienUTC()) {
                                crayon = QPen(crayon.color(), 4);
                                crayon.setCosmetic(true);
                            }
                        }

                        // Affichage de l'heure
                        if (aecr) {

                            aecr = false;

                            // Dessin d'une petite ligne correspondant a la date
                            lig2 = QLineF(lsat2, bsat2, lsat3, bsat3).normalVector();
                            lig2.setLength(4);
                            scene->addLine(lig2, QPen(couleurEtoiles, 1.));

                            sdate = "";
                            if (amax) {
                                amax = false;

                                if (maxFlash) {
                                    nomFlash = satellites.at(0).tle().nom().section(QRegExp("[ -]"), 0, 0).toLower();
                                    nomFlash[0] = nomFlash[0].toUpper();
                                    nomFlash[3] = nomFlash[3].toUpper();
                                    sdate = tr("Flash %1").arg(nomFlash);
                                }
                            } else {
                                const DateSysteme sys = SYSTEME_24H;/*(cond.syst()) ? SYSTEME_24H : SYSTEME_12H;*/ // TODO
                                sdate = dateTrace.ToShortDate(FORMAT_COURT, sys).split(" ", Qt::SkipEmptyParts).last();
                                sdate = (sys == SYSTEME_12H) ? sdate.mid(0, 5) + sdate.right(1) : sdate.mid(0, 5);
                            }

                            if (!sdate.isEmpty()) {

                                txtTrace = new QGraphicsSimpleTextItem(sdate);
                                txtTrace->setBrush(couleurEtoiles);

                                const double ang = -lig2.angle();
                                const double ca = cos(ang * DEG2RAD);
                                const double sa = sin(ang * DEG2RAD);
                                const double xnc = lsat2 + 6. * ca + 11. * sa;
                                const double ync = bsat2 + 6. * sa - 11. * ca;

                                txtTrace->setPos(xnc, ync);
                                txtTrace->setRotation(ang);
                                scene->addItem(txtTrace);
                            }
                        }
                    }

                    scene->addLine(lig, crayon);

                    lsat1 = lsat2;
                    bsat1 = bsat2;
                }
            }

            if (!labelHeure) {

                // Calcul des coordonnees radar du satellite
                const int lsat = qRound(lciel - lciel * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) * sin(satellites.at(isat).azimut()));
                const int bsat = qRound(hciel - hciel * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) * cos(satellites.at(isat).azimut()));

                rectangle = QRect(lsat - 3, bsat - 3, 6, 6);

                if (satellites.at(isat).conditionEclipse().eclipseTotale()) {
                    couleur = crimson;
                } else if (satellites.at(isat).conditionEclipse().eclipsePartielle() || satellites.at(isat).conditionEclipse().eclipseAnnulaire()) {
                    couleur = Qt::green;
                } else {
                    couleur = Qt::yellow;
                }
                scene->addEllipse(rectangle, noir, QBrush(couleur, Qt::SolidPattern));
            }
        }
    }

    scene->addEllipse(-20, -20, ui->vueCiel->width() + 40, ui->vueCiel->height() + 40, QPen(QBrush(palette().window().color()), 44));
    scene->addEllipse(1, 1, ui->vueCiel->width() - 3, ui->vueCiel->height() - 3, QPen(QBrush(Qt::gray), 3));

    ui->vueCiel->setScene(scene);

    /* Retour */
    return;
}

/*
 * Affichage de la phase lunaire
 */
QPolygonF Ciel::AffichagePhaseLune(const Lune &lune, const int dimensionPx)
{
    /* Declarations des variables locales */
    QVector<QPointF> pt;

    /* Initialisations */
    const int rayonX = dimensionPx;
    const int rayonY = static_cast<int> (-cos(lune.anglePhase()) * rayonX);
    const int sph = (lune.luneCroissante()) ? -1 : 1;
    double ang = PI_SUR_DEUX;

    /* Corps de la methode */
    for(int i=0; i<36; i++) {

        const double x = sph * ((i < 19) ? rayonY : rayonX) * cos(ang) + dimensionPx - 1;
        const double y = rayonX * sin(ang) + 8;

        pt.append(QPointF(x, y));
        ang += 10. * DEG2RAD;
    }

    const QPolygonF poly(pt);

    /* Retour */
    return poly;
}

/*
 * Determination de la couleur du ciel
 */
QBrush Ciel::CalculCouleurCiel(const double hauteurSoleil)
{
    /* Declarations des variables locales */
    QBrush couleurCiel;

    /* Initialisations */

    /* Corps de la methode */
    if (hauteurSoleil >= 0.) {
        // Jour
        couleurCiel = QBrush(QColor::fromRgb(213, 255, 254));

    } else {

        const int red = static_cast<int> (213.15126 / (1. + 0.0018199 * exp(-0.983684 * hauteurSoleil)) + 0.041477);
        const int green = static_cast<int> (qMax(qMin(256.928983 / (1. + 0.008251 * exp(-0.531535 * hauteurSoleil)) - 0.927648, 255.), 0.));

        // Algorithme special pour le bleu
        int blue;
        if (hauteurSoleil >= -6.) {
            blue = 254;
        } else if (hauteurSoleil >= -12.) {
            blue = static_cast<int> (-2.74359 * hauteurSoleil * hauteurSoleil - 31.551282 * hauteurSoleil + 163.461538);
        } else {
            blue = static_cast<int> (qMax(273.1116 / (1. + 0.0281866 * exp(-0.282853 * hauteurSoleil)) - 1.46635, 0.));
        }
        couleurCiel = QBrush(QColor::fromRgb(red, green, blue));
    }

    /* Retour */
    return couleurCiel;
}

void Ciel::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt)

    if (!Configuration::instance()->isCarteMonde() || _fenetreMax) {

        ui->vueCiel->setGeometry(ui->vueCiel->x(), ui->vueCiel->y(), parentWidget()->height()-32, parentWidget()->height()-32);
        setGeometry((parentWidget()->width() - width() - ui->est->width()) / 2, 0, ui->vueCiel->width() + 2 * ui->est->width(),
                    parentWidget()->height());

        ui->est->setGeometry(ui->est->x(), ui->est->y(), ui->est->width(), ui->vueCiel->height());
        ui->ouest->setGeometry(ui->est->width() + ui->vueCiel->width(), ui->ouest->y(), ui->ouest->width(), ui->vueCiel->height());
        ui->nord->setGeometry(ui->nord->x(), ui->nord->y(), 2 * ui->est->width() + ui->vueCiel->width(), ui->nord->height());
        ui->sud->setGeometry(ui->sud->x(), ui->nord->height() + ui->vueCiel->height(), ui->nord->width(), ui->sud->height());

        show(Configuration::instance()->observateur(),
             Configuration::instance()->soleil(),
             Configuration::instance()->lune(),
             Configuration::instance()->lignesCst(),
             Configuration::instance()->constellations(),
             Configuration::instance()->etoiles(),
             Configuration::instance()->planetes(),
             Configuration::instance()->listeSatellites(),
             _fenetreMax);
    }
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
bool Ciel::eventFilter(QObject *watched, QEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(watched)

    /* Corps de la methode */
    if (ui->vueCiel->underMouse()) {

        const QMouseEvent* const evt = static_cast<const QMouseEvent*>(event);

        const int lciel = qRound(0.5 * ui->vueCiel->width());
        const int hciel = qRound(0.5 * ui->vueCiel->height());
        const int x1 = evt->x() - lciel;
        const int y1 = evt->y() - hciel;

        if ((event->type() == QEvent::MouseMove) || (event->type() == QEvent::HoverEnter)) {

            if (!_labelHeure) {

                // Le curseur est au-dessus de la carte du ciel
                if ((x1 * x1 + y1 * y1) <= (hciel * lciel)) {

                    const double x2 = -x1 / (double) lciel;
                    const double y2 = -y1 / (double) hciel;

                    const double ht = PI_SUR_DEUX * (1. - sqrt(x2 * x2 + y2 * y2));
                    double az = atan2(x2, y2);
                    if (az < 0.) {
                        az += DEUX_PI;
                    }

                    const double ch = cos(ht);
                    const Vecteur3D vec1(-cos(az) * ch, sin(az) * ch, sin(ht));
                    const Vecteur3D vec2(_observateur.rotHz().Transposee() * vec1);

                    // Declinaison
                    const double dec = asin(vec2.z());

                    // Ascension droite
                    double ad = atan2(vec2.y(), vec2.x());
                    if (ad < 0.) {
                        ad += DEUX_PI;
                    }

                    // Affichage des coordonnees dans la barre de statut
                    emit AfficherMessageStatut2(tr("Ascension droite : %1").arg(Maths::ToSexagesimal(ad, HEURE1, 2, 0, false, false).mid(0, 7)));
                    emit AfficherMessageStatut3(tr("Déclinaison : %1").arg(Maths::ToSexagesimal(dec, DEGRE, 2, 0, true, false).mid(0, 7)));

                    // Survol d'un satellite avec le curseur
                    bool atrouve = false;
                    QListIterator<Satellite> it(_satellites);
                    while (it.hasNext() && !atrouve) {

                        const Satellite sat = it.next();
                        const int lsat = qRound(-0.5 * ui->vueCiel->width() * (1. - sat.hauteur() * DEUX_SUR_PI) * sin(sat.azimut()));
                        const int bsat = qRound(-0.5 * ui->vueCiel->height() * (1. - sat.hauteur() * DEUX_SUR_PI) * cos(sat.azimut()));

                        // Distance au carre du satellite au curseur
                        const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                        // Le curseur est au(dessus d'un satellite
                        if ((dt <= 16) && (sat.altitude() > 0.)) {
                            atrouve = true;
                            setToolTip(tr("<font color='blue'><b>%1</b></font><br />NORAD : <b>%2</b><br />COSPAR : <b>%3</b>")
                                       .arg(sat.tle().nom()).arg(sat.tle().norad()).arg(sat.tle().cospar()));

                            emit AfficherMessageStatut(tr("<b>%1</b> (numéro NORAD : <b>%2</b>  -  COSPAR : <b>%3</b>)")
                                                       .arg(sat.tle().nom()).arg(sat.tle().norad()).arg(sat.tle().cospar()));
                            setCursor(Qt::CrossCursor);
                        } else {

                            emit EffacerMessageStatut();
                            setToolTip("");
                            setCursor(Qt::ArrowCursor);
                        }
                    }

                    // Survol des planetes avec le curseur
                    if (_onglets->ui()->affplanetes->checkState() != Qt::Unchecked) {

                        bool atrouve2 = false;
                        static bool aplanete = false;
                        for(int ipla=MERCURE; ipla<=NEPTUNE && !atrouve2; ipla++) {

                            const int lpla = qRound(-0.5 * ui->vueCiel->width() * (1. - _planetes.at(ipla).hauteur() * DEUX_SUR_PI) *
                                                    sin(_planetes.at(ipla).azimut()));
                            const int bpla = qRound(-0.5 * ui->vueCiel->height() * (1. - _planetes.at(ipla).hauteur() * DEUX_SUR_PI) *
                                                    cos(_planetes.at(ipla).azimut()));

                            // Distance au carre de la planete au curseur
                            const int dt = (x1 - lpla) * (x1 - lpla) + (y1 - bpla) * (y1 - bpla);

                            // Le curseur est au-dessus d'une planete
                            if (dt <= 16) {
                                aplanete = true;
                                atrouve2 = true;
                                emit AfficherMessageStatut(_planetes.at(ipla).nom());
                                setToolTip(_planetes.at(ipla).nom());
                                setCursor(Qt::CrossCursor);
                            } else {
                                if (aplanete) {
                                    emit EffacerMessageStatut();
                                    setToolTip("");
                                    setCursor(Qt::ArrowCursor);
                                    aplanete = false;
                                }
                            }
                        }
                    }

                    // Survol du Soleil avec le curseur
                    if (_onglets->ui()->affsoleil->isChecked()) {

                        static bool asoleil = false;
                        const int lsol = qRound(-0.5 * ui->vueCiel->width() * (1. - _soleil.hauteur() * DEUX_SUR_PI) * sin(_soleil.azimut()));
                        const int bsol = qRound(-0.5 * ui->vueCiel->height() * (1. - _soleil.hauteur() * DEUX_SUR_PI) * cos(_soleil.azimut()));

                        // Distance au carre du Soleil au curseur
                        const int dt = (x1 - lsol) * (x1 - lsol) + (y1 - bsol) * (y1 - bsol);

                        // Le curseur est au-dessus du Soleil
                        if (dt <= 81) {
                            emit AfficherMessageStatut(tr("Soleil"));
                            setToolTip(tr("Soleil"));
                            setCursor(Qt::CrossCursor);
                            asoleil = true;
                        } else {
                            if (asoleil) {
                                emit EffacerMessageStatut();
                                setToolTip("");
                                setCursor(Qt::ArrowCursor);
                                asoleil = false;
                            }
                        }
                    }

                    // Survol de la Lune avec le curseur
                    if (_onglets->ui()->afflune->isChecked()) {

                        static bool alune = false;
                        const int llun = qRound(-0.5 * ui->vueCiel->width() * (1. - _lune.hauteur() * DEUX_SUR_PI) * sin(_lune.azimut()));
                        const int blun = qRound(-0.5 * ui->vueCiel->height() * (1. - _lune.hauteur() * DEUX_SUR_PI) * cos(_lune.azimut()));

                        // Distance au carre de la Lune au curseur
                        const int dt = (x1 - llun) * (x1 - llun) + (y1 - blun) * (y1 - blun);

                        // Le curseur est au-dessus de la Lune
                        if (dt <= 81) {
                            emit AfficherMessageStatut(tr("Lune"));
                            setToolTip(tr("Lune"));
                            setCursor(Qt::CrossCursor);
                            alune = true;
                        } else {
                            if (alune) {
                                emit EffacerMessageStatut();
                                setToolTip("");
                                setCursor(Qt::ArrowCursor);
                                alune = false;
                            }
                        }
                    }
                }
            }
        } else if ((event->type() == QEvent::MouseButtonPress)) {

            if (evt->button() == Qt::LeftButton) {

                // Clic sur un satellite
                QListIterator<Satellite> it(Configuration::instance()->listeSatellites());
                bool atrouve = false;
                int idx = 1;
                while (it.hasNext() && !atrouve) {

                    const Satellite sat = it.next();
                    const int lsat = qRound(-0.5 * ui->vueCiel->width() * (1. - sat.hauteur() * DEUX_SUR_PI) * sin(sat.azimut()));
                    const int bsat = qRound(-0.5 * ui->vueCiel->height() * (1. - sat.hauteur() * DEUX_SUR_PI) * cos(sat.azimut()));

                    // Distance au carre du curseur au satellite
                    const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                    // Le curseur est au(dessus d'un satellite
                    if ((dt <= 16) && (sat.altitude() > 0.)) {

                        atrouve = true;

#if QT_VERSION < 0x050D00
                        Configuration::instance()->listeSatellites().swap(--idx, 0);
#else
                        Configuration::instance()->listeSatellites().swapItemsAt(--idx, 0);
#endif
                        // On definit le satellite choisi comme satellite par defaut
                        Configuration::instance()->tleDefaut().nomsat = sat.tle().nom();
                        Configuration::instance()->tleDefaut().l1 = sat.tle().ligne1();
                        Configuration::instance()->tleDefaut().l2 = sat.tle().ligne2();

                        emit RecalculerPositions();

                        Configuration::instance()->EcritureConfiguration();
                        emit EcritureTleDefautRegistre();
                    }
                    idx++;
                }
            }
        } else if ((event->type() == QEvent::Leave) || (event->type() == QEvent::HoverLeave)) {
            setCursor(Qt::ArrowCursor);
            setToolTip("");
            emit EffacerMessageStatut();
            emit AfficherMessageStatut2("");
            emit AfficherMessageStatut3("");
        }
    }

    /* Retour */
    return QFrame::eventFilter(watched, event);
}
