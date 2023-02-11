/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    coordiss.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Coordonnees ISS du Wall Command Center
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 mars 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef COORDISS_H
#define COORDISS_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFrame>
#pragma GCC diagnostic warning "-Wconversion"


class Date;

namespace Ui {
class CoordISS;
}

class CoordISS : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief CoordISS Constructeur par defaut
     * @param parent parent
     */
    explicit CoordISS(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CoordISS();

    /*
     * Accesseurs
     */
    Ui::CoordISS *ui();

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
     * @brief CalculNumeroOrbiteISS Calcul du numero d'orbite de l'ISS
     * @param date date
     * @return numero d'orbite de l'ISS
     */
    static int CalculNumeroOrbiteISS(const Date &date);


public slots:

    /**
     * @brief show
     */
    /**
     * @brief show Affichage des coordonnees ISS
     * @param dateCourante date courante
     * @param dateEcl date d'eclipse
     */
    void show(const Date &dateCourante, const Date &dateEcl);

    void setPolice();


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
    Ui::CoordISS *_ui;


    /*
     * Methodes privees
     */


};

#endif // COORDISS_H
