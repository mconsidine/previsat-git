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
 * >    general.cpp
 *
 * Localisation
 * >    interface.onglets.general
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >    8 avril 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSoundEffect>
#include <QThread>
#include "ui_options.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "general.h"
#include "configuration/configuration.h"
#include "interface/onglets/osculateurs/osculateurs.h"
#include "interface/onglets/previsions/calculsflashs.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "previsions/flashs.h"
#include "ui_calculsflashs.h"
#include "ui_general.h"
#include "ui_osculateurs.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);

static const char* _titresLuneSoleil[] = {
    QT_TRANSLATE_NOOP("General", "Coordonnées du Soleil"),
    QT_TRANSLATE_NOOP("General", "Coordonnées de la Lune"),
    QT_TRANSLATE_NOOP("General", "Évènements Soleil"),
    QT_TRANSLATE_NOOP("General", "Évènements Lune")
};


bool General::_acalcDN;
bool General::_isEclipse;
Date *General::_dateEclipse;

bool General::_acalcAOS;
double General::_htSat;
ElementsAOS *General::_elementsAOS;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
General::General(CalculsFlashs *flashs, Osculateurs *osculateurs, QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::General)
{
    _ui->setupUi(this);

    _flashs = flashs;
    _osculateurs = osculateurs;

    _dateEclipse = nullptr;
    _elementsAOS = nullptr;
    _uniteVitesse = false;

    try {

#if (BUILD_TEST == false)
        Initialisation();
#endif

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
General::~General()
{
    EFFACE_OBJET(_dateEclipse);
    EFFACE_OBJET(_elementsAOS);
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::General *General::ui() const
{
    return _ui;
}

ElementsAOS *General::elementsAOS() const
{
    return _elementsAOS;
}

Date General::dateEclipse()
{
    return *_dateEclipse;
}


/*
 * Modificateurs
 */


/*
 * Methodes publiques
 */
void General::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Affichage de la date
    AffichageDate(date);

    // Affichage des donnees du Soleil et de la Lune
    AffichageDonneesSoleilLune();

    if (Configuration::instance()->listeSatellites().isEmpty()) {

        _ui->frame_satellite->setVisible(false);

    } else {

        _ui->frame_satellite->setVisible(true);

        // Affichage des donnees relatives au satellite par defaut
        AffichageDonneesSatellite(date);
    }

    /* Retour */
    return;
}

void General::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);

        _ui->soleilLunePrec->setToolTip(
                    QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + _ui->stackedWidget_soleilLune->count() - 1)
                    % _ui->stackedWidget_soleilLune->count()]));
        _ui->soleilLuneSuiv->setToolTip(QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + 1) %
                                        _ui->stackedWidget_soleilLune->count()]));
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
 * Affichage de la date
 */
void General::AffichageDate(const Date &date)
{
    /* Declarations des variables locales */
    QString chaineUTC;

    /* Initialisations */

    /* Corps de la methode */
    if (fabs(date.offsetUTC()) > MATHS::EPSDBL100) {
        QTime heur(0, 0);
        heur = heur.addSecs(qRound(fabs(date.offsetUTC()) * DATE::NB_SEC_PAR_JOUR));
        chaineUTC = tr("UTC %1 %2", "Universal Time Coordinated").arg((date.offsetUTC() > 0.) ? "+" : "-").arg(heur.toString("hh:mm"));
        _ui->utcDateHeure->setText(chaineUTC);
    } else {
        chaineUTC = tr("UTC", "Universal Time Coordinated");
        _ui->utcDateHeure->setText(chaineUTC);
    }

    const DateSysteme syst = (settings.value("affichage/systemeHoraire").toBool()) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H;
    const QString chaine = QString("%1  %2").arg(date.ToLongDate(Configuration::instance()->locale(), syst)).arg(chaineUTC);

    _ui->dateHeure1->setText(chaine);
    _osculateurs->ui()->dateHeure1->setText(chaine);

    /* Retour */
    return;
}

/*
 * Affichage des donnees relatives au satellite par defaut
 */
