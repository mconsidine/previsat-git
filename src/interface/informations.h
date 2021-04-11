/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    informations.h
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    1er mai 2019
 *
 * Date de revision
 * >    20 octobre 2019
 *
 */

#ifndef INFORMATIONS_H
#define INFORMATIONS_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"


class QUrl;
class Onglets;

namespace Ui {
class Informations;
}

class Informations : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Informations Constructeur par defaut
     * @param fenetreParent fenetre parent
     */
    explicit Informations(QWidget *fenetreParent, Onglets *onglets);

    /*
     * Destructeur
     */
    ~Informations();

    /*
     * Accesseurs
     */

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
     * @brief UrlExiste Verifie si l'url existe
     * @param url url
     * @return vrai si l'url existe
     */
    static bool UrlExiste(const QUrl &url);


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


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    Ui::Informations *ui;
    Onglets *_onglets;

    /*
     * Methodes privees
     */
private slots:

    void on_ok_clicked();


};

#endif // INFORMATIONS_H
