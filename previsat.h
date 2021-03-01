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
 * >
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
 * >    18 juillet 2020
 *
 */

#ifndef PREVISAT_H
#define PREVISAT_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wpacked"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wpacked"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QTableWidget>
#include "librairies/dates/date.h"


namespace Ui {
class PreviSat;
}

class PreviSat : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviSat(QWidget *fenetreParent = 0);

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

    /**
     * @brief DeterminationLocale Determination de la locale
     * @return locale
     */
    static QString DeterminationLocale();

    ~PreviSat();

private:
    Ui::PreviSat *ui;

    /*******************
     * Initialisations *
     ******************/
    /**
     * @brief InitAffichageDemarrage Affichage au demarrage
     */
    void InitAffichageDemarrage() const;

    /**
     * @brief InitBarreStatut Initialisation de la barre de statut
     * @param police police
     */
    void InitBarreStatut(const QFont &police);

    /**
     * @brief InitChainesVideo Chargement de la liste des chaines video
     */
    void InitChainesVideo() const;

    /**
     * @brief InitChampsDefaut Initialisation des champs par defaut
     */
    void InitChampsDefaut();

    /**
     * @brief InitChargementStations Chargement des stations
     */
    void InitChargementStations() const;

    /**
     * @brief InitChargementTDRS Chargement des satellites TDRS
     */
    void InitChargementTDRS() const;

    /**
     * @brief InitFicMap Chargement de la liste de cartes du monde
     * @param majAff mise a jour de l'affichage
     */
    void InitFicMap(const bool majAff) const;

    /**
     * @brief InitFicObs Chargement des fichiers d'observation
     * @param alarm affichage d'un message d'avertissement
     */
    void InitFicObs(const bool alarm) const;

    /**
     * @brief InitFicPref Chargement de la liste de fichiers de preferences
     * @param majAff mise a jour de l'affichage
     */
    void InitFicPref(const bool majAff) const;

    /**
     * @brief InitFicSon Chargement de la liste de cartes du monde
     */
    void InitFicSon() const;

    /**
     * @brief InitFicTLE Chargement de la liste de fichiers TLE
     */
    void InitFicTLE() const;

    /**
     * @brief InitMenus Initialisation des menus
     * @param police police
     */
    void InitMenus(const QFont &police) const;

    /**
     * @brief InitWallCommandCenter Affichage du Wall Command Center au demarrage
     */
    void InitWallCommandCenter();

    /**
     * @brief VerifieFichiersData Verifie la presence des fichiers du repertoire data
     * @param dirData repertoire data
     * @param listeFicData liste de fichiers du repertoire data
     */
    void VerifieFichiersData(const QString &dirData, const QStringList &listeFicData) const;


    /**************
     * Affichages *
     *************/
    /**
     * @brief AffichageCourbes Affichage des elements graphiques
     */
    void AffichageCourbes() const;

    /**
     * @brief AffichageDonnees Affichage des donnees numeriques
     */
    void AffichageDonnees();

    /**
     * @brief AffichageElementsOsculateurs Affichage des elements osculateurs
     */
    void AffichageElementsOsculateurs() const;

    /**
     * @brief AffichageGroupesTLE Affichage des groupes de TLE
     */
    void AffichageGroupesTLE() const;

    /**
     * @brief AffichageLieuObs Affichage du lieu d'observation sur l'interface graphique
     */
    void AffichageLieuObs() const;

    /**
     * @brief AffichageManoeuvresISS Affichage des manoeuvres ISS
     */
    void AffichageManoeuvresISS() const;

    /**
     * @brief AffichageSatellite Affichage d'un satellite sur la carte du monde
     * @param isat indice du satellite
     * @param lsat longitude du satellite
     * @param bsat latitude du satellite
     * @param lcarte longueur de la carte
     * @param hcarte hauteur de la carte
     */
    void AffichageSatellite(const int isat, const int lsat, const int bsat, const int lcarte, const int hcarte) const;

    /**
     * @brief AfficherListeSatellites Affichage des noms des satellites dans les listes
     * @param fichier fichier TLE
     * @param listeSat liste des satellites
     */
    void AfficherListeSatellites(const QString &fichier, const QStringList &listeSat, const bool aMajListesSecondaires = true) const;

    /**
     * @brief CalculsAffichage Enchainement des calculs et affichage
     */
    void CalculsAffichage();


    /***********
     * Calculs *
     **********/
    /**
     * @brief CalculAOS Calcul du prochain AOS/LOS
     * @return vrai si le satellite peut se lever et se coucher dans le ciel de l'observateur
     */
    bool CalculAOS() const;

    /**
     * @brief CalculAgeTLETransitISS Calcul de l'age du TLE de l'ISS pour le calcul des transits
     */
    void CalculAgeTLETransitISS() const;

    /**
     * @brief CalculDN Calcul de la prochaine date d'eclipse de l'ISS
     */
    void CalculDN() const;

    /**
     * @brief CalculNumeroOrbiteISS Calcul du numero d'orbite de l'ISS
     * @param date date
     * @return numero d'orbite de l'ISS
     */
    int CalculNumeroOrbiteISS(const Date &date) const;

    /**
     * @brief EnchainementCalculs Enchainement des calculs
     */
    void EnchainementCalculs() const;


    /******************************
     * Telechargement de fichiers *
     *****************************/
    /**
     * @brief AjoutFichier Ajout d'un fichier dans la liste de fichiers a telecharger
     * @param url adresse du fichier
     */
    void AjoutFichier(const QUrl &url);

    /**
     * @brief MajFichierTLE Mise a jour du fichier TLE courant
     */
    void MajFichierTLE();

    /**
     * @brief MajWebTLE Mise a jour automatique des TLE
     */
    void MajWebTLE();

    /**
     * @brief MiseAJourFichiers Mise a jour des fichiers internes et du logiciel
     * @param action type de mise a jour
     * @param typeMAJ nom de la mise a jour
     */
    void MiseAJourFichiers(QAction *action, const QString &typeMAJ);

    /**
     * @brief TelechargementFichier Telechargement d'un fichier
     * @param ficHttp adresse du fichier
     * @param async vrai si le telechargement doit s'effectuer de maniere asynchrone
     */
    void TelechargementFichier(const QString &ficHttp, const bool async);

    /**
     * @brief VerifAgeTLE Verification de l'age d'un TLE
     */
    void VerifAgeTLE();

    /**
     * @brief VerifMAJPreviSat Verification des mises a jour (logiciel, fichiers internes)
     */
    void VerifMAJPreviSat();


    /*************
     * Interface *
     *************/
    /**
     * @brief AffichageListeFichiersTLE Affichage des fichiers TLE dans les listes deroulantes
     * @param fichier fichier TLE
     * @param comboBox liste deroulante
     * @param listeFicTLEs liste des fichiers TLE
     */
    void AffichageListeFichiersTLE(const QString &fichier, QComboBox *comboBox, QStringList &listeFicTLEs);

    /**
     * @brief AfficherLieuSelectionne Affichage des donnees du lieu selectionne lorsqu'on clique sur son nom
     * @param index indice du lieu dans la liste
     */
    void AfficherLieuSelectionne(const int index);

    /**
     * @brief ChargementPref Chargement des preferences
     */
    void ChargementPref() const;

    /**
     * @brief EcritureCompteRenduMaj Ecriture du compte-rendu de mise a jour des TLE
     * @param compteRendu compte-rendu
     * @param aecr ecriture du compte-rendu
     */
    void EcritureCompteRenduMaj(const QStringList &compteRendu, bool &aecr);

    /**
     * @brief getListeItemChecked Compte du nombre de satellites coches dans une liste
     * @param listWidget liste
     * @return nombre de satellites coches
     */
    int getListeItemChecked(const QListWidget *listWidget) const;

    /**
     * @brief ModificationOption Modification d'une option d'affichage
     */
    void ModificationOption();

    /**
     * @brief OuvertureFichierTLE Ouverture d'un fichier TLE
     * @param fichier nom du fichier
     */
    void OuvertureFichierTLE(const QString &fichier);

    /**
     * @brief SauveOngletElementsOsculateurs Sauvegarde des donnees de l'onglet Elements osculateurs
     * @param fic nom du fichier
     */
    void SauveOngletElementsOsculateurs(const QString &fic) const;

    /**
     * @brief SauveOngletGeneral Sauvegarde des donnees de l'onglet General
     * @param fic nom du fichier
     */
    void SauveOngletGeneral(const QString &fic) const;

    /**
     * @brief SauveOngletInformations Sauvegarde des donnees de l'onglet Informations
     * @param fic nom du fichier
     */
    void SauveOngletInformations(const QString &fic) const;

    /**
     * @brief SauvePreferences Sauvegarde des preferences
     * @param fic fichier
     */
    void SauvePreferences(const QString &fic) const;


    /***********
     * Systeme *
     **********/
    /**
     * @brief DecompressionFichierGz Decompression d'un fichier TLE au format gz
     * @param fichierGz fichier gz
     * @param fichierDecompresse fichier decompresse
     * @return
     */
    bool DecompressionFichierGz(const QString &fichierGz, const QString &fichierDecompresse) const;

    /**
     * @brief EcritureListeRegistre Ecriture de la liste de satellites dans la base de registre
     */
    void EcritureListeRegistre() const;


