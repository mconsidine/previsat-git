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
 * >    11 novembre 2022
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDate>
#include <QStyle>
#include <QSettings>
#include "ui_apropos.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "apropos.h"
#include "configuration/configuration.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
Apropos::Apropos(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    _ui(new Ui::Apropos)
{
    _ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));
}


/*
 * Destructeur
 */
Apropos::~Apropos()
{
    delete _ui;
}


/*
 * Accesseurs
 */

/*
 * Modificateurs
 */

/*
 * Methodes publiques
 */
void Apropos::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void Apropos::show()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ui->ok->setFocus();

    setFont(Configuration::instance()->police());
    const QString titre = tr("À propos de %1 %2");
    setWindowTitle(titre.arg(APP_NAME).arg(QString(APP_VER_MAJ)));

    QGraphicsScene * const scene = new QGraphicsScene;
    scene->setSceneRect(_ui->imagePreviSat->rect());
    scene->setBackgroundBrush(QBrush(palette().window().color()));
    scene->addPixmap(QPixmap(":/resources/interface/apropos.png").scaled(_ui->imagePreviSat->size()));

    _ui->imagePreviSat->setScene(scene);
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
        _ui->imagePreviSat->setBackgroundBrush(alpha);
    }

    const QString informations = "Copyright © %1 %2";
    _ui->informationsCopyright->setText(informations.arg(ORG_NAME).arg(QString(APP_ANNEES_DEV)));

    const QString logiciel = "%1 %2";
    _ui->nomLogiciel->setText(logiciel.arg(APP_NAME).arg(QString(APP_VER_MAJ)));

    const QString msg = tr("Version %1  (%2)");
    _ui->numeroVersion->setText(msg.arg(APP_VERSION).arg(QLocale(Configuration::instance()->locale())
                                                         .toString(QDate::fromString(QString(__DATE__), "MMM d yyyy"),
                                                                   tr("d MMMM yyyy", "Date format"))));

    /* Retour */
    return;
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
void Apropos::on_ok_clicked()
{
    close();
}
