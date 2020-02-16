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
 * >    gestionnairetle.h
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

#ifndef GESTIONNAIRETLE_H
#define GESTIONNAIRETLE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QMainWindow>
#include <QtNetwork>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wpacked"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"

namespace Ui {
    class GestionnaireTLE;
}

class GestionnaireTLE : public QMainWindow
{
    Q_OBJECT

public:

    /* Constructeurs */
    explicit GestionnaireTLE(QWidget *fenetreParent = 0);
    ~GestionnaireTLE();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */

    /* Accesseurs */


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private slots:

    void closeEvent(QCloseEvent *evt);

    /**
     * @brief on_fermer_clicked Fermeture de la fenetre
     */
    void on_fermer_clicked();

    /**
     * @brief on_actionCreer_un_groupe_triggered Creation d'un groupe de TLE
     */
    void on_actionCreer_un_groupe_triggered();

    /**
     * @brief on_actionSupprimerGroupe_triggered Suppression d'un groupe de TLE
     */
    void on_actionSupprimerGroupe_triggered();

    /**
     * @brief on_listeGroupeTLE_customContextMenuRequested Affichage du menu contextuel pour les groupes de TLE
     * @param position position du curseur
     */
    void on_listeGroupeTLE_customContextMenuRequested(const QPoint &position);

    /**
     * @brief on_listeGroupeTLE_currentRowChanged Selection d'un groupe de TLE
     * @param currentRow indice du groupe
     */
    void on_listeGroupeTLE_currentRowChanged(int currentRow);

    /**
     * @brief on_valider_clicked Validation de la creation d'un groupe de TLE
     */
    void on_valider_clicked();

    /**
     * @brief on_annuler_clicked Annulation de la creation d'un groupe de TLE
     */
    void on_annuler_clicked();

    /**
     * @brief on_actionAjouter_des_fichiers_triggered Ajout de fichiers dans le groupe de TLE
     */
    void on_actionAjouter_des_fichiers_triggered();

    /**
     * @brief on_actionSupprimer_triggered Suppression d'un fichier dans le groupe de TLE
     */
    void on_actionSupprimer_triggered();

    /**
     * @brief on_listeFichiersTLE_customContextMenuRequested Menu contextuel de la liste de fichiers TLE
     * @param position position du curseur
     */
    void on_listeFichiersTLE_customContextMenuRequested(const QPoint &position);

    /**
     * @brief on_MajAutoGroupe_toggled Selection de la mise a jour automatique du groupe de TLE
     * @param checked Mise a jour si coche
     */
    void on_MajAutoGroupe_toggled(bool checked);

    /**
     * @brief on_creationGroupe_clicked Creation d'un groupe de TLE
     */
    void on_creationGroupe_clicked();

    /**
     * @brief on_ajoutFichiersTLE_clicked Ajout de fichiers TLE
     */
    void on_ajoutFichiersTLE_clicked();


private:

    /* Constantes privees */

    /* Variables privees */
    Ui::GestionnaireTLE *ui;

    /* Methodes privees */
    /**
     * @brief load Chargement des elements dans la fenetre
     */
    void load();
};

#endif // GESTIONNAIRETLE_H
