/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    corps.h
 *
 * Localisation
 * >    librairies.corps
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

#ifndef CORPS_H
#define CORPS_H

#include <QList>
#include <QPointF>
#include <QVector>
#include <string>
#include "librairies/dates/date.h"
#include "librairies/maths/vecteur3d.h"
#include "librairies/observateur/observateur.h"

class Corps
{
public:

    /* Constructeurs */
    Corps();
    ~Corps();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void CalculCoordEquat(const Observateur &observateur);
    void CalculCoordHoriz(const Observateur &observateur, const bool acalc = true);
    void CalculCoordTerrestres(const Observateur &observateur);
    void CalculCoordTerrestres(const Date &date);
    void CalculZoneVisibilite();

    /* Accesseurs */
    double getAltitude() const;
    double getAscensionDroite() const;
    double getAzimut() const;
    QString getConstellation() const;
    double getDeclinaison() const;
    Vecteur3D getDist() const;
    double getDistance() const;
    double getHauteur() const;
    double getLatitude() const;
    double getLongitude() const;
    Vecteur3D getPosition() const;
    double getRangeRate() const;
    bool isVisible() const;
    Vecteur3D getVitesse() const;
    QVector<QPointF> getZone() const;

    /* Modificateurs */
    void setPosition(Vecteur3D position);


protected:

    /* Constantes protegees */

    /* Variables protegees */
    // Coordonnees horizontales
    double _azimut;
    double _hauteur;
    double _distance;

    // Coordonnees equatoriales
    double _ascensionDroite;
    double _declinaison;
    QString _constellation;

    // Coordonnees terrestres
    double _longitude;
    double _latitude;
    double _altitude;

    bool _visible;
    double _rangeRate;

    // Coordonnees cartesiennes
    Vecteur3D _position;
    Vecteur3D _vitesse;
    Vecteur3D _dist;

    // Zone de visibilite
    QVector<QPointF> _zone;

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */

    /* Methodes privees */
    void CalculLatitudeAltitude();
    void InitTabConstellations();

};

#endif // CORPS_H