void General::AffichageDonneesSatellite(const Date &date)
{
    /* Declarations des variables locales */
    QString text;

    /* Initialisations */
    const QString fmt = "%1 %2";
    const QString unite = (settings.value("affichage/unite").toBool()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile");
    const Satellite &satellite = Configuration::instance()->listeSatellites().first();

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat->setText(satellite.elementsOrbitaux().nom);

    // Age du TLE
    QPalette palette;
    QBrush couleur;
    const double age = fabs(satellite.ageElementsOrbitaux());
    if (age <= 5.) {
        couleur.setColor(QColor("forestgreen"));
    } else if (age <= 10.) {
        couleur.setColor(Qt::darkYellow);
    } else if (age <= 15.) {
        couleur.setColor(QColor("orange"));
    } else {
        couleur.setColor(Qt::red);
    }

    couleur.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::WindowText, couleur);
    _ui->tempsDepuisEpoque->setPalette(palette);
    _ui->tempsDepuisEpoque->setText(fmt.arg(satellite.ageElementsOrbitaux(), 0, 'f', 2).arg(tr("jours")));

    // Longitude/Latitude/Altitude
    const QString ews = (satellite.longitude() >= 0.) ? tr("Ouest") : tr("Est");
    _ui->longitudeSat->setText(fmt.arg(Maths::ToSexagesimal(fabs(satellite.longitude()), AngleFormatType::DEGRE, 3, 0, false, true)).arg(ews));
    const QString nss = (satellite.latitude() >= 0.) ? tr("Nord") : tr("Sud");
    _ui->latitudeSat->setText(fmt.arg(Maths::ToSexagesimal(fabs(satellite.latitude()), AngleFormatType::DEGRE, 2, 0, false, true)).arg(nss));
    if (settings.value("affichage/unite").toBool()) {
        _ui->altitudeSat->setText(text.asprintf("%.1f ", satellite.altitude()) + unite);
    } else {
        _ui->altitudeSat->setText(text.asprintf("%.1f ", satellite.altitude() * TERRE::MILE_PAR_KM) + unite);
    }

    // Hauteur/Azimut/Distance
    _ui->hauteurSat->setText(Maths::ToSexagesimal(satellite.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutSat->setText(Maths::ToSexagesimal(satellite.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));
    if (settings.value("affichage/unite").toBool()) {
        _ui->distanceSat->setText(text.asprintf("%.1f ", satellite.distance()) + unite);
    } else {
        _ui->distanceSat->setText(text.asprintf("%.1f ", satellite.distance() * TERRE::MILE_PAR_KM) + unite);
    }

    // Ascension droite/declinaison/constellation
    _ui->ascensionDroiteSat->setText(Maths::ToSexagesimal(satellite.ascensionDroite(), AngleFormatType::HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonSat->setText(Maths::ToSexagesimal(satellite.declinaison(), AngleFormatType::DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationSat->setText(satellite.constellation());

    // Direction/vitesse/range rate
    _ui->directionSat->setText((satellite.vitesse().z() >= 0.) ? tr("Ascendant") : tr("Descendant"));
    AffichageVitesses(date);

    // Numero d'orbite
    _ui->orbiteSat1->setText(tr("Orbite n°%1").arg(satellite.nbOrbites()));
    _ui->orbiteSat2->setText(_ui->orbiteSat1->text());

    // Magnitude/illumination
    const double fractionIlluminee = 100. * satellite.magnitude().fractionIlluminee();
    ConditionEclipse conditionEclipse = satellite.conditionEclipse();
    const QString corpsOccultant = (conditionEclipse.eclipseLune().luminosite < conditionEclipse.eclipseSoleil().luminosite) ? " " + tr("Lune") : "";

    if (satellite.elementsOrbitaux().donnees.magnitudeStandard() > 98.) {

        // Magnitude standard inconnue
        EcritureInformationsEclipse(corpsOccultant, fractionIlluminee);

    } else {

        // Satellite au dessus de l'horizon
        if (satellite.isVisible()) {

            // Satellite en eclipse totale
            if (conditionEclipse.eclipseTotale()) {
                _ui->magnitudeSat->setText(tr("Satellite en éclipse totale%1").arg(corpsOccultant));
            } else {

                // Satellite eclaire ou partiellement eclaire
                const QString fmt1 = tr("Magnitude (Illumination) : %1 (%2%)");
                double magn = satellite.magnitude().magnitude();

                // Le satellite est un MetOp ou un SkyMed, on calcule la veritable magnitude (flash)
                if (Configuration::instance()->mapFlashs().keys().contains(satellite.elementsOrbitaux().norad)
                        && settings.value("affichage/affnotif").toBool()) {

                    const double mag = Flashs::CalculMagnitudeFlash(date, satellite, Configuration::instance()->soleil(),
                                                                    settings.value("affichage/effetEclipsesMagnitude").toBool(),
                                                                    settings.value("affichage/refractionAtmospherique").toBool());

                    magn = qMin(mag, magn);

                    double crep = 0.;
                    if (_flashs->ui()->hauteurSoleilMetOp->currentIndex() <= 3) {
                        crep = -6. * _flashs->ui()->hauteurSoleilMetOp->currentIndex();
                    } else if (_flashs->ui()->hauteurSoleilMetOp->currentIndex() == 4) {
                        crep = 90.;
                    } else if (_flashs->ui()->hauteurSoleilMetOp->currentIndex() == 5) {
                        crep = _flashs->ui()->valHauteurSoleilMetOp->text().toInt();
                    } else {
                    }
                    crep *= MATHS::DEG2RAD;

                    NotificationSonore notif = Configuration::instance()->notifFlashs();
                    if (Configuration::instance()->soleil().hauteur() <= crep) {

                        if ((notif == NotificationSonore::ATTENTE_LOS) && (magn <= _flashs->ui()->magnitudeMaxMetOp->value())) {
                            notif = NotificationSonore::NOTIFICATION_AOS;
                        }


                        if ((notif == NotificationSonore::ATTENTE_AOS) && (magn >= _flashs->ui()->magnitudeMaxMetOp->value())) {
                            notif = NotificationSonore::NOTIFICATION_LOS;
                        }
                    }

                    if (notif == NotificationSonore::NOTIFICATION_AOS) {
                        if (!((Configuration::instance()->notifAOS() == NotificationSonore::NOTIFICATION_AOS)
                              || (Configuration::instance()->notifAOS() == NotificationSonore::NOTIFICATION_LOS))) {
                            JouerSonFlash();
                        }

                        notif = NotificationSonore::ATTENTE_AOS;
                    }

                    if (notif == NotificationSonore::NOTIFICATION_LOS) {
                        if (!((Configuration::instance()->notifAOS() == NotificationSonore::NOTIFICATION_AOS)
                              || (Configuration::instance()->notifAOS() == NotificationSonore::NOTIFICATION_LOS))) {
                            JouerSonFlash();
                        }

                        notif = NotificationSonore::ATTENTE_LOS;
                    }
                }

                const QString magnitude = fmt1.arg(text.asprintf("%+.1f", magn)).arg(text.asprintf("%.0f", fractionIlluminee));

                QString eclipse;
                const QString fmt2 = " %1/%2";
                const QString corps = (corpsOccultant.isEmpty()) ? tr("S", "Sun") : tr("L", "Moon");

                if (conditionEclipse.eclipsePartielle()) {
                    eclipse = fmt2.arg(corps).arg(tr("P", "partial eclipse"));
                }

                if (conditionEclipse.eclipseAnnulaire()) {
                    eclipse = fmt2.arg(corps).arg(tr("A", "annular eclipse"));
                }

                _ui->magnitudeSat->setText(magnitude + eclipse);
            }
        } else {
            EcritureInformationsEclipse(corpsOccultant, fractionIlluminee);
        }
    }
    _ui->magnitudeSat2->setText(_ui->magnitudeSat->text() + ".");

    // Prochaine transition jour/nuit
    _acalcDN = !(_isEclipse && satellite.conditionEclipse().eclipseTotale());
    if (_acalcDN) {

        EFFACE_OBJET(_dateEclipse);
        _dateEclipse = new Date(Evenements::CalculOmbrePenombre(date, satellite, settings.value("affichage/nombreTrajectoires").toInt(),
                                                                settings.value("affichage/eclipsesLune").toBool(),
                                                                settings.value("affichage/refractionAtmospherique").toBool()), date.offsetUTC());
        _acalcDN = false;
        _isEclipse = satellite.conditionEclipse().eclipseTotale();
    }

    const DateSysteme syst = (settings.value("affichage/systemeHoraire").toBool()) ? DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H;
    double delai = _dateEclipse->jourJulienUTC() - date.jourJulienUTC();

    if ((delai >= -DATE::EPS_DATES) && (_dateEclipse->jourJulienUTC() < satellite.traceAuSol().last().jourJulienUTC)) {

        QString transitionJN = tr("Prochain %1 :");
        _ui->lbl_prochainJN->setText(transitionJN.arg((satellite.conditionEclipse().eclipseTotale()) ?
                                                          tr("N>J", "Night to day") : tr("J>N", "Day to night")));
        _ui->lbl_prochainJN->setToolTip(satellite.conditionEclipse().eclipseTotale() ? tr("Nuit > Jour") : tr("Jour > Nuit"));

        // Delai de l'evenement
        transitionJN = tr("%1 (dans %2).", "Delay in hours, minutes or seconds");
        const Date delaiEcl = Date(delai - 0.5 + DATE::EPS_DATES, 0.);
        QString cDelaiEcl;

        if (delai >= 1.) {

            transitionJN = "%1%2";
            cDelaiEcl = "";

        } else if (delai >= (DATE::NB_JOUR_PAR_HEUR - DATE::EPS_DATES)) {

            cDelaiEcl = delaiEcl.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).section(" ", 1).mid(0, 5)
                    .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute"));

        } else {
            cDelaiEcl = delaiEcl.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).section(" ", 1).mid(3, 5)
                    .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));
        }

        _ui->dateJN->setText(transitionJN.arg(_dateEclipse->ToShortDate(DateFormat::FORMAT_COURT, syst)).arg(cDelaiEcl));
        _ui->dateJN->adjustSize();
        _ui->dateJN->resize(_ui->dateJN->width(), 16);
        _ui->stackedWidget_aos->setCurrentIndex(0);
    } else {
        _ui->stackedWidget_aos->setCurrentIndex(1);
    }

    // Prochain AOS/LOS
    _acalcAOS = ((_htSat * satellite.hauteur()) <= 0.);
    if (_acalcAOS) {

        EFFACE_OBJET(_elementsAOS);
        _elementsAOS = new ElementsAOS();

        *_elementsAOS = Evenements::CalculAOS(date, satellite, Configuration::instance()->observateur());
        _acalcAOS = false;
        _htSat = satellite.hauteur();
    }

    if (_elementsAOS->aos) {

        // Type d'evenement (AOS ou LOS)
        QString chaine = tr("Prochain %1 :").arg(_elementsAOS->typeAOS);
        _ui->lbl_prochainAOS1->setText(chaine);
        _ui->lbl_prochainAOS2->setText(chaine);
        _ui->lbl_prochainAOS1->setToolTip((chaine.contains(tr("AOS"))) ? tr("Acquisition du signal") : tr("Perte du signal"));
        _ui->lbl_prochainAOS2->setToolTip(_ui->lbl_prochainAOS1->text());

        // Delai de l'evenement
        chaine = tr("%1  (dans %2).", "Delay in hours, minutes or seconds");
        Date dateAOS = Date(_elementsAOS->date, date.offsetUTC());
        delai = dateAOS.jourJulienUTC() - date.jourJulienUTC();

        const Date delaiAOS = Date(delai - 0.5 + DATE::EPS_DATES, 0.);
        QString cDelaiAOS;

        if (delai >= 1.) {

            chaine = "%1%2";
            cDelaiAOS = "";

        } else if (delai >= (DATE::NB_JOUR_PAR_HEUR - DATE::EPS_DATES)) {

            cDelaiAOS = delaiAOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).section(" ", 1).mid(0, 5)
                    .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute"));

        } else {
            cDelaiAOS = delaiAOS.ToShortDate(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).section(" ", 1).mid(3, 5)
                    .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));
        }

        _ui->dateAOS1->setText(chaine.arg(dateAOS.ToShortDate(DateFormat::FORMAT_COURT, syst).trimmed()).arg(cDelaiAOS));
        _ui->dateAOS2->setText(_ui->dateAOS1->text());
        _ui->lbl_azimut1->setText(tr("Azimut : %1").arg(Maths::ToSexagesimal(_elementsAOS->azimut, AngleFormatType::DEGRE, 3, 0, false, true)
                                                        .mid(0, 9)));
        _ui->lbl_azimut2->setText(_ui->lbl_azimut1->text());
    }

    // Angle beta
    const QString angleBeta = tr("Beta : %1", "Beta angle (angle between orbit plane and direction of Sun)");
    _ui->lbl_beta1->setText(angleBeta.arg(Maths::ToSexagesimal(satellite.beta(), AngleFormatType::DEGRE, 3, 0, false, true).mid(0, 9)));
    _ui->lbl_beta2->setText(_ui->lbl_beta1->text());

    /* Retour */
    return;
}

