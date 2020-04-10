/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    informations.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre Informations
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    1er mai 2019
 *
 * Date de revision
 * >    10 avril 2020
 *
 */

#include <QDir>
#include <QSettings>
#include <QUrl>
#include "informations.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QTcpSocket>
#include "ui_informations.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"


// Registre
static QSettings settings("Astropedia", "previsat");

Informations::Informations(const QString &localePreviSat, QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::Informations)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));

    ui->ongletsInfos->setCurrentIndex(0);
    ui->afficherInfosDemarrage->setChecked(settings.value("affichage/informationsDemarrage", true).toBool());
    ui->ok->setFocus();

    // Derniere information
    const QUrl urlLastNews(settings.value("fichier/dirHttpPrevi", "").toString() + "informations/last_news_" + localePreviSat + ".html");
    if (UrlExiste(urlLastNews)) {
        ui->derniereInfo->load(urlLastNews);
    } else {
        deleteLater();
    }

    // Anciennes informations
    const QUrl urlHistoNews(settings.value("fichier/dirHttpPrevi", "").toString() + "informations/histo_news_" + localePreviSat + ".html");
    if (UrlExiste(urlHistoNews)) {
        ui->anciennesInfos->load(urlHistoNews);
    } else {
        ui->ongletsInfos->removeTab(1);
    }
}

Informations::~Informations()
{
    delete ui;
}

void Informations::on_ok_clicked()
{
    settings.setValue("affichage/informationsDemarrage", ui->afficherInfosDemarrage->isChecked());
    close();
}

bool Informations::UrlExiste(const QUrl &url)
{
    QTcpSocket socket;
    socket.connectToHost(url.host(), 80);
    if (socket.waitForConnected()) {
        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\n"
                                                     "Host: " + url.host().toUtf8() + "\r\n"
                                                                                      "\r\n");
        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();
            if (bytes.contains("200 OK")) {
                return true;
            }
        }
    }
    return false;
}
