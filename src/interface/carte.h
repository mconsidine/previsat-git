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
 * >    carte.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Carte du monde
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 decembre 2019
 *
 * Date de revision
 * >    6 novembre 2021
 *
 */

#ifndef CARTE_H
#define CARTE_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFrame>
#pragma GCC diagnostic warning "-Wconversion"


class Date;
class Observateur;
class Onglets;
class Satellite;
class QGraphicsScene;

namespace Ui {
class Carte;
}

class Carte : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Carte Constructeur par defaut
     * @param parent parent
     */
    explicit Carte(Onglets *onglets, QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Carte();

    /*
     * Accesseurs
     */

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */


public slots:

    /*
     * Methodes publiques
     */    
    /**
     * @brief show Affichage des courbes sur la carte du monde
     */
    void show();

    void resizeEvent(QResizeEvent *evt);

    /**
     * @brief AffichageSiteLancement Affichage de l'info bulle du site de lancement
     * @param acronyme acronyme
     * @param siteLancement site de lancement
     */
    void AffichageSiteLancement(const QString &acronyme, const Observateur &siteLancement);


signals:

    void AfficherMessageStatut(const QString &message, const int secondes = -1);
    void AfficherMessageStatut2(const QString &message);
    void AfficherMessageStatut3(const QString &message);
    void EcritureTleDefautRegistre();
    void EffacerMessageStatut();
    void RecalculerPositions();


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
    double DEG2PXHZ;
    double DEG2PXVT;

    Ui::Carte *ui;
    QGraphicsScene *scene;
    Onglets *_onglets;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageSatellite Affichage par defaut d'un satellite (sans icone)
     * @param satellite satellite
     * @param lsat longitude du satellite, en pixels
     * @param bsat latitude du satellite, en pixels
     * @param lcarte largeur de la carte du monde, en pixels
     * @param hcarte hauteur de la carte du monde, en pixels
     */
    void AffichageSatellite(const Satellite &satellite, const int lsat, const int bsat, const int lcarte, const int hcarte) const;


private slots:

    bool eventFilter(QObject *watched, QEvent *event);

};

#endif // CARTE_H
