/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#ifndef PREVISAT_H
#define PREVISAT_H

#include <QKeyEvent>
#include <QMainWindow>

namespace Ui {
class PreviSat;
}

class PreviSat : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviSat(QWidget *parent = 0);
    void Initialisations();


    ~PreviSat();

private:
    Ui::PreviSat *ui;
    void InitFicObs(const bool alarm);
    void InitFicMap();

    void AffichageDonnees();
    void AffichageCourbes();
    void AffichageCiel() const;
    void AffichageLieuObs();
    void AfficherListeSatellites(const QString fichier, const QStringList listeSat);
    void EnchainementCalculs();
    void EnregistrerOnglet() const;
    void MajWebTLE(const bool alarm) const;
    void VerifAgeTLE() const;
    void EcritureListeRegistre();

private slots:
    void GestionTempsReel();
    void SauveOngletGeneral();
    void SauveOngletElementsOsculateurs();
    void SauveOngletInformations();
    bool DecompressionFichierGz(const QString fichierGz, const QString fichierDecompresse);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

    void on_maximise_clicked();
    void on_actionOuvrir_fichier_TLE_activated();
    void on_actionEnregistrer_activated();
    void on_actionTelecharger_les_mises_jour_activated();
    void on_actionDonnez_votre_avis_activated();
    void on_actionWww_space_track_org_activated();
    void on_actionWww_celestrak_com_activated();
    void on_actionRapport_de_bug_activated();
    void on_actionAstropedia_free_fr_activated();
    void on_affichageCiel_clicked();
};

#endif // PREVISAT_H
