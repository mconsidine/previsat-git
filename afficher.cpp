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
 * >    afficher.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre d'affichage des resultats ou du fichier d'aide
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2011
 *
 * Date de revision
 * >    17 decembre 2013
 *
 */

#if defined QT_NO_DEBUG
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include "ui_afficher.h"
#include "afficher.h"
#pragma GCC diagnostic warning "-Wshadow"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"

static QString dirOut;
static QString dirTmp;
static QString prev;
static QSettings settings("Astropedia", "previsat");

Afficher::Afficher(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::Afficher)
{
    ui->setupUi(this);

    const int xmax = QApplication::desktop()->availableGeometry().width();
    const int ymax = QApplication::desktop()->availableGeometry().height();
    int xAff = width();
    int yAff = height();

    if (x() < 0 || y() < 0)
        move(0, 0);

    // Redimensionnement de la fenetre si necessaire
    if (xAff > xmax)
        xAff = xmax;
    if (yAff > ymax)
        yAff = ymax;
    if (xAff < width() || yAff < height()) {
        if (xmax < minimumWidth())
            setMinimumWidth(xmax);
        if (ymax < minimumHeight())
            setMinimumHeight(ymax);
        resize(xAff, yAff);
    }

    QFont police;

#if defined (Q_OS_WIN)
    police.setFamily("MS Shell Dlg 2");
    police.setPointSize(8);

#elif defined (Q_OS_LINUX)
    police.setFamily("Sans Serif");
    police.setPointSize(7);

#elif defined (Q_OS_MAC)
    police.setFamily("Marion");
    police.setPointSize(11);
#else
#endif

    setFont(police);
    QStyle * const styleBouton = QApplication::style();
    ui->actionEnregistrer->setIcon(styleBouton->standardIcon(QStyle::SP_DialogSaveButton));

    if (settings.value("affichage/flagIntensiteVision", false).toBool()) {

        QPalette paletteWin, palList;
        const int red = settings.value("affichage/valIntensiteVision", 0).toInt();
        const QBrush alpha = QBrush(QColor::fromRgb(red, 0, 0, 255));
        const QColor coulList = QColor(red + 27, 0, 0);

        paletteWin.setBrush(this->backgroundRole(), alpha);
        palList.setColor(QPalette::Base, coulList);

        this->setPalette(paletteWin);
        ui->fichier->setPalette(palList);
    }

#if defined (Q_OS_WIN)
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                            QDir::separator() + QCoreApplication::organizationName() + QDir::separator() +
                            QCoreApplication::applicationName()).toString();

#elif defined (Q_OS_LINUX)
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
                            QDir::separator() + QCoreApplication::applicationName()).toString();

#elif defined (Q_OS_MAC)
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) +
                            QDir::separator() + QCoreApplication::applicationName()).toString();

#else
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                            QDir::separator() + QCoreApplication::organizationName() + QDir::separator() +
                            QCoreApplication::applicationName()).toString();
#endif

    dirOut = QDir::convertSeparators(dirOut);
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
}

Afficher::~Afficher()
{
    delete ui;
}

void Afficher::show(const QString &fic)
{
    _fichier = fic;
    QFile fi(_fichier);
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
#if defined (Q_OS_WIN)
    prev = fi.readAll();
#else
    prev = fi.trUtf8(fi.readAll());
#endif
    ui->fichier->setText(prev);
    setVisible(true);
    prev = "";
}

void Afficher::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    ui->fichier->clear();
}

void Afficher::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt)
    ui->fichier->setGeometry(0, 0, width(), height() - ui->barreOutils->height());
}

void Afficher::on_actionEnregistrer_activated()
{
#if defined (Q_OS_WIN)
    const QString nomRepDefaut = dirOut.replace(QDir::separator(), "\\");
#else
    const QString nomRepDefaut = dirOut;
#endif
    const QString nomFicDefaut = _fichier.split(QDir::separator()).last();
    const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."),
                                                         nomRepDefaut + QDir::separator() + nomFicDefaut,
                                                         tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        QFile fi(fichier);
        if (fi.exists())
            fi.remove();

        QFile fi2(_fichier);
        fi2.copy(fi.fileName());
        QFileInfo fi3(fichier);
        settings.setValue("fichier/sauvegarde", fi3.absolutePath());
    }
}
