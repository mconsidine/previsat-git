/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    telechargement.h
 *
 * Localisation
 * >    librairies.systeme
 *
 * Heritage
 * >
 *
 * Description
 * >    Telechargement de fichiers
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

#ifndef TELECHARGEMENT_H
#define TELECHARGEMENT_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QtCore>
#include <QtNetwork>
#pragma GCC diagnostic warning "-Wconversion"


class Telechargement : public QObject
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Telechargement Constructeur par defaut
     * @param dirDwn repertoire de destination
     */
    Telechargement(const QString &dirDwn);

    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */
    static void setDirTmp(const QString &dirTmp);

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief AjoutFichier Ajout d'un fichier dans la liste de telechargement (telechargement asynchrone)
     * @param url url
     */
    void AjoutFichier(const QUrl &url);

    /**
     * @brief AjoutListeFichiers Ajout d'une liste de fichiers a telecharger (telechargement asynchrone)
     * @param listeFichiers liste de fichiers
     */
    void AjoutListeFichiers(const QStringList &listeFichiers);

    /**
     * @brief TelechargementFichier Telechargement d'un fichier
     * @param fichier nom du fichier
     * @param async telechargement asynchrone
     */
    void TelechargementFichier(const QString &fichier, const bool async = true);


signals:

    void TelechargementFini();
    void Progression(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private slots:

    /**
     * @brief EcritureFichier Ecriture du fichier
     */
    void EcritureFichier();

    /**
     * @brief ProgressionTelechargement Progression du telechargement
     * @param octetsRecus octets recus
     * @param octetsTotal octets du fichier total
     */
    void ProgressionTelechargement(qint64 octetsRecus, qint64 octetsTotal);

    /**
     * @brief FinEnregistrementFichier Fin de l'enregistrement du fichier
     */
    void FinEnregistrementFichier();

    /**
     * @brief TelechargementSuivant Telechargement du fichier suivant
     */
    void TelechargementSuivant();


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    bool _async;
    QString _dirDwn;
    static QString _dirTmp;
    QFile _fichier;
    QNetworkAccessManager _mng;
    QQueue<QUrl> _listeFichiers;
    QNetworkReply *_rep = nullptr;
    QElapsedTimer _tempsEcoule;

    /*
     * Methodes privees
     */


};

#endif // TELECHARGEMENT_H
