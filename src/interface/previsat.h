/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Description
 * >    Fenetre principale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    10 octobre 2021
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
class Ciel;
class Date;
class Onglets;
class Radar;
class QListWidgetItem;
class QToolButton;

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
     * @brief MAJTLE Mise a jour des TLE lors du demarrage
     */
    void MAJTLE();

    /**
     * @brief DemarrageApplication Demarrage de l'application apres le chargement de la configuration
     */
    void DemarrageApplication();


public slots:


    /**
     * @brief ChargementTLE Chargement du fichier TLE par defaut
     */
    void ChargementTLE();


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

    // Elements de la fenetre
    Carte *_carte;
    Ciel *_ciel;
    Onglets *_onglets;
    Radar *_radar;
    QToolButton* _maximise;
    QToolButton* _affichageCiel;

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
    QTimer *_timerStatut;

    QString _majInfosDate;

    /*
     * Methodes privees
     */
    /*******************
     * Initialisations *
     ******************/
    /**
     * @brief ChargementTraduction Chargement de la traduction
     * @param langue langue
     */
    void ChargementTraduction(const QString &langue);

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
     * @brief InitMenus Initialisation des menus
     */
    void InitMenus() const;

    /**
     * @brief InstallationTraduction Installation de la traduction
     * @param langue langue
     * @param traduction traduction
     */
    void InstallationTraduction(const QString &langue, QTranslator &traduction);


    /***********
     * Calculs *
     * ********/
    /**
     * @brief EnchainementCalculs Enchainement des calculs
     */
    void EnchainementCalculs();

    /**
     * @brief MajFichierTLE Mise a jour du fichier TLE courant
     */
    void MajFichierTLE();

    /**
     * @brief MettreAJourGroupeTLE Mettre a jour un groupe de TLE
     * @param groupe nom du groupe
     */
    void MettreAJourGroupeTLE(const QString &groupe);

    /**
     * @brief OuvertureFichierTLE Ouverture d'un fichier TLE
     * @param fichier nom du fichier
     */
    void OuvertureFichierTLE(const QString &fichier);

private slots:

    /**
     * @brief ChangementCarte Mise a jour de l'affichage suite a un changement de carte
     */
    void ChangementCarte();

    /**
     * @brief ChangementDate Mise a jour de l'affichage suite a un changement de date
     * @param date date
     */
    void ChangementDate(const QDateTime &date);

    /**
     * @brief ChangementLangue Changement dynamique de la langue
     * @param index indice
     */
    void ChangementLangue(const int index);

    /**
     * @brief ChangementZoom Mise a jour de l'affichage suite a un changement de zoom
     */
    void ChangementZoom();

    /**
     * @brief InitFicTLE Liste des fichiers TLE
     */
    void InitFicTLE() const;


    /*************
     * Affichage *
     ************/
    /**
     * @brief AfficherListeSatellites Afficher les noms des satellites dans les listes
     * @param nomfic nom du fichier TLE
     * @param majListesOnglets mise a jour des listes dans les onglets
     */
    void AfficherListeSatellites(const QString &nomfic, const bool majListesOnglets = true);

    /**
     * @brief AfficherMessageStatut Affichage d'un message dans la zone de statut
     * @param message message
     * @param secondes nombre de secondes pendant lesquelles le message est affiche
     */
    void AfficherMessageStatut(const QString &message, const int secondes = -1);

    /**
     * @brief ChargementFenetre Chargement des elements de la fenetre
     */
    void ChargementFenetre();

    /**
     * @brief EffacerMessageStatut Effacer la zone de message de statut
     */
    void EffacerMessageStatut();

    /**
     * @brief GestionTempsReel Gestion du temps reel
     */
    void GestionTempsReel();

    /**
     * @brief TempsReel Affichage en temps reel
     */
    void TempsReel();


    void mousePressEvent(QMouseEvent *evt);
    void resizeEvent(QResizeEvent *evt);

    void on_directHelp_clicked();
    void on_actionPaypal_triggered();
    void on_actionTipeee_triggered();
    void on_meteoBasesNASA_clicked();
    void on_meteo_clicked();

    // Menu deroulant
    void on_actionOuvrir_fichier_TLE_triggered();
    void on_actionEnregistrer_triggered();
    void on_actionImprimer_carte_triggered();
    void on_actionMettre_jour_TLE_courant_triggered();
    void on_actionMettre_jour_groupe_TLE_triggered();
    void on_actionMettre_jour_TLE_communs_triggered();
    void on_actionMettre_jour_tous_les_groupes_de_TLE_triggered();
    void on_actionMettre_jour_les_fichiers_de_donnees_triggered();
    void on_actionFichier_d_aide_triggered();
    void on_actionInformations_triggered();
    void on_actionFaire_triggered();
    void on_actionDonation_Tipeee_triggered();
    void on_actionPrevisat_sourceforge_net_triggered();
    void on_actionRapport_de_bug_triggered();
    void on_actionSkywatcher_com_triggered();
    void on_actionWww_celestrak_com_triggered();
    void on_actionWww_space_track_org_triggered();
    void on_actionA_propos_triggered();

    void on_tempsReel_toggled(bool checked);
    void on_modeManuel_toggled(bool checked);
    void on_pasReel_currentIndexChanged(int index);
    void on_pasManuel_currentIndexChanged(int index);

    void on_listeFichiersTLE_currentIndexChanged(int index);

    void on_liste1_itemClicked(QListWidgetItem *item);
    void on_liste1_itemEntered(QListWidgetItem *item);

};

#endif // PREVISAT_H
