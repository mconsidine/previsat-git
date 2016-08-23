﻿/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2016  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    metop.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des flashs MetOp et COSMO-SkyMed
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    12 septembre 2015
 *
 * Date de revision
 * >    22 aout 2016
 *
 */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpacked"
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wpacked"
#include "metop.h"

#define NB_PAN 3

// Nom et numeros des panneaux
static const QByteArray LISTE_MIR = QObject::tr("FCB").toLatin1();
static const int LISTE_PAN[NB_PAN] = { 0, 1, 2 };
static const double AIRE_PAN_METOP = 3.00 * 1.00 * 1.e-6;
static const double AIRE_PAN_SKYMED = 5.70 * 1.40 * 1.e-6;
static double tabYaw[NB_PAN] = { 0. };
static double tabPitch[NB_PAN] = { 0. };


/*
 * Calcul de l'angle de reflexion du panneau
 */
double MetOp::AngleReflexion(const Satellite &satellite, const Soleil &soleil)
{
    /* Declarations des variables locales */
    int imin, imax;

    /* Initialisations */
    double ang = PI;
    int j = 0;
    if (_pan == -1) {

        int k = 0;
        const QStringList list = _sts.split(" ", QString::SkipEmptyParts);
        for(int i=2; i<list.length(); i+=2) {
            tabYaw[k] = list.at(i).toDouble() * DEG2RAD;
            tabPitch[k] = list.at(i + 1).toDouble() * DEG2RAD;
            k++;
        }
        imin = 0;
        imax = k;
    } else {
        imin = _pan;
        imax = _pan + 1;
    }

    /* Corps de la methode */
    for(int i=imin; i<imax; i++) {

        Matrice3D matrice1 = RotationYawSteering(satellite, tabYaw[i], tabPitch[i]);
        const Matrice3D matrice2 = matrice1.Transposee();
        const Vecteur3D vecteur1 = matrice2.vecteur1();

        const Vecteur3D solsat = soleil.position() - satellite.position();
        const double surf = solsat.Angle(vecteur1);

        Matrice3D matrice3 = RotationRV(solsat, vecteur1, 0., 0., 0);
        const Matrice3D matrice4(AXE_Z, -surf);
        const Vecteur3D vecteur2 = matrice4.vecteur1();
        const Matrice3D matrice5 = matrice3.Transposee();
        const Vecteur3D vecteur3 = matrice5 * vecteur2;

        const Vecteur3D obsat = -satellite.dist();
        const double tmp = vecteur3.Angle(obsat);

        if (tmp < ang) {
            ang = tmp;
            j = i;
            _mir = (_sts.toLower().contains("metop")) ? LISTE_MIR[j] : 'S';
            _direction = vecteur3;
            _surf = surf;
        }
    }

    if (_pan == -1)
        _pan = LISTE_PAN[j];

    /* Retour */
    return (ang);
}

/*
 * Calcul des flashs MetOp
 */
void MetOp::CalculFlashsMetOp(const Conditions &conditions, Observateur &observateur, QStringList &result)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString idsat("Satellite ");

    /* Corps de la methode */
    CalculFlashs(idsat, conditions, observateur, result);

    /* Retour */
    return;
}

double MetOp::CalculMagnitudeMetOp(const bool extinction, const bool eclPartielle, const QStringList &tabSts, const Satellite &satellite,
                                   const Soleil &soleil, const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;
    _pan = -1;
    Satellite sat = satellite;

    /* Corps de la methode */
    QStringListIterator it(tabSts);
    while (it.hasNext()) {
        const QString ligne = it.next();
        if (ligne.contains(sat.tle().norad())) {
            _sts = ligne;
            it.toBack();
        }
    }

    if (!_sts.isEmpty()) {
        const double angRef = AngleReflexion(satellite, soleil);
        magnitude = MagnitudeFlash(extinction, eclPartielle, angRef, observateur, sat);
    }

    /* Retour */
    return (magnitude);
}

/*
 * Lecture du fichier de statut des satellites MetOp
 */
void MetOp::LectureStatutMetOp(QStringList &tabStsMetOp)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else

#if QT_VERSION >= 0x050000
    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    const QString dirLocalData =
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory).at(0) +
            dirAstr + QDir::separator() + "data";
#else
    const QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

#endif

    /* Corps de la methode */
    QFile fichier(dirLocalData + QDir::separator() + "flares" + QDir::separator() + "flares.sts");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    while (!flux.atEnd()) {
        const QString ligne = flux.readLine();
        if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#'))
            tabStsMetOp.append(ligne);
    }
    fichier.close();

    /* Retour */
    return;
}

/*
 * Determination de la magnitude du flash
 */
double MetOp::MagnitudeFlash(const bool ext, const bool eclPartielle, const double angle, const Observateur &observateur,
                             Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;
    const double angDeg = angle * RAD2DEG;
    const QString typSat = _sts.toLower();

    /* Corps de la methode */
    if (typSat.contains("metop"))
        magnitude = -5. + angDeg * (0.239 + angDeg * 2.2573);

    if (typSat.contains("skymed"))
        magnitude = 0.3075 * angDeg - 2.92;

    // Prise en compte des eclipses partielles ou annulaires
    if (eclPartielle) {
        const double luminositeEclipse = qMin(satellite.luminositeEclipseLune(), satellite.luminositeEclipseSoleil());
        if (luminositeEclipse > 0. && luminositeEclipse <= 1.)
            magnitude += -2.5 * log10(luminositeEclipse);
    }

    // Prise en compte de l'extinction atmospherique
    if (ext)
        magnitude += satellite.ExtinctionAtmospherique(observateur);

    /* Retour */
    return (magnitude);
}
