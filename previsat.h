/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    7 decembre 2013
 *
 */

#ifndef PREVISAT_H
#define PREVISAT_H

#include <QComboBox>
#include <QKeyEvent>
#include <QListWidget>
#include <QMainWindow>
#include <QModelIndex>
#include <QtNetwork>
#include "vlc.h"
#include <QMessageBox>
namespace Ui {
class PreviSat;
}

class PreviSat : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviSat(QWidget *fenetreParent = 0);
    void ChargementConfig();
    void ChargementTLE();
    void MAJTLE();
    void DemarrageApplication();
    static QString DeterminationLocale();

    ~PreviSat();

private:
    Ui::PreviSat *ui;

    bool _isPlaying;
    libvlc_instance_t *_vlcinstance;
    libvlc_media_player_t *_mp, *_mp2;
    libvlc_media_t *_m;

    // Initialisation
    void InitFicObs(const bool alarm) const;
    void InitFicTLE() const;
    void InitFicMap(const bool majAff) const;

    // Affichage
    void AffichageDonnees();
    void AffichageElementsOsculateurs() const;
    void AffichageCourbes() const;
    void AffichageGroupesTLE() const;
    void AffichageLieuObs() const;
    void AfficherListeSatellites(const QString &fichier, const QStringList &listeSat) const;
    void CalculsAffichage();

    // Calculs
    bool CalculAOS() const;
    void CalculDN() const;
    void CalculAgeTLETransitISS() const;
    void EnchainementCalculs() const;
    void MajWebTLE();
    void AjoutFichier(const QUrl &url);
    void VerifAgeTLE();
    void VerifMAJPreviSat();
    void MiseAJourFichiers(QAction *action, const QString &typeMAJ);
    void TelechargementFichier(const QString &ficHttp, const bool async);

    // Interface
    void OuvertureFichierTLE(const QString &fichier);
    void AffichageListeFichiersTLE(const QString &fichier, QComboBox *comboBox, QStringList &listeFicTLEs);
    void SauveOngletGeneral(const QString &fic) const;
    void SauveOngletElementsOsculateurs(const QString &fic) const;
    void SauveOngletInformations(const QString &fic) const;
    void EcritureCompteRenduMaj(const QStringList &compteRendu, bool &aecr);
    void ModificationOption();
    void AfficherLieuSelectionne(const int index);
    int getListeItemChecked(const QListWidget *listWidget) const;

    // Systeme
    void EcritureListeRegistre() const;
    bool DecompressionFichierGz(const QString &fichierGz, const QString &fichierDecompresse) const;

signals:
    void TelechargementFini();

