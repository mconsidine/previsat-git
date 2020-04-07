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
 * >    tle.cpp
 *
 * Localisation
 * >    librairies.corps.satellite
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    3 mars 2019
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QMessageBox>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QTextStream>
#include "librairies/exceptions/previsatexception.h"
#include "donnees.h"
#include "satellite.h"
#include "tle.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
TLE::TLE()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _nbOrbitesEpoque = 0;
    _argpo = 0.;
    _bstar = 0.;
    _ecco = 0.;
    _inclo = 0.;
    _mo = 0.;
    _no = 0.;
    _omegao = 0.;

    /* Retour */
    return;
}

/*
 * Definition a partir des composantes du TLE
 */
TLE::TLE(const QString &lig0, const QString &lig1, const QString &lig2, const Date &dateDebValid) :
    _ligne0(lig0), _ligne1(lig1), _ligne2(lig2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    // Numero NORAD
    _norad = _ligne1.mid(2, 5);

    // Date de debut de validite du TLE
    _dateDebutValidite = dateDebValid;

    // Designation COSPAR
    int annee = _ligne1.mid(9, 2).toInt();
    annee += (annee < 57) ? AN2000 : 1900;
    _cospar = QString("%1-%2").arg(annee).arg(_ligne1.mid(11, 6).trimmed());

    // Epoque
    int an = _ligne1.mid(18, 2).toInt();
    const double jrs = _ligne1.mid(20, 12).toDouble();
    an += (an < 57) ? AN2000 : 1900;
    const Date date(an, 1, 1., 0.);
    _epoque = Date(date.jourJulienUTC() + jrs - 1., 0., true);

    // Derivees du moyen mouvement
    _ndt20 = (_ligne1.mid(33, 1).trimmed() + "0" + _ligne1.mid(34, 9)).toDouble();
    _ndd60 = 1.e-5 * (_ligne1.mid(44, 6) + "e" + _ligne1.mid(50, 2)).toDouble();

    // Coefficient pseudo-balistique
    _bstar = 1.e-5 * (_ligne1.mid(53, 6) + "e" + _ligne1.mid(59, 2)).toDouble();

    // Elements orbitaux moyens
    // Inclinaison
    _inclo = _ligne2.mid(8, 8).toDouble();

    // Ascension droite du noeud ascendant
    _omegao = _ligne2.mid(17, 8).toDouble();

    // Excentricite
    _ecco = 1.e-7 * _ligne2.mid(26, 7).toDouble();

    // Argument du perigee
    _argpo = _ligne2.mid(34, 8).toDouble();

    // Anomalie moyenne
    _mo = _ligne2.mid(43, 8).toDouble();

    // Moyen mouvement
    _no = _ligne2.mid(52, 11).toDouble();

    // Nombre d'orbites a l'epoque
    _nbOrbitesEpoque = _ligne2.mid(63, 5).toUInt();

    /* Retour */
    return;
}


/*
 * Accesseurs
 */
unsigned int TLE::nbOrbitesEpoque() const
{
    return _nbOrbitesEpoque;
}

Date TLE::dateDebutValidite() const
{
    return _dateDebutValidite;
}

double TLE::argpo() const
{
    return _argpo;
}

double TLE::bstar() const
{
    return _bstar;
}

double TLE::ecco() const
{
    return _ecco;
}

Date TLE::epoque() const
{
    return _epoque;
}

double TLE::inclo() const
{
    return _inclo;
}

QString TLE::ligne0() const
{
    return _ligne0;
}

QString TLE::ligne1() const
{
    return _ligne1;
}

QString TLE::ligne2() const
{
    return _ligne2;
}

double TLE::mo() const
{
    return _mo;
}

double TLE::no() const
{
    return _no;
}

QString TLE::nom() const
{
    return _nom;
}

double TLE::omegao() const
{
    return _omegao;
}

QString TLE::norad() const
{
    return _norad;
}

QString TLE::cospar() const
{
    return _cospar;
}

Donnees TLE::donnees() const
{
    return _donnees;
}


