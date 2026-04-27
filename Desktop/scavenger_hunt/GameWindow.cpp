#include "GameWindow.h"
#include "mapscene.h"
#include <QGraphicsView>
#include <QKeyEvent>
#include "clue.h"



GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
{
    MapScene *scene = new MapScene(this);

    player = new Player();
    scene->addItem(player);

    Clue *clue = new Clue();
    clue->setPos(200, 200);
    scene->addItem(clue);

    QGraphicsView *view = new QGraphicsView(scene, this);
    setCentralWidget(view);
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
        player->moveUp();
    else if (event->key() == Qt::Key_S)
        player->moveDown();
    else if (event->key() == Qt::Key_A)
        player->moveLeft();
    else if (event->key() == Qt::Key_D)
        player->moveRight();
}

GameWindow::~GameWindow()
{
}
