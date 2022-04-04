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
 * >    onglets.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 decembre 2019
 *
 * Date de revision
 * >    2 decembre 2021
 *
 */

#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QSettings>
#pragma GCC diagnostic ignored "-Wshadow"
#include <QtConcurrent>
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QInputDialog>
#include "ui_onglets.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QProgressDialog>
#include <QScrollBar>
#include <QTextStream>
#include <QToolTip>
#include "afficher.h"
#include "gestionnairetle.h"
#include "onglets.h"
#include "telecharger.h"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/systemesolaire/terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/systeme/decompression.h"
#include "previsions/evenementsorbitaux.h"
#include "previsions/flashs.h"
#include "previsions/prevision.h"
#include "previsions/telescope.h"
#include "previsions/transitsiss.h"


bool Onglets::_acalcDN;
bool Onglets::_isEclipse;
Date *Onglets::_dateEclipse;

bool Onglets::_acalcAOS;
double Onglets::_htSat;
ElementsAOS *Onglets::_elementsAOS;

static QString _ficSuivi;

static Afficher *afficherResultats = nullptr;

// Registre
static QSettings settings("Astropedia", "PreviSat");

static const char* _titreInformations[] = {
    QT_TRANSLATE_NOOP("Onglets", "Informations satellite"),
    QT_TRANSLATE_NOOP("Onglets", "Recherche données") };

static const char* _titreMajTLE[] = {
    QT_TRANSLATE_NOOP("Onglets", "Mise à jour TLE auto"),
    QT_TRANSLATE_NOOP("Onglets", "Mise à jour TLE manuelle") };

/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Onglets::Onglets(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::Onglets)
{
    _ui->setupUi(this);

    _ui->categorieOrbite->installEventFilter(this);
    _ui->categorieOrbiteDonneesSat->installEventFilter(this);
    _ui->pays->installEventFilter(this);
    _ui->paysDonneesSat->installEventFilter(this);
    _ui->siteLancement->installEventFilter(this);
    _ui->siteLancementDonneesSat->installEventFilter(this);

    _date = nullptr;

    _info = true;
    _uniteVitesse = false;
    _acalcDN = true;
    _isEclipse = false;

    _acalcAOS = true;
    _htSat = 0.;

    // TODO sauvegarder les index
    _indexInfo = 0;
    _indexOption = 0;
    _indexMajTLE = 0;
    _nbInformations = 2;
    _titreOptions << tr("Satellites") << tr("Système solaire, étoiles") << tr("Affichage") << tr("Système", "Operating system");

    // Initialisation au demarrage
    InitAffichageDemarrage();

#if (BUILD_TEST == false)

    // Chargement des groupes de TLE (onglet Outils)
    AffichageGroupesTLE();

    // Chargement des fichiers de preference
    InitFicPref(false);
    ChargementPref();

    // Chargement des stations
    InitChargementStations();

    // Chargement des fichiers d'observation
    InitFicObs(true);
    if (_ui->categoriesObs->count() > 0) {
        _ui->categoriesObs->setCurrentRow(0);
        _ui->ajdfic->setCurrentIndex(0);
    }

    // Chargement des fichiers images de cartes du monde
    InitFicMap(false);

    // Chargement des fichiers sons (pour les AOS et LOS)
    InitFicSon();
#endif
}

/*
 * Destructeur
 */
Onglets::~Onglets()
{
    if (_date != nullptr) {
        delete _date;
        _date = nullptr;
    }

    if (_dateEclipse != nullptr) {
        delete _dateEclipse;
        _dateEclipse = nullptr;
    }

    if (_elementsAOS != nullptr) {
        delete _elementsAOS;
        _elementsAOS = nullptr;
    }

    if (afficherResultats != nullptr) {
        delete afficherResultats;
        afficherResultats = nullptr;
    }

    delete _ui;
}


/*
 * Accesseurs
 */
QString Onglets::dirDwn() const
{
    return _dirDwn;
}

Date Onglets::dateEclipse()
{
    return *_dateEclipse;
}

Ui::Onglets *Onglets::ui()
{
    return _ui;
}

/*
 * Modificateurs
 */
void Onglets::setAcalcAOS(bool acalcAOS)
{
    _acalcAOS = acalcAOS;
}

void Onglets::setAcalcDN(bool acalcDN)
{
    _acalcDN = acalcDN;
}

void Onglets::setInfo(bool info)
{
    _info = info;
}

void Onglets::setDirDwn(const QString &dirDwn)
{
    _dirDwn = dirDwn;
}


/*
 * Methodes publiques
 */
/*
 * Affichage des donnees numeriques
 */
void Onglets::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (_date != nullptr) {
        delete _date;
        _date = nullptr;
    }
    _date = new Date(date, date.offsetUTC());

    /* Corps de la methode */
    // Affichage de la date
    AffichageDate();

    // Affichage des donnees du Soleil et de la Lune
    AffichageDonneesSoleilLune();

    if (Configuration::instance()->listeSatellites().isEmpty()) {

        _ui->satellite->setVisible(false);
        _ui->barreOnglets->removeTab(_ui->barreOnglets->indexOf(_ui->osculateurs));
        _ui->informations->removeWidget(_ui->infos);

        if (_nbInformations == 2) {
            _indexInfo = 0;
            _nbInformations = 1;
            _ui->informations->setCurrentIndex(_indexInfo);
            _ui->barreOnglets->setTabText(_ui->barreOnglets->indexOf(_ui->informationsSatellite),
                                          QCoreApplication::translate("Onglets", _titreInformations[0]));
            on_informations_currentChanged(_indexInfo);
            _ui->infoPrec->setVisible(false);
            _ui->infoSuiv->setVisible(false);
        }
    } else {

        if (_ui->barreOnglets->count() < _nbOnglets) {

            _nbInformations = 2;
            _ui->satellite->setVisible(true);
            _ui->barreOnglets->insertTab(1, _ui->osculateurs, tr("Éléments osculateurs"));
            _ui->informations->insertWidget(0, _ui->infos);
            on_infoSuiv_clicked();
            on_informations_currentChanged(0);
        }

        // Affichage des donnees relatives au satellite par defaut
        AffichageDonneesSatellite();

        // Affichage des elements osculateurs du satellite par defaut
        AffichageElementsOSculateurs();

        // Affichage des informations sur le satellite
        if (_info) {
            AffichageInformationsSatellite();
            _info = false;
        }
    }

#if defined (Q_OS_WIN)
    if (_ui->telescope->isVisible() && (getListItemChecked(_ui->liste4) > 0)) {
        CalculAosSatSuivi();
    }
#endif

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
 * Affichage de la date
 */
void Onglets::AffichageDate() const
{
    /* Declarations des variables locales */
    QString chaineUTC;
    Date date;

    /* Initialisations */

    /* Corps de la methode */
    if (fabs(_date->offsetUTC()) > EPSDBL100) {
        QTime heur(0, 0);
        heur = heur.addSecs(qRound(fabs(_date->offsetUTC()) * NB_SEC_PAR_JOUR));
        chaineUTC = tr("UTC %1 %2", "Universal Time Coordinated").arg((_date->offsetUTC() > 0.) ? "+" : "-").arg(heur.toString("hh:mm"));
        _ui->utcManuel->setText(chaineUTC);
        date = *_date;
    } else {
        chaineUTC = tr("UTC", "Universal Time Coordinated");
        _ui->utcManuel->setText(chaineUTC);
        date = Date(_date->jourJulienUTC(), 0., true);
    }

    const QString chaine = QString("%1  %2").arg(date.ToLongDate((_ui->syst12h->isChecked()) ? SYSTEME_12H : SYSTEME_24H)).arg(chaineUTC);
    _ui->dateHeure1->setText(chaine);
    _ui->dateHeure2->setText(chaine);

    /* Retour */
    return;
}

/*
 * Affichage des donnees relatives au satellite par defaut
 */
