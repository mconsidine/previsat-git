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
 * >
 *
 */

#include "ajustementdates.h"
#include "librairies/corps/satellite/evenements.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/exceptions/previsatexception.h"
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
AjustementDates::AjustementDates(const QDateTime &dateInitiale, const QDateTime &dateFinale, const Observateur &observateur,
                                 const ElementsOrbitaux &elements, const double offset, const double hauteur, QWidget *parent) :
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

    } catch (PreviSatException &e) {
        qCritical() << "Erreur Initialisation" << metaObject()->className();
        throw PreviSatException();
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
    MajCoordHorizDebut();
    MajCoordHorizFin();
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
    if (value < _ui->ajustementDateFinale->value()) {

        _ui->dateInitiale->setDateTime(_date1.addSecs(value));
        MajCoordHorizDebut();

    } else {
        _ui->ajustementDateInitiale->setValue(_ui->ajustementDateFinale->value());
    }
}

void AjustementDates::on_ajustementDateFinale_valueChanged(int value)
{
    if (value > _ui->ajustementDateInitiale->value()) {

        _ui->dateFinale->setDateTime(_date1.addSecs(value));
        MajCoordHorizFin();

    } else {
        _ui->ajustementDateFinale->setValue(_ui->ajustementDateInitiale->value());
    }
}

void AjustementDates::on_buttonBox_accepted()
{
    emit AjusterDates(_ui->dateInitiale->dateTime(), _ui->dateFinale->dateTime());
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

    const Date dateInit(_date1.addSecs(-10), _offset);
    Satellite sat(_elements);
    const ElementsAOS elementsAos1 = Evenements::CalculAOS(dateInit, sat, _observateur, SensCalcul::CHRONOLOGIQUE, _hauteur);
    QDateTime dt1 = elementsAos1.date.ToQDateTime(1).addSecs(static_cast<quint64> (floor(_offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES)));
    if (dt1 > _date2) {
        dt1 = _date1;
    }

    _ui->dateInitiale->setMinimumDateTime(_date1);
    _ui->dateInitiale->setMaximumDateTime(_date2.addSecs(-1));
    _ui->dateInitiale->setDateTime(dt1);

    const Date dateFin(_date2, _offset);
    const ElementsAOS elementsAos2 = Evenements::CalculAOS(dateFin, sat, _observateur, SensCalcul::ANTI_CHRONOLOGIQUE, _hauteur);
    QDateTime dt2 = elementsAos2.date.ToQDateTime(1).addSecs(static_cast<quint64> (floor(_offset * DATE::NB_SEC_PAR_JOUR + DATE::EPS_DATES)));
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
    const double jj = Date(_ui->dateInitiale->dateTime(), 0.).jourJulienUTC();
    const double offset = Date::CalculOffsetUTC(_ui->dateInitiale->dateTime());
    const Date date(jj - offset, 0.);

    /* Corps de la methode */
    _observateur.CalculPosVit(date);

    Satellite sat(_elements);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(_observateur);

    _ui->hauteurDebut->setText(Maths::ToSexagesimal(sat.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutDebut->setText(Maths::ToSexagesimal(sat.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));

    /* Retour */
    return;
}

/*
 * Mise a jour des coordonnees horizontales pour la date de debut
 */
void AjustementDates::MajCoordHorizFin()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const double jj = Date(_ui->dateFinale->dateTime(), 0.).jourJulienUTC();
    const double offset = Date::CalculOffsetUTC(_ui->dateFinale->dateTime());
    const Date date(jj - offset, 0.);

    /* Corps de la methode */
    _observateur.CalculPosVit(date);

    Satellite sat(_elements);
    sat.CalculPosVit(date);
    sat.CalculCoordHoriz(_observateur);

    _ui->hauteurFin->setText(Maths::ToSexagesimal(sat.hauteur(), AngleFormatType::DEGRE, 2, 0, true, true));
    _ui->azimutFin->setText(Maths::ToSexagesimal(sat.azimut(), AngleFormatType::DEGRE, 3, 0, false, true));

    /* Retour */
    return;
}
