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
 * >    20 novembre 2023
 *
 */

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
#include "ajustementdates.h"
#include "suivitelescope.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "interface/listWidgetItem.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/exception.h"
#include "previsions/telescope.h"


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
SuiviTelescope::SuiviTelescope(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::SuiviTelescope)
{
    _ui->setupUi(this);

    try {

        _afficherResultats = nullptr;
        _date = nullptr;
        _dateAos = nullptr;
        _dateAosSuivi = nullptr;
        _dateLos = nullptr;
        _dateLosSuivi = nullptr;

        Initialisation();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
}


/*
 * Destructeur
 */
SuiviTelescope::~SuiviTelescope()
{
    settings.setValue("previsions/hauteurSatSuivi", _ui->hauteurSatSuivi->currentIndex());
    settings.setValue("previsions/lieuxObservationTelescope", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/pasSuivi", _ui->pasSuivi->value());
    settings.setValue("previsions/pecDelai", _ui->pecDelai->isChecked());
    settings.setValue("previsions/delaiTelescope", _ui->delaiTelescope->value());
    settings.setValue("previsions/demarrerSuiviTelescope", _ui->demarrerSuiviTelescope->isChecked());

    EFFACE_OBJET(_afficherResultats);
    EFFACE_OBJET(_date);
    EFFACE_OBJET(_dateAos);
    EFFACE_OBJET(_dateAosSuivi);
    EFFACE_OBJET(_dateLos);
    EFFACE_OBJET(_dateLosSuivi);

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
    elem->setData(Qt::CheckStateRole, (check && (norad == noradDefaut)) ? Qt::Checked : Qt::Unchecked);
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
    _ui->satellitesChoisis->setChecked(false);
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
void SuiviTelescope::CalculAos()
{
    /* Declarations des variables locales */
    QFont bld;
    ConditionEclipse condEcl;

    /* Initialisations */

    /* Corps de la methode */
    try {

        Date date1;
        const double hauteurMin = MATHS::DEG2RAD * ((_ui->hauteurSatSuivi->currentIndex() == 5) ?
                                                        abs(_ui->valHauteurSatSuivi->text().toInt()) : 5 * _ui->hauteurSatSuivi->currentIndex());

        const bool systeme = settings.value("affichage/systemeHoraire").toBool();

        Date dateCalcul(*_date, _date->offsetUTC());
        Observateur obs = Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        if (_ui->listeTelescope->count() == 0) {
            throw Exception();
        }

        _ui->frameSatSelectionne->setVisible(true);
        const int nsat = getListItemChecked(_ui->listeTelescope);
        if ((nsat == 0) && (_ui->listeTelescope->count() > 0) && _ui->listeTelescope->isVisible()) {
            _ui->frameSatSelectionne->setVisible(false);
            throw Exception();
        }

        for (int i=0; i<_ui->listeTelescope->count(); i++) {
            if (_ui->listeTelescope->item(i)->checkState() == Qt::Checked) {
                _norad = _ui->listeTelescope->item(i)->data(Qt::UserRole).toString();
            }
        }

        // Position de l'observateur
        obs.CalculPosVit(dateCalcul);

        // Position du satellite
        const ElementsOrbitaux elements = Configuration::instance()->mapElementsOrbitaux()[_norad];
        _ui->nomsatSuivi->setText(elements.nom);

        Satellite satSuivi(elements);
        satSuivi.CalculPosVit(dateCalcul);
        satSuivi.CalculCoordHoriz(obs, true);
        satSuivi.CalculElementsOsculateurs(dateCalcul);

        Soleil sol;
        sol.CalculPosVit(dateCalcul);

        Lune lun;
        lun.CalculPosVit(dateCalcul);

        condEcl.CalculSatelliteEclipse(satSuivi.position(), sol, &lun, true);
        const QString ecl = (condEcl.eclipseTotale()) ? tr("Satellite en éclipse") : tr("Satellite éclairé");

        // Date de lever ou de coucher
        const ElementsAOS elemAos = Evenements::CalculAOS(dateCalcul, satSuivi, obs, SensCalcul::CHRONOLOGIQUE, 0.);

        EFFACE_OBJET(_dateAosSuivi);
        _dateAosSuivi = new Date(elemAos.date.jourJulienUTC(), _date->offsetUTC());
        date1 = *_dateAosSuivi;

        if (elemAos.aos) {

            double azim;
            double delai;
            QString chaine = tr("%1 (dans %2). Azimut : %3", "Delay in hour, minutes, seconds");

            if (elemAos.typeAOS == tr("AOS", "Acquisition of signal")) {

                const ElementsAOS elemLos = Evenements::CalculAOS(Date(_dateAosSuivi->jourJulienUTC() + 10. * DATE::NB_JOUR_PAR_SEC, _date->offsetUTC()),
                                                                  satSuivi, obs, SensCalcul::CHRONOLOGIQUE, 0.);

                // Date de coucher
                EFFACE_OBJET(_dateLosSuivi);
                _dateLosSuivi = new Date(elemLos.date, _date->offsetUTC());
                azim = elemLos.azimut;

                // Lever
                delai = _dateAosSuivi->jourJulienUTC() - dateCalcul.jourJulienUTC();
                const Date delaiAOS = Date(delai - 0.5 + DATE::EPS_DATES, 0.);

                QString cDelaiAOS;

                if (delai >= 1.) {

                    cDelaiAOS = "";
                    chaine = tr("%1%2. Azimut : %3");

                } else if (delai >= (DATE::NB_JOUR_PAR_HEUR - DATE::EPS_DATES)) {

                    cDelaiAOS = delaiAOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(11, 5)
                            .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute"));

                } else {

                    cDelaiAOS = delaiAOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(14, 5)
                            .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));
                }

                _ui->leverSatSuivi->setText(
                            chaine.arg(_dateAosSuivi->ToShortDate(DateFormat::FORMAT_COURT,
                                                                 (systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H).trimmed())
                            .arg(cDelaiAOS).arg(Maths::ToSexagesimal(elemAos.azimut, AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)));

                _ui->lbl_leverSatSuivi->setVisible(true);
                bld.setBold(false);
                _ui->leverSatSuivi->setFont(bld);
                _ui->leverSatSuivi->move(_ui->hauteurMaxSatSuivi->x(), _ui->leverSatSuivi->y());
                _ui->leverSatSuivi->adjustSize();
                _ui->leverSatSuivi2->setVisible(false);

            } else {

                // Le satellite est deja dans le ciel
                satSuivi.CalculElementsOsculateurs(dateCalcul);

                EFFACE_OBJET(_dateLosSuivi);
                _dateLosSuivi = new Date(_dateAosSuivi->jourJulienUTC(), _date->offsetUTC());

                const ElementsAOS elem = Evenements::CalculAOS(Date(_dateLosSuivi->jourJulienUTC() - 10. * DATE::NB_JOUR_PAR_SEC, _date->offsetUTC()),
                                                               satSuivi, obs, SensCalcul::ANTI_CHRONOLOGIQUE, 0.);

                EFFACE_OBJET(_dateAosSuivi);
                _dateAosSuivi = new Date((elem.date.jourJulienUTC() < _date->jourJulienUTC()) ? *_date : elem.date, _date->offsetUTC());
                azim = elemAos.azimut;
                date1 = Date(elem.date.jourJulienUTC(), 0.);

                const QString chaine2 = tr("Satellite dans le ciel. Hauteur actuelle : %1. Azimut : %2. %3");
                _ui->leverSatSuivi2->setText(
                            chaine2.arg(Maths::ToSexagesimal(satSuivi.hauteur(), AngleFormatType::DEGRE, 2, 0, false, true).mid(0, 9).trimmed())
                            .arg(Maths::ToSexagesimal(satSuivi.azimut(), AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)).arg(ecl));

                bld.setBold(true);
                _ui->leverSatSuivi2->setFont(bld);
                _ui->lbl_leverSatSuivi->setVisible(false);
                _ui->leverSatSuivi2->setVisible(true);
                _ui->leverSatSuivi2->adjustSize();
                _ui->leverSatSuivi->setText("");
            }

            // Coucher
            delai = _dateLosSuivi->jourJulienUTC() - _date->jourJulienUTC();
            const Date delaiLOS = Date(delai - 0.5 + DATE::EPS_DATES, 0.);

            QString cDelaiLOS;
            if (delai >= 1.) {
                cDelaiLOS = "";

            } else if (delai >= (DATE::NB_JOUR_PAR_HEUR - DATE::EPS_DATES)) {

                cDelaiLOS = delaiLOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(11, 5)
                        .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute"));

            } else {
                cDelaiLOS = delaiLOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).mid(14, 5)
                        .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));
            }

            _ui->coucherSatSuivi->setText(
                        chaine.arg(_dateLosSuivi->ToShortDate(DateFormat::FORMAT_COURT, ((systeme) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H))
                                   .trimmed()).arg(cDelaiLOS).arg(Maths::ToSexagesimal(azim, AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)));

            // Hauteur max
            std::array<double, MATHS::DEGRE_INTERPOLATION> jjm;
            QPointF minmax;

            double jj0 = 0.5 * (date1.jourJulienUTC() + _dateLosSuivi->jourJulienUTC());
            double pas = _dateLosSuivi->jourJulienUTC() - jj0;

            jjm[0] = jj0 - pas;
            jjm[1] = jj0;
            jjm[2] = jj0 + pas;

            minmax = CalculHauteurMax(jjm, obs, satSuivi);
            pas *= 0.5;

            for(int i=0; i<4; i++) {

                jjm[0] = minmax.x() - pas;
                jjm[1] = minmax.x();
                jjm[2] = minmax.x() + pas;

                minmax = CalculHauteurMax(jjm, obs, satSuivi);
                pas *= 0.5;
            }

            _ui->hauteurMaxSatSuivi->setText(QString("%1").arg(Maths::ToSexagesimal(minmax.y(), AngleFormatType::DEGRE, 2, 0, false, true).mid(0, 8)
                                                               .trimmed()));

            _ui->ajusterDates->setEnabled(minmax.y() >= hauteurMin);
            _ui->lbl_hauteurMaxSatSuivi->setVisible(true);
            _ui->lbl_coucherSatSuivi->setVisible(true);
            _ui->hauteurMaxSatSuivi->setVisible(true);
            _ui->coucherSatSuivi->setVisible(true);
            _ui->visibiliteSatSuivi->setVisible(false);
            _ui->frameSatSuivi->setVisible(true);

        } else {

            // Cas des satellites geostationnaires (10 minutes de suivi)
            if (satSuivi.hauteur() > 0.) {

                EFFACE_OBJET(_dateAosSuivi);
                _dateAosSuivi = new Date(*_date, _date->offsetUTC());

                const QString chaine2 = tr("Satellite dans le ciel. Hauteur actuelle : %1. Azimut : %2. %3");
                _ui->leverSatSuivi->setText(
                            chaine2.arg(Maths::ToSexagesimal(satSuivi.hauteur(), AngleFormatType::DEGRE, 2, 0, false, true).mid(0, 8).trimmed())
                            .arg(Maths::ToSexagesimal(satSuivi.azimut(), AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)).arg(ecl));

                bld.setBold(true);
                _ui->leverSatSuivi->setFont(bld);

                _ui->ajusterDates->setEnabled(satSuivi.hauteur() >= hauteurMin);
                _ui->lbl_leverSatSuivi->setVisible(false);
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

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

/*
 * Calcul de la hauteur maximale d'un satellite dans le ciel
 */
QPointF SuiviTelescope::CalculHauteurMax(const std::array<double, MATHS::DEGRE_INTERPOLATION> &jjm, const Observateur &obs, Satellite &satSuivi) const
{
    /* Declarations des variables locales */
    std::array<QPointF, MATHS::DEGRE_INTERPOLATION> ht;

    /* Initialisations */
    Observateur observateur = obs;

    /* Corps de la methode */
    for (unsigned int i=0; i<MATHS::DEGRE_INTERPOLATION; i++) {

        const Date date(jjm[i], 0., false);

        observateur.CalculPosVit(date);

        // Position du satellite
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(observateur, true);

        // Hauteur
        ht[i] = QPointF(jjm[i], satSuivi.hauteur());
    }

    /* Retour */
    return Maths::CalculExtremumInterpolation3(ht);
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

    QAction const * effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &SuiviTelescope::on_filtreSatellites_returnPressed);
    }

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Ajuster les dates initiale et finale
 */
void SuiviTelescope::AjusterDates(const QDateTime &date1, const QDateTime &date2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    EFFACE_OBJET(_dateAos);
    _dateAos = new Date(date1.addSecs(static_cast<quint64> (-floor(_date->offsetUTC() * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES))), 0.);

    EFFACE_OBJET(_dateLos);
    _dateLos = new Date(date2.addSecs(static_cast<quint64> (-floor(_date->offsetUTC() * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES))), 0.);

    _ui->genererPositions->setDefault(true);
    _ui->afficherSuivi->setEnabled(false);

    /* Retour */
    return;
}

void SuiviTelescope::on_genererPositions_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (_ui->listeTelescope->count() == 0) {
            throw Exception();
        }

        // Elements orbitaux
        QMap<QString, ElementsOrbitaux> mapElem;
        for(int i=0; i<_ui->listeTelescope->count(); i++) {
            if (_ui->listeTelescope->item(i)->checkState() == Qt::Checked) {
                const QString norad = _ui->listeTelescope->item(i)->data(Qt::UserRole).toString();
                mapElem.insert(norad, Configuration::instance()->mapElementsOrbitaux()[norad]);
            }
        }

        if (mapElem.isEmpty()) {
            throw Exception(tr("Aucun satellite n'est sélectionné dans la liste"), MessageType::WARNING);
        }

        // Calcul de l'intervalle de temps lorsque le satellite est au-dessus de l'horizon
        const Date date(*_date, _date->offsetUTC());
        Observateur obs =  Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());

        obs.CalculPosVit(date);

        Satellite satSuivi(mapElem.first());
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(obs, true);
        satSuivi.CalculElementsOsculateurs(date);

        // Hauteur minimale du satellite
        const double hauteurMin = MATHS::DEG2RAD * ((_ui->hauteurSatSuivi->currentIndex() == 5) ?
                                                 abs(_ui->valHauteurSatSuivi->text().toInt()) : 5 * _ui->hauteurSatSuivi->currentIndex());

        int nbIter = 0;
        const ElementsAOS elementsAos = Evenements::CalculAOS(date, satSuivi, obs, SensCalcul::CHRONOLOGIQUE, hauteurMin);
        Date date1 = (_dateAos == nullptr) ? *_dateAosSuivi : *_dateAos;
        Date date2 = (_dateLos == nullptr) ? *_dateLosSuivi : *_dateLos;

        if (elementsAos.aos) {

            if (elementsAos.typeAOS == tr("LOS", "Loss of signal")) {
                // Le satellite est deja dans le ciel, on prend la date courante au lieu de la date de l'AOS
                date1 = date;
            }

            nbIter = qRound((date2.jourJulienUTC() - date1.jourJulienUTC()) * DATE::NB_MILLISEC_PAR_JOUR + 10000.) / _ui->pasSuivi->value();

        } else if (satSuivi.hauteur() > 0.) {

            // Cas des satellites geostationnaires (10 minutes de suivi)
            date1 = date;
            nbIter = 600000 / _ui->pasSuivi->value();
            date2 = Date(date1.jourJulienUTC() + nbIter * DATE::NB_JOUR_PAR_MILLISEC, 0.);
        }

        if (nbIter > 0) {

            _ui->genererPositions->setEnabled(false);

            const QString fmtFicOut = "%1%2%3T%4%5_%6%7%8T%9%10_%11.csv";
            const QString ficOut = fmtFicOut.arg(date1.annee()).arg(date1.mois(), 2, 10, QChar('0')).arg(date1.jour(), 2, 10, QChar('0'))
                    .arg(date1.heure(), 2, 10, QChar('0')).arg(date1.minutes(), 2, 10, QChar('0'))
                    .arg(date2.annee()).arg(date2.mois(), 2, 10, QChar('0')).arg(date2.jour(), 2, 10, QChar('0'))
                    .arg(date2.heure(), 2, 10, QChar('0')).arg(date2.minutes() + 1, 2, 10, QChar('0')).arg(mapElem.first().norad);

            ConditionsPrevisions conditions;
            conditions.observateur = obs;
            conditions.pas = _ui->pasSuivi->value();
            conditions.nbIter = nbIter;
            conditions.tabElem = mapElem;
            conditions.ficRes = Configuration::instance()->dirOut() + QDir::separator() + ficOut;
            conditions.jj1 = date1.jourJulienUTC();

            // Ecriture des informations de prévisions dans le fichier de log
            qInfo() << "--";
            qInfo() << "Génération des positions pour le suivi avec in télescope :";
            qInfo() << "Date de début =" << date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
            qInfo() << "Pas de génération =" << conditions.pas;
            qInfo() << "Nombre d'itérations =" << conditions.nbIter;

            qInfo() << QString("Lieu d'observation : %1 %2 %3")
                       .arg(conditions.observateur.longitude() * MATHS::RAD2DEG, 0, 'f', 9)
                       .arg(conditions.observateur.latitude() * MATHS::RAD2DEG, 0, 'f', 9)
                       .arg(conditions.observateur.altitude() * 1.e3);

            qInfo() << "Satellite sélectionné =" << conditions.tabElem.first().norad;

            qInfo() << "--";

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

    } catch (Exception const &) {
    }

    /* Retour */
    return;
}

