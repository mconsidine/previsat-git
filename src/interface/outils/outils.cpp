/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    29 decembre 2024
 *
 */

#include <QClipboard>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include "ui_outils.h"
#include "outils.h"
#include "configuration/configuration.h"
#include "configuration/gestionnairexml.h"
#include "interface/onglets/donnees/informationssatellite.h"
#include "librairies/corps/satellite/gpformat.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"


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
Outils::Outils(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::Outils)
{
    _ui->setupUi(this);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), parent->geometry()));

    try {

        _creerGroupe = nullptr;
        _supprimerGroupe = nullptr;
        _ajouterFichiers = nullptr;
        _supprimerFichier = nullptr;
        _copier = nullptr;
        _informations = new InformationsSatellite(_ui->frameInformations);

        Initialisation();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
}


/*
 * Destructeur
 */
Outils::~Outils()
{
    EFFACE_OBJET(_creerGroupe);
    EFFACE_OBJET(_supprimerGroupe);
    EFFACE_OBJET(_ajouterFichiers);
    EFFACE_OBJET(_supprimerFichier);
    EFFACE_OBJET(_copier);
    EFFACE_OBJET(_informations);

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
    _ui->stackedWidget_satellites->setVisible(false);
    _ui->frameBarreProgressionElem->setVisible(false);

    setFixedHeight(360);
    _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 320, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());

    const QDir di(Configuration::instance()->dirElem());
    const QStringList filtres(QStringList () << "*.txt" << "*.tle");
    const QStringList listeFicTLE = di.entryList(filtres, QDir::Files);

    InitGestionnaireTLE(listeFicTLE);

    const QDir di2(Configuration::instance()->dirRsc());
    const QStringList filtrePng(QStringList() << "*.png");
    const QStringList listeFicPng = di2.entryList(filtrePng, QDir::Files);

    InitGestionnaireIcones(listeFicPng);

    _ui->majMaintenant->setEnabled(!listeFicTLE.isEmpty());
    _ui->majMaintenant->setDefault(_ui->majMaintenant->isEnabled());
    _ui->majMaintenant->setToolTip((_ui->majMaintenant->isEnabled()) ? "" : tr("Aucun fichier TLE dans le répertoire d'éléments orbitaux"));
    _ui->affichageMsgMAJ->setEnabled(_ui->majMaintenant->isEnabled());
    _ui->frameBarreProgressionTLE->setVisible(false);
    _ui->compteRenduMajAuto->setVisible(false);
    _ui->compteRenduMajManuel->setVisible(false);
    _ui->affichageMsgMAJ->setCurrentIndex(settings.value("fichier/affichageMsgMAJ", 1).toInt());
    _ui->majTLE->setCurrentWidget(_ui->majTLEauto);

    CreationMenus();

    InitListeDomaines();

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Outils::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);

        if (_informations != nullptr) {
            _informations->changeEvent(evt);
        }
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
 * Creation des menus contextuels
 */
void Outils::CreationMenus()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _copier = new QAction(tr("Copier dans le presse-papier"), this);
    connect(_copier, &QAction::triggered, this, &Outils::Copier);

    _creerGroupe = new QAction(tr("Créer un groupe"), this);
    _creerGroupe->setIcon(QIcon(":/resources/interface/ajout.png"));
    connect(_creerGroupe, &QAction::triggered, this, &Outils::on_creationGroupe_clicked);

    _supprimerGroupe = new QAction(tr("Supprimer"), this);
    _supprimerGroupe->setIcon(QIcon(":/resources/interface/suppr.png"));
    connect(_supprimerGroupe, &QAction::triggered, this, &Outils::SupprimerGroupe);

    _ajouterFichiers = new QAction(tr("Ajouter des fichiers"), this);
    _ajouterFichiers->setIcon(QIcon(":/resources/interface/ajout.png"));
    connect(_ajouterFichiers, &QAction::triggered, this, &Outils::on_ajoutFichiersElem_clicked);

    _supprimerFichier = new QAction(tr("Supprimer"), this);
    _supprimerFichier->setIcon(QIcon(":/resources/interface/suppr.png"));
    connect(_supprimerFichier, &QAction::triggered, this, &Outils::SupprimerFichier);

    /* Retour */
    return;
}

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
            const QString nomsat = compteRendu.at(i).split("#").first();
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
    const bool etat = _ui->serveur->blockSignals(true);
    _ui->serveur->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->mapCategoriesElementsOrbitaux().keys());
    while (it.hasNext()) {
        const QString serveur = it.next();
        if (!serveur.trimmed().isEmpty()) {
            _ui->serveur->addItem(serveur);
        }
    }

    _ui->serveur->blockSignals(etat);
    _ui->serveur->setCurrentIndex(-1);
    _ui->serveur->setCurrentIndex(settings.value("fichier/serveur").toInt());

    /* Retour */
    return;
}

