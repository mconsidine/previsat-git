/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QtGui/QApplication>
#include <QTranslator>
#include "previsat.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qint64 pid = a.applicationPid();
    QSharedMemory mem;
    mem.setKey("pid");
    if (!mem.create(sizeof(pid))) {
        if (mem.error() == QSharedMemory::AlreadyExists) {
            if (mem.attach(QSharedMemory::ReadOnly)) {
                mem.lock();
                qint64 p = *(qint64 *) mem.constData();
                mem.unlock();
                QMessageBox::warning(0, QObject::tr("Information"), QObject::tr("Une instance de PreviSat est déjà ouverte"));
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

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/resources/splashscreen.png"));
    splash->show();

    const QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator qtTranslator;
    qtTranslator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator appTranslator;
    appTranslator.load(QString("PreviSat_") + locale, a.applicationDirPath());
    a.installTranslator(&appTranslator);

    PreviSat w;

    const Qt::Alignment alignement = Qt::AlignRight | Qt::AlignVCenter;
    splash->showMessage(QObject::tr("Initialisation de la configuration...") + "     ", alignement, Qt::white);
    w.ChargementConfig();

    splash->showMessage(QObject::tr("Ouverture du fichier TLE...") + "     ", alignement, Qt::white);
    w.ChargementTLE();

    splash->showMessage(QObject::tr("Mise à jour des TLE...") + "     ", alignement, Qt::white);
    w.MAJTLE();

    splash->showMessage(QObject::tr("Démarrage de l'application...") + "     ", alignement, Qt::white);
    w.DemarrageApplication();

    w.show();
    splash->finish(&w);
    delete splash;

    return a.exec();
}
