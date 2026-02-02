/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    calculsflashs.h
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Flashs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    26 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef CALCULSFLASHS_H
#define CALCULSFLASHS_H

#include <QFrame>


namespace Ui {
class CalculsFlashs;
}

class AfficherResultats;
class Date;


class CalculsFlashs : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief CalculsFlashs Constructeur par defaut
     * @param[in] parent parent
     */
    explicit CalculsFlashs(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CalculsFlashs();


    /*
     * Accesseurs
     */
    Ui::CalculsFlashs *ui() const;


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
     * @brief AffichageLieuObs Affichage des lieux d'observation dans la liste deroulante
     */
    void AffichageLieuObs();

    void changeEvent(QEvent *evt);


signals:

    void AfficherMessageStatut(const QString &, const int );
    void ChargementGP();
    void DemarrageApplication();


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
    Ui::CalculsFlashs *_ui;

    AfficherResultats *_afficherResultats;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculAgeElementsOrbitaux Calcul de l'age des elements orbitaux
     */
    void CalculAgeElementsOrbitaux();

    /**
     * @brief Initialisation Initialisation de la classe Flashs
     */
    void Initialisation();


private slots:

    void on_calculsFlashs_clicked();
    void on_parametrageDefautMetOp_clicked();
    void on_effacerHeuresMetOp_clicked();
    void on_hauteurSatMetOp_currentIndexChanged(int index);
    void on_hauteurSoleilMetOp_currentIndexChanged(int index);
    void on_majElementsOrbitaux_clicked();

};

#endif // CALCULSFLASHS_H