/*
 * Methodes publiques
 */
/*
 * Verification du fichier TLE
 */
int TLE::VerifieFichier(const QString &nomFichier, const bool alarme)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int nb = 0;

    /* Corps de la methode */
    try {

        QFile fi(nomFichier);
        if (fi.exists()) {

            fi.open(QIODevice::ReadOnly | QIODevice::Text);

            int itle = 0;
            QTextStream flux(&fi);

            while (!flux.atEnd()) {

                const QString ligne = flux.readLine();

                QString lig0;
                QString lig1;
                QString lig2;

                lig0 = ligne;
                if (ligne.mid(0, 2) == "1 ") {

                    // Cas des fichiers a 2 lignes
                    lig1 = ligne;

                } else {

                    // Lecture ligne 1
                    do {
                        lig1 = flux.readLine();
                    } while ((lig1.trimmed().length() == 0) && !flux.atEnd());
                }

                // Lecture ligne 2
                do {
                    lig2 = flux.readLine();
                } while ((lig2.trimmed().length() == 0) && !flux.atEnd());

                const QString nomsat = RecupereNomsat(lig0);
                VerifieLignes(lig1, lig2, nomsat, alarme);

                if (((lig1 == lig0) && (itle == 3)) || ((lig1 != lig0) && (itle== 2))) {
                    QString msg = "";
                    if (alarme) {
                        msg = QObject::tr("Le fichier %1 n'est pas valide").arg(nomFichier);
                    }
                    throw PreviSatException(msg, WARNING);
                }

                itle = (lig1 == lig0) ? 2 : 3;
                nb++;
            }
        } else {

            // Le fichier n'existe pas
            QString msg = "";
            if (alarme) {
                msg = QObject::tr("Le fichier %1 n'existe pas").arg(nomFichier);
            }
            throw PreviSatException(msg, WARNING);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            QString msg = "";
            if (alarme) {
                msg = QObject::tr("Le fichier %1 est vide").arg(nomFichier);
            }
            throw PreviSatException(msg, WARNING);
        }

        // Aucun satellite dans le fichier
        if (nb == 0) {
            const QString msg = (alarme) ? QObject::tr("Le fichier %1 ne contient aucun satellite").arg(nomFichier) : QString();
            throw PreviSatException(msg, WARNING);
        }

    } catch (PreviSatException &e) {
        nb = 0;
        if (alarme) {
            throw PreviSatException();
        }
    }

    /* Retour */
    return nb;
}

/*
 * Lecture du fichier TLE
 */
QMap<QString, TLE> TLE::LectureFichier(const QString &dirLocalData, const QString &nomFichier, const QStringList &listeSatellites)
{
    /* Declarations des variables locales */
    static QString magn;
    QMap<QString, TLE> mapTLE;

    /* Initialisations */
    // Lecture du fichier de donnees
    if (magn.isEmpty()) {
        const QString fic = dirLocalData + QDir::separator() + "donnees.sat";
        QFile donneesSatellites(fic);

        if (donneesSatellites.exists() && (donneesSatellites.size() != 0)) {
            donneesSatellites.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&donneesSatellites);
            magn = flux.readAll();
        } else {
            magn = "";
        }
        donneesSatellites.close();
    }

    /* Corps de la methode */
    QFile fi(nomFichier);
    if (fi.exists() && (fi.size() != 0)) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);

        QTextStream flux(&fi);

        while (!flux.atEnd()) {

            const QString ligne = flux.readLine();
            QString lig0 = ligne;
            QString lig1;
            QString lig2;

            if (ligne.mid(0, 2) == "1 ") {

                // Cas des fichiers a 2 lignes : on recupere le nom du satellite a partir du fichier de donnees
                lig1 = ligne;

                const int indx1 = magn.indexOf('\n' + lig1.mid(2, 5)) + 1;
                if (indx1 > 0) {
                    const int indx2 = magn.indexOf('\n', indx1) - indx1;
                    lig0 = magn.mid(indx1 + 123, indx2 - 123).trimmed();
                } else {
                    lig0 = lig1.mid(2, 5);
                }
            } else {

                // Lecture ligne 1
                do {
                    lig1 = flux.readLine();
                } while (lig1.trimmed().length() == 0);
            }

            // Lecture ligne 2
            do {
                lig2 = flux.readLine();
            } while (lig2.trimmed().length() == 0);

            const QString nomsat = RecupereNomsat(lig0);

            // Sauvegarde du TLE
            if (listeSatellites.isEmpty() || listeSatellites.contains(lig1.mid(2, 5))) {

                TLE tle(lig0, lig1, lig2);
                tle._nom = nomsat.trimmed();

                if (!mapTLE.contains(tle._norad)) {

                    // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map de TLE)
                    const int indx1 = magn.indexOf('\n' + tle._norad) + 1;
                    if (indx1 > 0) {
                        const int indx2 = magn.indexOf('\n', indx1);
                        tle._donnees = Donnees(magn.mid(indx1, indx2));
                    }

                    mapTLE.insert(tle._norad, tle);
                }
            }
        }
    }
    fi.close();

    /* Retour */
    return mapTLE;
}

