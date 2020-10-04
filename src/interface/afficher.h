/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    afficher.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >
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
 * >    4 octobre 2020
 *
 */

#ifndef AFFICHER_H
#define AFFICHER_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"
#include <QMap>
#include "previsions/prevision.h"


namespace Ui {
class Afficher;
}

class QTableWidget;
class QTableWidgetItem;

class Afficher : public QMainWindow
{
    Q_OBJECT
#if BUILDTEST == true
    friend class EvenementsOrbitauxTest;
    friend class FlashsTest;
    friend class PrevisionTest;
    friend class TransitsIssTest;
#endif
public:

    /*
     *  Constructeurs
     */
    Afficher(const TypeCalcul &typeCalcul, const ConditionsPrevisions &conditions, const DonneesPrevisions &donnees,
             const QMap<QString, QList<QList<ResultatPrevisions> > > &resultats, QWidget *parent = nullptr);
    ~Afficher();

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

private slots:
    void on_resultatsPrevisions_itemDoubleClicked(QTableWidgetItem *item);
    void on_actionEnregistrerTxt_triggered();


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    Ui::Afficher *ui;
    QTableWidget *tableDetail;
    QMainWindow *afficherDetail;
    QMap<QString, QList<QList<ResultatPrevisions> > > _resultats;
    ConditionsPrevisions _conditions;
    DonneesPrevisions _donnees;
    TypeCalcul _typeCalcul;

    /*
     * Methodes privees
     */
    /**
     * @brief ChargementResultats Chargement des resultats
     */
    void ChargementResultats() const;

    /**
     * @brief EcrireEntete Ecriture de l'entete du fichier de resultats
     */
    void EcrireEntete() const;

    /**
     * @brief ElementsEvenements Elements des evenements a afficher dans la fenetre de resultats
     * @param liste liste d'evenements
     * @return elements des evenements
     */
    QStringList ElementsEvenements(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsEvenements Elements des evenements pour la sauvegarde dans un fichier texte
     * @param res evenements
     * @return detail des evenements
     */
    QStringList ElementsDetailsEvenements(const ResultatPrevisions &res) const;

    /**
     * @brief ElementsFlashs Elements des flashs a afficher dans la fenetre de resultats
     * @param liste liste des flashs
     * @return elements des flashs
     */
    QStringList ElementsFlashs(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsFlashs Elements des flashs pour la sauvegarde dans un fichier texte ou pour afficher des details
     * @param res flashs
     * @return detail des flashs
     */
    QStringList ElementsDetailsFlashs(const ResultatPrevisions &res) const;

    /**
     * @brief ElementsPrevisions Elements des previsions de passage a afficher dans la fenetre de resultats
     * @param liste liste des previsions
     * @return elements des previsions
     */
    QStringList ElementsPrevisions(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsPrevisions Elements des previsions de passage pour la sauvegarde dans un fichier texte ou pour afficher des details
     * @param res previsions
     * @return detail des previsions
     */
    QStringList ElementsDetailsPrevisions(const ResultatPrevisions &res) const;

    /**
     * @brief ElementsTransits Elements des transits ISS a afficher dans la fenetre de resultats
     * @param liste liste des transits
     * @return elements des transits
     */
    QStringList ElementsTransits(const QList<ResultatPrevisions> &liste) const;

    /**
     * @brief ElementsDetailsTransits Elements des transits ISS pour la sauvegarde dans un fichier texte ou pour afficher des details
     * @param res transits
     * @return detail des transits
     */
    QStringList ElementsDetailsTransits(const ResultatPrevisions &res) const;

};

#endif // AFFICHER_H
