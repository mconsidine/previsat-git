/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2020  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >    interface
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    4 mars 2011
 *
 * Date de revision
 * >    5 octobre 2020
 *
 */

#include <QDir>
#include <QFile>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QFileDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "ui_afficher.h"
#pragma GCC diagnostic warning "-Wconversion"
#include <QTextStream>
#include "afficher.h"
#include "configuration/configuration.h"
#include "librairies/maths/maths.h"

Q_DECLARE_METATYPE(QList<ResultatPrevisions>)


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Afficher::Afficher(const TypeCalcul &typeCalcul, const ConditionsPrevisions &conditions, const DonneesPrevisions &donnees,
                   const QMap<QString, QList<QList<ResultatPrevisions> > > &resultats,
                   QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Afficher)
{
    /* Declarations des variables locales */
    QStringList titres;

    /* Initialisations */
    _typeCalcul = typeCalcul;
    _conditions = conditions;
    _donnees = donnees;
    _resultats = resultats;

    /* Corps du constructeur */
#if (BUILDTEST == false)
    ui->setupUi(this);

    QStyle * const styleBouton = QApplication::style();
    ui->actionEnregistrer->setIcon(styleBouton->standardIcon(QStyle::SP_DialogSaveButton));

    ui->resultatsPrevisions->clear();
    ui->detailsTransit->setVisible(false);

    ui->resultatsPrevisions->setColumnCount(6);

    QFont fnt;
    fnt.setBold(true);
    ui->resultatsPrevisions->horizontalHeader()->setFont(fnt);

    switch (_typeCalcul) {
    case PREVISIONS:
        setWindowTitle(tr("Prévisions de passage"));
        titres << tr("Satellite") << tr("Date de début") << tr("Date de fin") << tr("Hauteur max") << tr("Magn") << tr("Hauteur Soleil");
        break;

    case EVENEMENTS:
        setWindowTitle(tr("Évènements orbitaux"));
        titres << tr("Satellite") << tr("Date") << tr("PSO") << tr("Longitude") << tr("Latitude") << tr("Évènement");
        break;

    case FLASHS:
        setWindowTitle(tr("Flashs"));
        ui->resultatsPrevisions->setColumnCount(7);
        titres << tr("Satellite") << tr("Date de début") << tr("Date de fin") << tr("Hauteur Max") << tr("Magn") << tr("Mir") << tr("Hauteur Soleil");
        break;

    case TRANSITS:
        setWindowTitle(tr("Transits ISS"));
        titres << tr("Date de début") << tr("Date de fin") << tr("Angle") << tr("Type") << tr("Corps") << tr("Hauteur Soleil");
        ui->detailsTransit->setVisible(true);
        break;

    case TELESCOPE:
    default:
        break;
    }

    ui->resultatsPrevisions->setHorizontalHeaderLabels(titres);

    if (_resultats.isEmpty()) {

    } else {
        ChargementResultats();
    }
#endif

    /* Retour */
    return;
}

