/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2016  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * Heritage
 * >
 *
 * Description
 * >    Utilitaires lies a la manipulation des TLE
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 juillet 2011
 *
 * Date de revision
 * >    12 septembre 2016
 *
 */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wpacked"
#include <cmath>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wpacked"
#include "tle.h"
#include "librairies/dates/dateConstants.h"
#include "librairies/exceptions/previsatexception.h"

/* Constructeurs */
TLE::TLE()
{
    _nbOrbites = 0;
    _argpo = 0.;
    _bstar = 0.;
    _ecco = 0.;
    _inclo = 0.;
    _mo = 0.;
    _no = 0.;
    _omegao = 0.;
}

TLE::TLE(const QString &lig0, const QString &lig1, const QString &lig2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    _ligne0 = lig0;
    _ligne1 = lig1;
    _ligne2 = lig2;

    // Numero NORAD
    _norad = _ligne1.mid(2, 5);

    // Epoque
    int an = _ligne1.mid(18, 2).toInt();
    const double jrs = _ligne1.mid(20, 12).toDouble();
    an += (an < 57) ? AN2000 : 1900;
    const Date date(an, 1, 1., 0.);
    _epoque = Date(date.jourJulien() + jrs - 1., 0., true);

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
    _nbOrbites = _ligne2.mid(63, 5).toInt();

    /* Retour */
    return;
}

/* Destructeur */
TLE::~TLE()
{
}

/* Methodes */
/*
 * Verification du fichier TLE
 */
int TLE::VerifieFichier(const QString &nomFichier, const bool alarm)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int nb = 0;
    QString nomsat = "---";
    QString li1 = "";
    QString li2 = "";

    /* Corps de la methode */
    try {

        QFile fichier(nomFichier);
        if (fichier.exists()) {

            fichier.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fichier);

            int itle = 0;
            while (!flux.atEnd()) {

                const QString ligne = flux.readLine().trimmed();

                if (ligne.size() > 0) {

                    if (nomsat == "---" || nomsat.isEmpty())
                        nomsat = ligne;

                    if (ligne.mid(0, 2) == "1 ") {
                        li1 = ligne;

                        do {
                            li2 = flux.readLine();
                        } while (li2.trimmed().length() == 0);

                        VerifieLignes(li1, li2);
                        if ((li1 == nomsat && itle == 3) || (li1 != nomsat && itle == 2))
                            throw PreviSatException(8);

                        itle = (li1 == nomsat) ? 2 : 3;
                        nb++;
                        nomsat = "---";
                    }
                }
            }
        }
        fichier.close();

        if (nb == 0 || nomsat != "---")
            throw PreviSatException(8);

    } catch (PreviSatException &e) {

        nb = 0;
        if (alarm) {

            QString msg = "";

            // Construction du message
            const int ierr = QString(e.what()).toInt();
            switch (ierr) {
            case 1:
                msg = QObject::tr("La longueur des lignes du TLE du satellite %1 (numéro NORAD : %2) est incorrecte");
                msg = msg.arg(nomsat).arg(li2.mid(1, 6).trimmed());
                break;

            case 2:
                msg = QObject::tr("Les numéros de ligne du TLE du satellite %1 (numéro NORAD : %2 ) sont incorrects");
                msg = msg.arg(nomsat).arg(li2.mid(2, 5));
                break;

            case 3:
                msg = QObject::tr("Erreur position des espaces du TLE :\nSatellite %1 - numéro NORAD : %2");
                msg = msg.arg(nomsat).arg(li2.mid(2, 5));
                break;

            case 4:
                msg = QObject::tr("Erreur Ponctuation du TLE :\nSatellite %1 - numéro NORAD : %2");
                msg = msg.arg(nomsat).arg(li2.mid(2, 5));
                break;

            case 5:
                msg = QObject::tr("Les deux lignes du TLE du satellite %1 ont des numéros NORAD différents (%2 et %3)");
                msg = msg.arg(nomsat).arg(li1.mid(2, 5)).arg(li2.mid(2, 5));
                break;

            case 6:
            case 7:
                msg = QObject::tr("Erreur CheckSum ligne %1 :\nSatellite %2 - numéro NORAD : %3");
                msg = msg.arg(ierr - 5).arg(nomsat).arg(li1.mid(2, 5));
                break;

            default:
                msg = QObject::tr("Le fichier %1 ne contient aucun satellite ou n'est pas valide");
                msg = msg.arg(nomFichier);
            }

            throw PreviSatException(msg, WARNING);
        }
    }

    /* Retour */
    return (nb);
}

