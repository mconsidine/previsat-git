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
 * >    3 juin 2015
 *
 */

#ifndef OBSERVATEUR_H
#define OBSERVATEUR_H

#include "librairies/dates/date.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"
#include "librairies/maths/mathsConstants.h"
#include "librairies/maths/matrice3d.h"

class Observateur
{
public:

    /* Constructeurs */
    /**
     * @brief Observateur Constructeur par defaut
     */
    Observateur();

    /**
     * @brief Observateur Constructeur a partir des coordonnees geographiques d'un lieu d'observation
     * @param nom nom du lieu d'observation
     * @param lon longitude en degres (negative a l'est)
     * @param lat latitude en degres (negative au sud)
     * @param alt altitude en metres
     */
    Observateur(const QString &nom, const double lon, const double lat, const double alt);

    /**
     * @brief Observateur Definition a partir d'un objet Observateur
     * @param observateur lieu d'observation
     */
    Observateur(const Observateur &observateur);

    /**
     * @brief Observateur Constructeur a partir des donnees relatives au lieu d'observation a une date donnee
     * (pour le calcul des previsions)
     * @param pos position de l'observateur
     * @param vit vitesse de l'observateur
     * @param matRotHz matrice de rotation pour le calcul des coordonnees horizontales
     * @param aaerVal aray
     * @param arayVal aaer
     */
    Observateur(const Vecteur3D &pos, const Vecteur3D &vit, const Matrice3D &matRotHz, const double aaerVal, const double arayVal);

    ~Observateur();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief CalculPosVit Calcul de la position et de la vitesse du lieu d'observation
     * @param date date
     */
    void CalculPosVit(const Date &date);

    /**
     * @brief CalculTempsSideralGreenwich Calcul du temps sideral de Greenwich
     * D'apres la formule donnee dans l'Astronomical Algorithms 2nd edition de Jean Meeus, p88
     * @param date date
     * @return temps sideral de Greenwich
     */
    static double CalculTempsSideralGreenwich(const Date &date);

    /**
     * @brief CalculCap Calcul du cap d'un lieu d'observation par rapport a un autre
     * @param lieuDistant lieu distant
     * @return valeur du cap
     */
    double CalculCap(const Observateur &lieuDistant);

    /**
     * @brief CalculDistance Calcul de la distance entre 2 lieux d'observation mesuree le long de la surface terrestre
     * en tenant compte de l'applatissement du globe terrestre, mais sans prise en compte de l'altitude
     * Astronomical Algorithms 2nd edition de Jean Meeus, p85
     * @param observateur lieu d'observation distant
     * @return distance entre les 2 lieux d'observation
     */
    double CalculDistance(const Observateur &observateur) const;

    /**
     * @brief CalculIntersectionEllipsoide Calcul des coordonnees geographiques du lieu a l'intersection d'un vecteur pointant
     * vers la Terre et de l'ellipsoide terrestre
     * @param date date
     * @param origine vecteur origine
     * @param direction direction du vecteur vers l'ellipsoide
     * @return lieu pointe par le vecteur
     */
    static Observateur CalculIntersectionEllipsoide(const Date &date, const Vecteur3D origine, const Vecteur3D direction);

    /* Accesseurs */
    double aaer() const;
    double altitude() const;
    double aray() const;
    double latitude() const;
    double longitude() const;
    QString nomlieu() const;
    Vecteur3D position() const;
    Matrice3D rotHz() const;
    double tempsSideralGreenwich() const;
    Vecteur3D vitesse() const;


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

    Matrice3D _rotHz;

    /* Methodes privees */

};

#endif // OBSERVATEUR_H
