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
 * >    iridium.cpp
 *
 * Localisation
 * >    previsions
 *
 * Heritage
 * >
 *
 * Description
 * >    Calcul des flashs Iridium
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    17 juillet 2011
 *
 * Date de revision
 * >    30 septembre 2015
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "iridium.h"
#define NB_PAN 3

// Donnees sur la geometrie des MMA
static const double AIRE_MMA = 1.88 * 0.86 * 1.e-6;
static const double PHI = -40. * DEG2RAD;
static const double COSPHI = cos(PHI);
static const double SINPHI = sin(PHI);

// Nom et numeros des panneaux
static const QByteArray LISTE_MIR = QObject::tr("ADGS").toLatin1();
static const int LISTE_PAN[] = { 0, 1, 2, 3 };


/*
 * Calcul de l'angle de reflexion du panneau
 */
double Iridium::AngleReflexion(const Satellite &satellite, const Soleil &soleil)
{
    /* Declarations des variables locales */
    int imin, imax;

    /* Initialisations */
    double ang = PI;

    /* Corps de la methode */
    if (_pan == -1) {
        imin = 0;
        imax = NB_PAN;
    } else {
        imin = _pan;
        imax = _pan + 1;
    }

    // Angle de reflexion par les MMA
    int j = 0;
    if (_pan < NB_PAN) {

        const Vecteur3D xx = satellite.vitesse().Normalise();
        const Vecteur3D yy = satellite.position().Normalise() ^ xx;
        const Vecteur3D zz = xx ^ yy;

        // Matrice de passage ECI geocentrique -> ECI satellite
        const Matrice3D P(xx, yy, zz);

        for (int i=imin; i<imax; i++) {

            const double psi = i * DEUX_TIERS * PI;
            const double cospsi = cos(psi);
            const double sinpsi = sin(psi);

            const Vecteur3D v1(COSPHI * cospsi, -COSPHI * sinpsi, SINPHI);
            const Vecteur3D v2(sinpsi, cospsi, 0.);
            const Vecteur3D v3(-SINPHI * cospsi, SINPHI * sinpsi, COSPHI);

            // Matrice de rotation repere satellite -> repere panneau
            const Matrice3D R(v1, v2, v3);

            // Matrice produit P x R
            Matrice3D tmp = P * R;
            Matrice3D PR = tmp.Transposee();

            // Position observateur dans le repere panneau
            const Vecteur3D obsat = PR * (-satellite.dist());

            // Position Soleil dans le repere panneau
            Vecteur3D solsat = PR * (soleil.position() - satellite.position());

            // Position du reflet du Soleil
            solsat = Vecteur3D(solsat.x(), -solsat.y(), -solsat.z());

            // Angle de reflexion
            const double temp = obsat.Angle(solsat);
            if (_pan == -1) {
                if (temp < ang) {
                    ang = temp;
                    j = i;
                    _mir = LISTE_MIR[i];
                    _direction = PR.Transposee() * solsat;
                }
            } else {
                ang = temp;
                _direction = PR.Transposee() * solsat;
            }
        }
    }

    // Angle de reflexion par le panneau solaire
    if (_psol && (_pan == -1 || _pan == 3)) {

        // Calcul de l'angle beta (angle entre le plan de l'orbite et la direction du Soleil)
        Satellite sat = satellite;
        sat.CalculBeta(soleil);
        const double beta = fabs(sat.beta() * RAD2DEG);

        if (beta >= 40. && beta <= 65.) {

            const Matrice3D matrice1 = RotationRV(satellite.position(), satellite.vitesse(), PI_SUR_DEUX, -PI_SUR_DEUX, 1);
            const Vecteur3D vecteur1 = matrice1 * soleil.position();

            double phi = atan2(vecteur1.y(), vecteur1.x());
            if (phi < 0.)
                phi += DEUX_PI;
            const double theta = atan2(vecteur1.z(), sqrt(vecteur1.x() * vecteur1.x() + vecteur1.y() * vecteur1.y()));

            const double rotAD = DEUX_PI - phi;
            const double rotDec = (theta > 0.) ? -PHI : PHI;

            const Matrice3D matrice2 = Matrice3D(AXE_Z, -rotAD) * matrice1;
            Matrice3D matrice3 = Matrice3D(AXE_Y, -rotDec) * matrice2;
            const Matrice3D matrice4 = matrice3.Transposee();

            const Vecteur3D vecteur2 = matrice4.vecteur1();

            const Vecteur3D solsat = soleil.position() - satellite.position();
            _surf = vecteur2.Angle(solsat);

            Matrice3D matrice5 = RotationRV(solsat, vecteur2, 0., 0., 0);
            const Matrice3D matrice6(AXE_Z, -_surf);
            const Vecteur3D vecteur3 = matrice6.vecteur1();
            const Matrice3D matrice7 = matrice5.Transposee();
            const Vecteur3D vecteur4 = matrice7 * vecteur3;

            const Vecteur3D obsat = -satellite.dist();
            const double tmp = vecteur4.Angle(obsat);

            if (tmp < ang) {
                ang = tmp;
                j = 3;
                _mir = LISTE_MIR[j];
                _direction = vecteur4;
            }
        }
    }

    if (_pan == -1)
        _pan = LISTE_PAN[j];

    /* Retour */
    return (ang);
}

