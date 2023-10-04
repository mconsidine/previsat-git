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
 * >    afficherresultats.cpp
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
 * >    4 octobre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QScreen>
#include <QSettings>
#include <QStringDecoder>
#include <QtGlobal>
#include "ui_afficherresultats.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "ui_onglets.h"
#include "afficherresultats.h"
#include "interface/ciel/ciel.h"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/maths/maths.h"

Q_DECLARE_METATYPE(QList<ResultatPrevisions>)


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


QList<Etoile> etoiles;
QList<Constellation> constellations;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
AfficherResultats::AfficherResultats(const TypeCalcul &typeCalcul, const ConditionsPrevisions &conditions, const DonneesPrevisions &donnees,
                                     const QMap<QString, QList<QList<ResultatPrevisions> > > &resultats, const int zoom, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::AfficherResultats),
    _resultats(resultats),
    _conditions(conditions),
    _donnees(donnees),
    _typeCalcul(typeCalcul)
{
    /* Declarations des variables locales */
    QStringList titres;

    /* Initialisations */
    _ciel = nullptr;
    _tableDetail = nullptr;
    _afficherDetail = nullptr;
    scene = nullptr;
    _zoom = zoom;

    /* Corps du constructeur */
#if (BUILD_TEST == false)
    _ui->setupUi(this);

    Etoile::Initialisation(Configuration::instance()->dirCommonData(), etoiles);
    Constellation::Initialisation(Configuration::instance()->dirCommonData(), constellations);

    if (_typeCalcul == TypeCalcul::EVENEMENTS) {
        _ui->actionEnregistrer->setVisible(false);
        _ui->frameCiel->setVisible(false);
        _ui->resultatsPrevisions->setVisible(false);
        _ui->fichierTexte->setVisible(true);
        setMinimumSize(640, 630);
        _afficherEvt = true;
    } else {
        QStyle * const styleBouton = QApplication::style();
        _ui->actionEnregistrer->setIcon(styleBouton->standardIcon(QStyle::SP_DialogSaveButton));
        _ui->actionEnregistrer->setVisible(true);
        _ui->frameCiel->setVisible(true);
        _ui->resultatsPrevisions->setVisible(true);
        _ui->fichierTexte->setVisible(false);
        setMinimumSize(1226, 630);
        _afficherEvt = false;
    }
    resize(minimumSize());

    _ui->resultatsPrevisions->clear();
    _ui->detailsTransit->setVisible(false);
    _ui->afficherCarte->setVisible(false);

    switch (_typeCalcul) {
    case TypeCalcul::PREVISIONS:
        setWindowTitle(tr("Prévisions de passage"));
        titres << tr("Satellite") << tr("Date de début", "Date and hour") << tr("Date de fin", "Date and hour")
               << tr("Hauteur max", "Maximum elevation") << tr("Magnitude") << tr("Hauteur Soleil");
        break;

    case TypeCalcul::EVENEMENTS:
        setWindowTitle(tr("Évènements orbitaux"));
        break;

    case TypeCalcul::FLASHS:
        setWindowTitle(tr("Flashs"));
        _ui->afficherCarte->setVisible(true);
        titres << tr("Satellite") << tr("Date de début", "Date and hour") << tr("Date de fin", "Date and hour")
               << tr("Hauteur Max", "Maximum elevation") << tr("Magn", "Magnitude") << tr("Mir", "Mirror") << tr("Haut Soleil", "Solar elevation");
        break;

    case TypeCalcul::TRANSITS:
        setWindowTitle(tr("Transits"));
        scene = new QGraphicsScene;
        _ui->afficherCarte->setVisible(true);
        titres << tr("Satellite") << tr("Date du maximum", "Date and hour") << tr("Cst", "Constellation") << tr("Angle")
               << tr("Type", "Transit or conjunction") << tr("Corps") << tr("Illum", "Illumination") << tr("Durée") << tr("Hauteur Soleil");

        if (_resultats.size() > 0) {
            _ui->detailsTransit->setVisible(true);
        }
        break;

    case TypeCalcul::STARLINK:
        setWindowTitle(tr("Passages des Starlink"));
        titres << tr("Satellite") << tr("Date de début", "Date and hour") << tr("Date de fin", "Date and hour")
               << tr("Hauteur max", "Maximum elevation") << tr("Magnitude") << tr("Hauteur Soleil");
        break;

    case TypeCalcul::TELESCOPE:
    default:
        break;
    }

    if (_typeCalcul == TypeCalcul::EVENEMENTS) {

        on_actionEnregistrerTxt_triggered();

        QFile fi(_conditions.ficRes);

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QByteArray contenuFic = fi.readAll();
            auto syst = QStringDecoder(QStringDecoder::Utf8);
            _ui->fichierTexte->setReadOnly(true);
            _ui->fichierTexte->setText(syst(contenuFic));
        }
        fi.close();

    } else {

        _ui->resultatsPrevisions->setColumnCount(static_cast<int> (titres.count()));
        _ui->resultatsPrevisions->setHorizontalHeaderLabels(titres);
        _ui->resultatsPrevisions->horizontalHeader()->setToolTip("");

        switch (_typeCalcul) {

        case TypeCalcul::FLASHS:

            _ui->resultatsPrevisions->horizontalHeaderItem(3)->setToolTip(tr("Hauteur maximale"));
            _ui->resultatsPrevisions->horizontalHeaderItem(4)->setToolTip(tr("Magnitude"));
            _ui->resultatsPrevisions->horizontalHeaderItem(5)->setToolTip(tr("Miroir"));
            _ui->resultatsPrevisions->horizontalHeaderItem(6)->setToolTip(tr("Hauteur Soleil"));
            break;

        case TypeCalcul::TRANSITS:

            _ui->resultatsPrevisions->horizontalHeaderItem(2)->setToolTip(tr("Constellation"));
            _ui->resultatsPrevisions->horizontalHeaderItem(6)->setToolTip(tr("Illumination"));
            _ui->resultatsPrevisions->horizontalHeaderItem(7)->setToolTip(tr("secondes"));
            break;

        case TypeCalcul::PREVISIONS:
        case TypeCalcul::STARLINK:
        case TypeCalcul::EVENEMENTS:
        case TypeCalcul::TELESCOPE:
        default:
            break;
        }

        ChargementResultats();
    }

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
AfficherResultats::~AfficherResultats()
{
    EFFACE_OBJET(_ciel);
    EFFACE_OBJET(_tableDetail);
    EFFACE_OBJET(_afficherDetail);
    EFFACE_OBJET(scene);
    delete _ui;
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
/*
 * Affichage du detail d'un transit
 */
void AfficherResultats::AffichageDetailTransit(const Observateur &observateur, const Soleil &soleil, const Lune &lune,
                                               const QList<ResultatPrevisions> &list)
{
    /* Declarations des variables locales */
    double diamApparent;
    QPen couleur(Qt::black);
    couleur.setCosmetic(true);

    /* Initialisations */
    const int lciel = qRound(0.5 * _ui->detailsTransit->width());
    const int hciel = qRound(0.5 * _ui->detailsTransit->height());

    scene->clear();
    scene->setBackgroundBrush(Qt::white);

    // Affichage de la carte du ciel
    const QRectF rectangle(-lciel, -hciel, _ui->detailsTransit->width(), _ui->detailsTransit->height());
    scene->setSceneRect(rectangle);

    /* Corps de la methode */
    // Dessin de la Lune ou du Soleil
    if (list.at(0).typeCorps == CorpsTransit::CORPS_SOLEIL) {

        diamApparent = 2. * asin(SOLEIL::RAYON_SOLAIRE / soleil.distance()) * MATHS::RAD2DEG;

        QPixmap pixsol;
        pixsol.load(":/resources/interface/soleil.png");
        pixsol = pixsol.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QGraphicsPixmapItem * const sol = scene->addPixmap(pixsol);
        const QTransform transform = QTransform().translate(-pixsol.width() / 2, -pixsol.height() / 2);
        sol->setTransform(transform);

    } else {

        diamApparent = 2. * asin(LUNE::RAYON_LUNAIRE / lune.distance()) * MATHS::RAD2DEG;

        // Angle horaire
        const double angleHoraire = observateur.tempsSideralGreenwich() - observateur.longitude() - lune.ascensionDroite();

        // Angle parallactique
        const double angleParallactique = MATHS::RAD2DEG *
                atan(sin(angleHoraire) / (tan(observateur.latitude()) * cos(lune.declinaison()) - sin(lune.declinaison()) * cos(angleHoraire)));

        QPixmap pixlun;
        pixlun.load(":/resources/interface/lune.png");
        pixlun = pixlun.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dessin de la Lune et rotations
        QGraphicsPixmapItem * const lun = scene->addPixmap(pixlun);

        const QRectF rect = pixlun.rect();
        const QPointF centre = rect.center();
        const double angle = angleParallactique + ((settings.value("affichage/rotationLune").toBool() && (observateur.latitude() < 0.)) ? 180. : 0.);

        const QTransform transform1 = QTransform().translate(centre.x() - pixlun.width() / 2, centre.y() - pixlun.height() / 2)
                .rotate(angle).translate(-centre.x(), -centre.y());
        lun->setTransform(transform1);

        // Dessin de la phase
        if (settings.value("affichage/affphaselune").toBool()) {

            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, 160));
            QPen stylo(Qt::NoBrush, 0);
            stylo.setCosmetic(true);
            const QPolygonF poly = Ciel::AffichagePhaseLune(lune, 50);

            QGraphicsPolygonItem * const omb = scene->addPolygon(poly, stylo, alpha);

            const QTransform transform2 = QTransform().translate(-49, -8);
            omb->setTransform(transform2);
            omb->setTransformOriginPoint(49, 8);
            omb->setRotation(angle);
        }

        couleur = QPen((lune.fractionIlluminee() < 0.5) ? Qt::lightGray : Qt::black);
    }

    const double fact = 50. / diamApparent;

    double deltaAzimut;
    double deltaHauteur;
    QList<QPointF> coord;
    QListIterator it(list);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Coordonnees relatives du satellite par rapport a la Lune ou au Soleil
        if (res.typeCorps == CorpsTransit::CORPS_SOLEIL) {

            deltaAzimut = MATHS::RAD2DEG * (res.azimut - res.azimutSoleil);
            deltaHauteur = MATHS::RAD2DEG * (res.hauteur - res.hauteurSoleil);

        } else {
            deltaAzimut = MATHS::RAD2DEG * (res.azimut - lune.azimut());
            deltaHauteur = MATHS::RAD2DEG * (res.hauteur - lune.hauteur());
        }

        const QPointF pt(fact * deltaAzimut, -fact * deltaHauteur);
        coord.append(pt);
    }

    double lsat1 = coord.first().x();
    double hsat1 = coord.first().y();

    QLineF lig1;
    QLineF lig2;

    QListIterator it2(coord);
    it2.next();
    while (it2.hasNext()) {

        const QPointF pt = it2.next();
        const QLineF lig(pt.x(), pt.y(), lsat1, hsat1);

        scene->addLine(lig, couleur);

        if ((fabs(pt.x() - coord.last().x()) < MATHS::EPSDBL100) && (fabs(pt.y() - coord.last().y()) < MATHS::EPSDBL100)) {

            lig1 = lig.normalVector();
            lig2 = lig1;

            lig1.setAngle(lig1.angle() - 60.);
            lig1.setLength(12.);
            lig2.setAngle(lig2.angle() - 120.);
            lig2.setLength(12.);

            scene->addLine(lig1, couleur);
            scene->addLine(lig2, couleur);
        }

        lsat1 = pt.x();
        hsat1 = pt.y();
    }

    _ui->detailsTransit->setScene(scene);
    _ui->detailsTransit->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    /* Retour */
    return;
}

