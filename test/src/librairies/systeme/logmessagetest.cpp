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
 * >    logmessagetest.cpp
 *
 * Localisation
 * >    test.librairies.systeme
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    21 mai 2022
 *
 * Date de revision
 * >
 *
 */

#include <QtTest>
#include "librairies/systeme/logmessage.h"
#include "logmessagetest.h"
#include "testtools.h"


using namespace TestTools;


void LogMessageTest::testAll()
{
    testLogMessage();
}

void LogMessageTest::testLogMessage()
{
    qInfo(Q_FUNC_INFO);

    QDir dir = QDir::current();
    dir.mkpath(dir.path() + QDir::separator() + "test");
    dir.cdUp();
    dir.cdUp();
    static_cast<bool> (dir.cd(APP_NAME));

    const QString ficRes = QDir::current().path() + QDir::separator() + "test" + QDir::separator() + APP_NAME;
    LogMessage(ficRes, 1);

    // Ecriture de messages informatifs
    qInfo() << APP_NAME;
    qInfo() << ORG_NAME;

    // Ecriture de messages de debug
    qDebug() << "Message de debug 1";
    qDebug() << "Message de debug 2";

    // Ecriture de messages d'avertissements
    qWarning() << "Message d'avertissement 1";
    qWarning() << "Message d'avertissement 2";

    // Ecriture de messages d'erreurs
    qCritical() << R"("Message d'erreurs 1")";
    qCritical() << "Message d'erreurs 2";

    const QString ficRef = dir.path() + QDir::separator() + "test" + QDir::separator() + "ref" + QDir::separator() + APP_NAME + ".log";
    CompareFichiers(ficRes + ".log", ficRef);

    QCOMPARE(LogMessage::nomFicLog(), QDir::toNativeSeparators(ficRes + ".log"));
    LogMessage::Fermeture();
}
