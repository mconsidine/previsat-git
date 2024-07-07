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
 * >    7 juillet 2024
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

Donnees::Donnees(const QSqlQuery &req)
{
    QMap<QString, QVariant> res;

    QSqlRecord record = req.record();

    for(int i=0; i<record.count(); i++) {
        res.insert(record.fieldName(i), req.value(i));
    }

    _norad = res["norad"].toString();
    _cospar = res["cospar"].toString();
    if (_cospar.isEmpty()) {
        _cospar = QObject::tr("N/A");
    }

    _dateLancement = res["dateLancement"].toString();
    _dateRentree = res["dateRentree"].toString();
    _stsDateRentree = res["stsDateRentree"].toString();
    _stsHeureRentree = res["stsHeureRentree"].toString();

    _masseSec = res["masseSec"].toString();
    _masseTot = res["masseTot"].toString();

    _longueur = res["longueur"].toDouble();
    _diametre = res["diametre"].toDouble();
    _envergure = res["envergure"].toDouble();

    _forme = res["forme"].toString();
    _classe = res["classe"].toString().toLatin1();
    _categorie = res["categorie"].toString();
    _discipline = res["discipline"].toString();

    _magnitudeStandard = res["magnitudeStandard"].toDouble();
    _methMagnitude = res["methMagnitude"].toString();

    _periode = res["periode"].toString();
    _perigee = res["perigee"].toDouble();
    _apogee = res["apogee"].toDouble();

    _inclinaison = res["inclinaison"].toString();
    _orbite = res["orbite"].toString();

    _pays = res["pays"].toString();
    _siteLancement = res["siteLancement"].toString();
    _nom = res["nom"].toString();
}


/*
 * Methodes publiques
 */
/*
 * Recherche des donnees satellites par designation COSPAR
 */
QList<Donnees> Donnees::RequeteCospar(const QSqlDatabase &db, const QString &cospar)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString qLect = QString("select * from Satellite where cospar like '%%1%';").arg(cospar);

    /* Corps de la methode */

    /* Retour */
    return Requete(db, qLect);
}

/*
 * Recherche des donnees satellites par nom de satellite
 */
QList<Donnees> Donnees::RequeteNom(const QSqlDatabase &db, const QString &nom)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString qLect = QString("select * from Satellite where nom like '%%1%';").arg(nom);

    /* Corps de la methode */

    /* Retour */
    return Requete(db, qLect);
}

/*
 * Recherche des donnees satellites par numero NORAD
 */
QList<Donnees> Donnees::RequeteNorad(const QSqlDatabase &db, const QString &norad)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString qLect = QString("select * from Satellite where norad = %1;").arg(norad);

    /* Corps de la methode */

    /* Retour */
    return Requete(db, qLect);
}


/*
 * Accesseurs
 */
QString Donnees::methMagnitude() const
{
    return _methMagnitude;
}

QString Donnees::stsDateRentree() const
{
    return _stsDateRentree;
}

QString Donnees::stsHeureRentree() const
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
/*
 * Requete generique dans la base de donnees
 */
QList<Donnees> Donnees::Requete(const QSqlDatabase &db, const QString &requete)
{
    /* Declarations des variables locales */
    QList<Donnees> res;

    /* Initialisations */
    QSqlQuery q(db);

    /* Corps de la methode */
    if (q.exec(requete)) {
        while (q.next()) {
            res.append(Donnees(q));
        }
    }

    /* Retour */
    return res;
}
