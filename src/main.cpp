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
 * >    23 decembre 2024
 *
 */

#include <QApplication>
#include <QDir>
#include <QLabel>
#include <QScreen>
#include <QSettings>
#include <QSharedMemory>
#include <QStyle>
#include "configuration/configuration.h"
#include "interface/previsat.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/logmessage.h"


// Registre
#if (PORTABLE_BUILD)
static QSettings settings(QString("%1.ini").arg(APP_NAME), QSettings::IniFormat);
#else
static QSettings settings(ORG_NAME, APP_NAME);
#endif


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
        LogMessage(Configuration::instance()->dirLog() + QDir::separator() + APP_NAME, settings.value("fichier/nbFichiersLog", 10).toUInt());

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
                    throw Exception(QObject::tr("Une instance de %1 est déjà ouverte").arg(APP_NAME), MessageType::WARNING);
                }

            } else if (mem.error() != QSharedMemory::NoError) {
                qWarning() << "Erreur lors de la verification d'instance unique :" << mem.errorString();
                throw Exception(mem.errorString(), MessageType::WARNING);
            }
        }

        // Preparation du splashscreen
        QLabel * const splash = new QLabel(nullptr);
        splash->setPixmap(QPixmap(":/resources/interface/splashscreen.png"));
        splash->adjustSize();
        splash->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, splash->size(), qApp->primaryScreen()->geometry()));
        splash->setWindowFlags(Qt::FramelessWindowHint);
        splash->setAlignment(Qt::AlignCenter);
        splash->setAttribute(Qt::WA_TranslucentBackground);

        QFont police;
        police.setPointSize(16);
        police.setWeight(QFont::DemiBold);

        // Nom du logiciel et numero de version
        QLabel * const logiciel = new QLabel(QString(APP_NAME) + " " + VER_MAJ, splash);
        logiciel->setStyleSheet("color: white;");
        logiciel->setFont(police);
        logiciel->adjustSize();
        logiciel->move(95, 220);

        // Annees de developpement
        police.setPointSize(12);
        QLabel * const annees = new QLabel(ANNEES_DEV, splash);
        annees->setStyleSheet("color: white;");
        annees->setFont(police);
        annees->adjustSize();
        annees->move(112, 250);

        // Description
        police.setPointSize(15);
        QLabel * const description = new QLabel("Satellite tracking software", splash);
        description->setStyleSheet("color: white;");
        description->setFont(police);
        description->adjustSize();
        description->move(36, 270);

        // Site
        police.setBold(false);
        QLabel * const site = new QLabel(DOMAIN_NAME, splash);
        site->setStyleSheet("color: white;");
        site->setFont(police);
        site->adjustSize();
        site->move(270, 40);

        // Messages
        police.setPointSizeF(10.5);
        QLabel * const message = new QLabel(QObject::tr("Initialisation de la configuration..."), splash);
        message->setStyleSheet("color: white;");
        message->setFont(police);
        message->setAlignment(Qt::AlignRight);
        message->setGeometry(180, 160, 300, message->height());

        // Lancement du splashscreen
        splash->show();

        // Initialisation de la configuration
        qInfo() << "Initialisation de la configuration";
        a.processEvents();
        w.ChargementConfiguration();

        // Ouverture du fichier d'elements orbitaux par defaut
        qInfo() << "Ouverture du fichier GP";
        message->setText(QObject::tr("Ouverture du fichier GP..."));
        a.processEvents();
        w.ChargementGP();

        // Mise a jour eventuelle des fichiers d'elements orbitaux
        qInfo() << "Mise a jour des elements orbitaux";
        message->setText(QObject::tr("Mise à jour des éléments orbitaux..."));
        a.processEvents();
        w.MajGP();

        // Telechargement des groupes Starlink
        qInfo() << "Mise a jour des Starlink";
        message->setText(QObject::tr("Mise à jour des Starlink..."));
        a.processEvents();
        w.TelechargementGroupesStarlink(false);

        // Demarrage de l'application en mode temps reel
        qInfo() << "Demarrage de l'application";
        message->setText(QObject::tr("Démarrage de l'application..."));
        a.processEvents();
        w.DemarrageApplication();

        // Affichage de la fenetre principale et fermeture du splashscreen
        w.show();
        splash->close();

        /* Retour */
        return a.exec();

    } catch (Exception const &e) {
        return EXIT_FAILURE;
    }
}
