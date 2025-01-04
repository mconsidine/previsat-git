/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    options.h
 *
 * Localisation
 * >    interface.options
 *
 * Heritage
 * >    QDialog
 *
 * Description
 * >    Fenetre d'options
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    13 aout 2022
 *
 * Date de revision
 * >    4 janvier 2025
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>


namespace Ui {
class Options;
}

class QIntValidator;
class QRegularExpressionValidator;
class Observateur;


class Options : public QDialog
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Options Constructeur par defaut
     * @param[in] parent fenetre parent
     * @throw Exception
     */
    explicit Options(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Options();


    /*
     * Accesseurs
     */
    Ui::Options *ui();


    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Initialisation Initialisation de la fenetre Options
     */
    void Initialisation();


public slots:

    /**
     * @brief EcritureRegistre Ecriture en base de registre
     */
    void EcritureRegistre();

    void changeEvent(QEvent *evt);

    void show(QWidget *parent = nullptr);


signals:

    void AfficherLieuObs();
    void AfficherListeSatellites(const QString &nomfic, const bool majListesOnglets = true);
    void ChangementFuseauHoraire(const int offset);
    void ChargementCarteDuMonde();
    void ChargementTraduction(const QString &langue);


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
    Ui::Options *_ui;

    QMap<QString, Observateur> _mapObs;

    // Elements du menu contextuel des categories
    QAction *_creerCategorie;
    QAction *_renommerCategorie;
    QAction *_supprimerCategorie;
    QAction *_telechargerCategorie;

    // Elements du menu contextuel des lieux d'observations
    QAction *_ajouterLieu;
    QAction *_creerLieu;
    QAction *_ajouterLieuMesPreferes;
    QAction *_renommerLieu;
    QAction *_modifierLieu;
    QAction *_supprimerLieu;

    // Elements du menu contextuel des lieux selectionnes
    QAction *_deselectionnerObs;
    QAction *_renommerObs;
    QAction *_modifierObs;
    bool _isObs;

    // Validateurs des coordonnees geographiques
    QRegularExpressionValidator *_validateurLongitudeDegre;
    QRegularExpressionValidator *_validateurLongitudeDecimal;

    QRegularExpressionValidator *_validateurLatitudeDegre;
    QRegularExpressionValidator *_validateurLatitudeDecimal;

    QIntValidator *_validateurAltitudeMetres;
    QIntValidator *_validateurAltitudePieds;

    // Presse-papier
    QClipboard *_clipBoard;


    /*
     * Methodes privees
     */
    /**
     * @brief AffichageLieuObs Affichage des lieux d'observation selectionnes
     */
    void AffichageLieuObs();

    /**
     * @brief AfficherLieu Afficher les coordonnees du lieu selectionne
     * @param[in] obs
     */
    void AfficherLieu(const Observateur &obs);

    /**
     * @brief AppliquerPreferences Appliquer les preferences d'affichage et enregistrer le fichier de preferences
     */
    void AppliquerPreferences();

    /**
     * @brief ChargementPref Chargement du fichier de preferences
     */
    void ChargementPref();

    /**
     * @brief ConversionVersDecimal Conversion des coordonnees geographiques en decimal
     */
    void ConversionVersDecimal();

    /**
     * @brief ConversionVersDegres Conversion des coordonnees geographiques en degres
     */
    void ConversionVersDegres();

    /**
     * @brief CreerMenus Creation des menus contextuels
     */
    void CreerMenus();

    /**
     * @brief InitChargementStations Chargement de la liste des stations
     */
    void InitChargementStations();

    /**
     * @brief InitFicLang Chargement de la liste des langues disponibles
     */
    void InitFicLang();

    /**
     * @brief InitFicMap Chargement de la liste des cartes du monde
     */
    void InitFicMap();

    /**
     * @brief InitFicObs Chargement de la liste des fichiers de lieux d'observation
     */
    void InitFicObs();

    /**
     * @brief InitFicPref Chargement de la liste des fichiers de preference
     */
    void InitFicPref();

    /**
     * @brief InitFicSon Chargement de la liste des fichiers de notification sonore
     */
    void InitFicSon();

    /**
     * @brief InitPoliceWCC Initialisation de la police du Wall Command Center
     */
    void InitPoliceWCC();

    /**
     * @brief InitWallCommandCenter Initialisation des options du Wall Command Center
     */
    void InitWallCommandCenter();

    /**
     * @brief SauvePreferences Sauvegarde des preferences d'affichage
     * @param[in] fichierPref fichier de preferences
     */
    void SauvePreferences(const QString &fichierPref);


private slots:

    // Gestion des categories
    void CreerCategorie();
    void RenommerCategorie();
    void SupprimerCategorie();
    void TelechargerCategorie();

    // Gestion des lieux d'observation
    void CreerLieu();
    void AjouterLieuMesPreferes();
    void RenommerLieu();
    void ModifierLieu();
    void RecupereCoordonneesMaps();
    void SupprimerLieu();

    // Gestion des lieux selectionnes
    void RenommerObs();
    void ModifierObs();

    void closeEvent(QCloseEvent *evt);
    void on_listeOptions_currentRowChanged(int currentRow);
    void on_listeBoutonsOptions_accepted();
    void on_listeBoutonsOptions_rejected();

    void on_creationCategorie_clicked();
    void on_categoriesObs_currentRowChanged(int currentRow);
    void on_categoriesObs_customContextMenuRequested(const QPoint &pos);
    void on_validerCategorie_clicked();
    void on_annulerCategorie_clicked();

    void on_filtreLieuxObs_textChanged(const QString &arg1);
    void on_lieuxObs_currentRowChanged(int currentRow);
    void on_lieuxObs_customContextMenuRequested(const QPoint &pos);
    void on_lieuxObs_itemDoubleClicked(QListWidgetItem *item);
    void on_filtreSelecLieux_textChanged(const QString &arg1);
    void on_selecLieux_currentRowChanged(int currentRow);
    void on_selecLieux_customContextMenuRequested(const QPoint &pos);
    void on_selecLieux_itemDoubleClicked(QListWidgetItem *item);
    void on_creationLieu_clicked();
    void on_ouvrirMaps_clicked();
    void on_decimal_toggled(bool checked);
    void on_sexagesimal_toggled(bool checked);
    void on_validerObs_clicked();
    void on_annulerObs_clicked();
    void on_ajoutLieu_clicked();
    void on_supprLieu_clicked();

    void on_afficone_toggled(bool checked);
    void on_affnomlieu_checkStateChanged(const Qt::CheckState &arg1);
    void on_afftraj_toggled(bool checked);
    void on_affnotif_toggled(bool checked);
    void on_affnuit_stateChanged(int arg1);
    void on_afflune_toggled(bool checked);
    void on_affradar_toggled(bool checked);

    void on_listeMap_currentIndexChanged(int index);
    void on_listeSons_currentIndexChanged(int index);

    void on_updown_valueChanged(int arg1);
    void on_utcAuto_toggled(bool checked);
    void on_heureLegale_toggled(bool checked);
    void on_utc_toggled(bool checked);

};

#endif // OPTIONS_H
