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
 * >    28 octobre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QSettings>
#include "ui_options.h"
#include "ui_radar.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "interface/ciel/ciel.h"
#include "interface/options/options.h"
#include "radar.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Radar::Radar(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Radar)
{
    _ui->setupUi(this);
    scene = nullptr;
}

/*
 * Destructeur
 */
Radar::~Radar()
{
    EFFACE_OBJET(scene);
    delete _ui;
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
 */
void Radar::mouseMoveEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->vueRadar->underMouse()) {
        const int lciel = qRound(0.5 * _ui->vueRadar->width());
        const int hciel = qRound(0.5 * _ui->vueRadar->height());

        const int x1 = static_cast<int> (evt->position().x() - lciel);
        const int y1 = static_cast<int> (evt->position().y() - hciel);

        const int xf = (settings.value("affichage/affinvew").toBool()) ? 1 : -1;
        const int yf = (settings.value("affichage/affinvns").toBool()) ? -1 : 1;

        const int lciel2 = lciel * xf;
        const int hciel2 = hciel * yf;

        // Le curseur est au-dessus du radar
        if ((x1 * x1 + y1 * y1) <= (hciel * lciel)) {

            const double x2 = -x1 * xf / (double) lciel;
            const double y2 = -y1 * yf / (double) hciel;

            const double ht = PI_SUR_DEUX * (1. - sqrt(x2 * x2 + y2 * y2));
            double az = atan2(x2, y2);
            if (az < 0.) {
                az += DEUX_PI;
            }

            // Affichage des coordonnees dans la barre de statut
            emit AfficherMessageStatut2(tr("Azimut : %1°").arg(fabs(az) * RAD2DEG, 6, 'f', 2, QChar('0')));
            emit AfficherMessageStatut3(tr("Hauteur : %1°").arg(fabs(ht) * RAD2DEG, 5, 'f', 2, QChar('0')));

            // Survol d'un satellite avec le curseur
            QListIterator<Satellite> it(Configuration::instance()->listeSatellites());
            bool atrouve = false;
            while (it.hasNext() && !atrouve) {

                const Satellite sat = it.next();
                const int lsat = qRound(-lciel2 * (1. - sat.hauteur() * DEUX_SUR_PI) * sin(sat.azimut()));
                const int bsat = qRound(-hciel2 * (1. - sat.hauteur() * DEUX_SUR_PI) * cos(sat.azimut()));

                // Distance au carre du satellite au curseur
                const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                // Le curseur est au(dessus d'un satellite
                if ((dt <= 16) && (sat.altitude() > 0.)) {

                    atrouve = true;
                    setToolTip(tr("<font color='blue'><b>%1</b></font><br />NORAD : <b>%2</b><br />COSPAR : <b>%3</b>")
                               .arg(sat.elementsOrbitaux().nom).arg(sat.elementsOrbitaux().norad).arg(sat.elementsOrbitaux().cospar));

                    emit AfficherMessageStatut(tr("<b>%1</b> (numéro NORAD : <b>%2</b>  -  COSPAR : <b>%3</b>)")
                                               .arg(sat.elementsOrbitaux().nom).arg(sat.elementsOrbitaux().norad).arg(sat.elementsOrbitaux().cospar));
                    setCursor(Qt::CrossCursor);
                } else {

                    emit EffacerMessageStatut();
                    setToolTip("");
                    setCursor(Qt::ArrowCursor);
                }
            }

            // Survol du Soleil avec le curseur
            if (settings.value("affichage/affsoleil").toBool()) {

                static bool asoleil = false;
                const Soleil &soleil = Configuration::instance()->soleil();

                const int lsol = qRound(-lciel2 * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
                const int bsol = qRound(-hciel2 * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

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
            if (settings.value("affichage/afflune").toBool()) {

                static bool alune = false;
                const Lune &lune = Configuration::instance()->lune();

                const int llun = qRound(-lciel2 * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
                const int blun = qRound(-hciel2 * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

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
        } else {
            setCursor(Qt::ArrowCursor);
            setToolTip("");
            emit EffacerMessageStatut();
            emit AfficherMessageStatut2("");
            emit AfficherMessageStatut3("");
        }
    }

    /* Retour */
    return;
}

void Radar::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->vueRadar->underMouse()) {

        const int lciel = qRound(0.5 * _ui->vueRadar->width());
        const int hciel = qRound(0.5 * _ui->vueRadar->height());

        const int x1 = static_cast<int> (evt->position().x() - lciel);
        const int y1 = static_cast<int> (evt->position().y() - hciel);

        const int xf = (settings.value("affichage/affinvew").toBool()) ? 1 : -1;
        const int yf = (settings.value("affichage/affinvns").toBool()) ? -1 : 1;

        const int lciel2 = lciel * xf;
        const int hciel2 = hciel * yf;

        if (evt->button() == Qt::LeftButton) {

            // Clic sur un satellite
            QListIterator<Satellite> it(Configuration::instance()->listeSatellites());
            bool atrouve = false;
            int idx = 0;
            while (it.hasNext() && !atrouve) {

                const Satellite sat = it.next();
                const int lsat = qRound(-lciel2 * (1. - sat.hauteur() * DEUX_SUR_PI) * sin(sat.azimut()));
                const int bsat = qRound(-hciel2 * (1. - sat.hauteur() * DEUX_SUR_PI) * cos(sat.azimut()));

                // Distance au carre du curseur au satellite
                const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                // Le curseur est au-dessus d'un satellite
                if ((dt <= 16) && (sat.altitude() > 0.)) {

                    atrouve = true;
                    Configuration::instance()->listeSatellites().move(idx, 0);
                    QStringList &listeNorad = Configuration::instance()->mapSatellitesFichierElem()[Configuration::instance()->nomfic()];
                    listeNorad.move(idx, 0);

                    // On definit le satellite choisi comme satellite par defaut
                    Configuration::instance()->noradDefaut() = sat.elementsOrbitaux().norad;
                    emit ReinitFlags();

                    Configuration::instance()->notifAOS() = NotificationSonore::ATTENTE_LOS;
                    Configuration::instance()->notifFlashs() = NotificationSonore::ATTENTE_LOS;

                    emit RecalculerPositions();
                    GestionnaireXml::EcritureConfiguration();
                }
                idx++;
            }
        }
    }

    /* Retour */
    return;
}

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
    scene->setSceneRect(_ui->vueRadar->rect());

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
    if (settings.value("affichage/affinvns").toBool()) {
        _ui->coordGeo1->setText(tr("Sud"));
        _ui->coordGeo2->setText(tr("Nord"));
        yf = -1;
    } else {
        _ui->coordGeo1->setText(tr("Nord"));
        _ui->coordGeo2->setText(tr("Sud"));
        yf = 1;
    }
    if (settings.value("affichage/affinvew").toBool()) {
        _ui->coordGeo3->setText(tr("Est"));
        _ui->coordGeo4->setText(tr("Ouest"));
        xf = 1;
    } else {
        _ui->coordGeo3->setText(tr("Ouest"));
        _ui->coordGeo4->setText(tr("Est"));
        xf = -1;
    }

    // Affichage du Soleil
    const Soleil &soleil = Configuration::instance()->soleil();
    if (settings.value("affichage/affsoleil").toBool() && soleil.isVisible()) {

        // Calcul des coordonnees radar du Soleil
        const int lsol = qRound(100. - 100. * xf * (1. - soleil.hauteur() * DEUX_SUR_PI) * sin(soleil.azimut()));
        const int bsol = qRound(100. - 100. * yf * (1. - soleil.hauteur() * DEUX_SUR_PI) * cos(soleil.azimut()));

        QPixmap pixsol;
        pixsol.load(":/resources/interface/soleil.png");
        pixsol = pixsol.scaled(17, 17);

        QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
        QTransform transform;
        transform.translate(lsol, bsol);
        transform.translate(-7, -7);
        sol->setTransform(transform);
    }

    // Affichage de la Lune
    const Lune &lune = Configuration::instance()->lune();
    if (settings.value("affichage/afflune").toBool() && lune.isVisible()) {

        // Calcul des coordonnees radar de la Lune
        const int llun = qRound(100. - 100. * xf * (1. - lune.hauteur() * DEUX_SUR_PI) * sin(lune.azimut()));
        const int blun = qRound(100. - 100. * yf * (1. - lune.hauteur() * DEUX_SUR_PI) * cos(lune.azimut()));

        const int lpol = 100;
        const int bpol = qRound(100 - 100 * (1. - Configuration::instance()->observateurs().at(0).latitude() * DEUX_SUR_PI));

        QPixmap pixlun;
        pixlun.load(":/resources/interface/lune.png");
        pixlun = pixlun.scaled(17, 17);

        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun, blun);
        transform.rotate(180. - QLineF(llun, blun, lpol, bpol).normalVector().angle());
        if (settings.value("affichage/rotationLune").toBool() && (Configuration::instance()->observateurs().at(0).latitude() < 0.)) {
            transform.rotate(180.);
        }
        transform.translate(-7, -7);
        lun->setTransform(transform);

        if (settings.value("affichage/affphaselune").toBool()) {

            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 160));
            const QPen stylo(Qt::NoBrush, 0);
            const QPolygonF poly = Ciel::AffichagePhaseLune(lune, 9);

            QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);
            omb->setTransform(transform);
        }
    }

    // Affichage des satellites
    int lsat1;
    int bsat1;
    QColor couleur;
    const QList<Satellite> &satellites = Configuration::instance()->listeSatellites();

    for(int isat=static_cast<int> (satellites.size()-1); isat>=0; isat--) {

        if (satellites.at(isat).isVisible() && (satellites.at(isat).altitude() >= 0.)) {

            // Affichage de la trace dans le radar
            const QList<ElementsTraceCiel> &trace = satellites.at(isat).traceCiel();
            if (settings.value("affichage/afftraceCiel").toBool() && (trace.size() > 0)) {

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

    _ui->vueRadar->setScene(scene);

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

