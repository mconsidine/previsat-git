/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    ajustementdates.h
 *
 * Localisation
 * >    interface.onglets.telescope
 *
 * Heritage
 * >    QDialog
 *
 * Description
 * >    Ajustement des dates initiale et finale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 mars 2023
 *
 * Date de revision
 * >
 *
 */

#ifndef AJUSTEMENTDATES_H
#define AJUSTEMENTDATES_H

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDateTime>
#include <QDialog>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"


namespace Ui {
class AjustementDates;
}

class AjustementDates : public QDialog
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    explicit AjustementDates(const QDateTime &dateInitiale, const QDateTime &dateFinale, const int pas, QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~AjustementDates();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */

public slots:

    void changeEvent(QEvent *evt);


signals:

    void AjusterDates(const QDateTime &date1, const QDateTime &date2);


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private slots:

    void on_ajustementDateInitiale_valueChanged(int value);
    void on_ajustementDateFinale_valueChanged(int value);
    void on_buttonBox_accepted();


private:

    /*
     * Variables privees
     */
    Ui::AjustementDates *_ui;

    QDateTime _date1;
    QDateTime _date2;
    int _pas;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe AjustementDates
     */
    void Initialisation();


};

#endif // AJUSTEMENTDATES_H