/*
 * Chargement de la carte representant la trace du maximum
 */
void AfficherResultats::ChargementCarte(const Observateur &observateur, const QList<ResultatPrevisions> &list)
{
    /* Declarations des variables locales */
    static QString map;

    /* Initialisations */
    const QString lon(QString::number(-observateur.longitude() * MATHS::RAD2DEG));
    const QString lat(QString::number(observateur.latitude() * MATHS::RAD2DEG));
    const QString unite((_conditions.unite == tr("km", "Kilometer")) ? tr("m", "meter") : tr("ft", "foot"));
    const QString alt(QString::number((int) (observateur.altitude() * 1000. * ((unite == tr("m", "meter")) ? 1. : TERRE::PIED_PAR_METRE) +
                                             MATHS::EPSDBL100)) + " " + unite);

    /* Corps de la methode */
    // Lecture du fichier balise
    if (map.isEmpty()) {

        const QString ficMap = Configuration::instance()->dirLocalData() + QDir::separator() + "html" + QDir::separator() + "resultat.map";
        QFile fi(ficMap);

        if (fi.exists() && (fi.size() != 0)) {
            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                map = fi.readAll();
            }
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
            .replace("CHAINE_ALTITUDE", tr("Altitude", "Altitude of observer"))
            .replace("LONGITUDE1", QString::number(-list.first().obsmax.longitude() * MATHS::RAD2DEG))
            .replace("LONGITUDE2", QString::number(-list.last().obsmax.longitude() * MATHS::RAD2DEG))
            .replace("LATITUDE1", QString::number(list.first().obsmax.latitude() * MATHS::RAD2DEG))
            .replace("LATITUDE2", QString::number(list.last().obsmax.latitude() * MATHS::RAD2DEG))
            .replace("VALEUR_ZOOM", QString::number(_zoom));

    // Creation du fichier html
    QFile fr(Configuration::instance()->dirTmp() + QDir::separator() + "resultat.html");
    if (fr.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fr);
        flux << _mapResultats;
    }
    fr.close();

    /* Retour */
    return;
}

