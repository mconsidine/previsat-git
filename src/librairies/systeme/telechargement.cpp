/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    19 decembre 2019
 *
 * Date de revision
 * >
 *
 */

#include "telechargement.h"

QString Telechargement::_dirTmp;


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Telechargement::Telechargement(const QString &dirDwn)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _mng.setNetworkAccessible(QNetworkAccessManager::Accessible);
    _dirDwn = dirDwn;

    /* Retour */
    return;
}

void Telechargement::setDirTmp(const QString &dirTmp)
{
    _dirTmp = dirTmp;
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
 */
/*
 * Ajout d'un fichier dans la liste de telechargement
 */
void Telechargement::AjoutFichier(const QUrl &url)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_listeFichiers.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(TelechargementSuivant()));
    }
    _listeFichiers.enqueue(url);

    /* Retour */
    return;
}

/*
 * Ajout d'une liste de fichiers a telecharger
 */
void Telechargement::AjoutListeFichiers(const QStringList &listeFichiers)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    for(const QString &url : listeFichiers) {
        AjoutFichier(QUrl::fromEncoded(url.toLocal8Bit()));
    }

    if (_listeFichiers.isEmpty()) {
        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
    }

    /* Retour */
    return;
}

/*
 * Telechargement d'un fichier
 */
void Telechargement::TelechargementFichier(const QString &fichier, const bool async)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QUrl url(fichier);

    /* Corps de la methode */
    AjoutFichier(url);

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    const QNetworkRequest requete(url);
    _rep = _mng.get(requete);

    if (!async) {
        // Creation d'une boucle pour rendre le telechargement synchrone
        QEventLoop loop;
        connect(_rep, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
    }

    if (_listeFichiers.isEmpty()) {
        QTimer::singleShot(0, this, SIGNAL(TelechargementFini()));
    }

    /* Retour */
    return;
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
 * Ecriture du fichier
 */
void Telechargement::EcritureFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _fichier.write(_rep->readAll());

    /* Retour */
    return;
}

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

    if (vitesse < 1024.) {
        unite = tr("o/s");
    } else if (vitesse < 1048576.) {
        vitesse /= 1024.;
        unite = tr("ko/s");
    } else {
        vitesse /= 1048576.;
        unite = tr("Mo/s");
    }

    emit Progression(static_cast<int>(octetsRecus), static_cast<int>(octetsTotal), vitesse, unite);

    /* Retour */
    return;
}

/*
 * Fin de l'enregistrement du fichier
 */
void Telechargement::FinEnregistrementFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    _fichier.close();

    if (_rep->error()) {
        // Erreur lors du telechargement
        _fichier.remove();
    } else {
        // Deplacement du fichier vers le repertoire de destination
        const QFileInfo f(_fichier);
        QFile fi(_dirDwn + QDir::separator() + f.fileName());
        if (fi.exists()) {
            fi.remove();
        }
        _fichier.rename(fi.fileName());
    }
    _rep->deleteLater();
    TelechargementSuivant();

    /* Retour */
    return;
}

/*
 * Telechargement du fichier suivant
 */
void Telechargement::TelechargementSuivant()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_listeFichiers.isEmpty()) {
        emit TelechargementFini();
    } else {
        const QUrl url = _listeFichiers.dequeue();
        const QString fic = QFileInfo(url.path()).fileName();
        _fichier.setFileName(_dirTmp + QDir::separator() + fic);

        if (_fichier.open(QIODevice::WriteOnly)) {

            const QNetworkRequest requete(url);
            _rep = _mng.get(requete);
            connect(_rep, SIGNAL(downloadProgress(qint64, qint64)), SLOT(ProgressionTelechargement(qint64, qint64)));
            connect(_rep, SIGNAL(finished()), SLOT(FinEnregistrementFichier()));
            connect(_rep, SIGNAL(readyRead()), SLOT(EcritureFichier()));
            _tempsEcoule.start();
        } else {
            // Erreur
            TelechargementSuivant();
        }
    }

    /* Retour */
    return;
}
