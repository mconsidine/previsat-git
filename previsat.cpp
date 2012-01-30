#include <QDateTime>
#include <QDir>
#include <QGraphicsTextItem>
#include <QPalette>
#include <QSettings>
#include <QSound>
#include <QTextStream>
#include "librairies/corps/satellite/satellite.h"
#include "librairies/corps/satellite/tle.h"
#include "librairies/corps/systemesolaire/lune.h"
#include "librairies/dates/date.h"
#include "librairies/exceptions/messages.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/observateur/observateur.h"
#include "previsat.h"
#include "ui_previsat.h"

// Repertoires
static QString dirExe;
static QString dirDat;
static QString dirCoo;
static QString dirMap;
static QString dirTle;

// TLE par defaut
static QString nom;
static QString l1;
static QString l2;

// Liste de satellites
static bool info;
static bool notif;
static int nbSat;
static QString nomfic;
static QStringList liste;
static QVector<bool> bipSat;
static QVector<TLE> tles;
static QList<Satellite> satellites;
static QStringList mapSatellites;

// Date courante
static Date dateCourante;
static double offsetUTC;

// Lieux d'observation
static QList<Observateur> observateurs;
static QStringList ficObs;
static QStringList listeObs;
static QStringList listeFicTLE;

// Cartes du monde
static QStringList ficMap;
static double DEG2PXHZ;
static double DEG2PXVT;

// Soleil, Lune
static Soleil soleil;
static Lune lune;

// Registre
static QSettings settings("Astropedia", "previsat");

PreviSat::PreviSat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreviSat)
{
    ui->setupUi(this);
    PreviSat::move(0, 0);
    ui->onglets->setCurrentIndex(0);
}

PreviSat::~PreviSat()
{
    delete ui;
}

/*
 * Initialisations generales
 */
void PreviSat::Initialisations()
{
    /* Declarations des variables locales */
    QDir di;

    /* Initialisations */
    info = true;
    dirExe = QCoreApplication::applicationDirPath();
    dirDat = dirExe + QDir::separator() + "data";
    dirCoo = dirDat + QDir::separator() + "coordonnees";
    dirMap = dirDat + QDir::separator() + "map";
    dirTle = dirExe + QDir::separator() + "tle";
    DEG2PXHZ = 1. / 0.45;
    DEG2PXVT = 1. / 0.45;

    /* Corps de la methode */
    // Verifications preliminaires
    di = QDir(dirTle);
    if (!di.exists())
        di.mkdir(dirTle);

    di = QDir(dirDat);
    if (!di.exists()) {
        QString message = tr("POSITION : Erreur rencontrée lors de l'initialisation\nLe répertoire %1 n'existe pas, veuillez réinstaller PreviSat");
        message = message.arg(dirDat);
        Messages::Afficher(message, Messages::ERREUR);
        exit(1);
    }

    // Verification de la presence des fichiers du repertoire data
    QStringList ficdata;
    ficdata << "chimes.wav" << "constellations.cst" << "constlines.cst" << "cross.cur" << "donnees.sat" <<
               "etoiles.str" << "gestionnaireTLE" << "iridium.sts";
    QStringListIterator it1(ficdata);
    while (it1.hasNext()) {
        const QFile fi(dirDat + QDir::separator() + it1.next());
        if (!fi.exists()) {
            QString message = tr("POSITION : Le fichier %1 n'existe pas, veuillez réinstaller PreviSat");
            message = message.arg(fi.fileName());
            Messages::Afficher(message, Messages::ERREUR);
            exit(1);
        }
    }

    // Chargement des fichiers images de cartes du monde
    di = QDir(dirMap);
    if (!di.exists())
        di.mkdir(dirMap);
    InitFicMap();

    // Verification de l'absence du fichier de mise a jour
    //    QFile fi(dirExe + QDir::separator() + "maj.exe");
    //    if (fi.exists())
    //        fi.remove();

    // Recuperation des donnees en memoire
    // TLE par defaut (lors de la premiere utilisation de PreviSat, ces chaines de caracteres sont vides)
    nom = settings.value("TLE/nom", "").toString();
    l1 = settings.value("TLE/l1", "").toString();
    l2 = settings.value("TLE/l2", "").toString();

    // Affichage des champs par defaut
    ui->affconst->setChecked(settings.value("affichage/affconst", true).toBool());
    ui->affcoord->setChecked(settings.value("affichage/affcoord", true).toBool());
    ui->affetoiles->setChecked(settings.value("affichage/affetoiles", true).toBool());
    ui->affgrille->setChecked(settings.value("affichage/affgrille", true).toBool());
    ui->affinvew->setChecked(settings.value("affichage/affinvew", false).toBool());
    ui->affinvns->setChecked(settings.value("affichage/affinvns", false).toBool());
    ui->afflune->setChecked(settings.value("affichage/afflune", true).toBool());
    ui->affnomlieu->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomlieu", Qt::PartiallyChecked).toUInt()));
    ui->affnomsat->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affnomsat", Qt::PartiallyChecked).toUInt()));
    ui->affnotif->setChecked(settings.value("affichage/affnotif", true).toBool());
    ui->affnuit->setChecked(settings.value("affichage/affnuit", true).toBool());
    ui->affphaselune->setChecked(settings.value("affichage/affphaselune", true).toBool());
    ui->affradar->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affradar", Qt::PartiallyChecked).toUInt()));
    ui->affsoleil->setChecked(settings.value("affichage/affsoleil", true).toBool());
    ui->afftraj->setChecked(settings.value("affichage/afftraj", true).toBool());
    ui->affvisib->setCheckState(static_cast<Qt::CheckState> (settings.value("affichage/affcoord", Qt::PartiallyChecked).toUInt()));
    ui->calJulien->setChecked(settings.value("affichage/calJulien", true).toBool());
    ui->extinctionAtmospherique->setChecked(settings.value("affichage/extinction", true).toBool());
    ui->intensiteOmbre->setValue(settings.value("affichage/intensiteOmbre", 40).toInt());
    ui->magnitudeEtoiles->setValue(settings.value("affichage/magnitudeEtoiles", 4.0).toDouble());
    ui->nombreTrajectoires->setValue(settings.value("affichage/nombreTrajectoires", 1).toUInt());
    ui->utcAuto->setChecked(settings.value("affichage/utcAuto", true).toBool());

    if (settings.value("affichage/utc", false).toBool())
        ui->utc->setChecked(true);
    else
        ui->heureLegale->setChecked(true);

    settings.setValue("fichier/path", dirExe);

    // Initialisation du lieu d'observation
    AffichageLieuObs();

    // Initialisation de la gestion des fichiers de lieux d'observation
    InitFicObs(true);
    if (ui->fichiersObs->count() > 0) {
        ui->fichiersObs->setCurrentRow(0);
        ui->ajdfic->setCurrentIndex(0);
    }

    // Affichage des fichiers TLE
    AffichageListeFichiersTLE();

    // Ouverture du fichier TLE par defaut
    nbSat = liste.size();
    if (nbSat == 0) {
        tles.resize(1);
        nbSat = 1;
    } else {
        tles.resize(nbSat);
        bipSat.resize(nbSat);
    }

    // Lecture du fichier TLE par defaut
    const QFile fi(nomfic);
    if (fi.exists()) {

        try {

            // Verification du fichier
            TLE::VerifieFichier(nomfic, true);

            // Lecture du fichier
            TLE::LectureFichier(nomfic, liste, tles);
        } catch (PreviSatException &e) {
        }

        // Mise a jour de la liste de satellites
        int i = 0;
        liste.clear();
        bipSat.clear();
        QVectorIterator<TLE> it2(tles);
        while (it2.hasNext()) {
            const TLE tle = it2.next();
            if (tle.getNorad() == "") {
                tles.remove(i);
            } else {
                liste.append(tles.at(i).getNorad());
                bipSat.append(false);
                i++;
            }
        }
        nbSat = tles.size();

        if (nbSat == 0) {
            tles[0] = TLE(l1, l2);

            // Ouverture du fichier TLE (pour placer dans la liste de l'interface graphique les satellites
            // contenus dans le fichier)
            AfficherListeSatellites(nomfic, liste);
            l1 = "";
            l2 = "";
        } else {
            AfficherListeSatellites(nomfic, liste);
        }
    } else {

        // Remplissage du premier element du tableau du TLE avec le satellite en memoire
        tles.clear();
        tles.append(TLE(l1, l2));
        liste.takeFirst();
        bipSat.resize(1);
        l1 = "";
        l2 = "";
        //...
    }

    // Recuperation des donnees satellites
    Satellite::LectureDonnees(liste, tles, satellites);


    // Initialisation de la date
    // Determination automatique de l'ecart heure locale - UTC
    const QDateTime dateLocale = QDateTime::currentDateTime();
    QDateTime dateUTC(dateLocale);
    dateUTC.setTimeSpec(Qt::UTC);
    const double ecart = dateLocale.secsTo(dateUTC) * NB_JOUR_PAR_SEC;
    offsetUTC = (ui->utcAuto->isChecked()) ? ecart : settings.value("temps/dtu", ecart).toDouble();
    ui->updown->setValue((int) (offsetUTC * NB_MIN_PAR_JOUR + EPS_DATES));

    // Date et heure locales
    dateCourante = Date(offsetUTC);

    // Calcul de la position des etoiles
    //...

    /* Retour */
    return;
}

