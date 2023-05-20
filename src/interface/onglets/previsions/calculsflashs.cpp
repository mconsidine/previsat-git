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
 * >    calculsflashs.cpp
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    26 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wshadow"
#include <QDir>
#include <QFutureWatcher>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "ui_calculsflashs.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "calculsflashs.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"
#include "previsions/flashs.h"


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
CalculsFlashs::CalculsFlashs(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsFlashs)
{
    _ui->setupUi(this);

    try {

        _afficherResultats = nullptr;

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
CalculsFlashs::~CalculsFlashs()
{
    EFFACE_OBJET(_afficherResultats);
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::CalculsFlashs *CalculsFlashs::ui() const
{
    return _ui;
}


/*
 * Modificateurs
 */


/*
 * Methodes publiques
 */
/*
 * Affichage des informations de l'onglet
 */
void CalculsFlashs::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeMetOp->setDateTime(date.ToQDateTime(0));
    _ui->dateFinaleMetOp->setDateTime(_ui->dateInitialeMetOp->dateTime().addDays(7));

    CalculAgeElementsOrbitaux();

    /* Retour */
    return;
}

/*
 * Affichage des lieux d'observation dans la liste deroulante
 */
void CalculsFlashs::AffichageLieuObs()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->lieuxObservation->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->observateurs());
    while (it.hasNext()) {
        const QString nomlieu = it.next().nomlieu();
        _ui->lieuxObservation->addItem(nomlieu);
    }

    _ui->lieuxObservation->setCurrentIndex(0);

    /* Retour */
    return;
}

void CalculsFlashs::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
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
/*
 * Calcul de l'age des elements orbitaux
 */
void CalculsFlashs::CalculAgeElementsOrbitaux()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QMap<QString, ElementsOrbitaux> mapElem = GPFormat::LectureFichier(Configuration::instance()->dirElem() + QDir::separator() +
                                                                             "flares-spctrk.xml", Configuration::instance()->donneesSatellites(),
                                                                             Configuration::instance()->lgRec());

    if (mapElem.isEmpty()) {

        _ui->gridLayoutWidget->setVisible(false);

    } else {

        double elemMin = -DATE::DATE_INFINIE;
        double elemMax = DATE::DATE_INFINIE;

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeMetOp->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeMetOp->date().year(), _ui->dateInitialeMetOp->date().month(), _ui->dateInitialeMetOp->date().day(),
                         _ui->dateInitialeMetOp->time().hour(), _ui->dateInitialeMetOp->time().minute(), _ui->dateInitialeMetOp->time().second(), 0.);

        // Jour julien initial
        const double jj1 = date1.jourJulien() - offset1;

        // Determination de l'age maximal des elements orbitaux
        QMapIterator it(mapElem);
        while (it.hasNext()) {
            it.next();

            const ElementsOrbitaux elem = it.value();
            const double epok = elem.epoque.jourJulienUTC();

            if (epok > elemMin) {
                elemMin = epok;
            }

            if (epok < elemMax) {
                elemMax = epok;
            }
        }

        QBrush couleur;
        const std::array<double, 2> age = { fabs(jj1 - elemMin), fabs(jj1 - elemMax) };
        std::array<QPalette, 2> palette;

        for(int i=0; i<2; i++) {

            if (age[i] <= 5.) {
                couleur.setColor(QColor("forestgreen"));
            } else if (age[i] <= 10.) {
                couleur.setColor(Qt::darkYellow);
            } else if (age[i] <= 15.) {
                couleur.setColor(QColor("orange"));
            } else {
                couleur.setColor(Qt::red);
            }

            couleur.setStyle(Qt::SolidPattern);
            palette[i].setBrush(QPalette::WindowText, couleur);
        }

        _ui->ageElementsOrbitaux1->setPalette(palette[0]);
        _ui->ageElementsOrbitaux1->setText(QString("%1").arg(age[0], 0, 'f', 2));

        _ui->ageElementsOrbitaux2->setPalette(palette[1]);
        _ui->ageElementsOrbitaux2->setText(QString("%1").arg(age[1], 0, 'f', 2));

        _ui->gridLayoutWidget->setVisible(true);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de la classe CalculsFlashs
 */
