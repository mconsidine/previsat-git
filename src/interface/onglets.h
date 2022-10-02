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
 * >    interface
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
 * >    2 octobre 2022
 *
 */

#ifndef ONGLETS_H
#define ONGLETS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QLineEdit>
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QMainWindow>
#include <QtNetwork>
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/observateur/observateur.h"


struct ElementsAOS;
class Date;
class Satellite;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;

namespace Ui {
class Onglets;
}

class Onglets : public QMainWindow
{
    Q_OBJECT

#if (BUILD_TEST == true)
    friend class OngletsTest;
#endif

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
    QString dirDwn() const;
    static Date dateEclipse();
    Ui::Onglets *ui();

    /*
     * Modificateurs
     */
    static void setAcalcDN(bool acalcDN);
    static void setAcalcAOS(bool acalcAOS);
    void setInfo(bool info);
    void setDirDwn(const QString &dirDwn);


    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief show Affichage des donnees numeriques
     * @param date date
     */
    void show(const Date &date);

    /**
     * @brief AffichageLieuObs Affichage du lieu d'observation
     */
    void AffichageLieuObs();

#if defined (Q_OS_WIN)
    /**
     * @brief CalculAosSatSuivi Calcul des informations AOS/LOS pour le suivi d'un satellite
     */
    void CalculAosSatSuivi() const;
#endif

    /**
     * @brief InitChargementOnglets Initialisation du chargement des onglets
     */
    void InitChargementOnglets();

    /**
     * @brief MettreAJourGroupesTLE Mettre a jour un groupe de TLE
     * @param groupe nom du groupe
     */
    void MettreAJourGroupeTLE(const QString &groupe);

    /**
     * @brief RechargerListes Rechargement des listes suite a un changement de langue
     */
    void RechargerListes();

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
     * @param fichierPref nom du fichier
     */
    void SauvePreferences(const QString &fichierPref);


    /******************
     * Telechargement *
     * ***************/
    /**
     * @brief AjoutFichier Ajout d'un fichier dans la liste de telechargement (telechargement asynchrone)
     * @param url url
     */
    void AjoutFichier(const QUrl &url);

    /**
     * @brief AjoutListeFichiers Ajout d'une liste de fichiers a telecharger (telechargement asynchrone)
     * @param listeFichiers liste de fichiers
     */
    void AjoutListeFichiers(const QStringList &listeFichiers);

    /**
     * @brief TelechargementFichier Telechargement d'un fichier
     * @param fichier nom du fichier
     * @param async telechargement asynchrone
     */
    void TelechargementFichier(const QString &fichier, const bool async = true);


public slots:

    void changeEvent(QEvent *evt);
    void on_majMaintenant_clicked();
    void on_pause_clicked();


signals:

    void AffichageSiteLancement(const QString &acronyme, const Observateur &site);
    void AfficherMessageStatut(const QString &message, const int secondes = -1);
    void EffacerMessageStatut();
    void ModeManuel(bool enabled);
    void ChangementDate(const QDateTime &dateTime);
    void ChangementDate(const Date &date);
    void MiseAJourCarte(QResizeEvent *evt = nullptr);
    void RechargerTLE();
    void RecalculerPositions();
    void InitFicTLE();

    void Progression(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);
    void TelechargementFini();


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
    Ui::Onglets *_ui;

    static bool _acalcDN;
    static bool _isEclipse;
    static Date *_dateEclipse;

    static bool _acalcAOS;
    static double _htSat;
    static ElementsAOS *_elementsAOS;

    bool _uniteVitesse;

    bool _info;
    int _indexInfo;
    int _indexOption;
    int _indexMajTLE;
    int _nbOnglets;
    int _nbInformations;

    QStringList _resultatsSatellitesTrouves;

    Date *_date;

    QList<Observateur> _listeObs;

    QPoint _positionSouris;

    QStringList _ficSonAOS;
    QStringList _ficSonLOS;
    QStringList _ficTLEMetOp;

    // Telechargement de fichiers
    bool _async;
    QString _dirDwn;
    QFile _fichier;
    QNetworkAccessManager _mng;
    QNetworkReply *_rep = nullptr;
    QQueue<QUrl> _listeFichiersTelechargement;
    QElapsedTimer _tempsEcoule;

    QTimer *_chronometreUdp;
    QUdpSocket *_udpSocket;
    QString _structureMessageUdp;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageDate Affichage de la date
     */
    void AffichageDate() const;

    /**
     * @brief AffichageDonneesSatellite Affichage des donnees relatives au satellite par defaut
     */
    void AffichageDonneesSatellite() const;

    /**
     * @brief AffichageDonneesSoleilLune Affichage des donnees du Soleil et de la Lune
     */
    void AffichageDonneesSoleilLune() const;

