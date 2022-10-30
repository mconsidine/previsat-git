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
 * >    28 decembre 2019
 *
 * Date de revision
 * >    30 octobre 2022
 *
 */

#ifndef ONGLETS_H
#define ONGLETS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QTabWidget>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class Onglets;
}

class General;
class Osculateurs;

class InformationsSatellite;
class RechercheSatellite;
class InformationsISS;

class CalculsPrevisions;
class CalculsFlashs;
class CalculsTransits;
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
     * @param parent parent
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

    CalculsPrevisions *previsions() const;
    CalculsTransits *transits() const;
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
    void show(const Date &date);


public slots:

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

    CalculsPrevisions *_previsions;
    CalculsFlashs *_flashs;
    CalculsTransits *_transits;
    CalculsEvenementsOrbitaux *_evenements;

#if defined (Q_OS_WIN)
    SuiviTelescope *_suiviTelescope;
#endif
    Antenne *_antenne;

    bool _acalcAOS;
    bool _acalcDN;
    bool _info;
    int _indexInfo;
    int _nbInformations;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageLieuObservation Affichage du lieu d'observation
     */
    void AffichageLieuObservation();

    /**
     * @brief Initialisation Initialisation de la classe Onglets
     */
    void Initialisation();


private slots:

    void on_infoPrec_clicked();
    void on_infoSuiv_clicked();

    void on_previsionPrec_clicked();
    void on_previsionSuiv_clicked();
    void on_stackedWidget_informations_currentChanged(int arg1);
    void on_stackedWidget_previsions_currentChanged(int arg1);

};

#endif // ONGLETS_H
