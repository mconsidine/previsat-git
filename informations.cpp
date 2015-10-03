/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2015  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    informations.cpp
 *
 * Localisation
 * >
 *
 * Heritage
 * >    QMainWindow
 *
 * Description
 * >    Fenetre d'affichage des informations satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    3 octobre 2015
 *
 * Date de revision
 * >
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QSettings>
#include <QTextStream>
#include "informations.h"
#include "librairies/corps/systemesolaire/TerreConstants.h"
#include "librairies/maths/maths.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include "ui_informations.h"
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wswitch-default"

static bool chg;
static QSettings settings("Astropedia", "previsat");
static QString magn;
static QStringList res;


Informations::Informations(const QString &norad, QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    ui(new Ui::Informations)
{
    ui->setupUi(this);

#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    const QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

    const QString ficData = dirLocalData + QDir::separator() + "donnees.sat";

    QFile donneesSatellites(ficData);
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&donneesSatellites);
        magn = flux.readAll().toLower();
    } else {
        magn = "";
    }
    donneesSatellites.close();

    chg = true;
    ui->frameResultats->setVisible(false);
    ui->norad->setValue(norad.toInt());
}

Informations::~Informations()
{
    delete ui;
}

void Informations::closeEvent(QCloseEvent *evt)
{
    Q_UNUSED(evt)
    magn = "";
}

void Informations::on_nom_returnPressed()
{
    /* Declarations des variables locales */

    /* Initialisations */
    int indx1 = 0;
    int indx2 = 0;
    chg = false;
    res.clear();

    /* Corps de la methode */
    do {
        indx1 = magn.indexOf(ui->nom->text().toLower().trimmed(), indx1 + indx2);
        if (indx1 >= 0) {
            indx1 = magn.lastIndexOf("\n", indx1) + 1;
            indx2 = magn.indexOf("\n", indx1) - indx1;
            const QString ligne = magn.mid(indx1, indx2);
            if (ligne.length() > 0)
                res.append(ligne);
        }
    } while (indx1 > 0);

    ui->cospar->setText(res.at(0).mid(6, 11).toUpper().trimmed());
    ui->norad->setValue(res.at(0).mid(0, 5).toInt());
    chg = true;
    AffichageResultats();

    /* Retour */
    return;
}

void Informations::on_norad_valueChanged(int arg1)
{
    /* Declarations des variables locales */
    int indx1 = 0;
    int indx2 = 0;
    const QString chaine = "%1 ";
    const QString norad = chaine.arg(arg1, 5, 10, QChar('0'));

    /* Initialisations */
    if (chg) {
        res.clear();
        do {
            indx1 = magn.indexOf(norad, indx1 + 1);
        } while (magn.at(indx1 - 1) != '\n');
        if (indx1 >= 0)
            indx2 = magn.indexOf("\n", indx1) - indx1;

        /* Corps de la methode */
        const QString ligne = magn.mid(indx1, indx2).trimmed();
        if (ligne.length() > 0)
            res.append(ligne);

        QString nom = ligne.mid(117).trimmed();
        if (nom == "iss (zarya)")
            nom = "ISS";
        ui->nom->setText(nom.toUpper());

        ui->cospar->setText(ligne.mid(6, 11).toUpper());

        AffichageResultats();
    }

    /* Retour */
    return;
}

void Informations::on_cospar_returnPressed()
{
    /* Declarations des variables locales */
    int indx1 = 0;
    int indx2 = 0;

    /* Initialisations */
    chg = false;
    res.clear();

    /* Corps de la methode */
    do {
        indx1 = magn.indexOf(ui->cospar->text().toLower().trimmed(), indx1 + indx2);
        if (indx1 >= 0) {
            indx1 = magn.lastIndexOf("\n", indx1) + 1;
            indx2 = magn.indexOf("\n", indx1) - indx1;
            const QString ligne = magn.mid(indx1, indx2);
            if (ligne.length() > 0)
                res.append(ligne);
        }
    } while (indx1 > 0);

    ui->nom->setText(res.at(0).mid(117).toUpper().trimmed());
    ui->norad->setValue(res.at(0).mid(0, 5).toInt());
    chg = true;
    AffichageResultats();

    /* Retour */
    return;
}

