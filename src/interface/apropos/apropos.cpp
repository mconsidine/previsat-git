/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2024  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    1er octobre 2023
 *
 */

#include <QDate>
#include <QMessageBox>
#include <QStyle>
#include <QSettings>
#include "ui_apropos.h"
#include "apropos.h"
#include "configuration/configuration.h"


// Registre
#if (PORTABLE_BUILD == true)
static QSettings settings(QString("%1.ini").arg(APP_NAME), QSettings::IniFormat);
#else
static QSettings settings(ORG_NAME, APP_NAME);
#endif


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
    setWindowTitle(titre.arg(APP_NAME).arg(QString(VER_MAJ)));
    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

    QGraphicsScene * const scene = new QGraphicsScene;
    scene->setSceneRect(_ui->imagePreviSat->rect());
    scene->setBackgroundBrush(QBrush(palette().window().color()));
    scene->addPixmap(QPixmap(":/resources/interface/apropos.png").scaled(_ui->imagePreviSat->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    _ui->imagePreviSat->setScene(scene);
    QGraphicsView view(scene);
    view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    const QString informations = "Copyright © %1 %2";
    _ui->informationsCopyright->setText(informations.arg(ORG_NAME).arg(QString(ANNEES_DEV)));

    const QString logiciel = "%1 %2";
    _ui->nomLogiciel->setText(logiciel.arg(APP_NAME).arg(QString(VER_MAJ)));

    const QString msg = tr("Version %1  (%2)");
    const QString dateCompilation = QString(__DATE__).split(" ", Qt::SkipEmptyParts).join(" ");
    _ui->numeroVersion->setText(msg.arg(VERSION).arg(QLocale(Configuration::instance()->locale())
                                                         .toString(QDate::fromString(dateCompilation, "MMM d yyyy"), tr("d MMMM yyyy", "Date format"))));

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

void Apropos::on_verifierMisesAJour_clicked()
{
    const bool anew = emit VerifMajVersion(QString("version%1").arg(APP_NAME));

    if (!anew) {
        QMessageBox::information(0, tr("Information"), tr("Vous utilisez la dernière version de %1").arg(APP_NAME));
    }
}

