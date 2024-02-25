/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    interface.carte
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

#include <QFrame>


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
     * @param[in] parent parent
     * @throw Exception
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
     * Methodes publiques
     */
    /**
     * @brief CalculNumeroOrbiteISS Calcul du numero d'orbite de l'ISS
     * @param[in] date date
     * @return numero d'orbite de l'ISS
     */
    static int CalculNumeroOrbiteISS(const Date &date);


public slots:

    /**
     * @brief show
     */
    /**
     * @brief show Affichage des coordonnees ISS
     * @param[in] dateCourante date courante
     * @param[in] dateEcl date d'eclipse
     */
    void show(const Date &dateCourante, const Date &dateEcl);

    void setPolice();


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
    Ui::CoordISS *_ui;


    /*
     * Methodes privees
     */


};

#endif // COORDISS_H
