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
 * >    fichierxml.h
 *
 * Localisation
 * >    librairies.systeme
 *
 * Heritage
 * >    QFile
 *
 * Description
 * >    Utilitaires lies a la manipulation des fichiers xml
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 novembre 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef FICHIERXML_H
#define FICHIERXML_H

#include <QFile>
#include <QtXml/QtXml>


class QString;

class FichierXml : public QFile
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief FichierXml Constructeur par defaut
     * @param[in] fichierXml nom du fichier xml
     */
    explicit FichierXml(const QString &fichierXml);


    /*
     * Methodes publiques
     */
    /**
     * @brief Ouverture Ouverture d'un fichier xml
     * @param[in] alarme affichage des messages d'erreur
     * @return document xml
     * @throw Exception
     */
    QDomDocument Ouverture(const bool alarme = true);


    /*
     * Accesseurs
     */
    QString nomfic() const;
    QString version();


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
    QString _fichier;
    QString _nomfic;
    QString _version;


    /*
     * Methodes privees
     */

};

#endif // FICHIERXML_H
