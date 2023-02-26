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
 * >    25 fevrier 2023
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


/*
 *  Declaration des structures
 */
struct ConditionsPrevisions
{
    bool calcEclipseLune;
    bool eclipse;
    bool effetEclipsePartielle;
    bool extinction;
    bool refraction;
    bool systeme;

    double crepuscule;
    double hauteur;
    double jj1;
    double jj2;
    double magnitudeLimite;
    double offset;
    double pas;

    QString fichier;
    QString ficRes;
    QString unite;
    QStringList listeSatellites;

    Observateur observateur;

    QMap<QString, ElementsOrbitaux> tabElem;


    // Pour les flashs
    double angleLimite;

    // Pour les transits
    bool calcEphemLune;
    bool calcEphemSoleil;
    bool calcTransitLunaireJour;
    double seuilConjonction;
    QList<ElementsOrbitaux> listeElemIss;

    // Pour les evenements orbitaux
    bool passageApogee;
    bool passageNoeuds;
    bool passageOmbre;
    bool passagePso;
    bool transitionsJourNuit;

    // Pour le suivi avec un telescope
    int nbIter;

};


struct ResultatPrevisions
{
    bool eclipse = false;
    bool penombre = false;
    bool penombreMax = false;
    bool transit = false;

    double altitude = 0.;
    double angle = 0.;
    double angleReflexion = 0.;
    double ascensionDroite = 0.;
    double azimut = 0.;
    double azimutSoleil = 0.;
    double declinaison = 0.;
    double distance = 0.;
    double distanceObs = 0.;
    double duree = 0.;
    double hauteur = 0.;
    double hauteurSoleil = 0.;
    double latitude = 0.;
    double longitude = 0.;
    double magnitude = 0.;
    double magnitudeMax = 0.;
    double magnitudeStd = 0.;
    double pso = 0.;

    QString cap;
    QString constellation;
    QString miroir;
    QString nom;
    QString typeEvenement;

    CorpsTransit typeCorps = CorpsTransit::CORPS_SOLEIL;
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
