/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre principale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 decembre 2019
 *
 * Date de revision
 * >
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QMessageBox>
#include <QSettings>
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QInputDialog>
#include "ui_onglets.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QTextStream>
#include <QToolTip>
#include "configuration/configuration.h"
#include "onglets.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/systemesolaire/terreconst.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


bool Onglets::_acalcDN;
bool Onglets::_isEclipse;
Date *Onglets::_dateEclipse;

bool Onglets::_acalcAOS;
double Onglets::_htSat;
ElementsAOS *Onglets::_elementsAOS;

QString Onglets::_donneesSat;

// Registre
static QSettings settings("Astropedia", "PreviSat");


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

    _ui->siteLancement->installEventFilter(this);
    _ui->siteLancementDonneesSat->installEventFilter(this);

    _info = true;
    _uniteVitesse = false;
    _acalcDN = true;
    _isEclipse = false;

    _acalcAOS = true;
    _htSat = 0.;

    // TODO sauvegarder les index
    _indexInfo = 0;
    _indexOption = 0;
    _titreInformations << tr("Informations satellite") << tr("Recherche données") << tr("Rentrées atmosphériques");
    _titreOptions << tr("Satellites") << tr("Système solaire, étoiles") << tr("Affichage") << tr("Système");

    // Initialisation au demarrage
    InitAffichageDemarrage();

    // Chargement des fichiers de preference
    InitFicPref(false);
    ChargementPref();

    // Chargement des fichiers d'observation
    InitFicObs(false);
    _ui->categoriesObs->setCurrentRow(0);
}

/*
 * Destructeur
 */
Onglets::~Onglets()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Onglets *Onglets::ui()
{
    return _ui;
}


/*
 * Methodes publiques
 */
