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
 * >    donneessatellite.cpp
 *
 * Localisation
 * >    interface.onglets.donnees
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    18 juillet 2024
 *
 * Date de revision
 * >
 *
 */

#include "configuration/configuration.h"
#include "donneessatellite.h"
#include "librairies/corps/satellite/donnees.h"
#include "librairies/corps/satellite/satellite.h"
#include "librairies/maths/maths.h"
#include "ui_donneessatellite.h"


// Registre
#if (PORTABLE_BUILD)
static QSettings settings(QString("%1.ini").arg(APP_NAME), QSettings::IniFormat);
#else
static QSettings settings(ORG_NAME, APP_NAME);
#endif


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
/*
 * Constructeur par defaut
 */
DonneesSatellite::DonneesSatellite(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::DonneesSatellite)
{
    _ui->setupUi(this);
    _ui->siteLancement->installEventFilter(this);
    _ui->pays->installEventFilter(this);
    _ui->categorieOrbite->installEventFilter(this);
}


/*
 * Destructeur
 */
DonneesSatellite::~DonneesSatellite()
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
void DonneesSatellite::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void DonneesSatellite::show(const Donnees &donnees)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    // Nom du satellite
    _ui->nomsat->setText(donnees.nom());

    // Numero NORAD
    _ui->norad->setText(donnees.norad());

    // Designation COSPAR
    _ui->cospar->setText(donnees.cospar());


    // Date de lancement
    _ui->dateLancement->setText((donnees.dateLancement().isEmpty()) ? tr("Inconnue") : donnees.dateLancement().replace("T", " "));

    // Categorie d'orbite
    _ui->categorieOrbite->setText((donnees.orbite().isEmpty()) ? tr("Inconnue") : donnees.orbite());

    // Pays ou organisation
    _ui->pays->setText((donnees.pays().isEmpty()) ? tr("Inconnu") : donnees.pays());

    // Site de lancement
    _ui->siteLancement->setText((donnees.siteLancement().isEmpty()) ? tr("Inconnu") : donnees.siteLancement());


    // Masse sec, totale
    if ((donnees.masseSec() == "0") && (donnees.masseTot() == "0")) {
        _ui->masseSecTot->setText("?/?");
    } else {
        _ui->masseSecTot->setText(QString("%1/%2 kg").arg(donnees.masseSec()).arg(donnees.masseTot()));
    }


    // Classe, categorie et discipline
    const QString classe = (donnees.classe().isEmpty()) ? "?" : donnees.classe();
    const QString categorie = (donnees.categorie().isEmpty()) ? "?" : donnees.categorie();
    const QString discipline = (donnees.discipline().isEmpty()) ? "?" : donnees.discipline();
    _ui->classeCategDiscip->setText(QString("%1 / %2 / %3").arg(classe).arg(categorie).arg(discipline));


    double perigee = donnees.perigee();
    double apogee = donnees.apogee();

    double ap = apogee + TERRE::RAYON_TERRESTRE;
    double per = perigee + TERRE::RAYON_TERRESTRE;


    // Magnitude standard/maximale
    if ((donnees.magnitudeStandard() > 98.) || (perigee < MATHS::EPSDBL100) || (apogee < MATHS::EPSDBL100)) {
        _ui->magnitudeStdMax->setText("?/?");
    } else {

        // Estimation de la magnitude maximale
        const double demiGrandAxe = 0.5 * (ap + per);
        const double exc = 2. * ap / (ap + per) - 1.;
        const double magMax = donnees.magnitudeStandard() - 15.75 + 5. * log10(1.45 * (demiGrandAxe * (1. - exc) - TERRE::RAYON_TERRESTRE));

        QString text;
        _ui->magnitudeStdMax->setText(text.asprintf("%+.2f%s/%+.1f", donnees.magnitudeStandard(),
                                                    donnees.methMagnitude().toStdString().c_str(), magMax));
    }

    // Modele orbital
    const bool modeleDS = (donnees.periode().toDouble() > 225.);
    const QString modele = (modeleDS) ? tr("SGP4 (DS)", "Orbital model SGP4 (deep space)") : tr("SGP4 (NE)", "Orbital model SGP4 (near Earth)");
    _ui->modele->setText((donnees.periode().isEmpty()) ? tr("Non applicable") : modele);

    if (!donnees.periode().isEmpty()) {
        _ui->modele->setToolTip((modeleDS) ? tr("Modèle haute orbite") : tr("Modèle basse orbite"));
    }

    // Forme et dimensions du satellite
    double t1 = donnees.longueur();
    double t2 = donnees.diametre();
    double t3 = donnees.envergure();

    QString unite1 = tr("m", "meter");
    QString unite2 = tr("km", "kilometer");

    if (!settings.value("affichage/unite").toBool()) {

        apogee *= TERRE::MILE_PAR_KM;
        perigee *= TERRE::MILE_PAR_KM;
        ap *= TERRE::MILE_PAR_KM;
        per *= TERRE::MILE_PAR_KM;

        t1 *= TERRE::PIED_PAR_METRE;
        t2 *= TERRE::PIED_PAR_METRE;
        t3 *= TERRE::PIED_PAR_METRE;

        unite1 = tr("ft", "foot");
        unite2 = tr("nmi", "nautical mile");
    }

    QString dimensions;
    if ((fabs(t1) < MATHS::EPSDBL100) && (fabs(t2) < MATHS::EPSDBL100) && (fabs(t3) < MATHS::EPSDBL100)) {
        dimensions = tr("Inconnues");
    } else {
        dimensions = QString("%1. %2 x %3 x %4 %5").arg(donnees.forme()).arg(t1, 0, 'f', 1).arg(t2, 0, 'f', 1).arg(t3, 0, 'f', 1).arg(unite1);
    }

    _ui->dimensions->setText(dimensions);

    // Apogee/perigee/periode orbitale
    const QString fmt = "%1 %2 (%3 %2)";
    if (fabs(apogee) < MATHS::EPSDBL100) {
        _ui->apogee->setText(tr("Inconnu"));
    } else {
        _ui->apogee->setText(fmt.arg(ap, 0, 'f', 0).arg(unite2).arg(apogee, 0, 'f', 0));
    }

    if (fabs(perigee) < MATHS::EPSDBL100) {
        _ui->perigee->setText(tr("Inconnu"));
    } else {
        _ui->perigee->setText(fmt.arg(per, 0, 'f', 0).arg(unite2).arg(perigee, 0, 'f', 0));
    }

    QString periode;
    if (donnees.periode().isEmpty()) {
        periode = tr("Inconnue");
    } else {
        periode = Maths::ToSexagesimal(donnees.periode().toDouble() * DATE::NB_HEUR_PAR_MIN * MATHS::HEUR2RAD,
                                       AngleFormatType::HEURE1, 1, 0, false, true).trimmed();
    }

    _ui->periode->setText(periode);

    // Inclinaison
    _ui->inclinaison->setText((donnees.inclinaison().isEmpty()) ? tr("Inconnue") : donnees.inclinaison() + "°");

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
bool DonneesSatellite::eventFilter(QObject *watched, QEvent *event)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (event->type() == QEvent::MouseMove) {

        if (_ui->siteLancement->underMouse()) {

            const QString acronyme = _ui->siteLancement->text();
            const Observateur site = Configuration::instance()->mapSitesLancement()[acronyme];
            _ui->siteLancement->setToolTip(site.nomlieu());

        } else if (_ui->pays->underMouse()) {

            const QString pays = Configuration::instance()->mapPays()[_ui->pays->text()];
            _ui->pays->setToolTip(pays);

        } else if (_ui->categorieOrbite->underMouse()) {

            const QString categorie = Configuration::instance()->mapCategoriesOrbite()[_ui->categorieOrbite->text()];
            _ui->categorieOrbite->setToolTip(categorie);
        }
    }

    /* Retour */
    return QMainWindow::eventFilter(watched, event);
}
