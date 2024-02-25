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
 * >    informationsiss.h
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Informations Station Spatiale
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    26 juin 2022
 *
 * Date de revision
 * >     27 aout 2022
 *
 */

#ifndef INFORMATIONSISS_H
#define INFORMATIONSISS_H

#include <QFrame>


namespace Ui {
class InformationsISS;
}

class InformationsISS : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief InformationsISS Constructeur par defaut
     * @param[in] parent parent
     */
    explicit InformationsISS(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~InformationsISS();


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

    void show();


signals:

    void AfficherMessageStatut(const QString &, const int duree = -1);


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
    Ui::InformationsISS *_ui;


    /*
     * Methodes privees
     */

private slots:

    void on_majEvenementsIss_clicked();

};

#endif // INFORMATIONSISS_H
