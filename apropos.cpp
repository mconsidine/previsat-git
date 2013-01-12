/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2013  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre A propos
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    10 mars 2012
 *
 * Date de revision
 * >
 *
 */

#include <QDate>
#include <QSettings>
#include "apropos.h"
#include "ui_apropos.h"

static QSettings settings("Astropedia", "previsat");

Apropos::Apropos(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Apropos)
{
    ui->setupUi(this);
    setWindowTitle(tr("À propos de PreviSat") + " " + APPVER_MAJ);

    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(ui->imagePreviSat->rect());
    scene->setBackgroundBrush(QBrush(Apropos::palette().background().color()));
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

    ui->nomLogiciel->setText("PreviSat " + QString(APPVER_MAJ));
    const QString msg = tr("Version %1  (%2)");
    ui->numeroVersion->setText(msg.arg(APPVERSION).
                               arg(QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).
                                   toString(Qt::SystemLocaleShortDate)));
}

Apropos::~Apropos()
{
    delete ui;
}

void Apropos::on_ok_clicked()
{
    close();
}
