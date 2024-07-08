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
 * >    recherchesatellite.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
 *
 * Date de revision
 * >    8 juillet 2024
 *
 */

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QToolTip>
#include "ui_recherchesatellite.h"
#include "configuration/configuration.h"
#include "recherchesatellite.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/exception.h"
#include "librairies/maths/maths.h"


// Registre
#if (PORTABLE_BUILD)
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
RechercheSatellite::RechercheSatellite(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::RechercheSatellite)
{
    _ui->setupUi(this);
    _ui->siteLancementDonneesSat->installEventFilter(this);
    _ui->paysDonneesSat->installEventFilter(this);
    _ui->categorieOrbiteDonneesSat->installEventFilter(this);
}


/*
 * Destructeur
 */
RechercheSatellite::~RechercheSatellite()
{
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
 * Sauvegarde des donnees de l'onglet
 */
void RechercheSatellite::SauveOngletRecherche(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fichier);
        if (!sw.open(QIODevice::WriteOnly | QIODevice::Text) || !sw.isWritable()) {

            qWarning() << "Problème de droits d'écriture du fichier" << sw.fileName();
            throw Exception(tr("Problème de droits d'écriture du fichier %1").arg(sw.fileName()), MessageType::WARNING);
        }

        QTextStream flux(&sw);
        flux.setEncoding(QStringConverter::Utf8);

#if (!BUILD_TEST)
        const QString titre = "%1 %2 / %3 (c) %4";
        flux << titre.arg(APP_NAME).arg(QString(VER_MAJ)).arg(ORG_NAME).arg(QString(ANNEES_DEV)) << Qt::endl << Qt::endl << Qt::endl;
#endif

        // Donnees sur le satellite
        flux << tr("Nom                :") + " " + _ui->nomsat->text() << Qt::endl << Qt::endl;

        QString chaine = tr("Numéro NORAD       : %1\t\tMagnitude std/max  : %2", "Standard/Maximum magnitude");
        flux << chaine.arg(_ui->numNorad->text()).arg(_ui->magnitudeStdMaxDonneesSat->text()) << Qt::endl;

        chaine = tr("Désignation COSPAR : %1\t\tModèle orbital     : %2");
        flux << chaine.arg(_ui->desigCospar->text()).arg(_ui->modeleDonneesSat->text()) << Qt::endl;

        chaine = tr("Forme/Dimensions : %1");
        flux << chaine.arg(_ui->dimensionsDonneesSat->text()) << Qt::endl << Qt::endl;


        flux << tr("Date de lancement  : %1").arg(_ui->dateLancementDonneesSat->text()) << Qt::endl;

        if (_ui->dateRentree->isVisible()) {
            flux << tr("Date de rentrée    : %1").arg(_ui->dateRentree->text()) << Qt::endl;
        }

        flux << tr("Catégorie d'orbite : %1").arg(_ui->categorieOrbiteDonneesSat->text()) << Qt::endl;
        flux << tr("Pays/Organisation  : %1").arg(_ui->paysDonneesSat->text()) << Qt::endl;
        flux << tr("Site de lancement  : %1").arg(_ui->siteLancementDonneesSat->text()) << Qt::endl << Qt::endl;


        flux << tr("Classe/Catégorie/Discipline : %1").arg(_ui->classeCategDiscip->text()) << Qt::endl;
        flux << tr("Masse sec/totale            : %1").arg(_ui->masseSecTot->text()) << Qt::endl;
        flux << tr("Apogée  (Altitude)          : %1").arg(_ui->apogeeDonneesSat->text()) << Qt::endl;
        flux << tr("Périgée (Altitude)          : %1").arg(_ui->perigeeDonneesSat->text()) << Qt::endl;
        flux << tr("Période orbitale            : %1").arg(_ui->periodeDonneesSat->text()) << Qt::endl;
        flux << tr("Inclinaison                 : %1").arg(_ui->inclinaisonDonneesSat->text());

        sw.close();

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

void RechercheSatellite::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void RechercheSatellite::on_noradDonneesSat_valueChanged(int arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (arg1 > 0) {

        const QString norad = QString::number(arg1);
        _resultatsSatellitesTrouves = Donnees::RequeteNorad(Configuration::instance()->dbSatellites(), norad);

        const Donnees donnees = _resultatsSatellitesTrouves.first();
        QString nomsat = donnees.nom();
        if (nomsat.contains("iss (zarya)", Qt::CaseInsensitive)) {
            nomsat = "ISS";
        }

        _ui->nom->setText(nomsat.toUpper());
        _ui->cosparDonneesSat->setText(donnees.cospar());

        if (_resultatsSatellitesTrouves.isEmpty()) {
            _ui->nom->setText("");
            _ui->cosparDonneesSat->setText("");
        }

        show();
    }

    /* Retour */
    return;
}

void RechercheSatellite::show()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->satellitesTrouves->clear();
    _ui->frameResultats->setVisible(false);
    _ui->lbl_fichiersElementsOrbitaux->setVisible(false);
    _ui->fichiersElementsOrbitaux->setVisible(false);

    /* Corps de la methode */
    if (_resultatsSatellitesTrouves.isEmpty()) {
        _ui->lbl_satellitesTrouves->setText(tr("Objets trouvés :"));
    } else {

        const QString chaine = tr("Objets trouvés (%1) :");
        _ui->lbl_satellitesTrouves->setText(chaine.arg(_resultatsSatellitesTrouves.count()));

        // Remplissage de la liste de resultats
        QListIterator it(_resultatsSatellitesTrouves);
        while (it.hasNext()) {

            const Donnees donnees = it.next();

            _ui->satellitesTrouves->addItem(donnees.nom());
        }

        _ui->satellitesTrouves->setCurrentRow(0);
        _ui->frameResultats->setVisible(true);
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
bool RechercheSatellite::eventFilter(QObject *watched, QEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (event->type() == QEvent::MouseMove) {

        if (_ui->siteLancementDonneesSat->underMouse()) {

            const QString acronyme = _ui->siteLancementDonneesSat->text();
            const Observateur site = Configuration::instance()->mapSitesLancement()[acronyme];

            emit AffichageSiteLancement(acronyme, site);
            emit AfficherMessageStatut(site.nomlieu(), 10);
            _ui->siteLancementDonneesSat->setToolTip(site.nomlieu());

        } else if (_ui->paysDonneesSat->underMouse()) {

            const QString pays = Configuration::instance()->mapPays()[_ui->paysDonneesSat->text()];
            _ui->paysDonneesSat->setToolTip(pays);
            emit AfficherMessageStatut(pays, 10);

        } else if (_ui->categorieOrbiteDonneesSat->underMouse()) {

            const QString categorie = Configuration::instance()->mapCategoriesOrbite()[_ui->categorieOrbiteDonneesSat->text()];
            _ui->categorieOrbiteDonneesSat->setToolTip(categorie);
            emit AfficherMessageStatut(categorie, 10);
        }
    }

    /* Retour */
    return QFrame::eventFilter(watched, event);
}

void RechercheSatellite::on_nom_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomsat = _ui->nom->text();

    /* Corps de la methode */
    if (nomsat.length() >= 3) {

        // Recherche dans le tableau de donnees a partir du nom de l'objet
        _resultatsSatellitesTrouves = Donnees::RequeteNom(Configuration::instance()->dbSatellites(), nomsat);

        if (_resultatsSatellitesTrouves.isEmpty()) {
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(999999);
            _ui->noradDonneesSat->blockSignals(false);
            _ui->cosparDonneesSat->setText("");
        } else {
            const Donnees donnees(_resultatsSatellitesTrouves.first());
            _ui->cosparDonneesSat->setText(donnees.cospar());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(donnees.norad().toInt());
            _ui->noradDonneesSat->blockSignals(false);
        }

        show();

    } else {
        QToolTip::showText(_ui->nom->mapToGlobal(QPoint(0, 0)), _ui->nom->toolTip());
    }

    /* Retour */
    return;
}

void RechercheSatellite::on_cosparDonneesSat_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString cospar = _ui->cosparDonneesSat->text();

    /* Corps de la methode */
    if ((cospar.length() > 1) && (cospar.contains("-"))) {

        // Recherche dans le tableau de donnees a partir de la designation COSPAR
        _resultatsSatellitesTrouves = Donnees::RequeteCospar(Configuration::instance()->dbSatellites(), cospar);

        if (_resultatsSatellitesTrouves.isEmpty()) {
            _ui->nom->setText("");
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(999999);
            _ui->noradDonneesSat->blockSignals(false);
        } else {
            const Donnees donnees(_resultatsSatellitesTrouves.first());
            _ui->nom->setText(donnees.nom());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(donnees.norad().toInt());
            _ui->noradDonneesSat->blockSignals(false);
        }

        show();
    }

    /* Retour */
    return;
}

