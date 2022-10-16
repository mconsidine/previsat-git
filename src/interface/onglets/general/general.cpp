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
 * >    general.cpp
 *
 * Localisation
 * >    interface.onglets.general
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 juin 2022
 *
 * Date de revision
 * >
 *
 */

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wswitch-default"
#include <QSettings>
#include "ui_options.h"
#pragma GCC diagnostic warning "-Wswitch-default"
#pragma GCC diagnostic warning "-Wconversion"
#include "general.h"
#include "configuration/configuration.h"
#include "interface/options/options.h"
#include "librairies/exceptions/previsatexception.h"
#include "librairies/maths/maths.h"
#include "ui_general.h"


// Registre
static QSettings settings(ORG_NAME, APP_NAME);

static const char* _titresLuneSoleil[] = {
    QT_TRANSLATE_NOOP("General", "Coordonnées du Soleil"),
    QT_TRANSLATE_NOOP("General", "Coordonnées de la Lune"),
    QT_TRANSLATE_NOOP("General", "Évènements Soleil"),
    QT_TRANSLATE_NOOP("General", "Évènements Lune")
};


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
General::General(Options *options, QWidget *parent) :
    QFrame(parent),
    _ui(new Ui::General)
{
    _ui->setupUi(this);

    _options = options;

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
General::~General()
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
void General::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);

        _ui->soleilLunePrec->setToolTip(
                    QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + _ui->stackedWidget_soleilLune->count() - 1)
                % _ui->stackedWidget_soleilLune->count()]));
        _ui->soleilLuneSuiv->setToolTip(QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + 1) %
                                  _ui->stackedWidget_soleilLune->count()]));
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
 * Initialisation de la classe General
 */
void General::Initialisation()
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    qInfo() << "Début Initialisation" << metaObject()->className();

    _indexLuneSoleil = settings.value("affichage/indexInformations", 0).toUInt();

    _ui->stackedWidget_soleilLune->setCurrentIndex(_indexLuneSoleil);

    _ui->soleilLunePrec->setToolTip(
                QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + _ui->stackedWidget_soleilLune->count() - 1)
            % _ui->stackedWidget_soleilLune->count()]));
    _ui->soleilLuneSuiv->setToolTip(QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + 1) %
                              _ui->stackedWidget_soleilLune->count()]));

    qInfo() << "Fin   Initialisation" << metaObject()->className();

    /* Retour */
    return;
}

void General::AffichageLieuObs()
{
    /* Declarations des variables locales */

    /* Initialisations */
    bool premier = true;
    _ui->lieuObservation->clear();

    /* Corps de la methode */
    QListIterator it(Configuration::instance()->observateurs());
    while (it.hasNext()) {

        const Observateur obs = it.next();
        const QString nomlieu = obs.nomlieu();

        _ui->lieuObservation->addItem(nomlieu);

        if (premier) {

            // Longitude/Latitude/Altitude
            const double lo = obs.longitude();
            const double la = obs.latitude();
            const double atd = obs.altitude() * 1000.;

            const QString ew = (lo < 0.) ? tr("Est") : tr("Ouest");
            const QString ns = (la < 0.) ? tr("Sud") : tr("Nord");

            // Affichage des coordonnees
            const QString fmt = "%1 %2";
            _ui->longitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(lo), AngleFormatType::DEGRE, 3, 0, false, true)).arg(ew));
            _ui->latitudeObs->setText(fmt.arg(Maths::ToSexagesimal(fabs(la), AngleFormatType::DEGRE, 2, 0,false, true)).arg(ns));
            _ui->altitudeObs->setText(fmt.arg((_options->ui()->unitesKm->isChecked()) ? atd : qRound(atd * PIED_PAR_METRE + 0.5 * sgn(atd))).
                                      arg((_options->ui()->unitesKm->isChecked()) ? tr("m", "meter") : tr("ft", "foot")));
            premier = false;
        }
    }

    _ui->lieuObservation->setCurrentIndex(0);

    /* Retour */
    return;
}

void General::on_soleilLunePrec_clicked()
{
    _indexLuneSoleil = (_ui->stackedWidget_soleilLune->currentIndex() + _ui->stackedWidget_soleilLune->count() - 1)
            % _ui->stackedWidget_soleilLune->count();
    _ui->stackedWidget_soleilLune->setCurrentIndex(_indexLuneSoleil);
}

void General::on_soleilLuneSuiv_clicked()
{
    _indexLuneSoleil = (_ui->stackedWidget_soleilLune->currentIndex() + 1) % _ui->stackedWidget_soleilLune->count();
    _ui->stackedWidget_soleilLune->setCurrentIndex(_indexLuneSoleil);
}

void General::on_stackedWidget_soleilLune_currentChanged(int arg1)
{
    Q_UNUSED(arg1)

    _ui->soleilLunePrec->setToolTip(
                QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + _ui->stackedWidget_soleilLune->count() - 1)
            % _ui->stackedWidget_soleilLune->count()]));
    _ui->soleilLuneSuiv->setToolTip(QCoreApplication::translate("General", _titresLuneSoleil[(_indexLuneSoleil + 1) %
                              _ui->stackedWidget_soleilLune->count()]));
}