Afficher::~Afficher()
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
void Afficher::on_resultatsPrevisions_itemDoubleClicked(QTableWidgetItem *item)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (_typeCalcul != EVENEMENTS) {

        int j = 0;
        const QList<ResultatPrevisions> list = ui->resultatsPrevisions->item(item->row(), 0)->data(Qt::UserRole).value<QList<ResultatPrevisions> > ();

        tableDetail = new QTableWidget;

        switch (_typeCalcul) {

        case PREVISIONS:
            tableDetail->setColumnCount(12);
            tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date") << tr("Azimut Sat") << tr("Hauteur Sat")
                                                   << tr("AD Sat") << tr("Decl Sat") << tr("Const") << tr("Magn") << tr("Altitude") << tr("Distance")
                                                   << tr("Az Soleil") << tr("Haut Soleil"));
            break;

        case FLASHS:
            tableDetail->setColumnCount(18);
            tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Satellite") << tr("Date") << tr("Azimut Sat") << tr("Hauteur Sat")
                                                   << tr("AD Sat") << tr("Decl Sat") << tr("Const") << tr("Ang") << tr("Mir") << tr("Magn")
                                                   << tr("Altitude") << tr("Dist") << tr("Az Soleil") << tr("Haut Soleil") << tr("Long Max")
                                                   << tr("Lat Max") << tr("Magn Max") << tr("Distance"));
            break;

        case TRANSITS:
            tableDetail->setColumnCount(17);
            tableDetail->setHorizontalHeaderLabels(QStringList() << tr("Date") << tr("Azimut Sat") << tr("Hauteur Sat") << tr("AD Sat")
                                                   << tr("Decl Sat") << tr("Cst") << tr("Ang") << tr("Type") << tr("Corps") << tr("Alt") << tr("Dist")
                                                   << tr("Az Soleil") << tr("Haut Soleil") << tr("Long Max") << tr("Lat Max") << tr("Distance"));
            break;

        case TELESCOPE:
        case EVENEMENTS:
        default:
            break;
        }

        tableDetail->setSelectionMode(QTableWidget::NoSelection);
        tableDetail->setCornerButtonEnabled(false);
        tableDetail->verticalHeader()->setVisible(false);

        QFont fnt;
        fnt.setBold(true);
        tableDetail->horizontalHeader()->setFont(fnt);

        QListIterator<ResultatPrevisions> it(list);
        while (it.hasNext()) {

            QStringList elems;
            const ResultatPrevisions res = it.next();

            switch (_typeCalcul) {
            case PREVISIONS:
                elems = ElementsDetailsPrevisions(res);
                break;

            case FLASHS:
                elems = ElementsDetailsFlashs(res);
                break;

            case TRANSITS:
                elems = ElementsDetailsTransits(res);
                break;

            case TELESCOPE:
            case EVENEMENTS:
            default:
                break;
            }

            // Ajout d'une ligne dans le tableau de resultats
            tableDetail->insertRow(j);
            tableDetail->setRowHeight(j, 16);

            int kmin;
            int kmax;
            if (_typeCalcul == TRANSITS) {
                kmin = 1;
                kmax = 4;
            } else {
                kmin = 0;
                kmax = elems.count();
            }

            for(int k=kmin; k<kmax; k++) {

                // Remplissage des elements d'une ligne
                QTableWidgetItem * const itm = new QTableWidgetItem(elems.at(k));
                itm->setTextAlignment(Qt::AlignCenter);
                itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
                if (k == 0) {
                    itm->setToolTip(elems.at(0));
                }

                tableDetail->setItem(j, k, itm);
                tableDetail->resizeColumnToContents(k);
            }

            j++;
        }

        tableDetail->setSelectionMode(QTableWidget::SingleSelection);
        tableDetail->setSelectionBehavior(QTableWidget::SelectRows);
#if QT_VERSION >= 0x050000
        tableDetail->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
        tableDetail->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

        afficherDetail = new QMainWindow;
        afficherDetail->setStyleSheet("QHeaderView::section { background-color:rgb(235, 235, 235) }");

        switch (_typeCalcul) {

        case PREVISIONS:
            afficherDetail->setWindowTitle(tr("Détail du passage"));
            break;

        case FLASHS:
            afficherDetail->setWindowTitle(tr("Détail du flash"));
            break;

        case TRANSITS:
            afficherDetail->setWindowTitle(tr("Détail du transit ou conjonction"));
            break;

        case TELESCOPE:
        case EVENEMENTS:
        default:
            break;
        }

        afficherDetail->setCentralWidget(tableDetail);
#if defined (Q_OS_LINUX)
        int lrg = 5;
#else
        int lrg = 2;
#endif
        if (tableDetail->rowCount() > 10) {
            lrg += 17;
        }
        for(int i=0; i<tableDetail->columnCount(); i++) {
            lrg += tableDetail->columnWidth(i);
        }

        afficherDetail->setFixedSize(lrg, tableDetail->horizontalHeader()->height() + tableDetail->rowHeight(1) * qMin(10, tableDetail->rowCount()));
        afficherDetail->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, afficherDetail->size(), geometry()));
        afficherDetail->show();
    }

    /* Retour */
    return;
}

void Afficher::on_actionEnregistrerTxt_triggered()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
#if (BUILDTEST == true)
    const QString fic = _conditions.ficRes;
#else
    const QString nomFicDefaut = _conditions.ficRes.split(QDir::separator()).last();
    const QString fic = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), Configuration::instance()->dirOut() + QDir::separator() +
                                                     nomFicDefaut, tr("Fichiers texte (*.txt);;Tous les fichiers (*)"));
