/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    12 juin 2015
 *
 */

#include <QDesktopServices>
#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QMessageBox>
#include <QTextStream>
#include "ui_gestionnairetle.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-conversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "gestionnairetle.h"
#include "previsat.h"
#include "librairies/exceptions/previsatexception.h"

static bool init;
static int selec;
static QString ficTLE;
static QString dirLocalData;
static QString dirTmp;
static QString localePrevisat;
static QSettings settings("Astropedia", "previsat");

GestionnaireTLE::GestionnaireTLE(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::GestionnaireTLE)
{
    ui->setupUi(this);

    QFont police;

#if defined (Q_OS_WIN)
    police.setFamily("MS Shell Dlg 2");
    police.setPointSize(8);

#elif defined (Q_OS_LINUX)
    police.setFamily("Sans Serif");
    police.setPointSize(7);

#elif defined (Q_OS_MAC)
    police.setFamily("Marion");
    police.setPointSize(11);
#else
#endif

    setFont(police);
    load();
}

GestionnaireTLE::~GestionnaireTLE()
{
    delete ui;
}

void GestionnaireTLE::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    on_fermer_clicked();
}

void GestionnaireTLE::load()
{
    /* Declarations des variables locales */

    /* Initialisations */
    selec = -1;
    localePrevisat = PreviSat::DeterminationLocale();
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

    dirLocalData = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(), QStandardPaths::LocateDirectory) + "data";

    dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);
    ficTLE = dirLocalData + QDir::separator() + "gestionnaireTLE_" + localePrevisat + ".gst";

    /* Corps de la methode */
    QFile fi(ficTLE);
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    while (!flux.atEnd()) {
        const QStringList ligne = flux.readLine().split("#");
        QString nomGroupe = ligne.at(0);
        nomGroupe[0] = nomGroupe[0].toUpper();
        ui->listeGroupeTLE->addItem(nomGroupe);
    }
    fi.close();

    ui->listeGroupeTLE->setCurrentRow(0);
    init = true;

    /* Retour */
    return;
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
    QDir di(dirTmp);
    if (!di.exists())
        di.mkpath(dirTmp);

    /* Corps de la methode */
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer le groupe \"%1\"?");
    QMessageBox msgbox(tr("Information"), msg.arg(groupe), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::Yes) {

        const QString adresse = groupe.toLower();

        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);

        QFile sw(dirTmp + QDir::separator() + "grp.tmp");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&sw);

        while (!flux.atEnd()) {
            const QString ligne = flux.readLine();
            if (ligne.mid(0, adresse.length()) != adresse)
                flux2 << ligne << endl;
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(ficTLE);

        load();
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
    init = false;
    ui->groupe->setVisible(false);

    /* Corps de la methode */
    if (selec != currentRow) {
        selec = currentRow;
        if (selec >= 0) {

            ui->listeFichiersTLE->clear();
            if (!ui->listeFichiersTLE->isEnabled())
                ui->listeFichiersTLE->setEnabled(true);
            const QString groupe = ui->listeGroupeTLE->currentItem()->text();
            const QString adresse = groupe.toLower();

            QFile sr(ficTLE);
            sr.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&sr);

            while (!flux.atEnd()) {
                const QString ligne = flux.readLine();
                if (adresse == ligne.mid(0, ligne.lastIndexOf("#") - 2)) {
                    QStringList listeTLE = ligne.split("#").at(2).split(",");

                    QStringListIterator it(listeTLE);
                    while (it.hasNext()) {
                        const QString item = it.next();
                        if (!item.isEmpty())
                            ui->listeFichiersTLE->addItem(item);
                    }
                    ui->MajAutoGroupe->setChecked(settings.value("TLE/" + adresse, 0).toInt() == 1);
                }
            }
            sr.close();
            init = true;
        }
    }
    ui->listeFichiersTLE->setCurrentRow(-1);

    /* Retour */
    return;
}

void GestionnaireTLE::on_valider_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {
        if (ui->domaine->text().trimmed().isEmpty())
            throw PreviSatException(tr("Le nom du domaine n'est pas spécifié"), WARNING);

        if (ui->nomGroupe->text().trimmed().isEmpty())
            throw PreviSatException(tr("Le nom du groupe n'est pas spécifié"), WARNING);

        const QString groupeDomaine = ui->nomGroupe->text().toLower().trimmed() + "@" + ui->domaine->text().trimmed();
        const QString listeFics = ui->listeFichiers->document()->toPlainText().replace("\n", ",");

        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);
        QStringList list;
        bool ajout = false;
        while (!flux.atEnd()) {
            const QString ligne = flux.readLine();
            if (ligne.mid(0, ligne.indexOf("#")) == groupeDomaine) {
                list.append(groupeDomaine + "#0#" + listeFics);
                settings.setValue("TLE/" + groupeDomaine, (ui->MajAutoGroupe->isChecked()) ? "1" : "0");
                ajout = true;
            } else {
                list.append(ligne);
            }
        }
        if (!ajout) {
            list.append(groupeDomaine + "#0#" + listeFics);
            settings.setValue("TLE/" + groupeDomaine, (ui->MajAutoGroupe->isChecked()) ? "1" : "0");
        }
        sr.close();

        QFile sw(ficTLE);
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&sw);

        QStringListIterator it(list);
        while (it.hasNext()) {
            flux2 << it.next() << endl;
        }
        sw.close();

        load();
        ui->groupe->setVisible(false);

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
    for(int i=0; i<ui->listeFichiersTLE->count(); i++)
        ui->listeFichiers->setPlainText(ui->listeFichiers->document()->toPlainText() + ui->listeFichiersTLE->item(i)->text() + "\n");

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
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer ce(s) fichier(s) du groupe \"%1\"?");

    QMessageBox msgbox(tr("Information"), msg.arg(groupe), QMessageBox::Question, QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No, QMessageBox::NoButton, this);
    msgbox.setButtonText(QMessageBox::Yes, tr("Oui"));
    msgbox.setButtonText(QMessageBox::No, tr("Non"));
    msgbox.exec();
    const int res = msgbox.result();

    if (res == QMessageBox::Yes) {

        const QString adresse = groupe.toLower();
        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);

        QDir di(dirTmp);
        if (!di.exists())
            di.mkpath(dirTmp);

        QFile sw(dirTmp + QDir::separator() + "tmp.txt");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&sw);

        while (!flux.atEnd()) {
            QString ligne = flux.readLine();
            if (ligne.mid(0, adresse.length()) == adresse) {
                for(int i=0; i<ui->listeFichiersTLE->selectedItems().count(); i++)
                    ligne = ligne.replace(ui->listeFichiersTLE->selectedItems().at(i)->text(), "").replace(",,", ",").
                            replace("#,", "#");
                if (ligne.endsWith(","))
                    ligne = ligne.remove(ligne.length() - 1, 1);
            }
            flux2 << ligne << endl;
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(sr.fileName());
        load();
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
    Q_UNUSED(checked)

    /* Corps de la methode */
    if (init)
        settings.setValue("TLE/" + ui->listeGroupeTLE->currentItem()->text(), (checked) ? 1 : 0);

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
