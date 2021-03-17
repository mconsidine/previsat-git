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
 * >    previsionsconst.h
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >     Constantes liees aux previsions
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >
 *
 * Date de revision
 * >
 *
 */

#ifndef PREVISIONSCONST_H
#define PREVISIONSCONST_H

#include <QString>
#include "librairies/dates/date.h"
#include "librairies/observateur/observateur.h"


/* Enumerations */
enum TypeCalcul {
    PREVISIONS,
    EVENEMENTS,
    FLASHS,
    TRANSITS,
    TELESCOPE
};

enum CorpsTransit {
    CORPS_SOLEIL = 1,
    CORPS_LUNE
};


/* Declaration des constantes */

struct ConditionsPrevisions
{
    double jj1;
    double jj2;
    double offset;
    bool systeme;
    double pas;

    QString unite;

    double crepuscule;
    double hauteur;
    double magnitudeLimite;
    double angleLimite;
    double seuilConjonction;

    bool calcEclipseLune;
    bool ecart;
    bool eclipse;
    bool effetEclipsePartielle;
    bool extinction;
    bool refraction;
    bool calcTransitLunaireJour;
    bool calcEphemSoleil;
    bool calcEphemLune;
    bool passageNoeuds;
    bool passageOmbre;
    bool passageApogee;
    bool passagePso;
    bool transitionsJourNuit;

    int nbIter;

    QString fichier;
    QStringList listeSatellites;
    Observateur observateur;

    QString ficRes;
};

struct ResultatPrevisions
{
    QString nom;

    Date date;
    double azimut;
    double hauteur;
    double ascensionDroite;
    double declinaison;
    QString constellation;
    double magnitude;
    double magnitudeStd;
    bool penombre;
    double distance;
    double altitude;

    double azimutSoleil;
    double hauteurSoleil;

    // Pour les flashs
    double angleReflexion;
    double distanceObs;
    double magnitudeMax;
    bool penombreMax;
    QString miroir;
    QString cap;
    Observateur obsmax;

    // Pour les transits ISS
    bool eclipse;
    bool transit;
    CorpsTransit typeCorps;
    double angle;

    // Pour les evenements orbitaux
    double pso;
    double longitude;
    double latitude;
    QString typeEvenement;
};

struct DonneesPrevisions
{
    long long tempsEcoule;
    QList<double> ageTle;
};


#endif // PREVISIONSCONST_H
