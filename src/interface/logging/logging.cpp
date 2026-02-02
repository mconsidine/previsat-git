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
 * >    logging.cpp
 *
 * Localisation
 * >    interface.logging
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    11 novembre 2022
 *
 * Date de revision
 * >    14 octobre 2025
 *
 */

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include "configuration/configuration.h"
#include "librairies/systeme/logmessage.h"
#include "logging.h"
#include "ui_logging.h"


/**********
 * PUBLIC *
 **********/

/*
 * Constructeurs
 */
Logging::Logging(QWidget *fenetreParent) :
    QMainWindow(fenetreParent),
    _ui(new Ui::Logging)
{
    _ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), fenetreParent->geometry()));
}


/*
 * Destructeur
 */
Logging::~Logging()
{
    delete _ui;
}


/*
 * Accesseurs
 */

/*
 * Methodes publiques
 */
void Logging::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        _ui->retranslateUi(this);
    }
}

void Logging::show()
{
    /* Declarations des variables locales */

    /* Initialisations */
    _ui->fichiersLog->setStyleSheet("QHeaderView::section {" \
                                    "background-color:rgb(235, 235, 235);" \
                                    "border-top: 0px solid grey;" \
                                    "border-bottom: 1px solid grey;" \
                                    "border-right: 1px solid grey;" \
                                    "font-size: 12px;" \
                                    "font-weight: 600 }");

    /* Corps de la methode */
    _ui->fichiersLog->resizeColumnsToContents();

    QBrush couleur;
    QFile fi;
    QFileInfo ff;
    QFont fnt;
    QString niveauMsg;
    QTableWidgetItem *itemNom;
    QTableWidgetItem *itemNiveau;
    QTableWidgetItem *itemNbMsg;

    int j = 0;
    unsigned int nbMsg = 0;
    fnt.setBold(true);

    _ui->fichiersLog->disconnect();
    _ui->fichiersLog->model()->removeRows(0, _ui->fichiersLog->rowCount());

    const QStringList filtre(QStringList () << QString(APP_NAME) + "_*.log");
    const QDir di(Configuration::instance()->dirLog());
    const QStringList listeFichiersLog = di.entryList(filtre, QDir::Files);

    QStringListIterator it(listeFichiersLog);
    it.toBack();
    while (it.hasPrevious()) {

        const QString fic = it.previous();

        ff = QFileInfo(Configuration::instance()->dirLog() + QDir::separator() + fic);
        if (QDir::toNativeSeparators(ff.absoluteFilePath()) != LogMessage::nomFicLog()) {

            _ui->fichiersLog->insertRow(j);
            _ui->fichiersLog->setRowHeight(j, 16);

            // Chargement du fichier
            fi.setFileName(ff.absoluteFilePath());
            (void) fi.open(QIODevice::ReadOnly | QIODevice::Text);
            const QString contenu = fi.readAll();
            fi.close();

            // Compte du nombre de message par type d'erreur
            const unsigned int nbMsgFatal = static_cast<unsigned int> (contenu.count("FATAL"));
            const unsigned int nbMsgErreur = static_cast<unsigned int> (contenu.count("ERREUR"));
            const unsigned int nbMsgWarning = static_cast<unsigned int> (contenu.count("WARNING"));
            const unsigned int nbMsgDebug = static_cast<unsigned int> (contenu.count("DEBUG"));
            const unsigned int nbMsgInfo = static_cast<unsigned int> (contenu.count("INFO"));

            couleur = QBrush(Qt::NoBrush);

            if (nbMsgFatal > 0) {

                nbMsg = nbMsgFatal;
                niveauMsg = tr("FATAL");
                couleur = QBrush(QColor(220, 20, 60));

            } else if (nbMsgErreur > 0) {

                nbMsg = nbMsgErreur;
                niveauMsg = tr("ERREUR");
                couleur = QBrush(Qt::red);

            } else if (nbMsgWarning > 0) {

                nbMsg = nbMsgWarning;
                niveauMsg = tr("WARNING");
                couleur = QBrush(QColor("orange"));

            } else if (nbMsgDebug > 0) {

                nbMsg = nbMsgDebug;
                niveauMsg = tr("DEBUG");
                couleur = QBrush(Qt::blue);

            } else {
                nbMsg = nbMsgInfo;
                niveauMsg = tr("INFO");
            }

            // Nom du fichier
            itemNom = new QTableWidgetItem(ff.baseName());
            itemNom->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            itemNom->setFlags(itemNom->flags() & ~Qt::ItemIsEditable);
            _ui->fichiersLog->setItem(j, 0, itemNom);

            // Niveau d'erreur
            itemNiveau = new QTableWidgetItem(niveauMsg);
            itemNiveau->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            itemNiveau->setFlags(itemNiveau->flags() & ~Qt::ItemIsEditable);
            itemNiveau->setForeground(couleur);
            itemNiveau->setFont(fnt);
            _ui->fichiersLog->setItem(j, 1, itemNiveau);

            // Nombre de messages correspondant
            itemNbMsg = new QTableWidgetItem(QString::number(nbMsg));
            itemNbMsg->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            itemNbMsg->setFlags(itemNbMsg->flags() & ~Qt::ItemIsEditable);
            _ui->fichiersLog->setItem(j, 2, itemNbMsg);

            j++;
        }
    }

    _ui->fichiersLog->horizontalHeader()->setStretchLastSection(true);
    _ui->fichiersLog->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _ui->fichiersLog->selectRow(0);
    _ui->fichiersLog->setFocus();

    connect(_ui->fichiersLog, &QTableWidget::doubleClicked, this, &Logging::OuvrirFichier);

    _ui->listeBoutonsExporterLog->button(QDialogButtonBox::Save)->setVisible(_ui->fichiersLog->rowCount() > 0);
    _ui->listeBoutonsExporterLog->button(QDialogButtonBox::Save)->setText(tr("Exporter ..."));

    setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

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
 * Ouverture du fichier log selectionne
 */
void Logging::OuvrirFichier()
{
    /* Declarations des variables locales */

    /* Initialisations */
    const int idx = _ui->fichiersLog->currentRow();
    const QString fichier = Configuration::instance()->dirLog() + QDir::separator() + _ui->fichiersLog->item(idx, 0)->text() + ".log";

    /* Corps de la methode */
    QDesktopServices::openUrl(QUrl("file:///" + fichier));

    /* Retour */
    return;
}

void Logging::on_listeBoutonsExporterLog_clicked(QAbstractButton *button)
{
    /* Declarations des variables locales */

    /* Initialisations */

    /* Corps de la methode */
    if (button == _ui->listeBoutonsExporterLog->button(QDialogButtonBox::Save)) {

        // Export du fichier selectionne
        const QString fic = _ui->fichiersLog->item(_ui->fichiersLog->currentRow(), 0)->text() + ".log";
        const QString ficlog = Configuration::instance()->dirOut() + QDir::separator() + fic;

        QFile fi(Configuration::instance()->dirLog() + QDir::separator() + fic);

        if (fi.exists()) {

            const QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer sous..."), ficlog, tr("Fichiers log (*.log)"));

            if (!fichier.isEmpty()) {

                // Sauvegarde du fichier log
                if (fi.copy(ficlog)) {
                    qInfo().noquote() << "Export du fichier" << fi.fileName() << "OK";
                } else {
                    qWarning().noquote() << "Export du fichier" << fi.fileName() << "KO";
                }
            }
        }

    } else if (button == _ui->listeBoutonsExporterLog->button(QDialogButtonBox::Close)) {

        // Fermeture de la fenetre
        close();
    }

    /* Retour */
    return;
}