void Onglets::show(const Date &date)
{
    /* Declarations des variables locales */

    /* Initialisations */
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

        if (_titreInformations.size() == 3) {
            _titreInformations.removeAt(0);
            _indexInfo = 0;
            _ui->informations->setCurrentIndex(_indexInfo);
            _ui->barreOnglets->setTabText(_ui->barreOnglets->indexOf(_ui->informationsSatellite), _titreInformations.at(0));
            on_informations_currentChanged(_indexInfo);
        }
    } else {

        if (_ui->barreOnglets->count() < 9) {
            _ui->satellite->setVisible(true);
            _ui->barreOnglets->insertTab(1, _ui->osculateurs, tr("Éléments osculateurs"));
            on_informations_currentChanged(_indexInfo);
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
        chaineUTC = tr("UTC %1 %2").arg((_date->offsetUTC() > 0.) ? "+" : "-").arg(heur.toString("hh:mm"));
        _ui->utcManuel->setText(chaineUTC);
        date = *_date;
    } else {
        chaineUTC = tr("UTC");
        _ui->utcManuel->setText(chaineUTC);
        date = Date(_date->jourJulienUTC(), 0., true);
    }

    QString chaine = QString("%1  %2").arg(date.ToLongDate((_ui->syst12h->isChecked()) ? SYSTEME_12H : SYSTEME_24H)).arg(chaineUTC);
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
    const QString unite = (_ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");
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
    _ui->ageTLE->setText(fmt.arg(satellite.ageTLE(), 0, 'f', 2).arg("jours"));

    // Longitude/Latitude/Altitude
    const QString ews = (satellite.longitude() >= 0.) ? tr("Ouest") : tr("Est");
    _ui->longitudeSat->setText(fmt.arg(Maths::ToSexagesimal(fabs(satellite.longitude()), DEGRE, 3, 0, false, true)).arg(ews));
    const QString nss = (satellite.latitude() >= 0.) ? tr("Nord") : tr("Sud");
    _ui->latitudeSat->setText(fmt.arg(Maths::ToSexagesimal(fabs(satellite.latitude()), DEGRE, 2, 0, false, true)).arg(nss));
    if (_ui->unitesKm->isChecked()) {
        _ui->altitudeSat->setText(text.sprintf("%.1f ", satellite.altitude()) + unite);
    } else {
        _ui->altitudeSat->setText(text.sprintf("%.1f ", satellite.altitude() * MILE_PAR_KM) + unite);
    }

    // Hauteur/Azimut/Distance
    _ui->hauteurSat->setText(Maths::ToSexagesimal(satellite.hauteur(), DEGRE, 2, 0, true, true));
    _ui->azimutSat->setText(Maths::ToSexagesimal(satellite.azimut(), DEGRE, 3, 0, false, true));
    if (_ui->unitesKm->isChecked()) {
        _ui->distanceSat->setText(text.sprintf("%.1f ", satellite.distance()) + unite);
    } else {
        _ui->distanceSat->setText(text.sprintf("%.1f ", satellite.distance() * MILE_PAR_KM) + unite);
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
    if (std::isnan(satellite.tle().donnees().magnitudeStandard())) {

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

                const QString magnitude = fmt1.arg(text.sprintf("%+.1f", magn)).arg(fractionIlluminee);

                QString eclipse;
                const QString fmt2 = " %1/%2";
                const QString corps = (corpsOccultant.isEmpty()) ? tr("T") : tr("L");

                if (conditionEclipse.eclipsePartielle()) {
                    eclipse = fmt2.arg(corps).arg(tr("P"));
                }

                if (conditionEclipse.eclipseAnnulaire()) {
                    eclipse = fmt2.arg(corps).arg(tr("A"));
                }

                _ui->magnitudeSat->setText(magnitude + eclipse);
            }
        } else {
            EcritureInformationsEclipse(corpsOccultant, fractionIlluminee);
        }
    }

    // Prochaine transition jour/nuit
    _acalcDN = !(_isEclipse && satellite.conditionEclipse().eclipseTotale());
    if (_acalcDN) {
        _dateEclipse = new Date(Evenements::CalculOmbrePenombre(*_date, satellite, _ui->nombreTrajectoires->value(), _ui->eclipsesLune->isChecked(),
                                                                _ui->refractionPourEclipses->isChecked()), _date->offsetUTC());
        _acalcDN = false;
        _isEclipse = satellite.conditionEclipse().eclipseTotale();
    }

    double delai = _dateEclipse->jourJulienUTC() - _date->jourJulienUTC();
    if ((delai >= -EPS_DATES) && (_dateEclipse->jourJulienUTC() < satellite.traceAuSol().last().jourJulienUTC)) {

        QString transitionJN = tr("Prochain %1 :");
        _ui->lbl_prochainJN->setText(transitionJN.arg((satellite.conditionEclipse().eclipseTotale()) ? tr("N>J") : tr("J>N")));

        // Delai de l'evenement
        transitionJN = tr("%1 (dans %2).");
        const Date delaiEcl = Date(delai - 0.5 + EPS_DATES, 0.);
        const QString cDelaiEcl = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                    delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5).replace(":", tr("h").append(" ")).append(tr("min")) :
                    delaiEcl.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5).replace(":", tr("min").append(" ")).append(tr("s"));

        _ui->dateJN->setText(transitionJN.arg(_dateEclipse->ToShortDate(FORMAT_COURT, ((_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).arg(cDelaiEcl));

        _ui->lbl_prochainJN->setVisible(true);
        _ui->dateJN->setVisible(true);
        _ui->magnitudeSat->move(333, 78);
    } else {
        _ui->lbl_prochainJN->setVisible(false);
        _ui->dateJN->setVisible(false);
        _ui->magnitudeSat->move(177, 93);
    }

    // Prochain AOS/LOS
    _acalcAOS = ((_htSat * satellite.hauteur()) <= 0.);
    if (_acalcAOS) {

        _elementsAOS = new ElementsAOS();
        *_elementsAOS = Evenements::CalculAOS(*_date, satellite, Configuration::instance()->observateur());
        _acalcAOS = false;
        _htSat = satellite.hauteur();
    }

    if (_elementsAOS->aos) {

        // Type d'evenement (AOS ou LOS)
        QString chaine = tr("Prochain %1 :");
        _ui->lbl_prochainAOS->setText(chaine.arg(_elementsAOS->typeAOS));

        // Delai de l'evenement
        chaine = tr("%1 (dans %2). Azimut : %3");
        Date dateAOS = Date(_elementsAOS->date, _date->offsetUTC());
        delai = dateAOS.jourJulienUTC() - _date->jourJulienUTC();
        const Date delaiAOS = Date(delai - 0.5 + EPS_DATES, 0.);
        const QString cDelaiAOS = (delai >= NB_JOUR_PAR_HEUR - EPS_DATES) ?
                    delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(11, 5).replace(":", tr("h").append(" ")).append(tr("min")) :
                    delaiAOS.ToShortDate(FORMAT_COURT, SYSTEME_24H).mid(14, 5).replace(":", tr("min").append(" ")).append(tr("s"));

        _ui->dateAOS->setText(chaine.arg(dateAOS.ToShortDate(FORMAT_COURT, ((_ui->syst24h->isChecked()) ? SYSTEME_24H : SYSTEME_12H))).
                              arg(cDelaiAOS).arg(Maths::ToSexagesimal(_elementsAOS->azimut, DEGRE, 3, 0, false, true).mid(0, 9)));

        _ui->lbl_prochainAOS->setVisible(true);
        _ui->dateAOS->adjustSize();
        _ui->dateAOS->resize(_ui->dateAOS->width(), 16);
        _ui->dateAOS->setVisible(true);

    } else {
        _ui->lbl_prochainAOS->setVisible(false);
        _ui->dateAOS->setVisible(false);
    }

    // Angle beta
    const QString angleBeta = tr("Beta : %1");
    _ui->lbl_beta->setText(angleBeta.arg(Maths::ToSexagesimal(satellite.beta(), DEGRE, 2, 0, false, true).mid(0, 9)));

    _ui->magnitudeSat->adjustSize();
    if (_ui->magnitudeSat->x() == 177) {

        const int posMag = _ui->magnitudeSat->x() + _ui->magnitudeSat->width();
        _ui->lbl_beta->move((posMag > 321) ? posMag + 10 : 333, _ui->dateJN->y());
        if (!_elementsAOS->aos) {
            _ui->lbl_beta->move(_ui->lbl_prochainAOS->pos());
        }

    } else {
        _ui->lbl_beta->adjustSize();
        _ui->lbl_beta->move(_ui->dateAOS->x() + _ui->dateAOS->width() - _ui->lbl_beta->width() + 2, _ui->dateJN->y());
    }

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
    _ui->distanceSoleil->setText(QString("%1 UA").arg(soleil.distanceUA(), 0, 'f', 3));

    // Ascension droite/declinaison/constellation du Soleil
    _ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.ascensionDroite(), HEURE1, 2, 0, false, true).trimmed());
    _ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.declinaison(), DEGRE, 2, 0, true, true).trimmed());
    _ui->constellationSoleil->setText(soleil.constellation());


    // Hauteur/azimut/distance de la Lune
    _ui->hauteurLune->setText(Maths::ToSexagesimal(lune.hauteur(), DEGRE, 2, 0, true, true));
    _ui->azimutLune->setText(Maths::ToSexagesimal(lune.azimut(), DEGRE, 3, 0, false, true));
    _ui->distanceLune->setText(QString("%1 %2").arg(lune.distance(), 0, 'f', 0).arg((_ui->unitesKm->isChecked()) ? tr("km") : tr("nmi")));

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
    const QString unite = (_ui->unitesKm->isChecked()) ? tr("km") : tr("nmi");

    if (_ui->typeRepere->currentIndex() == 0) {
        position = satellite.position();
    } else {
        satellite.CalculPosVitECEF(*_date, position, vitesse);
    }

    if (_ui->unitesMi->isChecked()) {
        position *= MILE_PAR_KM;
    }

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat2->setText(satellite.tle().nom());

    // Lignes du TLE
    _ui->ligne1->setText(satellite.tle().ligne1());
    _ui->ligne2->setText(satellite.tle().ligne2());

    // Position cartesienne
    _ui->xsat->setText(text.sprintf("%+.3f ", position.x()) + unite);
    _ui->ysat->setText(text.sprintf("%+.3f ", position.y()) + unite);
    _ui->zsat->setText(text.sprintf("%+.3f ", position.z()) + unite);

    // Elements osculateurs
    const QString fmt1 = "%1";
    const QString fmt2 = "%1°";
    _ui->demiGrandAxe->setText(text.sprintf("%.1f ", satellite.elements().demiGrandAxe()) + unite);

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
    apogeeAlt = (_ui->unitesKm->isChecked()) ? apogeeAlt : apogeeAlt * RAYON_TERRESTRE;
    _ui->apogee->setText(fmt3.arg(apogee, 0, 'f', 1).arg(unite).arg(apogeeAlt, 0, 'f', 1));

    const double perigee = (_ui->unitesKm->isChecked()) ? satellite.elements().perigee() : satellite.elements().perigee() * MILE_PAR_KM;
    double perigeeAlt = satellite.elements().perigee() - RAYON_TERRESTRE;
    perigeeAlt = (_ui->unitesKm->isChecked()) ? perigeeAlt : perigeeAlt * RAYON_TERRESTRE;
    _ui->perigee->setText(fmt3.arg(perigee, 0, 'f', 1).arg(unite).arg(perigeeAlt, 0, 'f', 1));

    _ui->periode->setText(Maths::ToSexagesimal(satellite.elements().periode() * HEUR2RAD, HEURE1, 1, 0, false, true));

    // Informations de signal
    _ui->doppler->setText(text.sprintf("%+.0f Hz", satellite.signal().doppler()));
    _ui->attenuation->setText(fmt1.arg(satellite.signal().attenuation(), 0, 'f', 2) + " dB");
    _ui->delai->setText(fmt1.arg(satellite.signal().delai(), 0, 'f', 2) + " ms");

    // Triplet de phasage
    const QString fmt4 = "[ %1; %2; %3 ] %4";
    const int nu0 = satellite.phasage().nu0();
    const int dt0 = satellite.phasage().dt0();
    const int ct0 = satellite.phasage().ct0();
    const int nbOrb = satellite.phasage().nbOrb();
    if ((nu0 < -98) || (dt0 < -98) || (ct0 < -98) || (nbOrb < -98)) {
        _ui->phasage->setText(tr("N/A"));
    } else {
        _ui->phasage->setText(fmt4.arg(nu0).arg(dt0).arg(ct0).arg(nbOrb));
    }

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
    _ui->lbl_revjour2->adjustSize();
    _ui->lbl_revjour2->setFixedHeight(16);
    _ui->lbl_revjour2_2->move(_ui->lbl_revjour2->x() + _ui->lbl_revjour2->width() + 1, _ui->lbl_revjour2_2->y());

    _ui->nbRev3->setText(fmt1.arg(tle.ndd60(), 0, 'f', 8));
    _ui->lbl_revjour3->adjustSize();
    _ui->lbl_revjour3->setFixedHeight(16);
    _ui->lbl_revjour3_3->move(_ui->lbl_revjour3->x() + _ui->lbl_revjour3->width() + 1, _ui->lbl_revjour3_3->y());

    // Nombre d'orbites a l'epoque
    _ui->nbOrbitesEpoque->setText(fmt1.arg(tle.nbOrbitesEpoque() + static_cast<unsigned int> (satellite.deltaNbOrb())));


    // Date de lancement
    _ui->dateLancement->setText(donnee.dateLancement());

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
    if (std::isnan(donnee.magnitudeStandard())) {
        _ui->magnitudeStdMax->setText("?/?");
    } else {

        QString text;
        const double magMax = donnee.magnitudeStandard() - 15.75
                + 5. * log10(1.45 * (satellite.elements().demiGrandAxe() * (1. - satellite.elements().excentricite()) - RAYON_TERRESTRE));
        _ui->magnitudeStdMax->setText(text.sprintf("%+.1f%c/%+.1f", donnee.magnitudeStandard(), donnee.methMagnitude(), magMax));
    }

    // Modele orbital
    _ui->modele->setText((satellite.method() == 'd') ? "SGP4 (DS)" : "SGP4 (NE)");
    _ui->modele->setToolTip((satellite.method() == 'd') ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));

    // Dimensions du satellite
    double t1 = donnee.t1();
    double t2 = donnee.t2();
    double t3 = donnee.t3();
    double section = donnee.section();
    const QString unite = (_ui->unitesKm->isChecked()) ? tr("m") : tr("ft");
    if (_ui->unitesMi->isChecked()) {
        t1 *= PIED_PAR_METRE;
        t2 *= PIED_PAR_METRE;
        t3 *= PIED_PAR_METRE;
        section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
    }

    QString dimensions;
    if ((fabs(t2) < EPSDBL100) && (fabs(t3) < EPSDBL100)) {
        const QString fmt3 = tr("Sphérique. R=%1 %2");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite);
    }
    if ((fabs(t2) >= EPSDBL100) && (fabs(t3) < EPSDBL100)) {
        const QString fmt3 = tr("Cylindrique. L=%1 %2, R=%3 %2");
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
    _ui->dimensions->adjustSize();
    _ui->dimensions->setFixedHeight(16);
    _ui->sq->move(_ui->dimensions->x() + _ui->dimensions->width() + 1, _ui->sq->y());

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
        QStringListIterator it(_resultatsSatellitesTrouves);
        while (it.hasNext()) {

            const QString item = it.next().toUpper();
            QString nomsat = item.mid(123).trimmed();

            if (nomsat.isEmpty()) {
                nomsat = item.mid(0, 5);
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

    /* Initialisations */
    const Satellite satellite = Configuration::instance()->listeSatellites().at(0);
    Vecteur3D vitesse = satellite.vitesse();
    double rangeRate = satellite.rangeRate();
    if (_ui->unitesKm->isChecked()) {
        unite = (_uniteVitesse) ? tr("km/h") : tr("km/s");
    } else {
        unite = (_uniteVitesse) ? tr("nmi/h") : tr("nmi/s");
        vitesse *= MILE_PAR_KM;
        rangeRate *= MILE_PAR_KM;
    }

    /* Corps de la methode */
    if (_uniteVitesse) {

        vitesse *= NB_SEC_PAR_HEUR;

        _ui->vitesseSat->setText(text.sprintf("%.0f ", vitesse.Norme()) + unite);
        _ui->rangeRate->setText(text.sprintf("%+.0f ", rangeRate * NB_SEC_PAR_HEUR) + unite);

        // Vitesse cartesienne
        _ui->vxsat->setText(text.sprintf("%+.3f ", vitesse.x()) + unite);
        _ui->vysat->setText(text.sprintf("%+.3f ", vitesse.y()) + unite);
        _ui->vzsat->setText(text.sprintf("%+.3f ", vitesse.z()) + unite);

    } else {
        _ui->vitesseSat->setText(text.sprintf("%.3f ", vitesse.Norme()) + unite);
        _ui->rangeRate->setText(text.sprintf("%+.3f ", rangeRate) + unite);

        // Vitesse cartesienne
        _ui->vxsat->setText(text.sprintf("%+.6f ", vitesse.x()) + unite);
        _ui->vysat->setText(text.sprintf("%+.6f ", vitesse.y()) + unite);
        _ui->vzsat->setText(text.sprintf("%+.6f ", vitesse.z()) + unite);
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
 * Chargement des preferences
 */
void Onglets::ChargementPref() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomPref = Configuration::instance()->dirPrf() + QDir::separator() + Configuration::instance()->listeFicPref().at(_ui->preferences->currentIndex());

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
        _ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 40).toInt());
        _ui->intensiteVision->setValue(settings.value("affichage/intensiteVision", 50).toInt());
        _ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
        _ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 2).toInt());
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
            const double atd = obs.altitude();

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");

            // Affichage des coordonnees
            const QString fmt = "%1 %2";
            _ui->longitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(lo), DEGRE, 3, 0, false, true)).arg(ew));
            _ui->latitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(la), DEGRE, 2, 0,false, true)).arg(ns));
            _ui->altitudeObs->setText(fmt.arg((_ui->unitesKm->isChecked()) ? atd : qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd))).
                                      arg((_ui->unitesKm->isChecked()) ? tr("m") : tr("ft")));
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
 * Affichage au demarrage
 */
