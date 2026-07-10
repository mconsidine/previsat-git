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
 * >    22 aout 2024
 *
 */

#ifndef GENERAL_H
#define GENERAL_H

#include <QFrame>


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
#if (BUILD_TEST)
    friend class OsculateursTest;
#endif
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief General Constructeur par defaut
     * @param[in] flashs onglet flashs
     * @param[in] osculateurs onglet osculateurs
     * @param[in] parent parent
     * @throw Exception
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
    static Date dateEclipse();


    /*
     * Modificateurs
     */
    static void setAcalcAOS(const bool acalc);
    static void setAcalcDN(const bool acalc);


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

    /**
     * @brief SauveOngletGeneral Sauvegarde des donnees de l'onglet
     * @param[in] fichier nom du fichier
     */
    void SauveOngletGeneral(const QString &fichier);

    void changeEvent(QEvent *evt);


signals:

    void AfficherMessageStatut(const QString &message, const int secondes = -1);
    void EffacerMessageStatut();
    void ModificationDate(const QDateTime &date);
    void ModeManuel(const bool enable);
    void RecalculerPositions();


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
     * @param[in] date date
     */
    void AffichageDate(const Date &date);

    /**
     * @brief AffichageDonneesSatellite Affichage des donnees relatives au satellite par defaut
     * @param[in] date date
     */
    void AffichageDonneesSatellite(const Date &date);

    /**
     * @brief AffichageDonneesSoleilLune Affichage des donnees du Soleil et de la Lune
     */
    void AffichageDonneesSoleilLune();

    /**
     * @brief EcritureInformationsEclipse Ecriture des informations d'eclipse
     * @param[in] corpsOccultant corps occultant
     * @param[in] fractionIlluminee fraction illuminee
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
     * @param[in] date date
     */
    void AffichageVitesses(const Date &date, const bool enable = false);

    void mouseDoubleClickEvent(QMouseEvent *evt);

    void on_dateHeure2_dateTimeChanged(const QDateTime &dateTime);
    void on_lieuObservation_currentIndexChanged(int index);

    void on_soleilLuneSuiv_clicked();
    void on_soleilLunePrec_clicked();
    void on_stackedWidget_soleilLune_currentChanged(int arg1);

    void on_pause_clicked();
    void on_play_clicked();
    void on_rewind_clicked();
    void on_forward_clicked();
    void on_backward_clicked();

};

#endif // GENERAL_H