/*
 * Initialisation du gestionnaire des icones satellites
 */
void Outils::InitGestionnaireIcones(const QStringList &listeFicPng)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->listeIcones->clear();
    QListWidgetItem *elem;

    /* Corps de la methode */
    QStringListIterator it(listeFicPng);
    while (it.hasNext()) {

        const QString nom = it.next();

        elem = new QListWidgetItem(nom, _ui->listeIcones);
        elem->setData(Qt::CheckStateRole, Qt::Unchecked);
        elem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    }

    _ui->supprimerIcone->setEnabled(false);

    /* Retour */
    return;
}

/*
 * Initialisation du gestionnaire de TLE
 */
void Outils::InitGestionnaireTLE(const QStringList &listeFicTLE)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->listeTLE->clear();
    QListWidgetItem *elem;

    /* Corps de la methode */
    QStringListIterator it(listeFicTLE);
    while (it.hasNext()) {

        const QString nom = it.next();
        const QString fic = QDir::toNativeSeparators(Configuration::instance()->dirElem() + QDir::separator() + nom);

        if (TLE::VerifieFichier(fic) > 0) {

            elem = new QListWidgetItem(nom, _ui->listeTLE);
            elem->setData(Qt::CheckStateRole, Qt::Unchecked);
            elem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        }
    }

    _ui->supprimerTLE->setEnabled(false);

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
 * Supprimer un fichier de la liste d'un groupe d'elements orbitaux
 */
void Outils::SupprimerFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString groupe = _ui->listeGroupeElem->currentItem()->text();

    /* Corps de la methode */
    QMessageBox msgbox(QMessageBox::Question, tr("Information"), tr("Voulez-vous vraiment supprimer ce fichier du groupe <b>%1</b> ?").arg(groupe));
    const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
    msgbox.setDefaultButton(non);
    msgbox.exec();

    if (msgbox.clickedButton() == oui) {

        Configuration::instance()->mapCategoriesElementsOrbitaux()[_ui->serveur->currentText()]
                [_ui->listeGroupeElem->currentRow()].fichiers.removeOne(_ui->listeFichiersElem->currentItem()->text());

        GestionnaireXml::EcritureGestionnaireElementsOrbitaux();
        InitListeDomaines();
    }

    /* Retour */
    return;
}

/*
 * Supprimer un groupe d'elements orbitaux
 */
void Outils::SupprimerGroupe()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString groupe = _ui->listeGroupeElem->currentItem()->text();

    /* Corps de la methode */
    QMessageBox msgbox(QMessageBox::Question, tr("Information"), tr("Voulez-vous vraiment supprimer le groupe <b>%1</b> ?").arg(groupe));
    const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
    msgbox.setDefaultButton(non);
    msgbox.exec();

    if (msgbox.clickedButton() == oui) {

        Configuration::instance()->mapCategoriesElementsOrbitaux()[_ui->serveur->currentText()].removeAt(_ui->listeGroupeElem->currentRow());
        GestionnaireXml::EcritureGestionnaireElementsOrbitaux();
        InitListeDomaines();
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
    on_listeBoutonsOutils_rejected();
}

void Outils::on_listeOutils_currentRowChanged(int currentRow)
{
    _ui->stackedWidget_outils->setCurrentIndex(currentRow);
    _ui->stackedWidget_satellites->setVisible(false);
}

void Outils::on_listeOutils_itemSelectionChanged()
{
    setFixedHeight(360);
    _ui->listeFichiersElem->setCurrentRow(-1);
    _ui->stackedWidget_satellites->setVisible(false);
    _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 320, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());
}

