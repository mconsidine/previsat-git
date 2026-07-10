/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    logging.h
 *
 * Localisation
 * >    interface.logging
 *
 * Heritage
 * >
 *
 * Description
 * >    Gestion des fichiers de log
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 novembre 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <QMainWindow>


namespace Ui {
class Logging;
}

class QAbstractButton;
class QTableWidgetItem;

class Logging : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Logging Constructeur par defaut
     * @param[in] fenetreParent fenetre parent
     */
    explicit Logging(QWidget *fenetreParent = nullptr);


    /*
     * Destructeur
     */
    ~Logging();


    /*
     * Methodes publiques
     */


public slots:

    void changeEvent(QEvent *evt);

    void show();


    /*
     * Accesseurs
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
    Ui::Logging *_ui;


    /*
     * Methodes privees
     */


private slots:

    /**
     * @brief OuvrirFichier Ouverture du fichier log selectionne
     */
    void OuvrirFichier();

    void on_listeBoutonsExporterLog_clicked(QAbstractButton *button);

};

#endif // LOGGING_H
