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
 * >    fichierxmltest.cpp
 *
 * Localisation
 * >    test.librairies.systeme
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    24 fevrier 2024
 *
 * Date de revision
 * >    27 avril 2025
 *
 */

#include <QtTest>
#include "fichierxmltest.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"
#include "testtools.h"


using namespace TestTools;

static QDir dir;

void FichierXmlTest::testAll()
{
    dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    static_cast<void> (dir.cd(APP_NAME));

    testFichierXml();
    testOuverture1();
    testOuverture2();
}

void FichierXmlTest::testFichierXml()
{
    qInfo(Q_FUNC_INFO);

    const QString fic = dir.path()
                        + QDir::separator() + "test" +
                        QDir::separator() + "data" +
                        QDir::separator() + "config" +
                        QDir::separator() + "flares.xml";

    FichierXml fi(fic);

    QCOMPARE(fi.nomfic(), "flares.xml");
    QCOMPARE(fi.version(), "1.0");
}

void FichierXmlTest::testOuverture1()
{
    qInfo(Q_FUNC_INFO);

    // Ouverture nominale
    const QString fic = dir.path() +
                        QDir::separator() + "test" +
                        QDir::separator() + "data" +
                        QDir::separator() + "config" +
                        QDir::separator() + "flares.xml";

    FichierXml fi(fic);

    const QDomDocument document = fi.Ouverture(false);

    QCOMPARE(document.isNull(), false);
    QCOMPARE(fi.version(), "1.0");
}

void FichierXmlTest::testOuverture2()
{
    qInfo(Q_FUNC_INFO);

    // Cas degrades
    const QString fic = dir.path() +
                         QDir::separator() + "test" +
                         QDir::separator() + "data" +
                         QDir::separator() + "conf" +
                         QDir::separator() + "flares.xml";

    FichierXml fi(fic);

    const QDomDocument document = fi.Ouverture(true);

    QCOMPARE(document.isNull(), true);
}
