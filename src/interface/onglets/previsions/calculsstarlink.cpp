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
 * >    22 novembre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wshadow"
#include <QDesktopServices>
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
    settings.setValue("previsions/nbJoursGeneration", _ui->nbJoursGeneration->value());
    settings.setValue("previsions/pasGenerationStarlink", _ui->pasGeneration->currentIndex());
    settings.setValue("previsions/lieuxObservationStarlink", _ui->lieuxObservation->currentIndex());
    settings.setValue("previsions/hauteurSatStarlink", _ui->hauteurSat->currentIndex());
    settings.setValue("previsions/valHauteurSatPrev", _ui->valHauteurSat->text());
    settings.setValue("previsions/hauteurSoleilStarlink", _ui->hauteurSoleil->currentIndex());
    settings.setValue("previsions/valHauteurSoleilStarlink", _ui->valHauteurSoleil->text());

    EFFACE_OBJET(_afficherResultats);

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
    QString groupe;
    QString lancement;
    QString deploiement;
    QStringList grp;
    QMap<QString, QString> mapDates;

    /* Initialisations */
    const bool etat = _ui->groupe->blockSignals(true);
    _ui->groupe->clear();

    /* Corps de la methode */
    QMap<QString, SatellitesStarlink> &mapStarlink = Configuration::instance()->satellitesStarlink();

    // Creation d'une map des dates de lancement
    QMapIterator it(mapStarlink);
    while (it.hasNext()) {
        it.next();

        groupe = it.key();
        mapDates.insert(it.value().lancement, groupe);
    }

    // Classement des groupes en ordre decroissant des dates de lancement
    QMapIterator it2(mapDates);
    it2.toBack();
    while (it2.hasPrevious()) {
        it2.previous();

        groupe = it2.value();

        const bool isBackup = groupe.split(" ").at(1).contains("B");
        if (isBackup) {
            grp.append(groupe);
        } else {

            const QString nomGroupe = groupe.split(" ", Qt::SkipEmptyParts).at(1);
            if (Configuration::instance()->groupesStarlink().keys().contains(nomGroupe)) {

                const QString nvgroupe = groupe.split(" ", Qt::SkipEmptyParts).first() + " " + nomGroupe;
                if (nvgroupe != groupe) {
                    mapStarlink.insert(nvgroupe, mapStarlink[groupe]);
                    mapStarlink[nvgroupe].fichier = "starlink";
                    mapStarlink.remove(groupe);
                    groupe = nvgroupe;
                }
            }

            _ui->groupe->addItem(groupe);
        }
    }

    // Gestion des backup (affichage apres les dates nominales de tir)
    QString nom;
    QStringListIterator it3(grp);
    while (it3.hasNext()) {

        groupe = it3.next();
        nom = groupe;

        const QString nomGroupeBackup = groupe.split(" ").at(1);
        const QString nomGroupe = nomGroupeBackup.mid(0, nomGroupeBackup.lastIndexOf("B"));
        nom.replace(nomGroupeBackup, nomGroupe);
        const int idx = _ui->groupe->findText(nom) + 1;

        _ui->groupe->insertItem(idx, groupe);
    }

    if (_ui->groupe->count() > 0) {

        const SatellitesStarlink starlink = mapStarlink[_ui->groupe->currentText()];

        // Dates de lancement et de deploiement
        lancement = starlink.lancement;
        deploiement = starlink.deploiement;

        _ui->lancement->setText(lancement.replace("T", " ") + " " + tr("UTC"));
        _ui->deploiement->setText(deploiement.replace("T", " ") + " " + tr("UTC"));
        _ui->groupe->setPlaceholderText("");

    } else {
        _ui->groupe->setPlaceholderText(tr("Aucun groupe Starlink trouvé"));
    }

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

    _ui->nbJoursGeneration->setValue(settings.value("previsions/nbJoursGeneration", 7).toInt());
    _ui->pasGeneration->setCurrentIndex(settings.value("previsions/pasGenerationStarlink", 5).toInt());
    _ui->lieuxObservation->setCurrentIndex(settings.value("previsions/lieuxObservationStarlink", 0).toInt());
    _ui->valHauteurSat->setVisible(false);
    _ui->hauteurSat->setCurrentIndex(settings.value("previsions/hauteurSatStarlink", 0).toInt());
    _ui->valHauteurSoleil->setVisible(false);
    _ui->hauteurSoleil->setCurrentIndex(settings.value("previsions/hauteurSoleilStarlink", 1).toInt());
    _ui->ouvrirRocketLaunchLive->setVisible(!Configuration::instance()->adresseRocketLaunchLive().isEmpty());

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
    try {

        /* Declarations des variables locales */
        Date date1;
        ConditionsPrevisions conditions;
        QVector<int> vecSat;
        QMap<QString, ElementsOrbitaux> tabElements;

        /* Initialisations */
        vecSat.append(0);

        /* Corps de la methode */
        qInfo() << "Lancement des calculs de prévisions des trains de Starlink";

        // Groupe Starlink selectionne dans la liste deroulante
        const QString fichier = Configuration::instance()->satellitesStarlink()[_ui->groupe->currentText()].fichier;

        QStringList listeStarlink;
        if (fichier == "starlink") {
            const QString grp = _ui->groupe->currentText().split(" ", Qt::SkipEmptyParts).last();
            listeStarlink = Configuration::instance()->groupesStarlink()[grp];
        }

        // Elements orbitaux du train de satellites
        const QMap<QString, ElementsOrbitaux> tabElem =
            GPFormat::LectureFichier(Configuration::instance()->dirStarlink() + QDir::separator() + fichier + ".xml", "", -1, listeStarlink, true, true);

        // Cas ou les elements orbitaux des satellites sont connus
        if (fichier == "starlink") {

            tabElements = tabElem;

            // Date et heure initiales
            date1 = Date(floor(date1.jourJulienUTC() * DATE::NB_MIN_PAR_JOUR + 0.5) * DATE::NB_JOUR_PAR_MIN + DATE::EPS_DATES, 0.);

            // Ecart heure locale - UTC
            conditions.offset = Date::CalculOffsetUTC(date1.ToQDateTime(1));

        } else {

            tabElements[tabElem.firstKey()] = tabElem[tabElem.firstKey()];

            // Date et heure initiales
            QString lancement = Configuration::instance()->satellitesStarlink()[_ui->groupe->currentText()].lancement;
            const Date dateLancement = Date::ConversionDateIso(lancement.replace(" ", "T"));
            date1 = Date(floor(dateLancement.jourJulienUTC() * DATE::NB_MIN_PAR_JOUR + 0.5) * DATE::NB_JOUR_PAR_MIN + 5. * DATE::NB_JOUR_PAR_MIN +
                                 DATE::EPS_DATES, 0.);

            // Ecart heure locale - UTC
            conditions.offset = Date::CalculOffsetUTC(dateLancement.ToQDateTime(1));
        }

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
        conditions.tabElem = tabElements;
        QMapIterator it2(tabElements);
        while (it2.hasNext()) {
            it2.next();

            const QString norad = it2.key();
            conditions.tabElem[norad].donnees.setMagnitudeStandard(STARLINK::MAGNITUDE_STANDARD);
        }

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

void CalculsStarlink::on_ouvrirRocketLaunchLive_clicked()
{
    QDesktopServices::openUrl(QUrl(Configuration::instance()->adresseRocketLaunchLive()));
}

void CalculsStarlink::on_verifGpDisponibles_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    emit AfficherMessageStatut(tr("Mise à jour des éléments orbitaux..."), 10);
    _ui->groupe->setEnabled(false);
    _ui->verifGpDisponibles->setEnabled(false);
    _ui->calculs->setEnabled(false);

    emit MajElementsOrbitaux();
    emit AfficherMessageStatut(tr("Téléchargement terminé"), 10);
    _ui->groupe->setEnabled(true);
    _ui->verifGpDisponibles->setEnabled(true);
    _ui->calculs->setEnabled(true);

    /* Retour */
    return;
}
