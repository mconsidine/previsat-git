#include "carte.h"
#pragma GCC diagnostic ignored "-Wconversion"
#include "ui_carte.h"
#pragma GCC diagnostic warning "-Wconversion"
#include <QGraphicsSimpleTextItem>


Carte::Carte(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Carte)
{
    ui->setupUi(this);
}

Carte::~Carte()
{
    delete ui;
    delete scene;
}

void Carte::AffichageCourbes()
{
    const QString nomMap = ":/resources/map.png";
    scene = new QGraphicsScene;
    scene->addPixmap(QPixmap(nomMap).scaled(size()));
    ui->carte->setScene(scene);

}

void Carte::AffichageSiteLancement(const QString &acronyme, const Observateur &observateur)
{
    AffichageCourbes();
    QGraphicsSimpleTextItem * const txt = new QGraphicsSimpleTextItem(acronyme);
    txt->setBrush(Qt::white);
    txt->setPos(200, 200);
    scene->addItem(txt);

}

void Carte::resizeEvent(QResizeEvent *evt)
{
    Q_UNUSED(evt)

    AffichageCourbes();
}
