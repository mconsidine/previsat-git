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
 * >    ciel.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Radar
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    3 avril 2020
 *
 * Date de revision
 * >
 *
 */

#ifndef CIEL_H
#define CIEL_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFrame>
#pragma GCC diagnostic warning "-Wconversion"


class Onglets;
class QGraphicsScene;
class Observateur;
class Soleil;
class Lune;
class LigneConstellation;
class Constellation;
class Etoile;
class Planete;
class Satellite;

namespace Ui {
class Ciel;
}

class Ciel : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Ciel Constructeur par defaut
     * @param parent parent
     */
    explicit Ciel(Onglets *onglets, QWidget *parent = nullptr);

    /*
     * Destructeur
     */
    ~Ciel();


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
     * @brief show Affichage du ciel
     */
    void show(const Observateur &observateur,
              const Soleil &soleil,
              const Lune &lune,
              const QList<LigneConstellation> &lignesCst,
              const QList<Constellation> &constellations,
              const QList<Etoile> &etoiles,
              const QList<Planete> &planetes,
              const QList<Satellite> &satellites,
              const bool maxFlash = false,
              const bool labelHeure = false,
              const double offset = 0.);

    /**
     * @brief CalculCouleurCiel Determination de la couleur du ciel
     * @param hauteurSoleil hauteur du Soleil (en degres)
     * @return
     */
    static QBrush CalculCouleurCiel(const double hauteurSoleil);


public slots:

    void resizeEvent(QResizeEvent *evt);


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
    Ui::Ciel *ui;
    QGraphicsScene *scene;
    Onglets *_onglets;

    /*
     * Methodes privees
     */


};

#endif // CIEL_H
