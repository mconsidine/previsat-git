/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    7 juillet 2024
 *
 */

#include <QFile>
#include <QtXml/QtXml>
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"
#include "gpformat.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */

/*
 * Methodes publiques
 */
/*
 * Calcul du nombre d'orbites a l'epoque (cas depassant 100000 orbites)
 */
int GPFormat::CalculNombreOrbitesEpoque(const ElementsOrbitaux &elements)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int nbOrbitesEpoque = elements.nbOrbitesEpoque;
    const QString dateLancement = elements.donnees.dateLancement();

    /* Corps de la methode */
    if (!dateLancement.isEmpty()) {

        const Date dateLct(QDateTime::fromString(dateLancement, Qt::ISODate), 0.);

        // Nombre theorique d'orbites a l'epoque
        const int nbOrbTheo = static_cast<int> (elements.no * (elements.epoque.jourJulienUTC() - dateLct.jourJulienUTC()));
        int resteOrb = nbOrbTheo % 100000;

        resteOrb += (((elements.nbOrbitesEpoque > 50000) && (resteOrb < 50000)) ? 100000 : 0);
        resteOrb -= (((elements.nbOrbitesEpoque < 50000) && (resteOrb > 50000)) ? 100000 : 0);
        const int deltaNbOrb = nbOrbTheo - resteOrb;

        nbOrbitesEpoque = elements.nbOrbitesEpoque + deltaNbOrb;
    }

    /* Retour */
    return nbOrbitesEpoque;
}

/*
 * Lecture d'un fichier au format GP
 */
QMap<QString, ElementsOrbitaux> GPFormat::Lecture(const QString &fichier,
                                                  const QSqlDatabase &db,
                                                  const QStringList &listeSatellites,
                                                  const bool ajoutDonnees,
                                                  const bool alarme)
{
    /* Declarations des variables locales */
    QMap<QString, ElementsOrbitaux> mapElem;

    try {

        /* Initialisations */
        FichierXml fi(fichier);
        const QDomDocument document = fi.Ouverture(alarme);

        /* Corps de la methode */
        ElementsOrbitaux elem;
        const QDomElement root = document.firstChildElement();
        const QDomNodeList sats = root.elementsByTagName("omm");

        for(int i=0; i<sats.count(); i++) {

            elem = LectureElements(sats.at(i));

            if ((listeSatellites.isEmpty() || listeSatellites.contains(elem.norad) || listeSatellites.contains(elem.cospar))
                && !mapElem.contains(elem.norad)) {

                // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map d'elements orbitaux)
                if (ajoutDonnees && db.isValid()) {

                    elem.donnees = Donnees::RequeteNorad(db, elem.norad).first();

                    // Correction eventuelle du nombre d'orbites a l'epoque
                    elem.nbOrbitesEpoque = CalculNombreOrbitesEpoque(elem);
                }

                mapElem.insert(elem.norad, elem);
            }
        }
    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return mapElem;
}

/*
 * Lecture d'un fichier GP contenant une liste d'elements orbitaux pour un meme satellite
 */
QList<ElementsOrbitaux> GPFormat::LectureListeGP(const QString &fichier,
                                                 const QSqlDatabase &db,
                                                 const bool alarme)
{
    /* Declarations des variables locales */
    QList<ElementsOrbitaux> listeElem;

    try {

        ElementsOrbitaux elem;

        /* Initialisations */
        FichierXml fi(fichier);
        const QDomDocument document = fi.Ouverture(alarme);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        const QDomNodeList elems = root.elementsByTagName("omm");

        for(int i=0; i<elems.count(); i++) {

            elem = LectureElements(elems.at(i));

            if (!elem.norad.isEmpty() && db.isValid()) {

                // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map d'elements orbitaux)
                elem.donnees = Donnees::RequeteNorad(db, elem.norad).first();

                // Correction eventuelle du nombre d'orbites a l'epoque
                elem.nbOrbitesEpoque = CalculNombreOrbitesEpoque(elem);
            }

            listeElem.append(elem);
        }

    } catch (Exception const &e) {
        throw Exception();
    }

    /* Retour */
    return listeElem;
}

/*
 * Recupere le nom du satellite
 */
QString GPFormat::RecupereNomsat(const QString &nom)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString nomsat = nom.trimmed();

    /* Corps de la methode */
    if ((nomsat.size() > 25) && (nomsat.mid(25).contains('.') > 0)) {
        nomsat = nomsat.mid(0, 15).trimmed();
    }

    if (nomsat.startsWith("0 ")) {
        nomsat = nomsat.mid(2);
    }

    if (nomsat.startsWith("1 ")) {
        nomsat = nomsat.split(" ", Qt::SkipEmptyParts).at(1);
    }

    if (nomsat.toLower().trimmed() == "iss (zarya)") {
        nomsat = "ISS";
    }

    if ((nomsat.contains("iridium", Qt::CaseInsensitive) || nomsat.contains("iss", Qt::CaseInsensitive)) && (nomsat.contains("["))) {
        nomsat = nomsat.mid(0, nomsat.indexOf('[')).trimmed();
    }

    /* Retour */
    return nomsat;
}


/*
 * Accesseurs
 */


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
 * Lecture des elements orbitaux
 */
ElementsOrbitaux GPFormat::LectureElements(const QDomNode &sat)
{
    /* Declarations des variables locales */
    ElementsOrbitaux elem {};

    /* Initialisations */

    /* Corps de la methode */
    try {

        if (!sat.isNull()) {

            // Lecture section metadata
            const QDomNode metadata = sat.toElement().elementsByTagName("metadata").at(0);
            const QString nomsat = metadata.firstChildElement("OBJECT_NAME").text();
            elem.nom = RecupereNomsat(nomsat);
            elem.cospar = metadata.firstChildElement("OBJECT_ID").text();

            // Lecture section meanElements
            const QDomNode meanElements = sat.toElement().elementsByTagName("meanElements").at(0);
            elem.epoque = Date::ConversionDateIso(meanElements.firstChildElement("EPOCH").text());
            elem.no = meanElements.firstChildElement("MEAN_MOTION").text().toDouble();
            elem.ecco = meanElements.firstChildElement("ECCENTRICITY").text().toDouble();
            elem.inclo = meanElements.firstChildElement("INCLINATION").text().toDouble();
            elem.omegao = meanElements.firstChildElement("RA_OF_ASC_NODE").text().toDouble();
            elem.argpo = meanElements.firstChildElement("ARG_OF_PERICENTER").text().toDouble();
            elem.mo = meanElements.firstChildElement("MEAN_ANOMALY").text().toDouble();

            // Lecture section tleParameters
            const QDomNode tleParameters = sat.toElement().elementsByTagName("tleParameters").at(0);
            elem.norad = QString("%1").arg(tleParameters.firstChildElement("NORAD_CAT_ID").text(), 6, QChar('0'));
            elem.nbOrbitesEpoque = tleParameters.firstChildElement("REV_AT_EPOCH").text().toUInt();
            elem.bstar = tleParameters.firstChildElement("BSTAR").text().toDouble();
            elem.ndt20 = tleParameters.firstChildElement("MEAN_MOTION_DOT").text().toDouble();
            elem.ndd60 = tleParameters.firstChildElement("MEAN_MOTION_DDOT").text().toDouble();
        }
    } catch (Exception const &e) {
    }

    /* Retour */
    return elem;
}
