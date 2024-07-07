/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    26 decembre 2023
 *
 */

#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QSettings>
#include "../options/ui_options.h"
#include "ui_radar.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "interface/ciel/ciel.h"
#include "interface/options/options.h"
#include "librairies/exceptions/exception.h"
#include "radar.h"


// Registre
#if (PORTABLE_BUILD)
static QSettings settings(QString("%1.ini").arg(APP_NAME), QSettings::IniFormat);
#else
static QSettings settings(ORG_NAME, APP_NAME);
#endif


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

    try {

        qInfo() << "Début Initialisation" << metaObject()->className();

        scene = new QGraphicsScene;

        qInfo() << "Fin   Initialisation" << metaObject()->className();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
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

            const double ht = MATHS::PI_SUR_DEUX * (1. - sqrt(x2 * x2 + y2 * y2));
            double az = atan2(x2, y2);
            if (az < 0.) {
                az += MATHS::DEUX_PI;
            }

            // Affichage des coordonnees dans la barre de statut
            emit AfficherMessageStatut2(tr("Azimut : %1°").arg(fabs(az) * MATHS::RAD2DEG, 6, 'f', 2, QChar('0')));
            emit AfficherMessageStatut3(tr("Hauteur : %1°").arg(fabs(ht) * MATHS::RAD2DEG, 5, 'f', 2, QChar('0')));

            // Survol d'un satellite avec le curseur
            QListIterator it(Configuration::instance()->listeSatellites());
            bool atrouve = false;
            while (it.hasNext() && !atrouve) {

                const Satellite sat = it.next();
                const int lsat = TOPO2X(lciel2, sat.hauteur(), sat.azimut(), 1) - lciel2;
                const int bsat = TOPO2Y(hciel2, sat.hauteur(), sat.azimut(), 1) - hciel2;

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

                const int lsol = TOPO2X(lciel2, soleil.hauteur(), soleil.azimut(), 1) - lciel2;
                const int bsol = TOPO2Y(hciel2, soleil.hauteur(), soleil.azimut(), 1) - hciel2;

                // Distance au carre du Soleil au curseur
                const int dt = (x1 - lsol) * (x1 - lsol) + (y1 - bsol) * (y1 - bsol);

                // Le curseur est au-dessus du Soleil
                if (dt <= 81) {
                    emit AfficherMessageStatut(tr("Soleil"));
                    setToolTip(tr("Soleil"));
                    setCursor(Qt::CrossCursor);
                    asoleil = true;
                } else if (asoleil) {
                    emit EffacerMessageStatut();
                    setToolTip("");
                    setCursor(Qt::ArrowCursor);
                    asoleil = false;
                }
            }

            // Survol de la Lune avec le curseur
            if (settings.value("affichage/afflune").toBool()) {

                static bool alune = false;
                const Lune &lune = Configuration::instance()->lune();

                const int llun = TOPO2X(lciel2, lune.hauteur(), lune.azimut(), 1) - lciel2;
                const int blun = TOPO2Y(hciel2, lune.hauteur(), lune.azimut(), 1) - hciel2;

                // Distance au carre de la Lune au curseur
                const int dt = (x1 - llun) * (x1 - llun) + (y1 - blun) * (y1 - blun);

                // Le curseur est au-dessus de la Lune
                if (dt <= 81) {

                    emit AfficherMessageStatut(tr("Lune"));
                    setToolTip(tr("Lune"));
                    setCursor(Qt::CrossCursor);
                    alune = true;

                } else if (alune) {

                    emit EffacerMessageStatut();
                    setToolTip("");
                    setCursor(Qt::ArrowCursor);
                    alune = false;
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
            QListIterator it(Configuration::instance()->listeSatellites());
            bool atrouve = false;
            int idx = 0;
            while (it.hasNext() && !atrouve) {

                const Satellite sat = it.next();
                const int lsat = TOPO2X(lciel2, sat.hauteur(), sat.azimut(), 1) - lciel2;
                const int bsat = TOPO2Y(hciel2, sat.hauteur(), sat.azimut(), 1) - hciel2;

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
                    Configuration::instance()->EcritureConfiguration();
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
    QColor couleur;

    /* Initialisations */
    const QColor crimson(220, 20, 60);
    const QPen noir(Qt::black);

    // Determination de la couleur du ciel
    const double hts = Configuration::instance()->soleil().hauteur() * MATHS::RAD2DEG;
    const QBrush couleurCiel = Ciel::CalculCouleurCiel(hts);

    scene->clear();
    scene->setBackgroundBrush(QBrush(palette().window().color()));
    scene->setSceneRect(_ui->vueRadar->rect());

    /* Corps de la methode */
    // Affichage de la couleur du ciel
    QRect rectangle(2, 2, 196, 196);
    const QPen coulCiel(couleurCiel, Qt::SolidPattern);
    scene->addEllipse(rectangle, coulCiel, couleurCiel);

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
    if (settings.value("affichage/affsoleil").toBool() && soleil.visible()) {

        // Calcul des coordonnees radar du Soleil
        const int lsol = TOPO2X(100., soleil.hauteur(), soleil.azimut(), xf);
        const int bsol = TOPO2Y(100., soleil.hauteur(), soleil.azimut(), yf);

        QPixmap pixsol;
        pixsol.load(":/resources/interface/soleil.png");
        pixsol = pixsol.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
        QTransform transform;
        transform.translate(lsol, bsol);
        transform.translate(-8, -8);
        sol->setTransform(transform);
    }

    // Affichage de la Lune
    const Lune &lune = Configuration::instance()->lune();
    if (settings.value("affichage/afflune").toBool() && lune.visible()) {

        // Calcul des coordonnees radar de la Lune
        const int llun = TOPO2X(100., lune.hauteur(), lune.azimut(), xf);
        const int blun = TOPO2Y(100., lune.hauteur(), lune.azimut(), yf);

        const int lpol = 100;
        const int bpol = qRound(100 - 100 * (1. - Configuration::instance()->observateurs().first().latitude() * MATHS::DEUX_SUR_PI));

        QPixmap pixlun;
        pixlun.load(":/resources/interface/lune.png");
        pixlun = pixlun.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun, blun);
        transform.rotate(180. - QLineF(llun, blun, lpol, bpol).normalVector().angle());
        if (settings.value("affichage/rotationLune").toBool() && (Configuration::instance()->observateurs().first().latitude() < 0.)) {
            transform.rotate(180.);
        }

        transform.translate(-8, -8);
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
    QPainterPath res;
    QPolygonF poly;
    QVector<QPolygonF> traces;
    QVector<QColor> couleurs;

    QPen pen;
    pen.setWidthF(1.2);

    QListIterator it1(Configuration::instance()->listeSatellites());
    it1.toBack();

    while (it1.hasPrevious()) {

        poly.clear();
        couleurs.clear();
        traces.clear();

        const Satellite sat = it1.previous();

        if (sat.visible() && (sat.altitude() >= 0.) && settings.value("affichage/afftraceCiel").toBool() && !sat.traceCiel().isEmpty()) {

            const ElementsTraceCiel trace0 = sat.traceCiel().first();
            const QPointF coord0(trace0.hauteur, trace0.azimut);
            const QPointF pt0(TOPO2X(100., coord0.x(), coord0.y(), xf), TOPO2Y(100., coord0.x(), coord0.y(), yf));

            poly.append(pt0);
            couleurs.append(Ciel::CouleurTraceCiel(trace0));

            QListIterator it2(sat.traceCiel());
            it2.next();
            while (it2.hasNext()) {

                const ElementsTraceCiel trace = it2.next();

                const QPointF coord(trace.hauteur, trace.azimut);
                const QPointF ptActuel(TOPO2X(100., coord.x(), coord.y(), xf), TOPO2Y(100., coord.x(), coord.y(), yf));

                const QColor couleurActuel = Ciel::CouleurTraceCiel(trace);
                const QColor couleurPrec = couleurs.last();

                // Gestion de la couleur de la trace dans le ciel
                if (couleurActuel != couleurPrec) {

                    traces.append(poly);
                    couleurs.append(couleurActuel);
                    poly.clear();
                    poly.append(traces.last().last());
                }

                poly.append(ptActuel);
            }

            traces.append(poly);

            // Dessin de la trace dans le ciel
            for(unsigned int i=0; i<traces.size(); i++) {

                res.clear();
                res.addPolygon(traces[i]);
                QGraphicsPathItem * const path = new QGraphicsPathItem(res);

                pen.setColor(couleurs[i]);
                pen.setCosmetic(true);
                path->setPen(pen);
                scene->addItem(path);
            }
        }

        // Calcul des coordonnees radar du satellite
        const int lsat = TOPO2X(100., sat.hauteur(), sat.azimut(), xf);
        const int bsat = TOPO2Y(100., sat.hauteur(), sat.azimut(), yf);

        rectangle = QRect(lsat - 3, bsat - 3, 6, 6);

        if (sat.conditionEclipse().eclipseTotale()) {
            couleur = crimson;
        } else if (sat.conditionEclipse().eclipsePartielle() || sat.conditionEclipse().eclipseAnnulaire()) {
            couleur = Qt::green;
        } else {
            couleur = Qt::yellow;
        }

        scene->addEllipse(rectangle, noir, QBrush(couleur, Qt::SolidPattern));
    }

    scene->addEllipse(-26, -26, 251, 251, QPen(QBrush(palette().window().color()), 56));
    scene->addEllipse(1, 1, 197, 197, QPen(QBrush(Qt::gray), 3));

    _ui->vueRadar->setScene(scene);
    _ui->vueRadar->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

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

