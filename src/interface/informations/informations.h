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
 * >    24 octobre 2023
 *
 */

#ifndef INFORMATIONS_H
#define INFORMATIONS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


class QTextBrowser;
class QUrl;

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
     * @param[in] fenetreParent fenetre parent
     */
    explicit Informations(QWidget *fenetreParent);


    /*
     * Destructeur
     */
    ~Informations();


    /*
     * Accesseurs
     */
    Ui::Informations *ui() const;


public slots:

    /*
     * Methodes publiques
     */
    void changeEvent(QEvent *evt);


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
    Ui::Informations *_ui;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe Informations
     */
    void Initialisation();

    /**
     * @brief OuvertureInfo Ouverture du fichier d'informations
     * @param[in] nomfic nom du fichier
     * @param[in] onglet onglet
     * @param[out] zoneTexte zone de texte
     */
    void OuvertureInfo(const QString &nomfic, QWidget *onglet, QTextBrowser *zoneTexte);


private slots:

    void on_ok_clicked();


};

#endif // INFORMATIONS_H
