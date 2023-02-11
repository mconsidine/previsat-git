/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    listwidgetitem.h
 *
 * Localisation
 * >    interface
 *
 * Heritage
 * >    QListWidgetItem
 *
 * Description
 * >    Liste personnalisee
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >
 *
 * Date de revision
 * >
 *
 */

#ifndef LISTWIDGETITEM_H
#define LISTWIDGETITEM_H

#include <QListWidgetItem>


class ListWidgetItem : public QListWidgetItem
{
public:

    /*
     *  Constructeurs
     */
    explicit ListWidgetItem(const QString &text, QListWidget *listview = nullptr) :
        QListWidgetItem(text, listview) {
    }


    /*
     * Accesseurs
     */

    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    virtual bool operator < (const QListWidgetItem & other) const {
        return  (data(Qt::DisplayRole) == other.data(Qt::DisplayRole)) ?
                    (data(Qt::UserRole).toString() < other.data(Qt::UserRole).toString()) : QListWidgetItem::operator < (other);
    }


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

    /*
     * Methodes privees
     */


};

#endif // LISTWIDGETITEM_H