#endif

    if (!fic.isEmpty()) {

        QFile fichier(_conditions.ficRes);
        if (fichier.exists()) {
            fichier.remove();
        }

        // Ecriture de l'entete du fichier de resultat
        EcrireEntete();

        // Ouverture du fichier de resultat
        fichier.open(QIODevice::Append | QIODevice::Text);
        QTextStream flux(&fichier);

        bool ecrireNomColonnes = true;
        QMapIterator<QString, QList<QList<ResultatPrevisions> > > it1(_resultats);
        while (it1.hasNext()) {
            it1.next();

            // Nom du satellite
            QString nomsat = it1.value().at(0).at(0).nom;
            if (nomsat.contains("R/B") || nomsat.contains(" DEB")) {
                nomsat += "  " + tr("(numéro NORAD : %1)").arg(it1.key().split(" ").last());
            }

            // Description des colonnes
            switch (_typeCalcul) {

            case PREVISIONS:
                flux << nomsat << endl;
                flux << tr("   Date      Heure    Azimut Sat Hauteur Sat  AD Sat    Decl Sat  Const Magn  Altitude  Distance  Az Soleil  Haut Soleil")
                     << endl;
                break;

            case FLASHS:
                flux << tr("Satellite     Date      Heure      Azimut Sat Hauteur Sat  AD Sat    Decl Sat   Cst  Ang  Mir Magn" \
                           "       Alt      Dist  Az Soleil  Haut Soleil   Long Max    Lat Max    Magn Max  Distance")
                     << endl;
                break;

            case TRANSITS:
                if (ecrireNomColonnes) {
                    flux << tr("   Date      Heure      Azimut Sat Hauteur Sat  AD Sat    Decl Sat   Cst  Ang  Type Corps" \
                               " Ill    Alt    Dist  Az Soleil  Haut Soleil   Long Max    Lat Max     Distance")
                         << endl;
                    ecrireNomColonnes = false;
                }
                break;

            case EVENEMENTS:
                flux << nomsat << endl;
                flux << tr("   Date      Heure      PSO    Longitude  Latitude   Évènements") << endl;
                break;

            case TELESCOPE:
            default:
                break;
            }

            QListIterator<QList<ResultatPrevisions> > it2(it1.value());
            while (it2.hasNext()) {

                int i = 0;
                QStringList evts;
                QListIterator<ResultatPrevisions> it3(it2.next());
                while (it3.hasNext()) {

                    int kmin = 0;
                    QStringList elems;
                    const ResultatPrevisions res = it3.next();

                    switch (_typeCalcul) {
                    case PREVISIONS:
                        kmin = 1;
                        elems = ElementsDetailsPrevisions(res);
                        break;

                    case FLASHS:
                        elems = ElementsDetailsFlashs(res);
                        break;

                    case TRANSITS:
                        if ((i > 0) && (i < 4)) {
                            elems = ElementsDetailsTransits(res);
                        }
                        break;

                    case EVENEMENTS:
                        kmin = 1;
                        elems = ElementsDetailsEvenements(res);
                        break;

                    case TELESCOPE:
                    default:
                        break;
                    }

                    if (!elems.isEmpty()) {
                        QString ligne;
                        for(int k=kmin; k<elems.count(); k++) {
                            ligne += elems.at(k) + " ";
                        }

                        if (_typeCalcul == EVENEMENTS) {
                            evts.append(ligne);
                        } else {
                            flux << ligne.trimmed() << endl;
                        }
                    }
                    i++;
                }

                if (_typeCalcul == EVENEMENTS) {

                    evts.sort();
                    QStringListIterator it4(evts);
                    while (it4.hasNext()) {
                        flux << it4.next().trimmed() << endl;
                    }
                }
                flux << endl;
            }
        }

#if (BUILDTEST == false)
        flux << endl << tr("Temps écoulé : %1s").arg(1.e-3 * static_cast<double> (_donnees.tempsEcoule), 0, 'f', 2) << endl;
#endif
        fichier.close();

#if (BUILDTEST == false)
        QFile fi2(fic);
        fichier.copy(fi2.fileName());
#endif
    }

    /* Retour */
    return;
}

/*
 * Chargement des resultats
 */