void Outils::on_serveur_currentTextChanged(const QString &arg1)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (!arg1.isEmpty()) {

        QListWidgetItem *elem;

        _ui->listeGroupeElem->clear();
        const QString locale = Configuration::instance()->locale();

        QListIterator it(Configuration::instance()->mapCategoriesElementsOrbitaux()[arg1]);
        while (it.hasNext()) {

            const CategorieElementsOrbitaux categorie = it.next();
            const QString nom = categorie.nom[locale];

            elem = new QListWidgetItem(nom);
            elem->setData(Qt::UserRole, categorie.fichiers);
            elem->setData(Qt::CheckStateRole, categorie.miseAjour);

            _ui->listeGroupeElem->addItem(elem);
        }

        _ui->listeGroupeElem->setCurrentRow(0);
    }

    /* Retour */
    return;
}

void Outils::on_listeGroupeElem_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->stackedWidget_satellites->setVisible(false);
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

void Outils::on_listeGroupeElem_itemClicked(QListWidgetItem *item)
{
    if ((item != nullptr) && !_ui->serveur->currentText().isEmpty()) {

        const int idx = _ui->listeGroupeElem->indexFromItem(item).row();
        const Qt::CheckState check = static_cast<Qt::CheckState> (item->data(Qt::CheckStateRole).toInt());

        Configuration::instance()->mapCategoriesElementsOrbitaux()[_ui->serveur->currentText()][idx].miseAjour = check;
    }
}

void Outils::on_listeGroupeElem_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QListWidgetItem const *item = _ui->listeGroupeElem->itemAt(pos);

    /* Corps de la methode */
    QMenu menu(this);
    menu.addAction(_creerGroupe);

    if (item != nullptr) {
        menu.addAction(_supprimerGroupe);
    }

    menu.exec(QCursor::pos());

    /* Retour */
    return;
}

void Outils::on_listeFichiersElem_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QListWidgetItem const *item = _ui->listeFichiersElem->itemAt(pos);

    /* Corps de la methode */
    QMenu menu(this);
    menu.addAction(_ajouterFichiers);

    if (item != nullptr) {
        menu.addAction(_supprimerFichier);
    }

    menu.exec(QCursor::pos());

    /* Retour */
    return;
}

void Outils::on_creationGroupe_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->listeFichiersElem->setCurrentRow(-1);
    _ui->listeFichiers->clear();
    _ui->domaine->clear();
    _ui->domaine->setEnabled(true);
    _ui->domaine->setFocus();

    _ui->nomGroupe->clear();
    _ui->nomGroupe->setEnabled(true);
    _ui->stackedWidget_satellites->setVisible(true);
    _ui->stackedWidget_satellites->setCurrentWidget(_ui->groupe);
    _ui->valider->setDefault(true);

    setFixedHeight(360);
    _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 320, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());

    /* Retour */
    return;
}

void Outils::on_ajoutFichiersElem_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->listeFichiers->clear();
    _ui->listeFichiersElem->setCurrentRow(-1);
    _ui->stackedWidget_satellites->setVisible(true);
    _ui->stackedWidget_satellites->setCurrentWidget(_ui->groupe);
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

    setFixedHeight(360);
    _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 320, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());

    /* Retour */
    return;
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

void Outils::on_ageMaxElementsOrbitaux_toggled(bool checked)
{
    settings.setValue("temps/ageMaxElementsOrbitaux", checked);
}

void Outils::on_valider_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Nom du domaine
        const QString domaine = _ui->domaine->text().trimmed();
        if (domaine.isEmpty()) {
            throw Exception(tr("Le nom du domaine n'est pas spécifié"), MessageType::WARNING);
        }

        // Nom du groupe (dans la langue de l'utilisateur)
        const QString nomGroupe = _ui->nomGroupe->text().trimmed();
        if (nomGroupe.isEmpty()) {
            throw Exception(tr("Le nom du groupe n'est pas spécifié"), MessageType::WARNING);
        }

        // Liste des fichiers d'elements orbitaux
        const QStringList listeFics = _ui->listeFichiers->document()->toPlainText().split("\n");
        if (listeFics.isEmpty()) {
            throw Exception(tr("La liste de fichiers est vide"), MessageType::WARNING);
        }

        QMap<QString, QList<CategorieElementsOrbitaux> > &mapElem = Configuration::instance()->mapCategoriesElementsOrbitaux();

        bool atrouve = false;
        int idx = 0;
        QListIterator it(mapElem[domaine]);
        while (it.hasNext()) {

            const CategorieElementsOrbitaux categ = it.next();

            if (categ.nom[Configuration::instance()->locale()] == nomGroupe) {

                atrouve = true;
                mapElem[domaine][idx].fichiers = listeFics;
                it.toBack();
            }
            idx++;
        }

        if (!atrouve) {

            QMap<QString, QString> nom;
            nom.insert(Configuration::instance()->locale(), nomGroupe);

            const CategorieElementsOrbitaux categorie = { Qt::Unchecked, nom, listeFics };
            const QList<CategorieElementsOrbitaux> liste(QList<CategorieElementsOrbitaux>() << categorie);
            mapElem[domaine].append(liste);
        }
        
        GestionnaireXml::EcritureGestionnaireElementsOrbitaux();
        InitListeDomaines();

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

