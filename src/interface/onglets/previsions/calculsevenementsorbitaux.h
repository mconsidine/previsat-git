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
 * >    calculsevenementsorbitaux.h
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Evenements orbitaux
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

#ifndef CALCULSEVENEMENTSORBITAUX_H
#define CALCULSEVENEMENTSORBITAUX_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class CalculsEvenementsOrbitaux;
}

class QListWidgetItem;
class AfficherResultats;


class CalculsEvenementsOrbitaux : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief CalculsEvenementsOrbitaux Constructeur par defaut
     * @param parent parent
     */
    explicit CalculsEvenementsOrbitaux(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CalculsEvenementsOrbitaux();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */

public slots:

    /**
     * @brief AfficherListeSatellites Affichage des satellites dans la liste
     * @param nomsat nom du satellite
     * @param norad numero NORAD
     * @param noradDefaut numero NORAD du satellite par defaut
     * @param tooltip tooltip a afficher
     * @param check vrai si le satellite doit etre coche
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
    Ui::CalculsEvenementsOrbitaux *_ui;

    AfficherResultats *_afficherResultats;

    QAction *_aucun;
    QAction *_tous;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe CalculsEvenementsOrbitaux
     */
    void Initialisation();


private slots:

    void Aucun();
    void Tous();

    void on_calculsEvt_clicked();
    void on_filtreSatellites_textChanged(const QString &arg1);
    void on_filtreSatellites_returnPressed();
    void on_parametrageDefautEvt_clicked();
    void on_effacerHeuresEvt_clicked();
    void on_listeEvenements_customContextMenuRequested(const QPoint &pos);

};

#endif // CALCULSEVENEMENTSORBITAUX_H