void RechercheSatellite::on_satellitesTrouves_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        const Donnees donnees(_resultatsSatellitesTrouves.at(currentRow));

        double perigee = donnees.perigee();
        double apogee = donnees.apogee();

        double ap = apogee + TERRE::RAYON_TERRESTRE;
        double per = perigee + TERRE::RAYON_TERRESTRE;

        // Nom du satellite
        _ui->nomsat->setText((donnees.nom().isEmpty()) ? tr("Inconnu") : donnees.nom());

        // Numero NORAD
        _ui->numNorad->setText(donnees.norad());

        // Designation COSPAR
        _ui->desigCospar->setText((donnees.cospar().isEmpty()) ? tr("Inconnue") : donnees.cospar());

        _ui->nom->setText(donnees.nom());
        _ui->noradDonneesSat->blockSignals(true);
        _ui->noradDonneesSat->setValue(donnees.norad().toInt());
        _ui->noradDonneesSat->blockSignals(false);
        _ui->cosparDonneesSat->setText(donnees.cospar());


        // Magnitude standard/maximale
        if ((donnees.magnitudeStandard() > 98.) || (perigee < MATHS::EPSDBL100) || (apogee < MATHS::EPSDBL100)) {
            _ui->magnitudeStdMaxDonneesSat->setText("?/?");
        } else {

            // Estimation de la magnitude maximale
            const double demiGrandAxe = 0.5 * (ap + per);
            const double exc = 2. * ap / (ap + per) - 1.;
            const double magMax = donnees.magnitudeStandard() - 15.75 + 5. * log10(1.45 * (demiGrandAxe * (1. - exc) - TERRE::RAYON_TERRESTRE));

            QString text;
            _ui->magnitudeStdMaxDonneesSat->setText(text.asprintf("%+.2f%s/%+.1f", donnees.magnitudeStandard(),
                                                                  donnees.methMagnitude().toStdString().c_str(), magMax));
        }

        // Modele orbital
        const bool modeleDS = (donnees.periode().toDouble() > 225.);
        const QString modele = (modeleDS) ? tr("SGP4 (DS)", "Orbital model SGP4 (deep space)") : tr("SGP4 (NE)", "Orbital model SGP4 (near Earth)");
        _ui->modeleDonneesSat->setText((donnees.periode().isEmpty()) ? tr("Non applicable") : modele);

        if (!donnees.periode().isEmpty()) {
            _ui->modeleDonneesSat->adjustSize();
            _ui->modeleDonneesSat->setFixedHeight(16);
            _ui->modeleDonneesSat->setToolTip((modeleDS) ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));
        }

        // Dimensions du satellite
        double t1 = donnees.longueur();
        double t2 = donnees.diametre();
        double t3 = donnees.envergure();

        QString unite1 = tr("m", "meter");
        QString unite2 = tr("km", "kilometer");

        if (!settings.value("affichage/unite").toBool()) {

            apogee *= TERRE::MILE_PAR_KM;
            perigee *= TERRE::MILE_PAR_KM;
            ap *= TERRE::MILE_PAR_KM;
            per *= TERRE::MILE_PAR_KM;

            t1 *= TERRE::PIED_PAR_METRE;
            t2 *= TERRE::PIED_PAR_METRE;
            t3 *= TERRE::PIED_PAR_METRE;

            unite1 = tr("ft", "foot");
            unite2 = tr("nmi", "nautical mile");
        }

        QString dimensions;
        if ((fabs(t1) < MATHS::EPSDBL100) && (fabs(t2) < MATHS::EPSDBL100) && (fabs(t3) < MATHS::EPSDBL100)) {
            dimensions = tr("Inconnues");
        } else {
            dimensions = QString("%1. %2 x %3 x %4 %5").arg(donnees.forme()).arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1).arg(unite1);
        }

        _ui->dimensionsDonneesSat->setText(dimensions);

        // Classe/Categorie/Discipline
        _ui->classeCategDiscip->setText(QString("%1 / %2 / %3").arg(donnees.classe()).arg(donnees.categorie()).arg(donnees.discipline()));

        // Masse sec, totale
        if ((donnees.masseSec() == "0") && (donnees.masseTot() == "0")) {
            _ui->masseSecTot->setText("?/?");
        } else {
            _ui->masseSecTot->setText(QString("%1/%2 kg").arg(donnees.masseSec()).arg(donnees.masseTot()));
        }

        // Apogee/perigee/periode orbitale
        const QString fmt = "%1 %2 (%3 %2)";
        if (fabs(apogee) < MATHS::EPSDBL100) {
            _ui->apogeeDonneesSat->setText(tr("Inconnu"));
        } else {
            _ui->apogeeDonneesSat->setText(fmt.arg(ap, 0, 'f', 0).arg(unite2).arg(apogee, 0, 'f', 0));
        }

        if (fabs(perigee) < MATHS::EPSDBL100) {
            _ui->perigeeDonneesSat->setText(tr("Inconnu"));
        } else {
            _ui->perigeeDonneesSat->setText(fmt.arg(per, 0, 'f', 0).arg(unite2).arg(perigee, 0, 'f', 0));
        }

        QString periode;
        if (donnees.periode().isEmpty()) {
            periode = tr("Inconnue");
        } else {
            periode = Maths::ToSexagesimal(donnees.periode().toDouble() * DATE::NB_HEUR_PAR_MIN * MATHS::HEUR2RAD,
                                           AngleFormatType::HEURE1, 1, 0, false, true).trimmed();
        }

        _ui->periodeDonneesSat->setText(periode);

        // Inclinaison
        _ui->inclinaisonDonneesSat->setText((donnees.inclinaison().isEmpty()) ? tr("Inconnue") : donnees.inclinaison() + "°");

        // Date de lancement
        _ui->dateLancementDonneesSat->setText((donnees.dateLancement().isEmpty()) ? tr("Inconnue") : donnees.dateLancement().replace("T", " "));

        // Date de rentree
        if (donnees.dateRentree().isEmpty()) {

            _ui->lbl_dateRentree->setVisible(false);
            _ui->dateRentree->setVisible(false);

        } else {

            const QString dateRentree = donnees.dateRentree() + " " +
                                        (((donnees.stsDateRentree() == '?') || (donnees.stsHeureRentree() == '?')) ? "?" : "");

            _ui->dateRentree->setText(dateRentree);
            _ui->lbl_dateRentree->setVisible(true);
            _ui->dateRentree->setVisible(true);

            if (donnees.stsDateRentree() == '?') {
                _ui->dateRentree->setToolTip(tr("Date de rentrée incertaine"));

            } else if (donnees.stsHeureRentree() == '?') {
                _ui->dateRentree->setToolTip(tr("Heure de rentrée incertaine"));

            } else {
                _ui->dateRentree->setToolTip("");
            }
        }

        // Categorie d'orbite
        _ui->categorieOrbiteDonneesSat->setText((donnees.orbite().isEmpty()) ? tr("Inconnue") : donnees.orbite());

        // Pays/Organisation
        _ui->paysDonneesSat->setText((donnees.pays().isEmpty()) ? tr("Inconnu") : donnees.pays());

        // Site de lancement
        _ui->siteLancementDonneesSat->setText((donnees.siteLancement().isEmpty()) ? tr("Inconnu") : donnees.siteLancement());
        _ui->siteLancementDonneesSat->adjustSize();
        _ui->siteLancementDonneesSat->setFixedHeight(16);

        // Recherche des fichiers d'elements orbitaux dans lesquels le satellite est present
        _ui->fichiersElementsOrbitaux->clear();
        const QString norad = QString("%1").arg(donnees.norad(), 6, QChar('0'));

        for(const QString &fic : Configuration::instance()->mapFichierElemNorad().keys()) {

            if (Configuration::instance()->mapFichierElemNorad()[fic].contains(norad)) {
                _ui->fichiersElementsOrbitaux->addItem(fic);
            }
        }

        if (_ui->fichiersElementsOrbitaux->count() > 0) {
            _ui->lbl_fichiersElementsOrbitaux->setVisible(true);
            _ui->fichiersElementsOrbitaux->setVisible(true);
        } else {
            _ui->lbl_fichiersElementsOrbitaux->setVisible(false);
            _ui->fichiersElementsOrbitaux->setVisible(false);
        }
    }

    /* Retour */
    return;
}