void Outils::on_annuler_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->stackedWidget_satellites->setVisible(false);
    _ui->domaine->setVisible(true);
    _ui->nomGroupe->setVisible(true);

    /* Retour */
    return;
}

void Outils::on_listeFichiersElem_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->listeSatellites->clear();
    _ui->stackedWidget_satellites->setVisible(false);

    /* Corps de la methode */
    if (currentRow >= 0) {

        QListWidgetItem *item;
        const QString fichier = Configuration::instance()->dirElem() + QDir::separator() + _ui->listeFichiersElem->currentItem()->text();

        QFileInfo ff(fichier);
        if (ff.exists()) {

            _mapElem = GPFormat::Lecture(fichier, Configuration::instance()->dbSatellites());

            QMapIterator it(_mapElem);
            while (it.hasNext()) {
                it.next();

                const ElementsOrbitaux elem = it.value();

                item = new QListWidgetItem(elem.nom, _ui->listeSatellites);
                item->setData(Qt::UserRole, it.key());
            }

            _ui->listeSatellites->sortItems();

            _ui->stackedWidget_satellites->setVisible(true);
            _ui->stackedWidget_satellites->setCurrentWidget(_ui->satellites);

        } else {
            setFixedHeight(360);
            _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 320, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());
        }
    }

    /* Retour */
    return;
}

void Outils::on_listeFichiersElem_itemSelectionChanged()
{
    if (_ui->listeFichiersElem->selectedItems().isEmpty()) {
        setFixedHeight(360);
        _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 320, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());
    }
}

void Outils::on_listeSatellites_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        const QString norad = _ui->listeSatellites->currentItem()->data(Qt::UserRole).toString();
        Satellite satellite(_mapElem[norad]);
        _informations->show(satellite);
        setFixedHeight(600);
        _ui->listeBoutonsOutils->setGeometry(_ui->listeBoutonsOutils->x(), 560, _ui->listeBoutonsOutils->width(), _ui->listeBoutonsOutils->height());
    }

    /* Retour */
    return;
}

void Outils::on_filtreSatellites_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeSatellites->count(); i++) {
        const QString elem = _ui->listeSatellites->item(i)->text();
        _ui->listeSatellites->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}


void Outils::on_majMaintenant_clicked()
{
    /* Declarations des variables locales */

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

        const QStringList compteRendu = TLE::MiseAJourFichier(fichierAmettreAjour, fichierAlire, Configuration::instance()->dbSatellites(), affMsg);

        EcritureCompteRenduMaj(compteRendu, _ui->compteRenduMajAuto);
    }

    disconnect(&tel, &Telechargement::Progression, this, &Outils::ProgressionTLE);
    _ui->frameBarreProgressionTLE->setVisible(false);

    /* Retour */
    return;
}

