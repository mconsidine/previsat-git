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
 * >    informationssatellite.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
 *
 * Date de revision
 * >    7 aout 2023
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFile>
#include <QSettings>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/satellite.h"
#include "informationssatellite.h"
#include "ui_informationssatellite.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
InformationsSatellite::InformationsSatellite(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::InformationsSatellite)
{
    _ui->setupUi(this);
    _ui->siteLancement->installEventFilter(this);
    _ui->pays->installEventFilter(this);
    _ui->categorieOrbite->installEventFilter(this);
}


/*
 * Destructeur
 */
InformationsSatellite::~InformationsSatellite()
{
    delete _ui;
}


/*
 * Accesseurs
 */
Ui::InformationsSatellite *InformationsSatellite::ui() const
{
    return _ui;
}

/*
 * Modificateurs
 */


/*
 * Methodes publiques
 */
/*
 * Affichage des informations de l'onglet
 */
void InformationsSatellite::show(const Satellite &satellite)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const QString fmt1 = "%1";
    const QString fmt2 = "%1°";
    const ElementsOrbitaux elem = satellite.elementsOrbitaux();
    const Donnees donnee = elem.donnees;

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat->setText(elem.nom);

    // Numero NORAD
    _ui->norad->setText(elem.norad);

    // Designation COSPAR
    _ui->cospar->setText(elem.cospar);

    // Epoque des elements orbitaux
    _ui->epoque->setText(elem.epoque.ToShortDate(DateFormat::FORMAT_COURT, ( settings.value("affichage/systemeHoraire").toBool()) ?
                                                     DateSysteme::SYSTEME_24H : DateSysteme::SYSTEME_12H));

    // Coefficient pseudo-ballistique
    _ui->bstar->setText(fmt1.arg(elem.bstar, 0, 'g', 6));


    // Moyen mouvement
    _ui->nbRev->setText(fmt1.arg(elem.no, 0, 'f', 8));

    // Derivees du moyen mouvement
    _ui->nbRev2->setText(fmt1.arg(elem.ndt20, 0, 'f', 8));
    _ui->nbRev3->setText(fmt1.arg(elem.ndd60, 0, 'f', 8));

    // Nombre d'orbites a l'epoque
    _ui->nbOrbitesEpoque->setText(fmt1.arg(elem.nbOrbitesEpoque));


    // Date de lancement
    _ui->dateLancement->setText((donnee.dateLancement().isEmpty()) ? tr("Inconnue") : donnee.dateLancement());

    // Categorie d'orbite
    _ui->categorieOrbite->setText((donnee.categorieOrbite().isEmpty()) ? tr("Inconnue") : donnee.categorieOrbite());

    // Pays ou organisation
    _ui->pays->setText((donnee.pays().isEmpty()) ? tr("Inconnu") : donnee.pays());

    // Site de lancement
    _ui->siteLancement->setText((donnee.siteLancement().isEmpty()) ? tr("Inconnu") : donnee.siteLancement());
    _ui->siteLancement->adjustSize();
    _ui->siteLancement->setFixedHeight(16);


    // Inclinaison moyenne
    _ui->inclinaisonMoy->setText(fmt2.arg(elem.inclo, 0, 'f', 4));

    // Ascension droite moyenne du noeud ascendant
    _ui->ADNoeudAscendantMoy->setText(fmt2.arg(elem.omegao, 0, 'f', 4));

    // Excentricite moyenne
    _ui->excentriciteMoy->setText(fmt1.arg(elem.ecco, 0, 'f', 7));

    // Argument du perigee moyen
    _ui->argumentPerigeeMoy->setText(fmt2.arg(elem.argpo, 0, 'f', 4));


    // Anomalie moyenne (moyenne)
    _ui->anomalieMoy->setText(fmt2.arg(elem.mo, 0, 'f', 4));

    // Magnitude standard, methode de determination de la magnitude, magnitude maximale
    if (donnee.magnitudeStandard() < CORPS::MAGNITUDE_INDEFINIE) {

        QString text;
        Satellite sat(satellite.elementsOrbitaux());
        sat.CalculPosVit(sat.elementsOrbitaux().epoque);
        sat.CalculElementsOsculateurs(sat.elementsOrbitaux().epoque);
        const ElementsOsculateurs elements = sat.elementsOsculateurs();
        const double magMax = donnee.magnitudeStandard() - 15.75 + 5. * log10(1.45 * (elements.demiGrandAxe() * (1. - elements.excentricite())
                                                                                      - TERRE::RAYON_TERRESTRE));
        _ui->magnitudeStdMax->setText(text.asprintf("%+.2f%c/%+.1f", donnee.magnitudeStandard(), donnee.methMagnitude(), magMax));

    } else {
        _ui->magnitudeStdMax->setText("?/?");
    }

    // Modele orbital
    _ui->modele->setText((satellite.method() == 'd') ? "SGP4 (DS)" : "SGP4 (NE)");
    _ui->modele->setToolTip((satellite.method() == 'd') ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));

    // Dimensions du satellite
    double t1 = donnee.t1();
    double t2 = donnee.t2();
    double t3 = donnee.t3();
    double section = donnee.section();
    const QString unite = (settings.value("affichage/unite").toBool()) ? tr("m", "meter") : tr("ft", "foot");
    if (!settings.value("affichage/unite").toBool()) {
        t1 *= TERRE::PIED_PAR_METRE;
        t2 *= TERRE::PIED_PAR_METRE;
        t3 *= TERRE::PIED_PAR_METRE;
        section = arrondi(section * TERRE::PIED_PAR_METRE * TERRE::PIED_PAR_METRE, 0);
    }

    QString dimensions;
    if ((fabs(t2) < MATHS::EPSDBL100) && (fabs(t3) < MATHS::EPSDBL100)) {
        const QString fmt3 = tr("Sphérique. R=%1 %2", "R = radius");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite);
    }
    if ((fabs(t2) >= MATHS::EPSDBL100) && (fabs(t3) < MATHS::EPSDBL100)) {
        const QString fmt3 = tr("Cylindrique. L=%1 %2, R=%3 %2", "L = height; R = radius");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(unite).arg(t2, 0, 'f', 1);
    }
    if ((fabs(t2) >= MATHS::EPSDBL100) && (fabs(t3) >= MATHS::EPSDBL100)) {
        const QString fmt3 = tr("Boîte. %1 x %2 x %3 %4");
        dimensions = fmt3.arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1).arg(unite);
    }
    if (fabs(t1) < MATHS::EPSDBL100) {
        dimensions = tr("Inconnues");
    }

    if (fabs(section) > MATHS::EPSDBL100) {
        dimensions.append(" / %1 %2");
        dimensions = dimensions.arg(section, 0, 'f', 2).arg(unite);
        _ui->sq->setVisible(true);
    } else {
        _ui->sq->setVisible(false);
    }

    _ui->dimensions->setText(dimensions);

    /* Retour */
    return;
}

