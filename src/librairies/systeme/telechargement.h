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
 * >    24 octobre 2023
 *
 */

#ifndef TELECHARGEMENT_H
#define TELECHARGEMENT_H

#include <QElapsedTimer>
#include <QObject>


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
     * @param[in] dir repertoire de telechargement
     */
    explicit Telechargement(const QString &dir);


    /*
     * Methodes publiques
     */
    /**
     * @brief TelechargementFichier Telechargement de fichier
     * @param[in] url adresse
     * @param[in] alarme affichage d'une boite de message en cas d'erreur
     * @param[in] logAlarme affichage d'un message d'erreur dans le fichier de log
     * @throw Exception
     */
    void TelechargementFichier(const QUrl &url,
                               const bool alarme = true,
                               const bool logAlarme = true);

    /**
     * @brief UrlExiste Verification de l'existence d'une adresse
     * @param[in] url url
     * @return vrai si l'url existe
     */
    static bool UrlExiste(const QUrl &url);


    /*
     * Accesseurs
     */
    QString dirDwn() const;


signals:

    void Progression(const int octetsRecus,
                     const int octetsTotal,
                     const double vitesse,
                     const QString &unite);


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
     * @param[in] octetsRecus octets recus
     * @param[in] octetsTotal octets du fichier total
     */
    void ProgressionTelechargement(qint64 octetsRecus,
                                   qint64 octetsTotal);

};

#endif // TELECHARGEMENT_H
