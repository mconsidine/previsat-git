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
 * >    suivitelescope.cpp
 *
 * Localisation
 * >    interface.onglets.telescope
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
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMenu>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "ui_suivitelescope.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "suivitelescope.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "interface/listWidgetItem.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/previsatexception.h"
#include "previsions/telescope.h"


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
SuiviTelescope::SuiviTelescope(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::SuiviTelescope)
{
    _ui->setupUi(this);

    try {

        _afficherResultats = nullptr;
        _date = nullptr;

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
SuiviTelescope::~SuiviTelescope()
{
    EFFACE_OBJET(_afficherResultats);
    EFFACE_OBJET(_date);

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::SuiviTelescope *SuiviTelescope::ui()
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
 * Calcul des informations AOS/LOS pour le suivi d'un satellite
 */
void SuiviTelescope::CalculAosSatSuivi(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    EFFACE_OBJET(_date);
    _date = new Date(date, date.offsetUTC());

    /* Corps de la methode */
    CalculAos();

    /* Retour */
    return;
}

/*
 * Affichage des lieux d'observation dans la liste deroulante
 */
void SuiviTelescope::AffichageLieuObs()
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
void SuiviTelescope::AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip,
                                             const bool check)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ListWidgetItem *elem = new ListWidgetItem(nomsat, _ui->listeTelescope);
    elem->setData(Qt::UserRole, norad);
    elem->setData(Qt::CheckStateRole, (check) ? Qt::Checked : Qt::Unchecked);
    elem->setToolTip(tooltip);
    elem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

    if (norad == noradDefaut) {
        _ui->listeTelescope->setCurrentItem(elem);
    }

    /* Retour */
    return;
}

/*
 * Initialisation de l'affichage de la liste
 */
void SuiviTelescope::InitAffichageListeSatellites()
{
    _ui->listeTelescope->clear();
    _ui->listeTelescope->scrollToTop();
}

/*
 * Tri dans l'affichage des satellites
 */
void SuiviTelescope::TriAffichageListeSatellites()
{
    _ui->listeTelescope->sortItems();
    _ui->listeTelescope->scrollToItem(_ui->listeTelescope->currentItem(), QAbstractItemView::PositionAtTop);
}

void SuiviTelescope::changeEvent(QEvent *evt)
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
 * Calcul des informations AOS/LOS
 */
void SuiviTelescope::CalculAos() const
{
    /* Declarations des variables locales */
    double azim;
    double delai;
    QFont bld;
    ConditionEclipse condEcl;

    /* Initialisations */

    /* Corps de la methode */
    try {

        const bool systeme = settings.value("affichage/syst24h").toBool();

        Date dateCalcul(*_date, _date->offsetUTC());
        Observateur obs = Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        if (_ui->listeTelescope->count() == 0) {
            throw PreviSatException();
        }

        _ui->frameSatSelectionne->setVisible(true);
        const int nsat = getListItemChecked(_ui->listeTelescope);
        if ((nsat == 0) && (_ui->listeTelescope->count() > 0) && _ui->listeTelescope->isVisible()) {
            _ui->frameSatSelectionne->setVisible(false);
            throw PreviSatException();
        }

        QString norad;
        for (int i=0; i<_ui->listeTelescope->count(); i++) {
            if (_ui->listeTelescope->item(i)->checkState() == Qt::Checked) {
                norad = _ui->listeTelescope->item(i)->data(Qt::UserRole).toString();
            }
        }

        // Position de l'observateur
        obs.CalculPosVit(dateCalcul);

        // Position du satellite
        const ElementsOrbitaux elements = Configuration::instance()->mapElementsOrbitaux()[norad];
        _ui->nomsatSuivi->setText(elements.nom);

        Satellite satSuivi(elements);
        satSuivi.CalculPosVit(dateCalcul);
        satSuivi.CalculCoordHoriz(obs);
        satSuivi.CalculElementsOsculateurs(dateCalcul);

        Soleil sol;
        sol.CalculPosition(dateCalcul);

        Lune lun;
        lun.CalculPosition(dateCalcul);

        condEcl.CalculSatelliteEclipse(satSuivi.position(), sol, &lun, true);
        const QString ecl = (condEcl.eclipseTotale()) ? tr("Satellite en éclipse") : tr("Satellite éclairé");

        // Date de lever ou de coucher
        const ElementsAOS elemAos = Evenements::CalculAOS(dateCalcul, satSuivi, obs, SensCalcul::CHRONOLOGIQUE, 0.);
        Date dateAosSuivi(elemAos.date.jourJulienUTC(), _date->offsetUTC());

        if (elemAos.aos) {

            Date dateLosSuivi;
            QString chaine = tr("%1 (dans %2). Azimut : %3", "Delay in hour, minutes, seconds");

            if (elemAos.typeAOS == tr("AOS", "Acquisition of signal")) {

                const ElementsAOS elemLos = Evenements::CalculAOS(Date(dateAosSuivi.jourJulienUTC() + 10. * NB_JOUR_PAR_SEC, _date->offsetUTC()),
                                                                  satSuivi, obs, SensCalcul::CHRONOLOGIQUE, 0.);

                // Date de coucher
                dateLosSuivi = Date(elemLos.date, _date->offsetUTC());
                azim = elemLos.azimut;

                // Lever
                delai = dateAosSuivi.jourJulienUTC() - dateCalcul.jourJulienUTC();
                const Date delaiAOS = Date(delai - 0.5 + EPS_DATES, 0.);

                QString cDelaiAOS;

                if (delai >= 1.) {

                    cDelaiAOS = "";
                    chaine = tr("%1%2. Azimut : %3");

                } else if (delai >= (NB_JOUR_PAR_HEUR - EPS_DATES)) {

                    cDelaiAOS = delaiAOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(11, 5)
                            .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute"));

                } else {

                    cDelaiAOS = delaiAOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(14, 5)
                            .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));
                }

                _ui->leverSatSuivi->setText(
                            chaine.arg(dateAosSuivi.ToShortDate(DateFormat::FORMAT_COURT, (systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H)
                                       .trimmed()).arg(cDelaiAOS).arg(Maths::ToSexagesimal(elemAos.azimut, AngleFormatType::DEGRE, 3, 0, false, true)
                                                                      .mid(0, 9)));

                _ui->lbl_leverSatSuivi->setVisible(true);
                bld.setBold(false);
                _ui->leverSatSuivi->setFont(bld);
                _ui->leverSatSuivi->move(_ui->hauteurMaxSatSuivi->x(), _ui->leverSatSuivi->y());
                _ui->leverSatSuivi->adjustSize();
                _ui->leverSatSuivi2->setVisible(false);

            } else {

                // Le satellite est deja dans le ciel
                satSuivi.CalculElementsOsculateurs(dateCalcul);

                dateLosSuivi = dateAosSuivi;

                const ElementsAOS elem = Evenements::CalculAOS(Date(dateLosSuivi.jourJulienUTC() - 10. * NB_JOUR_PAR_SEC, _date->offsetUTC()),
                                                               satSuivi, obs, SensCalcul::ANTI_CHRONOLOGIQUE, 0.);

                dateAosSuivi = elem.date;
                azim = elemAos.azimut;

                const QString chaine2 = tr("Satellite dans le ciel. Hauteur actuelle : %1. Azimut : %2. %3");
                _ui->leverSatSuivi2->setText(
                            chaine2.arg(Maths::ToSexagesimal(satSuivi.hauteur(), AngleFormatType::DEGRE, 2, 0, false, true).mid(0, 8).trimmed())
                            .arg(Maths::ToSexagesimal(satSuivi.azimut(), AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)).arg(ecl));

                bld.setBold(true);
                _ui->leverSatSuivi2->setFont(bld);
                _ui->lbl_leverSatSuivi->setVisible(false);
                _ui->leverSatSuivi2->setVisible(true);
                _ui->leverSatSuivi2->adjustSize();
                _ui->leverSatSuivi->setText("");
            }

            // Coucher
            delai = dateLosSuivi.jourJulienUTC() - _date->jourJulienUTC();
            const Date delaiLOS = Date(delai - 0.5 + EPS_DATES, 0.);

            QString cDelaiLOS;
            if (delai >= 1.) {
                cDelaiLOS = "";

            } else if (delai >= (NB_JOUR_PAR_HEUR - EPS_DATES)) {

                cDelaiLOS = delaiLOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(11, 5)
                        .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute"));

            } else {
                cDelaiLOS = delaiLOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(14, 5)
                        .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));
            }

            _ui->coucherSatSuivi->setText(
                        chaine.arg(dateLosSuivi.ToShortDate(DateFormat::FORMAT_COURT, ((systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H))
                                   .trimmed()).arg(cDelaiLOS).arg(Maths::ToSexagesimal(azim, AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)));

            // Hauteur max
            std::array<double, DEGRE_INTERPOLATION> jjm;
            QPair<double, double> minmax;

            double jj0 = 0.5 * (dateAosSuivi.jourJulienUTC() + dateLosSuivi.jourJulienUTC());
            double pas = dateLosSuivi.jourJulienUTC() - jj0;

            jjm[0] = jj0 - pas;
            jjm[1] = jj0;
            jjm[2] = jj0 + pas;

            minmax = CalculHauteurMax(jjm, obs, satSuivi);
            pas *= 0.5;

            for(int i=0; i<4; i++) {

                jjm[0] = minmax.first - pas;
                jjm[1] = minmax.first;
                jjm[2] = minmax.first + pas;

                minmax = CalculHauteurMax(jjm, obs, satSuivi);
                pas *= 0.5;
            }

            _ui->hauteurMaxSatSuivi->setText(QString("%1").arg(Maths::ToSexagesimal(minmax.second, AngleFormatType::DEGRE, 2, 0, false, true).mid(0, 8)
                                                               .trimmed()));

            _ui->lbl_hauteurMaxSatSuivi->setVisible(true);
            _ui->lbl_coucherSatSuivi->setVisible(true);
            _ui->hauteurMaxSatSuivi->setVisible(true);
            _ui->coucherSatSuivi->setVisible(true);
            _ui->visibiliteSatSuivi->setVisible(false);
            _ui->frameSatSuivi->setVisible(true);

        } else {

            // Cas des satellites geostationnaires (10 minutes de suivi)
            if (satSuivi.hauteur() > 0.) {
                dateAosSuivi = *_date;

                const QString chaine2 = tr("Satellite dans le ciel. Hauteur actuelle : %1. Azimut : %2. %3");
                _ui->leverSatSuivi->setText(
                            chaine2.arg(Maths::ToSexagesimal(satSuivi.hauteur(), AngleFormatType::DEGRE, 2, 0, false, true).mid(0, 7).trimmed())
                            .arg(Maths::ToSexagesimal(satSuivi.azimut(), AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)).arg(ecl));

                bld.setBold(true);
                _ui->leverSatSuivi->setFont(bld);
                _ui->lbl_leverSatSuivi->setVisible(false);
                _ui->leverSatSuivi->move(0, _ui->leverSatSuivi->y());
                _ui->lbl_hauteurMaxSatSuivi->setVisible(false);
                _ui->lbl_coucherSatSuivi->setVisible(false);
                _ui->hauteurMaxSatSuivi->setVisible(false);
                _ui->coucherSatSuivi->setVisible(false);
                _ui->visibiliteSatSuivi->setVisible(false);
                _ui->frameSatSuivi->setVisible(true);

            } else {
                _ui->visibiliteSatSuivi->setVisible(true);
                _ui->frameSatSuivi->setVisible(false);
            }
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Calcul de la hauteur maximale d'un satellite dans le ciel
 */
QPair<double, double> SuiviTelescope::CalculHauteurMax(const std::array<double, DEGRE_INTERPOLATION> &jjm, Observateur &obs, Satellite &satSuivi) const
{
    /* Declarations des variables locales */
    std::array<double, 3> ht;

    /* Initialisations */

    /* Corps de la methode */
    for (unsigned int i=0; i<DEGRE_INTERPOLATION; i++) {

        const Date date(jjm[i], 0., false);

        obs.CalculPosVit(date);

        // Position du satellite
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(obs);

        // Hauteur
        ht[i] = satSuivi.hauteur();
    }

    /* Retour */
    return Maths::CalculExtremumInterpolation3(jjm, ht);
}

/*
 * Initialisation de la classe SuiviTelescope
 */
void SuiviTelescope::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->valHauteurSatSuivi->setVisible(false);
    _ui->hauteurSatSuivi->setCurrentIndex(settings.value("previsions/hauteurSatSuivi", 2).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationTelescope", 0).toInt());
    _ui->pasSuivi->setValue(settings.value("previsions/pasSuivi", 20).toInt());
    _ui->pecDelai->setChecked(settings.value("previsions/pecDelai", false).toBool());
    _ui->delaiTelescope->setValue(settings.value("previsions/delaiTelescope", 60).toInt());
    _ui->delaiTelescope->setEnabled(_ui->pecDelai->isChecked());
    _ui->demarrerSuiviTelescope->setChecked(settings.value("previsions/demarrerSuiviTelescope", false).toBool());

    QAction* effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &SuiviTelescope::on_filtreSatellites_returnPressed);
    }

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

int SuiviTelescope::getListItemChecked(const QListWidget * const liste) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    int k = 0;

    /* Corps de la methode */
    for(int i=0; i<liste->count(); i++) {
        if (liste->item(i)->checkState() == Qt::Checked) {
            k++;
        }
    }

    /* Retour */
    return k;
}