void Outils::on_listeBoutonsOutils_rejected()
{
    GestionnaireXml::EcritureGestionnaireElementsOrbitaux();

    _ui->compteRenduMajAuto->setVisible(false);
    _ui->compteRenduMajManuel->setVisible(false);

    settings.setValue("fichier/serveur", _ui->serveur->currentIndex());
    settings.setValue("fichier/fichierAMettreAJour", _ui->fichierAMettreAJour->text());
    settings.setValue("fichier/fichierALire", _ui->fichierALire->text());
    settings.setValue("fichier/affichageMsgMAJ", _ui->affichageMsgMAJ->currentIndex());
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
            throw Exception(tr("Le nom du fichier à mettre à jour n'est pas spécifié"), MessageType::WARNING);
        }

        if (_ui->fichierALire->text().isEmpty()) {
            throw Exception(tr("Le nom du fichier à lire n'est pas spécifié"), MessageType::WARNING);
        }

        QFileInfo fi(_ui->fichierALire->text());
        QString fic = QDir::toNativeSeparators(fi.absoluteFilePath());

        const QStringList listeFic(QStringList () << _ui->fichierAMettreAJour->text() << fic);
        foreach(QString file, listeFic) {
            fi = QFileInfo(file);
            if (!fi.exists()) {
                const QString msg = tr("Le fichier %1 n'existe pas");
                throw Exception(msg.arg(fi.absoluteFilePath()), MessageType::WARNING);
            }
        }

        const int affMsg = _ui->affichageMsgMAJ->currentIndex();
        const QStringList compteRendu = TLE::MiseAJourFichier(_ui->fichierAMettreAJour->text(), fic, Configuration::instance()->dbSatellites(), affMsg);

        EcritureCompteRenduMaj(compteRendu, _ui->compteRenduMajManuel);

        _ui->compteRenduMajManuel->setVisible(true);


    } catch (Exception const &e) {
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

void Outils::on_listeTLE_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->listeTLE->hasFocus() && (_ui->listeTLE->currentRow() >= 0)) {

        item->setData(Qt::CheckStateRole, (item->checkState() == Qt::Checked) ? Qt::Unchecked : Qt::Checked);
    }

    _ui->supprimerTLE->setEnabled(false);
    for(int i=0; i<_ui->listeTLE->count(); i++) {
        if (_ui->listeTLE->item(i)->checkState() == Qt::Checked) {
            _ui->supprimerTLE->setEnabled(true);
        }
    }

    /* Retour */
    return;
}

void Outils::on_importerTLE_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Ouverture d'un fichier TLE
        const QString fichier = QFileDialog::getOpenFileName(this, tr("Importer TLE..."),
                                                             QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                             tr("Fichiers TLE (*.txt *.tle);;Tous les fichiers (*.*)"));

        // Ouverture du fichier d'elements orbitaux
        if (!fichier.isEmpty()) {

            qInfo().noquote() << "Ouverture du fichier" << fichier;

            QFileInfo ff(fichier);
            const int nbElem = TLE::VerifieFichier(fichier, true);

            if (nbElem > 0) {

                QFile fo(Configuration::instance()->instance()->dirElem() + QDir::separator() + ff.fileName());

                if (fo.exists()) {

                    qWarning() << "Le fichier TLE existe déjà";
                    throw Exception(tr("Le fichier %1 existe déjà").arg(ff.fileName()), MessageType::WARNING);

                } else {

                    QFile fi(fichier);

                    // Le fichier contient des elements orbitaux, on le copie dans le repertoire d'elements orbitaux
                    if (fi.copy(fo.fileName())) {

                        qInfo().noquote() << "Import du fichier TLE" << ff.fileName() << "OK";

                        const QDir di(Configuration::instance()->dirElem());
                        const QStringList filtres(QStringList () << "*.txt" << "*.tle");
                        InitGestionnaireTLE(di.entryList(filtres, QDir::Files));

                    } else {
                        qWarning().noquote() << "Import du fichier TLE" << ff.fileName() << "KO";
                    }
                }
            }
        }

    } catch (Exception const &e) {
    }

    qInfo() << "Fin   Fonction" << __FUNCTION__;

    /* Retour */
    return;
}

void Outils::on_supprimerTLE_clicked()
{
    /* Declarations des variables locales */
    QStringList listeFicTLE;

    /* Initialisations */

    /* Corps de la methode */
    for(int i=0; i<_ui->listeTLE->count(); i++) {
        if (_ui->listeTLE->item(i)->checkState() == Qt::Checked) {
            listeFicTLE.append(_ui->listeTLE->item(i)->text());
        }
    }

    QMessageBox msgbox(QMessageBox::Question, tr("Avertissement"), tr("Voulez-vous vraiment supprimer les fichiers TLE sélectionnés ?"));
    const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
    msgbox.setDefaultButton(non);
    msgbox.exec();

    if (msgbox.clickedButton() == oui) {

        QFile fi;
        QStringListIterator it(listeFicTLE);
        while (it.hasNext()) {

            const QString fic = it.next();
            fi.setFileName(Configuration::instance()->dirElem() + QDir::separator() + fic);
            fi.remove();
        }

        emit InitFicGP();

        const QDir di(Configuration::instance()->dirElem());
        const QStringList filtres(QStringList () << "*.txt" << "*.tle");
        InitGestionnaireTLE(di.entryList(filtres, QDir::Files));
    }

    /* Retour */
    return;
}

