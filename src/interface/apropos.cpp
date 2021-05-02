/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2021  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    apropos.cpp
 *
 * Localisation
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 mars 2012
 *
 * Date de revision
 * >    12 novembre 2018
 *
 */

#include <QDate>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QLocale>
#include <QStyle>
#include "ui_apropos.h"
#pragma GCC diagnostic warning "-Wconversion"
#include <QSettings>
#include "apropos.h"
#include "configuration/configuration.h"


static QSettings settings("Astropedia", "previsat");


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Apropos::Apropos(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::Apropos)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));

    ui->ok->setFocus();

    setFont(Configuration::instance()->police());
    const QString titre = tr("À propos de %1 %2");
    setWindowTitle(titre.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)));

    QGraphicsScene * const scene = new QGraphicsScene;
    scene->setSceneRect(ui->imagePreviSat->rect());
    scene->setBackgroundBrush(QBrush(palette().window().color()));
    scene->addPixmap(QPixmap(":/resources/apropos.png").scaled(ui->imagePreviSat->size()));

    ui->imagePreviSat->setScene(scene);
    QGraphicsView view(scene);
    view.setRenderHints(QPainter::Antialiasing);

    if (settings.value("affichage/flagIntensiteVision", false).toBool()) {

        QPalette paletteWin, palList;
        const int red = settings.value("affichage/valIntensiteVision", 0).toInt();
        const QBrush alpha = QBrush(QColor::fromRgb(red, 0, 0, 255));
        const QColor coulList = QColor(red + 27, 0, 0);

        paletteWin.setBrush(this->backgroundRole(), alpha);
        palList.setColor(QPalette::Base, coulList);

        this->setPalette(paletteWin);
        ui->imagePreviSat->setBackgroundBrush(alpha);
    }

    const QString informations = "Copyright © %1 %2";
    ui->informationsCopyright->setText(informations.arg(QCoreApplication::organizationName()).arg(QString(APP_ANNEES_DEV)));

    const QString logiciel = "%1 %2";
    ui->nomLogiciel->setText(logiciel.arg(QCoreApplication::applicationName()).arg(QString(APPVER_MAJ)));

    const QString msg = tr("Version %1  (%2)");
    ui->numeroVersion->setText(msg.arg(APPVERSION).arg(QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).
                                   toString(Qt::SystemLocaleShortDate)));
}

/*
 * Destructeur
 */
Apropos::~Apropos()
{
    delete ui;
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
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
void Apropos::on_ok_clicked()
{
    close();
}
