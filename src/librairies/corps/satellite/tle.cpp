/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2023  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    20 mai 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "librairies/exceptions/previsatexception.h"
#include "gpformat.h"
#include "tle.h"


QMap<QString, TLE> TLE::_mapTLE;


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
    _elements.epoque = Date();

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
    _elements.norad = _ligne1.mid(2, 5);

    // Designation COSPAR
    int annee = _ligne1.mid(9, 2).toInt();
    annee += (annee < 57) ? DATE::AN2000 : 1900;
    _elements.cospar = QString("%1-%2").arg(annee).arg(_ligne1.mid(11, 6).trimmed());

    // Epoque
    annee = _ligne1.mid(18, 2).toInt();
    const double jrs = _ligne1.mid(20, 12).toDouble();
    annee += (annee < 57) ? DATE::AN2000 : 1900;
    const Date date(annee, 1, 1., 0.);
    _elements.epoque = Date(date.jourJulienUTC() + jrs - 1., 0., true);

    // Derivees du moyen mouvement
    _elements.ndt20 = (_ligne1.mid(33, 1).trimmed() + "0" + _ligne1.mid(34, 9)).toDouble();
    _elements.ndd60 = 1.e-5 * (_ligne1.mid(44, 6) + "e" + _ligne1.mid(50, 2)).toDouble();

    // Coefficient pseudo-balistique
    _elements.bstar = 1.e-5 * (_ligne1.mid(53, 6) + "e" + _ligne1.mid(59, 2)).toDouble();

    // Elements orbitaux moyens
    // Inclinaison
    _elements.inclo = _ligne2.mid(8, 8).toDouble();

    // Ascension droite du noeud ascendant
    _elements.omegao = _ligne2.mid(17, 8).toDouble();

    // Excentricite
    _elements.ecco = 1.e-7 * _ligne2.mid(26, 7).toDouble();

    // Argument du perigee
    _elements.argpo = _ligne2.mid(34, 8).toDouble();

    // Anomalie moyenne
    _elements.mo = _ligne2.mid(43, 8).toDouble();

    // Moyen mouvement
    _elements.no = _ligne2.mid(52, 11).toDouble();

    // Nombre d'orbites a l'epoque
    _elements.nbOrbitesEpoque = _ligne2.mid(63, 5).toUInt();

    /* Retour */
    return;
}


/*
 * Methodes publiques
 */
/*
 * Lecture du fichier TLE
 */
