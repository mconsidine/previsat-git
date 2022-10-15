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
 * >    gpformat.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFile>
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QFileInfo>
#include <QXmlStreamReader>
#include "librairies/exceptions/previsatexception.h"
#include "gpformat.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur a partir des elements orbitaux
 */
GPFormat::GPFormat(const ElementsOrbitaux &elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _elements = elem;

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
QMap<QString, ElementsOrbitaux> GPFormat::LectureFichier(const QString &nomFichier, const QString &donneesSat, const int lgRec,
                                                         const QStringList &listeSatellites, const bool ajoutDonnees)
{
    /* Declarations des variables locales */
    QMap<QString, ElementsOrbitaux> mapElem;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(nomFichier);
    if (!fi.exists() || (fi.size() == 0)) {
        const QFileInfo ff(fi.fileName());
        throw PreviSatException(QT_TRANSLATE_NOOP("GPFormat", QString("Le fichier %1 n'existe pas ou est vide").arg(ff.fileName())),
                                MessageType::WARNING);
    }

    if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QXmlStreamReader gp(&fi);
        gp.readNextStartElement();

        if (gp.name().toString() == "ndm") {

            ElementsOrbitaux elem;

            while (gp.readNextStartElement()) {

                if (gp.name().toString() == "omm") {

                    elem.norad.clear();

                    while (gp.readNextStartElement()) {

                        if (gp.name().toString() == "body") {

                            LectureSectionBody(gp, elem);

                        } else {
                            gp.skipCurrentElement();
                        }
                    }
                } else {
                    gp.skipCurrentElement();
                }

                if (listeSatellites.isEmpty() || listeSatellites.contains(elem.norad)) {

                    if (!mapElem.contains(elem.norad)) {

                        if (ajoutDonnees) {

                            // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map d'elements orbitaux)
                            const int idx = lgRec * elem.norad.toInt();
                            if ((idx >= 0) && (idx < donneesSat.size())) {

                                elem.donnees = Donnees(donneesSat.mid(idx, lgRec));

                                // Correction eventuelle du nombre d'orbites a l'epoque
                                const QString dateLancement = elem.donnees.dateLancement();
                                if (!dateLancement.isEmpty()) {

                                    const QDateTime dateTimeLct = QDateTime::fromString(dateLancement, Qt::ISODate);
                                    const Date dateLct(dateTimeLct.date().year(), dateTimeLct.date().month(), dateTimeLct.date().day(), 0.);

                                    // Nombre theorique d'orbites a l'epoque
                                    const int nbOrbTheo = static_cast<int> (elem.no * (elem.epoque.jourJulienUTC() - dateLct.jourJulienUTC()));
                                    int resteOrb = nbOrbTheo%100000;
                                    resteOrb += (((elem.nbOrbitesEpoque > 50000) && (resteOrb < 50000)) ? 100000 : 0);
                                    resteOrb -= (((elem.nbOrbitesEpoque < 50000) && (resteOrb > 50000)) ? 100000 : 0);
                                    const int deltaNbOrb = nbOrbTheo - resteOrb;

                                    elem.nbOrbitesEpoque += deltaNbOrb;
                                }
                            }
                        }

                        mapElem.insert(elem.norad, elem);
                    }
                }
            }

        } else {
            fi.close();
            qWarning() << QString("Le fichier %1 ne contient aucun satellite").arg(nomFichier);
            throw PreviSatException(QObject::tr("Le fichier %1 ne contient aucun satellite").arg(nomFichier), MessageType::WARNING);
        }
        fi.close();

    }

    /* Retour */
    return mapElem;
}


/*
 * Accesseurs
 */
const ElementsOrbitaux &GPFormat::elements() const
{
    return _elements;
}


/*************
 * PROTECTED *
 *************/

/*
 * Methodes protegees
 */


/***********
 * PRIVATE *
 ***********/

/*
 * Methodes privees
 */
/*
 * Lecture de la section Body du fichier d'elements orbitaux
 */
void GPFormat::LectureSectionBody(QXmlStreamReader &gp, ElementsOrbitaux &elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (gp.readNextStartElement()) {

        if (gp.name().toString() == "segment") {

            while (gp.readNextStartElement()) {

                if (gp.name().toString() == "metadata") {

                    LectureSectionMetaData(gp, elem);

                } else if (gp.name().toString() == "data") {

                    LectureSectionData(gp, elem);

                } else {
                    gp.skipCurrentElement();
                }
            }
        } else {
            gp.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section Data du fichier d'elements orbitaux
 */
void GPFormat::LectureSectionData(QXmlStreamReader &gp, ElementsOrbitaux &elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (gp.readNextStartElement()) {

        if (gp.name().toString() == "meanElements") {

            LectureSectionMeanElements(gp, elem);

        } else if (gp.name().toString() == "tleParameters") {

            LectureSectionTleParameters(gp, elem);

        } else {
            gp.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section MeanElements du fichier d'elements orbitaux
 */
void GPFormat::LectureSectionMeanElements(QXmlStreamReader &gp, ElementsOrbitaux &elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (gp.readNextStartElement()) {

        if (gp.name().toString() == "EPOCH") {

            // Epoque des elements orbitaux
            elem.epoque = Date::ConversionDateIso(gp.readElementText());

        } else if (gp.name().toString() == "MEAN_MOTION") {

            // Moyen mouvement
            elem.no = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "ECCENTRICITY") {

            // Excentricite
            elem.ecco = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "INCLINATION") {

            // Inclinaison
            elem.inclo = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "RA_OF_ASC_NODE") {

            // Ascension droite du noeud ascendant
            elem.omegao = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "ARG_OF_PERICENTER") {

            // Argument du pericentre
            elem.argpo = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "MEAN_ANOMALY") {

            // Anomalie moyenne
            elem.mo = gp.readElementText().toDouble();

        } else {
            gp.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section MetaData du fichier d'elements orbitaux
 */
void GPFormat::LectureSectionMetaData(QXmlStreamReader &gp, ElementsOrbitaux &elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (gp.readNextStartElement()) {

        if (gp.name().toString() == "OBJECT_NAME") {

            // Nom de l'objet
            elem.nom = gp.readElementText();

        } else if (gp.name().toString() == "OBJECT_ID") {

            // Designation COSPAR
            elem.cospar = gp.readElementText();

        } else {
            gp.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}

/*
 * Lecture de la section TleParameters du fichier d'elements orbitaux
 */
void GPFormat::LectureSectionTleParameters(QXmlStreamReader &gp, ElementsOrbitaux &elem)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    while (gp.readNextStartElement()) {

        if (gp.name().toString() == "NORAD_CAT_ID") {

            // Numero NORAD
            elem.norad = QString("%1").arg(gp.readElementText(), 6, QChar('0'));

        } else if (gp.name().toString() == "REV_AT_EPOCH") {

            // Nombre d'orbites a l'epoque
            elem.nbOrbitesEpoque = gp.readElementText().toInt();

        } else if (gp.name().toString() == "BSTAR") {

            // Coefficient pseudo-balistique
            elem.bstar = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "MEAN_MOTION_DOT") {

            // Derivee du moyen mouvement / 2
            elem.ndt20 = gp.readElementText().toDouble();

        } else if (gp.name().toString() == "MEAN_MOTION_DDOT") {

            // Derivee seconde du moyen mouvement / 6
            elem.ndd60 = gp.readElementText().toDouble();

        } else {
            gp.skipCurrentElement();
        }
    }

    /* Retour */
    return;
}
