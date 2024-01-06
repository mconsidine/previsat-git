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
 * >    20 mars 2023
 *
 */

#ifndef CIEL_H
#define CIEL_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/dates/date.h"
#include "librairies/observateur/observateur.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/corps/systemesolaire/soleil.h"

#define TOPO2X(lciel, ht, az, xf) qRound(lciel - lciel * xf * (1. - ht * MATHS::DEUX_SUR_PI) * sin(az))
#define TOPO2Y(hciel, ht, az, yf) qRound(hciel - hciel * yf * (1. - ht * MATHS::DEUX_SUR_PI) * cos(az))


class QPolygonF;
class QGraphicsScene;
class Constellation;
class Etoile;
class LigneConstellation;


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
     * @param[in] parent parent
     */
    explicit Ciel(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Ciel();


    /*
     * Accesseurs
     */
    bool fenetreMax() const;


    /*
     * Modificateurs
     */
    void setFenetreMax(bool f);


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
              const std::array<Planete, PLANETE::NB_PLANETES> &planetes,
              const QList<Satellite> &satellites,
              const bool fenetreMax,
              const bool maxFlash = false,
              const bool labelHeure = false,
              const Date &dateDeb = Date(), const Date &dateMax = Date(), const Date &dateFin = Date());

    /**
     * @brief AffichagePhaseLune Affichage de la phase lunaire
     * @param[in] lune lune
     * @param[in] dimensionPx rayon de la lune (en pixels)
     * @return ensemble des points decrivant la partie dans l'ombre
     */
    static QPolygonF AffichagePhaseLune(const Lune &lune, const int dimensionPx);

    /**
     * @brief CalculCouleurCiel Determination de la couleur du ciel
     * @param[in] hauteurSoleil hauteur du Soleil (en degres)
     * @return couleur du ciel
     */
    static QBrush CalculCouleurCiel(const double hauteurSoleil);

    /**
     * @brief CouleurTraceCiel Determination de la couleur de la trace dans le ciel
     * @param[in] trace element de la trace dans le ciel
     * @return couleur de l'element de la trace dans le ciel
     */
    static QColor CouleurTraceCiel(const ElementsTraceCiel &trace);


public slots:

    void mouseMoveEvent(QMouseEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void resizeEvent(QResizeEvent *evt);


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
    Ui::Ciel *_ui;
    QGraphicsScene *scene;

    Observateur _observateur;
    QList<Satellite> _satellites;
    std::array<Planete, PLANETE::NB_PLANETES> _planetes;
    Soleil _soleil;
    Lune _lune;
    bool _fenetreMax;
    bool _labelHeure;

    int _lciel;
    int _hciel;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageConstellations Affichage des constellations
     * @param[in] lignesCst lignes des constellations
     * @param[in] constellations constellations
     */
    void AffichageConstellations(const QList<LigneConstellation> &lignesCst, const QList<Constellation> &constellations);

    /**
     * @brief AffichageEtoiles Affichage des etoiles
     * @param[in] etoiles etoiles
     */
    void AffichageEtoiles(const QList<Etoile> &etoiles);

    /**
     * @brief AffichageLune Affichage de la Lune
     */
    void AffichageLune();

    /**
     * @brief AffichagePlanetes1 Affichage des planetes (cas general)
     */
    void AffichagePlanetes1();

    /**
     * @brief AffichagePlanetes2 Affichage des planetes (si une planete inferieure est plus proche que la Soleil)
     */
    void AffichagePlanetes2();

    /**
     * @brief AffichageSatellites Affichage des satellites
     * @param[in] dateDeb date de debut de flash
     * @param[in] dateMax date max de flash
     * @param[in] dateFin date de fin de flash
     * @param[in] maxFlash maximum du flash
     */
    void AffichageSatellites(const Date &dateDeb, const Date &dateMax, const Date &dateFin, const bool maxFlash);

    /**
     * @brief AffichageSoleil Affichage du Soleil
     */
    void AffichageSoleil();

    /**
     * @brief Initialisation Initialisation de la classe Ciel
     */
    void Initialisation();

};

#endif // CIEL_H
