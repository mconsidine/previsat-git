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
 * >    general.h
 *
 * Localisation
 * >    interface.onglets.general
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet General
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef GENERAL_H
#define GENERAL_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class General;
}

class Options;


class General : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief General Constructeur par defaut
     * @param parent parent
     */
    explicit General(Options *options, QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~General();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */

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
    Ui::General *_ui;
    unsigned int _indexLuneSoleil;

    Options *_options;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe General
     */
    void Initialisation();


private slots:

    void AffichageLieuObs();

    void on_soleilLuneSuiv_clicked();
    void on_soleilLunePrec_clicked();
    void on_stackedWidget_soleilLune_currentChanged(int arg1);
};

#endif // GENERAL_H
