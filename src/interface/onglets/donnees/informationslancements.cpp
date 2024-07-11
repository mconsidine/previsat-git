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
 * >    informationslancements.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#include "configuration/configuration.h"
#include "configuration/lancements.h"
#include "informationslancements.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"
#include "ui_informationslancements.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
InformationsLancements::InformationsLancements(QWidget *parent)
    : QFrame(parent)
    , _ui(new Ui::InformationsLancements)
{
    _ui->setupUi(this);

    _ui->lancements->setStyleSheet("QHeaderView::section {" \
                                   "background-color:rgb(235, 235, 235);" \
                                   "border-top: 0px solid grey;" \
                                   "border-bottom: 1px solid grey;" \
                                   "border-right: 1px solid grey;" \
                                   "font-size: 12px;" \
                                   "font-weight: 600 }");

    _ui->lancements->horizontalHeader()->setStretchLastSection(true);
}


/*
 * Destructeur
 */
InformationsLancements::~InformationsLancements()
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
void InformationsLancements::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void InformationsLancements::show()
{
    /* Declarations des variables locales */
    QTableWidgetItem * itemDate;
    QTableWidgetItem * itemHeure;
    QTableWidgetItem * itemLancement;
    QTableWidgetItem * itemSite;
    QTableWidgetItem * itemCommentaire;

    /* Initialisations */
    int j = 0;
    const QList<CalendrierLancements> &lancements = Configuration::instance()->calendrierLancements();
    _ui->lancements->model()->removeRows(0, _ui->lancements->rowCount());
    _ui->lancements->horizontalHeader()->setToolTip(tr("Survolez les éléments du tableau pour afficher plus d'informations"));

    /* Corps de la methode */
    QListIterator it(lancements);
    while (it.hasNext()) {

        const CalendrierLancements calendrier = it.next();

        // Ajout d'une ligne dans le tableau
        _ui->lancements->insertRow(j);
        _ui->lancements->setRowHeight(j, 16);

        // Date
        itemDate = new QTableWidgetItem(calendrier.date);
        itemDate->setTextAlignment(Qt::AlignCenter);
        itemDate->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
        _ui->lancements->setItem(j, 0, itemDate);

        // Heure
        const QString heureSimp = (calendrier.heure.contains("(")) ? calendrier.heure.split("(").at(0): calendrier.heure;
        itemHeure = new QTableWidgetItem(heureSimp);
        itemHeure->setToolTip(calendrier.heure);
        itemHeure->setTextAlignment(Qt::AlignCenter);
        itemHeure->setFlags(itemHeure->flags() & ~Qt::ItemIsEditable);
        _ui->lancements->setItem(j, 1, itemHeure);

        // Lancement
        itemLancement = new QTableWidgetItem(calendrier.lancement);
        itemLancement->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemLancement->setFlags(itemLancement->flags() & ~Qt::ItemIsEditable);
        _ui->lancements->setItem(j, 2, itemLancement);

        // Site
        const QString siteSimp = (calendrier.site.contains(",")) ? calendrier.site.split(",").at(0) : calendrier.site;
        itemSite = new QTableWidgetItem(siteSimp);
        itemSite->setToolTip(calendrier.site);
        itemSite->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemSite->setFlags(itemSite->flags() & ~Qt::ItemIsEditable);
        _ui->lancements->setItem(j, 3, itemSite);

        // Commentaire
        itemCommentaire = new QTableWidgetItem(calendrier.commentaire);
        itemCommentaire->setToolTip(calendrier.commentaire);
        itemCommentaire->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        itemCommentaire->setFlags(itemCommentaire->flags() & ~Qt::ItemIsEditable);
        _ui->lancements->setItem(j, 4, itemCommentaire);
        _ui->lancements->resizeColumnsToContents();

        j++;
    }

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
void InformationsLancements::on_majLancements_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    Telechargement tel(Configuration::instance()->dirLocalData());

    /* Corps de la methode */
    try {

        emit AfficherMessageStatut(tr("Téléchargement du fichier de lancements..."));
        tel.TelechargementFichier(QUrl(QString("%1data/lancements.txt").arg(DOMAIN_NAME)));
        emit AfficherMessageStatut(tr("Téléchargement terminé"), 5);

        Configuration::instance()->calendrierLancements() = Lancements::LectureCalendrierLancements();
        show();

    } catch (Exception const &e) {
    }

    /* Retour */
    return;
}

