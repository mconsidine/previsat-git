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
 * >    calculsstarlink.cpp
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 septembre 2023
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
#include "ui_calculsstarlink.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "calculsstarlink.h"
#include "configuration/configuration.h"
#include "interface/afficherresultats.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"
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
CalculsStarlink::CalculsStarlink(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::CalculsStarlink)
{
    _ui->setupUi(this);

    try {

        _afficherResultats = nullptr;

        Initialisation();

    } catch (PreviSatException const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
CalculsStarlink::~CalculsStarlink()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::CalculsStarlink *CalculsStarlink::ui() const
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
void CalculsStarlink::show()
{
    /* Declarations des variables locales */
    QString lancement;
    QString deploiement;

    /* Initialisations */
    const bool etat = _ui->groupe->blockSignals(true);
    _ui->groupe->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->satellitesStarlink().keys());
    while (it.hasNext()) {

        const QString groupe = it.next();
        _ui->groupe->addItem(groupe);
    }

    const SatellitesStarlink starlink = Configuration::instance()->satellitesStarlink()[_ui->groupe->currentText()];

    // Dates de lancement et de deploiement
    lancement = starlink.lancement;
    deploiement = starlink.deploiement;

    _ui->lancement->setText(lancement.replace("T", " ") + " " + tr("UTC"));
    _ui->deploiement->setText(deploiement.replace("T", " ") + " " + tr("UTC"));

    _ui->frame_starlink->setVisible(_ui->groupe->count() > 0);
    _ui->groupe->blockSignals(etat);

    /* Retour */
    return;
}

/*
 * Affichage des lieux d'observation dans la liste deroulante
 */
void CalculsStarlink::AffichageLieuObs()
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

void CalculsStarlink::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
        show();
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
 * Initialisation Initialisation de la classe CalculsStarlink
 */
void CalculsStarlink::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->pasGeneration->setCurrentIndex(settings.value("previsions/pasGenerationStarlink", 5).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationStarlink", 0).toInt());
    _ui->valHauteurSat->setVisible(false);
    _ui->hauteurSat->setCurrentIndex(settings.value("previsions/hauteurSatStarlink", 0).toInt());
    _ui->valHauteurSoleil->setVisible(false);
    _ui->hauteurSoleil->setCurrentIndex(settings.value("previsions/hauteurSoleilStarlink", 1).toInt());

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void CalculsStarlink::on_groupe_currentTextChanged(const QString &arg1)
{
    if (!arg1.isEmpty()) {

        const SatellitesStarlink starlink = Configuration::instance()->satellitesStarlink()[arg1];

        QString lancement = starlink.lancement;
        QString deploiement = starlink.deploiement;

        _ui->lancement->setText(lancement.replace("T", " ") + " " + tr("UTC"));
        _ui->deploiement->setText(deploiement.replace("T", " ") + " " + tr("UTC"));
    }
}

void CalculsStarlink::on_parametrageDefaut_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->nbJoursGeneration->setValue(7);
    _ui->pasGeneration->setCurrentIndex(5);
    _ui->lieuxObservation->setCurrentIndex(0);
    _ui->hauteurSat->setCurrentIndex(0);
    _ui->hauteurSoleil->setCurrentIndex(1);
    _ui->valHauteurSat->setVisible(false);
    _ui->valHauteurSoleil->setVisible(false);
    if (!_ui->calculs->isEnabled()) {
        _ui->calculs->setEnabled(true);
    }

    /* Retour */
    return;
}

void CalculsStarlink::on_hauteurSoleil_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSoleil->count() - 1) {
        _ui->valHauteurSoleil->setText(settings.value("previsions/valHauteurSoleilStarlink", 0).toString());
        _ui->valHauteurSoleil->setVisible(true);
        _ui->valHauteurSoleil->setCursorPosition(0);
        _ui->valHauteurSoleil->setFocus();
    } else {
        _ui->valHauteurSoleil->setVisible(false);
    }

    /* Retour */
    return;
}

void CalculsStarlink::on_hauteurSat_currentIndexChanged(int index)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (index == _ui->hauteurSat->count() - 1) {
        _ui->valHauteurSat->setText(settings.value("previsions/valHauteurSatStarlink", 0).toString());
        _ui->valHauteurSat->setVisible(true);
        _ui->valHauteurSat->setCursorPosition(0);
        _ui->valHauteurSat->setFocus();
    } else {
        _ui->valHauteurSat->setVisible(false);
    }

    /* Retour */
    return;
}

