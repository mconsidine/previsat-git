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
 * >    telechargementtest.cpp
 *
 * Localisation
 * >    test.librairies.systeme
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    16 juin 2022
 *
 * Date de revision
 * >    27 octobre 2023
 *
 */

#include <QtTest>
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"
#include "telechargementtest.h"
#include "testtools.h"


using namespace TestTools;


void TelechargementTest::testAll()
{
    testTelechargementFichier1();
    testTelechargementFichier2();
    testUrlExiste();
}

void TelechargementTest::testTelechargementFichier1()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.mkpath(dir.path() + QDir::separator() + "test");
    dir.cdUp();
    dir.cdUp();
    dir.cd(APP_NAME);

    // Telechargement d'un fichier texte
    const QString fic = QString("%1test/visual-ok.txt").arg(DOMAIN_NAME);

    Telechargement dwn(QDir::current().path() + QDir::separator() + "test" + QDir::separator());
    const QUrl url(fic);
    dwn.TelechargementFichier(url);

    QString ficRes = dwn.dirDwn() + QDir::separator() + QFileInfo(url.path()).fileName();
    QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "visual-ok.txt";

    CompareFichiers(ficRes, ficRef);
}

void TelechargementTest::testTelechargementFichier2()
{
    qInfo(Q_FUNC_INFO);

    Telechargement dwn(QDir::current().path() + QDir::separator() + "test" + QDir::separator());
    const QString fic = QString("%1test/visual-ok2.txt").arg(DOMAIN_NAME);
    const QUrl url(fic);
    QVERIFY_THROWS_EXCEPTION(Exception, dwn.TelechargementFichier(url));
}

void TelechargementTest::testUrlExiste()
{
    qInfo(Q_FUNC_INFO);

    const QUrl url(QString("%1test/visual-ok.txt").arg(DOMAIN_NAME));
    QCOMPARE(Telechargement::UrlExiste(url), true);
}