void Afficher::ChargementResultats() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    int j = 0;

    /* Corps de la methode */
    QMapIterator<QString, QList<QList<ResultatPrevisions> > > it1(_resultats);
    while (it1.hasNext()) {
        it1.next();

        QListIterator<QList<ResultatPrevisions> > it2(it1.value());
        while (it2.hasNext()) {

            const QList<ResultatPrevisions> list = it2.next();

            QStringList elems;
            switch (_typeCalcul) {

            case PREVISIONS:
                elems = ElementsPrevisions(list);
                break;

            case FLASHS:
                elems = ElementsFlashs(list);
                break;

            case TRANSITS:
                elems = ElementsTransits(list);
                break;

            case EVENEMENTS:
                elems = ElementsEvenements(list);
                break;

            case TELESCOPE:
            default:
                break;
            }

            // Ajout d'une ligne dans le tableau de resultats
            ui->resultatsPrevisions->insertRow(j);
            ui->resultatsPrevisions->setRowHeight(j, 16);

            for(int k=0; k<elems.count(); k++) {

                // Remplissage des elements d'une ligne
                QTableWidgetItem * const item = new QTableWidgetItem(elems.at(k));
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);

                if (k == 0) {
                    item->setToolTip(elems.at(0));
                    item->setData(Qt::UserRole, QVariant::fromValue<QList<ResultatPrevisions> > (list));
                }

                ui->resultatsPrevisions->setItem(j, k, item);
                if (k > 0) {
                    ui->resultatsPrevisions->resizeColumnToContents(k);
                }
            }

            j++;
        }
    }
    ui->resultatsPrevisions->setStyleSheet("QHeaderView::section { background-color:rgb(235, 235, 235) }");
    ui->resultatsPrevisions->horizontalHeader()->setStretchLastSection(true);
    ui->resultatsPrevisions->setToolTip(tr("Double-cliquez sur une ligne pour afficher plus de détails"));
#if QT_VERSION >= 0x050000
    ui->resultatsPrevisions->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    ui->resultatsPrevisions->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    ui->resultatsPrevisions->setAlternatingRowColors(true);

    /* Retour */
    return;
}

/*
 * Ecriture de l'entete du fichier de resultats
 */
void Afficher::EcrireEntete() const
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString ligne;
    QFile fi(_conditions.ficRes);
    if (fi.exists()) {
        fi.remove();
    }

    QDir di(Configuration::instance()->dirTmp());
    if (!di.exists()) {
        di.mkpath(Configuration::instance()->dirTmp());
    }

    /* Corps de la methode */
    QFile fichier(_conditions.ficRes);
    fichier.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flux(&fichier);

    // Ligne d'entete
#if (BUILDTEST == false)
    flux << QString("%1 %2 / %3 (c) %4").arg(QCoreApplication::applicationName()).arg(QString(APPVERSION)).arg(QCoreApplication::organizationName()).
            arg(QString(APP_ANNEES_DEV)) << endl << endl;
