/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QScopedPointer>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "logmessage.h"


QString LogMessage::_nomFicLog;
QScopedPointer<QFile> _fichierLog;

static const QHash<QtMsgType, QString> typeMessage = {
    { QtMsgType::QtInfoMsg,     "INFO   " },
    { QtMsgType::QtDebugMsg,    "DEBUG  " },
    { QtMsgType::QtWarningMsg,  "WARNING" },
    { QtMsgType::QtCriticalMsg, "ERREUR " },
    { QtMsgType::QtFatalMsg,    "FATAL  " }
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
LogMessage::LogMessage(const QString &baseNomFichier, const unsigned int nbMaxFic)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo ff(baseNomFichier);
    const QDir di(ff.absoluteDir());
    const QStringList filtre(QStringList () << ff.fileName() + "*.log");
    QStringList listeFicLog = di.entryList(filtre, QDir::Files);

    QFile fi1;
    while (listeFicLog.size() > nbMaxFic) {
        fi1.setFileName(di.absolutePath() + QDir::separator() + listeFicLog.first());
        fi1.remove();
        listeFicLog.removeFirst();
    }

#if (BUILD_TEST == true)
    const QString dateFichierLog = "";
#else
    const QString dateFichierLog = QDateTime::currentDateTimeUtc().toString("_yyyy-MM-dd_hh-mm-ss-zzz");
#endif

    /* Corps du constructeur */
    _nomFicLog = QDir::toNativeSeparators(baseNomFichier + dateFichierLog + ".log");
    _fichierLog.reset(new QFile(_nomFicLog));
    _fichierLog.data()->open(QFile::WriteOnly | QFile::Text);
    qInstallMessageHandler(messageHandler);

#if (BUILD_TEST == false)
    QTextStream out(_fichierLog.data());
    out << QString("       Date (UTC)       : Type    : %1 : %2 : Message").arg("Fichier", -45).arg("Fonction", -45) << Qt::endl;
    out << QString(162, '-') << Qt::endl;
    out.flush();
#endif

    /* Retour */
    return;
}

/*
 * Destructeur
 */
LogMessage::~LogMessage()
{
    _fichierLog.data()->close();
    qInstallMessageHandler(0);
}


/*
 * Accesseurs
 */
const QString &LogMessage::nomFicLog()
{
    return _nomFicLog;
}


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
        out << QString("%1 : %2 : ").arg(fic, -45).arg(nomFonction, -45);
    }

    if (message.startsWith("\"") && message.endsWith("\"")) {
        message.remove(0, 1).chop(1);
    }

    out << message << Qt::endl;
    out.flush();

    /* Retour */
    return;
}
