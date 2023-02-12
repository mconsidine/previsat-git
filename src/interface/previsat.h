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
 * >    12 fevrier 2023
 *
 */

#ifndef PREVISAT_H
#define PREVISAT_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QMainWindow>
#include <QTranslator>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include <QDate>


QT_BEGIN_NAMESPACE
namespace Ui { class PreviSat; }
QT_END_NAMESPACE


class QLabel;
class QListWidgetItem;
class Carte;
class Ciel;
class Date;
class Informations;
class Logging;
class Onglets;
class Options;
class Outils;
class Radar;

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
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief MajGP Mise a jour des elements orbitaux lors du demarrage
     */
    void MajGP();

    /**
     * @brief DemarrageApplication Demarrage de l'application apres le chargement de la configuration
     */
    void DemarrageApplication();


public slots:

    /**
     * @brief ChargementGP Chargement du fichier d'elements orbitaux par defaut
     */
    void ChargementGP();


signals:

    void AffichageListeSatellites(const QString &, const QString &, const QString &, const QString &, const bool);
    void DeconnecterUdp();
    void InitAffichageListeSatellites();
    void SauveOngletGeneral(const QString &);
    void SauveOngletElementsOsculateurs(const QString &);
    void SauveOngletInformations(const QString &);
    void SauveOngletRecherche(const QString &);
    void TriAffichageListeSatellites();


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
    QTranslator _qtTraduction;
    QTranslator _appTraduction;

    QString _majInfosDate;

    Ui::PreviSat *_ui;

    Carte *_carte;
    Ciel *_ciel;
    Onglets *_onglets;
    Radar *_radar;
    Informations *_informations;
    Options *_options;
    Outils *_outils;
    Logging *_logging;

    QAction *_previsions;
    QAction *_flashs;
    QAction *_transits;
    QAction *_evenements;
    QAction *_informationsSatellite;
    QAction *_recherche;
    QAction *_station;

    // Barre de statut
    QLabel *_messageStatut;
    QLabel *_messageStatut2;
    QLabel *_messageStatut3;
    QLabel *_modeFonctionnement;
    QLabel *_stsDate;
    QLabel *_stsHeure;

    QTimer *_chronometre;
    QTimer *_chronometreMs;
    QTimer *_timerStatut;

    Date *_dateCourante;

    bool _isCarteMonde;


    /*
     * Methodes privees
     */
    /**
     * @brief ChargementTraduction Chargement de la traduction
     * @param langue langue (fr, en, ja)
     */
    void ChargementTraduction(const QString &langue);

    /**
     * @brief ConnexionsSignauxSlots Connexions entre les differents elements de l'interface
     */
    void ConnexionsSignauxSlots();

    /**
     * @brief CreationMenus Creation des menus
     */
    void CreationMenus();

    /**
     * @brief CreationRaccourcis Creation des raccourcis clavier
     */
    void CreationRaccourcis();

    /**
     * @brief EnchainementCalculs Enchainement des calculs (satellites, Soleil, Lune, planetes, etoiles)
     */
    void EnchainementCalculs();

    /**
     * @brief GestionPolice Gestion de la police
     */
    void GestionPolice();

    /**
     * @brief Initialisation Initialisation de la fenetre principale
     */
    void Initialisation();

    /**
     * @brief InitBarreStatut Initialisation de la barre de statut
     */
    void InitBarreStatut();

    /**
     * @brief InitDate Initialisation de la date
     */
    void InitDate();

    /**
     * @brief InitVerificationsMAJ Verification des mises a jour au demarrage
     */
    void InitVerificationsMAJ();

    /**
     * @brief InstallationTraduction Installation de la traduction
     * @param langue langue
     * @param traduction traduction
     */
    void InstallationTraduction(const QString &langue, QTranslator &traduction);

    /**
     * @brief MajFichierGP Mise a jour du fichier GP courant
     */
    void MajFichierGP();

    /**
     * @brief MajWebGP Mise a jour automatique des elements orbitaux
     */
    void MajWebGP();

    /**
     * @brief VerifAgeGP Verification de l'age des elements orbitaux d'un satellite
     */
    void VerifAgeGP();

    /**
     * @brief VerifMajDate Verification d'une mise a jour a partir d'une date
     * @param fichier nom du fichier contenant la date
     * @param listeFichierMaj liste des fichiers a mettre a jour
     * @param dateMaj date de reference
     * @return vrai si la date contenue dans le fichier est plus recente
     */
    bool VerifMajDate(const QString &fichier, const QStringList &listeFichierMaj = QStringList(), const QDate &dateMaj = QDate());

    /**
     * @brief VerifMajPreviSat Verification des mises a jour (logiciel, fichiers internes)
     */
    void VerifMajPreviSat();

    /**
     * @brief VerifMajVersion Verification d'une mise a jour a partir d'un fichier de version
     * @param fichier nom du fichier de version
     * @return vrai si le numero de version du fichier est plus recent
     */
    bool VerifMajVersion(const QString &fichier);