/*
 * Lecture du fichier 3le
 */
QList<TLE> TLE::LectureFichier3le(const QString &nomFichier3le)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QList<TLE> tabtle;

    /* Corps de la methode */
    try {

        QFile fichier(nomFichier3le);
        if (fichier.exists() && (fichier.size() != 0)) {

            fichier.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fichier);

            while (!flux.atEnd()) {

                const QString ligne = flux.readLine();
                if (!ligne.trimmed().isEmpty()) {

                    const QString ligne0 = ligne;

                    QString ligne1, ligne2;
                    while (ligne1.trimmed().isEmpty()) {
                        ligne1 = flux.readLine();
                    }

                    while (ligne2.trimmed().isEmpty()) {
                        ligne2 = flux.readLine();
                    }

                    const Date dateDeb(ligne0.mid(15).toDouble(), 0., false);

                    const TLE tle(ligne0.mid(0, 15).trimmed(), ligne1, ligne2, dateDeb);
                    tabtle.append(tle);
                }
            }
            fichier.close();
        }

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return tabtle;
}

/*
 * Lecture du fichier Human Space Flight
 */
void TLE::LectureTrajectoryData(const QString &fichierHsf, const QString &fichier3le, QStringList &tabManoeuvres)
{
    /* Declarations des variables locales */
    QStringList tabMan;
    QStringList tabMasse;
    QVector<TLE> tabtle;

    /* Initialisations */
    tabManoeuvres.clear();
    tabtle.clear();

    /* Corps de la methode */
    QFile fi(fichierHsf);
    if (fi.exists() && (fi.size() != 0)) {

        fi.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fi);

        Date debValid(-DATE_INFINIE, 0., false);

        QString dateFormatNasa;
        QString orb;
        while (!flux.atEnd()) {

            QString ligne = flux.readLine();

            // Lecture des manoeuvres
            if (ligne.contains("Maneuvers contained")) {

                while (!flux.readLine().contains("---")) {
                }

                while (!(ligne = flux.readLine()).contains("Coasting Arc")) {

                    if (!ligne.trimmed().isEmpty()) {

                        // Premiere ligne
                        const QStringList tab1 = ligne.split(" ", QString::SkipEmptyParts);
                        const QString date = tab1.at(0);
                        const QString dvx_m50 = tab1.at(1);
                        const QString dvx_lvlh = tab1.at(2);
                        const QString deltaV = tab1.last();

                        // Deuxieme ligne
                        ligne = flux.readLine();
                        const QStringList tab2 = ligne.split(" ", QString::SkipEmptyParts);
                        const QString dvy_m50 = tab2.at(1);
                        const QString dvy_lvlh = tab2.at(2);
                        const QString apogee = tab2.last();

                        // Troisieme ligne
                        ligne = flux.readLine();
                        const QStringList tab3 = ligne.split(" ", QString::SkipEmptyParts);
                        const QString dvz_m50 = tab3.at(1);
                        const QString dvz_lvlh = tab3.at(2);
                        const QString duree = QString("%1").arg((Date::ConversionDateNasa("2000/" + tab3.at(0)).jourJulienUTC() + 1.5) *
                                                                NB_SEC_PAR_JOUR, 0, 'f', 3);
                        const QString perigee = tab3.last();
                        tabMan.append(date + " " + apogee + " " + perigee + " " + deltaV + " " + duree + " " + dvx_m50 + " " + dvy_m50 + " " +
                                      dvz_m50 + " " + dvx_lvlh + " " + dvy_lvlh + " " + dvz_lvlh);
                    }
                }
            }

            // Nombre d'orbites NASA
            if (ligne.contains("Coasting Arc")) {
                orb = ligne.split(" ", QString::SkipEmptyParts).last().replace(")", "");
            }

            // Date du TLE
            if (ligne.contains("Vector Time (GMT)")) {
                dateFormatNasa = ligne.split(" ", QString::SkipEmptyParts).last();
                debValid = Date::ConversionDateNasa(dateFormatNasa);
            }

            // Masse
            if (ligne.contains("Weight")) {
                tabMasse.append(dateFormatNasa + " " + orb + " " + ligne.split(" ", QString::SkipEmptyParts).last());
            }

            // TLE
            if (ligne.trimmed() == "TWO LINE MEAN ELEMENT SET") {

                bool afin = false;
                while (!afin) {

                    const QString lig = flux.readLine().trimmed();

                    if (lig.size() > 0) {
                        afin = true;
                        const QString li0 = lig;
                        const QString li1 = flux.readLine().trimmed();
                        const QString li2 = flux.readLine().trimmed();

                        VerifieLignes(li1, li2, li0, false);
                        tabtle.append(TLE(li0, li1, li2, debValid));
                    }
                }
            }
        }
        fi.close();
    }

    // Tableau de manoeuvres
    int i = 0;
    Date dateArc1(-DATE_INFINIE, 0.);
    QString masse1;
    QStringListIterator it(tabMasse);
    while (it.hasNext() && (i < tabMan.size())) {

        const QString ligne = it.next();
        const QString dateFormatNasa = ligne.split(" ", QString::SkipEmptyParts).first();
        const QString orb = ligne.split(" ", QString::SkipEmptyParts).at(1);

        const Date dateArc2 = Date::ConversionDateNasa(dateFormatNasa);
        const QString masse2 = ligne.split(" ", QString::SkipEmptyParts).last();
        const Date dateMan = Date::ConversionDateNasa(dateFormatNasa.split("/", QString::SkipEmptyParts).first() + "/" +
                                                      tabMan.at(i).split(" ", QString::SkipEmptyParts).first());

        if ((dateMan.jourJulienUTC() >= dateArc1.jourJulienUTC()) && (dateMan.jourJulienUTC() < dateArc2.jourJulienUTC())) {
            QString fmt = "%1";
            if (masse1.isEmpty()) {
                masse1 = masse2;
            }
            tabManoeuvres.append(fmt.arg(dateMan.jourJulienUTC(), 0, 'f', 12) + " " + orb + " " + masse1 + " " + tabMan.at(i).mid(tabMan.at(i).indexOf(" ")+1));
            i++;
        }
        dateArc1 = dateArc2;
        masse1 = masse2;
    }

    if (!tabtle.isEmpty()) {

        QFile fichier(fichier3le);
        if (fichier.exists() && (fichier.size() != 0)) {

            fichier.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream flux(&fichier);

            const QString fmt = "%1";
            QVectorIterator<TLE> it2(tabtle);
            while (it2.hasNext()) {
                const TLE tle = it2.next();
                flux << tle._ligne0 << QString(15 - tle._ligne0.length(), QChar(' '))
                     << fmt.arg(tle._dateDebutValidite.jourJulienUTC(), 0, 'f', 12) << endl;
                flux << tle._ligne1 << endl;
                flux << tle._ligne2 << endl;
            }
            fichier.close();
        }
    }

    /* Retour */
    return;
}

