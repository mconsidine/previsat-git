/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    3 octobre 2015
 *
 * Date de revision
 * >
 *
 */

#ifndef INFORMATIONS_H
#define INFORMATIONS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"

namespace Ui {
class Informations;
}

class Informations : public QMainWindow
{
    Q_OBJECT

public:

    /* Constructeurs */
    explicit Informations(const QString &norad, QWidget *fenetreParent = 0);
    ~Informations();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private slots:

    void closeEvent(QCloseEvent *evt);

    /**
     * @brief on_nom_returnPressed Recherche basee sur le nom des objets
     */
    void on_nom_returnPressed();

    /**
     * @brief on_norad_valueChanged Recherche basee sur le numero NORAD
     * @param arg1 numero NORAD
     */
    void on_norad_valueChanged(int arg1);

    /**
     * @brief on_cospar_returnPressed Recherche basee sur la designation COSPAR
     */
    void on_cospar_returnPressed();

    /**
     * @brief on_satellitesTrouves_currentRowChanged Selection d'un objet dans la liste
     * @param currentRow indice de l'objet
     */
    void on_satellitesTrouves_currentRowChanged(int currentRow);


private:

    /* Constantes privees */

    /* Variables privees */
    Ui::Informations *ui;

    /* Methodes privees */
    /**
     * @brief AffichageResultats Remplissage de la liste d'objets
     */
    void AffichageResultats();

};

#endif // INFORMATIONS_H