/*
 * Calcul des flashs Iridium
 */
void Iridium::CalculFlashsIridium(const Conditions &conditions, Observateur &observateur, QStringList &result)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString idsat("Ir  ");

    /* Corps de la methode */
    CalculFlashs(idsat, conditions, observateur, result);

    /* Retour */
    return;
}

double Iridium::CalculMagnitudeIridium(const bool extinction, const bool ope, const QStringList &tabSts, const Satellite &satellite,
                                       const Soleil &soleil, const Observateur &observateur)
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool acalc = false;
    double magnitude = 99.;
    Satellite sat = satellite;

    /* Corps de la methode */
    QStringListIterator it(tabSts);
    while (it.hasNext()) {
        const QString ligne = it.next();
        if (ligne.contains(sat.tle().norad()) && !ligne.toLower().contains("t")) {
            acalc = true;
            if (ligne.contains("?") && ope)
                acalc = false;
            it.toBack();
        }
    }

    if (acalc) {
        _pan = -1;
        _psol = true;

        const double angRef = AngleReflexion(satellite, soleil);
        magnitude = MagnitudeFlash(extinction, angRef, observateur, soleil, sat);
    }

    /* Retour */
    return (magnitude);
}

/*
 * Lecture du fichier de statut des satellites Iridium
 */
void Iridium::LectureStatutIridium(const char ope, QStringList &tabStsIri)
{
    /* Declarations des variables locales */

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    const QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

    /* Corps de la methode */
    QFile fichier(dirLocalData + QDir::separator() + "iridium.sts");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    while (!flux.atEnd()) {
        const QString ligne = flux.readLine();
        tabStsIri.append(ligne);
        if (ligne.size() == 9) {
            tabStsIri.append(ligne);
        } else {
            if (ligne.at(10) == '?' && ope == 'n')
                tabStsIri.append(ligne);
        }
    }
    fichier.close();

    /* Retour */
    return;
}

/*
 * Determination de la magnitude du flash
 */
double Iridium::MagnitudeFlash(const bool ext, const double angle, const Observateur &observateur, const Soleil &soleil,
                               Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    double magnitude = 99.;

    /* Corps de la methode */
    if ( _pan < 3) {

        // Calcul de la magnitude du flash produit par les MMA
        const double omega = RAYON_SOLAIRE / (soleil.distanceUA() * UA2KM);
        const double invDist3 = 1. / (satellite.distance() * satellite.distance() * satellite.distance());
        const double aireProjetee = fabs(satellite.dist().x()) * invDist3 * AIRE_MMA;

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
    } else {

        // Calcul de la magnitude du flash produit par les panneaux solaires
        magnitude = 5. * log10(satellite.distance()) - 14.4;

        // Prise en compte de la surface illuminee
        double cossurf = cos(_surf);
        if (cossurf <= 0.) cossurf = 0.0001;
        magnitude -= 2.5 * log10(cossurf);

        // Prise en compte de l'angle de reflexion
        const double angRefDeg = angle * RAD2DEG;
        const double corrMag = (angRefDeg < 3.5) ? -3.867 + 0.993 * angRefDeg : -5.6415 + 1.5 * angRefDeg;
        magnitude += corrMag - 0.7;
    }

    // Prise en compte de l'extinction atmospherique
    if (ext)
        magnitude += satellite.ExtinctionAtmospherique(observateur);

    /* Retour */
    return (magnitude);
}
