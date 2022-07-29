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
 * >    planetetest.cpp
 *
 * Localisation
 * >    test.librairies.corps.systemesolaire
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    18 juin 2019
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include <QtTest>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wswitch-enum"
#include "planetetest.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "test/src/testtools.h"


using namespace TestTools;

void PlaneteTest::testAll()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    dir.cd(qApp->applicationName());

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);
    Date::Initialisation(dirCommonData);
    Planete::Initialisation(dirCommonData);

    testCalculPosition();
}

void PlaneteTest::testCalculPosition()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 10, 12, 23, 59, 0.816, 0.);

    std::array<Vecteur3D, 7> posRef;
    posRef[static_cast<int> (IndicePlanete::MERCURE)] = Vecteur3D(-151297638.9475474, -107875353.83721398,  -51807455.07121706);
    posRef[static_cast<int> (IndicePlanete::VENUS)]   = Vecteur3D(-124657523.43849899, -144456598.3549202,  -65228782.47538642);
    posRef[static_cast<int> (IndicePlanete::MARS)]    = Vecteur3D(-42871016.4223671,    139075545.2313155,   62335531.06638172);
    posRef[static_cast<int> (IndicePlanete::JUPITER)] = Vecteur3D(-953089064.5931276, -13357932.538614603,  13840000.921255339);
    posRef[static_cast<int> (IndicePlanete::SATURNE)] = Vecteur3D(948064975.8679179,   -961607082.5213879, -445045350.99212754);
    posRef[static_cast<int> (IndicePlanete::URANUS)]  = Vecteur3D(759827322.6007938,  -2592358565.0031056,  -1148007264.731504);
    posRef[static_cast<int> (IndicePlanete::NEPTUNE)] = Vecteur3D(1223773476.7445397, -4028231280.0757127, -1684110165.6722252);

    Planete planete;
    Soleil soleil;
    soleil.CalculPosition(date);

    for(int i=static_cast<int> (IndicePlanete::MERCURE); i<=static_cast<int> (IndicePlanete::NEPTUNE); i++) {
        planete = Planete(static_cast<IndicePlanete> (i));
        planete.CalculPosition(date, soleil);
        CompareVecteurs3D(planete.position(), posRef[i]);
    }
}