void Onglets::AffichageDonneesSatellite() const
{
    /* Declarations des variables locales */
    QString text;

    /* Initialisations */
    const QString fmt = "%1 %2";
    const QString unite = (_ui->unitesKm->isChecked()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile");
    const Satellite satellite = Configuration::instance()->listeSatellites().at(0);

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat1->setText(satellite.tle().nom());

    // Age du TLE
    QPalette palette;
    QBrush couleur;
    const double age = fabs(satellite.ageTLE());
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
    _ui->ageTLE->setPalette(palette);
    _ui->ageTLE->setText(fmt.arg(satellite.ageTLE(), 0, 'f', 2).arg(tr("jours")));

    // Longitude/Latitude/Altitude
    const QString ews = (satellite.longitude() >= 0.) ? tr("Ouest") : tr("Est");
    _ui->longitudeSat->setText(fmt.arg(Maths::ToSexagesimal(fabs(satellite.longitude()), DEGRE, 3, 0, false, true)).arg(ews));
    const QString nss = (satellite.latitude() >= 0.) ? tr("Nord") : tr("Sud");
    _ui->latitudeSat->setText(fmt.arg(Maths::ToSexagesimal(fabs(satellite.latitude()), DEGRE, 2, 0, false, true)).arg(nss));
    if (_ui->unitesKm->isChecked()) {
        _ui->altitudeSat->setText(text.asprintf("%.1f ", satellite.altitude()) + unite);
    } else {
        _ui->altitudeSat->setText(text.asprintf("%.1f ", satellite.altitude() * MILE_PAR_KM) + unite);
    }

    // Hauteur/Azimut/Distance
    _ui->hauteurSat->setText(Maths::ToSexagesimal(satellite.hauteur(), DEGRE, 2, 0, true, true));
    _ui->azimutSat->setText(Maths::ToSexagesimal(satellite.azimut(), DEGRE, 3, 0, false, true));
    if (_ui->unitesKm->isChecked()) {
        _ui->distanceSat->setText(text.asprintf("%.1f ", satellite.distance()) + unite);
    } else {
        _ui->distanceSat->setText(text.asprintf("%.1f ", satellite.distance() * MILE_PAR_KM) + unite);
    }

    // Ascension droite/declinaison/constellation
    _ui->ascensionDroiteSat->setText(Maths::ToSexagesimal(satellite.ascensionDroite(), HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonSat->setText(Maths::ToSexagesimal(satellite.declinaison(), DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationSat->setText(satellite.constellation());

    // Direction/vitesse/range rate
    _ui->directionSat->setText((satellite.vitesse().z() >= 0.) ? tr("Ascendant") : tr("Descendant"));
    AffichageVitesses();

    // Numero d'orbite
    _ui->nbOrbitesSat->setText(QString("%1").arg(satellite.nbOrbites()));

    // Magnitude/illumination
    const double fractionIlluminee = 100. * satellite.magnitude().fractionIlluminee();
    ConditionEclipse conditionEclipse = satellite.conditionEclipse();
    const QString corpsOccultant = (conditionEclipse.eclipseLune().luminosite < conditionEclipse.eclipseSoleil().luminosite) ? " " + tr("Lune") : "";
    if (satellite.tle().donnees().magnitudeStandard() > 98.) {

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
                // TODO

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
        if (_dateEclipse != nullptr) {
            delete _dateEclipse;
            _dateEclipse = nullptr;
        }
        _dateEclipse = new Date(Evenements::CalculOmbrePenombre(*_date, satellite, _ui->nombreTrajectoires->value(), _ui->eclipsesLune->isChecked(),
                                                                _ui->refractionPourEclipses->isChecked()), _date->offsetUTC());
        _acalcDN = false;
        _isEclipse = satellite.conditionEclipse().eclipseTotale();
    }

    double delai = _dateEclipse->jourJulienUTC() - _date->jourJulienUTC();
    if ((delai >= -EPS_DATES) && (_dateEclipse->jourJulienUTC() < satellite.traceAuSol().last().jourJulienUTC)) {

        QString transitionJN = tr("Prochain %1 :");
        _ui->lbl_prochainJN->setText(transitionJN.arg((satellite.conditionEclipse().eclipseTotale()) ?
                                                          tr("N>J", "Night to day") : tr("J>N", "Day to night")));
        _ui->lbl_prochainJN->setToolTip(satellite.conditionEclipse().eclipseTotale() ? tr("Nuit > Jour") : tr("Jour > Nuit"));

        // Delai de l'evenement
        transitionJN = tr("%1 (dans %2).", "Delay in hours, minutes or seconds");
        const Date delaiEcl = Date(delai - 0.5 + EPS_DATES, 0.);
        const QString cDelaiEcl = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                    delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5)
                    .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute")) :
                    delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5)
                    .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));

        _ui->dateJN->setText(transitionJN.arg(_dateEclipse->ToShortDate(FORMAT_COURT, ((_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H)))
                             .arg(cDelaiEcl));

        _ui->dateJN->adjustSize();
        _ui->dateJN->resize(_ui->dateJN->width(), 16);
        _ui->lbl_prochainJN->setVisible(true);
        _ui->dateJN->setVisible(true);
        _ui->magnitudeSat->setVisible(true);
        _ui->lbl_beta->setVisible(true);
        _ui->formLayoutWidget_19->setVisible(false);
    } else {
        _ui->lbl_prochainJN->setVisible(false);
        _ui->dateJN->setVisible(false);
        _ui->magnitudeSat->setVisible(false);
        _ui->lbl_beta->setVisible(false);
        _ui->formLayoutWidget_19->setVisible(true);
    }

    // Prochain AOS/LOS
    _acalcAOS = ((_htSat * satellite.hauteur()) <= 0.);
    if (_acalcAOS) {

        if (_elementsAOS != nullptr) {
            delete _elementsAOS;
            _elementsAOS = nullptr;
        }
        _elementsAOS = new ElementsAOS();
        *_elementsAOS = Evenements::CalculAOS(*_date, satellite, Configuration::instance()->observateur());
        _acalcAOS = false;
        _htSat = satellite.hauteur();
    }

    if (_elementsAOS->aos) {

        // Type d'evenement (AOS ou LOS)
        QString chaine = tr("Prochain %1 :").arg(_elementsAOS->typeAOS);
        _ui->lbl_prochainAOS->setText(chaine);
        _ui->lbl_prochainAOS->setToolTip((chaine.contains(tr("AOS"))) ? tr("Acquisition du signal") : tr("Perte du signal"));

        // Delai de l'evenement
        chaine = tr("%1 (dans %2).", "Delay in hours, minutes or seconds");
        Date dateAOS = Date(_elementsAOS->date, _date->offsetUTC());
        delai = dateAOS.jourJulienUTC() - _date->jourJulienUTC();
        const Date delaiAOS = Date(delai - 0.5 + EPS_DATES, 0.);
        const QString cDelaiAOS = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                    delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5)
                    .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute")) :
                    delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5)
                    .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));

        _ui->dateAOS->setText(chaine.arg(dateAOS.ToShortDate(FORMAT_COURT, ((_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).
                              arg(cDelaiAOS));
        _ui->lbl_azimut->setText(tr("Azimut : %1").arg(Maths::ToSexagesimal(_elementsAOS->azimut, DEGRE, 3, 0, false, true).mid(0, 9)));

        _ui->lbl_prochainAOS->setVisible(true);
        _ui->dateAOS->adjustSize();
        _ui->dateAOS->resize(_ui->dateAOS->width(), 16);
        _ui->dateAOS->setVisible(true);

    } else {
        _ui->lbl_prochainAOS->setVisible(false);
        _ui->dateAOS->setVisible(false);
    }

    // Angle beta
    const QString angleBeta = tr("Beta : %1", "Beta angle (angle between orbit plane and direction of Sun)");
    _ui->lbl_beta->setText(angleBeta.arg(Maths::ToSexagesimal(satellite.beta(), DEGRE, 3, 0, false, true).mid(0, 9)));
    _ui->lbl_beta2->setText(_ui->lbl_beta->text());

    /* Retour */
    return;
}


/*
 * Affichage des donnees du Soleil et de la Lune
 */
void Onglets::AffichageDonneesSoleilLune() const
{
    /* Declarations des variables locales */
    QString cond;

    /* Initialisations */
    const Soleil soleil = Configuration::instance()->soleil();
    const Lune lune = Configuration::instance()->lune();

    /* Corps de la methode */
    // Conditions d'observation
    const double ht = soleil.hauteur() * RAD2DEG;
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

    // Hauteur/azimut/distance du Soleil
    _ui->hauteurSoleil->setText(Maths::ToSexagesimal(soleil.hauteur(), DEGRE, 2, 0, true, true));
    _ui->azimutSoleil->setText(Maths::ToSexagesimal(soleil.azimut(), DEGRE, 3, 0, false, true));
    _ui->distanceSoleil->setText(tr("%1 UA").arg(soleil.distanceUA(), 0, 'f', 3));

    // Ascension droite/declinaison/constellation du Soleil
    _ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.ascensionDroite(), HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.declinaison(), DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationSoleil->setText(soleil.constellation());


    // Hauteur/azimut/distance de la Lune
    _ui->hauteurLune->setText(Maths::ToSexagesimal(lune.hauteur(), DEGRE, 2, 0, true, true));
    _ui->azimutLune->setText(Maths::ToSexagesimal(lune.azimut(), DEGRE, 3, 0, false, true));
    _ui->distanceLune->setText(QString("%1 %2").arg(lune.distance(), 0, 'f', 0)
                               .arg((_ui->unitesKm->isChecked()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile")));

    // Ascension droite/declinaison/constellation de la Lune
    _ui->ascensionDroiteLune->setText(Maths::ToSexagesimal(lune.ascensionDroite(), HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonLune->setText(Maths::ToSexagesimal(lune.declinaison(), DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationLune->setText(lune.constellation());

    // Phase/illumination/magnitude
    _ui->phaseLune->setText(lune.phase());
    _ui->illuminationLune->setText(QString("%1%").arg(lune.fractionIlluminee() * 100., 0, 'f', 0));
    _ui->magnitudeLune->setText(QString("%1").arg(lune.magnitude(), 0, 'f', 2));

    /* Retour */
    return;
}

/*
 * Affichage des elements osculateurs du satellite par defaut
 */
void Onglets::AffichageElementsOSculateurs() const
{
    /* Declarations des variables locales */
    QString text;
    Vecteur3D position;
    Vecteur3D vitesse;

    /* Initialisations */
    const Satellite satellite = Configuration::instance()->listeSatellites().at(0);
    const QString unite = (_ui->unitesKm->isChecked()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile");
    double demiGrandAxe = satellite.elements().demiGrandAxe();

    if (_ui->typeRepere->currentIndex() == 0) {
        position = satellite.position();
    } else {
        satellite.CalculPosVitECEF(*_date, position, vitesse);
    }

    if (_ui->unitesMi->isChecked()) {
        position *= MILE_PAR_KM;
        demiGrandAxe *= MILE_PAR_KM;
    }

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat2->setText(satellite.tle().nom());

    // Lignes du TLE
    _ui->ligne1->setText(satellite.tle().ligne1());
    _ui->ligne2->setText(satellite.tle().ligne2());

    // Position cartesienne
    _ui->xsat->setText(text.asprintf("%+.3f ", position.x()) + unite);
    _ui->ysat->setText(text.asprintf("%+.3f ", position.y()) + unite);
    _ui->zsat->setText(text.asprintf("%+.3f ", position.z()) + unite);

    // Elements osculateurs
    const QString fmt1 = "%1";
    const QString fmt2 = "%1°";
    const QString demiGdAxe = text.asprintf("%.1f ", demiGrandAxe) + unite;
    _ui->demiGrandAxeKeplerien->setText(demiGdAxe);
    _ui->demiGrandAxeCirc->setText(demiGdAxe);
    _ui->demiGrandAxeEquat->setText(demiGdAxe);
    _ui->demiGrandAxeCircEquat->setText(demiGdAxe);

    _ui->frameCirculaire->setVisible(false);
    _ui->frameCirculaireEquatorial->setVisible(false);
    _ui->frameEquatorial->setVisible(false);
    _ui->frameKeplerien->setVisible(false);

    switch (_ui->typeParametres->currentIndex()) {
    default:
    case 0:

        // Parametres kepleriens
        _ui->frameKeplerien->setVisible(true);
        _ui->excentricite->setText(fmt1.arg(satellite.elements().excentricite(), 0, 'f', 7));
        _ui->inclinaison->setText(fmt2.arg(satellite.elements().inclinaison() * RAD2DEG, 0, 'f', 4));
        _ui->ADNoeudAscendant->setText(fmt2.arg(satellite.elements().ascensionDroiteNoeudAscendant() * RAD2DEG, 0, 'f', 4));
        _ui->argumentPerigee->setText(fmt2.arg(satellite.elements().argumentPerigee() * RAD2DEG, 0, 'f', 4));
        _ui->anomalieMoyenne->setText(fmt2.arg(satellite.elements().anomalieMoyenne() * RAD2DEG, 0, 'f', 4));
        break;

    case 1:

        // Parametres circulaires
        _ui->frameCirculaire->setVisible(true);
        _ui->ex1->setText(fmt1.arg(satellite.elements().exCirc(), 0, 'f', 7));
        _ui->ey1->setText(fmt1.arg(satellite.elements().eyCirc(), 0, 'f', 7));
        _ui->inclinaison2->setText(fmt2.arg(satellite.elements().inclinaison() * RAD2DEG, 0, 'f', 4));
        _ui->ADNoeudAscendant2->setText(fmt2.arg(satellite.elements().ascensionDroiteNoeudAscendant() * RAD2DEG, 0, 'f', 4));
        _ui->positionSurOrbite->setText(fmt2.arg(satellite.elements().pso() * RAD2DEG, 0, 'f', 4));
        break;

    case 2:

        // Parametres equatoriaux
        _ui->frameEquatorial->setVisible(true);
        _ui->excentricite2->setText(fmt1.arg(satellite.elements().excentricite(), 0, 'f', 7));
        _ui->ix1->setText(fmt1.arg(satellite.elements().ix(), 0, 'f', 7));
        _ui->iy1->setText(fmt1.arg(satellite.elements().iy(), 0, 'f', 7));
        _ui->longitudePerigee->setText(fmt2.arg((satellite.elements().ascensionDroiteNoeudAscendant() +
                                                 satellite.elements().argumentPerigee()) * RAD2DEG, 0, 'f', 4));
        _ui->anomalieMoyenne2->setText(fmt2.arg(satellite.elements().anomalieMoyenne() * RAD2DEG, 0, 'f', 4));
        break;

    case 3:

        // Parametres circulaires equatoriaux
        _ui->frameCirculaireEquatorial->setVisible(true);
        _ui->ex2->setText(fmt1.arg(satellite.elements().exCEq(), 0, 'f', 7));
        _ui->ey2->setText(fmt1.arg(satellite.elements().eyCEq(), 0, 'f', 7));
        _ui->ix2->setText(fmt1.arg(satellite.elements().ix(), 0, 'f', 7));
        _ui->iy2->setText(fmt1.arg(satellite.elements().iy(), 0, 'f', 7));
        _ui->argumentLongitudeVraie2->setText(fmt2.arg(satellite.elements().argumentLongitudeVraie() * RAD2DEG, 0, 'f', 4));
        break;
    }

    // Autres elements osculateurs
    _ui->anomalieVraie->setText(fmt2.arg(satellite.elements().anomalieVraie() * RAD2DEG, 0, 'f', 4));
    _ui->anomalieExcentrique->setText(fmt2.arg(satellite.elements().anomalieExcentrique() * RAD2DEG, 0, 'f', 4));
    _ui->champDeVue->setText("±" + fmt2.arg(acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellite.altitude())) * RAD2DEG, 0, 'f', 2));

    // Apogee/perigee/periode orbitale
    const QString fmt3 = "%1 %2 (%3 %2)";
    const double apogee = (_ui->unitesKm->isChecked()) ? satellite.elements().apogee() : satellite.elements().apogee() * MILE_PAR_KM;
    double apogeeAlt = satellite.elements().apogee() - RAYON_TERRESTRE;
    apogeeAlt = (_ui->unitesKm->isChecked()) ? apogeeAlt : apogeeAlt * MILE_PAR_KM;
    _ui->apogee->setText(fmt3.arg(apogee, 0, 'f', 1).arg(unite).arg(apogeeAlt, 0, 'f', 1));

    const double perigee = (_ui->unitesKm->isChecked()) ? satellite.elements().perigee() : satellite.elements().perigee() * MILE_PAR_KM;
    double perigeeAlt = satellite.elements().perigee() - RAYON_TERRESTRE;
    perigeeAlt = (_ui->unitesKm->isChecked()) ? perigeeAlt : perigeeAlt * MILE_PAR_KM;
    _ui->perigee->setText(fmt3.arg(perigee, 0, 'f', 1).arg(unite).arg(perigeeAlt, 0, 'f', 1));
    _ui->periode->setText(Maths::ToSexagesimal(satellite.elements().periode() * HEUR2RAD, HEURE1, 1, 0, false, true));

    // Informations de signal
    _ui->doppler->setText(text.asprintf("%+.0f Hz", satellite.signal().doppler()));
    _ui->attenuation->setText(fmt1.arg(satellite.signal().attenuation(), 0, 'f', 2) + " dB");
    _ui->delai->setText(fmt1.arg(satellite.signal().delai(), 0, 'f', 2) + " ms");

    // Triplet de phasage
    const QString fmt4 = "[ %1; %2; %3 ] %4";
    const int nu0 = satellite.phasage().nu0();
    const int dt0 = satellite.phasage().dt0();
    const int ct0 = satellite.phasage().ct0();
    const int nbOrb = satellite.phasage().nbOrb();
    if ((nu0 < -98) || (dt0 < -98) || (ct0 < -98) || (nbOrb < -98)) {
        _ui->phasage->setText(tr("N/A", "Not applicable"));
    } else {
        _ui->phasage->setText(fmt4.arg(nu0).arg(dt0).arg(ct0).arg(nbOrb));
    }

    /* Retour */
    return;
}

/*
 * Affichage des groupes de TLE
 */
void Onglets::AffichageGroupesTLE() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const bool etat = _ui->groupeTLE->blockSignals(true);
    _ui->groupeTLE->clear();
    QList<CategorieTLE> &listeCategorieMajTLE = Configuration::instance()->listeCategoriesMajTLE();
    listeCategorieMajTLE.clear();

    /* Corps de la methode */
    QListIterator<CategorieTLE> it(Configuration::instance()->listeCategoriesTLE());
    while (it.hasNext()) {

        const CategorieTLE categorie = it.next();
        const QString nomCategorie = categorie.nom[Configuration::instance()->locale()];
        _ui->groupeTLE->addItem(nomCategorie);

#if !defined (Q_OS_MAC)
        if (settings.value("TLE/" + nomCategorie, 0).toInt() == 1) {
            listeCategorieMajTLE.append(categorie);
        }
#endif
    }

    _ui->groupeTLE->setCurrentIndex(settings.value("affichage/groupeTLE", 0).toInt());
    _ui->groupeTLE->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Affichage des informations sur le satellite
 */
void Onglets::AffichageInformationsSatellite() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt1 = "%1";
    const QString fmt2 = "%1°";
    const Satellite satellite = Configuration::instance()->listeSatellites().at(0);
    const TLE tle = satellite.tle();
    const Donnees donnee = tle.donnees();

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat3->setText(tle.nom());

    // Lignes du TLE
    _ui->line1->setText(tle.ligne1());
    _ui->line2->setText(tle.ligne2());


    // Numero NORAD
    _ui->norad->setText(tle.norad());

    // Designation COSPAR
    _ui->cospar->setText(tle.cospar());

    // Epoque du TLE
    _ui->epoque->setText(tle.epoque().ToShortDate(FORMAT_COURT, (_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H));

    // Coefficient pseudo-ballistique
    _ui->bstar->setText(fmt1.arg(tle.bstar(), 0, 'g', 6));


    // Moyen mouvement
    _ui->nbRev->setText(fmt1.arg(tle.no(), 0, 'f', 8));

    // Derivees du moyen mouvement
    _ui->nbRev2->setText(fmt1.arg(tle.ndt20(), 0, 'f', 8));
    _ui->nbRev3->setText(fmt1.arg(tle.ndd60(), 0, 'f', 8));

    // Nombre d'orbites a l'epoque
    _ui->nbOrbitesEpoque->setText(fmt1.arg(tle.nbOrbitesEpoque() + static_cast<unsigned int> (satellite.deltaNbOrb())));


    // Date de lancement
    _ui->dateLancement->setText(QDate::fromString(donnee.dateLancement(), "yyyy/MM/dd").toString(tr("yyyy/MM/dd")));

    // Categorie d'orbite
    _ui->categorieOrbite->setText(donnee.categorieOrbite());

    // Pays ou organisation
    _ui->pays->setText(donnee.pays());

    // Site de lancement
    _ui->siteLancement->setText(donnee.siteLancement());
    _ui->siteLancement->adjustSize();
    _ui->siteLancement->setFixedHeight(16);


    // Inclinaison moyenne
    _ui->inclinaisonMoy->setText(fmt2.arg(tle.inclo(), 0, 'f', 4));

    // Ascension droite moyenne du noeud ascendant
    _ui->ADNoeudAscendantMoy->setText(fmt2.arg(tle.omegao(), 0, 'f', 4));

    // Excentricite moyenne
    _ui->excentriciteMoy->setText(fmt1.arg(tle.ecco(), 0, 'f', 7));

    // Argument du perigee moyen
    _ui->argumentPerigeeMoy->setText(fmt2.arg(tle.argpo(), 0, 'f', 4));


    // Anomalie moyenne (moyenne)
    _ui->anomalieMoy->setText(fmt2.arg(tle.mo(), 0, 'f', 4));

    // Magnitude standard, methode de determination de la magnitude, magnitude maximale
    if (donnee.magnitudeStandard() > 98.) {
        _ui->magnitudeStdMax->setText("?/?");
    } else {

        QString text;
        const double magMax = donnee.magnitudeStandard() - 15.75
                + 5. * log10(1.45 * (satellite.elements().demiGrandAxe() * (1. - satellite.elements().excentricite()) - RAYON_TERRESTRE));
        _ui->magnitudeStdMax->setText(text.asprintf("%+.1f%c/%+.1f", donnee.magnitudeStandard(), donnee.methMagnitude(), magMax));
    }

    // Modele orbital
    _ui->modele->setText((satellite.method() == 'd') ? "SGP4 (DS)" : "SGP4 (NE)");
    _ui->modele->setToolTip((satellite.method() == 'd') ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));

    // Dimensions du satellite
    double t1 = donnee.t1();
    double t2 = donnee.t2();
    double t3 = donnee.t3();
    double section = donnee.section();
    const QString unite = (_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot");
    if (_ui->unitesMi->isChecked()) {
        t1 *= PIED_PAR_METRE;
        t2 *= PIED_PAR_METRE;
        t3 *= PIED_PAR_METRE;
        section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
    }

    QString dimensions;
    if ((fabs(t2) < EPSDBL100) && (fabs(t3) < EPSDBL100)) {
        const QString fmt3 = tr("Sphérique. R=%1 %2", "R = radius");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite);
    }
    if ((fabs(t2) >= EPSDBL100) && (fabs(t3) < EPSDBL100)) {
        const QString fmt3 = tr("Cylindrique. L=%1 %2, R=%3 %2", "L = height; R = radius");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite).arg(t2, 0, 'f', 1);
    }
    if ((fabs(t2) >= EPSDBL100) && (fabs(t3) >= EPSDBL100)) {
        const QString fmt3 = tr("Boîte. %1 x %2 x %3 %4");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1).arg(unite);
    }
    if (fabs(t1) < EPSDBL100) {
        dimensions = tr("Inconnues");
    }

    if (fabs(section) > EPSDBL100) {
        dimensions.append(" / %1 %2");
        dimensions = dimensions.arg(section, 0, 'f', 2).arg(unite);
        _ui->sq->setVisible(true);
    } else {
        _ui->sq->setVisible(false);
    }

    _ui->dimensions->setText(dimensions);

    /* Retour */
    return;
}

/*
 * Affichage des manoeuvres ISS
 */
void Onglets::AffichageManoeuvresISS() const
{
    /* Declarations des variables locales */
    QString masse;
    QString valeur;
    QString uniteMasse;
    QString uniteDist;
    QString uniteDv;

    /* Initialisations */
    int j = 0;
    _ui->manoeuvresISS->disconnect();
    _ui->manoeuvresISS->model()->removeRows(0, _ui->manoeuvresISS->rowCount());
    const QString annee = Configuration::instance()->dateDebutISS().split("-").at(0);

    if (_ui->unitesKm->isChecked()) {
        uniteDist = tr("km", "Kilometer");
        uniteDv = tr("m/s", "meter per second");
        uniteMasse = tr("kg", "Kilogram");
        masse = QString::number(Configuration::instance()->masseISS().at(0), 'f', 2);

    } else {
        uniteDist = tr("nmi", "nautical mile");
        uniteDv = tr("ft/s", "foot per second");
        uniteMasse = tr("lb", "pound");
        masse = QString::number(Configuration::instance()->masseISS().at(0) / KG_PAR_LIVRE, 'f', 2);
    }

    /* Corps de la methode */
    QTableWidgetItem * itemEvt;
    QTableWidgetItem * itemDate;
    QTableWidgetItem * itemMasse;
    QTableWidgetItem * item;

    QStringListIterator it(Configuration::instance()->evenementsISS());
    while (it.hasNext()) {

        const QString evt = it.next();
        const QString intitule = evt.mid(0, 23).trimmed();
        const QStringList details = evt.mid(23).split(" ", Qt::SkipEmptyParts);

        const QString dateEvt = Date::ConversionDateNasa(annee + "-" + details.at(0))
                .ToShortDateAMJ(FORMAT_MILLISEC, (_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H);

        // Ajout d'une ligne dans le tableau
        _ui->manoeuvresISS->insertRow(j);
        _ui->manoeuvresISS->setRowHeight(j, 16);

        // Intitule de l'evenement
        itemEvt = new QTableWidgetItem(intitule);
        itemEvt->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemEvt->setFlags(itemEvt->flags() & ~Qt::ItemIsEditable);
        itemEvt->setToolTip(intitule);
        _ui->manoeuvresISS->setItem(j, 0, itemEvt);

        // Date
        itemDate = new QTableWidgetItem(dateEvt);
        itemDate->setTextAlignment(Qt::AlignCenter);
        itemDate->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
        _ui->manoeuvresISS->setItem(j, 1, itemDate);

        // Masse
        itemMasse = new QTableWidgetItem(masse);
        itemMasse->setTextAlignment(Qt::AlignCenter);
        itemMasse->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
        itemMasse->setToolTip(uniteMasse);
        _ui->manoeuvresISS->setItem(j, 2, itemMasse);

        for(int k=1; k<details.count(); k++) {

            valeur = details.at(k);
            if (k == 1) {
                if (_ui->unitesMi->isChecked()) {
                    valeur = QString::number(details.at(k).toDouble() * PIED_PAR_METRE, 'f', 1);
                }
            } else {
                if (_ui->unitesMi->isChecked()) {
                    valeur = QString::number(details.at(k).toDouble() * MILE_PAR_KM, 'f', 1);
                }
            }

            item = new QTableWidgetItem(valeur);
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setToolTip((k == 1) ? uniteDv : uniteDist);
            _ui->manoeuvresISS->setItem(j, k + 2, item);
        }
    }

    _ui->manoeuvresISS->setStyleSheet("QHeaderView::section {" \
            "background-color:rgb(235, 235, 235);" \
            "border-top: 0px solid grey;" \
            "border-bottom: 1px solid grey;" \
            "border-right: 1px solid grey;" \
            "font-size: 12px;" \
            "font-weight: 600 }");

    _ui->manoeuvresISS->horizontalHeader()->setStretchLastSection(true);
    _ui->manoeuvresISS->setColumnWidth(0, 75);
    _ui->manoeuvresISS->setColumnWidth(1, 135);
    _ui->manoeuvresISS->setColumnWidth(2, 60);
    _ui->manoeuvresISS->setColumnWidth(3, 40);
    _ui->manoeuvresISS->setColumnWidth(4, 50);
    _ui->manoeuvresISS->setColumnWidth(5, 50);
    _ui->manoeuvresISS->sortItems(1);
    _ui->manoeuvresISS->setVisible(true);

    /* Retour */
    return;
}

/*
 * Affichage des elements de la liste deroulante pour l'affichage des messages lors de la mise a jour des TLE
 */
void Onglets::AffichageMessagesMaj()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const bool etat = _ui->affichageMsgMAJ->blockSignals(true);
    _ui->affichageMsgMAJ->clear();
    _ui->affichageMsgMAJ->addItem(tr("Affichage des messages informatifs"));
    _ui->affichageMsgMAJ->addItem(tr("Accepter ajout/suppression de TLE"));
    _ui->affichageMsgMAJ->addItem(tr("Refuser ajout/suppression de TLE"));
    _ui->affichageMsgMAJ->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Affichage des resultats de la recherche des donnees satellite
 */
void Onglets::AffichageResultatsDonnees() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->satellitesTrouves->clear();
    _ui->frameResultats->setVisible(false);
    _ui->lbl_fichiersTle->setVisible(false);
    _ui->fichiersTle->setVisible(false);

    /* Corps de la methode */
    if (_resultatsSatellitesTrouves.isEmpty()) {
        _ui->lbl_satellitesTrouves->setText(tr("Objets trouvés :"));
    } else {

        const QString chaine = tr("Objets trouvés (%1) :");
        _ui->lbl_satellitesTrouves->setText(chaine.arg(_resultatsSatellitesTrouves.count()));

        // Remplissage de la liste de resultats
        QString nomsat;
        QStringListIterator it(_resultatsSatellitesTrouves);
        while (it.hasNext()) {

            const QString item = it.next().toUpper();
            nomsat = item.mid(124).trimmed();

            if (nomsat.isEmpty()) {
                nomsat = item.mid(0, 6);
            }

            _ui->satellitesTrouves->addItem(nomsat);
        }
        _ui->satellitesTrouves->setCurrentRow(0);
        _ui->frameResultats->setVisible(true);
    }

    /* Retour */
    return;
}

/*
 * Affichage des vitesses (par seconde ou par heure)
 */
void Onglets::AffichageVitesses() const
{
    /* Declarations des variables locales */
    QString text;
    QString unite;
    Vecteur3D vitesse;

    /* Initialisations */
    const Satellite satellite = Configuration::instance()->listeSatellites().at(0);
    if (_ui->typeRepere->currentIndex() == 0) {
        vitesse = satellite.vitesse();
    } else {
        Vecteur3D position;
        satellite.CalculPosVitECEF(*_date, position, vitesse);
    }

    double rangeRate = satellite.rangeRate();

    if (_ui->unitesKm->isChecked()) {
        unite = (_uniteVitesse) ? tr("km/h", "Kilometer per hour") : tr("km/s", "Kilometer per second");
    } else {
        unite = (_uniteVitesse) ? tr("kn", "Knot") : tr("nmi/s", "Nautical mile per second");
        vitesse *= MILE_PAR_KM;
        rangeRate *= MILE_PAR_KM;
    }

    /* Corps de la methode */
    if (_uniteVitesse) {

        vitesse *= NB_SEC_PAR_HEUR;

        _ui->vitesseSat->setText(text.asprintf("%.0f ", vitesse.Norme()) + unite);
        _ui->rangeRate->setText(text.asprintf("%+.0f ", rangeRate * NB_SEC_PAR_HEUR) + unite);

        // Vitesse cartesienne
        _ui->vxsat->setText(text.asprintf("%+.3f ", vitesse.x()) + unite);
        _ui->vysat->setText(text.asprintf("%+.3f ", vitesse.y()) + unite);
        _ui->vzsat->setText(text.asprintf("%+.3f ", vitesse.z()) + unite);

    } else {
        _ui->vitesseSat->setText(text.asprintf("%.3f ", vitesse.Norme()) + unite);
        _ui->rangeRate->setText(text.asprintf("%+.3f ", rangeRate) + unite);

        // Vitesse cartesienne
        _ui->vxsat->setText(text.asprintf("%+.6f ", vitesse.x()) + unite);
        _ui->vysat->setText(text.asprintf("%+.6f ", vitesse.y()) + unite);
        _ui->vzsat->setText(text.asprintf("%+.6f ", vitesse.z()) + unite);
    }

    /* Retour */
    return;
}

/*
 * Affichage des donnees du lieu selectionne lorsqu'on clique sur son nom
 */
void Onglets::AfficherLieuSelectionne(const int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString nomlieu = "";

    /* Corps de la methode */

    /* Retour */
    return;
}

/*
 * Calcul de l'age des TLE de l'ISS pour transits ISS
 */
void Onglets::CalculAgeTLETransitISS()
{
    /* Declarations des variables locales */
    double age1;

    /* Initialisations */
    double ageISS = 0.;
    const double jjcour = _date->jourJulienUTC();

    QString fichier = Configuration::instance()->dirTle() + QDir::separator() + "iss.3le";
    const int nbt = TLE::VerifieFichier(fichier, false);

    /* Corps de la methode */
    if (nbt == 0) {

        fichier = Configuration::instance()->dirTle() + QDir::separator() + "visual.txt";
        const int nbt2 = TLE::VerifieFichier(fichier, false);

        if (nbt2 > 0) {
            const QStringList listeSat(QStringList() << Configuration::instance()->noradStationSpatiale());
            const QMap<QString, TLE> mapTle = TLE::LectureFichier(fichier, listeSat, false);

            age1 = fabs(mapTle.first().epoque().jourJulienUTC() - jjcour);
            ageISS = age1;
        }

    } else {

        // Calcul de l'age du TLE de l'ISS
        const QList<TLE> tabTle = TLE::LectureFichier3le(fichier);

        age1 = tabTle.first().epoque().jourJulienUTC() - jjcour;
        const double age2 = jjcour - tabTle.last().epoque().jourJulienUTC();
        ageISS = qMax(age1, age2);
    }

    if (ageISS > 0) {

        QPalette paletteTLE;
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);

        // Indicateur de l'age du TLE
        const double ageTleIss = fabs(ageISS);
        if (ageTleIss > (_ui->ageMaxTLETransit->value() + 8.)) {
            brush.setColor(Qt::red);
        } else if (ageTleIss > (_ui->ageMaxTLETransit->value() + 4.)) {
            brush.setColor(QColor("orange"));
        } else if (ageTleIss > _ui->ageMaxTLETransit->value()) {
            brush.setColor(Qt::darkYellow);
        } else {
            brush.setColor(QColor("forestgreen"));
        }

        paletteTLE.setBrush(QPalette::WindowText, brush);
        _ui->ageTLETransit->setPalette(paletteTLE);

        if (nbt == 0) {
            _ui->lbl_ageTLETransit->setText(tr("Age du TLE :"));
        } else {
            if (fabs(ageISS - age1) < EPSDBL100) {
                _ui->lbl_ageTLETransit->setText(tr("Age du premier TLE :"));
            } else {
                _ui->lbl_ageTLETransit->setText(tr("Age du dernier TLE :"));
            }
        }

        _ui->ageTLETransit->setText(QString(tr("%1 jours")).arg(ageISS, 0, 'f', 2));
        _ui->ageTLETransit->setVisible(true);
        _ui->lbl_ageTLETransit->setVisible(true);
    } else {
        _ui->ageTLETransit->setVisible(false);
        _ui->lbl_ageTLETransit->setVisible(false);
    }

    /* Retour */
    return;
}

#if defined (Q_OS_WIN)
/*
 * Calcul de la hauteur maximale d'un satellite dans le ciel
 */
QPair<double, double> Onglets::CalculHauteurMax(const QList<double> &jjm, Observateur &obs, Satellite &satSuivi) const
{
    /* Declarations des variables locales */
    QList<double> ht;

    /* Initialisations */

    /* Corps de la methode */
    for (int i=0; i<3; i++) {

        const Date date(jjm[i], 0., false);

        obs.CalculPosVit(date);

        // Position du satellite
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(obs);

        // Hauteur
        ht.append(satSuivi.hauteur());
    }

    /* Retour */
    return Maths::CalculExtremumInterpolation3(jjm, ht);;
}
#endif

/*
 * Chargement des preferences
 */
void Onglets::ChargementPref() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomPref = Configuration::instance()->dirPrf() + QDir::separator() +
            Configuration::instance()->listeFicPref().at(_ui->preferences->currentIndex());

    /* Corps de la methode */
    QFile fichier(nomPref);
    if (fichier.exists() && (fichier.size() != 0)) {

        // Lecture du fichier de preferences
        fichier.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichier);
        const QStringList listePrf = flux.readAll().split("\n", QString::SkipEmptyParts);
        fichier.close();

        QStringListIterator it(listePrf);
        while (it.hasNext()) {

            const QStringList item = it.next().split(" ", QString::SkipEmptyParts);

            if (item.at(1) == "true") {
                settings.setValue(item.at(0), true);

            } else if (item.at(1) == "false") {
                settings.setValue(item.at(0), false);

            } else {
                if (item.at(0) == "affichage/magnitudeEtoiles") {
                    settings.setValue(item.at(0), item.at(1).toDouble());

                } else if ((item.at(0) == "affichage/affconst") || (item.at(0) == "affichage/affnomlieu") || (item.at(0) == "affichage/affnomsat") ||
                           (item.at(0) == "affichage/affplanetes") || (item.at(0) == "affichage/affradar") || (item.at(0) == "affichage/affvisib") ||
                           (item.at(0) == "affichage/intensiteOmbre") || (item.at(0) == "affichage/intensiteVision")) {
                    settings.setValue(item.at(0), item.at(1).toUInt());
                } else {
                }
            }
        }

        _ui->affconst->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affconst", Qt::Checked).toUInt()));
        _ui->affcoord->setChecked(settings.value("affichage/affcoord", true).toBool());
        _ui->affetoiles->setChecked(settings.value("affichage/affetoiles", true).toBool());
        _ui->affgrille->setChecked(settings.value("affichage/affgrille", true).toBool());
        _ui->afficone->setChecked(settings.value("affichage/afficone", true).toBool());
        _ui->affinvew->setChecked(settings.value("affichage/affinvew", false).toBool());
        _ui->affinvns->setChecked(settings.value("affichage/affinvns", false).toBool());
        _ui->afflune->setChecked(settings.value("affichage/afflune", true).toBool());
        _ui->affnomlieu->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomlieu", Qt::Checked).toUInt()));
        _ui->affnomsat->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomsat", Qt::Checked).toUInt()));
        _ui->affnotif->setChecked(settings.value("affichage/affnotif", true).toBool());
        _ui->affnuit->setChecked(settings.value("affichage/affnuit", true).toBool());
        _ui->affphaselune->setChecked(settings.value("affichage/affphaselune", true).toBool());
        _ui->affplanetes->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affplanetes", Qt::Checked).toUInt()));
        _ui->affradar->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affradar", Qt::Checked).toUInt()));
        _ui->affSAA->setChecked(settings.value("affichage/affSAA", false).toBool());
        _ui->affsoleil->setChecked(settings.value("affichage/affsoleil", true).toBool());
        _ui->afftraceCiel->setChecked(settings.value("affichage/afftraceCiel", true).toBool());
        _ui->afftraj->setChecked(settings.value("affichage/afftraj", true).toBool());
        _ui->affvisib->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affvisib", Qt::Checked).toUInt()));
        _ui->calJulien->setChecked(settings.value("affichage/calJulien", false).toBool());
        _ui->extinctionAtmospherique->setChecked(settings.value("affichage/extinction", true).toBool());
        _ui->refractionPourEclipses->setChecked(settings.value("affichage/refractionPourEclipses", true).toBool());
        _ui->effetEclipsesMagnitude->setChecked(settings.value("affichage/effetEclipsesMagnitude", true).toBool());
        _ui->eclipsesLune->setChecked(settings.value("affichage/eclipsesLune", true).toBool());
        _ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 30).toInt());
        _ui->intensiteVision->setValue(settings.value("affichage/intensiteVision", 50).toInt());
        _ui->langue->setCurrentIndex(Configuration::instance()->listeFicLang().indexOf(settings.value("affichage/langue", "en").toString()));
        _ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
        _ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 2).toInt());
        _ui->proportionsCarte->setChecked(settings.value("affichage/proportionsCarte", true).toBool());
        _ui->rotationIconeISS->setChecked(settings.value("affichage/rotationIconeISS", true).toBool());
        _ui->rotationLune->setChecked(settings.value("affichage/rotationLune", false).toBool());
        _ui->utcAuto->setChecked(settings.value("affichage/utcAuto", true).toBool());
        _ui->typeRepere->setCurrentIndex(settings.value("affichage/typeRepere", 0).toInt());
        _ui->typeParametres->setCurrentIndex(settings.value("affichage/typeParametres", 0).toInt());
        _ui->affichageMsgMAJ->setCurrentIndex(settings.value("fichier/affichageMsgMAJ", 1).toInt());
        _ui->verifMAJ->setChecked(settings.value("affichage/verifMAJ", false).toBool());
    }

    /* Retour */
    return;
}

/*
 * Ecriture du compte-rendu de mise a jour des TLE
 */
bool Onglets::EcritureCompteRenduMaj(const QStringList &compteRendu, QPlainTextEdit * compteRenduMaj)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const int nbsup = compteRendu.at(compteRendu.count()-1).toInt();
    const int nbadd = compteRendu.at(compteRendu.count()-2).toInt();
    const int nbold = compteRendu.at(compteRendu.count()-3).toInt();
    const int nbmaj = compteRendu.at(compteRendu.count()-4).toInt();
    const QString fic = compteRendu.at(compteRendu.count()-5);

    if (!compteRenduMaj->toPlainText().isEmpty()) {
        if (!compteRenduMaj->toPlainText().split("\n").last().trimmed().isEmpty()) {
            compteRenduMaj->appendPlainText("");
        }
    }

    compteRenduMaj->appendPlainText(QString(tr("Fichier %1 :").arg(fic)));

    QString msgcpt;
    if ((nbmaj < nbold) && (nbmaj > 0)) {

        msgcpt = tr("TLE du satellite %1 (%2) non réactualisé");

        for(int i=0; i<compteRendu.count()-5; i++) {
            const QString nomsat = compteRendu.at(i).split("#").at(0);
            const QString norad = compteRendu.at(i).split("#").at(1);
            compteRenduMaj->appendPlainText(msgcpt.arg(nomsat).arg(norad));
        }
    }

    if (nbsup > 0) {
        msgcpt = tr("Nombre de TLE(s) supprimés : %1");
        compteRenduMaj->appendPlainText(msgcpt.arg(nbsup));
    }

    if (nbadd > 0) {
        msgcpt = tr("Nombre de TLE(s) ajoutés : %1");
        compteRenduMaj->appendPlainText(msgcpt.arg(nbadd));
    }

    if ((nbmaj < nbold) && (nbmaj > 0)) {
        msgcpt = tr("%1 TLE(s) sur %2 mis à jour");
        compteRenduMaj->appendPlainText(msgcpt.arg(nbmaj).arg(nbold));
    }

    if ((nbmaj == nbold) && (nbold != 0)) {
        msgcpt = tr("Mise à jour de tous les TLE effectuée (fichier de %1 satellite(s))");
        compteRenduMaj->appendPlainText(msgcpt.arg(nbold));
    }

    if ((nbmaj == 0) && (nbold != 0) && (nbadd == 0) && (nbsup == 0)) {
        compteRenduMaj->appendPlainText(tr("Aucun TLE mis à jour"));
    }

    compteRenduMaj->appendPlainText("");
    compteRenduMaj->verticalScrollBar()->setValue(compteRenduMaj->blockCount());

    /* Retour */
    return ((nbold > 0) && ((nbmaj > 0) || (nbsup > 0) || (nbadd > 0)));
}

/*
 * Ecriture des informations d'eclipse dans l'onglet General
 */