void CalculsFlashs::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->hauteurSatMetOp->setCurrentIndex(settings.value("previsions/hauteurSatMetOp", 2).toInt());
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->hauteurSoleilMetOp->setCurrentIndex(settings.value("previsions/hauteurSoleilMetOp", 1).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationMetOp", 0).toInt());
    _ui->ordreChronologiqueMetOp->setChecked(settings.value("previsions/ordreChronologiqueMetOp", true).toBool());
    _ui->magnitudeMaxMetOp->setValue(settings.value("previsions/magnitudeMaxMetOp", 2.).toDouble());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsFlashs::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)

    settings.setValue("previsions/hauteurSatMetOp", _ui->hauteurSatMetOp->currentIndex());
    settings.setValue("previsions/hauteurSoleilMetOp", _ui->hauteurSoleilMetOp->currentIndex());
    settings.setValue("previsions/lieuxObservationMetOp", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/ordreChronologiqueMetOp", _ui->ordreChronologiqueMetOp->isChecked());
    settings.setValue("previsions/magnitudeMaxMetOp", _ui->magnitudeMaxMetOp->value());
}

void CalculsFlashs::on_calculsFlashs_clicked()
{
    /* Declarations des variables locales */
    ConditionsPrevisions conditions;

    /* Initialisations */
    conditions.listeSatellites.clear();

    /* Corps de la methode */
    try {

        QVector<int> vecSat;
        vecSat.append(0);

        // Fichier d'elements orbitaux
        const QString fichier = Configuration::instance()->dirElem() + QDir::separator() + "flares-spctrk.xml";

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeMetOp->dateTime());
        const double offset2 = Date::CalculOffsetUTC(_ui->dateFinaleMetOp->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeMetOp->date().year(), _ui->dateInitialeMetOp->date().month(), _ui->dateInitialeMetOp->date().day(),
                         _ui->dateInitialeMetOp->time().hour(), _ui->dateInitialeMetOp->time().minute(), _ui->dateInitialeMetOp->time().second(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinaleMetOp->date().year(), _ui->dateFinaleMetOp->date().month(), _ui->dateFinaleMetOp->date().day(),
                         _ui->dateFinaleMetOp->time().hour(), _ui->dateFinaleMetOp->time().minute(), _ui->dateFinaleMetOp->time().second(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            qSwap(conditions.jj1, conditions.jj2);
        }

        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = settings.value("affichage/systemeHoraire").toBool();

        // Lieu d'observation
        conditions.observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        // Unites de longueur
        conditions.unite = (settings.value("affichage/unite").toBool()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Magnitude maximale
        conditions.magnitudeLimite = _ui->magnitudeMaxMetOp->value();

        // Angle limite
        conditions.angleLimite = MATHS::PI;

        // Hauteur minimale du satellite
        conditions.hauteur = MATHS::DEG2RAD * ((_ui->hauteurSatMetOp->currentIndex() == 5) ?
                                                   abs(_ui->valHauteurSatMetOp->text().toInt()) : 5 * _ui->hauteurSatMetOp->currentIndex());

        // Hauteur maximale du Soleil
        if (_ui->hauteurSoleilMetOp->currentIndex() <= 3) {
            conditions.crepuscule = -6. * MATHS::DEG2RAD * _ui->hauteurSoleilMetOp->currentIndex();
        } else if (_ui->hauteurSoleilMetOp->currentIndex() == 4) {
            conditions.crepuscule = MATHS::PI_SUR_DEUX;
        } else if (_ui->hauteurSoleilMetOp->currentIndex() == 5) {
            conditions.crepuscule = MATHS::DEG2RAD * _ui->valHauteurSoleilMetOp->text().toInt();
        }

        // Tri par ordre chronologique
        conditions.chrono = _ui->ordreChronologiqueMetOp->isChecked();

        // Prise en compte de l'extinction atmospherique
        conditions.extinction = settings.value("affichage/extinctionAtmospherique").toBool();

        // Prise en compte de la refraction atmospherique
        conditions.refraction = settings.value("affichage/refractionAtmospherique").toBool();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = settings.value("affichage/effetEclipsesMagnitude").toBool();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = settings.value("affichage/eclipsesLune").toBool();

        // Liste des satellites pouvant produire des flashs
        QStringList listeSatellites = Configuration::instance()->mapFlashs().keys();

        // Lecture du fichier d'elements orbitaux
        QMap<QString, ElementsOrbitaux> tabElem = GPFormat::LectureFichier(fichier, Configuration::instance()->donneesSatellites(),
                                                                           Configuration::instance()->lgRec(), listeSatellites);

        // Mise a jour de la liste de satellites et creation du tableau de satellites
        QMutableStringListIterator it(listeSatellites);
        while (it.hasNext()) {
            const QString norad = it.next();
            if (!tabElem.contains(norad)) {
                it.remove();
            }
        }

        // Il n'y a aucun satellite produisant des flashs dans le fichier d'elements orbitaux
        if (listeSatellites.size() == 0) {
            qWarning() << "Aucun satellite produisant des flashs n'a été trouvé dans le fichier d'éléments orbitaux";
            throw PreviSatException(tr("Aucun satellite produisant des flashs n'a été trouvé dans le fichier d'éléments orbitaux"), MessageType::WARNING);
        }

        conditions.tabElem = tabElem;
        conditions.listeSatellites = listeSatellites;

        // Ecriture des informations de prévisions dans le fichier de log
        qInfo() << "--";
        qInfo() << "Calcul des flashs :";
        qInfo() << "Date de début =" << date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date1 =" << offset1;

        qInfo() << "Date de fin =" << date2.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date2 =" << offset2;

        qInfo() << QString("Lieu d'observation : %1 %2 %3")
                   .arg(conditions.observateur.longitude() * MATHS::RAD2DEG, 0, 'f', 9)
                   .arg(conditions.observateur.latitude() * MATHS::RAD2DEG, 0, 'f', 9)
                   .arg(conditions.observateur.altitude() * 1.e3);

        qInfo() << "Unité de longueur =" << conditions.unite;
        qInfo() << "Magnitude limite =" << conditions.magnitudeLimite;
        qInfo() << "Angle limite =" << conditions.angleLimite;
        qInfo() << "Hauteur minimale du satellite =" << conditions.hauteur;
        qInfo() << "Hauteur maximale du Soleil = " << conditions.crepuscule;
        qInfo() << "Prise en compte de l'extinction atmospherique =" << conditions.extinction;
        qInfo() << "Prise en compte de la refraction atmospherique =" << conditions.refraction;
        qInfo() << "Prise en compte de l'effet des eclipses partielles sur la magnitude =" << conditions.effetEclipsePartielle;
        qInfo() << "Prise en compte des eclipses de Lune =" << conditions.calcEclipseLune;
        qInfo() << "Liste de numéros NORAD =" << conditions.listeSatellites;

        qInfo() << "--";

        // Nom du fichier resultat
        const QString chaine = tr("flashs", "file name (without accent)") + "_%1_%2.txt";
        conditions.ficRes = Configuration::instance()->dirTmp() + QDir::separator() +
                chaine.arg(date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).remove("/").split(" ").first()).
                arg(date2.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).remove("/").split(" ").first());

        // Barre de progression
        auto barreProgression = new QProgressBar();
        barreProgression->setAlignment(Qt::AlignHCenter);

        QProgressDialog fenetreProgression;
        fenetreProgression.setWindowTitle(tr("Calculs en cours..."));
        fenetreProgression.setCancelButtonText(tr("Annuler"));
        fenetreProgression.setBar(barreProgression);
        fenetreProgression.setWindowFlags(fenetreProgression.windowFlags() & ~Qt::WindowContextHelpButtonHint);

        // Lancement des calculs
        Flashs::setConditions(conditions);
        QFutureWatcher<void> calculs;

        connect(&fenetreProgression, SIGNAL(canceled()), &calculs, SLOT(cancel()));
        connect(&calculs, SIGNAL(finished()), &fenetreProgression, SLOT(reset()));
        connect(&calculs, SIGNAL(progressRangeChanged(int, int)), &fenetreProgression, SLOT(setRange(int,int)));
        connect(&calculs, SIGNAL(progressValueChanged(int)), &fenetreProgression, SLOT(setValue(int)));

        calculs.setFuture(QtConcurrent::map(vecSat, &Flashs::CalculFlashs));

        fenetreProgression.exec();
        calculs.waitForFinished();

        if (calculs.isCanceled()) {
            Flashs::resultats().clear();
        } else {

            // Affichage des resultats
            emit AfficherMessageStatut(tr("Calculs terminés"), 10);

            if (Flashs::resultats().isEmpty()) {
                qInfo() << "Aucun flash n'a été trouvé sur la période donnée";
                Message::Afficher(tr("Aucun flash n'a été trouvé sur la période donnée"), MessageType::INFO);
            } else {
                EFFACE_OBJET(_afficherResultats);
                _afficherResultats = new AfficherResultats(TypeCalcul::FLASHS, conditions, Flashs::donnees(), Flashs::resultats(),
                                                           settings.value("affichage/valeurZoomMap").toInt());
                _afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void CalculsFlashs::on_parametrageDefautMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatMetOp->setCurrentIndex(2);
    _ui->hauteurSoleilMetOp->setCurrentIndex(1);
    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->ordreChronologiqueMetOp->setChecked(true);
    _ui->magnitudeMaxMetOp->setValue(4.);
    if (!_ui->calculsFlashs->isEnabled()) {
        _ui->calculsFlashs->setEnabled(true);
    }

    /* Retour */
    return;
}

void CalculsFlashs::on_effacerHeuresMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeMetOp->setTime(QTime(0, 0, 0));
    _ui->dateFinaleMetOp->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}


void CalculsFlashs::on_hauteurSatMetOp_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSatMetOp->count() - 1) {
        _ui->valHauteurSatMetOp->setText(settings.value("previsions/valHauteurSatMetOp", 0).toString());
        _ui->valHauteurSatMetOp->setVisible(true);
        _ui->valHauteurSatMetOp->setCursorPosition(0);
        _ui->valHauteurSatMetOp->setFocus();
    } else {
        _ui->valHauteurSatMetOp->setVisible(false);
    }

    /* Retour */
    return;
}


