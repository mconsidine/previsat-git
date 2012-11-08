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
 * >    11 octobre 2012
 *
 */

#include <cmath>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
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

/*
 * Constructeur a partir des 2 lignes du TLE
 */
TLE::TLE(const QString &ligne1, const QString &ligne2)
{
    /* Declarations des variables locales */
    int an, ibe;
    double jrs;

    /* Initialisations */

    /* Corps du constructeur */
    _ligne1 = ligne1;
    _ligne2 = ligne2;

    // Numero NORAD
    _norad = _ligne1.mid(2, 5);

    // Epoque
    an = _ligne1.mid(18, 2).toInt();
    jrs = _ligne1.mid(20, 12).toDouble();
    an = (an < 57) ? an + AN2000 : an + 1900;
    const Date date = Date(an, 1, 1., 0.);
    _epoque = Date(date.getJourJulien() + jrs - 1., 0., true);

    // Coefficient pseudo-balistique
    _bstar = _ligne1.mid(53, 6).toDouble();
    ibe = _ligne1.mid(59, 2).toInt();
    _bstar *= 1.e-5 * pow(10., ibe);

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

    // Inclinaison
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
    int ierr, itle, nb;
    QString li1, li2, nomsat;

    /* Initialisations */
    ierr = 0;
    itle = 0;
    nb = 0;
    nomsat = "---";
    li1 = "";
    li2 = "";

    /* Corps de la methode */
    try {

        QFile fichier(nomFichier);
        if (fichier.exists()) {

            fichier.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream flux(&fichier);

            while (!flux.atEnd()) {

                const QString ligne = flux.readLine();
                if (nomsat == "---" || nomsat.isEmpty())
                    nomsat = ligne.trimmed();

                if (ligne.size() > 0) {
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

        QString msg = "";
        nb = 0;

        // Construction du message
        ierr = QString(e.what()).toInt();
        switch (ierr) {
        case 1:
            msg = QObject::tr("POSITION : La longueur des lignes du TLE du satellite %1 (numéro NORAD : %2) est incorrecte");
            msg = msg.arg(nomsat).arg(li2.mid(1, 6).trimmed());
            break;

        case 2:
            msg = QObject::tr("POSITION : Les numéros de ligne du TLE du satellite %1 (numéro NORAD : %2 ) sont incorrects");
            msg = msg.arg(nomsat).arg(li2.mid(2, 5));
            break;

        case 3:
            msg = QObject::tr("POSITION : Erreur position des espaces du TLE :\nSatellite %1 - numéro NORAD : %2");
            msg = msg.arg(nomsat).arg(li2.mid(2, 5));
            break;

        case 4:
            msg = QObject::tr("POSITION : Erreur Ponctuation du TLE :\nSatellite %1 - numéro NORAD : %2");
            msg = msg.arg(nomsat).arg(li2.mid(2, 5));
            break;

        case 5:
            msg = QObject::tr("POSITION : Les deux lignes du TLE du satellite %1 ont des numéros NORAD différents (%2 et %3)");
            msg = msg.arg(nomsat).arg(li1.mid(2, 5)).arg(li2.mid(2, 5));
            break;

        case 6:
        case 7:
            msg = QObject::tr("POSITION : Erreur CheckSum ligne %1 :\nSatellite %2 - numéro NORAD : %3");
            msg = msg.arg(ierr - 5).arg(nomsat).arg(li1.mid(2, 5));
            break;

        default:
            msg = QObject::tr("POSITION : Le fichier %1 ne contient aucun satellite ou n'est pas valide");
            msg = msg.arg(nomFichier);
        }

        if (alarm)
            throw PreviSatException(msg, WARNING);
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
    QString dirDat;

    /* Initialisations */
#if defined (Q_OS_WIN)
    dirDat = QCoreApplication::applicationDirPath() + QDir::separator() + "data";
#else
    dirDat = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + "data";
#endif
    const int jmax = (listeSatellites.size() == 0) ? tabtle.size() : listeSatellites.size();
    const QString fic = dirDat + QDir::separator() + "donnees.sat";
    QFile donneesSatellites(fic.toStdString().c_str());
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&donneesSatellites);
        magn = flux.readAll();
    }
    donneesSatellites.close();
    tabtle.resize(jmax);

    /* Corps de la methode */
    try {

        int j;
        QString nomsat;

        j = 0;
        nomsat = "---";
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

                if (nomsat.mid(0, 2) == "1 " || nomsat == "---") {
                    const int indx1 = magn.indexOf(li1.mid(2, 5));
                    if (indx1 >= 0) {
                        const int indx2 = magn.indexOf('\n', indx1) - indx1;
                        nomsat = magn.mid(indx1 + 36, indx2 - 36).trimmed();
                    } else {
                        nomsat = li1.mid(2, 5);
                    }
                }

                if (nomsat.size() > 25 && nomsat.mid(25).contains('.') > 0)
                    nomsat = nomsat.mid(0, 15).trimmed();

                if (nomsat.toLower() == "iss (zarya)")
                    nomsat = "ISS";
                if (nomsat.toLower().contains("iridium") && nomsat.contains("["))
                    nomsat = nomsat.mid(0, nomsat.indexOf('[')).trimmed();

                if (listeSatellites.size() == 0) {
                    TLE tle = TLE(li1, li2);
                    tle._nom = nomsat.trimmed();
                    tabtle.append(tle);
                } else {
                    for (int i=0; i<listeSatellites.size(); i++) {
                        if (listeSatellites.at(i) == ligne.mid(2, 5)) {
                            TLE tle = TLE(li1, li2);
                            tle._nom = nomsat.trimmed();
                            tabtle.replace(i, tle);
                            j++;
                            break;
                        }
                    }
                }
            }
            nomsat = ligne.trimmed();
        }
        fichier.close();

    } catch (PreviSatException &e) {
    }

    /* Retour */
    return;
}