void Onglets::EcritureInformationsEclipse(const QString &corpsOccultant, const double fractionIlluminee) const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const ConditionEclipse conditionEclipse = Configuration::instance()->listeSatellites().at(0).conditionEclipse();

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

int Onglets::getListItemChecked(const QListWidget * const liste) const
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
 * Affichage du lieu d'observation
 */
void Onglets::AffichageLieuObs() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->lieuxObservation1->clear();
    _ui->lieuxObservation2->clear();
    _ui->lieuxObservation3->clear();
    _ui->lieuxObservation4->clear();
    _ui->lieuxObservation5->clear();
    _ui->selecLieux->clear();

    QListIterator<Observateur> it(Configuration::instance()->observateurs());
    bool premier = true;
    while (it.hasNext()) {

        const Observateur obs = it.next();
        const QString nomlieu = obs.nomlieu();

        _ui->lieuxObservation1->addItem(nomlieu);
        _ui->lieuxObservation2->addItem(nomlieu);
        _ui->lieuxObservation3->addItem(nomlieu);
        _ui->lieuxObservation4->addItem(nomlieu);
        _ui->lieuxObservation5->addItem(nomlieu);
        _ui->selecLieux->addItem(nomlieu);

        if (premier) {

            // Longitude/Latitude/Altitude
            const double lo = obs.longitude();
            const double la = obs.latitude();
            const double atd = obs.altitude() * 1000.;

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");

            // Affichage des coordonnees
            const QString fmt = "%1 %2";
            _ui->longitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(lo), DEGRE, 3, 0, false, true)).arg(ew));
            _ui->latitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(la), DEGRE, 2, 0,false, true)).arg(ns));
            _ui->altitudeObs->setText(fmt.arg((_ui->unitesKm->isChecked()) ? atd : qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd))).
                                      arg((_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot")));
            premier = false;
        }
    }

    _ui->lieuxObservation1->setCurrentIndex(0);
    _ui->lieuxObservation2->setCurrentIndex(0);
    _ui->lieuxObservation3->setCurrentIndex(0);
    _ui->lieuxObservation4->setCurrentIndex(0);
    _ui->lieuxObservation5->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Calcul des informations AOS/LOS pour le suivi d'un satellite
 */
#if defined (Q_OS_WIN)
void Onglets::CalculAosSatSuivi() const
{
    /* Declarations des variables locales */
    double azim;
    double delai;
    QFont bld;
    ConditionEclipse condEcl;

    /* Initialisations */

    /* Corps de la methode */
    try {

        Date date(_date->offsetUTC());
        Observateur obs = Configuration::instance()->observateurs().at(_ui->lieuxObservation5->currentIndex());

        if (_ui->liste4->count() == 0) {
            throw PreviSatException();
        }

        const int nsat = getListItemChecked(_ui->liste4);
        if ((nsat == 0) && (_ui->liste4->count() > 0) && _ui->liste4->isVisible()) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), WARNING);
        }

        _ui->frameSatSelectionne->setVisible(true);

        QStringList satelliteSelectionne;
        for (int i=0; i<_ui->liste4->count(); i++) {
            if (_ui->liste4->item(i)->checkState() == Qt::Checked) {
                satelliteSelectionne.append(_ui->liste4->item(i)->data(Qt::UserRole).toString());
            }
        }

        // Position de l'observateur
        obs.CalculPosVit(date);

        // Position du satellite
        const QMap<QString, TLE> tle = TLE::LectureFichier(Configuration::instance()->nomfic(), satelliteSelectionne);
        _ui->nomsatSuivi->setText(tle.first().nom());

        Satellite satSuivi(tle.first());
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(obs);
        satSuivi.CalculElementsOsculateurs(date);

        Soleil sol;
        sol.CalculPosition(date);

        Lune lun;
        lun.CalculPosition(date);

        condEcl.CalculSatelliteEclipse(satSuivi.position(), sol, lun, true);
        const QString ecl = (condEcl.eclipseTotale()) ? tr("Satellite en éclipse") : tr("Satellite éclairé");

        // Hauteur minimale du satellite
        const double hauteurMin = DEG2RAD * ((_ui->hauteurSatSuivi->currentIndex() == 5) ?
                                                 abs(_ui->valHauteurSatSuivi->text().toInt()) : 5 * _ui->hauteurSatSuivi->currentIndex());

        // Date de lever
        const ElementsAOS elemAos = Evenements::CalculAOS(date, satSuivi, obs, true, hauteurMin);
        Date dateAosSuivi(elemAos.date.jourJulienUTC(), _date->offsetUTC());
        if (elemAos.aos) {

            Date dateLosSuivi;
            const QString chaine = tr("%1 (dans %2). Azimut : %3", "Delay in hour, minutes, seconds");

            if (elemAos.typeAOS == tr("AOS", "Acquisition of signal")) {

                const ElementsAOS elemLos = Evenements::CalculAOS(Date(dateAosSuivi.jourJulienUTC() + 10. * NB_JOUR_PAR_SEC, _date->offsetUTC()),
                                                                  satSuivi, obs, true, hauteurMin);

                // Date de coucher
                dateLosSuivi = Date(elemLos.date, _date->offsetUTC());
                azim = elemLos.azimut;

                // Lever
                delai = dateAosSuivi.jourJulienUTC() - date.jourJulienUTC();
                const Date delaiAOS = Date(delai - 0.5 + EPS_DATES, 0.);
                const QString cDelaiAOS = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                            delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5)
                            .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute")) :
                            delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5)
                            .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));

                _ui->leverSatSuivi->setText(
                            chaine.arg(dateAosSuivi.ToShortDate(FORMAT_COURT, ((_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).
                            arg(cDelaiAOS).arg(Maths::ToSexagesimal(elemAos.azimut, DEGRE, 3, 0, false, true).mid(0, 9)));

                _ui->lbl_leverSatSuivi->setVisible(true);
                bld.setBold(false);
                _ui->leverSatSuivi->setFont(bld);
                _ui->leverSatSuivi->move(_ui->hauteurMaxSatSuivi->x(), _ui->leverSatSuivi->y());

            } else {

                // Le satellite est deja dans le ciel
                satSuivi.CalculElementsOsculateurs(date);

                dateLosSuivi = dateAosSuivi;

                const ElementsAOS elem = Evenements::CalculAOS(dateLosSuivi, satSuivi, obs, false, hauteurMin);

                dateAosSuivi = elem.date;
                azim = elemAos.azimut;

                const QString chaine2 = tr("Satellite dans le ciel. Hauteur actuelle : %1. Azimut : %2. %3");
                _ui->leverSatSuivi->setText(chaine2.arg(Maths::ToSexagesimal(satSuivi.hauteur(), DEGRE, 2, 0, false, false).mid(0, 7).trimmed())
                                            .arg(Maths::ToSexagesimal(satSuivi.azimut(), DEGRE, 3, 0, false, true).mid(0, 9)).arg(ecl));

                bld.setBold(true);
                _ui->leverSatSuivi->setFont(bld);
                _ui->lbl_leverSatSuivi->setVisible(false);
                _ui->leverSatSuivi->move(0, _ui->leverSatSuivi->y());
            }

            // Coucher
            delai = dateLosSuivi.jourJulienUTC() - date.jourJulienUTC();
            const Date delaiLOS = Date(delai - 0.5 + EPS_DATES, 0.);
            const QString cDelaiLOS = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                        delaiLOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5)
                        .replace(":", tr("h", "hour").append(" ")).append(tr("min", "minute")) :
                        delaiLOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5)
                        .replace(":", tr("min", "minute").append(" ")).append(tr("s", "second"));

            _ui->coucherSatSuivi->setText(
                        chaine.arg(dateLosSuivi.ToShortDate(FORMAT_COURT, ((_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).
                        arg(cDelaiLOS).arg(Maths::ToSexagesimal(azim, DEGRE, 3, 0, false, true).mid(0, 9)));

            // Hauteur max
            QList<double> jjm;
            QPair<double, double> minmax;
            double jj0 = 0.5 * (dateAosSuivi.jourJulienUTC() + dateLosSuivi.jourJulienUTC());
            double pas = dateLosSuivi.jourJulienUTC() - jj0;

            jjm.append(jj0 - pas);
            jjm.append(jj0);
            jjm.append(jj0 + pas);

            minmax = CalculHauteurMax(jjm, obs, satSuivi);
            pas *= 0.5;

            for(int i=0; i<4; i++) {
                jjm[0] = minmax.first - pas;
                jjm[1] = minmax.first;
                jjm[2] = minmax.first + pas;

                minmax = CalculHauteurMax(jjm, obs, satSuivi);
                pas *= 0.5;
            }
            _ui->hauteurMaxSatSuivi->setText(QString("%1").arg(Maths::ToSexagesimal(minmax.second, DEGRE, 2, 0, false, false).mid(0, 7).trimmed()));

            _ui->lbl_hauteurMaxSatSuivi->setVisible(true);
            _ui->lbl_coucherSatSuivi->setVisible(true);
            _ui->hauteurMaxSatSuivi->setVisible(true);
            _ui->coucherSatSuivi->setVisible(true);
            _ui->visibiliteSatSuivi->setVisible(false);
            _ui->frameSatSuivi->setVisible(true);

        } else {

            // Cas des satellites geostationnaires (10 minutes de suivi)
            if (satSuivi.hauteur() > 0.) {
                dateAosSuivi = date;

                const QString chaine2 = tr("Satellite dans le ciel. Hauteur actuelle : %1. Azimut : %2. %3");
                _ui->leverSatSuivi->setText(chaine2.arg(Maths::ToSexagesimal(satSuivi.hauteur(), DEGRE, 2, 0, false, false).mid(0, 7).trimmed())
                                            .arg(Maths::ToSexagesimal(satSuivi.azimut(), DEGRE, 3, 0, false, true).mid(0, 9)).arg(ecl));

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
#endif

/*
 * Mettre a jour un groupe de TLE
 */
void Onglets::MettreAJourGroupeTLE(const QString &groupe)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _dirDwn = Configuration::instance()->dirTle();
    _ui->majMaintenant->setEnabled(false);
    _ui->compteRenduMaj->clear();
    connect(this, SIGNAL(TelechargementFini()), this, SLOT(FinTelechargementTle()));

    /* Corps de la methode */
    QListIterator<CategorieTLE> it1(Configuration::instance()->listeCategoriesTLE());
    while (it1.hasNext()) {

        const CategorieTLE categorie = it1.next();
        if (categorie.nom[Configuration::instance()->locale()].toLower().contains(groupe)) {

            const QString adresse = (categorie.site.contains("celestrak")) ?
                        Configuration::instance()->adresseCelestrakNorad() : Configuration::instance()->adresseAstropedia() + "previsat/tle/";

            QStringListIterator it2(categorie.fichiers);
            while (it2.hasNext()) {
                AjoutFichier(QUrl(adresse + it2.next()));
            }

            if (_listeFichiersTelechargement.isEmpty()) {
                QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
            }
        }
    }

    /* Retour */
    return;
}

/*
 * Rechargement des listes suite a un changement de langue
 */
void Onglets::RechargerListes()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    InitFicMap(false);
    InitFicObs(false);
    InitFicPref(false);
    InitFicSon();
    AffichageGroupesTLE();
    AffichageLieuObs();
    AffichageMessagesMaj();

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet Elements osculateurs
 */
void Onglets::SauveOngletElementsOsculateurs(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

#if (BUILD_TEST == false)
        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
                arg(QString(APP_ANNEES_DEV)) << endl << endl << endl;
#endif
        flux << tr("Date :", "Date and hour") << " " << _ui->dateHeure2->text() << endl << endl;

        // Donnees sur le satellite
        flux << tr("Nom du satellite :") + " " + _ui->nomsat2->text() << endl;
        flux << _ui->ligne1->text() << endl;
        flux << _ui->ligne2->text() << endl << endl;

        flux << tr("Vecteur d'état") << " (" << _ui->typeRepere->currentText() << ") :" << endl;
        QString chaine = tr("x : %1\tvx : %2", "Position, velocity");
        flux << chaine.arg(_ui->xsat->text().rightJustified(13, ' ')).arg(_ui->vxsat->text().rightJustified(15, ' ')) << endl;

        chaine = tr("y : %1\tvy : %2", "Position, velocity");
        flux << chaine.arg(_ui->ysat->text().rightJustified(13, ' ')).arg(_ui->vysat->text().rightJustified(15, ' ')) << endl;

        chaine = tr("z : %1\tvz : %2", "Position, velocity");
        flux << chaine.arg(_ui->zsat->text().rightJustified(13, ' ')).arg(_ui->vzsat->text().rightJustified(15, ' ')) << endl << endl;

        flux << tr("Éléments osculateurs :") << endl;
        switch (_ui->typeParametres->currentIndex()) {

        case 0:
            // Parametres kepleriens
            chaine = tr("Demi-grand axe       : %1\tAscension droite du noeud ascendant : %2");
            flux << chaine.arg(_ui->demiGrandAxeKeplerien->text()).arg(_ui->ADNoeudAscendant->text().rightJustified(9, '0')) << endl;

            chaine = tr("Excentricité         : %1\tArgument du périgée                 : %2");
            flux << chaine.arg(_ui->excentricite->text()).arg(_ui->argumentPerigee->text().rightJustified(9, '0')) << endl;

            chaine = tr("Inclinaison          : %1\tAnomalie moyenne                    : %2");
            flux << chaine.arg(_ui->inclinaison->text().rightJustified(9, '0')).arg(_ui->anomalieMoyenne->text().rightJustified(9, '0'))
                 << endl << endl;
            break;

        case 1:
            // Parametres circulaires
            chaine = tr("Demi-grand axe       : %1\tAscension droite du noeud ascendant : %2");
            flux << chaine.arg(_ui->demiGrandAxeCirc->text()).arg(_ui->ADNoeudAscendant2->text().rightJustified(9, '0')) << endl;

            chaine = tr("Ex                   : %1\tInclinaison                         : %2", "Ex = Component X of eccentricity vector");
            flux << chaine.arg(_ui->ex1->text().rightJustified(10, ' ')).arg(_ui->inclinaison2->text().rightJustified(9, '0')) << endl;

            chaine = tr("Ey                   : %1\tPosition sur orbite                 : %2", "Ey = Component Y of eccentricity vector");
            flux << chaine.arg(_ui->ey1->text().rightJustified(10, ' ')).arg(_ui->positionSurOrbite->text().rightJustified(9, '0'))
                 << endl << endl;
            break;

        case 2:
            // Parametres equatoriaux
            chaine = tr("Demi-grand axe       : %1\tIx                 : %2", "Ix = Component X of inclination vector");
            flux << chaine.arg(_ui->demiGrandAxeEquat->text()).arg(_ui->ix1->text()) << endl;

            chaine = tr("Excentricité         : %1\tIy                 : %2", "Iy = Component Y of inclination vector");
            flux << chaine.arg(_ui->excentricite2->text()).arg(_ui->iy1->text()) << endl;

            chaine = tr("Longitude du périgée : %1\tAnomalie moyenne   : %2");
            flux << chaine.arg(_ui->longitudePerigee->text().rightJustified(9, '0')).arg(_ui->anomalieMoyenne2->text().rightJustified(9, '0'))
                 << endl << endl;
            break;

        case 3:
            // Parametres circulaires equatoriaux
            chaine = tr("Demi-grand axe       : %1\tIx                          : %2", "Ix = Component X of inclination vector");
            flux << chaine.arg(_ui->demiGrandAxeCircEquat->text()).arg(_ui->ix2->text().rightJustified(10, ' ')) << endl;

            chaine = tr("Ex                   : %1\tIy                          : %2",
                        "Ex = Component X of eccentricity vector, Iy = Component Y of inclination vector");
            flux << chaine.arg(_ui->ex2->text().rightJustified(10, ' ')).arg(_ui->iy2->text().rightJustified(10, ' ')) << endl;

            chaine = tr("Ey                   : %1\tArgument de longitude vraie : %2", "Ey = Component Y of eccentricity vector");
            flux << chaine.arg(_ui->ey2->text().rightJustified(10, ' ')).arg(_ui->argumentLongitudeVraie2->text().rightJustified(9, '0'))
                 << endl << endl;
            break;

        default:
            break;
        }

        chaine = tr("Anomalie vraie       : %1\tApogée  (Altitude) : %2");
        flux << chaine.arg(_ui->anomalieVraie->text().rightJustified(9, '0')).arg(_ui->apogee->text()) << endl;

        chaine = tr("Anomalie excentrique : %1\tPérigée (Altitude) : %2");
        flux << chaine.arg(_ui->anomalieExcentrique->text().rightJustified(9, '0')).arg(_ui->perigee->text()) << endl;

        chaine = tr("Champ de vue         : %1  \tPériode orbitale   : %2");
        flux << chaine.arg(_ui->champDeVue->text()).arg(_ui->periode->text().replace(" ", "")) << endl << endl;


        flux << tr("Divers :") << endl;
        chaine = tr("Doppler @ 100 MHz    : %1", "Doppler effect at 100 MegaHertz");
        flux << chaine.arg(_ui->doppler->text()) << endl;

        chaine = tr("Atténuation          : %1");
        flux << chaine.arg(_ui->attenuation->text()) << endl;

        chaine = tr("Délai                : %1", "Delay of signal at light speed");
        flux << chaine.arg(_ui->delai->text()) << endl << endl;

        chaine = tr("Phasage              : %1");
        flux << chaine.arg(_ui->phasage->text()) << endl;

        sw.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet General
 */
void Onglets::SauveOngletGeneral(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

#if (BUILD_TEST == false)
        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
                arg(QString(APP_ANNEES_DEV)) << endl << endl << endl;
#endif

        flux << tr("Date :", "Date and hour") << " " << _ui->dateHeure1->text() << endl << endl;

        flux << tr("Lieu d'observation :") << " " << _ui->lieuxObservation1->currentText() << endl;
        QString chaine = tr("Longitude  : %1\tLatitude : %2\tAltitude : %3", "Observer coordinates");
        flux << chaine.arg(_ui->longitudeObs->text()).arg(_ui->latitudeObs->text()).arg(_ui->altitudeObs->text()) << endl;
        chaine = tr("Conditions : %1", "Conditions of observation");
        flux << chaine.arg(_ui->conditionsObservation->text()) << endl << endl << endl;

        if (_ui->barreOnglets->count() == _nbOnglets) {

            // Donnees sur le satellite
            flux << tr("Nom du satellite :") + " " + _ui->nomsat1->text() << endl << endl;

            chaine = tr("Longitude : %1\t\tHauteur    : %2\tAscension droite :  %3");
            flux << chaine.arg(_ui->longitudeSat->text().trimmed()).arg(_ui->hauteurSat->text()).arg(_ui->ascensionDroiteSat->text().trimmed())
                 << endl;

            chaine = tr("Latitude  :  %1\t\tAzimut (N) : %2\tDéclinaison      : %3");
            flux << chaine.arg(_ui->latitudeSat->text().trimmed()).arg(_ui->azimutSat->text().trimmed()).arg(_ui->declinaisonSat->text()) << endl;

            chaine = tr("Altitude  :  %1\t\tDistance   : %2\tConstellation    : %3", "Altitude of satellite");
            flux << chaine.arg(_ui->altitudeSat->text().leftJustified(13, ' ')).arg(_ui->distanceSat->text().leftJustified(13, ' '))
                    .arg(_ui->constellationSat->text()) << endl << endl;

            chaine = tr("Direction          : %1  \tOrbite n°%2      \t\t%3");
            flux << chaine.arg(_ui->directionSat->text()).arg(_ui->nbOrbitesSat->text())
                    .arg((_ui->magnitudeSat->x() == 333) ? _ui->magnitudeSat->text() : "").trimmed() << endl;

            chaine = tr("Vitesse orbitale   : %1\t%2\t%3");
            flux << chaine.arg(_ui->vitesseSat->text().rightJustified(11, ' '))
        #if (BUILD_TEST == true)
                    .arg(_ui->lbl_prochainJN->text() + " " + _ui->dateJN->text() + " ")
                    .arg(_ui->lbl_beta->text()).trimmed() << endl;
#else
                    .arg((_ui->dateJN->isVisible()) ? _ui->lbl_prochainJN->text() + " " + _ui->dateJN->text() + " " : _ui->magnitudeSat->text())
                    .arg((_ui->dateAOS->isVisible()) ? _ui->lbl_beta->text() : "").trimmed() << endl;
#endif

            chaine = tr("Variation distance : %1  \t%2", "Range rate");
            flux << chaine.arg(_ui->rangeRate->text().rightJustified(11, ' '))
        #if (BUILD_TEST == true)
                    .arg(_ui->lbl_prochainAOS->text() + " " + _ui->dateAOS->text())
                    .trimmed() << endl << endl << endl;
#else
                    .arg((_ui->dateAOS->isVisible()) ? _ui->lbl_prochainAOS->text() + " " + _ui->dateAOS->text() : _ui->lbl_beta->text())
                    .trimmed() << endl << endl << endl;
#endif
        }

        // Donnees sur le Soleil
        flux << tr("Coordonnées du Soleil :") << endl;
        chaine = tr("Hauteur    : %1\t\tAscension droite :  %2");
        flux << chaine.arg(_ui->hauteurSoleil->text().trimmed()).arg(_ui->ascensionDroiteSoleil->text()) << endl;

        chaine = tr("Azimut (N) : %1\t\tDéclinaison      : %2", "Azimuth from the North");
        flux << chaine.arg(_ui->azimutSoleil->text().trimmed()).arg(_ui->declinaisonSoleil->text()) << endl;

        chaine = tr("Distance   : %1   \t\tConstellation    : %2");
        flux << chaine.arg(_ui->distanceSoleil->text()).arg(_ui->constellationSoleil->text()) << endl << endl << endl;

        // Donnees sur la Lune
        flux << tr("Coordonnées de la Lune :") << endl;
        chaine = tr("Hauteur    : %1\t\tAscension droite :  %2");
        flux << chaine.arg(_ui->hauteurLune->text().trimmed()).arg(_ui->ascensionDroiteLune->text()) << endl;

        chaine = tr("Azimut (N) : %1\t\tDéclinaison      : %2", "Azimuth from the North");
        flux << chaine.arg(_ui->azimutLune->text().trimmed()).arg(_ui->declinaisonLune->text()) << endl;

        chaine = tr("Distance   : %1  \t\tConstellation    : %2");
        flux << chaine.arg(_ui->distanceLune->text()).arg(_ui->constellationLune->text()) << endl << endl;
        flux << tr("Phase        :", "Moon phase") + " " + _ui->phaseLune->text() << endl;
        flux << tr("Illumination :") + " " + _ui->illuminationLune->text() << endl;
        flux << tr("Magnitude    :") + " " + _ui->magnitudeLune->text() << endl;

        sw.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet Informations
 */
void Onglets::SauveOngletInformations(const QString &fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fic);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

#if (BUILD_TEST == false)
        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)).arg(QCoreApplication::organizationName()).
                arg(QString(APP_ANNEES_DEV)) << endl << endl << endl;
#endif

        if (_ui->informations->currentIndex() == _ui->informations->indexOf(_ui->infos)) {

            // Donnees sur le satellite
            flux << tr("Nom du satellite :") + " " + _ui->nomsat3->text() << endl;
            flux << _ui->line1->text() << endl;
            flux << _ui->line2->text() << endl << endl;

            QString chaine = tr("Numéro NORAD            : %1 \t\tMoyen mouvement       : %2 rev/jour\t Date de lancement  : %3", "revolution per day");
            flux << chaine.arg(_ui->norad->text()).arg(_ui->nbRev->text()).arg(_ui->dateLancement->text()) << endl;

            chaine = tr("Désignation COSPAR      : %1\t\tn'/2                  : %2 rev/jour^2\t Catégorie d'orbite : %3",
                        "n'/2 = derivative of the mean motion divided by two (in revolution per day square)");
            flux << chaine.arg(_ui->cospar->text()).arg(_ui->nbRev2->text().rightJustified(11, ' ')).arg(_ui->categorieOrbite->text()) << endl;

            chaine = tr("Époque (UTC)            : %1\tn\"/6                  : %2 rev/jour^3\t Pays/Organisation  : %3",
                        "n\"/6 = second derivative of the mean motion divided by six (in revolution per day cube)");
            flux << chaine.arg(_ui->epoque->text()).arg(_ui->nbRev3->text().rightJustified(11, ' ')).arg(_ui->pays->text()) << endl;

            chaine = tr("Coeff pseudo-balistique : %1 (1/Re)\tNb orbites à l'époque : %2\t\t\t Site de lancement  : %3",
                        "Pseudo-ballistic coefficient in 1/Earth radius");
            flux << chaine.arg(_ui->bstar->text()).arg(_ui->nbOrbitesEpoque->text()).arg(_ui->siteLancement->text()) << endl << endl;

            chaine = tr("Inclinaison             : %1\t\tAnomalie moyenne      : %2");
            flux << chaine.arg(_ui->inclinaisonMoy->text().trimmed().rightJustified(9, '0'))
                    .arg(_ui->anomalieMoy->text().trimmed().rightJustified(9, '0')) << endl;

            chaine = tr("AD noeud ascendant      : %1\t\tMagnitude std/max     : %2",
                        "Right ascension of the ascending node, Standard/Maximal magnitude");
            flux << chaine.arg(_ui->ADNoeudAscendantMoy->text().trimmed().rightJustified(9, '0')).arg(_ui->magnitudeStdMax->text()) << endl;

            chaine = tr("Excentricité            : %1\t\tModèle orbital        : %2");
            flux << chaine.arg(_ui->excentriciteMoy->text()).arg(_ui->modele->text()) << endl;

            chaine = tr("Argument du périgée     : %1\t\tDimensions/Section    : %2%3");
            flux << chaine.arg(_ui->argumentPerigeeMoy->text().trimmed().rightJustified(9, '0')).arg(_ui->dimensions->text())
                    .arg((_ui->dimensions->text() == tr("Inconnues")) ? "" : "^2") << endl;

        } else if (_ui->informations->currentIndex() == _ui->informations->indexOf(_ui->recherche)) {

            // Donnees sur le satellite
            flux << tr("Nom                :") + " " + _ui->nomsat->text() << endl;

            QString chaine = tr("Numéro NORAD       : %1\t\tMagnitude std/max  : %2", "Standard/Maximal magnitude");
            flux << chaine.arg(_ui->numNorad->text()).arg(_ui->magnitudeStdMaxDonneesSat->text()) << endl;

            chaine = tr("Désignation COSPAR : %1\t\tModèle orbital     : %2");
            flux << chaine.arg(_ui->desigCospar->text()).arg(_ui->modele->text()) << endl;

            chaine = tr("Dimensions/Section : %1%2");
            flux << chaine.arg(_ui->dimensionsDonneesSat->text()).
                    arg((_ui->dimensionsDonneesSat->text() == tr("Inconnues")) ? "" : "^2") << endl << endl;

            chaine = tr("Date de lancement  : %1\t\tApogée  (Altitude) : %2");
            flux << chaine.arg(_ui->dateLancementDonneesSat->text()).arg(_ui->apogeeDonneesSat->text()) << endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Date de rentrée    : %1\t\t").arg(_ui->dateRentree->text()) :
                                                       tr("Catégorie d'orbite : %1\t\t").arg(_ui->categorieOrbiteDonneesSat->text());
            flux << chaine + tr("Périgée (Altitude) : %1").arg(_ui->perigeeDonneesSat->text()) << endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Catégorie d'orbite : %1\t\t").arg(_ui->categorieOrbiteDonneesSat->text()) :
                                                       tr("Pays/Organisation  : %1\t\t").arg(_ui->paysDonneesSat->text());
            flux << chaine + tr("Période orbitale   : %1").arg(_ui->periodeDonneesSat->text()) << endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Pays/Organisation  : %1\t\t").arg(_ui->paysDonneesSat->text()) :
                                                       tr("Site de lancement  : %1\t\t").arg(_ui->siteLancement->text());
            flux << chaine + tr("Inclinaison        : %1").arg(_ui->inclinaisonDonneesSat->text()) << endl;

            if (_ui->dateRentree->isVisible()) {
                flux << tr("Site de lancement  : %1").arg(_ui->siteLancementDonneesSat->text());
            }

            //} else if (_ui->informations->currentIndex() == _ui->informations->indexOf(_ui->rentrees)) {
            // TODO
        }

        sw.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Ajout d'un fichier dans la liste de telechargement (telechargement asynchrone)
 */
void Onglets::AjoutFichier(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_listeFichiersTelechargement.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(TelechargementSuivant()));
    }

    _listeFichiersTelechargement.enqueue(url);

    /* Retour */
    return;
}

/*
 * Ajout d'une liste de fichiers a telecharger (telechargement asynchrone)
 */
void Onglets::AjoutListeFichiers(const QStringList &listeFichiers)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    for(const QString &url : listeFichiers) {
        AjoutFichier(QUrl::fromEncoded(url.toLocal8Bit()));
    }

    if (_listeFichiersTelechargement.isEmpty()) {
        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
    }

    /* Retour */
    return;
}

