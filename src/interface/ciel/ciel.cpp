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
 * >    26 decembre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QSettings>
#include "ui_ciel.h"
#include "ui_options.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "ciel.h"
#include "interface/options/options.h"
#include "librairies/corps/etoiles/constellation.h"
#include "librairies/corps/etoiles/ligneconstellation.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


// Couleur des planetes
static const QHash<IndicePlanete, QColor> couleurPlanetes = {
    { IndicePlanete::MERCURE, Qt::gray         },
    { IndicePlanete::VENUS,   Qt::white        },
    { IndicePlanete::MARS,    Qt::red          },
    { IndicePlanete::JUPITER, QColor("orange") },
    { IndicePlanete::SATURNE, Qt::darkYellow   },
    { IndicePlanete::URANUS,  Qt::green        },
    { IndicePlanete::NEPTUNE, Qt::blue         }
};

// Ecliptique
static constexpr std::array<std::array<double, 2>, 49> tabEcliptique = {
    { { 0., 0. }, { 0.5, 3.233 }, { 1., 6.4 }, { 1.5, 9.417 },  { 2., 12.217 }, { 2.5, 14.783 }, { 3., 17. }, { 3.5, 18.983 }, { 4., 20.567 },
      { 4.5, 21.817 }, { 5., 22.75 }, { 5.5, 23.25 }, { 6., 23.433 }, { 6.5, 23.25 }, { 7., 22.75 }, { 7.5, 21.817 }, { 8., 20.567 }, { 8.5, 18.983 },
      { 9., 17. }, { 9.5, 14.783 }, { 10., 12.217 }, { 10.5, 9.417 }, { 11., 6.4 }, { 11.5, 3.233 }, { 12., 0. }, { 12.5, -3.233 }, { 13., -6.4 },
      { 13.5, -9.417 }, { 14., -12.217 }, { 14.5, -14.783 }, { 15., -17. }, { 15.5, -18.983 }, { 16., -20.567 }, { 16.5, -21.817 }, { 17., -22.75 },
      { 17.5, -23.25 }, { 18., -23.433 }, { 18.5, -23.25 }, { 19., -22.75 }, { 19.5, -21.817 }, { 20., -20.567 }, { 20.5, -18.983 }, { 21., -17. },
      { 21.5, -14.783 }, { 22., -12.217 }, { 22.5, -9.417 }, { 23., -6.4 }, { 23.5, -3.233 }, { 24., 0. } }
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
Ciel::Ciel(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Ciel)
{
    _ui->setupUi(this);

    scene = new QGraphicsScene;

    try {

        resize(parent->height(), parent->height());
        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}

/*
 * Destructeur
 */
Ciel::~Ciel()
{
    EFFACE_OBJET(scene);
    delete _ui;
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
                const std::array<Planete, PLANETE::NB_PLANETES> &planetes,
                const QList<Satellite> &satellites,
                const bool fenetreMax,
                const bool maxFlash,
                const bool labelHeure,
                const Date &dateDeb,
                const Date &dateMax,
                const Date &dateFin)
{
    /* Declarations des variables locales */
    QRect rectangle;

    /* Initialisations */
    _observateur = observateur;
    _labelHeure = labelHeure;
    _fenetreMax = fenetreMax;
    _satellites = satellites;
    _planetes = planetes;
    _soleil = soleil;
    _lune = lune;

    QWidget::show();

    if (!_labelHeure) {
        _ui->vueCiel->setGeometry(_ui->vueCiel->x(), _ui->vueCiel->y(), parentWidget()->height()-32, parentWidget()->height()-32);
        setGeometry((parentWidget()->width() - width() - _ui->est->width()) / 2, 0, _ui->vueCiel->width() + 2 * _ui->est->width(),
                    parentWidget()->height());

        _ui->est->setGeometry(_ui->est->x(), _ui->est->y(), _ui->est->width(), _ui->vueCiel->height());
        _ui->ouest->setGeometry(_ui->est->width() + _ui->vueCiel->width(), _ui->ouest->y(), _ui->ouest->width(), _ui->vueCiel->height());
        _ui->nord->setGeometry(_ui->nord->x(), _ui->nord->y(), 2 * _ui->est->width() + _ui->vueCiel->width(), _ui->nord->height());
        _ui->sud->setGeometry(_ui->sud->x(), _ui->nord->height() + _ui->vueCiel->height(), _ui->nord->width(), _ui->sud->height());
    }

    // Determination de la couleur du ciel
    const double hts = soleil.hauteur() * MATHS::RAD2DEG;
    const QBrush couleurCiel = CalculCouleurCiel(hts);

    scene->clear();
    scene->setBackgroundBrush(QBrush(palette().window().color()));

    /* Corps de la methode */
    // Affichage de la carte du ciel
    if (_labelHeure) {
        _lciel = 260;
        _hciel = 260;
        rectangle = QRect(2, 2, 516, 516);
    } else {
        _lciel = _ui->vueCiel->width() / 2;
        _hciel = _ui->vueCiel->width() / 2;
        rectangle = QRect(2, 2, _ui->vueCiel->width() - 4, _ui->vueCiel->height() - 4);
    }

    scene->setSceneRect(rectangle);

    QPen pen(couleurCiel, Qt::SolidPattern);
    pen.setCosmetic(true);

    scene->addEllipse(rectangle, pen, couleurCiel);


    // Affichage des constellations
    AffichageConstellations(lignesCst, constellations);

    // Affichage des etoiles
    AffichageEtoiles(etoiles);

    // Affichage des planetes
    AffichagePlanetes1();

    // Affichage du Soleil et de l'ecliptique
    AffichageSoleil();

    // Affichage des planetes
    AffichagePlanetes2();

    // Affichage de la Lune
    AffichageLune();

    // Affichage des satellites
    AffichageSatellites(dateDeb, dateMax, dateFin, maxFlash);

    if (_labelHeure) {
        scene->addEllipse(-20, -20, 560, 560, QPen(QBrush(palette().window().color()), 44));
        scene->addEllipse(1, 1, 517, 517, QPen(QBrush(Qt::gray), 3));
    } else {
        scene->addEllipse(-30, -30, _ui->vueCiel->width() + 60, _ui->vueCiel->height() + 60, QPen(QBrush(palette().window().color()), 64));
        scene->addEllipse(1, 1, _ui->vueCiel->width() - 3, _ui->vueCiel->height() - 3, QPen(QBrush(Qt::gray), 3));
    }

    _ui->vueCiel->setScene(scene);
    _ui->vueCiel->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

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
    double ang = MATHS::PI_SUR_DEUX;

    /* Corps de la methode */
    for(int i=0; i<36; i++) {

        const double x = sph * ((i < 19) ? rayonY : rayonX) * cos(ang) + dimensionPx - 1;
        const double y = rayonX * sin(ang) + 8;

        pt.append(QPointF(x, y));
        ang += 10. * MATHS::DEG2RAD;
    }

    /* Retour */
    return QPolygonF(pt);
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

/*
 * Determination de la couleur de la trace dans le ciel
 */
QColor Ciel::CouleurTraceCiel(const ElementsTraceCiel &trace)
{
    /* Declarations des variables locales */
    QColor res;

    /* Initialisations */

    /* Corps de la methode */
    if (trace.eclipseTotale) {
        res = QColor(220, 20, 60);

    } else if (trace.eclipsePartielle) {
        res = QColor(Qt::green);

    } else {

        const double hauteurSoleil = Configuration::instance()->soleil().hauteur();
        if (hauteurSoleil > -0.08) {
            res = QColor(173, 216, 230);

        } else if (hauteurSoleil > -0.12) {
            res = QColor("deepskyblue");

        } else {
            res = QColor("cyan");
        }
    }

    /* Retour */
    return res;
}

void Ciel::mouseMoveEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->vueCiel->underMouse()) {

        const int x1 = static_cast<int> (evt->position().x() - _lciel);
        const int y1 = static_cast<int> (evt->position().y() - _hciel);

        if (!_labelHeure) {

            // Le curseur est au-dessus de la carte du ciel
            if ((x1 * x1 + y1 * y1) <= (_hciel * _lciel)) {

                const double x2 = -x1 / (double) _lciel;
                const double y2 = -y1 / (double) _hciel;

                const double ht = MATHS::PI_SUR_DEUX * (1. - sqrt(x2 * x2 + y2 * y2));
                double az = atan2(x2, y2);
                if (az < 0.) {
                    az += MATHS::DEUX_PI;
                }

                const double ch = cos(ht);
                const Vecteur3D vec1(-cos(az) * ch, sin(az) * ch, sin(ht));
                const Vecteur3D vec2(_observateur.rotHz().Transposee() * vec1);

                // Declinaison
                const double dec = asin(vec2.z());

                // Ascension droite
                double ad = atan2(vec2.y(), vec2.x());
                if (ad < 0.) {
                    ad += MATHS::DEUX_PI;
                }

                // Affichage des coordonnees dans la barre de statut
                emit AfficherMessageStatut2(tr("Ascension droite : %1")
                                                .arg(Maths::ToSexagesimal(ad, AngleFormatType::HEURE1, 2, 0, false, false).mid(0, 7)));
                emit AfficherMessageStatut3(tr("Déclinaison : %1").arg(Maths::ToSexagesimal(dec, AngleFormatType::DEGRE, 2, 0, true, false).mid(0, 7)));

                // Survol d'un satellite avec le curseur
                bool atrouve = false;
                QListIterator it(_satellites);
                while (it.hasNext() && !atrouve) {

                    const Satellite sat = it.next();
                    const int lsat = TOPO2X(_lciel, sat.hauteur(), sat.azimut(), 1) - _lciel;
                    const int bsat = TOPO2Y(_hciel, sat.hauteur(), sat.azimut(), 1) - _hciel;

                    // Distance au carre du satellite au curseur
                    const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

                    // Le curseur est au(dessus d'un satellite
                    if ((dt <= 16) && (sat.hauteur() >= 0.)) {
                        atrouve = true;
                        setToolTip(tr("<font color='blue'><b>%1</b></font><br />NORAD : <b>%2</b><br />COSPAR : <b>%3</b>")
                                   .arg(sat.elementsOrbitaux().nom).arg(sat.elementsOrbitaux().norad).arg(sat.elementsOrbitaux().cospar));

                        emit AfficherMessageStatut(tr("<b>%1</b> (numéro NORAD : <b>%2</b>  -  COSPAR : <b>%3</b>)")
                                                   .arg(sat.elementsOrbitaux().nom).arg(sat.elementsOrbitaux().norad)
                                                   .arg(sat.elementsOrbitaux().cospar));

                        setCursor(Qt::CrossCursor);

                    } else {

                        emit EffacerMessageStatut();
                        setToolTip("");
                        setCursor(Qt::ArrowCursor);
                    }
                }

                // Survol des planetes avec le curseur
                if (static_cast<Qt::CheckState> (settings.value("affichage/affplanetes").toUInt()) != Qt::Unchecked) {

                    bool atrouve2 = false;
                    static bool aplanete = false;
                    for(unsigned int ipla=0; (ipla<PLANETE::NB_PLANETES) && !atrouve2; ipla++) {

                        const int lpla = TOPO2X(_lciel, _planetes.at(ipla).hauteur(), _planetes.at(ipla).azimut(), 1) - _lciel;
                        const int bpla = TOPO2Y(_hciel, _planetes.at(ipla).hauteur(), _planetes.at(ipla).azimut(), 1) - _hciel;

                        // Distance au carre de la planete au curseur
                        const int dt = (x1 - lpla) * (x1 - lpla) + (y1 - bpla) * (y1 - bpla);

                        // Le curseur est au-dessus d'une planete
                        if (dt <= 16) {

                            aplanete = true;
                            atrouve2 = true;
                            emit AfficherMessageStatut(_planetes.at(ipla).nom());
                            setToolTip(_planetes.at(ipla).nom());
                            setCursor(Qt::CrossCursor);

                        } else if (aplanete) {

                            emit EffacerMessageStatut();
                            setToolTip("");
                            setCursor(Qt::ArrowCursor);
                            aplanete = false;
                        }
                    }
                }

                // Survol du Soleil avec le curseur
                if (settings.value("affichage/affsoleil").toBool()) {

                    static bool asoleil = false;
                    const int lsol = TOPO2X(_lciel, _soleil.hauteur(), _soleil.azimut(), 1) - _lciel;
                    const int bsol = TOPO2Y(_hciel, _soleil.hauteur(), _soleil.azimut(), 1) - _hciel;

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
                    const int llun = TOPO2X(_lciel, _lune.hauteur(), _lune.azimut(), 1) - _lciel;
                    const int blun = TOPO2Y(_hciel, _lune.hauteur(), _lune.azimut(), 1) - _hciel;

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
    }

    /* Retour */
    return;
}

void Ciel::mousePressEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->vueCiel->underMouse()) {

        bool atrouve = false;
        int idx = 1;

        const int x1 = static_cast<int> (evt->position().x() - _lciel);
        const int y1 = static_cast<int> (evt->position().y() - _hciel);

        // Clic sur un satellite
        QListIterator it(Configuration::instance()->listeSatellites());
        while (it.hasNext() && !atrouve) {

            const Satellite sat = it.next();
            const int lsat = TOPO2X(_lciel, sat.hauteur(), sat.azimut(), 1) - _lciel;
            const int bsat = TOPO2Y(_hciel, sat.hauteur(), sat.azimut(), 1) - _hciel;

            // Distance au carre du curseur au satellite
            const int dt = (x1 - lsat) * (x1 - lsat) + (y1 - bsat) * (y1 - bsat);

            // Le curseur est au-dessus d'un satellite
            if ((dt <= 16) && (sat.hauteur() > 0.)) {

                atrouve = true;
                Configuration::instance()->listeSatellites().move(--idx, 0);
                QStringList &listeNorad = Configuration::instance()->mapSatellitesFichierElem()[Configuration::instance()->nomfic()];
                listeNorad.move(idx, 0);

                // On definit le satellite choisi comme satellite par defaut
                Configuration::instance()->noradDefaut() = sat.elementsOrbitaux().norad;
                emit ReinitFlags();

                emit RecalculerPositions();
                GestionnaireXml::EcritureConfiguration();
            }

            idx++;
        }
    }

    /* Retour */
    return;
}

void Ciel::resizeEvent(QResizeEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (!_labelHeure) {

        show(Configuration::instance()->observateur(),
             Configuration::instance()->soleil(),
             Configuration::instance()->lune(),
             Configuration::instance()->lignesCst(),
             Configuration::instance()->constellations(),
             Configuration::instance()->etoiles(),
             Configuration::instance()->planetes(),
             Configuration::instance()->listeSatellites(),
             Configuration::instance()->isCarteMaximisee());
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
/*
 * Affichage des constellations
 */
void Ciel::AffichageConstellations(const QList<LigneConstellation> &lignesCst, const QList<Constellation> &constellations)
{
    /* Declarations des variables locales */
    QPen crayon;

    /* Initialisations */
    const QColor bleuClair(173, 216, 230);

    /* Corps de la methode */
    if (static_cast<Qt::CheckState> (settings.value("affichage/affconst").toUInt()) != Qt::Unchecked) {

        QListIterator it(lignesCst);
        while (it.hasNext()) {

            const LigneConstellation lig = it.next();
            if (lig.isDessin()) {

                // Calcul des coordonnees radar des etoiles pour le dessin de la constellation
                const int lstr1 = TOPO2X(_lciel, lig.etoile1().hauteur(), lig.etoile1().azimut(), 1);
                const int bstr1 = TOPO2Y(_hciel, lig.etoile1().hauteur(), lig.etoile1().azimut(), 1);

                const int lstr2 = TOPO2X(_lciel, lig.etoile2().hauteur(), lig.etoile2().azimut(), 1);
                const int bstr2 = TOPO2Y(_hciel, lig.etoile2().hauteur(), lig.etoile2().azimut(), 1);

                if (_soleil.hauteur() > -0.08) {
                    crayon = bleuClair;
                } else if (_soleil.hauteur() > -0.12) {
                    crayon = QColor("deepskyblue");
                } else {
                    crayon = QColor(Qt::cyan);
                }

                crayon.setCosmetic(true);

                if (((lstr2 - lstr1) * (lstr2 - lstr1) + (bstr2 - bstr1) * (bstr2 - bstr1)) < (_lciel * _hciel)) {
                    scene->addLine(lstr1, bstr1, lstr2, bstr2, crayon);
                }
            }
        }

        // Affichage du nom des constellations
        if (static_cast<Qt::CheckState> (settings.value("affichage/affconst").toUInt()) == Qt::Checked) {

            if (Configuration::instance()->isCarteMaximisee() || _fenetreMax) {

                QListIterator it2(constellations);
                while (it2.hasNext()) {

                    const Constellation cst = it2.next();
                    if (cst.isVisible()) {

                        // Calcul des coordonnees radar du label
                        const int lcst = TOPO2X(_lciel, cst.hauteur(), cst.azimut(), 1);
                        const int bcst = TOPO2Y(_hciel, cst.hauteur(), cst.azimut(), 1);

                        const int lst = lcst - _lciel;
                        const int bst = _hciel - bcst;

                        QGraphicsSimpleTextItem * const txtCst = new QGraphicsSimpleTextItem(cst.nom());
                        const int lng = static_cast<int> (txtCst->boundingRect().width());

                        const double dist = sqrt((lst + lng) * (lst + lng) + bst * bst);
                        const int xncst = (dist > _lciel) ? lcst - lng - 1 : lcst + 1;
                        const int yncst = (bcst + 9 > _hciel) ? bcst - 10 : bcst + 1;

                        txtCst->setBrush(QBrush(Qt::darkYellow));
                        txtCst->setPos(xncst, yncst);
                        txtCst->setFont(QFont(font().family(), 8));
                        scene->addItem(txtCst);
                    }
                }
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage des etoiles
 */
void Ciel::AffichageEtoiles(const QList<Etoile> &etoiles)
{
    /* Declarations des variables locales */
    QGraphicsSimpleTextItem * txtStr;

    /* Initialisations */
    const QBrush couleurEtoiles = (_soleil.hauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);

    /* Corps de la methode */
    QListIterator it1(etoiles);
    while (it1.hasNext()) {

        const Etoile etoile = it1.next();
        if (etoile.isVisible() && (etoile.magnitude() <= settings.value("affichage/magnitudeEtoiles").toDouble())) {

            const int lstr = TOPO2X(_lciel, etoile.hauteur(), etoile.azimut(), 1);
            const int bstr = TOPO2Y(_hciel, etoile.hauteur(), etoile.azimut(), 1);

            if (etoile.magnitude() <= 3.) {
                const QRect rectangle = QRect(lstr-1, bstr-1, 2, 3);
                scene->addEllipse(rectangle, QPen(Qt::NoPen), couleurEtoiles);
            } else {
                const QLine lin(lstr, bstr, lstr, bstr);
                scene->addLine(lin, couleurEtoiles.color());
            }

            // Nom des etoiles les plus brillantes
            if (settings.value("affichage/affetoiles").toBool()) {

                if (Configuration::instance()->isCarteMaximisee() || _fenetreMax) {

                    if (!etoile.nom().isEmpty() && etoile.nom().at(0).isUpper()) {
                        if (etoile.magnitude() < (settings.value("affichage/magnitudeEtoiles").toDouble() - 1.9)) {

                            const int lst = lstr - _lciel;
                            const int bst = _hciel - bstr;
                            const QString nomstr = etoile.nom().mid(0, 1) + etoile.nom().mid(1).toLower();

                            txtStr = new QGraphicsSimpleTextItem(nomstr);
                            const int lng = static_cast<int> (txtStr->boundingRect().width());

                            const double dist = sqrt((lst + lng) * (lst + lng) + bst * bst);
                            const int xnstr = (dist > _lciel) ? lstr - lng - 1 : lstr + 1;
                            const int ynstr = (bstr + 9 > _ui->vueCiel->height()) ? bstr - 10 : bstr + 1;

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

    /* Retour */
    return;
}

/*
 * Affichage de la Lune
 */
void Ciel::AffichageLune()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (settings.value("affichage/afflune").toBool() && _lune.isVisible()) {

        // Calcul des coordonnees radar de la Lune
        const int llun = TOPO2X(_lciel, _lune.hauteur(), _lune.azimut(), 1);
        const int blun = TOPO2Y(_hciel, _lune.hauteur(), _lune.azimut(), 1);

        const int lpol = _lciel;
        const int bpol = qRound(_hciel - _hciel * (1. - _observateur.latitude() * MATHS::DEUX_SUR_PI));

        QPixmap pixlun;
        pixlun.load(":/resources/interface/lune.png");
        pixlun = pixlun.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dessin de la Lune et rotations
        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);
        QTransform transform;
        transform.translate(llun, blun);
        transform.rotate(180. - QLineF(llun, blun, lpol, bpol).normalVector().angle());
        if (settings.value("affichage/rotationLune").toBool() && (_observateur.latitude() < 0.)) {
            transform.rotate(180.);
        }

        transform.translate(-8, -8);
        lun->setTransform(transform);

        // Dessin de la phase
        if (settings.value("affichage/affphaselune").toBool()) {

            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 160));
            QPen stylo(Qt::NoBrush, 0);
            stylo.setCosmetic(true);
            const QPolygonF poly = AffichagePhaseLune(_lune, 9);

            QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);
            omb->setTransform(transform);
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage des planetes (cas general)
 */
void Ciel::AffichagePlanetes1()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (static_cast<Qt::CheckState> (settings.value("affichage/affplanetes").toUInt()) != Qt::Unchecked) {

        // Calcul des coordonnees radar des planetes
        QGraphicsSimpleTextItem * txtPla;
        QList<Planete> planetes;
        planetes.reserve(_planetes.size());
        std::copy(_planetes.begin(), _planetes.end(), std::back_inserter(planetes));
        QListIterator it(planetes);

        while (it.hasNext()) {

            const Planete planete = it.next();

            if (planete.hauteur() >= 0.) {

                if ((((planete.indice() == IndicePlanete::MERCURE) || (planete.indice() == IndicePlanete::VENUS))
                     && (planete.distance() > _soleil.distance())) || (planete.indice() >= IndicePlanete::MARS)) {

                    const int lpla = TOPO2X(_lciel, planete.hauteur(), planete.azimut(), 1);
                    const int bpla = TOPO2Y(_hciel, planete.hauteur(), planete.azimut(), 1);

                    const QBrush coulPlanete(QBrush(couleurPlanetes[planete.indice()], Qt::SolidPattern));
                    const QRect rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                    scene->addEllipse(rectangle, QPen(couleurPlanetes[planete.indice()]), coulPlanete);

                    if ((Configuration::instance()->isCarteMaximisee() || _fenetreMax) &&
                        (static_cast<Qt::CheckState> (settings.value("affichage/affplanetes").toUInt()) == Qt::Checked)) {

                        const int lpl = lpla - _lciel;
                        const int bpl = _hciel - bpla;
                        const QString nompla = planete.nom();
                        txtPla = new QGraphicsSimpleTextItem(nompla);
                        const int lng = static_cast<int> (txtPla->boundingRect().width());

                        const double dist = sqrt((lpl + lng) * (lpl + lng) + bpl * bpl);
                        const int xnpla = (dist > _lciel) ? lpla - lng - 1 : lpla + 3;
                        const int ynpla = (bpla + 9 > _ui->vueCiel->height()) ? bpla - 10 : bpla + 2;

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

    /* Retour */
    return;
}

/*
 * Affichage des planetes (si une planete inferieure est plus proche que la Soleil)
 */
void Ciel::AffichagePlanetes2()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (static_cast<Qt::CheckState> (settings.value("affichage/affplanetes").toUInt()) != Qt::Unchecked) {

        // Calcul des coordonnees radar des planetes Mercure et Venus
        QGraphicsSimpleTextItem * txtPla;

        QList<Planete> planetes;
        planetes.reserve(_planetes.size());
        std::copy(_planetes.begin(), _planetes.end(), std::back_inserter(planetes));
        QListIterator it(planetes);

        while (it.hasNext()) {

            const Planete planete = it.next();

            if (planete.hauteur() >= 0.) {

                if (planete.distance() < _soleil.distance()) {

                    const int lpla = TOPO2X(_lciel, planete.hauteur(), planete.azimut(), 1);
                    const int bpla = TOPO2Y(_hciel, planete.hauteur(), planete.azimut(), 1);

                    const QBrush coulPlanete(QBrush(couleurPlanetes[planete.indice()], Qt::SolidPattern));
                    const QRect rectangle = QRect(lpla - 2, bpla - 2, 4, 4);
                    scene->addEllipse(rectangle, QPen(couleurPlanetes[planete.indice()]), coulPlanete);

                    if ((Configuration::instance()->isCarteMaximisee() || _fenetreMax)
                        && (static_cast<Qt::CheckState> (settings.value("affichage/affplanetes").toUInt()) == Qt::Checked)) {

                        const int lpl = lpla - _lciel;
                        const int bpl = _hciel - bpla;
                        const QString nompla = planete.nom();
                        txtPla = new QGraphicsSimpleTextItem(nompla);
                        const int lng = static_cast<int> (txtPla->boundingRect().width());

                        const double dist = sqrt((lpl + lng) * (lpl + lng) + bpl * bpl);
                        const int xnpla = (dist > _lciel) ? lpla - lng - 1 : lpla + 3;
                        const int ynpla = (bpla + 9 > _ui->vueCiel->height()) ? bpla - 10 : bpla + 2;

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

    /* Retour */
    return;
}

/*
 * Affichage des satellites
 */
void Ciel::AffichageSatellites(const Date &dateDeb, const Date &dateMax, const Date &dateFin, const bool maxFlash)
{
    /* Declarations des variables locales */
    bool aecr;
    int min;
    int lsat0;
    int bsat0;
    int lsat1;
    int bsat1;
    int lsat3 = 0;
    int bsat3 = 0;
    QColor couleur;
    QPen crayon;

    /* Initialisations */
    bool adeb = false;
    bool amax = false;
    const QColor crimson(220, 20, 60);
    const QColor bleuClair(173, 216, 230);
    QPen noir(Qt::black);
    noir.setCosmetic(true);
    const QBrush couleurEtoiles = (_soleil.hauteur() > -0.08) ? QBrush(Qt::black) : QBrush(Qt::white);

    /* Corps de la methode */
    QListIterator it(_satellites);
    it.toBack();

    while (it.hasPrevious()) {

        const Satellite sat = it.previous();

        if (sat.isVisible() && (sat.altitude() >= 0.)) {

            // Affichage de la trace dans le ciel
            const QList<ElementsTraceCiel> trace = sat.traceCiel();
            if (settings.value("affichage/afftraceCiel").toBool() && !trace.isEmpty()) {

                lsat1 = TOPO2X(_lciel, trace.first().hauteur, trace.first().azimut, 1);
                bsat1 = TOPO2Y(_hciel, trace.first().hauteur, trace.first().azimut, 1);

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

                    const ElementsTraceCiel trc = trace.at(i);

                    if (trc.eclipseTotale) {
                        crayon = crimson;

                    } else if (trc.eclipsePartielle) {
                        crayon = QPen(Qt::green);

                    } else {

                        const double hauteurSoleil = _soleil.hauteur();
                        if (hauteurSoleil > -0.08) {
                            crayon = bleuClair;

                        } else if (hauteurSoleil > -0.12) {
                            crayon = QPen(QColor("deepskyblue"));

                        } else {
                            crayon = QPen(QColor("cyan"));
                        }
                    }

                    const int lsat2 = TOPO2X(_lciel, trc.hauteur, trc.azimut, 1);
                    const int bsat2 = TOPO2Y(_hciel, trc.hauteur, trc.azimut, 1);

                    const QLineF lig(lsat2, bsat2, lsat1, bsat1);

                    if (_labelHeure) {

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

                            if ((dateTrace.jourJulienUTC() >= dateDeb.jourJulienUTC()) && (dateTrace.jourJulienUTC() <= dateFin.jourJulienUTC())) {
                                crayon = QPen(crayon.color(), 4);
                            }
                        }

                        // Affichage de l'heure
                        if (aecr) {

                            aecr = false;

                            // Dessin d'une petite ligne correspondant a la date
                            lig2 = QLineF(lsat2, bsat2, lsat3, bsat3).normalVector();
                            lig2.setLength(4);
                            scene->addLine(lig2, QPen(crayon.color(), 1.));

                            sdate = "";
                            if (amax) {

                                amax = false;

                                if (maxFlash) {
                                    nomFlash = _satellites.first().elementsOrbitaux().nom.section(QRegularExpression("[ -]"), 0, 0).toLower();
                                    nomFlash[0] = nomFlash[0].toUpper();
                                    nomFlash[3] = nomFlash[3].toUpper();
                                    sdate = tr("Flash %1").arg(nomFlash);
                                }
                            } else {

                                const DateSysteme sys = (settings.value("affichage/systemeHoraire").toBool()) ?
                                    DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H;

                                sdate = dateTrace.ToShortDate(DateFormat::FORMAT_COURT, sys).split(" ", Qt::SkipEmptyParts).last();
                                sdate = (sys == DateSysteme::SYSTEME_12H) ? sdate.mid(0, 5) + sdate.right(1) : sdate.mid(0, 5);
                            }

                            if (!sdate.isEmpty()) {

                                txtTrace = new QGraphicsSimpleTextItem(sdate);
                                txtTrace->setBrush(couleurEtoiles);

                                const double ang = -lig2.angle();
                                const double ca = cos(ang * MATHS::DEG2RAD);
                                const double sa = sin(ang * MATHS::DEG2RAD);
                                const double xnc = lsat2 + 6. * ca + 11. * sa;
                                const double ync = bsat2 + 6. * sa - 11. * ca;

                                txtTrace->setPos(xnc, ync);
                                txtTrace->setRotation(ang);
                                scene->addItem(txtTrace);
                            }
                        }
                    }

                    crayon.setCosmetic(true);
                    scene->addLine(lig, crayon);

                    lsat1 = lsat2;
                    bsat1 = bsat2;
                }
            }

            if (!_labelHeure) {

                // Calcul des coordonnees radar du satellite
                const int lsat = TOPO2X(_lciel, sat.hauteur(), sat.azimut(), 1);
                const int bsat = TOPO2Y(_hciel, sat.hauteur(), sat.azimut(), 1);

                const QRect rectangle = QRect(lsat - 3, bsat - 3, 6, 6);

                if (sat.conditionEclipse().eclipseTotale()) {
                    couleur = crimson;
                } else if (sat.conditionEclipse().eclipsePartielle() || sat.conditionEclipse().eclipseAnnulaire()) {
                    couleur = Qt::green;
                } else {
                    couleur = Qt::yellow;
                }

                scene->addEllipse(rectangle, noir, QBrush(couleur, Qt::SolidPattern));
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Affichage du Soleil
 */
void Ciel::AffichageSoleil()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (settings.value("affichage/affsoleil").toBool()) {

        // Dessin de l'ecliptique
        if (Configuration::instance()->isCarteMaximisee() || _fenetreMax) {

            const double ad1 = tabEcliptique[0][0] * MATHS::HEUR2RAD;
            const double de1 = tabEcliptique[0][1] * MATHS::DEG2RAD;
            const double cd1 = cos(de1);
            const Vecteur3D vec(cos(ad1) * cd1, sin(ad1) * cd1, sin(de1));
            const Vecteur3D vec1 = _observateur.rotHz() * vec;

            double ht1 = asin(vec1.z());
            double az1 = atan2(vec1.y(), -vec1.x());
            if (az1 < 0.) {
                az1 += MATHS::DEUX_PI;
            }

            int lecl1 = TOPO2X(_lciel, ht1, az1, 1);
            int becl1 = TOPO2Y(_hciel, ht1, az1, 1);

            double az2;

            for(int i=1; i<49; i++) {

                const double ad2 = tabEcliptique[i][0] * MATHS::HEUR2RAD;
                const double de2 = tabEcliptique[i][1] * MATHS::DEG2RAD;
                const double cd2 = cos(de2);
                const Vecteur3D vec0(cos(ad2) * cd2, sin(ad2) * cd2, sin(de2));
                const Vecteur3D vec2 = _observateur.rotHz() * vec0;

                const double ht2 = asin(vec2.z());

                az2 = atan2(vec2.y(), -vec2.x());
                if (az2 < 0.) {
                    az2 += MATHS::DEUX_PI;
                }

                const int lecl2 = TOPO2X(_lciel, ht2, az2, 1);
                const int becl2 = TOPO2Y(_hciel, ht2, az2, 1);

                if ((ht1 >= 0.) || (ht2 >= 0.)) {
                    scene->addLine(lecl1, becl1, lecl2, becl2, QPen(Qt::darkYellow));
                }

                lecl1 = lecl2;
                becl1 = becl2;
                ht1 = ht2;
            }
        }

        if (_soleil.isVisible()) {

            // Calcul des coordonnees radar du Soleil
            const int lsol = TOPO2X(_lciel, _soleil.hauteur(), _soleil.azimut(), 1);
            const int bsol = TOPO2Y(_hciel, _soleil.hauteur(), _soleil.azimut(), 1);

            QPixmap pixsol;
            pixsol.load(":/resources/interface/soleil.png");
            pixsol = pixsol.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
            QTransform transform;
            transform.translate(lsol, bsol);
            transform.translate(-8, -8);
            sol->setTransform(transform);
        }
    }

    /* Retour */
    return;
}

/*
 * Initialisation de la classe Ciel
 */
void Ciel::Initialisation()
{
    qInfo() << "Début Initialisation" << metaObject()->className();

    _fenetreMax = false;

    qInfo() << "Fin   Initialisation" << metaObject()->className();
}