/*
 * Affichage des donnees du Soleil et de la Lune
 */
void General::AffichageDonneesSoleilLune()
{
    /* Declarations des variables locales */
    QString cond;

    /* Initialisations */
    const QString fmt = "%1 %2";
    const Soleil &soleil = Configuration::instance()->soleil();
    const Lune &lune = Configuration::instance()->lune();

    /* Corps de la methode */
    // Conditions d'observation
    const double ht = soleil.hauteur() * MATHS::RAD2DEG;
    if (ht >= 0.) {
        cond = tr("Jour");
    } else if (ht >= -6.) {
        cond = tr("Crépuscule civil");
    } else if (ht >= -12.) {
        cond = tr("Crépuscule nautique");
    } else if (ht >= -18.) {
        cond = tr("Crépuscule astronomique");
    } else {
        cond = tr("Nuit");
    }
    _ui->conditionsObservation->setText(cond);

    /*
     * Soleil
     */

    // Hauteur/azimut/distance du Soleil
    _ui->hauteurSoleil->setText(Maths::ToSexagesimal(soleil.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutSoleil->setText(Maths::ToSexagesimal(soleil.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));
    _ui->distanceSoleil->setText(tr("%1 UA").arg(soleil.distanceUA(), 0, 'f', 3));

    // Ascension droite/declinaison/constellation du Soleil
    _ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.ascensionDroite(), AngleFormatType::HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.declinaison(), AngleFormatType::DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationSoleil->setText(soleil.constellation());

    // Longitude/latitude/diametre apparent
    const QString ews = (soleil.longitude() >= 0.) ? tr("Ouest") : tr("Est");
    _ui->longitudeSol->setText(fmt.arg(Maths::ToSexagesimal(fabs(soleil.longitude()), AngleFormatType::DEGRE, 3, 0, false, true)).arg(ews));
    const QString nss = (soleil.latitude() >= 0.) ? tr("Nord") : tr("Sud");
    _ui->latitudeSol->setText(fmt.arg(Maths::ToSexagesimal(fabs(soleil.latitude()), AngleFormatType::DEGRE, 2, 0, false, true)).arg(nss));
    _ui->diametreApparentSol->setText(Maths::ToSexagesimal(2. * asin(SOLEIL::RAYON_SOLAIRE / soleil.distance()),
                                                           AngleFormatType::DEGRE, 0, 0, false, true).section("°", 1));

    // Heures de lever/passage au meridien/coucher/crepuscules
    _ui->leverSoleil->setText(soleil.dateLever());
    _ui->meridienSoleil->setText(soleil.dateMeridien());
    _ui->coucherSoleil->setText(soleil.dateCoucher());

    _ui->aubeAstro->setText(soleil.datesCrepuscules()[2]);
    _ui->aubeNautique->setText(soleil.datesCrepuscules()[1]);
    _ui->aubeCivile->setText(soleil.datesCrepuscules()[0]);

    _ui->crepusculeCivil->setText(soleil.datesCrepuscules()[3]);
    _ui->crepusculeNautique->setText(soleil.datesCrepuscules()[4]);
    _ui->crepusculeAstro->setText(soleil.datesCrepuscules()[5]);


    /*
     * Lune
     */

    // Hauteur/azimut/distance de la Lune
    _ui->hauteurLune->setText(Maths::ToSexagesimal(lune.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutLune->setText(Maths::ToSexagesimal(lune.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));
    _ui->distanceLune->setText(QString("%1 %2").arg(lune.distance(), 0, 'f', 0)
                               .arg((settings.value("affichage/unite").toBool()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile")));

    // Ascension droite/declinaison/constellation de la Lune
    _ui->ascensionDroiteLune->setText(Maths::ToSexagesimal(lune.ascensionDroite(), AngleFormatType::HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonLune->setText(Maths::ToSexagesimal(lune.declinaison(), AngleFormatType::DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationLune->setText(lune.constellation());

    // Phase/illumination/magnitude
    _ui->phaseLune->setText(lune.phase());
    _ui->magnitudeIllumLune->setText(QString("%1 (%2%)").arg(lune.magnitude(), 0, 'f', 2).arg(lune.fractionIlluminee() * 100., 0, 'f', 0));
    _ui->diametreApparentLune->setText(Maths::ToSexagesimal(2. * asin(LUNE::RAYON_LUNAIRE / lune.distance()), AngleFormatType::DEGRE, 0, 0, false, true)
                                       .section("°", 1));

    // Lever/passage au meridien/coucher/phases
    _ui->leverLune->setText(lune.dateLever());
    _ui->meridienLune->setText(lune.dateMeridien());
    _ui->coucherLune->setText(lune.dateCoucher());

    _ui->nouvelleLune->setText(lune.datesPhases()[0]);
    _ui->premierQuartier->setText(lune.datesPhases()[1]);
    _ui->pleineLune->setText(lune.datesPhases()[2]);
    _ui->dernierQuartier->setText(lune.datesPhases()[3]);

    /* Retour */
    return;
}

void General::EcritureInformationsEclipse(const QString &corpsOccultant, const double fractionIlluminee) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const ConditionEclipse &conditionEclipse = Configuration::instance()->listeSatellites().first().conditionEclipse();

    /* Corps de la methode */
    if (conditionEclipse.eclipseTotale()) {
        _ui->magnitudeSat->setText(tr("Satellite en éclipse totale%1").arg(corpsOccultant));
    } else {

        QString illumination = tr("Satellite non éclipsé");
        if (conditionEclipse.eclipsePartielle()) {
            illumination = tr("Satellite en éclipse partielle%1").arg(corpsOccultant);
        }
        if (conditionEclipse.eclipseAnnulaire()) {
            illumination = tr("Satellite en éclipse annulaire%1").arg(corpsOccultant);
        }
        illumination.append(" (%1%)");
        _ui->magnitudeSat->setText(illumination.arg(fractionIlluminee, 0, 'f', 0));
    }

    /* Retour */
    return;
}

/*
 * Affichage des vitesses (par seconde ou par heure)
 */
void General::AffichageVitesses(const Date &date)
{
    /* Declarations des variables locales */
    QString text;
    QString unite;
    Vecteur3D vitesse;

    /* Initialisations */
    const Satellite satellite = Configuration::instance()->listeSatellites().first();
    if (_osculateurs->ui()->typeRepere->currentIndex() == 0) {
        vitesse = satellite.vitesse();
    } else {
        Vecteur3D position;
        satellite.CalculPosVitECEF(date, position, vitesse);
    }

    double rangeRate = satellite.rangeRate();

    if (settings.value("affichage/unite").toBool()) {
        unite = (_uniteVitesse) ? tr("km/h", "Kilometer per hour") : tr("km/s", "Kilometer per second");
    } else {
        unite = (_uniteVitesse) ? tr("kn", "Knot") : tr("nmi/s", "Nautical mile per second");
        vitesse *= TERRE::MILE_PAR_KM;
        rangeRate *= TERRE::MILE_PAR_KM;
    }

    /* Corps de la methode */
    if (_uniteVitesse) {

        vitesse *= DATE::NB_SEC_PAR_HEUR;

        _ui->vitesseSat->setText(text.asprintf("%.0f ", vitesse.Norme()) + unite);
        _ui->rangeRate->setText(text.asprintf("%+.0f ", rangeRate * DATE::NB_SEC_PAR_HEUR) + unite);

        // Vitesse cartesienne
        _osculateurs->ui()->vxsat->setText(text.asprintf("%+.3f ", vitesse.x()) + unite);
        _osculateurs->ui()->vysat->setText(text.asprintf("%+.3f ", vitesse.y()) + unite);
        _osculateurs->ui()->vzsat->setText(text.asprintf("%+.3f ", vitesse.z()) + unite);

    } else {

        _ui->vitesseSat->setText(text.asprintf("%.3f ", vitesse.Norme()) + unite);
        _ui->rangeRate->setText(text.asprintf("%+.3f ", rangeRate) + unite);

        // Vitesse cartesienne
        _osculateurs->ui()->vxsat->setText(text.asprintf("%+.6f ", vitesse.x()) + unite);
        _osculateurs->ui()->vysat->setText(text.asprintf("%+.6f ", vitesse.y()) + unite);
        _osculateurs->ui()->vzsat->setText(text.asprintf("%+.6f ", vitesse.z()) + unite);
    }

    /* Retour */
    return;
}

void General::mouseDoubleClickEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (_ui->dateHeure1->underMouse()) {

        // Passage en mode manuel
        emit ModeManuel(true);
    }

    /* Retour */
    return;
}

/*
 * Affichage des lieux d'observation dans la liste deroulante
 */
void General::AffichageLieuObs()
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool premier = true;
    _ui->lieuObservation->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->observateurs());
    while (it.hasNext()) {

        const Observateur obs = it.next();
        const QString nomlieu = obs.nomlieu();

        _ui->lieuObservation->addItem(nomlieu);

        if (premier) {

            // Longitude/Latitude/Altitude
            const double lo = obs.longitude();
            const double la = obs.latitude();
            const double atd = obs.altitude() * 1000.;

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");

            // Affichage des coordonnees
            const QString fmt = "%1 %2";
            _ui->longitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(lo), AngleFormatType::DEGRE, 3, 0, false, true)).arg(ew));
            _ui->latitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(la), AngleFormatType::DEGRE, 2, 0,false, true)).arg(ns));
            _ui->altitudeObs->setText(
                fmt.arg((settings.value("affichage/unite").toBool()) ? atd : qRound(atd * TERRE::PIED_PAR_METRE + 0.5 * sgn(atd)))
                    .arg((settings.value("affichage/unite").toBool()) ? tr("m", "meter") : tr("ft", "foot")));
            premier = false;
        }
    }

    _ui->lieuObservation->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet
 */
void General::SauveOngletGeneral(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fichier);
        if (sw.open(QIODevice::WriteOnly | QIODevice::Text)) {

            if (!sw.isWritable()) {
                qWarning() << "Problème de droits d'écriture du fichier" << sw.fileName();
                throw PreviSatException(tr("Problème de droits d'écriture du fichier %1").arg(sw.fileName()), MessageType::WARNING);
            }

            QTextStream flux(&sw);
            flux.setEncoding(QStringConverter::System);

#if (BUILD_TEST == false)
            const QString titre = "%1 %2 / %3 (c) %4";
            flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APP_VER_MAJ)).arg(APP_NAME).arg(QString(APP_ANNEES_DEV))
                 << Qt::endl << Qt::endl << Qt::endl;
#endif

            flux << tr("Date :", "Date and hour") << " " << _ui->dateHeure1->text() << Qt::endl << Qt::endl;

            flux << tr("Lieu d'observation :") << " " << _ui->lieuObservation->currentText() << Qt::endl;
            QString chaine = tr("Longitude  : %1\tLatitude : %2\tAltitude : %3", "Observer coordinates");
            flux << chaine.arg(_ui->longitudeObs->text()).arg(_ui->latitudeObs->text()).arg(_ui->altitudeObs->text()) << Qt::endl;
            chaine = tr("Conditions : %1", "Conditions of observation");
            flux << chaine.arg(_ui->conditionsObservation->text()) << Qt::endl << Qt::endl << Qt::endl;

#if (BUILD_TEST == false)
            if (_ui->frame_satellite->isVisible())
#endif
            {
                // Donnees sur le satellite
                flux << tr("Nom du satellite :") + " " + _ui->nomsat->text() << Qt::endl << Qt::endl;

                chaine = tr("Longitude : %1\t\tHauteur    : %2\tAscension droite :  %3");
                flux << chaine.arg(_ui->longitudeSat->text().trimmed()).arg(_ui->hauteurSat->text()).arg(_ui->ascensionDroiteSat->text().trimmed())
                     << Qt::endl;

                chaine = tr("Latitude  :  %1\t\tAzimut (N) : %2\tDéclinaison      : %3");
                flux << chaine.arg(_ui->latitudeSat->text().trimmed()).arg(_ui->azimutSat->text().trimmed()).arg(_ui->declinaisonSat->text())
                     << Qt::endl;

                chaine = tr("Altitude  :  %1\t\tDistance   : %2\tConstellation    : %3", "Altitude of satellite");
                flux << chaine.arg(_ui->altitudeSat->text().leftJustified(13, ' ')).arg(_ui->distanceSat->text().leftJustified(13, ' '))
                        .arg(_ui->constellationSat->text()) << Qt::endl << Qt::endl;

                chaine = tr("Direction          : %1  \t%2      \t\t%3");
                flux << chaine.arg(_ui->directionSat->text()).arg(_ui->orbiteSat1->text())
                        .arg((_ui->magnitudeSat->x() == 333) ? _ui->magnitudeSat->text() : "").trimmed() << Qt::endl;

                chaine = tr("Vitesse orbitale   : %1\t%2\t%3");
                flux << chaine.arg(_ui->vitesseSat->text().rightJustified(11, ' '))
#if (BUILD_TEST == true)
                        .arg(_ui->lbl_prochainJN->text() + " " + _ui->dateJN->text() + " ")
                        .arg(_ui->lbl_beta1->text()).trimmed() << Qt::endl;
#else
                        .arg((_ui->dateJN->isVisible()) ? _ui->lbl_prochainJN->text() + " " + _ui->dateJN->text() + " " : _ui->magnitudeSat->text())
                        .arg((_ui->dateAOS1->isVisible()) ? _ui->lbl_beta1->text() : "").trimmed() << Qt::endl;
#endif

                chaine = tr("Variation distance : %1  \t%2", "Range rate");
                flux << chaine.arg(_ui->rangeRate->text().rightJustified(11, ' '))
#if (BUILD_TEST == true)
                        .arg(_ui->lbl_prochainAOS1->text() + " " + _ui->dateAOS1->text()).trimmed() + " " + _ui->lbl_azimut1->text()
                     << Qt::endl << Qt::endl << Qt::endl;
#else
                        .arg((_ui->dateAOS1->isVisible()) ? _ui->lbl_prochainAOS1->text() + " " + _ui->dateAOS1->text() + " " + _ui->lbl_azimut1->text()
                                                          : _ui->lbl_beta1->text()).trimmed() << Qt::endl << Qt::endl << Qt::endl;
#endif
            }

            // Donnees sur le Soleil
            flux << tr("Coordonnées du Soleil :") << Qt::endl;
            chaine = tr("Hauteur    : %1\t\tAscension droite  :  %2 \tLongitude                : %3");
            flux << chaine.arg(_ui->hauteurSoleil->text().trimmed()).arg(_ui->ascensionDroiteSoleil->text()).arg(_ui->longitudeSol->text().trimmed())
                 << Qt::endl;

            chaine = tr("Azimut (N) : %1\t\tDéclinaison       : %2 \tLatitude                 : %3", "Azimuth from the North");
            flux << chaine.arg(_ui->azimutSoleil->text().trimmed()).arg(_ui->declinaisonSoleil->text()).arg(_ui->latitudeSol->text().trimmed())
                 << Qt::endl;

            chaine = tr("Distance   : %1   \t\tConstellation     : %2\t\t\tDiamètre apparent        : %3");
            flux << chaine.arg(_ui->distanceSoleil->text()).arg(_ui->constellationSoleil->text()).arg(_ui->diametreApparentSol->text().trimmed())
                 << Qt::endl << Qt::endl;

            flux << tr("Évènements Soleil :") << Qt::endl;
            chaine = tr("Lever    : %1\t\t\tAube astronomique : %2\t\tCrépuscule civil         : %3", "Sunrise");
            flux << chaine.arg(_ui->leverSoleil->text()).arg(_ui->aubeAstro->text()).arg(_ui->crepusculeCivil->text()) << Qt::endl;
            chaine = tr("Méridien : %1\t\t\tAube nautique     : %2\t\tCrépuscule nautique      : %3", "Meridian pass for the Sun");
            flux << chaine.arg(_ui->meridienSoleil->text()).arg(_ui->aubeNautique->text()).arg(_ui->crepusculeNautique->text()) << Qt::endl;
            chaine = tr("Coucher  : %1\t\t\tAube civile       : %2\t\tCrépuscule astronomique  : %3", "Sunset");
            flux << chaine.arg(_ui->coucherSoleil->text()).arg(_ui->aubeCivile->text()).arg(_ui->crepusculeAstro->text())
                 << Qt::endl << Qt::endl << Qt::endl;


            // Donnees sur la Lune
            flux << tr("Coordonnées de la Lune :") << Qt::endl;
            chaine = tr("Hauteur    : %1\t\tAscension droite :  %2 \tPhase                    : %3", "Moon phase");
            flux << chaine.arg(_ui->hauteurLune->text().trimmed()).arg(_ui->ascensionDroiteLune->text()).arg(_ui->phaseLune->text()) << Qt::endl;

            chaine = tr("Azimut (N) : %1\t\tDéclinaison      : %2 \tMagnitude (Illumination) : %3", "Azimuth from the North");
            flux << chaine.arg(_ui->azimutLune->text().trimmed()).arg(_ui->declinaisonLune->text()).arg(_ui->magnitudeIllumLune->text()) << Qt::endl;

            chaine = tr("Distance   : %1  \t\tConstellation    : %2 \t\t\tDiamètre apparent        : %3");
            flux << chaine.arg(_ui->distanceLune->text()).arg(_ui->constellationLune->text()).arg(_ui->diametreApparentLune->text().trimmed())
                 << Qt::endl << Qt::endl;

            flux << tr("Évènements Lune :") << Qt::endl;
            flux << tr("Lever    : %1", "Moonrise").arg(_ui->leverLune->text()) << Qt::endl;
            flux << tr("Méridien : %1", "Meridian pass for the Moon").arg(_ui->meridienLune->text()) << Qt::endl;
            flux << tr("Coucher  : %1", "Moonset").arg(_ui->coucherLune->text()) << Qt::endl << Qt::endl;

            flux << tr("Nouvelle Lune    : %1").arg(_ui->nouvelleLune->text()) << Qt::endl;
            flux << tr("Premier quartier : %1").arg(_ui->premierQuartier->text()) << Qt::endl;
            flux << tr("Pleine Lune      : %1").arg(_ui->pleineLune->text()) << Qt::endl;
            flux << tr("Dernier quartier : %1").arg(_ui->dernierQuartier->text()) << Qt::endl;
        }

        sw.close();

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

