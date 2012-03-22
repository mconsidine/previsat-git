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
#include <QSplashScreen>
#include <QtGui/QApplication>
#include <QTranslator>
#include "previsat.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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

    Qt::Alignment alignement = Qt::AlignRight | Qt::AlignVCenter;
    splash->showMessage(QObject::tr("Initialisations...") + "     ", alignement, Qt::white);
    w.Initialisations();

    splash->showMessage(QObject::tr("Chargement du fichier TLE...") + "     ", alignement, Qt::white);
    w.InitFicTLE();

    splash->showMessage(QObject::tr("Démarrage de l'application...") + "     ", alignement, Qt::white);
    w.DemarrageApplication();

    w.show();
    splash->finish(&w);
    delete splash;

    return a.exec();
}
