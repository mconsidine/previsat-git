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
 * >    previsat.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre principale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    8 mai 2022
 *
 */

#ifndef PREVISAT_H
#define PREVISAT_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include <QTranslator>


QT_BEGIN_NAMESPACE
namespace Ui { class PreviSat; }
QT_END_NAMESPACE

class Onglets;

class PreviSat : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief PreviSat Constructeur par defaut
     * @param parent parent
     */
    PreviSat(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~PreviSat();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */


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
    QTranslator _qtTraduction;
    QTranslator _appTraduction;

    Ui::PreviSat *_ui;

    Onglets *_onglets;


    /*
     * Methodes privees
     */
    /**
     * @brief ChargementTraduction Chargement de la traduction
     * @param langue langue (fr, en, ja)
     */
    void ChargementTraduction(const QString &langue);

    /**
     * @brief Initialisation Initialisation de la fenetre principale
     */
    void Initialisation();

    /**
     * @brief InstallationTraduction Installation de la traduction
     * @param langue langue
     * @param traduction traduction
     */
    void InstallationTraduction(const QString &langue, QTranslator &traduction);


private slots:

    void on_actionRaccourci_Previsions_triggered();
    void on_actionRaccourci_Flashs_triggered();
    void on_actionRaccourci_Transits_triggered();
    void on_actionRaccourci_Evenements_triggered();
    void on_actionRaccourci_Informations_triggered();
    void on_actionRaccourci_Recherche_triggered();
    void on_actionRaccourci_Station_triggered();


};
#endif // PREVISAT_H
