#ifndef CARTE_H
#define CARTE_H

#pragma GCC diagnostic ignored "-Wconversion"
#include <QFrame>
#pragma GCC diagnostic warning "-Wconversion"
#include <QGraphicsScene>


namespace Ui {
class Carte;
}

class Observateur;

class Carte : public QFrame
{
    Q_OBJECT

public:
    explicit Carte(QWidget *parent = nullptr);
    ~Carte();

    void AffichageCourbes();


public slots:

    void AffichageSiteLancement(const QString &acronyme, const Observateur &observateur);


private slots:

    void resizeEvent(QResizeEvent *evt);

private:
    Ui::Carte *ui;
    QGraphicsScene *scene;
};

#endif // CARTE_H
