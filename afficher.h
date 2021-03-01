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
 * >    afficher.h
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2012
 *
 * Date de revision
 * >    5 mars 2016
 *
 */

#ifndef AFFICHER_H
#define AFFICHER_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QMainWindow>
#include <QTableWidgetItem>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wpacked"
#include "librairies/observateur/observateur.h"
#include "previsions/conditions.h"

namespace Ui {
    class Afficher;
}

class Afficher : public QMainWindow
{
    Q_OBJECT

public:

    /* Constructeurs */
    explicit Afficher(const Conditions &conditions, const Observateur &observateur, QStringList &result, QWidget *fenetreParent = 0);
    ~Afficher();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief show Affichage des resultats
     * @param fic fichier resultat
     */
    void show(const QString &fic);

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    Ui::Afficher *ui;
    QString _fichier;

    /* Methodes privees */
    /**
     * @brief load Chargement des resultats
     */
    void load();

    /**
     * @brief loadMap Chargement de la map
     * @param i indice du tableau de resultats
     */
    void loadMap(const int i);

    /**
     * @brief loadSky Chargement de la carte du ciel
     * @param j indice du tableau de resultats
     */
    void loadSky(const int j);

private slots:

    void closeEvent(QCloseEvent *evt);
    void resizeEvent(QResizeEvent * evt);

    /**
     * @brief on_actionEnregistrer_triggered Enregistrement du fichier resultat
     */
    void on_actionEnregistrer_triggered();

    /**
     * @brief on_listePrevisions_currentCellChanged Selection d'un passage
     * @param currentRow indice du tableau
     * @param currentColumn (non utilise)
     * @param previousRow (non utilise)
     * @param previousColumn (non utilise)
     */
    void on_listePrevisions_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    /**
     * @brief on_ongletsResultats_currentChanged Selection de l'onglet des resultats
     * @param index (non utilise)
     */
    void on_ongletsResultats_currentChanged(int index);
};

#endif // AFFICHER_H
