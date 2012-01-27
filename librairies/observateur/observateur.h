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
 * >    observateur.h
 *
 * Localisation
 * >    librairies.observateur
 *
 * Heritage
 * >
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

#ifndef OBSERVATEUR_H
#define OBSERVATEUR_H

#include "librairies/maths/mathConstants.h"
#include "librairies/maths/matrice.h"
#include "librairies/dates/date.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"

class Observateur
{
public:

    /* Constructeurs */
    Observateur();
    Observateur(const QString nomlieu, const double longitude, const double latitude, const double altitude);
    Observateur(const Observateur &observateur);
    Observateur(const Vecteur3D &position, const Vecteur3D &vitesse, const Matrice &rotHz, const double aaer,
                const double aray);
    ~Observateur();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void CalculPosVit(const Date &date);
    static double CalculTempsSideralGreenwich(const Date &date);
    double CalculDistance(const Observateur &observateur) const;
    static Observateur CalculIntersectionEllipsoide(const Date &date, Vecteur3D origine, Vecteur3D direction);

    /* Accesseurs */
    double getAaer() const;
    double getAltitude() const;
    double getAray() const;
    double getLatitude() const;
    double getLongitude() const;
    QString getNomlieu() const;
    Vecteur3D getPosition() const;
    Matrice getRotHz() const;
    double getTempsSideralGreenwich() const;
    Vecteur3D getVitesse() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    // Coordonnees geographiques
    double _longitude;
    double _latitude;
    double _altitude;
    QString _nomlieu;

    double _coslat;
    double _sinlat;
    double _rayon;
    double _posZ;

    double _aaer;
    double _aray;

    double _tempsSideralGreenwich;

    Vecteur3D _position;
    Vecteur3D _vitesse;

    Matrice _rotHz;

    /* Methodes privees */

};

#endif // OBSERVATEUR_H
