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
 * >    telechargementoptions.h
 *
 * Localisation
 * >    interface.options
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre de telechargement de lieux d'observation ou de sons
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 avril 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef TELECHARGEMENTOPTIONS_H
#define TELECHARGEMENTOPTIONS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"


namespace Ui {
class TelechargementOptions;
}

class QListWidgetItem;

class TelechargementOptions : public QMainWindow
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
    explicit TelechargementOptions(const TypeTelechargement &type, QWidget *parent);


    /*
     * Destructeur
     */
    ~TelechargementOptions();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */


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
    Ui::TelechargementOptions *_ui;
    TypeTelechargement _type;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la fenetre TelechargementOptions
     */
    void Initialisation();

    /**
     * @brief TelechargerListe Telechargement de la liste de fichiers
     */
    void TelechargerListe();


private slots:

    void on_fermer_clicked();
    void on_filtreFichiers_textChanged(const QString &arg1);
    void on_listeFichiers_itemClicked(QListWidgetItem *item);
    void on_telecharger_clicked();
};

#endif // TELECHARGEMENTOPTIONS_H
