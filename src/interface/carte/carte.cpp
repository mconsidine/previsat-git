/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    interface.carte
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 decembre 2019
 *
 * Date de revision
 * >    20 mai 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QSettings>
#include <QXmlStreamReader>
#include "ui_options.h"
#include "ui_carte.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "carte.h"
#include "coordiss.h"
#include "itemgroup.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "interface/ciel/ciel.h"
#include "interface/options/options.h"
#include "librairies/exceptions/previsatexception.h"

#define DEG2PX(x) (_ui->carte->width() * (x) / MATHS::T360)
#define ECHELLE_MAX (2.)
#define FACTEUR_ECHELLE (1.05)


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


// SAA
static constexpr std::array<std::array<double, 2>, 59> tabSAA = {
    { { -96.5, -29. }, { -95., -24.5 }, { -90., -16. }, { -85., -10. }, { -80., -6. }, { -75., -3.5 }, { -70., 0. }, { -65., 4. }, { -60., 6.5 },
      { -55., 8. }, { -50., 9. }, { -45., 10. }, { -40., 11. }, { -35., 12. }, { -30., 13. }, { -25., 12. }, { -20., 9.5 }, { -15., 8. }, { -10., 7. },
      { -5., 6. }, { 0., 4. }, { 5., 2. }, { 10., -3. }, { 15., -4. }, { 20., -5. }, { 25., -6. }, { 30., -8. }, { 35., -11.5 }, { 40., -14. },
      { 42.5, -17.5 }, { 40., -22. }, { 35., -23.5 }, { 30., -25. }, { 25., -27. }, { 20., -29. }, { 15., -32. }, { 10., -33.5 }, { 5., -35. },
      { 0., -36. }, { -5., -37. }, { -10., -38.5 }, { -15., -42.5 }, { -20., -44.5 }, { -25., -46. }, { -30., -47.5 }, { -35., -48.5 }, { -40., -49.5 },
      { -45., -49. }, { -50., -48.5 }, { -55., -47.5 }, { -60., -47. }, { -65., -46.5 }, { -70., -45.5 }, { -75., -43.5 }, { -80., -42. },
      { -85., -38.5 }, { -90., -36. }, { -95., -33. }, { -96.5, -29. } }
};