void SuiviTelescope::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)

    settings.setValue("previsions/hauteurSatSuivi", _ui->hauteurSatSuivi->currentIndex());
    settings.setValue("previsions/lieuxObservationTelescope", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/pasSuivi", _ui->pasSuivi->value());
    settings.setValue("previsions/pecDelai", _ui->pecDelai->isChecked());
    settings.setValue("previsions/delaiTelescope", _ui->delaiTelescope->value());
    settings.setValue("previsions/demarrerSuiviTelescope", _ui->demarrerSuiviTelescope->isChecked());
}

void SuiviTelescope::on_genererPositions_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // TODO
        if (_ui->listeTelescope->count() == 0) {
            throw PreviSatException();
        }

        QStringList satelliteSelectionne;
        for(int i=0; i<_ui->listeTelescope->count(); i++) {
            if (_ui->listeTelescope->item(i)->checkState() == Qt::Checked) {
                satelliteSelectionne.append(_ui->listeTelescope->item(i)->data(Qt::UserRole).toString());
            }
        }

        if (satelliteSelectionne.isEmpty()) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), MessageType::WARNING);
        }

        // TODO
        // Lecture du fichier d'elements orbitaux
        const QMap<QString, ElementsOrbitaux> tabElem =
                GPFormat::LectureFichier(Configuration::instance()->nomfic(), Configuration::instance()->donneesSatellites(),
                                         Configuration::instance()->lgRec(), satelliteSelectionne);

        // TODO
        // Calcul de l'intervalle de temps lorsque le satellite est au-dessus de l'horizon
        const Date date;//(*_date, _date->offsetUTC());
        Observateur obs =  Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        obs.CalculPosVit(date);

        Satellite satSuivi(tabElem[satelliteSelectionne.first()]);
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(obs);
        satSuivi.CalculElementsOsculateurs(date);

        // Hauteur minimale du satellite
        const double hauteurMin = DEG2RAD * ((_ui->hauteurSatSuivi->currentIndex() == 5) ?
                                                 abs(_ui->valHauteurSatSuivi->text().toInt()) : 5 * _ui->hauteurSatSuivi->currentIndex());


        int nbIter = 0;
        const ElementsAOS elementsAos = Evenements::CalculAOS(date, satSuivi, obs, SensCalcul::CHRONOLOGIQUE, hauteurMin);
        Date date1 = elementsAos.date;
        Date date2;

        if (elementsAos.aos) {

            if (elementsAos.typeAOS == tr("AOS", "Acquisition of signal")) {
                date2 = Evenements::CalculAOS(Date(date1.jourJulienUTC() + 10. * NB_JOUR_PAR_SEC, 0.), satSuivi, obs, SensCalcul::CHRONOLOGIQUE,
                                              hauteurMin).date;

            } else {
                // Le satellite est deja dans le ciel
                date2 = Evenements::CalculAOS(Date(date.jourJulienUTC() + NB_JOUR_PAR_SEC, 0.), satSuivi, obs, SensCalcul::CHRONOLOGIQUE,
                                              hauteurMin).date;
                date1 = date;
            }

            nbIter = qRound((date2.jourJulienUTC() - date1.jourJulienUTC()) * NB_MILLISEC_PAR_JOUR + 10000.) / _ui->pasSuivi->value();

        } else {

            // Cas des satellites geostationnaires (10 minutes de suivi)
            if (satSuivi.hauteur() > 0.) {
                date1 = date;
                nbIter = 600000 / _ui->pasSuivi->value();
                date2 = Date(date1.jourJulienUTC() + nbIter * NB_JOUR_PAR_MILLISEC, 0.);
            }
        }

        if (nbIter > 0) {

            _ui->genererPositions->setEnabled(false);

            const QString fmtFicOut = "%1%2%3T%4%5_%6%7%8T%9%10_%11.csv";
            const QString ficOut = fmtFicOut.arg(date1.annee()).arg(date1.mois(), 2, 10, QChar('0')).arg(date1.jour(), 2, 10, QChar('0'))
                    .arg(date1.heure(), 2, 10, QChar('0')).arg(date1.minutes(), 2, 10, QChar('0'))
                    .arg(date2.annee()).arg(date2.mois(), 2, 10, QChar('0')).arg(date2.jour(), 2, 10, QChar('0'))
                    .arg(date2.heure(), 2, 10, QChar('0')).arg(date2.minutes() + 1, 2, 10, QChar('0')).arg(satelliteSelectionne.at(0));

            ConditionsPrevisions conditions;
            conditions.observateur = obs;
            conditions.listeSatellites = satelliteSelectionne;
            conditions.pas = _ui->pasSuivi->value();
            conditions.nbIter = nbIter;
            conditions.fichier = Configuration::instance()->nomfic();
            conditions.ficRes = Configuration::instance()->dirOut() + QDir::separator() + ficOut;
            conditions.jj1 = date1.jourJulienUTC();

            _ficSuivi = conditions.ficRes;

            QVector<int> vecSat;
            vecSat.append(1);

            // Barre de progression
            auto barreProgression = new QProgressBar();
            barreProgression->setAlignment(Qt::AlignHCenter);

            QProgressDialog fenetreProgression;
            fenetreProgression.setWindowTitle(tr("Calculs en cours..."));
            fenetreProgression.setCancelButtonText(tr("Annuler"));
            fenetreProgression.setBar(barreProgression);
            fenetreProgression.setWindowFlags(fenetreProgression.windowFlags() & ~Qt::WindowContextHelpButtonHint);

            // Lancement des calculs
            Telescope::setConditions(conditions);
            QFutureWatcher<void> calculs;

            connect(&fenetreProgression, SIGNAL(canceled()), &calculs, SLOT(cancel()));
            connect(&calculs, SIGNAL(finished()), &fenetreProgression, SLOT(reset()));
            connect(&calculs, SIGNAL(progressRangeChanged(int, int)), &fenetreProgression, SLOT(setRange(int,int)));
            connect(&calculs, SIGNAL(progressValueChanged(int)), &fenetreProgression, SLOT(setValue(int)));

            calculs.setFuture(QtConcurrent::map(vecSat, &Telescope::CalculSuiviTelescope));

            fenetreProgression.exec();
            calculs.waitForFinished();

            if (calculs.isCanceled()) {
                _ui->genererPositions->setEnabled(true);
            } else {

                _ui->genererPositions->setDefault(false);
                _ui->afficherSuivi->setEnabled(true);
                _ui->afficherSuivi->setDefault(true);
                _ui->afficherSuivi->setFocus();
                _ui->genererPositions->setEnabled(true);

                // Affichage des resultats
                emit AfficherMessageStatut(tr("Calculs terminés"), 10);
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void SuiviTelescope::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeTelescope->count(); i++) {
        const QString elem = _ui->listeTelescope->item(i)->text();
        _ui->listeTelescope->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void SuiviTelescope::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void SuiviTelescope::on_parametrageDefautSuivi_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatSuivi->setCurrentIndex(2);
    _ui->pasSuivi->setValue(20);
    _ui->pecDelai->setChecked(false);
    _ui->delaiTelescope->setEnabled(false);
    _ui->delaiTelescope->setValue(60);
    _ui->demarrerSuiviTelescope->setChecked(false);

    /* Retour */
    return;
}

void SuiviTelescope::on_afficherSuivi_clicked()
{
    if (!_ficSuivi.isEmpty()) {
        QDesktopServices::openUrl(QUrl(_ficSuivi.replace("\\", "/")));
    }
}

void SuiviTelescope::on_listeTelescope_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (item != nullptr) {
        for(int i=0; i<_ui->listeTelescope->count(); i++) {
            if ((_ui->listeTelescope->item(i)->checkState() == Qt::Checked) && (item != _ui->listeTelescope->item(i))) {
                _ui->listeTelescope->item(i)->setCheckState(Qt::Unchecked);
            }
        }
        item->setSelected(true);
        item->setCheckState(Qt::Checked);
    }

    _ui->frameSatSelectionne->setVisible(false);

    if (_date != nullptr) {
        CalculAos();
    }

    _ui->genererPositions->setDefault(true);
    _ui->afficherSuivi->setEnabled(false);

    /* Retour */
    return;
}

