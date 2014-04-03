/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2014  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    conditions.h
 *
 * Localisation
 * >    previsions
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
 * >    3 avril 2014
 *
 */

#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <string>
#include "librairies/observateur/observateur.h"
#include "librairies/corps/satellite/tle.h"

class Conditions
{
public:

    /* Constructeurs */
    Conditions();

    // Constructeur pour le calcul des previsions de passages
    Conditions(const bool ecart, const bool ecl, const bool ext, const bool refr, const bool syst, const int crep, const int haut,
               const int pas0, const double jj1, const double jj2, const double offset, const double mgn1, const QString &fic,
               const QString &out, const QString &unite, const QStringList &listeSatellites);

    // Constructeur pour le calcul des flashs Iridium
    Conditions(const bool ecart, const bool ext, const bool refr, const bool syst, const int crep, const int haut, const int nbl,
               const char chr, const double ang0, const double jj1, const double jj2, const double offset, const double mgn1,
               const double mgn2, const QString &fic, const QString &out, const QString &unite, const QStringList &tabStsIri,
               const QVector<TLE> &tabtle);

    // Constructeur pour le calcul des evenements orbitaux
    Conditions(const bool apassApogee, const bool apassNoeuds, const bool apassOmbre, const bool apassPso, const bool atransJn,
               const bool ecart, const bool refr, const bool syst, const double jj1, const double jj2, const double offset,
               const QString &fic, const QString &out, const QString &unite, const QStringList &listeSatellites);

    // Constructeur pour le calcul des transits ISS
    Conditions(const bool acalcLune, const bool acalcSoleil, const bool ecart, const bool refr, const bool syst, const int haut,
               const double ageTLE, const double seuilConjonction, const double jj1, const double jj2, const double offset,
               const QString &fic, const QString &out, const QString &unite);
    ~Conditions();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    static void EcrireEntete(const Observateur &observateur, const Conditions &conditions, QVector<TLE> &tabtle,
                             const bool itransit);

    /* Accesseurs */
    bool getEcart() const;
    bool getEcl() const;
    bool getExt() const;
    bool getRefr() const;
    bool getSyst() const;
    double getCrep() const;
    double getHaut() const;
    double getPas0() const;
    double getJj1() const;
    double getJj2() const;
    double getOffset() const;
    double getMgn1() const;
    QString getFic() const;
    QString getOut() const;
    QString getUnite() const;
    QStringList getListeSatellites() const;

    int getNbl() const;
    char getChr() const;
    char getOpe() const;
    double getAng0() const;
    double getMgn2() const;
    QStringList getTabStsIri() const;
    QVector<TLE> getTabtle() const;

    bool getApassApogee() const;
    bool getApassNoeuds() const;
    bool getApassOmbre() const;
    bool getApassPso() const;
    bool getAtransJn() const;

    bool getAcalcLune() const;
    bool getAcalcSol() const;
    double getAgeTLE() const;
    double getSeuilConjonction() const;

protected:

    /* Constantes protegees */

    /* Variables protegees */

    /* Methodes protegees */


private:

    /* Constantes privees */

    /* Variables privees */
    // Donnees communes
    bool _ecart;
    bool _ecl;
    bool _ext;
    bool _refr;
    bool _syst;
    double _crep;
    double _haut;
    double _pas0;
    double _jj1;
    double _jj2;
    double _offset;
    double _mgn1;
    QString _fic;
    QString _out;
    QString _unite;
    QStringList _listeSatellites;

    // Pour le calcul des flashs Iridium
    int _nbl;
    char _chr;
    double _ang0;
    double _mgn2;
    QStringList _tabStsIri;
    QVector<TLE> _tabtle;

    // Pour le calcul des evenements orbitaux
    bool _apassApogee;
    bool _apassNoeuds;
    bool _apassOmbre;
    bool _apassPso;
    bool _atransJn;

    // Pour le calcul des transits ISS
    bool _acalcLune;
    bool _acalcSol;
    double _ageTLE;
    double _seuilConjonction;

    /* Methodes privees */

};

#endif // CONDITIONS_H
