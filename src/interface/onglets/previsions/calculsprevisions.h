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
 * >    calculsprevisions.h
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Previsions de passage
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

#ifndef CALCULSPREVISIONS_H
#define CALCULSPREVISIONS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class CalculsPrevisions;
}

class QListWidgetItem;
class AfficherResultats;
class Date;


class CalculsPrevisions : public QFrame
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
    explicit CalculsPrevisions(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CalculsPrevisions();


    /*
     * Accesseurs
     */
    Ui::CalculsPrevisions *ui() const;


    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    void show(const Date &date);


public slots:

    /**
     * @brief AffichageLieuObs Affichage des lieux d'observation dans la liste deroulante
     */
    void AffichageLieuObs();

    /**
     * @brief AfficherListeSatellites Affichage des satellites dans la liste
     * @param[in] nomsat nom du satellite
     * @param[in] norad numero NORAD
     * @param[in] noradDefaut numero NORAD du satellite par defaut
     * @param[in] tooltip tooltip a afficher
     * @param[in] check vrai si le satellite doit etre coche
     */
    void AfficherListeSatellites(const QString &nomsat, const QString &norad, const QString &noradDefaut, const QString &tooltip, const bool check);

    /**
     * @brief InitAffichageListeSatellites Initialisation de l'affichage de la liste
     */
    void InitAffichageListeSatellites();

    /**
     * @brief TriAffichageListeSatellites Tri dans l'affichage des satellites
     */
    void TriAffichageListeSatellites();

    void changeEvent(QEvent *evt);


signals:

    void AfficherMessageStatut(const QString &, const int );
    void MajFichierGP();


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
    Ui::CalculsPrevisions *_ui;

    AfficherResultats *_afficherResultats;

    QAction *_aucun;
    QAction *_tous;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculAgeElementsOrbitaux Calcul de l'age des elements orbitaux
     */
    void CalculAgeElementsOrbitaux();

    /**
     * @brief Initialisation Initialisation de la classe CalculsPrevisions
     */
    void Initialisation();


private slots:

    void Aucun();
    void Tous();

    void closeEvent(QCloseEvent *evt);
    void on_calculsPrev_clicked();
    void on_filtreSatellites_textChanged(const QString &arg1);
    void on_filtreSatellites_returnPressed();
    void on_parametrageDefautPrev_clicked();
    void on_listePrevisions_customContextMenuRequested(const QPoint &pos);
    void on_effacerHeuresPrev_clicked();
    void on_hauteurSatPrev_currentIndexChanged(int index);
    void on_hauteurSoleilPrev_currentIndexChanged(int index);
    void on_magnitudeMaxPrev_toggled(bool checked);
    void on_majElementsOrbitaux_clicked();

};

#endif // CALCULSPREVISIONS_H