void SuiviTelescope::on_filtreSatellites_textChanged(const QString &arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!arg1.isEmpty()) {
        const bool etat = _ui->satellitesChoisis->blockSignals(true);
        _ui->satellitesChoisis->setChecked(false);
        _ui->satellitesChoisis->blockSignals(etat);
    }

    for(int i=0; i<_ui->listeTelescope->count(); i++) {
        const QString elem = _ui->listeTelescope->item(i)->text();
        _ui->listeTelescope->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }

    /* Retour */
    return;
}

void SuiviTelescope::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void SuiviTelescope::on_satellitesChoisis_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (checked) {
        _ui->filtreSatellites->clear();
        for(int i=0; i<_ui->listeTelescope->count(); i++) {
            const bool chk = !(_ui->listeTelescope->item(i)->data(Qt::CheckStateRole).toUInt() == Qt::Checked);
            _ui->listeTelescope->item(i)->setHidden(chk);
        }
    } else {
        on_filtreSatellites_textChanged("");
        on_filtreSatellites_returnPressed();
    }

    /* Retour */
    return;
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
        QDesktopServices::openUrl(QUrl(_ficSuivi.replace(R"(\)", "/")));
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
        EFFACE_OBJET(_dateAos);
        EFFACE_OBJET(_dateLos);
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
            EFFACE_OBJET(_dateAos);
            EFFACE_OBJET(_dateLos);
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
            EFFACE_OBJET(_dateAos);
            EFFACE_OBJET(_dateLos);
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
        EFFACE_OBJET(_dateAos);
        EFFACE_OBJET(_dateLos);
    }

    _ui->genererPositions->setDefault(true);
    _ui->afficherSuivi->setEnabled(false);

    /* Retour */
    return;
}

void SuiviTelescope::on_ajusterDates_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Observateur observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation->currentIndex());
    const ElementsOrbitaux elements = Configuration::instance()->mapElementsOrbitaux()[_norad];
    const double hauteur = MATHS::DEG2RAD * ((_ui->hauteurSatSuivi->currentIndex() == 5) ?
                                                    abs(_ui->valHauteurSatSuivi->text().toInt()) : 5 * _ui->hauteurSatSuivi->currentIndex());

    /* Corps de la methode */
    AjustementDates * const ajustementDates = new AjustementDates(_dateAosSuivi->ToQDateTime(DateFormatSec::FORMAT_SEC),
                                                                 _dateLosSuivi->ToQDateTime(DateFormatSec::FORMAT_SEC),
                                                                 observateur, elements, _date->offsetUTC(), hauteur, this);

    QEvent evt(QEvent::LanguageChange);

    ajustementDates->changeEvent(&evt);
    ajustementDates->setWindowModality(Qt::ApplicationModal);
    ajustementDates->show();
    ajustementDates->setVisible(true);

    connect(ajustementDates, &AjustementDates::AjusterDates, this, &SuiviTelescope::AjusterDates);

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
