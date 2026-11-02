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
 * File name
 * >    mountcontrol.h
 *
 * Location
 * >    interface/onglets/telescope
 *
 * Heritage
 * >    QDialog
 *
 * Description
 * >    Window to connect a mount (LX200, OnStepX, SynScan) and drive it along the selected pass
 *
 * Creation date
 * >    2 November 2026
 *
 */

#ifndef MOUNTCONTROL_H
#define MOUNTCONTROL_H

#include <QDialog>
#include <QThread>
#include "mount/passtracker.h"


class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QStackedWidget;

class MountControl : public QDialog
{
    Q_OBJECT

public:

    explicit MountControl(QWidget *parent = nullptr);
    ~MountControl() override;

    /**
     * @brief SetPass Pass to track (from the telescope tab)
     */
    void SetPass(const PassDefinition &pass);


protected:

    void closeEvent(QCloseEvent *evt) override;


private slots:

    void ToggleConnection();
    void ArmPass();
    void AbortPass();
    void RefreshSerialPorts();

    void OnConnectionChanged(const bool connected,
                             const QString &identity);
    void OnStateChanged(const int state,
                        const QString &description);
    void OnLogMessage(const QString &message);
    void OnPositionUpdated(const double azimuth,
                           const double altitude,
                           const double separationArcmin);


private:

    QThread _thread;
    PassTracker *_tracker;
    PassDefinition _pass;
    bool _hasPass;
    bool _connected;
    int _state;

    // Connection
    QComboBox *_protocol;
    QComboBox *_link;
    QStackedWidget *_linkSettings;
    QComboBox *_serialPort;
    QComboBox *_baudRate;
    QLineEdit *_host;
    QSpinBox *_tcpPort;
    QCheckBox *_sendSiteAndTime;
    QPushButton *_connectButton;
    QLabel *_identity;

    // Pass
    QLabel *_passDescription;
    QSpinBox *_minAltitude;
    QSpinBox *_maxAltitude;
    QSpinBox *_prePosition;
    QDoubleSpinBox *_dwell;
    QCheckBox *_parkAtEnd;
    QPushButton *_armButton;
    QPushButton *_abortButton;

    // Status
    QLabel *_status;
    QLabel *_position;
    QPlainTextEdit *_log;

    void BuildInterface();
    void LoadSettings();
    void SaveSettings() const;
    void UpdateButtons();

};

#endif // MOUNTCONTROL_H
