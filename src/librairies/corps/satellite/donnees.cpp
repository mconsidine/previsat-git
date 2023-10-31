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
 * >    31 octobre 2023
 *
 */

#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QObject>
#pragma GCC diagnostic warning "-Wswitch-default"
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

        _norad = donnee.mid(0, 6);
        _cospar = donnee.mid(7, 11).trimmed();
        if (_cospar.isEmpty()) {
            _cospar = QObject::tr("N/A");
        }

        _t1 = donnee.mid(19, 5).toDouble();
        _t2 = donnee.mid(25, 4).toDouble();
        _t3 = donnee.mid(30, 4).toDouble();

        _magnitudeStandard = donnee.mid(35, 5).toDouble();
        _methMagnitude = donnee.at(41).toLatin1();
        _section = donnee.mid(43, 6).toDouble();

        _dateLancement = donnee.mid(50, 10);
        _dateRentree = donnee.mid(61, 10).trimmed();

        _periode = donnee.mid(72, 10).trimmed();
        _perigee = donnee.mid(83, 7).trimmed();
        _apogee = donnee.mid(91, 7).trimmed();

        _inclinaison = donnee.mid(99, 6).trimmed();
        _categorieOrbite = donnee.mid(106, 6).trimmed();

        _pays = donnee.mid(113, 5).trimmed();
        _siteLancement = donnee.mid(119, 5).trimmed();
        _nom = donnee.mid(125).trimmed();
    }

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */

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

QString Donnees::norad() const
{
    return _norad;
}

const QString &Donnees::cospar() const
{
    return _cospar;
}

const QString &Donnees::dateLancement() const
{
    return _dateLancement;
}

const QString &Donnees::dateRentree() const
{
    return _dateRentree;
}

const QString &Donnees::periode() const
{
    return _periode;
}

const QString &Donnees::perigee() const
{
    return _perigee;
}

const QString &Donnees::apogee() const
{
    return _apogee;
}

const QString &Donnees::inclinaison() const
{
    return _inclinaison;
}

const QString &Donnees::categorieOrbite() const
{
    return _categorieOrbite;
}

const QString &Donnees::pays() const
{
    return _pays;
}

const QString &Donnees::siteLancement() const
{
    return _siteLancement;
}

QString Donnees::nom() const
{
    return _nom;
}


/*
 * Modificateurs
 */
void Donnees::setMagnitudeStandard(const double m)
{
    _magnitudeStandard = m;
}


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