void Onglets::InitAffichageDemarrage()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();

    /* Corps de la methode */
    _ui->menuBar->setVisible(false);
    _ui->barreOnglets->setCurrentIndex(0);
    _ui->ongletsOptions->setCurrentIndex(0);
    _ui->ongletsOutils->setCurrentIndex(0);
    _ui->informations->setCurrentIndex(_indexInfo);
    _ui->infoPrec->setToolTip(_titreInformations.at((_indexInfo + _ui->informations->count() - 1) % _ui->informations->count()));
    _ui->infoSuiv->setToolTip(_titreInformations.at((_indexInfo + 1) % _ui->informations->count()));

    _ui->configuration->setCurrentIndex(_indexOption);
    _ui->optionPrec->setToolTip(_titreOptions.at((_indexOption + 3) % 4));
    _ui->optionSuiv->setToolTip(_titreOptions.at((_indexOption + 1) % 4));

#if !defined (Q_OS_WIN)
    _ui->grpVecteurEtat->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
    _ui->grpElementsOsculateurs->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
    _ui->grpSignal->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 2px;}");
#endif

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
    _ui->ageTLE->move(_ui->lbl_ageTLE->x() + _ui->lbl_ageTLE->width() + 7, _ui->ageTLE->y());

    _ui->lbl_nbOrbitesSat->adjustSize();
    _ui->lbl_nbOrbitesSat->resize(_ui->lbl_nbOrbitesSat->width(), 16);
    _ui->nbOrbitesSat->move(_ui->lbl_nbOrbitesSat->x() + _ui->lbl_nbOrbitesSat->width() + 2, _ui->nbOrbitesSat->y());

    _ui->lbl_prochainAOS->adjustSize();
    _ui->lbl_prochainAOS->resize(_ui->lbl_prochainAOS->width(), 16);
    _ui->dateAOS->move(_ui->lbl_prochainAOS->x() + _ui->lbl_prochainAOS->width() + 7, _ui->dateAOS->y());
    _ui->dateJN->move(_ui->dateAOS->x(), _ui->dateJN->y());
    _ui->lbl_beta->move(_ui->dateAOS->x() + _ui->dateAOS->width() - _ui->lbl_beta->width() + 2, _ui->dateJN->y());

    _ui->ajoutLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowRight));
    _ui->supprLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowLeft));

    const QRegExpValidator *valLon = new QRegExpValidator(QRegExp("((0\\d\\d|1[0-7]\\d)°[0-5]\\d'[0-5]\\d\"|180°0?0'0?0\")"));
    _ui->nvLongitude->setValidator(valLon);

    const QRegExpValidator *valLat = new QRegExpValidator(QRegExp("((0\\d|[0-8]\\d)°[0-5]\\d'[0-5]\\d\"|90°0?0'0?0\")"));
    _ui->nvLatitude->setValidator(valLat);

    const QString unite = (_ui->unitesKm->isChecked()) ? tr("m") : tr("ft");
    QIntValidator *valAlt;
    if (_ui->unitesKm->isChecked()) {
        valAlt = new QIntValidator(-500, 8900);
        _ui->nvAltitude->setValidator(valAlt);
    } else {
        valAlt = new QIntValidator(-1640, 29200);
        _ui->nvAltitude->setValidator(valAlt);
    }
    const QString fmt = tr("L'altitude doit être comprise entre %1%2 et %3%2");
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

    _ui->numeroNORADCreerTLE->setCurrentIndex(0);
    _ui->ADNoeudAscendantCreerTLE->setCurrentIndex(0);
    _ui->excentriciteCreerTLE->setCurrentIndex(0);
    _ui->inclinaisonCreerTLE->setCurrentIndex(0);
    _ui->argumentPerigeeCreerTLE->setCurrentIndex(0);
    _ui->fichierTelechargement->setText("");
    _ui->barreProgression->setValue(0);
    _ui->frameBarreProgression->setVisible(false);
    _ui->compteRenduMaj->setVisible(false);
    _ui->frameADNA->setVisible(false);
    _ui->frameArgumentPerigee->setVisible(false);
    _ui->frameExcentricite->setVisible(false);
    _ui->frameIncl->setVisible(false);
    _ui->frameNORAD->setVisible(false);

    _ui->passageApogee->setChecked(settings.value("previsions/passageApogee", true).toBool());
    _ui->passageNoeuds->setChecked(settings.value("previsions/passageNoeuds", true).toBool());
    _ui->passageOmbre->setChecked(settings.value("previsions/passageOmbre", true).toBool());
    _ui->passageQuadrangles->setChecked(settings.value("previsions/passageQuadrangles", true).toBool());
    _ui->transitionJourNuit->setChecked(settings.value("previsions/transitionJourNuit", true).toBool());

    _ui->valHauteurSatTransit->setVisible(false);
    _ui->manoeuvresISS->setVisible(false);
    _ui->hauteurSatTransit->setCurrentIndex(settings.value("previsions/hauteurSatTransit", 1).toInt());
    _ui->lieuxObservation4->setCurrentIndex(settings.value("previsions/lieuxObservation4", 0).toInt());
    _ui->ageMaxTLETransit->setValue(settings.value("previsions/ageMaxTLETransit", 2.).toDouble());
    _ui->elongationMaxCorps->setValue(settings.value("previsions/elongationMaxCorps", 5.).toDouble());

    _ui->valHauteurSatMetOp->setVisible(false);
    _ui->hauteurSatMetOp->setCurrentIndex(settings.value("previsions/hauteurSatMetOp", 2).toInt());
    _ui->valHauteurSoleilMetOp->setVisible(false);
    _ui->hauteurSoleilMetOp->setCurrentIndex(settings.value("previsions/hauteurSoleilMetOp", 1).toInt());
    _ui->lieuxObservation5->setCurrentIndex(settings.value("previsions/lieuxObservation5", 0).toInt());
    _ui->ordreChronologiqueMetOp->setChecked(settings.value("previsions/ordreChronologiqueMetOp", true).toBool());
    _ui->magnitudeMaxMetOp->setValue(settings.value("previsions/magnitudeMaxMetOp", 2.).toDouble());
    if (settings.value("previsions/affichage3lignesMetOp", true).toBool()) {
        _ui->affichage3lignesMetOp->setChecked(true);
    } else {
        _ui->affichage1ligneMetOp->setChecked(true);
    }

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

    /* Retour */
    return;
}

