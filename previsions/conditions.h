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
 * >    5 septembre 2015
 *
 */

#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <string>
#include "librairies/corps/satellite/tle.h"
#include "librairies/observateur/observateur.h"

class Conditions
{
public:

    /* Constructeurs */
    /**
     * @brief Conditions Constructeur par defaut
     */
    Conditions();

    /**
     * @brief Conditions Constructeur pour le calcul des previsions de passages
     * @param pecEcart Prise en compte de l'ecart heure legale - UTC
     * @param eclipse Prise en compte de l'etat d'eclipse du satellite
     * @param extinction Prise en compte de l'extinction atmospherique
     * @param refraction Prise en compte de la refraction
     * @param systeme systeme horaire
     * @param crepuscule valeur de la hauteur du Soleil
     * @param haut hauteur minimale du satellite
     * @param pas0 pas de generation des previsions
     * @param jj1 jour julien initial
     * @param jj2 jour julien final
     * @param offset ecart heure legale - UTC
     * @param mgn1 magnitude limite
     * @param fic fichier de TLE
     * @param out fichier de previsions
     * @param unite unite de distance
     * @param listeSat liste des satellites
     */
    Conditions(const bool pecEcart, const bool eclipse, const bool extinction, const bool refraction, const bool systeme,
               const int crepuscule, const int haut, const int pas0, const double jj1, const double jj2, const double offset,
               const double mgn1, const QString &fic, const QString &out, const QString &unite, const QStringList &listeSat);

    /**
     * @brief Conditions Constructeur pour le calcul des flashs Iridium
     * @param ecart Prise en compte de l'ecart heure legale - UTC
     * @param ext Prise en compte de l'extinction atmospherique
     * @param refr Prise en compte de la refraction
     * @param syst systeme horaire
     * @param chrono tri par ordre chronologique
     * @param panSol prise en compte des panneaux solaires
     * @param crepuscule valeur de la hauteur du Soleil
     * @param hauteur hauteur minimale du satellite
     * @param nbLig nombre de lignes pour l'ecriture du flash
     * @param angle0 angle maximal de reflexion
     * @param jourJulien1 jour julien initial
     * @param jourJulien2 jour julien final
     * @param offsetUTC ecart heure legale - UTC
     * @param magn1 magnitude limite (nuit)
     * @param magn2 magnitude limite (jour)
     * @param ficEnt fichier de TLE
     * @param ficOut fichier de previsions
     * @param unit unite de distance
     * @param tabStsIridium tableau de statut des Iridium
     * @param tabTLEIri tableau des TLE des satellites Iridium
     */
    Conditions(const bool ecart, const bool ext, const bool refr, const bool syst, const bool chrono, const bool panSol,
               const int crepuscule, const int hauteur, const int nbLig, const double angle0, const double jourJulien1,
               const double jourJulien2, const double offsetUTC, const double magn1, const double magn2, const QString &ficEnt,
               const QString &ficOut, const QString &unit, const QStringList &tabStsIridium, const QVector<TLE> &tabTLEIri);

    /**
     * @brief Conditions Constructeur pour le calcul des evenements orbitaux
     * @param apassageApogee Calcul des passages aux perigees/apogees
     * @param apassageNoeuds Calcul des passages aux noeuds
     * @param apassageOmbre Calcul des passages ombre/penombre/lumiere
     * @param apassagePso Calcul des passages aux PSO 0° et 180°
     * @param atransitionJn Calcul des passages aux transitions jour/nuit
     * @param pecEcart Prise en compte de l'ecart heure legale - UTC
     * @param refraction Prise en compte de la refraction
     * @param systeme systeme horaire
     * @param jourJulien1 jour julien initial
     * @param jourJulien2 jour julien final
     * @param offsetUTC ecart heure legale - UTC
     * @param ficEnt fichier de TLE
     * @param ficOut fichier de previsions
     * @param unit unite de distance
     * @param listeSat liste des satellites
     */
    Conditions(const bool apassageApogee, const bool apassageNoeuds, const bool apassageOmbre, const bool apassagePso,
               const bool atransitionJn, const bool pecEcart, const bool refraction, const bool systeme, const double jourJulien1,
               const double jourJulien2, const double offsetUTC, const QString &ficEnt, const QString &ficOut, const QString &unit,
               const QStringList &listeSat);

    /**
     * @brief Conditions Constructeur pour le calcul des transits ISS
     * @param acalculLune Calcul des transits/conjonctions avec la Lune
     * @param acalculSoleil Calcul des transits/conjonctions avec le Soleil
     * @param pecEcart Prise en compte de l'ecart heure legale - UTC
     * @param refraction Prise en compte de la refraction
     * @param systeme systeme horaire
     * @param hauteur hauteur minimale du satellite
     * @param age age du TLE
     * @param seuilConj seuil maximal de conjonction
     * @param jourJulien1 jour julien initial
     * @param jourJulien2 jour julien final
     * @param offsetUTC ecart heure legale - UTC
     * @param ficEnt fichier de TLE
     * @param ficOut fichier de previsions
     * @param unit unite de distance
     */
    Conditions(const bool acalculLune, const bool acalculSoleil, const bool pecEcart, const bool refraction, const bool systeme,
               const int hauteur, const double age, const double seuilConj, const double jourJulien1, const double jourJulien2,
               const double offsetUTC, const QString &ficEnt, const QString &ficOut, const QString &unit);

    ~Conditions();

    /* Constantes publiques */

    /* Variables publiques */

    /* Methodes publiques */
    /**
     * @brief EcrireEntete Ecriture de l'entete du fichier de previsions
     * @param observateur observateur
     * @param conditions conditions d'observation
     * @param tabtle tableau de TLE
     * @param itransit car du calcul des transits ISS
     */
    static void EcrireEntete(const Observateur &observateur, const Conditions &conditions, QVector<TLE> &tabtle, const bool itransit);

    /* Accesseurs */
    bool ecart() const;
    bool ecl() const;
    bool ext() const;
    bool refr() const;
    bool syst() const;
    double crep() const;
    double haut() const;
    double pas0() const;
    double jj1() const;
    double jj2() const;
    double offset() const;
    double mgn1() const;
    QString fic() const;
    QString out() const;
    QString unite() const;
    QStringList listeSatellites() const;

    int nbl() const;
    bool chr() const;
    bool psol() const;
    char ope() const;
    double ang0() const;
    double mgn2() const;
    QStringList tabStsIri() const;
    QVector<TLE> tabtle() const;

    bool apassApogee() const;
    bool apassNoeuds() const;
    bool apassOmbre() const;
    bool apassPso() const;
    bool atransJn() const;

    bool acalcLune() const;
    bool acalcSol() const;
    double ageTLE() const;
    double seuilConjonction() const;

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
    bool _chr;
    bool _psol;
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
