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
 * >    logmessage.cpp
 *
 * Localisation
 * >    librairies.systeme
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    21 mai 2022
 *
 * Date de revision
 * >    18 aout 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDateTime>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include <QFile>
#include <QFileInfo>
#include <QScopedPointer>
#include <QTextStream>
#include "logmessage.h"


QScopedPointer<QFile> _fichierLog;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
LogMessage::LogMessage(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _fichierLog.reset(new QFile(fichier));
    _fichierLog.data()->open(QFile::WriteOnly | QFile::Text);
    qInstallMessageHandler(messageHandler);

    QTextStream out(_fichierLog.data());
    out << "       Date (UTC)       : Type    : Fichier                                       : Fonction                                 : Message"
        << Qt::endl;
    out << "-----------------------------------------------------------------------------------------------------------------------------"
           "-------------------------------------" << Qt::endl;
    out.flush();

    /* Retour */
    return;
}

LogMessage::~LogMessage()
{
    _fichierLog.data()->close();
    qInstallMessageHandler(0);
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
 */


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */
/*
 * Ecriture du message de log
 */
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QTextStream out(_fichierLog.data());
#if (BUILD_TEST == false)
    out << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss.zzz : ");
#endif

    switch (type)
    {
    default:
    case QtInfoMsg:
        out << "INFO    ";
        break;

    case QtDebugMsg:
        out << "DEBUG   ";
        break;

    case QtWarningMsg:
        out << "WARNING ";
        break;

    case QtCriticalMsg:
    case QtFatalMsg:
        out << "ERREUR  ";
        break;
    }

    const QString nomFichier = QFileInfo(context.file).fileName();
    const QString nomFonction = (nomFichier.isEmpty()) ?
                "" : QString(context.function).split(":", Qt::SkipEmptyParts).at(1).split("(", Qt::SkipEmptyParts).at(0);
    QString message = msg;

    out << ": " << ((nomFichier.isEmpty()) ?
                        "" : QString("%1 : %2 : ").arg(nomFichier + " (ligne " + QString::number(context.line) + ")", -45).arg(nomFonction, -40))
        << message.replace("\"", "") << Qt::endl;
    out.flush();

    /* Retour */
    return;
}
