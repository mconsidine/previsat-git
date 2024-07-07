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
 * >    telechargement.cpp
 *
 * Localisation
 * >    librairies.systeme
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    16 juin 2022
 *
 * Date de revision
 * >    24 octobre 2023
 *
 */

#include <QtNetwork/QtNetwork>
#include "librairies/exceptions/exception.h"
#include "telechargement.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Telechargement::Telechargement(const QString &dir) :
    _dirDwn(dir)
{
}


/*
 * Methodes publiques
 */
/*
 * Telechargement de fichier
 */
void Telechargement::TelechargementFichier(const QUrl &url,
                                           const bool alarme,
                                           const bool logAlarme)
{
    /* Declarations des variables locales */
    QNetworkAccessManager manager;

    /* Initialisations */
#if (BUILD_TEST)
    Q_UNUSED(logAlarme)
#endif
    QNetworkReply *reponse = manager.get(QNetworkRequest(url));
    _tempsEcoule.restart();

    /* Corps de la methode */
    QEventLoop event;
    connect(reponse, &QNetworkReply::downloadProgress, this, &Telechargement::ProgressionTelechargement);
    connect(reponse, &QNetworkReply::finished, &event, &QEventLoop::quit);
    event.exec();

    const QFileInfo ff(url.path());
    const QString fic = (ff.fileName().contains("gp")) ? url.query().section("&", -2, -2).section("=", -1) + ".xml" : ff.fileName();

    if (reponse->error()) {
#if (!BUILD_TEST)
        if (logAlarme) {
            qWarning() << "Erreur lors du téléchargement du fichier" << fic;
        }
#endif
        if (alarme) {
            throw Exception(tr("Erreur lors du téléchargement du fichier %1").arg(fic), MessageType::WARNING);
        } else {
            throw Exception();
        }

    } else {

        // Ecriture du fichier telecharge
        QFile fi(_dirDwn + QDir::separator() + fic);
        if (fi.open(QIODevice::WriteOnly)) {

            fi.write(reponse->readAll());
            fi.close();

#if (!BUILD_TEST)
            qInfo () << "Téléchargement du fichier" << fic << "OK";
#endif

        } else {

#if (!BUILD_TEST)
            if (logAlarme) {
                qWarning() << QString("Impossible d'écrire le fichier %1 dans le répertoire %2").arg(fic).arg(_dirDwn);
            }

            if (alarme) {
                throw Exception(tr("Impossible d'écrire le fichier %1 dans le répertoire %2").arg(fic).arg(_dirDwn), MessageType::WARNING);
            } else {
                throw Exception();
            }
#endif
        }
    }

    /* Retour */
    return;
}

/*
 * Verification de l'existence d'une adresse
 */
bool Telechargement::UrlExiste(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool res = false;

    /* Corps de la methode */
    QTcpSocket socket;
    socket.connectToHost(url.host(), 80);

    if (socket.waitForConnected()) {

        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\nHost: " + url.host().toUtf8() + "\r\n\r\n");

        if (socket.waitForReadyRead()) {

            const QByteArray bytes = socket.readAll();
            if (bytes.contains("200 OK")) {
                res = true;
            }
        }
    }

    /* Retour */
    return res;
}


/*
 * Accesseurs
 */
QString Telechargement::dirDwn() const
{
    return _dirDwn;
}


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
 * Progression du telechargement
 */
void Telechargement::ProgressionTelechargement(qint64 octetsRecus,
                                               qint64 octetsTotal)
{
    /* Declarations des variables locales */
    QString unite;

    /* Initialisations */

    /* Corps de la methode */
    // Calcul de la vitesse de telechargement
    double vitesse = static_cast<double> (octetsRecus) * 1000. / static_cast<double> (_tempsEcoule.elapsed());

#if (!COVERAGE_TEST)
    if (vitesse < 1024.) {
        unite = tr("o/s");
    } else if (vitesse < 1048576.) {
        vitesse /= 1024.;
        unite = tr("ko/s");
    } else {
        vitesse /= 1048576.;
        unite = tr("Mo/s");
    }
#endif

    emit Progression(static_cast<int>(octetsRecus), static_cast<int>(octetsTotal), vitesse, unite);

    /* Retour */
    return;
}
