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
 * >    main.cpp
 *
 * Localisation
 * >
 *
 * Description
 * >    Demarrage de l'application
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    1er mai 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QSettings>
#include <QSplashScreen>
#include <QSharedMemory>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "interface/previsat.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/logmessage.h"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#include <QApplication>


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


int main(int argc, char *argv[])
{
    try {

        /* Declarations des variables locales */

        /* Initialisations */
        QApplication a(argc, argv);

        a.setOrganizationName(ORG_NAME);
        a.setApplicationName(APP_NAME);
        a.setOrganizationDomain(DOMAIN_NAME);

        /* Corps de la methode */
        // Definitions preliminaires pour le logiciel
        Configuration::instance()->Initialisation();

        // Installation de la gestion du fichier de log
        const LogMessage msg(Configuration::instance()->dirLog() + QDir::separator() + APP_NAME, settings.value("fichier/nbFichiersLog", 10).toUInt());
        Q_UNUSED(msg)

        // Creation de la fenetre principale
        PreviSat w;

        // Verification si une instance de PreviSat existe
        const qint64 pid = a.applicationPid();
        QSharedMemory mem;
        mem.setKey("pid" + QString(APP_NAME));

        if (!mem.create(sizeof(pid))) {
            if (mem.error() == QSharedMemory::AlreadyExists) {
                if (mem.attach(QSharedMemory::ReadOnly)) {

                    qWarning() << QString("Une instance de %1 est déjà ouverte").arg(APP_NAME);
                    throw PreviSatException(QObject::tr("Une instance de %1 est déjà ouverte").arg(APP_NAME), MessageType::WARNING);
                }

            } else if (mem.error() != QSharedMemory::NoError) {
                qWarning() << "Erreur lors de la verification d'instance unique :" << mem.errorString();
                throw PreviSatException(mem.errorString(), MessageType::WARNING);
            }
        }

        // Lancement du splash screen
        QSplashScreen * const splash = new QSplashScreen;
        splash->setPixmap(QPixmap(":/resources/interface/splashscreen.png"));
        splash->show();

        // Initialisation de la configuration
        const Qt::Alignment alignement = Qt::AlignRight | Qt::AlignVCenter;
        splash->showMessage(QObject::tr("Initialisation de la configuration...") + "     ", alignement, Qt::white);
        a.processEvents();
        w.ChargementConfiguration();

        // Ouverture du fichier d'elements orbitaux par defaut
        splash->showMessage(QObject::tr("Ouverture du fichier GP...") + "     ", alignement, Qt::white);
        a.processEvents();
        w.ChargementGP();

        // Mise a jour eventuelle des fichiers d'elements orbitaux
        splash->showMessage(QObject::tr("Mise à jour des éléments orbitaux...") + "     ", alignement, Qt::white);
        a.processEvents();
        w.MajGP();

        // Demarrage de l'application en mode temps reel
        splash->showMessage(QObject::tr("Démarrage de l'application...") + "     ", alignement, Qt::white);
        a.processEvents();
        w.DemarrageApplication();

        // Affichage de la fenetre principale et suppression du splash screen
        w.show();
        splash->finish(&w);
        delete splash;

        /* Retour */
        return a.exec();

    } catch (PreviSatException const &e) {
        return EXIT_FAILURE;
    }
}
