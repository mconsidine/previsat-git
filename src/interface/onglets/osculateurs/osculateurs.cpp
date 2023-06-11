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
 * >    osculateurs.cpp
 *
 * Localisation
 * >    interface.onglets.osculateurs
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    22 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QFile>
#include <QSettings>
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "osculateurs.h"
#include "ui_osculateurs.h"
#include "configuration/configuration.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "librairies/maths/vecteur3d.h"


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
Osculateurs::Osculateurs(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Osculateurs)
{
    _ui->setupUi(this);

    try {

        _date = nullptr;

#if (BUILD_TEST == false)
        Initialisation();
#endif

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Osculateurs::~Osculateurs()
{
    settings.setValue("affichage/typeParametres", _ui->typeParametres->currentIndex());
    settings.setValue("affichage/typeRepere", _ui->typeRepere->currentIndex());

    EFFACE_OBJET(_date);

    delete _ui;
}


/*
 * Accesseurs
 */
Ui::Osculateurs *Osculateurs::ui() const
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
void Osculateurs::show(const Date &date)
{
    /* Declarations des variables locales */
    QString text;

    /* Initialisations */
    EFFACE_OBJET(_date);
    _date = new Date(date);

    const QString fmt1 = "%1";
    const QString fmt2 = "%1°";

    const Satellite &satellite = Configuration::instance()->listeSatellites().first();
    const ElementsOsculateurs &elements = satellite.elementsOsculateurs();
    const QString unite = (settings.value("affichage/unite").toBool()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile");

    /* Corps de la methode */
    AffichageVecteurEtat(date);
    AffichageElementsOsculateurs();

    // Nom du satellite
    _ui->nomsat->setText(satellite.elementsOrbitaux().nom);

    // Autres elements osculateurs
    _ui->anomalieVraie->setText(fmt2.arg(elements.anomalieVraie() * MATHS::RAD2DEG, 0, 'f', 4));
    _ui->anomalieExcentrique->setText(fmt2.arg(elements.anomalieExcentrique() * MATHS::RAD2DEG, 0, 'f', 4));
    _ui->champDeVue->setText("±" + fmt2.arg(acos(TERRE::RAYON_TERRESTRE / (TERRE::RAYON_TERRESTRE + satellite.altitude())) * MATHS::RAD2DEG, 0, 'f', 2));

    // Apogee/perigee/periode orbitale
    const QString fmt3 = "%1 %2 (%3 %2)";
    const double apogee = (settings.value("affichage/unite").toBool()) ? elements.apogee() : elements.apogee() * TERRE::MILE_PAR_KM;
    double apogeeAlt = elements.apogee() - TERRE::RAYON_TERRESTRE;
    apogeeAlt = (settings.value("affichage/unite").toBool()) ? apogeeAlt : apogeeAlt * TERRE::MILE_PAR_KM;
    _ui->apogee->setText(fmt3.arg(apogee, 0, 'f', 1).arg(unite).arg(apogeeAlt, 0, 'f', 1));

    const double perigee = (settings.value("affichage/unite").toBool()) ? elements.perigee() : elements.perigee() * TERRE::MILE_PAR_KM;
    double perigeeAlt = elements.perigee() - TERRE::RAYON_TERRESTRE;
    perigeeAlt = (settings.value("affichage/unite").toBool()) ? perigeeAlt : perigeeAlt * TERRE::MILE_PAR_KM;
    _ui->perigee->setText(fmt3.arg(perigee, 0, 'f', 1).arg(unite).arg(perigeeAlt, 0, 'f', 1));
    _ui->periode->setText(Maths::ToSexagesimal(elements.periode() * MATHS::HEUR2RAD, AngleFormatType::HEURE1, 1, 0, false, true));

    // Informations de signal
    _ui->doppler->setText(text.asprintf("%+.0f Hz", satellite.signal().doppler()));
    _ui->attenuation->setText(fmt1.arg(satellite.signal().attenuation(), 0, 'f', 2) + " dB");
    _ui->delai->setText(fmt1.arg(satellite.signal().delai(), 0, 'f', 2) + " ms");

    // Triplet de phasage
    const QString fmt4 = "[ %1; %2; %3 ] %4";
    const int nu0 = satellite.phasage().nu0();
    const int dt0 = satellite.phasage().dt0();
    const int ct0 = satellite.phasage().ct0();
    const int nbOrb = satellite.phasage().nbOrb();

    if ((nu0 == CORPS::ELEMENT_PHASAGE_INDEFINI)
            || (dt0 == CORPS::ELEMENT_PHASAGE_INDEFINI)
            || (ct0 == CORPS::ELEMENT_PHASAGE_INDEFINI)
            || (nbOrb == CORPS::ELEMENT_PHASAGE_INDEFINI)) {
        _ui->phasage->setText(tr("N/A", "Not applicable"));
    } else {
        _ui->phasage->setText(fmt4.arg(nu0).arg(dt0).arg(ct0).arg(nbOrb));
    }

    /* Retour */
    return;
}


/*
 * Sauvegarde des donnees de l'onglet
 */
void Osculateurs::SauveOngletElementsOsculateurs(const QString &fichier)
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
            flux << tr("Date :", "Date and hour") << " " << _ui->dateHeure1->text() << Qt::endl << Qt::endl;

            // Donnees sur le satellite
            flux << tr("Nom du satellite :") + " " + _ui->nomsat->text() << Qt::endl << Qt::endl;

            flux << tr("Vecteur d'état") << " (" << _ui->typeRepere->currentText() << ") :" << Qt::endl;
            QString chaine = tr("x : %1\t\tvx : %2", "Position, velocity");
            flux << chaine.arg(_ui->xsat->text().rightJustified(13, ' ')).arg(_ui->vxsat->text().rightJustified(15, ' ')) << Qt::endl;

            chaine = tr("y : %1\t\tvy : %2", "Position, velocity");
            flux << chaine.arg(_ui->ysat->text().rightJustified(13, ' ')).arg(_ui->vysat->text().rightJustified(15, ' ')) << Qt::endl;

            chaine = tr("z : %1\t\tvz : %2", "Position, velocity");
            flux << chaine.arg(_ui->zsat->text().rightJustified(13, ' ')).arg(_ui->vzsat->text().rightJustified(15, ' ')) << Qt::endl << Qt::endl;

            flux << tr("Éléments osculateurs :") << Qt::endl;
            switch (_ui->typeParametres->currentIndex()) {

            case 0:
                // Parametres kepleriens
                chaine = tr("Demi-grand axe       : %1\tAscension droite du noeud ascendant : %2");
                flux << chaine.arg(_ui->demiGrandAxeKeplerien->text()).arg(_ui->ADNoeudAscendant->text().rightJustified(9, '0')) << Qt::endl;

                chaine = tr("Excentricité         : %1\tArgument du périgée                 : %2");
                flux << chaine.arg(_ui->excentricite->text()).arg(_ui->argumentPerigee->text().rightJustified(9, '0')) << Qt::endl;

                chaine = tr("Inclinaison          : %1\tAnomalie moyenne                    : %2");
                flux << chaine.arg(_ui->inclinaison->text().rightJustified(9, '0')).arg(_ui->anomalieMoyenne->text().rightJustified(9, '0'))
                     << Qt::endl << Qt::endl;
                break;

            case 1:
                // Parametres circulaires
                chaine = tr("Demi-grand axe       : %1\tAscension droite du noeud ascendant : %2");
                flux << chaine.arg(_ui->demiGrandAxeCirc->text()).arg(_ui->ADNoeudAscendant2->text().rightJustified(9, '0')) << Qt::endl;

                chaine = tr("Ex                   : %1\tInclinaison                         : %2", "Ex = Component X of eccentricity vector");
                flux << chaine.arg(_ui->ex1->text().rightJustified(10, ' ')).arg(_ui->inclinaison2->text().rightJustified(9, '0')) << Qt::endl;

                chaine = tr("Ey                   : %1\tPosition sur orbite                 : %2", "Ey = Component Y of eccentricity vector");
                flux << chaine.arg(_ui->ey1->text().rightJustified(10, ' ')).arg(_ui->positionSurOrbite->text().rightJustified(9, '0'))
                     << Qt::endl << Qt::endl;
                break;

            case 2:
                // Parametres equatoriaux
                chaine = tr("Demi-grand axe       : %1\tIx                 : %2", "Ix = Component X of inclination vector");
                flux << chaine.arg(_ui->demiGrandAxeEquat->text()).arg(_ui->ix1->text()) << Qt::endl;

                chaine = tr("Excentricité         : %1\tIy                 : %2", "Iy = Component Y of inclination vector");
                flux << chaine.arg(_ui->excentricite2->text()).arg(_ui->iy1->text()) << Qt::endl;

                chaine = tr("Longitude du périgée : %1\tAnomalie moyenne   : %2");
                flux << chaine.arg(_ui->longitudePerigee->text().rightJustified(9, '0')).arg(_ui->anomalieMoyenne2->text().rightJustified(9, '0'))
                     << Qt::endl << Qt::endl;
                break;

            case 3:
                // Parametres circulaires equatoriaux
                chaine = tr("Demi-grand axe       : %1\tIx                          : %2", "Ix = Component X of inclination vector");
                flux << chaine.arg(_ui->demiGrandAxeCircEquat->text()).arg(_ui->ix2->text().rightJustified(10, ' ')) << Qt::endl;

                chaine = tr("Ex                   : %1\tIy                          : %2",
                            "Ex = Component X of eccentricity vector, Iy = Component Y of inclination vector");
                flux << chaine.arg(_ui->ex2->text().rightJustified(10, ' ')).arg(_ui->iy2->text().rightJustified(10, ' ')) << Qt::endl;

                chaine = tr("Ey                   : %1\tArgument de longitude vraie : %2", "Ey = Component Y of eccentricity vector");
                flux << chaine.arg(_ui->ey2->text().rightJustified(10, ' ')).arg(_ui->argumentLongitudeVraie2->text().rightJustified(9, '0'))
                     << Qt::endl << Qt::endl;
                break;

            default:
                break;
            }

            chaine = tr("Anomalie vraie       : %1\tApogée  (Altitude) : %2");
            flux << chaine.arg(_ui->anomalieVraie->text().rightJustified(9, '0')).arg(_ui->apogee->text()) << Qt::endl;

            chaine = tr("Anomalie excentrique : %1\tPérigée (Altitude) : %2");
            flux << chaine.arg(_ui->anomalieExcentrique->text().rightJustified(9, '0')).arg(_ui->perigee->text()) << Qt::endl;

            chaine = tr("Champ de vue         : %1  \tPériode orbitale   : %2");
            flux << chaine.arg(_ui->champDeVue->text()).arg(_ui->periode->text().replace(" ", "")) << Qt::endl << Qt::endl;


            flux << tr("Divers :") << Qt::endl;
            chaine = tr("Doppler @ 100 MHz    : %1", "Doppler effect at 100 MegaHertz");
            flux << chaine.arg(_ui->doppler->text()) << Qt::endl;

            chaine = tr("Atténuation          : %1");
            flux << chaine.arg(_ui->attenuation->text()) << Qt::endl;

            chaine = tr("Délai                : %1", "Delay of signal at light speed");
            flux << chaine.arg(_ui->delai->text()) << Qt::endl << Qt::endl;

            chaine = tr("Phasage              : %1");
            flux << chaine.arg(_ui->phasage->text()) << Qt::endl;
        }

        sw.close();

    } catch (PreviSatException const &e) {
    }

    /* Retour */
    return;
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
 * Affichage des elements osculateurs
 */
void Osculateurs::AffichageElementsOsculateurs()
{
    /* Declarations des variables locales */
    QString text;

    /* Initialisations */
    const QString fmt1 = "%1";
    const QString fmt2 = "%1°";

    const QString unite = (settings.value("affichage/unite").toBool()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile");
    const ElementsOsculateurs &elements = Configuration::instance()->listeSatellites().first().elementsOsculateurs();
    double demiGrandAxe = elements.demiGrandAxe();

    if (!settings.value("affichage/unite").toBool()) {
        demiGrandAxe *= TERRE::MILE_PAR_KM;
    }

    const QString demiGdAxe = text.asprintf("%.1f ", demiGrandAxe) + unite;

    /* Corps de la methode */
    _ui->demiGrandAxeKeplerien->setText(demiGdAxe);
    _ui->demiGrandAxeCirc->setText(demiGdAxe);
    _ui->demiGrandAxeEquat->setText(demiGdAxe);
    _ui->demiGrandAxeCircEquat->setText(demiGdAxe);

    _ui->stackedWidget_elements->setCurrentIndex(_ui->typeParametres->currentIndex());

    switch (_ui->typeParametres->currentIndex()) {
    default:
    case 0:

        // Parametres kepleriens
        _ui->excentricite->setText(fmt1.arg(elements.excentricite(), 0, 'f', 7));
        _ui->inclinaison->setText(fmt2.arg(elements.inclinaison() * MATHS::RAD2DEG, 0, 'f', 4));
        _ui->ADNoeudAscendant->setText(fmt2.arg(elements.ascensionDroiteNoeudAscendant() * MATHS::RAD2DEG, 0, 'f', 4));
        _ui->argumentPerigee->setText(fmt2.arg(elements.argumentPerigee() * MATHS::RAD2DEG, 0, 'f', 4));
        _ui->anomalieMoyenne->setText(fmt2.arg(elements.anomalieMoyenne() * MATHS::RAD2DEG, 0, 'f', 4));
        break;

    case 1:

        // Parametres circulaires
        _ui->ex1->setText(fmt1.arg(elements.exCirc(), 0, 'f', 7));
        _ui->ey1->setText(fmt1.arg(elements.eyCirc(), 0, 'f', 7));
        _ui->inclinaison2->setText(fmt2.arg(elements.inclinaison() * MATHS::RAD2DEG, 0, 'f', 4));
        _ui->ADNoeudAscendant2->setText(fmt2.arg(elements.ascensionDroiteNoeudAscendant() * MATHS::RAD2DEG, 0, 'f', 4));
        _ui->positionSurOrbite->setText(fmt2.arg(elements.pso() * MATHS::RAD2DEG, 0, 'f', 4));
        break;

    case 2:

        // Parametres equatoriaux
        _ui->excentricite2->setText(fmt1.arg(elements.excentricite(), 0, 'f', 7));
        _ui->ix1->setText(fmt1.arg(elements.ix(), 0, 'f', 7));
        _ui->iy1->setText(fmt1.arg(elements.iy(), 0, 'f', 7));
        _ui->longitudePerigee->setText(fmt2.arg((elements.ascensionDroiteNoeudAscendant() + elements.argumentPerigee()) * MATHS::RAD2DEG, 0, 'f', 4));
        _ui->anomalieMoyenne2->setText(fmt2.arg(elements.anomalieMoyenne() * MATHS::RAD2DEG, 0, 'f', 4));
        break;

    case 3:

        // Parametres circulaires equatoriaux
        _ui->ex2->setText(fmt1.arg(elements.exCEq(), 0, 'f', 7));
        _ui->ey2->setText(fmt1.arg(elements.eyCEq(), 0, 'f', 7));
        _ui->ix2->setText(fmt1.arg(elements.ix(), 0, 'f', 7));
        _ui->iy2->setText(fmt1.arg(elements.iy(), 0, 'f', 7));
        _ui->argumentLongitudeVraie2->setText(fmt2.arg(elements.argumentLongitudeVraie() * MATHS::RAD2DEG, 0, 'f', 4));
        break;
    }

    /* Retour */
    return;
}

/*
 * Affichage du vecteur d'etat
 */
void Osculateurs::AffichageVecteurEtat(const Date &date)
{
    /* Declarations des variables locales */
    QString text;
    Vecteur3D position;
    Vecteur3D vitesse;

    /* Initialisations */

    /* Corps de la methode */
    const Satellite &satellite = Configuration::instance()->listeSatellites().first();
    const QString unite = (settings.value("affichage/unite").toBool()) ? tr("km", "Kilometer") : tr("nmi", "nautical mile");

    if (_ui->typeRepere->currentIndex() == 0) {
        position = satellite.position();
    } else {
        satellite.CalculPosVitECEF(date, position, vitesse);
    }

    if (!settings.value("affichage/unite").toBool()) {
        position *= TERRE::MILE_PAR_KM;
    }

    // Position cartesienne
    _ui->xsat->setText(text.asprintf("%+.3f ", position.x()) + unite);
    _ui->ysat->setText(text.asprintf("%+.3f ", position.y()) + unite);
    _ui->zsat->setText(text.asprintf("%+.3f ", position.z()) + unite);

    /* Retour */
    return;
}

/*
 * Initialisation de la classe Osculateurs
 */
void Osculateurs::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    const bool etat1 = _ui->typeRepere->blockSignals(true);
    _ui->typeRepere->setCurrentIndex(settings.value("affichage/typeRepere", 0).toInt());
    _ui->typeRepere->blockSignals(etat1);

    const bool etat2 = _ui->typeParametres->blockSignals(true);
    _ui->typeParametres->setCurrentIndex(settings.value("affichage/typeParametres", 0).toInt());
    _ui->typeParametres->blockSignals(etat2);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void Osculateurs::mouseDoubleClickEvent(QMouseEvent *evt)
{
    /* Declarations des variables locales */

    /* Initialisations */
    Q_UNUSED(evt)

    /* Corps de la methode */
    if (_ui->dateHeure1->underMouse()) {

        // Passage en mode manuel
        emit ModeManuel(true);

    } else if (_ui->vxsat->underMouse() || _ui->vysat->underMouse() || _ui->vzsat->underMouse()) {

        // Changement des unites de vitesse
        emit AffichageVitesses(*_date, true);
    }

    /* Retour */
    return;
}

void Osculateurs::on_dateHeure2_dateTimeChanged(const QDateTime &dateTime)
{
    if (_ui->dateHeure2->isVisible()) {
        emit ModificationDate(dateTime);
    }
}

void Osculateurs::on_typeRepere_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AffichageVecteurEtat(*_date);
    emit AffichageVitesses(*_date);
}


void Osculateurs::on_typeParametres_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    AffichageElementsOsculateurs();
}
