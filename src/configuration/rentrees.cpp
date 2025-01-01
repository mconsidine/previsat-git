/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    rentrees.cpp
 *
 * Localisation
 * >    configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#include "configuration.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"
#include "librairies/systeme/telechargement.h"
#include "rentrees.h"
#include "rentreesatmospheriques.h"


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
 * Lecture du fichier contenant les rentrees atmospheriques
 */
QList<RentreesAtmospheriques> Rentrees::LectureFichierRentrees()
{
    /* Declarations des variables locales */
    QList<RentreesAtmospheriques> rentreesAtmospheriques;

    /* Initialisations */
    FichierXml fi(Configuration::instance()->dirLocalData() + QDir::separator() + "rentrees.xml");

    if (!fi.exists()) {

        // Telechargement du fichier
        Telechargement tel(Configuration::instance()->dirLocalData());
        tel.TelechargementFichier(QUrl(QString("%1data/rentrees.xml").arg(DOMAIN_NAME)));
    }

    /* Corps de la methode */
    try {

        const QDomDocument document = fi.Ouverture(false);

        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "xml") {
            throw Exception();
        }

        QStringList norads;
        const QDomNodeList sats = root.elementsByTagName("item");

        for(int i=0; i<sats.count(); i++) {

            const RentreesAtmospheriques rentree = LectureRentree(sats.at(i));

            if (!norads.contains(rentree.norad)) {
                norads.append(rentree.norad);
                rentreesAtmospheriques.append(rentree);
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(fi.nomfic());

    } catch (Exception const &e) {
        qWarning() << QString("Erreur lors de l'ouverture du fichier %1").arg(fi.nomfic());
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(fi.nomfic()), MessageType::WARNING);
    }

    /* Retour */
    return rentreesAtmospheriques;
}


/*
 * Accesseurs
 */

/*
 * Modificateurs
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
RentreesAtmospheriques Rentrees::LectureRentree(const QDomNode &sat)
{
    /* Declarations des variables locales */
    RentreesAtmospheriques rentree {};

    /* Initialisations */

    /* Corps de la methode */
    if (!sat.isNull()) {

        rentree.norad = sat.firstChildElement("NORAD_CAT_ID").text();
        rentree.nom = sat.firstChildElement("OBJECT_NAME").text();
        rentree.cospar = sat.firstChildElement("OBJECT_ID").text();
        rentree.dateRentree = QDateTime::fromString(sat.firstChildElement("DECAY_EPOCH").text(), "yyyy-MM-dd H:mm:ss");
        rentree.taille = sat.firstChildElement("RCS_SIZE").text();
        rentree.pays = sat.firstChildElement("COUNTRY").text();
        rentree.typeMessage = sat.firstChildElement("MSG_TYPE").text();
    }

    /* Retour */
    return rentree;
}