/*
 * Initialisation de la classe General
 */
void General::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _indexLuneSoleil = settings.value("affichage/indexInformations", 0).toUInt();

    _ui->stackedWidget_soleilLune->setCurrentIndex(_indexLuneSoleil);
    _ui->lbl_crepusculeCivil->setContentsMargins(0, 3, 0, 0);

    _ui->soleilLunePrec->setToolTip(
                QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + _ui->stackedWidget_soleilLune->count() - 1)
                % _ui->stackedWidget_soleilLune->count()]));
    _ui->soleilLuneSuiv->setToolTip(QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + 1) %
                                    _ui->stackedWidget_soleilLune->count()]));

    _ui->frameSimu->setVisible(false);

    connect(_osculateurs, &Osculateurs::AffichageVitesses, this, &General::AffichageVitesses);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Execute la notification sonore d'un flash
 */
void General::JouerSonFlash()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomSonFlash = Configuration::instance()->dirCommonData() + QDir::separator() + "sound" + QDir::separator() + "flare.wav";

    /* Corps de la methode */
    const QFileInfo ff(nomSonFlash);
    if (ff.exists()) {
        QSoundEffect son;
        son.setSource(QUrl(nomSonFlash));
        son.play();
    }

    /* Retour */
    return;
}

