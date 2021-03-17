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
 * >    radar.cpp
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
 * >
 *
 */

#include "configuration/configuration.h"
#include "ciel.h"
#include "onglets.h"
#include "radar.h"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "ui_onglets.h"
#include "ui_radar.h"
#pragma GCC diagnostic warning "-Wconversion"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Radar::Radar(Onglets *onglets, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Radar)
{
    ui->setupUi(this);
    scene = nullptr;
    _onglets = onglets;
}

/*
 * Destructeur
 */
Radar::~Radar()
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
 * Affichage du radar
 */
void Radar::show()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QColor crimson(220, 20, 60);
    const QColor bleuClair(173, 216, 230);
    const QPen noir(Qt::black);
    QPen crayon(Qt::white);

    // Determination de la couleur du ciel
    const double hts = Configuration::instance()->soleil().hauteur() * RAD2DEG;
    const QBrush couleurCiel = Ciel::CalculCouleurCiel(hts);

    if (scene != nullptr) {
        scene->deleteLater();
    }

    scene = new QGraphicsScene;
    scene->setBackgroundBrush(QBrush(palette().window().color()));
    scene->setSceneRect(ui->vueRadar->rect());

    /* Corps de la methode */
    // Affichage de la couleur du ciel
    QRect rectangle(2, 2, 196, 196);
    const QPen pen(couleurCiel, Qt::SolidPattern);
    scene->addEllipse(rectangle, pen, couleurCiel);

    // Dessin des cercles concentriques
    scene->addEllipse(33, 33, 133, 133, QPen(Qt::gray));
    scene->addEllipse(67, 67, 67, 67, QPen(Qt::gray));

    // Dessin des segments
    scene->addLine(0, 100, 200, 100, QPen(Qt::gray));
    scene->addLine(100, 0, 100, 200, QPen(Qt::gray));
    scene->addLine(13, 50, 187, 150, QPen(Qt::gray));
    scene->addLine(13, 150, 187, 50, QPen(Qt::gray));
    scene->addLine(50, 13, 150, 187, QPen(Qt::gray));
    scene->addLine(50, 187, 150, 13, QPen(Qt::gray));

    // Inversion des coordonnees du Soleil et du satellite
    int xf, yf;
    if (_onglets->ui()->affinvns->isChecked()) {
        ui->coordGeo1->setText(tr("Sud"));
        ui->coordGeo2->setText(tr("Nord"));
        yf = -1;
    } else {
        ui->coordGeo1->setText(tr("Nord"));
        ui->coordGeo2->setText(tr("Sud"));
        yf = 1;
    }
    if (_onglets->ui()->affinvew->isChecked()) {
        ui->coordGeo3->setText(tr("Est"));
        ui->coordGeo4->setText(tr("Ouest"));
        xf = 1;
    } else {
        ui->coordGeo3->setText(tr("Ouest"));
        ui->coordGeo4->setText(tr("Est"));
        xf = -1;
    }

    // Affichage du Soleil
    const Soleil &soleil = Configuration::instance()->soleil();
    if (_onglets->ui()->affsoleil->isChecked() && soleil.isVisible()) {

        // Calcul des coordonnees radar du Soleil
        const int lsol = qRound(100. - 100. * xf * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
        const int bsol = qRound(100. - 100. * yf * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

        QPixmap pixsol;
        pixsol.load(":/resources/soleil.png");
        pixsol = pixsol.scaled(17, 17);

        QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
        QTransform transform;
        transform.translate(lsol, bsol);
        transform.translate(-7, -7);
        sol->setTransform(transform);
    }

    // Affichage de la Lune
    const Lune &lune = Configuration::instance()->lune();
    if (_onglets->ui()->afflune->isChecked() && lune.isVisible()) {

        // Calcul des coordonnees radar de la Lune
        const int llun = qRound(100. - 100. * xf * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
        const int blun = qRound(100. - 100. * yf * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

        QPixmap pixlun;
        pixlun.load(":/resources/lune.png");
        pixlun = pixlun.scaled(17, 17);

        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun, blun);
        if (_onglets->ui()->rotationLune->isChecked() && (Configuration::instance()->observateurs().at(0).latitude() < 0.)) {
            transform.rotate(180.);
        }
        transform.translate(-7, -7);
        lun->setTransform(transform);

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
        }
    }

    // Affichage des satellites
    int lsat1;
    int bsat1;
    QColor couleur;
    const QList<Satellite> &satellites = Configuration::instance()->listeSatellites();

    for(int isat=satellites.size()-1; isat>=0; isat--) {

        if (satellites.at(isat).isVisible() && (satellites.at(isat).altitude() >= 0.)) {

            // Affichage de la trace dans le radar
            const QList<ElementsTraceCiel> &trace = satellites.at(isat).traceCiel();
            if (_onglets->ui()->afftraceCiel->isChecked() && (trace.size() > 0)) {

                const double ht1 = trace.at(0).hauteur;
                const double az1 = trace.at(0).azimut;
                lsat1 = qRound(100. - 100. * xf * (1. - ht1 * DEUX_SUR_PI) * sin(az1));
                bsat1 = qRound(100. - 100. * yf * (1. - ht1 * DEUX_SUR_PI) * cos(az1));

                for(int i=1; i<trace.size(); i++) {

                    const double ht2 = trace.at(i).hauteur;
                    const double az2 = trace.at(i).azimut;

                    if (trace.at(i).eclipseTotale) {
                        crayon = crimson;

                    } else if (trace.at(i).eclipsePartielle) {
                        crayon = QPen(Qt::green);

                    } else {

                        const double hauteurSoleil = Configuration::instance()->soleil().hauteur();
                        if (hauteurSoleil > -0.08) {
                            crayon = bleuClair;

                        } else if (hauteurSoleil > -0.12) {
                            crayon = QPen(QColor("deepskyblue"));

                        } else {
                            crayon = QPen(QColor("cyan"));
                        }
                    }

                    const int lsat2 = qRound(100. - 100. * xf * (1. - ht2 * DEUX_SUR_PI) * sin(az2));
                    const int bsat2 = qRound(100. - 100. * yf * (1. - ht2 * DEUX_SUR_PI) * cos(az2));

                    scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                    lsat1 = lsat2;
                    bsat1 = bsat2;
                }
            }

            // Calcul des coordonnees radar du satellite
            const int lsat = qRound(100. - 100. * xf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) * sin(satellites.at(isat).azimut()));
            const int bsat = qRound(100. - 100. * yf * (1. - satellites.at(isat).hauteur() * DEUX_SUR_PI) * cos(satellites.at(isat).azimut()));

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

    scene->addEllipse(-26, -26, 251, 251, QPen(QBrush(palette().window().color()), 56));
    scene->addEllipse(1, 1, 197, 197, QPen(QBrush(Qt::gray), 3));

    ui->vueRadar->setScene(scene);

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