/*
 * Lecture du fichier TLE
 */
void TLE::LectureFichier(const QString &nomFichier, const QStringList &listeSatellites, QVector<TLE> &tabtle)
{
    /* Declarations des variables locales */
    QString magn;

    /* Initialisations */
#if defined (Q_OS_MAC)
    const QString dirLocalData = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else

#if QT_VERSION >= 0x050000
    const QString dirAstr = QCoreApplication::organizationName() + QDir::separator() + QCoreApplication::applicationName();
    const QString dirLocalData =
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString(), QStandardPaths::LocateDirectory).at(0) +
            dirAstr + QDir::separator() + "data";
#else
    const QString dirLocalData = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif

#endif

    const int jmax = (listeSatellites.size() == 0) ? tabtle.size() : listeSatellites.size();
    const QString fic = dirLocalData + QDir::separator() + "donnees.sat";

    QFile donneesSatellites(fic);
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&donneesSatellites);
        magn = flux.readAll();
    } else {
        magn = "";
    }
    donneesSatellites.close();
    if (listeSatellites.size() == 0)
        tabtle.clear();
    else
        tabtle.resize(jmax);

    /* Corps de la methode */
    try {

        QString li0;

        int j = 0;
        QString nomsat = "---";
        QFile fichier(nomFichier);
        fichier.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichier);

        while (!flux.atEnd() || j > jmax) {

            const QString ligne = flux.readLine();
            if (ligne.mid(0, 2) == "1 ") {

                QString li1, li2;
                li1 = ligne;
                do {
                    li2 = flux.readLine();
                } while (li2.trimmed().length() == 0);

                if (nomsat.mid(0, 2) == "1 " || nomsat == "---" || nomsat.isEmpty()) {

                    const int indx1 = magn.indexOf('\n' + li1.mid(2, 5)) + 1;
                    if (indx1 > 0) {
                        const int indx2 = magn.indexOf('\n', indx1) - indx1;
                        nomsat = magn.mid(indx1 + 123, indx2 - 123).trimmed();
                    } else {
                        nomsat = li1.mid(2, 5);
                    }
                }

                if (nomsat.size() > 25 && nomsat.mid(25).contains('.') > 0)
                    nomsat = nomsat.mid(0, 15).trimmed();

                if (nomsat.mid(0, 2) == "0 ")
                    nomsat = nomsat.mid(2);

                if (nomsat.toLower() == "iss (zarya)")
                    nomsat = "ISS";

                if (nomsat.toLower().contains("iridium") && nomsat.contains("["))
                    nomsat = nomsat.mid(0, nomsat.indexOf('[')).trimmed();

                if (listeSatellites.size() == 0) {
                    TLE tle = TLE(li0, li1, li2);
                    tle._nom = nomsat.trimmed();
                    tabtle.append(tle);
                } else {

                    for (int i=0; i<listeSatellites.size(); i++) {
                        if (listeSatellites.at(i) == ligne.mid(2, 5)) {
                            TLE tle = TLE(li0, li1, li2);
                            tle._nom = nomsat.trimmed();
                            tabtle.replace(i, tle);
                            j++;
                            break;
                        }
                    }
                }
            }
            li0 = ligne.trimmed();
            nomsat = li0;
        }
        fichier.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void TLE::MiseAJourFichier(const QString &ficOld, const QString &ficNew, const int affMsg, QStringList &compteRendu)
{
    /* Declarations des variables locales */
    QVector<TLE> tleNew, tleOld;
    QStringList liste;

    /* Initialisations */
    const QString nomFicOld = ficOld.mid(ficOld.lastIndexOf(QDir::separator())+1).trimmed();
    const QString nomFicNew = ficNew.mid(ficNew.lastIndexOf(QDir::separator())+1).trimmed();

    // Verification du fichier contenant les anciens TLE
    int nbOld = VerifieFichier(ficOld, false);
    if (nbOld == 0)
        throw PreviSatException(QObject::tr("Erreur rencontrée lors du chargement du fichier\n" \
                                            "Le fichier %1 n'est pas un TLE").arg(nomFicOld), WARNING);

    // Lecture du TLE
    LectureFichier(ficOld, liste, tleOld);

    // Verification du fichier contenant les TLE recents
    const int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0)
        throw PreviSatException(QObject::tr("Erreur rencontrée lors du chargement du fichier\n" \
                                            "Le fichier %1 n'est pas un TLE").arg(nomFicNew), WARNING);

    // Lecture du TLE
    LectureFichier(ficNew, liste, tleNew);

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

        const QString norad1 = (isat < nbOld) ? tleOld.at(isat)._norad : (nomFicOld == nomFicNew) ? "99999" : "";
        QString norad2;
        if (nomFicOld == nomFicNew) {
            norad2 = (j < nbNew) ? tleNew.at(j)._norad : "99999";
        } else {

            if (j < nbNew && !norad1.isEmpty()) {
                while (j < nbNew && (norad2 = tleNew.at(j)._norad).compare(norad1))
                    j++;
            } else {
                j = nbNew;
                norad2 = "0";
            }
        }

        if (norad1 == norad2) {

            if (tleOld.at(isat)._epoque.jourJulienUTC() < tleNew.at(j)._epoque.jourJulienUTC()) {
                tleOld[isat] = tleNew[j];
                tleOld[isat]._ligne0 = (tleNew.at(j)._nom == norad2) ? tleOld.at(isat)._ligne0 : tleNew.at(j)._ligne0;
                nbMaj++;
            } else {
                compteRendu.append(tleOld[isat]._nom + "#" + tleOld[isat]._norad);
            }
            j++;

        } else {
            if (nomFicOld == nomFicNew) {

                if (norad1.toInt() > norad2.toInt()) {

                    // TLE absent du fichier de TLE anciens
                    // Demande d'ajout
                    if (res1 != QMessageBox::YesToAll && res1 != QMessageBox::NoToAll) {
                        const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier " \
                                                            "à mettre à jour.\nVoulez-vous ajouter ce TLE dans le fichier " \
                                                            "à mettre à jour ?");

                        QMessageBox msgbox(QMessageBox::Question, QObject::tr("Ajout du nouveau TLE"),
                                           message.arg(tleNew.at(j)._nom).arg(tleNew.at(j)._norad), QMessageBox::Yes |
                                           QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll, 0);

                        msgbox.setDefaultButton(QMessageBox::No);
                        msgbox.setButtonText(QMessageBox::Yes, QObject::tr("Oui"));
                        msgbox.setButtonText(QMessageBox::YesToAll, QObject::tr("Oui à tout"));
                        msgbox.setButtonText(QMessageBox::No, QObject::tr("Non"));
                        msgbox.setButtonText(QMessageBox::NoToAll, QObject::tr("Non à tout"));
                        msgbox.exec();
                        res1 = msgbox.result();
                    }
                    if (res1 == QMessageBox::Yes || res1 == QMessageBox::YesToAll) {
                        tleOld.insert(isat, tleNew.at(j));
                        nbOld++;
                        nbAdd++;
                    }
                    j++;
                } else {

                    // TLE absent du fichier de TLE recents
                    // Demande de suppression
                    if (res2 != QMessageBox::YesToAll && res2 != QMessageBox::NoToAll) {
                        const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier de TLE " \
                                                            "récents.\nVoulez-vous supprimer ce TLE du fichier à mettre à jour ?");

                        QMessageBox msgbox(QMessageBox::Question, QObject::tr("Suppression du TLE"),
                                           message.arg(tleNew.at(j)._nom).arg(tleNew.at(j)._norad), QMessageBox::Yes |
                                           QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll, 0);

                        msgbox.setDefaultButton(QMessageBox::No);
                        msgbox.setButtonText(QMessageBox::Yes, QObject::tr("Oui"));
                        msgbox.setButtonText(QMessageBox::YesToAll, QObject::tr("Oui à tout"));
                        msgbox.setButtonText(QMessageBox::No, QObject::tr("Non"));
                        msgbox.setButtonText(QMessageBox::NoToAll, QObject::tr("Non à tout"));
                        msgbox.exec();
                        res2 = msgbox.result();
                    }
                    if (res2 == QMessageBox::Yes || res2 == QMessageBox::YesToAll) {
                        tleOld.remove(isat);
                        isat--;
                        nbOld--;
                        nbSup++;
                    }
                }
            } else {
                if (!norad1.isEmpty())
                    compteRendu.append(tleOld[isat]._nom + "#" + tleOld[isat]._norad);
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
    if (nbMaj > 0 || nbAdd > 0 || nbSup > 0) {

        QFile fichier(ficOld);
        fichier.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream flux(&fichier);

        QVectorIterator<TLE> it(tleOld);
        while (it.hasNext()) {
            const TLE tle = it.next();
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
        if (chr >= 0 && chr <= 9) {
            check += chr;
        } else {
            if (ligne.at(i) == '-')
                check++;
        }
    }
    chr = ligne.at(68).digitValue();

    /* Retour */
    return (check % 10 == chr);
}

/*
 * Verification des lignes du TLE
 */
void TLE::VerifieLignes(const QString &li1, const QString &li2)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int exc = 0;
    int ierr = 0;

    /* Corps de la methode */
    // Verification de la longueur des lignes
    if (li1.size() != 69 || li2.size() != 69) {
        exc = 1;
        ierr++;
    }

    // Verification du numero des lignes
    if (li1.mid(0, 2) != "1 " || li2.mid(0, 2) != "2 ") {
        exc = 2;
        ierr++;
    }

    if (ierr == 1)
        throw PreviSatException(exc);
    else if (ierr > 1)
        throw PreviSatException(8);
    else
    {}

    // Verification des espaces dans les lignes
    if (li1.at(1) != ' ' || li1.at(8) != ' ' || li1.at(17) != ' ' || li1.at(32) != ' ' || li1.at(43) != ' ' ||
            li1.at(52) != ' ' || li1.at(61) != ' ' || li1.at(63) != ' ' || li2.at(1) != ' ' || li2.at(7) != ' ' ||
            li2.at(16) != ' ' || li2.at(25) != ' ' || li2.at(33) != ' ' || li2.at(42) != ' ' || li2.at(51) != ' ')
        throw PreviSatException(3);

    // Verification de la ponctuation des lignes
    if (li1.at(23) != '.' || li1.at(34) != '.' || li2.at(11) != '.' || li2.at(20) != '.' || li2.at(37) != '.' ||
            li2.at(46) != '.' || li2.at(54) != '.')
        throw PreviSatException(4);

    // Verification du numero NORAD
    if (li1.mid(2, 5) != li2.mid(2, 5))
        throw PreviSatException(5);

    // Verification des checksums
    if (!CheckSum(li1))
        throw PreviSatException(6);

    if (!CheckSum(li2))
        throw PreviSatException(7);

    /* Retour */
    return;
}

/* Accesseurs */
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

int TLE::nbOrbites() const
{
    return _nbOrbites;
}

double TLE::no() const
{
    return _no;
}

QString TLE::nom() const
{
    return _nom;
}

QString TLE::norad() const
{
    return _norad;
}

double TLE::omegao() const
{
    return _omegao;
}