/*
 * Sauvegarde des donnees de l'onglet
 */
void InformationsSatellite::SauveOngletInformations(const QString &fichier)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    try {

        QFile sw(fichier);
        if (sw.open(QIODevice::WriteOnly | QIODevice::Text)) {

            if (!sw.isWritable()) {
                qWarning() << "Problème de droits d'écriture du fichier" << sw.fileName();
                throw PreviSatException(tr("Problème de droits d'écriture du fichier %1").arg(sw.fileName()), MessageType::WARNING);
            }

            QTextStream flux(&sw);
            flux.setEncoding(QStringConverter::Utf8);

#if (BUILD_TEST == false)
            const QString titre = "%1 %2 / %3 (c) %4";
            flux << titre.arg(APP_NAME).arg(QString(APP_VER_MAJ)).arg(ORG_NAME).arg(QString(APP_ANNEES_DEV)) << Qt::endl << Qt::endl << Qt::endl;
#endif

            // Donnees sur le satellite
            flux << tr("Nom du satellite :") + " " + _ui->nomsat->text() << Qt::endl << Qt::endl;

            QString chaine = tr("Numéro NORAD            : %1 \t\tMoyen mouvement       : %2 rev/jour\t Date de lancement  : %3",
                                "revolution per day");
            flux << chaine.arg(_ui->norad->text()).arg(_ui->nbRev->text()).arg(_ui->dateLancement->text()) << Qt::endl;

            chaine = tr("Désignation COSPAR      : %1\t\tn'/2                  : %2 rev/jour^2\t Catégorie d'orbite : %3",
                        "n'/2 = derivative of the mean motion divided by two (in revolution per day square)");
            flux << chaine.arg(_ui->cospar->text()).arg(_ui->nbRev2->text().rightJustified(11, ' ')).arg(_ui->categorieOrbite->text()) << Qt::endl;

            chaine = tr("Époque (UTC)            : %1\tn\"/6                  : %2 rev/jour^3\t Pays/Organisation  : %3",
                        "n\"/6 = second derivative of the mean motion divided by six (in revolution per day cube)");
            flux << chaine.arg(_ui->epoque->text()).arg(_ui->nbRev3->text().rightJustified(11, ' ')).arg(_ui->pays->text()) << Qt::endl;

            chaine = tr("Coeff pseudo-balistique : %1 (1/Re)\tNb orbites à l'époque : %2\t\t\t Site de lancement  : %3",
                        "Pseudo-ballistic coefficient in 1/Earth radius");
            flux << chaine.arg(_ui->bstar->text()).arg(_ui->nbOrbitesEpoque->text()).arg(_ui->siteLancement->text()) << Qt::endl << Qt::endl;

            chaine = tr("Inclinaison             : %1\t\tAnomalie moyenne      : %2");
            flux << chaine.arg(_ui->inclinaisonMoy->text().trimmed().rightJustified(9, '0'))
                    .arg(_ui->anomalieMoy->text().trimmed().rightJustified(9, '0')) << Qt::endl;

            chaine = tr("AD noeud ascendant      : %1\t\tMagnitude std/max     : %2",
                        "Right ascension of the ascending node, Standard/Maximum magnitude");
            flux << chaine.arg(_ui->ADNoeudAscendantMoy->text().trimmed().rightJustified(9, '0')).arg(_ui->magnitudeStdMax->text()) << Qt::endl;

            chaine = tr("Excentricité            : %1\t\tModèle orbital        : %2");
            flux << chaine.arg(_ui->excentriciteMoy->text()).arg(_ui->modele->text()) << Qt::endl;

            chaine = tr("Argument du périgée     : %1\t\tDimensions/Section    : %2%3");
            flux << chaine.arg(_ui->argumentPerigeeMoy->text().trimmed().rightJustified(9, '0')).arg(_ui->dimensions->text())
                    .arg((_ui->dimensions->text() == tr("Inconnues")) ? "" : "^2") << Qt::endl;

        }
        sw.close();

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
}

void InformationsSatellite::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
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
bool InformationsSatellite::eventFilter(QObject *watched, QEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (event->type() == QEvent::MouseMove) {

        if (_ui->siteLancement->underMouse()) {

            const QString acronyme = _ui->siteLancement->text();
            const Observateur site = Configuration::instance()->mapSitesLancement()[acronyme];

            emit AffichageSiteLancement(acronyme, site);
            emit AfficherMessageStatut(site.nomlieu(), 10);
            _ui->siteLancement->setToolTip(site.nomlieu());

        } else if (_ui->pays->underMouse()) {

            const QString pays = Configuration::instance()->mapPays()[_ui->pays->text()];
            _ui->pays->setToolTip(pays);
            emit AfficherMessageStatut(pays, 10);

        } else if (_ui->categorieOrbite->underMouse()) {

            const QString categorie = Configuration::instance()->mapCategoriesOrbite()[_ui->categorieOrbite->text()];
            _ui->categorieOrbite->setToolTip(categorie);
            emit AfficherMessageStatut(categorie, 10);
        }
    }

    /* Retour */
    return QFrame::eventFilter(watched, event);
}
