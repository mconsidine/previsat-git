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
 * >    calculsstarlink.h
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Previsions de passage des Starlink
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 septembre 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef CALCULSSTARLINK_H
#define CALCULSSTARLINK_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class CalculsStarlink;
}

class AfficherResultats;

class CalculsStarlink : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief CalculsPrevisions Constructeur par defaut
     * @param[in] parent parent
     */
    explicit CalculsStarlink(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CalculsStarlink();


    /*
     * Accesseurs
     */
    Ui::CalculsStarlink *ui() const;


    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief show Affichage des informations de l'onglet
     */
    void show();


public slots:

    /**
     * @brief AffichageLieuObs Affichage des lieux d'observation dans la liste deroulante
     */
    void AffichageLieuObs();

    void changeEvent(QEvent *evt);


signals:

    void AfficherMessageStatut(const QString &, const int );


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
    Ui::CalculsStarlink *_ui;

    AfficherResultats *_afficherResultats;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe CalculsStarlink
     */
    void Initialisation();


private slots:

    void on_groupe_currentTextChanged(const QString &arg1);
    void on_parametrageDefaut_clicked();
    void on_hauteurSoleil_currentIndexChanged(int index);
    void on_hauteurSat_currentIndexChanged(int index);

    void on_calculs_clicked();
};

#endif // CALCULSSTARLINK_H
