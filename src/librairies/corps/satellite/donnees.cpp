/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    donnees.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    20 novembre 2019
 *
 * Date de revision
 * >
 *
 */


#include "donnees.h"

/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Donnees::Donnees()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _methMagnitude = 'v';
    _magnitudeStandard = 99.;
    _section = 0.;
    _t1 = 0.;
    _t2 = 0.;
    _t3 = 0.;

    /* Retour */
    return;
}

/*
 * Definition a partir des informations
 */
Donnees::Donnees(const QString &donnee)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    if (!donnee.isEmpty()) {

        _cospar = donnee.mid(6, 11).trimmed();
        _t1 = donnee.mid(18, 5).toDouble();
        _t2 = donnee.mid(24, 4).toDouble();
        _t3 = donnee.mid(29, 4).toDouble();
        _magnitudeStandard = donnee.mid(34, 4).toDouble();
        _methMagnitude = donnee.at(39).toLatin1();
        _section = donnee.mid(41, 6).toDouble();
        _dateLancement = donnee.mid(48, 10);
        _periode = donnee.mid(70, 10).trimmed();
        _perigee = donnee.mid(81, 7).trimmed();
        _apogee = donnee.mid(89, 7).trimmed();
        _inclinaison = donnee.mid(97, 6).trimmed();
        _categorieOrbite = donnee.mid(104, 6).trimmed();
        _pays = donnee.mid(111, 5).trimmed();
        _siteLancement = donnee.mid(117, 5).trimmed();
    }

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
char Donnees::methMagnitude() const
{
    return _methMagnitude;
}

double Donnees::magnitudeStandard() const
{
    return _magnitudeStandard;
}

double Donnees::section() const
{
    return _section;
}

double Donnees::t1() const
{
    return _t1;
}

double Donnees::t2() const
{
    return _t2;
}

double Donnees::t3() const
{
    return _t3;
}

QString Donnees::cospar() const
{
    return _cospar;
}

QString Donnees::dateLancement() const
{
    return _dateLancement;
}

QString Donnees::periode() const
{
    return _periode;
}

QString Donnees::perigee() const
{
    return _perigee;
}

QString Donnees::apogee() const
{
    return _apogee;
}

QString Donnees::inclinaison() const
{
    return _inclinaison;
}

QString Donnees::categorieOrbite() const
{
    return _categorieOrbite;
}

QString Donnees::pays() const
{
    return _pays;
}

QString Donnees::siteLancement() const
{
    return _siteLancement;
}


/*
 * Methodes publiques
 */


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */

