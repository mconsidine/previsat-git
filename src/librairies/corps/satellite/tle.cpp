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
 * >    25 mai 2022
 *
 */

#include <QDir>
#pragma GCC diagnostic ignored "-Wconversion"
#include <QMessageBox>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"
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
    _ndd60 = 0.;
    _ndt20 = 0.;
    _no = 0.;
    _omegao = 0.;

    /* Retour */
    return;
}

/*
 * Definition a partir des composantes du TLE
 */
TLE::TLE(const QString &lig0, const QString &lig1, const QString &lig2) :
    _ligne0(lig0), _ligne1(lig1), _ligne2(lig2)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps du constructeur */
    // Numero NORAD
    _norad = _ligne1.mid(2, 5);

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

double TLE::ndt20() const
{
    return _ndt20;
}

double TLE::ndd60() const
{
    return _ndd60;
}

Donnees TLE::donnees() const
{
    return _donnees;
}


/*
 * Methodes publiques
 */
/*
 * Lecture du fichier TLE
 */
QMap<QString, TLE> TLE::LectureFichier(const QString &nomFichier, const QString &donneesSat, const int lgRec, const QStringList &listeSatellites,
                                       const bool ajoutDonnees)
{
    /* Declarations des variables locales */
    QMap<QString, TLE> mapTLE;

    /* Initialisations */

    /* Corps de la methode */
    QFile fi(nomFichier);
    if (fi.exists() && (fi.size() != 0)) {

        QString lig0;
        QString lig1;
        QString lig2;
        QString numeroNorad;
        TLE tle;

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QTextStream flux(&fi);
            const QString contenuFichier = flux.readAll();

            QStringListIterator it(contenuFichier.split("\n", Qt::SkipEmptyParts));
            while (it.hasNext()) {

                const QString ligne = it.next();

                if (ligne.startsWith("1 ")) {

                    // Cas des TLE a 2 lignes
                    lig1 = ligne;
                    lig2 = it.next();

                    // Recuperation du nom du satellite dans le fichier de donnees
                    numeroNorad = lig1.mid(2, 5);
                    lig0 = numeroNorad;

                    if (ajoutDonnees) {

                        const int idx = lgRec * numeroNorad.toInt();
                        if ((idx >= 0) && (idx < donneesSat.size())) {

                            const QString donnee = donneesSat.mid(idx, lgRec);
                            tle._donnees = Donnees(donnee);
                            lig0 = donnee.mid(125).trimmed();
                        }
                    }
                } else {

                    // Cas des TLE a 3 lignes
                    lig0 = ligne;
                    lig1 = it.next();
                    lig2 = it.next();
                }

                const QString nomsat = RecupereNomsat(lig0);

                // Sauvegarde du TLE
                if (listeSatellites.isEmpty() || listeSatellites.contains(lig1.mid(2, 5))) {

                    tle = TLE(lig0, lig1, lig2);
                    tle._nom = nomsat.trimmed();

                    if (!mapTLE.contains(tle._norad)) {

                        if (ajoutDonnees) {

                            // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map de TLE)
                            const int idx = lgRec * tle._norad.toInt();
                            if ((idx >= 0) && (idx < donneesSat.size())) {
                                tle._donnees = Donnees(donneesSat.mid(idx, lgRec));
                            }
                        }

                        mapTLE.insert(tle._norad, tle);
                    }
                }
            }
        }
        fi.close();
    }

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

            if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {

                QTextStream flux(&fichier);
                const QString contenuFichier = flux.readAll();

                QStringListIterator it(contenuFichier.split("\n", Qt::SkipEmptyParts));
                while (it.hasNext()) {

                    const QString lig0 = it.next();
                    const QString lig1 = it.next();
                    const QString lig2 = it.next();

                    const TLE tle(lig0, lig1, lig2);
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
 * Mise a jour du fichier TLE
 */
void TLE::MiseAJourFichier(const QString &ficOld, const QString &ficNew, const QString &donneesSat, const int lgRec,  const int affMsg,
                           QStringList &compteRendu)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int res1 = (affMsg == 0) ? -1 : (affMsg == 1) ? QMessageBox::YesToAll : QMessageBox::NoToAll;
    int res2 = (affMsg == 0) ? -1 : (affMsg == 1) ? QMessageBox::YesToAll : QMessageBox::NoToAll;

    const QString nomFicOld = ficOld.mid(ficOld.lastIndexOf(QDir::separator())+1).trimmed();
    const QString nomFicNew = ficNew.mid(ficNew.lastIndexOf(QDir::separator())+1).trimmed();

    // Verification du fichier contenant les anciens TLE
    const int nbOld = VerifieFichier(ficOld, false);
    if (nbOld == 0) {
        throw PreviSatException(QObject::tr("Erreur rencontrée lors du chargement du fichier\n" \
                                            "Le fichier %1 n'est pas un TLE").arg(nomFicOld), WARNING);
    }

    // Lecture du TLE
    QMap<QString, TLE> tleOld = LectureFichier(ficOld, donneesSat, lgRec, QStringList(), false);

    // Verification du fichier contenant les TLE recents
    const int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0) {
        throw PreviSatException(QObject::tr("Erreur rencontrée lors du chargement du fichier\n" \
                                            "Le fichier %1 n'est pas un TLE").arg(nomFicNew), WARNING);
    }

    // Lecture du TLE
    QMap<QString, TLE> tleNew = LectureFichier(ficNew, donneesSat, lgRec, QStringList(), false);

    /* Corps de la methode */
    int nbMaj = 0;

    QStringList listeNoradOld = tleOld.keys();
    QStringList listeNoradNew = tleNew.keys();

    // Mise a jour des TLE communs aux 2 fichiers
    QStringListIterator it1(tleNew.keys());
    while (it1.hasNext()) {

        const QString numeroNorad = it1.next();
        if (listeNoradOld.contains(numeroNorad)) {

            // Mise a jour du TLE
            if (tleOld[numeroNorad]._epoque.jourJulienUTC() < tleNew[numeroNorad]._epoque.jourJulienUTC()) {

                tleOld[numeroNorad] = tleNew[numeroNorad];
                tleOld[numeroNorad]._ligne0 = (tleNew[numeroNorad]._nom == numeroNorad) ? tleOld[numeroNorad]._ligne0 : tleNew[numeroNorad]._ligne0;
                nbMaj++;

            } else {
                compteRendu.append(tleOld[numeroNorad]._nom + "#" + tleOld[numeroNorad]._norad);
            }

            listeNoradNew.removeOne(numeroNorad);
            listeNoradOld.removeOne(numeroNorad);

        } else {
            if (nomFicOld != nomFicNew) {
                compteRendu.append(tleOld[numeroNorad]._nom + "#" + tleOld[numeroNorad]._norad);
            }
        }
    }

    int nbAdd = 0;
    int nbSup = 0;

    if (nomFicOld == nomFicNew) {

        // Suppression des TLE non mis a jour
        QStringListIterator it2(listeNoradOld);
        while (it2.hasNext()) {

            const QString numeroNorad = it2.next();

            // TLE absent du fichier de TLE recents
            // Demande de suppression
            if ((res2 != QMessageBox::YesToAll) && (res2 != QMessageBox::NoToAll)) {
                const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier de TLE " \
                                                    "récents.\nVoulez-vous supprimer ce TLE du fichier à mettre à jour ?");

                QMessageBox msgbox(QMessageBox::Question, QObject::tr("Suppression du TLE"),
                                   message.arg(tleOld[numeroNorad]._nom).arg(tleOld[numeroNorad]._norad), QMessageBox::Yes |
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
                tleOld.remove(numeroNorad);
                nbSup++;
            }
        }

        // Ajout des nouveaux TLE
        QStringListIterator it3(listeNoradNew);
        while (it3.hasNext()) {

            const QString numeroNorad = it3.next();

            // TLE absent du fichier de TLE anciens
            // Demande d'ajout
            if ((res1 != QMessageBox::YesToAll) && (res1 != QMessageBox::NoToAll)) {
                const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier " \
                                                    "à mettre à jour.\nVoulez-vous ajouter ce TLE dans le fichier à mettre à jour ?");

                QMessageBox msgbox(QMessageBox::Question, QObject::tr("Ajout du nouveau TLE"),
                                   message.arg(tleNew[numeroNorad]._nom).arg(tleNew[numeroNorad]._norad), QMessageBox::Yes |
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
                tleOld.insert(numeroNorad, tleNew[numeroNorad]);
                nbAdd++;
            }
        }
    }

    compteRendu.append(nomFicOld);
    compteRendu.append(QString::number(nbMaj));
    compteRendu.append(QString::number(tleOld.keys().size()));
    compteRendu.append(QString::number(nbAdd));
    compteRendu.append(QString::number(nbSup));

    // Copie des TLE dans le fichier
    if ((nbMaj > 0) || (nbAdd > 0) || (nbSup > 0)) {

        QFile fichier(ficOld);
        if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {

            QTextStream flux(&fichier);

            QMapIterator<QString, TLE> it(tleOld);
            while (it.hasNext()) {
                it.next();

                const TLE tle = it.value();
                flux << tle._ligne0 << endl;
                flux << tle._ligne1 << endl;
                flux << tle._ligne2 << endl;
            }
        }
        fichier.close();
    }

    tleOld.clear();
    tleNew.clear();

    /* Retour */
    return;
}

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

            if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

                int itle = 0;
                QString lig0;
                QString lig1;
                QString lig2;
                QString msg;

                QTextStream flux(&fi);
                const QString contenuFichier = flux.readAll();

                QStringListIterator it(contenuFichier.split("\n", Qt::SkipEmptyParts));
                while (it.hasNext()) {

                    const QString ligne = it.next();

                    if (ligne.startsWith("1 ")) {

                        // Cas des TLE a 2 lignes
                        lig1 = ligne;
                        if (it.hasNext()) {
                            lig2 = it.next();
                            lig0 = lig1;
                        }

                    } else {

                        // Cas des TLE a 3 lignes
                        lig0 = ligne;
                        if (it.hasNext()) {
                            lig1 = it.next();
                        }

                        if (it.hasNext()) {
                            lig2 = it.next();
                        }
                    }

                    const QString nomsat = RecupereNomsat(lig0);

                    if (((lig1 == lig0) && (itle == 3))
                            || ((lig1 != lig0) && (itle== 2))
                            || (!lig1.startsWith("1 ") && !lig2.startsWith("2 "))) {

                        msg = "";
                        if (alarme) {
                            msg = QObject::tr("Le fichier %1 n'est pas valide").arg(nomFichier);
                        }
                        throw PreviSatException(msg, WARNING);
                    }
                    VerifieLignes(lig1, lig2, nomsat, alarme);

                    itle = (lig1 == lig0) ? 2 : 3;
                    nb++;
                }
            }
            fi.close();

        } else {

            // Le fichier n'existe pas
            const QString msg = (alarme) ? QObject::tr("Le fichier %1 n'existe pas").arg(nomFichier) : "";
            throw PreviSatException(msg, WARNING);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            const QString msg = (alarme) ? QObject::tr("Le fichier %1 est vide").arg(nomFichier) : "";
            throw PreviSatException(msg, WARNING);
        }

        // Aucun satellite dans le fichier
        if (nb == 0) {
            const QString msg = (alarme) ? QObject::tr("Le fichier %1 ne contient aucun satellite").arg(nomFichier) : "";
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

    /* Retour */
    return ((check % 10) == ligne.at(68).digitValue());
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

    if (nomsat.startsWith("0 ")) {
        nomsat = nomsat.mid(2);
    }

    if (nomsat.startsWith("1 ")) {
        nomsat = nomsat.mid(2, 5);
    }

    if (nomsat.toLower().trimmed() == "iss (zarya)") {
        nomsat = "ISS";
    }

    if ((nomsat.contains("iridium", Qt::CaseInsensitive)) && (nomsat.contains("["))) {
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

    /* Corps de la methode */
    // Verification si les lignes sont vides
    if (li1.isEmpty() || li2.isEmpty()) {
        const QString msg = (alarme) ? QObject::tr("Une des lignes du TLE est vide") : "";
        throw PreviSatException(msg, WARNING);
    }

    // Verification du numero des lignes
    if (!li1.startsWith("1 ") || !li2.startsWith("2 ")) {
        const QString msg = (alarme) ?
                    QObject::tr("Les numéros de ligne du TLE du satellite %1 (numéro NORAD : %2 ) sont incorrects").arg(nomsat).arg(li2.mid(2, 5)) : "";
        throw PreviSatException(msg, WARNING);
    }

    // Verification de la longueur des lignes
    if ((li1.size() != 69) || (li2.size() != 69)) {
        const QString msg = (alarme) ? QObject::tr("La longueur des lignes du TLE du satellite %1 (numéro NORAD : %2) est incorrecte").arg(nomsat)
                                       .arg(li2.mid(1, 6).trimmed()) : "";
        throw PreviSatException(msg, WARNING);
    }

    // Verification des espaces dans les lignes
    if ((li1.at(1) != ' ') || (li1.at(8) != ' ') || (li1.at(17) != ' ') || (li1.at(32) != ' ') || (li1.at(43) != ' ') ||
            (li1.at(52) != ' ') || (li1.at(61) != ' ') || (li1.at(63) != ' ') || (li2.at(1) != ' ') || (li2.at(7) != ' ') ||
            (li2.at(16) != ' ') || (li2.at(25) != ' ') || (li2.at(33) != ' ') || (li2.at(42) != ' ') || (li2.at(51) != ' ')) {
        const QString msg = (alarme) ?
                    QObject::tr("Erreur position des espaces du TLE :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li2.mid(2, 5)) : "";
        throw PreviSatException(msg, WARNING);
    }

    // Verification de la ponctuation des lignes
    if ((li1.at(23) != '.') || (li1.at(34) != '.') || (li2.at(11) != '.') || (li2.at(20) != '.') || (li2.at(37) != '.') || (li2.at(46) != '.') ||
            (li2.at(54) != '.')) {
        const QString msg = (alarme) ? QObject::tr("Erreur Ponctuation du TLE :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li2.mid(2, 5)) : "";
        throw PreviSatException(msg, WARNING);
    }

    // Verification du numero NORAD
    if (li1.mid(2, 5) != li2.mid(2, 5)) {
        const QString msg = (alarme) ? QObject::tr("Les deux lignes du TLE du satellite %1 ont des numéros NORAD différents (%2 et %3)").arg(nomsat)
                                       .arg(li1.mid(2, 5)).arg(li2.mid(2, 5)) : "";
        throw PreviSatException(msg, WARNING);
    }

    // Verification des checksums
    if (!CheckSum(li1)) {
        const QString msg = (alarme) ? QObject::tr("Erreur CheckSum ligne 1 :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li1.mid(2, 5)) : "";
        throw PreviSatException(msg, WARNING);
    }

    if (!CheckSum(li2)) {
        const QString msg = (alarme) ? QObject::tr("Erreur CheckSum ligne 2 :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li1.mid(2, 5)) : "";
        throw PreviSatException(msg, WARNING);
    }

    /* Retour */
    return;
}
