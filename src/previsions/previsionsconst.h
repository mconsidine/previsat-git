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
 * >    15 aout 2020
 *
 * Date de revision
 * >    14 octobre 2022
 *
 */

#ifndef PREVISIONSCONST_H
#define PREVISIONSCONST_H

#include <QString>
#include "librairies/corps/satellite/elementsorbitaux.h"
#include "librairies/dates/date.h"
#include "librairies/observateur/observateur.h"


/* Enumerations */
enum class TypeCalcul {
    PREVISIONS,
    EVENEMENTS,
    FLASHS,
    TRANSITS,
    TELESCOPE
};

enum class CorpsTransit {
    CORPS_SOLEIL = 1,
    CORPS_LUNE
};


/* Declaration des structures */

struct ConditionsPrevisions
{
    bool calcEclipseLune;
    bool eclipse;
    bool effetEclipsePartielle;
    bool extinction;
    bool refraction;

    double crepuscule;
    double hauteur;
    double jj1;
    double jj2;
    double magnitudeLimite;
    double pas;

    QString fichier;
    QStringList listeSatellites;

    Observateur observateur;
};

struct ConditionsFlashs
{
    bool calcEclipseLune;
    bool effetEclipsePartielle;
    bool extinction;
    bool refraction;

    double angleLimite;
    double crepuscule;
    double hauteur;
    double jj1;
    double jj2;
    double magnitudeLimite;

    QString fichier;
    QStringList listeSatellites;

    Observateur observateur;
};

struct ConditionsTransits
{
    bool calcEphemLune;
    bool calcEphemSoleil;
    bool calcTransitLunaireJour;
    bool refraction;

    double hauteur;
    double jj1;
    double jj2;
    double seuilConjonction;

    Observateur observateur;
    QList<ElementsOrbitaux> tabElem;
};

struct ConditionsEvenements
{
    bool calcEclipseLune;
    bool passageApogee;
    bool passageNoeuds;
    bool passageOmbre;
    bool passagePso;
    bool transitionsJourNuit;

    double jj1;
    double jj2;

    QString fichier;
    QString unite;
    QStringList listeSatellites;
};

struct ResultatPrevisions
{
    bool eclipse;
    bool penombre;
    bool penombreMax;
    bool transit;

    double altitude;
    double angle;
    double angleReflexion;
    double ascensionDroite;
    double azimut;
    double azimutSoleil;
    double declinaison;
    double distance;
    double distanceObs;
    double duree;
    double hauteur;
    double hauteurSoleil;
    double latitude;
    double longitude;
    double magnitude;
    double magnitudeMax;
    double magnitudeStd;
    double pso;

    QString cap;
    QString constellation;
    QString miroir;
    QString nom;
    QString typeEvenement;

    CorpsTransit typeCorps;
    Date date;
    ElementsOrbitaux elements;
    Observateur obsmax;
};

struct DonneesPrevisions
{
    long long tempsEcoule;
    QList<double> ageElementsOrbitaux;
};


#endif // PREVISIONSCONST_H
