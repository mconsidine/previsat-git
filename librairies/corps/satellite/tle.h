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
 * >    tle.h
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

#ifndef TLE_H
#define TLE_H

#include <QVector>
#include <QStringList>
#include "librairies/dates/date.h"

class TLE
{
public:

    /* Constructeurs */
    TLE();
    TLE(const QString ligne1, const QString ligne2);
    ~TLE();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    static int VerifieFichier(const QString nomFichier, const bool alarm);
    static void LectureFichier(const QString &nomFichier, const QStringList &listeSatellites,
                               QVector<TLE> &tabtle);
    static void MiseAJourFichier(const QString ficOld, const QString ficNew, QStringList &compteRendu);

    /* Accesseurs */
    double getArgpo() const;
    double getBstar() const;
    double getEcco() const;
    Date getEpoque() const;
    double getInclo() const;
    QString getLigne1() const;
    QString getLigne2() const;
    double getMo() const;
    int getNbOrbites() const;
    double getNo() const;
    QString getNom() const;
    QString getNorad() const;
    double getOmegao() const;


protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    int _nbOrbites;
    double _argpo;
    double _bstar;
    double _ecco;
    double _inclo;
    double _mo;
    double _no;
    double _omegao;
    Date _epoque;

    QString _nom;
    QString _ligne1;
    QString _ligne2;
    QString _norad;

    /* Methodes privees */
    static bool CheckSum(const QString ligne);
    static void VerifieLignes(const QString li1, const QString li2);

};

#endif // TLE_H
