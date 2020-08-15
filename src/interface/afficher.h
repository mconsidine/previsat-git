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
 * >
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
    friend class PrevisionTest;
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
    void ChargementResultats() const;

    void EcrireEntete() const;

    QStringList ElementsPrevisions(const QList<ResultatPrevisions> &liste) const;
    QStringList ElementsDetailsPrevisions(const ResultatPrevisions &res) const;

};

#endif // AFFICHER_H
