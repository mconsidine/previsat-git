/*
 *     PreviSat, Satellite tracking software
 *     Copyright (C) 2005-2026  Astropedia web: http://previsat.free.fr  -  mailto: previsat.app@gmail.com
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
 * >    ajustementdates.cpp
 *
 * Localisation
 * >    interface.onglets.telescope
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    5 mars 2023
 *
 * Date de revision
 * >    1er janvier 2025
 *
 */

#include "ajustementdates.h"
#include "interface/radar/radar.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/exceptions/exception.h"
#include "librairies/maths/maths.h"
#include "ui_ajustementdates.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
AjustementDates::AjustementDates(const QDateTime &dateInitiale,
                                 const QDateTime &dateFinale,
                                 const Observateur &observateur,
                                 const ElementsOrbitaux &elements,
                                 const double offset,
                                 const double hauteur,
                                 QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::AjustementDates),
    _date1(dateInitiale),
    _date2(dateFinale),
    _offset(offset),
    _hauteur(hauteur),
    _observateur(observateur),
    _elements(elements)
{
    _ui->setupUi(this);

    try {

        Initialisation();

    } catch (Exception const &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw Exception();
    }
}


/*
 * Destructeur
 */
AjustementDates::~AjustementDates()
{
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
void AjustementDates::show()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    MajCoordHorizDebut();
    MajCoordHorizFin();

    /* Retour */
    return;
}

void AjustementDates::changeEvent(QEvent *evt)
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
void AjustementDates::on_ajustementDateInitiale_valueChanged(int value)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (value < _ui->ajustementDateFinale->value()) {

        _ui->dateInitiale->setDateTime(_date1.addSecs(value));
        MajCoordHorizDebut();

    } else {
        _ui->ajustementDateInitiale->setValue(_ui->ajustementDateFinale->value());
    }

    /* Retour */
    return;
}

void AjustementDates::on_ajustementDateFinale_valueChanged(int value)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (value > _ui->ajustementDateInitiale->value()) {

        _ui->dateFinale->setDateTime(_date1.addSecs(value));
        MajCoordHorizFin();

    } else {
        _ui->ajustementDateFinale->setValue(_ui->ajustementDateInitiale->value());
    }

    /* Retour */
    return;
}

void AjustementDates::on_buttonBox_accepted()
{
    emit AjusterDates(_ui->dateInitiale->dateTime(), _ui->dateFinale->dateTime());
}

void AjustementDates::on_buttonBox_rejected()
{
    emit AjusterDates(_date1, _date2);
}


/*
 * Initialisation de la classe AjustementDates
 */
void AjustementDates::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _radar = new Radar(_ui->frameRadar, false);

    const Date dateInit(_date1.addSecs(-10), _offset);
    Satellite sat(_elements);
    const ElementsAOS elementsAos1 = Evenements::CalculAOS(dateInit, sat, _observateur, SensCalcul::CHRONOLOGIQUE, _hauteur);
    QDateTime dt1 = elementsAos1.date.ToQDateTime(DateFormatSec::FORMAT_SEC).addSecs(static_cast<quint64> (floor(_offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES)));
    if (dt1 > _date2) {
        dt1 = _date1;
    }

    _ui->dateInitiale->setMinimumDateTime(_date1);
    _ui->dateInitiale->setMaximumDateTime(_date2.addSecs(-1));
    _ui->dateInitiale->setDateTime(dt1);

    const Date dateFin(_date2, _offset);
    const ElementsAOS elementsAos2 = Evenements::CalculAOS(dateFin, sat, _observateur, SensCalcul::ANTI_CHRONOLOGIQUE, _hauteur);
    QDateTime dt2 = elementsAos2.date.ToQDateTime(DateFormatSec::FORMAT_SEC)
                        .addSecs(static_cast<quint64> (floor(_offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES)));
    if (dt2 < _date1) {
        dt2 = _date2;
    }

    _ui->dateFinale->setMinimumDateTime(_date1.addSecs(1));
    _ui->dateFinale->setMaximumDateTime(_date2);
    _ui->dateFinale->setDateTime(dt2);

    const int ecart = static_cast<int> (_date1.secsTo(_date2));

    const bool etat1 = _ui->ajustementDateInitiale->blockSignals(true);
    _ui->ajustementDateInitiale->setMinimum(1);
    _ui->ajustementDateInitiale->setMaximum(ecart - 1);
    _ui->ajustementDateInitiale->setValue(static_cast<int> (_date1.secsTo(dt1)));
    _ui->ajustementDateInitiale->setTickInterval(ecart / 10);
    _ui->ajustementDateInitiale->blockSignals(etat1);

    const bool etat2 = _ui->ajustementDateFinale->blockSignals(true);
    _ui->ajustementDateFinale->setMinimum(1);
    _ui->ajustementDateFinale->setMaximum(ecart - 1);
    _ui->ajustementDateFinale->setValue(ecart - static_cast<int> (dt2.secsTo(_date2)));
    _ui->ajustementDateFinale->setTickInterval(ecart / 10);
    _ui->ajustementDateFinale->blockSignals(etat2);

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

