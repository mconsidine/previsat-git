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

#include <QtTest>
#include "planetetest.h"
#include "librairies/corps/systemesolaire/planete.h"
#include "librairies/corps/systemesolaire/soleil.h"
#include "librairies/dates/date.h"
#include "testtools.h"


using namespace TestTools;

void PlaneteTest::testAll()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    static_cast<void> (dir.cd(APP_NAME));

    const QString dirCommonData = dir.path() + QDir::separator() + "test" + QDir::separator() + "data";
    Corps::Initialisation(dirCommonData);
    Date::Initialisation(dirCommonData);
    Planete::Initialisation(dirCommonData);

    testCalculAnglesReductionEquat();
    testCalculPositionSimp();
}

void PlaneteTest::testCalculAnglesReductionEquat()
{
    qInfo(Q_FUNC_INFO);

    const Date date(2028, 11, 13.189199259258, 0.);

    Planete planete;
    const std::array<double, 3> res = planete.CalculAnglesReductionEquat(date);

    QCOMPARE(res[0], 0.003227530972259288);
    QCOMPARE(res[1], 0.002804731797614525);
    QCOMPARE(res[2], 0.00322785138899811);
}

void PlaneteTest::testCalculPositionSimp()
{
    qInfo(Q_FUNC_INFO);

    const Date date(1992, 10, 12, 23, 59, 0.816, 0.);

    QHash<IndicePlanete, Vecteur3D> posRef = {
        { IndicePlanete::MERCURE, Vecteur3D( -151297638.9194788, -107875353.82788849,   -51807455.0671741) },
        { IndicePlanete::VENUS,   Vecteur3D( -124657523.4104318, -144456598.34559423,  -65228782.47134303) },
        { IndicePlanete::MARS,    Vecteur3D(-42871016.394301444,  139075545.24064124,   62335531.07042512) },
        { IndicePlanete::JUPITER, Vecteur3D( -953089064.5650622, -13357932.529289419,  13840000.925298497) },
        { IndicePlanete::SATURNE, Vecteur3D(  948064975.8959835,  -961607082.5120621,  -445045350.9880842) },
        { IndicePlanete::URANUS,  Vecteur3D(  759827322.6288594, -2592358564.9937806, -1148007264.7274606) },
        { IndicePlanete::NEPTUNE, Vecteur3D( 1223773476.7726054,  -4028231280.066387, -1684110165.6681817) }
    };

    Planete planete;
    Soleil soleil;
    soleil.CalculPositionSimp(date);

    QHashIterator it(posRef);
    while (it.hasNext()) {

        const IndicePlanete idx = it.next().key();

        planete = Planete(idx);
        planete.CalculPositionSimp(date, soleil);

        QCOMPARE(planete.nom(), PLANETE::nomPlanetes[idx]);
        QCOMPARE(planete.indice(), idx);
        CompareVecteurs3D(planete.position(), posRef[idx]);
    }
}
