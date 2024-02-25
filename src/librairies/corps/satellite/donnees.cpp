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

#include "librairies/corps/corpsconst.h"
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
    _longueur = 0.;
    _diametre = 0.;
    _envergure = 0.;

    _methMagnitude = 'v';
    _magnitudeStandard = CORPS::MAGNITUDE_INDEFINIE;

    _stsDateRentree = ' ';
    _stsHeureRentree = ' ';

    _apogee = 0.;
    _perigee = 0.;

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

        _dateLancement = donnee.mid(19, 19).trimmed();
        _dateRentree = donnee.mid(39, 19).trimmed();
        _stsDateRentree = donnee.at(59).toLatin1();
        _stsHeureRentree = donnee.at(60).toLatin1();

        _masseSec = donnee.mid(62, 8).trimmed();
        _masseTot = donnee.mid(71, 9).trimmed();

        _longueur = donnee.mid(81, 7).toDouble();
        _diametre = donnee.mid(89, 6).toDouble();
        _envergure = donnee.mid(96, 8).toDouble();

        _forme = donnee.mid(105, 32).trimmed();
        _classe = donnee.at(138).toLatin1();
        _categorie = donnee.mid(140, 4).trimmed();
        _discipline = donnee.mid(145, 16).trimmed();

        _magnitudeStandard = donnee.mid(162, 5).toDouble();
        _methMagnitude = donnee.at(168).toLatin1();

        _periode = donnee.mid(170, 10).trimmed();
        _perigee = donnee.mid(181, 7).toDouble();
        _apogee = donnee.mid(189, 7).toDouble();

        _inclinaison = donnee.mid(197, 6).trimmed();
        _orbite = donnee.mid(204, 6).trimmed();

        _pays = donnee.mid(211, 5).trimmed();
        _siteLancement = donnee.mid(217, 5).trimmed();
        _nom = donnee.mid(223).trimmed();
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

char Donnees::stsDateRentree() const
{
    return _stsDateRentree;
}

char Donnees::stsHeureRentree() const
{
    return _stsHeureRentree;
}

double Donnees::magnitudeStandard() const
{
    return _magnitudeStandard;
}

double Donnees::longueur() const
{
    return _longueur;
}

double Donnees::diametre() const
{
    return _diametre;
}

double Donnees::envergure() const
{
    return _envergure;
}

QString Donnees::periode() const
{
    return _periode;
}

double Donnees::perigee() const
{
    return _perigee;
}

double Donnees::apogee() const
{
    return _apogee;
}

QString Donnees::inclinaison() const
{
    return _inclinaison;
}

QString Donnees::norad() const
{
    return _norad;
}

QString Donnees::cospar() const
{
    return _cospar;
}

QString Donnees::dateLancement() const
{
    return _dateLancement;
}

QString Donnees::dateRentree() const
{
    return _dateRentree;
}

QString Donnees::masseSec() const
{
    return _masseSec;
}

QString Donnees::masseTot() const
{
    return _masseTot;
}

QString Donnees::forme() const
{
    return _forme;
}

QString Donnees::classe() const
{
    return _classe;
}

QString Donnees::categorie() const
{
    return _categorie;
}

QString Donnees::discipline() const
{
    return _discipline;
}

QString Donnees::orbite() const
{
    return _orbite;
}

QString Donnees::pays() const
{
    return _pays;
}

QString Donnees::siteLancement() const
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

