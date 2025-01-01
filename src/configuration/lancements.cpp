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
 * >    lancements.cpp
 *
 * Localisation
 * >    configuration
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#include <QFile>
#include "calendrierlancements.h"
#include "lancements.h"
#include "configuration.h"
#include "librairies/exceptions/exception.h"
#include "librairies/systeme/telechargement.h"


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
 * Lecture du fichier contenant le calendrier des lancements
 */
QList<CalendrierLancements> Lancements::LectureCalendrierLancements()
{
    /* Declarations des variables locales */
    QList<CalendrierLancements> calendrierLancements;

    /* Initialisations */
    if (Configuration::instance()->mapVerrous().contains("lancements")) {

        QFile fi(Configuration::instance()->dirLocalData() + QDir::separator() + "lancements.txt");
        const QFileInfo ff(fi.fileName());

        if (!fi.exists()) {

            // Telechargement du fichier
            Telechargement tel(Configuration::instance()->dirLocalData());
            tel.TelechargementFichier(QUrl(QString("%1data/lancements.txt").arg(DOMAIN_NAME)), false);
        }

        /* Corps de la methode */
        if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
#if (!COVERAGE_TEST)
            qWarning() << QString("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName());
            throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName()), MessageType::WARNING);
#endif
        }

        const QStringList contenu = QString(fi.readAll()).split("\n");
        fi.close();

        CalendrierLancements calendrier;
        QStringList tab(6);
        QStringListIterator it(contenu);

        while (it.hasNext()) {

            tab.fill("");
            for(unsigned int i=0; i<tab.size(); i++) {
                if (it.hasNext()) {
                    tab[i] = it.next();
                }
            }

            if (!tab.first().isEmpty() && !tab.last().isEmpty()) {

                calendrier.date = tab.at(0);
                calendrier.lancement = tab.at(1);
                calendrier.heure = tab.at(2);
                calendrier.site = tab.at(3);
                calendrier.commentaire = tab.at(4);
                calendrier.maj = tab.at(5).trimmed();

                calendrierLancements.append(calendrier);
            }

            if (it.hasNext()) {
                it.next();
            }
        }

        qInfo() << QString("Lecture fichier %1 OK").arg(ff.fileName());
    }

    /* Retour */
    return calendrierLancements;
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
