/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    planete.cpp
 *
 * Localisation
 * >    librairies.corps.systemesolaire
 *
 * Heritage
 * >    Corps
 *
 * Description
 * >    Utilitaires lies a la position des planetes
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    28 janvier 2012
 *
 * Date de revision
 * >
 * >    4 septembre 2012
 */

#include <QStringList>
#include "planete.h"

bool Planete::_init;
static QStringList nomPlanetes;

static const double _tabPlanetes[7][6][4] = {
    // Mercure
    {
        { 252.250906, 149474.0722491, 0.00030350, 0.000000018 }, // Longitude moyenne
        { 0.387098310, 0., 0., 0. }, // Demi-grand axe
        { 0.20563175, 0.000020407, -0.0000000283, -0.00000000018 }, //Excentricite
        { 7.004986, 0.0018215, -0.00001810, 0.000000056 }, // Inclinaison
        { 48.330893, 1.1861883, 0.00017542, 0.000000215 }, // Longitude du noeud ascendant
        { 77.456119, 1.5564776, 0.00029544, 0.000000009 }  // Longitude du perihelie
    },
    // Venus
    {
        { 181.979801, 58519.2130302, 0.00031014, 0.000000015 },
        { 0.723329820, 0., 0., 0. },
        { 0.00677192, -0.000047765, 0.0000000981, 0.00000000046 },
        { 3.394662, 0.0010037, -0.00000088, -0.000000007 },
        { 76.679920, 0.9011206, 0.00040618, -0.000000093 },
        { 131.563703, 1.4022288, -0.00107618, -0.000005678 }
    },
    // Mars
    {
        { 355.433000, 19141.6964471, 0.00031052, 0.000000016 },
        { 1.523679342, 0., 0., 0. },
        { 0.09340065, 0.000090484, -0.0000000806, -0.00000000025 },
        { 1.849726, -0.0006011, 0.00001276, -0.000000007 },
        { 49.558093, 0.7720959, 0.00001557, 0.000002267 },
        { 336.060234, 1.8410449, 0.00013477, 0.000000536 }
    },
    // Jupiter
    {
        { 34.351519, 3036.3027748, 0.00022330, 0.000000037 },
        { 5.202603209, 0.0000001913, 0., 0. },
        { 0.04849793, 0.000163225, -0.0000004714, -0.00000000201 },
        { 1.303267, -0.0054965, 0.00000466, -0.000000002 },
        { 100.464407, 1.0209774, 0.00040315, 0.000000404 },
        { 14.331207, 1.6126352, 0.00103042, 0.000000037 }
    },
    // Saturne
    {
        { 50.077444, 1223.5110686, 0.00051908, -0.000000030 },
        { 9.554909192, -0.0000021390, 0.000000004, 0. },
        { 0.05554814, -0.000346641, -0.0000006436, 0.00000000340 },
        { 2.488879, -0.0037362, -0.00001519, 0.000000087 },
        { 113.665503, 0.8770880, -0.00012176, -0.000002249 },
        { 93.057237, 1.9637613, 0.00083753, 0.000004928 }
    },
    // Uranus
    {
        { 314.055005, 429.8640561, 0.00030390, 0.000000026 },
        { 19.218446062, -0.0000000372, 0.00000000098, 0. },
        { 0.04638122, -0.000027293, 0.0000000789, 0.00000000024 },
        { 0.773197, 0.0007744, 0.00003749, -0.000000092 },
        { 74.005957, 0.5211278, 0.00133947, 0.000018484 },
        { 173.005957, 1.4863790, 0.00021406, 0.0000000434 }
    },
    // Neptune
    {
        { 304.348665, 219.8833092, 0.00030882, 0.000000018 },
        { 30.110386869, -0.0000001663, 0.00000000069, 0. },
        { 0.00945575, 0.000006033, 0., -0.00000000005 },
        { 1.769953, -0.0093082, -0.000000708, 0.000000027 },
        { 131.784057, 1.1022039, 0.00025952, -0.000000637 },
        { 48.120276, 1.4262957, 0.00038434, 0.000000020 }
    }
};