void SuiviTelescope::on_listeTelescope_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->frameSatSelectionne->setVisible(false);
    if ((currentRow >= 0) && (getListItemChecked(_ui->listeTelescope) > 0)) {

        if (_date != nullptr) {
            CalculAos();
        }

        _ui->genererPositions->setDefault(true);
        _ui->afficherSuivi->setEnabled(false);
    }

    /* Retour */
    return;
}

void SuiviTelescope::on_lieuxObservation_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->frameSatSelectionne->setVisible(false);
    if ((index >= 0) && (getListItemChecked(_ui->listeTelescope) > 0)) {

        if (_date != nullptr) {
            CalculAos();
        }

        _ui->genererPositions->setDefault(true);
        _ui->afficherSuivi->setEnabled(false);
    }

    /* Retour */
    return;
}

void SuiviTelescope::on_hauteurSatSuivi_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSatSuivi->count() - 1) {
        _ui->valHauteurSatSuivi->setText(settings.value("previsions/valHauteurSatSuivi", 0).toString());
        _ui->valHauteurSatSuivi->setVisible(true);
        _ui->valHauteurSatSuivi->setCursorPosition(0);
        _ui->valHauteurSatSuivi->setFocus();
    } else {
        _ui->valHauteurSatSuivi->setVisible(false);
    }

    if (_date != nullptr) {
        CalculAos();
    }

    _ui->genererPositions->setDefault(true);
    _ui->afficherSuivi->setEnabled(false);

    /* Retour */
    return;
}

