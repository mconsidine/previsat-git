/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    17 decembre 2013
 *
 */

#ifndef AFFICHER_H
#define AFFICHER_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QMainWindow>

namespace Ui {
    class Afficher;
}

class Afficher : public QMainWindow
{
    Q_OBJECT

public:
    explicit Afficher(QWidget *fenetreParent = 0);
    void show(const QString &fic);
    ~Afficher();

private:
    Ui::Afficher *ui;
    QString _fichier;

private slots:
    void closeEvent(QCloseEvent *evt);
    void resizeEvent(QResizeEvent * evt);
    void on_actionEnregistrer_activated();
};

#endif // AFFICHER_H
