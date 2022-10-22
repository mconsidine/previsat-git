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
 * >    27 aout 2022
 *
 */

#ifndef OUTILS_H
#define OUTILS_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDialog>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class Outils;
}

class QPlainTextEdit;

class Outils : public QDialog
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Outils Constructeur par defaut
     * @param parent fenetre parent
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

    QAction *_copier;


    /*
     * Methodes privees
     */
    /**
     * @brief EcritureCompteRenduMaj Ecriture du compte-rendu de mise a jour des TLE
     * @param compteRendu compte-rendu
     * @param compteRenduMaj zone de texte ou ecrire le compte-rendu
     */
    void EcritureCompteRenduMaj(const QStringList &compteRendu, QPlainTextEdit * compteRenduMaj);

    /**
     * @brief InitListeDomaines Initialisation de la liste deroulante des noms de domaine
     */
    void InitListeDomaines();


private slots:

    /**
     * @brief Copier Copier les compte-rendus de mise a jour des TLE dans le presse-papier
     */
    void Copier();

    /**
     * @brief ProgressionElem Affichage de la progression de telechargement des elements orbitaux
     * @param octetsRecus octets recus
     * @param octetsTotal octets au total
     * @param vitesse vitesse de telechargement
     * @param unite unite de vitesse de telechargement
     */
    void ProgressionElem(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);

    /**
     * @brief ProgressionElem Affichage de la progression de telechargement des TLE
     * @param octetsRecus octets recus
     * @param octetsTotal octets au total
     * @param vitesse vitesse de telechargement
     * @param unite unite de vitesse de telechargement
     */
    void ProgressionTLE(const int octetsRecus, const int octetsTotal, const double vitesse, const QString &unite);

    void on_listeOutils_currentRowChanged(int currentRow);
    void on_serveur_currentTextChanged(const QString &arg1);
    void on_listeGroupeElem_currentRowChanged(int currentRow);
    void on_creationGroupe_clicked();
    void on_ajoutFichiersElem_clicked();
    void on_majGroupe_clicked();

    void on_majMaintenant_clicked();
    void on_listeBoutonsOutils_rejected();
    void on_compteRenduMajAuto_customContextMenuRequested(const QPoint &pos);
    void on_parcourirMaj1_clicked();
    void on_parcourirMaj2_clicked();
    void on_mettreAJourTLE_clicked();
    void on_compteRenduMajManuel_customContextMenuRequested(const QPoint &pos);

};

#endif // OUTILS_H
