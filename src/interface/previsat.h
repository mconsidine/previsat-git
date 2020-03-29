/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    previsat.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QMainWindow
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    2 mai 2019
 *
 */

#ifndef PREVISAT_H
#define PREVISAT_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QLabel>
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"
#include <QTranslator>


class Carte;
class Date;
class Onglets;
class QListWidgetItem;

namespace Ui {
class PreviSat;
}

class PreviSat : public QMainWindow
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief PreviSat Constructeur par defaut
     * @param parent parent
     */
    explicit PreviSat(QWidget *parent = nullptr);

    /*
     * Destructeur
     */
    ~PreviSat();


    /*
     * Accesseurs
     */

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /*******************
     * Initialisations *
     ******************/
    /**
     * @brief ChargementConfig Chargement de la configuration
     */
    void ChargementConfig();

    /**
     * @brief ChargementTLE Chargement du fichier TLE par defaut
     */
    void ChargementTLE();

    /**
     * @brief MAJTLE Mise a jour des TLE lors du demarrage
     */
    void MAJTLE();

    /**
     * @brief DemarrageApplication Demarrage de l'application apres le chargement de la configuration
     */
    void DemarrageApplication();


public slots:

    /**
     * @brief ChangementLangue Changement dynamique de la langue
     * @param index indice
     */
    void ChangementLangue(const int index);

    /**
     * @brief AfficherMessageStatut Affichage d'un message dans la zone de statut
     * @param message message
     * @param secondes nombre de secondes pendant lesquelles le message est affiche
     */
    void AfficherMessageStatut(const QString &message, const int secondes);


protected:

    /*
     * Constantes protegees
     */

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Constantes privees
     */

    /*
     * Variables privees
     */
    Ui::PreviSat *ui;

    Carte *_carte;
    Onglets *_onglets;

    // Barre de statut
    QLabel *_messageStatut;
    QLabel *_messageStatut2;
    QLabel *_messageStatut3;
    QLabel *_modeFonctionnement;
    QLabel *_stsDate;
    QLabel *_stsHeure;


    QTranslator _qtTraduction;
    QTranslator _appTraduction;

    Date *_dateCourante;
    QTimer *_chronometre;
    QTimer *_chronometreMs;


    /*
     * Methodes privees
     */
    /*******************
     * Initialisations *
     ******************/
    /**
     * @brief GestionPolice Gestion de la police
     */
    void GestionPolice();

    /**
     * @brief InitAffichageDemarrage Affichage au demarrage
     */
    void InitAffichageDemarrage() const;

    /**
     * @brief InitBarreStatut Initialisation de la barre de statut
     */
    void InitBarreStatut();

    /**
     * @brief InitChampsDefaut Initialisation des champs par defaut
     */
    void InitChampsDefaut();

    /**
     * @brief InitDate Initialisation de la date
     */
    void InitDate();

    /**
     * @brief InitFicTLE Liste des fichiers TLE
     */
    void InitFicTLE() const;

    /**
     * @brief InitMenus Initialisation des menus
     */
    void InitMenus() const;

    /**
     * @brief InstallationTraduction Installation de la traduction
     * @param langue langue
     * @param traduction traduction
     */
    void InstallationTraduction(const QString &langue, QTranslator &traduction);


    /*************
     * Affichage *
     ************/
    /**
     * @brief AfficherListeSatellites Afficher les noms des satellites dans les listes
     * @param nomfic nom du fichier TLE
     * @param majListesOnglets mise a jour des listes dans les onglets
     */
    void AfficherListeSatellites(const QString &nomfic, const bool majListesOnglets = true);


    /***********
     * Calculs *
     * ********/
    /**
     * @brief EnchainementCalculs Enchainement des calculs
     */
    void EnchainementCalculs();


private slots:

    void on_pasReel_currentIndexChanged(int index);

    void on_pasManuel_currentIndexChanged(int index);

    void mousePressEvent(QMouseEvent *evt);

    void on_actionA_propos_triggered();


    void on_liste1_itemClicked(QListWidgetItem *item);
    void on_liste1_itemEntered(QListWidgetItem *item);

    /**
     * @brief EffacerMessageStatut Effacer la zone de message de statut
     */
    void EffacerMessageStatut();

    void GestionTempsReel();

    void TempsReel();


};

#endif // PREVISAT_H