Planete::Planete(const int iplanete)
{
    _iplanete = iplanete;
    if (!_init)
        nomPlanetes << QObject::tr("Mercure") << QObject::tr("V�nus") << QObject::tr("Mars") << QObject::tr("Jupiter") <<
                       QObject::tr("Saturne") << QObject::tr("Uranus") << QObject::tr("Neptune");
    _init = true;
    for(int i=0; i<6; i++)
        _elem[i] = 0.;
}

/*
 * Calcul de la position d'une planete
 */
void Planete::CalculPosition(const Date &date, const Soleil &soleil)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Vecteur3D solpos = soleil.getPosition() * (1. / UA2KM);

    /* Corps de la methode */
    // Calcul des elements orbitaux
    CalculElements(date);

    // Calcul des coordonnees spheriques heliocentriques
    CalculCoordonneesSpheriques();

    // Coordonnees cartesiennes equatoriales
    _position = Sph2Cart(_positionSph, date);

    // Position geocentrique
    _dist = _position + solpos;

    // Distance geocentrique (UA)
    _distance = _dist.Norme();

    // Prise en compte de l'aberration
    const double jj2 = date.getJourJulienUTC() - 0.0057755183 * _distance;
    const Date date2(jj2, 0., false);

    CalculElements(date2);
    CalculCoordonneesSpheriques();
    _position = (Sph2Cart(_positionSph, date2) + solpos) * UA2KM;

    /* Retour */
    return;
}

/*
 * Calcul des elements orbitaux moyens d'une planete
 */
void Planete::CalculElements(const Date &date)
{
    /* Declarations des variables locales */
    double tu[4];

    /* Initialisations */
    tu[0] = 1.;
    tu[1] = date.getJourJulienUTC() * NB_SIECJ_PAR_JOURS;
    tu[2] = tu[1] * tu[1];
    tu[3] = tu[2] * tu[1];

    /* Corps de la methode */
    // Calcul des elements orbitaux
    for(int i=0; i<6; i++) {
        _elem[i] = 0.;
        for(int j=0; j<4; j++)
            _elem[i] += _tabPlanetes[_iplanete][i][j] * tu[j];
    }

    // Reduction des elements
    _elem[0] = fmod(_elem[0], T360) * DEG2RAD;
    for(int i=3; i<6; i++)
        _elem[i] *= DEG2RAD;

    /* Retour */
    return;
}

/*
 * Calcul des coordonnees spheriques ecliptiques d'une planete
 */
void Planete::CalculCoordonneesSpheriques()
{
    /* Declarations des variables locales */
    double u, u1;

    /* Initialisations */

    /* Corps de la methode */
    // Anomalie moyenne
    double na = atan(tan(_elem[5] - _elem[4]) / cos(_elem[3]));
    if (cos(_elem[5] - _elem[4]) < 0.)
        na += PI;
    double nm = atan(tan(_elem[0] - _elem[4]) / cos(_elem[3]));
    if (cos(_elem[0] - _elem[4] - nm) < 0.)
        nm += PI;
    const double m = nm - na;

    // Anomalie excentrique (equation de Kepler)
    u = m;
    do {
        u1 = u;
        u = u1 + (m + _elem[2] * sin(u1) - u1) / (1. - _elem[2] * cos(u1));
    } while (fabs(u - u1) > 1.e-9);

    // Anomalie vraie
    const double v = 2. * atan(sqrt((1. + _elem[2]) / (1. - _elem[2])) * tan(0.5 * u));

    // Longitude ecliptique vraie
    double nl = atan(tan(na + v) * cos(_elem[3]));
    if (cos(na + v - nl) < 0.)
        nl += PI;
    const double l = _elem[4] + nl;

    // Latitude ecliptique
    const double b = atan(sin(nl) * tan(_elem[3]));

    // Rayon vecteur
    const double r = _elem[1] * (1. - _elem[2] * cos(u));

    // Position
    _positionSph = Vecteur3D(l, b, r);

    /* Retour */
    return;
}

/* Accesseurs */
QString Planete::getNom() const
{
    return (nomPlanetes.at(_iplanete));
}