void CalculsFlashs::on_hauteurSoleilMetOp_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSoleilMetOp->count() - 1) {
        _ui->valHauteurSoleilMetOp->setText(settings.value("previsions/valHauteurSoleilMetOp", 0).toString());
        _ui->valHauteurSoleilMetOp->setVisible(true);
        _ui->valHauteurSoleilMetOp->setCursorPosition(0);
        _ui->valHauteurSoleilMetOp->setFocus();
    } else {
        _ui->valHauteurSoleilMetOp->setVisible(false);
    }

    /* Retour */
    return;
}

void CalculsFlashs::on_majElementsOrbitaux_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        qInfo() << "Mise a jour du fichier flares-spctrk.xml";

        const QFileInfo ff("flares-spctrk.xml");

        Telechargement tel(Configuration::instance()->dirElem());
        emit AfficherMessageStatut(tr("Mise à jour du fichier GP %1 en cours...").arg(ff.fileName()), 0);

        const QString ficMaj = QString("%1elem/%2").arg(DOMAIN_NAME).arg(ff.fileName());

        tel.TelechargementFichier(QUrl(ficMaj));
        AfficherMessageStatut(tr("Téléchargement terminé"), 10);

        if (Configuration::instance()->nomfic() == ff.fileName()) {
            emit ChargementGP();
            emit DemarrageApplication();
        }

        CalculAgeElementsOrbitaux();

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}
