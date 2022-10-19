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
 * >    antenne.cpp
 *
 * Localisation
 * >    interface.onglets.antenne
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 octobre 2022
 *
 * Date de revision
 * >
 *
 */

#include "antenne.h"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_antenne.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "configuration/configuration.h"
#include "librairies/corps/satellite/evenementsconst.h"
#include "librairies/exceptions/previsatexception.h"


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
Antenne::Antenne(QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::Antenne)
{
    _ui->setupUi(this);

    try {

        Initialisation();

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
    }
}


/*
 * Destructeur
 */
Antenne::~Antenne()
{
    settings.setValue("previsions/adresse", _ui->adresse->text());
    settings.setValue("previsions/port", _ui->port->value());
    delete _ui;
}


/*
 * Accesseurs
 */


/*
 * Modificateurs
 */


/*
 * Methodes publiques
 */
/*
 * Initialisation de l'affichage des frequences
 */
void Antenne::InitAffichageFrequences()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->frequenceMontante->clear();
    _ui->frequenceDescendante->clear();
    const QString norad = Configuration::instance()->listeSatellites().first().elementsOrbitaux().norad;
    _ui->frameFrequences->setVisible(false);

    /* Corps de la methode */
    if (Configuration::instance()->mapFrequencesRadio().contains(norad)) {

        const bool etat1 = _ui->frequenceMontante->blockSignals(true);
        const bool etat2 = _ui->frequenceDescendante->blockSignals(true);

        _ui->frameFrequences->setVisible(true);

        // Recuperation des frequences
        const QList<FrequenceRadio> listeFrequences = Configuration::instance()->mapFrequencesRadio()[norad];
        QListIterator<FrequenceRadio> it(listeFrequences);
        while (it.hasNext()) {

            const FrequenceRadio frequences = it.next();

            for(const QString &freq : frequences.frequenceMontante) {
                _ui->frequenceMontante->addItem(freq);
            }

            for(const QString &freq : frequences.frequenceDescendante) {
                _ui->frequenceDescendante->addItem(freq);
            }
        }

        if (_ui->frequenceMontante->count() == 0) {
            _ui->frequenceMontante->addItem("-");
        }

        if (_ui->frequenceDescendante->count() == 0) {
            _ui->frequenceDescendante->addItem("-");
        }

        _ui->frequenceMontante->blockSignals(etat1);
        _ui->frequenceDescendante->blockSignals(etat2);
    }

    /* Retour */
    return;
}

