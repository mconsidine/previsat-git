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
 * >    carte.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 decembre 2019
 *
 * Date de revision
 * >    8 aout 2022
 *
 */

#include "configuration/configuration.h"
#include "carte.h"
#include "ciel.h"
#include "coordiss.h"
#include "onglets.h"
#include <QGraphicsSimpleTextItem>
#include "ui_carte.h"
#include "ui_onglets.h"


// SAA
static const double tabSAA[59][2] = { { -96.5, -29. }, { -95., -24.5 }, { -90., -16. }, { -85., -10. }, { -80., -6. }, { -75., -3.5 }, { -70., 0. },
                                      { -65., 4. }, { -60., 6.5 }, { -55., 8. }, { -50., 9. }, { -45., 10. }, { -40., 11. }, { -35., 12. },
                                      { -30., 13. }, { -25., 12. }, { -20., 9.5 }, { -15., 8. }, { -10., 7. }, { -5., 6. }, { 0., 4. }, { 5., 2. },
                                      { 10., -3. }, { 15., -4. }, { 20., -5. }, { 25., -6. }, { 30., -8. }, { 35., -11.5 }, { 40., -14. },
                                      { 42.5, -17.5 }, { 40., -22. }, { 35., -23.5 }, { 30., -25. }, { 25., -27. }, { 20., -29. }, { 15., -32. },
                                      { 10., -33.5 }, { 5., -35. }, { 0., -36. }, { -5., -37. }, { -10., -38.5 }, { -15., -42.5 }, { -20., -44.5 },
                                      { -25., -46. }, { -30., -47.5 }, { -35., -48.5 }, { -40., -49.5 }, { -45., -49. }, { -50., -48.5 },
                                      { -55., -47.5 }, { -60., -47. }, { -65., -46.5 }, { -70., -45.5 }, { -75., -43.5 }, { -80., -42. },
                                      { -85., -38.5 }, { -90., -36. }, { -95., -33. }, { -96.5, -29. } };

// SAA pour la visualisation Wall Command Center
static const double tabSAA_ISS[16][2] = { { 55.5, -17.3 }, { 47., -17.3 }, { 34.3, -20. }, { 14.5, -28. }, { -16., -31.6 }, { -26.5, -35.5 },
                                          { -28.3, -40.6 }, { -21.6, -45.6 }, { 2.5, -53. }, { 42., -53. }, { 54., -47.6 }, { 62.2, -36. },
                                          { 63.3, -31. }, { 63.3, -24. }, { 60.5, -19.2 }, { 55.5, -17.3 } };

/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Carte::Carte(Onglets *onglets, QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Carte)
{
    _ui->setupUi(this);
    scene = nullptr;
    _onglets = onglets;

    _ui->carte->installEventFilter(this);
    _ui->carte->viewport()->installEventFilter(this);

    DEG2PXHZ = (1. / 0.45);
    DEG2PXVT = (1. / 0.45);
}


/*
 * Destructeur
 */
