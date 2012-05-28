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
#include <QMessageBox>
#include <QQueue>
#include <QTextStream>
#include <QTimer>
#include "telecharger.h"
#include "librairies/exceptions/messages.h"
#include "librairies/exceptions/previsatexception.h"
#include "ui_telecharger.h"

static int dirHttp;
static QString fic;
static QString dirCoo;
static QString dirMap;
static QString dirTmp;
static const QString httpDirList1 = "http://astropedia.free.fr/previsat/commun/data/coordonnees/";
static const QString httpDirList2 = "http://astropedia.free.fr/previsat/commun/data/map/";
static QFile ficDwn;
static QNetworkAccessManager mng;
static QQueue<QUrl> downQueue;
static QNetworkReply *rep;

Telecharger::Telecharger(const int idirHttp, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Telecharger)
{
    ui->setupUi(this);

    dirHttp = idirHttp;
    const QString dirExe = QCoreApplication::applicationDirPath();

#if defined (Q_OS_WIN)
    const QString dirDat = dirExe + QDir::separator() + "data";
#else
    const QString dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

    dirCoo = dirDat + QDir::separator() + "coordonnees";
    dirMap = dirDat + QDir::separator() + "map";
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);

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
    fic = dirTmp + QDir::separator() + "listeMap.tmp";
    const QNetworkRequest requete(url);
    rep = mng.get(requete);

    connect(rep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(MessageErreur(QNetworkReply::NetworkError)));
    connect(rep, SIGNAL(finished()), this, SLOT(Enregistrer()));

    /* Retour */
    return;
}

void Telecharger::MessageErreur(QNetworkReply::NetworkError) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */

    /* Retour */
    throw PreviSatException(tr("Erreur lors du téléchargement du fichier :") + "\n" + rep->errorString(), Messages::WARNING);
}

void Telecharger::Enregistrer() const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(fic);
    fi.open(QIODevice::WriteOnly | QIODevice::Text);
    fi.write(rep->readAll());
    fi.close();
    rep->deleteLater();

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

    /* Corps de la methode */
    for(int i=0; i<ui->listeLieuxObs->count(); i++) {

        if (ui->listeLieuxObs->item(i)->checkState() == Qt::Checked) {

            QString httpDirList, dest;

            fic = ui->listeLieuxObs->item(i)->text();
            if (dirHttp == 1) {
                httpDirList = httpDirList1;
                dest = dirCoo + QDir::separator();
                fic.toLower();
            } else {
                httpDirList = httpDirList2;
                dest = dirMap + QDir::separator();
            }

            const QUrl url(httpDirList + fic);
            fic = fic.toLower().insert(0, dest);

            AjoutFichier(url);
        }
        if (downQueue.isEmpty())
            QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
    }

    /* Retour */
    return;
}

void Telecharger::AjoutFichier(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (downQueue.isEmpty())
        QTimer::singleShot(0, this, SLOT(TelechargementSuivant()));
    downQueue.enqueue(url);

    /* Retour */
    return;
}

void Telecharger::TelechargementSuivant()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (downQueue.isEmpty()) {
        emit TelechargementFini();
        ui->barreProgression->setVisible(false);

        QMessageBox::information(0, tr("Information"), tr("Veuillez redémarrer PreviSat pour prendre en compte la mise à jour"));
    } else {

        ui->barreProgression->setVisible(true);
        ui->barreProgression->setValue(0);
        QUrl url = downQueue.dequeue();
        const QString dest = (dirHttp == 1) ? dirCoo + QDir::separator() : dirMap + QDir::separator();
        ficDwn.setFileName(dest + QDir::separator() + QFileInfo(url.path()).fileName());

        if (ficDwn.open(QIODevice::WriteOnly)) {

            QNetworkRequest requete(url);
            rep = mng.get(requete);
            connect(rep, SIGNAL(downloadProgress(qint64,qint64)), SLOT(ProgressionTelechargement(qint64,qint64)));
            connect(rep, SIGNAL(finished()), SLOT(FinEnregistrementFichier()));
            connect(rep, SIGNAL(readyRead()), SLOT(EcritureFichier()));
        }
    }

    /* Retour */
    return;
}

void Telecharger::FinEnregistrementFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */
    ficDwn.close();

    /* Corps de la methode */

    rep->deleteLater();
    TelechargementSuivant();

    /* Retour */
    return;
}

void Telecharger::EcritureFichier()
{
    ficDwn.write(rep->readAll());
}
