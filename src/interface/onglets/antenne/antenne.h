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
 * >    antenne.h
 *
 * Localisation
 * >    interface.onglets.antenne
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Pilotage d'antenne
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 octobre 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef ANTENNE_H
#define ANTENNE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"

namespace Ui {
class Antenne;
}

struct ElementsAOS;


class Antenne : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Antenne Constructeur par defaut
     * @param parent parent
     */
    explicit Antenne(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Antenne();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief InitAffichageFrequences Initialisation de l'affichage des frequences
     */
    void InitAffichageFrequences();

    void show(const QString &nomsat, const QString &dateAOS, const ElementsAOS &elementsAOS);


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
    Ui::Antenne *_ui;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe Antenne
     */
    void Initialisation();


};

#endif // ANTENNE_H