void General::on_dateHeure2_dateTimeChanged(const QDateTime &dateTime)
{
    _ui->dateHeure2->setDisplayFormat(tr("dddd dd MMMM yyyy  hh:mm:ss") + ((settings.value("affichage/systemeHoraire").toBool()) ? "" : "a"));
    emit ChangementDate(dateTime);
}

void General::on_soleilLunePrec_clicked()
{
    _indexLuneSoleil = (_ui->stackedWidget_soleilLune->currentIndex() + _ui->stackedWidget_soleilLune->count() - 1)
            % _ui->stackedWidget_soleilLune->count();
    _ui->stackedWidget_soleilLune->setCurrentIndex(_indexLuneSoleil);
}

void General::on_soleilLuneSuiv_clicked()
{
    _indexLuneSoleil = (_ui->stackedWidget_soleilLune->currentIndex() + 1) % _ui->stackedWidget_soleilLune->count();
    _ui->stackedWidget_soleilLune->setCurrentIndex(_indexLuneSoleil);
}

void General::on_stackedWidget_soleilLune_currentChanged(int arg1)
{
    Q_UNUSED(arg1)

    _ui->soleilLunePrec->setToolTip(
                QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + _ui->stackedWidget_soleilLune->count() - 1)
                % _ui->stackedWidget_soleilLune->count()]));
    _ui->soleilLuneSuiv->setToolTip(QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + 1) %
                                    _ui->stackedWidget_soleilLune->count()]));
}