/*
 * Chargement de la liste de lieux d'observation
 */
void PreviSat::InitFicObs(const bool alarm)
{
    /* Declarations des variables locales */

    /* Initialisations */
    ui->coordonnees->setVisible(false);
    ui->nouveauLieu->setVisible(false);
    ui->nouvelleCategorie->setVisible(false);

    /* Corps de la methode */
    ui->fichiersObs->clear();
    ui->ajdfic->clear();

    // Repertoire contenant les fichiers de lieux d'observation
    const QDir di = QDir(dirCoo);
    if (di.exists()) {

        // Nombre de fichiers contenus dans le repertoire 'coordonnees'
        if (di.entryList(QDir::Files).count() == 0) {
            if (alarm)
                Messages::Afficher(tr("POSITION : Erreur rencontrée lors de l'initialisation\nIl n'existe aucun fichier de lieux d'observation"), Messages::WARNING);
        } else {

            int sts;
			// Liste de fichiers de lieux d'observation
            foreach(QString fic, di.entryList(QDir::Files)) {

                sts = 0;

                // Verification que le fichier est un fichier de lieux d'observation
                const QString file = dirCoo + QDir::separator() + fic;
                QFile fi(file);
                if (fi.exists()) {

                    fi.open(QIODevice::ReadOnly);
                    QTextStream flux(&fi);
                    while (!flux.atEnd()) {
                        const QString lieu = flux.readLine();
                        if (lieu.size() > 0) {
                            if (lieu.size() > 33) {
                                if (lieu.at(14) == ' ' && lieu.at(28) == ' ' && lieu.at(33) == ' ') {
                                    sts = 0;
                                } else {
                                    sts = 1;
                                    break;
                                }
                            } else {
                                sts = 1;
                                break;
                            }
                        }
                    }
                }

                if (sts == 0) {
                    ficObs.append(file);
                    if (fic == "aapreferes") {
                        ui->fichiersObs->addItem(tr("Mes Préférés"));
                        ui->ajdfic->addItem(tr("Mes Préférés"));
                    } else {
                        ui->fichiersObs->addItem(fic.at(0).toUpper() + fic.mid(1));
                        ui->ajdfic->addItem(fic.at(0).toUpper() + fic.mid(1));
                    }
                }
            }
        }
    } else {
        if (alarm)
            Messages::Afficher(tr("POSITION : Erreur rencontrée lors de l'initialisation\nLe répertoire contenant les fichiers de lieux d'observation n'existe pas"), Messages::WARNING);
    }

    /* Retour */
    return;
}

/*
 * Chargement de la liste de cartes du monde
 */
void PreviSat::InitFicMap()
{
    /* Declarations des variables locales */
    QStringList filtres;

    /* Initialisations */
    QDir di(dirMap);
    filtres << "*.bmp" << "*.jpg" << "*.png";

    /* Corps de la methode */
    ui->listeMap->clear();
    ui->listeMap->addItem(tr("* Défaut"));
    ui->listeMap->setCurrentIndex(0);

    if (di.entryList(filtres, QDir::Files).count() == 0) {
        ui->listeMap->addItem(tr("Télécharger..."));
        return;
    }

    foreach(QString fic, di.entryList(filtres, QDir::Files)) {

        const QString file = dirMap + QDir::separator() + fic;
        ficMap.append(file);
        ui->listeMap->addItem(fic.at(0).toUpper() + fic.mid(1, fic.lastIndexOf(".")-1));
        if (settings.value("fichier/listeMap", "").toString() == file)
            ui->listeMap->setCurrentIndex(ficMap.indexOf(file));
    }
    ui->listeMap->addItem(tr("Télécharger..."));
//    if (ui->listeMap->currentIndex() > 0)



    /* Retour */
    return;
}

