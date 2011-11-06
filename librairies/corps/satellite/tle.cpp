/*
 *     PreviSat, position of artificial satellites, prediction of their passes, Iridium flares
 *     Copyright (C) 2005-2011  Astropedia web: http://astropedia.free.fr  -  mailto: astropedia@free.fr
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
 * >
 *
 */

#include <cmath>
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
}

/*
 * Constructeur a partir des 2 lignes du TLE
 */
TLE::TLE(const QString ligne1, const QString ligne2)
{
    /* Declarations des variables locales */
    int an, ibe;
    double jrs;

    /* Initialisations */

    /* Corps du constructeur */
    _ligne1 = ligne1;
    _ligne2 = ligne2;
    char li1[69], li2[69];
    strncpy(li1, ligne1.toStdString().c_str(), ligne1.size());
    strncpy(li2, ligne2.toStdString().c_str(), ligne2.size());

    // Numero NORAD
    _norad = _ligne1.mid(2, 5);

    // Lecture de la premiere ligne
    sscanf(li1, "%*2d%*5d%*c%*10s%2d%12lf%*11f%*7f%*2d%7lf%2d", &an, &jrs, &_bstar, &ibe);

    an = (an < 57) ? an + AN2000 : an + 1900;
    const Date date = Date(an, 1, 1., 0.);
    _epoque = Date(date.getJourJulien() + jrs - 1., 0., true);
    _bstar *= 1.e-5 * pow(10., ibe);

    // Lecture de la seconde ligne
    sscanf(li2, "%*2d%*5d%9lf%9lf%8lf%9lf%9lf%11lf%5d", &_inclo, &_omegao, &_ecco, &_argpo, &_mo, &_no,
           &_nbOrbites);
    _ecco *= 1.e-7;

    /* Retour */
    return;
}

/* Destructeur */
TLE::~TLE()
{
}

/* Methodes */
/*
 * Verication du fichier TLE
 */
