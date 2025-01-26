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
 * >    fichierobs.cpp
 *
 * Localisation
 * >    configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    21 juin 2022
 *
 * Date de revision
 * >    18 aout 2024
 *
 */

#include <QDir>
#include "configuration.h"
#include "fichierobs.h"
#include "gestionnairexml.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/fichierxml.h"


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
 * Ecriture du fichier de lieu d'observation
 */
void FichierObs::Ecriture(const QString &ficObsXml)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(Configuration::instance()->dirCoord() + QDir::separator() + ficObsXml);

    if (!fi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        const QFileInfo ff(fi.fileName());
        qWarning().noquote() << QString("Erreur lors de l'écriture du fichier %1").arg(ff.fileName());
        throw Exception(QObject::tr("Erreur lors de l'écriture du fichier %1").arg(ff.fileName()), MessageType::WARNING);
    }

    QXmlStreamWriter cfg(&fi);

    cfg.setAutoFormatting(true);
    cfg.writeStartDocument();
    cfg.writeStartElement("PreviSatObservateurs");

    // Observateurs
    QMapIterator it(Configuration::instance()->mapObs());
    while (it.hasNext()) {
        it.next();

        const Observateur obs = it.value();
        cfg.writeStartElement("Observateur");
        cfg.writeTextElement("Nom", obs.nomlieu());

        cfg.writeTextElement("Longitude", QString::number(obs.longitude() * MATHS::RAD2DEG, 'f', 9));
        cfg.writeTextElement("Latitude", QString::number(obs.latitude() * MATHS::RAD2DEG, 'f', 9));
        cfg.writeTextElement("Altitude", QString::number(obs.altitude() * 1000.));
        cfg.writeEndElement();
    }

    cfg.writeEndElement();
    cfg.writeEndDocument();
    fi.close();

    /* Retour */
    return;
}

/*
 * Lecture du fichier de lieu d'observation
 */
QMap<QString, Observateur> FichierObs::Lecture(const QString &ficObsXml,
                                               const bool alarme)
{
    /* Declarations des variables locales */
    QMap<QString, Observateur> mapObs;

    /* Initialisations */
    FichierXml fi(Configuration::instance()->dirCoord() + QDir::separator() + ficObsXml);

    try {

        const QDomDocument document = fi.Ouverture(false);

        /* Corps de la methode */
        const QDomElement root = document.firstChildElement();
        if (root.nodeName() != "PreviSatObservateurs") {
            throw Exception();
        }

        const QList<Observateur> observateurs = GestionnaireXml::LectureLieuxObservation(root);

        QListIterator it(observateurs);
        while (it.hasNext()) {

            const Observateur obs = it.next();
            const QString nom = obs.nomlieu();
            mapObs.insert(nom, obs);
        }

        qInfo().noquote() << QString("Lecture fichier %1 OK").arg(fi.nomfic());

    } catch (Exception const &e) {
        qCritical().noquote() << QString("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(fi.nomfic()).arg(APP_NAME);
        if (alarme) {
            throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1, veuillez réinstaller %2").arg(fi.nomfic()).arg(APP_NAME), MessageType::ERREUR);
        }
    }

    /* Retour */
    return mapObs;
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