#endif

    // Lieu d'observation
    if (_typeCalcul != EVENEMENTS) {

        ligne = QObject::tr("Lieu d'observation        : %1     %2 %3   %4 %5   %6 %7");
        const QString lon = Maths::ToSexagesimal(fabs(_conditions.observateur.longitude()), DEGRE, 3, 0, false, false);
        const QString ew = (_conditions.observateur.longitude() >= 0.) ? QObject::tr("Ouest") : QObject::tr("Est");

        const QString lat = Maths::ToSexagesimal(fabs(_conditions.observateur.latitude()), DEGRE, 2, 0, false, false);
        const QString ns = (_conditions.observateur.latitude() >= 0.) ? QObject::tr("Nord") : QObject::tr("Sud");

        const double alt = (_conditions.unite == QObject::tr("km")) ? _conditions.observateur.altitude() :
                                                                      _conditions.observateur.altitude() * PIED_PAR_METRE;
        const QString unite = (_conditions.unite == QObject::tr("km")) ? QObject::tr("m") : QObject::tr("ft");

        ligne = ligne.arg(_conditions.observateur.nomlieu()).arg(lon).arg(ew).arg(lat).arg(ns).arg(1000. * alt, 0, 'f', 0).arg(unite);
        flux << ligne << endl;
    }

    // Fuseau horaire
    ligne = QObject::tr("Fuseau horaire            : %1");
    QString chaine = QObject::tr("UTC");
    if (_conditions.ecart) {
        if (fabs(_conditions.offset) > EPSDBL100) {
            QTime heur(0, 0);
            heur = heur.addSecs((int) (_conditions.offset * NB_SEC_PAR_JOUR + EPS_DATES));
            chaine = chaine.append((_conditions.offset > 0.) ? " + " : " - ").append(heur.toString("hh:mm"));
        }
    }
    flux << ligne.arg(chaine) << endl;

    if (_typeCalcul != EVENEMENTS) {

        const QString cond1 = QObject::tr("Conditions d'observations :") + " ";
        const QString cond2 = QObject::tr("Hauteur minimale du satellite = %1°");

        // Conditions d'observations
        if (_typeCalcul == TRANSITS) {
            flux << (cond1 + cond2).arg(_conditions.hauteur * RAD2DEG) << endl;
        } else {
            flux << cond1 + QObject::tr("Hauteur maximale du Soleil = %1°").arg(_conditions.crepuscule * RAD2DEG) << endl;
            flux << QString(cond1.size(), ' ') << cond2.arg(_conditions.hauteur * RAD2DEG) << endl;
        }

        // Unite de distance
        flux << tr("Unité de distance         : %1").arg(_conditions.unite) << endl;
    }

    // Age des TLE
    const QString date = Date(_conditions.jj1 + _conditions.offset + EPS_DATES, 0.)
            .ToShortDate(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H).trimmed();

    if (_donnees.ageTle.count() == 1) {
        flux << endl << tr("Age du TLE                : %1 jours (au %2)").arg(_donnees.ageTle.at(0), 4, 'f', 2).arg(date) << endl << endl << endl;
    } else {

        flux << endl << tr("Age du TLE le plus récent : %1 jours (au %2)\nAge du TLE le plus ancien : %3 jours")
                .arg(_donnees.ageTle.at(0), 4, 'f', 2).arg(date).arg(_donnees.ageTle.at(1), 4, 'f', 2) << endl << endl << endl;
    }

    /* Retour */
    return;
}

/*
 * Elements des evenements a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsEvenements(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {
        elems.append(ElementsDetailsEvenements(it.next()));
    }

    /* Retour */
    return elems;
}

/*
 * Elements des evenements pour la sauvegarde dans un fichier texte
 */
QStringList Afficher::ElementsDetailsEvenements(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // PSO
    elems.append(QString("%1°").arg(res.pso * RAD2DEG, 6, 'f', 2, QChar('0')));

    // Longitude, latitude
    elems.append(QString("  %1° %2").arg(fabs(res.longitude * RAD2DEG), 6, 'f', 2, QChar('0')).arg((res.longitude >= 0.) ? tr("W") : tr("E")));
    elems.append(QString(" %1° %2 ").arg(fabs(res.latitude * RAD2DEG), 5, 'f', 2, QChar('0')).arg((res.latitude >= 0.) ? tr("N") : tr("S")));

    // Type d'evenement
    elems.append(res.typeEvenement);

    /* Retour */
    return elems;
}

