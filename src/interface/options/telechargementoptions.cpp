/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    telechargementoptions.cpp
 *
 * Localisation
 * >    interface.options
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 avril
 *
 * Date de revision
 * >
 *
 */

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include "ui_telechargementoptions.h"
#include "telechargementoptions.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
TelechargementOptions::TelechargementOptions(const TypeTelechargement &type, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::TelechargementOptions),
    _type(type)
{
    _ui->setupUi(this);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), parent->geometry()));

    try {

        Initialisation();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
}


/*
 * Destructeur
 */
TelechargementOptions::~TelechargementOptions()
{
    delete _ui;
}


/*
 * Accesseurs
 */

/*
 * Modificateurs
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
/*
 * Initialisation de la fenetre TelechargementOptions
 */
void TelechargementOptions::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    switch (_type) {
    case TypeTelechargement::COORDONNEES:
        setWindowTitle(tr("Télécharger des lieux d'observation..."));
    break;

    case TypeTelechargement::CARTES:
        setWindowTitle(tr("Télécharger des cartes du monde..."));
    break;

    case TypeTelechargement::NOTIFICATIONS:
        setWindowTitle(tr("Télécharger des fichiers de notification sonore..."));
        break;

    default:
        break;
    }

    _ui->telecharger->setVisible(false);
    _ui->barreProgression->setVisible(false);
    TelechargerListe();

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Telechargement de la liste de fichiers
 */
void TelechargementOptions::TelechargerListe()
{
    /* Declarations des variables locales */

    /* Initialisations */
    Telechargement tel(Configuration::instance()->dirTmp());
    const QUrl url(Configuration::instance()->mapAdressesTelechargement()[_type] + "liste");
    const QString fichier = Configuration::instance()->dirTmp() + QDir::separator() + "liste";

    /* Corps de la methode */
    tel.TelechargementFichier(url);

    QFile fi(fichier);
    if (fi.exists() && (fi.size() != 0) && fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QListWidgetItem *elem;
        const QStringList contenuFichier = QString(fi.readAll()).split('\n', Qt::SkipEmptyParts);

        _ui->listeFichiers->clear();
        _ui->listeFichiers->scrollToTop();

        QStringListIterator it(contenuFichier);
        while (it.hasNext()) {

            const QString fic = it.next();

            elem = new QListWidgetItem(fic, _ui->listeFichiers);
            elem->setData(Qt::CheckStateRole, Qt::Unchecked);
        }

        _ui->listeFichiers->sortItems();
    }

    /* Retour */
    return;
}

void TelechargementOptions::on_fermer_clicked()
{
    close();
}

void TelechargementOptions::on_filtreFichiers_textChanged(const QString &arg1)
{
    for(int i=0; i<_ui->listeFichiers->count(); i++) {
        const QString elem = _ui->listeFichiers->item(i)->text();
        _ui->listeFichiers->item(i)->setHidden(!elem.contains(arg1, Qt::CaseInsensitive));
    }
}

void TelechargementOptions::on_listeFichiers_itemClicked(QListWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(item)
    bool atrouve = false;

    /* Corps de la methode */
    for(int i=0; i<_ui->listeFichiers->count() && !atrouve; i++) {
        if (_ui->listeFichiers->item(i)->checkState() == Qt::Checked) {
            atrouve = true;
        }
    }

    _ui->telecharger->setVisible(atrouve);
    _ui->telecharger->setDefault(atrouve);
    _ui->fermer->setDefault(!atrouve);

    /* Retour */
    return;
}


void TelechargementOptions::on_telecharger_clicked()
{
    /* Declarations des variables locales */
    QStringList liste;

    /* Initialisations */

    /* Corps de la methode */
    for(int i=0; i<_ui->listeFichiers->count(); i++) {
        if (_ui->listeFichiers->item(i)->checkState() == Qt::Checked) {
            liste.append(_ui->listeFichiers->item(i)->text());
        }
    }

    const QString dir = Configuration::instance()->mapAdressesTelechargement()[_type].split("/", Qt::SkipEmptyParts).last();
    const QString dirHttp = Configuration::instance()->mapAdressesTelechargement()[_type];
    Telechargement tel(Configuration::instance()->dirLocalData() + QDir::separator() + dir);

    QStringListIterator it(liste);
    while (it.hasNext()) {

        const QString fic = it.next();

        switch (_type) {

        case TypeTelechargement::COORDONNEES:

            tel.TelechargementFichier(dirHttp + fic + ".xml");
            break;

        case TypeTelechargement::NOTIFICATIONS:

            tel.TelechargementFichier(dirHttp + "aos-" + fic + ".wav");
            tel.TelechargementFichier(dirHttp + "los-" + fic + ".wav");
            break;

        case TypeTelechargement::CARTES:

            tel.TelechargementFichier(dirHttp + fic);
            break;

        default:
            break;
        }
    }

    QMessageBox::information(0, tr("Information"), tr("Veuillez redémarrer %1 pour prendre en compte la mise à jour").arg(APP_NAME));
    close();

    /* Retour */
    return;
}

