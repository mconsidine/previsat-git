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
 * >    calculstransits.cpp
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
 * >    7 juillet 2024
 *
 */

#include <QFileInfo>
#include <QFutureWatcher>
#include <QMenu>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "ui_calculstransits.h"
#include "calculstransits.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "interface/listwidgetitem.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"
#include "previsions/transits.h"


// Registre
#if (PORTABLE_BUILD == true)
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
CalculsTransits::CalculsTransits(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsTransits)
{
    _ui->setupUi(this);

    try {

        _afficherResultats = nullptr;
        _aucun = nullptr;
        _tous = nullptr;

        Initialisation();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
}


/*
 * Destructeur
 */
CalculsTransits::~CalculsTransits()
{
    settings.setValue("previsions/hauteurSatTransit", _ui->hauteurSatTransit->currentIndex());
    settings.setValue("previsions/valHauteurSatTransit", _ui->valHauteurSatTransit->text().toInt());
    settings.setValue("previsions/lieuxObservationTransit", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/elongationMaxCorps", _ui->elongationMaxCorps->value());
    settings.setValue("previsions/calcTransitLunaireJour", _ui->calcTransitLunaireJour->isChecked());

    EFFACE_OBJET(_afficherResultats);
    EFFACE_OBJET(_aucun);
    EFFACE_OBJET(_tous);

    delete _ui;
}


/*
 * Accesseurs
 */


/*
 * Modificateurs
 */


/*
 * Methodes publiques
 */
/*
 * Affichage des lieux d'observation dans la liste deroulante
 */
void CalculsTransits::AffichageLieuObs()
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

/*
 * Affichage des satellites dans la liste
 */
void CalculsTransits::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                              const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listeTransits);
    elem->setData(Qt::UserRole, norad);
    elem->setData(Qt::CheckStateRole, (check) ? Qt::Checked : Qt::Unchecked);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

    if (norad == noradDefaut) {
        _ui->listeTransits->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void CalculsTransits::InitAffichageListeSatellites()
{
    _ui->listeTransits->clear();
    _ui->listeTransits->scrollToTop();
    _ui->satellitesChoisis->setChecked(false);
}

/*
 * Tri dans l'affichage des satellites
 */
void CalculsTransits::TriAffichageListeSatellites()
{
    _ui->listeTransits->sortItems();
    _ui->listeTransits->scrollToItem(_ui->listeTransits->currentItem(), QAbstractItemView::PositionAtTop);
}

void CalculsTransits::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

/*
 * Affichage des informations de l'onglet
 */
void CalculsTransits::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeTransit->setDateTime(date.ToQDateTime(DateFormatSec::FORMAT_SEC_ZERO));
    _ui->dateFinaleTransit->setDateTime(_ui->dateInitialeTransit->dateTime().addDays(7));

    CalculAgeElementsOrbitaux();

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
 * Calcul de l'age des elements orbitaux
 */
void CalculsTransits::CalculAgeElementsOrbitaux()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->mapElementsOrbitaux().isEmpty()) {

        _ui->gridLayoutWidget->setVisible(false);

    } else {

        QPalette pal = _paletteBouton;
        _ui->majElementsOrbitauxIss->setToolTip("");
        double elemMin = -DATE::DATE_INFINIE;
        double elemMax = DATE::DATE_INFINIE;

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeTransit->dateTime());
        const double offset2 = Date::CalculOffsetUTC(_ui->dateFinaleTransit->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeTransit->dateTime(), 0.);

        // Jour julien initial
        const double jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinaleTransit->dateTime(), 0.);

        // Jour julien final
        const double jj2 = date2.jourJulien() - offset2;

        // Determination de l'age maximal des elements orbitaux
        QMapIterator it(Configuration::instance()->mapElementsOrbitaux());
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

        _ui->ageElementsOrbitauxTransit1->setPalette(palette[0]);
        _ui->ageElementsOrbitauxTransit1->setText(QString("%1").arg(age[0], 0, 'f', 2));

        _ui->ageElementsOrbitauxTransit2->setPalette(palette[1]);
        _ui->ageElementsOrbitauxTransit2->setText(QString("%1").arg(age[1], 0, 'f', 2));

        // Lecture du fichier d'elements orbitaux de l'ISS
        const QString fichier = Configuration::instance()->dirElem() + QDir::separator() + "iss.gp";
        _listeElemIss = GPFormat::LectureListeGP(fichier, Configuration::instance()->dbSatellites());

        if (!_listeElemIss.isEmpty()) {

            // Dates des premier et dernier elements orbitaux
            const double datePremierElem = _listeElemIss.first().epoque.jourJulienUTC();
            const double dateDernierElem = _listeElemIss.last().epoque.jourJulienUTC();

            double age1 = 0.;
            if (jj1 < datePremierElem) {
                age1 = datePremierElem - jj1;
            }

            if (jj1 > dateDernierElem) {
                age1 = jj1 - dateDernierElem;
            }

            double age2 = 0.;
            if (jj2 < datePremierElem) {
                age2 = datePremierElem - jj2;
            }

            if (jj2 > dateDernierElem) {
                age2 = jj2 - dateDernierElem;
            }

            if ((age1 > (2.05)) || (age2 > (2.05))) {

                // Affichage du contour du bouton en rouge
                pal.setColor(QPalette::Button, QColor(Qt::red));
                _ui->majElementsOrbitauxIss->setToolTip(tr("Les éléments orbitaux de l'ISS sont vieux, il est conseillé de les mettre à jour"));
            }

            _ui->majElementsOrbitauxIss->setPalette(pal);
            _ui->majElementsOrbitauxIss->update();
            _ui->gridLayoutWidget->setVisible(true);
        }
    }

    /* Retour */
    return;
}

