/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2025  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    12 aout 2024
 *
 */

#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include "librairies/exceptions/exception.h"
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
TLE::TLE(const QString &lig0,
         const QString &lig1,
         const QString &lig2) :
    _ligne0(lig0),
    _ligne1(lig1),
    _ligne2(lig2)
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
QMap<QString, ElementsOrbitaux> TLE::Lecture(const QString &nomFichier,
                                             const QSqlDatabase &db,
                                             const QStringList &listeSatellites,
                                             const bool ajoutDonnees)
{
    /* Declarations des variables locales */
    QMap<QString, ElementsOrbitaux> mapElem;

    /* Initialisations */
    _mapTLE.clear();

    /* Corps de la methode */
    QFile fi(nomFichier);
    if (!fi.exists() || (fi.size() == 0)) {

        const QFileInfo ff(fi.fileName());
#if (!BUILD_TEST)
        qWarning() << QString("Le fichier %1 n'existe pas ou est vide").arg(ff.fileName());
#endif
        throw Exception(QObject::tr("Le fichier %1 n'existe pas ou est vide").arg(ff.fileName()), MessageType::WARNING);
    }

    if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QFileInfo ff(fi.fileName());
#if (!BUILD_TEST)
        qWarning() << QString("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName());
#endif
        throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName()), MessageType::ERREUR);
    }

    QString lig0;
    QString lig1;
    QString lig2;
    QString numeroNorad;
    TLE tle;

    const QStringList contenu = QString(fi.readAll()).split("\n", Qt::SkipEmptyParts);

    QStringListIterator it(contenu);
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

            if (ajoutDonnees && db.isValid()) {

                const QList<Donnees> listeDonnees = Donnees::RequeteNorad(db, tle._elements.norad);

                if (!listeDonnees.isEmpty()) {
                    tle._elements.donnees = Donnees::RequeteNorad(db, tle._elements.norad).first();
                    lig0 = tle._elements.donnees.nom();
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
        if ((listeSatellites.isEmpty() || listeSatellites.contains(lig1.mid(2, 5))) && !lig0.isEmpty() && !lig1.isEmpty() && !lig2.isEmpty()) {

            tle = TLE(lig0, lig1, lig2);
            tle._elements.nom = nomsat.trimmed();

            if (ajoutDonnees && db.isValid()) {

                // Donnees relatives au satellite (pour des raisons pratiques elles sont stockees dans la map d'elements orbitaux)
                const QList<Donnees> listeDonnees = Donnees::RequeteNorad(db, tle._elements.norad);

                if (!listeDonnees.isEmpty()) {

                    // Recuperation des donnees satellite
                    tle._elements.donnees = Donnees::RequeteNorad(db, tle._elements.norad).first();

                    // Correction eventuelle du nombre d'orbites a l'epoque
                    tle._elements.nbOrbitesEpoque = GPFormat::CalculNombreOrbitesEpoque(tle._elements);
                }
            }

            if (!mapElem.contains(tle._elements.norad)) {

                mapElem.insert(tle._elements.norad, tle._elements);
                _mapTLE.insert(tle._elements.norad, tle);
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
QStringList TLE::MiseAJourFichier(const QString &ficOld,
                                  const QString &ficNew,
                                  const QSqlDatabase &donneesSat,
                                  const int affMsg)
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
        throw Exception(QObject::tr("Le fichier %1 n'est pas un TLE").arg(nomFicOld), MessageType::WARNING);
    }

    // Lecture du TLE
    QMap<QString, ElementsOrbitaux> tleOld = Lecture(ficOld, donneesSat, QStringList(), false);
    QMap<QString, TLE> mapTleOld = _mapTLE;

    // Verification du fichier contenant les TLE recents
    const int nbNew = VerifieFichier(ficNew, false);
    if (nbNew == 0) {
        throw Exception(QObject::tr("Le fichier %1 n'est pas un TLE").arg(nomFicNew), MessageType::WARNING);
    }

    // Lecture du TLE
    QMap<QString, ElementsOrbitaux> tleNew = Lecture(ficNew, donneesSat, QStringList(), false);
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
#if (!COVERAGE_TEST)
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
#if (!COVERAGE_TEST)
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
int TLE::VerifieFichier(const QString &nomFichier,
                        const bool alarme)
{
    /* Declarations des variables locales */

    /* Initialisations */
    int nb = 0;

    /* Corps de la methode */
    QFile fi(nomFichier);
    if (!fi.exists() || (fi.size() == 0)) {

        const QFileInfo ff(fi.fileName());
#if (!BUILD_TEST)
        qWarning() << QString("Le fichier %1 n'existe pas ou est vide").arg(ff.fileName());
#endif
        if (alarme) {
            throw Exception(QObject::tr("Le fichier %1 n'existe pas ou est vide").arg(ff.fileName()), MessageType::WARNING);
        }
    }

    if (!fi.open(QIODevice::ReadOnly | QIODevice::Text)) {
#if (!COVERAGE_TEST)
        const QFileInfo ff(fi.fileName());
#if (!BUILD_TEST)
        qWarning() << QString("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName());
#endif
        if (alarme) {
            throw Exception(QObject::tr("Erreur lors de l'ouverture du fichier %1").arg(ff.fileName()), MessageType::ERREUR);
        }
#endif
    }

    try {

        int itle = 0;
        QString lig0;
        QString lig1;
        QString lig2;

        const QStringList contenu = QString(fi.readAll()).split("\n", Qt::SkipEmptyParts);
        fi.close();

        QStringListIterator it(contenu);
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

                throw Exception((alarme) ? QObject::tr("Le fichier %1 n'est pas valide").arg(nomFichier) : "", MessageType::WARNING);
            }

            VerifieLignes(lig1, lig2, nomsat, alarme);

            itle = (lig1 == lig0) ? 2 : 3;
            nb++;
        }

    } catch (Exception const &e) {
        nb = 0;
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
bool TLE::CheckSum(const QString &ligne, int &checksum)
{
    /* Declarations des variables locales */

    /* Initialisations */
    checksum = 0;

    /* Corps de la methode */
    for (int i=0; i<68; i++) {

        const int chr = ligne.at(i).digitValue();
        if ((chr >= 0) && (chr <= 9)) {
            checksum += chr;
        } else if (ligne.at(i) == '-') {
            checksum++;
        }
    }

    checksum %= 10;

    /* Retour */
    return (checksum == ligne.at(68).digitValue());
}

/*
 * Verification des lignes du TLE
 */
void TLE::VerifieLignes(const QString &li1,
                        const QString &li2,
                        const QString &nomsat,
                        const bool alarme)
{
    /* Declarations des variables locales */
    int checksum1;
    int checksum2;

    /* Initialisations */

    /* Corps de la methode */
    // Verification si les lignes sont vides
    if (li1.isEmpty() || li2.isEmpty()) {
        throw Exception((alarme) ? QObject::tr("Une des lignes du TLE est vide") : "", MessageType::WARNING);
    }

    // Verification du numero des lignes
    if (!li1.startsWith("1 ") || !li2.startsWith("2 ")) {
        const QString msg = QObject::tr("Les numéros de ligne du TLE du satellite %1 (numéro NORAD : %2 ) sont incorrects").arg(nomsat).
                            arg(li2.mid(2, 5));
        throw Exception((alarme) ? msg : "", MessageType::WARNING);
    }

    // Verification de la longueur des lignes
    if ((li1.size() != 69) || (li2.size() != 69)) {
        const QString msg = QObject::tr("La longueur des lignes du TLE du satellite %1 (numéro NORAD : %2) est incorrecte").
                            arg(nomsat).arg(li2.mid(1, 6).trimmed());
        throw Exception((alarme) ? msg : "", MessageType::WARNING);
    }

    // Verification des espaces dans les lignes
    if ((li1.at(1) != ' ') || (li1.at(8) != ' ') || (li1.at(17) != ' ') || (li1.at(32) != ' ') || (li1.at(43) != ' ') ||
            (li1.at(52) != ' ') || (li1.at(61) != ' ') || (li1.at(63) != ' ') || (li2.at(1) != ' ') || (li2.at(7) != ' ') ||
            (li2.at(16) != ' ') || (li2.at(25) != ' ') || (li2.at(33) != ' ') || (li2.at(42) != ' ') || (li2.at(51) != ' ')) {

        const QString msg = QObject::tr("Erreur position des espaces du TLE :\nSatellite %1 - numéro NORAD : %2").arg(nomsat).arg(li2.mid(2, 5));
        throw Exception((alarme) ? msg : "", MessageType::WARNING);
    }

    // Verification de la ponctuation des lignes
    if ((li1.at(23) != '.') || (li1.at(34) != '.') || (li2.at(11) != '.') || (li2.at(20) != '.') || (li2.at(37) != '.') || (li2.at(46) != '.') ||
            (li2.at(54) != '.')) {

        throw Exception((alarme) ? QObject::tr("Erreur Ponctuation du TLE :\nSatellite %1 - numéro NORAD : %2")
                                           .arg(nomsat).arg(li2.mid(2, 5)) : "", MessageType::WARNING);
    }

    // Verification du numero NORAD
    if (li1.mid(2, 5) != li2.mid(2, 5)) {

        throw Exception((alarme) ? QObject::tr("Les deux lignes du TLE du satellite %1 ont des numéros NORAD différents (%2 et %3)")
                                           .arg(nomsat).arg(li1.mid(2, 5)).arg(li2.mid(2, 5)) : "", MessageType::WARNING);
    }

    // Verification des checksums
    if (!CheckSum(li1, checksum1)) {
        throw Exception((alarme) ? QObject::tr("Erreur CheckSum ligne 1 :\nSatellite %1 - numéro NORAD : %2\nValeur attendue : %3 - Valeur calculée : %4")
                                       .arg(nomsat).arg(li1.mid(2, 5)).arg(li1.at(68)).arg(checksum1) : "", MessageType::WARNING);
    }

    if (!CheckSum(li2, checksum2)) {
        throw Exception((alarme) ? QObject::tr("Erreur CheckSum ligne 2 :\nSatellite %1 - numéro NORAD : %2\nValeur attendue : %3 - Valeur calculée : %4")
                                           .arg(nomsat).arg(li1.mid(2, 5)).arg(li2.at(68)).arg(checksum2) : "", MessageType::WARNING);
    }

    /* Retour */
    return;
}
