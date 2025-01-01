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
 * >    fichierxml.cpp
 *
 * Localisation
 * >    librairies.systeme
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 novembre 2023
 *
 * Date de revision
 * >
 *
 */

#include <QFileInfo>
#include "fichierxml.h"
#include "librairies/exceptions/exception.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
FichierXml::FichierXml(const QString &fichierXml)
    : QFile(fichierXml)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QFileInfo ff(fichierXml);

    /* Corps du constructeur */
    _fichier = fichierXml;
    _nomfic = ff.fileName();

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Ouverture d'un fichier xml
 */
QDomDocument FichierXml::Ouverture(const bool alarme)
{
    /* Declarations des variables locales */
    QDomDocument document;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(_fichier);

    // Verifications
    if (!fi.exists() || (fi.size() == 0)) {

#if (!BUILD_TEST)
        qWarning() << QString("Le fichier %1 n'existe pas ou est vide").arg(_nomfic);
#endif
        if (alarme) {
            throw Exception(QObject::tr("Le fichier %1 n'existe pas ou est vide").arg(_nomfic), MessageType::WARNING);
        } else {
            throw Exception();
        }
    }

    if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

#if (!BUILD_TEST)
        qWarning() << QString("Erreur lors de l'ouverture du fichier %1").arg(_nomfic);
#endif
#if (!COVERAGE_TEST)
        if (alarme) {
            throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(_nomfic), MessageType::WARNING);
        } else {
            throw Exception();
        }
#endif
    }

    // Chargement du fichier xml
    if (!document.setContent(&fi)) {
        fi.close();

#if (!BUILD_TEST)
        qWarning() << QString("Erreur lors du chargement du fichier %1").arg(_nomfic);
#endif
        if (alarme) {
            throw Exception(QObject::tr("Erreur lors du chargement du fichier %1").arg(_nomfic), MessageType::WARNING);
        } else {
            throw Exception();
        }
    }

    _version = document.documentElement().attribute("version");
    fi.close();

    /* Retour */
    return document;
}


/*
 * Accesseurs
 */
QString FichierXml::nomfic() const
{
    return _nomfic;
}

QString FichierXml::version()
{
    /* Declarations des variables locales */

    /* Initialisations */
    QFile fi(_fichier);

    /* Corps de la methode */
    if (_version.isEmpty() && fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QDomDocument document;
        document.setContent(&fi);
        _version = document.documentElement().attribute("version");

        fi.close();
    }

    /* Retour */
    return _version;
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

