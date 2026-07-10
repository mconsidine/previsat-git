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
 * >    calculstransits.h
 *
 * Localisation
 * >    interface.onglets.previsions
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet CalculsTransits
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

#ifndef CALCULSTRANSITS_H
#define CALCULSTRANSITS_H

#include <QFrame>


namespace Ui {
class CalculsTransits;
}

class QListWidgetItem;
class AfficherResultats;
class Date;
class ElementsOrbitaux;


class CalculsTransits : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief CalculsTransits Constructeur par defaut
     * @param[in] parent parent
     * @throw Exception
     */
    explicit CalculsTransits(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~CalculsTransits();


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

    /**
     * @brief show Affichage des informations de l'onglet
     * @param[in] date date
     */
    void show(const Date &date);


signals:

    void AfficherMessageStatut(const QString &message, const int secondes = -1);
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
    Ui::CalculsTransits *_ui;

    AfficherResultats *_afficherResultats;

    QAction *_aucun;
    QAction *_tous;

    bool _etatElementsIss;
    QPalette _paletteBouton;
    QList<ElementsOrbitaux> _listeElemIss;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculAgeElementsOrbitaux Calcul de l'age des elements orbitaux
     */
    void CalculAgeElementsOrbitaux();

    /**
     * @brief Initialisation Initialisation de la classe CalculsTransits
     */
    void Initialisation();


private slots:

    void Aucun();
    void Tous();

    void on_calculsTransit_clicked();
    void on_filtreSatellites_textChanged(const QString &arg1);
    void on_filtreSatellites_returnPressed();
    void on_satellitesChoisis_toggled(bool checked);
    void on_parametrageDefautTransit_clicked();
    void on_effacerHeuresTransit_clicked();
    void on_hauteurSatTransit_currentIndexChanged(int index);
    void on_majElementsOrbitauxIss_clicked();
    void on_listeTransits_customContextMenuRequested(const QPoint &pos);
    void on_majElementsOrbitaux_clicked();

};

#endif // CALCULSTRANSITS_H
