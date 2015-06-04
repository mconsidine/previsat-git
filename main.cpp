/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
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
 * >    3 juin 2015
 *
 */

#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wconversion"
#include "previsat.h"
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QTranslator>
#pragma GCC diagnostic warning "-Wconversion"

int main(int argc, char *argv[])
{
    /* Declarations des variables locales */

    /* Initialisations */
    QApplication a(argc, argv);
    a.setOrganizationName("Astropedia");
    a.setApplicationName("PreviSat");
    a.setOrganizationDomain("http://astropedia.free.fr/");

    /* Corps de la methode */
    const QString locale = PreviSat::DeterminationLocale();

    QTranslator qtTranslator;
    qtTranslator.load(QString("qt_") + locale, a.applicationDirPath());
    a.installTranslator(&qtTranslator);

    QTranslator appTranslator;
    appTranslator.load(QString("PreviSat_") + locale, a.applicationDirPath());
    a.installTranslator(&appTranslator);

    // Verification si une instance de PreviSat existe
    const qint64 pid = a.applicationPid();
    QSharedMemory mem;
    mem.setKey("pid");
    if (!mem.create(sizeof(pid))) {
        if (mem.error() == QSharedMemory::AlreadyExists) {
            if (mem.attach(QSharedMemory::ReadOnly)) {
                const QString msg = QObject::tr("Une instance de %1 est déjà ouverte");
                QMessageBox::warning(0, QObject::tr("Information"), msg.arg(QCoreApplication::applicationName()));
                return 1;
            }
        } else {
            QMessageBox::information(0, QObject::tr("Information"), mem.errorString());
        }
    } else {
        mem.lock();
        memcpy(mem.data(), &pid, mem.size());
        mem.unlock();
    }

    // Lancement du splash screen et demarrage de l'application
    QSplashScreen * const splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/resources/splashscreen.png"));
    splash->show();

    PreviSat w;

    const Qt::Alignment alignement = Qt::AlignRight | Qt::AlignVCenter;
    splash->showMessage(QObject::tr("Initialisation de la configuration...") + "     ", alignement, Qt::white);
    a.processEvents();
    w.ChargementConfig();

    splash->showMessage(QObject::tr("Ouverture du fichier TLE...") + "     ", alignement, Qt::white);
    a.processEvents();
    w.ChargementTLE();

    splash->showMessage(QObject::tr("Mise à jour des TLE...") + "     ", alignement, Qt::white);
    a.processEvents();
    w.MAJTLE();

    splash->showMessage(QObject::tr("Démarrage de l'application...") + "     ", alignement, Qt::white);
    a.processEvents();
    w.DemarrageApplication();

    w.show();
    splash->finish(&w);
    delete splash;

    /* Retour */
    return a.exec();

}