/*
 * Telechargement d'un fichier
 */
void Onglets::TelechargementFichier(const QString &fichier, const bool async)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QUrl url(fichier);

    /* Corps de la methode */
    if (async) {
        AjoutFichier(url);
    }

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    const QNetworkRequest requete(url);
    _rep = _mng.get(requete);

    if (async) {
        if (_listeFichiersTelechargement.isEmpty()) {
            QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
        }
    } else {

        // Creation d'une boucle pour rendre le telechargement synchrone
        QEventLoop loop;
        connect(_rep, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        const QString fic = QFileInfo(url.path()).fileName();
        _fichier.setFileName(_dirDwn + QDir::separator() + fic);
        if (_fichier.open(QIODevice::WriteOnly)) {
            _fichier.write(_rep->readAll());
            _fichier.close();
        }
    }

    /* Retour */
    return;
}


void Onglets::on_majMaintenant_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->compteRenduMaj->clear();
    _ui->compteRenduMaj->setVisible(true);

    const CategorieTLE categorie = Configuration::instance()->listeCategoriesTLE().at(_ui->groupeTLE->currentIndex());
    const QString nom = categorie.nom[Configuration::instance()->locale()].split("@").at(0);
    const QString adresse = (categorie.site.contains("celestrak")) ?
                Configuration::instance()->adresseCelestrakNorad() : Configuration::instance()->adresseAstropedia() + "previsat/tle/";
    connect(this, SIGNAL(TelechargementFini()), this, SLOT(FinTelechargementTle()));

    /* Corps de la methode */
    _dirDwn = Configuration::instance()->dirTle();
    _ui->majMaintenant->setEnabled(false);

    const QString chaine = tr("Mise à jour du groupe de TLE \"%1\" (à partir de %2) en cours...");
    emit AfficherMessageStatut(chaine.arg(nom).arg(categorie.site), 10);

    _ui->frameBarreProgression->setVisible(true);
    QStringListIterator it(categorie.fichiers);
    while (it.hasNext()) {
        AjoutFichier(QUrl(adresse + it.next()));
    }

    if (_listeFichiersTelechargement.isEmpty()) {

        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));

        if (categorie.fichiers.contains(Configuration::instance()->nomfic())) {
            emit RechargerTLE();
        }
    }

    /* Retour */
    return;
}

void Onglets::on_pause_clicked()
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
    emit ModeManuel(enb);

    /* Retour */
    return;
}


/*
 * Affichage au demarrage
 */
void Onglets::InitAffichageDemarrage()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();
    _nbOnglets = _ui->barreOnglets->count();

    /* Corps de la methode */
    _ui->menuBar->setVisible(false);
    _ui->barreOnglets->setCurrentIndex(0);
    _ui->ongletsOptions->setCurrentIndex(0);
    _ui->ongletsOutils->setCurrentIndex(0);

    on_informations_currentChanged(_indexInfo);
    _ui->infoPrec->setToolTip(QCoreApplication::translate("Onglets", _titreInformations[(_indexInfo + _ui->informations->count() - 1)
                              % _ui->informations->count()]));
    _ui->infoSuiv->setToolTip(QCoreApplication::translate("Onglets", _titreInformations[(_indexInfo + 1) % _ui->informations->count()]));

    _ui->configuration->setCurrentIndex(_indexOption);
    _ui->optionPrec->setToolTip(_titreOptions.at((_indexOption + 3) % 4));
    _ui->optionSuiv->setToolTip(_titreOptions.at((_indexOption + 1) % 4));

    on_miseAJourTLE_currentChanged(_indexMajTLE);
    _ui->majPrec->setToolTip(QCoreApplication::translate("Onglets", _titreMajTLE[(_indexMajTLE + 2) % 2]));
    _ui->majSuiv->setToolTip(QCoreApplication::translate("Onglets", _titreMajTLE[(_indexMajTLE + 1) % 2]));


#if !defined (Q_OS_WIN)
    _ui->grpVecteurEtat->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
    _ui->grpElementsOsculateurs->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
    _ui->grpSignal->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");

    _ui->barreOnglets->removeTab(_ui->barreOnglets->indexOf(_ui->telescope));
    _nbOnglets--;
#endif

    _ui->policeWCC->clear();
#if defined (Q_OS_WIN)
    _ui->policeWCC->addItem("Lucida Console");
    _ui->policeWCC->addItem("MS Shell Dlg 2");

#elif defined (Q_OS_LINUX)
    _ui->policeWCC->addItem("FreeSans");
    _ui->policeWCC->addItem("Sans Serif");

#elif defined (Q_OS_MAC)
    _ui->policeWCC->addItem("Lucida Grande");
    _ui->policeWCC->addItem("Marion");
#endif

    _ui->policeWCC->setCurrentIndex(settings.value("affichage/policeWCC", 0).toInt());

    _ui->enregistrerPref->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));

    _ui->lbl_ageTLE->adjustSize();
    _ui->ageTLE->move(_ui->lbl_ageTLE->x() + _ui->lbl_ageTLE->width() + 3, _ui->ageTLE->y());

    _ui->lbl_nbOrbitesSat->adjustSize();
    _ui->lbl_nbOrbitesSat->resize(_ui->lbl_nbOrbitesSat->width(), 16);
    _ui->nbOrbitesSat->move(_ui->lbl_nbOrbitesSat->x() + _ui->lbl_nbOrbitesSat->width() + 2, _ui->nbOrbitesSat->y());

    _ui->lbl_prochainAOS->adjustSize();
    _ui->lbl_prochainAOS->resize(_ui->lbl_prochainAOS->width(), 16);
    _ui->dateAOS->move(_ui->lbl_prochainAOS->x() + _ui->lbl_prochainAOS->width() + 7, _ui->dateAOS->y());
    _ui->dateJN->move(_ui->dateAOS->x(), _ui->dateJN->y());

    _ui->ajoutLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowRight));
    _ui->supprLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowLeft));

    const QRegExpValidator *valLon = new QRegExpValidator(QRegExp("((0\\d\\d|1[0-7]\\d)°[0-5]\\d'[0-5]\\d\"|180°0?0'0?0\")"));
    _ui->nvLongitude->setValidator(valLon);

    const QRegExpValidator *valLat = new QRegExpValidator(QRegExp("((0\\d|[0-8]\\d)°[0-5]\\d'[0-5]\\d\"|90°0?0'0?0\")"));
    _ui->nvLatitude->setValidator(valLat);

    const QString unite = (_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot");
    QIntValidator *valAlt;
    if (_ui->unitesKm->isChecked()) {
        valAlt = new QIntValidator(-500, 8900);
        _ui->nvAltitude->setValidator(valAlt);
    } else {
        valAlt = new QIntValidator(-1640, 29200);
        _ui->nvAltitude->setValidator(valAlt);
    }
    const QString fmt = tr("L'altitude doit être comprise entre %1%2 et %3%2", "Observer altitude");
    _ui->nvAltitude->setToolTip(fmt.arg(valAlt->bottom()).arg(unite).arg(valAlt->top()));

    _ui->dateHeure3->setVisible(false);
    _ui->utcManuel->setVisible(false);
    _ui->dateHeure4->setVisible(false);
    _ui->utcManuel2->setVisible(false);
    _ui->frameSimu->setVisible(false);
    _ui->pause->setEnabled(false);

    _ui->lbl_prochainAOS->setVisible(false);
    _ui->dateAOS->setVisible(false);

    QPalette pal;
    const QBrush coulLabel = QBrush(QColor::fromRgb(227, 227, 227));
    pal.setBrush(_ui->lbl_coordonneesSoleil->backgroundRole(), coulLabel);
    _ui->lbl_coordonneesSoleil->setPalette(pal);
    _ui->lbl_coordonneesLune->setPalette(pal);

    _ui->pasGeneration->setCurrentIndex(settings.value("previsions/pasGeneration", 5).toInt());
    _ui->lieuxObservation2->setCurrentIndex(settings.value("previsions/lieuxObservation2", 0).toInt());
    _ui->valHauteurSatPrev->setVisible(false);
    _ui->hauteurSatPrev->setCurrentIndex(settings.value("previsions/hauteurSatPrev", 0).toInt());
    _ui->valHauteurSoleilPrev->setVisible(false);
    _ui->hauteurSoleilPrev->setCurrentIndex(settings.value("previsions/hauteurSoleilPrev", 1).toInt());
    _ui->illuminationPrev->setChecked(settings.value("previsions/illuminationPrev", true).toBool());
    _ui->magnitudeMaxPrev->setChecked(settings.value("previsions/magnitudeMaxPrev", false).toBool());
    _ui->valMagnitudeMaxPrev->setVisible(_ui->magnitudeMaxPrev->isChecked());

    _ui->outilsLieuxObservation->setVisible(false);
    _ui->nvEw->setCurrentIndex(0);
    _ui->nvNs->setCurrentIndex(0);

    const QIcon suppr(":/resources/suppr.png");
    _ui->actionSupprimerCategorie->setIcon(suppr);
    _ui->actionSupprimerLieu->setIcon(suppr);
    _ui->actionSupprimerLieuSelec->setIcon(suppr);

    const QIcon ajout(":/resources/ajout.png");
    _ui->actionCreer_une_categorie->setIcon(ajout);
    _ui->creationCategorie->setIcon(ajout);
    _ui->creationCategorie->setToolTip(tr("Créer une catégorie"));
    _ui->actionCreer_un_nouveau_lieu->setIcon(ajout);
    _ui->creationLieu->setIcon(ajout);
    _ui->creationLieu->setToolTip(tr("Créer un nouveau lieu"));

    _ui->actionModifier_coordonnees->setIcon(QIcon(":/resources/editer.png"));
    _ui->actionAjouter_Mes_Preferes->setIcon(QIcon(":/resources/pref.png"));

    _ui->inclinaisonExtraction->setCurrentIndex(0);
    _ui->valeursInclinaison2->setVisible(false);
    _ui->fichierTelechargement->setText("");
    _ui->barreProgression->setValue(0);
    _ui->frameBarreProgression->setVisible(false);
    _ui->compteRenduMaj->setVisible(false);
    _ui->compteRenduMaj2->setVisible(false);

    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->hauteurSatMetOp->setCurrentIndex(settings.value("previsions/hauteurSatMetOp", 2).toInt());
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->hauteurSoleilMetOp->setCurrentIndex(settings.value("previsions/hauteurSoleilMetOp", 1).toInt());
    _ui->lieuxObservation3->setCurrentIndex(settings.value("previsions/lieuxObservation3", 0).toInt());
    _ui->ordreChronologiqueMetOp->setChecked(settings.value("previsions/ordreChronologiqueMetOp", true).toBool());
    _ui->magnitudeMaxMetOp->setValue(settings.value("previsions/magnitudeMaxMetOp", 2.).toDouble());

    _ui->valHauteurSatTransit->setVisible(false);
    _ui->manoeuvresISS->setVisible(false);
    _ui->hauteurSatTransit->setCurrentIndex(settings.value("previsions/hauteurSatTransit", 1).toInt());
    _ui->lieuxObservation4->setCurrentIndex(settings.value("previsions/lieuxObservation4", 0).toInt());
    _ui->ageMaxTLETransit->setValue(settings.value("previsions/ageMaxTLETransit", 2.).toDouble());
    _ui->elongationMaxCorps->setValue(settings.value("previsions/elongationMaxCorps", 5.).toDouble());

    // Fichier flashs MetOp et SkyMed
    _ficTLEMetOp.clear();
    const QString nomFicMetOp = settings.value("fichier/fichierTLEMetOp", Configuration::instance()->dirTle() + QDir::separator() +
                                               "flares-spctrk.txt").toString().trimmed();
    const QFileInfo fim(nomFicMetOp);
    if (fim.exists() && fim.size() != 0) {
        _ui->fichierTLEMetOp->addItem(fim.fileName());
        _ui->fichierTLEMetOp->setItemData(0, QColor(Qt::gray), Qt::BackgroundRole);
        _ficTLEMetOp.append(QDir::toNativeSeparators(nomFicMetOp));
    }

    if (_ficTLEMetOp.isEmpty()) {
        _ui->fichierTLEMetOp->addItem("");
    }
    _ui->fichierTLEMetOp->addItem(tr("Parcourir..."));

    // Affichage des manoeuvres ISS
    if (!Configuration::instance()->evenementsISS().isEmpty()) {
        AffichageManoeuvresISS();
    }

    _ui->valHauteurSatSuivi->setVisible(false);
    _ui->hauteurSatSuivi->setCurrentIndex(settings.value("previsions/hauteurSatSuivi", 2).toInt());
    _ui->lieuxObservation5->setCurrentIndex(settings.value("previsions/lieuxObservation5", 0).toInt());

    _ui->passageApogee->setChecked(settings.value("previsions/passageApogee", true).toBool());
    _ui->passageNoeuds->setChecked(settings.value("previsions/passageNoeuds", true).toBool());
    _ui->passageOmbre->setChecked(settings.value("previsions/passageOmbre", true).toBool());
    _ui->passageQuadrangles->setChecked(settings.value("previsions/passageQuadrangles", true).toBool());
    _ui->transitionJourNuit->setChecked(settings.value("previsions/transitionJourNuit", true).toBool());

    AffichageMessagesMaj();

    // Chargement des langues
    const bool etat = _ui->langue->blockSignals(true);
    _ui->langue->clear();
    _ui->langue->addItem(QIcon(":/resources/drapeaux/fr.png"), "Français");

    const QDir di(Configuration::instance()->dirLang());
    const QStringList filtres(QStringList () << QCoreApplication::applicationName() + "_*.qm");
    QStringList listeFicTrad = di.entryList(filtres, QDir::Files);
    QTranslator trad;
    QPixmap fond(30, 20);
    fond.fill(_ui->langue->palette().window().color());

    for(int i = 0; i < listeFicTrad.size(); i++) {

        const QString locale = Configuration::instance()->listeFicLang().at(i + 1);

        trad.load(Configuration::instance()->dirLang() + QDir::separator() + listeFicTrad.at(i));
        const QString langue = trad.translate("Onglets", "Langue", "Translate by the name of language, for example : English, Français, Español");

        const QFileInfo fi(":/resources/drapeaux/" + locale + ".png");
        const QIcon drapeau = (fi.exists()) ? QIcon(fi.filePath()) : fond;

        _ui->langue->addItem(drapeau, langue);
    }

    _ui->langue->setCurrentIndex(Configuration::instance()->listeFicLang().indexOf(settings.value("affichage/langue", "en").toString()));
    _ui->langue->blockSignals(etat);

    if (settings.value("affichage/utc", false).toBool()) {
        _ui->utc->setChecked(true);
    } else {
        _ui->heureLegale->setChecked(true);
    }

    if (settings.value("affichage/unite", true).toBool()) {
        _ui->unitesKm->setChecked(true);
    } else {
        _ui->unitesMi->setChecked(true);
    }

    if (settings.value("affichage/systemeHoraire", true).toBool()) {
        _ui->syst24h->setChecked(true);
    } else {
        _ui->syst12h->setChecked(true);
    }

    /* Retour */
    return;
}

/*
 * Chargement des stations
 */
void Onglets::InitChargementStations()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->listeStations->clear();

    /* Corps de la methode */
    QMapIterator<QString, Observateur> it(Configuration::instance()->mapStations());
    while (it.hasNext()) {
        it.next();

        const QString acronyme = it.key();
        const QString nom = it.value().nomlieu();

        QListWidgetItem * const station = new QListWidgetItem(QString("%1 (%2)").arg(nom).arg(acronyme), _ui->listeStations);
        station->setCheckState((static_cast<Qt::CheckState> (settings.value("affichage/station" + acronyme, Qt::Checked).
                                                             toUInt())) ? Qt::Checked : Qt::Unchecked);
        station->setData(Qt::UserRole, acronyme);
    }
    _ui->listeStations->sortItems();

    /* Retour */
    return;
}

/*
 * Chargement des fichiers de lieux d'observation
 */
void Onglets::InitFicObs(const bool alarme)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const bool etat1 = _ui->categoriesObs->blockSignals(true);
    const bool etat2 = _ui->ajdfic->blockSignals(true);
    _ui->categoriesObs->clear();
    _ui->ajdfic->clear();

    /* Corps de la methode */
    const QDir di(Configuration::instance()->dirCoord());
    if (di.exists()) {

        // Liste des fichiers de coordonnees
        const QStringList fichiersObs = di.entryList(QDir::Files);
        if (fichiersObs.count() == 0) {
            if (alarme) {
                Message::Afficher(tr("Erreur rencontrée lors de l'initialisation\n" \
                                     "Il n'existe aucun fichier de lieux d'observation"), WARNING);
            }
        } else {

            QStringList &ficObs = Configuration::instance()->listeFicObs();
            ficObs.clear();

            foreach (QString fic, fichiersObs) {

                try {

                    const QString fichier = Configuration::instance()->dirCoord() + QDir::separator() + fic;
                    QFile fi(fichier);
                    if (fi.exists()) {

                        fi.open(QIODevice::ReadOnly | QIODevice::Text);
                        QTextStream flux(&fi);
                        while (!flux.atEnd()) {

                            const QString lieu = flux.readLine();
                            if (lieu.size() > 33) {
                                if ((lieu.at(14) != ' ') || (lieu.at(28) != ' ') || (lieu.at(33) != ' ')) {
                                    throw PreviSatException();
                                }
                            } else {
                                throw PreviSatException();
                            }
                        }

                        if (fic == "preferes") {
                            ficObs.insert(0, fichier);
                            _ui->categoriesObs->insertItem(0, tr("Mes Préférés"));
                            _ui->ajdfic->insertItem(0, tr("Mes Préférés"));
                        } else {
                            ficObs.append(fichier);
                            fic[0] = fic[0].toUpper();
                            _ui->categoriesObs->addItem(fic);
                            _ui->ajdfic->addItem(fic);
                        }
                    }

                } catch (PreviSatException &e) {
                }
            }

            if (ficObs.isEmpty()) {
                if (alarme) {
                    Message::Afficher(tr("Erreur rencontrée lors de l'initialisation\n" \
                                         "Il n'existe aucun fichier de lieux d'observation"), WARNING);
                }
            }
        }
    } else {
        if (alarme) {
            Message::Afficher(tr("Erreur rencontrée lors de l'initialisation\n" \
                                 "Le répertoire contenant les fichiers de lieux d'observation n'existe pas"), WARNING);
        }
    }

    _ui->categoriesObs->blockSignals(etat1);
    _ui->ajdfic->blockSignals(etat2);

    /* Retour */
    return;
}

/*
 * Chargement de la liste de cartes du monde
 */
void Onglets::InitFicMap(const bool majAff)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const bool etat = _ui->listeMap->blockSignals(true);
    _ui->listeMap->clear();
    _ui->listeMap->addItem(tr("* Défaut"));
    _ui->listeMap->setCurrentIndex(0);

    if (Configuration::instance()->listeFicMap().count() == 0) {
        _ui->listeMap->addItem(tr("Télécharger..."));
    } else {

        foreach(QString fic, Configuration::instance()->listeFicMap()) {

            const QString file = Configuration::instance()->dirMap() + QDir::separator() + fic;
            _ui->listeMap->addItem(fic.at(0).toUpper() + fic.mid(1, fic.lastIndexOf(".")-1));

            if (settings.value("fichier/listeMap", "").toString() == file) {
                _ui->listeMap->setCurrentIndex(Configuration::instance()->listeFicMap().indexOf(file)+1);
            }
        }

        _ui->listeMap->addItem(tr("Télécharger..."));
        if ((_ui->listeMap->currentIndex() > 0) && majAff) {
            emit MiseAJourCarte();
        }
    }
    _ui->listeMap->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Chargement de la liste de fichiers de preferences
 */
void Onglets::InitFicPref(const bool majAff)
{
    /* Declarations des variables locales */
    QString fichier;

    /* Initialisations */
    const bool etat = _ui->preferences->blockSignals(true);
    _ui->preferences->clear();

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicPref());
    while (it.hasNext()) {

        const QString fic = Configuration::instance()->dirPrf() + QDir::separator() + it.next();
        const QFileInfo fi(fic);
        fichier = fi.completeBaseName();
        fichier[0] = fichier[0].toUpper();

        _ui->preferences->addItem((fi.completeBaseName() == "defaut") ? tr("* Défaut") : fichier);
        if (settings.value("fichier/preferences", Configuration::instance()->dirPrf() + QDir::separator() + "defaut").toString() == fic) {
            _ui->preferences->setCurrentIndex(_ui->preferences->count() - 1);
        }
    }

    _ui->preferences->addItem(tr("Enregistrer sous..."));
    if (majAff) {
        emit MiseAJourCarte();
    }

    _ui->preferences->addItem(tr("Supprimer..."));
    _ui->preferences->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Chargement de la liste de notifications sonores
 */
void Onglets::InitFicSon()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QDir di(Configuration::instance()->dirSon());
    const QStringList filtresAOS(QStringList () << "aos-*.wav");
    const QStringList filtresLOS(QStringList () << "los-*.wav");

    /* Corps de la methode */
    _ficSonAOS.clear();
    const bool etat = _ui->listeSons->blockSignals(true);
    _ui->listeSons->clear();
    _ui->listeSons->addItem(tr("* Défaut"));
    _ui->listeSons->setCurrentIndex(0);

    QStringList listSonsAOS = di.entryList(filtresAOS, QDir::Files);
    listSonsAOS.removeAll("aos-default.wav");
    QStringList listSonsLOS = di.entryList(filtresLOS, QDir::Files);
    listSonsLOS.removeAll("los-default.wav");

    if ((listSonsAOS.count() == 0) || (listSonsLOS.count() == 0)) {
        _ui->listeSons->addItem(tr("Télécharger..."));
    } else {

        foreach(QString fic, listSonsAOS) {

            const QString file = Configuration::instance()->dirSon() + QDir::separator() + fic;
            _ficSonAOS.append(file);
            _ui->listeSons->addItem(fic.at(4).toUpper() + fic.mid(5, fic.mid(4).lastIndexOf(".")-1));
            if (settings.value("fichier/listeSon", "").toString() == file) {
                _ui->listeSons->setCurrentIndex(_ficSonAOS.indexOf(file)+1);
            }
        }

        foreach(QString fic, listSonsLOS) {
            const QString file = Configuration::instance()->dirSon() + QDir::separator() + fic;
            _ficSonLOS.append(file);
        }
        _ui->listeSons->addItem(tr("Télécharger..."));
    }

    _ui->listeSons->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Rechargement du fichier TLE et rafraichissement de l'affichage
 */
