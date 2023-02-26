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
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include <QtNetwork/QtNetwork>
#pragma GCC diagnostic warning "-Wredundant-decls"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"
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
void Telechargement::TelechargementFichier(const QUrl &url, const bool alarme)
{
    /* Declarations des variables locales */
    QNetworkAccessManager manager;

    /* Initialisations */
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
#if (BUILD_TEST == false)
        qWarning() << "Erreur lors du téléchargement du fichier" << fic;
#endif
        if (alarme) {
            throw PreviSatException(tr("Erreur lors du téléchargement du fichier %1").arg(fic), MessageType::WARNING);
        }

    } else {

        // Ecriture du fichier telecharge
        QFile fi(_dirDwn + QDir::separator() + fic);
        if (fi.open(QIODevice::WriteOnly)) {

            fi.write(reponse->readAll());
            fi.close();
#if (BUILD_TEST == false)
            qInfo () << "Téléchargement du fichier" << fic << "OK";
#endif

        } else {

            qWarning() << QString("Impossible d'écrire le fichier %1 dans le répertoire %2").arg(fic).arg(_dirDwn);

            if (alarme) {
                throw PreviSatException(tr("Impossible d'écrire le fichier %1 dans le répertoire %2").arg(fic).arg(_dirDwn), MessageType::WARNING);
            }
        }
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
const QString &Telechargement::dirDwn() const
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
void Telechargement::ProgressionTelechargement(qint64 octetsRecus, qint64 octetsTotal)
{
    /* Declarations des variables locales */
    QString unite;

    /* Initialisations */

    /* Corps de la methode */
    // Calcul de la vitesse de telechargement
    double vitesse = static_cast<double> (octetsRecus) * 1000. / static_cast<double> (_tempsEcoule.elapsed());

#if (COVERAGE_TEST == false)
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
