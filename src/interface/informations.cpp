/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    18 aout 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <QSettings>
#include <QStyle>
#pragma GCC diagnostic ignored "-Wswitch-default"*
#include "ui_informations.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "informations.h"
#include "onglets.h"
#include "configuration/configuration.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


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
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));

    ui->ongletsInfos->setCurrentIndex(0);
    ui->afficherInfosDemarrage->setChecked(settings.value("affichage/informationsDemarrage", true).toBool());
    ui->ok->setFocus();
    _onglets = onglets;
    _onglets->setDirDwn(Configuration::instance()->dirTmp());

    // Derniere information
    const QString nomDerniereInfo = "last_news_" + Configuration::instance()->locale() + ".html";
    OuvertureInfo(nomDerniereInfo, ui->ongletDerniereInfo, ui->derniereInfo);

    // Anciennes informations
    const QString nomInfosAnciennes = "histo_news_" + Configuration::instance()->locale() + ".html";
    OuvertureInfo(nomInfosAnciennes, ui->ongletAnciennesInfos, ui->anciennesInfos);

    // Versions
    const QString nomVersions = "histo_versions_" + Configuration::instance()->locale() + ".html";
    OuvertureInfo(nomVersions, ui->ongletVersions, ui->versions);

    /* Retour */
    return;
}

/*
 * Destructeur
 */
Informations::~Informations()
{
    if (_onglets != nullptr) {
        delete _onglets;
        _onglets = nullptr;
    }

    delete ui;
}

/*
 * Accesseurs
 */


/*
 * Methodes publiques
 */
/*
 * Verification de l'existence d'une adresse
 */
bool Informations::UrlExiste(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
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

    /* Retour */
    return false;
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
 * Ouverture du fichier d'informations
 */
void Informations::OuvertureInfo(const QString &nomfic, QWidget *onglet, QTextBrowser *zoneTexte)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString url = QString("%1%2/Qt/informations/").arg(DOMAIN_NAME).arg(QString(APP_NAME).toLower()) + nomfic;

    /* Corps de la methode */
    if (UrlExiste(url)) {

        _onglets->TelechargementFichier(url, false);

        QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + nomfic);
        if (fi.exists() && (fi.size() > 0)) {

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                zoneTexte->setHtml(fi.readAll());
            }
            fi.close();
        }

    } else {
        if (onglet == ui->ongletDerniereInfo) {
            deleteLater();
        } else {
            ui->ongletsInfos->removeTab(ui->ongletsInfos->indexOf(onglet));
        }
    }

    /* Retour */
    return;
}

void Informations::on_ok_clicked()
{
    settings.setValue("affichage/informationsDemarrage", ui->afficherInfosDemarrage->isChecked());
    close();
}