void PreviSat::AffichageDonnees()
{
    /* Declarations des variables locales */
    QString chaine2;

    /* Initialisations */
    QString unite1 = (ui->unitesKm->isChecked()) ? QObject::tr("km") : QObject::tr("mi");
    const QString unite2 = (ui->unitesKm->isChecked()) ? QObject::tr("km/s") : QObject::tr("mi/s");

    /* Corps de la methode */
    //if
    // Affichage maximise : on ne reactualise pas l'affichage des donnees numeriques
    //} else {

    if (satellites.isEmpty()) {
        l1 = "";
        l2 = "";
    }
    if (l1 == "" && l2 == "") {

        // Cas ou aucun satellite n'est selectionne dans la liste de satellites
        ui->satellite->setVisible(false);
        PreviSat::setWindowTitle("PreviSat 3.0");

        if (ui->onglets->count() == 7) {
            ui->onglets->removeTab(1);
            ui->onglets->removeTab(1);
        }
    } else {
        ui->satellite->setVisible(true);
        if (ui->onglets->count() < 7) {
            ui->onglets->insertTab(1, ui->osculateurs, tr("Éléments osculateurs"));
            ui->onglets->insertTab(2, ui->informations, tr("Informations satellite"));
        }
    }

    /*
     * Affichage des donnees sur l'onglet General
     */
    QString chaine = dateCourante.ToLongDate().append("  ").append(tr("UTC"));
    if (fabs(dateCourante.getOffsetUTC()) > EPSDBL100) {
        QTime heur;
        heur = heur.addSecs(NB_SEC_PAR_JOUR * fabs(dateCourante.getOffsetUTC() + EPS_DATES));
        chaine = chaine.append((dateCourante.getOffsetUTC() > 0.) ? " + " : " - ").
                append(heur.toString("hh:mm"));
        //...
    } else {
        //...
    }
    ui->dateHeure1->setText(chaine);
    ui->dateHeure2->setText(chaine);


    /*
     * Affichage des donnees relatives au satellite par defaut
     */
    if (l1 != "" && l2 != "") {
        // Nom
        ui->nomsat1->setText(nom);
        PreviSat::setWindowTitle("PreviSat 3.0 - " + nom);

        // Temps ecoule depuis l'epoque
        chaine = tr("%1 jours");
        QBrush brush;
        QColor col;
        QPalette palette;
        brush.setStyle(Qt::SolidPattern);

        // Indicateur de l'age du TLE
        if (fabs(satellites.at(0).getAgeTLE()) > 15.) {
            brush.setColor(Qt::red);
        } else if (fabs(satellites.at(0).getAgeTLE()) > 10.) {
            col.setNamedColor("orange");
            brush.setColor(col);
        } else if (fabs(satellites.at(0).getAgeTLE()) > 5.) {
            col.setNamedColor("goldenrod");
            brush.setColor(col);
        } else {
            col.setNamedColor("forestgreen");
            brush.setColor(col);
        }
        palette.setBrush(QPalette::WindowText, brush);
        ui->ageTLE->setPalette(palette);
        ui->ageTLE->setText(chaine.arg(dateCourante.getJourJulienUTC() - tles.at(0).getEpoque().
                                       getJourJulienUTC(), 0, 'f', 2));

        // Longitude/latitude/altitude
        const QString ews = (satellites.at(0).getLongitude() > 0.) ? tr("Ouest") : tr("Est");
        ui->longitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).getLongitude()), Maths::DEGRE,
                                                       3, 0, false, true).append(" ").append(ews));
        const QString nss = (satellites.at(0).getLatitude() > 0.) ? tr("Nord") : tr("Sud");
        ui->latitudeSat->setText(Maths::ToSexagesimal(fabs(satellites.at(0).getLatitude()), Maths::DEGRE,
                                                      2, 0, false, true).append(" ").append(nss));
        chaine = "%1 " + unite1;
        if (ui->unitesKm->isChecked()) {
            chaine = chaine.arg(satellites.at(0).getAltitude(), 0, 'f', 1);
            ui->altitudeSat->setText(chaine);
        } else {
            chaine = chaine.arg(satellites.at(0).getAltitude() * MILE_PAR_KM, 0, 'f', 1);
            ui->altitudeSat->setText(chaine);
        }

        // Hauteur/azimut/distance
        ui->hauteurSat->setText(Maths::ToSexagesimal(satellites.at(0).getHauteur(), Maths::DEGRE, 2, 0,
                                                     true, true));
        ui->azimutSat->setText(Maths::ToSexagesimal(satellites.at(0).getAzimut(), Maths::DEGRE, 3, 0,
                                                    false, true));
        chaine = "%1 " + unite1;
        if (ui->unitesKm->isChecked()) {
            chaine = chaine.arg(satellites.at(0).getDistance(), 0, 'f', 1);
            ui->distanceSat->setText(chaine);
        } else {
            chaine = chaine.arg(satellites.at(0).getDistance() * MILE_PAR_KM, 0, 'f', 1);
            ui->distanceSat->setText(chaine);
        }

        // Ascension droite/declinaison/constellation
        ui->ascensionDroiteSat->setText(Maths::ToSexagesimal(satellites.at(0).getAscensionDroite(),
                                                             Maths::HEURE1, 2, 0, false, true).trimmed());
        ui->declinaisonSat->setText(Maths::ToSexagesimal(satellites.at(0).getDeclinaison(), Maths::DEGRE,
                                                         2, 0, true, true).trimmed());
        ui->constellationSat->setText(satellites.at(0).getConstellation());

        // Direction/vitesse/range rate
        ui->directionSat->setText((satellites.at(0).getVitesse().getZ() > 0.) ? tr("Ascendant") :
                                                                                tr("Descendant"));
        chaine = "%1 " + unite2;
        if (ui->unitesKm->isChecked()) {
            chaine = chaine.arg(satellites.at(0).getVitesse().Norme(), 0, 'f', 3);
            ui->vitesseSat->setText(chaine);

            chaine = "%1%2 " + unite2;
            chaine = chaine.arg((satellites.at(0).getRangeRate() >= 0.) ? "+" : "-").
                    arg(fabs(satellites.at(0).getRangeRate()), 0, 'f', 3);
            ui->rangeRate->setText(chaine);

        } else {
            chaine = chaine.arg(satellites.at(0).getVitesse().Norme() * MILE_PAR_KM, 0, 'f', 3);
            ui->vitesseSat->setText(chaine);

            chaine = "%1%2 " + unite2;
            chaine = chaine.arg((satellites.at(0).getRangeRate() >= 0.) ? "+" : "-").
                    arg(fabs(satellites.at(0).getRangeRate() * MILE_PAR_KM), 0, 'f', 3);
            ui->rangeRate->setText(chaine);
        }

        // Magnitude/Illumination
        const double fractionilluminee = 100. * satellites.at(0).getFractionIlluminee();
        if (satellites.at(0).getMagnitudeStandard() > 98.) {

            // Magnitude standand inconnue
            if (satellites.at(0).isEclipse()) {
                ui->magnitudeSat->setText(tr("Satellite non visible (Ombre)"));
            } else {
                chaine = tr("Illumination : %1%");
                chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                if (satellites.at(0).isPenombre())
                    chaine = chaine.append(" ").append(tr("Pénombre"));
                ui->magnitudeSat->setText(chaine);
            }
        } else {

            // Satellite au dessus de l'horizon
            if (satellites.at(0).isVisible()) {

                // Satellite eclipse
                if (satellites.at(0).isEclipse()) {
                    ui->magnitudeSat->setText(tr("Satellite non visible (Ombre)"));
                } else {
                    chaine = tr("Magnitude (Illumination) : %1%2");
                    chaine = chaine.arg((satellites.at(0).getMagnitude() >= 0.) ? "+" : "-").
                            arg(fabs(satellites.at(0).getMagnitude()), 0, 'f', 1);
                    if (satellites.at(0).isPenombre())
                        chaine = chaine.append("*");
                    chaine = chaine.append(("(%1%)"));
                    chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                    ui->magnitudeSat->setText(chaine);
                }
            } else {
                chaine = tr("Satellite non visible");
                if (satellites.at(0).isPenombre())
                    chaine = chaine.append(" : ").append(tr("Pénombre"));
                chaine = chaine.append(" (%1%)");
                chaine = chaine.arg(fractionilluminee, 0, 'f', 0);
                if (satellites.at(0).isEclipse() && !satellites.at(0).isPenombre())
                    chaine = tr("Satellite non visible (Ombre)");
                ui->magnitudeSat->setText(chaine);
            }
        }

        // Conditions d'observation
        const double ht = soleil.getHauteur() * RAD2DEG;
        if (ht >= 0.)
            chaine = tr("Jour");
        else if (ht >= -6.)
            chaine = tr("Crépuscule civil");
        else if (ht >= -12.)
            chaine = tr("Crépuscule nautique");
        else if (ht >= -18.)
            chaine = tr("Crépuscule astronomique");
        else
            chaine = tr("Nuit");
        ui->conditionsObservation->setText(chaine);

        // Nombre d'orbites du satellite
        chaine = "%1";
        ui->nbOrbitesSat->setText(chaine.arg(satellites.at(0).getNbOrbites()));
    }


    /*
     * Donnees relatives au Soleil
     */
    // Hauteur/azimut/distance
    ui->hauteurSoleil->setText(Maths::ToSexagesimal(soleil.getHauteur(), Maths::DEGRE, 2, 0, true, true));
    ui->azimutSoleil->setText(Maths::ToSexagesimal(soleil.getAzimut(), Maths::DEGRE, 3, 0, false, true));
    chaine = "%1 " + tr("UA");
    chaine = chaine.arg(soleil.getDistanceUA(), 0, 'f', 3);
    ui->distanceSoleil->setText(chaine);

    // Ascension droite/declinaison/constellation
    ui->ascensionDroiteSoleil->setText(Maths::ToSexagesimal(soleil.getAscensionDroite(), Maths::HEURE1,
                                                            2, 0, false, true).trimmed());
    ui->declinaisonSoleil->setText(Maths::ToSexagesimal(soleil.getDeclinaison(), Maths::DEGRE,
                                                        2, 0, true, true).trimmed());
    ui->constellationSoleil->setText(soleil.getConstellation());


    /*
     * Donnees relatives a la Lune
     */
    // Hauteur/azimut/distance
    ui->hauteurLune->setText(Maths::ToSexagesimal(lune.getHauteur(), Maths::DEGRE, 2, 0, true, true));
    ui->azimutLune->setText(Maths::ToSexagesimal(lune.getAzimut(), Maths::DEGRE, 3, 0, false, true));
    chaine = "%1 " + unite1;
    if (ui->unitesKm->isChecked()) {
        chaine = chaine.arg(lune.getDistance(), 0, 'f', 0);
        ui->distanceLune->setText(chaine);
    } else {
        chaine = chaine.arg(lune.getDistance() * MILE_PAR_KM, 0, 'f', 0);
        ui->distanceLune->setText(chaine);
    }

    // Ascension droite/declinaison/constellation
    ui->ascensionDroiteLune->setText(Maths::ToSexagesimal(lune.getAscensionDroite(), Maths::HEURE1, 2, 0,
                                                          false, true).trimmed());
    ui->declinaisonLune->setText(Maths::ToSexagesimal(lune.getDeclinaison(), Maths::DEGRE, 2, 0,
                                                      true, true).trimmed());
    ui->constellationLune->setText(lune.getConstellation());

    // Illumination/Phase
    chaine = "%1%";
    chaine = chaine.arg(lune.getFractionIlluminee() * 100., 0, 'f', 0);
    ui->illuminationLune->setText(chaine);
    ui->phaseLune->setText(lune.getPhase());


    /*
     * Affichage des donnees sur l'onglet Elements Osculateurs
     */
    if (l1 != "" && l2 != "") {

        ui->nomsat2->setText(nom);
        ui->ligne1->setText(l1);
        ui->ligne2->setText(l2);

        // Vecteur d'etat
        // Position
        Vecteur3D vect;
        vect = (ui->unitesKm->isChecked()) ? satellites.at(0).getPosition() :
                                             satellites.at(0).getPosition() * MILE_PAR_KM;
        chaine2 = "%1%2 " + unite1;
        chaine = chaine2.arg((vect.getX() >= 0.) ? "+" : "-").arg(fabs(vect.getX()), 0, 'f', 3);
        ui->xsat->setText(chaine);
        chaine = chaine2.arg((vect.getY() >= 0.) ? "+" : "-").arg(fabs(vect.getY()), 0, 'f', 3);
        ui->ysat->setText(chaine);
        chaine = chaine2.arg((vect.getZ() >= 0.) ? "+" : "-").arg(fabs(vect.getZ()), 0, 'f', 3);
        ui->zsat->setText(chaine);

        // Vitesse
        vect = (ui->unitesKm->isChecked()) ? satellites.at(0).getVitesse() :
                                             satellites.at(0).getVitesse() * MILE_PAR_KM;
        chaine2 = "%1%2 " + unite2;
        chaine = chaine2.arg((vect.getX() >= 0.) ? "+" : "-").arg(fabs(vect.getX()), 0, 'f', 6);
        ui->vxsat->setText(chaine);
        chaine = chaine2.arg((vect.getY() >= 0.) ? "+" : "-").arg(fabs(vect.getY()), 0, 'f', 6);
        ui->vysat->setText(chaine);
        chaine = chaine2.arg((vect.getZ() >= 0.) ? "+" : "-").arg(fabs(vect.getZ()), 0, 'f', 6);
        ui->vzsat->setText(chaine);

        // Elements osculateurs
        chaine = "%1 " + unite1;
        double xval = (ui->unitesKm->isChecked()) ?
                    satellites.at(0).getElements().getDemiGrandAxe() :
                    satellites.at(0).getElements().getDemiGrandAxe() * MILE_PAR_KM;
        chaine = chaine.arg(xval, 0, 'f', 1);
        ui->demiGrandAxe->setText(chaine);

        chaine = "%1";
        chaine = chaine.arg(satellites.at(0).getElements().getExcentricite(), 0, 'f', 7);
        ui->excentricite->setText(chaine);

        chaine2 = "%1°";
        chaine = chaine2.arg(satellites.at(0).getElements().getInclinaison() * RAD2DEG, 0, 'f', 4);
        ui->inclinaison->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).getElements().getAscensionDroiteNA() * RAD2DEG, 0, 'f', 4);
        ui->ADNoeudAscendant->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).getElements().getArgumentPerigee() * RAD2DEG, 0, 'f', 4);
        ui->argumentPerigee->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).getElements().getAnomalieMoyenne() * RAD2DEG, 0, 'f', 4);
        ui->anomalieMoyenne->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).getElements().getAnomalieVraie() * RAD2DEG, 0, 'f', 4);
        ui->anomalieVraie->setText(chaine);

        chaine = chaine2.arg(satellites.at(0).getElements().getAnomalieExcentrique() * RAD2DEG, 0, 'f', 4);
        ui->anomalieExcentrique->setText(chaine);

        chaine = "±" +
                chaine2.arg(acos(RAYON_TERRESTRE / (RAYON_TERRESTRE + satellites.at(0).getAltitude())) *
                            RAD2DEG, 0, 'f', 2);
        ui->champDeVue->setText(chaine);

        // Apogee/perigee/periode orbitale
        chaine2 = "%2 %1 (%3 %1)";
        chaine2 = chaine2.arg(unite1);
        xval = (ui->unitesKm->isChecked()) ? satellites.at(0).getElements().getApogee() :
                                             satellites.at(0).getElements().getApogee() * MILE_PAR_KM;
        chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval - RAYON_TERRESTRE, 0, 'f', 1);
        ui->apogee->setText(chaine);

        xval = (ui->unitesKm->isChecked()) ? satellites.at(0).getElements().getPerigee() :
                                             satellites.at(0).getElements().getPerigee() * MILE_PAR_KM;
        chaine = chaine2.arg(xval, 0, 'f', 1).arg(xval - RAYON_TERRESTRE, 0, 'f', 1);
        ui->perigee->setText(chaine);

        ui->periode->setText(Maths::ToSexagesimal(satellites.at(0).getElements().getPeriode() * HEUR2RAD,
                                                  Maths::HEURE1, 1, 0, false, true));

        if (info) {
            // Affichage des donnees sur l'onglet Informations satellite

            // Nom du satellite
            ui->nomsat3->setText(nom);

            // Lignes du TLE
            ui->line1->setText(ui->ligne1->text());
            ui->line2->setText(ui->ligne2->text());

            // Numero NORAD
            ui->norad->setText(l1.mid(2, 5));

            // Designation COSPAR
            ui->cospar->setText(l1.mid(9, 8).trimmed());

            // Epoque du TLE
            ui->epoque->setText(tles.at(0).getEpoque().ToShortDate(Date::COURT));

            // Nombre d'orbites a l'epoque
            ui->nbOrbitesEpoque->setText(l2.mid(63, 5).trimmed());

            // Anomalie moyenne
            ui->anomalieMoy->setText(l2.mid(43, 8).append("°"));

            // Inclinaison
            ui->inclinaisonMoy->setText(l2.mid(8, 8).append("°"));

            // Excentricite
            ui->excentriciteMoy->setText("0." + l2.mid(26, 7));

            // Ascension droite du noeud ascendant
            ui->ADNoeudAscendantMoy->setText(l2.mid(17, 8).append("°"));

            // Argument du perigee
            ui->argumentPerigeeMoy->setText(l2.mid(34, 8).append("°"));

            // Nombre de revolutions par jour
            ui->nbRev->setText(l2.mid(52, 11));

            // (derivee de nbRev) / 2
            chaine2 = "%1";
            chaine = chaine2.arg(QString(l1.mid(33, 1).trimmed() + "0" + l1.mid(34, 9)).
                                 toDouble(), 0, 'f', 8);
            ui->nbRev2->setText(chaine);

            // (derivee seconde de nbRev) / 6
            chaine = chaine2.arg(QString(l1.mid(44, 1) + "0." + l1.mid(45, 5)).toDouble() *
                                 pow(10., QString(l1.mid(50, 2)).toDouble()), 0, 'f', 8);
            ui->nbRev3->setText(chaine);

            // Coefficient pseudo-balistique
            chaine = chaine2.arg(QString(l1.mid(53, 1) + "0." + l1.mid(54, 5)).toDouble() *
                                 pow(10., QString(l1.mid(59, 2)).toDouble()));
            ui->bstar->setText(chaine);

            // Magnitude standard/maximale
            if (satellites.at(0).getMagnitudeStandard() > 98.) {
                ui->magnitudeStdMax->setText("?/?");
            } else {
                // Estimation de la magnitude maximale
                xval = satellites.at(0).getMagnitudeStandard() - 15.75 + 5. *
                        log10(1.45 * (satellites.at(0).getElements().getDemiGrandAxe() *
                                      (1. - satellites.at(0).getElements().getExcentricite()) -
                                      RAYON_TERRESTRE));
                chaine = "%1%2/%3%4";
                chaine = chaine.arg((satellites.at(0).getMagnitudeStandard() >= 0.) ? "+" : "-").
                        arg(fabs(satellites.at(0).getMagnitudeStandard()), 0, 'f', 1).
                        arg((xval >= 0.) ? "+" : "-").arg(fabs(xval), 0, 'f', 1);
                ui->magnitudeStdMax->setText(chaine);
            }

            // Modele orbital
            chaine = (satellites.at(0).getMethod() == 'd') ? QObject::tr("SGP4 (DS)") :
                                                             QObject::tr("SPG4 (NE)");
            ui->modele->setText(chaine);

            // Dimensions du satellite
            double t1 = satellites.at(0).getT1();
            double t2 = satellites.at(0).getT2();
            double t3 = satellites.at(0).getT3();
            double section = satellites.at(0).getSection();
            unite1 = QObject::tr("m");
            if (ui->unitesMi->isChecked()) {
                t1 *= PIED_PAR_METRE;
                t2 *= PIED_PAR_METRE;
                t3 *= PIED_PAR_METRE;
                section = Maths::arrondi(section * PIED_PAR_METRE * PIED_PAR_METRE, 0);
                unite1 = QObject::tr("ft");
            }

            chaine2 = "%1 " + unite1;
            if (fabs(t2) < EPSDBL100 && fabs(t3) < EPSDBL100)
                chaine = QObject::tr("Sphérique. R=").append(chaine2.arg(t1, 0, 'f', 1));
            if (fabs(t2) >= EPSDBL100 && fabs(t3) < EPSDBL100)
                chaine = QObject::tr("Cylindrique. L=").append(chaine2.arg(t1, 0, 'f', 1)).
                        append(QObject::tr(", R=")).append(chaine2.arg(t2, 0, 'f', 1));
            if (fabs(t2) >= EPSDBL100 && fabs(t3) >= EPSDBL100) {
                chaine = QObject::tr("Boîte.").append(" %1 x %2 x %3 ").append(unite1);
                chaine = chaine.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1);
            }
            if (fabs(t1) < EPSDBL100)
                chaine = QObject::tr("Inconnues");

            if (fabs(section) > EPSDBL100) {
                chaine = chaine + " / %1 " + unite1;
                chaine = chaine.arg(section, 0, 'f', 2);
                ui->sq->setVisible(true);
            } else {
                ui->sq->setVisible(false);
            }

            ui->dimensions->setText(chaine);
            ui->sq->move(ui->dimensions->x() + ui->dimensions->width() - 4, ui->sq->y());
            info = false;
        }
    }
    //}

    /* Retour */
    return;
}