/*
 * Mise a jour du fichier TLE
 */
void TLE::MiseAJourFichier(const QString &dirLocalData, const QString &ficOld, const QString &ficNew, const int affMsg, QStringList &compteRendu)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString nomFicOld = ficOld.mid(ficOld.lastIndexOf(QDir::separator())+1).trimmed();
    const QString nomFicNew = ficNew.mid(ficNew.lastIndexOf(QDir::separator())+1).trimmed();

    // Verification du fichier contenant les anciens TLE
    int nbOld = VerifieFichier(ficOld, false);
    if (nbOld == 0) {
        throw PreviSatException(QObject::tr("Erreur rencontrée lors du chargement du fichier\n" \
                                            "Le fichier %1 n'est pas un TLE").arg(nomFicOld), WARNING);
    }

    // Lecture du TLE
    QMap<QString, TLE> tleOld = LectureFichier(dirLocalData, ficOld);

    // Verification du fichier contenant les TLE recents
    const int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0) {
        throw PreviSatException(QObject::tr("Erreur rencontrée lors du chargement du fichier\n" \
                                            "Le fichier %1 n'est pas un TLE").arg(nomFicNew), WARNING);
    }

    // Lecture du TLE
    QMap<QString, TLE> tleNew = LectureFichier(dirLocalData, ficNew);

    /* Corps de la methode */
    // Mise a jour
    int j = 0;
    int nbMaj = 0;
    int nbAdd = 0;
    int nbSup = 0;
    int isat = 0;
    int res1 = (affMsg == 0) ? -1 : (affMsg == 1) ? QMessageBox::YesToAll : QMessageBox::NoToAll;
    int res2 = (affMsg == 0) ? -1 : (affMsg == 1) ? QMessageBox::YesToAll : QMessageBox::NoToAll;
    while (isat < nbOld || j < nbNew) {

        const QString sat = QString::number(isat);
        const QString noradInf = (nomFicOld == nomFicNew) ? "99999" : "";
        const QString norad1 = (isat < nbOld) ? tleOld[sat]._norad : noradInf;
        QString norad2;
        if (nomFicOld == nomFicNew) {
            norad2 = (j < nbNew) ? tleNew[QString::number(j)]._norad : "99999";
        } else {

            if ((j < nbNew) && !norad1.isEmpty()) {
                while ((j < nbNew) && ((norad2 = tleNew[QString::number(j)]._norad).compare(norad1) < 0)) {
                    j++;
                }
            } else {
                j = nbNew;
                norad2 = "0";
            }
        }
        const QString js = QString::number(j);

        if (norad1 == norad2) {

            if (tleOld[sat]._epoque.jourJulienUTC() < tleNew[js]._epoque.jourJulienUTC()) {
                tleOld[sat] = tleNew[js];
                tleOld[sat]._ligne0 = (tleNew[js]._nom == norad2) ? tleOld[sat]._ligne0 : tleNew[js]._ligne0;
                nbMaj++;
            } else {
                compteRendu.append(tleOld[sat]._nom + "#" + tleOld[sat]._norad);
            }
            j++;

        } else {
            if (nomFicOld == nomFicNew) {

                if (norad1.toInt() > norad2.toInt()) {

                    // TLE absent du fichier de TLE anciens
                    // Demande d'ajout
                    if ((res1 != QMessageBox::YesToAll) && (res1 != QMessageBox::NoToAll)) {
                        const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier " \
                                                            "à mettre à jour.\nVoulez-vous ajouter ce TLE dans le fichier à mettre à jour ?");

                        QMessageBox msgbox(QMessageBox::Question, QObject::tr("Ajout du nouveau TLE"),
                                           message.arg(tleNew[js]._nom).arg(tleNew[js]._norad), QMessageBox::Yes |
                                           QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll, 0);

                        msgbox.setDefaultButton(QMessageBox::No);
                        msgbox.setButtonText(QMessageBox::Yes, QObject::tr("Oui"));
                        msgbox.setButtonText(QMessageBox::YesToAll, QObject::tr("Oui à tout"));
                        msgbox.setButtonText(QMessageBox::No, QObject::tr("Non"));
                        msgbox.setButtonText(QMessageBox::NoToAll, QObject::tr("Non à tout"));
                        msgbox.exec();
                        res1 = msgbox.result();
                    }
                    if ((res1 == QMessageBox::Yes) || (res1 == QMessageBox::YesToAll)) {
                        tleOld.insert(sat, tleNew[js]);
                        nbOld++;
                        nbAdd++;
                    }
                    j++;
                } else {

                    // TLE absent du fichier de TLE recents
                    // Demande de suppression
                    if ((res2 != QMessageBox::YesToAll) && (res2 != QMessageBox::NoToAll)) {
                        const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier de TLE " \
                                                            "récents.\nVoulez-vous supprimer ce TLE du fichier à mettre à jour ?");

                        QMessageBox msgbox(QMessageBox::Question, QObject::tr("Suppression du TLE"),
                                           message.arg(tleNew[js]._nom).arg(tleNew[js]._norad), QMessageBox::Yes |
                                           QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll, 0);

                        msgbox.setDefaultButton(QMessageBox::No);
                        msgbox.setButtonText(QMessageBox::Yes, QObject::tr("Oui"));
                        msgbox.setButtonText(QMessageBox::YesToAll, QObject::tr("Oui à tout"));
                        msgbox.setButtonText(QMessageBox::No, QObject::tr("Non"));
                        msgbox.setButtonText(QMessageBox::NoToAll, QObject::tr("Non à tout"));
                        msgbox.exec();
                        res2 = msgbox.result();
                    }
                    if ((res2 == QMessageBox::Yes) || (res2 == QMessageBox::YesToAll)) {
                        tleOld.remove(sat);
                        isat--;
                        nbOld--;
                        nbSup++;
                    }
                }
            } else {
                if (!norad1.isEmpty()) {
                    compteRendu.append(tleOld[sat]._nom + "#" + tleOld[sat]._norad);
                }
            }
        }
        isat++;
    }

    compteRendu.append(nomFicOld);
    compteRendu.append(QString::number(nbMaj));
    compteRendu.append(QString::number(nbOld));
    compteRendu.append(QString::number(nbAdd));
    compteRendu.append(QString::number(nbSup));

    // Copie des TLE dans le fichier
    if ((nbMaj > 0) || (nbAdd > 0) || (nbSup > 0)) {

        QFile fichier(ficOld);
        fichier.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux(&fichier);

        QMapIterator<QString, TLE> it(tleOld);
        while (it.hasNext()) {
            it.next();
            const TLE tle = it.value();
            flux << tle._ligne0 << endl;
            flux << tle._ligne1 << endl;
            flux << tle._ligne2 << endl;
        }
        fichier.close();
    }
    tleOld.clear();
    tleNew.clear();

    /* Retour */
    return;
}

