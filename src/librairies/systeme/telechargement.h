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
 * >    16 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef TELECHARGEMENT_H
#define TELECHARGEMENT_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QElapsedTimer>
#include <QObject>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


class QString;
class QUrl;

class Telechargement : public QObject
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Telechargement Constructeur par defaut
     * @param dir repertoire de telechargement
     */
    explicit Telechargement(const QString &dir);


    /*
     * Methodes publiques
     */
    /**
     * @brief TelechargementFichier Telechargement de fichier
     * @param url adresse
     * @param alarme affichage des messages d'erreur
     */
    void TelechargementFichier(const QUrl &url, const bool alarme = true);


    /*
     * Accesseurs
     */
    const QString &dirDwn() const;


signals:

    void Progression(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */
    QString _dirDwn;
    QElapsedTimer _tempsEcoule;


    /*
     * Methodes privees
     */


private slots:

    /**
     * @brief ProgressionTelechargement Progression du telechargement
     * @param octetsRecus octets recus
     * @param octetsTotal octets du fichier total
     */
    void ProgressionTelechargement(qint64 octetsRecus, qint64 octetsTotal);

};

#endif // TELECHARGEMENT_H
