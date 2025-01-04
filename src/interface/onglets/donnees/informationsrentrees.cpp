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
 * >    informationsrentrees.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2024
 *
 * Date de revision
 * >    4 janvier 2025
 *
 */

#include "configuration/configuration.h"
#include "configuration/rentrees.h"
#include "donneessatellite.h"
#include "informationsrentrees.h"
#include "ui_informationsrentrees.h"
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
InformationsRentrees::InformationsRentrees(QWidget *parent)
    : QFrame(parent)
    , _ui(new Ui::InformationsRentrees)
{
    _ui->setupUi(this);

    _ui->rentrees->setToolTip(tr("Double-cliquez sur une ligne pour afficher les informations satellite"));

    _donneesSatellite = nullptr;
}


/*
 * Destructeur
 */
InformationsRentrees::~InformationsRentrees()
{
    EFFACE_OBJET(_donneesSatellite);

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
void InformationsRentrees::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void InformationsRentrees::show()
{
    /* Declarations des variables locales */
    QTableWidgetItem * itemNom;
    QTableWidgetItem * itemDate;
    QTableWidgetItem * itemNorad;
    QTableWidgetItem * itemCospar;
    QTableWidgetItem * itemTaille;
    QTableWidgetItem * itemPays;
    QTableWidgetItem * itemTypeMessage;

    /* Initialisations */
    _ui->rentrees->setStyleSheet("QHeaderView::section {" \
                                 "background-color:rgb(235, 235, 235);" \
                                 "border-top: 0px solid grey;" \
                                 "border-bottom: 1px solid grey;" \
                                 "border-right: 1px solid grey;" \
                                 "font-size: 12px;" \
                                 "font-weight: 600 }");

    _ui->rentrees->horizontalHeader()->setStretchLastSection(true);
    _ui->rentrees->model()->removeRows(0, _ui->rentrees->rowCount());

    int j = 0;
    const QList<RentreesAtmospheriques> &rentrees = Configuration::instance()->rentreesAtmospheriques();
    const QColor fond = (settings.value("affichage/modeSombre").toBool()) ? QColor::fromRgb(66, 66, 66) : QColor::fromRgb(240, 240, 240);

    /* Corps de la methode */
    QListIterator it(rentrees);
    while (it.hasNext()) {

        const RentreesAtmospheriques rentree = it.next();

        // Ajout d'une ligne dans le tableau
        _ui->rentrees->insertRow(j);
        _ui->rentrees->setRowHeight(j, 16);

        const QBrush couleur((rentree.dateRentree < QDateTime::currentDateTimeUtc()) ? fond : Qt::transparent);

        // Nom
        itemNom = new QTableWidgetItem(rentree.nom);
        itemNom->setTextAlignment(Qt::AlignCenter);
        itemNom->setFlags(itemNom->flags() & ~Qt::ItemIsEditable);
        itemNom->setBackground(couleur);
        _ui->rentrees->setItem(j, 0, itemNom);

        // Norad
        itemNorad = new QTableWidgetItem(rentree.norad);
        itemNorad->setTextAlignment(Qt::AlignCenter);
        itemNorad->setFlags(itemNorad->flags() & ~Qt::ItemIsEditable);
        itemNorad->setBackground(couleur);
        _ui->rentrees->setItem(j, 1, itemNorad);

        // COSPAR
        itemCospar = new QTableWidgetItem(rentree.cospar);
        itemCospar->setTextAlignment(Qt::AlignCenter);
        itemCospar->setFlags(itemCospar->flags() & ~Qt::ItemIsEditable);
        itemCospar->setBackground(couleur);
        _ui->rentrees->setItem(j, 2, itemCospar);

        // Date
        itemDate = new QTableWidgetItem(rentree.dateRentree.toString("yyyy-MM-dd HH:mm:ss"));
        itemDate->setTextAlignment(Qt::AlignCenter);
        itemDate->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
        itemDate->setBackground(couleur);
        _ui->rentrees->setItem(j, 3, itemDate);

        // Taille
        itemTaille = new QTableWidgetItem(rentree.taille);
        itemTaille->setTextAlignment(Qt::AlignCenter);
        itemTaille->setFlags(itemTaille->flags() & ~Qt::ItemIsEditable);
        itemTaille->setBackground(couleur);
        _ui->rentrees->setItem(j, 4, itemTaille);

        // Pays
        itemPays = new QTableWidgetItem(rentree.pays);
        itemPays->setTextAlignment(Qt::AlignCenter);
        itemPays->setFlags(itemPays->flags() & ~Qt::ItemIsEditable);
        itemPays->setBackground(couleur);
        _ui->rentrees->setItem(j, 5, itemPays);

        // Type de message
        itemTypeMessage = new QTableWidgetItem(rentree.typeMessage);
        itemTypeMessage->setTextAlignment(Qt::AlignCenter);
        itemTypeMessage->setFlags(itemTypeMessage->flags() & ~Qt::ItemIsEditable);
        itemTypeMessage->setBackground(couleur);
        _ui->rentrees->setItem(j, 6, itemTypeMessage);

        j++;
    }

    _ui->rentrees->resizeColumnToContents(0);
    _ui->rentrees->resizeColumnToContents(3);

    /* Retour */
    return;
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
void InformationsRentrees::on_majRentrees_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    Telechargement tel(Configuration::instance()->dirLocalData());

    /* Corps de la methode */
    try {

        emit AfficherMessageStatut(tr("Téléchargement du fichier de rentrées atmosphériques..."));
        tel.TelechargementFichier(QUrl(QString("%1data/rentrees.xml").arg(DOMAIN_NAME)));
        emit AfficherMessageStatut(tr("Téléchargement terminé"), 5);

        Configuration::instance()->rentreesAtmospheriques() = Rentrees::LectureFichierRentrees();
        show();

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

void InformationsRentrees::on_rentrees_itemDoubleClicked(QTableWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QEvent evt(QEvent::LanguageChange);
    const QString norad = _ui->rentrees->item(item->row(), 1)->text();

    /* Corps de la methode */
    const QList<Donnees> donnees = Donnees::RequeteNorad(Configuration::instance()->dbSatellites(), norad);

    EFFACE_OBJET(_donneesSatellite);
    _donneesSatellite = new DonneesSatellite;
    _donneesSatellite->changeEvent(&evt);
    _donneesSatellite->setWindowModality(Qt::ApplicationModal);
    _donneesSatellite->show(donnees.first());
    _donneesSatellite->setVisible(true);

    /* Retour */
    return;
}
