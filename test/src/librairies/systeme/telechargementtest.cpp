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

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/telechargement.h"
#include "telechargementtest.h"
#include "test/src/testtools.h"


using namespace TestTools;


void TelechargementTest::testAll()
{
    testTelechargementFichier();
    testUrlExiste();
}

void TelechargementTest::testTelechargementFichier()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.mkpath(dir.path() + QDir::separator() + "test");
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    // Telechargement d'un fichier texte
    const QString fic = QString("%1test/visual-ok.txt").arg(DOMAIN_NAME);

    Telechargement dwn(QDir::current().path() + QDir::separator() + "test" + QDir::separator());
    const QUrl url(fic);
    dwn.TelechargementFichier(url);

    QString ficRes = dwn.dirDwn() + QDir::separator() + QFileInfo(url.path()).fileName();
    QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + "visual-ok.txt";

    CompareFichiers(ficRes, ficRef);

    try {
        const QString fic2 = QString("%1test/visual-ok2.txt").arg(DOMAIN_NAME);
        const QUrl url2(fic2);
        dwn.TelechargementFichier(url2);
    } catch (PreviSatException &e) {
    }
}

void TelechargementTest::testUrlExiste()
{
    qInfo(Q_FUNC_INFO);

    const QUrl urlLastNews(QString("%1informations/").arg(DOMAIN_NAME) + "last_news_fr.html");
    QCOMPARE(Telechargement::UrlExiste(urlLastNews), true);
}