void TLE::MiseAJourFichier(const QString &ficOld, const QString &ficNew, const bool affMsg, QStringList &compteRendu)
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
        throw PreviSatException(QObject::tr("MISE A JOUR : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE").arg(ficOld), WARNING);

    // Lecture du TLE
    LectureFichier(ficOld, liste, tleOld);

    // Verification du fichier contenant les TLE recents
    int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0)
        throw PreviSatException(QObject::tr("MISE A JOUR : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE").arg(ficNew), WARNING);

    // Lecture du TLE
    LectureFichier(ficNew, liste, tleNew);

    /* Corps de la methode */
    // Mise a jour
    int j = 0;
    int nbMaj = 0;
    int nbAdd = 0;
    int nbSup = 0;
    int isat = 0;
    int res1 = (affMsg) ? -1 : QMessageBox::YesToAll;
    int res2 = (affMsg) ? -1 : QMessageBox::YesToAll;
    while (isat < nbOld || j < nbNew) {

        QString norad1 = (isat < nbOld) ? tleOld.at(isat)._norad : (nomFicOld == nomFicNew) ? "99999" : "";
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
            if (tleOld.at(isat)._epoque.getJourJulienUTC() < tleNew.at(j)._epoque.getJourJulienUTC()) {
                const QString nomsat =
                        (tleNew.at(j)._nom == norad2) ? tleOld.at(isat)._nom : tleNew.at(j)._nom;
                tleOld[isat] = tleNew[j];
                tleOld[isat]._nom = nomsat;
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
                    if (res1 != QMessageBox::YesToAll && res1 != QMessageBox::NoToAll && affMsg) {
                        const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier à mettre à jour.\nVoulez-vous ajouter ce TLE dans le fichier à mettre à jour ?");
                        res1 = QMessageBox::question(0, QObject::tr("Ajout du nouveau TLE"),
                                                     message.arg(tleNew.at(j)._nom).arg(tleNew.at(j)._norad),
                                                     QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No |
                                                     QMessageBox::NoToAll, QMessageBox::No);
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
                    if (res2 != QMessageBox::YesToAll && res2 != QMessageBox::NoToAll && affMsg) {
                        const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier de TLE récents.\nVoulez-vous supprimer ce TLE du fichier à mettre à jour ?");
                        res2 = QMessageBox::question(0, QObject::tr("Suppression du TLE"),
                                                     message.arg(tleOld.at(isat)._nom).arg(tleOld.at(isat)._norad),
                                                     QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No |
                                                     QMessageBox::NoToAll, QMessageBox::No);
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
            flux << tle._nom << endl;
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
bool TLE::CheckSum(const QString ligne)
{
    /* Declarations des variables locales */
    int check, chr;

    /* Initialisations */
    check = 0;

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
    return ((check % 10 == chr) ? true : false);
}

/*
 * Verification des lignes du TLE
 */
void TLE::VerifieLignes(const QString &li1, const QString &li2)
{
    /* Declarations des variables locales */
    int exc, ierr;

    /* Initialisations */
    exc = 0;
    ierr = 0;

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
double TLE::getArgpo() const
{
    return _argpo;
}

double TLE::getBstar() const
{
    return _bstar;
}

double TLE::getEcco() const
{
    return _ecco;
}

Date TLE::getEpoque() const
{
    return _epoque;
}

double TLE::getInclo() const
{
    return _inclo;
}

QString TLE::getLigne1() const
{
    return _ligne1;
}

QString TLE::getLigne2() const
{
    return _ligne2;
}

double TLE::getMo() const
{
    return _mo;
}

int TLE::getNbOrbites() const
{
    return _nbOrbites;
}

double TLE::getNo() const
{
    return _no;
}

QString TLE::getNom() const
{
    return _nom;
}

QString TLE::getNorad() const
{
    return _norad;
}

double TLE::getOmegao() const
{
    return _omegao;
}
