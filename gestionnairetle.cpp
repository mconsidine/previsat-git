/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include "librairies/exceptions/previsatexception.h"
#include "gestionnairetle.h"
#include "ui_gestionnairetle.h"

static bool init;
static int selec;
static QString dirOut;
static QString ficTLE;

GestionnaireTLE::GestionnaireTLE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GestionnaireTLE)
{
    ui->setupUi(this);
    load();
}

GestionnaireTLE::~GestionnaireTLE()
{
    delete ui;
}

void GestionnaireTLE::load()
{
    QCoreApplication::setApplicationName("PreviSat");
    QCoreApplication::setOrganizationName("Astropedia");
    dirOut = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    selec = -1;
    ui->listeGroupeTLE->clear();
    ui->listeFichiersTLE->clear();
    ficTLE = QCoreApplication::applicationDirPath() + QDir::separator() + "data" + QDir::separator() +
            "gestionnaireTLE.gst";
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
    ui->barreMenu->setVisible(false);
    ui->listeGroupeTLE->setCurrentRow(0);
    init = true;
}

void GestionnaireTLE::on_fermer_clicked()
{
    close();
}

void GestionnaireTLE::on_actionCreer_un_groupe_activated()
{
    ui->listeFichiers->clear();
    ui->domaine->setText("");
    ui->domaine->setEnabled(true);
    ui->domaine->setFocus();

    ui->nomGroupe->setText("");
    ui->nomGroupe->setEnabled(true);
    ui->groupe->setVisible(true);
}

void GestionnaireTLE::on_actionSupprimerGroupe_activated()
{
    QDir di(dirOut);
    if (!di.exists())
        di.mkpath(dirOut);

    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer le groupe ""%1""?");
    const int res = QMessageBox::question(this, tr("Information"), msg.arg(groupe), QMessageBox::Yes | QMessageBox::No);

    if (res == QMessageBox::Yes) {

        const QString adresse = groupe.toLower();

        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);

        QFile sw(dirOut + QDir::separator() + "grp.tmp");
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
}

void GestionnaireTLE::on_listeGroupeTLE_customContextMenuRequested(const QPoint &pos)
{
    ui->listeGroupeTLE->setCurrentRow(ui->listeGroupeTLE->indexAt(pos).row());
    ui->actionSupprimerGroupe->setVisible(ui->listeGroupeTLE->currentRow() >= 0);
    ui->menuContextuelGroupe->exec(QCursor::pos());
}

void GestionnaireTLE::on_listeGroupeTLE_currentRowChanged(int currentRow)
{
    init = false;
    ui->groupe->setVisible(false);
    ui->listeFichiersTLE->setCurrentRow(-1);
    if (selec != ui->listeGroupeTLE->currentRow()) {
        selec = ui->listeGroupeTLE->currentRow();
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

                    ui->MajAutoGroupe->setChecked(ligne.mid(ligne.lastIndexOf("#") - 1, 1) == "1");
                }
            }
            sr.close();
            init = true;
        }
    }
}

void GestionnaireTLE::on_valider_clicked()
{
    try {
        if (ui->domaine->text().trimmed().isEmpty())
            throw PreviSatException(tr("Le nom du domaine n'est pas sp�cifi�"), Messages::WARNING);

        if (ui->nomGroupe->text().trimmed().isEmpty())
            throw PreviSatException(tr("Le nom du groupe n'est pas sp�cifi�"), Messages::WARNING);

        const QString ligne = ui->nomGroupe->text().trimmed() + "@" + ui->domaine->text().trimmed();
        if (ui->nomGroupe->isEnabled()) {
            for(int i=0; i<ui->listeGroupeTLE->count(); i++) {
                if (ligne.toLower() == ui->listeGroupeTLE->item(i)->text().toLower()) {
                    const QString msg = tr("Le groupe ""%1"" existe d�j�");
                    throw PreviSatException(msg.arg(ligne), Messages::WARNING);
                }
            }
        }

        QFile sw(ficTLE);
        sw.open(QIODevice::Append | QIODevice::Text);
        QTextStream flux(&sw);
        flux << ligne + "#" + ((ui->MajAutoGroupe->isChecked()) ? "1" : "0") + ui->listeFichiers->text().replace("\n", ",") << endl;
        sw.close();
        load();
        ui->groupe->setVisible(false);

    } catch (PreviSatException &e) {
    }
}

void GestionnaireTLE::on_annuler_clicked()
{
    ui->groupe->setVisible(false);
    ui->domaine->setVisible(true);
    ui->nomGroupe->setVisible(true);
}

void GestionnaireTLE::on_actionAjouter_des_fichiers_activated()
{
    const QStringList nomGroupe = ui->listeGroupeTLE->currentItem()->text().split("@");
    ui->groupe->setEnabled(false);
    ui->nomGroupe->setText(nomGroupe.at(0));

    ui->domaine->setEnabled(false);
    ui->domaine->setText(nomGroupe.at(1));

    ui->listeFichiers->clear();
    for(int i=0; i<ui->listeFichiersTLE->count(); i++) {
        ui->listeFichiers->setText(ui->listeFichiers->text() + "\n" + ui->listeFichiersTLE->item(i)->text());
    }
    ui->groupe->setVisible(true);
    ui->listeFichiers->setFocus();
}

void GestionnaireTLE::on_actionSupprimer_activated()
{
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer ce(s) fichier(s) du groupe ""%1""?");
    const int res = QMessageBox::question(this, tr("Information"), msg.arg(groupe), QMessageBox::Yes | QMessageBox::No);

    if (res == QMessageBox::Yes) {

        const QString adresse = groupe.toLower();
        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);

        QDir di(dirOut);
        if (!di.exists())
            di.mkpath(dirOut);

        QFile sw(dirOut + QDir::separator() + "tmp.txt");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&sw);

        while (!flux.atEnd()) {
            QString ligne = flux.readLine();
            if (ligne.mid(0, adresse.length()) == adresse) {
                for(int i=0; i<ui->listeFichiersTLE->selectedItems().count(); i++)
                    ligne = ligne.replace(ui->listeFichiersTLE->selectedItems().at(i)->text(), "").replace(",,", ",").replace("#,", "#");
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
}

void GestionnaireTLE::on_listeFichiersTLE_customContextMenuRequested(const QPoint &pos)
{
    ui->groupe->setVisible(false);
    ui->actionSupprimer->setVisible(ui->listeFichiersTLE->currentRow() >= 0);
}

void GestionnaireTLE::on_MajAutoGroupe_toggled(bool checked)
{
    if (init) {
        QDir di(dirOut);
        if (!di.exists())
            di.mkpath(dirOut);

        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);

        QFile sw(dirOut + QDir::separator() + "tmp.txt");
        sw.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux2(&sw);

        const QString groupe = ui->listeGroupeTLE->currentItem()->text();
        const QString adresse = groupe.toLower();
        while (!flux.atEnd()) {
            QString ligne = flux.readLine();
            if (ligne.mid(0, adresse.length()) == adresse)
                ligne = ligne.replace(ligne.mid(ligne.lastIndexOf("#") - 1, 1), (ui->MajAutoGroupe->isChecked()) ? "1" : "0");
            flux2 << ligne << endl;
        }
        sw.close();
        sr.close();

        sr.remove();
        sw.rename(sr.fileName());
    }
}

void GestionnaireTLE::on_MajMaintenant_clicked()
{
    //...
}