/*
 * Chargement de la liste de fichiers de preferences
 */
void Onglets::InitFicPref(const bool majAff) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->preferences->clear();

    QStringListIterator it(Configuration::instance()->listeFicPref());
    while (it.hasNext()) {

        const QString fic = Configuration::instance()->dirPrf() + QDir::separator() + it.next();
        const QFileInfo fi(fic);
        QString fichier = fi.completeBaseName();
        fichier[0] = fichier[0].toUpper();

        _ui->preferences->addItem((fi.completeBaseName() == "defaut") ? tr("* Défaut") : fichier);
        if (settings.value("fichier/preferences", Configuration::instance()->dirPrf() + QDir::separator() + "defaut").toString() == fic) {
            _ui->preferences->setCurrentIndex(_ui->preferences->count() - 1);
        }
    }

    _ui->preferences->addItem(tr("Enregistrer sous..."));
    if (majAff) {
        //            AffichageCourbes();
    }
    _ui->preferences->addItem(tr("Supprimer..."));

    /* Retour */
    return;
}

bool Onglets::eventFilter(QObject *object, QEvent *evt)
{
    Q_UNUSED(object)

    if (evt->type() == QEvent::MouseMove) {

        // Affichage du site de lancement
        if (_ui->siteLancement->underMouse() || _ui->siteLancementDonneesSat->underMouse()) {

            const QString acronyme = Configuration::instance()->listeSatellites().at(0).tle().donnees().siteLancement();
            const Observateur site = Configuration::instance()->mapSites()[acronyme];

            _ui->siteLancement->setToolTip(site.nomlieu());
            _ui->siteLancementDonneesSat->setToolTip(site.nomlieu());
            emit AffichageSiteLancement(acronyme, site);
        } else {
            emit AffichageSiteLancement("", Observateur());
        }
    }

    return false;
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
    if ((_ui->vitesseSat->underMouse() && _ui->vitesseSat->isVisible()) || (_ui->rangeRate->underMouse() && _ui->rangeRate->isVisible()) ||
            (_ui->vxsat->underMouse() && _ui->vxsat->isVisible()) || (_ui->vysat->underMouse() && _ui->vysat->isVisible()) ||
            (_ui->vzsat->underMouse() && _ui->vzsat->isVisible())) {
        _uniteVitesse = !_uniteVitesse;
        AffichageVitesses();
    }
}

