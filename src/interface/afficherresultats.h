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
 * >    afficherresultats.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Affichage des resultats de previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    8 juin 2023
 *
 */

#ifndef AFFICHERRESULTATS_H
#define AFFICHERRESULTATS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "previsions/prevision.h"


namespace Ui {
class AfficherResultats;
}

class QGraphicsScene;
class QTableWidget;
class QTableWidgetItem;
class Ciel;
class Lune;
class Soleil;

class AfficherResultats : public QMainWindow
{
    Q_OBJECT
#if (BUILD_TEST == true)
    friend class EvenementsOrbitauxTest;
    friend class FlashsTest;
    friend class PrevisionTest;
    friend class TransitsTest;
#endif
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief AfficherResultats Constructeur par defaut
     * @param[in] typeCalcul type de calcul
     * @param[in] conditions conditions de prevision
     * @param[in] donnees donnees de prevision
     * @param[in] resultats resultats du calcul
     * @param[in] onglets onglets
     * @param[in] zoom niveau de zoom pour les maps
     * @param[in] parent fenetre parent
     */
    AfficherResultats(const TypeCalcul &typeCalcul, const ConditionsPrevisions &conditions, const DonneesPrevisions &donnees,
             const QMap<QString, QList<QList<ResultatPrevisions> > > &resultats, const int zoom = 9,
             QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~AfficherResultats();


    /*
     * Accesseurs
     */

    /*
     * Methodes publiques
     */


protected:

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
    Ui::AfficherResultats *_ui;
    QTableWidget *_tableDetail;
    QMainWindow *_afficherDetail;
    Ciel *_ciel;
    QGraphicsScene *scene;

    QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
    ConditionsPrevisions _conditions;
    DonneesPrevisions _donnees;
    TypeCalcul _typeCalcul;
    QString _mapResultats;
    int _zoom;
    bool _afficherEvt;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageDetailTransit Affichage du detail d'un transit
     * @param[in] observateur observateur
     * @param[in] soleil soleil
     * @param[in] lune lune
     * @param[in] list informations sur le transit
     */
    void AffichageDetailTransit(const Observateur &observateur, const Soleil &soleil, const Lune &lune, const QList<ResultatPrevisions> &list);

    /**
     * @brief ChargementCarte Chargement de la carte representant la trace du maximum
     * @param[in] observateur observateur
     * @param[in] list informations sur le maximum
     */
    void ChargementCarte(const Observateur &observateur, const QList<ResultatPrevisions> &list);

    /**
     * @brief ChargementResultats Chargement des resultats
     */
    void ChargementResultats();

    /**
     * @brief EcrireEntete Ecriture de l'entete du fichier de resultats
     */
    void EcrireEntete() const;

    /**
     * @brief ElementsEvenements Elements des evenements a afficher dans la fenetre de resultats
     * @param[in] index indice de la liste d'evenements
     * @param[in] liste liste d'evenements
     * @return elements des evenements
     */
    QStringList ElementsEvenements(const int index, const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsEvenements Elements des evenements pour la sauvegarde dans un fichier texte
     * @param[in] res evenements
     * @return detail des evenements
     */
    QStringList ElementsDetailsEvenements(const ResultatPrevisions &res) const;

    /**
     * @brief ElementsFlashs Elements des flashs a afficher dans la fenetre de resultats
     * @param[in] liste liste des flashs
     * @return elements des flashs
     */
    QStringList ElementsFlashs(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsFlashs Elements des flashs pour la sauvegarde dans un fichier texte ou pour afficher des details
     * @param[in] res flashs
     * @return detail des flashs
     */
    QStringList ElementsDetailsFlashs(const ResultatPrevisions &res) const;

    /**
     * @brief ElementsPrevisions Elements des previsions de passage a afficher dans la fenetre de resultats
     * @param[in] liste liste des previsions
     * @return elements des previsions
     */
    QStringList ElementsPrevisions(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsPrevisions Elements des previsions de passage pour la sauvegarde dans un fichier texte ou pour afficher des details
     * @param[in] res previsions
     * @return detail des previsions
     */
    QStringList ElementsDetailsPrevisions(const ResultatPrevisions &res) const;

    /**
     * @brief ElementsTransits Elements des transits a afficher dans la fenetre de resultats
     * @param[in] liste liste des transits
     * @return elements des transits
     */
    QStringList ElementsTransits(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsTransits Elements des transits pour la sauvegarde dans un fichier texte ou pour afficher des details
     * @param[in] res transits
     * @return detail des transits
     */
    QStringList ElementsDetailsTransits(const ResultatPrevisions &res) const;


private slots:

    void on_resultatsPrevisions_itemDoubleClicked(QTableWidgetItem *item);
    void on_actionEnregistrer_triggered();
    void on_actionEnregistrerTxt_triggered();
    void on_resultatsPrevisions_itemSelectionChanged();
    void on_afficherCarte_clicked();    

};

#endif // AFFICHERRESULTATS_H
