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
 * >    general.h
 *
 * Localisation
 * >    interface.onglets.general
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet General
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >
 *
 */

#ifndef GENERAL_H
#define GENERAL_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class General;
}

class CalculsFlashs;
class Date;
class Osculateurs;
struct ElementsAOS;


class General : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief General Constructeur par defaut
     * @param parent parent
     */
    explicit General(CalculsFlashs *flashs, Osculateurs *osculateurs, QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~General();


    /*
     * Accesseurs
     */
    Ui::General *ui() const;
    ElementsAOS *elementsAOS() const;


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
     * @brief SauveOngletGeneral Sauvegarde des donnees de l'onglet
     * @param fichier nom du fichier
     */
    void SauveOngletGeneral(const QString &fichier);

    void changeEvent(QEvent *evt);


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
    Ui::General *_ui;
    CalculsFlashs *_flashs;
    Osculateurs *_osculateurs;

    bool _uniteVitesse;
    unsigned int _indexLuneSoleil;

    static bool _acalcDN;
    static bool _isEclipse;
    static Date *_dateEclipse;

    static bool _acalcAOS;
    static double _htSat;
    static ElementsAOS *_elementsAOS;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageDate Affichage de la date
     * @param date date
     */
    void AffichageDate(const Date &date);

    /**
     * @brief AffichageDonneesSatellite Affichage des donnees relatives au satellite par defaut
     * @param date date
     */
    void AffichageDonneesSatellite(const Date &date);

    /**
     * @brief AffichageDonneesSoleilLune Affichage des donnees du Soleil et de la Lune
     */
    void AffichageDonneesSoleilLune();

    /**
     * @brief EcritureInformationsEclipse Ecriture des informations d'eclipse
     * @param corpsOccultant corps occultant
     * @param fractionIlluminee fraction illuminee
     */
    void EcritureInformationsEclipse(const QString &corpsOccultant, const double fractionIlluminee) const;

    /**
     * @brief Initialisation Initialisation de la classe General
     */
    void Initialisation();

    /**
     * @brief JouerSonFlash Execute la notification sonore d'un flash
     */
    void JouerSonFlash();


private slots:

    /**
     * @brief AffichageVitesses Affichage des vitesses (par seconde ou par heure)
     * @param date date
     */
    void AffichageVitesses(const Date &date);

    void on_soleilLuneSuiv_clicked();
    void on_soleilLunePrec_clicked();
    void on_stackedWidget_soleilLune_currentChanged(int arg1);
};

#endif // GENERAL_H
