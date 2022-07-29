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
 * >    gpformattest.cpp
 *
 * Localisation
 * >    test.librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/corps/satellite/gpformat.h"
#include "gpformattest.h"
#include "test/src/testtools.h"


using namespace TestTools;

void GPFormatTest::testAll()
{
    testLectureFichier();
}

void GPFormatTest::testLectureFichier()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString fic = dir.path() + QDir::separator() + "test" + QDir::separator() + "elem" + QDir::separator() + "visual.xml";
    const QMap<QString, ElementsOrbitaux> mapElem = GPFormat::LectureFichier(fic, QString(), 0);

    QCOMPARE(mapElem.keys().size(), 160);
}