QMap<QString, ElementsOrbitaux> TLE::LectureFichier(const QString &nomFichier, const QString &donneesSat, const int lgRec,
                                                    const QStringList &listeSatellites, const bool ajoutDonnees)
{
    /* Declarations des variables locales */
    QMap<QString, ElementsOrbitaux> mapElem;

    /* Initialisations */
    _mapTLE.clear();

    /* Corps de la methode */
    QFile fi(nomFichier);
    if (!fi.exists() || (fi.size() == 0)) {

        const QFileInfo ff(fi.fileName());
#if (BUILD_TEST == false)
        throw PreviSatException(QObject::tr("Le fichier %1 n'existe pas ou est vide").arg(ff.fileName()), MessageType::WARNING);
#endif
    }

    if (fi.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QString lig0;
        QString lig1;
        QString lig2;
        QString numeroNorad;
        TLE tle;

        const QString contenuFichier = fi.readAll();

        QStringListIterator it(contenuFichier.split("\n", Qt::SkipEmptyParts));
        while (it.hasNext()) {

            const QString ligne = it.next();

            if (ligne.startsWith("1 ")) {

                // Cas des TLE a 2 lignes
                lig1 = ligne;

                if (it.hasNext()) {
                    lig2 = it.next();
                }

                // Recuperation du nom du satellite dans le fichier de donnees
                numeroNorad = lig1.mid(2, 5);
                lig0 = numeroNorad;

                if (ajoutDonnees) {

                    const int idx = lgRec * numeroNorad.toInt();
                    if ((idx >= 0) && (idx < donneesSat.size())) {

                        const QString donnee = donneesSat.mid(idx, lgRec);
                        tle._elements.donnees = Donnees(donnee);
                        lig0 = donnee.mid(125).trimmed();
                    }
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

            const QString nomsat = GPFormat::RecupereNomsat(lig0);

            // Sauvegarde du TLE
            if (listeSatellites.isEmpty() || listeSatellites.contains(lig1.mid(2, 5))) {

                if (!lig0.isEmpty() && !lig1.isEmpty() && !lig2.isEmpty()) {

                    tle = TLE(lig0, lig1, lig2);
                    tle._elements.nom = nomsat.trimmed();

                    if (!mapElem.contains(tle._elements.norad)) {

                        if (ajoutDonnees) {

                            // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map d'elements orbitaux)
                            const int idx = lgRec * tle._elements.norad.toInt();
                            if ((idx >= 0) && (idx < donneesSat.size())) {

                                tle._elements.donnees = Donnees(donneesSat.mid(idx, lgRec));
                                tle._elements.nbOrbitesEpoque = GPFormat::CalculNombreOrbitesEpoque(tle._elements);
                            }
                        }

                        mapElem.insert(tle._elements.norad, tle._elements);
                        _mapTLE.insert(tle._elements.norad, tle);
                    }
                }
            }
        }
    }
    fi.close();

    /* Retour */
    return mapElem;
}

/*
 * Mise a jour du fichier TLE
 */
QStringList TLE::MiseAJourFichier(const QString &ficOld, const QString &ficNew, const QString &donneesSat, const int lgRec,  const int affMsg)
{
    /* Declarations des variables locales */
    QStringList compteRendu;

    /* Initialisations */
    int res1 = (affMsg == 0) ? -1 : (affMsg == 1) ? QMessageBox::YesToAll : QMessageBox::NoToAll;
    int res2 = (affMsg == 0) ? -1 : (affMsg == 1) ? QMessageBox::YesToAll : QMessageBox::NoToAll;

    const QString nomFicOld = ficOld.mid(ficOld.lastIndexOf(QDir::separator())+1).trimmed();
    const QString nomFicNew = ficNew.mid(ficNew.lastIndexOf(QDir::separator())+1).trimmed();

    // Verification du fichier contenant les anciens TLE
    const int nbOld = VerifieFichier(ficOld, false);
    if (nbOld == 0) {
        throw PreviSatException(QObject::tr("Le fichier %1 n'est pas un TLE").arg(nomFicOld), MessageType::WARNING);
    }

    // Lecture du TLE
    QMap<QString, ElementsOrbitaux> tleOld = LectureFichier(ficOld, donneesSat, lgRec, QStringList(), false);
    QMap<QString, TLE> mapTleOld = _mapTLE;

    // Verification du fichier contenant les TLE recents
    const int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0) {
        throw PreviSatException(QObject::tr("Le fichier %1 n'est pas un TLE").arg(nomFicNew), MessageType::WARNING);
    }

    // Lecture du TLE
    QMap<QString, ElementsOrbitaux> tleNew = LectureFichier(ficNew, donneesSat, lgRec, QStringList(), false);
    QMap<QString, TLE> mapTleNew = _mapTLE;

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
            if (tleOld[numeroNorad].epoque.jourJulienUTC() < tleNew[numeroNorad].epoque.jourJulienUTC()) {

                tleOld[numeroNorad] = tleNew[numeroNorad];
                mapTleOld[numeroNorad]._ligne0 = (tleNew[numeroNorad].nom == numeroNorad) ?
                            mapTleOld[numeroNorad]._ligne0 : mapTleNew[numeroNorad]._ligne0;
                nbMaj++;

            } else {
                compteRendu.append(tleOld[numeroNorad].nom + "#" + tleOld[numeroNorad].norad);
            }

            listeNoradNew.removeOne(numeroNorad);
            listeNoradOld.removeOne(numeroNorad);

        } else if (nomFicOld != nomFicNew) {
            compteRendu.append(tleOld[numeroNorad].nom + "#" + tleOld[numeroNorad].norad);
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
#if (COVERAGE_TEST == false)
            // Demande de suppression
            if ((res2 != QMessageBox::YesToAll) && (res2 != QMessageBox::NoToAll)) {
                const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier de TLE récents.\n" \
                                                    "Voulez-vous supprimer ce TLE du fichier à mettre à jour ?");

                QMessageBox msgbox(QMessageBox::Question, QObject::tr("Suppression du TLE"),
                                   message.arg(tleOld[numeroNorad].nom).arg(tleOld[numeroNorad].norad));

                msgbox.addButton(QObject::tr("Oui"), QMessageBox::YesRole);
                msgbox.addButton(QObject::tr("Oui à tout"), QMessageBox::AcceptRole);
                QPushButton * const non = msgbox.addButton(QObject::tr("Non"), QMessageBox::NoRole);
                msgbox.addButton(QObject::tr("Non à tout"), QMessageBox::ActionRole);
                msgbox.setDefaultButton(non);

                msgbox.exec();
                res2 = msgbox.result();
            }
#endif

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
#if (COVERAGE_TEST == false)
            // Demande d'ajout
            if ((res1 != QMessageBox::YesToAll) && (res1 != QMessageBox::NoToAll)) {
                const QString message = QObject::tr("Le satellite %1 (numéro NORAD : %2) n'existe pas dans le fichier " \
                                                    "à mettre à jour.\nVoulez-vous ajouter ce TLE dans le fichier à mettre à jour ?");

                QMessageBox msgbox(QMessageBox::Question, QObject::tr("Ajout du nouveau TLE"),
                                   message.arg(tleNew[numeroNorad].nom).arg(tleNew[numeroNorad].norad));

                msgbox.addButton(QObject::tr("Oui"), QMessageBox::YesRole);
                msgbox.addButton(QObject::tr("Oui à tout"), QMessageBox::AcceptRole);
                QPushButton * const non = msgbox.addButton(QObject::tr("Non"), QMessageBox::NoRole);
                msgbox.addButton(QObject::tr("Non à tout"), QMessageBox::AcceptRole);
                msgbox.setDefaultButton(non);

                msgbox.exec();
                res1 = msgbox.result();
            }
#endif

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

            QMapIterator it(mapTleOld);
            while (it.hasNext()) {
                it.next();

                const TLE tle = it.value();
                flux << tle._ligne0 << Qt::endl;
                flux << tle._ligne1 << Qt::endl;
                flux << tle._ligne2 << Qt::endl;
            }
        }
        fichier.close();
    }

    tleOld.clear();
    tleNew.clear();

    /* Retour */
    return compteRendu;
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

                const QString contenuFichier = fi.readAll();

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

                    const QString nomsat = GPFormat::RecupereNomsat(lig0);

                    if (((lig1 == lig0) && (itle == 3))
                            || ((lig1 != lig0) && (itle== 2))
                            || (!lig1.startsWith("1 ") && !lig2.startsWith("2 "))) {

                        throw PreviSatException((alarme) ? QObject::tr("Le fichier %1 n'est pas valide").arg(nomFichier) : "", MessageType::WARNING);
                    }

                    VerifieLignes(lig1, lig2, nomsat, alarme);

                    itle = (lig1 == lig0) ? 2 : 3;
                    nb++;
                }
            }
            fi.close();

        } else {

            // Le fichier n'existe pas
            throw PreviSatException((alarme) ? QObject::tr("Le fichier %1 n'existe pas").arg(nomFichier) : "", MessageType::WARNING);
        }

        // Le fichier est vide
        if (fi.size() == 0) {
            throw PreviSatException((alarme) ? QObject::tr("Le fichier %1 est vide").arg(nomFichier) : "", MessageType::WARNING);
        }

        // Aucun satellite dans le fichier
        if (nb == 0) {
            throw PreviSatException(
                        (alarme) ? QObject::tr("Le fichier %1 ne contient aucun satellite").arg(nomFichier) : "", MessageType::WARNING);
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
 * Accesseurs
 */
const ElementsOrbitaux &TLE::elements() const
{
    return _elements;
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

    /* Initialisations */
    int check = 0;

    /* Corps de la methode */
    for (int i=0; i<68; i++) {

        const int chr = ligne.at(i).digitValue();
        if ((chr >= 0) && (chr <= 9)) {
            check += chr;
        } else if (ligne.at(i) == '-') {
            check++;
        }
    }

    /* Retour */
    return ((check % 10) == ligne.at(68).digitValue());
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
        throw PreviSatException((alarme) ? QObject::tr("Une des lignes du TLE est vide") : "", MessageType::WARNING);
    }

    // Verification du numero des lignes
    if (!li1.startsWith("1 ") || !li2.startsWith("2 ")) {
        throw PreviSatException(
                    (alarme) ?
                        QObject::tr("Les numéros de ligne du TLE du satellite %1 (numéro NORAD : %2 ) sont incorrects")
                        .arg(nomsat).arg(li2.mid(2, 5)) : "", MessageType::WARNING);
    }

    // Verification de la longueur des lignes
    if ((li1.size() != 69) || (li2.size() != 69)) {
        throw PreviSatException((alarme) ? QObject::tr("La longueur des lignes du TLE du satellite %1 (numéro NORAD : %2) est incorrecte")
                                           .arg(nomsat).arg(li2.mid(1, 6).trimmed()) : "", MessageType::WARNING);
    }

    // Verification des espaces dans les lignes
    if ((li1.at(1) != ' ') || (li1.at(8) != ' ') || (li1.at(17) != ' ') || (li1.at(32) != ' ') || (li1.at(43) != ' ') ||
            (li1.at(52) != ' ') || (li1.at(61) != ' ') || (li1.at(63) != ' ') || (li2.at(1) != ' ') || (li2.at(7) != ' ') ||
            (li2.at(16) != ' ') || (li2.at(25) != ' ') || (li2.at(33) != ' ') || (li2.at(42) != ' ') || (li2.at(51) != ' ')) {

        throw PreviSatException((alarme) ?
                                    QObject::tr("Erreur position des espaces du TLE :\nSatellite %1 - numéro NORAD : %2")
                                    .arg(nomsat).arg(li2.mid(2, 5)) : "", MessageType::WARNING);
    }

    // Verification de la ponctuation des lignes
    if ((li1.at(23) != '.') || (li1.at(34) != '.') || (li2.at(11) != '.') || (li2.at(20) != '.') || (li2.at(37) != '.') || (li2.at(46) != '.') ||
            (li2.at(54) != '.')) {

        throw PreviSatException((alarme) ? QObject::tr("Erreur Ponctuation du TLE :\nSatellite %1 - numéro NORAD : %2")
                                           .arg(nomsat).arg(li2.mid(2, 5)) : "", MessageType::WARNING);
    }

    // Verification du numero NORAD
    if (li1.mid(2, 5) != li2.mid(2, 5)) {

        throw PreviSatException((alarme) ? QObject::tr("Les deux lignes du TLE du satellite %1 ont des numéros NORAD différents (%2 et %3)")
                                           .arg(nomsat).arg(li1.mid(2, 5)).arg(li2.mid(2, 5)) : "", MessageType::WARNING);
    }

    // Verification des checksums
    if (!CheckSum(li1)) {
        throw PreviSatException((alarme) ? QObject::tr("Erreur CheckSum ligne 1 :\nSatellite %1 - numéro NORAD : %2")
                                           .arg(nomsat).arg(li1.mid(2, 5)) : "", MessageType::WARNING);
    }

    if (!CheckSum(li2)) {
        throw PreviSatException((alarme) ? QObject::tr("Erreur CheckSum ligne 2 :\nSatellite %1 - numéro NORAD : %2")
                                           .arg(nomsat).arg(li1.mid(2, 5)) : "", MessageType::WARNING);
    }

    /* Retour */
    return;
}
