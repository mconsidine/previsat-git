/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    24 octobre 2023
 *
 */

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStyle>
#include <QTcpSocket>
#include "ui_informations.h"
#include "informations.h"
#include "configuration/configuration.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"


// Registre
#if (PORTABLE_BUILD)
static QSettings settings(QString("%1.ini").arg(APP_NAME), QSettings::IniFormat);
#else
static QSettings settings(ORG_NAME, APP_NAME);
#endif


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

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
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
void Informations::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);

        Initialisation();
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

        if (Telechargement::UrlExiste(url)) {

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
    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

void Informations::on_ok_clicked()
{
    settings.setValue("affichage/informationsDemarrage", _ui->afficherInfosDemarrage->isChecked());
    close();
}
