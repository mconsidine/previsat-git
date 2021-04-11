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
#pragma GCC diagnostic ignored "-Wconversion"
#include <QSettings>
#include <QStyle>
#pragma GCC diagnostic warning "-Wconversion"
#include <QTcpSocket>
#include "informations.h"
#include "onglets.h"
#include "ui_informations.h"
#include "configuration/configuration.h"


// Registre
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
Informations::Informations(QWidget *fenetreParent, Onglets *onglets) :
    ui(new Ui::Informations)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));

    ui->ongletsInfos->setCurrentIndex(0);
    ui->afficherInfosDemarrage->setChecked(settings.value("affichage/informationsDemarrage", true).toBool());
    ui->ok->setFocus();
    _onglets = onglets;
    _onglets->setDirDwn(Configuration::instance()->dirTmp());

    // Derniere information
    const QString nomDerniereInfo = "last_news_" + Configuration::instance()->locale() + ".html";
    const QString urlLastNews = settings.value("fichier/dirHttpPrevi", "").toString() + "informations/" + nomDerniereInfo;

    if (UrlExiste(urlLastNews)) {

        _onglets->TelechargementFichier(urlLastNews, false);

        QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + nomDerniereInfo);
        if (fi.exists() && (fi.size() > 0)) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            ui->derniereInfo->setHtml(flux.readAll());
            fi.close();
        }
    } else {
        deleteLater();
    }

    // Anciennes informations
    const QString nomInfosAnciennes = "histo_news_" + Configuration::instance()->locale() + ".html";
    const QString urlHistoNews = settings.value("fichier/dirHttpPrevi", "").toString() + "informations/" + nomInfosAnciennes;

    if (UrlExiste(urlHistoNews)) {

        _onglets->TelechargementFichier(urlHistoNews, false);

        QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + nomInfosAnciennes);
        if (fi.exists() && (fi.size() > 0)) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);
            ui->anciennesInfos->setHtml(flux.readAll());
            fi.close();
        }
    } else {
        ui->ongletsInfos->removeTab(1);
    }
}

Informations::~Informations()
{
    delete ui;
}

/*
 * Accesseurs
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
