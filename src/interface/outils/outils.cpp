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
 * >    outils.cpp
 *
 * Localisation
 * >    interface.outils
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    14 aout 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QClipboard>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSettings>
#include "ui_outils.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "outils.h"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"


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
Outils::Outils(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Outils)
{
    _ui->setupUi(this);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), parent->geometry()));

    try {

        Initialisation();

        _copier = new QAction(tr("Copier dans le presse-papier"));
        connect(_copier, &QAction::triggered, this, &Outils::Copier);

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Outils::~Outils()
{
    EFFACE_OBJET(_copier);
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
 * Initialisation de la fenetre Outils
 */
void Outils::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->listeOutils->setCurrentRow(0);
    _ui->listeOutils->setFocus();
    _ui->listeBoutonsOutils->button(QDialogButtonBox::Close)->setDefault(true);

    _ui->creationGroupe->setIcon(QIcon(":/resources/interface/ajout.png"));
    _ui->creationGroupe->setToolTip(tr("Créer un groupe d'éléments orbitaux"));
    _ui->ajoutFichiersElem->setIcon(QIcon(":/resources/interface/ajout.png"));
    _ui->ajoutFichiersElem->setToolTip(tr("Ajouter des fichiers d'éléments orbitaux"));
    _ui->groupe->setVisible(false);
    _ui->frameBarreProgressionElem->setVisible(false);

    const QDir di(Configuration::instance()->dirElem());
    const QStringList filtres(QStringList () << "*.txt" << "*.tle");
    _ui->majMaintenant->setEnabled(!di.entryList(filtres, QDir::Files).isEmpty());
    _ui->majMaintenant->setDefault(_ui->majMaintenant->isEnabled());
    _ui->frameBarreProgressionTLE->setVisible(false);
    _ui->compteRenduMajAuto->setVisible(false);
    _ui->compteRenduMajManuel->setVisible(false);
    _ui->affichageMsgMAJ->setCurrentIndex(settings.value("fichier/affichageMsgMAJ", 1).toInt());
    _ui->majTLE->setCurrentWidget(_ui->majTLEauto);

    InitListeDomaines();

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Outils::changeEvent(QEvent *evt)
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
 * Ecriture du compte-rendu de mise a jour des TLE
 */
void Outils::EcritureCompteRenduMaj(const QStringList &compteRendu, QPlainTextEdit *compteRenduMaj)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int nbsup = compteRendu.at(compteRendu.count()-1).toInt();
    const int nbadd = compteRendu.at(compteRendu.count()-2).toInt();
    const int nbold = compteRendu.at(compteRendu.count()-3).toInt();
    const int nbmaj = compteRendu.at(compteRendu.count()-4).toInt();
    const QString fic = compteRendu.at(compteRendu.count()-5);

    /* Corps de la methode */
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
    return;
}

/*
 * Initialisation de la liste deroulante des noms de domaine
 */
void Outils::InitListeDomaines()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->serveur->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->mapCategoriesElementsOrbitaux().keys());
    while (it.hasNext()) {
        _ui->serveur->addItem(it.next());
    }

    _ui->serveur->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Copier les compte-rendus de mise a jour des TLE dans le presse-papier
 */
void Outils::Copier()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QClipboard * const clipboard = QApplication::clipboard();

    /* Corps de la methode */
    if (_ui->compteRenduMajAuto->isVisible()) {
        clipboard->setText(_ui->compteRenduMajAuto->toPlainText());

    } else if (_ui->compteRenduMajManuel->isVisible()) {
        clipboard->setText(_ui->compteRenduMajManuel->toPlainText());
    }

    /* Retour */
    return;
}

/*
 * Affichage de la progression de telechargement des elements orbitaux
 */
void Outils::ProgressionElem(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite)
{
    if (octetsTotal != -1) {

        _ui->barreProgressionElem->setRange(0, octetsTotal);
        _ui->barreProgressionElem->setValue(static_cast<int> (octetsRecus));
        _ui->vitesseTelechargementElem->setText(QString("%1 %2").arg(vitesse, 0, 'f', 1).arg(unite));
    }
}

/*
 * Affichage de la progression de telechargement des TLE
 */
void Outils::ProgressionTLE(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite)
{
    if (octetsTotal != -1) {

        _ui->barreProgressionTLE->setRange(0, octetsTotal);
        _ui->barreProgressionTLE->setValue(static_cast<int> (octetsRecus));
        _ui->vitesseTelechargementTLE->setText(QString("%1 %2").arg(vitesse, 0, 'f', 1).arg(unite));
    }
}