void Antenne::show(const QString &nomsat, const QString &dateAOS,  const ElementsAOS &elementsAOS)
{
    /* Declarations des variables locales */

    /* Initialisations */
    const Satellite &sat = Configuration::instance()->listeSatellites().first();
    const QString norad = sat.elementsOrbitaux().norad;

    /* Corps de la methode */
    if (Configuration::instance()->mapFrequencesRadio().contains(norad)) {

        double frequenceDescendante;
        double frequenceMontante;
        Signal signal;
        FrequenceRadio frequencesMontant;
        FrequenceRadio frequencesDescendant;

        const double rangeRate = sat.rangeRate();
        const double distance = sat.distance();

        const int indexMontant = _ui->frequenceMontante->currentIndex();
        const int indexDescendant = _ui->frequenceDescendante->currentIndex();

        if (indexMontant >= 0) {
            frequencesMontant = Configuration::instance()->mapFrequencesRadio()[norad].at(indexMontant);
        }

        if (indexDescendant >= 0) {
            frequencesDescendant = Configuration::instance()->mapFrequencesRadio()[norad].at(indexDescendant);
        }

        // Frequences en Hertz
        const QString freqMontante = _ui->frequenceMontante->currentText();
        if (freqMontante.contains("-")) {

            // On prend la moyenne des 2 frequences
            const QStringList freqs = freqMontante.split(" ", Qt::SkipEmptyParts).first().split("-", Qt::SkipEmptyParts);
            frequenceMontante = (freqs.first().toDouble() + freqs.last().toDouble()) * 0.5e6;

        } else {
            frequenceMontante = (_ui->frequenceMontante->currentText().split(" ", Qt::SkipEmptyParts).first() + "0").toDouble() * 1.e6;
        }

        const QString freqDescendante = _ui->frequenceDescendante->currentText();
        if (freqDescendante.contains("-")) {

            // On prend la moyenne des 2 frequences
            const QStringList freqs = freqDescendante.split(" ", Qt::SkipEmptyParts).first().split("-", Qt::SkipEmptyParts);
            frequenceDescendante = (freqs.first().toDouble() + freqs.last().toDouble()) * 0.5e6;

        } else {
            frequenceDescendante = (_ui->frequenceDescendante->currentText().split(" ", Qt::SkipEmptyParts).first() + "0").toDouble() * 1.e6;
        }

        // Donnees sur le signal montant
        signal.Calcul(rangeRate, distance, frequenceMontante);
        const bool aff1 = (fabs(frequenceMontante) > 0.);
        _ui->dopplerMontant->setText((aff1) ? QString("%1 Hz").arg(-signal.doppler(), 0, 'f', 0) : "-");
        _ui->frequenceMontanteReelle->setText((aff1) ? QString("%1 MHz").arg((frequenceMontante - signal.doppler()) * 1.e-6, 0, 'f', 6) : "-");
        _ui->frequenceMontanteReelle->setStyleSheet(QString("font-weight: ") + ((sat.isVisible() && aff1) ? "bold" : "normal"));
        _ui->attenuationMontant->setText((aff1) ? QString("%1 dB").arg(signal.attenuation(), 0, 'f', 2) : "-");
        _ui->delaiMontant->setText((aff1) ? QString("%1 ms").arg(signal.delai(), 0, 'f', 2) : "-");
        _ui->baliseMontant->setText((frequencesMontant.balise.isEmpty()) ? "-" : frequencesMontant.balise);
        _ui->modeMontant->setText((frequencesMontant.mode.isEmpty()) ? "-" : frequencesMontant.mode);
        _ui->signalAppelMontant->setText((frequencesMontant.signalAppel.isEmpty()) ? "-" : frequencesMontant.signalAppel);

        // Donnees sur le signal descendant
        signal.Calcul(rangeRate, distance, frequenceDescendante);
        const bool aff2 = (fabs(frequenceDescendante) > 0.);
        _ui->dopplerDescendant->setText((aff2) ? QString("%1 Hz").arg(signal.doppler(), 0, 'f', 0) : "-");
        _ui->frequenceDescendanteReelle->setText((aff2) ? QString("%1 MHz").arg((frequenceDescendante + signal.doppler()) * 1.e-6, 0, 'f', 6) : "-");
        _ui->frequenceDescendanteReelle->setStyleSheet(QString("font-weight: ") + ((sat.isVisible() && aff2) ? "bold" : "normal"));
        _ui->attenuationDescendant->setText((aff2) ? QString("%1 dB").arg(signal.attenuation(), 0, 'f', 2) : "-");
        _ui->delaiDescendant->setText((aff2) ? QString("%1 ms").arg(signal.delai(), 0, 'f', 2) : "-");
        _ui->baliseDescendant->setText((frequencesDescendant.balise.isEmpty()) ? "-" : frequencesDescendant.balise);
        _ui->modeDescendant->setText((frequencesDescendant.mode.isEmpty()) ? "-" : frequencesDescendant.mode);
        _ui->signalAppelDescendant->setText((frequencesDescendant.signalAppel.isEmpty()) ? "-" : frequencesDescendant.signalAppel);
    }

    // Nom du satellite
    _ui->nomsatRadio->setText(nomsat);

    // Affichage du prochain AOS/LOS
    if (elementsAOS.aos) {

        const QString delai = dateAOS.section(")", -2, -2).section("(", 1);
        const QString chaine = tr("Prochain %1 %2").arg(elementsAOS.typeAOS).arg(delai);
        _ui->prochainAOS->setText(chaine);
        _ui->prochainAOS->setToolTip((chaine.contains(tr("AOS"))) ? tr("Acquisition du signal") : tr("Perte du signal"));
        _ui->prochainAOS->setVisible(true);

    } else {
        _ui->prochainAOS->setVisible(false);
    }

    /* Retour */
    return;
}

void Antenne::changeEvent(QEvent *evt)
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
/*
 * Initialisation de la classe Antenne
 */
void Antenne::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _ui->adresse->setText(settings.value("previsions/adresse").toString());
    _ui->port->setValue(settings.value("previsions/port").toInt());

    _ui->frameFrequences->setVisible(false);
    _ui->frameSatellite->setVisible(false);
    _ui->donneesTransmises->setVisible(false);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}
