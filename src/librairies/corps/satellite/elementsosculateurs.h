/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2022  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    elementsosculateurs.h
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Heritage
 * >
 *
 * Description
 * >    Definition des elements osculateurs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    30 decembre 2018
 *
 */

#ifndef ELEMENTSOSCULATEURS_H
#define ELEMENTSOSCULATEURS_H


class Vecteur3D;

class ElementsOsculateurs
{
public:

    /*
     *  Constructeurs
     */
    /**
     * @brief ElementsOsculateurs Constructeur par defaut
     */
    ElementsOsculateurs();

    /*
     * Accesseurs
     */
    double demiGrandAxe() const;
    double excentricite() const;
    double inclinaison() const;
    double ascensionDroiteNoeudAscendant() const;
    double argumentPerigee() const;
    double anomalieVraie() const;
    double anomalieExcentrique() const;
    double anomalieMoyenne() const;
    double apogee() const;
    double perigee() const;
    double periode() const;
    double exCirc() const;
    double eyCirc() const;
    double exCEq() const;
    double eyCEq() const;
    double pso() const;
    double ix() const;
    double iy() const;
    double argumentLongitudeVraie() const;


    /*
     * Constantes publiques
     */

    /*
     * Variables publiques
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief Calcul Calcul des elements osculateurs pour une orbite elliptique
     * D'apres les formules de Fundamental Astrodynamics and applications, 2nd edition, D. Vallado
     * Trajectoires spatiales, O. Zarrouati
     * @param position vecteur position
     * @param vitesse vecteur vitesse
     */
    void Calcul(const Vecteur3D &position, const Vecteur3D &vitesse);


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
    double _demiGrandAxe;
    double _excentricite;
    double _inclinaison;
    double _ascensionDroiteNoeudAscendant;
    double _argumentPerigee;
    double _anomalieVraie;
    double _anomalieExcentrique;
    double _anomalieMoyenne;
    double _apogee;
    double _perigee;
    double _periode;

    // Parametres adaptes
    double _exCirc;
    double _eyCirc;
    double _exCEq;
    double _eyCEq;
    double _pso;
    double _ix;
    double _iy;
    double _argumentLongitudeVraie;


    /*
     * Methodes privees
     */


};

#endif // ELEMENTSOSCULATEURS_H
