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
 * >    calculsprevisions.cpp
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
 * >    20 novembre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wshadow"
#include <QDir>
#include <QFutureWatcher>
#include <QMenu>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "ui_calculsprevisions.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "calculsprevisions.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "interface/listwidgetitem.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "previsions/prevision.h"


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
CalculsPrevisions::CalculsPrevisions(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsPrevisions)
{
    _ui->setupUi(this);

    try {

        _afficherResultats = nullptr;
        _aucun = nullptr;
        _tous = nullptr;

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
CalculsPrevisions::~CalculsPrevisions()
{
    settings.setValue("previsions/pasGeneration", _ui->pasGeneration->currentIndex());
    settings.setValue("previsions/lieuxObservationPrev", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/hauteurSatPrev", _ui->hauteurSatPrev->currentIndex());
    settings.setValue("previsions/valHauteurSatPrev", _ui->valHauteurSatPrev->text().toInt());
    settings.setValue("previsions/hauteurSoleilPrev", _ui->hauteurSoleilPrev->currentIndex());
    settings.setValue("previsions/valHauteurSoleilPrev", _ui->valHauteurSoleilPrev->text().toInt());
    settings.setValue("previsions/illuminationPrev", _ui->illuminationPrev->isChecked());
    settings.setValue("previsions/magnitudeMaxPrev", _ui->magnitudeMaxPrev->isChecked());

    EFFACE_OBJET(_afficherResultats);
    EFFACE_OBJET(_aucun);
    EFFACE_OBJET(_tous);

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::CalculsPrevisions *CalculsPrevisions::ui() const
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
void CalculsPrevisions::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialePrev->setDateTime(date.ToQDateTime(0));
    _ui->dateFinalePrev->setDateTime(_ui->dateInitialePrev->dateTime().addDays(7));

    CalculAgeElementsOrbitaux();

    /* Retour */
    return;
}

/*
 * Affichage des lieux d'observation dans la liste deroulante
 */
void CalculsPrevisions::AffichageLieuObs()
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
void CalculsPrevisions::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                                const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listePrevisions);
    elem->setData(Qt::UserRole, norad);
    elem->setData(Qt::CheckStateRole, (check) ? Qt::Checked : Qt::Unchecked);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

    if (norad == noradDefaut) {
        _ui->listePrevisions->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void CalculsPrevisions::InitAffichageListeSatellites()
{
    _ui->listePrevisions->clear();
    _ui->listePrevisions->scrollToTop();
    _ui->satellitesChoisis->setChecked(false);
}

/*
 * Tri dans l'affichage des satellites
 */
void CalculsPrevisions::TriAffichageListeSatellites()
{
    _ui->listePrevisions->sortItems();
    _ui->listePrevisions->scrollToItem(_ui->listePrevisions->currentItem(), QAbstractItemView::PositionAtTop);
}

void CalculsPrevisions::changeEvent(QEvent *evt)
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
void CalculsPrevisions::CalculAgeElementsOrbitaux()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (Configuration::instance()->mapElementsOrbitaux().isEmpty()) {

        _ui->gridLayoutWidget->setVisible(false);

    } else {

        double elemMin = -DATE::DATE_INFINIE;
        double elemMax = DATE::DATE_INFINIE;

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialePrev->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialePrev->dateTime(), 0.);

        // Jour julien initial
        const double jj1 = date1.jourJulien() - offset1;

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
 * Initialisation de la classe CalculsPrevisions
 */
void CalculsPrevisions::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->pasGeneration->setCurrentIndex(settings.value("previsions/pasGeneration", 5).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationPrev", 0).toInt());
    _ui->valHauteurSatPrev->setVisible(false);
    _ui->hauteurSatPrev->setCurrentIndex(settings.value("previsions/hauteurSatPrev", 0).toInt());
    _ui->valHauteurSoleilPrev->setVisible(false);
    _ui->hauteurSoleilPrev->setCurrentIndex(settings.value("previsions/hauteurSoleilPrev", 1).toInt());
    _ui->illuminationPrev->setChecked(settings.value("previsions/illuminationPrev", true).toBool());
    _ui->magnitudeMaxPrev->setChecked(settings.value("previsions/magnitudeMaxPrev", false).toBool());
    _ui->valMagnitudeMaxPrev->setVisible(_ui->magnitudeMaxPrev->isChecked());

    QAction const * effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &CalculsPrevisions::on_filtreSatellites_returnPressed);
    }

    _aucun = new QAction(tr("Aucun"), this);
    connect(_aucun, &QAction::triggered, this, &CalculsPrevisions::Aucun);

    _tous = new QAction(tr("Tous"), this);
    connect(_tous, &QAction::triggered, this, &CalculsPrevisions::Tous);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsPrevisions::Aucun()
{
    for(int i=0; i<_ui->listePrevisions->count(); i++) {
        _ui->listePrevisions->item(i)->setCheckState(Qt::Unchecked);
    }
}

void CalculsPrevisions::Tous()
{
    for(int i=0; i<_ui->listePrevisions->count(); i++) {
        _ui->listePrevisions->item(i)->setCheckState(Qt::Checked);
    }
}

void CalculsPrevisions::on_calculsPrev_clicked()
{
    /* Declarations des variables locales */
    QVector<int> vecSat;
    ConditionsPrevisions conditions;

    /* Initialisations */
    conditions.listeSatellites.clear();
    vecSat.append(0);

    /* Corps de la methode */
    try {

        qInfo() << "Lancement des calculs de prévisions";

        int j = 0;

        if (_ui->listePrevisions->count() == 0) {
            qCritical() << "Aucun satellite n'est affiché dans la liste";
            throw PreviSatException();
        }

        for(int i = 0; i < _ui->listePrevisions->count(); i++) {
            if (_ui->listePrevisions->item(i)->checkState() == Qt::Checked) {
                conditions.listeSatellites.append(_ui->listePrevisions->item(i)->data(Qt::UserRole).toString());
                j++;
            }
        }

        if (conditions.listeSatellites.isEmpty()) {
            qWarning() << "Aucun satellite n'est sélectionné dans la liste";
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), MessageType::WARNING);
        }

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialePrev->dateTime());
        const double offset2 = Date::CalculOffsetUTC(_ui->dateFinalePrev->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialePrev->dateTime(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinalePrev->dateTime(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            std::swap(conditions.jj1, conditions.jj2);
        }

        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = settings.value("affichage/systemeHoraire").toBool();

        // Pas de generation
        conditions.pas = _ui->pasGeneration->currentText().split(QRegularExpression("[^0-9]+")).first().toDouble();
        if (_ui->pasGeneration->currentIndex() < 5) {
            conditions.pas *= DATE::NB_JOUR_PAR_SEC;
        } else {
            conditions.pas *= DATE::NB_JOUR_PAR_MIN;
        }

        // Lieu d'observation
        conditions.observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        // Unites de longueur
        conditions.unite = (settings.value("affichage/unite").toBool()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Conditions d'eclairement du satellite
        conditions.eclipse = _ui->illuminationPrev->isChecked();

        // Magnitude maximale
        conditions.magnitudeLimite = (_ui->magnitudeMaxPrev->isChecked()) ? _ui->valMagnitudeMaxPrev->value() : 99.;

        // Hauteur minimale du satellite
        conditions.hauteur = MATHS::DEG2RAD * ((_ui->hauteurSatPrev->currentIndex() == 5) ?
                                                   abs(_ui->valHauteurSatPrev->text().toInt()) : 5 * _ui->hauteurSatPrev->currentIndex());

        // Hauteur maximale du Soleil
        if (_ui->hauteurSoleilPrev->currentIndex() <= 3) {
            conditions.crepuscule = -6. * MATHS::DEG2RAD * _ui->hauteurSoleilPrev->currentIndex();
        } else if (_ui->hauteurSoleilPrev->currentIndex() == 4) {
            conditions.crepuscule = MATHS::PI_SUR_DEUX;
        } else if (_ui->hauteurSoleilPrev->currentIndex() == 5) {
            conditions.crepuscule = MATHS::DEG2RAD * _ui->valHauteurSoleilPrev->text().toInt();
        }

        // Prise en compte de l'extinction atmospherique
        conditions.extinction = settings.value("affichage/extinctionAtmospherique").toBool();

        // Prise en compte de la refraction atmospherique
        conditions.refraction = settings.value("affichage/refractionAtmospherique").toBool();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = settings.value("affichage/effetEclipsesMagnitude").toBool();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = settings.value("affichage/eclipsesLune").toBool();

        // Elements orbitaux
        conditions.tabElem.clear();
        for(int i=0; i<_ui->listePrevisions->count(); i++) {
            if (_ui->listePrevisions->item(i)->checkState() == Qt::Checked) {
                const QString norad = _ui->listePrevisions->item(i)->data(Qt::UserRole).toString();
                conditions.tabElem.insert(norad, Configuration::instance()->mapElementsOrbitaux()[norad]);
            }
        }

        // Ecriture des informations de prévisions dans le fichier de log
        qInfo() << "--";
        qInfo() << "Calcul des prévisions de passage :";
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
        qInfo() << "Conditions d'eclairement du satellite =" << conditions.eclipse;
        qInfo() << "Magnitude limite =" << conditions.magnitudeLimite;
        qInfo() << "Hauteur minimale du satellite =" << conditions.hauteur;
        qInfo() << "Hauteur maximale du Soleil = " << conditions.crepuscule;
        qInfo() << "Prise en compte de l'extinction atmospherique =" << conditions.extinction;
        qInfo() << "Prise en compte de la refraction atmospherique =" << conditions.refraction;
        qInfo() << "Prise en compte de l'effet des eclipses partielles sur la magnitude =" << conditions.effetEclipsePartielle;
        qInfo() << "Prise en compte des eclipses de Lune =" << conditions.calcEclipseLune;
        qInfo() << "Liste de numéros NORAD =" << conditions.tabElem.keys();

        qInfo() << "--";

        // Nom du fichier resultat
        const QString chaine = tr("previsions", "filename (without accent)") + "_%1_%2.txt";
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
        Prevision::setConditions(conditions);
        QFutureWatcher<void> calculs;

        connect(&fenetreProgression, SIGNAL(canceled()), &calculs, SLOT(cancel()));
        connect(&calculs, SIGNAL(finished()), &fenetreProgression, SLOT(reset()));
        connect(&calculs, SIGNAL(progressRangeChanged(int, int)), &fenetreProgression, SLOT(setRange(int,int)));
        connect(&calculs, SIGNAL(progressValueChanged(int)), &fenetreProgression, SLOT(setValue(int)));

        calculs.setFuture(QtConcurrent::map(vecSat, &Prevision::CalculPrevisions));

        fenetreProgression.exec();
        calculs.waitForFinished();

        if (calculs.isCanceled()) {
            Prevision::resultats().clear();
        } else {

            // Affichage des resultats
            emit AfficherMessageStatut(tr("Calculs terminés"), 10);

            if (Prevision::resultats().isEmpty()) {
                qInfo() << "Aucun passage n'a été trouvé sur la période donnée";
                Message::Afficher(tr("Aucun passage n'a été trouvé sur la période donnée"), MessageType::INFO);
            } else {
                EFFACE_OBJET(_afficherResultats);
                _afficherResultats = new AfficherResultats(TypeCalcul::PREVISIONS, conditions, Prevision::donnees(), Prevision::resultats());
                _afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void CalculsPrevisions::on_filtreSatellites_textChanged(const QString &arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!arg1.isEmpty()) {
        const bool etat = _ui->satellitesChoisis->blockSignals(true);
        _ui->satellitesChoisis->setChecked(false);
        _ui->satellitesChoisis->blockSignals(etat);
    }

    for(int i=0; i<_ui->listePrevisions->count(); i++) {
        const QString elem = _ui->listePrevisions->item(i)->text();
        _ui->listePrevisions->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }

    /* Retour */
    return;
}

void CalculsPrevisions::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsPrevisions::on_satellitesChoisis_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {
        _ui->filtreSatellites->clear();
        for(int i=0; i<_ui->listePrevisions->count(); i++) {
            const bool chk = !(_ui->listePrevisions->item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked);
            _ui->listePrevisions->item(i)->setHidden(chk);
        }
    } else {
        on_filtreSatellites_textChanged("");
        on_filtreSatellites_returnPressed();
    }
}

void CalculsPrevisions::on_parametrageDefautPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->pasGeneration->setCurrentIndex(5);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->hauteurSatPrev->setCurrentIndex(0);
    _ui->hauteurSoleilPrev->setCurrentIndex(1);
    _ui->valHauteurSatPrev->setVisible(false);
    _ui->valHauteurSoleilPrev->setVisible(false);
    _ui->valMagnitudeMaxPrev->setVisible(false);
    _ui->illuminationPrev->setChecked(true);
    _ui->magnitudeMaxPrev->setChecked(false);
    if (!_ui->calculsPrev->isEnabled()) {
        _ui->calculsPrev->setEnabled(true);
    }

    /* Retour */
    return;
}

void CalculsPrevisions::on_effacerHeuresPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialePrev->setTime(QTime(0, 0, 0));
    _ui->dateFinalePrev->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void CalculsPrevisions::on_hauteurSatPrev_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSatPrev->count() - 1) {
        _ui->valHauteurSatPrev->setText(settings.value("previsions/valHauteurSatPrev", 0).toString());
        _ui->valHauteurSatPrev->setVisible(true);
        _ui->valHauteurSatPrev->setCursorPosition(0);
        _ui->valHauteurSatPrev->setFocus();
    } else {
        _ui->valHauteurSatPrev->setVisible(false);
    }

    /* Retour */
    return;
}

void CalculsPrevisions::on_hauteurSoleilPrev_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSoleilPrev->count() - 1) {
        _ui->valHauteurSoleilPrev->setText(settings.value("previsions/valHauteurSoleilPrev", 0).toString());
        _ui->valHauteurSoleilPrev->setVisible(true);
        _ui->valHauteurSoleilPrev->setCursorPosition(0);
        _ui->valHauteurSoleilPrev->setFocus();
    } else {
        _ui->valHauteurSoleilPrev->setVisible(false);
    }

    /* Retour */
    return;
}

void CalculsPrevisions::on_magnitudeMaxPrev_toggled(bool checked)
{
    _ui->valMagnitudeMaxPrev->setVisible(checked);
}

void CalculsPrevisions::on_listePrevisions_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    if (_ui->listePrevisions->currentRow() >= 0) {
        QMenu menu(this);
        menu.addAction(_tous);
        menu.addAction(_aucun);
        menu.exec(QCursor::pos());
    }

    /* Retour */
    return;
}

void CalculsPrevisions::on_majElementsOrbitaux_clicked()
{
    emit MajFichierGP();
    CalculAgeElementsOrbitaux();
}
