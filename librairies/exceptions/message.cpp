/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2019  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    message.cpp
 *
 * Localisation
 * >    librairies.exceptions
 *
 * Heritage
 * >
 *
 * Description
 * >    Affichage d'un message
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    31 decembre 2018
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QMainWindow>
#include <QMessageBox>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#include "message.h"


QMainWindow *_fenetre;

/*
 * Affichage d'un message dans une boite de message
 */
void Message::Afficher(const QString &message, const MessageType ierr)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    switch (ierr) {

    case INFO:
        QMessageBox::information(_fenetre, QObject::tr("Information"), message);
        break;

    case WARNING:
        QMessageBox::warning(_fenetre, QObject::tr("Avertissement"), message);
        break;

    case ERREUR:
        QMessageBox::critical(_fenetre, QObject::tr("Erreur"), message);

    default:
        break;
    }

    /* Retour */
    return;
}

/*
 * Definition de la fenetre parent (pour centrer l'affichage)
 */
void Message::setFenetreParent(QMainWindow *fenetre)
{
    _fenetre = fenetre;
}