signals:

    /**
     * @brief TelechargementFini Signal du telechargement fini
     */
    void TelechargementFini();


private slots:

    /***********
     * Calculs *
     **********/
    /**
     * @brief CalculsTermines Fin des calculs de previsions
     */
    void CalculsTermines();

    /**
     * @brief GestionTempsReel Gestion du temps reel
     */
    void GestionTempsReel();


    /******************************
     * Telechargement de fichiers *
     *****************************/
    /**
     * @brief EcritureFichier Ecriture du fichier telecharge
     */
    void EcritureFichier();

    /**
     * @brief FinEnregistrementFichier Gestion de l'enregistrement des fichiers telecharges
     */
    void FinEnregistrementFichier();

    /**
     * @brief ProgressionTelechargement Progression du telechargement
     * @param recu nombre d'octets recus
     * @param total nombre d'octets total
     */
    void ProgressionTelechargement(qint64 recu, qint64 total) const;

    /**
     * @brief TelechargementSuivant Demarrage du telechargement suivant
     */
    void TelechargementSuivant();

    void closeEvent(QCloseEvent *evt);
    void resizeEvent(QResizeEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
    void mouseDoubleClickEvent(QMouseEvent *evt);

    void on_maximise_clicked();
    void on_affichageCiel_clicked();
    void on_pasReel_currentIndexChanged(int index);
    void on_pasManuel_currentIndexChanged(int index);

    void on_faireDon_clicked();
    void on_meteo_clicked();
    void on_meteoBasesNASA_clicked();
    void MeteoPleinEcran();
    void ActualiseMeteoNASA();

    void on_mccISS_toggled(bool checked);
    void on_chaine_valueChanged(int arg1);
    void on_fluxVideo_clicked();

    void on_directHelp_clicked();

    QString getText(QWidget *fenetreParent, const QString &titre, const QString &label, const QString &texteOk, const QString &texteAnnule,
                    QLineEdit::EchoMode mode = QLineEdit::Normal, const QString &texte = QString(), Qt::WindowFlags flags = 0,
                    Qt::InputMethodHints hints = Qt::ImhNone);

    // Menu deroulant
    void on_actionOuvrir_fichier_TLE_triggered();
    void on_actionEnregistrer_triggered();
    void on_actionImprimer_carte_triggered();
    void on_actionVision_nocturne_toggled(bool arg1);
    void on_actionFichier_d_aide_triggered();
    void on_actionInformations_triggered();
    void on_actionFaire_triggered();
    void on_actionPrevisat_sourceforge_net_triggered();
    void on_actionTelecharger_la_mise_a_jour_triggered();
    void on_actionMettre_jour_fichiers_internes_triggered();
    void on_actionRapport_de_bug_triggered();
    void on_actionWww_celestrak_com_triggered();
    void on_actionWww_space_track_org_triggered();
    void on_actionA_propos_triggered();

    // Gestion de la liste principale de satellites
    void on_listeFichiersTLE_currentIndexChanged(int index);
    void on_actionDefinir_par_defaut_triggered();
    void on_actionNouveau_fichier_TLE_triggered();
    void on_actionFichier_TLE_existant_triggered();
    void on_liste1_clicked(const QModelIndex &index);
    void on_liste1_customContextMenuRequested(const QPoint &position);
    void on_liste1_entered(const QModelIndex &index);

    // Gestion de l'onglet General
    void on_lieuxObservation1_currentIndexChanged(int index);
    void on_tempsReel_toggled(bool checked);
    void on_modeManuel_toggled(bool checked);
    void on_dateHeure3_dateTimeChanged(const QDateTime &date);
    void on_dateHeure4_dateTimeChanged(const QDateTime &date);
    void on_play_clicked();
    void on_pause_clicked();
    void on_rewind_clicked();
    void on_forward_clicked();
    void on_backward_clicked();

    void on_rechercheDonneesSat_toggled(bool checked);
    void on_nom_returnPressed();
    void on_noradDonneesSat_valueChanged(int arg1);
    void on_cosparDonneesSat_returnPressed();
    void on_satellitesTrouves_currentRowChanged(int currentRow);
    void AffichageResultats();

    // Gestion des options d'affichage
    void on_preferences_currentIndexChanged(int index);
    void on_enregistrerPref_clicked();
    void on_affsoleil_stateChanged(int arg1);
    void on_affnuit_stateChanged(int arg1);
    void on_intensiteOmbre_valueChanged(int value);
    void on_affgrille_stateChanged(int arg1);
    void on_afflune_stateChanged(int arg1);
    void on_affphaselune_stateChanged(int arg1);
    void on_rotationIconeISS_stateChanged(int arg1);
    void on_rotationLune_stateChanged(int arg1);
    void on_affnomsat_stateChanged(int arg1);
    void on_affvisib_stateChanged(int arg1);
    void on_afftraceCiel_stateChanged(int arg1);
    void on_afftraj_stateChanged(int arg1);
    void on_nombreTrajectoires_valueChanged(int arg1);
    void on_affradar_stateChanged(int arg1);
    void on_afficone_stateChanged(int arg1);
    void on_affinvns_stateChanged(int arg1);
    void on_affinvew_stateChanged(int arg1);
    void on_affnomlieu_stateChanged(int arg1);
    void on_affnotif_stateChanged(int arg1);
    void on_calJulien_stateChanged(int arg1);
    void on_affcoord_stateChanged(int arg1);
    void on_extinctionAtmospherique_stateChanged(int arg1);
    void on_refractionPourEclipses_stateChanged(int arg1);
    void on_effetEclipsesMagnitude_stateChanged(int arg1);
    void on_eclipsesLune_stateChanged(int arg1);
    void on_affetoiles_stateChanged(int arg1);
    void on_affconst_stateChanged(int arg1);
    void on_magnitudeEtoiles_valueChanged(double arg1);
    void on_affSAA_stateChanged(int arg1);
    void on_affplanetes_stateChanged(int arg1);
    void on_intensiteVision_valueChanged(int value);
    void on_unitesKm_toggled(bool checked);
    void on_unitesMi_toggled(bool checked);
    void on_syst24h_toggled(bool checked);
    void on_syst12h_toggled(bool checked);
    void on_proportionsCarte_stateChanged(int arg1);
    void on_typeRepere_currentIndexChanged(int index);
    void on_typeParametres_currentIndexChanged(int index);
    void on_heureLegale_toggled(bool checked);
    void on_utc_toggled(bool checked);
    void on_updown_valueChanged(int arg1);
    void on_utcAuto_stateChanged(int arg1);
    void on_listeMap_currentIndexChanged(int index);
    void on_listeSons_currentIndexChanged(int index);

    void on_affBetaWCC_toggled(bool checked);
    void on_affCerclesAcq_toggled(bool checked);
    void on_affNbOrbWCC_toggled(bool checked);
    void on_affSAA_ZOE_toggled(bool checked);
    void on_listeStations_clicked(const QModelIndex &index);
    void on_listeStations_customContextMenuRequested(const QPoint &position);
    void on_styleWCC_toggled(bool checked);
    void on_coulGMT_currentIndexChanged(int index);
    void on_coulZOE_currentIndexChanged(int index);
    void on_coulEquateur_currentIndexChanged(int index);
    void on_coulTerminateur_currentIndexChanged(int index);
    void on_coulCercleVisibilite_currentIndexChanged(int index);
    void on_policeWCC_currentIndexChanged(int index);

    // Gestion des lieux d'observation
    void on_actionCreer_une_categorie_triggered();
    void on_creationCategorie_clicked();
    void on_actionSupprimerCategorie_triggered();
    void on_actionTelechargerCategorie_triggered();
    void on_annulerCategorie_clicked();
    void on_validerCategorie_clicked();
    void on_actionRenommerCategorie_triggered();
    void on_fichiersObs_currentRowChanged(int currentRow);
    void on_fichiersObs_customContextMenuRequested(const QPoint &position);
    void on_lieuxObs_currentRowChanged(int currentRow);
    void on_lieuxObs_customContextMenuRequested(const QPoint &position);
    void on_selecLieux_currentRowChanged(int currentRow);
    void on_selecLieux_customContextMenuRequested(const QPoint &position);
    void on_actionCreer_un_nouveau_lieu_triggered();
    void on_creationLieu_clicked();
    void on_actionAjouter_Mes_Preferes_triggered();
    void on_actionModifier_coordonnees_triggered();
    void on_validerObs_clicked();
    void on_annulerObs_clicked();
    void on_actionRenommerLieu_triggered();
    void on_actionSupprimerLieu_triggered();
    void on_actionSupprimerLieuSelec_triggered();
    void on_ajoutLieu_clicked();
    void on_supprLieu_clicked();

    // Gestion des onglets
    void on_barreMenu_pressed();
    void on_onglets_currentChanged(int index);
    void on_ongletsOutils_currentChanged(int index);

    // Mise a jour des TLE
    void on_groupeTLE_currentIndexChanged(int index);
    void on_majMaintenant_clicked();
    void on_actionMettre_jour_TLE_courant_triggered();
    void on_actionMettre_jour_groupe_TLE_triggered();
    void MettreAJourGroupesTLE(const QString &groupe);
    void on_actionMettre_jour_TLE_communs_triggered();
    void on_actionMettre_jour_tous_les_groupes_de_TLE_triggered();
    void on_parcourirMaj1_clicked();
    void on_parcourirMaj2_clicked();
    void on_mettreAJourTLE_clicked();
    void on_gestionnaireMajTLE_clicked();
    void on_compteRenduMaj_customContextMenuRequested(const QPoint &position);
    void on_actionCopier_dans_le_presse_papier_triggered();

    // Extraction d'un fichier TLE
    void on_numeroNORADCreerTLE_currentIndexChanged(int index);
    void on_ADNoeudAscendantCreerTLE_currentIndexChanged(int index);
    void on_excentriciteCreerTLE_currentIndexChanged(int index);
    void on_inclinaisonCreerTLE_currentIndexChanged(int index);
    void on_argumentPerigeeCreerTLE_currentIndexChanged(int index);
    void on_parcourir1CreerTLE_clicked();
    void on_parcourir2CreerTLE_clicked();
    void on_rechercheCreerTLE_clicked();

    // Calcul des previsions de passage
    void on_effacerHeuresPrev_clicked();
    void on_liste2_customContextMenuRequested(const QPoint &position);
    void on_liste2_entered(const QModelIndex &index);
    void on_actionTous_triggered();
    void on_actionAucun_triggered();
    void on_hauteurSatPrev_currentIndexChanged(int index);
    void on_hauteurSoleilPrev_currentIndexChanged(int index);
    void on_magnitudeMaxPrev_toggled(bool checked);
    void on_parametrageDefautPrev_clicked();
    void on_calculsPrev_clicked();
    void on_afficherPrev_clicked();

    // Calcul des evenements orbitaux
    void on_effacerHeuresEvt_clicked();
    void on_liste3_customContextMenuRequested(const QPoint &position);
    void on_liste3_entered(const QModelIndex &index);
    void on_parametrageDefautEvt_clicked();
    void on_calculsEvt_clicked();
    void on_afficherEvt_clicked();

    // Calcul des transits ISS
    void on_effacerHeuresTransit_clicked();
    void on_hauteurSatTransit_currentIndexChanged(int index);
    void on_parametrageDefautTransit_clicked();
    void on_majTleIss_clicked();
    void on_manoeuvresISS_itemDoubleClicked(QTableWidgetItem *item);
    void on_calculsTransit_clicked();
    void on_afficherTransit_clicked();

    // Calcul des flashs MetOp
    void on_effacerHeuresMetOp_clicked();
    void on_fichierTLEMetOp_currentIndexChanged(int index);
    void on_hauteurSatMetOp_currentIndexChanged(int index);
    void on_hauteurSoleilMetOp_currentIndexChanged(int index);
    void on_parametrageDefautMetOp_clicked();
    void on_calculsMetOp_clicked();
    void on_afficherMetOp_clicked();

};

#endif // PREVISAT_H