void PreviSat::AffichageCourbes()
{
    /* Declarations des variables locales */
    int ind;

    /* Initialisations */
    bool ht = false;
    for(int j=0; j<nbSat; j++)
        if (satellites.at(j).isVisible())
            ht = true;

    if (!ht) {
        for(int j=0; j<nbSat; j++)
            bipSat[j] = false;
    }

    // Notification sonore
    if (notif && ui->affnotif->isChecked()) {
        if (ui->tempsReel->isChecked())
            QSound::play(dirDat + QDir::separator() + "chimes.wav");
        notif = false;
    }

    // Phase de la Lune
    if (ui->affphaselune->isChecked()) {

        const double ll = atan2(lune.getPosition().getY(), lune.getPosition().getX());
        const double ls = atan2(soleil.getPosition().getY(), soleil.getPosition().getX());

        double diff = (ll - ls) * RAD2DEG;
        if (diff < 0.)
            diff += T360;
        ind = (int) (diff / 12.857) + 2;
        if (ind > 28)
            ind = 0;
    } else {
        ind = 14;
    }

    // Couleur du ciel
    QBrush bru(Qt::black);
    QBrush bru2(Qt::black);
    //if ()
    const double hts = soleil.getHauteur() * RAD2DEG;
    if (hts >= 0.) {
        // Jour
        bru = QBrush(QColor::fromRgb(213, 255, 254));

    } else {
        const int red = (int) (213.15126 / (1. + 0.0018199 * exp(-0.983684 * hts)) + 0.041477);
        const int green = (int) (qMax(qMin(256.928983 / (1. + 0.008251 * exp(-0.531535 * hts)) - 0.927648,
                                           255.), 0.));

        // Algorithme special pour le bleu
        int blue;
        if (hts >= -6.) {
            blue = 254;
        } else if (hts >= -12.) {
            blue = (int) (-2.74359 * hts * hts - 31.551282 * hts + 163.461538);
        } else {
            blue = (int) (qMax(273.1116 / (1. + 0.0281866 * exp(-0.282853 * hts)) - 1.46635, 0.));
        }
        bru = QBrush(QColor::fromRgb(red, green, blue));
    }
    //}

    /* Corps de la methode */
    if (!ui->carte->isHidden()) {

        QRect rect;
        QGraphicsScene *scene = new QGraphicsScene;
        QPixmap pixmap;
        const QString nomMap = (ui->listeMap->currentIndex() == 0) ?
                    ":/resources/map.jpg" :
                    ficMap.at(ui->listeMap->currentIndex());

        pixmap.load(nomMap);
        pixmap = pixmap.scaled(ui->carte->size());
        scene->addPixmap(pixmap);


        const int hcarte = ui->carte->height() - 3;
        const int lcarte = ui->carte->width() - 3;
        const int hcarte2 = qRound(hcarte * 0.5);
        const int lcarte2 = qRound(lcarte * 0.5);

        if (ui->affgrille->isChecked()) {

            scene->addLine(0, hcarte2, lcarte, hcarte2, QPen(Qt::white));
            scene->addLine(lcarte2, 0, lcarte2, hcarte, QPen(Qt::white));

            QPen style(Qt::lightGray);
            const int tablat[] = { hcarte / 6, hcarte / 3, (int) (hcarte / 1.5), (int) (hcarte / 1.2) };
            const int tablon[] = { lcarte / 12, lcarte / 6, (int) (lcarte * 0.25), lcarte / 3,
                                   (int) (lcarte / 2.4), (int) (7. * lcarte / 12.), (int) (lcarte / 1.5),
                                   (int) (lcarte * 0.75), (int) (lcarte / 1.2), (int) (11. * lcarte / 12.) };

            for(int j=0; j<4; j++)
                scene->addLine(0, tablat[j], lcarte, tablat[j], style);

            for(int j=0; j<10; j++)
                scene->addLine(tablon[j], 0, tablon[j], hcarte, style);

            // Tropiques
            style.setStyle(Qt::DashLine);
            scene->addLine(0, 66.55 * DEG2PXVT, lcarte, 66.55 * DEG2PXVT, style);
            scene->addLine(0, 113.45 * DEG2PXHZ, lcarte, 113.45 * DEG2PXHZ, style);

            if (!ui->carte->isHidden()) {
                //...
            }
        } else {
            //...
        }


        // Affichage du Soleil
        const int lsol = qRound((180. - soleil.getLongitude() * RAD2DEG) * DEG2PXHZ);
        const int bsol = qRound((90. - soleil.getLatitude() * RAD2DEG) * DEG2PXVT);

        if (ui->affsoleil->isChecked()) {
            rect = QRect(lsol - 7, bsol - 7, 15, 15);
            scene->addEllipse(rect, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::SolidPattern));
        }

        // Zone d'ombre
        if (ui->affnuit->isChecked()) {
            int jmin;
            int xmin = ui->carte->width() - 3;
            const QBrush alpha = QBrush(QColor::fromRgb(0, 0, 0, (int) (2.55 * ui->intensiteOmbre->value())));

            QVector<QPoint> zone;
            zone.resize(361);
            for(int j=0; j<361; j++) {
                zone[j].setX(qRound(soleil.getZone().at(j).x() * DEG2PXHZ));
                zone[j].setY(qRound(soleil.getZone().at(j).y() * DEG2PXVT));

                if (soleil.getLatitude() < 0.) {
                    if (zone[j].x() <= xmin) {
                        xmin = zone[j].x();
                        jmin = j;
                    }
                } else {
                    if (zone[j].x() < xmin) {
                        xmin = zone[j].x();
                        jmin = j;
                    }
                }
            }

            if (fabs(soleil.getLatitude()) > 0.002449 * DEG2RAD) {

                zone.resize(366);
                if (soleil.getLatitude() < 0.) {

                    for(int j=3; j<363; j++) {
                        zone[j].setX(qRound(soleil.getZone().at((j+jmin-2)%360).x() * DEG2PXHZ));
                        zone[j].setY(qRound(soleil.getZone().at((j+jmin-2)%360).y() * DEG2PXVT));
                    }
                    zone[0] = QPoint(ui->carte->width() - 1, 0);
                    zone[1] = QPoint(ui->carte->width() - 1, hcarte + 1);
                    zone[2] = QPoint(ui->carte->width() - 1, qRound(0.5 * (zone[3].y() + zone[362].y())));

                    zone[363] = QPoint(0, qRound(0.5 * (zone[3].y() + zone[362].y())));
                    zone[364] = QPoint(0, hcarte + 1);
                    zone[365] = QPoint(0, 0);

                } else {

                    for(int j=2; j<362; j++) {
                        zone[j].setX(qRound(soleil.getZone().at((j+jmin-2)%360).x() * DEG2PXHZ));
                        zone[j].setY(qRound(soleil.getZone().at((j+jmin-2)%360).y() * DEG2PXVT));
                    }

                    zone[0] = QPoint(0, 0);
                    zone[1] = QPoint(0, qRound(0.5 * (zone[2].y() + zone[361].y())));

                    zone[362] = QPoint(ui->carte->width() - 1, qRound(0.5 * (zone[2].y() + zone[361].y())));
                    zone[363] = QPoint(ui->carte->width() - 1, 0);
                    zone[364] = QPoint(ui->carte->width() - 1, hcarte + 1);
                    zone[365] = QPoint(0, hcarte + 1);
                }

                const QPolygonF poly(zone);
                scene->addPolygon(poly, QPen(Qt::NoBrush, 0), alpha);
            } else {

                QVector<QPoint> zone1;
                zone1.resize(4);
                const int x1 = qRound(qMin(soleil.getZone().at(90).x(), soleil.getZone().at(270).x()) *
                                      DEG2PXHZ);
                const int x2 = qRound(qMax(soleil.getZone().at(90).x(), soleil.getZone().at(270).x()) *
                                      DEG2PXHZ);

                if (lsol > lcarte / 4 && lsol < (4 * lcarte) / 3) {

                    QVector<QPoint> zone2;

                    zone1[0] = QPoint(0, 0);
                    zone1[1] = QPoint(x1, 0);
                    zone1[2] = QPoint(x1, hcarte + 1);
                    zone1[3] = QPoint(0, hcarte + 1);

                    zone2[0] = QPoint(ui->carte->width() - 1, 0);
                    zone2[1] = QPoint(x2, 0);
                    zone2[2] = QPoint(x2, hcarte + 1);
                    zone2[3] = QPoint(lcarte + 1, hcarte + 1);

                    const QPolygonF poly1(zone1);
                    const QPolygonF poly2(zone2);
                    scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
                    scene->addPolygon(poly2, QPen(Qt::NoBrush, 0), alpha);

                } else {

                    zone1[0] = QPoint(x1, 0);
                    zone1[1] = QPoint(x1, hcarte + 1);
                    zone1[2] = QPoint(x2, hcarte + 1);
                    zone1[3] = QPoint(x2, 0);

                    const QPolygonF poly1(zone1);
                    scene->addPolygon(poly1, QPen(Qt::NoBrush, 0), alpha);
                }
            }
        }

        // Affichage de la Lune
        if (ui->afflune->isChecked()) {

            const int llun = qRound((180. - lune.getLongitude() * RAD2DEG) * DEG2PXHZ) +
                    ui->carte->geometry().left() - 1;
            const int blun = qRound((90. - lune.getLatitude() * RAD2DEG) * DEG2PXVT) +
                    ui->carte->geometry().top() - 1;
            QString src = ":/resources/lune%1.gif";

            QGraphicsScene *scnlun = new QGraphicsScene;
            QPixmap pixlun;
            pixlun.load(src.arg(ind, 2, 10, QChar('0')));
            pixlun = pixlun.scaled(ui->imglun->size());
            scnlun->addPixmap(pixlun);

            ui->imglun->setStyleSheet("background: transparent; border: none");
            ui->imglun->setScene(scnlun);
            if (ui->rotationLune->isChecked() && observateurs.at(0).getLatitude() < 0.)
                ui->imglun->rotate(180.);

            ui->imglun->setGeometry(llun-7, blun-7, 17, 17);

        }

        // Lieux d'observation
        int nbMax;
        if (ui->affnomlieu->checkState() == Qt::Unchecked)
            nbMax = 0;
        if (ui->affnomlieu->checkState() == Qt::Checked ||
                ui->affnomlieu->checkState() == Qt::PartiallyChecked)
            nbMax = ui->lieuxObservation1->count() - 1;

        QPen crayon(Qt::white);
        for(int j=nbMax; j>=0; j--) {

            const int lobs = qRound((180. - observateurs.at(j).getLongitude() * RAD2DEG) * DEG2PXHZ);
            const int bobs = qRound((90. - observateurs.at(j).getLatitude() * RAD2DEG) * DEG2PXVT);

            scene->addLine(lobs-4, bobs, lobs+4, bobs, crayon);
            scene->addLine(lobs, bobs-4, lobs, bobs+4, crayon);

            if (j == 0 || ui->affnomlieu->checkState() == Qt::PartiallyChecked) {

                QGraphicsSimpleTextItem *txtObs = new QGraphicsSimpleTextItem(observateurs.at(j).
                                                                              getNomlieu());
                const int lng = txtObs->boundingRect().width();
                const int xnobs = (lobs + 4 + lng > lcarte) ? lobs - lng - 1 : lobs + 4;
                const int ynobs = (bobs + 9 > hcarte) ? bobs - 12 : bobs;

                txtObs->setBrush(Qt::white);
                txtObs->setPos(xnobs, ynobs);
                scene->addItem(txtObs);
            }
        }

        // Affichage des traces au sol du satellite par defaut
        const QColor crimson(220, 20, 60);
        if (nbSat > 0) {
            if (!satellites.at(0).isIeralt()) {
                int lsat1 = qRound(satellites.at(0).getTraceAuSol().at(0).at(0) * DEG2PXHZ);
                int bsat1 = qRound(satellites.at(0).getTraceAuSol().at(0).at(1) * DEG2PXVT);

                const QColor bleuClair(173, 216, 230);

                for(int j=1; j<satellites.at(0).getTraceAuSol().size()-1; j++) {
                    int lsat2 = qRound(satellites.at(0).getTraceAuSol().at(j).at(0) * DEG2PXHZ);
                    int bsat2 = qRound(satellites.at(0).getTraceAuSol().at(j).at(1) * DEG2PXVT);
                    int ils = 99999;

                    if (fabs(lsat2 - lsat1) > lcarte2) {
                        if (lsat2 < lsat1)
                            lsat2 += lcarte;
                        else
                            lsat1 += lcarte;
                        ils = j;
                    }

                    crayon = (satellites.at(0).getTraceAuSol().at(j).at(2) == 0) ? bleuClair : crimson;
                    scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                    if (ils == j) {
                        lsat1 -= lcarte;
                        lsat2 -= lcarte;
                        scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);
                        ils = 0;
                    }
                    lsat1 = qRound(satellites.at(0).getTraceAuSol().at(j).at(0) * DEG2PXHZ);
                    bsat1 = qRound(satellites.at(0).getTraceAuSol().at(j).at(1) * DEG2PXVT);
                }
            }
        }

        // Affichage de la zone de visibilite des satellites
        if (ui->affvisib->isChecked()) {
            if (ui->affvisib->checkState() == Qt::Checked)
                nbMax = 1;
            if (ui->affvisib->checkState() == Qt::PartiallyChecked)
                nbMax = liste.size();

            crayon = QPen(Qt::white);
            for(int isat=0; isat<nbMax; isat++) {

                if (!satellites.at(isat).isIeralt()) {
                    int lsat1 = qRound(satellites.at(isat).getZone().at(0).x() * DEG2PXHZ);
                    int bsat1 = qRound(satellites.at(isat).getZone().at(0).y() * DEG2PXVT);


                    for(int j=1; j<361; j++) {
                        int lsat2 = qRound(satellites.at(isat).getZone().at(j).x() * DEG2PXHZ);
                        int bsat2 = qRound(satellites.at(isat).getZone().at(j).y() * DEG2PXVT);
                        int ils = 99999;

                        if (fabs(lsat2 - lsat1) > lcarte2) {
                            if (lsat2 < lsat1)
                                lsat2 += lcarte;
                            else
                                lsat1 += lcarte;
                            ils = j;
                        }

                        scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);

                        if (ils == j) {
                            lsat1 -= lcarte + 1;
                            lsat2 -= lcarte + 1;
                            scene->addLine(lsat1, bsat1, lsat2, bsat2, crayon);
                            ils = 0;
                        }
                        lsat1 = qRound(satellites.at(isat).getZone().at(j).x() * DEG2PXHZ);
                        bsat1 = qRound(satellites.at(isat).getZone().at(j).y() * DEG2PXVT);
                    }
                }
            }
        }

        const QPen noir(Qt::black);
        QColor col;
        for(int isat=nbSat-1; isat>=0; isat--) {

            // Dessin des satellites
            if (!satellites.at(isat).isIeralt()) {
                const int lsat = qRound((180. - satellites.at(isat).getLongitude() * RAD2DEG) * DEG2PXHZ);
                const int bsat = qRound((90. - satellites.at(isat).getLatitude() * RAD2DEG) * DEG2PXVT);

                rect = QRect(lsat - 3, bsat - 3, 6, 6);
                col = (satellites.at(isat).isEclipse()) ? crimson : Qt::yellow;
                scene->addEllipse(rect, noir, QBrush(col, Qt::SolidPattern));

                // Nom des satellites
                if (ui->affnomsat->isChecked()) {

                    if ((ui->affnomsat->checkState() == Qt::Checked && isat == 0) ||
                            ui->affnomsat->checkState() == Qt::PartiallyChecked) {
                        QGraphicsSimpleTextItem *txtSat = new QGraphicsSimpleTextItem(tles.at(isat).getNom());
                        const int lng = txtSat->boundingRect().width();
                        const int xnsat = (lsat + 4 + lng > lcarte) ? lsat - lng - 1 : lsat + 4;
                        const int ynsat = (bsat + 9 > hcarte) ? bsat - 12 : bsat;

                        txtSat->setBrush(Qt::white);
                        txtSat->setPos(xnsat, ynsat);
                        scene->addItem(txtSat);
                    }
                }
            }
        }


        ui->carte->setScene(scene);
    }

    /* Retour */
    return;
}

/*
 * Affichage du lieu d'observation sur l'interface graphique
 */
void PreviSat::AffichageLieuObs()
{
    /* Declarations des variables locales */
    double atd, la, lo;

    /* Initialisations */
    listeObs = settings.value("observateur/lieu", "Paris                                                                 -002.345277778 +48.860833333 0060").toString().split("#");

    /* Corps de la methode */
    ui->lieuxObservation1->clear();
    QStringListIterator it(listeObs);
    while (it.hasNext()) {

        const QString obs = it.next();
        const QString nomlieu = obs.mid(0, 70).trimmed();

        ui->lieuxObservation1->addItem(nomlieu);
        //....

        // Longitude/Latitude/Altitude
        lo = obs.mid(70, 14).toDouble();
        la = obs.mid(85, 13).toDouble();
        atd = obs.mid(99, 4).toInt();

        // Creation du lieu d'observation
        observateurs.append(Observateur(nomlieu, lo, la, atd));

        if (obs == listeObs.first()) {

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");
            QString alt = "%1 %2";

            // Affichage des coordonnees
            ui->longitudeObs->setText(Maths::ToSexagesimal(fabs(lo) * DEG2RAD, Maths::DEGRE, 3, 0,
                                                           false, true).append(" ").append(ew));
            ui->latitudeObs->setText(Maths::ToSexagesimal(fabs(la) * DEG2RAD, Maths::DEGRE, 2, 0,
                                                          false, true).append(" ").append(ns));
            alt = alt.arg((ui->unitesKm->isChecked()) ? atd : atd * PIED_PAR_METRE).
                    arg((ui->unitesKm->isChecked()) ? tr("m") : tr("ft"));
            ui->altitudeObs->setText(alt);
        }
    }
    ui->lieuxObservation1->setCurrentIndex(0);
    //...

    /* Retour */
    return;
}

