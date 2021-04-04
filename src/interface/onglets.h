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
 * >    4 octobre 2020
 *
 */

#ifndef ONGLETS_H
#define ONGLETS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QLineEdit>
#include <QMainWindow>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include <QLabel>
#include <QMap>
#include "librairies/observateur/observateur.h"


struct ElementsAOS;
class Date;
class Satellite;
class QListWidget;
class QListWidgetItem;

struct Coordonnees {
    QString lon;
    QString lat;
    QString alt;
};

namespace Ui {
class Onglets;
}

class Onglets : public QMainWindow
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

    /*
     * Modificateurs
     */
    static void setAcalcDN(bool acalcDN);
    static void setAcalcAOS(bool acalcAOS);
    void setInfo(bool info);


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
    void AffichageLieuObs() const;

#if defined (Q_OS_WIN)
    /**
     * @brief CalculAosSatSuivi Calcul des informations AOS/LOS pour le suivi d'un satellite
     */
    void CalculAosSatSuivi() const;
#endif

public slots:

    void on_pause_clicked();


signals:

    void AffichageSiteLancement(const QString &acronyme, const Observateur &site);
    void AfficherMessageStatut(const QString &message, const int secondes);
    void EffacerMessageStatut();
    void ModeManuel(bool enabled);
    void ChangementDate(const QDateTime &dateTime);


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
    int _nbOnglets;
    QStringList _titreInformations;
    QStringList _titreOptions;

    static QString _donneesSat;
    QStringList _resultatsSatellitesTrouves;

    Date *_date;
    QMap<QString, Observateur> _mapObs;

    Coordonnees _ligneCoord;

    QPoint _positionSouris;

    QStringList _ficTLEMetOp;


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
     * @brief AffichageInformationsSatellite Affichage des informations sur le satellite
     */
    void AffichageInformationsSatellite() const;

    /**
     * @brief AffichageManoeuvresISS Affichage des manoeuvres ISS
     */
    void AffichageManoeuvresISS() const;

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
    void ChargementPref() const;

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
     * @brief InitFicObs
     */
    /**
     * @brief InitFicObs Chargement des fichiers de lieux d'observation
     * @param alarme Affichage des messages d'erreur
     */
    void InitFicObs(const bool alarme);

    /**
     * @brief InitFicPref Chargement de la liste de fichiers de preferences
     * @param majAff mise a jour de l'affichage
     */
    void InitFicPref(const bool majAff) const;


private slots:

    bool eventFilter(QObject *object, QEvent *evt) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
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

#if defined (Q_OS_WIN)
    // Suivi d'un satellite
    void on_genererPositions_clicked();
    void on_afficherSuivi_clicked();
    void on_liste4_itemClicked(QListWidgetItem *item);
    void on_liste4_currentRowChanged(int currentRow);
    void on_lieuxObservation5_currentIndexChanged(int index);
    void on_hauteurSatSuivi_currentIndexChanged(int index);
#endif

    // Calcul des evenements orbitaux
    void on_calculsEvt_clicked();
    void on_parametrageDefautEvt_clicked();
    void on_effacerHeuresEvt_clicked();
    void on_liste3_itemClicked(QListWidgetItem *item);
    void on_liste3_customContextMenuRequested(const QPoint &pos);
};

#endif // ONGLETS_H