void Onglets::ReactualiserAffichage()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _info = true;
    _acalcAOS = true;
    _acalcDN = true;

    /* Corps de la methode */
    emit RechargerTLE();

    const QString noradDefaut = Configuration::instance()->tleDefaut().l1.mid(2, 5);
    QList<Satellite> &satellites = Configuration::instance()->listeSatellites();
    const QFileInfo fi(Configuration::instance()->nomfic());

    if (!Configuration::instance()->mapTLE().isEmpty()) {

        satellites.clear();
        QStringListIterator it(Configuration::instance()->mapSatellitesFicTLE()[fi.fileName()]);
        while (it.hasNext()) {

            const QString norad = it.next();
            const TLE tle = Configuration::instance()->mapTLE()[norad];

            if (norad == noradDefaut) {
                satellites.insert(0, Satellite(tle));
            } else {
                satellites.append(Satellite(tle));
            }
        }
    }

    emit RecalculerPositions();

    /* Retour */
    return;
}


/*
 * Ecriture du fichier
 */
void Onglets::EcritureFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _fichier.write(_rep->readAll());

    /* Retour */
    return;
}

/*
 * Progression du telechargement
 */
void Onglets::ProgressionTelechargement(qint64 octetsRecus, qint64 octetsTotal)
{
    /* Declarations des variables locales */
    QString unite;

    /* Initialisations */

    /* Corps de la methode */
    // Calcul de la vitesse de telechargement
    double vitesse = static_cast<double> (octetsRecus) * 1000. / static_cast<double> (_tempsEcoule.elapsed());

    if (vitesse < 1024.) {
        unite = tr("o/s", "Byte per second");
    } else if (vitesse < 1048576.) {
        vitesse /= 1024.;
        unite = tr("ko/s", "kilo-byte per second");
    } else {
        vitesse /= 1048576.;
        unite = tr("Mo/s", "Mega-byte per second");
    }

    emit Progression(static_cast<int>(octetsRecus), static_cast<int>(octetsTotal), vitesse, unite);

    if ((octetsTotal != -1) && (_ui->miseAJourTLEauto->isVisible())) {

        _ui->barreProgression->setRange(0, (int) octetsTotal);
        _ui->barreProgression->setValue((int) octetsRecus);

        _ui->vitesseTelechargement->setText(QString("%1 %2").arg(vitesse, 0, 'f', 1).arg(unite));
    }

    /* Retour */
    return;
}

/*
 * Fin de l'enregistrement du fichier
 */
void Onglets::FinEnregistrementFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _fichier.close();

    QFile fd(_fichier.fileName());
    fd.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fd);
    const QString lg = flux.readLine();
    fd.close();

    if (_rep->error() || lg.toLower().contains("doctype") || lg.trimmed().isEmpty()) {
        // Erreur lors du telechargement
        _fichier.remove();
    } else {

        if (_dirDwn == Configuration::instance()->dirTle()) {

            QString fichierALire = QDir::toNativeSeparators(_fichier.fileName());
            QFileInfo ff(fichierALire);
            QString fichierAMettreAJour = QDir::toNativeSeparators(Configuration::instance()->dirTle() + QDir::separator() + ff.fileName());

            QFile fi(fichierAMettreAJour);
            if (fi.exists() && fi.size() > 0) {

                QStringList compteRendu;
                const int affMsg = _ui->affichageMsgMAJ->currentIndex();
                try {
                    TLE::MiseAJourFichier(fichierAMettreAJour, fichierALire, affMsg, compteRendu);
                    EcritureCompteRenduMaj(compteRendu, _ui->compteRenduMaj);

                } catch (PreviSatException &ex) {
                }

            } else {

                const QString msg = (fi.exists() && (fi.size() == 0)) ? tr("Remplacement du fichier %1") : tr("Ajout du fichier %1");

                if (fi.size() == 0) {
                    fi.remove();
                }

                fi.copy(fichierALire, fichierAMettreAJour);
                _ui->compteRenduMaj->appendPlainText(msg.arg(fd.fileName()));
                InitFicTLE();
            }

            // Verification et chargement du fichier TLE courant
            if (fichierAMettreAJour == Configuration::instance()->nomfic()) {

                const int nb = TLE::VerifieFichier(Configuration::instance()->nomfic(), false);
                if (nb == 0) {
                    const QString msg = tr("Erreur lors du téléchargement du fichier %1");
                    Message::Afficher(msg.arg(Configuration::instance()->nomfic()), WARNING);
                    _ui->compteRenduMaj->setVisible(false);
                } else {
                    ReactualiserAffichage();
                }
            }
        } else if (_dirDwn != Configuration::instance()->dirTmp()) {

            const QFileInfo f(_fichier);
            const QString fic = _dirDwn + QDir::separator() +
                    ((_dirDwn == Configuration::instance()->dirLocalData()) ?
                         Configuration::instance()->listeFicLocalData().filter(f.fileName()).first() :
                         f.fileName());

            // Deplacement du fichier vers le repertoire de destination
            QFile fi(fic);
            if (fi.exists()) {
                fi.remove();
            }
            _fichier.rename(fi.fileName());
        }
    }
    _rep->deleteLater();
    TelechargementSuivant();

    /* Retour */
    return;
}

/*
 * Traitements a effectuer apres le telechargement des fichiers de donnees
 */
void Onglets::FinTelechargementDonnees()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    emit AfficherMessageStatut(tr("Téléchargement terminé"), 10);
    Message::Afficher(tr("Veuillez redémarrer %1 pour prendre en compte la mise à jour des fichiers"), INFO);

    disconnect(this, SIGNAL(TelechargementFini()), this, SLOT(FinTelechargementDonnees()));

    /* Retour */
    return;
}

/*
 * Traitements a effectuer apres le telechargement des fichiers TLE
 */
void Onglets::FinTelechargementTle()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->majMaintenant->setEnabled(true);
    _ui->frameBarreProgression->setVisible(false);
    emit AfficherMessageStatut(tr("Téléchargement terminé"), 10);

    disconnect(this, SIGNAL(TelechargementFini()), this, SLOT(FinTelechargementTle()));
    emit RecalculerPositions();
    emit MiseAJourCarte();

    /* Retour */
    return;
}

/*
 * Telechargement du fichier suivant
 */
void Onglets::TelechargementSuivant()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_listeFichiersTelechargement.isEmpty()) {
        emit TelechargementFini();
    } else {

        const QUrl url = _listeFichiersTelechargement.dequeue();
        const QString fic = QFileInfo(url.path()).fileName();

        _fichier.setFileName(Configuration::instance()->dirTmp() + "/" + fic);

        if (fic.endsWith("txt")) {
            _ui->fichierTelechargement->setText(fic.mid(fic.indexOf("/") + 1));
        } else {
            _ui->fichierTelechargement->setText("TLE ISS");
        }

        if (_ui->miseAJourTLEauto->isVisible()) {
            _ui->barreProgression->setValue(0);
            _ui->frameBarreProgression->setVisible(true);
            _ui->compteRenduMaj->setVisible(true);
        }

        if (_fichier.open(QIODevice::WriteOnly)) {

            const QNetworkRequest requete(url);
            _rep = _mng.get(requete);
            connect(_rep, SIGNAL(downloadProgress(qint64, qint64)), SLOT(ProgressionTelechargement(qint64, qint64)));
            connect(_rep, SIGNAL(finished()), SLOT(FinEnregistrementFichier()));
            connect(_rep, SIGNAL(readyRead()), SLOT(EcritureFichier()));
            _tempsEcoule.start();

        } else {
            // Erreur
            TelechargementSuivant();
        }
    }

    /* Retour */
    return;
}

bool Onglets::eventFilter(QObject *object, QEvent *evt)
{
    Q_UNUSED(object)

    if (evt->type() == QEvent::MouseMove) {

        // Affichage de la categorie d'orbite
        if (_ui->categorieOrbite->underMouse() || _ui->categorieOrbiteDonneesSat->underMouse()) {

            QString acronyme = Configuration::instance()->listeSatellites().at(0).tle().donnees().categorieOrbite();
            _ui->categorieOrbite->setToolTip(Configuration::instance()->mapCategoriesOrbite()[acronyme]);

            acronyme = _ui->categorieOrbiteDonneesSat->text();
            _ui->categorieOrbiteDonneesSat->setToolTip(Configuration::instance()->mapCategoriesOrbite()[acronyme]);
        }

        // Affichage du pays ou de l'organisation
        if (_ui->pays->underMouse() || _ui->paysDonneesSat->underMouse()) {

            QString acronyme = Configuration::instance()->listeSatellites().at(0).tle().donnees().pays();
            _ui->pays->setToolTip(Configuration::instance()->mapPays()[acronyme]);

            acronyme = _ui->paysDonneesSat->text();
            _ui->paysDonneesSat->setToolTip(Configuration::instance()->mapPays()[acronyme]);
        }

        // Affichage du site de lancement
        if (_ui->siteLancement->underMouse() || _ui->siteLancementDonneesSat->underMouse()) {

            if (_ui->siteLancement->underMouse()) {

                const QString acronyme = Configuration::instance()->listeSatellites().at(0).tle().donnees().siteLancement();
                const Observateur site = Configuration::instance()->mapSites()[acronyme];

                if (!site.nomlieu().isEmpty()) {
                    _ui->siteLancement->setToolTip(site.nomlieu());
                    emit AffichageSiteLancement(acronyme, site);
                }
            }

            if (_ui->siteLancementDonneesSat->underMouse()) {

                const QString acronyme = _ui->siteLancementDonneesSat->text();
                const Observateur site = Configuration::instance()->mapSites()[acronyme];

                if (!site.nomlieu().isEmpty()) {
                    _ui->siteLancementDonneesSat->setToolTip(site.nomlieu());
                    emit AffichageSiteLancement(acronyme, site);
                }
            }
        } else {
            emit AffichageSiteLancement("", Observateur());
        }
    }

    return QMainWindow::eventFilter(object, evt);
}

void Onglets::mousePressEvent(QMouseEvent *event)
{
    if (_ui->lieuxObs->underMouse()) {
        if (event->button() == Qt::LeftButton) {
            _positionSouris = event->pos();
        }
    }
}

void Onglets::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        const int distance = (event->pos() - _positionSouris).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {

        }
    }
}

void Onglets::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ((_ui->dateHeure1->underMouse() && _ui->dateHeure1->isVisible()) || (_ui->dateHeure2->underMouse() && _ui->dateHeure2->isVisible())) {
        emit ModeManuel(true);
    }

    if ((_ui->vitesseSat->underMouse() && _ui->vitesseSat->isVisible()) || (_ui->rangeRate->underMouse() && _ui->rangeRate->isVisible()) ||
            (_ui->vxsat->underMouse() && _ui->vxsat->isVisible()) || (_ui->vysat->underMouse() && _ui->vysat->isVisible()) ||
            (_ui->vzsat->underMouse() && _ui->vzsat->isVisible())) {
        _uniteVitesse = !_uniteVitesse;
        AffichageVitesses();
    }
}

void Onglets::on_typeParametres_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AffichageElementsOSculateurs();
}

void Onglets::on_typeRepere_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AffichageElementsOSculateurs();
}

void Onglets::on_informations_currentChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->informations->count() == 2) {
        if (arg1 == 0) {
            _ui->infoPrec->setVisible(false);
            _ui->infoSuiv->setVisible(true);
            _ui->infoSuiv->setToolTip(QCoreApplication::translate("Onglets", _titreInformations[1]));
        } else {
            _ui->infoPrec->setVisible(true);
            _ui->infoPrec->setToolTip(QCoreApplication::translate("Onglets", _titreInformations[0]));
            _ui->infoSuiv->setVisible(false);
        }
    } else {
        _ui->infoPrec->setVisible(true);
        _ui->infoSuiv->setVisible(true);
        _ui->infoPrec->setToolTip(QCoreApplication::translate("Onglets", _titreInformations[(arg1 + _ui->informations->count() - 1)
                                  % _ui->informations->count()]));
        _ui->infoSuiv->setToolTip(QCoreApplication::translate("Onglets", _titreInformations[(arg1 + 1) % _ui->informations->count()]));
    }

    // Recherche donnees satellite
    if (arg1 == _ui->informations->indexOf(_ui->recherche)) {

        _ui->noradDonneesSat->setValue(0);

        _ui->frameResultats->setVisible(false);
        if (!Configuration::instance()->listeSatellites().isEmpty()) {
            _ui->noradDonneesSat->setValue(Configuration::instance()->listeSatellites().at(0).tle().norad().toInt());
        }
        _ui->nom->setFocus();
    }

    /* Retour */
    return;
}


QString Onglets::getText(QWidget *fenetreParent, const QString &titre, const QString &label, const QString &texteOk, const QString &texteAnnule,
                         QLineEdit::EchoMode mode, const QString &texte, Qt::WindowFlags flags, Qt::InputMethodHints hints)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QInputDialog input(fenetreParent, flags);
    input.setWindowTitle(titre);
    input.setLabelText(label);
    input.setTextValue(texte);
    input.setTextEchoMode(mode);
    input.setInputMethodHints(hints);
    input.setOkButtonText(texteOk);
    input.setCancelButtonText(texteAnnule);

    const int ret = input.exec();

    /* Retour */
    return ((ret) ? input.textValue() : QString());
}

void Onglets::on_dateHeure3_dateTimeChanged(const QDateTime &dateTime)
{
    emit ChangementDate(dateTime);
}

void Onglets::on_dateHeure4_dateTimeChanged(const QDateTime &dateTime)
{
    emit ChangementDate(dateTime);
}

void Onglets::on_play_clicked()
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

void Onglets::on_rewind_clicked()
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

void Onglets::on_forward_clicked()
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
    emit ModeManuel(enb);

    /* Retour */
    return;
}

void Onglets::on_backward_clicked()
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
    emit ModeManuel(enb);

    /* Retour */
    return;
}

void Onglets::on_infoPrec_clicked()
{
    if (_ui->informations->isVisible()) {
        _indexInfo = (_ui->informations->currentIndex() + _ui->informations->count() - 1) % _ui->informations->count();
        _ui->informations->setCurrentIndex(_indexInfo);
        _ui->barreOnglets->setTabText(2, QCoreApplication::translate("Onglets", _titreInformations[_indexInfo]));
        on_informations_currentChanged(_indexInfo);
    }
}

void Onglets::on_infoSuiv_clicked()
{
    if (_ui->informations->isVisible()) {
        _indexInfo = (_ui->informations->currentIndex() + 1) % _ui->informations->count();
        _ui->informations->setCurrentIndex(_indexInfo);
        _ui->barreOnglets->setTabText(2, QCoreApplication::translate("Onglets", _titreInformations[_indexInfo]));
        on_informations_currentChanged(_indexInfo);
    }
}

void Onglets::on_nom_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomsat = _ui->nom->text();

    /* Corps de la methode */
    if (nomsat.length() >= 3) {

        int indx1 = 127;
        int indx2 = 0;
        int indx3;
        _resultatsSatellitesTrouves.clear();
        const QString &donneesSat = Configuration::instance()->donneesSatellites();

        // Recherche dans le tableau de donnees a partir du nom de l'objet
        do {
            indx1 = donneesSat.toLower().indexOf(nomsat.toLower().trimmed(), indx1 + indx2);
            if (indx1 >= 0) {

                indx3 = donneesSat.lastIndexOf("\n", indx1) + 1;
                indx2 = donneesSat.indexOf("\n", indx3) - indx3;
                if ((indx1 - indx3) >= 124) {

                    const QString ligne = donneesSat.mid(indx3, indx2);
                    if (!ligne.isEmpty()) {
                        _resultatsSatellitesTrouves.append(ligne);
                    }
                    indx1 = indx3;
                } else {
                    indx1 += nomsat.trimmed().length() - indx2;
                }
            }
        } while (indx1 >= 0);

        if (!_resultatsSatellitesTrouves.isEmpty()) {
            _ui->cosparDonneesSat->setText(_resultatsSatellitesTrouves.at(0).mid(7, 11).toUpper().trimmed());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(_resultatsSatellitesTrouves.at(0).mid(0, 6).toInt());
            _ui->noradDonneesSat->blockSignals(false);
        }

        AffichageResultatsDonnees();

    } else {
        QToolTip::showText(_ui->nom->mapToGlobal(QPoint(0, 0)), _ui->nom->toolTip());
    }

    /* Retour */
    return;
}

void Onglets::on_noradDonneesSat_valueChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (arg1 > 0) {

        _resultatsSatellitesTrouves.clear();
        const QString &donneesSat = Configuration::instance()->donneesSatellites();
        const int lgRec = Configuration::instance()->lgRec();

        const int idx = lgRec * arg1;
        if ((idx >= 0) && (idx < donneesSat.size())) {

            const QString ligne = donneesSat.mid(idx, lgRec);
            _resultatsSatellitesTrouves.append(ligne);

            QString nomsat = ligne.mid(124).trimmed();
            if (nomsat == "iss (zarya)") {
                nomsat = "ISS";
            }

            _ui->nom->setText(nomsat.toUpper());
            _ui->cosparDonneesSat->setText(ligne.mid(7, 11).toUpper().trimmed());
        }

        AffichageResultatsDonnees();
    }

    /* Retour */
    return;
}

void Onglets::on_cosparDonneesSat_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if ((_ui->cosparDonneesSat->text().length() > 1) && (_ui->cosparDonneesSat->text().contains("-"))) {

        int indx1 = 0;
        int indx2 = 1;
        _resultatsSatellitesTrouves.clear();
        const QString &donneesSat = Configuration::instance()->donneesSatellites();

        // Recherche dans le tableau de donnees a partir de la designation COSPAR
        do {
            indx1 = donneesSat.indexOf(_ui->cosparDonneesSat->text().toLower().trimmed(), indx1 + indx2);
            if ((indx1 >= 0) && (donneesSat.at(qMax(0, indx1 - 7)) == '\n')) {

                indx1 = donneesSat.lastIndexOf("\n", indx1) + 1;
                indx2 = donneesSat.indexOf("\n", indx1) - indx1;
                const QString ligne = donneesSat.mid(indx1, indx2);

                if (!ligne.isEmpty()) {
                    _resultatsSatellitesTrouves.append(ligne);
                }
            }
        } while (indx1 >= 0);

        if (!_resultatsSatellitesTrouves.isEmpty()) {
            _ui->nom->setText(_resultatsSatellitesTrouves.at(0).mid(124).toUpper().trimmed());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(_resultatsSatellitesTrouves.at(0).mid(0, 6).toInt());
            _ui->noradDonneesSat->blockSignals(false);
        }

        AffichageResultatsDonnees();
    }

    /* Retour */
    return;
}

void Onglets::on_satellitesTrouves_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        const QString ligne = _resultatsSatellitesTrouves.at(currentRow).toUpper();
        const double magnitudeStandard = ligne.mid(35, 4).toDouble();

        const QString dateRentree = ligne.mid(60, 10).trimmed();
        const QString periode = ligne.mid(71, 10).trimmed();
        double perigee = ligne.mid(82, 7).trimmed().toDouble();
        double apogee = ligne.mid(90, 7).trimmed().toDouble();

        double ap = apogee + RAYON_TERRESTRE;
        double per = perigee + RAYON_TERRESTRE;

        // Nom du satellite
        QString nomsat = ligne.mid(124).trimmed();
        if (nomsat.toLower() == "iss (zarya)") {
            nomsat = "ISS";
        }
        _ui->nomsat->setText((nomsat.isEmpty()) ? tr("Inconnu") : nomsat);

        // Numero NORAD
        const QString norad = ligne.mid(0, 6);
        _ui->numNorad->setText(norad);

        // Designation COSPAR
        const QString cospar = ligne.mid(7, 11).trimmed();
        _ui->desigCospar->setText((cospar.isEmpty()) ? tr("Inconnue") : cospar);

        // Magnitude standard/maximale
        if ((magnitudeStandard > 98.) || (perigee < EPSDBL100) || (apogee < EPSDBL100)) {
            _ui->magnitudeStdMaxDonneesSat->setText("?/?");
        } else {

            // Estimation de la magnitude maximale
            const double demiGrandAxe = 0.5 * (ap + per);
            const double exc = 2. * ap / (ap + per) - 1.;
            const double magMax = magnitudeStandard - 15.75 + 5. * log10(1.45 * (demiGrandAxe * (1. - exc) - RAYON_TERRESTRE));
            char methMagnitude = ligne.at(40).toLower().toLatin1();

            QString text;
            _ui->magnitudeStdMaxDonneesSat->setText(text.asprintf("%+.1f%c/%+.1f", magnitudeStandard, methMagnitude, magMax));
        }

        // Modele orbital
        const bool modeleDS = (periode.toDouble() > 225.);
        const QString modele = (modeleDS) ? tr("SGP4 (DS)", "Orbital model SGP4 (deep space)") : tr("SGP4 (NE)", "Orbital model SGP4 (near Earth)");
        _ui->modeleDonneesSat->setText((periode.isEmpty()) ? tr("Non applicable") : modele);
        if (!periode.isEmpty()) {
            _ui->modeleDonneesSat->adjustSize();
            _ui->modeleDonneesSat->setFixedHeight(16);
            _ui->modele->setToolTip((modeleDS) ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));
        }

        // Dimensions du satellite
        double t1 = ligne.mid(19, 5).toDouble();
        double t2 = ligne.mid(25, 4).toDouble();
        double t3 = ligne.mid(30, 4).toDouble();
        double section = ligne.mid(42, 6).toDouble();
        QString unite1 = tr("m", "meter");
        QString unite2 = tr("km", "kilometer");
        if (_ui->unitesMi->isChecked()) {

            apogee *= MILE_PAR_KM;
            perigee *= MILE_PAR_KM;
            ap *= MILE_PAR_KM;
            per *= MILE_PAR_KM;

            t1 *= PIED_PAR_METRE;
            t2 *= PIED_PAR_METRE;
            t3 *= PIED_PAR_METRE;
            section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
            unite1 = tr("ft", "foot");
            unite2 = tr("nmi", "nautical mile");
        }

        QString dimensions;
        if ((fabs(t2) < EPSDBL100) && (fabs(t3) < EPSDBL100)) {
            const QString fmt3 = tr("Sphérique. R=%1 %2", "R = radius");
            dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite1);
        }
        if ((fabs(t2) >= EPSDBL100) && (fabs(t3) < EPSDBL100)) {
            const QString fmt3 = tr("Cylindrique. L=%1 %2, R=%3 %2", "L = height, R = radius");
            dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite1).arg(t2, 0, 'f', 1);
        }
        if ((fabs(t2) >= EPSDBL100) && (fabs(t3) >= EPSDBL100)) {
            const QString fmt3 = tr("Boîte. %1 x %2 x %3 %4");
            dimensions = fmt3.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1).arg(unite1);
        }
        if (fabs(t1) < EPSDBL100) {
            dimensions = tr("Inconnues");
        }

        if (fabs(section) > EPSDBL100) {
            dimensions.append(" / %1 %2");
            dimensions = dimensions.arg(section, 0, 'f', 2).arg(unite1);
            _ui->sqDonneesSat->setVisible(true);
        } else {
            _ui->sqDonneesSat->setVisible(false);
        }

        _ui->dimensionsDonneesSat->setText(dimensions);

        // Apogee/perigee/periode orbitale
        const QString fmt = "%1 %2 (%3 %2)";
        if (fabs(apogee) < EPSDBL100) {
            _ui->apogeeDonneesSat->setText(tr("Inconnu"));
        } else {
            _ui->apogeeDonneesSat->setText(fmt.arg(ap, 0, 'f', 0).arg(unite2).arg(apogee, 0, 'f', 0));
        }

        if (fabs(perigee) < EPSDBL100) {
            _ui->perigeeDonneesSat->setText(tr("Inconnu"));
        } else {
            _ui->perigeeDonneesSat->setText(fmt.arg(per, 0, 'f', 0).arg(unite2).arg(perigee, 0, 'f', 0));
        }

        const QString period = (periode.isEmpty()) ?
                    tr("Inconnue") : Maths::ToSexagesimal(periode.toDouble() * NB_HEUR_PAR_MIN * HEUR2RAD, HEURE1, 1, 0, false, true).trimmed();
        _ui->periodeDonneesSat->setText(period);

        // Inclinaison
        const QString inclinaison = ligne.mid(97, 6).trimmed();
        _ui->inclinaisonDonneesSat->setText((inclinaison.isEmpty()) ? tr("Inconnue") : inclinaison + "°");

        // Date de lancement
        const QString dateLancement = ligne.mid(48, 10).trimmed();
        const int annee_lct = dateLancement.mid(0, 4).toInt();
        const int mois_lct = dateLancement.mid(5, 2).toInt();
        const double jour_lct = dateLancement.mid(8, 2).toDouble();
        const Date date_lancement(annee_lct, mois_lct, jour_lct, 0.);

        _ui->dateLancementDonneesSat->setText((dateLancement.isEmpty()) ?
                                                  tr("Inconnue") : date_lancement.ToShortDate(FORMAT_COURT, SYSTEME_24H).left(10));

        // Date de rentree
        if (dateRentree.isEmpty()) {
            _ui->lbl_dateRentree->setVisible(false);
            _ui->dateRentree->setVisible(false);

            _ui->lbl_categorieOrbiteDonneesSat->move(0, 15);
            _ui->categorieOrbiteDonneesSat->move(_ui->categorieOrbiteDonneesSat->x(), 15);

            _ui->lbl_paysDonneesSat->move(0, 30);
            _ui->paysDonneesSat->move(_ui->paysDonneesSat->x(), 30);

            _ui->lbl_siteLancementDonneesSat->move(0, 45);
            _ui->siteLancementDonneesSat->move( _ui->siteLancementDonneesSat->x(), 45);

        } else {

            const int annee_rentree = dateRentree.mid(0, 4).toInt();
            const int mois_rentree = dateRentree.mid(5, 2).toInt();
            const double jour_rentree = dateRentree.mid(8, 2).toDouble();
            const Date date_rentree(annee_rentree, mois_rentree, jour_rentree, 0.);

            _ui->dateRentree->setText(date_rentree.ToShortDate(FORMAT_COURT, SYSTEME_24H).left(10));
            _ui->lbl_dateRentree->setVisible(true);
            _ui->dateRentree->setVisible(true);

            _ui->lbl_categorieOrbiteDonneesSat->move(0, 85);
            _ui->categorieOrbiteDonneesSat->move(111, 85);

            _ui->lbl_paysDonneesSat->move(0, 100);
            _ui->paysDonneesSat->move(111, 100);

            _ui->lbl_siteLancementDonneesSat->move(0, 115);
            _ui->siteLancementDonneesSat->move(111, 115);
        }

        // Categorie d'orbite
        const QString categorie = ligne.mid(105, 6).trimmed();
        _ui->categorieOrbiteDonneesSat->setText((categorie.isEmpty()) ? tr("Inconnue") : categorie);

        // Pays/Organisation
        const QString pays = ligne.mid(112, 5).trimmed();
        _ui->paysDonneesSat->setText((pays.isEmpty()) ? tr("Inconnu") : pays);

        // Site de lancement
        const QString siteLancement = ligne.mid(118, 5).trimmed();
        _ui->siteLancementDonneesSat->setText((siteLancement.isEmpty()) ? tr("Inconnu") : siteLancement);
        _ui->siteLancementDonneesSat->adjustSize();
        _ui->siteLancementDonneesSat->setFixedHeight(16);

        // Recherche des fichiers TLE dans lesquels le satellite est present
        _ui->fichiersTle->clear();
        const QDir di(Configuration::instance()->dirTle());
        const QStringList filtres(QStringList () << "*.txt");
        const QStringList listeFicTle = di.entryList(filtres, QDir::Files);

        foreach(const QString fic, listeFicTle) {

            QFile fi(Configuration::instance()->dirTle() + QDir::separator() + fic);
            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);

            bool atr = false;
            const int nor = norad.toInt();
            while (!flux.atEnd() && !atr) {

                const QString lig = flux.readLine();
                if (lig.mid(0, 2) == "1 ") {
                    if (lig.mid(2, 5).toInt() == nor) {
                        atr = true;
                    }
                }
            }
            fi.close();

            if (atr) {
                _ui->fichiersTle->addItem(fic);
            }
        }

        if (_ui->fichiersTle->count() > 0) {
            _ui->lbl_fichiersTle->setVisible(true);
            _ui->fichiersTle->setVisible(true);
        } else {
            _ui->lbl_fichiersTle->setVisible(false);
            _ui->fichiersTle->setVisible(false);
        }
    }

    /* Retour */
    return;
}

