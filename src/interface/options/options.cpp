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
 * >    options.cpp
 *
 * Localisation
 * >    interface.options
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 aout 2022
 *
 * Date de revision
 * >
 *
 */

#include "options.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include "ui_options.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "librairies/exceptions/previsatexception.h"


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
Options::Options(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Options)
{
    _ui->setupUi(this);

    try {

        Initialisation();

        // Chargement des fichiers de preference
        InitFicPref(false);
        ChargementPref();

        connect(_ui->listeBoutonsOptions->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &Options::AppliquerPreferences);

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Options::~Options()
{
    AppliquerPreferences();
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Options *Options::ui()
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
 * Initialisation de la fenetre Options
 */
void Options::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QStyle * const styleIcones = QApplication::style();

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->listeOptions->setCurrentRow(0);
    _ui->enregistrerPref->setIcon(styleIcones->standardIcon(QStyle::SP_DialogSaveButton));
    _ui->ajoutLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowRight));
    _ui->supprLieu->setIcon(styleIcones->standardIcon(QStyle::SP_ArrowLeft));
    _ui->listeOptions->setFocus();

    const int index = settings.value("affichage/policeWCC", 0).toInt();
    _ui->policeWCC->clear();

#if defined (Q_OS_WIN)
    _ui->policeWCC->addItem("Lucida Console");
    _ui->policeWCC->addItem("MS Shell Dlg 2");

    const int taille = 10;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#elif defined (Q_OS_LINUX)
    _ui->policeWCC->addItem("FreeSans");
    _ui->policeWCC->addItem("Sans Serif");

    const int taille = 11;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille);

#elif defined (Q_OS_MAC)
    _ui->policeWCC->addItem("Lucida Grande");
    _ui->policeWCC->addItem("Marion");

    const int taille = 13;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille, ((index == 0) ? QFont::Normal : QFont::Bold));

#else
    const int taille = 11;
    QFont policeWcc(_ui->policeWCC->itemText(index), taille);

#endif

    Configuration::instance()->setPoliceWcc(policeWcc);

    // Affichage des lieux d'observation selectionnes
    AffichageLieuObs();

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Options::changeEvent(QEvent *evt)
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
 * Affichage des lieux d'observation selectionnes
 */
void Options::AffichageLieuObs()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->selecLieux->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->observateurs());
    while (it.hasNext()) {
        const QString nomlieu = it.next().nomlieu();
        _ui->selecLieux->addItem(nomlieu);
    }

    /* Retour */
    return;
}

void Options::ChargementPref()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomPref = Configuration::instance()->dirPref() + QDir::separator() +
            Configuration::instance()->listeFicPref().at(_ui->preferences->currentIndex());

    /* Corps de la methode */
    QFile fichier(nomPref);
    if (fichier.exists() && (fichier.size() != 0)) {

        // Lecture du fichier de preferences
        if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {

            const QStringList listePrf = QString(fichier.readAll()).split("\n", Qt::SkipEmptyParts);
            fichier.close();

            QStringListIterator it(listePrf);
            while (it.hasNext()) {

                const QStringList item = it.next().split(" ", Qt::SkipEmptyParts);

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
        }

        const bool etat = blockSignals(true);
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
        _ui->refractionAtmospherique->setChecked(settings.value("affichage/refractionAtmospherique", true).toBool());
        _ui->effetEclipsesMagnitude->setChecked(settings.value("affichage/effetEclipsesMagnitude", true).toBool());
        _ui->eclipsesLune->setChecked(settings.value("affichage/eclipsesLune", true).toBool());
        _ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 30).toInt());
        _ui->intensiteVision->setValue(settings.value("affichage/intensiteVision", 50).toInt());
        _ui->langue->setCurrentIndex(static_cast<int> (Configuration::instance()->listeFicLang()
                                                       .indexOf(settings.value("affichage/langue", "en").toString())));
        _ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
        _ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 2).toInt());
        _ui->proportionsCarte->setChecked(settings.value("affichage/proportionsCarte", true).toBool());
        _ui->rotationIconeISS->setChecked(settings.value("affichage/rotationIconeISS", true).toBool());
        _ui->affNoradListes->setChecked(settings.value("affichage/affNoradListes", 0).toInt());
        _ui->rotationLune->setChecked(settings.value("affichage/rotationLune", false).toBool());
        _ui->utcAuto->setChecked(settings.value("affichage/utcAuto", true).toBool());
        _ui->verifMAJ->setChecked(settings.value("affichage/verifMAJ", false).toBool());
        blockSignals(etat);
    }

    /* Retour */
    return;
}

