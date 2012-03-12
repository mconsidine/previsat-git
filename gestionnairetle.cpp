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
#include "globals.h"
#include "librairies/corps/satellite/satellite.h"
#include "ui_gestionnairetle.h"

static bool init;
static int selec;
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
    /* Declarations des variables locales */

    /* Initialisations */
    selec = -1;
    ui->barreMenu->setVisible(false);
    ui->frame->setVisible(false);
    ui->fichierTelechargement->setText("");
    ui->barreProgression->setValue(0);
    ui->listeGroupeTLE->clear();
    ui->listeFichiersTLE->clear();
    ficTLE = dirDat + QDir::separator() + "gestionnaireTLE.gst";

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
    close();
}

void GestionnaireTLE::on_actionCreer_un_groupe_activated()
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

void GestionnaireTLE::on_actionSupprimerGroupe_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QDir di(dirTmp);
    if (!di.exists())
        di.mkpath(dirTmp);

    /* Corps de la methode */
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer le groupe ""%1""?");
    const int res = QMessageBox::question(this, tr("Information"), msg.arg(groupe), QMessageBox::Yes | QMessageBox::No);

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

void GestionnaireTLE::on_listeGroupeTLE_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->listeGroupeTLE->setCurrentRow(ui->listeGroupeTLE->indexAt(pos).row());
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
    ui->listeFichiersTLE->setCurrentRow(-1);

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
                    ui->MajAutoGroupe->setChecked(ligne.mid(ligne.lastIndexOf("#") - 1, 1) == "1");
                }
            }
            sr.close();
            init = true;
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
        if (ui->domaine->text().trimmed().isEmpty())
            throw PreviSatException(tr("Le nom du domaine n'est pas spécifié"), Messages::WARNING);

        if (ui->nomGroupe->text().trimmed().isEmpty())
            throw PreviSatException(tr("Le nom du groupe n'est pas spécifié"), Messages::WARNING);

        const QString ligne = ui->nomGroupe->text().trimmed() + "@" + ui->domaine->text().trimmed();
        if (ui->nomGroupe->isEnabled()) {
            for(int i=0; i<ui->listeGroupeTLE->count(); i++) {
                if (ligne.toLower() == ui->listeGroupeTLE->item(i)->text().toLower()) {
                    const QString msg = tr("Le groupe ""%1"" existe déjà");
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

void GestionnaireTLE::on_actionAjouter_des_fichiers_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
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

    /* Retour */
    return;
}

void GestionnaireTLE::on_actionSupprimer_activated()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    const QString msg = tr("Voulez-vous vraiment supprimer ce(s) fichier(s) du groupe ""%1""?");
    const int res = QMessageBox::question(this, tr("Information"), msg.arg(groupe), QMessageBox::Yes | QMessageBox::No);

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

    /* Retour */
    return;
}

void GestionnaireTLE::on_listeFichiersTLE_customContextMenuRequested(const QPoint &pos)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->groupe->setVisible(false);
    ui->actionSupprimer->setVisible(ui->listeFichiersTLE->currentRow() >= 0);

    /* Retour */
    return;
}

void GestionnaireTLE::on_MajAutoGroupe_toggled(bool checked)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (init) {
        QDir di(dirTmp);
        if (!di.exists())
            di.mkpath(dirTmp);

        QFile sr(ficTLE);
        sr.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&sr);

        QFile sw(dirTmp + QDir::separator() + "tmp.txt");
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

    /* Retour */
    return;
}

void GestionnaireTLE::on_MajMaintenant_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString groupe = ui->listeGroupeTLE->currentItem()->text();
    QString adresse = groupe.toLower();

    /* Corps de la methode */
    ui->MajMaintenant->setEnabled(false);
    QFile sr(ficTLE);
    sr.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&sr);

    try {
        while (!flux.atEnd()) {
            const QString ligne = flux.readLine();
            const QStringList nomGroupe = ligne.split("#");

            QStringList listeTLE;
            if (nomGroupe.at(0) == adresse)
                nomGroupe.at(2).split(",");
            if (adresse.contains("celestrak"))
                adresse = "http://www.celestrak.com/NORAD/elements/";
            if (!adresse.startsWith("http://"))
                adresse.insert(0, "http://");
            if (!adresse.endsWith("/"))
                adresse.append("/");

            ui->frame->setVisible(true);
            for(int i=0; listeTLE.count(); i++) {

                const QUrl url(adresse + listeTLE.at(i));
                const QString fic = dirTle + QDir::separator() + listeTLE.at(i);
                const QNetworkRequest requete(url);
                QNetworkAccessManager *mng = new QNetworkAccessManager;
                QNetworkReply *rep = mng->get(requete);
                ui->fichierTelechargement->setText(listeTLE.at(i));
                ui->fichierTelechargement->setGeometry(83 - ui->fichierTelechargement->width() * 0.5,
                                                       ui->fichierTelechargement->y(), ui->fichierTelechargement->width(),
                                                       ui->fichierTelechargement->height());

                connect(rep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(MessageErreur(QNetworkReply::NetworkError)));
                connect(rep, SIGNAL(finished()), this, SLOT(Enregistrer(fic)));
                connect(rep, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(ProgressionTelechargement(qint64, qint64)));

                if (fic == nomfic) {

                    // Recuperation des TLE de la liste
                    TLE::LectureFichier(nomfic, liste, tles);
                }
            }
            ui->frame->setVisible(false);
        }
        Messages::Afficher(tr("Mise à jour de la catégorie effectuée"), Messages::INFO);

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void GestionnaireTLE::MessageErreur(QNetworkReply::NetworkError) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QNetworkReply *rep = qobject_cast<QNetworkReply*>(sender());

    /* Retour */
    throw PreviSatException(tr("Erreur lors du téléchargement du fichier :") + "\n" + rep->errorString(), Messages::WARNING);
}

void GestionnaireTLE::Enregistrer(const QString fic) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QNetworkReply *rep = qobject_cast<QNetworkReply*>(sender());
    QFile fi(fic);
    fi.open(QIODevice::WriteOnly | QIODevice::Text);
    fi.write(rep->readAll());
    fi.close();
    rep->deleteLater();

    /* Retour */
    return;
}

void GestionnaireTLE::ProgressionTelechargement(qint64 recu, qint64 total) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (total != -1) {
       ui->barreProgression->setRange(0, total);
       ui->barreProgression->setValue(recu);
    }

    /* Retour */
    return;
}