/*
 * Affichage de la liste de fichiers TLE
 */
void PreviSat::AffichageListeFichiersTLE()
{
    /* Declarations des variables locales */

    /* Initialisations */
    listeFicTLE = settings.value("fichier/nomFichiersTLE", "visual.txt#25544&20580").toString().split("$");

    /* Corps de la methode */
    ui->nomFichiersTLE->clear();
    QStringListIterator it(listeFicTLE);
    while (it.hasNext()) {

        const QString ficTLE = it.next();
        const QString fic = ficTLE.split("#").at(0);
        const QStringList listeTLE = ficTLE.split("#").at(1).split("&");

        const QFile fi((fic.contains(QDir::separator())) ? fic : dirTle + QDir::separator() + fic);
        if (fi.exists()) {
            ui->nomFichiersTLE->addItem(fic);

            if (ficTLE == listeFicTLE.first()) {
                nomfic = fi.fileName();
                liste = listeTLE;
            }
        }
    }
    ui->nomFichiersTLE->addItem(tr("* Parcourir..."));
    ui->nomFichiersTLE->setCurrentIndex(0);

    /* Retour */
    return;
}

/*
 * Affichage des noms des satellites dans les listes
 */
void PreviSat::AfficherListeSatellites(const QString fichier, const QStringList listeSat)
{
    /* Declarations des variables locales */
    QString ligne, li1, li2, magn, nomsat, norad;

    /* Initialisations */
    nomsat = "---";
    QFile donneesSatellites("data/donnees.sat");
    if (donneesSatellites.exists()) {
        donneesSatellites.open(QIODevice::ReadOnly);
        QTextStream flux(&donneesSatellites);
        magn = flux.readAll();
    }
    donneesSatellites.close();

    /* Corps de la methode */
    // NB : le fichier doit etre verifie au prealable
    ui->liste1->clear();
    QFile fichierTLE(fichier);
    if (fichierTLE.exists()) {

        fichierTLE.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream flux(&fichierTLE);

        while (!flux.atEnd()) {

            ligne = flux.readLine();

            if (ligne.at(0) == '1') {

                li1 = ligne;
                do {
                    li2 = flux.readLine();
                } while (li2.trimmed().length() == 0);

                norad = li1.mid(2, 5);
                if (nomsat.at(0) == '1' || nomsat == "---") {

                    const int indx1 = magn.indexOf(norad);
                    if (indx1 >= 0) {
                        const int indx2 = magn.indexOf("\n", indx1) - indx1;
                        nomsat = magn.mid(indx1 + 36, indx2 - 36).trimmed();
                    } else {
                        nomsat = norad;
                    }
                }

                if (nomsat.length() > 25 && nomsat.mid(25).contains("."))
                    nomsat = nomsat.mid(0, 15).trimmed();
                if (nomsat.toLower() == "iss (zarya)")
                    nomsat = "ISS";
                if (nomsat.toLower().contains("iridium") && nomsat.contains("["))
                    nomsat = nomsat.mid(0, nomsat.indexOf("[")).trimmed();
                //                if (nomsat.contains(" DEB") || nomsat.contains("R/B"))
                //                    nomsat = nomsat.append("  (").append(norad).append(")");

                bool check = false;
                for (int j=0; j<listeSat.length(); j++) {
                    if (norad == listeSat.at(j)) {
                        check = true;
                        if (j == 0) {
                            settings.setValue("TLE/nom", nomsat);
                            settings.setValue("TLE/lg1", li1);
                            settings.setValue("TLE/lg2", li2);
                            nom = nomsat;

                            l1 = li1;
                            l2 = li2;
                        }
                        break;
                    }
                }

                // Ajout du satellite dans la liste de satellites
                mapSatellites.append(nomsat + "#" + norad);
                QListWidgetItem *elem1 = new QListWidgetItem(nomsat, ui->liste1);
                elem1->setCheckState((check) ? Qt::Checked : Qt::Unchecked);
            }
            nomsat = ligne.trimmed();
        }
        mapSatellites.sort();
        //        if (li1.length() > 0) {
        //            ui->liste1->setCurrentRow(mapSatellites.indexOf(nom));
        //        }
    }

    /* Retour */
    return;
}