/*
 * Initialisation de la classe CalculsTransits
 */
void CalculsTransits::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->valHauteurSatTransit->setVisible(false);
    _ui->hauteurSatTransit->setCurrentIndex(settings.value("previsions/hauteurSatTransit", 1).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationTransit", 0).toInt());
    _ui->elongationMaxCorps->setValue(settings.value("previsions/elongationMaxCorps", 5.).toDouble());

    QAction const * effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &CalculsTransits::on_filtreSatellites_returnPressed);
    }

    _paletteBouton = _ui->majElementsOrbitauxIss->palette();
    _ui->majElementsOrbitauxIss->setAutoFillBackground(true);

    _aucun = new QAction(tr("Aucun"), this);
    connect(_aucun, &QAction::triggered, this, &CalculsTransits::Aucun);

    _tous = new QAction(tr("Tous"), this);
    connect(_tous, &QAction::triggered, this, &CalculsTransits::Tous);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsTransits::Aucun()
{
    for(int i=0; i<_ui->listeTransits->count(); i++) {
        _ui->listeTransits->item(i)->setCheckState(Qt::Unchecked);
    }
}

void CalculsTransits::Tous()
{
    for(int i=0; i<_ui->listeTransits->count(); i++) {
        _ui->listeTransits->item(i)->setCheckState(Qt::Checked);
    }
}