void Onglets::on_liste2_itemClicked(QListWidgetItem *item)
{
    const QString norad = item->data(Qt::UserRole).toString();
}

void Onglets::on_calculsPrev_clicked()
{
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
            _ui->infoSuiv->setToolTip(_titreInformations.at(1));
        } else {
            _ui->infoPrec->setVisible(true);
            _ui->infoPrec->setToolTip(_titreInformations.at(0));
            _ui->infoSuiv->setVisible(false);
        }
    } else {
        _ui->infoPrec->setVisible(true);
        _ui->infoSuiv->setVisible(true);
        _ui->infoPrec->setToolTip(_titreInformations.at((arg1 + _ui->informations->count() - 1) % _ui->informations->count()));
        _ui->infoSuiv->setToolTip(_titreInformations.at((arg1 + 1) % _ui->informations->count()));
    }

    // Recherche donnees satellite
    if (arg1 == _ui->informations->indexOf(_ui->recherche)) {

        _ui->noradDonneesSat->setValue(0);

        // Lecture du fichier donnees.sat en entier
        if (_donneesSat.isEmpty()) {

            const QString fic = Configuration::instance()->dirLocalData() + QDir::separator() + "donnees.sat";

            QFile fi(fic);
            if (fi.exists() && (fi.size() != 0)) {
                fi.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream flux(&fi);
                _donneesSat = flux.readAll().toLower();
            } else {
                _donneesSat = "";
            }
            fi.close();
        }

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

void Onglets::on_infoPrec_clicked()
{
    if (_ui->informations->isVisible()) {
        _indexInfo = (_ui->informations->currentIndex() + _ui->informations->count() - 1) % _ui->informations->count();
        _ui->informations->setCurrentIndex(_indexInfo);
        _ui->barreOnglets->setTabText(2, _titreInformations.at(_indexInfo));
        on_informations_currentChanged(_indexInfo);
    }
}

void Onglets::on_infoSuiv_clicked()
{
    if (_ui->informations->isVisible()) {
        _indexInfo = (_ui->informations->currentIndex() + 1) % _ui->informations->count();
        _ui->informations->setCurrentIndex(_indexInfo);
        _ui->barreOnglets->setTabText(2, _titreInformations.at(_indexInfo));
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
        _resultatsSatellitesTrouves.clear();

        // Recherche dans le tableau de donnees a partir du nom de l'objet
        do {
            indx1 = _donneesSat.indexOf(nomsat.toLower().trimmed(), indx1 + indx2);
            if (indx1 >= 0) {

                int indx3 = _donneesSat.lastIndexOf("\n", indx1) + 1;
                indx2 = _donneesSat.indexOf("\n", indx3) - indx3;
                if ((indx1 - indx3) >= 123) {

                    const QString ligne = _donneesSat.mid(indx3, indx2);
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
            _ui->cosparDonneesSat->setText(_resultatsSatellitesTrouves.at(0).mid(6, 11).toUpper().trimmed());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(_resultatsSatellitesTrouves.at(0).mid(0, 5).toInt());
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
        int indx1 = 0;
        const QString norad = QString("%1 ").arg(arg1, 5, 10, QChar('0'));

        // Recherche dans le tableau de donnees a partir du numero NORAD
        do {
            indx1 = _donneesSat.indexOf(norad, indx1 + 1);
        } while ((indx1 >= 0) && (_donneesSat.at(qMax(0, indx1 - 1)) != '\n'));

        if (indx1 >= 0) {

            int indx2 = _donneesSat.indexOf("\n", indx1) - indx1;
            const QString ligne = _donneesSat.mid(indx1, indx2).trimmed();
            if (!ligne.isEmpty()) {
                _resultatsSatellitesTrouves.append(ligne);
            }

            QString nomsat = ligne.mid(123).trimmed();
            if (nomsat == "iss (zarya)") {
                nomsat = "ISS";
            }

            _ui->nom->setText(nomsat.toUpper());
            _ui->cosparDonneesSat->setText(ligne.mid(6, 11).toUpper().trimmed());
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

        // Recherche dans le tableau de donnees a partir de la designation COSPAR
        do {
            indx1 = _donneesSat.indexOf(_ui->cosparDonneesSat->text().toLower().trimmed(), indx1 + indx2);
            if ((indx1 >= 0) && (_donneesSat.at(qMax(0, indx1 - 7)) == '\n')) {

                indx1 = _donneesSat.lastIndexOf("\n", indx1) + 1;
                indx2 = _donneesSat.indexOf("\n", indx1) - indx1;
                const QString ligne = _donneesSat.mid(indx1, indx2);
                if (!ligne.isEmpty()) {
                    _resultatsSatellitesTrouves.append(ligne);
                }
            }
        } while (indx1 >= 0);

        if (!_resultatsSatellitesTrouves.isEmpty()) {
            _ui->nom->setText(_resultatsSatellitesTrouves.at(0).mid(123).toUpper().trimmed());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(_resultatsSatellitesTrouves.at(0).mid(0, 5).toInt());
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
        const double magnitudeStandard = ligne.mid(34, 4).toDouble();

        const QString dateRentree = ligne.mid(59, 10).trimmed();
        const QString periode = ligne.mid(70, 10).trimmed();
        double perigee = ligne.mid(81, 7).trimmed().toDouble();
        double apogee = ligne.mid(89, 7).trimmed().toDouble();

        double ap = apogee + RAYON_TERRESTRE;
        double per = perigee + RAYON_TERRESTRE;

        // Nom du satellite
        QString nomsat = ligne.mid(123).trimmed();
        if (nomsat.toLower() == "iss (zarya)") {
            nomsat = "ISS";
        }
        _ui->nomsat->setText((nomsat.isEmpty()) ? tr("Inconnu") : nomsat);

        // Numero NORAD
        const QString norad = ligne.mid(0, 5);
        _ui->numNorad->setText(norad);

        // Designation COSPAR
        const QString cospar = ligne.mid(6, 11).trimmed();
        _ui->desigCospar->setText((cospar.isEmpty()) ? tr("Inconnue") : cospar);

        // Magnitude standard/maximale
        if ((magnitudeStandard > 98.) || (perigee < EPSDBL100) || (apogee < EPSDBL100)) {
            _ui->magnitudeStdMaxDonneesSat->setText("?/?");
        } else {

            // Estimation de la magnitude maximale
            const double demiGrandAxe = 0.5 * (ap + per);
            const double exc = 2. * ap / (ap + per) - 1.;
            const double magMax = magnitudeStandard - 15.75 + 5. * log10(1.45 * (demiGrandAxe * (1. - exc) - RAYON_TERRESTRE));
            char methMagnitude = ligne.at(39).toLower().toLatin1();

            QString text;
            _ui->magnitudeStdMaxDonneesSat->setText(text.sprintf("%+.1f%c/%+.1f", magnitudeStandard, methMagnitude, magMax));
        }

        // Modele orbital
        const bool modeleDS = (periode.toDouble() > 225.);
        const QString modele = (modeleDS) ? tr("SGP4 (DS)") : tr("SGP4 (NE)");
        _ui->modeleDonneesSat->setText((periode.isEmpty()) ? tr("Non applicable") : modele);
        if (!periode.isEmpty()) {
            _ui->modeleDonneesSat->adjustSize();
            _ui->modeleDonneesSat->setFixedHeight(16);
            _ui->modele->setToolTip((modeleDS) ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));
        }

        // Dimensions du satellite
        double t1 = ligne.mid(18, 5).toDouble();
        double t2 = ligne.mid(24, 4).toDouble();
        double t3 = ligne.mid(29, 4).toDouble();
        double section = ligne.mid(41, 6).toDouble();
        QString unite1 = tr("m");
        QString unite2 = tr("km");
        if (_ui->unitesMi->isChecked()) {

            apogee *= MILE_PAR_KM;
            perigee *= MILE_PAR_KM;
            ap *= MILE_PAR_KM;
            per *= MILE_PAR_KM;

            t1 *= PIED_PAR_METRE;
            t2 *= PIED_PAR_METRE;
            t3 *= PIED_PAR_METRE;
            section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
            unite1 = tr("ft");
            unite2 = tr("nmi");
        }

        QString dimensions;
        if ((fabs(t2) < EPSDBL100) && (fabs(t3) < EPSDBL100)) {
            const QString fmt3 = tr("Sphérique. R=%1 %2");
            dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite1);
        }
        if ((fabs(t2) >= EPSDBL100) && (fabs(t3) < EPSDBL100)) {
            const QString fmt3 = tr("Cylindrique. L=%1 %2, R=%3 %2");
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
        _ui->dimensionsDonneesSat->adjustSize();
        _ui->dimensionsDonneesSat->setFixedHeight(16);
        _ui->sqDonneesSat->move(_ui->dimensionsDonneesSat->x() + _ui->dimensionsDonneesSat->width() + 1, _ui->sqDonneesSat->y());

        // Apogee/perigee/periode orbitale
        const QString fmt = "%1 %2 (%3 %2)";
        if (fabs(apogee) < EPSDBL100) {
            _ui->apogeeDonneesSat->setText(tr("Inconnu"));
        } else {
            _ui->apogeeDonneesSat->setText(fmt.arg(apogee, 0, 'f', 0).arg(unite2).arg(ap, 0, 'f', 0));
        }

        if (fabs(perigee) < EPSDBL100) {
            _ui->perigeeDonneesSat->setText(tr("Inconnu"));
        } else {
            _ui->perigeeDonneesSat->setText(fmt.arg(perigee, 0, 'f', 0).arg(unite2).arg(per, 0, 'f', 0));
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

        _ui->dateLancementDonneesSat->setText((dateLancement.isEmpty()) ? tr("Inconnue") : date_lancement.ToShortDate(FORMAT_COURT, SYSTEME_24H).left(10));

        // Date de rentree
        if (dateRentree.isEmpty()) {
            _ui->lbl_dateRentree->setVisible(false);
            _ui->dateRentree->setVisible(false);

            _ui->lbl_categorieOrbiteDonneesSat->move(0, 70);
            _ui->categorieOrbiteDonneesSat->move(111, 70);

            _ui->lbl_paysDonneesSat->move(0, 85);
            _ui->paysDonneesSat->move(111, 85);

            _ui->lbl_siteLancementDonneesSat->move(0, 100);
            _ui->siteLancementDonneesSat->move(111, 100);
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
        const QString categorie = ligne.mid(104, 6).trimmed();
        _ui->categorieOrbiteDonneesSat->setText((categorie.isEmpty()) ? tr("Inconnue") : categorie);

        // Pays/Organisation
        const QString pays = ligne.mid(111, 5).trimmed();
        _ui->paysDonneesSat->setText((pays.isEmpty()) ? tr("Inconnu") : pays);

        // Site de lancement
        const QString siteLancement = ligne.mid(117, 5).trimmed();
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
            while (!flux.atEnd() && !atr) {

                const QString lig = flux.readLine();
                if (lig.mid(0, 2) == "1 ") {
                    if (lig.mid(2, 5) == norad) {
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

    QMessageBox msgbox(tr("Information"), tr("Voulez-vous vraiment supprimer la catégorie \"%1\"?").arg(categorie), QMessageBox::Question, QMessageBox::Yes,
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

    _ui->lbl_nvUnite->setText((_ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
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
            flux << text.sprintf("%+13.9f %+12.9f %04d ", x1, x1, atd) + nomlieu << endl;
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
        const QString ligne = text.sprintf("%+13.9f %+12.9f %04d ", lieu.longitude(), lieu.latitude(), static_cast<int> (lieu.altitude())) + nom;

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
    _ui->lbl_nvUnite->setText((_ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
    _ligneCoord.lon = _ui->nvLongitude->text();
    _ligneCoord.lat = _ui->nvLatitude->text();
    _ligneCoord.alt = _ui->nvAltitude->text();

    _ui->lbl_ajouterDans->setVisible(false);
    _ui->ajdfic->setVisible(false);

    // Suppression du lieu du fichier
    QString text;
    const Observateur lieu = _mapObs[_ui->lieuxObs->currentItem()->text()];
    const QString ligne = text.sprintf("%+13.9f %+12.9f %04d ", lieu.longitude(), lieu.latitude(), static_cast<int> (lieu.altitude())) + lieu.nomlieu();

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
                                      QLineEdit::Normal, _ui->lieuxObs->item(_ui->lieuxObs->currentRow())->text(), Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    if (!nvNomLieu.trimmed().isEmpty()) {

        const QString dirCoord = Configuration::instance()->dirCoord();
        const QString fic = dirCoord + QDir::separator() + ((_ui->categoriesObs->currentRow() == 0) ? "preferes" : _ui->categoriesObs->currentItem()->text());

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
        const QString ligne = text.sprintf("%+13.9f %+12.9f %04d ", lieu.longitude(), lieu.latitude(), static_cast<int> (lieu.altitude())) + lieu.nomlieu();

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
        }

        _ui->selecLieux->addItem(_ui->lieuxObs->currentItem()->text());
        Configuration::instance()->observateurs().append(_mapObs[_ui->lieuxObs->currentItem()->text()]);
        Configuration::instance()->EcritureConfiguration();
        _ui->lieuxObs->setFocus();

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
        _ui->selecLieux->removeItemWidget(_ui->selecLieux->currentItem());
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
