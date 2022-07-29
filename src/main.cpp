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
 * >    main.cpp
 *
 * Localisation
 * >
 *
 * Description
 * >    Demarrage de l'application
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    20 juin 2022
 *
 */

#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#include <QDir>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#include "configuration/configuration.h"
#include "interface/previsat.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/systeme/logmessage.h"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#include <QApplication>


int main(int argc, char *argv[])
{
    try {

        /* Declarations des variables locales */

        /* Initialisations */
        QApplication a(argc, argv);

        a.setOrganizationName(ORG_NAME);
        a.setApplicationName(APP_NAME);
        a.setOrganizationDomain(DOMAIN_NAME);

        /* Corps de la methode */
        // Definition du repertoire du fichier de log
        Configuration::instance()->DefinitionDirLog();

        // Installation de la gestion du fichier de log
        const LogMessage msg(Configuration::instance()->dirLog() + QDir::separator() + APP_NAME + ".log");
        Q_UNUSED(msg)


        PreviSat w;
        w.show();

        /* Retour */
        return a.exec();

    } catch (PreviSatException &e) {
        return EXIT_FAILURE;
    }
}