/*
 * Mise a jour des coordonnees horizontales pour la date de debut
 */
void AjustementDates::MajCoordHorizDebut()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double jj0 = Date(_ui->dateInitiale->dateTime(), 0.).jourJulienUTC();
    const double jj1 = Date(_ui->dateFinale->dateTime(), 0.).jourJulienUTC();
    const double offset = Date::CalculOffsetUTC(_ui->dateInitiale->dateTime());
    const Date date0(jj0 - offset, 0.);
    const Date date1(jj1 - offset, 0.);

    /* Corps de la methode */
    // Position de l'observateur
    _observateur.CalculPosVit(date0);

    // Position Soleil et Lune
    Soleil soleil;
    soleil.CalculPositionSimp(date0);
    soleil.CalculCoordHoriz(_observateur, true);

    Lune lune;
    lune.CalculPositionSimp(date0);
    lune.CalculCoordHoriz(_observateur, true);

    // Position du satellite
    Satellite sat(_elements);
    sat.CalculPosVit(date0);
    sat.CalculCoordHoriz(_observateur, true);

    ConditionEclipse condEcl;
    condEcl.CalculSatelliteEclipse(sat.position(), soleil);
    sat.setConditionEclipse(condEcl);
    sat.CalculTraceCiel(date0, true, true, _observateur, 0, date1.jourJulienUTC());

    // Mise a jour de l'interface
    _radar->show(_observateur, soleil, lune, QList<Satellite> () << sat);

    // Mise a jour de l'interface
    _ui->hauteurDebut->setText(Maths::ToSexagesimal(sat.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutDebut->setText(Maths::ToSexagesimal(sat.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));

    /* Retour */
    return;
}

/*
 * Mise a jour des coordonnees horizontales pour la date de fin
 */
void AjustementDates::MajCoordHorizFin()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double jj0 = Date(_ui->dateInitiale->dateTime(), 0.).jourJulienUTC();
    const double jj1 = Date(_ui->dateFinale->dateTime(), 0.).jourJulienUTC();
    const double offset = Date::CalculOffsetUTC(_ui->dateInitiale->dateTime());
    const Date date0(jj0 - offset, 0.);
    const Date date1(jj1 - offset, 0.);

    /* Corps de la methode */
    // Position de l'observateur
    _observateur.CalculPosVit(date0);

    // Position Soleil et Lune
    Soleil soleil;
    soleil.CalculPositionSimp(date0);
    soleil.CalculCoordHoriz(_observateur, true);

    Lune lune;
    lune.CalculPositionSimp(date0);
    lune.CalculCoordHoriz(_observateur, true);

    // Position du satellite
    Satellite sat(_elements);
    sat.CalculPosVit(date0);
    sat.CalculCoordHoriz(_observateur, true);

    ConditionEclipse condEcl;
    condEcl.CalculSatelliteEclipse(sat.position(), soleil);
    sat.setConditionEclipse(condEcl);
    sat.CalculTraceCiel(date0, true, true, _observateur, 0, date1.jourJulienUTC());

    _radar->show(_observateur, soleil, lune, QList<Satellite> () << sat);

    // Position de l'observateur
    _observateur.CalculPosVit(date1);

    // Position du satellite a la date de fin
    sat.CalculPosVit(date1);
    sat.CalculCoordHoriz(_observateur, true);

    // Mise a jour de l'interface
    _ui->hauteurFin->setText(Maths::ToSexagesimal(sat.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutFin->setText(Maths::ToSexagesimal(sat.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));

    /* Retour */
    return;
}