    /**
     * @brief AffichageElementsOSculateurs Affichage des elements osculateurs du satellite par defaut
     */
    void AffichageElementsOSculateurs() const;

    /**
     * @brief AffichageFrequencesRadio Affichage des frequences radio dans les listes deroulantes
     */
    void AffichageFrequencesRadio() const;

    /**
     * @brief AffichageGroupesTLE Affichage des groupes de TLE
     */
    void AffichageGroupesTLE() const;

    /**
     * @brief AffichageInformationsSatellite Affichage des informations sur le satellite
     */
    void AffichageInformationsSatellite() const;

    /**
     * @brief AffichageManoeuvresISS Affichage des manoeuvres ISS
     */
    void AffichageManoeuvresISS() const;

    /**
     * @brief AffichageMessagesMaj Affichage des elements de la liste deroulante pour l'affichage des messages lors de la mise a jour des TLE
     */
    void AffichageMessagesMaj();

    /**
     * @brief AffichageResultatsDonnees Affichage des resultats de la recherche des donnees satellite
     */
    void AffichageResultatsDonnees() const;

    /**
     * @brief AffichageVitesses Affichage des vitesses (par seconde ou par heure)
     */
    void AffichageVitesses() const;

    /**
     * @brief AfficherLieuSelectionne Affichage des donnees du lieu selectionne lorsqu'on clique sur son nom
     * @param index indice du lieu dans la liste
     */
    void AfficherLieuSelectionne(const int index);

    /**
     * @brief CalculAgeTLETransitISS Calcul de l'age des TLE de l'ISS pour transits ISS
     */
    void CalculAgeTLETransitISS();

    /**
     * @brief CalculFrequencesRadio Calcul des donnees de frequence et affichage des valeurs
     */
    void CalculFrequencesRadio() const;

#if defined (Q_OS_WIN)
    /**
     * @brief CalculHauteurMax Calcul de la hauteur maximale d'un satellite dans le ciel
     * @param jjm dates
     * @param obs observateur
     * @param satSuivi satellite
     * @return jour et hauteur maximale
     */
    QPair<double, double> CalculHauteurMax(const QList<double> &jjm, Observateur &obs, Satellite &satSuivi) const;
#endif

    /**
     * @brief ChargementPref Chargement des preferences
     */
    void ChargementPref();

    /**
     * @brief EcritureCompteRenduMaj Ecriture du compte-rendu de mise a jour des TLE
     * @param compteRendu compte rendu de la mise a jour
     * @param compteRenduMaj zone de texte a ecrire
     * @return ecriture du compte rendu
     */
    bool EcritureCompteRenduMaj(const QStringList &compteRendu, QPlainTextEdit *compteRenduMaj);

    /**
     * @brief EcritureInformationsEclipse Ecriture des informations d'eclipse dans l'onglet General
     */
    void EcritureInformationsEclipse(const QString &corpsOccultant, const double fractionIlluminee) const;

    /**
     * @brief getListItemChecked Compte du nombre de satellites coches dans une liste
     * @param liste liste
     * @return nombre de satellites coches
     */
    int getListItemChecked(const QListWidget * const liste) const;

    /**
     * @brief InitAffichageDemarrage Affichage au demarrage
     */
    void InitAffichageDemarrage();

    /**
     * @brief InitChargementStations Chargement des stations
     */
    void InitChargementStations();

    /**
     * @brief InitFicObs Chargement des fichiers de lieux d'observation
     * @param alarme Affichage des messages d'erreur
     */
    void InitFicObs(const bool alarme);

    /**
     * @brief InitFicMap Chargement de la liste de cartes du monde
     * @param majAff mise a jour de l'affichage
     */
    void InitFicMap(const bool majAff);

    /**
     * @brief InitFicPref Chargement de la liste de fichiers de preferences
     * @param majAff mise a jour de l'affichage
     */
    void InitFicPref(const bool majAff);

    /**
     * @brief InitFicSon Chargement de la liste de notifications sonores
     */
    void InitFicSon(void);

    /**
     * @brief InitWallCommandCenter Affichage du Wall Command Center au demarrage
     */
    void InitWallCommandCenter();

    /**
     * @brief JouerSonFlash Execute le son pour les flashs
     */
    static void JouerSonFlash();

    /**
     * @brief ReactualiserAffichage Rechargement du fichier TLE et rafraichissement de l'affichage
     */
    void ReactualiserAffichage();


private slots:

    void EnvoiUdp();

    void ReceptionUdp();


    /******************
     * Telechargement *
     * ***************/
    /**
     * @brief EcritureFichier Ecriture du fichier
     */
    void EcritureFichier();

    /**
     * @brief ProgressionTelechargement Progression du telechargement
     * @param octetsRecus octets recus
     * @param octetsTotal octets du fichier total
     */
    void ProgressionTelechargement(qint64 octetsRecus, qint64 octetsTotal);

