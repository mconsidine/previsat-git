/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    gestionnairetle.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Gestionnaire de mise a jour des TLE
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2012
 *
 * Date de revision
 * >    30 octobre 2021
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <QMessageBox>
#include <QSettings>
#pragma GCC diagnostic ignored "-Wswitch-default"
#include "ui_gestionnairetle.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "gestionnairetle.h"
#include "configuration/configuration.h"
#include "librairies/exceptions/previsatexception.h"


static QSettings settings("Astropedia", "previsat");


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
GestionnaireTLE::GestionnaireTLE(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::GestionnaireTLE)
{
    ui->setupUi(this);
    setFont(Configuration::instance()->police());
    load();
}

/*
 * Destructeur
 */
GestionnaireTLE::~GestionnaireTLE()
{
    delete ui;
}

/*
 * Accesseurs
 */

/*
 * Constantes publiques
 */

/*
 * Variables publiques
 */

/*
 * Methodes publiques
 */


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
void GestionnaireTLE::load()
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->barreMenu->setVisible(false);
    ui->groupe->setVisible(false);
    ui->listeGroupeTLE->clear();
    ui->listeFichiersTLE->clear();
    ui->nbJoursAgeMaxTLE->setValue(settings.value("temps/ageMax", 15).toInt());
    ui->ageMaxTLE->setChecked(settings.value("temps/ageMaxTLE", true).toBool());

    const QIcon ajout(":/resources/ajout.png");
    ui->actionCreer_un_groupe->setIcon(ajout);
    ui->creationGroupe->setIcon(ajout);
    ui->creationGroupe->setToolTip(tr("Créer un groupe de TLE"));

    ui->actionAjouter_des_fichiers->setIcon(ajout);
    ui->ajoutFichiersTLE->setIcon(ajout);
    ui->ajoutFichiersTLE->setToolTip(tr("Ajouter des fichiers TLE"));

    const QIcon suppr(":/resources/suppr.png");
    ui->actionSupprimer->setIcon(suppr);
    ui->actionSupprimerGroupe->setIcon(suppr);

    if (settings.value("affichage/flagIntensiteVision", false).toBool()) {

        QPalette paletteWin, palList;
        const int red = settings.value("affichage/valIntensiteVision", 0).toInt();
        const QBrush alpha = QBrush(QColor::fromRgb(red, 0, 0, 255));
        const QColor coulList = QColor(red + 27, 0, 0);

        paletteWin.setBrush(this->backgroundRole(), alpha);
        palList.setColor(QPalette::Base, coulList);

        this->setPalette(paletteWin);
        ui->listeGroupeTLE->setPalette(palList);
        ui->listeFichiersTLE->setPalette(palList);
        ui->domaine->setPalette(palList);
        ui->nomGroupe->setPalette(palList);
        ui->listeFichiers->setPalette(palList);
        ui->nbJoursAgeMaxTLE->setPalette(palList);
    }

    /* Corps de la methode */
    QListIterator<CategorieTLE> it(Configuration::instance()->listeCategoriesTLE());
    while (it.hasNext()) {
        ui->listeGroupeTLE->addItem(it.next().nom[Configuration::instance()->locale()]);
    }

    ui->listeGroupeTLE->setCurrentRow(0);

    /* Retour */
    return;
}

void GestionnaireTLE::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    on_fermer_clicked();
}


void GestionnaireTLE::on_fermer_clicked()
{
    settings.setValue("temps/ageMax", ui->nbJoursAgeMaxTLE->value());
    settings.setValue("temps/ageMaxTLE", ui->ageMaxTLE->isChecked());
    close();
}

void GestionnaireTLE::on_actionCreer_un_groupe_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->listeFichiers->clear();
    ui->domaine->setText("");
    ui->domaine->setEnabled(true);
    ui->domaine->setFocus();

    ui->nomGroupe->setText("");
    ui->nomGroupe->setEnabled(true);
    ui->groupe->setVisible(true);

    /* Retour */
    return;
}

void GestionnaireTLE::on_actionSupprimerGroupe_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int index = ui->listeGroupeTLE->currentRow();

    /* Corps de la methode */
    if (index >= 0) {

        const QString groupe = ui->listeGroupeTLE->currentItem()->text();
        const QString msg = tr("Voulez-vous vraiment supprimer le groupe \"%1\"?");
        QMessageBox msgbox(tr("Information"), msg.arg(groupe), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                           QMessageBox::No, QMessageBox::NoButton, this);
        msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
        msgbox.setButtonText(QMessageBox::No, tr("Non"));
        msgbox.exec();
        const int res = msgbox.result();

        if (res == QMessageBox::Yes) {

            Configuration::instance()->listeCategoriesTLE().removeAt(index);

            const bool etat = ui->listeGroupeTLE->blockSignals(true);
            load();
            ui->listeGroupeTLE->blockSignals(etat);
            ui->listeGroupeTLE->setCurrentRow(-1);
            ui->listeGroupeTLE->setCurrentRow(0);
        }
    }

    /* Retour */
    return;
}

void GestionnaireTLE::on_listeGroupeTLE_customContextMenuRequested(const QPoint &position)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->listeGroupeTLE->setCurrentRow(ui->listeGroupeTLE->indexAt(position).row());
    ui->actionSupprimerGroupe->setVisible(ui->listeGroupeTLE->currentRow() >= 0);
    ui->menuContextuelGroupe->exec(QCursor::pos());

    /* Retour */
    return;
}

