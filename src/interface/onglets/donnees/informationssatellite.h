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
 * >    informationssatellite.h
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Informations satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
 *
 * Date de revision
 * >     27 aout 2022
 *
 */

#ifndef INFORMATIONSSATELLITE_H
#define INFORMATIONSSATELLITE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class InformationsSatellite;
}

class Observateur;
class Satellite;

class InformationsSatellite : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief InformationsSatellite Constructeur par defaut
     * @param[in] parent parent
     */
    explicit InformationsSatellite(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~InformationsSatellite();


    /*
     * Accesseurs
     */
    Ui::InformationsSatellite *ui() const;


    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    void show(const Satellite &satellite);


public slots:

    /**
     * @brief SauveOngletInformations Sauvegarde des donnees de l'onglet
     * @param[in] fichier nom du fichier
     */
    void SauveOngletInformations(const QString &fichier);

    void changeEvent(QEvent *evt);


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
    Ui::InformationsSatellite *_ui;


    /*
     * Methodes privees
     */
    bool eventFilter(QObject *watched, QEvent *event);

};

#endif // INFORMATIONSSATELLITE_H