void General::on_pause_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->pause->setEnabled(!_ui->pause->isEnabled());
    const bool enb = !_ui->pause->isEnabled();
    _ui->play->setEnabled(enb);
    _ui->rewind->setEnabled(enb);
    _ui->forward->setEnabled(enb);
    _ui->backward->setEnabled(enb);
    _ui->frameSimu->setFocus();
    QThread::msleep(100);
    emit ModeManuel(enb);

    /* Retour */
    return;
}

void General::on_play_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->play->setEnabled(!_ui->play->isEnabled());
    const bool enb = !_ui->play->isEnabled();
    _ui->pause->setEnabled(enb);
    _ui->rewind->setEnabled(enb);
    _ui->forward->setEnabled(enb);
    _ui->backward->setEnabled(enb);
    _ui->frameSimu->setFocus();
    emit ModeManuel(enb);

    /* Retour */
    return;
}

void General::on_rewind_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->rewind->setEnabled(!_ui->rewind->isEnabled());
    const bool enb = !_ui->rewind->isEnabled();
    _ui->play->setEnabled(enb);
    _ui->pause->setEnabled(enb);
    _ui->forward->setEnabled(enb);
    _ui->backward->setEnabled(enb);
    _ui->frameSimu->setFocus();
    emit ModeManuel(enb);

    /* Retour */
    return;
}

void General::on_forward_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->forward->setEnabled(!_ui->forward->isEnabled());
    const bool enb = !_ui->forward->isEnabled();
    _ui->play->setEnabled(enb);
    _ui->pause->setEnabled(enb);
    _ui->rewind->setEnabled(enb);
    _ui->backward->setEnabled(enb);
    _ui->frameSimu->setFocus();
    QThread::msleep(100);
    emit ModeManuel(enb);

    /* Retour */
    return;
}

void General::on_backward_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->backward->setEnabled(!_ui->backward->isEnabled());
    const bool enb = !_ui->backward->isEnabled();
    _ui->play->setEnabled(enb);
    _ui->pause->setEnabled(enb);
    _ui->rewind->setEnabled(enb);
    _ui->forward->setEnabled(enb);
    _ui->frameSimu->setFocus();
    QThread::msleep(100);
    emit ModeManuel(enb);

    /* Retour */
    return;
}
