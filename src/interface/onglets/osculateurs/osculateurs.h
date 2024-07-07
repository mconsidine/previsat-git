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
 * >    osculateurs.h
 *
 * Localisation
 * >    interface.onglets.osculateurs
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Elements osculateurs
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

#ifndef OSCULATEURS_H
#define OSCULATEURS_H

#include <QFrame>


namespace Ui {
class Osculateurs;
}

class Date;


class Osculateurs : public QFrame
{
    Q_OBJECT
#if (BUILD_TEST)
    friend class OsculateursTest;
#endif

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Osculateurs Constructeur par defaut
     * @param[in] parent parent
     * @throw Exception
     */
    explicit Osculateurs(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Osculateurs();


    /*
     * Accesseurs
     */
    Ui::Osculateurs *ui() const;


    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief show Affichage des informations de l'onglet
     * @param[in] date date
     */
    void show(const Date &date);


public slots:

    /**
     * @brief SauveOngletElementsOsculateurs Sauvegarde des donnees de l'onglet
     * @param[in] fichier nom du fichier
     */
    void SauveOngletElementsOsculateurs(const QString &fichier);


signals:

    void AffichageVitesses(const Date &date, const bool enable = false);
    void ModeManuel(const bool enable);
    void ModificationDate(const QDateTime &date);


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
    Ui::Osculateurs *_ui;

    Date *_date;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageElementsOsculateurs Affichage des elements osculateurs
     */
    void AffichageElementsOsculateurs();

    /**
     * @brief AffichageVecteurEtat Affichage du vecteur d'etat
     * @param[in] date date
     */
    void AffichageVecteurEtat(const Date &date);

    /**
     * @brief Initialisation Initialisation de la classe Osculateurs
     */
    void Initialisation();


private slots:

    void mouseDoubleClickEvent(QMouseEvent *evt);

    void on_dateHeure2_dateTimeChanged(const QDateTime &dateTime);
    void on_typeRepere_currentIndexChanged(int index);
    void on_typeParametres_currentIndexChanged(int index);

};

#endif // OSCULATEURS_H