void GestionnaireTLE::on_listeGroupeTLE_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->groupe->setVisible(false);

    /* Corps de la methode */
    ui->listeFichiersTLE->clear();
    if (!ui->listeFichiersTLE->isEnabled()) {
        ui->listeFichiersTLE->setEnabled(true);
    }

    if (currentRow >= 0) {
        QStringListIterator it(Configuration::instance()->listeCategoriesTLE().at(currentRow).fichiers);
        while (it.hasNext()) {
            ui->listeFichiersTLE->addItem(it.next());
            ui->MajAutoGroupe->setChecked(settings.value("TLE/" + Configuration::instance()->listeCategoriesTLE().at(currentRow)
                                                         .nom[Configuration::instance()->locale()], 0).toInt() == 1);
        }
    }

    /* Retour */
    return;
}

void GestionnaireTLE::on_valider_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString domaine = ui->domaine->text().trimmed();
        if (domaine.isEmpty()) {
            throw PreviSatException(tr("Le nom du domaine n'est pas spécifié"), WARNING);
        }

        const QString nomGroupe = ui->nomGroupe->text().trimmed();
        if (nomGroupe.isEmpty()) {
            throw PreviSatException(tr("Le nom du groupe n'est pas spécifié"), WARNING);
        }

        QMap<QString, QString> groupeDomaine;
        groupeDomaine[Configuration::instance()->locale()] = nomGroupe + "@" + domaine;
        const QStringList listeFics = ui->listeFichiers->document()->toPlainText().split("\n");

        Configuration::instance()->listeCategoriesTLE().append({ groupeDomaine, domaine, listeFics });

        const bool etat = ui->listeGroupeTLE->blockSignals(true);
        load();
        ui->listeGroupeTLE->blockSignals(etat);
        ui->groupe->setVisible(false);
        ui->listeGroupeTLE->setCurrentRow(-1);
        ui->listeGroupeTLE->setCurrentRow(0);

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void GestionnaireTLE::on_annuler_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->groupe->setVisible(false);
    ui->domaine->setVisible(true);
    ui->nomGroupe->setVisible(true);

    /* Retour */
    return;
}

void GestionnaireTLE::on_actionAjouter_des_fichiers_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QStringList nomGroupe = ui->listeGroupeTLE->currentItem()->text().split("@");
    ui->groupe->setEnabled(true);
    ui->nomGroupe->setEnabled(false);
    ui->nomGroupe->setText(nomGroupe.at(0));

    ui->domaine->setEnabled(false);
    ui->domaine->setText(nomGroupe.at(1));

    ui->listeFichiers->clear();
    for(int i=0; i<ui->listeFichiersTLE->count(); i++) {
        ui->listeFichiers->setPlainText(ui->listeFichiers->document()->toPlainText() +
                                        ui->listeFichiersTLE->item(i)->text().trimmed() + "\n");
    }

    ui->groupe->setVisible(true);
    ui->listeFichiers->moveCursor(QTextCursor::End);
    ui->listeFichiers->setFocus();

    /* Retour */
    return;
}

void GestionnaireTLE::on_actionSupprimer_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString msg = tr("Voulez-vous vraiment supprimer ce(s) fichier(s) du groupe \"%1\"?");
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();

    QMessageBox msgbox(tr("Information"), msg.arg(groupe), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::Yes) {

        const int idxGrp = ui->listeGroupeTLE->currentRow();

        if (idxGrp >= 0) {

            QStringList &fichiers = Configuration::instance()->listeCategoriesTLE()[idxGrp].fichiers;

            const int idxFic = ui->listeFichiersTLE->currentRow();
            if (idxFic >= 0) {
                fichiers.removeAt(idxFic);
            }
        }

        const bool etat = ui->listeGroupeTLE->blockSignals(true);
        load();
        ui->listeGroupeTLE->blockSignals(etat);
        ui->listeGroupeTLE->setCurrentRow(-1);
        ui->listeGroupeTLE->setCurrentRow(0);
    }

    /* Retour */
    return;
}

void GestionnaireTLE::on_listeFichiersTLE_customContextMenuRequested(const QPoint &position)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->groupe->setVisible(false);
    ui->listeFichiersTLE->setCurrentRow(ui->listeFichiersTLE->indexAt(position).row());
    ui->actionSupprimer->setVisible(ui->listeFichiersTLE->currentRow() >= 0);
    ui->menuContextuelFichiersTLE->exec(QCursor::pos());

    /* Retour */
    return;
}

void GestionnaireTLE::on_MajAutoGroupe_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (ui->listeGroupeTLE->currentRow() >= 0) {
        settings.setValue("TLE/" + ui->listeGroupeTLE->currentItem()->text(), (checked) ? 1 : 0);
    }

    if (checked) {
        settings.setValue("temps/lastUpdate", 0.);
    }

    /* Retour */
    return;
}

void GestionnaireTLE::on_creationGroupe_clicked()
{
    on_actionCreer_un_groupe_triggered();
}

void GestionnaireTLE::on_ajoutFichiersTLE_clicked()
{
    on_actionAjouter_des_fichiers_triggered();
}