/*
 * Chargement des resultats
 */
void AfficherResultats::ChargementResultats()
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */
    int j = 0;
    int imax = 1;
    const bool etat = _ui->resultatsPrevisions->blockSignals(true);

    /* Corps de la methode */
    QTableWidgetItem * item;
    QMapIterator it1(_resultats);
    while (it1.hasNext()) {
        it1.next();

        QListIterator it2(it1.value());
        while (it2.hasNext()) {

            const QList<ResultatPrevisions> list = it2.next();

            elems.clear();
            switch (_typeCalcul) {

            case TypeCalcul::PREVISIONS:
            case TypeCalcul::STARLINK:
                elems = ElementsPrevisions(list);
                break;

            case TypeCalcul::FLASHS:
                elems = ElementsFlashs(list);
                break;

            case TypeCalcul::TRANSITS:
                elems = ElementsTransits(list);
                break;

            case TypeCalcul::EVENEMENTS:
                imax = static_cast<int> (list.size());
                break;

            case TypeCalcul::TELESCOPE:
            default:
                break;
            }

            for(int i=0; i<imax; i++) {

                if (_typeCalcul == TypeCalcul::EVENEMENTS) {
                    elems = ElementsEvenements(i, list);
                }

                // Ajout d'une ligne dans le tableau de resultats
                _ui->resultatsPrevisions->insertRow(j);
                _ui->resultatsPrevisions->setRowHeight(j, 16);

                if (_typeCalcul == TypeCalcul::STARLINK) {
                    elems[0] = elems[0].remove("STACK").trimmed();
                }

                for(int k=0; k<elems.count(); k++) {

                    // Remplissage des elements d'une ligne
                    item = new QTableWidgetItem(elems.at(k));
                    item->setTextAlignment(((_typeCalcul == TypeCalcul::EVENEMENTS) && (k == (elems.count() - 1))) ? Qt::AlignLeft : Qt::AlignCenter);
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

                    if (k == 0) {
                        item->setToolTip(elems.at(0));
                        item->setData(Qt::UserRole, QVariant::fromValue<QList<ResultatPrevisions> > (list));
                    }

                    _ui->resultatsPrevisions->setItem(j, k, item);
                    if ((k > 0) || (_typeCalcul == TypeCalcul::TRANSITS)) {
                        _ui->resultatsPrevisions->resizeColumnToContents(k);
                    }
                }

                j++;
            }
        }
    }

    _ui->resultatsPrevisions->setStyleSheet("QHeaderView::section {" \
                                            "background-color:rgb(235, 235, 235);" \
                                            "border-top: 0px solid grey;" \
                                            "border-bottom: 1px solid grey;" \
                                            "border-right: 1px solid grey;" \
                                            "font-size: 12px;" \
                                            "font-weight: 600 }");

    if (_typeCalcul == TypeCalcul::TRANSITS) {
        _ui->resultatsPrevisions->setColumnWidth(0, 140);
    }

    _ui->resultatsPrevisions->horizontalHeader()->setStretchLastSection(true);
    if (_typeCalcul != TypeCalcul::EVENEMENTS) {
        _ui->resultatsPrevisions->setToolTip(tr("Double-cliquez sur une ligne pour afficher plus de détails"));
    }

    _ui->resultatsPrevisions->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    _ui->resultatsPrevisions->setAlternatingRowColors(true);

    if (_typeCalcul == TypeCalcul::FLASHS) {

        if (_conditions.chrono) {
            _ui->resultatsPrevisions->sortItems(1);
        }

        int i = 0;
        while (i < _ui->resultatsPrevisions->rowCount()) {

            const auto list = _ui->resultatsPrevisions->findItems(_ui->resultatsPrevisions->item(i, 1)->text(), Qt::MatchCaseSensitive);
            if (list.size() > 1) {
                _ui->resultatsPrevisions->removeRow(i);
            } else {
                i++;
            }
        }
    }

    _ui->resultatsPrevisions->blockSignals(etat);
    _ui->resultatsPrevisions->selectRow(0);

    /* Retour */
    return;
}

/*
 * Ecriture de l'entete du fichier de resultats
 */
void AfficherResultats::EcrireEntete() const
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
    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream flux(&fichier);
        flux.setEncoding(QStringConverter::Utf8);

        // Ligne d'entete
#if (BUILD_TEST == false)
        flux << QString("%1 %2 / %3 (c) %4").arg(APP_NAME).arg(QString(APP_VERSION)).arg(ORG_NAME).arg(QString(APP_ANNEES_DEV)) << Qt::endl << Qt::endl;
#endif

        // Lieu d'observation
        if (_typeCalcul != TypeCalcul::EVENEMENTS) {

            ligne = QObject::tr("Lieu d'observation              : %1     %2 %3   %4 %5   %6 %7");
            const QString lon = Maths::ToSexagesimal(fabs(_conditions.observateur.longitude()), AngleFormatType::DEGRE, 3, 0, false, false);
            const QString ew = (_conditions.observateur.longitude() >= 0.) ? QObject::tr("Ouest") : QObject::tr("Est");

            const QString lat = Maths::ToSexagesimal(fabs(_conditions.observateur.latitude()), AngleFormatType::DEGRE, 2, 0, false, false);
            const QString ns = (_conditions.observateur.latitude() >= 0.) ? QObject::tr("Nord") : QObject::tr("Sud");

            const double alt = (_conditions.unite == QObject::tr("km", "kilometer")) ? _conditions.observateur.altitude() :
                                                                                       _conditions.observateur.altitude() * TERRE::PIED_PAR_METRE;
            const QString unite = (_conditions.unite == QObject::tr("km", "kilometer")) ? QObject::tr("m", "meter") : QObject::tr("ft", "foot");

            ligne = ligne.arg(_conditions.observateur.nomlieu()).arg(lon).arg(ew).arg(lat).arg(ns).arg(1000. * alt, 0, 'f', 0).arg(unite);
            flux << ligne << Qt::endl;
        }

        // Fuseau horaire
        ligne = QObject::tr("Fuseau horaire                  : %1");
        QString chaine = QObject::tr("UTC", "Universal Time Coordinated");
        if (
        #if (BUILD_TEST == false)
                !settings.value("affichage/utc").toBool() &&
        #endif
                (fabs(_conditions.offset) > MATHS::EPSDBL100)) {
            QTime heur(0, 0);
            heur = heur.addSecs((int) (_conditions.offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES));
            chaine = chaine.append((_conditions.offset > 0.) ? " + " : " - ").append(heur.toString("HH:mm"));
        }
        flux << ligne.arg(chaine) << Qt::endl;

        if (_typeCalcul != TypeCalcul::EVENEMENTS) {

            const QString cond1 = QObject::tr("Conditions d'observations       :") + " ";
            const QString cond2 = QObject::tr("Hauteur minimale du satellite = %1°");

            // Conditions d'observations
            if (_typeCalcul == TypeCalcul::TRANSITS) {
                flux << (cond1 + cond2).arg(_conditions.hauteur * MATHS::RAD2DEG) << Qt::endl;
            } else {
                flux << cond1 + QObject::tr("Hauteur maximale du Soleil = %1°").arg(_conditions.crepuscule * MATHS::RAD2DEG) << " / ";
                flux << cond2.arg(_conditions.hauteur * MATHS::RAD2DEG) << Qt::endl;
            }

            // Unite de distance
            flux << tr("Unité de distance               : %1").arg(_conditions.unite) << Qt::endl;
        }

        // Age des elements orbitaux
        const QString date = Date(_conditions.jj1 + _conditions.offset + DATE::EPS_DATES, 0.)
                .ToShortDate(DateFormat::FORMAT_COURT, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H).trimmed();

        if (_donnees.ageElementsOrbitaux.count() == 1) {
            flux << Qt::endl << tr("Age de l'élément                : %1 jours (au %2)").arg(_donnees.ageElementsOrbitaux.at(0), 4, 'f', 2).arg(date)
                 << Qt::endl << Qt::endl << Qt::endl;
        } else {

            flux << Qt::endl << tr("Age de l'élément le plus récent : %1 jours (au %2)\nAge de l'élément le plus ancien : %3 jours")
                    .arg(_donnees.ageElementsOrbitaux.at(0), 4, 'f', 2).arg(date).arg(_donnees.ageElementsOrbitaux.at(1), 4, 'f', 2)
                 << Qt::endl << Qt::endl << Qt::endl;
        }
    }
    fichier.close();

    /* Retour */
    return;
}