int TLE::VerifieFichier(const QString nomFichier, const bool alarm)
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

            fichier.open(QIODevice::ReadOnly);
            QTextStream flux(&fichier);

            while (!flux.atEnd()) {

                const QString ligne = flux.readLine();
                if (nomsat == "---" || nomsat == "")
                    nomsat = ligne.trimmed();

                if (ligne.size() > 0) {
                    if (ligne.at(0) == '1') {
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

    } catch (PreviSatException e) {

        QString msg = "";
        nb = 0;

        // Construction du message
        ierr = (int) e.what();
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
            PreviSatException(msg, Messages::WARNING);
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
    const int jmax = (listeSatellites.size() == 0) ? tabtle.size() : listeSatellites.size();

    QFile donneesSatellites("data/donnees.sat");
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly);
        QTextStream flux(&donneesSatellites);
        magn = flux.readAll();
    }
    donneesSatellites.close();
    tabtle.resize(jmax);

    /* Corps de la methode */
    try {

        int j, k;
        QString nomsat;

        j = 0;
        k = 0;
        nomsat = "---";
        QFile fichier(nomFichier);
        fichier.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichier);

        while (!flux.atEnd() || j > jmax) {

            const QString ligne = flux.readLine();
            if (ligne.at(0) == '1') {

                QString li1, li2;
                li1 = ligne;
                do {
                    li2 = flux.readLine();
                } while (li2.trimmed().length() == 0);

                if (nomsat.at(0) == '1' || nomsat == "---") {
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

    } catch (PreviSatException e) {
    }

    /* Retour */
    return;
}

void TLE::MiseAJourFichier(const QString ficOld, const QString ficNew, QStringList &compteRendu)
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
        throw PreviSatException(QObject::tr("MISE A JOUR : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE").arg(ficOld), Messages::WARNING);

    // Lecture du TLE
    LectureFichier(ficOld, liste, tleOld);

    // Verification du fichier contenant les TLE recents
    const int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0)
        throw PreviSatException(QObject::tr("MISE A JOUR : Erreur rencontrée lors du chargement du fichier\nLe fichier %1 n'est pas un TLE").arg(ficNew), Messages::WARNING);

    // Lecture du TLE
    LectureFichier(ficNew, liste, tleNew);

    /* Corps de la methode */
    // Mise a jour
    int j = 0;
    int nbMaj = 0;
    int nbAdd = 0;
    int nbSup = 0;
    int isat = 0;
    while (isat < nbOld || j < nbNew) {

        bool amaj = false;
        if (isat >= nbOld)
            isat = nbOld - 1;
        QString norad1 = tleOld.at(isat)._norad;
        QString norad2;
        if (nomFicOld == nomFicNew) {
            norad2 = tleNew.at(j)._norad;
            if (j == isat && norad1 > norad2)
                amaj = true;

        } else {
            while ((norad2 = tleNew.at(j)._norad).compare(norad1))
                j++;
        }

        if (norad1 == norad2) {
            if (tleOld.at(isat)._epoque.getJourJulienUTC() < tleNew.at(isat)._epoque.getJourJulienUTC()) {
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
                if (norad1.toInt() < norad2.toInt() && j > isat || amaj) {

                    // TLE absent du fichier de TLE anciens
                    // Ajout ?
                    QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier à mettre à jour.\nVoulez-vous ajouter ce TLE dans le fichier à mettre à jour ?");
                    message = message.arg(tleNew.at(j)._nom).arg(tleNew.at(j)._norad);
                    const int res = QMessageBox::question(0, QObject::tr("Ajout du nouveau TLE"), message,
                                                          QMessageBox::Yes, QMessageBox::No);

                    if (res == QMessageBox::Yes) {
                        if (isat > 0)
                            isat++;
                        tleOld.insert(isat, tleNew.at(j));
                        nbOld++;
                        nbAdd++;
                    }
                    j++;
                } else {

                    // TLE absent du fichier de TLE recents
                    // Suppression ?
                    QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier de TLE récents.\nVoulez-vous supprimer ce TLE du fichier à mettre à jour ?");
                    message = message.arg(tleOld.at(isat)._nom).arg(tleOld.at(isat)._norad);
                    const int res = QMessageBox::question(0, QObject::tr("Suppression du TLE"), message,
                                                          QMessageBox::Yes, QMessageBox::No);

                    if (res == QMessageBox::Yes) {
                        tleOld.remove(isat);
                        isat--;
                        nbOld--;
                        nbSup++;
                    }
                }
            } else {
                compteRendu.append(tleOld[isat]._nom + "#" + tleOld[isat]._norad);
            }
        }
        isat++;
    }

    compteRendu.append(QString(nbMaj));
    compteRendu.append(QString(nbOld));
    compteRendu.append(QString(nbAdd));
    compteRendu.append(QString(nbSup));

    // Copie des TLE dans le fichier
    if (nbMaj > 0 || nbAdd > 0 || nbSup > 0) {

        QFile fichier(ficOld);
        fichier.open(QIODevice::WriteOnly);
        QTextStream flux(&fichier);

        QVectorIterator<TLE> it(tleOld);
        int i = 0;
        while (it.hasNext()) {
            flux << tleOld.at(i)._nom << endl;
            flux << tleOld.at(i)._ligne1 << endl;
            flux << tleOld.at(i)._ligne2 << endl;
            i++;
        }
        fichier.close();
    }

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
void TLE::VerifieLignes(const QString li1, const QString li2)
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
    if (li1.at(0) != '1' || li2.at(0) != '2') {
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
    if (li1.at(1) != ' ' || li1.at(8) != ' ' || li1.at(17) != ' ' || li1.at(32) != ' ' ||
            li1.at(43) != ' ' || li1.at(52) != ' ' || li1.at(61) != ' ' || li1.at(63) != ' ' ||
            li2.at(1) != ' ' || li2.at(7) != ' ' || li2.at(16) != ' ' || li2.at(25) != ' ' ||
            li2.at(33) != ' ' || li2.at(42) != ' ' || li2.at(51) != ' ')
        throw PreviSatException(3);

    // Verification de la ponctuation des lignes
    if (li1.at(23) != '.' || li1.at(34) != '.' || li2.at(11) != '.' || li2.at(20) != '.' ||
            li2.at(37) != '.' || li2.at(46) != '.' || li2.at(54) != '.')
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
