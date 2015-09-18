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
 * >    metop.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des flashs MetOp
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    12 septembre 2015
 *
 * Date de revision
 * >
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "metop.h"
#define NB_PAN 3

// Nom et numeros des panneaux
static const QByteArray LISTE_MIR = QObject::tr("FCB").toLatin1();
static const int LISTE_PAN[NB_PAN] = { 0, 1, 2 };
static const double AIRE_PAN = 3.00 * 1.00 * 1.e-6;

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
        for(int i=2; i<8; i+=2) {
            tabYaw[k] = list.at(i).toDouble() * DEG2RAD;
            tabPitch[k] = list.at(i + 1).toDouble() * DEG2RAD;
            k++;
        }
        imin = 0;
        imax = NB_PAN;
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
            _mir = LISTE_MIR[j];
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

double MetOp::CalculMagnitudeMetOp(const bool extinction, const Satellite &satellite, const Soleil &soleil,
                                   const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    _pan = -1;
    Satellite sat = satellite;

    /* Corps de la methode */
    const double angRef = AngleReflexion(satellite, soleil);

    /* Retour */
    return (MagnitudeFlash(extinction, angRef, observateur, soleil, sat));
}

/*
 * Lecture du fichier de statut des satellites MetOp
 */
int MetOp::LectureStatutMetOp(QStringList &tabStsMetOp)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    const QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

    int i = 0;

    /* Corps de la methode */
    QFile fichier(dirLocalData + QDir::separator() + "metop.sts");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    while (!flux.atEnd()) {
        const QString ligne = flux.readLine();
        if (!ligne.trimmed().isEmpty() && !ligne.trimmed().startsWith('#')) {

            tabStsMetOp.append(ligne);
            i++;
        }
    }
    fichier.close();

    /* Retour */
    return (i);
}

/*
 * Determination de la magnitude du flash
 */
double MetOp::MagnitudeFlash(const bool ext, const double angle, const Observateur &observateur, const Soleil &soleil, Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;

    /* Corps de la methode */
    const double omega = RAYON_SOLAIRE / (soleil.distanceUA() * UA2KM);
    const double invDist3 = 1. / (satellite.distance() * satellite.distance() * satellite.distance());
    const double aireProjetee = fabs(satellite.dist().x()) * invDist3 * AIRE_PAN;

    if (angle < omega) {
        // Reflexion speculaire

        // Calcul de la magnitude du point du Soleil
        const double cosAngle = cos(angle);
        const double cosOmega = cos(omega);
        const double cosPsi = sqrt((cosAngle * cosAngle - cosOmega * cosOmega)) / sin(omega);
        double psiterm = 1.;
        double intens = 0.;
        for (int i=0; i<3; i++) {
            intens += TAB_INT[i] * psiterm;
            psiterm *= cosPsi;
        }

        const double magSol = MAGNITUDE_SOLEIL - 2.5 * log10(intens);

        // Correction due a la surface eclairee
        const double surface = PI * omega * omega;
        const double magCorr = -2.5 * log10(aireProjetee / surface);

        // Magnitude du flash
        magnitude = magSol + magCorr;

    } else {
        // Reflexion non speculaire

        // Magnitude standard (approche empirique)
        const double magnitudeStandard = 3.2 * log(angle * RAD2DEG) - 2.450012;
        magnitude = magnitudeStandard - 2.5 * log10(aireProjetee / 1.e-12);
    }


    // Prise en compte de l'extinction atmospherique
    if (ext)
        magnitude += satellite.ExtinctionAtmospherique(observateur);

    /* Retour */
    return (magnitude);
}
