/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    options.h
 *
 * Localisation
 * >    interface.options
 *
 * Heritage
 * >    QDialog
 *
 * Description
 * >    Fenetre d'options
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 aout 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDialog>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Options Constructeur par defaut
     * @param parent fenetre parent
     */
    explicit Options(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Options();


    /*
     * Accesseurs
     */
    Ui::Options *ui();


    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Initialisation Initialisation de la fenetre Options
     */
    void Initialisation();


public slots:

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
    Ui::Options *_ui;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageLieuObs Affichage des lieux d'observation selectionnes
     */
    void AffichageLieuObs();


private slots:

    void on_listeOptions_currentRowChanged(int currentRow);


};

#endif // OPTIONS_H