/*
 * Elements des flashs a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsFlashs(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(liste.first().nom);

    // Date de debut
    const Date dateDeb(liste.first().date, Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1)));
    elems.append(dateDeb.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Date de fin
    const Date dateFin(liste.last().date, Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1)));
    elems.append(dateFin.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    double htMax = -1.;
    double magnMax = 99.;
    double htSolMax;
    bool penombre;
    QString miroir;
    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Hauteur max
        if (res.hauteur >= htMax) {
            htMax = res.hauteur;
            htSolMax = res.hauteurSoleil;
        }

        // Magnitude maximale
        if (res.magnitude <= magnMax) {
            magnMax = res.magnitude;
            penombre = res.penombre;
            miroir = res.miroir;
        }
    }

    // Hauteur maximale du satellite
    elems.append(Maths::ToSexagesimal(htMax, DEGRE, 2, 0, false, false));

    // Magnitude
    const QString fmagn = "%1%2%3";
    const QString signe = (magnMax >= 0.) ? "+" : "-";
    const QString pen = (penombre) ? "*" : " ";
    elems.append(fmagn.arg(signe).arg(fabs(magnMax), 0, 'f', 1, QChar('0')).arg(pen));

    // Panneau reflechissant
    elems.append(miroir);

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des flashs pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList Afficher::ElementsDetailsFlashs(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(QString("%1").arg(res.nom, -10));

    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Angle de reflexion, miroir
    elems.append(QString("%1  ").arg(res.angleReflexion * RAD2DEG, 5, 'f', 2));
    elems.append(QString("%1 ").arg(res.miroir));

    // Magnitude
    const QString fmagn = "%1%2%3";
    QString signe = (res.magnitude >= 0.) ? "+" : "-";
    QString pen = (res.penombre) ? "*" : " ";
    elems.append(fmagn.arg(signe).arg(fabs(res.magnitude), 0, 'f', 1, QChar('0')).arg(pen));

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, DEGRE, 2, 0, true, false));

    if (!res.obsmax.nomlieu().isEmpty()) {

        const QString ew = (res.obsmax.longitude() >= 0.) ? QObject::tr("W") : QObject::tr("E");
        const QString ns = (res.obsmax.latitude() >= 0.) ? QObject::tr("N") : QObject::tr("S");

        // Longitude du maximum
        elems.append(QString("  %1 %2").arg(fabs(res.obsmax.longitude() * RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew));

        // Latitude du maximum
        elems.append(QString(" %1 %2    ").arg(fabs(res.obsmax.latitude() * RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns));

        // Magnitude au maximum
        signe = (res.magnitudeMax >= 0.) ? "+" : "-";
        pen = (res.penombreMax) ? "*" : " ";
        elems.append(fmagn.arg(signe).arg(fabs(res.magnitudeMax), 0, 'f', 1, QChar('0')).arg(pen));

        // Distance au maximum et cap
        elems.append(QString(" %1 (%2)").arg(res.distanceObs, 6, 'f', 1).arg(res.cap));
    }

    /* Retour */
    return elems;
}

/*
 * Elements des previsions de passage a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsPrevisions(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(liste.first().nom);

    // Date de debut
    const Date dateDeb(liste.first().date, Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1)));
    elems.append(dateDeb.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Date de fin
    const Date dateFin(liste.last().date, Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1)));
    elems.append(dateFin.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    double htMax = -1.;
    double magnMax = 99.;
    double magnStd = 99.;
    double htSolMax;
    bool penombre;
    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Hauteur max
        if (res.hauteur >= htMax) {
            htMax = res.hauteur;
            htSolMax = res.hauteurSoleil;
        }

        // Magnitude maximale
        if (res.magnitude <= magnMax) {
            magnMax = res.magnitude;
            magnStd = res.magnitudeStd;
            penombre = res.penombre;
        }
    }

    // Hauteur maximale du satellite
    elems.append(Maths::ToSexagesimal(htMax, DEGRE, 2, 0, false, false));

    // Magnitude
    if (magnMax > 98.) {
        elems.append((_conditions.eclipse || (magnStd > 98.)) ? "????" : "----");
    } else {
        const QString fmagn = "%1%2%3";
        const QString signe = (magnMax >= 0.) ? "+" : "-";
        const QString pen = (penombre) ? "*" : " ";
        elems.append(fmagn.arg(signe).arg(fabs(magnMax), 0, 'f', 1, QChar('0')).arg(pen));
    }

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des previsions de passage pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList Afficher::ElementsDetailsPrevisions(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    elems.append(res.nom);

    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_COURT, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Magnitude
    if (res.magnitude > 98.) {
        elems.append((_conditions.eclipse || (res.magnitudeStd > 98.)) ? " ???? " : " ---- ");
    } else {
        const QString fmagn = "%1%2%3%4";
        const QString esp = (res.magnitude < 9.95) ? " " : "";
        const QString signe = (res.magnitude >= 0.) ? "+" : "-";
        const QString pen = (res.penombre) ? "*" : " ";
        elems.append(fmagn.arg(esp).arg(signe).arg(fabs(res.magnitude), 0, 'f', 1, QChar('0')).arg(pen));
    }

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 8, 'f', 1));
    elems.append(QString("%1").arg(distance, 9, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des transits ISS a afficher dans la fenetre de resultats
 */
