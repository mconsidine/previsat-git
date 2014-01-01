/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2014  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    17 decembre 2013
 *
 */

#ifndef TELECHARGER_H
#define TELECHARGER_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QMainWindow>
#include <QNetworkReply>

namespace Ui {
    class Telecharger;
}

class Telecharger : public QMainWindow
{
    Q_OBJECT

public:
    explicit Telecharger(const int idirHttp, QWidget *fenetreParent = 0);
    void AjoutFichier(const QUrl &url);
    ~Telecharger();

signals:
    void TelechargementFini();

private slots:
    void on_fermer_clicked();
    void on_interrogerServeur_clicked();
    void MessageErreur(QNetworkReply::NetworkError) const;
    void Enregistrer() const;
    void ProgressionTelechargement(qint64 recu, qint64 total) const;
    void TelechargementSuivant();
    void FinEnregistrementFichier();
    void EcritureFichier();

    void on_telecharger_clicked();

private:
    Ui::Telecharger *ui;
};

#endif // TELECHARGER_H
