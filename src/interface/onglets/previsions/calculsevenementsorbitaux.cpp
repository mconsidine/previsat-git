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
 * >    calculsevenementsorbitaux.cpp
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

#include <QDir>
#include <QFutureWatcher>
#include <QMenu>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "ui_calculsevenementsorbitaux.h"
#include "calculsevenementsorbitaux.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "interface/listwidgetitem.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/exception.h"
#include "previsions/evenementsorbitaux.h"


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
CalculsEvenementsOrbitaux::CalculsEvenementsOrbitaux(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsEvenementsOrbitaux)
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
CalculsEvenementsOrbitaux::~CalculsEvenementsOrbitaux()
{
    settings.setValue("previsions/passageApogee", _ui->passageApogee->isChecked());
    settings.setValue("previsions/passageNoeuds", _ui->passageNoeuds->isChecked());
    settings.setValue("previsions/passageOmbre", _ui->passageOmbre->isChecked());
    settings.setValue("previsions/passageQuadrangles", _ui->passageQuadrangles->isChecked());
    settings.setValue("previsions/transitionJourNuit", _ui->transitionJourNuit->isChecked());

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
 * Affichage des informations de l'onglet
 */
void CalculsEvenementsOrbitaux::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeEvt->setDateTime(date.ToQDateTime(DateFormatSec::FORMAT_SEC_ZERO));
    _ui->dateFinaleEvt->setDateTime(_ui->dateInitialeEvt->dateTime().addDays(7));

    CalculAgeElementsOrbitaux();

    /* Retour */
    return;
}

/*
 * Affichage des satellites dans la liste
 */
void CalculsEvenementsOrbitaux::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                                        const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listeEvenements);
    elem->setData(Qt::UserRole, norad);
    elem->setData(Qt::CheckStateRole, (check) ? Qt::Checked : Qt::Unchecked);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

    if (norad == noradDefaut) {
        _ui->listeEvenements->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void CalculsEvenementsOrbitaux::InitAffichageListeSatellites()
{
    _ui->listeEvenements->clear();
    _ui->listeEvenements->scrollToTop();
    _ui->satellitesChoisis->setChecked(false);
}

/*
 * Tri dans l'affichage des satellites
 */
void CalculsEvenementsOrbitaux::TriAffichageListeSatellites()
{
    _ui->listeEvenements->sortItems();
    _ui->listeEvenements->scrollToItem(_ui->listeEvenements->currentItem(), QAbstractItemView::PositionAtTop);
}

void CalculsEvenementsOrbitaux::changeEvent(QEvent *evt)
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
void CalculsEvenementsOrbitaux::CalculAgeElementsOrbitaux()
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
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeEvt->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeEvt->dateTime(), 0.);

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
 * Initialisation de la classe CalculsEvenementsOrbitaux
 */
void CalculsEvenementsOrbitaux::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->passageApogee->setChecked(settings.value("previsions/passageApogee", true).toBool());
    _ui->passageNoeuds->setChecked(settings.value("previsions/passageNoeuds", true).toBool());
    _ui->passageOmbre->setChecked(settings.value("previsions/passageOmbre", true).toBool());
    _ui->passageQuadrangles->setChecked(settings.value("previsions/passageQuadrangles", true).toBool());
    _ui->transitionJourNuit->setChecked(settings.value("previsions/transitionJourNuit", true).toBool());

    QAction const * effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &CalculsEvenementsOrbitaux::on_filtreSatellites_returnPressed);
    }

    _aucun = new QAction(tr("Aucun"), this);
    connect(_aucun, &QAction::triggered, this, &CalculsEvenementsOrbitaux::Aucun);

    _tous = new QAction(tr("Tous"), this);
    connect(_tous, &QAction::triggered, this, &CalculsEvenementsOrbitaux::Tous);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::Aucun()
{
    for(int i=0; i<_ui->listeEvenements->count(); i++) {
        _ui->listeEvenements->item(i)->setCheckState(Qt::Unchecked);
    }
}

void CalculsEvenementsOrbitaux::Tous()
{
    for(int i=0; i<_ui->listeEvenements->count(); i++) {
        _ui->listeEvenements->item(i)->setCheckState(Qt::Checked);
    }
}