QStringList Afficher::ElementsTransits(const QList<ResultatPrevisions> &liste) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Date de debut
    const Date dateDeb(liste.first().date, Date::CalculOffsetUTC(liste.first().date.ToQDateTime(1)));
    elems.append(dateDeb.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Date de fin
    const Date dateFin(liste.last().date, Date::CalculOffsetUTC(liste.last().date.ToQDateTime(1)));
    elems.append(dateFin.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    double angMin = PI;
    QString type;
    QString corps;
    double htSolMax;
    QListIterator<ResultatPrevisions> it(liste);
    while (it.hasNext()) {

        const ResultatPrevisions res = it.next();

        // Hauteur max
        if (res.angle <= angMin) {
            angMin = res.angle;
            htSolMax = res.hauteurSoleil;
            type = (res.transit) ? tr("T") : tr("C");
            corps = (res.typeCorps == CORPS_SOLEIL) ? tr("S") : tr("L");
        }
    }

    // Angle minimal
    elems.append(QString("%1").arg(angMin, 0, 'f', 2));

    // Type, corps
    elems.append(type);
    elems.append(corps);

    // Hauteur maximale du Soleil
    elems.append(Maths::ToSexagesimal(htSolMax, DEGRE, 2, 0, true, false));

    /* Retour */
    return elems;
}

/*
 * Elements des transits ISS pour la sauvegarde dans un fichier texte ou pour afficher des details
 */
QStringList Afficher::ElementsDetailsTransits(const ResultatPrevisions &res) const
{
    /* Declarations des variables locales */
    QStringList elems;

    /* Initialisations */

    /* Corps de la methode */
    // Date
    const Date date(res.date, Date::CalculOffsetUTC(res.date.ToQDateTime(1)));
    elems.append(date.ToShortDateAMJ(FORMAT_LONG, (_conditions.systeme) ? SYSTEME_24H : SYSTEME_12H));

    // Azimut et hauteur du satellite
    elems.append(Maths::ToSexagesimal(res.azimut, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteur, DEGRE, 2, 0, false, false));

    // Ascension droite, declinaison, constellation
    elems.append(Maths::ToSexagesimal(res.ascensionDroite, HEURE1, 2, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.declinaison, DEGRE, 2, 0, true, false));
    elems.append(" " + res.constellation);

    // Angle de reflexion, type, corps
    elems.append(QString("%1  ").arg(res.angle * RAD2DEG, 5, 'f', 2));
    elems.append(QString("%1   ").arg((res.transit) ? tr("T") : tr("C")));
    elems.append(QString("%1  ").arg((res.typeCorps == CORPS_SOLEIL) ? tr("S") : tr("L")));

    // Illumination
    QString illumination = tr("Lum");
    if (res.eclipse) {
        illumination = tr("Omb");
    }

    if (res.penombre) {
        illumination = tr("Pen");
    }

    elems.append(illumination);

    // Altitude, distance
    double altitude = res.altitude;
    double distance = res.distance;
    if (_conditions.unite == tr("nmi")) {
        altitude *= MILE_PAR_KM;
        distance *= MILE_PAR_KM;
    }
    elems.append(QString("%1").arg(altitude, 6, 'f', 1));
    elems.append(QString("%1").arg(distance, 7, 'f', 1));

    // Azimut et hauteur Soleil
    elems.append(Maths::ToSexagesimal(res.azimutSoleil, DEGRE, 3, 0, false, false));
    elems.append(Maths::ToSexagesimal(res.hauteurSoleil, DEGRE, 2, 0, true, false));

    if (!res.obsmax.nomlieu().isEmpty()) {

        const QString ew = (res.obsmax.longitude() >= 0.) ? QObject::tr("W") : QObject::tr("E");
        const QString ns = (res.obsmax.latitude() >= 0.) ? QObject::tr("N") : QObject::tr("S");

        // Longitude du maximum
        elems.append(QString("  %1 %2").arg(fabs(res.obsmax.longitude() * RAD2DEG), 8, 'f', 4, QChar('0')).arg(ew));

        // Latitude du maximum
        elems.append(QString(" %1 %2 ").arg(fabs(res.obsmax.latitude() * RAD2DEG), 7, 'f', 4, QChar('0')).arg(ns));

        // Distance au maximum et cap
        elems.append(QString(" %1 (%2)").arg(res.distanceObs, 5, 'f', 1).arg(res.cap));
    }

    /* Retour */
    return elems;
}
