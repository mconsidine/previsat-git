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
 * >    18 octobre 2022
 *
 */

#ifndef CARTE_H
#define CARTE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


class Date;
class Observateur;
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
    explicit Carte(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Carte();

    /*
     * Accesseurs
     */
    Ui::Carte *ui() const;


    /*
     * Methodes publiques
     */

public slots:

    void mouseMoveEvent(QMouseEvent *evt);
    void mousePressEvent(QMouseEvent *evt);

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
    void EffacerMessageStatut();
    void RecalculerPositions();
    void ReinitFlags();


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
    double DEG2PXHZ;
    double DEG2PXVT;

    Ui::Carte *_ui;
    QGraphicsScene *scene;

    bool _mcc;
    int _lsol;
    int _bsol;

    double _largeur;
    double _hauteur;
    double _largeurParalleles;
    double _hauteurMeridiens;

    unsigned int _maxMeridiens;
    unsigned int _maxParalleles;
    double _stepMeridiens;
    double _stepParalleles;

    QList<bool> _als;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageGrille Affichage de la grille de coordonnees
     */
    void AffichageGrille();

    /**
     * @brief AffichageLune Affichage de la Lune
     */
    void AffichageLune();

    /**
     * @brief AffichageSatellite Affichage par defaut d'un satellite (sans icone)
     * @param satellite satellite
     * @param lsat longitude du satellite, en pixels
     * @param bsat latitude du satellite, en pixels
     * @param lcarte largeur de la carte du monde, en pixels
     * @param hcarte hauteur de la carte du monde, en pixels
     */
    void AffichageSatelliteDefaut(const Satellite &satellite, const int lsat, const int bsat, const int lcarte, const int hcarte) const;

    /**
     * @brief AffichageSatellites Affichage des satellites
     */
    void AffichageSatellites();

    /**
     * @brief AffichageSoleil Affichage du Soleil
     */
    void AffichageSoleil();

    /**
     * @brief AffichageStations Affichage des stations
     */
    void AffichageStations();

    /**
     * @brief AffichageTraceAuSol Affichage de la trace au sol du satellite par defaut
     */
    void AffichageTraceAuSol();

    /**
     * @brief AffichageZoneOmbre Affichage de la zone d'ombre
     */
    void AffichageZoneOmbre();

    /**
     * @brief AffichageZoneVisibilite Affichage de la zone de visibilite des satellites
     */
    void AffichageZoneVisibilite();

    /**
     * @brief Initialisation Initialisation de la classe Carte
     */
    void Initialisation();

};

#endif // CARTE_H