/*
 * Gestion des lieux d'observation
 */
void Onglets::on_categoriesObs_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    _ui->actionSupprimerCategorie->setVisible(_ui->categoriesObs->currentRow() > 0);
    _ui->actionRenommerCategorie->setVisible(_ui->categoriesObs->currentRow() > 0);
    _ui->menuContextuelCategorie->exec(QCursor::pos());

    /* Retour */
    return;
}

void Onglets::on_categoriesObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    if (_ui->outilsLieuxObservation->isVisible() && !_ui->ajdfic->isVisible()) {
        _ui->outilsLieuxObservation->setVisible(false);
        on_validerObs_clicked();
    }

    _ui->lieuxObs->clear();
    _mapObs.clear();
    emit AfficherMessageStatut("", 0);

    /* Corps de la methode */
    QFile fi(Configuration::instance()->listeFicObs().at(currentRow));
    if (fi.exists()) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();
            const QString nom = ligne.mid(34).trimmed();

            const QStringList coord = ligne.split(" ", QString::SkipEmptyParts);
            const double lon = coord.at(0).toDouble();
            const double lat = coord.at(1).toDouble();
            const double alt = coord.at(2).toDouble();

            _mapObs.insert(nom, Observateur(nom, lon, lat, alt));
            _ui->lieuxObs->addItem(nom.toLatin1());
        }
    }

    /* Retour */
    return;
}

void Onglets::on_actionCreer_une_categorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->outilsLieuxObservation->setCurrentWidget(_ui->nouvelleCategorie);
    _ui->outilsLieuxObservation->setVisible(true);
    _ui->nvCategorie->setText("");
    _ui->nvCategorie->setFocus();

    /* Retour */
    return;
}

void Onglets::on_creationCategorie_clicked()
{
    on_actionCreer_une_categorie_triggered();
}

void Onglets::on_validerCategorie_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->nvCategorie->text().trimmed().isEmpty()) {
        Message::Afficher(tr("Le nom de la catégorie n'est pas spécifié"), WARNING);
    } else {

        if (_ui->categoriesObs->findItems(_ui->nvCategorie->text(), Qt::MatchContains).isEmpty()) {

            const QString dirCoord = Configuration::instance()->dirCoord();
            QDir di(dirCoord);
            if (!di.exists()) {
                di.mkdir(dirCoord);
            }

            QFile fi(dirCoord + QDir::separator() + _ui->nvCategorie->text().toLower());
            fi.open(QIODevice::WriteOnly | QIODevice::Text);
            fi.write("");
            fi.close();
            InitFicObs(false);

            if (_ui->categoriesObs->count() > 0) {
                _ui->categoriesObs->setCurrentRow(0);
            }
            emit AfficherMessageStatut(tr("La nouvelle catégorie de lieux d'observation a été créée"), 10);
            _ui->outilsLieuxObservation->setVisible(false);

        } else {
            Message::Afficher(tr("La catégorie spécifiée existe déjà"), WARNING);
        }
    }

    /* Retour */
    return;
}

void Onglets::on_annulerCategorie_clicked()
{
    _ui->outilsLieuxObservation->setVisible(false);
}

void Onglets::on_actionSupprimerCategorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString categorie = _ui->categoriesObs->currentItem()->text();
    const QString fic = categorie.toLower();

    QMessageBox msgbox(tr("Information"), tr("Voulez-vous vraiment supprimer la catégorie \"%1\"?")
                       .arg(categorie), QMessageBox::Question, QMessageBox::Yes,
                       QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::No) {
        emit AfficherMessageStatut("", 0);
    } else {
        QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + fic);
        fi.remove();
        _ui->lieuxObs->clear();
        InitFicObs(false);
        _ui->categoriesObs->setCurrentRow(0);
        emit AfficherMessageStatut(tr("La catégorie \"%1\" a été supprimée").arg(categorie), 10);
    }

    /* Retour */
    return;
}

void Onglets::on_actionRenommerCategorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString nvNomCateg = getText(this, tr("Catégorie"), tr("Nouveau nom de la catégorie :"), tr("OK"), tr("Annuler"),
                                       QLineEdit::Normal, _ui->categoriesObs->currentItem()->text(), Qt::WindowTitleHint | Qt::WindowCloseButtonHint);


    if (!nvNomCateg.trimmed().isEmpty()) {

        bool ok = true;
        QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + _ui->categoriesObs->currentItem()->text().toLower());

        if (fi.exists()) {

            QMessageBox msgbox(tr("Information"), tr("La catégorie existe déjà. Voulez-vous l'écraser ?"), QMessageBox::Question, QMessageBox::Yes,
                               QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
            msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
            msgbox.setButtonText(QMessageBox::No, tr("Non"));
            msgbox.exec();
            ok = (msgbox.result() == QMessageBox::Yes);

        } else {
            ok = true;
        }

        if (ok) {
            fi.rename(Configuration::instance()->dirCoord() + QDir::separator() + nvNomCateg.trimmed().toLower());
            InitFicObs(false);
        }
    }

    /* Retour */
    return;
}

void Onglets::on_actionTelechargerCategorie_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    Telecharger * const telechargerLieux = new Telecharger(COORDONNEES, this);
    telechargerLieux->setWindowModality(Qt::ApplicationModal);
    telechargerLieux->show();

    /* Retour */
    return;
}

void Onglets::on_lieuxObs_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->lieuxObs->indexAt(pos).row() < 0) {
        _ui->actionRenommerLieu->setVisible(false);
        _ui->actionModifier_coordonnees->setVisible(false);
        _ui->actionSupprimerLieu->setVisible(false);
        _ui->actionAjouter_Mes_Preferes->setVisible(false);
    } else {
        _ui->actionRenommerLieu->setVisible(true);
        _ui->actionModifier_coordonnees->setVisible(true);
        _ui->actionSupprimerLieu->setVisible(true);
        const QFileInfo fi(Configuration::instance()->dirCoord() + QDir::separator() + "preferes");
        _ui->actionAjouter_Mes_Preferes->setVisible((fi.exists()) ? (_ui->categoriesObs->currentRow() > 0) : (_ui->categoriesObs->currentRow() >= 0));
    }
    _ui->actionCreer_un_nouveau_lieu->setVisible(_ui->categoriesObs->count() > 0);
    _ui->menuContextuelLieux->exec(QCursor::pos());

    /* Retour */
    return;
}

void Onglets::on_lieuxObs_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    emit EffacerMessageStatut();

    /* Corps de la methode */
    if (_ui->outilsLieuxObservation->isVisible() && !_ui->ajdfic->isVisible()) {
        _ui->outilsLieuxObservation->setVisible(false);
        on_validerObs_clicked();
    }
    AfficherLieuSelectionne(currentRow);

    /* Retour */
    return;
}

void Onglets::on_actionCreer_un_nouveau_lieu_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->outilsLieuxObservation->setCurrentWidget(_ui->nouveauLieu);
    _ui->outilsLieuxObservation->setVisible(true);

    _ui->nvLieu->setText("");
    _ui->nvLongitude->setText("000°00'00\"");
    _ui->nvLatitude->setText("000°00'00\"");

    if (_ui->unitesKm->isChecked()) {
        _ui->nvAltitude->setText("0000");
        _ui->nvAltitude->setInputMask("####");
    } else {
        _ui->nvAltitude->setText("00000");
        _ui->nvAltitude->setInputMask("#####");
    }

    _ui->lbl_nvUnite->setText((_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot"));
    _ui->lbl_ajouterDans->setVisible(true);
    _ui->ajdfic->setVisible(true);
    _ui->ajdfic->setCurrentIndex(_ui->categoriesObs->currentRow());
    emit AfficherMessageStatut("", 0);
    _ui->nvLieu->setFocus();

    /* Retour */
    return;
}

void Onglets::on_creationLieu_clicked()
{
    on_actionCreer_un_nouveau_lieu_triggered();
}

void Onglets::on_validerObs_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        QString nomlieu = _ui->nvLieu->text().trimmed();
        if (nomlieu.isEmpty()) {
            throw PreviSatException(tr("Le nom du lieu d'observation n'est pas spécifié"), WARNING);
        } else {

            nomlieu[0] = nomlieu.at(0).toUpper();

            // Recuperation de la longitude
            const QStringList lon = _ui->nvLongitude->text().split(QRegExp("[°'\""), QString::SkipEmptyParts);
            const int lo1 = lon.at(0).toInt();
            const int lo2 = lon.at(1).toInt();
            const int lo3 = lon.at(2).toInt();

            if ((lo1 < 0) || (lo1 > 180) || (lo2 < 0) || (lo2 > 59) || (lo3 < 0) || (lo3 > 59)) {
                throw PreviSatException(tr("Erreur dans la saisie de la longitude"), WARNING);
            }

            // Recuperation de la latitude
            const QStringList lat = _ui->nvLatitude->text().split(QRegExp("[°'\""), QString::SkipEmptyParts);
            const int la1 = lat.at(0).toInt();
            const int la2 = lat.at(1).toInt();
            const int la3 = lat.at(2).toInt();

            if ((la1 < 0) || (la1 > 90) || (la2 < 0) || (la2 > 59) || (la3 < 0) || (la3 > 59)) {
                throw PreviSatException(tr("Erreur dans la saisie de la latitude"), WARNING);
            }

            // Recuperation de l'altitude
            int atd = _ui->nvAltitude->text().toInt();
            if (_ui->unitesMi->isChecked()) {
                atd = qRound(atd / PIED_PAR_METRE);
            }
            if ((atd < -500) || (atd > 8900)) {
                throw PreviSatException(tr("Erreur dans la saisie de l'altitude"), WARNING);
            }

            const QString fic = Configuration::instance()->listeFicObs().at(_ui->ajdfic->currentIndex());

            // Ajout du lieu d'observation dans le fichier de coordonnees selectionne
            QFile fi(fic);
            fi.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream flux(&fi);

            while (!flux.atEnd()) {

                const QString ligne = flux.readLine();

                if ((ligne.mid(34).toLower().trimmed() == nomlieu.toLower().trimmed()) && _ui->ajdfic->isVisible()) {
                    const QString msg = tr("Le lieu existe déjà dans la catégorie \"%1\"");
                    throw PreviSatException(msg.arg(_ui->ajdfic->currentText()), WARNING);
                }
            }
            const double x1 = ((_ui->nvEw->currentText() == tr("Est")) ? -1. : 1.) * (lo1 + lo2 * DEG_PAR_ARCMIN + lo3 * DEG_PAR_ARCSEC);
            const double x2 = ((_ui->nvNs->currentText() == tr("Sud")) ? -1. : 1.) * (la1 + la2 * DEG_PAR_ARCMIN + la3 * DEG_PAR_ARCSEC);

            QString text;
            flux << text.asprintf("%+13.9f %+12.9f %04d ", x1, x2, atd) + nomlieu << endl;
            fi.close();

            on_categoriesObs_currentRowChanged(0);
            AffichageLieuObs();
            _ui->outilsLieuxObservation->setVisible(false);
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Onglets::on_annulerObs_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->outilsLieuxObservation->isVisible() && !_ui->ajdfic->isVisible()) {
        _ui->outilsLieuxObservation->setVisible(false);

        // En cas de modification des coordonnees
        if (!_ligneCoord.lon.isEmpty() || !_ligneCoord.lat.isEmpty() || !_ligneCoord.alt.isEmpty()) {
            _ui->nvLongitude->setText(_ligneCoord.lon);
            _ui->nvLatitude->setText(_ligneCoord.lat);
            _ui->nvAltitude->setText(_ligneCoord.alt);
        }
        on_validerObs_clicked();
    }
    _ui->outilsLieuxObservation->setVisible(false);

    /* Retour */
    return;
}

void Onglets::on_actionAjouter_Mes_Preferes_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString fic = Configuration::instance()->listeFicObs().at(0);
    if (!fic.contains("preferes")) {
        // Le fichier preferes n'existe pas, on le cree
        Configuration::instance()->listeFicObs().insert(0, Configuration::instance()->dirCoord() + QDir::separator() + "preferes");
        fic = Configuration::instance()->listeFicObs().at(0);

        QFile fi(fic);
        fi.open(QIODevice::WriteOnly | QIODevice::Text);
        fi.write("");
        fi.close();
        InitFicObs(false);
    }

    /* Corps de la methode */
    const Observateur lieu = _mapObs[_ui->lieuxObs->currentItem()->text()];
    QString nom = lieu.nomlieu();
    nom[0] = nom[0].toUpper();

    // Verification que le lieu d'observation n'existe pas deja dans Mes Preferes
    QFile fichier(fic);
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);
    bool atrouve = false;
    while (!flux.atEnd() && !atrouve) {
        const QString ligne = flux.readLine().mid(34).toLower().trimmed();
        atrouve = (nom == ligne);
    }

    if (atrouve) {
        const QString msg = tr("Le lieu d'observation \"%1\" fait déjà partie de \"Mes Préférés\"");
        Message::Afficher(msg.arg(nom), WARNING);
    } else {

        QString text;
        const QString ligne = text.asprintf("%+13.9f %+12.9f %04d ", lieu.longitude(), lieu.latitude(), static_cast<int> (lieu.altitude())) + nom;

        // Ajout du lieu d'observation dans Mes Preferes
        QFile fich(fic);
        fich.open(QIODevice::Append | QIODevice::Text);
        QTextStream flux2(&fich);
        flux2 << ligne << endl;
        fich.close();

        emit AfficherMessageStatut(tr("Le lieu d'observation \"%1\" a été ajouté dans la catégorie \"Mes Préférés\"").arg(nom), 10);
    }

    /* Retour */
    return;
}

void Onglets::on_actionModifier_coordonnees_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->outilsLieuxObservation->setCurrentWidget(_ui->nouveauLieu);
    _ui->outilsLieuxObservation->setVisible(true);

    if (_ui->unitesKm->isChecked()) {
        _ui->nvAltitude->setInputMask("####");
    } else {
        _ui->nvAltitude->setInputMask("#####");
    }

    const Observateur obs = _mapObs[_ui->lieuxObs->currentItem()->text()];
    _ui->nvLieu->setText(obs.nomlieu().trimmed());

    _ui->nvLongitude->setText(Maths::ToSexagesimal(fabs(obs.longitude()) * DEG2RAD, DEGRE, 3, 0, false, true));
    _ui->nvLongitude->setPalette(QPalette());
    _ui->nvEw->setCurrentIndex((obs.longitude() <= 0.) ? 0 : 1);

    _ui->nvLatitude->setText(Maths::ToSexagesimal(fabs(obs.latitude()) * DEG2RAD, DEGRE, 2, 0,false, true));
    _ui->nvLatitude->setPalette(QPalette());
    _ui->nvNs->setCurrentIndex((obs.latitude() >= 0.) ? 0 : 1);

    const QString alt = "%1";
    const int atd = static_cast<int> (obs.altitude());
    if (_ui->unitesKm->isChecked()) {
        _ui->nvAltitude->setText(alt.arg(atd, 4, 10, QChar('0')));
    } else {
        _ui->nvAltitude->setText(alt.arg(qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd)), 5, 10, QChar('0')));
    }
    _ui->nvAltitude->setPalette(QPalette());
    _ui->lbl_nvUnite->setText((_ui->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot"));
    _ligneCoord.lon = _ui->nvLongitude->text();
    _ligneCoord.lat = _ui->nvLatitude->text();
    _ligneCoord.alt = _ui->nvAltitude->text();

    _ui->lbl_ajouterDans->setVisible(false);
    _ui->ajdfic->setVisible(false);

    // Suppression du lieu du fichier
    QString text;
    const Observateur lieu = _mapObs[_ui->lieuxObs->currentItem()->text()];
    const QString ligne = text.asprintf("%+13.9f %+12.9f %04d ", lieu.longitude(), lieu.latitude(), static_cast<int> (lieu.altitude()))
            + lieu.nomlieu();

    const QString fic = Configuration::instance()->dirCoord() + QDir::separator() +
            ((_ui->categoriesObs->currentRow() == 0) ? "preferes" : _ui->categoriesObs->currentItem()->text());

    QFile sr(fic);
    sr.open(QIODevice::ReadOnly | QIODevice::Text);

    QFile sw(Configuration::instance()->dirTmp() + QDir::separator() + "obs.tmp");
    sw.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream flux(&sr);
    QTextStream flux2(&sw);
    while (!flux.atEnd()) {
        const QString ligne2 = flux.readLine();
        if (ligne.mid(34).trimmed() != ligne2.mid(34).trimmed()) {
            flux2 << ligne2 << endl;
        }
    }
    sw.close();
    sr.close();

    sr.remove();
    sw.rename(fic);

    emit AfficherMessageStatut("", 0);
    _ui->nvLieu->setFocus();

    /* Retour */
    return;
}

void Onglets::on_actionRenommerLieu_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString nvNomLieu = getText(this, tr("Lieu d'observation"), tr("Nouveau nom du lieu d'observation :"), tr("OK"), tr("Annuler"),
                                      QLineEdit::Normal, _ui->lieuxObs->item(_ui->lieuxObs->currentRow())->text(),
                                      Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    if (!nvNomLieu.trimmed().isEmpty()) {

        const QString dirCoord = Configuration::instance()->dirCoord();
        const QString fic = dirCoord + QDir::separator() + ((_ui->categoriesObs->currentRow() == 0) ?
                                                                "preferes" : _ui->categoriesObs->currentItem()->text());

        QFile sr(fic);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);

        QFile sw(Configuration::instance()->dirTmp() + QDir::separator() + "obs.tmp");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream flux(&sr);
        QTextStream flux2(&sw);
        while (!flux.atEnd()) {
            const QString ligne = flux.readLine();
            if (ligne.mid(34).toLower().trimmed() == _ui->lieuxObs->currentItem()->text().toLower().trimmed()) {
                flux2 << ligne.mid(0, 34) << nvNomLieu.trimmed() << endl;
            } else {
                flux2 << ligne << endl;
            }
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(fic);
        on_categoriesObs_currentRowChanged(_ui->categoriesObs->currentRow());
    }

    /* Retour */
    return;
}

void Onglets::on_actionSupprimerLieu_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString fic = Configuration::instance()->listeFicObs().at(_ui->categoriesObs->currentRow());
    const QString nomlieu = _ui->lieuxObs->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer \"%1\" de la catégorie \"%2\"?");

    QMessageBox msgbox(tr("Avertissement"), msg.arg(nomlieu).arg(_ui->categoriesObs->currentItem()->text()), QMessageBox::Question,
                       QMessageBox::Yes, QMessageBox::No | QMessageBox::Default, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::Yes) {

        QString text;
        const Observateur lieu = _mapObs[_ui->lieuxObs->currentItem()->text()];
        const QString ligne = text.asprintf("%+13.9f %+12.9f %04d ", lieu.longitude(), lieu.latitude(), static_cast<int> (lieu.altitude()))
                + lieu.nomlieu();

        QFile sr(fic);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);

        QFile sw(Configuration::instance()->dirTmp() + QDir::separator() + "obs.tmp");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream flux(&sr);
        QTextStream flux2(&sw);
        while (!flux.atEnd()) {
            const QString ligne2 = flux.readLine();
            if (ligne.trimmed() != ligne2.trimmed()) {
                flux2 << ligne2 << endl;
            }
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(fic);
        on_categoriesObs_currentRowChanged(0);
        emit AfficherMessageStatut(tr("Le lieu d'observation \"%1\" a été supprimé de la catégorie \"%2\"").arg(nomlieu)
                                   .arg(_ui->categoriesObs->currentItem()->text()), 10);
    } else {
        emit AfficherMessageStatut("", 0);
    }

    /* Retour */
    return;
}

void Onglets::on_actionSupprimerLieuSelec_triggered()
{
    on_supprLieu_clicked();
}

void Onglets::on_ajoutLieu_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        if (_ui->lieuxObs->currentRow() >= 0) {
            if (!_ui->selecLieux->findItems(_ui->lieuxObs->currentItem()->text(), Qt::MatchExactly).isEmpty()) {
                emit AfficherMessageStatut(tr("Lieu d'observation déjà sélectionné"), 10);
                throw PreviSatException();
            }

            _ui->selecLieux->addItem(_ui->lieuxObs->currentItem()->text());
            Configuration::instance()->observateurs().append(_mapObs[_ui->lieuxObs->currentItem()->text()]);
            Configuration::instance()->EcritureConfiguration();
            _ui->lieuxObs->setFocus();
        }

        // TODO

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Onglets::on_supprLieu_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if ((_ui->selecLieux->currentRow() >= 0) && (_ui->selecLieux->count() > 1)) {

        Configuration::instance()->observateurs().removeAt(_ui->selecLieux->currentRow());
        AffichageLieuObs();
        Configuration::instance()->EcritureConfiguration();

        // TODO
        _ui->outilsLieuxObservation->setVisible(false);
    }

    /* Retour */
    return;
}

void Onglets::on_selecLieux_customContextMenuRequested(const QPoint &pos)
{
    if ((_ui->selecLieux->count() > 1) && (_ui->selecLieux->indexAt(pos).row() >= 0)) {
        _ui->menuContextuelLieuxSelec->exec(QCursor::pos());
    }
}

void Onglets::on_selecLieux_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->outilsLieuxObservation->isVisible() && !_ui->ajdfic->isVisible()) {
        _ui->outilsLieuxObservation->setVisible(false);
        on_validerObs_clicked();
    }
    AfficherLieuSelectionne(currentRow);

    /* Retour */
    return;
}

void Onglets::on_optionPrec_clicked()
{
    _indexOption = (_ui->configuration->currentIndex() + 3) % 4;
    _ui->configuration->setCurrentIndex(_indexOption);
}

void Onglets::on_optionSuiv_clicked()
{
    _indexOption = (_ui->configuration->currentIndex() + 1) % 4;
    _ui->configuration->setCurrentIndex(_indexOption);

}

void Onglets::on_actionTous_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->liste2->hasFocus()) {
        for(int i=0; i<_ui->liste2->count(); i++) {
            _ui->liste2->item(i)->setCheckState(Qt::Checked);
        }
    }

    if (_ui->liste3->hasFocus()) {
        for(int i=0; i<_ui->liste3->count(); i++) {
            _ui->liste3->item(i)->setCheckState(Qt::Checked);
        }
    }

    if (_ui->listeStations->hasFocus()) {
        for(int i=0; i<_ui->listeStations->count(); i++) {
            _ui->listeStations->item(i)->setCheckState(Qt::Checked);
        }
        //ModificationOption();
    }

    /* Retour */
    return;
}

void Onglets::on_actionAucun_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->liste2->hasFocus()) {
        for(int i=0; i<_ui->liste2->count(); i++) {
            _ui->liste2->item(i)->setCheckState(Qt::Unchecked);
        }
    }

    if (_ui->liste3->hasFocus()) {
        for(int i=0; i<_ui->liste3->count(); i++) {
            _ui->liste3->item(i)->setCheckState(Qt::Unchecked);
        }
    }

    if (_ui->listeStations->hasFocus()) {
        for(int i=0; i<_ui->listeStations->count(); i++) {
            _ui->listeStations->item(i)->setCheckState(Qt::Unchecked);
        }
        //ModificationOption();
    }

    /* Retour */
    return;
}

