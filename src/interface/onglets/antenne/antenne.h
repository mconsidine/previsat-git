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
 * >    antenne.h
 *
 * Localisation
 * >    interface.onglets.antenne
 *
 * Heritage
 * >    QFrame
 *
 * Description
 * >    Onglet Pilotage d'antenne
 *
 * Auteur
 * >    Astropedia
 *
 * Date de creation
 * >    9 octobre 2022
 *
 * Date de revision
 * >    2 fevrier 2025
 *
 */

#ifndef ANTENNE_H
#define ANTENNE_H

#include <QFrame>

namespace Ui {
class Antenne;
}

class QUdpSocket;
class Date;
struct ElementsAOS;


class Antenne : public QFrame
{
    Q_OBJECT

public:

    /*
     *  Constructeurs
     */
    /**
     * @brief Antenne Constructeur par defaut
     * @param[in] parent parent
     * @throw Exception
     */
    explicit Antenne(QWidget *parent = nullptr);


    /*
     * Destructeur
     */
    ~Antenne();


    /*
     * Accesseurs
     */

    /*
     * Modificateurs
     */

    /*
     * Methodes publiques
     */
    /**
     * @brief InitAffichageFrequences Initialisation de l'affichage des frequences
     */
    void InitAffichageFrequences();


public slots:

    /**
     * @brief show Affichage des informations de l'onglet
     * @param[in] date date
     */
    void show(const Date &date);

    void changeEvent(QEvent *evt);


    /**
     * @brief DeconnecterUdp Deconnecter le protocole UDP
     */
    void DeconnecterUdp();


protected:

    /*
     * Variables protegees
     */

    /*
     * Methodes protegees
     */


private:

    /*
     * Variables privees
     */
    Ui::Antenne *_ui;

    QTimer *_chronometreUdp;
    QUdpSocket *_udpSocket;
    QString _structureMessageUdp;

    Date *_date;


    /*
     * Methodes privees
     */
    /**
     * @brief Initialisation Initialisation de la classe Antenne
     */
    void Initialisation();


private slots:

    /**
     * @brief EnvoiUdp Envoi du datagram UDP
     */
    void EnvoiUdp();

    /**
     * @brief ReceptionUdp Reception du datagram UDP
     */
    void ReceptionUdp();

    void on_connexion_clicked();
    void on_ouvrirCatRotator_clicked();
    void on_frequenceMontante_currentIndexChanged(int index);
    void on_frequenceDescendante_currentIndexChanged(int index);
    void on_parametrageDefautRadio_clicked();

};

#endif // ANTENNE_H
