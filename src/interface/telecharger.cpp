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
 * >    telecharger.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre de telechargement des categories de lieux d'observation, de cartes du monde et de fichiers sons
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 mars 2012
 *
 * Date de revision
 * >    8 decembre 2019
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#include <QMessageBox>
#include <QSettings>
#pragma GCC diagnostic warning "-Wconversion"
#include "onglets.h"
#include "telecharger.h"
#include "librairies/exceptions/message.h"
#include "librairies/exceptions/previsatexception.h"
#include "ui_telecharger.h"


static QSettings settings("Astropedia", "previsat");


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Telecharger::Telecharger(const AdressesTelechargement &adresse, Onglets *onglets) :
    ui(new Ui::Telecharger)
{
    ui->setupUi(this);

    _adresse = adresse;
    _onglets = onglets;
    setFont(Configuration::instance()->police());

    ui->listeFichiers->clear();
    ui->barreProgression->setVisible(false);
    ui->telecharger->setVisible(false);
    ui->interrogerServeur->setDefault(true);

    connect(_onglets, SIGNAL(TelechargementFini()), this, SLOT(FinTelechargement()));

    if (settings.value("affichage/flagIntensiteVision", false).toBool()) {

        QPalette paletteWin, palList;
        const int red = settings.value("affichage/valIntensiteVision", 0).toInt();
        const QBrush alpha = QBrush(QColor::fromRgb(red, 0, 0, 255));
        const QColor coulList = QColor(red + 27, 0, 0);

        paletteWin.setBrush(this->backgroundRole(), alpha);
        palList.setColor(QPalette::Base, coulList);

        this->setPalette(paletteWin);
        ui->listeFichiers->setPalette(palList);
    }
}

Telecharger::~Telecharger()
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
void Telecharger::on_fermer_clicked()
{
    close();
}

void Telecharger::on_interrogerServeur_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->listeFichiers->clear();
    const QString httpDirList = Configuration::instance()->mapAdressesTelechargement()[_adresse];

    /* Corps de la methode */
    try {

        _typeTelechargement = TELECHARGEMENT_LISTE;
        const QUrl url(httpDirList + "liste");
        _onglets->setDirDwn(Configuration::instance()->dirTmp());
        _onglets->AjoutFichier(url);

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void Telecharger::FinTelechargement()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_typeTelechargement == TELECHARGEMENT_LISTE) {

        QFile fi(Configuration::instance()->dirTmp() + QDir::separator() + "liste");

        if (fi.exists() && (fi.size() != 0)) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fi);

            QString ligne;
            while (!flux.atEnd()) {

                ligne = flux.readLine();
                ligne[0] = ligne[0].toUpper();

                QListWidgetItem * const elem1 = new QListWidgetItem(ligne, ui->listeFichiers);
                elem1->setCheckState(Qt::Unchecked);
            }
            fi.close();

            ui->telecharger->setVisible(true);
            ui->telecharger->setDefault(true);
            ui->interrogerServeur->setDefault(false);
        }
    } else if (_typeTelechargement == TELECHARGEMENT_FICHIERS) {
        const QString msg = tr("Veuillez redémarrer %1 pour prendre en compte la mise à jour");
        QMessageBox::information(0, tr("Information"), msg.arg(QCoreApplication::applicationName()));
        close();
    }

    /* Retour */
    return;
}

void Telecharger::on_telecharger_clicked()
{
    /* Declarations des variables locales */
    QString fic;
    QStringList listeFic;

    /* Initialisations */
    const QString httpDirList = Configuration::instance()->mapAdressesTelechargement()[_adresse];

    /* Corps de la methode */
    for(int i=0; i<ui->listeFichiers->count(); i++) {

        if (ui->listeFichiers->item(i)->checkState() == Qt::Checked) {

            fic = ui->listeFichiers->item(i)->text();
            if (_adresse != COORDONNEES) {
                fic = fic.toLower();
            }

            if (_adresse == NOTIFICATIONS) {

                listeFic.append(httpDirList + "aos-" + fic + ".wav");
                listeFic.append(httpDirList + "los-" + fic + ".wav");

            } else {
                listeFic.append(httpDirList + fic);
            }
        }
    }

    if (!listeFic.isEmpty()) {
        _typeTelechargement = TELECHARGEMENT_FICHIERS;

        QString repDest;
        switch (_adresse) {

        case COORDONNEES:
            repDest = Configuration::instance()->dirCoord();
            break;

        case CARTES:
            repDest = Configuration::instance()->dirMap();
            break;

        case NOTIFICATIONS:
            repDest = Configuration::instance()->dirSon();
            break;

        default:
            break;
        }
        _onglets->setDirDwn(repDest);
        _onglets->AjoutListeFichiers(listeFic);
    }

    /* Retour */
    return;
}