void Outils::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)

    settings.setValue("fichier/fichierAMettreAJour", _ui->fichierAMettreAJour->text());
    settings.setValue("fichier/fichierALire", _ui->fichierALire->text());
    settings.setValue("fichier/affichageMsgMAJ", _ui->affichageMsgMAJ->currentIndex());
}

void Outils::on_listeOutils_currentRowChanged(int currentRow)
{
    _ui->stackedWidget_outils->setCurrentIndex(currentRow);
}

void Outils::on_serveur_currentTextChanged(const QString &arg1)
{
    /* Declarations des variables locales */
    QListWidgetItem *elem;

    /* Initialisations */
    _ui->listeGroupeElem->clear();
    const QString locale = Configuration::instance()->locale();

    /* Corps de la methode */
    const QList<CategorieElementsOrbitaux> &listeElem = Configuration::instance()->mapCategoriesElementsOrbitaux()[arg1];
    QListIterator it(listeElem);
    while (it.hasNext()) {

        const CategorieElementsOrbitaux categorie = it.next();
        const QString nom = categorie.nom[locale];

        elem = new QListWidgetItem(nom);
        elem->setData(Qt::UserRole, categorie.fichiers);
        elem->setData(Qt::CheckStateRole, categorie.miseAjour);

        _ui->listeGroupeElem->addItem(elem);
    }

    _ui->listeGroupeElem->setCurrentRow(0);

    /* Retour */
    return;
}

void Outils::on_listeGroupeElem_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->groupe->setVisible(false);
    _ui->listeFichiersElem->clear();

    /* Corps de la methode */
    if (currentRow >= 0) {

        QStringListIterator it(_ui->listeGroupeElem->item(currentRow)->data(Qt::UserRole).toStringList());
        while (it.hasNext()) {
            _ui->listeFichiersElem->addItem(it.next());
        }
    }

    /* Retour */
    return;
}

void Outils::on_creationGroupe_clicked()
{
    _ui->groupe->setVisible(true);
    _ui->domaine->clear();
    _ui->domaine->setEnabled(true);
    _ui->nomGroupe->clear();
    _ui->nomGroupe->setEnabled(true);
    _ui->listeFichiers->clear();
    _ui->domaine->setFocus();
    _ui->valider->setDefault(true);
}

void Outils::on_ajoutFichiersElem_clicked()
{
    _ui->groupe->setVisible(true);
    _ui->domaine->setText(_ui->serveur->currentText());
    _ui->domaine->setEnabled(false);
    _ui->nomGroupe->setText(_ui->listeGroupeElem->currentItem()->text());
    _ui->nomGroupe->setEnabled(false);

    for(int i=0; i<_ui->listeFichiersElem->count(); i++) {
        _ui->listeFichiers->setPlainText(_ui->listeFichiers->document()->toPlainText() + _ui->listeFichiersElem->item(i)->text().trimmed() + "\n");
    }

    _ui->listeFichiers->moveCursor(QTextCursor::End);
    _ui->listeFichiers->setFocus();
    _ui->valider->setDefault(true);
}

void Outils::on_majGroupe_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    Telechargement tel(Configuration::instance()->dirElem());
    const QString adresse = (_ui->serveur->currentText().contains("celestrak")) ?
                Configuration::instance()->adresseCelestrakNorad() : QString(DOMAIN_NAME) + "elem/%1";

    connect(&tel, &Telechargement::Progression, this, &Outils::ProgressionElem);
    _ui->frameBarreProgressionElem->setVisible(true);

    /* Corps de la methode */
    QStringListIterator it(_ui->listeGroupeElem->currentItem()->data(Qt::UserRole).toStringList());
    while (it.hasNext()) {
        const QString fic = it.next();
        const QString fichier = (adresse.contains("celestrak")) ? QFileInfo(fic).baseName() : fic;
        _ui->fichierTelechargementElem->setText(fic);
        tel.TelechargementFichier(QUrl(adresse.arg(fichier)));
    }

    disconnect(&tel, &Telechargement::Progression, this, &Outils::ProgressionElem);
    _ui->frameBarreProgressionElem->setVisible(false);

    Configuration::instance()->InitListeFichiersElem();
    emit InitFicGP();
    emit ChargementGP();

    /* Retour */
    return;
}