// SAA pour la visualisation Wall Command Center
static constexpr std::array<std::array<double, 2>, 16> tabSAA_ISS = {
    { { 55.5, -17.3 }, { 47., -17.3 }, { 34.3, -20. }, { 14.5, -28. }, { -16., -31.6 }, { -26.5, -35.5 }, { -28.3, -40.6 }, { -21.6, -45.6 },
      { 2.5, -53. }, { 42., -53. }, { 54., -47.6 }, { 62.2, -36. }, { 63.3, -31. }, { 63.3, -24. }, { 60.5, -19.2 }, { 55.5, -17.3 } }
};


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Carte::Carte(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Carte)
{
    _ui->setupUi(this);
    scene = nullptr;

    try {

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Carte::~Carte()
{
    EFFACE_OBJET(scene);
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
void Carte::mouseMoveEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->carte->setCursor(Qt::ArrowCursor);
    _ui->carte->viewport()->setCursor(Qt::ArrowCursor);

    const int xCur = static_cast<int> (evt->position().x() + 1);
    const int yCur = static_cast<int> (evt->position().y() + 1);

    /* Corps de la methode */
    if (settings.value("affichage/affcoord").toBool()) {

        // Longitude
        const double lo0 = 180. - xCur * MATHS::T360 / _largeurCarte;
        const QString ew = (lo0 < 0.) ? tr("Est") : tr("Ouest");

        // Latitude
        const double la0 = 90. - yCur * MATHS::T360 / _largeurCarte;
        const QString ns = (la0 < 0.) ? tr("Sud") : tr("Nord");

        // Affichage des coordonnees dans la barre de statut
        emit AfficherMessageStatut2(tr("Longitude : %1° %2").arg(fabs(lo0), 6, 'f', 2, QChar('0')).arg(ew));
        emit AfficherMessageStatut3(tr("Latitude : %1° %2").arg(fabs(la0), 6, 'f', 2, QChar('0')).arg(ns));
    }

    // Survol d'un satellite avec le curseur
    QListIterator it(Configuration::instance()->listeSatellites());
    bool atrouve = false;
    while (it.hasNext() && !atrouve) {

        const Satellite sat = it.next();
        const int lsat = qRound(DEG2PX(180. - sat.longitude() * MATHS::RAD2DEG)) + 2;
        const int bsat = qRound(DEG2PX(90. - sat.latitude() * MATHS::RAD2DEG)) + 2;

        // Distance au carre du curseur au satellite
        const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

        // Le curseur est au(dessus d'un satellite
        if ((dt <= 16) && (sat.altitude() > 0.)) {

            atrouve = true;
            setToolTip(tr("<font color='blue'><b>%1</b></font><br />NORAD : <b>%2</b><br />COSPAR : <b>%3</b>")
                       .arg(sat.elementsOrbitaux().nom).arg(sat.elementsOrbitaux().norad).arg(sat.elementsOrbitaux().cospar));

            emit AfficherMessageStatut(tr("<b>%1</b> (numéro NORAD : <b>%2</b>  -  COSPAR : <b>%3</b>)")
                                       .arg(sat.elementsOrbitaux().nom).arg(sat.elementsOrbitaux().norad).arg(sat.elementsOrbitaux().cospar));
            setCursor(Qt::CrossCursor);
            _ui->carte->viewport()->setCursor(Qt::CrossCursor);
        } else {

            emit EffacerMessageStatut();
            setToolTip("");
            setCursor(Qt::ArrowCursor);
            _ui->carte->viewport()->setCursor(Qt::ArrowCursor);
        }
    }

    // Survol du Soleil avec le curseur
    if (settings.value("affichage/affsoleil").toBool()) {

        static bool asoleil = false;
        const int lsol = qRound(DEG2PX(180. - Configuration::instance()->soleil().longitude() * MATHS::RAD2DEG)) + 2;
        const int bsol = qRound(DEG2PX(90. - Configuration::instance()->soleil().latitude() * MATHS::RAD2DEG)) + 2;

        // Distance au carre du curseur au Soleil
        const int dt = (xCur - lsol) * (xCur - lsol) + (yCur - bsol) * (yCur - bsol);

        // Le curseur est au-dessus du Soleil
        if (dt <= 81) {
            emit AfficherMessageStatut(tr("Soleil"));
            setToolTip(tr("Soleil"));
            setCursor(Qt::CrossCursor);
            _ui->carte->viewport()->setCursor(Qt::CrossCursor);
            asoleil = true;
        } else if (asoleil) {
            emit EffacerMessageStatut();
            setToolTip("");
            setCursor(Qt::ArrowCursor);
            _ui->carte->viewport()->setCursor(Qt::ArrowCursor);
            asoleil = false;
        }
    }

    // Survol de la Lune avec le curseur
    if (settings.value("affichage/afflune").toBool()) {

        static bool alune = false;
        const int llun = qRound(DEG2PX(180. - Configuration::instance()->lune().longitude() * MATHS::RAD2DEG)) + 2;
        const int blun = qRound(DEG2PX(90. - Configuration::instance()->lune().latitude() * MATHS::RAD2DEG)) + 2;

        // Distance au carre du curseur a la Lune
        const int dt = (xCur - llun) * (xCur - llun) + (yCur - blun) * (yCur - blun);

        // Le curseur est au-dessus de la Lune
        if (dt <= 81) {
            emit AfficherMessageStatut(tr("Lune"));
            setToolTip(tr("Lune"));
            setCursor(Qt::CrossCursor);
            _ui->carte->viewport()->setCursor(Qt::CrossCursor);
            alune = true;

        } else if (alune) {
            emit EffacerMessageStatut();
            setToolTip("");
            setCursor(Qt::ArrowCursor);
            _ui->carte->viewport()->setCursor(Qt::ArrowCursor);
            alune = false;
        }
    }

    /* Retour */
    return;
}

void Carte::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (evt->button() == Qt::LeftButton) {

        const int xCur = static_cast<int> (evt->position().x() + 1);
        const int yCur = static_cast<int> (evt->position().y() + 1);

        // Clic sur un satellite
        QListIterator it(Configuration::instance()->listeSatellites());
        bool atrouve = false;
        int idx = 1;
        while (it.hasNext() && !atrouve) {

            const Satellite sat = it.next();
            const int lsat = qRound(DEG2PX(180. - sat.longitude() * MATHS::RAD2DEG)) + 2;
            const int bsat = qRound(DEG2PX(90. - sat.latitude() * MATHS::RAD2DEG)) + 2;

            // Distance au carre du curseur au satellite
            const int dt = (xCur - lsat) * (xCur - lsat) + (yCur - bsat) * (yCur - bsat);

            // Le curseur est au-dessus d'un satellite
            if ((dt <= 16) && (sat.altitude() > 0.)) {

                atrouve = true;

                Configuration::instance()->listeSatellites().move(--idx, 0);
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

    /* Retour */
    return;
}

void Carte::resizeEvent(QResizeEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    const int href = 2 * height() - 1;
    const int lref = width();
    const int lc = qMin(href, lref);
    const int hc = (lc + 1) / 2;
    _ui->carte->setGeometry((width() - lc) / 2, 0, lc, hc);
    _resize = true;

    ChargementCarteDuMonde();
    show();

    /* Retour */
    return;
}

void Carte::wheelEvent(QWheelEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->carte->underMouse()) {

        static double echelle = 1.;

        if (evt->angleDelta().y() < 0) {

            if (echelle > 1.) {

                _ui->carte->scale(1. / FACTEUR_ECHELLE, 1. / FACTEUR_ECHELLE);
                echelle /= FACTEUR_ECHELLE;
            }

        } else if (echelle < ECHELLE_MAX) {
            _ui->carte->scale(FACTEUR_ECHELLE, FACTEUR_ECHELLE);
            echelle *= FACTEUR_ECHELLE;
        }

        emit SendCurrentScale(echelle);
        _ui->carte->update();
        _ui->carte->viewport()->update();
    }

    /* Retour */
    return;
}


/*
 * Affichage des courbes sur la carte du monde
 */
void Carte::show()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _mcc = (Configuration::instance()->issLive() && settings.value("affichage/styleWCC").toBool());
    scene->clear();
    _groupes.clear();

    /* Corps de la methode */
    scene->addPixmap(_pixMap);

    // Affichage des frontieres
    AffichageFrontieres();

    // Affichage de la grille de coordonnees
    AffichageGrille();

    // Affichage du Soleil
    AffichageSoleil();

    // Zone d'ombre
    AffichageZoneOmbre();

    // Affichage de la ZOE et de la SAA pour le Wall Command Center
    AffichageSAA_ZOE();

    // Affichage des lieux d'observation
    AffichageLieuxObservation();

    // Affichage des stations
    AffichageStations();

    // Affichage de la Lune
    AffichageLune();

    // Affichage de la trace au sol du satellite par defaut
    AffichageTraceAuSol();

    // Affichage de la zone de visibilite des satellites
    AffichageZoneVisibilite();

    // Affichage des satellites
    AffichageSatellites();

    _ui->carte->setScene(scene);
    _ui->carte->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

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

        const int lobs = qRound(DEG2PX(180. - siteLancement.longitude() * MATHS::RAD2DEG))+1;
        const int bobs = qRound(DEG2PX(90. - siteLancement.latitude() * MATHS::RAD2DEG))+1;

        scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
        scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

        QGraphicsSimpleTextItem * const txtObs = new QGraphicsSimpleTextItem(acronyme);
        const int lng = static_cast<int> (txtObs->boundingRect().width());
        const int xnobs = ((lobs + 7 + lng) > _largeurCarte) ? lobs - lng - 1 : lobs + 4;
        const int ynobs = ((bobs + 12) > _hauteurCarte) ? bobs - 12 : bobs;

        txtObs->setBrush(Qt::white);
        txtObs->setPos(xnobs, ynobs);
        scene->addItem(txtObs);
    }

    /* Retour */
    return;
}

/*
 * Chargement de la carte du monde
 */
void Carte::ChargementCarteDuMonde()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString map = settings.value("fichier/nomMap").toString().trimmed();
    const QString nomMap = (map.isEmpty()) ? ":/resources/interface/map.png" : Configuration::instance()->dirMap() + QDir::separator() + map;

    /* Corps de la methode */
    _pixMap.fill(palette().window().color());
    _pixMap.load(nomMap);
    _pixMap = _pixMap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    _largeurCarte = _ui->carte->width() - 2;
    _hauteurCarte = _ui->carte->height() - 2;

    scene->setSceneRect(0, 0, _largeurCarte, _hauteurCarte);

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
 * Affichage des frontieres
 */
void Carte::AffichageFrontieres()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (settings.value("affichage/affichageFrontieres").toBool() && !_mcc) {

        try {

            // Lecture des fichiers de frontieres
            LectureFichierKml(Configuration::instance()->dirBnd() + QDir::separator() + "countries.kml");

            for(unsigned int i=0; i<_groupes.size(); i++) {
                scene->addItem(_groupes.at(i));
            }

        } catch (PreviSatException const &e) {
            throw PreviSatException();
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage de la grille de coordonnees
 */
void Carte::AffichageGrille()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _largeurParalleles = _largeurCarte - 24.;
    _hauteurMeridiens = _hauteurCarte - 24.;

    _maxMeridiens = 12;
    _maxParalleles = 6;
    _stepMeridiens = 30.;
    _stepParalleles = 30.;

    if (_mcc) {
        _maxMeridiens *= 2;
        _maxParalleles *= 2;
        _stepMeridiens /= 2.;
        _stepParalleles /= 2.;
    }

    /* Corps de la methode */
    if (settings.value("affichage/affgrille").toBool()) {

        const bool conditionIss = (!Configuration::instance()->listeSatellites().isEmpty() && _mcc
                                   && (Configuration::instance()->listeSatellites().first().elementsOrbitaux().norad ==
                                       Configuration::instance()->noradStationSpatiale()));

        QPen pen = QPen((conditionIss) ? ((settings.value("affichage/coulEquateur").toInt() == 0) ? Qt::red : Qt::white) : Qt::white);
        pen.setCosmetic(true);

        QPen pen2(QBrush(Qt::lightGray), 1.);
        pen2.setCosmetic(true);

        // Meridiens
        double ht = _hauteurMeridiens;
        for(unsigned int i=1; i<_maxMeridiens; i++) {

            const double x = DEG2PX(_stepMeridiens * i);

            if (Configuration::instance()->issLive()) {
                ht = (((i % 2) == 0) || (i == 11) || (i == 13)) ? _hauteurMeridiens : _hauteurCarte;
            }

            scene->addLine(x, 0., x, ht, (i == (_maxMeridiens / 2)) ? pen : pen2);
        }

        // Paralleles
        double lg = _largeurParalleles;
        for(unsigned int i=1; i<_maxParalleles; i++) {

            const double y = DEG2PX(_stepParalleles * i);

            if (Configuration::instance()->issLive()) {
                lg = (((i % 2) == 0) || (i == 5) || (i == 7)) ? _largeurParalleles : _largeurCarte;
            }

            scene->addLine(0., y, lg, y, (i == (_maxParalleles / 2)) ? pen : pen2);
        }

        if (!_mcc) {

            // Tropiques
            pen2.setStyle(Qt::DashDotLine);
            scene->addLine(0, DEG2PX(66.55), _largeurCarte, DEG2PX(66.55), pen2);
            scene->addLine(0, DEG2PX(113.45), _largeurCarte, DEG2PX(113.45), pen2);

            // Cercles polaires
            scene->addLine(0, DEG2PX(23.45), _largeurCarte, DEG2PX(23.45), pen2);
            scene->addLine(0, DEG2PX(156.55), _largeurCarte, DEG2PX(156.55), pen2);
        }
    }

    // Affichage de la SAA
    if (settings.value("affichage/affSAA").toBool() && !Configuration::instance()->issLive()) {

        const QBrush alpha = QBrush(QColor::fromRgb(255, 0, 0, 50));
        QVector<QPointF> zoneSAA;
        for(int i=0; i<59; i++) {
            zoneSAA.append(QPointF(DEG2PX(180. + tabSAA[i][0]), DEG2PX(90. - tabSAA[i][1])));
        }

        const QPolygonF poly(zoneSAA);
        scene->addPolygon(poly, QPen(Qt::NoBrush, 0), alpha);
    }

    /* Retour */
    return;
}

/*
 * Affichage des lieux d'observation
 */
void Carte::AffichageLieuxObservation()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QPen crayon(Qt::white);
    crayon.setCosmetic(true);

    /* Corps de la methode */
    QGraphicsSimpleTextItem * txtObs;
    const int nbMax = static_cast<int> ((settings.value("affichage/affnomlieu") == QVariant(Qt::Unchecked)) ?
                                            0 : Configuration::instance()->observateurs().size() - 1);
    for(int j=nbMax; j>=0; j--) {

        const int lobs = qRound(DEG2PX(180. - Configuration::instance()->observateurs().at(j).longitude() * MATHS::RAD2DEG))+1;
        const int bobs = qRound(DEG2PX(90. - Configuration::instance()->observateurs().at(j).latitude() * MATHS::RAD2DEG))+1;

        scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
        scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

        if ((j == 0) || (settings.value("affichage/affnomlieu") == QVariant(Qt::Checked))) {

            txtObs = new QGraphicsSimpleTextItem(Configuration::instance()->observateurs().at(j).nomlieu());
            const int lng = static_cast<int> (txtObs->boundingRect().width());
            const int xnobs = ((lobs + 7 + lng) > _largeurCarte) ? lobs - lng - 1 : lobs + 4;
            const int ynobs = ((bobs + 12) > _hauteurCarte) ? bobs - 12 : bobs;

            txtObs->setBrush(Qt::white);
            txtObs->setPos(xnobs, ynobs);
            scene->addItem(txtObs);
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage de la Lune
 */
void Carte::AffichageLune()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (settings.value("affichage/afflune").toBool() && !_mcc) {

        QPixmap pixlun;
        pixlun.load(":/resources/interface/lune.png");
        pixlun = pixlun.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        const int llun = qRound(DEG2PX(180. - Configuration::instance()->lune().longitude() * MATHS::RAD2DEG));
        const int blun = qRound(DEG2PX(90. - Configuration::instance()->lune().latitude() * MATHS::RAD2DEG));

        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        QTransform transform2;
        transform.translate(llun, blun);
        if (settings.value("affichage/rotationLune").toBool() && (Configuration::instance()->observateur().latitude() < 0.)) {
            transform.rotate(180.);
        }

        transform.translate(-7, -7);
        lun->setTransform(transform);

        // Lune au niveau du meridien 180 degres, on duplique le dessin
        if (((llun + 7) > _largeurCarte) || ((llun - 7) < 0)) {

            QGraphicsPixmapItem * const lun2 = scene->addPixmap(pixlun);

            if ((llun + 7) > _largeurCarte) {
                transform2.translate(llun - _largeurCarte, blun);
            }

            if ((llun - 7) < 0) {
                transform2.translate(llun + _largeurCarte, blun);
            }

            if (settings.value("affichage/rotationLune").toBool() && (Configuration::instance()->observateur().latitude() < 0.)) {
                transform2.rotate(180.);
            }

            transform2.translate(-7, -7);
            lun2->setTransform(transform2);
        }

        if (settings.value("affichage/affphaselune").toBool()) {

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

    /* Retour */
    return;
}

/*
 * Affichage par defaut d'un satellite (sans icone)
 */
void Carte::AffichageSatelliteDefaut(const Satellite &satellite, const int lsat, const int bsat) const
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
    if (settings.value("affichage/affnomsat") != QVariant(Qt::Unchecked)) {

        const QVariant etat = settings.value("affichage/affnomsat");
        if (((etat == QVariant(Qt::PartiallyChecked)) &&
             (satellite.elementsOrbitaux().norad == Configuration::instance()->listeSatellites().first().elementsOrbitaux().norad)) ||
                (etat == QVariant(Qt::Checked))) {

            QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(satellite.elementsOrbitaux().nom);
            const int lng = static_cast<int> (txtSat->boundingRect().width());
            const int xnsat = (lsat + 4 + lng > _largeurCarte) ? lsat - lng - 1 : lsat + 4;
            const int ynsat = (bsat + 9 > _hauteurCarte) ? bsat - 12 : bsat;

            txtSat->setBrush(Qt::white);
            txtSat->setPos(xnsat, ynsat);
            scene->addItem(txtSat);
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage des satellites
 */
void Carte::AffichageSatellites()
{
    /* Declarations des variables locales */
    double angle;
    QStringList listeIcones;
    QPixmap img;
    QTransform transform;
    QGraphicsPixmapItem * pm;
    QGraphicsPixmapItem * pm2;

    /* Initialisations */
    const QList<Satellite> &satellites = Configuration::instance()->listeSatellites();

    /* Corps de la methode */
    for(int isat=static_cast<int> (satellites.size()-1); isat>=0; isat--) {

        if (satellites.at(isat).altitude() >= 0.) {

            const int lsat = qRound(DEG2PX(180. - satellites.at(isat).longitude() * MATHS::RAD2DEG))+1;
            const int bsat = qRound(DEG2PX(90. - satellites.at(isat).latitude() * MATHS::RAD2DEG))+1;

            if (_mcc || settings.value("affichage/afficone").toBool()) {

                // Affichage de l'icone du satellite a partir du numero NORAD ou du nom
                const QString norad = satellites.at(isat).elementsOrbitaux().norad;
                const QString nomsat = satellites.at(isat).elementsOrbitaux().nom.section(" ", 0, 0);

                const QDir di(Configuration::instance()->dirRsc());
                const QDir di2(":/resources/icones");
                const QStringList filtre(QStringList () << norad + ".png" << nomsat + ".png");

                // L'icone de l'utilisateur est prioritaire sur l'icone par defaut
                listeIcones = di.entryList(filtre, QDir::Files);
                if (listeIcones.isEmpty()) {
                    listeIcones = di2.entryList(filtre, QDir::Files).replaceInStrings(QRegularExpression("^"), di2.path() + "/");
                } else {
                    listeIcones.replaceInStrings(QRegularExpression("^"), di.path() + QDir::separator());
                }

                if (listeIcones.isEmpty()) {

                    // L'icone du satellite n'a pas ete trouvee, affichage par defaut
                    AffichageSatelliteDefaut(satellites.at(isat), lsat, bsat);

                } else {

                    // Affichage de l'icone satellite
                    img = QPixmap(listeIcones.first());
                    img = img.scaled(qMin(_largeurCarte / 12, img.width()), qMin(_hauteurCarte / 6, img.height()), Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);

                    pm = scene->addPixmap(img);

                    transform.reset();
                    transform.translate(lsat, bsat);

                    // Rotation de l'icone de l'ISS
                    angle = 0.;
                    if (settings.value("affichage/rotationIconeISS").toBool() &&
                            (satellites.at(isat).elementsOrbitaux().norad == Configuration::instance()->noradStationSpatiale())) {

                        const double vxsat = satellites.at(isat).vitesse().x();
                        const double vysat = satellites.at(isat).vitesse().y();
                        const double vzsat = satellites.at(isat).vitesse().z();

                        angle = MATHS::RAD2DEG * (-atan(vzsat / sqrt(vxsat * vxsat + vysat * vysat)));
                        transform.rotate(angle);
                    }

                    transform.translate(-img.width() / 2, -img.height() / 2);
                    pm->setTransform(transform);

                    // Icone sur le bord de la carte du monde
                    pm2 = scene->addPixmap(img);
                    transform.reset();

                    if (lsat > _largeurCarte / 2) {
                        transform.translate(lsat - _largeurCarte, bsat);
                    } else {
                        transform.translate(lsat + _largeurCarte, bsat);
                    }

                    transform.rotate(angle);
                    transform.translate(-img.width() / 2, -img.height() / 2);
                    pm2->setTransform(transform);
                }
            } else {
                AffichageSatelliteDefaut(satellites.at(isat), lsat, bsat);
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage du Soleil
 */
void Carte::AffichageSoleil()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const Soleil &soleil = Configuration::instance()->soleil();
    _lsol = qRound(DEG2PX(180. - soleil.longitude() * MATHS::RAD2DEG));
    const int bsol = qRound(DEG2PX(90. - soleil.latitude() * MATHS::RAD2DEG));

    if (settings.value("affichage/affsoleil").toBool()) {

        const QString iconeSoleil = (_mcc) ? ":/resources/icones/soleil.png" : ":/resources/interface/soleil.png";

        QPixmap pixsol;
        pixsol.load(iconeSoleil);

        QTransform transform;
        transform.translate(_lsol, bsol);

        if (_mcc) {
            transform.translate(-15, -10);
        } else {
            pixsol = pixsol.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            transform.translate(-7, -7);
        }

        QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
        sol->setTransform(transform);

        // Soleil au niveau du meridien 180 degres, on duplique le dessin
        if (((_lsol + 7) > _largeurCarte) || ((_lsol - 7) < 0)) {

            QGraphicsPixmapItem * const sol2 = scene->addPixmap(pixsol);
            transform.reset();

            if ((_lsol + 7) > _largeurCarte) {
                transform.translate(_lsol - _largeurCarte, bsol);
            }

            if ((_lsol - 7) < 0) {
                transform.translate(_lsol + _largeurCarte, bsol);
            }

            transform.translate(-7, -7);
            sol2->setTransform(transform);
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage des stations
 */
void Carte::AffichageStations()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->issLive()) {

        QPainterPath res;
        QPen crayon(Qt::yellow);
        crayon.setWidthF(1.2);
        crayon.setCosmetic(true);

        QPen crayon2 = (settings.value("affichage/styleWCC").toBool()) ? QPen(Qt::yellow, 2.) : crayon;
        crayon2.setCosmetic(true);

        QMapIterator it(Configuration::instance()->mapStations());
        while (it.hasNext()) {
            it.next();

            const QString acronyme = it.key();

            if (settings.value("affichage/station" + acronyme).toBool()) {

                const Observateur station = Configuration::instance()->mapStations()[acronyme];

                const int lsta = qRound(DEG2PX(180. - station.longitude() * MATHS::RAD2DEG));
                const int bsta = qRound(DEG2PX(90. - station.latitude() * MATHS::RAD2DEG));

                scene->addLine(lsta-4, bsta, lsta+4, bsta, crayon);
                scene->addLine(lsta, bsta-4, lsta, bsta+4, crayon);

                QGraphicsSimpleTextItem * const txtSta = new QGraphicsSimpleTextItem(acronyme);
                const QFont policeSta(settings.value("affichage/policeWCC").toString(), 10);
                txtSta->setFont(policeSta);

                const int lng = (int) txtSta->boundingRect().width();
                const int xnsta = lsta - lng / 2 + 1;
                const int ynsta = (bsta > 16) ? bsta - 16 : bsta + 3;

                txtSta->setBrush(Qt::yellow);
                txtSta->setPos(xnsta, ynsta);
                scene->addItem(txtSta);

                if (settings.value("affichage/affCerclesAcq").toBool() && !Configuration::instance()->listeSatellites().isEmpty()
                        && (Configuration::instance()->listeSatellites().first().altitude() > 0.)
                        && (Configuration::instance()->listeSatellites().first().elementsOrbitaux().norad ==
                            Configuration::instance()->noradStationSpatiale())) {

                    Satellite sat = Configuration::instance()->listeSatellites().first();
                    sat.CalculCercleAcquisition(station);

                    QPolygonF poly;
                    QVector<QPolygonF> zones;

                    const QPointF pt0(QPointF(DEG2PX(sat.zone().at(0).x()), DEG2PX(sat.zone().at(0).y())));
                    poly.append(pt0);

                    for(auto pt = std::begin(sat.zone()) + 1; pt != std::end(sat.zone()); ++pt) {

                        const QPointF ptActuel(DEG2PX(pt->x()), DEG2PX(pt->y()));
                        const QPointF ptPrec = poly.last();

                        // Gestion du meridien 180 degres
                        if (fabs(ptActuel.x() - ptPrec.x()) > (_largeurCarte / 2)) {

                            const int sgn = (ptActuel.x() < ptPrec.x()) ? 1 : -1;
                            poly.append(QPointF(ptActuel.x() + sgn * _largeurCarte, ptActuel.y()));
                            zones.append(poly);

                            poly.clear();
                            poly.append(QPointF(ptActuel.x() - sgn * _largeurCarte, ptActuel.y()));
                        }

                        poly.append(ptActuel);
                    }

                    if (fabs(poly.last().x() - pt0.x()) < (_largeurCarte / 2)) {
                        poly.append(pt0);
                    }

                    zones.append(poly);

                    QVectorIterator it2(zones);
                    while (it2.hasNext()) {

                        res.clear();
                        res.addPolygon(it2.next());

                        QGraphicsPathItem * const path = new QGraphicsPathItem(res);
                        path->setPen(crayon2);
                        scene->addItem(path);
                    }
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage de la trace au sol du satellite par defaut
 */
void Carte::AffichageTraceAuSol()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (settings.value("affichage/afftraj").toBool() || _mcc) {

        if (Configuration::instance()->listeSatellites().size() > 0) {

            const Satellite &satellite = Configuration::instance()->listeSatellites().first();
            const QList<ElementsTraceSol> traceAuSol = satellite.traceAuSol();

            if ((satellite.altitude() >= 0.) && (!traceAuSol.isEmpty())) {

                QString txt;
                QPolygonF poly;
                QVector<QPolygonF> traces;
                QVector<QColor> couleurs;

                QListIterator it(traceAuSol);
                const ElementsTraceSol t = it.next();
                poly.append(QPointF(DEG2PX(t.longitude), DEG2PX(t.latitude)));
                couleurs.append(CouleurTraceAuSol(t));

                ElementsTraceSol tracePrec = t;

                while (it.hasNext()) {

                    const ElementsTraceSol trace = it.next();

                    const QPointF ptActuel(DEG2PX(trace.longitude), DEG2PX(trace.latitude));
                    const QPointF ptPrec = poly.last();

                    const QColor couleurActuel = CouleurTraceAuSol(trace);
                    const QColor couleurPrec = couleurs.last();

                    // Gestion du meridien 180 degres
                    if (fabs(ptActuel.x() - ptPrec.x()) > (_largeurCarte / 2)) {

                        const int sgn = (ptActuel.x() < ptPrec.x()) ? 1 : -1;
                        poly.append(QPointF(ptActuel.x() + sgn * _largeurCarte, ptActuel.y()));
                        traces.append(poly);
                        couleurs.append(couleurActuel);

                        poly.clear();
                        poly.append(QPointF(ptActuel.x() - sgn * _largeurCarte, ptActuel.y()));
                    }

                    // Gestion de la couleur de la trace au sol
                    if (couleurActuel != couleurPrec) {

                        traces.append(poly);
                        couleurs.append(couleurActuel);
                        poly.clear();

                        if (fabs(ptActuel.x() - ptPrec.x()) < (_largeurCarte / 2)) {
                            poly.append(ptPrec);
                        }

                        poly.append(ptActuel);
                    }

                    if (Configuration::instance()->issLive()
                            && (satellite.elementsOrbitaux().norad == Configuration::instance()->noradStationSpatiale())) {

                        // Affichage du numero d'orbite (WCC)
                        if ((trace.latitude < 90.) && (tracePrec.latitude > 90.)) {

                            const Date dateISS(Date(trace.jourJulienUTC, 0., false));
                            const int numOrb = CoordISS::CalculNumeroOrbiteISS(dateISS);

                            QGraphicsSimpleTextItem * const txtOrb = new QGraphicsSimpleTextItem(QString::number(numOrb));

                            const QFont policeOrb(Configuration::instance()->police().family(), 10,
                                                  (settings.value("affichage/styleWCC").toBool()) ? QFont::Bold : QFont::Normal);
                            txtOrb->setFont(policeOrb);
                            txtOrb->setBrush(Qt::white);

                            const int lng = static_cast<int> (txtOrb->boundingRect().width());
                            const double xnorb = (ptActuel.x() - lng < 0) ? ptActuel.x() + _largeurCarte - lng - 8 : ptActuel.x() - lng;
                            txtOrb->setPos(xnorb, _largeurCarte / 4 - 18);
                            scene->addItem(txtOrb);
                        }

                        // Affichage des crochets des transitions jour/nuit (WCC)
                        if (settings.value("affichage/styleWCC").toBool()) {

                            txt = "";
                            if (trace.eclipseTotale && !tracePrec.eclipseTotale) {
                                txt = "[";
                            }

                            if (!trace.eclipseTotale && tracePrec.eclipseTotale) {
                                txt = "]";
                            }

                            if (!txt.isEmpty()) {

                                const QLineF lig(ptActuel, ptPrec);
                                const double ang = fmod(180. - lig.angle(), MATHS::T360);

#if defined (Q_OS_MAC)
                                const QFont policeOmb(Configuration::instance()->police().family(), 24);
#else
                                const QFont policeOmb(Configuration::instance()->police().family(), 14);
#endif

                                QGraphicsSimpleTextItem * const txtOmb = new QGraphicsSimpleTextItem(txt);

                                txtOmb->setFont(policeOmb);
                                txtOmb->setBrush(Qt::white);
                                const double dy = txtOmb->boundingRect().height() / 2. + 2.;
                                txtOmb->setPos(ptActuel.x(), ptActuel.y() - dy);
                                txtOmb->setTransformOriginPoint(0, dy);
                                txtOmb->setRotation(ang);
                                scene->addItem(txtOmb);
                            }
                        }
                    }

                    poly.append(ptActuel);
                    tracePrec = trace;
                }

                traces.append(poly);

                QPainterPath res;
                QPen pen;
                pen.setWidthF((_mcc && settings.value("affichage/styleWCC").toBool()) ? 2. : 1.2);

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
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage de la zone d'ombre
 */
void Carte::AffichageZoneOmbre()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QPen crayon(Qt::white);
    crayon.setCosmetic(true);

    QFont police;
    police.setPointSize(9);

    Soleil &soleil = Configuration::instance()->soleil();

    /* Corps de la methode */
    if (settings.value("affichage/affnuit") != QVariant(Qt::Unchecked)) {

        double beta = MATHS::PI_SUR_DEUX;
        const int imax = ((settings.value("affichage/affnuit") == QVariant(Qt::PartiallyChecked)) || _mcc) ? 1 : 4;

        const QBrush alpha1 = QBrush(QColor::fromRgb(0, 0, 0, static_cast<int> (2.55 * settings.value("affichage/intensiteOmbre").toDouble())));
        const QBrush alpha = (_mcc) ? QBrush(QColor::fromRgb(0, 0, 0, qMin(255, 2 * alpha1.color().alpha()))) : alpha1;

        const QPen stylo1 = (settings.value("affichage/coulTerminateur").toUInt() == 0) ?
                    QPen(QColor::fromRgb(102, 50, 16), 2) : QPen(Qt::darkYellow, 2);
        QPen stylo((_mcc) ? stylo1 : QPen(Qt::NoBrush, 0));
        stylo.setCosmetic(true);

        QVector<QPointF> zone;
        QVector<QPointF> zone1;
        QVector<QPointF> zone2;
        QList<int> idxIntersection;

        for(int i=0; i<imax; i++) {

            if (_mcc && (i != 0)) {
                stylo = QPen(Qt::NoBrush, 0);
                stylo.setCosmetic(true);
            }

            soleil.CalculZoneVisibilite(beta);

            // Coordonnees de la zone d'ombre, en pixels
            zone.clear();
            idxIntersection.clear();
            zone.append(QPointF(DEG2PX(soleil.zone().at(0).x()), DEG2PX(soleil.zone().at(0).y())));

            for(unsigned int j=1; j<soleil.zone().size(); j++) {

                zone.append(QPointF(DEG2PX(soleil.zone().at(j).x()), DEG2PX(soleil.zone().at(j).y())));

                if (fabs(zone.at(j).x() - zone.at(j-1).x()) > (_ui->carte->width() / 2)) {
                    idxIntersection.append(j);
                }
            }

            zone.append(zone[0]);
            if (fabs(zone.at(soleil.zone().size()).x() - zone.at(soleil.zone().size()-1).x()) > (_ui->carte->width() / 2)) {
                idxIntersection.append(static_cast<unsigned int> (soleil.zone().size()));
            }

            if ((fabs(soleil.latitude()) > 0.002449 * MATHS::DEG2RAD) || (i > 0)) {

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
                            zone.insert(0, QPointF(zone.last().x() - _ui->carte->width(), _hauteurCarte));
                            zone.insert(0, QPointF(zone.at(2).x() + _ui->carte->width(), _hauteurCarte));

                        } else {
                            zone.insert(0, QPointF(zone.last().x() - _ui->carte->width(), -1.));
                            zone.insert(0, QPointF(zone.at(2).x() + _ui->carte->width(), -1.));
                        }
                        zone.insert(0, QPointF(zone.at(3).x() + _ui->carte->width(), zone.at(3).y()));

                    } else {

                        zone.insert(0, QPointF(zone.last().x() + _ui->carte->width(), zone.last().y()));

                        if (soleil.latitude() > 0.) {
                            zone.insert(0, QPointF(zone.last().x() + _ui->carte->width(), _hauteurCarte));
                            zone.insert(0, QPointF(zone.at(2).x() - _ui->carte->width(), _hauteurCarte));

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
                const double x1 = DEG2PX(qMin(soleil.zone().at(90).x(), soleil.zone().at(270).x()));
                const double x2 = DEG2PX(qMax(soleil.zone().at(90).x(), soleil.zone().at(270).x()));

                if ((_lsol > (_largeurCarte / 4)) && (_lsol < (3 * _largeurCarte) / 4)) {

                    zone1.clear();
                    zone1.append(QPointF(-1., -1.));
                    zone1.append(QPointF(x1, -1.));
                    zone1.append(QPointF(x1, _ui->carte->width()));
                    zone1.append(QPointF(-1., _ui->carte->width()));

                    zone2.clear();
                    zone2.append(QPointF(_largeurCarte, -1.));
                    zone2.append(QPointF(x2, -1.));
                    zone2.append(QPointF(x2, _ui->carte->width()));
                    zone2.append(QPointF(_largeurCarte, _ui->carte->width()));

                    const QPolygonF poly1(zone1);
                    const QPolygonF poly2(zone2);
                    scene->addPolygon(poly1, stylo, alpha);
                    scene->addPolygon(poly2, stylo, alpha);

                } else {

                    zone1.clear();
                    zone1.append(QPointF(x1, -1.));
                    zone1.append(QPointF(x1, _ui->carte->width()));
                    zone1.append(QPointF(x2, _ui->carte->width()));
                    zone1.append(QPointF(x2, -1.));

                    const QPolygonF poly1(zone1);
                    scene->addPolygon(poly1, stylo, alpha);
                }
            }

            beta += (6. * MATHS::DEG2RAD);
        }
    }

    if (settings.value("affichage/affgrille").toBool()) {

        // Etiquettes de longitude
        QBrush couleur((soleil.latitude() > 0.) ? Qt::lightGray : Qt::gray);
        for(unsigned int i=1; i<_maxMeridiens; i++) {

            if (((i % 2) == 0) || !_mcc) {

                const double lon = _stepMeridiens * i;
                const double x = DEG2PX(lon);
                const double val = (_mcc) ? lon - 180. : fabs(lon - 180.);

                QGraphicsSimpleTextItem * const txt = new QGraphicsSimpleTextItem(QString("%1").arg(val));
                txt->setBrush(couleur);
                txt->setFont(police);

                const double dx = txt->boundingRect().width() * 0.5;
                txt->setPos(x - dx, _hauteurMeridiens + 4.);
                scene->addItem(txt);
            }
        }

        if (!_mcc) {

            QGraphicsSimpleTextItem * txtl = new QGraphicsSimpleTextItem(tr("W", "Symbol for West"));
            txtl->setBrush(couleur);
            txtl->setFont(police);
            txtl->setPos(DEG2PX(165.) - txtl->boundingRect().width() * 0.5, _hauteurMeridiens + 4.);
            scene->addItem(txtl);

            txtl = new QGraphicsSimpleTextItem(tr("E", "Symbol for East"));
            txtl->setBrush(couleur);
            txtl->setFont(police);
            txtl->setPos(DEG2PX(195.) - txtl->boundingRect().width() * 0.5, _hauteurMeridiens + 4.);
            scene->addItem(txtl);
        }

        // Etiquettes de latitude
        for(unsigned int i=1; i<_maxParalleles; i++) {

            if (((i % 2) == 0) || !_mcc) {

                const double lat = _stepParalleles * i;
                const double y = DEG2PX(lat);
                const double val = (_mcc) ? 90. - lat : fabs(lat - 90.);

                QGraphicsSimpleTextItem * const txt = new QGraphicsSimpleTextItem(QString("%1").arg(val));
                txt->setBrush(Qt::lightGray);
                txt->setFont(police);

                const double dy = txt->boundingRect().height() * 0.5 - 0.5;
                txt->setPos(_largeurCarte - 7. - txt->boundingRect().width(), y - dy);
                scene->addItem(txt);
            }
        }

        if (!_mcc) {

            QGraphicsSimpleTextItem * txtl = new QGraphicsSimpleTextItem(tr("N", "Symbol for North"));
            txtl->setFont(police);
            txtl->setBrush(Qt::lightGray);
            txtl->setPos(_largeurCarte - 7. - txtl->boundingRect().width(), DEG2PX(75.) - txtl->boundingRect().height() * 0.5);
            scene->addItem(txtl);

            txtl = new QGraphicsSimpleTextItem(tr("S", "Symbol for South"));
            txtl->setFont(police);
            txtl->setBrush(Qt::lightGray);
            txtl->setPos(_largeurCarte - 7. - txtl->boundingRect().width(), DEG2PX(105.) - txtl->boundingRect().height() * 0.5);
            scene->addItem(txtl);
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage de la ZOE et de la SAA pour le Wall Command Center
 */
void Carte::AffichageSAA_ZOE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (settings.value("affichage/affSAA_ZOE").toBool() && Configuration::instance()->issLive()
            && !Configuration::instance()->listeSatellites().isEmpty()
            && (Configuration::instance()->listeSatellites().first().elementsOrbitaux().norad == Configuration::instance()->noradStationSpatiale())) {

        // Zone Of Exclusion (ZOE)
        QGraphicsSimpleTextItem * const txtZOE = new QGraphicsSimpleTextItem("ZOE");
        const double xnZOE = DEG2PX(252.);
        const double ynZOE = DEG2PX(66.);

        txtZOE->setBrush((settings.value("affichage/coulZOE").toUInt() == 0) ? Qt::black : Qt::white);
        const QFont policeZOE(settings.value("affichage/policeWCC").toString(), 14);
        txtZOE->setFont(policeZOE);
        const int htt = (int) txtZOE->boundingRect().height();

        txtZOE->setPos(xnZOE, ynZOE - htt);
        scene->addItem(txtZOE);

        // South Atlantic Anomaly (SAA)
        QGraphicsSimpleTextItem * const txtSAA = new QGraphicsSimpleTextItem("SAA");
        const double xnSAA = DEG2PX(150.);
        const double ynSAA = DEG2PX(125.);

        txtSAA->setBrush(Qt::white);
        const QFont policeSAA(settings.value("affichage/policeWCC").toString(), 11);
        txtSAA->setFont(policeSAA);
        txtSAA->setPos(xnSAA, ynSAA);
        scene->addItem(txtSAA);

        // Dessin du contour de la SAA
        QVector<QPointF> zoneSAA_ISS;
        for(unsigned int i=0; i<tabSAA_ISS.size(); i++) {
            zoneSAA_ISS.append(QPointF(DEG2PX(180. - tabSAA_ISS[i][0]), DEG2PX(90. - tabSAA_ISS[i][1])));
        }

        const QPolygonF poly(zoneSAA_ISS);
        scene->addPolygon(poly, QPen(Qt::white, (settings.value("affichage/styleWCC").toBool()) ? 2 : 1));
    }

    /* Retour */
    return;
}

/*
 * Affichage de la zone de visibilite des satellites
 */
void Carte::AffichageZoneVisibilite()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QList<Satellite> &satellites = Configuration::instance()->listeSatellites();

    /* Corps de la methode */
    if (!satellites.isEmpty() && ((settings.value("affichage/affvisib") != QVariant(Qt::Unchecked)) || _mcc)) {

        QPen pen;
        QPainterPath res;
        const unsigned int nbMax = static_cast<int> (settings.value("affichage/affvisib") == QVariant(Qt::PartiallyChecked) ? 1 : satellites.size());

        for(unsigned int isat=0; isat<nbMax; isat++) {

            const Satellite sat = satellites.at(isat);

            if ((sat.altitude() >= 0.) && (!sat.zone().empty())) {

                QPolygonF poly;
                QVector<QPolygonF> zones;

                const QPointF pt0(QPointF(DEG2PX(sat.zone().at(0).x()), DEG2PX(sat.zone().at(0).y())));
                poly.append(pt0);

                for(auto pt = std::begin(sat.zone()) + 1; pt != std::end(sat.zone()); ++pt) {

                    const QPointF ptActuel(DEG2PX(pt->x()), DEG2PX(pt->y()));
                    const QPointF ptPrec = poly.last();

                    // Gestion du meridien 180 degres
                    if (fabs(ptActuel.x() - ptPrec.x()) > (_largeurCarte / 2)) {

                        const int sgn = (ptActuel.x() < ptPrec.x()) ? 1 : -1;
                        poly.append(QPointF(ptActuel.x() + sgn * _largeurCarte, ptActuel.y()));
                        zones.append(poly);

                        poly.clear();
                        poly.append(QPointF(ptActuel.x() - sgn * _largeurCarte, ptActuel.y()));
                    }

                    poly.append(ptActuel);
                }

                if (fabs(poly.last().x() - pt0.x()) < (_largeurCarte / 2)) {
                    poly.append(pt0);
                }

                zones.append(poly);

                if (_mcc) {

                    pen = QPen((settings.value("affichage/coulCercleVisibilite").toUInt() == 0) ? Qt::white : Qt::darkRed, 2.);

                    if (sat.elementsOrbitaux().nom.startsWith("tdrs", Qt::CaseInsensitive)) {

                        const int numeroTDRS = sat.elementsOrbitaux().nom.section(" ", 1).toInt();

                        QMapIterator it(Configuration::instance()->mapTDRS());
                        while (it.hasNext()) {
                            it.next();

                            if (it.key() == numeroTDRS) {

                                const SatelliteTDRS satTDRS = it.value();

                                // Affichage du nom du satellite TDRS
                                QGraphicsSimpleTextItem * const txtSat = new QGraphicsSimpleTextItem(satTDRS.denomination);
                                const QFont policeSat(settings.value("affichage/policeWCC").toString(), 11);
                                txtSat->setFont(policeSat);

                                const int lsat = qRound(DEG2PX(180. - sat.longitude() * MATHS::RAD2DEG));
                                const int bsat = qRound(DEG2PX(90. - sat.latitude() * MATHS::RAD2DEG));

                                pen = QPen(QColor(satTDRS.rouge, satTDRS.vert, satTDRS.bleu), 2.);
                                pen.setCosmetic(true);

                                txtSat->setBrush(pen.color());

                                const int lng = static_cast<int> (txtSat->boundingRect().width());
                                const int xnsat = lsat - lng / 2 + 1;
                                txtSat->setPos(xnsat, bsat + 28);
                                scene->addItem(txtSat);
                            }
                        }
                    }
                } else {
                    pen.setColor(Qt::white);
                    pen.setWidthF(1.1);
                }

                pen.setCosmetic(true);

                QVectorIterator it(zones);
                while (it.hasNext()) {

                    res.clear();
                    res.addPolygon(it.next());

                    QGraphicsPathItem * const path = new QGraphicsPathItem(res);
                    path->setPen(pen);
                    scene->addItem(path);
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Determination de la couleur du point de la trace au sol
 */
QColor Carte::CouleurTraceAuSol(const ElementsTraceSol &trace) const
{
    /* Declarations des variables locales */
    QColor res;

    /* Initialisations */

    /* Corps de la methode */
    if (_mcc && settings.value("affichage/styleWCC").toBool()) {
        res = QColor(Qt::white);

    } else {

        if (trace.eclipseTotale) {
            res = QColor(220, 20, 60);

        } else if (trace.eclipsePartielle) {
            res = QColor(Qt::green);

        } else {
            res = QColor(173, 216, 230);
        }
    }

    /* Retour */
    return res;
}

/*
 * Initialisation de la classe Carte
 */
void Carte::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->carte->setAttribute(Qt::WA_Hover);
    _ui->carte->setAttribute(Qt::WA_MouseTracking);
    _ui->carte->setMouseTracking(true);
    _ui->carte->viewport()->setMouseTracking(true);

    _ui->carte->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    _ui->carte->setDragMode(QGraphicsView::ScrollHandDrag);

    _ui->carte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _ui->carte->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _ui->carte->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    _ui->carte->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    _ui->carte->setCursor(Qt::ArrowCursor);
    _ui->carte->viewport()->setCursor(Qt::ArrowCursor);

    _resize = false;

    // Chargement de la carte du monde
    scene = new QGraphicsScene;
    ChargementCarteDuMonde();

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Lecture des coordonnees dans le fichier de coordonnees geographiques
 */
QPolygonF Carte::LectureCoordonnees(const QString &coordonnees)
{
    /* Declarations des variables locales */
    QPolygonF poly;

    /* Initialisations */

    /* Corps de la methode */
    if (!coordonnees.isEmpty()) {

        const QStringList latlng = coordonnees.split(QRegularExpression("[,\\s]"), Qt::SkipEmptyParts);

        for (int i = 0; i < latlng.size(); i+=2) {
            poly.append(QPointF(DEG2PX(latlng[i].toDouble() + 180.), DEG2PX(-latlng[i+1].toDouble() + 90.)));
        }
    }

    /* Retour */
    return poly;
}

/*
 * Lecture d'un fichier de coordonnees geographiques au format kml
 */
void Carte::LectureFichierKml(const QString &fichier, const bool visible, const double echelleMin)
{
    /* Declarations des variables locales */
    static QVector<QPolygonF> poly;

    /* Initialisations */
    ItemGroup *grp = new ItemGroup;

    if ((_ui->carte->width() < _ui->carte->minimumWidth()) || _resize) {
        poly.clear();
    }

    /* Corps de la methode */
    if (poly.isEmpty()) {

        QFileInfo ff(fichier);
        if (!ff.exists()) {
            qWarning() << "Le fichier de frontières" << ff.fileName() << "n'existe pas";
            throw PreviSatException(tr("Le fichier %1 n'existe pas, veuillez réinstaller %2").arg(ff.fileName()).arg(APP_NAME), MessageType::WARNING);
        }

        QFile fi(fichier);
        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QXmlStreamReader kml(&fi);

            while (!kml.atEnd() && !kml.hasError()) {

                QXmlStreamReader::TokenType token = kml.readNext();

                if (token == QXmlStreamReader::StartDocument) {
                    continue;
                }

                if (token == QXmlStreamReader::StartElement) {

                    if ((kml.name().toString() == "name") || (kml.name().toString() == "color")) {
                        continue;
                    }

                    if (kml.name().toString() == "coordinates") {

                        const QString frt = kml.readElementText();
                        poly.append(LectureCoordonnees(frt));
                        continue;
                    }
                }
            }
        }

        _resize = false;
        fi.close();
    }

    QPainterPath res;
    QPen pen;
    pen.setWidthF(1.);
    pen.setColor((echelleMin > 0.) ? QColor("#999999") :  QColor("#CCCCCC"));
    pen.setCosmetic(true);

    QVectorIterator it(poly);
    while (it.hasNext()) {

        const QPolygonF frontiere = it.next();

        res.clear();
        res.addPolygon(frontiere);

        QGraphicsPathItem * const path = new QGraphicsPathItem(res);
        path->setPen(pen);
        grp->addToGroup(path);
    }

    if (echelleMin >= 0.) {
        grp->setVisible(visible);
        grp->setVisibleMin(echelleMin);
        connect(this, &Carte::SendCurrentScale, grp, &ItemGroup::CheckVisibleMin);
    }

    _groupes.append(grp);

    /* Retour */
    return;
}