    /**
     * @brief FinEnregistrementFichier Fin de l'enregistrement du fichier
     */
    void FinEnregistrementFichier();

    /**
     * @brief FinTelechargementDonnees Traitements a effectuer apres le telechargement des fichiers de donnees
     */
    void FinTelechargementDonnees();

    /**
     * @brief FinTelechargementTle Traitements a effectuer apres le telechargement des fichiers TLE
     */
    void FinTelechargementTle();

    /**
     * @brief TelechargementSuivant Telechargement du fichier suivant
     */
    void TelechargementSuivant();

    bool eventFilter(QObject *object, QEvent *evt) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void on_typeParametres_currentIndexChanged(int index);

    void on_typeRepere_currentIndexChanged(int index);

    void on_informations_currentChanged(int arg1);

    QString getText(QWidget *fenetreParent, const QString &titre, const QString &label, const QString &texteOk, const QString &texteAnnule,
                    QLineEdit::EchoMode mode = QLineEdit::Normal, const QString &texte = QString(), Qt::WindowFlags flags = 0,
                    Qt::InputMethodHints hints = Qt::ImhNone);

    void on_dateHeure3_dateTimeChanged(const QDateTime &dateTime);
    void on_dateHeure4_dateTimeChanged(const QDateTime &dateTime);

    void on_play_clicked();
    void on_rewind_clicked();
    void on_forward_clicked();
    void on_backward_clicked();

    // Recherche des donnees satellites
    void on_infoPrec_clicked();
    void on_infoSuiv_clicked();
    void on_nom_returnPressed();
    void on_noradDonneesSat_valueChanged(int arg1);
    void on_cosparDonneesSat_returnPressed();
    void on_satellitesTrouves_currentRowChanged(int currentRow);

    // Gestion des lieux d'observation
    void on_categoriesObs_customContextMenuRequested(const QPoint &pos);
    void on_categoriesObs_currentRowChanged(int currentRow);
    void on_actionCreer_une_categorie_triggered();
    void on_creationCategorie_clicked();
    void on_validerCategorie_clicked();
    void on_annulerCategorie_clicked();
    void on_actionSupprimerCategorie_triggered();
    void on_actionRenommerCategorie_triggered();
    void on_actionTelechargerCategorie_triggered();

    void on_lieuxObs_customContextMenuRequested(const QPoint &pos);
    void on_lieuxObs_currentRowChanged(int currentRow);
    void on_actionCreer_un_nouveau_lieu_triggered();
    void on_creationLieu_clicked();
    void on_validerObs_clicked();
    void on_annulerObs_clicked();
    void on_actionAjouter_Mes_Preferes_triggered();
    void on_actionModifier_coordonnees_triggered();
    void on_actionRenommerLieu_triggered();
    void on_actionSupprimerLieu_triggered();
    void on_actionSupprimerLieuSelec_triggered();

    void on_ajoutLieu_clicked();
    void on_supprLieu_clicked();
    void on_selecLieux_customContextMenuRequested(const QPoint &pos);
    void on_selecLieux_currentRowChanged(int currentRow);
    void on_optionPrec_clicked();
    void on_optionSuiv_clicked();

    void on_actionTous_triggered();
    void on_actionAucun_triggered();

    void on_barreOnglets_currentChanged(int index);
    void on_ongletsOutils_currentChanged(int index);
    void on_gestionnaireMajTLE_clicked();

    void on_listeMap_currentIndexChanged(int index);
    void on_listeSons_currentIndexChanged(int index);

    void on_parcourirMaj1_clicked();
    void on_parcourirMaj2_clicked();
    void on_mettreAJourTLE_clicked();

    void on_majPrec_clicked();
    void on_majSuiv_clicked();
    void on_miseAJourTLE_currentChanged(int arg1);
    void on_compteRenduMaj_customContextMenuRequested(const QPoint &pos);
    void on_compteRenduMaj2_customContextMenuRequested(const QPoint &pos);
    void on_actionCopier_dans_le_presse_papier_triggered();


    // Calcul des previsions de passage
    void on_calculsPrev_clicked();
    void on_liste2_itemClicked(QListWidgetItem *item);
    void on_liste2_customContextMenuRequested(const QPoint &pos);
    void on_parametrageDefautPrev_clicked();
    void on_effacerHeuresPrev_clicked();
    void on_hauteurSatPrev_currentIndexChanged(int index);
    void on_hauteurSoleilPrev_currentIndexChanged(int index);
    void on_magnitudeMaxPrev_toggled(bool checked);

    // Calcul des flashs
    void on_calculsFlashs_clicked();
    void on_parametrageDefautMetOp_clicked();
    void on_effacerHeuresMetOp_clicked();
    void on_hauteurSatMetOp_currentIndexChanged(int index);
    void on_hauteurSoleilMetOp_currentIndexChanged(int index);

