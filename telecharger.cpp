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
 * >    telecharger.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre de telechargement des categories de lieux d'observation et de cartes du monde
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 mars 2012
 *
 * Date de revision
 * >
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "telecharger.h"
#include "librairies/exceptions/messages.h"
#include "librairies/exceptions/previsatexception.h"
#include "ui_telecharger.h"

static int dirHttp;
static QString dirCoo;
static QString dirMap;
static QString dirTmp;
static const QString httpDirList1 = "http://astropedia.free.fr/previsat/data/coordonnees/";
static const QString httpDirList2 = "http://astropedia.free.fr/previsat/data/map/";

Telecharger::Telecharger(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Telecharger)
{
    ui->setupUi(this);
}

Telecharger::Telecharger(const int idirHttp)
{
    dirHttp = idirHttp;
    QCoreApplication::setApplicationName("PreviSat");
    QCoreApplication::setOrganizationName("Astropedia");
    const QString dirExe = QCoreApplication::applicationDirPath();
    const QString dirDat = dirExe + QDir::separator() + "data";
    dirCoo = dirDat + QDir::separator() + "coordonnees";
    dirMap = dirDat + QDir::separator() + "map";
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    ui->listeLieuxObs->clear();
    ui->barreProgression->setVisible(false);
    ui->telecharger->setVisible(false);
}

Telecharger::~Telecharger()
{
    delete ui;
}

void Telecharger::on_fermer_clicked()
{
    close();
}

void Telecharger::on_interrogerServeur_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->listeLieuxObs->clear();
    const QString httpDirList = (dirHttp == 1) ? httpDirList1 : httpDirList2;

    /* Corps de la methode */
    const QUrl url(httpDirList + "liste");
    const QString fic = dirTmp + QDir::separator() + "listeMap.tmp";
    const QNetworkRequest requete(url);
    QNetworkAccessManager *mng = new QNetworkAccessManager;
    QNetworkReply *rep = mng->get(requete);

    connect(rep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(MessageErreur(QNetworkReply::NetworkError)));
    connect(rep, SIGNAL(finished()), this, SLOT(Enregistrer(fic)));

    QFile fi(fic);
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fi);

    while (!flux.atEnd()) {
        QString ligne = flux.readLine();
        ligne[0] = ligne[0].toUpper();

        QListWidgetItem *elem1 = new QListWidgetItem(ligne, ui->listeLieuxObs);
        elem1->setCheckState(Qt::Unchecked);
    }

    ui->telecharger->setVisible(true);

    /* Retour */
    return;
}

void Telecharger::MessageErreur(QNetworkReply::NetworkError) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QNetworkReply *rep = qobject_cast<QNetworkReply*>(sender());

    /* Retour */
    throw PreviSatException(tr("Erreur lors du téléchargement du fichier :") + "\n" + rep->errorString(), Messages::WARNING);
}

void Telecharger::Enregistrer(const QString fic) const
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

void Telecharger::ProgressionTelechargement(qint64 recu, qint64 total) const
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

void Telecharger::on_telecharger_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString httpDirList = (dirHttp == 1) ? httpDirList1 : httpDirList2;
    const QString dest = (dirHttp == 1) ? dirCoo + QDir::separator() : dirMap + QDir::separator();

    /* Corps de la methode */
    ui->barreProgression->setVisible(true);
    for(int i=0; i<ui->listeLieuxObs->count(); i++) {

        if (ui->listeLieuxObs->item(i)->checkState() == Qt::Checked) {

            QString fic = ui->listeLieuxObs->item(i)->text().toLower();
            const QUrl url(httpDirList + fic);
            fic.insert(0, dest);

            const QNetworkRequest requete(url);
            QNetworkAccessManager *mng = new QNetworkAccessManager;
            QNetworkReply *rep = mng->get(requete);

            connect(rep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(MessageErreur(QNetworkReply::NetworkError)));
            connect(rep, SIGNAL(finished()), this, SLOT(Enregistrer(fic)));
            connect(rep, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(ProgressionTelechargement(qint64, qint64)));
        }
    }
    ui->barreProgression->setVisible(false);

    /* Retour */
    return;
}
