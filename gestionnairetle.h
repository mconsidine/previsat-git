/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#ifndef GESTIONNAIRETLE_H
#define GESTIONNAIRETLE_H

#include <QMainWindow>

namespace Ui {
    class GestionnaireTLE;
}

class GestionnaireTLE : public QMainWindow
{
    Q_OBJECT

public:
    explicit GestionnaireTLE(QWidget *parent = 0);
    ~GestionnaireTLE();

private slots:
    void on_fermer_clicked();
    void on_actionCreer_un_groupe_activated();
    void on_actionSupprimerGroupe_activated();

    void on_listeGroupeTLE_customContextMenuRequested(const QPoint &pos);

    void on_listeGroupeTLE_currentRowChanged(int currentRow);

    void on_valider_clicked();

    void on_annuler_clicked();

    void on_actionAjouter_des_fichiers_activated();

    void on_actionSupprimer_activated();

    void on_listeFichiersTLE_customContextMenuRequested(const QPoint &pos);

    void on_MajAutoGroupe_toggled(bool checked);

    void on_MajMaintenant_clicked();

private:
    Ui::GestionnaireTLE *ui;
    void load();
};

#endif // GESTIONNAIRETLE_H
