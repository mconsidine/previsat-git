/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    onglets.h
 *
 * Localisation
 * >    interface.onglets
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Barre d'onglets
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    25 fevrier 2023
 *
 * Date de revision
 * >    18 juillet 2024
 *
 */

#ifndef ONGLETS_H
#define ONGLETS_H

#include <QTabWidget>


namespace Ui {
class Onglets;
}

class General;
class Osculateurs;

class InformationsSatellite;
class RechercheSatellite;
class InformationsISS;
class InformationsLancements;
class InformationsRentrees;

class CalculsPrevisions;
class CalculsFlashs;
class CalculsTransits;
class CalculsStarlink;
class CalculsEvenementsOrbitaux;

class SuiviTelescope;
class Antenne;
class Date;


class Onglets : public QTabWidget
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Onglets Constructeur par defaut
     * @param[in] parent parent
     * @throw Exception
     */
    explicit Onglets(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Onglets();


    /*
     * Accesseurs
     */
    Ui::Onglets *ui();

    General *general() const;
    Osculateurs *osculateurs() const;

    InformationsSatellite *informationsSatellite() const;
    InformationsISS *informationsISS() const;
    RechercheSatellite *rechercheSatellite() const;
    InformationsLancements *informationsLancements() const;
    InformationsRentrees *informationsRentrees() const;

    CalculsPrevisions *previsions() const;
    CalculsFlashs *flashs() const;
    CalculsTransits *transits() const;
    CalculsStarlink *starlink() const;
    CalculsEvenementsOrbitaux *evenements() const;
#if defined (Q_OS_WIN)
    SuiviTelescope *suiviTelescope() const;
#endif
    Antenne *antenne() const;


    /*
     * Modificateurs
     */
    void setAcalcAOS(const bool acalc);
    void setAcalcDN(const bool acalc);
    void setIndexInformations(const unsigned int newIndexInformations);
    void setIndexPrevisions(const unsigned int newIndexPrevisions);
    void setInfo(const bool info);


    /*
     * Methodes publiques
     */
    /**
     * @brief show Affichage des informations pour les onglets
     * @param[in] date date
     */
    void show(const Date &date);


public slots:

    /**
     * @brief ReinitFlags Reinitialisation des flags d'affichage
     */
    void ReinitFlags();

    void changeEvent(QEvent *evt);


signals:

    void AffichageLieuObs();


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
    unsigned int _indexInformations;
    unsigned int _indexPrevisions;
    Ui::Onglets *_ui;

    // Onglets
    General *_general;
    Osculateurs *_osculateurs;

    InformationsSatellite *_informationsSatellite;
    RechercheSatellite *_rechercheSatellite;
    InformationsISS *_informationsISS;
    InformationsLancements *_informationsLancements;
    InformationsRentrees *_informationsRentrees;

    CalculsPrevisions *_previsions;
    CalculsFlashs *_flashs;
    CalculsTransits *_transits;
    CalculsStarlink *_starlink;
    CalculsEvenementsOrbitaux *_evenements;

#if defined (Q_OS_WIN)
    SuiviTelescope *_suiviTelescope;
#endif
    Antenne *_antenne;
    bool _info;
    bool _isLancements;
    bool _isRentrees;
    bool _isStarlink;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageLieuObservation Affichage du lieu d'observation
     */
    void AffichageLieuObservation();

    /**
     * @brief AffichageOngletInformations Gestion de l'affichage de l'onglet Informations
     */
    void AffichageOngletInformations();

    /**
     * @brief Initialisation Initialisation de la classe Onglets
     */
    void Initialisation();


private slots:

    void on_Onglets_currentChanged(int index);

    void on_infoPrec_clicked();
    void on_infoSuiv_clicked();

    void on_previsionPrec_clicked();
    void on_previsionSuiv_clicked();
    void on_stackedWidget_previsions_currentChanged(int arg1);

};

#endif // ONGLETS_H
