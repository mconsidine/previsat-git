/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2012  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include "afficher.h"
#include "ui_afficher.h"

static QString dirOut;
static QString dirTmp;
static QSettings settings("Astropedia", "previsat");

Afficher::Afficher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Afficher)
{
    ui->setupUi(this);
    QStyle *style = QApplication::style();
    ui->actionEnregistrer->setIcon(style->standardIcon(QStyle::SP_DialogSaveButton));
    QCoreApplication::setApplicationName("PreviSat");
    QCoreApplication::setOrganizationName("Astropedia");
    dirOut = settings.value("fichier/sauvegarde", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) +
                            QDir::separator() + "Astropedia" + QDir::separator() + "PreviSat").toString();
    dirTmp = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
}

Afficher::~Afficher()
{
    delete ui;
}

void Afficher::show(const QString fic)
{
    QFile fi(fic);
    fi.open(QIODevice::ReadOnly | QIODevice::Text);
    QString prev = fi.readAll();
    ui->fichier->setText(prev);
    setVisible(true);
}

void Afficher::resizeEvent(QResizeEvent *event)
{
    ui->fichier->setGeometry(0, 0, Afficher::width(), Afficher::height() - ui->barreOutils->height());
}

void Afficher::on_actionEnregistrer_activated()
{
    const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), dirOut,
                                                   tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
    if (!fichier.isEmpty()) {
        QFile fi(fichier);
        if (fi.exists())
            fi.remove();

        QFile fi2(dirTmp + QDir::separator() + "prevision.txt");
        fi2.copy(fi.fileName());
        QFileInfo fi3(fichier);
        settings.setValue("fichier/sauvegarde", fi3.absolutePath());
    }
}