void Onglets::on_barreOnglets_currentChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt = tr("dd/MM/yyyy hh:mm:ss", "date format") + ((_ui->syst12h->isChecked()) ? "a" : "");
    EffacerMessageStatut();
    _ui->compteRenduMaj->setVisible(false);
    _ui->compteRenduMaj2->setVisible(false);

    /* Corps de la methode */
    if (index == _ui->barreOnglets->indexOf(_ui->osculateurs)) {

        //        if (ui->modeManuel->isChecked()) {
        //            _ui->dateHeure4->setDisplayFormat(tr("dddd dd MMMM yyyy  hh:mm:ss") + ((ui->syst12h->isChecked()) ? "a" : ""));
        //            _ui->dateHeure4->setDateTime(_ui->dateHeure3->dateTime());
        //        }

    } else if (index == _ui->barreOnglets->indexOf(_ui->informations)) {
        // TODO
    } else if (index == _ui->barreOnglets->indexOf(_ui->previsions)) {

        const Date date(_date->jourJulien() + EPS_DATES, 0.);
        _ui->dateInitialePrev->setDateTime(date.ToQDateTime(0));
        _ui->dateInitialePrev->setDisplayFormat(fmt);
        _ui->dateFinalePrev->setDateTime(_ui->dateInitialePrev->dateTime().addDays(7));
        _ui->dateFinalePrev->setDisplayFormat(fmt);

        _ui->calculsPrev->setDefault(true);
        _ui->calculsPrev->setFocus();

    } else if (index == _ui->barreOnglets->indexOf(_ui->flashs)) {

        const Date date(_date->jourJulien() + EPS_DATES, 0.);
        _ui->dateInitialeMetOp->setDateTime(date.ToQDateTime(0));
        _ui->dateInitialeMetOp->setDisplayFormat(fmt);
        _ui->dateFinaleMetOp->setDateTime(_ui->dateInitialeMetOp->dateTime().addDays(7));
        _ui->dateFinaleMetOp->setDisplayFormat(fmt);

        _ui->calculsFlashs->setDefault(true);
        _ui->calculsFlashs->setFocus();

    } else if (index == _ui->barreOnglets->indexOf(_ui->transits_ISS)) {

        const Date date(_date->jourJulien() + EPS_DATES, 0.);
        _ui->dateInitialeTransit->setDateTime(date.ToQDateTime(0));
        _ui->dateInitialeTransit->setDisplayFormat(fmt);
        _ui->dateFinaleTransit->setDateTime(_ui->dateInitialeTransit->dateTime().addDays(7));
        _ui->dateFinaleTransit->setDisplayFormat(fmt);

        _ui->calculsTransit->setDefault(true);
        _ui->calculsTransit->setFocus();

        CalculAgeTLETransitISS();


#if defined (Q_OS_WIN)
    } else if (index == _ui->barreOnglets->indexOf(_ui->telescope)) {

        _ui->afficherSuivi->setDefault(false);
        _ui->genererPositions->setDefault(true);

#endif
    } else if ((index == _ui->barreOnglets->indexOf(_ui->outils)) && _ui->evenementsOrbitaux->isVisible()) {
        on_ongletsOutils_currentChanged(_ui->ongletsOutils->indexOf(_ui->evenementsOrbitaux));
    }

    /* Retour */
    return;
}

void Onglets::on_ongletsOutils_currentChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt = tr("dd/MM/yyyy hh:mm:ss", "date format") + ((_ui->syst12h->isChecked()) ? "a" : "");

    /* Corps de la methode */
    if (index == _ui->ongletsOutils->indexOf(_ui->evenementsOrbitaux)) {

        const Date date(_date->jourJulien() + EPS_DATES, 0.);
        _ui->dateInitialeEvt->setDateTime(date.ToQDateTime(0));
        _ui->dateInitialeEvt->setDisplayFormat(fmt);
        _ui->dateFinaleEvt->setDateTime(_ui->dateInitialeEvt->dateTime().addDays(1));
        _ui->dateFinaleEvt->setDisplayFormat(fmt);

        _ui->calculsEvt->setDefault(true);
        _ui->calculsEvt->setFocus();
    }
}

void Onglets::on_gestionnaireMajTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    GestionnaireTLE * const gestionnaire = new GestionnaireTLE;
    gestionnaire->setWindowModality(Qt::ApplicationModal);
    gestionnaire->show();

    /* Retour */
    return;
}

void Onglets::on_listeMap_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->optionConfig->isVisible()) {

        if (index == 0) {
            settings.setValue("fichier/listeMap", "");
        } else {

            if (index == (_ui->listeMap->count() - 1)) {

                Telecharger * const telecharger = new Telecharger(CARTES, this);
                telecharger->setWindowModality(Qt::ApplicationModal);
                telecharger->show();

                const bool etat = _ui->listeMap->blockSignals(true);
                _ui->listeMap->setCurrentIndex(_ui->listeMap->findText(settings.value("fichier/listeMap", "").toString()));
                _ui->listeMap->blockSignals(etat);
                InitFicMap(true);

            } else {
                settings.setValue("fichier/listeMap", (index == 0) ?
                                      "" : Configuration::instance()->listeFicMap().at(qMax(0, _ui->listeMap->currentIndex()-1)));
            }
        }
    }

    /* Retour */
    return;
}

void Onglets::on_listeSons_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->optionConfig->isVisible()) {

        if (index == 0) {
            settings.setValue("fichier/listeSon", "");
        } else {

            if (index == (_ui->listeSons->count() - 1)) {

                Telecharger * const telecharger = new Telecharger(NOTIFICATIONS, this);
                telecharger->setWindowModality(Qt::ApplicationModal);
                telecharger->show();

                const bool etat = _ui->listeSons->blockSignals(true);
                _ui->listeSons->setCurrentIndex(_ui->listeSons->findText(settings.value("fichier/listeSon", "").toString()));
                _ui->listeSons->blockSignals(etat);
                InitFicSon();
            } else {
                settings.setValue("fichier/listeSon", (index == 0) ? "" : _ficSonAOS.at(qMax(0, _ui->listeSons->currentIndex()-1)));
            }
        }
    }

    /* Retour */
    return;
}


void Onglets::on_parcourirMaj1_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierMaj", Configuration::instance()->dirTle()).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle)"));

    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
        _ui->fichierAMettreAJour->setText(fichier);
    }

    /* Retour */
    return;
}

void Onglets::on_parcourirMaj2_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierALire", Configuration::instance()->dirTle()).toString(),
                                                   tr("Fichiers texte (*.txt);;Fichiers TLE (*.tle);;Fichiers gz (*.gz)"));

    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
        _ui->fichierALire->setText(fichier);
    }

    /* Retour */
    return;
}

void Onglets::on_mettreAJourTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    emit EffacerMessageStatut();
    _ui->compteRenduMaj2->clear();
    _ui->compteRenduMaj2->setVisible(false);

    /* Corps de la methode */
    try {
        bool agz = false;
        if (_ui->fichierAMettreAJour->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à mettre à jour n'est pas spécifié"), WARNING);
        }

        if (_ui->fichierALire->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à lire n'est pas spécifié"), WARNING);
        }

        // Fichier a lire au format gz
        QFileInfo fi(_ui->fichierALire->text());
        QString fic;
        if (fi.suffix() == "gz") {

            // Cas d'un fichier compresse au format gz
            fic = Configuration::instance()->dirTmp() + QDir::separator() + fi.completeBaseName();

            if (Decompression::DecompressionFichierGz(_ui->fichierALire->text())) {

                const int nsat = TLE::VerifieFichier(fic, false);
                if (nsat == 0) {
                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                    throw PreviSatException(msg.arg(fi.fileName()), WARNING);
                }
            } else {
                const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                throw PreviSatException(msg.arg(fi.fileName()), WARNING);
            }
        } else {
            fic = QDir::toNativeSeparators(fi.absoluteFilePath());
        }

        const QStringList listeFic(QStringList () << _ui->fichierAMettreAJour->text() << fic);
        foreach(QString file, listeFic) {
            fi = QFileInfo(file);
            if (!fi.exists()) {
                const QString msg = tr("Le fichier %1 n'existe pas");
                throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
            }
        }

        QStringList compteRendu;
        const int affMsg = _ui->affichageMsgMAJ->currentIndex();
        TLE::MiseAJourFichier(_ui->fichierAMettreAJour->text(), fic, affMsg, compteRendu);

        const bool aecr = EcritureCompteRenduMaj(compteRendu, _ui->compteRenduMaj2);

        if (agz) {
            QFile fich(fic);
            fich.remove();
        }

        emit AfficherMessageStatut(tr("Terminé !"), 10);
        _ui->compteRenduMaj2->setVisible(true);

        if ((Configuration::instance()->nomfic() == _ui->fichierAMettreAJour->text().trimmed()) && aecr) {
            ReactualiserAffichage();
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Onglets::on_majPrec_clicked()
{
    if (_ui->miseAJourTLE->isVisible()) {
        _indexMajTLE = (_ui->miseAJourTLE->currentIndex() + _ui->miseAJourTLE->count() - 1) % _ui->miseAJourTLE->count();
        _ui->miseAJourTLE->setCurrentIndex(_indexMajTLE);
        _ui->ongletsOutils->setTabText(0, QCoreApplication::translate("Onglets", _titreMajTLE[_indexMajTLE]));
    }
}

void Onglets::on_majSuiv_clicked()
{
    if (_ui->miseAJourTLE->isVisible()) {
        _indexMajTLE = (_ui->miseAJourTLE->currentIndex() + 1) % _ui->miseAJourTLE->count();
        _ui->miseAJourTLE->setCurrentIndex(_indexMajTLE);
        _ui->ongletsOutils->setTabText(0, QCoreApplication::translate("Onglets", _titreMajTLE[_indexMajTLE]));
    }
}

void Onglets::on_miseAJourTLE_currentChanged(int arg1)
{
    if (arg1 == 0) {
        _ui->majPrec->setVisible(false);
        _ui->majSuiv->setVisible(true);
        _ui->majSuiv->setToolTip(QCoreApplication::translate("Onglets", _titreMajTLE[1]));
    } else {
        _ui->majPrec->setVisible(true);
        _ui->majPrec->setToolTip(QCoreApplication::translate("Onglets", _titreMajTLE[0]));
        _ui->majSuiv->setVisible(false);
    }
}

void Onglets::on_compteRenduMaj_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    _ui->menuContextuelCompteRenduMaj->exec(QCursor::pos());
}

void Onglets::on_compteRenduMaj2_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    _ui->menuContextuelCompteRenduMaj->exec(QCursor::pos());
}

void Onglets::on_actionCopier_dans_le_presse_papier_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QClipboard * const clipboard = QApplication::clipboard();

    /* Corps de la methode */
    if (_ui->compteRenduMaj->isVisible()) {
        clipboard->setText(_ui->compteRenduMaj->toPlainText());

    } else if (_ui->compteRenduMaj2->isVisible()) {
        clipboard->setText(_ui->compteRenduMaj2->toPlainText());
    }

    /* Retour */
    return;
}


/*
 * Calcul des previsions de passage
 */
void Onglets::on_calculsPrev_clicked()
{
    /* Declarations des variables locales */
    QVector<int> vecSat;
    ConditionsPrevisions conditions;

    /* Initialisations */
    int j = 0;
    conditions.listeSatellites.clear();
    vecSat.append(0);

    /* Corps de la methode */
    try {

        if (_ui->liste2->count() == 0) {
            throw PreviSatException();
        }

        for(int i = 0; i < _ui->liste2->count(); i++) {
            if (_ui->liste2->item(i)->checkState() == Qt::Checked) {
                conditions.listeSatellites.append(_ui->liste2->item(i)->data(Qt::UserRole).toString());
                j++;
            }
        }

        if (conditions.listeSatellites.isEmpty()) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), WARNING);
        }

        // Ecart heure locale - UTC
        const bool ecart = (fabs(_date->offsetUTC() - Date::CalculOffsetUTC(_date->ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateInitialePrev->dateTime());
        const double offset2 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateFinalePrev->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialePrev->date().year(), _ui->dateInitialePrev->date().month(), _ui->dateInitialePrev->date().day(),
                         _ui->dateInitialePrev->time().hour(), _ui->dateInitialePrev->time().minute(), _ui->dateInitialePrev->time().second(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinalePrev->date().year(), _ui->dateFinalePrev->date().month(), _ui->dateFinalePrev->date().day(),
                         _ui->dateFinalePrev->time().hour(), _ui->dateFinalePrev->time().minute(), _ui->dateFinalePrev->time().second(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            const double tmp = conditions.jj2;
            conditions.jj2 = conditions.jj1;
            conditions.jj1 = tmp;
        }

        conditions.ecart = ecart;
        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = _ui->syst24h->isChecked();

        // Pas de generation
        conditions.pas = _ui->pasGeneration->currentText().split(QRegExp("[^0-9]+")).first().toDouble();
        if (_ui->pasGeneration->currentIndex() < 5) {
            conditions.pas *= NB_JOUR_PAR_SEC;
        } else {
            conditions.pas *= NB_JOUR_PAR_MIN;
        }

        // Lieu d'observation
        conditions.observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation2->currentIndex());

        // Unites de longueur
        conditions.unite = (_ui->unitesKm->isChecked()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Conditions d'eclairement du satellite
        conditions.eclipse = _ui->illuminationPrev->isChecked();

        // Magnitude maximale
        conditions.magnitudeLimite = (_ui->magnitudeMaxPrev->isChecked()) ? _ui->valMagnitudeMaxPrev->value() : 99.;

        // Hauteur minimale du satellite
        conditions.hauteur = DEG2RAD * ((_ui->hauteurSatPrev->currentIndex() == 5) ?
                                            abs(_ui->valHauteurSatPrev->text().toInt()) : 5 * _ui->hauteurSatPrev->currentIndex());

        // Hauteur maximale du Soleil
        if (_ui->hauteurSoleilPrev->currentIndex() <= 3) {
            conditions.crepuscule = -6. * DEG2RAD * _ui->hauteurSoleilPrev->currentIndex();
        } else if (_ui->hauteurSoleilPrev->currentIndex() == 4) {
            conditions.crepuscule = PI_SUR_DEUX;
        } else if (_ui->hauteurSoleilPrev->currentIndex() == 5) {
            conditions.crepuscule = DEG2RAD * _ui->valHauteurSoleilPrev->text().toInt();
        } else {
        }

        // Prise en compte de l'extinction atmospherique
        conditions.extinction = _ui->extinctionAtmospherique->isChecked();

        // Prise en compte de la refraction atmospherique
        conditions.refraction = _ui->refractionPourEclipses->isChecked();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = _ui->effetEclipsesMagnitude->isChecked();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = _ui->eclipsesLune->isChecked();

        // Fichier TLE
        conditions.fichier = Configuration::instance()->nomfic();

        // Nom du fichier resultat
        const QString chaine = tr("previsions", "filename (without accent)") + "_%1_%2.txt";
        conditions.ficRes = Configuration::instance()->dirTmp() + QDir::separator() +
                chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

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
                Message::Afficher(tr("Aucun passage n'a été trouvé sur la période donnée"), INFO);
            } else {
                afficherResultats = new Afficher(PREVISIONS, conditions, Prevision::donnees(), Prevision::resultats(), this);
                afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void Onglets::on_liste2_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)

    if (_ui->liste2->hasFocus() && (_ui->liste2->currentRow() >= 0)) {

        if (_ui->liste2->currentItem()->checkState() == Qt::Checked) {

            // Suppression d'un satellite dans la liste
            _ui->liste2->currentItem()->setCheckState(Qt::Unchecked);

        } else {

            // Ajout d'un satellite dans la liste
            _ui->liste2->currentItem()->setCheckState(Qt::Checked);
        }
    }
}

void Onglets::on_liste2_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (_ui->liste2->currentRow() >= 0) {
        _ui->menuContextuelListes->exec(QCursor::pos());
    }
}

void Onglets::on_parametrageDefautPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_barreOnglets_currentChanged(_ui->barreOnglets->indexOf(_ui->previsions));
    _ui->pasGeneration->setCurrentIndex(5);
    _ui->lieuxObservation2->setCurrentIndex(0);
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

void Onglets::on_effacerHeuresPrev_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialePrev->setTime(QTime(0, 0, 0));
    _ui->dateFinalePrev->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void Onglets::on_hauteurSatPrev_currentIndexChanged(int index)
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

void Onglets::on_hauteurSoleilPrev_currentIndexChanged(int index)
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

void Onglets::on_magnitudeMaxPrev_toggled(bool checked)
{
    _ui->valMagnitudeMaxPrev->setVisible(checked);
}

/*
 * Calcul des flashs
 */
void Onglets::on_calculsFlashs_clicked()
{
    /* Declarations des variables locales */
    QVector<int> vecSat;
    ConditionsPrevisions conditions;

    /* Initialisations */
    conditions.listeSatellites.clear();

    /* Corps de la methode */
    try {

        //        for(int i = 0; i < Configuration::instance()->mapFlashs().count(); i++) {
        //            vecSat.append(i);
        //        }
        vecSat.append(0);

        if (_ui->fichierTLEMetOp->currentText().trimmed().isEmpty() || (_ui->fichierTLEMetOp->currentText() == tr("Parcourir..."))) {
            throw PreviSatException(tr("Le nom du fichier TLE n'est pas spécifié"), WARNING);
        }

        const QFileInfo fi(_ficTLEMetOp.at(_ui->fichierTLEMetOp->currentIndex()));
        if (!fi.exists()) {
            _ui->fichierTLEMetOp->removeItem(_ui->fichierTLEMetOp->currentIndex());
            throw PreviSatException(tr("Le nom du fichier TLE est incorrect"), WARNING);
        }

        // Ecart heure locale - UTC
        const bool ecart = (fabs(_date->offsetUTC() - Date::CalculOffsetUTC(_date->ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateInitialePrev->dateTime());
        const double offset2 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateFinalePrev->dateTime());

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
            const double tmp = conditions.jj2;
            conditions.jj2 = conditions.jj1;
            conditions.jj1 = tmp;
        }

        conditions.ecart = ecart;
        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = _ui->syst24h->isChecked();

        // Lieu d'observation
        conditions.observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation3->currentIndex());

        // Unites de longueur
        conditions.unite = (_ui->unitesKm->isChecked()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Conditions d'eclairement du satellite
        conditions.eclipse = _ui->illuminationPrev->isChecked();

        // Magnitude maximale
        conditions.magnitudeLimite = _ui->magnitudeMaxMetOp->value();

        // Angle limite
        conditions.angleLimite = PI;

        // Hauteur minimale du satellite
        conditions.hauteur = DEG2RAD * ((_ui->hauteurSatMetOp->currentIndex() == 5) ?
                                            abs(_ui->valHauteurSatMetOp->text().toInt()) : 5 * _ui->hauteurSatMetOp->currentIndex());

        // Hauteur maximale du Soleil
        if (_ui->hauteurSoleilMetOp->currentIndex() <= 3) {
            conditions.crepuscule = -6. * DEG2RAD * _ui->hauteurSoleilMetOp->currentIndex();
        } else if (_ui->hauteurSoleilMetOp->currentIndex() == 4) {
            conditions.crepuscule = PI_SUR_DEUX;
        } else if (_ui->hauteurSoleilMetOp->currentIndex() == 5) {
            conditions.crepuscule = DEG2RAD * _ui->valHauteurSoleilMetOp->text().toInt();
        } else {
        }

        // Prise en compte de l'extinction atmospherique
        conditions.extinction = _ui->extinctionAtmospherique->isChecked();

        // Prise en compte de la refraction atmospherique
        conditions.refraction = _ui->refractionPourEclipses->isChecked();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = _ui->effetEclipsesMagnitude->isChecked();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = _ui->eclipsesLune->isChecked();

        // Liste des satellites pouvant produire des flashs
        QStringList listeSatellites = Configuration::instance()->mapFlashs().keys();

        // Verification du fichier TLE
        if (TLE::VerifieFichier(fi.absoluteFilePath(), false) == 0) {
            const QString msg = tr("Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE");
            throw PreviSatException(msg.arg(fi.absoluteFilePath()), WARNING);
        }

        // Lecture du fichier TLE
        QMap<QString, TLE> tabtle = TLE::LectureFichier(fi.filePath(), listeSatellites);

        // Mise a jour de la liste de satellites et creation du tableau de satellites
        QMutableStringListIterator it(listeSatellites);
        while (it.hasNext()) {
            const QString norad = it.next();
            if (!tabtle.contains(norad)) {
                it.remove();
            }
        }

        // Il n'y a aucun satellite produisant des flashs dans le fichier TLE
        if (listeSatellites.size() == 0) {
            throw PreviSatException(tr("Erreur rencontrée lors de l'exécution\n" \
                                       "Aucun satellite produisant des flashs n'a été trouvé dans le fichier TLE"), WARNING);
        }

        conditions.fichier = fi.filePath();
        conditions.listeSatellites = listeSatellites;

        // Nom du fichier resultat
        const QString chaine = tr("flashs", "file name (without accent)") + "_%1_%2.txt";
        conditions.ficRes = Configuration::instance()->dirTmp() + QDir::separator() +
                chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

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
                Message::Afficher(tr("Aucun flash n'a été trouvé sur la période donnée"), INFO);
            } else {
                afficherResultats = new Afficher(FLASHS, conditions, Flashs::donnees(), Flashs::resultats(), this, _ui->valeurZoomMap->value());
                afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void Onglets::on_parametrageDefautMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_barreOnglets_currentChanged(_ui->barreOnglets->indexOf(_ui->flashs));
    _ui->hauteurSatMetOp->setCurrentIndex(2);
    _ui->hauteurSoleilMetOp->setCurrentIndex(1);
    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->lieuxObservation5->setCurrentIndex(0);
    _ui->ordreChronologiqueMetOp->setChecked(true);
    _ui->magnitudeMaxMetOp->setValue(4.);
    if (!_ui->calculsFlashs->isEnabled()) {
        _ui->calculsFlashs->setEnabled(true);
    }

    /* Retour */
    return;
}

void Onglets::on_effacerHeuresMetOp_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeMetOp->setTime(QTime(0, 0, 0));
    _ui->dateFinaleMetOp->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void Onglets::on_hauteurSatMetOp_currentIndexChanged(int index)
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

void Onglets::on_hauteurSoleilMetOp_currentIndexChanged(int index)
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

void Onglets::on_calculsTransit_clicked()
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
        const bool ecart = (fabs(_date->offsetUTC() - Date::CalculOffsetUTC(_date->ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateInitialeTransit->dateTime());
        const double offset2 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateFinaleTransit->dateTime());

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

        conditions.ecart = ecart;
        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = _ui->syst24h->isChecked();

        // Hauteur minimale du satellite
        conditions.hauteur = DEG2RAD * ((_ui->hauteurSatTransit->currentIndex() == 5) ?
                                            abs(_ui->valHauteurSatTransit->text().toInt()) : 5 * _ui->hauteurSatTransit->currentIndex());

        // Elongation maximale
        conditions.seuilConjonction = DEG2RAD * _ui->elongationMaxCorps->value();

        // Selection des corps
        conditions.calcEphemSoleil = _ui->soleilTransit->isChecked();
        conditions.calcEphemLune = _ui->luneTransit->isChecked();

        // Lieu d'observation
        conditions.observateur = Configuration::instance()->observateurs().at(_ui->lieuxObservation4->currentIndex());

        // Unites de longueur
        conditions.unite = (_ui->unitesKm->isChecked()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Prise en compte de la refraction atmospherique
        conditions.refraction = _ui->refractionPourEclipses->isChecked();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = _ui->effetEclipsesMagnitude->isChecked();

        // Calcul des transits lunaires de jour
        conditions.calcTransitLunaireJour = _ui->calcTransitLunaireJour->isChecked();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = _ui->eclipsesLune->isChecked();

        // Age maximal du TLE
        const double ageTLE = _ui->ageMaxTLETransit->value();

        // Selection du fichier d'elements orbitaux
        const QFileInfo fi1(Configuration::instance()->dirTle() + QDir::separator() + "visual.txt");
        const QFileInfo fi2(Configuration::instance()->dirTle() + QDir::separator() + "iss.3le");
        const QStringList listeSatellites(QStringList () << Configuration::instance()->noradStationSpatiale());
        QList<TLE> tabtle;

        if (fi2.exists() && (fi2.size() != 0)) {

            // Verification du fichier
            if (TLE::VerifieFichier(fi2.absoluteFilePath()) == 0) {

                const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                       "Le fichier %1 n'est pas un TLE");
                throw PreviSatException(msg.arg(fi2.absoluteFilePath()), WARNING);
            }

            // Lecture du fichier iss.3le
            tabtle = TLE::LectureFichier3le(fi2.absoluteFilePath());

            // Verification des dates
            const double datePremierTLE = tabtle.first().epoque().jourJulienUTC();
            const double dateDernierTLE = tabtle.last().epoque().jourJulienUTC();

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
                    const QList<TLE> tle(QList<TLE> () << TLE::LectureFichier(fi1.absoluteFilePath(), listeSatellites).first());

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
                            conditions.tabtle = tabtle;
                        }
                    }
                }
            } else {

                // Utilisation des TLE du fichier iss.3le
                conditions.tabtle = tabtle;
            }

            if ((age1 > (ageTLE + 0.05)) || (age2 > (ageTLE + 0.05))) {
                const QString msg = tr("L'âge du TLE de l'ISS (%1 jours) est supérieur à %2 jours");
                Message::Afficher(msg.arg(fabs(qMax(age1, age2)), 0, 'f', 1).arg(ageTLE, 0, 'f', 1), INFO);
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
                tabtle.append(TLE::LectureFichier(fi1.absoluteFilePath(), listeSatellites).first());

                if (tabtle.isEmpty()) {

                    const QString msg = tr("Erreur rencontrée lors du chargement du fichier\n" \
                                           "Le fichier %1 ne contient pas le TLE de l'ISS");
                    throw PreviSatException(msg.arg(fi1.absoluteFilePath()), WARNING);

                } else {
                    conditions.tabtle = tabtle;
                }
            } else {
                throw PreviSatException(tr("Le fichier TLE n'existe pas"), WARNING);
            }
        }

        // Nom du fichier resultat
        const QString chaine = tr("transits", "file name (without accent)") + "_%1_%2.txt";
        conditions.ficRes = Configuration::instance()->dirTmp() + QDir::separator() +
                chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

        // Barre de progression
        auto barreProgression = new QProgressBar();
        barreProgression->setAlignment(Qt::AlignHCenter);

        QProgressDialog fenetreProgression;
        fenetreProgression.setWindowTitle(tr("Calculs en cours..."));
        fenetreProgression.setCancelButtonText(tr("Annuler"));
        fenetreProgression.setBar(barreProgression);
        fenetreProgression.setWindowFlags(fenetreProgression.windowFlags() & ~Qt::WindowContextHelpButtonHint);

        // Lancement des calculs
        TransitsIss::setConditions(conditions);
        QFutureWatcher<void> calculs;

        connect(&fenetreProgression, SIGNAL(canceled()), &calculs, SLOT(cancel()));
        connect(&calculs, SIGNAL(finished()), &fenetreProgression, SLOT(reset()));
        connect(&calculs, SIGNAL(progressRangeChanged(int, int)), &fenetreProgression, SLOT(setRange(int,int)));
        connect(&calculs, SIGNAL(progressValueChanged(int)), &fenetreProgression, SLOT(setValue(int)));

        calculs.setFuture(QtConcurrent::map(vecSat, &TransitsIss::CalculTransits));

        fenetreProgression.exec();
        calculs.waitForFinished();

        if (calculs.isCanceled()) {
            TransitsIss::resultats().clear();
        } else {

            // Affichage des resultats
            emit AfficherMessageStatut(tr("Calculs terminés"), 10);

            if (TransitsIss::resultats().isEmpty()) {
                Message::Afficher(tr("Aucun transit ISS n'a été trouvé sur la période donnée"), INFO);
            } else {
                afficherResultats = new Afficher(TRANSITS, conditions, TransitsIss::donnees(), TransitsIss::resultats(), this,
                                                 _ui->valeurZoomMap->value());
                afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void Onglets::on_parametrageDefautTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_barreOnglets_currentChanged(_ui->barreOnglets->indexOf(_ui->transits_ISS));
    _ui->hauteurSatTransit->setCurrentIndex(1);
    _ui->valHauteurSatTransit->setVisible(false);
    _ui->lieuxObservation4->setCurrentIndex(0);
    _ui->ageMaxTLETransit->setValue(2.);
    _ui->elongationMaxCorps->setValue(5.);
    if (!_ui->calculsTransit->isEnabled()) {
        _ui->calculsTransit->setEnabled(true);
    }

    /* Retour */
    return;
}

void Onglets::on_effacerHeuresTransit_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeTransit->setTime(QTime(0, 0, 0));
    _ui->dateFinaleTransit->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void Onglets::on_hauteurSatTransit_currentIndexChanged(int index)
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

void Onglets::on_majTleIss_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Mise a jour du fichier d'elements orbitaux
        emit AfficherMessageStatut(tr("Téléchargement du fichier TLE de l'ISS..."));
        const QString ficTle = Configuration::instance()->adresseCelestrakNorad() + "supplemental/iss.txt";
        _dirDwn = Configuration::instance()->dirTmp();
        TelechargementFichier(ficTle, false);
        emit AfficherMessageStatut(tr("Téléchargement terminé"), 5);

        // Verification du fichier
        const QString fic = Configuration::instance()->dirTmp() + QDir::separator() + "iss.txt";
        const QString fic3le = Configuration::instance()->dirTle() + QDir::separator() + "iss.3le";
        TLE::VerifieFichier(fic);

        QFile fi(fic);
        QFile fi2(fic3le);
        if (fi2.exists()) {
            fi2.remove();
        }
        fi.rename(fic3le);

        // Mise a jour de l'age du TLE de l'ISS
        CalculAgeTLETransitISS();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Onglets::on_majManIss_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Mise a jour du fichier de manoeuvres
    emit AfficherMessageStatut(tr("Téléchargement du fichier de manoeuvres ISS..."));
    _dirDwn = Configuration::instance()->dirLocalData();
    TelechargementFichier(Configuration::instance()->adresseAstropedia() + "previsat/Qt/commun/data/ISS.OEM_J2K_EPH.xml", false);
    emit AfficherMessageStatut(tr("Téléchargement terminé"), 5);

    // Lecture du fichier de manoeuvres
    Configuration::instance()->LectureManoeuvresISS();
    if (!Configuration::instance()->evenementsISS().isEmpty()) {
        AffichageManoeuvresISS();
    }

    /* Retour */
    return;
}