void CalculsTransits::on_calculsTransit_clicked()
{
    /* Declarations des variables locales */
    QVector<int> vecSat;
    ConditionsPrevisions conditions {};

    /* Initialisations */
    vecSat.append(0);
    conditions.listeSatellites.clear();
    conditions.listeElemIss.clear();

    /* Corps de la methode */
    try {

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeTransit->dateTime());
        const double offset2 = Date::CalculOffsetUTC(_ui->dateFinaleTransit->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeTransit->dateTime(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinaleTransit->dateTime(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            std::swap(conditions.jj1, conditions.jj2);
        }

        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = settings.value("affichage/systemeHoraire").toBool();

        // Hauteur minimale du satellite
        conditions.hauteur = MATHS::DEG2RAD * ((_ui->hauteurSatTransit->currentIndex() == 5) ?
                                                   abs(_ui->valHauteurSatTransit->text().toInt()) : 5 * _ui->hauteurSatTransit->currentIndex());

        // Elongation maximale
        conditions.seuilConjonction = MATHS::DEG2RAD * _ui->elongationMaxCorps->value();

        // Selection des corps
        conditions.calcEphemSoleil = _ui->soleilTransit->isChecked();
        conditions.calcEphemLune = _ui->luneTransit->isChecked();

        // Lieu d'observation
        conditions.observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        // Unites de longueur
        conditions.unite = (settings.value("affichage/unite").toBool()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Prise en compte de la refraction atmospherique
        conditions.refraction = settings.value("affichage/refractionAtmospherique").toBool();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = settings.value("affichage/effetEclipsesMagnitude").toBool();

        // Calcul des transits lunaires de jour
        conditions.calcTransitLunaireJour = _ui->calcTransitLunaireJour->isChecked();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = settings.value("affichage/eclipsesLune").toBool();

        // Elements orbitaux
        conditions.tabElem.clear();
        for(int i=0; i<_ui->listeTransits->count(); i++) {
            if (_ui->listeTransits->item(i)->checkState() == Qt::Checked) {
                const QString norad = _ui->listeTransits->item(i)->data(Qt::UserRole).toString();
                conditions.tabElem.insert(norad, Configuration::instance()->mapElementsOrbitaux()[norad]);
            }
        }

        if (!_listeElemIss.isEmpty()) {
            conditions.listeElemIss = _listeElemIss;
        }

        // Ecriture des informations de prévisions dans le fichier de log
        qInfo() << "--";
        qInfo() << "Calcul des transits :";
        qInfo() << "Date de début =" << date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date1 =" << offset1;

        qInfo() << "Date de fin =" << date2.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date2 =" << offset2;
        qInfo() << "Pas de génération =" << conditions.pas;

        qInfo() << QString("Lieu d'observation : %1 %2 %3")
                   .arg(conditions.observateur.longitude() * MATHS::RAD2DEG, 0, 'f', 9)
                   .arg(conditions.observateur.latitude() * MATHS::RAD2DEG, 0, 'f', 9)
                   .arg(conditions.observateur.altitude() * 1.e3);

        qInfo() << "Unité de longueur =" << conditions.unite;
        qInfo() << "Hauteur minimale du satellite =" << conditions.hauteur;
        qInfo() << "Elongation maximale = " << conditions.seuilConjonction;
        qInfo() << "Calcul pour le Soleil =" << conditions.calcEphemSoleil;
        qInfo() << "Calcul pour la Lune =" << conditions.calcEphemLune;
        qInfo() << "Prise en compte de la refraction atmospherique =" << conditions.refraction;
        qInfo() << "Prise en compte de l'effet des eclipses partielles sur la magnitude =" << conditions.effetEclipsePartielle;
        qInfo() << "Calcul des transits lunaires de jour = " << conditions.calcTransitLunaireJour;
        qInfo() << "Prise en compte des eclipses de Lune =" << conditions.calcEclipseLune;
        qInfo() << "Liste de numéros NORAD =" << conditions.tabElem.keys();

        qInfo() << "--";

        // Nom du fichier resultat
        const QString chaine = tr("transits", "file name (without accent)") + "_%1_%2.txt";
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
        Transits::setConditions(conditions);
        QFutureWatcher<void> calculs;

        connect(&fenetreProgression, SIGNAL(canceled()), &calculs, SLOT(cancel()));
        connect(&calculs, SIGNAL(finished()), &fenetreProgression, SLOT(reset()));
        connect(&calculs, SIGNAL(progressRangeChanged(int, int)), &fenetreProgression, SLOT(setRange(int,int)));
        connect(&calculs, SIGNAL(progressValueChanged(int)), &fenetreProgression, SLOT(setValue(int)));

        calculs.setFuture(QtConcurrent::map(vecSat, &Transits::CalculTransits));

        fenetreProgression.exec();
        calculs.waitForFinished();

        if (calculs.isCanceled()) {
            Transits::resultats().clear();
        } else {

            // Affichage des resultats
            emit AfficherMessageStatut(tr("Calculs terminés"), 10);

            if (Transits::resultats().isEmpty()) {
                Message::Afficher(tr("Aucun transit ISS n'a été trouvé sur la période donnée"), MessageType::INFO);
            } else {
                EFFACE_OBJET(_afficherResultats);
                _afficherResultats = new AfficherResultats(TypeCalcul::TRANSITS, conditions, Transits::donnees(), Transits::resultats(),
                                                           settings.value("affichage/valeurZoomMap", 9).toInt());
                _afficherResultats->show();
            }
        }

    } catch (Exception const &) {
    }

    /* Retour */
    return;
}

void CalculsTransits::on_filtreSatellites_textChanged(const QString &arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!arg1.isEmpty()) {
        const bool etat = _ui->satellitesChoisis->blockSignals(true);
        _ui->satellitesChoisis->setChecked(false);
        _ui->satellitesChoisis->blockSignals(etat);
    }

    for(int i=0; i<_ui->listeTransits->count(); i++) {
        const QString elem = _ui->listeTransits->item(i)->text();
        _ui->listeTransits->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }

    /* Retour */
    return;
}

void CalculsTransits::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsTransits::on_satellitesChoisis_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {
        _ui->filtreSatellites->clear();
        for(int i=0; i<_ui->listeTransits->count(); i++) {
            const bool chk = !(_ui->listeTransits->item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked);
            _ui->listeTransits->item(i)->setHidden(chk);
        }
    } else {
        on_filtreSatellites_textChanged("");
        on_filtreSatellites_returnPressed();
    }

    /* Retour */
    return;
}

