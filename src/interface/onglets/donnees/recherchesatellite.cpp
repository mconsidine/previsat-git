/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    recherchesatellite.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
 *
 * Date de revision
 * >     27 aout 2022•
 *
 */

#include "recherchesatellite.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFile>
#include "ui_recherchesatellite.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
RechercheSatellite::RechercheSatellite(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::RechercheSatellite)
{
    _ui->setupUi(this);
}


/*
 * Destructeur
 */
RechercheSatellite::~RechercheSatellite()
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
/*
 * Sauvegarde des donnees de l'onglet
 */
void RechercheSatellite::SauveOngletRecherche(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fichier);
        if (sw.open(QIODevice::WriteOnly | QIODevice::Text)) {

            if (!sw.isWritable()) {
                qWarning() << "Problème de droits d'écriture du fichier" << sw.fileName();
                throw PreviSatException(tr("Problème de droits d'écriture du fichier %1").arg(sw.fileName()), MessageType::WARNING);
            }

            QTextStream flux(&sw);

#if (BUILD_TEST == false)
            const QString titre = "%1 %2 / %3 (c) %4";
            flux << titre.arg(QCoreApplication::applicationName()).arg(QString(APP_VER_MAJ)).arg(APP_NAME).arg(QString(APP_ANNEES_DEV))
                 << Qt::endl << Qt::endl << Qt::endl;
#endif

            // Donnees sur le satellite
            flux << tr("Nom                :") + " " + _ui->nomsat->text() << Qt::endl;

            QString chaine = tr("Numéro NORAD       : %1\t\tMagnitude std/max  : %2", "Standard/Maximal magnitude");
            flux << chaine.arg(_ui->numNorad->text()).arg(_ui->magnitudeStdMaxDonneesSat->text()) << Qt::endl;

            chaine = tr("Désignation COSPAR : %1\t\tModèle orbital     : %2");
            flux << chaine.arg(_ui->desigCospar->text()).arg(_ui->modeleDonneesSat->text()) << Qt::endl;

            chaine = tr("Dimensions/Section : %1%2");
            flux << chaine.arg(_ui->dimensionsDonneesSat->text()).arg((_ui->dimensionsDonneesSat->text() == tr("Inconnues")) ? "" : "^2")
                 << Qt::endl << Qt::endl;

            chaine = tr("Date de lancement  : %1\t\tApogée  (Altitude) : %2");
            flux << chaine.arg(_ui->dateLancementDonneesSat->text()).arg(_ui->apogeeDonneesSat->text()) << Qt::endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Date de rentrée    : %1\t\t").arg(_ui->dateRentree->text()) :
                                                       tr("Catégorie d'orbite : %1\t\t").arg(_ui->categorieOrbiteDonneesSat->text());
            flux << chaine + tr("Périgée (Altitude) : %1").arg(_ui->perigeeDonneesSat->text()) << Qt::endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Catégorie d'orbite : %1\t\t").arg(_ui->categorieOrbiteDonneesSat->text()) :
                                                       tr("Pays/Organisation  : %1\t\t").arg(_ui->paysDonneesSat->text());
            flux << chaine + tr("Période orbitale   : %1").arg(_ui->periodeDonneesSat->text()) << Qt::endl;

            chaine = (_ui->dateRentree->isVisible()) ? tr("Pays/Organisation  : %1\t\t").arg(_ui->paysDonneesSat->text()) :
                                                       tr("Site de lancement  : %1\t\t").arg(_ui->siteLancementDonneesSat->text());
            flux << chaine + tr("Inclinaison        : %1").arg(_ui->inclinaisonDonneesSat->text()) << Qt::endl;

            if (_ui->dateRentree->isVisible()) {
                flux << tr("Site de lancement  : %1").arg(_ui->siteLancementDonneesSat->text());
            }
        }
        sw.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void RechercheSatellite::changeEvent(QEvent *evt)
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