void CalculsStarlink::on_calculs_clicked()
{
    /* Declarations des variables locales */
    ConditionsPrevisions conditions;
    QVector<int> vecSat;

    /* Initialisations */
    vecSat.append(0);

    /* Corps de la methode */
    try {

        qInfo() << "Lancement des calculs de prévisions des trains de Starlink";

        // Groupe Starlink selectionne dans la liste deroulante
        const QString fichier = Configuration::instance()->satellitesStarlink()[_ui->groupe->currentText()].fichier;

        // Elements orbitaux du train de satellites
        const QMap<QString, ElementsOrbitaux> tabElem =
            GPFormat::LectureFichier(Configuration::instance()->dirStarlink() + QDir::separator() + fichier + ".xml", "", -1, QStringList(), true, true);

        // Date et heure initiales
        QString lancement = Configuration::instance()->satellitesStarlink()[_ui->groupe->currentText()].lancement;
        const Date dateLancement = Date::ConversionDateIso(lancement.replace(" ", "T"));
        const Date date1(dateLancement.jourJulienUTC() + 5. * DATE::NB_JOUR_PAR_MIN, 0.);

        // Ecart heure locale - UTC
        conditions.offset = Date::CalculOffsetUTC(dateLancement.ToQDateTime(1));

        // Jour julien initial
        conditions.jj1 = date1.jourJulienUTC();

        // Date et heure finales
        const Date date2(date1.jourJulienUTC() + _ui->nbJoursGeneration->value(), 0.);

        // Jour julien final
        conditions.jj2 = date2.jourJulienUTC();

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
        conditions.eclipse = true;

        // Hauteur minimale du satellite
        conditions.hauteur = MATHS::DEG2RAD * ((_ui->hauteurSat->currentIndex() == 5) ?
                                                   abs(_ui->valHauteurSat->text().toInt()) : 5 * _ui->hauteurSat->currentIndex());

        // Hauteur maximale du Soleil
        if (_ui->hauteurSoleil->currentIndex() <= 3) {
            conditions.crepuscule = -6. * MATHS::DEG2RAD * _ui->hauteurSoleil->currentIndex();
        } else if (_ui->hauteurSoleil->currentIndex() == 4) {
            conditions.crepuscule = MATHS::PI_SUR_DEUX;
        } else if (_ui->hauteurSoleil->currentIndex() == 5) {
            conditions.crepuscule = MATHS::DEG2RAD * _ui->valHauteurSoleil->text().toInt();
        }

        // Magnitude limite des satellites
        conditions.magnitudeLimite = STARLINK::MAGNITUDE_LIMITE;

        // Prise en compte de l'extinction atmospherique
        conditions.extinction = settings.value("affichage/extinctionAtmospherique").toBool();

        // Prise en compte de la refraction atmospherique
        conditions.refraction = settings.value("affichage/refractionAtmospherique").toBool();

        // Prise en compte de l'effet des eclipses partielles sur la magnitude
        conditions.effetEclipsePartielle = settings.value("affichage/effetEclipsesMagnitude").toBool();

        // Prise en compte des eclipses de Lune
        conditions.calcEclipseLune = settings.value("affichage/eclipsesLune").toBool();

        // Elements orbitaux du train
        conditions.tabElem[tabElem.firstKey()] = tabElem[tabElem.firstKey()];
        conditions.tabElem.first().donnees.setMagnitudeStandard(STARLINK::MAGNITUDE_STANDARD);


        // Ecriture des informations de previsions des trains Starlink dans le fichier de log
        qInfo() << "--";
        qInfo() << "Calcul des prévisions de passage des trains Starlink :";

        qInfo() << "Date de début =" << date1.ToShortDateAMJ(DateFormat::FORMAT_COURT, DateSysteme::SYSTEME_24H).trimmed();
        qInfo() << "Ecart UTC date1 =" << conditions.offset;
        qInfo() << "Pas de génération =" << conditions.pas;

        qInfo() << QString("Lieu d'observation : %1 %2 %3")
                       .arg(conditions.observateur.longitude() * MATHS::RAD2DEG, 0, 'f', 9)
                       .arg(conditions.observateur.latitude() * MATHS::RAD2DEG, 0, 'f', 9)
                       .arg(conditions.observateur.altitude() * 1.e3);

        qInfo() << "Unité de longueur =" << conditions.unite;
        qInfo() << "Conditions d'eclairement du satellite =" << conditions.eclipse;
        qInfo() << "Hauteur minimale du satellite =" << conditions.hauteur;
        qInfo() << "Hauteur maximale du Soleil = " << conditions.crepuscule;
        qInfo() << "Prise en compte de l'extinction atmospherique =" << conditions.extinction;
        qInfo() << "Prise en compte de la refraction atmospherique =" << conditions.refraction;
        qInfo() << "Prise en compte de l'effet des eclipses partielles sur la magnitude =" << conditions.effetEclipsePartielle;
        qInfo() << "Prise en compte des eclipses de Lune =" << conditions.calcEclipseLune;
        qInfo() << "Groupe Starlink =" << _ui->groupe->currentText();

        qInfo() << "--";

        // Nom du fichier resultat
        const QString chaine = tr("starlink", "filename (without accent)") + "_%1_%2.txt";
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
                _afficherResultats = new AfficherResultats(TypeCalcul::STARLINK, conditions, Prevision::donnees(), Prevision::resultats());
                _afficherResultats->show();
            }
        }

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}
