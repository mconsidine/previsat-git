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
 * >
 *
 */

#include <QDir>
#include "configuration/configuration.h"
#include "carte.h"
#include "onglets.h"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include "ui_carte.h"
#include "ui_onglets.h"
#pragma GCC diagnostic warning "-Wconversion"


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
    ui(new Ui::Carte)
{
    ui->setupUi(this);
    scene = nullptr;
    _onglets = onglets;

    DEG2PXHZ = (1. / 0.45);
    DEG2PXVT = (1. / 0.45);
}


/*
 * Destructeur
 */
Carte::~Carte()
{
    delete ui;
    if (scene != nullptr) {
        delete scene;
    }
}


/*
 * Accesseurs
 */


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
    const QColor crimson(220, 20, 60);
    const QColor bleuClair(173, 216, 230);
    const QPen noir(Qt::black);
    QPen crayon(Qt::white);

    if (scene != nullptr) {
        scene->deleteLater();
    }
    scene = new QGraphicsScene;
    scene->setSceneRect(ui->carte->rect());

    /* Corps de la methode */
    // Chargement de la carte
    //    const QString nomMap = (_onglets->ui()->listeMap->currentIndex() == 0) ?
    //                ":/resources/map.png" : ficMap.at(_onglets->ui()->listeMap->currentIndex()-1);
    const QString nomMap = ":/resources/map.png";
    QPixmap pixMap;
    pixMap.load(nomMap);
    pixMap = pixMap.scaled(ui->carte->size());
    scene->addPixmap(pixMap);

    if (!ui->carte->isHidden()) {

        // Affichage du filtre de la carte en mode vision nocturne
        // TODO

        // Affichage de la grille de coordonnees
        if (_onglets->ui()->affgrille->isChecked()) {

            const QPen pen = QPen(/*(!satellites.isEmpty() && mcc && satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) ?*/
                                  /*((ui->coulEquateur->currentIndex() == 0) ? Qt::red : Qt::white) :*/ Qt::white);
            const int lcarte = ui->carte->width();
            const int hcarte = ui->carte->height();
            const int lcarte2 = lcarte / 2;
            const int hcarte2 = hcarte / 2;

            scene->addLine(0, hcarte2, lcarte, hcarte2, pen);
            scene->addLine(lcarte2, 0, lcarte2, hcarte, QPen(Qt::white));

            QPen stylo(Qt::lightGray);
            QList<int> tabLat, tabLon;

            //            if (mcc) {
            //                tabLat << hcarte / 12 << hcarte / 6 << static_cast<int> (hcarte * 0.25) << hcarte / 3 << static_cast<int> (hcarte / 2.4) <<
            //                          static_cast<int> (7. * hcarte / 12.) << static_cast<int> (hcarte / 1.5) << static_cast<int> (hcarte * 0.75) <<
            //                          static_cast<int> (hcarte / 1.2) <<
            //                          static_cast<int> (11. * hcarte / 12.);
            //                tabLon << lcarte / 24 << lcarte / 12 << lcarte / 8 << lcarte / 6 << static_cast<int> (lcarte / 4.8) << static_cast<int> (lcarte * 0.25) <<
            //                          static_cast<int> (7. * lcarte / 24.) << lcarte / 3 << static_cast<int> (3. * lcarte / 8.) << static_cast<int> (lcarte / 2.4) <<
            //                          static_cast<int> (11. * lcarte / 24.) << static_cast<int> (13. * lcarte / 24.) << static_cast<int> (7. * lcarte / 12.) <<
            //                          static_cast<int> (15. * lcarte / 24.) << static_cast<int> (lcarte / 1.5) << static_cast<int> (17. * lcarte / 24.) <<
            //                          static_cast<int> (lcarte * 0.75) <<
            //                          static_cast<int> (19. * lcarte / 24.) << static_cast<int> (lcarte / 1.2) << static_cast<int> (21. * lcarte / 24.) <<
            //                          static_cast<int> (11. * lcarte / 12.) << static_cast<int> (23. * lcarte / 24.);

            //                ui->W150->setText("-150");
            //                ui->W120->setText("-120");
            //                ui->W90->setText("-90");
            //                ui->W60->setText("-60");
            //                ui->W30->setText("-30");

            //                QPalette coul;
            //                coul.setColor(QPalette::WindowText, Qt::white);
            //                ui->N60->setPalette(coul);
            //                ui->N30->setPalette(coul);
            //                ui->N0->setPalette(coul);
            //                ui->S30->setPalette(coul);
            //                ui->S60->setPalette(coul);
            //                ui->S30->setText("-30");
            //                ui->S60->setText("-60");
            //                ui->N60->setAlignment(Qt::AlignRight);
            //                ui->N30->setAlignment(Qt::AlignRight);
            //                ui->N0->setAlignment(Qt::AlignRight);

            //            } else {
            tabLat << hcarte / 6 << hcarte / 3 << static_cast<int> (hcarte / 1.5) << static_cast<int> (hcarte / 1.2);
            tabLon << lcarte / 12 << lcarte / 6 << static_cast<int> (lcarte * 0.25) << lcarte / 3 << static_cast<int> (lcarte / 2.4)
                   << static_cast<int> (7. * lcarte / 12.) << static_cast<int> (lcarte / 1.5) << static_cast<int> (lcarte * 0.75)
                   << static_cast<int> (lcarte / 1.2) << static_cast<int> (11. * lcarte / 12.);
            /*
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
*/
            // Tropiques
            stylo.setStyle(Qt::DashLine);
            scene->addLine(0, 66.55 * DEG2PXVT, lcarte, 66.55 * DEG2PXVT, stylo);
            scene->addLine(0, 113.45 * DEG2PXVT, lcarte, 113.45 * DEG2PXVT, stylo);
            //            }
            /*
            const int dec1 = (mcc) ? 12 : 8;
            const int dec2 = (mcc) ? 9 : 5;
            ui->W150->move(static_cast<int> (lcarte / 12.) - dec1, 0);
            ui->W120->move(static_cast<int> (lcarte / 6.) - dec1, 0);
            ui->W90->move(static_cast<int> (lcarte / 4.) - dec2, 0);
            ui->W60->move(static_cast<int> (lcarte / 3.) - dec2, 0);
            ui->W30->move(static_cast<int> (lcarte / 2.4) - dec2, 0);
*/
            stylo.setStyle(Qt::SolidLine);
            for(int j=0; j<tabLat.size(); j++) {
                scene->addLine(0, tabLat.at(j), lcarte, tabLat.at(j), stylo);
            }

            for(int j=0; j<tabLon.size(); j++) {
                scene->addLine(tabLon.at(j), 0, tabLon.at(j), hcarte, stylo);
            }
            /*
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
   */     }


        // Affichage de la SAA
        if (_onglets->ui()->affSAA->isChecked()/* && !ui->mccISS->isChecked()*/) {

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
            //            if (mcc) {

            //                const QString iconeSoleil = ":/resources/icones/soleil.png";
            //                QGraphicsPixmapItem *pm = scene->addPixmap(QPixmap(iconeSoleil));
            //                pm->setPos(lsol - 15, bsol - 10);

            //            } else {
            //            QRect rectangle = QRect(lsol - 7, bsol - 7, 15, 15);
            //            scene->addEllipse(rectangle, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
            const QString iconeSoleil = ":/resources/soleil.png";
            //            }
            QPixmap pixsol;
            pixsol.load(iconeSoleil);
            pixsol = pixsol.scaled(17, 17);

            QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
            QTransform transform;
            transform.translate(lsol, bsol);
            transform.translate(-7, -7);
            sol->setTransform(transform);

            if (((lsol + 7) > ui->carte->width()) || ((lsol - 7) < 0)) {

                QGraphicsPixmapItem * const sol2 = scene->addPixmap(pixsol);
                transform.reset();

                if ((lsol + 7) > ui->carte->width()) {
                    transform.translate(lsol - ui->carte->width(), bsol);
                }
                if ((lsol - 7) < 0) {
                    transform.translate(lsol + ui->carte->width(), bsol);
                }
                transform.translate(-7, -7);
                sol2->setTransform(transform);
            }
        }

        // Zone d'ombre
        if (_onglets->ui()->affnuit->checkState() != Qt::Unchecked) {

            double beta = PI_SUR_DEUX - REFRACTION_HZ;
            const int imax = (_onglets->ui()->affnuit->checkState() == Qt::PartiallyChecked) ? 1 : 4;

            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, static_cast<int> (2.55 * _onglets->ui()->intensiteOmbre->value())));
            const QPen stylo(/*(mcc) ? ((ui->coulTerminateur->currentIndex() == 0) ?*/
                             /* QPen(QColor::fromRgb(102, 50, 16), 2) : QPen(Qt::darkYellow, 2)) :*/ QPen(Qt::NoBrush, 0));

            QVector<QPointF> zone;
            QVector<QPointF> zone1;
            QVector<QPointF> zone2;
            QList<int> idxIntersection;

            for(int i=0; i<imax; i++) {

                //if (i==0) {
                soleil.CalculZoneVisibilite(beta);

                // Coordonnees de la zone d'ombre, en pixels
                zone.clear();
                idxIntersection.clear();
                zone.append(QPointF(soleil.zone().at(0).x() * DEG2PXHZ, soleil.zone().at(0).y() * DEG2PXVT));

                for(int j=1; j<soleil.zone().size(); j++) {

                    zone.append(QPointF(soleil.zone().at(j).x() * DEG2PXHZ, soleil.zone().at(j).y() * DEG2PXVT));

                    if (fabs(zone.at(j).x() - zone.at(j-1).x()) > (ui->carte->width() / 2)) {
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

                            zone.insert(0, QPointF(zone.last().x() - ui->carte->width(), zone.last().y()));

                            if (soleil.latitude() > 0.) {
                                zone.insert(0, QPointF(zone.last().x() - ui->carte->width(), ui->carte->height()));
                                zone.insert(0, QPointF(zone.at(2).x() + ui->carte->width(), ui->carte->height()));

                            } else {
                                zone.insert(0, QPointF(zone.last().x() - ui->carte->width(), -1.));
                                zone.insert(0, QPointF(zone.at(2).x() + ui->carte->width(), -1.));
                            }
                            zone.insert(0, QPointF(zone.at(3).x() + ui->carte->width(), zone.at(3).y()));

                        } else {

                            zone.insert(0, QPointF(zone.last().x() + ui->carte->width(), zone.last().y()));

                            if (soleil.latitude() > 0.) {
                                zone.insert(0, QPointF(zone.last().x() + ui->carte->width(), ui->carte->height()));
                                zone.insert(0, QPointF(zone.at(2).x() - ui->carte->width(), ui->carte->height()));

                            } else {
                                zone.insert(0, QPointF(zone.last().x() + ui->carte->width(), -1.));
                                zone.insert(0, QPointF(zone.at(2).x() - ui->carte->width(), -1.));
                            }
                            zone.insert(0, QPointF(zone.at(3).x() - ui->carte->width(), zone.at(3).y()));
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
                                zone1.insert(jmed, QPointF(zone2.last().x() + ui->carte->width(), zone2.last().y()));
                                zone1.insert(jmed, QPointF(zone2.first().x() + ui->carte->width(), zone2.first().y()));

                            } else {
                                zone1.append(QPointF(zone2.last().x() + ui->carte->width(), zone2.last().y()));
                                zone1.append(QPointF(zone2.first().x() + ui->carte->width(), zone2.first().y()));
                            }

                            zone2.insert(0, QPointF(zone1.at(jmed-1).x() - ui->carte->width(), zone1.at(jmed-1).y()));
                            zone2.insert(0, QPointF(zone1.at(jmed+2).x() - ui->carte->width(), zone1.at(jmed+2).y()));

                        } else {

                            if (zone1.size() > jmed) {
                                zone1.insert(jmed, QPointF(zone2.last().x() - ui->carte->width(), zone2.last().y()));
                                zone1.insert(jmed, QPointF(zone2.first().x() - ui->carte->width(), zone2.first().y()));

                            } else {
                                zone1.append(QPointF(zone2.last().x() - ui->carte->width(), zone2.last().y()));
                                zone1.append(QPointF(zone2.first().x() - ui->carte->width(), zone2.first().y()));
                            }

                            zone2.insert(0, QPointF(zone1.at(jmed-1).x() + ui->carte->width(), zone1.at(jmed-1).y()));
                            zone2.insert(0, QPointF(zone1.at(jmed+2).x() + ui->carte->width(), zone1.at(jmed+2).y()));
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

                    if (lsol > ui->carte->width() / 4 && lsol < (3 * ui->carte->width()) / 4) {

                        zone1.clear();
                        zone1.append(QPointF(-1., -1.));
                        zone1.append(QPointF(x1, -1.));
                        zone1.append(QPointF(x1, ui->carte->height()));
                        zone1.append(QPointF(-1., ui->carte->height()));

                        zone2.clear();
                        zone2.append(QPointF(ui->carte->width(), -1.));
                        zone2.append(QPointF(x2, -1.));
                        zone2.append(QPointF(x2, ui->carte->height()));
                        zone2.append(QPointF(ui->carte->width(), ui->carte->height()));

                        const QPolygonF poly1(zone1);
                        const QPolygonF poly2(zone2);
                        scene->addPolygon(poly1, stylo, alpha);
                        scene->addPolygon(poly2, stylo, alpha);

                    } else {

                        zone1.clear();
                        zone1.append(QPointF(x1, -1.));
                        zone1.append(QPointF(x1, ui->carte->height()));
                        zone1.append(QPointF(x2, ui->carte->height()));
                        zone1.append(QPointF(x2, -1.));

                        const QPolygonF poly1(zone1);
                        scene->addPolygon(poly1, stylo, alpha);
                    }
                }
                //}
                beta += 6. * DEG2RAD;
            }
        }

        // Affichage de la ZOE et de la SAA pour le Wall Command Center
        // TODO

        // Lieux d'observation
        const int nbMax = (_onglets->ui()->affnomlieu->checkState() == Qt::Unchecked) ? 0 : Configuration::instance()->observateurs().size() - 1;
        for(int j=nbMax; j>=0; j--) {

            const int lobs = qRound((180. - Configuration::instance()->observateurs().at(j).longitude() * RAD2DEG) * DEG2PXHZ)+1;
            const int bobs = qRound((90. - Configuration::instance()->observateurs().at(j).latitude() * RAD2DEG) * DEG2PXVT)+1;

            scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
            scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

            if ((j == 0) || (_onglets->ui()->affnomlieu->checkState() == Qt::Checked)) {

                QGraphicsSimpleTextItem * const txtObs = new QGraphicsSimpleTextItem(Configuration::instance()->observateurs().at(j).nomlieu());
                const int lng = static_cast<int> (txtObs->boundingRect().width());
                const int xnobs = ((lobs + 7 + lng) > ui->carte->width()) ? lobs - lng - 1 : lobs + 4;
                const int ynobs = ((bobs + 12) > ui->carte->height()) ? bobs - 12 : bobs;

                txtObs->setBrush(Qt::white);
                txtObs->setPos(xnobs, ynobs);
                scene->addItem(txtObs);
            }
        }

        // Stations
        // TODO

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

            if (((llun + 7) > ui->carte->width()) || ((llun - 7) < 0)) {

                QGraphicsPixmapItem * const lun2 = scene->addPixmap(pixlun);

                if ((llun + 7) > ui->carte->width()) {
                    transform2.translate(llun - ui->carte->width(), blun);
                }
                if ((llun - 7) < 0) {
                    transform2.translate(lsol + ui->carte->width(), blun);
                }
                if (_onglets->ui()->rotationLune->isChecked() && (Configuration::instance()->observateurs().at(0).latitude() < 0.)) {
                    transform2.rotate(180.);
                }
                transform2.translate(-7, -7);
                lun2->setTransform(transform2);
            }

            if (_onglets->ui()->affphaselune->isChecked()) {

                QVector<QPointF> pt;
                const int rayonX = 9;
                const int rayonY = static_cast<int> (-cos(Configuration::instance()->lune().anglePhase()) * rayonX);
                const int sph = (Configuration::instance()->lune().luneCroissante()) ? -1 : 1;
                double ang = PI_SUR_DEUX;

                for(int i=0; i<36; i++) {

                    const double x = sph * ((i < 19) ? rayonY : rayonX) * cos(ang) + 8;
                    const double y = rayonX * sin(ang) + 8;

                    pt.append(QPointF(x, y));
                    ang += 10. * M_PI / 180.;
                }

                const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 255));
                const QPen stylo(Qt::NoBrush, 0);
                const QPolygonF poly(pt);

                QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);
                omb->setTransform(transform);

                if (!transform2.isIdentity()) {
                    QGraphicsPolygonItem * const omb2 = scene->addPolygon(poly, stylo, alpha);
                    omb2->setTransform(transform2);
                }
            }
            //}
        }
    }

    // Affichage de la trace au sol du satellite par defaut
    if (_onglets->ui()->afftraj->isChecked()/* || mcc*/) {

        if (Configuration::instance()->listeSatellites().size() > 0) {

            const Satellite &satellite = Configuration::instance()->listeSatellites().at(0);
            if ((satellite.altitude() >= 0.) && (!satellite.traceAuSol().isEmpty())) {

                double lsat1 = satellite.traceAuSol().at(0).longitude * DEG2PXHZ;
                double bsat1 = satellite.traceAuSol().at(0).latitude * DEG2PXVT;

                int nbOrb = 0;
                double lsat2;
                double bsat2;
                int ils;
                for(int j=1; j<satellite.traceAuSol().size()-1; j++) {

                    lsat2 = satellite.traceAuSol().at(j).longitude * DEG2PXHZ;
                    bsat2 = satellite.traceAuSol().at(j).latitude * DEG2PXVT;
                    ils = 99999;

                    if (fabs(lsat2 - lsat1) > ui->carte->width() / 2) {
                        if (lsat2 < lsat1) {
                            lsat2 += ui->carte->width();
                        } else {
                            lsat1 += ui->carte->width();
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

                    //                    if (ui->mccISS->isChecked()) {

                    //                        if (satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) {

                    //                            // Affichage du numero d'orbite
                    //                            if (satellites.at(0).traceAuSol().at(j).at(1) < 90. &&
                    //                                    satellites.at(0).traceAuSol().at(j-1).at(1) > 90. && nbOrb < 3) {

                    //                                nbOrb++;
                    //                                const Date dateISS(Date(satellites.at(0).traceAuSol().at(j+1).at(3), 0., false));
                    //                                const int numOrb = CalculNumeroOrbiteISS(dateISS);
                    //                                QGraphicsSimpleTextItem * const txtOrb = new QGraphicsSimpleTextItem(QString::number(numOrb));

                    //                                const QFont policeOrb(PreviSat::font().family(), 10, (ui->styleWCC->isChecked()) ?
                    //                                                          QFont::Bold : QFont::Normal);
                    //                                txtOrb->setFont(policeOrb);
                    //                                txtOrb->setBrush(Qt::white);

                    //                                const int lng = static_cast<int> txtOrb->boundingRect().width();
                    //                                const double xnorb = (lsat2 - lng < 0) ? lsat2 + lcarte - lng - 8 : lsat2 - lng;
                    //                                txtOrb->setPos(xnorb, hcarte2 - 18);
                    //                                scene->addItem(txtOrb);
                    //                            }

                    //                            if (ui->styleWCC->isChecked()) {

                    //                                if (satellites.at(0).tle().norad() == NORAD_STATION_SPATIALE) {
                    //                                    crayon = QPen(Qt::white, 2);
                    //                                }

                    //                                // Affichage des crochets des transitions jour/nuit
                    //                                const double ecl = satellites.at(0).traceAuSol().at(j).at(2);
                    //                                if (fabs(ecl - satellites.at(0).traceAuSol().at(j+1).at(2)) > 1.5) {

                    //                                    const double ang = fmod(-fabs(lig.angle()), T360);
                    //                                    const double ca = cos(ang * DEG2RAD);
                    //                                    const double sa = sin(ang * DEG2RAD);

                    //#if defined (Q_OS_MAC)
                    //                                    const QFont policeOmb(PreviSat::font().family(), 24);
                    //                                    const double fact = (ecl > EPSDBL100) ? 3. : 1.;
                    //                                    const double xnc = lsat2 - fact * ca + 10. * sa;
                    //                                    const double ync = bsat2 - fact * sa - 10. * ca;
                    //#else
                    //                                    const QFont policeOmb(PreviSat::font().family(), 14);
                    //                                    const double fact = (ecl > EPSDBL100) ? 4. : 2.;
                    //                                    const double xnc = lsat2 - fact * ca + 14. * sa;
                    //                                    const double ync = bsat2 - fact * sa - 13. * ca;
                    //#endif

                    //                                    QGraphicsSimpleTextItem * const txtOmb =
                    //                                            new QGraphicsSimpleTextItem((ecl > EPSDBL100) ? "[" : "]");

                    //                                    txtOmb->setFont(policeOmb);
                    //                                    txtOmb->setBrush(Qt::white);
                    //                                    txtOmb->setPos(xnc, ync);
                    //                                    txtOmb->setRotation(ang);
                    //                                    scene->addItem(txtOmb);
                    //                                }
                    //                            }
                    //                        }
                    //                    }
                    scene->addLine(lig, crayon);

                    if (ils == j) {
                        lsat2 -= ui->carte->width();
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
            const int nbMax2 = /*(ui->affvisib->checkState() == Qt::PartiallyChecked) ? 1 :*/ satellites.size();

            int ils;
            double lsat1;
            double bsat1;
            double lsat2;
            double bsat2;

            for(int isat=0; isat<nbMax2; isat++) {

                //if (mcc) {

                //                    crayon = QPen((ui->coulCercleVisibilite->currentIndex() == 0) ? Qt::white : Qt::darkRed, 2);
                //                    if (satellites.at(isat).tle().nom().toLower().startsWith("tdrs")) {

                //                        const int numeroTDRS = satellites.at(isat).tle().nom().section(" ", 1).toInt();

                //                        QStringListIterator it(tabTDRS);
                //                        while (it.hasNext()) {

                //                            const QString ligne = it.next().trimmed();
                //                            if (ligne.section(" ", 0, 0).toInt() == numeroTDRS) {

                //                                const QString nomTDRS = ligne.section(" ", 1, 1);
                //                                const int r = ligne.section(" ", 2, 2).toInt();
                //                                const int v = ligne.section(" ", 3, 3).toInt();
                //                                const int b = ligne.section(" ", 4, 4).toInt();

                //                                // Affichage du nom du satellite TDRS
                //                                QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(nomTDRS);
                //                                const QFont policeSat(ui->policeWCC->currentText(), 11);
                //                                txtSat->setFont(policeSat);

                //                                const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ);
                //                                const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT);

                //                                crayon = QPen(QColor(r, v, b), 2);
                //                                txtSat->setBrush(crayon.color());

                //                                const int lng = static_cast<int> txtSat->boundingRect().width();
                //                                const int xnsat = lsat - lng / 2 + 1;
                //                                txtSat->setPos(xnsat, bsat + 19);
                //                                scene->addItem(txtSat);
                //                            }
                //                        }
                //                    }
                //} else {
                crayon = QPen(Qt::white);
                //}

                if ((satellites.at(isat).altitude() >= 0.) && (!satellites.at(isat).zone().isEmpty())) {

                    lsat1 = satellites.at(isat).zone().at(0).x() * DEG2PXHZ + 1;
                    bsat1 = satellites.at(isat).zone().at(0).y() * DEG2PXVT + 1;

                    for(int j=1; j<361; j++) {

                        lsat2 = satellites.at(isat).zone().at(j).x() * DEG2PXHZ + 1;
                        bsat2 = satellites.at(isat).zone().at(j).y() * DEG2PXVT + 1;
                        ils = 99999;

                        if (fabs(lsat2 - lsat1) > ui->carte->width() / 2) {
                            if (lsat2 < lsat1) {
                                lsat2 += ui->carte->width();
                            } else {
                                lsat1 += ui->carte->width();
                            }
                            ils = j;
                            als[isat] = true;
                        }

                        scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        if (ils == j) {
                            lsat1 -= ui->carte->width() + 1;
                            lsat2 -= ui->carte->width() + 1;
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

    for(int isat=satellites.size()-1; isat>=0; isat--) {

        if (satellites.at(isat).altitude() >= 0.) {

            const int lsat = qRound((180. - satellites.at(isat).longitude() * RAD2DEG) * DEG2PXHZ)+1;
            const int bsat = qRound((90. - satellites.at(isat).latitude() * RAD2DEG) * DEG2PXVT)+1;

            if (/*mcc || */_onglets->ui()->afficone->isChecked()) {

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
                    AffichageSatellite(satellites.at(isat), lsat, bsat, ui->carte->width(), ui->carte->height());
                } else {

                    // Affichage de l'icone satellite
                    img = QPixmap(listeIcones.at(0));
                    img = img.scaled(qMin(ui->carte->width() / 12, img.width()), qMin(ui->carte->height() / 6, img.height()));

                    QGraphicsPixmapItem * const pm = scene->addPixmap(img);

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
                        QGraphicsPixmapItem * const pm2 = scene->addPixmap(img);
                        transform.reset();

                        if (lsat > ui->carte->width() / 2) {
                            transform.translate(lsat - ui->carte->width(), bsat);
                        } else {
                            transform.translate(lsat + ui->carte->width(), bsat);
                        }

                        transform.rotate(angle);
                        transform.translate(-img.width() / 2, -img.height() / 2);
                        pm2->setTransform(transform);
                    }
                }
            } else {
                AffichageSatellite(satellites.at(isat), lsat, bsat, ui->carte->width(), ui->carte->height());
            }
        }
    }
    ui->carte->setScene(scene);

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

        const int href = 2 * height() - 3;
        const int lref = width();
        const int lc = qMin(href, lref);
        const int hc = (lc + 3) / 2;
        ui->carte->setGeometry((width() - lc) / 2, 0, lc, hc);
    }
    const int hcarte = ui->carte->height() - 3;
    const int lcarte = ui->carte->width() - 3;

    DEG2PXHZ = lcarte * (1. / T360);
    DEG2PXVT = hcarte * (2. / T360);

    if (Configuration::instance()->isCarteMonde()) {
        show();
    }

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

    /* Corps de la methode */
    show();
    const int lobs = qRound((180. - siteLancement.longitude() * RAD2DEG) * DEG2PXHZ)+1;
    const int bobs = qRound((90. - siteLancement.latitude() * RAD2DEG) * DEG2PXVT)+1;

    scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
    scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

    QGraphicsSimpleTextItem * const txtObs = new QGraphicsSimpleTextItem(acronyme);
    const int lng = static_cast<int> (txtObs->boundingRect().width());
    const int xnobs = ((lobs + 7 + lng) > ui->carte->width()) ? lobs - lng - 1 : lobs + 4;
    const int ynobs = ((bobs + 12) > ui->carte->height()) ? bobs - 12 : bobs;

    txtObs->setBrush(Qt::white);
    txtObs->setPos(xnobs, ynobs);
    scene->addItem(txtObs);

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
/*
 * Affichage par defaut d'un satellite (sans icone)
 */
void Carte::AffichageSatellite(const Satellite &satellite, const int lsat, const int bsat, const int lcarte, const int hcarte) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Dessin du satellite
    const QColor crimson(220, 20, 60);
    const QPen noir(Qt::black);
    const QRect rectangle = QRect(lsat - 3, bsat - 3, 6, 6);

    QColor couleur;
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
