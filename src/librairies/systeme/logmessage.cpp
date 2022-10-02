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
 * >    2 octobre 2022
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
static const QHash<QtMsgType, QString> typeMessage = {
    { QtMsgType::QtInfoMsg,     "INFO   " },
    { QtMsgType::QtDebugMsg,    "DEBUG  " },
    { QtMsgType::QtWarningMsg,  "WARNING" },
    { QtMsgType::QtCriticalMsg, "ERREUR " },
    { QtMsgType::QtFatalMsg,    "ERREUR " }
};

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
    // Gestion du fichier de log en cas d'erreur lors de la precedente execution
    QFile fi(fichier);
    if (fi.exists() && (fi.size() != 0)) {

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            const QString contenu = fi.readAll();
            fi.close();

            QString dest = fichier;
            dest = dest.replace(".log", "-prev.log");

            if (contenu.contains("ERREUR")) {
                fi.rename(dest);
            } else {
                QFile fi2(dest);
                if (fi2.exists()) {
                    fi2.remove();
                }
            }
        }
    }

    /* Corps du constructeur */
    _fichierLog.reset(new QFile(fichier));
    _fichierLog.data()->open(QFile::WriteOnly | QFile::Text);
    qInstallMessageHandler(messageHandler);

#if (BUILD_TEST == false)
    QTextStream out(_fichierLog.data());
    out << "       Date (UTC)       : Type    : Fichier                                       : Fonction                                 : Message"
        << Qt::endl;
    out << "-----------------------------------------------------------------------------------------------------------------------------"
           "-------------------------------------" << Qt::endl;
    out.flush();
#endif

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
    const QString nomFichier = QFileInfo(context.file).fileName();
    const QString nomFonction = QString(context.function).section("(", -2, -2).section(" ", -1).section(":", -1);
    QString message = msg;

    /* Corps de la methode */
    QTextStream out(_fichierLog.data());
#if (BUILD_TEST == false)
    out << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss.zzz : ");
#endif

    out << typeMessage.value(type) << " : ";
    if (!nomFichier.isEmpty()) {
        const QString fic = QString("%1 (ligne %2)").arg(nomFichier).arg(context.line);
        out << QString("%1 : %2 : ").arg(fic, -45).arg(nomFonction, -40);
    }
    out << message.replace("\"", "") << Qt::endl;
    out.flush();

    /* Retour */
    return;
}
