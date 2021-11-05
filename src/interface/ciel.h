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
 * >    5 novembre 2021
 *
 */

#ifndef CIEL_H
#define CIEL_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFrame>
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/dates/date.h"
#include "librairies/observateur/observateur.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/corps/systemesolaire/soleil.h"


class QPolygonF;
class Onglets;
class QGraphicsScene;
class LigneConstellation;
class Constellation;
class Etoile;
class Planete;

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
              const Date &dateDeb = Date(), const Date &dateMax = Date(), const Date &dateFin = Date());

    /**
     * @brief AffichagePhaseLune Affichage de la phase lunaire
     * @param lune lune
     * @param dimensionPx rayon de la lune (en pixels)
     * @return ensemble des points decrivant la partie dans l'ombre
     */
    static QPolygonF AffichagePhaseLune(const Lune &lune, const int dimensionPx);

    /**
     * @brief CalculCouleurCiel Determination de la couleur du ciel
     * @param hauteurSoleil hauteur du Soleil (en degres)
     * @return
     */
    static QBrush CalculCouleurCiel(const double hauteurSoleil);


public slots:

    void resizeEvent(QResizeEvent *evt);


signals:

    void AfficherMessageStatut(const QString &message, const int secondes = -1);
    void AfficherMessageStatut2(const QString &message);
    void AfficherMessageStatut3(const QString &message);
    void EffacerMessageStatut();


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

    bool _labelHeure;
    QList<Satellite> _satellites;
    QList<Planete> _planetes;
    Observateur _observateur;
    Soleil _soleil;
    Lune _lune;

    /*
     * Methodes privees
     */


private slots:

    bool eventFilter(QObject *watched, QEvent *event);

};

#endif // CIEL_H
