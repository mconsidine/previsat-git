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
 * >    suivitelescope.h
 *
 * Localisation
 * >    interface.onglets.telescope
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Suivi avec un telescope
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

#ifndef SUIVITELESCOPE_H
#define SUIVITELESCOPE_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFrame>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/maths/maths.h"


namespace Ui {
class SuiviTelescope;
}

class QListWidget;
class QListWidgetItem;
class AfficherResultats;
class Date;
class Observateur;
class Satellite;


class SuiviTelescope : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief SuiviTelescope Constructeur par defaut
     * @param[in] parent parent
     */
    explicit SuiviTelescope(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~SuiviTelescope();


    /*
     * Accesseurs
     */
    Ui::SuiviTelescope *ui();

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief CalculAosSatSuivi Calcul des informations AOS/LOS pour le suivi d'un satellite
     * @param[in] date date
     */
    void CalculAosSatSuivi(const Date &date);

    int getListItemChecked(const QListWidget * const liste) const;


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

    void AfficherMessageStatut(const QString &message, const int secondes = -1);


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
    Ui::SuiviTelescope *_ui;
    AfficherResultats *_afficherResultats;
    Date *_date;
    QString _ficSuivi;
    QString _norad;

    Date *_dateAos;
    Date *_dateAosSuivi;
    Date *_dateLos;
    Date *_dateLosSuivi;


    /*
     * Methodes privees
     */
    /**
     * @brief CalculAos Calcul des informations AOS/LOS
     */
    void CalculAos();

    /**
     * @brief CalculHauteurMax Calcul de la hauteur maximale d'un satellite dans le ciel
     * @param[in] jjm dates
     * @param[in/out] obs observateur
     * @param[in/out] satSuivi satellite suivi
     * @return jour et hauteur maximale
     */
    QPointF CalculHauteurMax(const std::array<double, MATHS::DEGRE_INTERPOLATION> &jjm, const Observateur &obs, Satellite &satSuivi) const;

    /**
     * @brief Initialisation Initialisation de la classe SuiviTelescope
     */
    void Initialisation();


private slots:

    /**
     * @brief AjusterDates Ajuster les dates initiale et finale
     * @param[in] date1 date initiale
     * @param[in] date2 date finale
     */
    void AjusterDates(const QDateTime &date1, const QDateTime &date2);

    void on_genererPositions_clicked();
    void on_filtreSatellites_textChanged(const QString &arg1);
    void on_filtreSatellites_returnPressed();
    void on_satellitesChoisis_toggled(bool checked);
    void on_parametrageDefautSuivi_clicked();
    void on_afficherSuivi_clicked();
    void on_listeTelescope_itemClicked(QListWidgetItem *item);
    void on_listeTelescope_currentRowChanged(int currentRow);
    void on_lieuxObservation_currentIndexChanged(int index);
    void on_hauteurSatSuivi_currentIndexChanged(int index);
    void on_ajusterDates_clicked();
    void on_skywatcher_clicked();
    void on_ouvrirSatelliteTracker_clicked();
    void on_pecDelai_toggled(bool checked);

};

#endif // SUIVITELESCOPE_H
