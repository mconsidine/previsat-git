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
 * >    recherchesatellite.h
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Recherche satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef RECHERCHESATELLITE_H
#define RECHERCHESATELLITE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class RechercheSatellite;
}

class RechercheSatellite : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief RechercheSatellite Constructeur par defaut
     * @param parent parent
     */
    explicit RechercheSatellite(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~RechercheSatellite();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */


public slots:

    /**
     * @brief SauveOngletRecherche Sauvegarde des donnees de l'onglet
     * @param fichier nom du fichier
     */
    void SauveOngletRecherche(const QString &fichier);

    void changeEvent(QEvent *evt);
    void on_noradDonneesSat_valueChanged(int arg1);
    void show();


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
    Ui::RechercheSatellite *_ui;

    QStringList _resultatsSatellitesTrouves;


    /*
     * Methodes privees
     */

private slots:

    void on_nom_returnPressed();
    void on_cosparDonneesSat_returnPressed();
    void on_satellitesTrouves_currentRowChanged(int currentRow);
};

#endif // RECHERCHESATELLITE_H
