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
 * >    26 juillet 2015
 *
 */

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#include <QQueue>
#include <QTextStream>
#include "telecharger.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "ui_telecharger.h"

static int dirHttp;
static QString fic;
static QString dirCoo;
static QString dirMap;
static QString dirTmp;
static QFile ficDwn;
static QNetworkAccessManager mng;
static QQueue<QUrl> downQueue;
static QNetworkReply *rep;

static QSettings settings("Astropedia", "previsat");
static const QString httpDirList1 = settings.value("fichier/dirHttpList1").toString();
static const QString httpDirList2 = settings.value("fichier/dirHttpList2").toString();

Telecharger::Telecharger(const int idirHttp, QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::Telecharger)
{
    ui->setupUi(this);

    dirHttp = idirHttp;

    QFont police;

#if defined (Q_OS_WIN)
    const QString dirLocalData = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(),
                                                        QStandardPaths::LocateDirectory) + "data";

    police.setFamily("MS Shell Dlg 2");
    police.setPointSize(8);

#elif defined (Q_OS_LINUX)
    const QString dirLocalData = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(),
                                                        QStandardPaths::LocateDirectory) + "data";

    police.setFamily("Sans Serif");
    police.setPointSize(7);

#elif defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";

    police.setFamily("Marion");
    police.setPointSize(11);
#else
    const QString dirLocalData = QStandardPaths::locate(QStandardPaths::AppLocalDataLocation, QString(),
                                                        QStandardPaths::LocateDirectory) + "data";
#endif

    setFont(police);

    dirCoo = dirLocalData + QDir::separator() + "coordonnees";
    dirMap = dirLocalData + QDir::separator() + "map";
    dirTmp = QStandardPaths::locate(QStandardPaths::CacheLocation, QString(), QStandardPaths::LocateDirectory);

    ui->listeLieuxObs->clear();
    ui->barreProgression->setVisible(false);
    ui->telecharger->setVisible(false);

    if (settings.value("affichage/flagIntensiteVision", false).toBool()) {

        QPalette paletteWin, palList;
        const int red = settings.value("affichage/valIntensiteVision", 0).toInt();
        const QBrush alpha = QBrush(QColor::fromRgb(red, 0, 0, 255));
        const QColor coulList = QColor(red + 27, 0, 0);

        paletteWin.setBrush(this->backgroundRole(), alpha);
        palList.setColor(QPalette::Base, coulList);

        this->setPalette(paletteWin);
        ui->listeLieuxObs->setPalette(palList);
    }
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
    try {
        const QUrl url(httpDirList + "liste");
        fic = dirTmp + QDir::separator() + "listeMap.tmp";
        const QNetworkRequest requete(url);
        rep = mng.get(requete);

        connect(rep, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(MessageErreur(QNetworkReply::NetworkError)));
        connect(rep, SIGNAL(finished()), this, SLOT(Enregistrer()));
    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Telecharger::MessageErreur(QNetworkReply::NetworkError) const
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    const QString msg = tr("Erreur lors du téléchargement du fichier :\n%1");
    Message::Afficher(msg.arg(rep->errorString()), WARNING);

    /* Retour */
    return;
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

        QListWidgetItem * const elem1 = new QListWidgetItem(ligne, ui->listeLieuxObs);
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
        ui->barreProgression->setRange(0, (int) total);
        ui->barreProgression->setValue((int) recu);
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
            } else {
                httpDirList = httpDirList2;
                dest = dirMap + QDir::separator();
                fic = fic.toLower();
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

        const QString msg = tr("Veuillez redémarrer %1 pour prendre en compte la mise à jour");
        QMessageBox::information(0, tr("Information"), msg.arg(QCoreApplication::applicationName()));
    } else {

        ui->barreProgression->setVisible(true);
        ui->barreProgression->setValue(0);
        QUrl url = downQueue.dequeue();
        const QString dest = (dirHttp == 1) ? dirCoo + QDir::separator() : dirMap + QDir::separator();
        ficDwn.setFileName(dest + QDir::separator() + QFileInfo(url.path()).fileName());

        if (ficDwn.open(QIODevice::WriteOnly)) {

            const QNetworkRequest requete(url);
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