void SuiviTelescope::on_skywatcher_clicked()
{
    QDesktopServices::openUrl(QUrl("http://skywatcher.com/"));
}

void SuiviTelescope::on_ouvrirSatelliteTracker_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString exeSatelliteTracker = settings.value("fichier/satelliteTracker", "").toString();
    const QFileInfo fi(exeSatelliteTracker);

    /* Corps de la methode */
    if (exeSatelliteTracker.isEmpty() || !fi.exists()) {

        settings.setValue("fichier/satelliteTracker", "");
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir Satellite Tracker"), "Satellite Tracker.exe",
                                                       tr("Fichiers exécutables (*.exe)"));

        if (!fichier.isEmpty()) {
            fichier = QDir::toNativeSeparators(fichier);
            settings.setValue("fichier/satelliteTracker", fichier);
            exeSatelliteTracker = fichier;
        }
    }

    if (!exeSatelliteTracker.isEmpty()) {

        QStringList arguments;
        if (!_ficSuivi.isEmpty()) {

            arguments << "--infile" << _ficSuivi;

            if (_ui->demarrerSuiviTelescope->isChecked()) {
                arguments << "--start";
            }
        }

        if (_ui->pecDelai->isChecked()) {
            arguments << "--countdown" << QString::number(_ui->delaiTelescope->value());
        }

        QProcess proc;
        proc.setProgram(exeSatelliteTracker);
        proc.setArguments(arguments);
        proc.startDetached();
    }

    /* Retour */
    return;
}

void SuiviTelescope::on_pecDelai_toggled(bool checked)
{
    _ui->delaiTelescope->setEnabled(checked);
}
