/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    telecharger.h
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
 * >    10 mars 2012
 *
 * Date de revision
 * >    3 octobre 2015
 *
 */

#ifndef TELECHARGER_H
#define TELECHARGER_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QMainWindow>
#include <QNetworkReply>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"

namespace Ui {
    class Telecharger;
}

class Telecharger : public QMainWindow
{
    Q_OBJECT

public:

    /* Constructeurs */
    /**
     * @brief Telecharger Constructeur pour la fenetre de telechargement
     * @param idirHttp indice du repertoire cible
     * @param fenetreParent fenetre
     */
    explicit Telecharger(const int idirHttp, QWidget *fenetreParent = 0);

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief AjoutFichier Ajout d'un fichier dans la liste de fichiers a telecharger
     * @param url adresse du fichier
     */
    void AjoutFichier(const QUrl &url);

    ~Telecharger();

    /* Accesseurs */


signals:

    /**
     * @brief TelechargementFini Signal du telechargement fini
     */
    void TelechargementFini();


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private slots:

    /**
     * @brief on_fermer_clicked Fermeture de la fenetre
     */
    void on_fermer_clicked();

    /**
     * @brief on_interrogerServeur_clicked Interrogation du serveur
     */
    void on_interrogerServeur_clicked();

    /**
     * @brief MessageErreur Message d'erreur pour le telechargement
     */
    void MessageErreur(QNetworkReply::NetworkError) const;

    /**
     * @brief Enregistrer Enregistrement du fichier
     */
    void Enregistrer() const;

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

    /**
     * @brief FinEnregistrementFichier Gestion de l'enregistrement des fichiers telecharges
     */
    void FinEnregistrementFichier();

    /**
     * @brief EcritureFichier Ecriture du fichier telecharge
     */
    void EcritureFichier();

    /**
     * @brief on_telecharger_clicked Lancement du telechargement
     */
    void on_telecharger_clicked();

private:

    /* Constantes privees */

    /* Variables privees */
    Ui::Telecharger *ui;

    /* Methodes privees */


};

#endif // TELECHARGER_H