private slots:

    void GestionTempsReel();
    void CalculsTermines();
    void TelechargementSuivant();
    void FinEnregistrementFichier();
    void EcritureFichier();
    void ProgressionTelechargement(qint64 recu, qint64 total) const;

    void closeEvent(QCloseEvent *evt);
    void resizeEvent(QResizeEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
    void mouseDoubleClickEvent(QMouseEvent *evt);
    void CaptureVideo();

    void on_maximise_clicked();
    void on_affichageCiel_clicked();
    void on_pasReel_currentIndexChanged(int index);
    void on_pasManuel_currentIndexChanged(int index);

    void on_mccISS_toggled(bool checked);
    void on_fluxVideo_clicked();
    void on_muetVideo_clicked();
    void on_agrandirVideo_clicked();
    void on_fermerVideo_clicked();

    void on_directHelp_clicked();

    // Menu deroulant
    void on_actionOuvrir_fichier_TLE_activated();
    void on_actionEnregistrer_activated();
    void on_actionImprimer_carte_activated();
    void on_actionVision_nocturne_toggled(bool arg1);
    void on_actionFichier_d_aide_activated(int arg1);
    void on_actionAstropedia_free_fr_activated();
    void on_actionTelecharger_la_mise_a_jour_activated();
    void on_actionMettre_jour_fichiers_internes_activated();
    void on_actionRapport_de_bug_activated();
    void on_actionWww_celestrak_com_activated();
    void on_actionWww_space_track_org_activated();
    void on_actionA_propos_activated(int arg1);

    // Gestion de la liste principale de satellites
    void on_listeFichiersTLE_currentIndexChanged(int index);
    void on_actionDefinir_par_defaut_activated();
    void on_actionNouveau_fichier_TLE_activated();
    void on_actionFichier_TLE_existant_activated();
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

    // Gestion des options d'affichage
    void on_affsoleil_stateChanged(int arg1);
    void on_affnuit_stateChanged(int arg1);
    void on_intensiteOmbre_valueChanged(int value);
    void on_affgrille_stateChanged(int arg1);
    void on_afflune_stateChanged(int arg1);
    void on_affphaselune_stateChanged(int arg1);
    void on_rotationLune_stateChanged(int arg1);
    void on_affnomsat_stateChanged(int arg1);
    void on_affvisib_stateChanged(int arg1);
    void on_afftraj_stateChanged(int arg1);
    void on_nombreTrajectoires_valueChanged(int arg1);
    void on_affradar_stateChanged(int arg1);
    void on_affinvns_stateChanged(int arg1);
    void on_affinvew_stateChanged(int arg1);
    void on_affnomlieu_stateChanged(int arg1);
    void on_affnotif_stateChanged(int arg1);
    void on_calJulien_stateChanged(int arg1);
    void on_affcoord_stateChanged(int arg1);
    void on_extinctionAtmospherique_stateChanged(int arg1);
    void on_affetoiles_stateChanged(int arg1);
    void on_affconst_stateChanged(int arg1);
    void on_magnitudeEtoiles_valueChanged(double arg1);
    void on_affSAA_stateChanged(int arg1);
    void on_affplanetes_stateChanged(int arg1);
    void on_intensiteVision_valueChanged(int value);
    void on_unitesKm_toggled(bool checked);
    void on_unitesMi_toggled(bool checked);
    void on_proportionsCarte_stateChanged(int arg1);
    void on_typeParametres_currentIndexChanged(int index);
    void on_heureLegale_toggled(bool checked);
    void on_utc_toggled(bool checked);
    void on_updown_valueChanged(int arg1);
    void on_utcAuto_stateChanged(int arg1);
    void on_listeMap_currentIndexChanged(int index);

    // Gestion des lieux d'observation
    void on_actionCreer_une_categorie_activated(int arg1);
    void on_actionSupprimerCategorie_activated(int arg1);
    void on_actionTelechargerCategorie_activated(int arg1);
    void on_annulerCategorie_clicked();
    void on_validerCategorie_clicked();
    void on_actionRenommerCategorie_activated();
    void on_fichiersObs_currentRowChanged(int currentRow);
    void on_fichiersObs_customContextMenuRequested(const QPoint &position);
    void on_lieuxObs_currentRowChanged(int currentRow);
    void on_lieuxObs_customContextMenuRequested(const QPoint &position);
    void on_selecLieux_currentRowChanged(int currentRow);
    void on_selecLieux_customContextMenuRequested(const QPoint &position);
    void on_actionCreer_un_nouveau_lieu_activated();
    void on_actionAjouter_Mes_Preferes_activated();
    void on_actionModifier_coordonnees_activated();
    void on_validerObs_clicked();
    void on_annulerObs_clicked();
    void on_actionRenommerLieu_activated();
    void on_actionSupprimerLieu_activated();
    void on_actionSupprimerLieuSelec_activated();
    void on_ajoutLieu_clicked();
    void on_supprLieu_clicked();

    // Gestion des onglets
    void on_barreMenu_pressed();
    void on_onglets_currentChanged(QWidget *arg1);
    void on_ongletsOutils_currentChanged(QWidget *arg1);

    // Mise a jour des TLE
    void on_groupeTLE_currentIndexChanged(int index);
    void on_majMaintenant_clicked();
    void on_parcourirMaj1_clicked();
    void on_parcourirMaj2_clicked();
    void on_mettreAJourTLE_clicked();
    void on_gestionnaireMajTLE_clicked();
    void on_compteRenduMaj_customContextMenuRequested(const QPoint &position);
    void on_actionCopier_dans_le_presse_papier_activated();

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
    void on_actionTous_activated();
    void on_actionAucun_activated();
    void on_hauteurSatPrev_currentIndexChanged(int index);
    void on_hauteurSoleilPrev_currentIndexChanged(int index);
    void on_magnitudeMaxPrev_toggled(bool checked);
    void on_parametrageDefautPrev_clicked();
    void on_calculsPrev_clicked();
    void on_annulerPrev_clicked();
    void on_afficherPrev_clicked();

    // Calcul des flashs Iridium
    void on_effacerHeuresIri_clicked();
    void on_fichierTLEIri_currentIndexChanged(int index);
    void on_hauteurSatIri_currentIndexChanged(int index);
    void on_hauteurSoleilIri_currentIndexChanged(int index);
    void on_parametrageDefautIri_clicked();
    void on_calculsIri_clicked();
    void on_annulerIri_clicked();
    void on_afficherIri_clicked();

    // Calcul des evenements orbitaux
    void on_effacerHeuresEvt_clicked();
    void on_liste3_customContextMenuRequested(const QPoint &position);
    void on_liste3_entered(const QModelIndex &index);
    void on_parametrageDefautEvt_clicked();
    void on_calculsEvt_clicked();
    void on_annulerEvt_clicked();
    void on_afficherEvt_clicked();

    // Calcul des transits ISS
    void on_effacerHeuresTransit_clicked();
    void on_fichierTLETransit_currentIndexChanged(int index);
    void on_hauteurSatTransit_currentIndexChanged(int index);
    void on_parametrageDefautTransit_clicked();
    void on_calculsTransit_clicked();
    void on_annulerTransit_clicked();
    void on_afficherTransit_clicked();

};

#endif // PREVISAT_H
