/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    10 juin 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"*
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStyle>
#include <QTcpSocket>
#include "ui_informations.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "informations.h"
#include "configuration/configuration.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"


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
Informations::Informations(QWidget *fenetreParent) :
    _ui(new Ui::Informations)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _ui->setupUi(this);

    try {

        setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }

    /* Retour */
    return;
}


/*
 * Destructeur
 */
Informations::~Informations()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Informations *Informations::ui() const
{
    return _ui;
}


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

void Informations::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
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
 * Initialisation de la classe Informations
 */
void Informations::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->ongletsInfos->setCurrentIndex(0);
    _ui->afficherInfosDemarrage->setChecked(settings.value("affichage/informationsDemarrage", true).toBool());
    _ui->ok->setFocus();

    // Derniere information
    const QString nomDerniereInfo = "last_news_" + Configuration::instance()->locale() + ".html";
    OuvertureInfo(nomDerniereInfo, _ui->ongletDerniereInfo, _ui->derniereInfo);

    // Anciennes informations
    const QString nomInfosAnciennes = "histo_news_" + Configuration::instance()->locale() + ".html";
    OuvertureInfo(nomInfosAnciennes, _ui->ongletAnciennesInfos, _ui->anciennesInfos);

    // Versions
    const QString nomVersions = "histo_versions_" + Configuration::instance()->locale() + ".html";
    OuvertureInfo(nomVersions, _ui->ongletVersions, _ui->versions);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Ouverture du fichier d'informations
 */
void Informations::OuvertureInfo(const QString &nomfic, QWidget *onglet, QTextBrowser *zoneTexte)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        const QString url = QString(DOMAIN_NAME) + "informations/" + nomfic;

        if (UrlExiste(url)) {

            Telechargement telechargement(Configuration::instance()->dirTmp());
            telechargement.TelechargementFichier(url);

            QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + nomfic);
            if (fi.exists() && (fi.size() > 0)) {

                if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    zoneTexte->setHtml(fi.readAll());
                }
                fi.close();
            }

        } else {
            if (onglet == _ui->ongletDerniereInfo) {
                deleteLater();
            } else {
                _ui->ongletsInfos->removeTab(_ui->ongletsInfos->indexOf(onglet));
            }
        }
    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void Informations::on_ok_clicked()
{
    settings.setValue("affichage/informationsDemarrage", _ui->afficherInfosDemarrage->isChecked());
    close();
}
