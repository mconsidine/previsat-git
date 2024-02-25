/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    outils.h
 *
 * Localisation
 * >    interface.outils
 *
 * Heritage
 * >    QDialog
 *
 * Description
 * >    Fenetre d'outils
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    14 aout 2022
 *
 * Date de revision
 * >    27 decembre 2023
 *
 */

#ifndef OUTILS_H
#define OUTILS_H

#include <QDialog>
#include "librairies/corps/satellite/elementsorbitaux.h"


namespace Ui {
class Outils;
}

class QListWidgetItem;
class QPlainTextEdit;
class InformationsSatellite;

class Outils : public QDialog
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Outils Constructeur par defaut
     * @param[in] parent fenetre parent
     * @throw Exception
     */
    explicit Outils(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Outils();


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
     * @brief Initialisation Initialisation de la fenetre Outils
     */
    void Initialisation();


public slots:

    void changeEvent(QEvent *evt);


signals:

    void ChargementGP();
    void InitFicGP();


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
    Ui::Outils *_ui;

    QAction *_creerGroupe;
    QAction *_supprimerGroupe;
    QAction *_ajouterFichiers;
    QAction *_supprimerFichier;
    QAction *_copier;

    QMap<QString, ElementsOrbitaux> _mapElem;
    InformationsSatellite *_informations;


    /*
     * Methodes privees
     */
    /**
     * @brief CreationMenus Creation des menus contextuels
     */
    void CreationMenus();

    /**
     * @brief EcritureCompteRenduMaj Ecriture du compte-rendu de mise a jour des TLE
     * @param[in] compteRendu compte-rendu
     * @param[out] compteRenduMaj zone de texte ou ecrire le compte-rendu
     */
    void EcritureCompteRenduMaj(const QStringList &compteRendu, QPlainTextEdit * compteRenduMaj);

    /**
     * @brief InitListeDomaines Initialisation de la liste deroulante des noms de domaine
     */
    void InitListeDomaines();

    /**
     * @brief InitGestionnaireTLE Initialisation du gestionnaire de suppression de TLE
     * @param[in] listeFicTLE liste des fichiers TLE
     */
    void InitGestionnaireTLE(const QStringList &listeFicTLE);


private slots:

    /**
     * @brief Copier Copier les compte-rendus de mise a jour des TLE dans le presse-papier
     */
    void Copier();

    /**
     * @brief SupprimerFichier Supprimer un fichier de la liste d'un groupe d'elements orbitaux
     */
    void SupprimerFichier();

    /**
     * @brief SupprimerGroupe Supprimer un groupe d'elements orbitaux
     */
    void SupprimerGroupe();

    /**
     * @brief ProgressionElem Affichage de la progression de telechargement des elements orbitaux
     * @param[in] octetsRecus octets recus
     * @param[in] octetsTotal octets au total
     * @param[in] vitesse vitesse de telechargement
     * @param[in] unite unite de vitesse de telechargement
     */
    void ProgressionElem(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);

    /**
     * @brief ProgressionElem Affichage de la progression de telechargement des TLE
     * @param[in] octetsRecus octets recus
     * @param[in] octetsTotal octets au total
     * @param[in] vitesse vitesse de telechargement
     * @param[in] unite unite de vitesse de telechargement
     */
    void ProgressionTLE(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);

    void closeEvent(QCloseEvent *evt);

    void on_listeOutils_currentRowChanged(int currentRow);
    void on_listeOutils_itemSelectionChanged();
    void on_serveur_currentTextChanged(const QString &arg1);
    void on_listeGroupeElem_currentRowChanged(int currentRow);
    void on_listeGroupeElem_itemClicked(QListWidgetItem *item);
    void on_listeGroupeElem_customContextMenuRequested(const QPoint &pos);
    void on_listeFichiersElem_customContextMenuRequested(const QPoint &pos);
    void on_creationGroupe_clicked();
    void on_ajoutFichiersElem_clicked();
    void on_majGroupe_clicked();
    void on_ageMaxElementsOrbitaux_toggled(bool checked);
    void on_valider_clicked();
    void on_annuler_clicked();
    void on_listeFichiersElem_currentRowChanged(int currentRow);
    void on_listeFichiersElem_itemSelectionChanged();
    void on_listeSatellites_currentRowChanged(int currentRow);
    void on_filtreSatellites_textChanged(const QString &arg1);

    void on_majMaintenant_clicked();
    void on_listeBoutonsOutils_rejected();
    void on_compteRenduMajAuto_customContextMenuRequested(const QPoint &pos);
    void on_parcourirMaj1_clicked();
    void on_parcourirMaj2_clicked();
    void on_mettreAJourTLE_clicked();
    void on_compteRenduMajManuel_customContextMenuRequested(const QPoint &pos);

    void on_listeTLE_itemClicked(QListWidgetItem *item);
    void on_importerTLE_clicked();
    void on_supprimerTLE_clicked();

};

#endif // OUTILS_H
