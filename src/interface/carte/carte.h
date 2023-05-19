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
 * >    carte.h
 *
 * Localisation
 * >    interface.carte
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
 * >    19 mai 2023
 *
 */

#ifndef CARTE_H
#define CARTE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/corps/satellite/satellite.h"


class QGraphicsScene;
class Date;
class ItemGroup;
class Observateur;

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
     * @param[in] parent parent
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
    void resizeEvent(QResizeEvent *evt);
    void wheelEvent(QWheelEvent *evt);

    /**
     * @brief show Affichage des courbes sur la carte du monde
     */
    void show();

    /**
     * @brief AffichageSiteLancement Affichage de l'info bulle du site de lancement
     * @param[in] acronyme acronyme
     * @param[in] siteLancement site de lancement
     */
    void AffichageSiteLancement(const QString &acronyme, const Observateur &siteLancement);

    /**
     * @brief ChargementCarteDuMonde Chargement de la carte du monde
     */
    void ChargementCarteDuMonde();


signals:

    void AfficherMessageStatut(const QString &message, const int secondes = -1);
    void AfficherMessageStatut2(const QString &message);
    void AfficherMessageStatut3(const QString &message);
    void EffacerMessageStatut();
    void RecalculerPositions();
    void ReinitFlags();
    void SendCurrentScale(const double echelle);


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
    Ui::Carte *_ui;
    QGraphicsScene *scene;

    bool _mcc;
    int _largeurCarte;
    int _hauteurCarte;
    QPixmap _pixMap;
    int _lsol;

    double _largeurParalleles;
    double _hauteurMeridiens;

    unsigned int _maxMeridiens;
    unsigned int _maxParalleles;
    double _stepMeridiens;
    double _stepParalleles;

    bool _resize;
    QList<ItemGroup *> _groupes;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageFrontieres Affichage des frontieres
     */
    void AffichageFrontieres();

    /**
     * @brief AffichageGrille Affichage de la grille de coordonnees
     */
    void AffichageGrille();

    /**
     * @brief AffichageLieuxObservation Affichage des lieux d'observation
     */
    void AffichageLieuxObservation();

    /**
     * @brief AffichageLune Affichage de la Lune
     */
    void AffichageLune();

    /**
     * @brief AffichageSatellite Affichage par defaut d'un satellite (sans icone)
     * @param[in] satellite satellite
     * @param[in] lsat longitude du satellite, en pixels
     * @param[in] bsat latitude du satellite, en pixels
     */
    void AffichageSatelliteDefaut(const Satellite &satellite, const int lsat, const int bsat) const;

    /**
     * @brief AffichageSAA_ZOE Affichage de la ZOE et de la SAA pour le Wall Command Center
     */
    void AffichageSAA_ZOE();

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
     * @brief CouleurTraceAuSol Determination de la couleur du point de la trace au sol
     * @param[in] trace point de la trace au sol
     * @return couleur du point
     */
    QColor CouleurTraceAuSol(const ElementsTraceSol &trace) const;

    /**
     * @brief Initialisation Initialisation de la classe Carte
     */
    void Initialisation();

    /**
     * @brief LectureCoordonnees Lecture des coordonnees dans le fichier de coordonnees geographiques
     * @param[in] coordonnees coordonnees
     * @return Lignes de coordonnees
     */
    QPolygonF LectureCoordonnees(const QString &coordonnees);

    /**
     * @brief LectureFichierKml Lecture d'un fichier de coordonnees geographiques au format kml
     * @param[in] fichier nom du fichier
     * @param[in] visible visibilite des frontieres
     * @param[in] echelleMin echelle minimale ou la frontiere doit etre visible
     */
    void LectureFichierKml(const QString &fichier, const bool visible = true, const double echelleMin = 0.);

};

#endif // CARTE_H
