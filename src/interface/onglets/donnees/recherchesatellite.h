/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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

#include <QFrame>


namespace Ui {
class RechercheSatellite;
}

class Observateur;

class RechercheSatellite : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief RechercheSatellite Constructeur par defaut
     * @param[in] parent parent
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
     * @param[in] fichier nom du fichier
     */
    void SauveOngletRecherche(const QString &fichier);

    void changeEvent(QEvent *evt);
    void on_noradDonneesSat_valueChanged(int arg1);
    void show();


signals:

    void AffichageSiteLancement(const QString &acronyme, const Observateur &site);
    void AfficherMessageStatut(const QString &message, const int secondes);


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

    bool eventFilter(QObject *watched, QEvent *event);

    void on_nom_returnPressed();
    void on_cosparDonneesSat_returnPressed();
    void on_satellitesTrouves_currentRowChanged(int currentRow);

};

#endif // RECHERCHESATELLITE_H