double TLE::ndt20() const
{
    return _ndt20;
}

double TLE::ndd60() const
{
    return _ndd60;
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
/*
 * Verification du checksum de la ligne
 */
bool TLE::CheckSum(const QString &ligne)
{
    /* Declarations des variables locales */
    int chr;

    /* Initialisations */
    int check = 0;

    /* Corps de la methode */
    for (int i=0; i<68; i++) {

        chr = ligne.at(i).digitValue();
        if ((chr >= 0) && (chr <= 9)) {
            check += chr;
        } else {
            if (ligne.at(i) == '-') {
                check++;
            }
        }
    }
    chr = ligne.at(68).digitValue();

    /* Retour */
    return ((check % 10) == chr);
}

/*
 * Recupere le nom du satellite
 */
QString TLE::RecupereNomsat(const QString &lig0)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString nomsat = lig0.trimmed();

    /* Corps de la methode */
    if ((nomsat.size() > 25) && (nomsat.mid(25).contains('.') > 0)) {
        nomsat = nomsat.mid(0, 15).trimmed();
    }

    if (nomsat.mid(0, 2) == "0 ") {
        nomsat = nomsat.mid(2);
    }

    if (nomsat.mid(0, 2) == "1 ") {
        nomsat = nomsat.mid(2, 5);
    }

    if (nomsat.toLower().trimmed() == "iss (zarya)") {
        nomsat = "ISS";
    }

    if ((nomsat.toLower().contains("iridium")) && (nomsat.contains("["))) {
        nomsat = nomsat.mid(0, nomsat.indexOf('[')).trimmed();
    }

    /* Retour */
    return nomsat;
}