void Outils::on_listeIcones_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_ui->listeIcones->hasFocus() && (_ui->listeIcones->currentRow() >= 0)) {

        item->setData(Qt::CheckStateRole, (item->checkState() == Qt::Checked) ? Qt::Unchecked : Qt::Checked);
    }

    _ui->supprimerIcone->setEnabled(false);
    for(int i=0; i<_ui->listeIcones->count(); i++) {
        if (_ui->listeIcones->item(i)->checkState() == Qt::Checked) {
            _ui->supprimerIcone->setEnabled(true);
        }
    }

    /* Retour */
    return;
}

void Outils::on_importerIcone_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        // Ouverture d'un fichier png
        const QString fichier = QFileDialog::getOpenFileName(this, tr("Importer icône..."),
                                                             QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                                             tr("Fichiers PNG (*.png)"));

        if (!fichier.isEmpty()) {

            qInfo().noquote() << "Ouverture du fichier png" << fichier;

            const QString nomfic = QFileInfo(fichier).fileName();

            // Renommage de l'icone
            QInputDialog input(this, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
            input.setWindowTitle(tr("Renommer l'icône"));
            input.setLabelText(tr("Nom de l'icône (nom de l'objet ou numéro NORAD) :"));
            input.setTextValue(nomfic);
            input.setTextEchoMode(QLineEdit::Normal);
            input.setOkButtonText(tr("OK"));
            input.setCancelButtonText(tr("Annuler"));

            const int ret = input.exec();

            if (ret != 0) {

                const QString res = input.textValue();

                if (!res.isEmpty()) {

                    // Verification du nom de l'icone
                    const QRegularExpression reg("\\d*");
                    QString nomFichierPng = (reg.match(res).hasMatch()) ? QString("%1").arg(res, 6, QChar('0')) : res;

                    if (!nomFichierPng.endsWith(".png")) {
                        nomFichierPng += ".png";
                    }

                    QFile fi(fichier);
                    QFile fi2(Configuration::instance()->dirRsc() + QDir::separator() + nomFichierPng);

                    if (fi2.exists()) {
                        fi2.remove();
                    }

                    // Sauvegarde de l'icone
                    if (fi.copy(fi2.fileName())) {

                        qInfo().noquote() << "Import du fichier PNG" << nomFichierPng << "OK";

                        const QDir di(Configuration::instance()->dirRsc());
                        const QStringList filtres(QStringList () << "*.png");
                        InitGestionnaireIcones(di.entryList(filtres, QDir::Files));

                        emit AffichageCartesRadar();

                    } else {
                        qWarning().noquote() << "Import du fichier PNG" << nomFichierPng << "KO";
                    }
                }
            }
        }
    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

void Outils::on_supprimerIcone_clicked()
{
    /* Declarations des variables locales */
    QStringList listeFicPng;

    /* Initialisations */

    /* Corps de la methode */
    for(int i=0; i<_ui->listeIcones->count(); i++) {
        if (_ui->listeIcones->item(i)->checkState() == Qt::Checked) {
            listeFicPng.append(_ui->listeIcones->item(i)->text());
        }
    }

    QMessageBox msgbox(QMessageBox::Question, tr("Avertissement"), tr("Voulez-vous vraiment supprimer les icônes sélectionnées ?"));
    const QPushButton * const oui = msgbox.addButton(tr("Oui"), QMessageBox::YesRole);
    QPushButton * const non = msgbox.addButton(tr("Non"), QMessageBox::NoRole);
    msgbox.setDefaultButton(non);
    msgbox.exec();

    if (msgbox.clickedButton() == oui) {

        QFile fi;
        QStringListIterator it(listeFicPng);
        while (it.hasNext()) {

            const QString fic = it.next();
            fi.setFileName(Configuration::instance()->dirRsc() + QDir::separator() + fic);
            fi.remove();
        }

        const QDir di(Configuration::instance()->dirRsc());
        const QStringList filtres(QStringList () << "*.png");
        InitGestionnaireIcones(di.entryList(filtres, QDir::Files));
    }

    /* Retour */
    return;
}
