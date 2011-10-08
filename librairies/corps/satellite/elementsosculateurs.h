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
 * >    elementsosculateurs.h
 *
 * Localisation
 * >    librairies.corps.satellite
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

#ifndef ELEMENTSOSCULATEURS_H
#define ELEMENTSOSCULATEURS_H

#include "librairies/dates/date.h"
#include "librairies/maths/vecteur3d.h"

class ElementsOsculateurs
{
public:

    /* Constructeurs */
    ElementsOsculateurs();
    ~ElementsOsculateurs();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    void CalculElementsOsculateurs(Vecteur3D &position, Vecteur3D &vitesse);

    /* Accesseurs */
    double getAnomalieExcentrique() const;
    double getAnomalieMoyenne() const;
    double getAnomalieVraie() const;
    double getApogee() const;
    double getArgumentPerigee() const;
    double getAscensionDroiteNA() const;
    double getDemiGrandAxe() const;
    double getExcentricite() const;
    double getInclinaison() const;
    double getPerigee() const;
    double getPeriode() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
     double _demiGrandAxe;
     double _excentricite;
     double _inclinaison;
     double _ascDroiteNA;
     double _argumentPerigee;
     double _anomalieVraie;
     double _anomalieMoyenne;
     double _anomalieExcentrique;
     double _apogee;
     double _perigee;
     double _periode;

    /* Methodes privees */

};

#endif // ELEMENTSOSCULATEURS_H