/*
 * Verification des lignes du TLE
 */
void TLE::VerifieLignes(const QString &li1, const QString &li2, const QString &nomsat, const bool alarme)
{
    /* Declarations des variables locales */

    /* Initialisations */
    QString msg = "";

    /* Corps de la methode */
    // Verification si les lignes sont vides
    if (li1.isEmpty() || li2.isEmpty()) {
        if (alarme) {
            msg = QObject::tr("Une des lignes du TLE est vide");
        }
        throw PreviSatException(msg, WARNING);
    }

    // Verification du numero des lignes
    if ((li1.mid(0, 2) != "1 ") || (li2.mid(0, 2) != "2 ")) {
        if (alarme) {
            msg = QObject::tr("Les numéros de ligne du TLE du satellite %1 (numéro NORAD : %2 ) sont incorrects").arg(nomsat).arg(li2.mid(2, 5));
        }
        throw PreviSatException(msg, WARNING);
    }

    // Verification de la longueur des lignes
    if ((li1.size() != 69) || (li2.size() != 69)) {
        if (alarme) {
            msg = QObject::tr("La longueur des lignes du TLE du satellite %1 (numéro NORAD : %2) est incorrecte").arg(nomsat).arg(li2.mid(1, 6).trimmed());
        }
        throw PreviSatException(msg, WARNING);
    }

    // Verification des espaces dans les lignes
    if ((li1.at(1) != ' ') || (li1.at(8) != ' ') || (li1.at(17) != ' ') || (li1.at(32) != ' ') || (li1.at(43) != ' ') ||
            (li1.at(52) != ' ') || (li1.at(61) != ' ') || (li1.at(63) != ' ') || (li2.at(1) != ' ') || (li2.at(7) != ' ') ||
            (li2.at(16) != ' ') || (li2.at(25) != ' ') || (li2.at(33) != ' ') || (li2.at(42) != ' ') || (li2.at(51) != ' ')) {
        if (alarme) {
            msg = QObject::tr("Erreur position des espaces du TLE :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li2.mid(2, 5));
        }
        throw PreviSatException(msg, WARNING);
    }

    // Verification de la ponctuation des lignes
    if ((li1.at(23) != '.') || (li1.at(34) != '.') || (li2.at(11) != '.') || (li2.at(20) != '.') || (li2.at(37) != '.') || (li2.at(46) != '.') || (li2.at(54) != '.')) {
        if (alarme) {
            msg = QObject::tr("Erreur Ponctuation du TLE :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li2.mid(2, 5));
        }
        throw PreviSatException(msg, WARNING);
    }

    // Verification du numero NORAD
    if (li1.mid(2, 5) != li2.mid(2, 5)) {
        if (alarme) {
            msg = QObject::tr("Les deux lignes du TLE du satellite %1 ont des numéros NORAD différents (%2 et %3)").arg(nomsat).arg(li1.mid(2, 5)).arg(li2.mid(2, 5));
        }
        throw PreviSatException(msg, WARNING);
    }

    // Verification des checksums
    if (!CheckSum(li1)) {
        if (alarme) {
            msg = QObject::tr("Erreur CheckSum ligne 1 :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li1.mid(2, 5));
        }
        throw PreviSatException(msg, WARNING);
    }

    if (!CheckSum(li2)) {
        if (alarme) {
            msg = QObject::tr("Erreur CheckSum ligne 2 :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li1.mid(2, 5));
        }
        throw PreviSatException(msg, WARNING);
    }

    /* Retour */
    return;
}