void Informations::AffichageResultats()
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->satellitesTrouves->clear();

    /* Corps de la methode */
    if (res.count() > 0) {

        QStringListIterator it(res);
        while (it.hasNext()) {
            const QString item = it.next().toUpper();
            QString nom = item.mid(117).trimmed();

            if (nom.toLower() == "iss (zarya)")
                nom = "ISS";
            if (nom.length() == 0)
                nom = item.mid(0, 5);

            ui->satellitesTrouves->addItem(nom);
        }
        ui->satellitesTrouves->setCurrentRow(0);
        ui->frameResultats->setVisible(true);
    }

    /* Retour */
    return;
}

void Informations::on_satellitesTrouves_currentRowChanged(int currentRow)
{
    /* Declarations des variables locales */
    double xval, xval2;
    QString chaine;

    /* Initialisations */

    /* Corps de la methode */
    if (currentRow >= 0) {

        const QString ligne = res.at(currentRow).toUpper();
        const double magnitudeStandard = ligne.mid(34, 4).toDouble();
        const QString periode = ligne.mid(70, 10).trimmed();
        const QString perigee = ligne.mid(81, 7).trimmed();
        const QString apogee = ligne.mid(89, 7).trimmed();
        const double ap = apogee.toDouble() + RAYON_TERRESTRE;
        const double per = perigee.toDouble() + RAYON_TERRESTRE;
        const QString dateRentree = ligne.mid(59, 10).trimmed();

        // Nom du satellite
        QString nom = ligne.mid(117).trimmed();
        if (nom == "iss (zarya)")
            nom = "ISS";
        ui->nomsat->setText(nom);

        // Numero NORAD
        ui->numNorad->setText(ligne.mid(0, 5));

        // Designation COSPAR
        ui->desigCospar->setText(ligne.mid(6, 11));

        // Magnitude standard/maximale
        if (magnitudeStandard > 98. || perigee.isEmpty() || apogee.isEmpty()) {
            ui->magnitudeStdMax->setText("?/?");
        } else {

            // Estimation de la magnitude maximale
            const double demiGrandAxe = 0.5 * (ap + per);
            const double exc = 2. * ap / (ap + per) - 1.;
            xval = magnitudeStandard - 15.75 + 5. * log10(1.45 * (demiGrandAxe * (1. - exc) - RAYON_TERRESTRE));
            char methMagnitude = ligne.at(39).toLower().toLatin1();

            chaine = "%1%2%3/%4%5";
            chaine = chaine.arg((magnitudeStandard >= 0.) ? "+" : "-").arg(fabs(magnitudeStandard), 0, 'f', 1).
                    arg(methMagnitude).arg((xval >= 0.) ? "+" : "-").arg(fabs(xval), 0, 'f', 1);
            ui->magnitudeStdMax->setText(chaine);
        }

        // Modele orbital
        const QString modele = (periode.toDouble() > 225.) ? tr("SGP4 (DS)") : tr("SGP4 (NE)");
        ui->modele->setText((periode.isEmpty()) ? tr("Non applicable") : modele);

        // Dimensions du satellite
        double t1 = ligne.mid(18, 5).toDouble();
        double t2 = ligne.mid(24, 4).toDouble();
        double t3 = ligne.mid(29, 4).toDouble();
        double section = ligne.mid(41, 6).toDouble();
        QString unite1 = tr("m");
        const bool unite = settings.value("affichage/unite", true).toBool();
        if (!unite) {
            t1 *= PIED_PAR_METRE;
            t2 *= PIED_PAR_METRE;
            t3 *= PIED_PAR_METRE;
            section = arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
            unite1 = tr("ft");
        }

        QString chaine2 = "%1 " + unite1;
        if (fabs(t2) < EPSDBL100 && fabs(t3) < EPSDBL100)
            chaine = tr("Sphérique. R=").append(chaine2.arg(t1, 0, 'f', 1));

        if (fabs(t2) >= EPSDBL100 && fabs(t3) < EPSDBL100)
            chaine = tr("Cylindrique. L=").append(chaine2.arg(t1, 0, 'f', 1)).
                    append(tr(", R=")).append(chaine2.arg(t2, 0, 'f', 1));

        if (fabs(t2) >= EPSDBL100 && fabs(t3) >= EPSDBL100) {
            chaine = tr("Boîte.").append(" %1 x %2 x %3 ").append(unite1);
            chaine = chaine.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1);
        }

        if (fabs(t1) < EPSDBL100)
            chaine = tr("Inconnues");

        if (fabs(section) > EPSDBL100) {
            chaine = chaine + " / %1 " + unite1;
            chaine = chaine.arg(section, 0, 'f', 2);
            ui->sq->setVisible(true);
        } else {
            ui->sq->setVisible(false);
        }

        ui->dimensions->setText(chaine);
        ui->dimensions->adjustSize();
        ui->dimensions->setFixedHeight(16);
        ui->sq->move(ui->dimensions->x() + ui->dimensions->width() + 1, ui->sq->y());

        // Apogee/perigee/periode orbitale
        unite1 = (unite) ? tr("km") : tr("nmi");
        if (apogee.isEmpty()) {
            ui->apogee->setText(tr("Inconnu"));

        } else {
            chaine2 = "%2 %1 (%3 %1)";
            chaine2 = chaine2.arg(unite1);
            xval = (unite) ? ap : ap * MILE_PAR_KM;
            xval2 = (unite) ? apogee.toDouble() : apogee.toDouble() * MILE_PAR_KM;
            chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval2, 0, 'f', 1);
            ui->apogee->setText(chaine);
        }

        if (perigee.isEmpty() || perigee == "0") {
            ui->perigee->setText(tr("Inconnu"));
        } else {
            xval = (unite) ? per : per * MILE_PAR_KM;
            xval2 = (unite) ? perigee.toDouble() : perigee.toDouble() * MILE_PAR_KM;
            chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval2, 0, 'f', 1);
            ui->perigee->setText(chaine);
        }

        chaine = (periode.isEmpty()) ? tr("Inconnue") :
                                       Maths::ToSexagesimal(periode.toDouble() * NB_HEUR_PAR_MIN * HEUR2RAD, HEURE1, 1, 0, false, true);
        ui->periode->setText(chaine);

        // Inclinaison
        const QString inclinaison = ligne.mid(97, 6).trimmed();
        ui->inclinaison->setText((inclinaison.isEmpty()) ? tr("Inconnue") : inclinaison + "°");

        // Date de lancement
        ui->dateLancement->setText(ligne.mid(48, 10).trimmed());

        // Date de rentree
        if (dateRentree.isEmpty()) {
            ui->lbl_dateRentree->setVisible(false);
            ui->dateRentree->setVisible(false);

            ui->lbl_categorieOrbite->move(250, 70);
            ui->categorieOrbite->move(361, 70);

            ui->lbl_pays->move(250, 85);
            ui->pays->move(361, 85);
        } else {

            ui->dateRentree->setText(dateRentree);
            ui->lbl_dateRentree->setVisible(true);
            ui->dateRentree->setVisible(true);

            ui->lbl_categorieOrbite->move(250, 85);
            ui->categorieOrbite->move(361, 85);

            ui->lbl_pays->move(250, 100);
            ui->pays->move(361, 100);
        }

        // Categorie d'orbite
        const QString categorie = ligne.mid(104, 6).trimmed();
        ui->categorieOrbite->setText((categorie.isEmpty()) ? tr("Inconnue") : categorie);

        // Pays/Organisation
        const QString pays = ligne.mid(111, 5);
        ui->pays->setText((pays.isEmpty()) ? tr("Inconnu") : pays);
    }

    /* Retour */
    return;
}