void Options::InitFicPref(const bool majAff)
{
    /* Declarations des variables locales */
    QString fichier;

    /* Initialisations */
    const bool etat = _ui->preferences->blockSignals(true);
    _ui->preferences->clear();

    /* Corps de la methode */
    QStringListIterator it(Configuration::instance()->listeFicPref());
    while (it.hasNext()) {

        const QString fic = Configuration::instance()->dirPref() + QDir::separator() + it.next();
        const QFileInfo fi(fic);
        fichier = fi.completeBaseName();
        fichier[0] = fichier[0].toUpper();

        _ui->preferences->addItem((fi.completeBaseName() == "defaut") ? tr("* Défaut") : fichier);
        if (settings.value("fichier/preferences", Configuration::instance()->dirPref() + QDir::separator() + "defaut").toString() == fic) {
            _ui->preferences->setCurrentIndex(_ui->preferences->count() - 1);
        }
    }

    _ui->preferences->addItem(tr("Enregistrer sous..."));
    // TODO
    if (majAff) {
        //        emit MiseAJourCarte();
    }

    _ui->preferences->addItem(tr("Supprimer..."));
    _ui->preferences->blockSignals(etat);

    /* Retour */
    return;
}

void Options::SauvePreferences(const QString &fichierPref)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile fi(fichierPref);

        if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {

            if (!fi.isWritable()) {
                const QString msg = tr("Problème de droits d'écriture du fichier %1");
                throw PreviSatException(msg.arg(fi.fileName()), MessageType::WARNING);
            }

            QTextStream flux(&fi);

            flux << "affichage/affSAA " << QVariant(_ui->affSAA->isChecked()).toString() << Qt::endl
                 << "affichage/affconst " << _ui->affconst->checkState() << Qt::endl
                 << "affichage/affcoord " << QVariant(_ui->affcoord->isChecked()).toString() << Qt::endl
                 << "affichage/affetoiles " << QVariant(_ui->affetoiles->isChecked()).toString() << Qt::endl
                 << "affichage/affgrille " << QVariant(_ui->affgrille->isChecked()).toString() << Qt::endl
                 << "affichage/afficone " << QVariant(_ui->afficone->isChecked()).toString() << Qt::endl
                 << "affichage/affinvew " << QVariant(_ui->affinvew->isChecked()).toString() << Qt::endl
                 << "affichage/affinvns " << QVariant(_ui->affinvns->isChecked()).toString() << Qt::endl
                 << "affichage/afflune " << QVariant(_ui->afflune->isChecked()).toString() << Qt::endl
                 << "affichage/affnomlieu " << _ui->affnomlieu->checkState() << Qt::endl
                 << "affichage/affnomsat " << _ui->affnomsat->checkState() << Qt::endl
                 << "affichage/affNoradListes " << _ui->affNoradListes->checkState() << Qt::endl
                 << "affichage/affnotif " << QVariant(_ui->affnotif->isChecked()).toString() << Qt::endl
                 << "affichage/affnuit " << QVariant(_ui->affnuit->isChecked()).toString() << Qt::endl
                 << "affichage/affphaselune " << QVariant(_ui->affphaselune->isChecked()).toString() << Qt::endl
                 << "affichage/affplanetes " << _ui->affplanetes->checkState() << Qt::endl
                 << "affichage/affradar " << _ui->affradar->checkState() << Qt::endl
                 << "affichage/affsoleil " << QVariant(_ui->affsoleil->isChecked()).toString() << Qt::endl
                 << "affichage/afftraceCiel " << QVariant(_ui->afftraceCiel->isChecked()).toString() << Qt::endl
                 << "affichage/afftraj " << QVariant(_ui->afftraj->isChecked()).toString() << Qt::endl
                 << "affichage/affvisib " << _ui->affvisib->checkState() << Qt::endl
                 << "affichage/calJulien " << QVariant(_ui->calJulien->isChecked()).toString() << Qt::endl
                 << "affichage/eclipsesLune " << QVariant(_ui->eclipsesLune->isChecked()).toString() << Qt::endl
                 << "affichage/effetEclipsesMagnitude " << QVariant(_ui->effetEclipsesMagnitude->isChecked()).toString() << Qt::endl
                 << "affichage/extinction " << QVariant(_ui->extinctionAtmospherique->isChecked()).toString() << Qt::endl
                 << "affichage/intensiteOmbre " << _ui->intensiteOmbre->value() << Qt::endl
                 << "affichage/intensiteVision " << _ui->intensiteVision->value() << Qt::endl
                 << "affichage/langue " << Configuration::instance()->locale() << Qt::endl
                 << "affichage/magnitudeEtoiles " << _ui->magnitudeEtoiles->value() << Qt::endl
                 << "affichage/nombreTrajectoires " << _ui->nombreTrajectoires->value() << Qt::endl
                 << "affichage/proportionsCarte " << QVariant(_ui->proportionsCarte->isChecked()).toString() << Qt::endl
                 << "affichage/refractionAtmospherique " << QVariant(_ui->refractionAtmospherique->isChecked()).toString() << Qt::endl
                 << "affichage/rotationIconeISS " << QVariant(_ui->rotationIconeISS->isChecked()).toString() << Qt::endl
                 << "affichage/rotationLune " << QVariant(_ui->rotationLune->isChecked()).toString() << Qt::endl
                 << "affichage/systemeHoraire " << QVariant(_ui->syst24h->isChecked()).toString() << Qt::endl
                 << "affichage/unite " << QVariant(_ui->unitesKm->isChecked()).toString() << Qt::endl
                 << "affichage/utc " << QVariant(_ui->utc->isChecked()).toString() << Qt::endl
                 << "affichage/utcAuto " << QVariant(_ui->utcAuto->isChecked()).toString() << Qt::endl
                 << "affichage/valeurZoomMap " << _ui->valeurZoomMap->value() << Qt::endl
                 << "affichage/verifMAJ " << QVariant(_ui->verifMAJ->isChecked()).toString() << Qt::endl

                 << "affichage/affBetaWCC " << QVariant(_ui->affBetaWCC->isChecked()).toString() << Qt::endl
                 << "affichage/affCerclesAcq " << QVariant(_ui->affCerclesAcq->isChecked()).toString() << Qt::endl
                 << "affichage/affNbOrbWCC " << QVariant(_ui->affNbOrbWCC->isChecked()).toString() << Qt::endl
                 << "affichage/aff_ZOE " << QVariant(_ui->affSAA_ZOE->isChecked()).toString() << Qt::endl
                 << "affichage/styleWCC " << QVariant(_ui->styleWCC->isChecked()).toString() << Qt::endl
                 << "affichage/coulGMT " << _ui->coulGMT->currentIndex() << Qt::endl
                 << "affichage/coulZOE " << _ui->coulZOE->currentIndex() << Qt::endl
                 << "affichage/coulCercleVisibilite " << _ui->coulCercleVisibilite->currentIndex() << Qt::endl
                 << "affichage/coulEquateur " << _ui->coulEquateur->currentIndex() << Qt::endl
                 << "affichage/coulTerminateur " << _ui->coulTerminateur->currentIndex() << Qt::endl
                 << "affichage/policeWCC " << _ui->policeWCC->currentIndex() << Qt::endl;

            for(const QString &station : Configuration::instance()->mapStations().keys()) {
                flux << "affichage/station" + station + " " + QVariant(settings.value("affichage/station" + station).toString()).toString() << Qt::endl;
            }

            fi.close();
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

/*
 * Appliquer les preferences d'affichage et enregistrer le fichier de preferences
 */
void Options::AppliquerPreferences()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    settings.setValue("affichage/affSAA", _ui->affSAA->isChecked());
    settings.setValue("affichage/affconst", _ui->affconst->checkState());
    settings.setValue("affichage/affcoord", _ui->affcoord->isChecked());
    settings.setValue("affichage/affetoiles", _ui->affetoiles->isChecked());
    settings.setValue("affichage/affgrille", _ui->affgrille->isChecked());
    settings.setValue("affichage/afficone", _ui->afficone->isChecked());
    settings.setValue("affichage/affinvew", _ui->affinvew->isChecked());
    settings.setValue("affichage/affinvns", _ui->affinvns->isChecked());
    settings.setValue("affichage/afflune", _ui->afflune->isChecked());
    settings.setValue("affichage/affnomlieu", _ui->affnomlieu->checkState());
    settings.setValue("affichage/affnomsat", _ui->affnomsat->checkState());
    settings.setValue("affichage/affnotif", _ui->affnotif->isChecked());
    settings.setValue("affichage/affnuit", _ui->affnuit->isChecked());
    settings.setValue("affichage/affphaselune", _ui->affphaselune->isChecked());
    settings.setValue("affichage/affplanetes", _ui->affplanetes->checkState());
    settings.setValue("affichage/affradar", _ui->affradar->checkState());
    settings.setValue("affichage/affsoleil", _ui->affsoleil->isChecked());
    settings.setValue("affichage/afftraceCiel", _ui->afftraceCiel->isChecked());
    settings.setValue("affichage/afftraj", _ui->afftraj->isChecked());
    settings.setValue("affichage/affvisib", _ui->affvisib->checkState());
    settings.setValue("affichage/calJulien", _ui->calJulien->isChecked());
    settings.setValue("affichage/eclipsesLune", _ui->eclipsesLune->isChecked());
    settings.setValue("affichage/effetEclipsesMagnitude", _ui->effetEclipsesMagnitude->isChecked());
    settings.setValue("affichage/extinction", _ui->extinctionAtmospherique->isChecked());
    settings.setValue("affichage/intensiteOmbre", _ui->intensiteOmbre->value());
    settings.setValue("affichage/intensiteVision", _ui->intensiteVision->value());
    settings.setValue("affichage/magnitudeEtoiles", _ui->magnitudeEtoiles->value());
    settings.setValue("affichage/nombreTrajectoires", _ui->nombreTrajectoires->value());
    settings.setValue("affichage/proportionsCarte", _ui->proportionsCarte->isChecked());
    settings.setValue("affichage/refractionAtmospherique", _ui->refractionAtmospherique->isChecked());
    settings.setValue("affichage/rotationIconeISS", _ui->rotationIconeISS->isChecked());
    settings.setValue("affichage/affNoradListes", _ui->affNoradListes->isChecked());
    settings.setValue("affichage/rotationLune", _ui->rotationLune->isChecked());
    settings.setValue("affichage/systemeHoraire", _ui->syst24h->isChecked());
    settings.setValue("affichage/unite", _ui->unitesKm->isChecked());
    settings.setValue("affichage/utc", _ui->utc->isChecked());
    settings.setValue("affichage/utcAuto", _ui->utcAuto->isChecked());
    settings.setValue("affichage/valeurZoomMap", _ui->valeurZoomMap->value());
    settings.setValue("affichage/verifMAJ", _ui->verifMAJ->isChecked());

    settings.setValue("affichage/affBetaWCC", _ui->affBetaWCC->isChecked());
    settings.setValue("affichage/affCerclesAcq", _ui->affCerclesAcq->isChecked());
    settings.setValue("affichage/affNbOrbWCC", _ui->affNbOrbWCC->isChecked());
    settings.setValue("affichage/affSAA_ZOE", _ui->affSAA_ZOE->isChecked());
    settings.setValue("affichage/styleWCC", _ui->styleWCC->isChecked());
    settings.setValue("affichage/coulGMT", _ui->coulGMT->currentIndex());
    settings.setValue("affichage/coulZOE", _ui->coulZOE->currentIndex());
    settings.setValue("affichage/coulCercleVisibilite", _ui->coulCercleVisibilite->currentIndex());
    settings.setValue("affichage/coulEquateur", _ui->coulEquateur->currentIndex());
    settings.setValue("affichage/coulTerminateur", _ui->coulTerminateur->currentIndex());
    settings.setValue("affichage/policeWCC", _ui->policeWCC->currentIndex());

    for(int i=0; i<_ui->listeStations->count(); i++) {
        settings.setValue("affichage/station" + _ui->listeStations->item(i)->data(Qt::UserRole).toString(),
                          _ui->listeStations->item(i)->checkState());
    }

    // Ecriture du fichier de preferences
    if (_ui->preferences->currentIndex() < _ui->preferences->count() - 2) {

        const QString fichierPref = Configuration::instance()->dirPref() + QDir::separator() +
                Configuration::instance()->listeFicPref().at(_ui->preferences->currentIndex());

        SauvePreferences(fichierPref);
    }

    emit RecalculerPositions();

    /* Retour */
    return;
}

void Options::on_listeOptions_currentRowChanged(int currentRow)
{
    _ui->stackedWidget_options->setCurrentIndex(currentRow);
}

void Options::on_listeBoutonsOptions_accepted()
{
    AppliquerPreferences();
}

void Options::on_listeBoutonsOptions_rejected()
{
    // Rechargement du fichier de preferences si une option a ete modifiee et ne doit pas etre sauvegardee
    ChargementPref();
}