private slots:

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
     * @brief AfficherMessageStatut2 Affichage d'un message dans la zone de statut 2
     * @param message message
     */
    void AfficherMessageStatut2(const QString &message);

    /**
     * @brief AfficherMessageStatut3 Affichage d'un message dans la zone de statut 3
     * @param message message
     */
    void AfficherMessageStatut3(const QString &message);

    /**
     * @brief EffacerMessageStatut Effacer la zone de message de statut
     */
    void EffacerMessageStatut();

    /**
     * @brief GestionTempsReel Gestion du temps reel
     */
    void GestionTempsReel();

    /**
     * @brief InitFicGP Liste des fichiers d'elements orbitaux
     */
    void InitFicGP();

    /**
     * @brief MettreAjourGroupeElem Mise a jour d'un groupe d'elements orbitaux
     * @param groupe nom du groupe
     */
    void MettreAjourGroupeElem(const QString &groupe);

    // Raccourcis vers les fonctionnalites
    void RaccourciPrevisions();
    void RaccourciFlashs();
    void RaccourciTransits();
    void RaccourciEvenements();
    void RaccourciInformations();
    void RaccourciRecherche();
    void RaccourciStation();

    /**
     * @brief TempsReel Affichage en temps reel
     */
    void TempsReel();

    void closeEvent(QCloseEvent *evt);
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *evt);

    // Boutons de l'interface graphique
    void on_configuration_clicked();
    void on_outils_clicked();
    void on_aide_clicked();

    void on_tempsReel_toggled(bool checked);
    void on_modeManuel_toggled(bool checked);

    // Menu deroulant
    void on_actionImporter_fichier_TLE_GP_triggered();
    void on_actionEnregistrer_triggered();
    void on_actionImprimer_carte_triggered();

    void on_actionFichier_d_aide_triggered();
    void on_actionInformations_triggered();
    void on_actionOptions_triggered();
    void on_actionOutils_triggered();

    void on_actionMettre_a_jour_GP_courant_triggered();
    void on_actionMettre_a_jour_GP_communs_triggered();
    void on_actionMettre_a_jour_tous_les_groupes_de_GP_triggered();

    void on_actionTelecharger_la_mise_a_jour_triggered();
    void on_actionMettre_a_jour_les_fichiers_de_donnees_triggered();
    void on_actionExporter_fichier_log_triggered();

    void on_actionPayPal_triggered();
    void on_actionTipeee_triggered();
    void on_actionUtip_triggered();

    void on_actionSkywatcher_triggered();
    void on_actionPianetaRadio_triggered();

    void on_actionCelestrak_triggered();
    void on_actionSpaceTrack_triggered();

    void on_actionContact_triggered();
    void on_actionApropos_triggered();

    // Filtre sur les satellites
    void on_filtreSatellites_textChanged(const QString &arg1);
    void on_filtreSatellites_returnPressed();

    void on_listeSatellites_itemClicked(QListWidgetItem *item);
    void on_listeSatellites_itemEntered(QListWidgetItem *item);
    void on_listeSatellites_customContextMenuRequested(const QPoint &pos);
    void on_actionDefinir_par_defaut_triggered();

    void on_lancementVideoNasa_clicked();

};

#endif // PREVISAT_H
