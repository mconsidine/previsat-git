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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QToolTip>
#include "ui_recherchesatellite.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "recherchesatellite.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


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
RechercheSatellite::RechercheSatellite(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::RechercheSatellite)
{
    _ui->setupUi(this);
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
        if (sw.open(QIODevice::WriteOnly | QIODevice::Text)) {

            if (!sw.isWritable()) {
                qWarning() << "Problème de droits d'écriture du fichier" << sw.fileName();
                throw PreviSatException(tr("Problème de droits d'écriture du fichier %1").arg(sw.fileName()), MessageType::WARNING);
            }

            QTextStream flux(&sw);

#if (BUILD_TEST == false)
            const QString titre = "%1 %2 / %3 (c) %4";
            flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APP_VER_MAJ)).arg(APP_NAME).arg(QString(APP_ANNEES_DEV))
                 << Qt::endl << Qt::endl << Qt::endl;
#endif

            // Donnees sur le satellite
            flux << tr("Nom                :") + " " + _ui->nomsat->text() << Qt::endl;

            QString chaine = tr("Numéro NORAD       : %1\t\tMagnitude std/max  : %2", "Standard/Maximal magnitude");
            flux << chaine.arg(_ui->numNorad->text()).arg(_ui->magnitudeStdMaxDonneesSat->text()) << Qt::endl;

            chaine = tr("Désignation COSPAR : %1\t\tModèle orbital     : %2");
            flux << chaine.arg(_ui->desigCospar->text()).arg(_ui->modeleDonneesSat->text()) << Qt::endl;

            chaine = tr("Dimensions/Section : %1%2");
            flux << chaine.arg(_ui->dimensionsDonneesSat->text()).arg((_ui->dimensionsDonneesSat->text() == tr("Inconnues")) ? "" : "^2")
                 << Qt::endl << Qt::endl;

            chaine = tr("Date de lancement  : %1\t\tApogée  (Altitude) : %2");
            flux << chaine.arg(_ui->dateLancementDonneesSat->text()).arg(_ui->apogeeDonneesSat->text()) << Qt::endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Date de rentrée    : %1\t\t").arg(_ui->dateRentree->text()) :
                                                       tr("Catégorie d'orbite : %1\t\t").arg(_ui->categorieOrbiteDonneesSat->text());
            flux << chaine + tr("Périgée (Altitude) : %1").arg(_ui->perigeeDonneesSat->text()) << Qt::endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Catégorie d'orbite : %1\t\t").arg(_ui->categorieOrbiteDonneesSat->text()) :
                                                       tr("Pays/Organisation  : %1\t\t").arg(_ui->paysDonneesSat->text());
            flux << chaine + tr("Période orbitale   : %1").arg(_ui->periodeDonneesSat->text()) << Qt::endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Pays/Organisation  : %1\t\t").arg(_ui->paysDonneesSat->text()) :
                                                       tr("Site de lancement  : %1\t\t").arg(_ui->siteLancementDonneesSat->text());
            flux << chaine + tr("Inclinaison        : %1").arg(_ui->inclinaisonDonneesSat->text()) << Qt::endl;

            if (_ui->dateRentree->isVisible()) {
                flux << tr("Site de lancement  : %1").arg(_ui->siteLancementDonneesSat->text());
            }
        }
        sw.close();

    } catch (PreviSatException &e) {
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

        _resultatsSatellitesTrouves.clear();
        const QString &donneesSat = Configuration::instance()->donneesSatellites();
        const int lgRec = Configuration::instance()->lgRec();

        const int idx = lgRec * arg1;
        if ((idx >= 0) && (idx < donneesSat.size())) {

            const QString ligne = donneesSat.mid(idx, lgRec);
            _resultatsSatellitesTrouves.append(ligne);

            QString nomsat = ligne.mid(125).trimmed();
            if (nomsat.contains("iss (zarya)", Qt::CaseInsensitive)) {
                nomsat = "ISS";
            }

            _ui->nom->setText(nomsat.toUpper());
            _ui->cosparDonneesSat->setText(ligne.mid(7, 11).toUpper().trimmed());
        }

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
        QString nomsat;
        QStringListIterator it(_resultatsSatellitesTrouves);
        while (it.hasNext()) {

            const QString item = it.next().toUpper();
            nomsat = item.mid(125).trimmed();

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
void RechercheSatellite::on_nom_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomsat = _ui->nom->text();
    _resultatsSatellitesTrouves.clear();

    /* Corps de la methode */
    if (nomsat.length() >= 3) {

        // Recherche dans le tableau de donnees a partir du nom de l'objet
        QStringList donneesSat = Configuration::instance()->donneesSatellites().split('\n');
        donneesSat.removeFirst();

        QStringListIterator it(donneesSat);
        while (it.hasNext()) {

            const QString ligne = it.next();
            if (ligne.mid(125).contains(nomsat, Qt::CaseInsensitive)) {
                _resultatsSatellitesTrouves.append(ligne);
            }
        }

        if (_resultatsSatellitesTrouves.isEmpty()) {
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(999999);
            _ui->noradDonneesSat->blockSignals(false);
            _ui->cosparDonneesSat->setText("");
        } else {
            _ui->cosparDonneesSat->setText(_resultatsSatellitesTrouves.at(0).mid(7, 11).toUpper().trimmed());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(_resultatsSatellitesTrouves.at(0).mid(0, 6).toInt());
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
    _resultatsSatellitesTrouves.clear();

    /* Corps de la methode */
    if ((cospar.length() > 1) && (cospar.contains("-"))) {

        // Recherche dans le tableau de donnees a partir de la designation COSPAR
        QStringList donneesSat = Configuration::instance()->donneesSatellites().split('\n');
        donneesSat.removeFirst();

        QStringListIterator it(donneesSat);
        while (it.hasNext()) {

            const QString ligne = it.next();
            if (ligne.mid(7, 11).contains(cospar, Qt::CaseInsensitive)) {
                _resultatsSatellitesTrouves.append(ligne);
            }
        }

        if (_resultatsSatellitesTrouves.isEmpty()) {
            _ui->nom->setText("");
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(999999);
            _ui->noradDonneesSat->blockSignals(false);
        } else {
            _ui->nom->setText(_resultatsSatellitesTrouves.at(0).mid(125).toUpper().trimmed());
            _ui->noradDonneesSat->blockSignals(true);
            _ui->noradDonneesSat->setValue(_resultatsSatellitesTrouves.at(0).mid(0, 6).toInt());
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

        const QString ligne = _resultatsSatellitesTrouves.at(currentRow).toUpper();
        const double magnitudeStandard = ligne.mid(35, 4).toDouble();

        const QString dateRentree = ligne.mid(60, 10).trimmed();
        const QString periode = ligne.mid(71, 10).trimmed();
        double perigee = ligne.mid(82, 7).trimmed().toDouble();
        double apogee = ligne.mid(90, 7).trimmed().toDouble();

        double ap = apogee + RAYON_TERRESTRE;
        double per = perigee + RAYON_TERRESTRE;

        // Nom du satellite
        const QString nomsat = ligne.mid(125).trimmed();
        _ui->nomsat->setText((nomsat.isEmpty()) ? tr("Inconnu") : nomsat);

        // Numero NORAD
        const QString norad = ligne.mid(0, 6);
        _ui->numNorad->setText(norad);

        // Designation COSPAR
        const QString cospar = ligne.mid(7, 11).trimmed();
        _ui->desigCospar->setText((cospar.isEmpty()) ? tr("Inconnue") : cospar);

        _ui->nom->setText(nomsat);
        _ui->noradDonneesSat->blockSignals(true);
        _ui->noradDonneesSat->setValue(norad.toInt());
        _ui->noradDonneesSat->blockSignals(false);
        _ui->cosparDonneesSat->setText(cospar);


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
            _ui->modeleDonneesSat->setToolTip((modeleDS) ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));
        }

        // Dimensions du satellite
        double t1 = ligne.mid(19, 5).toDouble();
        double t2 = ligne.mid(25, 4).toDouble();
        double t3 = ligne.mid(30, 4).toDouble();
        double section = ligne.mid(42, 6).toDouble();
        QString unite1 = tr("m", "meter");
        QString unite2 = tr("km", "kilometer");
        if (!settings.value("affichage/unite").toBool()) {

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
                    tr("Inconnue") :
                    Maths::ToSexagesimal(periode.toDouble() * NB_HEUR_PAR_MIN * HEUR2RAD, AngleFormatType::HEURE1, 1, 0, false, true).trimmed();
        _ui->periodeDonneesSat->setText(period);

        // Inclinaison
        const QString inclinaison = ligne.mid(97, 6).trimmed();
        _ui->inclinaisonDonneesSat->setText((inclinaison.isEmpty()) ? tr("Inconnue") : inclinaison + "°");

        // Date de lancement
        const QString dateLancement = ligne.mid(49, 10).trimmed();
        _ui->dateLancementDonneesSat->setText((dateLancement.isEmpty()) ? tr("Inconnue") : dateLancement);

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

            _ui->dateRentree->setText(dateRentree);
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

        // Recherche des fichiers d'elements orbitaux dans lesquels le satellite est present
        _ui->fichiersElementsOrbitaux->clear();
        const QDir di(Configuration::instance()->dirElem());
        const QStringList filtres(QStringList () << "*.xml" << "*.txt" << "*.tle");
        const QStringList listeFicElem = di.entryList(filtres, QDir::Files);
        QString fichier;
        QMap<QString, ElementsOrbitaux> mapElem;

        foreach(const QString fic, listeFicElem) {

            fichier.clear();
            mapElem.clear();

            QFileInfo ff(Configuration::instance()->dirElem() + QDir::separator() + fic);
            if (ff.suffix() == "xml") {

                mapElem = GPFormat::LectureFichier(ff.filePath(), Configuration::instance()->donneesSatellites(),
                                                   Configuration::instance()->lgRec(), QStringList(), false);

                if (mapElem.contains(norad)) {
                    fichier = ff.baseName();
                }

            } else {

                mapElem = TLE::LectureFichier(ff.filePath(), Configuration::instance()->donneesSatellites(),
                                              Configuration::instance()->lgRec(), QStringList(), false);

                if (mapElem.contains(norad)) {
                    fichier = ff.baseName() + " (TLE)";
                }
            }

            if (!fichier.isEmpty()) {
                fichier[0] = fichier[0].toUpper();
                _ui->fichiersElementsOrbitaux->addItem(fichier);
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