void Outils::on_majMaintenant_clicked()
{
    /* Declarations des variables locales */
    QStringList compteRendu;

    /* Initialisations */
    const int affMsg = _ui->affichageMsgMAJ->currentIndex();
    const QDir di(Configuration::instance()->dirTmp());
    const QStringList filtres(QStringList () << "*.txt" << "*.tle");

    Telechargement tel(Configuration::instance()->dirElem());
    const QString adresse = (_ui->serveur->currentText().contains("celestrak")) ?
                Configuration::instance()->adresseCelestrakNorad() : QString(DOMAIN_NAME) + "elem/%1";

    connect(&tel, &Telechargement::Progression, this, &Outils::ProgressionTLE);
    _ui->frameBarreProgressionTLE->setVisible(true);
    _ui->compteRenduMajAuto->setVisible(true);
    _ui->compteRenduMajAuto->clear();

    /* Corps de la methode */
    QStringListIterator it(di.entryList(filtres, QDir::Files));
    while (it.hasNext()) {

        const QString fic = it.next();
        const QString fichier = (adresse.contains("celestrak")) ? QFileInfo(fic).baseName() : fic;

        // Telechargement du fichier
        _ui->fichierTelechargementTLE->setText(fic);
        tel.TelechargementFichier(QUrl(adresse.arg(fichier)));

        const QString fichierAlire = Configuration::instance()->dirTmp() + QDir::separator() + fic;
        const QString fichierAmettreAjour = Configuration::instance()->dirElem() + QDir::separator() + fic;

        TLE::MiseAJourFichier(fichierAmettreAjour, fichierAlire, Configuration::instance()->donneesSatellites(), Configuration::instance()->lgRec(),
                              affMsg, compteRendu);

        EcritureCompteRenduMaj(compteRendu, _ui->compteRenduMajAuto);
    }

    disconnect(&tel, &Telechargement::Progression, this, &Outils::ProgressionTLE);
    _ui->frameBarreProgressionTLE->setVisible(false);

    /* Retour */
    return;
}

void Outils::on_listeBoutonsOutils_rejected()
{
    _ui->compteRenduMajAuto->setVisible(false);
    _ui->compteRenduMajManuel->setVisible(false);
}

void Outils::on_compteRenduMajAuto_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    QMenu menu(this);
    menu.addAction(_copier);
    menu.exec(QCursor::pos());

    /* Retour */
    return;
}

void Outils::on_parcourirMaj1_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierMaj", Configuration::instance()->dirElem()).toString(),
                                                   tr("Fichiers TLE (*.txt *.tle);;Tous les fichiers (*.*)"));

    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
        _ui->fichierAMettreAJour->setText(fichier);
    }

    _ui->mettreAJourTLE->setDefault(!_ui->fichierAMettreAJour->text().isEmpty() && !_ui->fichierALire->text().isEmpty());

    /* Retour */
    return;
}

void Outils::on_parcourirMaj2_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir fichier TLE"),
                                                   settings.value("fichier/fichierALire", Configuration::instance()->dirOut()).toString(),
                                                   tr("Fichiers TLE (*.txt *.tle);;Tous les fichiers (*.*)"));

    if (!fichier.isEmpty()) {
        fichier = QDir::toNativeSeparators(fichier);
        _ui->fichierALire->setText(fichier);
    }

    _ui->mettreAJourTLE->setDefault(!_ui->fichierAMettreAJour->text().isEmpty() && !_ui->fichierALire->text().isEmpty());

    /* Retour */
    return;
}

void Outils::on_mettreAJourTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->compteRenduMajManuel->clear();
    _ui->compteRenduMajManuel->setVisible(false);

    /* Corps de la methode */
    try {

        if (_ui->fichierAMettreAJour->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à mettre à jour n'est pas spécifié"), MessageType::WARNING);
        }

        if (_ui->fichierALire->text().isEmpty()) {
            throw PreviSatException(tr("Le nom du fichier à lire n'est pas spécifié"), MessageType::WARNING);
        }

        QFileInfo fi(_ui->fichierALire->text());
        QString fic = QDir::toNativeSeparators(fi.absoluteFilePath());

        const QStringList listeFic(QStringList () << _ui->fichierAMettreAJour->text() << fic);
        foreach(QString file, listeFic) {
            fi = QFileInfo(file);
            if (!fi.exists()) {
                const QString msg = tr("Le fichier %1 n'existe pas");
                throw PreviSatException(msg.arg(fi.absoluteFilePath()), MessageType::WARNING);
            }
        }

        QStringList compteRendu;
        const int affMsg = _ui->affichageMsgMAJ->currentIndex();
        TLE::MiseAJourFichier(_ui->fichierAMettreAJour->text(), fic, Configuration::instance()->donneesSatellites(),
                              Configuration::instance()->lgRec(), affMsg, compteRendu);

        EcritureCompteRenduMaj(compteRendu, _ui->compteRenduMajManuel);

        _ui->compteRenduMajManuel->setVisible(true);


    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Outils::on_compteRenduMajManuel_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(pos)

    /* Corps de la methode */
    QMenu menu(this);
    menu.addAction(_copier);
    menu.exec(QCursor::pos());

    /* Retour */
    return;
}