/*
 * Suivi d'un satellite
 */
#if defined (Q_OS_WIN)
void Onglets::on_genererPositions_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (_ui->liste4->count() == 0) {
            throw PreviSatException();
        }

        QStringList satelliteSelectionne;
        for(int i=0; i<_ui->liste4->count(); i++) {
            if (_ui->liste4->item(i)->checkState() == Qt::Checked) {
                satelliteSelectionne.append(_ui->liste4->item(i)->data(Qt::UserRole).toString());
            }
        }

        if (satelliteSelectionne.isEmpty()) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), WARNING);
        }

        // Lecture du fichier TLE
        const QMap<QString, TLE> tle = TLE::LectureFichier(Configuration::instance()->nomfic(), satelliteSelectionne);

        // Calcul de l'intervalle de temps lorsque le satellite est au-dessus de l'horizon
        const Date date(_date->offsetUTC());
        Observateur obs =  Configuration::instance()->observateurs().at(_ui->lieuxObservation5->currentIndex());

        obs.CalculPosVit(date);

        Satellite satSuivi(tle[satelliteSelectionne.at(0)]);
        satSuivi.CalculPosVit(date);
        satSuivi.CalculCoordHoriz(obs);
        satSuivi.CalculElementsOsculateurs(date);

        // Hauteur minimale du satellite
        const double hauteurMin = DEG2RAD * ((_ui->hauteurSatSuivi->currentIndex() == 5) ?
                                                 abs(_ui->valHauteurSatSuivi->text().toInt()) : 5 * _ui->hauteurSatSuivi->currentIndex());


        int nbIter = 0;
        const ElementsAOS elementsAos = Evenements::CalculAOS(date, satSuivi, obs, true, hauteurMin);
        Date date1 = elementsAos.date;
        Date date2;

        if (elementsAos.aos) {

            if (elementsAos.typeAOS == tr("AOS", "Acquisition of signal")) {
                date2 = Evenements::CalculAOS(Date(date1.jourJulienUTC() + 10. * NB_JOUR_PAR_SEC, 0.), satSuivi, obs, true, hauteurMin).date;

            } else {
                // Le satellite est deja dans le ciel
                date2 = date1;
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

void Onglets::on_afficherSuivi_clicked()
{
    if (!_ficSuivi.isEmpty()) {
        QDesktopServices::openUrl(QUrl(_ficSuivi.replace("\\", "/")));
    }
}


void Onglets::on_liste4_itemClicked(QListWidgetItem *item)
{
    if (item != nullptr) {
        for(int i=0; i<_ui->liste4->count(); i++) {
            if ((_ui->liste4->item(i)->checkState() == Qt::Checked) && (item != _ui->liste4->item(i))) {
                _ui->liste4->item(i)->setCheckState(Qt::Unchecked);
            }
        }
        item->setSelected(true);
        item->setCheckState(Qt::Checked);
    }

    _ui->frameSatSelectionne->setVisible(false);
    CalculAosSatSuivi();

    _ui->genererPositions->setDefault(true);
    _ui->afficherSuivi->setEnabled(false);
}

void Onglets::on_liste4_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow)

    _ui->frameSatSelectionne->setVisible(false);
    if (_ui->telescope->isVisible()) {
        if ((currentRow >= 0) && (getListItemChecked(_ui->liste4) > 0)) {

            CalculAosSatSuivi();
            _ui->genererPositions->setDefault(true);
            _ui->afficherSuivi->setEnabled(false);
        }
    }
}

void Onglets::on_lieuxObservation5_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    _ui->frameSatSelectionne->setVisible(false);
    if (_ui->telescope->isVisible()) {
        if ((index >= 0) && (getListItemChecked(_ui->liste4) > 0)) {

            CalculAosSatSuivi();
            _ui->genererPositions->setDefault(true);
            _ui->afficherSuivi->setEnabled(false);
        }
    }
}

void Onglets::on_hauteurSatSuivi_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->telescope->isVisible()) {
        if (index == _ui->hauteurSatSuivi->count() - 1) {
            _ui->valHauteurSatSuivi->setText(settings.value("previsions/valHauteurSatSuivi", 0).toString());
            _ui->valHauteurSatSuivi->setVisible(true);
            _ui->valHauteurSatSuivi->setCursorPosition(0);
            _ui->valHauteurSatSuivi->setFocus();
        } else {
            _ui->valHauteurSatSuivi->setVisible(false);
        }

        CalculAosSatSuivi();
        _ui->genererPositions->setDefault(true);
        _ui->afficherSuivi->setEnabled(false);
    }

    /* Retour */
    return;
}

void Onglets::on_skywatcher_clicked()
{
    QDesktopServices::openUrl(QUrl("http://skywatcher.com/"));
}
#endif

/*
 * Calcul des evenements orbitaux
 */
void Onglets::on_calculsEvt_clicked()
{
    /* Declarations des variables locales */
    QVector<int> vecSat;
    ConditionsPrevisions conditions;

    /* Initialisations */
    int j = 0;
    conditions.listeSatellites.clear();

    /* Corps de la methode */
    try {

        if (_ui->liste3->count() == 0) {
            throw PreviSatException();
        }

        vecSat.append(0);

        for(int i = 0; i < _ui->liste3->count(); i++) {
            if (_ui->liste3->item(i)->checkState() == Qt::Checked) {
                conditions.listeSatellites.append(_ui->liste3->item(i)->data(Qt::UserRole).toString());
                //vecSat.append(j);
                j++;
            }
        }

        if (conditions.listeSatellites.isEmpty()) {
            throw PreviSatException(tr("Aucun satellite n'est sélectionné dans la liste"), WARNING);
        }

        // Ecart heure locale - UTC
        const bool ecart = (fabs(_date->offsetUTC() - Date::CalculOffsetUTC(_date->ToQDateTime(1))) > EPSDBL100);
        const double offset1 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateInitialeEvt->dateTime());
        const double offset2 = (ecart) ? _date->offsetUTC() : Date::CalculOffsetUTC(_ui->dateFinaleEvt->dateTime());

        // Date et heure initiales
        const Date date1(_ui->dateInitialeEvt->date().year(), _ui->dateInitialeEvt->date().month(), _ui->dateInitialeEvt->date().day(),
                         _ui->dateInitialeEvt->time().hour(), _ui->dateInitialeEvt->time().minute(), _ui->dateInitialeEvt->time().second(), 0.);

        // Jour julien initial
        conditions.jj1 = date1.jourJulien() - offset1;

        // Date et heure finales
        const Date date2(_ui->dateFinaleEvt->date().year(), _ui->dateFinaleEvt->date().month(), _ui->dateFinaleEvt->date().day(),
                         _ui->dateFinaleEvt->time().hour(), _ui->dateFinaleEvt->time().minute(), _ui->dateFinaleEvt->time().second(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulien() - offset2;

        // Cas ou la date finale precede la date initiale : on intervertit les dates
        if (conditions.jj1 > conditions.jj2) {
            const double tmp = conditions.jj2;
            conditions.jj2 = conditions.jj1;
            conditions.jj1 = tmp;
        }

        conditions.ecart = ecart;
        conditions.offset = offset1;

        // Systeme horaire
        conditions.systeme = _ui->syst24h->isChecked();

        // Unites de longueur
        conditions.unite = (_ui->unitesKm->isChecked()) ? tr("km", "kilometer") : tr("nmi", "nautical mile");

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = _ui->eclipsesLune->isChecked();

        // Fichier TLE
        conditions.fichier = Configuration::instance()->nomfic();

        // Nom du fichier resultat
        const QString chaine = tr("evenements", "file name (without accent)") + "_%1_%2.txt";
        conditions.ficRes = Configuration::instance()->dirTmp() + QDir::separator() +
                chaine.arg(date1.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0)).
                arg(date2.ToShortDateAMJ(FORMAT_COURT, SYSTEME_24H).remove("/").split(" ").at(0));

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
                Message::Afficher(tr("Aucun évènement n'a été trouvé sur la période donnée"), INFO);
            } else {
                afficherResultats = new Afficher(EVENEMENTS, conditions, EvenementsOrbitaux::donnees(), EvenementsOrbitaux::resultats(), this);
                afficherResultats->show();
            }
        }

    } catch (PreviSatException &) {
    }

    /* Retour */
    return;
}

void Onglets::on_parametrageDefautEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    on_ongletsOutils_currentChanged(_ui->ongletsOutils->indexOf(_ui->evenementsOrbitaux));
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

void Onglets::on_effacerHeuresEvt_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->dateInitialeEvt->setTime(QTime(0, 0, 0));
    _ui->dateFinaleEvt->setTime(QTime(0, 0, 0));

    /* Retour */
    return;
}

void Onglets::on_liste3_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)

    if (_ui->liste3->hasFocus() && (_ui->liste3->currentRow() >= 0)) {

        if (_ui->liste3->currentItem()->checkState() == Qt::Checked) {

            // Suppression d'un satellite dans la liste
            _ui->liste3->currentItem()->setCheckState(Qt::Unchecked);

        } else {

            // Ajout d'un satellite dans la liste
            _ui->liste3->currentItem()->setCheckState(Qt::Checked);
        }
    }
}

void Onglets::on_liste3_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    if (_ui->liste3->currentRow() >= 0) {
        _ui->menuContextuelListes->exec(QCursor::pos());
    }

    /* Retour */
    return;
}

void Onglets::on_rechercheCreerTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    emit EffacerMessageStatut();

    /* Corps de la methode */
    try {

        // Verification des fichiers
        QString ficlu = _ui->fichierALireCreerTLE->text();
        if (ficlu.isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à lire n'est pas spécifié"), WARNING);
        }

        QFileInfo fi(ficlu);
        if (fi.exists()) {
            if (fi.suffix() == "gz") {

                // Cas d'un fichier compresse au format gz
                //                const QString fic = dirTmp + QDir::separator() + fi.completeBaseName();

                //                if (DecompressionFichierGz(ficlu, fic)) {

                //                    const int nsat = TLE::VerifieFichier(fic, false);
                //                    if (nsat == 0) {
                //                        const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                //                        throw PreviSatException(msg.arg(ficlu), WARNING);
                //                    }
                //                    ficlu = fic;
                //                } else {
                //                    const QString msg = tr("Erreur rencontrée lors de la décompression du fichier %1");
                //                    throw PreviSatException(msg.arg(ficlu), WARNING);
                //                }
            }
        } else {
            const QString msg = tr("Le fichier %1 n'existe pas");
            throw PreviSatException(msg.arg(ficlu), WARNING);
        }

        const int nbs = TLE::VerifieFichier(ficlu, false);
        if (nbs == 0) {
            throw PreviSatException(tr("Erreur rencontrée lors du chargement du fichier %1").arg(ficlu), WARNING);
        }

        const QString ficperso = _ui->nomFichierPerso->text();
        if (ficperso.isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier personnel n'est pas spécifié"), WARNING);
        }

        QFileInfo fi2(ficperso);
        QDir di(fi2.absolutePath());
        if (!di.exists()) {
            di.mkpath(fi2.absolutePath());
        }

        if (fi.filePath() == fi2.filePath()) {
            throw PreviSatException(tr("Nom du fichier personnel et nom du fichier à lire identiques"), WARNING);
        }

        // Verification des numeros NORAD
        const QStringList valeursNorad = _ui->valeursNorad->text().split(QRegExp("(\\D+)"), QString::SkipEmptyParts);
        if (valeursNorad.size() != 2) {
            throw PreviSatException(tr("Erreur dans la saisie des numéros NORAD"), WARNING);
        }

        const int noradMin = valeursNorad.at(0).toInt();
        const int noradMax = valeursNorad.at(1).toInt();
        if ((noradMin < 1) || (noradMin > 999999) || (noradMax < 1) || (noradMax > 999999) || (noradMax < noradMin)) {
            throw PreviSatException(tr("Erreur dans la saisie des numéros NORAD"), WARNING);
        }

        // Verification du nombre de revolutions par jour
        const QStringList valeursRevParJour = _ui->valeursRevParJour->text().split(QRegExp("(\\D+).(\\D+)"), QString::SkipEmptyParts);
        if (valeursRevParJour.size() != 2) {
            throw PreviSatException(tr("Erreur dans la saisie des nombres de révolutions par jour"), WARNING);
        }

        const double revMin = valeursRevParJour.at(0).toDouble();
        const double revMax = valeursRevParJour.at(1).toDouble();
        if ((revMin < 0.) || (revMin > 18.) || (revMax < 0.) || (revMax > 18.) || (revMax < revMin)) {
            throw PreviSatException(tr("Erreur dans la saisie des nombres de révolutions par jour"), WARNING);
        }

        // Verification de l'ascension droite du noeud ascendant
        const QStringList valeursADNoeudAscendant = _ui->valeursADnoeudAscendant->text().split(QRegExp("(\\D+)"), QString::SkipEmptyParts);
        if (valeursADNoeudAscendant.size() != 2) {
            throw PreviSatException(tr("Erreur dans la saisie des ascensions droites du noeud ascendant"), WARNING);
        }

        const double adnaMin = valeursADNoeudAscendant.at(0).toDouble();
        const double adnaMax = valeursADNoeudAscendant.at(1).toDouble();
        if ((adnaMin < 0.) || (adnaMin > 359.) || (adnaMax < 0.) || (adnaMax > 360.) || (adnaMax < adnaMin)) {
            throw PreviSatException(tr("Erreur dans la saisie des ascensions droites du noeud ascendant"), WARNING);
        }

        // Verification de l'argument du perigee
        const QStringList valeursArgumentPerigee = _ui->valeursArgumentPerigee->text().split(QRegExp("(\\D+)"), QString::SkipEmptyParts);
        if (valeursArgumentPerigee.size() != 2) {
            throw PreviSatException(tr("Erreur dans la saisie des arguments du périgée"), WARNING);
        }

        const double argPerigeeMin = valeursArgumentPerigee.at(0).toDouble();
        const double argPerigeeMax = valeursArgumentPerigee.at(1).toDouble();
        if ((argPerigeeMin < 0.) || (argPerigeeMin > 359.) || (argPerigeeMax < 0.) || (argPerigeeMax > 360.) || (argPerigeeMax < argPerigeeMin)) {
            throw PreviSatException(tr("Erreur dans la saisie des arguments du périgée"), WARNING);
        }

        // Verification de l'excentricite
        const QStringList valeursExcentricite = _ui->valeursExcentricite->text().split(QRegExp("(\\D).(\\D+)"), QString::SkipEmptyParts);
        if (valeursExcentricite.size() != 2) {
            throw PreviSatException(tr("Erreur dans la saisie des excentricités"), WARNING);
        }

        const double excMin = valeursExcentricite.at(0).toDouble();
        const double excMax = valeursExcentricite.at(1).toDouble();
        if ((excMin < 0.) || (excMin > 0.9999999) || (excMax < 0.) || (excMax > 360.) || (excMax < excMin)) {
            throw PreviSatException(tr("Erreur dans la saisie des excentricités"), WARNING);
        }

        // Verification de l'inclinaison orbitale
        const QStringList valeursInclinaison1 = _ui->valeursInclinaison1->text().split(QRegExp("(\\D+)"), QString::SkipEmptyParts);
        if (valeursInclinaison1.size() != 2) {
            throw PreviSatException(tr("Erreur dans la saisie des inclinaisons (premier intervalle)"), WARNING);
        }

        const double incMin1 = valeursInclinaison1.at(0).toDouble();
        const double incMax1 = valeursInclinaison1.at(1).toDouble();
        if ((incMin1 < 0.) || (incMin1 > 179.) || (incMax1 < 0.) || (incMax1 > 180.) || (incMax1 < incMin1)) {
            throw PreviSatException(tr("Erreur dans la saisie des inclinaisons (premier intervalle)"), WARNING);
        }

        double incMin2 = 0.;
        double incMax2 = 180.;

        if (_ui->valeursInclinaison2->isVisible()) {

            const QStringList valeursInclinaison2 = _ui->valeursInclinaison2->text().split(QRegExp("(\\D+)"), QString::SkipEmptyParts);
            if (valeursInclinaison2.size() != 2) {
                throw PreviSatException(tr("Erreur dans la saisie des inclinaisons (deuxième intervalle)"), WARNING);
            }

            incMin2 = valeursInclinaison2.at(0).toDouble();
            incMax2 = valeursInclinaison2.at(1).toDouble();
            if ((incMin2 < 0.) || (incMin2 > 179.) || (incMax2 < 0.) || (incMax2 > 180.) || (incMax2 < incMin2)) {
                throw PreviSatException(tr("Erreur dans la saisie des inclinaisons (deuxième intervalle)"), WARNING);
            }
        }

        // Lecture du fichier TLE
        const QMap<QString, TLE> tabtle = TLE::LectureFichier(ficlu);

        // Ouverture du fichier personnel en ecriture
        QFile sw(ficperso);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);

        if (!sw.isWritable()) {
            const QString msg = tr("Problème de droits d'écriture du fichier %1");
            throw PreviSatException(msg.arg(sw.fileName()), WARNING);
        }
        QTextStream flux(&sw);

        bool ecritureTLE;
        double mag;

        QMapIterator<QString, TLE> it(tabtle);
        while (it.hasNext()) {
            it.next();

            const int norad = it.key().toInt();
            ecritureTLE = ((norad >= noradMin) && (norad <= noradMax));

            if (ecritureTLE) {

                const TLE tle = it.value();

                // Nombre de revolutions par jour
                ecritureTLE &= ((tle.no() >= revMin) && (tle.no() <= revMax));

                // Ascension droite du noeud ascendant
                ecritureTLE &= ((tle.omegao() >= adnaMin) && (tle.omegao() <= adnaMax));

                // Argument du perigee
                ecritureTLE &= ((tle.argpo() >= argPerigeeMin) && (tle.argpo() <= argPerigeeMax));

                // Excentricite
                ecritureTLE &= ((tle.ecco() >= excMin) && (tle.ecco() <= excMax));

                // Inclinaison orbitale
                ecritureTLE &= (((tle.inclo() >= incMin1) && (tle.inclo() <= incMax1)) ||
                                (((tle.inclo() >= incMin2) && (tle.inclo() <= incMax2)) && _ui->valeursInclinaison2->isVisible()));

                // Magnitude maximale
                if (tle.donnees().magnitudeStandard() < 99.) {
                    const double ax = RAYON_TERRESTRE * qPow(KE / (tle.no() * DEUX_PI * NB_JOUR_PAR_MIN), DEUX_TIERS);
                    mag = tle.donnees().magnitudeStandard() - 15.75 + 5. * log10(1.45 * (ax * (1. - tle.ecco()) - RAYON_TERRESTRE));
                } else {
                    mag = -10.;
                }
                ecritureTLE &= (mag <= _ui->magnitudeMaxCreerTLE->value());

                if (ecritureTLE) {

                    flux << tle.ligne0() << endl;
                    flux << tle.ligne1() << endl;
                    flux << tle.ligne2() << endl;
                }
            }
        }

        sw.close();

        if (sw.size() > 0) {
            emit AfficherMessageStatut(tr("Fichier %1 écrit").arg(ficperso), 10);
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Onglets::on_inclinaisonExtraction_currentIndexChanged(int index)
{
    _ui->valeursInclinaison2->setVisible(index == 1);
}

void Onglets::on_parametrageDefautExtraction_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->valeursNorad->setText(tr("De 000001 à 999999"));
    _ui->valeursRevParJour->setText(tr("De 00.000000000 à 18.000000000"));
    _ui->valeursADnoeudAscendant->setText(tr("De 000° à 360°"));
    _ui->valeursArgumentPerigee->setText(tr("De 000° à 360°"));
    _ui->magnitudeMaxCreerTLE->setValue(99);
    _ui->valeursInclinaison1->setText(tr("De 000° à 180°"));
    _ui->valeursInclinaison2->setText(tr("De 000° à 180°"));

    /* Retour */
    return;
}

void Onglets::on_lieuxObservation1_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->lieuxObservation1->hasFocus() && (index > 0)) {
        Configuration::instance()->setObservateurDefaut(index);
        AffichageLieuObs();
        emit RecalculerPositions();
    }

    /* Retour */
    return;
}

void Onglets::on_styleWCC_toggled(bool checked)
{
    emit RecalculerPositions();
    emit MiseAJourCarte();
}

void Onglets::on_affCerclesAcq_toggled(bool checked)
{
    emit MiseAJourCarte();
}

void Onglets::on_policeWCC_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
#if defined (Q_OS_WIN)
    const int taille = 10;
    QFont police(_ui->policeWCC->itemText(index), taille, ((_ui->policeWCC->currentIndex() == 0) ? QFont::Normal : QFont::Bold));

#elif defined (Q_OS_LINUX)
    const int taille = 11;
    QFont police(_ui->policeWCC->itemText(index), taille);

#elif defined (Q_OS_MAC)
    const int taille = 13;
    QFont police(_ui->policeWCC->itemText(index), taille, ((_ui->policeWCC->currentIndex() == 0) ? QFont::Normal : QFont::Bold));

#else
    const int taille = 11;
    QFont police(_ui->policeWCC->itemText(index), taille);
#endif

    Configuration::instance()->setPoliceWcc(police);
    emit MiseAJourCarte();

    /* Retour */
    return;
}

void Onglets::on_coulGMT_currentIndexChanged(int index)
{
    emit MiseAJourCarte();
}

void Onglets::on_affBetaWCC_toggled(bool checked)
{
    emit MiseAJourCarte();
}

void Onglets::on_affNbOrbWCC_toggled(bool checked)
{
    emit MiseAJourCarte();
}
