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
 * >    telecharger.h
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
 * >    10 mars 2012
 *
 * Date de revision
 * >    3 octobre 2015
 *
 */

#ifndef TELECHARGER_H
#define TELECHARGER_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"


class Onglets;

namespace Ui {
    class Telecharger;
}

enum TypeTelechargement {
    TELECHARGEMENT_LISTE,
    TELECHARGEMENT_FICHIERS
};

class Telecharger : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Telecharger Constructeur pour la fenetre de telechargement
     * @param adresse indice du repertoire cible
     * @param fenetreParent fenetre
     */
    explicit Telecharger(const AdressesTelechargement &adresse, Onglets *onglets);

    /*
     * Destructeur
     */
    ~Telecharger();

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
     * @brief AjoutFichier Ajout d'un fichier dans la liste de fichiers a telecharger
     * @param url adresse du fichier
     */
    void AjoutFichier(const QUrl &url);


signals:

    /**
     * @brief TelechargementFini Signal du telechargement fini
     */
    void TelechargementFini();


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
    Ui::Telecharger *ui;
    Onglets *_onglets;
    AdressesTelechargement _adresse;
    TypeTelechargement _typeTelechargement;

    /*
     * Methodes privees
     */


private slots:

    /**
     * @brief on_fermer_clicked Fermeture de la fenetre
     */
    void on_fermer_clicked();

    /**
     * @brief on_interrogerServeur_clicked Interrogation du serveur
     */
    void on_interrogerServeur_clicked();


    void FinTelechargement();

    /**
     * @brief on_telecharger_clicked Lancement du telechargement
     */
    void on_telecharger_clicked();


};

#endif // TELECHARGER_H