void CalculsTransits::on_parametrageDefautTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatTransit->setCurrentIndex(1);
    _ui->valHauteurSatTransit->setVisible(false);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->elongationMaxCorps->setValue(5.);
    if (!_ui->calculsTransit->isEnabled()) {
        _ui->calculsTransit->setEnabled(true);
    }

    /* Retour */
    return;
}

void CalculsTransits::on_effacerHeuresTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeTransit->setTime(QTime(0, 0, 0));
    _ui->dateFinaleTransit->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void CalculsTransits::on_hauteurSatTransit_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSatTransit->count() - 1) {
        _ui->valHauteurSatTransit->setText(settings.value("previsions/valHauteurSatTransit", 0).toString());
        _ui->valHauteurSatTransit->setVisible(true);
        _ui->valHauteurSatTransit->setCursorPosition(0);
        _ui->valHauteurSatTransit->setFocus();
    } else {
        _ui->valHauteurSatTransit->setVisible(false);
    }

    /* Retour */
    return;
}

void CalculsTransits::on_majElementsOrbitauxIss_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Mise a jour du fichier d'elements orbitaux
        emit AfficherMessageStatut(tr("Téléchargement du fichier d'élements orbitaux de l'ISS..."), -1);
        const QString ficElem = Configuration::instance()->adresseCelestrakSupplementalNoradFichier().arg("iss");
        Telechargement tel(Configuration::instance()->dirTmp());
        tel.TelechargementFichier(ficElem);
        emit AfficherMessageStatut(tr("Téléchargement terminé"), 5);

        const QString fic = Configuration::instance()->dirTmp() + QDir::separator() + "iss.xml";
        const QString ficGp = Configuration::instance()->dirElem() + QDir::separator() + "iss.gp";

        QFile fi(fic);
        QFile fi2(ficGp);
        if (fi2.exists()) {
            fi2.remove();
        }
        fi.rename(ficGp);

        // Mise a jour de l'age des elements orbitaux
        CalculAgeElementsOrbitaux();

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

void CalculsTransits::on_listeTransits_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    if (_ui->listeTransits->currentRow() >= 0) {
        QMenu menu(this);
        menu.addAction(_tous);
        menu.addAction(_aucun);
        menu.exec(QCursor::pos());
    }

    /* Retour */
    return;
}

void CalculsTransits::on_majElementsOrbitaux_clicked()
{
    emit MajFichierGP();
    CalculAgeElementsOrbitaux();
}
