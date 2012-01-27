#ifndef PREVISAT_H
#define PREVISAT_H

#include <QMainWindow>

namespace Ui {
class PreviSat;
}

class PreviSat : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviSat(QWidget *parent = 0);
    void Initialisations();
    void EnchainementCalculs();
    void AffichageDonnees();
    void AffichageCourbes();
    ~PreviSat();

private:
    Ui::PreviSat *ui;
    void InitFicObs(const bool alarm);
    void InitFicMap();

    void AffichageLieuObs();
    void AffichageListeFichiersTLE();
    void AfficherListeSatellites(const QString fichier, const QStringList listeSat);
};

#endif // PREVISAT_H