Carte::~Carte()
{
    if (scene != nullptr) {
        delete scene;
        scene = nullptr;
    }

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Carte *Carte::ui() const
{
    return _ui;
}


/*
 * Methodes publiques
 */
/*
 * Affichage des courbes sur la carte du monde
 */
void Carte::show()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const bool mcc = Configuration::instance()->issLive() && _onglets->ui()->styleWCC->isChecked();
    const QColor crimson(220, 20, 60);
    const QColor bleuClair(173, 216, 230);

    QPen crayon(Qt::white);
    crayon.setCosmetic(true);

    QFont police;
    police.setPointSize(9);

    if (scene != nullptr) {
        scene->deleteLater();
    }

    scene = new QGraphicsScene;
    scene->setSceneRect(rect());

    /* Corps de la methode */
    // Chargement de la carte
    const QString nomMap = (_onglets->ui()->listeMap->currentIndex() == 0) ?
                ":/resources/map.png" :
                Configuration::instance()->dirMap() + QDir::separator() +
                Configuration::instance()->listeFicMap().at(_onglets->ui()->listeMap->currentIndex() - 1);

    QPixmap pixMap;
    pixMap.load(nomMap);
    pixMap = pixMap.scaled(_ui->carte->size());
    scene->addPixmap(pixMap);

    if (!_ui->carte->isHidden()) {

        // Affichage du filtre de la carte en mode vision nocturne
        // TODO

        // Affichage de la grille de coordonnees
        const double largeur = _ui->carte->width();
        const double hauteur = _ui->carte->height();
        const double largeurParalleles = largeur - 24.;
        const double hauteurMeridiens = hauteur - 24.;

        unsigned int maxMeridiens = 12;
        unsigned int maxParalleles = 6;
        double stepMeridiens = 30.;
        double stepParalleles = 30.;

        if (mcc) {
            maxMeridiens *= 2;
            maxParalleles *= 2;
            stepMeridiens /= 2.;
            stepParalleles /= 2.;
        }

        if (_onglets->ui()->affgrille->isChecked()) {

            const bool conditionIss = !Configuration::instance()->listeSatellites().isEmpty() && mcc
                    && (Configuration::instance()->listeSatellites().at(0).tle().norad() == Configuration::instance()->noradStationSpatiale());

            QPen pen = QPen((conditionIss) ? ((_onglets->ui()->coulEquateur->currentIndex() == 0) ? Qt::red : Qt::white) : Qt::white);
            pen.setCosmetic(true);

            QPen pen2(QBrush(Qt::lightGray), 1.);
            pen2.setCosmetic(true);

            // Meridiens
            double ht = hauteurMeridiens;
            for(unsigned int i=1; i<maxMeridiens; i++) {

                const double lon = stepMeridiens * i;
                const double x = DEG2PXHZ * lon;

                if (Configuration::instance()->issLive()) {
                    ht = (((i % 2) == 0) || (i == 11) || (i == 13)) ? hauteurMeridiens : hauteur;
                }

                scene->addLine(x, 0., x, ht, (i == (maxMeridiens / 2)) ? pen : pen2);
            }

            // Paralleles
            double lg = largeurParalleles;
            for(unsigned int i=1; i<maxParalleles; i++) {

                const double lat = stepParalleles * i;
                const double y = DEG2PXVT * lat;

                if (Configuration::instance()->issLive()) {
                    lg = (((i % 2) == 0) || (i == 5) || (i == 7)) ? largeurParalleles : largeur;
                }

                scene->addLine(0., y, lg, y, (i == (maxParalleles / 2)) ? pen : pen2);
            }

            // Tropiques
            pen2.setStyle(Qt::DashDotLine);
            scene->addLine(0, 66.55 * DEG2PXVT, largeur, 66.55 * DEG2PXVT, pen2);
            scene->addLine(0, 113.45 * DEG2PXVT, largeur, 113.45 * DEG2PXVT, pen2);

            // Cercles polaires
            scene->addLine(0, 23.45 * DEG2PXVT, largeur, 23.45 * DEG2PXVT, pen2);
            scene->addLine(0, 156.55 * DEG2PXVT, largeur, 156.55 * DEG2PXVT, pen2);
        }


        // Affichage de la SAA
        if (_onglets->ui()->affSAA->isChecked() && !Configuration::instance()->issLive()) {

            const QBrush alpha = QBrush(QColor::fromRgb(255, 0, 0, 50));
            QVector<QPointF> zoneSAA;
            for(int i=0; i<59; i++) {
                zoneSAA.append(QPointF((180. + tabSAA[i][0]) * DEG2PXHZ, (90. - tabSAA[i][1]) * DEG2PXVT));
            }

            const QPolygonF poly(zoneSAA);
            scene->addPolygon(poly, QPen(Qt::NoBrush, 0), alpha);
        }

        // Affichage du Soleil
        Soleil &soleil = Configuration::instance()->soleil();
        const int lsol = qRound((180. - soleil.longitude() * RAD2DEG) * DEG2PXHZ);
        const int bsol = qRound((90. - soleil.latitude() * RAD2DEG) * DEG2PXVT);

        if (_onglets->ui()->affsoleil->isChecked()) {

            const QString iconeSoleil = (mcc) ? ":/resources/icones/soleil.png" : ":/resources/soleil.png";

            QPixmap pixsol;
            pixsol.load(iconeSoleil);

            QTransform transform;
            transform.translate(lsol, bsol);

            if (mcc) {
                transform.translate(-15, -10);
            } else {
                pixsol = pixsol.scaled(17, 17);
                transform.translate(-7, -7);
            }

            QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
            sol->setTransform(transform);

            if (((lsol + 7) > _ui->carte->width()) || ((lsol - 7) < 0)) {

                QGraphicsPixmapItem * const sol2 = scene->addPixmap(pixsol);
                transform.reset();

                if ((lsol + 7) > _ui->carte->width()) {
                    transform.translate(lsol - _ui->carte->width(), bsol);
                }

                if ((lsol - 7) < 0) {
                    transform.translate(lsol + _ui->carte->width(), bsol);
                }

                transform.translate(-7, -7);
                sol2->setTransform(transform);
            }
        }

        // Zone d'ombre
        if (_onglets->ui()->affnuit->checkState() != Qt::Unchecked) {

            double beta = PI_SUR_DEUX - REFRACTION_HZ;
            const int imax = (_onglets->ui()->affnuit->checkState() == Qt::PartiallyChecked || mcc) ? 1 : 4;

            const QBrush alpha1 = QBrush(QColor::fromRgb(0, 0, 0, static_cast<int> (2.55 * _onglets->ui()->intensiteOmbre->value())));
            const QBrush alpha = (mcc) ? QBrush(QColor::fromRgb(0, 0, 0, qMin(255, 2 * alpha1.color().alpha()))) : alpha1;

            const QPen stylo1 = (_onglets->ui()->coulTerminateur->currentIndex() == 0) ?
                        QPen(QColor::fromRgb(102, 50, 16), 2) : QPen(Qt::darkYellow, 2);
            QPen stylo((mcc) ? stylo1 : QPen(Qt::NoBrush, 0));
            stylo.setCosmetic(true);

            QVector<QPointF> zone;
            QVector<QPointF> zone1;
            QVector<QPointF> zone2;
            QList<int> idxIntersection;

            for(int i=0; i<imax; i++) {

                if (mcc && (i != 0)) {
                    stylo = QPen(Qt::NoBrush, 0);
                    stylo.setCosmetic(true);
                }

                soleil.CalculZoneVisibilite(beta);

                // Coordonnees de la zone d'ombre, en pixels
                zone.clear();
                idxIntersection.clear();
                zone.append(QPointF(soleil.zone().at(0).x() * DEG2PXHZ, soleil.zone().at(0).y() * DEG2PXVT));

                for(int j=1; j<soleil.zone().size(); j++) {

                    zone.append(QPointF(soleil.zone().at(j).x() * DEG2PXHZ, soleil.zone().at(j).y() * DEG2PXVT));

                    if (fabs(zone.at(j).x() - zone.at(j-1).x()) > (_ui->carte->width() / 2)) {
                        idxIntersection.append(j);
                    }
                }

                if ((fabs(soleil.latitude()) > 0.002449 * DEG2RAD) || (i > 0)) {

                    // Cas en dehors des equinoxes (ou pour les crepuscules)
                    switch (idxIntersection.size()) {

                    default:
                    case 0:
                    {
                        // Aucune intersection avec les bords de la carte (la zone d'ombre est patatoide)
                        const QPolygonF poly(zone);
                        scene->addPolygon(poly, stylo, alpha);
                        break;
                    }

                    case 1:
                    {
                        // Une intersection avec les bords de la carte (la zone d'ombre forme une bande)
                        for(int j=0; j<idxIntersection.first(); j++) {
                            zone.move(0, zone.size()-1);
                        }

                        // Ajout de points pour completer la zone d'ombre sur les bords de la carte
                        if (zone.first().x() < zone.last().x()) {

                            zone.insert(0, QPointF(zone.last().x() - _ui->carte->width(), zone.last().y()));

                            if (soleil.latitude() > 0.) {
                                zone.insert(0, QPointF(zone.last().x() - _ui->carte->width(), _ui->carte->height()));
                                zone.insert(0, QPointF(zone.at(2).x() + _ui->carte->width(), _ui->carte->height()));

                            } else {
                                zone.insert(0, QPointF(zone.last().x() - _ui->carte->width(), -1.));
                                zone.insert(0, QPointF(zone.at(2).x() + _ui->carte->width(), -1.));
                            }
                            zone.insert(0, QPointF(zone.at(3).x() + _ui->carte->width(), zone.at(3).y()));

                        } else {

                            zone.insert(0, QPointF(zone.last().x() + _ui->carte->width(), zone.last().y()));

                            if (soleil.latitude() > 0.) {
                                zone.insert(0, QPointF(zone.last().x() + _ui->carte->width(), _ui->carte->height()));
                                zone.insert(0, QPointF(zone.at(2).x() - _ui->carte->width(), _ui->carte->height()));

                            } else {
                                zone.insert(0, QPointF(zone.last().x() + _ui->carte->width(), -1.));
                                zone.insert(0, QPointF(zone.at(2).x() - _ui->carte->width(), -1.));
                            }
                            zone.insert(0, QPointF(zone.at(3).x() - _ui->carte->width(), zone.at(3).y()));
                        }

                        const QPolygonF poly(zone);
                        scene->addPolygon(poly, stylo, alpha);
                        break;
                    }

                    case 2:
                    {
                        // Deux intersections avec les bords de la carte (la zone d'ombre est patatoide et a cheval sur les extremites de la carte)
                        // Partage en 2 zones
                        const int jmed = idxIntersection.first();
                        zone1.clear();
                        zone2.clear();

                        for(int j=0; j<zone.size(); j++) {
                            if ((j < idxIntersection.first()) || (j >= idxIntersection.last())) {
                                zone1.append(zone.at(j));
                            } else {
                                zone2.append(zone.at(j));
                            }
                        }

                        // Ajout de points pour completer la zone d'ombre sur les bords de la carte
                        if (zone1.at(jmed-1).x() > zone2.first().x()) {

                            if (zone1.size() > jmed) {
                                zone1.insert(jmed, QPointF(zone2.last().x() + _ui->carte->width(), zone2.last().y()));
                                zone1.insert(jmed, QPointF(zone2.first().x() + _ui->carte->width(), zone2.first().y()));

                            } else {
                                zone1.append(QPointF(zone2.last().x() + _ui->carte->width(), zone2.last().y()));
                                zone1.append(QPointF(zone2.first().x() + _ui->carte->width(), zone2.first().y()));
                            }

                            zone2.insert(0, QPointF(zone1.at(jmed-1).x() - _ui->carte->width(), zone1.at(jmed-1).y()));
                            zone2.insert(0, QPointF(zone1.at(jmed+2).x() - _ui->carte->width(), zone1.at(jmed+2).y()));

                        } else {

                            if (zone1.size() > jmed) {
                                zone1.insert(jmed, QPointF(zone2.last().x() - _ui->carte->width(), zone2.last().y()));
                                zone1.insert(jmed, QPointF(zone2.first().x() - _ui->carte->width(), zone2.first().y()));

                            } else {
                                zone1.append(QPointF(zone2.last().x() - _ui->carte->width(), zone2.last().y()));
                                zone1.append(QPointF(zone2.first().x() - _ui->carte->width(), zone2.first().y()));
                            }

                            zone2.insert(0, QPointF(zone1.at(jmed-1).x() + _ui->carte->width(), zone1.at(jmed-1).y()));
                            zone2.insert(0, QPointF(zone1.at(jmed+2).x() + _ui->carte->width(), zone1.at(jmed+2).y()));
                        }

                        const QPolygonF poly1(zone1);
                        const QPolygonF poly2(zone2);
                        scene->addPolygon(poly1, stylo, alpha);
                        scene->addPolygon(poly2, stylo, alpha);

                        break;
                    }
                    }

                } else {

                    // Cas des equinoxes (on trace un terminateur vertical)
                    const double x1 = qMin(soleil.zone().at(90).x(), soleil.zone().at(270).x()) * DEG2PXHZ;
                    const double x2 = qMax(soleil.zone().at(90).x(), soleil.zone().at(270).x()) * DEG2PXHZ;

                    if ((lsol > (_ui->carte->width() / 4)) && (lsol < (3 * _ui->carte->width()) / 4)) {

                        zone1.clear();
                        zone1.append(QPointF(-1., -1.));
                        zone1.append(QPointF(x1, -1.));
                        zone1.append(QPointF(x1, _ui->carte->height()));
                        zone1.append(QPointF(-1., _ui->carte->height()));

                        zone2.clear();
                        zone2.append(QPointF(_ui->carte->width(), -1.));
                        zone2.append(QPointF(x2, -1.));
                        zone2.append(QPointF(x2, _ui->carte->height()));
                        zone2.append(QPointF(_ui->carte->width(), _ui->carte->height()));

                        const QPolygonF poly1(zone1);
                        const QPolygonF poly2(zone2);
                        scene->addPolygon(poly1, stylo, alpha);
                        scene->addPolygon(poly2, stylo, alpha);

                    } else {

                        zone1.clear();
                        zone1.append(QPointF(x1, -1.));
                        zone1.append(QPointF(x1, _ui->carte->height()));
                        zone1.append(QPointF(x2, _ui->carte->height()));
                        zone1.append(QPointF(x2, -1.));

                        const QPolygonF poly1(zone1);
                        scene->addPolygon(poly1, stylo, alpha);
                    }
                }

                beta += 6. * DEG2RAD;
            }
        }

        if (_onglets->ui()->affgrille->isChecked()) {

            // Etiquettes de longitude
            QBrush couleur((soleil.latitude() > 0.) ? Qt::lightGray : Qt::gray);
            for(unsigned int i=1; i<maxMeridiens; i++) {

                if (((i % 2) == 0) || !mcc) {

                    const double lon = stepMeridiens * i;
                    const double x = DEG2PXHZ * lon;

                    QGraphicsSimpleTextItem * const txt = new QGraphicsSimpleTextItem(QString("%1").arg(fabs(lon - 180.)));
                    txt->setBrush(couleur);
                    txt->setFont(police);

                    const double dx = txt->boundingRect().width() * 0.5;
                    txt->setPos(x - dx, hauteurMeridiens + 4.);
                    scene->addItem(txt);
                }
            }

            QGraphicsSimpleTextItem * txtl = new QGraphicsSimpleTextItem(tr("W", "Symbol for West"));
            txtl->setBrush(couleur);
            txtl->setFont(police);
            txtl->setPos(DEG2PXHZ * 165. - txtl->boundingRect().width() * 0.5, hauteurMeridiens + 4.);
            scene->addItem(txtl);

            txtl = new QGraphicsSimpleTextItem(tr("E", "Symbol for East"));
            txtl->setBrush(couleur);
            txtl->setFont(police);
            txtl->setPos(DEG2PXHZ * 195. - txtl->boundingRect().width() * 0.5, hauteurMeridiens + 4.);
            scene->addItem(txtl);

            // Etiquettes de latitude
            for(unsigned int i=1; i<maxParalleles; i++) {

                if (((i % 2) == 0) || !mcc) {

                    const double lat = stepParalleles * i;
                    const double y = DEG2PXVT * lat;

                    QGraphicsSimpleTextItem * const txt = new QGraphicsSimpleTextItem(QString("%1").arg(fabs(lat - 90.)));
                    txt->setBrush(Qt::lightGray);
                    txt->setFont(police);

                    const double dy = txt->boundingRect().height() * 0.5 - 0.5;
                    txt->setPos(largeur - 7. - txt->boundingRect().width(), y - dy);
                    scene->addItem(txt);
                }
            }

            txtl = new QGraphicsSimpleTextItem(tr("N", "Symbol for North"));
            txtl->setFont(police);
            txtl->setBrush(Qt::lightGray);
            txtl->setPos(largeur - 7. - txtl->boundingRect().width(), DEG2PXVT * 75. - txtl->boundingRect().height() * 0.5);
            scene->addItem(txtl);

            txtl = new QGraphicsSimpleTextItem(tr("S", "Symbol for South"));
            txtl->setFont(police);
            txtl->setBrush(Qt::lightGray);
            txtl->setPos(largeur - 7. - txtl->boundingRect().width(), DEG2PXVT * 105. - txtl->boundingRect().height() * 0.5);
            scene->addItem(txtl);
        }

        // Affichage de la ZOE et de la SAA pour le Wall Command Center
        if (_onglets->ui()->affSAA_ZOE->isChecked() && Configuration::instance()->issLive() && !Configuration::instance()->listeSatellites().isEmpty()
                && (Configuration::instance()->listeSatellites().at(0).tle().norad() == Configuration::instance()->noradStationSpatiale())) {

            // Zone Of Exclusion (ZOE)
            QGraphicsSimpleTextItem * const txtZOE = new QGraphicsSimpleTextItem("ZOE");
            const double xnZOE = 252. * DEG2PXHZ;
            const double ynZOE = 66. * DEG2PXVT;

            txtZOE->setBrush((_onglets->ui()->coulZOE->currentIndex() == 0) ? Qt::black : Qt::white);
            const QFont policeZOE(_onglets->ui()->policeWCC->currentText(), 14);
            txtZOE->setFont(policeZOE);
            const int htt = (int) txtZOE->boundingRect().height();

            txtZOE->setPos(xnZOE, ynZOE - htt);
            scene->addItem(txtZOE);

            // South Atlantic Anomaly (SAA)
            QGraphicsSimpleTextItem * const txtSAA = new QGraphicsSimpleTextItem("SAA");
            const double xnSAA = 150. * DEG2PXHZ;
            const double ynSAA = 125. * DEG2PXVT;

            txtSAA->setBrush(Qt::white);
            const QFont policeSAA(_onglets->ui()->policeWCC->currentText(), 11);
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
            scene->addPolygon(poly1, QPen(Qt::white, (_onglets->ui()->styleWCC->isChecked()) ? 2 : 1));
        }

        // Lieux d'observation
        QGraphicsSimpleTextItem * txtObs;
        const int nbMax = (_onglets->ui()->affnomlieu->checkState() == Qt::Unchecked) ? 0 : Configuration::instance()->observateurs().size() - 1;
        for(int j=nbMax; j>=0; j--) {

            const int lobs = qRound((180. - Configuration::instance()->observateurs().at(j).longitude() * RAD2DEG) * DEG2PXHZ)+1;
            const int bobs = qRound((90. - Configuration::instance()->observateurs().at(j).latitude() * RAD2DEG) * DEG2PXVT)+1;

            scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
            scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

            if ((j == 0) || (_onglets->ui()->affnomlieu->checkState() == Qt::Checked)) {

                txtObs = new QGraphicsSimpleTextItem(Configuration::instance()->observateurs().at(j).nomlieu());
                const int lng = static_cast<int> (txtObs->boundingRect().width());
                const int xnobs = ((lobs + 7 + lng) > _ui->carte->width()) ? lobs - lng - 1 : lobs + 4;
                const int ynobs = ((bobs + 12) > _ui->carte->height()) ? bobs - 12 : bobs;

                txtObs->setBrush(Qt::white);
                txtObs->setPos(xnobs, ynobs);
                scene->addItem(txtObs);
            }
        }

        // Stations
        if (Configuration::instance()->issLive()) {

            int ils;
            double lsat1;
            double bsat1;
            double lsat2;
            double bsat2;

            crayon = QPen(Qt::yellow);
            crayon.setCosmetic(true);

            QPen crayon2 = (_onglets->ui()->styleWCC->isChecked()) ? QPen(Qt::yellow, 2) : crayon;
            crayon2.setCosmetic(true);

            for(int j=0; j<Configuration::instance()->mapStations().count(); j++) {

                if (_onglets->ui()->listeStations->item(j)->checkState() == Qt::Checked) {

                    const QString acronyme = _onglets->ui()->listeStations->item(j)->data(Qt::UserRole).toString();
                    const Observateur station = Configuration::instance()->mapStations()[acronyme];

                    const int lsta = qRound((180. - station.longitude() * RAD2DEG) * DEG2PXHZ);
                    const int bsta = qRound((90. - station.latitude() * RAD2DEG) * DEG2PXVT);

                    scene->addLine(lsta-4, bsta, lsta+4, bsta, crayon);
                    scene->addLine(lsta, bsta-4, lsta, bsta+4, crayon);

                    QGraphicsSimpleTextItem * const txtSta = new QGraphicsSimpleTextItem(acronyme);
                    const QFont policeSta(_onglets->ui()->policeWCC->currentText(), 10);
                    txtSta->setFont(policeSta);

                    const int lng = (int) txtSta->boundingRect().width();
                    const int xnsta = lsta - lng / 2 + 1;
                    const int ynsta = (bsta > 16) ? bsta - 16 : bsta + 3;

                    txtSta->setBrush(Qt::yellow);
                    txtSta->setPos(xnsta, ynsta);
                    scene->addItem(txtSta);

                    if (_onglets->ui()->affCerclesAcq->isChecked() && !Configuration::instance()->listeSatellites().isEmpty()
                            && (Configuration::instance()->listeSatellites().at(0).altitude() > 0.)
                            && (Configuration::instance()->listeSatellites().at(0).tle().norad() ==
                                Configuration::instance()->noradStationSpatiale())) {

                        Satellite sat = Configuration::instance()->listeSatellites().at(0);
                        sat.CalculCercleAcquisition(station);

                        lsat1 = sat.zone().at(0).x() * DEG2PXHZ;
                        bsat1 = sat.zone().at(0).y() * DEG2PXVT + 1;

                        ils = 99999;
                        for(int k=1; k<361; k++) {

                            lsat2 = sat.zone().at(k).x() * DEG2PXHZ;
                            bsat2 = sat.zone().at(k).y() * DEG2PXVT + 1;

                            if (fabs(lsat2 - lsat1) > (_ui->carte->width() / 2)) {
                                if (lsat2 < lsat1) {
                                    lsat2 += _ui->carte->width();
                                } else {
                                    lsat1 += _ui->carte->width();
                                }
                                ils = k;
                            }

                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon2);

                            if (ils == k) {
                                lsat1 -= _ui->carte->width() + 1;
                                lsat2 -= _ui->carte->width() + 1;
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
        if (_onglets->ui()->afflune->isChecked()) {

            //if (!mcc) {

            QPixmap pixlun;
            pixlun.load(":/resources/lune.png");
            pixlun = pixlun.scaled(17, 17);

            const int llun = qRound((180. - Configuration::instance()->lune().longitude() * RAD2DEG) * DEG2PXHZ);
            const int blun = qRound((90. - Configuration::instance()->lune().latitude() * RAD2DEG) * DEG2PXVT);

            QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
            QTransform transform;
            QTransform transform2;
            transform.translate(llun, blun);
            if (_onglets->ui()->rotationLune->isChecked() && (Configuration::instance()->observateurs().at(0).latitude() < 0.)) {
                transform.rotate(180.);
            }

            transform.translate(-7, -7);
            lun->setTransform(transform);

            if (((llun + 7) > _ui->carte->width()) || ((llun - 7) < 0)) {

                QGraphicsPixmapItem * const lun2 = scene->addPixmap(pixlun);

                if ((llun + 7) > _ui->carte->width()) {
                    transform2.translate(llun - _ui->carte->width(), blun);
                }
                if ((llun - 7) < 0) {
                    transform2.translate(llun + _ui->carte->width(), blun);
                }
                if (_onglets->ui()->rotationLune->isChecked() && (Configuration::instance()->observateurs().at(0).latitude() < 0.)) {
                    transform2.rotate(180.);
                }
                transform2.translate(-7, -7);
                lun2->setTransform(transform2);
            }

            if (_onglets->ui()->affphaselune->isChecked()) {

                const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 160));
                QPen stylo(Qt::NoBrush, 0);
                stylo.setCosmetic(true);
                const QPolygonF poly = Ciel::AffichagePhaseLune(Configuration::instance()->lune(), 9);

                QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);
                omb->setTransform(transform);

                if (!transform2.isIdentity()) {
                    QGraphicsPolygonItem * const omb2 = scene->addPolygon(poly, stylo, alpha);
                    omb2->setTransform(transform2);
                }
            }
        }
    }

    // Affichage de la trace au sol du satellite par defaut
    if (_onglets->ui()->afftraj->isChecked() || mcc) {

        if (Configuration::instance()->listeSatellites().size() > 0) {

            const Satellite &satellite = Configuration::instance()->listeSatellites().at(0);
            if ((satellite.altitude() >= 0.) && (!satellite.traceAuSol().isEmpty())) {

                double lsat1 = satellite.traceAuSol().at(0).longitude * DEG2PXHZ;
                double bsat1 = satellite.traceAuSol().at(0).latitude * DEG2PXVT;


                int nbOrb = 0;
                double lsat2;
                double bsat2;
                QString txt;
                int ils = 99999;
                for(int j=1; j<satellite.traceAuSol().size()-1; j++) {

                    lsat2 = satellite.traceAuSol().at(j).longitude * DEG2PXHZ;
                    bsat2 = satellite.traceAuSol().at(j).latitude * DEG2PXVT;

                    if (fabs(lsat2 - lsat1) > _ui->carte->width() / 2) {
                        if (lsat2 < lsat1) {
                            lsat2 += _ui->carte->width();
                        } else {
                            lsat1 += _ui->carte->width();
                        }
                        ils = j;
                    }

                    const QLineF lig = QLineF(lsat2, bsat2, lsat1, bsat1);

                    if (satellite.traceAuSol().at(j).eclipseTotale) {
                        crayon = crimson;
                    } else if (satellite.traceAuSol().at(j).eclipsePartielle) {
                        crayon = QPen(Qt::green);
                    } else {
                        crayon = bleuClair;
                    }
                    crayon.setCosmetic(true);

                    if (Configuration::instance()->issLive()) {

                        if (satellite.tle().norad() == Configuration::instance()->noradStationSpatiale()) {

                            // Affichage du numero d'orbite
                            if ((satellite.traceAuSol().at(j).latitude < 90.)
                                    && (satellite.traceAuSol().at(j-1).latitude > 90.) && (nbOrb < 3)) {

                                nbOrb++;
                                const Date dateISS(Date(satellite.traceAuSol().at(j+1).jourJulienUTC, 0., false));
                                const int numOrb = CoordISS::CalculNumeroOrbiteISS(dateISS);

                                QGraphicsSimpleTextItem * const txtOrb = new QGraphicsSimpleTextItem(QString::number(numOrb));

                                const QFont policeOrb(Configuration::instance()->police().family(), 10, (_onglets->ui()->styleWCC->isChecked()) ?
                                                          QFont::Bold : QFont::Normal);
                                txtOrb->setFont(policeOrb);
                                txtOrb->setBrush(Qt::white);

                                const int lng = static_cast<int> (txtOrb->boundingRect().width());
                                const double xnorb = (lsat2 - lng < 0) ? lsat2 + _ui->carte->width() - lng - 8 : lsat2 - lng;
                                txtOrb->setPos(xnorb, _ui->carte->height() / 2 - 18);
                                scene->addItem(txtOrb);
                            }

                            if (_onglets->ui()->styleWCC->isChecked()) {

                                crayon = QPen(Qt::white, 2);
                                crayon.setCosmetic(true);

                                // Affichage des crochets des transitions jour/nuit
                                txt = "";
                                if (satellite.traceAuSol().at(j).eclipseTotale && !satellite.traceAuSol().at(j-1).eclipseTotale) {
                                    txt = "[";
                                }

                                if (satellite.traceAuSol().at(j-1).eclipseTotale && !satellite.traceAuSol().at(j).eclipseTotale) {
                                    txt = "]";
                                }

                                if (!txt.isEmpty()) {

                                    const double ang = fmod(180. - lig.angle(), T360);
                                    const double ca = cos(ang * DEG2RAD);
                                    const double sa = sin(ang * DEG2RAD);

#if defined (Q_OS_MAC)
                                    const QFont policeOmb(Configuration::instance()->police().family(), 24);
                                    const double fact = (satellite.traceAuSol().at(j).eclipsePartielle) ? 3. : 1.;
                                    const double xnc = lsat2 - fact * ca + 10. * sa;
                                    const double ync = bsat2 - fact * sa - 10. * ca;
#else
                                    const QFont policeOmb(Configuration::instance()->police().family(), 14);
                                    const double fact = (satellite.traceAuSol().at(j).eclipsePartielle) ? 4. : 2.;
                                    const double xnc = lsat2 - fact * ca + 14. * sa;
                                    const double ync = bsat2 - fact * sa - 13. * ca;
#endif

                                    QGraphicsSimpleTextItem * const txtOmb = new QGraphicsSimpleTextItem(txt);

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
                        lsat2 -= _ui->carte->width();
                        scene->addLine(lig, crayon);
                        ils = 0;
                    }
                    lsat1 = satellite.traceAuSol().at(j).longitude * DEG2PXHZ;
                    bsat1 = satellite.traceAuSol().at(j).latitude * DEG2PXVT;
                }
            }
        }
    }

    // Affichage de la zone de visibilite des satellites
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();

    QList<bool> als;
    for(int i=0; i<satellites.size(); i++) {
        als.append(false);
    }

    if (Configuration::instance()->listeSatellites().size() > 0) {

        if (_onglets->ui()->affvisib->isChecked()/* || mcc*/) {
            const int nbMax2 = (_onglets->ui()->affvisib->checkState() == Qt::PartiallyChecked) ? 1 : satellites.size();

            int ils = 99999;
            double lsat1;
            double bsat1;
            double lsat2;
            double bsat2;

            for(int isat=0; isat<nbMax2; isat++) {

                if (mcc) {

                    crayon = QPen((_onglets->ui()->coulCercleVisibilite->currentIndex() == 0) ? Qt::white : Qt::darkRed, 2);
                    crayon.setCosmetic(true);

                    if (Configuration::instance()->listeSatellites().at(isat).tle().nom().toLower().startsWith("tdrs")) {

                        const int numeroTDRS = Configuration::instance()->listeSatellites().at(isat).tle().nom().section(" ", 1).toInt();

                        QMapIterator<int, SatelliteTDRS> it(Configuration::instance()->mapTDRS());
                        while (it.hasNext()) {
                            it.next();

                            if (it.key() == numeroTDRS) {

                                const SatelliteTDRS satTDRS = it.value();

                                // Affichage du nom du satellite TDRS
                                QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(satTDRS.denomination);
                                const QFont policeSat(_onglets->ui()->policeWCC->currentText(), 11);
                                txtSat->setFont(policeSat);

                                const int lsat =
                                        qRound((180. - Configuration::instance()->listeSatellites().at(isat).longitude() * RAD2DEG) * DEG2PXHZ);
                                const int bsat = qRound((90. - Configuration::instance()->listeSatellites().at(isat).latitude() * RAD2DEG) * DEG2PXVT);

                                crayon = QPen(QColor(satTDRS.rouge, satTDRS.vert, satTDRS.bleu), 2);
                                crayon.setCosmetic(true);

                                txtSat->setBrush(crayon.color());

                                const int lng = static_cast<int> (txtSat->boundingRect().width());
                                const int xnsat = lsat - lng / 2 + 1;
                                txtSat->setPos(xnsat, bsat + 28);
                                scene->addItem(txtSat);
                            }
                        }
                    }
                } else {
                    crayon = QPen(Qt::white);
                    crayon.setCosmetic(true);
                }

                if ((satellites.at(isat).altitude() >= 0.) && (!satellites.at(isat).zone().isEmpty())) {

                    lsat1 = satellites.at(isat).zone().at(0).x() * DEG2PXHZ + 1;
                    bsat1 = satellites.at(isat).zone().at(0).y() * DEG2PXVT + 1;

                    for(int j=1; j<361; j++) {

                        lsat2 = satellites.at(isat).zone().at(j).x() * DEG2PXHZ + 1;
                        bsat2 = satellites.at(isat).zone().at(j).y() * DEG2PXVT + 1;

                        if (fabs(lsat2 - lsat1) > _ui->carte->width() / 2) {
                            if (lsat2 < lsat1) {
                                lsat2 += _ui->carte->width();
                            } else {
                                lsat1 += _ui->carte->width();
                            }
                            ils = j;
                            als[isat] = true;
                        }

                        scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        if (ils == j) {
                            lsat1 -= _ui->carte->width() + 1;
                            lsat2 -= _ui->carte->width() + 1;
                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);
                            ils = 0;
                        }
                        lsat1 = satellites.at(isat).zone().at(j).x() * DEG2PXHZ + 1;
                        bsat1 = satellites.at(isat).zone().at(j).y() * DEG2PXVT + 1;
                    }
                }
            }
        }
    }

    // Dessin des satellites
    double angle;
    QStringList listeIcones;
    QPixmap img;
    QTransform transform;
    QGraphicsPixmapItem * pm;
    QGraphicsPixmapItem * pm2;

    for(int isat=satellites.size()-1; isat>=0; isat--) {

        if (satellites.at(isat).altitude() >= 0.) {

            const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ)+1;
            const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT)+1;

            if (mcc || _onglets->ui()->afficone->isChecked()) {

                // Affichage de l'icone du satellite a partir du numero NORAD ou du nom
                const QString norad = satellites.at(isat).tle().norad();
                const QString nomsat = satellites.at(isat).tle().nom().section(" ", 0, 0);

                const QDir di(Configuration::instance()->dirRsc());
                const QDir di2(":/resources/icones");
                const QStringList filtre(QStringList () << norad + ".png" << nomsat + ".png");

                // L'icone de l'utilisateur est prioritaire sur l'icone par defaut
                listeIcones = di.entryList(filtre, QDir::Files);
                if (listeIcones.isEmpty()) {
                    listeIcones = di2.entryList(filtre, QDir::Files).replaceInStrings(QRegExp("^"), di2.path() + "/");
                } else {
                    listeIcones.replaceInStrings(QRegExp("^"), di.path() + QDir::separator());
                }

                if (listeIcones.isEmpty()) {
                    // L'icone du satellite n'a pas ete trouvee, affichage par defaut
                    AffichageSatellite(satellites.at(isat), lsat, bsat, _ui->carte->width(), _ui->carte->height());
                } else {

                    // Affichage de l'icone satellite
                    img = QPixmap(listeIcones.at(0));
                    img = img.scaled(qMin(_ui->carte->width() / 12, img.width()), qMin(_ui->carte->height() / 6, img.height()));

                    pm = scene->addPixmap(img);

                    transform.reset();
                    transform.translate(lsat, bsat);

                    // Rotation de l'icone de l'ISS
                    angle = 0.;
                    if (_onglets->ui()->rotationIconeISS->isChecked() &&
                            (satellites.at(isat).tle().norad() == Configuration::instance()->noradStationSpatiale())) {

                        const double vxsat = satellites.at(isat).vitesse().x();
                        const double vysat = satellites.at(isat).vitesse().y();
                        const double vzsat = satellites.at(isat).vitesse().z();

                        angle = RAD2DEG * (-atan(vzsat / sqrt(vxsat * vxsat + vysat * vysat)));
                        transform.rotate(angle);
                    }
                    transform.translate(-img.width() / 2, -img.height() / 2);
                    pm->setTransform(transform);

                    // Icone sur le bord de la carte du monde
                    if (als.at(isat)) {
                        pm2 = scene->addPixmap(img);
                        transform.reset();

                        if (lsat > _ui->carte->width() / 2) {
                            transform.translate(lsat - _ui->carte->width(), bsat);
                        } else {
                            transform.translate(lsat + _ui->carte->width(), bsat);
                        }

                        transform.rotate(angle);
                        transform.translate(-img.width() / 2, -img.height() / 2);
                        pm2->setTransform(transform);
                    }
                }
            } else {
                AffichageSatellite(satellites.at(isat), lsat, bsat, _ui->carte->width(), _ui->carte->height());
            }
        }
    }

    _ui->carte->setScene(scene);

    /* Retour */
    return;
}

void Carte::resizeEvent(QResizeEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (_onglets->ui()->proportionsCarte->isChecked()) {

        const int href = 2 * height() - 1;
        const int lref = width();
        const int lc = qMin(href, lref);
        const int hc = (lc + 1) / 2;
        _ui->carte->setGeometry((width() - lc) / 2, 0, lc, hc);
    } else {
        _ui->carte->setGeometry(geometry());
    }

    const int hcarte = _ui->carte->height() - 1;
    const int lcarte = _ui->carte->width() - 1;

    DEG2PXHZ = lcarte * (1. / T360);
    DEG2PXVT = hcarte * (2. / T360);

    show();

    /* Retour */
    return;
}

/*
 * Affichage de l'info bulle du site de lancement
 */
void Carte::AffichageSiteLancement(const QString &acronyme, const Observateur &siteLancement)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QPen crayon(Qt::white);
    crayon.setCosmetic(true);

    /* Corps de la methode */
    show();

    if (!acronyme.isEmpty()) {

        const int lobs = qRound((180. - siteLancement.longitude() * RAD2DEG) * DEG2PXHZ)+1;
        const int bobs = qRound((90. - siteLancement.latitude() * RAD2DEG) * DEG2PXVT)+1;

        scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
        scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

        QGraphicsSimpleTextItem * const txtObs = new QGraphicsSimpleTextItem(acronyme);
        const int lng = static_cast<int> (txtObs->boundingRect().width());
        const int xnobs = ((lobs + 7 + lng) > _ui->carte->width()) ? lobs - lng - 1 : lobs + 4;
        const int ynobs = ((bobs + 12) > _ui->carte->height()) ? bobs - 12 : bobs;

        txtObs->setBrush(Qt::white);
        txtObs->setPos(xnobs, ynobs);
        scene->addItem(txtObs);
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
bool Carte::eventFilter(QObject *watched, QEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->carte->underMouse()) {

        const QMouseEvent* const evt = static_cast<const QMouseEvent*>(event);
        const int xCur = evt->x() + 1;
        const int yCur = evt->y() + 1;

        if ((event->type() == QEvent::MouseMove) || (event->type() == QEvent::HoverEnter)) {

            if (_onglets->ui()->affcoord->isChecked()) {

                // Longitude
                const double lo0 = 180. - xCur / DEG2PXHZ;
                const QString ew = (lo0 < 0.) ? tr("Est") : tr("Ouest");

                // Latitude
                const double la0 = 90. - yCur / DEG2PXVT;
                const QString ns = (la0 < 0.) ? tr("Sud") : tr("Nord");

                // Affichage des coordonnees dans la barre de statut
                emit AfficherMessageStatut2(tr("Longitude : %1° %2").arg(fabs(lo0), 6, 'f', 2, QChar('0')).arg(ew));
                emit AfficherMessageStatut3(tr("Latitude : %1° %2").arg(fabs(la0), 6, 'f', 2, QChar('0')).arg(ns));
            }

            // Survol d'un satellite avec le curseur
            QListIterator<Satellite> it(Configuration::instance()->listeSatellites());
            bool atrouve = false;
            while (it.hasNext() && !atrouve) {

                const Satellite sat = it.next();
                const int lsat = qRound((180. - sat.longitude() * RAD2DEG) * DEG2PXHZ) + 2;
                const int bsat = qRound((90. - sat.latitude() * RAD2DEG) * DEG2PXVT) + 2;

                // Distance au carre du curseur au satellite
                const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

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

            // Survol du Soleil avec le curseur
            if (_onglets->ui()->affsoleil->isChecked()) {

                static bool asoleil = false;
                const int lsol = qRound((180. - Configuration::instance()->soleil().longitude() * RAD2DEG) * DEG2PXHZ) + 2;
                const int bsol = qRound((90. - Configuration::instance()->soleil().latitude() * RAD2DEG) * DEG2PXVT) + 2;

                // Distance au carre du curseur au Soleil
                const int dt = (xCur - lsol) * (xCur - lsol) + (yCur - bsol) * (yCur - bsol);

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
                const int llun = qRound((180. - Configuration::instance()->lune().longitude() * RAD2DEG) * DEG2PXHZ) + 2;
                const int blun = qRound((90. - Configuration::instance()->lune().latitude() * RAD2DEG) * DEG2PXVT) + 2;

                // Distance au carre du curseur a la Lune
                const int dt = (xCur - llun) * (xCur - llun) + (yCur - blun) * (yCur - blun);

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
        } else if (event->type() == QEvent::MouseButtonPress) {

            if (evt->button() == Qt::LeftButton) {

                // Clic sur un satellite
                QListIterator<Satellite> it(Configuration::instance()->listeSatellites());
                bool atrouve = false;
                int idx = 1;
                while (it.hasNext() && !atrouve) {

                    const Satellite sat = it.next();
                    const int lsat = qRound((180. - sat.longitude() * RAD2DEG) * DEG2PXHZ) + 2;
                    const int bsat = qRound((90. - sat.latitude() * RAD2DEG) * DEG2PXVT) + 2;

                    // Distance au carre du curseur au satellite
                    const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

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

                        _onglets->setAcalcAOS(true);
                        _onglets->setAcalcDN(true);
                        _onglets->setInfo(true);

                        Configuration::instance()->notifAOS() = ATTENTE_LOS;
                        Configuration::instance()->notifFlashs() = ATTENTE_LOS;

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

/*
 * Affichage par defaut d'un satellite (sans icone)
 */
void Carte::AffichageSatellite(const Satellite &satellite, const int lsat, const int bsat, const int lcarte, const int hcarte) const
{
    /* Declarations des variables locales */
    QColor couleur;

    /* Initialisations */
    const QRect rectangle = QRect(lsat - 3, bsat - 3, 6, 6);
    const QColor crimson(220, 20, 60);
    QPen noir(Qt::black);
    noir.setCosmetic(true);

    /* Corps de la methode */
    // Dessin du satellite
    if (satellite.conditionEclipse().eclipseTotale()) {
        couleur = crimson;
    } else if (satellite.conditionEclipse().eclipsePartielle() || satellite.conditionEclipse().eclipseAnnulaire()) {
        couleur = Qt::green;
    } else {
        couleur = Qt::yellow;
    }
    scene->addEllipse(rectangle, noir, QBrush(couleur, Qt::SolidPattern));

    // Nom des satellites
    if (_onglets->ui()->affnomsat->isChecked()) {

        if (((_onglets->ui()->affnomsat->checkState() == Qt::PartiallyChecked) &&
             (satellite.tle().norad() == Configuration::instance()->listeSatellites().at(0).tle().norad())) ||
                (_onglets->ui()->affnomsat->checkState() == Qt::Checked)) {

            QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(satellite.tle().nom());
            const int lng = static_cast<int> (txtSat->boundingRect().width());
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
