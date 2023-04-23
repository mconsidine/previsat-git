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
 * >    informationsiss.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    26 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "configuration/evenementsstationspatiale.h"
#include "informationsiss.h"
#include "ui_informationsiss.h"
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
InformationsISS::InformationsISS(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::InformationsISS)
{
    _ui->setupUi(this);

    _ui->evenementsISS->setStyleSheet("QHeaderView::section {" \
                                      "background-color:rgb(235, 235, 235);" \
                                      "border-top: 0px solid grey;" \
                                      "border-bottom: 1px solid grey;" \
                                      "border-right: 1px solid grey;" \
                                      "font-size: 12px;" \
                                      "font-weight: 600 }");

    _ui->evenementsISS->resizeColumnsToContents();

}


/*
 * Destructeur
 */
InformationsISS::~InformationsISS()
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
void InformationsISS::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void InformationsISS::show()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const EvenementsStation &evenements = Configuration::instance()->evenementsStation();
    _ui->evenementsISS->setVisible(false);
    _ui->formLayoutWidget->setVisible(false);
    _ui->formLayoutWidget_2->setVisible(false);
    _ui->evenementsISS->disconnect();
    _ui->evenementsISS->model()->removeRows(0, _ui->evenementsISS->rowCount());

    /* Corps de la methode */
    if (evenements.masseStationSpatiale > 0.) {

        QString masse;
        QString surface;
        QString uniteMasse;
        QString uniteDist;
        QString uniteDv;
        QString uniteSurface;

        if (settings.value("affichage/unite").toBool()) {

            uniteDist = tr("km", "Kilometer");
            uniteDv = tr("m/s", "meter per second");
            uniteMasse = tr("kg", "Kilogram");
            uniteSurface = tr("m^2", "meter square");

            masse = QString::number(evenements.masseStationSpatiale, 'f', 2);
            surface = QString::number(evenements.surfaceTraineeAtmospherique, 'f', 2);

        } else {

            uniteDist = tr("nmi", "nautical mile");
            uniteDv = tr("ft/s", "foot per second");
            uniteMasse = tr("lb", "pound");
            uniteSurface = tr("ft^2", "foot square");

            masse = QString::number(evenements.masseStationSpatiale / TERRE::KG_PAR_LIVRE, 'f', 2);
            surface = QString::number(evenements.surfaceTraineeAtmospherique * TERRE::PIED_PAR_METRE * TERRE::PIED_PAR_METRE, 'f', 2);
        }

        // Masse de la station spatiale
        _ui->masseISS->setText(QString("%1 %2").arg(masse).arg(uniteMasse));

        // Coefficient de trainnee atmospherique
        _ui->coefficientTrainee->setText(QString::number(evenements.coefficientTraineeAtmospherique, 'f', 2));

        // Surface de trainee atmospherique
        _ui->surfaceTrainee->setText(QString("%1 %2").arg(surface).arg(uniteSurface));

        _ui->formLayoutWidget->setVisible(true);
        _ui->formLayoutWidget_2->setVisible(true);

        // Dates de debut et de fin des evenements
        const Date dateDebut = Date::ConversionDateNasa(evenements.dateDebutEvenementsStationSpatiale);
        const QString annee = QString::number(dateDebut.annee());

        if (evenements.evenementsStationSpatiale.isEmpty()) {
            _ui->lbl_evenementsISS->setText(tr("Aucun évènement contenu dans le fichier ISS"));
        } else {

            int j = 0;
            QTableWidgetItem * itemEvt;
            QTableWidgetItem * itemDate;
            QTableWidgetItem * itemDeltaV;
            QTableWidgetItem * itemApogee;
            QTableWidgetItem * itemPerigee;

            // Liste des evenements
            QStringListIterator it(evenements.evenementsStationSpatiale);
            while (it.hasNext()) {

                const QString evt = it.next();

                // Nom de l'evenement
                const QString nomEvt = evt.mid(1, 20).trimmed();

                const QStringList detailsEvt = evt.mid(21).split(" ", Qt::SkipEmptyParts);

                // Date de l'evenement
                const QString dateEvt = Date::ConversionDateNasa(annee + "-" + detailsEvt.first())
                        .ToShortDateAMJ(DateFormat::FORMAT_MILLISEC, (settings.value("affichage/systemeHoraire").toBool()) ?
                                            DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H);

                // Delta-V
                const QString deltaV = (settings.value("affichage/unite").toBool()) ?
                            detailsEvt.at(1) : QString::number(detailsEvt.at(1).toDouble() * TERRE::PIED_PAR_METRE, 'f', 1);

                // Apogee et perigee
                const QString apogee = (settings.value("affichage/unite").toBool()) ?
                            detailsEvt.at(2) : QString::number(detailsEvt.at(2).toDouble() * TERRE::MILE_PAR_KM, 'f', 1);

                const QString perigee = (settings.value("affichage/unite").toBool()) ?
                            detailsEvt.at(3) : QString::number(detailsEvt.at(3).toDouble() * TERRE::MILE_PAR_KM, 'f', 1);

                // Ajout d'une ligne dans le tableau
                _ui->evenementsISS->insertRow(j);
                _ui->evenementsISS->setRowHeight(j, 16);

                // Intitule de l'evenement
                itemEvt = new QTableWidgetItem(nomEvt);
                itemEvt->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                itemEvt->setFlags(itemEvt->flags() & ~Qt::ItemIsEditable);
                _ui->evenementsISS->setItem(j, 0, itemEvt);

                // Date
                itemDate = new QTableWidgetItem(dateEvt);
                itemDate->setTextAlignment(Qt::AlignCenter);
                itemDate->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
                _ui->evenementsISS->setItem(j, 1, itemDate);

                // Delta-V
                itemDeltaV = new QTableWidgetItem(deltaV);
                itemDeltaV->setTextAlignment(Qt::AlignCenter);
                itemDeltaV->setFlags(itemDeltaV->flags() & ~Qt::ItemIsEditable);
                itemDeltaV->setToolTip(uniteDv);
                _ui->evenementsISS->setItem(j, 2, itemDeltaV);

                // Apogee
                itemApogee = new QTableWidgetItem(apogee);
                itemApogee->setTextAlignment(Qt::AlignCenter);
                itemApogee->setFlags(itemApogee->flags() & ~Qt::ItemIsEditable);
                itemApogee->setToolTip(uniteDist);
                _ui->evenementsISS->setItem(j, 3, itemApogee);

                // Perigee
                itemPerigee = new QTableWidgetItem(perigee);
                itemPerigee->setTextAlignment(Qt::AlignCenter);
                itemPerigee->setFlags(itemPerigee->flags() & ~Qt::ItemIsEditable);
                itemPerigee->setToolTip(uniteDist);
                _ui->evenementsISS->setItem(j, 4, itemPerigee);

                j++;
            }

            _ui->evenementsISS->horizontalHeader()->setStretchLastSection(true);
            _ui->evenementsISS->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            _ui->evenementsISS->sortItems(1);
            _ui->lbl_evenementsISS->setVisible(false);
            _ui->evenementsISS->setVisible(true);
        }
    } else {
        _ui->lbl_evenementsISS->setText("Fichier d'informations ISS absent :\nCliquer sur 'Mettre à jour les informations de l'ISS'");
        _ui->lbl_evenementsISS->setVisible(true);
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
void InformationsISS::on_majEvenementsIss_clicked()
{
    /* Declarations des variables locales */

    /* Initialisations */
    Telechargement tel(Configuration::instance()->dirLocalData());

    /* Corps de la methode */
    emit AfficherMessageStatut(tr("Téléchargement du fichier d'informations ISS..."));
    tel.TelechargementFichier(QUrl(QString("%1data/%2").arg(DOMAIN_NAME).arg(Configuration::instance()->nomFichierEvenementsStationSpatiale())));
    emit AfficherMessageStatut(tr("Téléchargement terminé"), 5);

    Configuration::instance()->evenementsStation() = EvenementsStationSpatiale::LectureEvenementsStationSpatiale();
    show();

    /* Retour */
    return;
}