/*
 * Elements des evenements a afficher dans la fenetre de resultats
 */
QStringList AfficherResultats::ElementsEvenements(const int index, const QList<ResultatPrevisions> &liste) const
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
QStringList AfficherResultats::ElementsDetailsEvenements(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const double offset = Date::CalculOffsetUTC(res.date.ToQDateTime(1));
    const Date date(res.date, offset);
    elems.append(date.ToShortDateAMJ(DateFormat::FORMAT_COURT, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // PSO
    elems.append(QString("%1°").arg(res.pso * MATHS::RAD2DEG, 6, 'f', 2, QChar('0')));

    // Longitude, latitude
    elems.append(QString("  %1° %2")
                 .arg(fabs(res.longitude * MATHS::RAD2DEG), 6, 'f', 2, QChar('0')).arg((res.longitude >= 0.) ? tr("W", "West") : tr("E", "East")));
    elems.append(QString(" %1° %2 ")
                 .arg(fabs(res.latitude * MATHS::RAD2DEG), 5, 'f', 2, QChar('0')).arg((res.latitude >= 0.) ? tr("N", "North") : tr("S", "South")));

    // Type d'evenement
    elems.append(res.typeEvenement);

    /* Retour */
    return elems;
}

/*
 * Elements des flashs a afficher dans la fenetre de resultats
 */
QStringList AfficherResultats::ElementsFlashs(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(liste.first().nom);

    // Date de debut
    const double offset1 = Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1));
    const Date dateDeb(liste.first().date, offset1);
    elems.append(dateDeb.ToShortDateAMJ(DateFormat::FORMAT_LONG, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Date de fin
    const double offset2 = Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1));
    const Date dateFin(liste.last().date, offset2);
    elems.append(dateFin.ToShortDateAMJ(DateFormat::FORMAT_LONG, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    double htMax = -1.;
    double magnMax = 99.;
    double htSolMax = 0.;
    bool penombre = false;
    QString miroir;

    QListIterator it(liste);
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
    elems.append(Maths::ToSexagesimal(htMax, AngleFormatType::DEGRE, 2, 0, false, false));

    // Magnitude
    const QString fmagn = "%1%2%3";
    const QString signe = (magnMax >= 0.) ? "+" : "-";
    const QString pen = (penombre) ? "*" : " ";
    elems.append(fmagn.arg(signe).arg(fabs(magnMax), 0, 'f', 1, QChar('0')).arg(pen));

    // Panneau reflechissant
    elems.append(miroir);

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, AngleFormatType::DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des flashs pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList AfficherResultats::ElementsDetailsFlashs(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(QString("%1").arg(res.nom, -10));

    // Date
    const double offset = Date::CalculOffsetUTC(res.date.ToQDateTime(1));
    const Date date(res.date, offset);
    elems.append(date.ToShortDateAMJ(DateFormat::FORMAT_LONG, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, AngleFormatType::DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, AngleFormatType::DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, AngleFormatType::HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, AngleFormatType::DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Angle de reflexion, miroir
    elems.append(QString("%1  ").arg(res.angleReflexion * MATHS::RAD2DEG, 5, 'f', 2));
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
        altitude *= TERRE::MILE_PAR_KM;
        distance *= TERRE::MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, AngleFormatType::DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, AngleFormatType::DEGRE, 2, 0, true, false));

    if (!res.obsmax.nomlieu().isEmpty()) {

        const QString ew = (res.obsmax.longitude() >= 0.) ? QObject::tr("W", "West") : QObject::tr("E", "East");
        const QString ns = (res.obsmax.latitude() >= 0.) ? QObject::tr("N", "North") : QObject::tr("S", "South");

        // Longitude du maximum
        elems.append(QString("  %1 %2").arg(fabs(res.obsmax.longitude() * MATHS::RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew));

        // Latitude du maximum
        elems.append(QString(" %1 %2    ").arg(fabs(res.obsmax.latitude() * MATHS::RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns));

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
QStringList AfficherResultats::ElementsPrevisions(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(liste.first().nom);

    // Date de debut
    const double offset1 = Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1));
    const Date dateDeb(liste.first().date, offset1);
    elems.append(dateDeb.ToShortDateAMJ(DateFormat::FORMAT_COURT, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Date de fin
    const double offset2 = Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1));
    const Date dateFin(liste.last().date, offset2);
    elems.append(dateFin.ToShortDateAMJ(DateFormat::FORMAT_COURT, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    double htMax = -1.;
    double magnMax = 99.;
    double magnStd = 99.;
    double htSolMax = 0.;
    bool penombre = false;

    QListIterator it(liste);
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
    elems.append(Maths::ToSexagesimal(htMax, AngleFormatType::DEGRE, 2, 0, false, false));

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
    elems.append(Maths::ToSexagesimal(htSolMax, AngleFormatType::DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des previsions de passage pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList AfficherResultats::ElementsDetailsPrevisions(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const double offset = Date::CalculOffsetUTC(res.date.ToQDateTime(1));
    const Date date(res.date, offset);
    elems.append(date.ToShortDateAMJ(DateFormat::FORMAT_COURT, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, AngleFormatType::DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, AngleFormatType::DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, AngleFormatType::HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, AngleFormatType::DEGRE, 2, 0, true, false));
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
        altitude *= TERRE::MILE_PAR_KM;
        distance *= TERRE::MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, AngleFormatType::DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, AngleFormatType::DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des transits a afficher dans la fenetre de resultats
 */
QStringList AfficherResultats::ElementsTransits(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */
    const ResultatPrevisions elem = liste.at(2);

    /* Corps de la methode */
    // Nom du satellite
    elems.append(elem.nom);

    // Date du maximum
    const double offset1 = Date::CalculOffsetUTC(liste.at(2).date.ToQDateTime(1));
    const Date dateMax(elem.date, offset1);
    elems.append(dateMax.ToShortDateAMJ(DateFormat::FORMAT_LONG, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Constellation
    elems.append(elem.constellation);

    // Angle minimal
    elems.append(QString("%1").arg(elem.angle * MATHS::RAD2DEG, 5, 'f', 2));

    // Type, corps
    elems.append((elem.transit) ? tr("T", "transit") : tr("C", "conjunction"));
    elems.append((elem.typeCorps == CorpsTransit::CORPS_SOLEIL) ? tr("S", "Sun") : tr("L", "Moon"));

    // Illumination
    QString illumination = tr("Lum", "Lit");
    if (elem.eclipse) {
        illumination = tr("Omb", "Shadow");
    }

    if (elem.penombre) {
        illumination = tr("Pen", "Penumbra");
    }

    elems.append(illumination);

    // Duree du transit ou de la conjonction
    elems.append(QString("%1").arg(elem.duree, 4, 'f', 1));

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(elem.hauteurSoleil, AngleFormatType::DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des transits pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList AfficherResultats::ElementsDetailsTransits(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const double offset = Date::CalculOffsetUTC(res.date.ToQDateTime(1));
    const Date date(res.date, offset);
    elems.append(date.ToShortDateAMJ(DateFormat::FORMAT_LONG, (_conditions.systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, AngleFormatType::DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, AngleFormatType::DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, AngleFormatType::HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, AngleFormatType::DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Angle de reflexion, type, corps
    elems.append(QString("%1  ").arg(res.angle * MATHS::RAD2DEG, 5, 'f', 2));
    elems.append(QString("%1   ").arg((res.transit) ? tr("T", "transit") : tr("C", "conjunction")));
    elems.append(QString("%1  ").arg((res.typeCorps == CorpsTransit::CORPS_SOLEIL) ? tr("S", "Sun") : tr("L", "Moon")));

    // Illumination
    QString illumination = tr("Lum", "Lit");
    if (res.eclipse) {
        illumination = tr("Omb", "Shadow");
    }

    if (res.penombre) {
        illumination = tr("Pen", "Penumbra");
    }

    elems.append(illumination);

    // Duree du transit ou de la conjonction
    elems.append((res.duree > 0.) ? QString("%1").arg(res.duree, 5, 'f', 1) : "");

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi", "nautical mile")) {
        altitude *= TERRE::MILE_PAR_KM;
        distance *= TERRE::MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, AngleFormatType::DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, AngleFormatType::DEGRE, 2, 0, true, false));

    if (!res.obsmax.nomlieu().isEmpty()) {

        const QString ew = (res.obsmax.longitude() >= 0.) ? QObject::tr("W", "West") : QObject::tr("E", "East");
        const QString ns = (res.obsmax.latitude() >= 0.) ? QObject::tr("N", "North") : QObject::tr("S", "South");

        // Longitude du maximum
        elems.append(QString("  %1 %2").arg(fabs(res.obsmax.longitude() * MATHS::RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew));

        // Latitude du maximum
        elems.append(QString(" %1 %2 ").arg(fabs(res.obsmax.latitude() * MATHS::RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns));

        // Distance au maximum et cap
        elems.append(QString(" %1 (%2)").arg(res.distanceObs, 6, 'f', 1).arg(res.cap));
    }

    /* Retour */
    return elems;
}

void AfficherResultats::on_resultatsPrevisions_itemDoubleClicked(QTableWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_typeCalcul != TypeCalcul::EVENEMENTS) {

        int j = 0;
        int m = 0;
        const QList<ResultatPrevisions> list = _ui->resultatsPrevisions->item(item->row(), 0)->data(Qt::UserRole).value<QList<ResultatPrevisions> > ();

        if (_tableDetail != nullptr) {
            delete _tableDetail;
            _tableDetail = nullptr;
        }

        _tableDetail = new QTableWidget;

        switch (_typeCalcul) {

        case TypeCalcul::PREVISIONS:
        case TypeCalcul::STARLINK:
            _tableDetail->setColumnCount(12);
            _tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date", "Date and hour")
                                                    << tr("Azimut Sat", "Satellite azimuth") << tr("Hauteur Sat", "Satellite elevation")
                                                    << tr("AD Sat", "Satellite right ascension") << tr("Decl Sat", "Satellite declination")
                                                    << tr("Const", "Constellation") << tr("Magn", "Magnitude")
                                                    << tr("Altitude", "Altitude of satellite") << tr("Distance") << tr("Az Soleil", "Solar azimuth")
                                                    << tr("Haut Soleil", "Solar elevation"));
            break;

        case TypeCalcul::FLASHS:
            _tableDetail->setColumnCount(18);
            _tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date", "Date and hour")
                                                    << tr("Azimut Sat", "Satellite azimuth") << tr("Hauteur Sat", "Satellite elevation")
                                                    << tr("AD Sat", "Satellite right ascension") << tr("Decl Sat", "Satellite declination")
                                                    << tr("Const", "Constellation") << tr("Ang", "Angle") << tr("Mir", "Mirror")
                                                    << tr("Magn", "Magnitude") << tr("Altitude", "Altitude of satellite") << tr("Dist", "Range")
                                                    << tr("Az Soleil", "Solar azimuth") << tr("Haut Soleil", "Solar elevation")
                                                    << tr("Long Max", "Longitude of the maximum") << tr("Lat Max", "Latitude of the maximum")
                                                    << tr("Magn Max", "Magnitude at the maximum") << tr("Distance"));
            break;

        case TypeCalcul::TRANSITS:
            _tableDetail->setColumnCount(19);
            _tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date", "Date and hour")
                                                    << tr("Azimut Sat", "Satellite azimuth") << tr("Hauteur Sat", "Satellite elevation")
                                                    << tr("AD Sat", "Satellite right ascension") << tr("Decl Sat", "Satellite declination")
                                                    << tr("Cst", "Constellation") << tr("Ang", "Angle") << tr("Type", "Transit or conjunction")
                                                    << tr("Corps") << tr("Ill", "Illumination") << tr("Durée") << tr("Alt", "Altitude of satellite")
                                                    << tr("Dist", "Range") << tr("Az Soleil", "Solar azimuth") << tr("Haut Soleil", "Solar elevation")
                                                    << tr("Long Max", "Longitude of the maximum") << tr("Lat Max", "Latitude of the maximum")
                                                    << tr("Distance"));
            break;

        case TypeCalcul::TELESCOPE:
        case TypeCalcul::EVENEMENTS:
        default:
            break;
        }

        _tableDetail->setSelectionMode(QTableWidget::NoSelection);
        _tableDetail->setCornerButtonEnabled(false);
        _tableDetail->verticalHeader()->setVisible(false);

        _tableDetail->horizontalHeader()->setToolTip("");
        _tableDetail->horizontalHeaderItem(2)->setToolTip(tr("Azimut satellite"));
        _tableDetail->horizontalHeaderItem(3)->setToolTip(tr("Hauteur satellite"));
        _tableDetail->horizontalHeaderItem(4)->setToolTip(tr("Ascension droite satellite"));
        _tableDetail->horizontalHeaderItem(5)->setToolTip(tr("Déclinaison satellite"));
        _tableDetail->horizontalHeaderItem(6)->setToolTip(tr("Constellation"));

        switch (_typeCalcul) {

        case TypeCalcul::PREVISIONS:
        case TypeCalcul::STARLINK:
            _tableDetail->horizontalHeaderItem(7)->setToolTip(tr("Magnitude"));
            _tableDetail->horizontalHeaderItem(10)->setToolTip(tr("Azimut Soleil"));
            _tableDetail->horizontalHeaderItem(11)->setToolTip(tr("Hauteur Soleil"));
            break;

        case TypeCalcul::FLASHS:
            _tableDetail->horizontalHeaderItem(7)->setToolTip(tr("Angle"));
            _tableDetail->horizontalHeaderItem(8)->setToolTip(tr("Miroir"));
            _tableDetail->horizontalHeaderItem(9)->setToolTip(tr("Magnitude"));
            _tableDetail->horizontalHeaderItem(11)->setToolTip(tr("Distance"));
            _tableDetail->horizontalHeaderItem(12)->setToolTip(tr("Azimut Soleil"));
            _tableDetail->horizontalHeaderItem(13)->setToolTip(tr("Hauteur Soleil"));
            _tableDetail->horizontalHeaderItem(14)->setToolTip(tr("Longitude du maximum"));
            _tableDetail->horizontalHeaderItem(15)->setToolTip(tr("Latitude du maximum"));
            _tableDetail->horizontalHeaderItem(16)->setToolTip(tr("Magnitude au maximum"));
            _tableDetail->horizontalHeaderItem(17)->setToolTip(tr("Distance au maximum"));
            break;

        case TypeCalcul::TRANSITS:
            _tableDetail->horizontalHeaderItem(7)->setToolTip(tr("Angle"));
            _tableDetail->horizontalHeaderItem(10)->setToolTip(tr("Illumination"));
            _tableDetail->horizontalHeaderItem(11)->setToolTip(tr("secondes"));
            _tableDetail->horizontalHeaderItem(12)->setToolTip(tr("Altitude", "Altitude of satellite"));
            _tableDetail->horizontalHeaderItem(13)->setToolTip(tr("Distance"));
            _tableDetail->horizontalHeaderItem(14)->setToolTip(tr("Azimut Soleil"));
            _tableDetail->horizontalHeaderItem(15)->setToolTip(tr("Hauteur Soleil"));
            _tableDetail->horizontalHeaderItem(16)->setToolTip(tr("Longitude du maximum"));
            _tableDetail->horizontalHeaderItem(17)->setToolTip(tr("Latitude du maximum"));
            _tableDetail->horizontalHeaderItem(18)->setToolTip(tr("Distance au maximum"));
            break;

        case TypeCalcul::EVENEMENTS:
        case TypeCalcul::TELESCOPE:
        default:
            break;
        }

        int kmax;
        int nmax = 0;
        QStringList elems;
        QTableWidgetItem * itm;
        QListIterator it(list);
        while (it.hasNext()) {

            elems.clear();
            const ResultatPrevisions res = it.next();

            switch (_typeCalcul) {
            case TypeCalcul::PREVISIONS:
            case TypeCalcul::STARLINK:
                elems = ElementsDetailsPrevisions(res);
                break;

            case TypeCalcul::FLASHS:
                elems = ElementsDetailsFlashs(res);
                break;

            case TypeCalcul::TRANSITS:
                if ((j > 0) && (j < 4)) {
                    elems = ElementsDetailsTransits(res);
                }
                break;

            case TypeCalcul::TELESCOPE:
            case TypeCalcul::EVENEMENTS:
            default:
                break;
            }

            // Ajout d'une ligne dans le tableau de resultats
            if (!elems.isEmpty()) {

                _tableDetail->insertRow(m);
                _tableDetail->setRowHeight(m, 16);

                kmax = _tableDetail->columnCount();
                if (_typeCalcul == TypeCalcul::FLASHS) {
                    if (j == 1) {
                        kmax = qMin(kmax, static_cast<int> (elems.count()));
                        nmax = kmax;
                    } else {
                        kmax = qMin(kmax - 4, static_cast<int> (elems.count()));
                    }
                }

                if (_typeCalcul == TypeCalcul::STARLINK) {
                    elems[0] = elems[0].remove("STACK").trimmed();
                }

                for(int k=0; k<kmax; k++) {

                    // Remplissage des elements d'une ligne
                    itm = new QTableWidgetItem(elems.at(k).trimmed());
                    itm->setTextAlignment(Qt::AlignCenter);
                    itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);

                    if ((k == 0) && (_typeCalcul != TypeCalcul::TRANSITS)) {
                        itm->setToolTip(elems.at(0));
                    }

                    _tableDetail->setItem(m, k, itm);
                    _tableDetail->resizeColumnToContents(k);
                }
                m++;
            }

            j++;
        }

        if (_typeCalcul == TypeCalcul::FLASHS) {
            _tableDetail->setColumnCount(nmax);
        }

        _tableDetail->setSelectionMode(QTableWidget::SingleSelection);
        _tableDetail->setSelectionBehavior(QTableWidget::SelectRows);
        _tableDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

        if (_afficherDetail != nullptr) {
            delete _afficherDetail;
            _afficherDetail = nullptr;
        }

        _afficherDetail = new QMainWindow;
        _afficherDetail->setStyleSheet("QHeaderView::section {" \
                                       "background-color:rgb(235, 235, 235);" \
                                       "border-top: 0px solid grey;" \
                                       "border-bottom: 1px solid grey;" \
                                       "border-right: 1px solid grey;" \
                                       "font-size: 12px;" \
                                       "font-weight: 600 }");

        switch (_typeCalcul) {

        case TypeCalcul::PREVISIONS:
        case TypeCalcul::STARLINK:
            _afficherDetail->setWindowTitle(tr("Détail du passage"));
            break;

        case TypeCalcul::FLASHS:
            _afficherDetail->setWindowTitle(tr("Détail du flash"));
            break;

        case TypeCalcul::TRANSITS:
            _afficherDetail->setWindowTitle(tr("Détail du transit ou conjonction"));
            break;

        case TypeCalcul::TELESCOPE:
        case TypeCalcul::EVENEMENTS:
        default:
            break;
        }

        _afficherDetail->setCentralWidget(_tableDetail);
#if defined (Q_OS_LINUX)
        const int lrg = 5;
#else
        const int lrg = 2;
#endif

        _tableDetail->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        _tableDetail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _tableDetail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _tableDetail->resizeColumnsToContents();

        _tableDetail->setFixedSize(_tableDetail->horizontalHeader()->length() + _tableDetail->verticalHeader()->width() + lrg,
                                   _tableDetail->verticalHeader()->length() + _tableDetail->horizontalHeader()->height() + lrg);

        _afficherDetail->setFixedSize(_tableDetail->size());
        _afficherDetail->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, _afficherDetail->size(), geometry()));
        _afficherDetail->show();
    }

    /* Retour */
    return;
}

void AfficherResultats::on_actionEnregistrer_triggered()
{
    /* Declarations des variables locales */
    QScreen *fenetre = QGuiApplication::primaryScreen();

    /* Initialisations */
    const QPixmap image = fenetre->grabWindow(_ui->centralwidget->winId());
    const QString nomFicDefaut = Configuration::instance()->dirOut() + QDir::separator() +
                                 _conditions.ficRes.split(QDir::separator()).last().remove(".txt");

    /* Corps de la methode */
    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), nomFicDefaut,
                                                     tr("Fichiers PNG (*.png);;Fichiers JPEG (*.jpg *.jpeg);;Fichiers BMP (*.bmp);;" \
                                                        "Tous les fichiers (*.*)"));

    if (!fic.isEmpty()) {
        image.save(fic);
        const QFileInfo fi(fic);
        settings.setValue("fichier/sauvegarde", fi.absolutePath());
    }

    /* Retour */
    return;
}

void AfficherResultats::on_actionEnregistrerTxt_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
#if (BUILD_TEST == true)
    const QString fic = _conditions.ficRes;
#else
    QString fic;
    if (_afficherEvt) {
        fic = _conditions.ficRes;
        _afficherEvt = false;
    } else {
        const QString nomFicDefaut = _conditions.ficRes.split(QDir::separator()).last();
        fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), Configuration::instance()->dirOut() + QDir::separator() +
                                           nomFicDefaut, tr("Fichiers texte (*.txt);;Tous les fichiers (*.*)"));
    }
#endif

    if (!fic.isEmpty()) {

        QFile fichier(_conditions.ficRes);
        if (fichier.exists()) {
            fichier.remove();
        }

        // Ecriture de l'entete du fichier de resultat
        EcrireEntete();

        // Ouverture du fichier de resultat
        if (fichier.open(QIODevice::Append | QIODevice::Text)) {

            QTextStream flux(&fichier);
            flux.setEncoding(QStringConverter::Utf8);

            bool ecr = true;
            int i;
            int kmin;
            int kmax;
            QString ligne;
            QString nomsat;
            QStringList evts;
            QStringList flashs;
            QStringList elems;

            QMapIterator it1(_resultats);
            while (it1.hasNext()) {
                it1.next();

                // Nom du satellite
                nomsat = it1.value().at(0).at(0).nom;
                if (nomsat.contains("R/B") || nomsat.contains(" DEB")) {
                    nomsat += "  " + tr("(numéro NORAD : %1)").arg(it1.key().split(" ").last());
                }

                // Description des colonnes
                switch (_typeCalcul) {

                case TypeCalcul::PREVISIONS:
                    flux << nomsat << Qt::endl;
                    flux << tr("   Date      Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const Magn  Altitude  Distance  Az Soleil  " \
                               "Haut Soleil",
                               "Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, " \
                               "Constellation, Magnitude, Altitude of satellite, Range, Solar azimuth, Solar elevation")
                         << Qt::endl;
                    break;

                case TypeCalcul::FLASHS:
                    if (ecr) {
                        flux << tr("Satellite     Date      Heure      Azimut Sat Hauteur Sat  AD Sat    Decl Sat   Cst  Ang   Mir Magn       " \
                                   "Alt      Dist  Az Soleil  Haut Soleil   Long Max    Lat Max    Magn Max  Distance",
                                   "Satellite, Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, " \
                                   "Constellation, Angle, Mirror, Magnitude, Altitude of satellite, Range, Solar azimuth, Solar elevation, " \
                                   "Longitude of the maximum, Latitude of the maximum, Magnitude at the maximum, Range from the maximum")
                             << Qt::endl;
                        ecr = false;
                    }
                    break;

                case TypeCalcul::TRANSITS:
                    flux << nomsat << Qt::endl;
                    flux << tr("   Date      Heure      Azimut Sat Hauteur Sat  AD Sat    Decl Sat   Cst  Ang  Type Corps " \
                               "Ill Durée  Altitude  Distance  Az Soleil  Haut Soleil   Long Max    Lat Max     Distance",
                               "Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, " \
                               "Constellation, Angle, Type, Body, Illumination, Duration, Altitude of satellite, Range, Solar azimuth, " \
                               "Solar elevation, Longitude of the maximum, Latitude of the maximum, Range from the maximum")
                         << Qt::endl;
                    break;

                case TypeCalcul::STARLINK:
                    flux << nomsat.remove("STACK").trimmed() << Qt::endl;
                    flux << tr("   Date      Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const Magn  Altitude  Distance  Az Soleil  " \
                               "Haut Soleil",
                               "Date, Hour, Satellite azimuth, Satellite elevation, Satellite right ascension, Satellite declination, " \
                               "Constellation, Magnitude, Altitude of satellite, Range, Solar azimuth, Solar elevation")
                         << Qt::endl;
                    break;

                case TypeCalcul::EVENEMENTS:
                    flux << nomsat << Qt::endl;
                    flux << tr("   Date      Heure     PSO    Longitude  Latitude  Évènements",
                               "Date, Hour, In orbit position, Longitude, Latitude, Events") << Qt::endl;
                    break;

                case TypeCalcul::TELESCOPE:
                default:
                    break;
                }

                QListIterator it2(it1.value());
                while (it2.hasNext()) {

                    i = 0;
                    evts.clear();
                    QListIterator it3(it2.next());
                    while (it3.hasNext()) {

                        kmin = 1;
                        elems.clear();
                        const ResultatPrevisions res = it3.next();

                        switch (_typeCalcul) {
                        case TypeCalcul::PREVISIONS:
                        case TypeCalcul::STARLINK:
                            elems = ElementsDetailsPrevisions(res);
                            break;

                        case TypeCalcul::FLASHS:
                            kmin = 0;
                            elems = ElementsDetailsFlashs(res);
                            break;

                        case TypeCalcul::TRANSITS:
                            if ((i > 0) && (i < 4)) {
                                elems = ElementsDetailsTransits(res);
                            }
                            break;

                        case TypeCalcul::EVENEMENTS:
                            elems = ElementsDetailsEvenements(res);
                            break;

                        case TypeCalcul::TELESCOPE:
                        default:
                            break;
                        }

                        if (!elems.isEmpty()) {

                            kmax = static_cast<int> (elems.count());

                            if (_typeCalcul == TypeCalcul::FLASHS) {
                                if (i == 1) {
                                    kmax = qMin(18, static_cast<int> (elems.count()));
                                } else {
                                    kmax = qMin(14, static_cast<int> (elems.count()));
                                }
                            }

                            ligne = "";
                            for(int k=kmin; k<kmax; k++) {
                                if (_typeCalcul == TypeCalcul::EVENEMENTS) {
                                    ligne += elems.at(k).trimmed() + "  ";
                                } else if (_conditions.chrono && (_typeCalcul == TypeCalcul::FLASHS)) {

                                    if (k < 2) {
                                        ligne += elems.at(1 - k) + " ";
                                    } else {
                                        ligne += elems.at(k) + " ";
                                    }

                                } else {
                                    if (elems.at(k).isEmpty()) {
                                        ligne += "      ";
                                    } else {
                                        ligne += elems.at(k) + " ";
                                    }
                                }
                            }

                            if (_typeCalcul == TypeCalcul::EVENEMENTS) {
                                evts.append(ligne);
                            } else if (_typeCalcul == TypeCalcul::FLASHS) {
                                flashs.append(ligne);
                            } else {
                                flux << ligne.trimmed() << Qt::endl;
                            }
                        }
                        i++;
                    }

                    if (_typeCalcul == TypeCalcul::EVENEMENTS) {

                        evts.sort();
                        QStringListIterator it4(evts);
                        while (it4.hasNext()) {
                            flux << it4.next().trimmed() << Qt::endl;
                        }
                    }

                    if (_typeCalcul != TypeCalcul::FLASHS) {
                        flux << Qt::endl;
                    }
                }
            }

            if (_typeCalcul == TypeCalcul::FLASHS) {

                if (_conditions.chrono) {
                    flashs.sort();
                }

                unsigned int cr = 0;
                QStringListIterator it4(flashs);
                while (it4.hasNext()) {

                    const QString lg = it4.next().trimmed();

                    if (_conditions.chrono) {
                        flux << lg.mid(23, 11) << lg.mid(0, 23) << lg.mid(34) << Qt::endl;
                    } else {
                        flux << lg << Qt::endl;
                    }

                    if ((cr % 3) == 2) {
                        flux << Qt::endl;
                    }
                    cr++;
                }
            }

#if (BUILD_TEST == false)
            flux << Qt::endl << tr("Temps écoulé : %1s").arg(1.e-3 * static_cast<double> (_donnees.tempsEcoule), 0, 'f', 2) << Qt::endl;
#endif
        }
        fichier.close();

#if (BUILD_TEST == false)
        QFile fi2(fic);
        if (fi2.exists() && (fic != _conditions.ficRes)) {
            fi2.remove();
        }
        fichier.copy(fi2.fileName());
#endif
    }

    /* Retour */
    return;
}

void AfficherResultats::on_resultatsPrevisions_itemSelectionChanged()
{
    /* Declarations des variables locales */
    Date dateMax;
    Soleil soleil;
    Lune lune;
    QList<LigneConstellation> lignesCst;
    std::array<Planete, PLANETE::NB_PLANETES> planetes;
    QList<Satellite> satellites;

    /* Initialisations */
    const QList<ResultatPrevisions> list = _ui->resultatsPrevisions->item(_ui->resultatsPrevisions->currentRow(), 0)->data(Qt::UserRole)
            .value<QList<ResultatPrevisions> > ();
    const Date dateDeb = Date(list.first().date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));
    const Date dateFin = Date(list.last().date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));

    if (_typeCalcul == TypeCalcul::FLASHS) {
        dateMax = Date(list.at(1).date, Date::CalculOffsetUTC(list.first().date.ToQDateTime(1)));
    } else if (_typeCalcul == TypeCalcul::TRANSITS) {
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
    if (settings.value("affichage/affconst").toUInt() != Qt::Unchecked) {
        Constellation::CalculConstellations(observateur, constellations);
        LigneConstellation::CalculLignesCst(etoiles, lignesCst);
    }

    // Calcul de la position des planetes
    for(unsigned int i=0; i<PLANETE::NB_PLANETES; i++) {

        planetes[i].CalculPosition(dateDeb, soleil);
        planetes[i].CalculCoordHoriz(observateur);
    }

    // Satellite selectionne
    Satellite sat(list.first().elements);
    sat.CalculPosVit(dateMax);
    sat.CalculCoordHoriz(observateur);
    sat.CalculElementsOsculateurs(dateMax);

    const Date dateLever = Evenements::CalculAOS(dateMax, sat, observateur, SensCalcul::ANTI_CHRONOLOGIQUE).date;
    sat.CalculTraceCiel(dateLever, true, settings.value("affichage/refractionAtmospherique").toBool(), observateur, 1);
    satellites.append(sat);

    // Chargement du ciel
    if (_ciel != nullptr) {
        delete _ciel;
        _ciel = nullptr;
    }
    _ciel = new Ciel(_ui->frameCiel);
    _ciel->resize(_ui->frameCiel->width(), _ui->frameCiel->width() - 50);

    // Affichage de la carte du ciel
    _ciel->show(observateur,
                soleil,
                lune,
                lignesCst,
                constellations,
                etoiles,
                planetes,
                satellites,
                true,
                (_typeCalcul == TypeCalcul::FLASHS),
                true,
                dateDeb,
                dateMax,
                dateFin);

    // Affichage du detail du transit
    if (_typeCalcul == TypeCalcul::TRANSITS) {

        _ui->detailsTransit->setVisible(list.at(2).transit);

        if (list.at(2).transit) {
            AffichageDetailTransit(observateur, soleil, lune, list);
        }
    }

    if ((_typeCalcul == TypeCalcul::FLASHS) || (_typeCalcul == TypeCalcul::TRANSITS)) {
        ChargementCarte(observateur, list);
    }

    /* Retour */
    return;
}

void AfficherResultats::on_afficherCarte_clicked()
{
    // Ouverture du navigateur
    QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + "resultat.html");
    if (fi.exists() && (fi.size() != 0)) {
        QDesktopServices::openUrl(QUrl("file:///" + fi.fileName()));
    }
}
