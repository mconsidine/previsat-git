/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wshadow"
#include <QFileInfo>
#include <QFutureWatcher>
#include <QMenu>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "ui_calculstransits.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "calculstransits.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "interface/listwidgetitem.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"
#include "previsions/transits.h"


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

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
CalculsTransits::~CalculsTransits()
{
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
void CalculsTransits::CalculAgeElementsOrbitauxTransit()
{

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
    _ui->ageMaxElementsOrbitauxTransit->setValue(settings.value("previsions/ageMaxElementsOrbitauxTransit", 2.).toDouble());
    _ui->elongationMaxCorps->setValue(settings.value("previsions/elongationMaxCorps", 5.).toDouble());

    QAction* effacerFiltre = _ui->filtreSatellites->findChild<QAction*>();
    if (effacerFiltre) {
        connect(effacerFiltre, &QAction::triggered, this, &CalculsTransits::on_filtreSatellites_returnPressed);
    }

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

void CalculsTransits::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)

    settings.setValue("previsions/hauteurSatTransit", _ui->hauteurSatTransit->currentIndex());
    settings.setValue("previsions/valHauteurSatTransit", _ui->valHauteurSatTransit->text().toInt());
    settings.setValue("previsions/lieuxObservationTransit", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/ageMaxElementsOrbitauxTransit", _ui->ageMaxElementsOrbitauxTransit->value());
    settings.setValue("previsions/elongationMaxCorps", _ui->elongationMaxCorps->value());
    settings.setValue("previsions/calcTransitLunaireJour", _ui->calcTransitLunaireJour->isChecked());
}

void CalculsTransits::on_calculsTransit_clicked()
{
    /* Declarations des variables locales */
    QVector<int> vecSat;
    ConditionsPrevisions conditions;

    /* Initialisations */
    vecSat.append(0);
    conditions.listeSatellites.clear();

    /* Corps de la methode */
    try {

        // Ecart heure locale - UTC
        const double offset1 = Date::CalculOffsetUTC(_ui->dateInitialeTransit->dateTime());
        const double offset2 = Date::CalculOffsetUTC(_ui->dateFinaleTransit->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeTransit->date().year(), _ui->dateInitialeTransit->date().month(), _ui->dateInitialeTransit->date().day(),
                         _ui->dateInitialeTransit->time().hour(), _ui->dateInitialeTransit->time().minute(),
                         _ui->dateInitialeTransit->time().second(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinaleTransit->date().year(), _ui->dateFinaleTransit->date().month(), _ui->dateFinaleTransit->date().day(),
                         _ui->dateFinaleTransit->time().hour(), _ui->dateFinaleTransit->time().minute(), _ui->dateFinaleTransit->time().second(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            const double tmp = conditions.jj2;
            conditions.jj2 = conditions.jj1;
            conditions.jj1 = tmp;
        }

        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = settings.value("affichage/syst24h").toBool();

        // Hauteur minimale du satellite
        conditions.hauteur = DEG2RAD * ((_ui->hauteurSatTransit->currentIndex() == 5) ?
                                            abs(_ui->valHauteurSatTransit->text().toInt()) : 5 * _ui->hauteurSatTransit->currentIndex());

        // Elongation maximale
        conditions.seuilConjonction = DEG2RAD * _ui->elongationMaxCorps->value();

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

        // Age maximal des elements orbitaux
/*        const double ageElements = _ui->ageMaxElementsOrbitauxTransit->value();

        // Selection du fichier d'elements orbitaux
        const QFileInfo fi1(Configuration::instance()->dirElem() + QDir::separator() + "visual.txt");
        const QFileInfo fi2(Configuration::instance()->dirElem() + QDir::separator() + "iss.3le");
        const QStringList listeSatellites(QStringList () << Configuration::instance()->noradStationSpatiale());
        QList<ElementsOrbitaux> tabElem;

        if (fi2.exists() && (fi2.size() != 0)) {

            // Verification du fichier
            if (TLE::VerifieFichier(fi2.absoluteFilePath()) == 0) {

                const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                       "Le fichier %1 n'est pas un TLE");
                throw PreviSatException(msg.arg(fi2.absoluteFilePath()), WARNING);
            }

            // Lecture du fichier iss.3le
            tabElem = TLE::LectureFichier3le(fi2.absoluteFilePath());

            // Verification des dates
            const double datePremierTLE = tabElem.first().epoque().jourJulienUTC();
            const double dateDernierTLE = tabElem.last().epoque().jourJulienUTC();

            double age1 = 0.;
            if (conditions.jj1 < datePremierTLE) {
                age1 = datePremierTLE - conditions.jj1;
            }

            if (conditions.jj1 > dateDernierTLE) {
                age1 = conditions.jj1 - dateDernierTLE;
            }

            double age2 = 0.;
            if (conditions.jj2 < datePremierTLE) {
                age2 = datePremierTLE - conditions.jj2;
            }

            if (conditions.jj2 > dateDernierTLE) {
                age2 = conditions.jj2 - dateDernierTLE;
            }

            if ((fabs(age1) > EPSDBL100) && (fabs(age2) > EPSDBL100)) {

                if (fi1.exists() && (fi1.size() != 0)) {

                    // Verification du fichier
                    if (TLE::VerifieFichier(fi1.absoluteFilePath()) == 0) {

                        const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                               "Le fichier %1 n'est pas un TLE");
                        throw PreviSatException(msg.arg(fi1.absoluteFilePath()), WARNING);
                    }

                    // Utilisation des elements orbitaux du fichier visual.txt
                    const QList<TLE> tle(QList<TLE> () << TLE::LectureFichier(fi1.absoluteFilePath(), Configuration::instance()->donneesSatellites(),
                                                                              Configuration::instance()->lgRec(), listeSatellites).first());

                    if (tle.isEmpty()) {

                        const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                               "Le fichier %1 ne contient pas le TLE de l'ISS");
                        throw PreviSatException(msg.arg(fi1.absoluteFilePath()), WARNING);

                    } else {

                        // Age du TLE de l'ISS contenu dans visual.txt
                        const double ageIss1 = fabs(conditions.jj1 - tle.first().epoque().jourJulienUTC());
                        const double ageIss2 = fabs(conditions.jj2 - tle.first().epoque().jourJulienUTC());

                        if (((age1 > 0.) || (age2 > 0.)) && (ageIss1 < age1) && (ageIss2 < age2)) {

                            // Utilisation du TLE du fichier visual.txt
                            age1 = qMin(ageIss1, ageIss2);
                            age2 = age1;
                            conditions.tabtle = tle;

                        } else {

                            // Utilisation des TLE du fichier iss.3le
                            conditions.tabtle = tabElem;
                        }
                    }
                }
            } else {

                // Utilisation des TLE du fichier iss.3le
                conditions.tabElem = tabElem;
            }

            if ((age1 > (ageElements + 0.05)) || (age2 > (ageElements + 0.05))) {
                const QString msg = tr("L'âge du TLE de l'ISS (%1 jours) est supérieur à %2 jours");
                Message::Afficher(msg.arg(fabs(qMax(age1, age2)), 0, 'f', 1).arg(ageElements, 0, 'f', 1), INFO);
            }

        } else {

            if (fi1.exists() && (fi1.size() != 0)) {

                // Verification du fichier
                if (TLE::VerifieFichier(fi1.absoluteFilePath()) == 0) {

                    const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                           "Le fichier %1 n'est pas un TLE");
                    throw PreviSatException(msg.arg(fi1.absoluteFilePath()), WARNING);
                }

                // Utilisation des elements orbitaux du fichier visual.txt
                tabElem.append(TLE::LectureFichier(fi1.absoluteFilePath(), Configuration::instance()->donneesSatellites(),
                                                  Configuration::instance()->lgRec(), listeSatellites).first());

                if (tabElem.isEmpty()) {

                    const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                           "Le fichier %1 ne contient pas le TLE de l'ISS");
                    throw PreviSatException(msg.arg(fi1.absoluteFilePath()), WARNING);

                } else {
                    conditions.tabElem = tabElem;
                }
            } else {
                throw PreviSatException(tr("Le fichier TLE n'existe pas"), WARNING);
            }
        }*/

        // Nom du fichier resultat
        const QString chaine = tr("transits", "file name (without accent)") + "_%1_%2.txt";
        conditions.ficRes = Configuration::instance()->dirTmp() + QDir::separator() +
                chaine.arg(date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).remove("/").split(" ").at(0));

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
                                                 settings.value("affichage/valeurZoomMap").toInt());
                _afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void CalculsTransits::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeTransits->count(); i++) {
        const QString elem = _ui->listeTransits->item(i)->text();
        _ui->listeTransits->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void CalculsTransits::on_filtreSatellites_returnPressed()
{
    _ui->filtreSatellites->clear();
    TriAffichageListeSatellites();
}

void CalculsTransits::on_parametrageDefautTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->hauteurSatTransit->setCurrentIndex(1);
    _ui->valHauteurSatTransit->setVisible(false);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->ageMaxElementsOrbitauxTransit->setValue(2.);
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
        const QString ficElem = Configuration::instance()->adresseCelestrakNorad().arg("iss").replace("gp", "sup-gp");
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
        CalculAgeElementsOrbitauxTransit();

    } catch (PreviSatException &e) {
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