    // Calcul des transits ISS
    void on_calculsTransit_clicked();
    void on_parametrageDefautTransit_clicked();
    void on_effacerHeuresTransit_clicked();
    void on_hauteurSatTransit_currentIndexChanged(int index);
    void on_majTleIss_clicked();
    void on_majManIss_clicked();

#if defined (Q_OS_WIN)
    // Suivi d'un satellite
    void on_genererPositions_clicked();
    void on_afficherSuivi_clicked();
    void on_liste4_itemClicked(QListWidgetItem *item);
    void on_liste4_currentRowChanged(int currentRow);
    void on_lieuxObservation5_currentIndexChanged(int index);
    void on_hauteurSatSuivi_currentIndexChanged(int index);
    void on_skywatcher_clicked();
    void on_ouvrirSatelliteTracker_clicked();
    void on_pecDelai_toggled(bool checked);
    void on_parametrageDefautSuivi_clicked();
#endif

    // Antenne radio
    void on_connexion_clicked();
    void on_ouvrirCatRotator_clicked();
    void on_parametrageDefautRadio_clicked();

    // Calcul des evenements orbitaux
    void on_calculsEvt_clicked();
    void on_parametrageDefautEvt_clicked();
    void on_effacerHeuresEvt_clicked();
    void on_liste3_itemClicked(QListWidgetItem *item);
    void on_liste3_customContextMenuRequested(const QPoint &pos);

    void on_rechercheCreerTLE_clicked();
    void on_inclinaisonExtraction_currentIndexChanged(int index);
    void on_parametrageDefautExtraction_clicked();

    void on_lieuxObservation1_currentIndexChanged(int index);

    // Options WCC
    void on_listeStations_clicked(const QModelIndex &index);
    void on_styleWCC_toggled(bool checked);
    void on_affCerclesAcq_toggled(bool checked);
    void on_policeWCC_currentIndexChanged(int index);
    void on_coulGMT_currentIndexChanged(int index);
    void on_coulZOE_currentIndexChanged(int index);
    void on_coulEquateur_currentIndexChanged(int index);
    void on_coulTerminateur_currentIndexChanged(int index);
    void on_coulCercleVisibilite_currentIndexChanged(int index);
    void on_affBetaWCC_toggled(bool checked);
    void on_affNbOrbWCC_toggled(bool checked);
    void on_affSAA_ZOE_toggled(bool checked);

    // Options d'affichage
    void on_affnomsat_stateChanged(int arg1);
    void on_afficone_stateChanged(int arg1);
    void on_rotationIconeISS_stateChanged(int arg1);
    void on_affNoradListes_stateChanged(int arg1);
    void on_afftraceCiel_stateChanged(int arg1);
    void on_afftraj_stateChanged(int arg1);
    void on_nombreTrajectoires_valueChanged(int arg1);
    void on_affnotif_stateChanged(int arg1);
    void on_affvisib_stateChanged(int arg1);
    void on_affsoleil_stateChanged(int arg1);
    void on_affnuit_stateChanged(int arg1);
    void on_intensiteOmbre_valueChanged(int value);
    void on_affplanetes_stateChanged(int arg1);
    void on_afflune_stateChanged(int arg1);
    void on_affphaselune_stateChanged(int arg1);
    void on_rotationLune_stateChanged(int arg1);
    void on_affetoiles_stateChanged(int arg1);
    void on_affconst_stateChanged(int arg1);
    void on_magnitudeEtoiles_valueChanged(double arg1);
    void on_affgrille_stateChanged(int arg1);
    void on_proportionsCarte_stateChanged(int arg1);
    void on_extinctionAtmospherique_stateChanged(int arg1);
    void on_refractionAtmospherique_stateChanged(int arg1);
    void on_eclipsesLune_stateChanged(int arg1);
    void on_effetEclipsesMagnitude_stateChanged(int arg1);
    void on_affradar_stateChanged(int arg1);
    void on_affinvns_stateChanged(int arg1);
    void on_affinvew_stateChanged(int arg1);
    void on_affcoord_stateChanged(int arg1);
    void on_affnomlieu_stateChanged(int arg1);
    void on_affSAA_toggled(bool checked);
    void on_unitesKm_toggled(bool checked);
    void on_unitesMi_toggled(bool checked);
    void on_syst24h_toggled(bool checked);
    void on_syst12h_toggled(bool checked);
    void on_updown_valueChanged(int arg1);
    void on_utcAuto_stateChanged(int arg1);
    void on_heureLegale_toggled(bool checked);
    void on_utc_toggled(bool checked);
    void on_preferences_currentIndexChanged(int index);
    void on_enregistrerPref_clicked();

    void on_frequenceMontante_currentIndexChanged(int index);
    void on_frequenceDescendante_currentIndexChanged(int index);
};

#endif // ONGLETS_H