void CalculsEvenementsOrbitaux::on_calculsEvt_clicked()
{
    /* Declarations des variables locales */
    ConditionsPrevisions conditions;

    /* Initialisations */
    conditions.listeSatellites.clear();

    /* Corps de la methode */
    try {

        if (_ui->listeEvenements->count() == 0) {
            throw Exception();
        }

        QVector<int> vecSat;
        vecSat.append(0);
        int j = 0;

        for(int i = 0; i < _ui->listeEvenements->count(); i++) {
            if (_ui->listeEvenements->item(i)->checkState() == Qt::Checked) {
                conditions.listeSatellites.append(_ui->listeEvenements->item(i)->data(Qt::UserRole).toString());
                //vecSat.append(j);
                j++;
            }
        }

        if (conditions.listeSatellites.isEmpty()) {
            throw Exception(tr("Aucun satellite n'est sélectionné dans la liste"), MessageType::WARNING);
        }

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeEvt->dateTime());
        const double offset2 = Date::CalculOffsetUTC(_ui->dateFinaleEvt->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeEvt->dateTime(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinaleEvt->dateTime(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            std::swap(conditions.jj1, conditions.jj2);
        }

        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = settings.value("affichage/systemeHoraire").toBool();

        // Unites de longueur
        conditions.unite = (settings.value("affichage/unite").toBool()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = settings.value("affichage/eclipsesLune").toBool();

        // Elements orbitaux
        conditions.tabElem.clear();
        for(int i=0; i<_ui->listeEvenements->count(); i++) {
            if (_ui->listeEvenements->item(i)->checkState() == Qt::Checked) {
                const QString norad = _ui->listeEvenements->item(i)->data(Qt::UserRole).toString();
                conditions.tabElem.insert(norad, Configuration::instance()->mapElementsOrbitaux()[norad]);
            }
        }

        // Types d'evenements
        conditions.passageNoeuds = _ui->passageNoeuds->isChecked();
        conditions.passageOmbre = _ui->passageOmbre->isChecked();
        conditions.passagePso = _ui->passageQuadrangles->isChecked();
        conditions.passageApogee = _ui->passageApogee->isChecked();
        conditions.transitionsJourNuit = _ui->transitionJourNuit->isChecked();

        qInfo() << "--";
        qInfo() << "Calcul des évènements orbitaux :";
        qInfo() << "Date de début =" << date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date1 =" << offset1;

        qInfo() << "Date de fin =" << date2.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date2 =" << offset2;

        qInfo() << "Unité de longueur =" << conditions.unite;
        qInfo() << "Prise en compte des eclipses de Lune =" << conditions.calcEclipseLune;
        qInfo() << "Liste de numéros NORAD =" << conditions.tabElem.keys();
        qInfo() << "--";

        // Nom du fichier resultat
        const QString chaine = tr("evenements", "file name (without accent)") + "_%1_%2.txt";
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
        EvenementsOrbitaux::setConditions(conditions);
        QFutureWatcher<void> calculs;

        connect(&fenetreProgression, SIGNAL(canceled()), &calculs, SLOT(cancel()));
        connect(&calculs, SIGNAL(finished()), &fenetreProgression, SLOT(reset()));
        connect(&calculs, SIGNAL(progressRangeChanged(int, int)), &fenetreProgression, SLOT(setRange(int,int)));
        connect(&calculs, SIGNAL(progressValueChanged(int)), &fenetreProgression, SLOT(setValue(int)));

        calculs.setFuture(QtConcurrent::map(vecSat, &EvenementsOrbitaux::CalculEvenements));

        fenetreProgression.exec();
        calculs.waitForFinished();

        if (calculs.isCanceled()) {
            EvenementsOrbitaux::resultats().clear();
        } else {

            // Affichage des resultats
            emit AfficherMessageStatut(tr("Calculs terminés"), 10);

            if (EvenementsOrbitaux::resultats().isEmpty()) {
                Message::Afficher(tr("Aucun évènement n'a été trouvé sur la période donnée"), MessageType::INFO);
            } else {
                EFFACE_OBJET(_afficherResultats);
                _afficherResultats = new AfficherResultats(TypeCalcul::EVENEMENTS, conditions, EvenementsOrbitaux::donnees(),
                                                           EvenementsOrbitaux::resultats());
                _afficherResultats->show();
            }
        }

    } catch (Exception const &) {
    }

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::on_filtreSatellites_textChanged(const QString &arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!arg1.isEmpty()) {
        const bool etat = _ui->satellitesChoisis->blockSignals(true);
        _ui->satellitesChoisis->setChecked(false);
        _ui->satellitesChoisis->blockSignals(etat);
    }

    for(int i=0; i<_ui->listeEvenements->count(); i++) {
        const QString elem = _ui->listeEvenements->item(i)->text();
        _ui->listeEvenements->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsEvenementsOrbitaux::on_satellitesChoisis_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {
        _ui->filtreSatellites->clear();
        for(int i=0; i<_ui->listeEvenements->count(); i++) {
            const bool chk = !(_ui->listeEvenements->item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked);
            _ui->listeEvenements->item(i)->setHidden(chk);
        }
    } else {
        on_filtreSatellites_textChanged("");
        on_filtreSatellites_returnPressed();
    }

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::on_parametrageDefautEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->passageApogee->setChecked(true);
    _ui->passageNoeuds->setChecked(true);
    _ui->passageOmbre->setChecked(true);
    _ui->passageQuadrangles->setChecked(true);
    _ui->transitionJourNuit->setChecked(true);
    if (!_ui->calculsEvt->isEnabled()) {
        _ui->calculsEvt->setEnabled(true);
    }

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::on_effacerHeuresEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeEvt->setTime(QTime(0, 0, 0));
    _ui->dateFinaleEvt->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::on_listeEvenements_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    if (_ui->listeEvenements->currentRow() >= 0) {
        QMenu menu(this);
        menu.addAction(_tous);
        menu.addAction(_aucun);
        menu.exec(QCursor::pos());
    }

    /* Retour */
    return;
}

void CalculsEvenementsOrbitaux::on_majElementsOrbitaux_clicked()
{
    emit MajFichierGP();
    CalculAgeElementsOrbitaux();
}