/*
 * Enchainement des calculs
 */
void PreviSat::EnchainementCalculs()
{
    /* Declarations des variables locales */
    bool extinction, visibilite;
    int nbTraces;

    /* Initialisations */
    // Nombre de traces au sol a afficher
    nbTraces = (ui->afftraj->isChecked()) ? ui->nombreTrajectoires->value() : 0;

    // Prise en compte de l'extinction atmospherique
    extinction = ui->extinctionAtmospherique->isChecked();

    // Calcul de la zone de visibilite des satellites
    visibilite = !ui->carte->isHidden();

    /* Corps de la methode */
    /*
     * Calcul de la position de l'observateur
     */
    observateurs[0].CalculPosVit(dateCourante);


    /*
     * Position du Soleil
     */
    soleil.CalculPosition(dateCourante);

    // Position topocentrique
    soleil.CalculCoordHoriz(observateurs.at(0));

    if (!ui->carte->isHidden()) {

        // Coordonnees terrestres
        soleil.CalculCoordTerrestres(observateurs.at(0));

        // Zone d'ombre
        soleil.CalculZoneVisibilite();

        //if (!)
        soleil.CalculCoordEquat(observateurs.at(0));
    }


    /*
     * Position de la Lune
     */
    if (ui->afflune->isChecked()) {

        lune.CalculPosition(dateCourante);
        lune.CalculPhase(soleil);

        // Coordonnees topocentriques
        lune.CalculCoordHoriz(observateurs.at(0));

        //if
        // Coordonnees equatoriales
        lune.CalculCoordEquat(observateurs.at(0));

        // Coordonnees terrestres
        lune.CalculCoordTerrestres(observateurs.at(0));
        //}
    }


    /*
     * Calcul de la position du catalogue d'etoiles
     */
    //if
    //...

    /*
     * Calcul de la position courante des satellites
     */
    Satellite::CalculPosVitListeSatellites(dateCourante, observateurs.at(0), soleil, nbTraces, visibilite,
                                           extinction, satellites);

    for (int i=0; i<nbSat; i++) {
        if (satellites[i].isVisible() && !bipSat[i]) {
            notif = true;
            bipSat[i] = true;
        }
    }

    if (nbSat > 0) {
        if (ui->onglets->count() == 7 && satellites[0].isIeralt()) {
            //...
            QString msg = tr("POSITION : Erreur rencontrée lors de l'exécution\nLa position du satellite %1 (numéro NORAD : %2) ne peut pas être calculée (altitude négative)");
            msg = msg.arg(tles.at(0).getNom()).arg(tles.at(0).getNorad());
            Messages::Afficher(msg, Messages::WARNING);
            //...
            l1 = "";
            l2 = "";
        }
        if (ui->onglets->count() < 7 && !satellites[0].isIeralt()) {
            l1 = tles.at(0).getLigne1();
            l2 = tles.at(0).getLigne2();
        }
    }

    /* Retour */
    return;
}
